#pragma once
#include <audioapi/core/AudioNode.h>

namespace audioapi {
class AudioBus;
class BaseAudioContext;

class MartigliNode : public AudioNode {
private:
  BaseAudioContext* _context;
  float _lfoPhaseTime = 0.0f;
  float _carrierPhase = 0.0f;
  float _rampElapsedTime = 0.0f;
  float _panEnvPhaseTime = 0.0f;
  float _panOscPhase = 0.0f;
  bool _isRamping = false;
  
  // Track the period for the current breathing cycle
  float _currentCycleInhale = 0.0f;
  float _currentCycleExhale = 0.0f;
  float _lastPhase = 0.0f;  // Track previous phase to detect wrapping
  
public:
  explicit MartigliNode(BaseAudioContext *context);
  
  // Core parameters
  float mf0 = 250.0f;           // Base frequency
  float ma = 90.0f;             // Modulation amount
  float mp0 = 10.0f;            // Initial period
  float mp1 = 20.0f;            // Final period
  float md = 600.0f;            // Ramp duration
  float inhaleDur = -1.0f;      // Inhale duration (optional)
  float exhaleDur = -1.0f;      // Exhale duration (optional)
  int waveformM = 0;            // Waveform type
  float volume = 0.5f;
  
  // Panning parameters
  int panOsc = 0;
  float panOscPeriod = 120.0f;
  float panOscTrans = 20.0f;
  
  // State
  float animationValue = 0.0f;
  bool isPaused = false;
  bool isOn = false;              // Only the active martigli publishes to registry
  bool shouldStart = false;
  bool shouldPause = false;
  bool shouldResume = false;
  bool shouldStop = false;
  
  // Current calculated values (read-only, updated during processing)
  float currentInhaleDur = 0.0f;
  float currentExhaleDur = 0.0f;
  float currentPeriod = 0.0f;
  
  void start();
  void pause();
  void resume();
  void stop();

private:
  // Volume ramping state
  float _currentGain = 0.0f;
  float _targetGain = 1.0f;
  float _rampDuration = 1.0f;
  float _rampElapsed = 0.0f;
  bool _isVolumeRamping = false;

protected:
  void processNode(const std::shared_ptr<AudioBus> &bus, int framesToProcess) override;
};
} // namespace audioapi
