#pragma once

#include <audioapi/core/AudioNode.h>
#include <audioapi/core/BaseAudioContext.h>
#include "AnimationValueRegistry.h"

namespace audioapi {

class MartigliBinauralNode : public AudioNode {
public:
    explicit MartigliBinauralNode(BaseAudioContext *context);
    ~MartigliBinauralNode() override = default;

    void processNode(const std::shared_ptr<AudioBus> &bus, int framesToProcess) override;
    
    // Control methods
    void start();
    void pause();
    void resume();
    void stop();
    
    // Public parameters
    float fl = 250.0f;           // Left carrier frequency
    float fr = 260.0f;           // Right carrier frequency
    int waveformL = 0;           // Left waveform (0=sine, 1=tri, 2=square, 3=saw)
    int waveformR = 0;           // Right waveform
    float ma = 90.0f;            // Modulation amount
    float mp0 = 11.0f;           // Initial period
    float mp1 = 20.0f;           // Final period
    float md = 600.0f;           // Ramp duration
    float inhaleDur = 3.0f;      // Inhale duration (base)
    float exhaleDur = 8.0f;      // Exhale duration (base)
    float volume = 1.0f;         // Master volume
    int panOsc = 0;              // Panning mode (0=none, 1=envelope, 2=sine, 3=LFO)
    float panOscPeriod = 120.0f; // Panning period
    float panOscTrans = 20.0f;   // Panning transition time
    bool isOn = false;           // Publish to AnimationValueRegistry
    
    // Control flags
    bool shouldStart = false;
    bool shouldPause = false;
    bool shouldResume = false;
    bool shouldStop = false;
    bool isPaused = false;
    
    // Exposed values for UI
    float animationValue = 0.0f;
    float currentInhaleDur = 0.0f;
    float currentExhaleDur = 0.0f;
    float currentPeriod = 0.0f;
    
private:
    // LFO phase tracking
    float _lfoPhaseTime = 0.0f;
    float _currentCycleInhale = 0.0f;
    float _currentCycleExhale = 0.0f;
    float _lastPhase = 0.0f;
    
    // Carrier phases
    float _carrierPhaseL = 0.0f;
    float _carrierPhaseR = 0.0f;
    
    // Volume ramping
    float _currentGain = 0.0f;
    float _targetGain = 1.0f;
    float _rampDuration = 1.0f;
    float _rampElapsed = 0.0f;
    bool _isVolumeRamping = false;
    
    // Period ramping
    bool _isRamping = false;
    float _rampElapsedTime = 0.0f;
    
    // Panning
    float _panEnvPhaseTime = 0.0f;
    float _panOscPhase = 0.0f;
};

} // namespace audioapi
