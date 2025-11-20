#include "BinauralNode.h"
#include <audioapi/core/BaseAudioContext.h>
#include <audioapi/utils/AudioBus.h>
#include <audioapi/utils/AudioArray.h>
#include <cmath>
#include <algorithm>
#include <cstdio>

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
    targetGain_ = 1.0f;
    rampDuration_ = 1.0f;
    rampElapsed_ = 0.0f;
    isRamping_ = true;
  }
  if (shouldStop) {
    shouldStop = false;
    // Start fade-out
    targetGain_ = 0.0f;
    rampDuration_ = 1.0f;
    rampElapsed_ = 0.0f;
    isRamping_ = true;
  }
  if (shouldPause) {
    isPaused = true;
    shouldPause = false;
    // Quick fade to silence
    targetGain_ = 0.0f;
    rampDuration_ = 0.5f;
    rampElapsed_ = 0.0f;
    isRamping_ = true;
  }
  if (shouldResume) {
    isPaused = false;
    shouldResume = false;
    // Quick fade back to full volume
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
        // If we finished fading out, stop the node
        if (targetGain_ == 0.0f && !isPaused) {
          isRunning_ = false;
        }
      } else {
        // Linear interpolation
        currentGain_ = currentGain_ + (targetGain_ - currentGain_) * t;
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
      // Envelope mode: linear transition and hold
      double envValue;
      if (envAttack_) {
        // Attack phase
        envValue = envPhase_ / panOscTrans;
        if (envPhase_ >= panOscTrans) {
          envAttack_ = false;
          envPhase_ = 0.0;
        }
      } else {
        // Hold phase
        envValue = 1.0;
        if (envPhase_ >= (panOscPeriod - panOscTrans)) {
          envAttack_ = true;
          envPhase_ = 0.0;
        }
      }
      // Clamp envValue to [0,1]
      if (envValue < 0.0) envValue = 0.0;
      if (envValue > 1.0) envValue = 1.0;
      // Map [0,1] to pan position: 0->left, 1->right
      // mul2, addm1, negate pattern from Tone.js
      double pan = 2.0 * envValue - 1.0;  // [0,1] -> [-1,1]
      panGainL = (1.0 - pan) * 0.5;  // left gain: 1 when pan=-1, 0 when pan=1
      panGainR = (1.0 + pan) * 0.5;  // right gain: 0 when pan=-1, 1 when pan=1

    } else if (panOsc == 2) {
      // Independent sine oscillator
      double panValue = std::sin(twoPi * panPhase_);
      panGainL = (1.0 - panValue) * 0.5;
      panGainR = (1.0 + panValue) * 0.5;
    }
    // panOsc == 0 or 3: no panning modulation (both gains = 1.0)

    // For binaural: left carrier goes to left channel, right carrier goes to right channel
    // Panning modulation is applied on top
    leftChannel[i] = static_cast<float>(carrierL * panGainL);
    rightChannel[i] = static_cast<float>(carrierR * panGainR);

    // Advance phases if not paused
    if (!isPaused) {
      phaseL_ += fl / sampleRate;
      phaseR_ += fr / sampleRate;

      // Wrap phases
      if (phaseL_ >= 1.0) phaseL_ -= std::floor(phaseL_);
      if (phaseR_ >= 1.0) phaseR_ -= std::floor(phaseR_);

      // Advance panning phase
      if (panOsc == 1) {
        envPhase_ += 1.0 / sampleRate;
      } else if (panOsc == 2) {
        panPhase_ += (1.0 / panOscPeriod) / sampleRate;
        if (panPhase_ >= 1.0) panPhase_ -= std::floor(panPhase_);
      }
    }
  }
}

} // namespace audioapi
