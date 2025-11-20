#pragma once
#include <audioapi/core/AudioNode.h>

namespace audioapi {
class AudioBus;
class BaseAudioContext;

class MartigliNode : public AudioNode {
private:
  BaseAudioContext* _context;
  
  // Internal state for LFO
  float _lfoPhaseTime = 0.0f;
  float _carrierPhase = 0.0f;
  
  // Ramp state
  float _rampElapsedTime = 0.0f;
  bool _isRamping = false;
  
  // Panning state
  float _panEnvPhaseTime = 0.0f;
  float _panOscPhase = 0.0f;
  bool _panEnvActive = false;
  
  // Helper methods
  float calculateLFOValue(float phaseTime, float inhaleDur, float exhaleDur);
  float getCurrentPeriod();
  void updatePanning(float* leftGain, float* rightGain, float lfoValue, int framesToProcess);
  
public:
  explicit MartigliNode(BaseAudioContext *context);
  
  // Martigli parameters
  float mf0 = 250.0f;        // Base frequency added to modulation
  float ma = 90.0f;          // Modulation amount (multiplier)
  float mp0 = 10.0f;         // Initial period (seconds)
  float mp1 = 20.0f;         // Final period (seconds)
  float md = 600.0f;         // Ramp duration (seconds)
  
  // Asymmetric breathing (optional)
  float inhaleDur = -1.0f;   // If > 0, use asymmetric breathing
  float exhaleDur = -1.0f;   // If > 0, use asymmetric breathing
  
  // Waveform and volume
  int waveformM = 0;         // 0=sine, 1=triangle, 2=square, 3=sawtooth
  float volume = 0.5f;       // Volume (linear 0-1)
  
  // Panning parameters
  int panOsc = 0;            // 0=none, 1=envelope, 2=independent sine, 3=synced to LFO
  float panOscPeriod = 120.0f;   // Period for panning (seconds)
  float panOscTrans = 20.0f;     // Transition time for envelope panning (seconds)
  
  // Animation value (for JS to read)
  float animationValue = 0.0f;
  
  // Pause/resume control
  bool isPaused = false;
  
  // Control flags (set by JS to trigger actions)
  bool shouldStart = false;
  bool shouldPause = false;
  bool shouldResume = false;
  
  // Control methods (called internally)
  void start();
  void pause();
  void resume();

protected:
  void processNode(const std::shared_ptr<AudioBus> &bus,
                   int framesToProcess) override;
};
} // namespace audioapi
