#pragma once
#include <audioapi/core/AudioNode.h>

namespace audioapi {
class AudioBus;
class BaseAudioContext;

class BinauralNode : public AudioNode {
private:
  BaseAudioContext* _context;
  
  // Oscillator phases
  double phaseL_ = 0.0;
  double phaseR_ = 0.0;
  double panPhase_ = 0.0;

  // Envelope state for panOsc mode 1 (ping-pong crossfade)
  double panOscPhase_ = 0.0;  // Phase within the full 2*panOscPeriod cycle
  bool isSwapped_ = false;    // Track if carriers are currently swapped

  // Audio state
  bool isRunning_ = false;

public:
  explicit BinauralNode(BaseAudioContext *context);

  // Oscillator frequencies
  double fl = 340.0;
  double fr = 160.0;

  // Waveforms (0=sine, 1=triangle, 2=square, 3=sawtooth)
  int waveformL = 0;
  int waveformR = 0;

  // Volume
  double volume = 0.5;

  // Panning oscillator settings
  int panOsc = 0;          // 0=none, 1=envelope, 2=independent sine, 3=synced to martigli
  double panOscPeriod = 120.0;
  double panOscTrans = 20.0;
  float martigliAnimationValue = 0.0f; // For panOsc=3: 0.0 to 1.0 from Martigli voice

  // Control flags
  bool shouldStart = false;
  bool shouldPause = false;
  bool shouldResume = false;
  bool shouldStop = false;
  bool isPaused = false;
  
  // Debug counter
  int frameCount = 0;

private:
  // Volume ramping state
  float currentGain_ = 0.0f;
  float startGain_ = 0.0f;
  float targetGain_ = 1.0f;
  float rampDuration_ = 1.0f; // seconds
  float rampElapsed_ = 0.0f;
  bool isRamping_ = false;

protected:
  void processNode(const std::shared_ptr<AudioBus> &bus, int framesToProcess) override;
};

} // namespace audioapi
