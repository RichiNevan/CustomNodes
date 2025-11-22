#include "NoiseNode.h"
#include <audioapi/utils/AudioBus.h>
#include <audioapi/utils/AudioArray.h>
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace audioapi {

NoiseNode::NoiseNode(BaseAudioContext *context)
    : AudioNode(context), _context(context) {
  channelCount_ = 2;
  channelCountMode_ = ChannelCountMode::EXPLICIT;
  channelInterpretation_ = ChannelInterpretation::SPEAKERS;
  isInitialized_ = true;
}

void NoiseNode::start() {
  shouldStart = true;
}

void NoiseNode::stop() {
  shouldStop = true;
}

void NoiseNode::pause() {
  shouldPause = true;
}

void NoiseNode::resume() {
  shouldResume = true;
}

float NoiseNode::generateWhiteNoise() {
  // Generate random value between -1 and 1, scaled down to match perceived loudness
  // White noise is perceptually louder due to high-frequency energy
  return ((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f) * 0.25f;
}

float NoiseNode::generatePinkNoise() {
  // Paul Kellet's pink noise algorithm
  // Approximates 1/f spectrum using weighted sum of white noise
  float white = generateWhiteNoise();
  
  pinkState_[0] = 0.99886f * pinkState_[0] + white * 0.0555179f;
  pinkState_[1] = 0.99332f * pinkState_[1] + white * 0.0750759f;
  pinkState_[2] = 0.96900f * pinkState_[2] + white * 0.1538520f;
  pinkState_[3] = 0.86650f * pinkState_[3] + white * 0.3104856f;
  pinkState_[4] = 0.55000f * pinkState_[4] + white * 0.5329522f;
  pinkState_[5] = -0.7616f * pinkState_[5] - white * 0.0168980f;
  
  float pink = pinkState_[0] + pinkState_[1] + pinkState_[2] + 
               pinkState_[3] + pinkState_[4] + pinkState_[5] + 
               pinkState_[6] + white * 0.5362f;
  
  pinkState_[6] = white * 0.115926f;
  
  return pink * 0.11f; // Scale down to ~[-1, 1]
}

float NoiseNode::generateBrownNoise() {
  // Brown noise (Brownian/red noise) via random walk
  // Use full-amplitude white noise for the random walk (not the scaled version)
  float white = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
  brownState_ += white * 0.02f;
  
  // Prevent drift too far from zero
  brownState_ *= 0.9999f;
  
  // Clamp to prevent overflow
  brownState_ = std::max(-1.0f, std::min(1.0f, brownState_));
  
  // Scale up brown noise to match perceived loudness with pink
  return brownState_ * 3.5f;
}

void NoiseNode::processNode(
  const std::shared_ptr<AudioBus> &processingBus,
  int framesToProcess
) {
  // Handle control flags
  if (shouldStart) {
    isRunning_ = true;
    isPaused = false;
    shouldStart = false;
    currentGain_ = 0.0f;
    startGain_ = 0.0f;
    targetGain_ = 1.0f;
    rampDuration_ = 0.3f;
    rampElapsed_ = 0.0f;
    isRamping_ = true;
  }
  
  if (shouldStop) {
    shouldStop = false;
    startGain_ = currentGain_;
    targetGain_ = 0.0f;
    rampDuration_ = 0.3f;
    rampElapsed_ = 0.0f;
    isRamping_ = true;
  }
  
  if (shouldPause) {
    shouldPause = false;
    startGain_ = isRamping_ ? currentGain_ : 1.0f;
    targetGain_ = 0.0f;
    rampDuration_ = 0.5f;
    rampElapsed_ = 0.0f;
    isRamping_ = true;
  }
  
  if (shouldResume) {
    isPaused = false;
    shouldResume = false;
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

  auto *leftChannel = processingBus->getChannel(0)->getData();
  auto *rightChannel = processingBus->getChannel(1)->getData();

  const double sampleRate = _context->getSampleRate();
  const float frameDuration = 1.0f / sampleRate;

  for (int i = 0; i < framesToProcess; ++i) {
    // Update volume ramping
    if (isRamping_) {
      rampElapsed_ += frameDuration;
      float t = rampElapsed_ / rampDuration_;
      if (t >= 1.0f) {
        currentGain_ = targetGain_;
        isRamping_ = false;
        if (targetGain_ == 0.0f) {
          isPaused = true;
        }
        if (targetGain_ == 0.0f && !isPaused) {
          isRunning_ = false;
        }
      } else {
        currentGain_ = startGain_ + (targetGain_ - startGain_) * t;
      }
    }

    // Handle color switching with crossfade
    float noiseSample = 0.0f;
    
    if (isColorSwitching_) {
      // During color switch, crossfade between old and new
      float oldSample = 0.0f;
      float newSample = 0.0f;
      
      switch (noiseColor) {
        case 0: oldSample = generateWhiteNoise(); break;
        case 1: oldSample = generatePinkNoise(); break;
        case 2: oldSample = generateBrownNoise(); break;
      }
      
      switch (targetNoiseColor_) {
        case 0: newSample = generateWhiteNoise(); break;
        case 1: newSample = generatePinkNoise(); break;
        case 2: newSample = generateBrownNoise(); break;
      }
      
      // Linear crossfade over 0.2s
      float crossfadeProgress = rampElapsed_ / 0.2f;
      if (crossfadeProgress >= 1.0f) {
        noiseColor = targetNoiseColor_;
        isColorSwitching_ = false;
        noiseSample = newSample;
      } else {
        noiseSample = oldSample * (1.0f - crossfadeProgress) + newSample * crossfadeProgress;
      }
    } else {
      // Normal noise generation
      switch (noiseColor) {
        case 0: noiseSample = generateWhiteNoise(); break;
        case 1: noiseSample = generatePinkNoise(); break;
        case 2: noiseSample = generateBrownNoise(); break;
      }
    }

    // Apply volume and gain ramping
    float output = noiseSample * volume * currentGain_;

    // Stereo output (same noise to both channels)
    leftChannel[i] = output;
    rightChannel[i] = output;
  }
}

} // namespace audioapi
