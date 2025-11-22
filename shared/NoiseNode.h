#pragma once
#include <audioapi/core/AudioNode.h>
#include <audioapi/core/BaseAudioContext.h>
#include <memory>

namespace audioapi {

class NoiseNode : public AudioNode {
public:
  explicit NoiseNode(BaseAudioContext *context);
  ~NoiseNode() override = default;

  void processNode(
    const std::shared_ptr<AudioBus> &processingBus,
    int framesToProcess
  ) override;

  // Control methods
  void start();
  void stop();
  void pause();
  void resume();

  // Properties
  int noiseColor = 0;  // 0=white, 1=pink, 2=brown
  float volume = 0.3;
  bool isPaused = false;

private:
  BaseAudioContext *_context;
  bool isRunning_ = false;
  
  // Control flags
  bool shouldStart = false;
  bool shouldStop = false;
  bool shouldPause = false;
  bool shouldResume = false;

  // Volume ramping
  float currentGain_ = 0.0f;
  float startGain_ = 0.0f;
  float targetGain_ = 0.0f;
  float rampDuration_ = 0.3f;  // Default 0.3s for start/stop
  float rampElapsed_ = 0.0f;
  bool isRamping_ = false;

  // Noise color switching
  int targetNoiseColor_ = 0;
  bool isColorSwitching_ = false;

  // Pink noise state (1/f filter)
  float pinkState_[7] = {0, 0, 0, 0, 0, 0, 0};

  // Brown noise state (integrated)
  float brownState_ = 0.0f;

  // Noise generation functions
  float generateWhiteNoise();
  float generatePinkNoise();
  float generateBrownNoise();
};

} // namespace audioapi
