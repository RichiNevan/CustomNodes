#include "BinauralNode.h"
#include "AnimationValueRegistry.h"
#include <audioapi/core/BaseAudioContext.h>
#include <audioapi/utils/AudioBus.h>
#include <audioapi/utils/AudioArray.h>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace audioapi {

BinauralNode::BinauralNode(BaseAudioContext *context)
    : AudioNode(context), _context(context) {
  channelCount_ = 2;
  channelCountMode_ = ChannelCountMode::EXPLICIT;
  channelInterpretation_ = ChannelInterpretation::SPEAKERS;
  isInitialized_ = true;
  printf("BinauralNode: Constructor called\n");
}

void BinauralNode::processNode(
  const std::shared_ptr<AudioBus> &processingBus,
  int framesToProcess
) {
  // Handle control flags
  if (shouldStart) {
    isRunning_ = true;
    isPaused = false;
    shouldStart = false;
    // Start fade-in
    currentGain_ = 0.0f;
    startGain_ = 0.0f;
    targetGain_ = 1.0f;
    rampDuration_ = 1.5f;
    rampElapsed_ = 0.0f;
    isRamping_ = true;
  }
  if (shouldStop) {
    shouldStop = false;
    // Start fade-out
    startGain_ = currentGain_;
    targetGain_ = 0.0f;
    rampDuration_ = 1.5f;
    rampElapsed_ = 0.0f;
    isRamping_ = true;
  }
  if (shouldPause) {
    shouldPause = false;
    // Don't set isPaused yet - let the audio ramp down first
    startGain_ = isRamping_ ? currentGain_ : 1.0f; // If not ramping, assume full volume
    targetGain_ = 0.0f;
    rampDuration_ = 0.5f;
    rampElapsed_ = 0.0f;
    isRamping_ = true;
  }
  if (shouldResume) {
    isPaused = false;
    shouldResume = false;
    // Quick fade back to full volume
    startGain_ = currentGain_;
    targetGain_ = 1.0f;
    rampDuration_ = 0.5f;
    rampElapsed_ = 0.0f;
    isRamping_ = true;
  }

  if (!isRunning_) {
    processingBus->zero();
    return;
  }
  
  // Increment frame counter for debugging
  frameCount++;

  auto *leftChannel = processingBus->getChannel(0)->getData();
  auto *rightChannel = processingBus->getChannel(1)->getData();

  const double sampleRate = _context->getSampleRate();
  const double twoPi = 2.0 * M_PI;
  const float frameDuration = 1.0f / sampleRate;

  for (int i = 0; i < framesToProcess; ++i) {
    // Update volume ramping
    if (isRamping_) {
      rampElapsed_ += frameDuration;
      float t = rampElapsed_ / rampDuration_;
      if (t >= 1.0f) {
        currentGain_ = targetGain_;
        isRamping_ = false;
        // If we just finished ramping to 0, set isPaused
        if (targetGain_ == 0.0f) {
          isPaused = true;
          std::cout << "BinauralNode: Pause ramp complete, now frozen" << std::endl;
        }
        // If we finished fading out completely, stop the node
        if (targetGain_ == 0.0f && !isPaused) {
          isRunning_ = false;
        }
      } else {
        // Linear interpolation from start to target
        currentGain_ = startGain_ + (targetGain_ - startGain_) * t;
      }
    }

    // Generate left carrier
    double carrierL = 0.0;
    switch (waveformL) {
      case 0: // sine
        carrierL = std::sin(twoPi * phaseL_);
        break;
      case 1: // triangle
        carrierL = 2.0 * std::abs(2.0 * (phaseL_ - std::floor(phaseL_ + 0.5))) - 1.0;
        break;
      case 2: // square
        carrierL = (phaseL_ - std::floor(phaseL_)) < 0.5 ? 1.0 : -1.0;
        break;
      case 3: // sawtooth
        carrierL = 2.0 * (phaseL_ - std::floor(phaseL_ + 0.5));
        break;
    }

    // Generate right carrier
    double carrierR = 0.0;
    switch (waveformR) {
      case 0: // sine
        carrierR = std::sin(twoPi * phaseR_);
        break;
      case 1: // triangle
        carrierR = 2.0 * std::abs(2.0 * (phaseR_ - std::floor(phaseR_ + 0.5))) - 1.0;
        break;
      case 2: // square
        carrierR = (phaseR_ - std::floor(phaseR_)) < 0.5 ? 1.0 : -1.0;
        break;
      case 3: // sawtooth
        carrierR = 2.0 * (phaseR_ - std::floor(phaseR_ + 0.5));
        break;
    }

    // Apply volume
    carrierL *= volume;
    carrierR *= volume;
    
    // Apply volume ramping
    carrierL *= currentGain_;
    carrierR *= currentGain_;

    // Calculate panning modulation based on panOsc mode
    double panGainL = 1.0;
    double panGainR = 1.0;

    if (panOsc == 1) {
      // Ping-pong crossfade mode
      // Full cycle = 2 * panOscPeriod
      // Phase 1: Hold at original (0 to panOscPeriod - panOscTrans)
      // Phase 2: Crossfade to opposite (panOscPeriod - panOscTrans to panOscPeriod)
      // Phase 3: Hold at opposite (panOscPeriod to 2*panOscPeriod - panOscTrans)
      // Phase 4: Crossfade back (2*panOscPeriod - panOscTrans to 2*panOscPeriod)
      
      double fullCycleDuration = 2.0 * panOscPeriod;
      double phaseInCycle = fmod(panOscPhase_, fullCycleDuration);
      double crossfadeValue = 0.0; // 0.0 = normal position, 1.0 = swapped
      
      if (phaseInCycle < (panOscPeriod - panOscTrans)) {
        // Phase 1: Hold at original
        crossfadeValue = 0.0;
      } else if (phaseInCycle < panOscPeriod) {
        // Phase 2: Crossfade to opposite
        double transitionProgress = (phaseInCycle - (panOscPeriod - panOscTrans)) / panOscTrans;
        crossfadeValue = transitionProgress; // 0 -> 1
      } else if (phaseInCycle < (2.0 * panOscPeriod - panOscTrans)) {
        // Phase 3: Hold at opposite
        crossfadeValue = 1.0;
      } else {
        // Phase 4: Crossfade back to original
        double transitionProgress = (phaseInCycle - (2.0 * panOscPeriod - panOscTrans)) / panOscTrans;
        crossfadeValue = 1.0 - transitionProgress; // 1 -> 0
      }
      
      // Apply crossfade
      // When crossfadeValue = 0: L→L, R→R (panGainL=1, panGainR=1)
      // When crossfadeValue = 1: L→R, R→L (panGainL=0, panGainR=0, but we swap the carriers)
      panGainL = 1.0 - crossfadeValue;
      panGainR = crossfadeValue;

    } else if (panOsc == 2) {
      // Continuous sinusoidal panning
      // sin(0) = 0 -> carriers at normal position
      // sin(π/2) = 1 -> carriers fully swapped
      // sin(π) = 0 -> carriers back to normal
      // sin(3π/2) = -1 -> carriers swapped opposite direction
      // sin(2π) = 0 -> back to start
      double sinValue = std::sin(twoPi * panPhase_);
      // Map sin [-1,1] to crossfade [0,1,0,1] pattern
      // We want: -1→0, 0→0.5, 1→1
      double crossfadeValue = (sinValue + 1.0) * 0.5; // [-1,1] -> [0,1]
      panGainL = 1.0 - crossfadeValue;
      panGainR = crossfadeValue;
      
    } else if (panOsc == 3) {
      // Follow Martigli animation value from registry
      // Read directly from registry (no JS bridge crossing!)
      double animValue = AnimationValueRegistry::getInstance().getMartigliAnimationValue();
      // animValue: 0.0 (trough) to 1.0 (peak)
      // At trough: normal position (L→L, R→R)
      // At peak: swapped position (L→R, R→L)
      panGainL = 1.0 - animValue;
      panGainR = animValue;
    }
    // panOsc == 0: no panning (both gains = 1.0)

    // Apply panning by mixing carriers to channels
    // Normal: leftCarrier→leftChannel (panGainL=1, panGainR=0)
    // Swapped: leftCarrier→rightChannel (panGainL=0, panGainR=1)
    leftChannel[i] = static_cast<float>(carrierL * panGainL + carrierR * panGainR);
    rightChannel[i] = static_cast<float>(carrierR * panGainL + carrierL * panGainR);

    // Advance phases if not paused
    if (!isPaused) {
      phaseL_ += fl / sampleRate;
      phaseR_ += fr / sampleRate;

      // Wrap phases
      if (phaseL_ >= 1.0) phaseL_ -= std::floor(phaseL_);
      if (phaseR_ >= 1.0) phaseR_ -= std::floor(phaseR_);

      // Advance panning phases
      if (panOsc == 1) {
        panOscPhase_ += frameDuration;
        // Wrap at full cycle (2 * panOscPeriod)
        double fullCycle = 2.0 * panOscPeriod;
        if (panOscPhase_ >= fullCycle) {
          panOscPhase_ = fmod(panOscPhase_, fullCycle);
        }
      } else if (panOsc == 2) {
        panPhase_ += frameDuration / panOscPeriod;
        if (panPhase_ >= 1.0) {
          panPhase_ = fmod(panPhase_, 1.0);
        }
      }
      // panOsc == 3: no phase advancement needed (uses external value)
    }
  }
}

} // namespace audioapi
