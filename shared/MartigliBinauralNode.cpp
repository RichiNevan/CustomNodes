#include "MartigliBinauralNode.h"
#include <audioapi/utils/AudioBus.h>
#include <audioapi/utils/AudioArray.h>
#include <cmath>

namespace audioapi {

MartigliBinauralNode::MartigliBinauralNode(BaseAudioContext *context) : AudioNode(context) {
    channelCount_ = 2;
    channelCountMode_ = ChannelCountMode::EXPLICIT;
    channelInterpretation_ = ChannelInterpretation::SPEAKERS;
    isInitialized_ = true;
}

void MartigliBinauralNode::start() {
    _isRamping = true;
    _rampElapsedTime = 0.0f;
    _lfoPhaseTime = 0.0f; // Start at trough (beginning of inhale)
    _carrierPhaseL = 0.0f;
    _carrierPhaseR = 0.0f;
    _lastPhase = 0.0f;
    
    // Initialize cycle durations
    if (inhaleDur > 0.0f && exhaleDur > 0.0f) {
        _currentCycleInhale = inhaleDur;
        _currentCycleExhale = exhaleDur;
    } else {
        _currentCycleInhale = _currentCycleExhale = mp0 * 0.5f;
    }
    
    isPaused = false;
    _currentGain = 0.0f;
    _startGain = 0.0f;
    _targetGain = 1.0f;
    _rampDuration = 1.5f;
    _rampElapsed = 0.0f;
    _isVolumeRamping = true;
}

void MartigliBinauralNode::pause() {
    _startGain = _isVolumeRamping ? _currentGain : 1.0f;
    _targetGain = 0.0f;
    _rampDuration = 0.5f;
    _rampElapsed = 0.0f;
    _isVolumeRamping = true;
}

void MartigliBinauralNode::resume() {
    isPaused = false;
    _lfoPhaseTime = 0.0f;
    _lastPhase = 0.0f;
    _startGain = _isVolumeRamping ? _currentGain : 0.0f;
    _targetGain = 1.0f;
    _rampDuration = 0.5f;
    _rampElapsed = 0.0f;
    _isVolumeRamping = true;
}

void MartigliBinauralNode::resetPhase() {
    _lfoPhaseTime = 0.0f;
    _lastPhase = 0.0f;
}

void MartigliBinauralNode::stop() {
    _startGain = _currentGain;
    _targetGain = 0.0f;
    _rampDuration = 1.5f;
    _rampElapsed = 0.0f;
    _isVolumeRamping = true;
}

void MartigliBinauralNode::processNode(const std::shared_ptr<AudioBus> &bus, int framesToProcess) {
    auto sampleRate = context_->getSampleRate();
    float dt = 1.0f / sampleRate;
    
    // Handle control flags
    if (shouldStart) { start(); shouldStart = false; }
    if (shouldPause) { pause(); shouldPause = false; }
    if (shouldResume) { resume(); shouldResume = false; }
    if (shouldStop) { stop(); shouldStop = false; }
    if (shouldResetPhase) { resetPhase(); shouldResetPhase = false; }
    
    // Calculate current period (with ramping)
    float currentPeriod = _isRamping && md > 0.0f ? mp0 + (mp1 - mp0) * std::min(_rampElapsedTime / md, 1.0f) : mp1;
    if (_isRamping && _rampElapsedTime >= md) {
        _isRamping = false;
        currentPeriod = mp1;
    }
    this->currentPeriod = currentPeriod;
    
    // Calculate and store inhale/exhale durations
    float scale = (inhaleDur > 0.0f && exhaleDur > 0.0f) ? currentPeriod / (inhaleDur + exhaleDur) : 0.5f;
    this->currentInhaleDur = (inhaleDur > 0.0f && exhaleDur > 0.0f) ? inhaleDur * scale : currentPeriod * 0.5f;
    this->currentExhaleDur = (inhaleDur > 0.0f && exhaleDur > 0.0f) ? exhaleDur * scale : currentPeriod * 0.5f;
    
    int numChannels = bus->getNumberOfChannels();
    
    for (int i = 0; i < framesToProcess; ++i) {
        // Update volume ramping
        if (_isVolumeRamping) {
            _rampElapsed += dt;
            float t = _rampElapsed / _rampDuration;
            if (t >= 1.0f) {
                _currentGain = _targetGain;
                _isVolumeRamping = false;
                if (_targetGain == 0.0f) {
                    isPaused = true;
                }
            } else {
                _currentGain = _startGain + (_targetGain - _startGain) * t;
            }
        }
        
        // Calculate LFO value using piecewise cosine with locked cycle durations
        float totalPeriod = _currentCycleInhale + _currentCycleExhale;
        float phase = fmod(_lfoPhaseTime, totalPeriod);
        
        // Detect cycle wrap and update durations for next cycle
        if (phase < _lastPhase) {
            _lfoPhaseTime = phase;
            float scale = (inhaleDur > 0.0f && exhaleDur > 0.0f) ? currentPeriod / (inhaleDur + exhaleDur) : 1.0f;
            _currentCycleInhale = (inhaleDur > 0.0f && exhaleDur > 0.0f) ? inhaleDur * scale : currentPeriod * 0.5f;
            _currentCycleExhale = (inhaleDur > 0.0f && exhaleDur > 0.0f) ? exhaleDur * scale : currentPeriod * 0.5f;
            totalPeriod = _currentCycleInhale + _currentCycleExhale;
        }
        _lastPhase = phase;
        
        float lfoValue = (phase < _currentCycleInhale) 
            ? -cosf(M_PI * phase / _currentCycleInhale)
            : cosf(M_PI * (phase - _currentCycleInhale) / _currentCycleExhale);
        
        animationValue = (lfoValue + 1.0f) * 0.5f;
        AnimationValueRegistry::getInstance().setMartigliAnimationValue(animationValue, isOn);
        
        // Generate left carrier with frequency modulation
        float carrierFreqL = lfoValue * ma + fl;
        if (carrierFreqL < 20.0f) carrierFreqL = 20.0f;
        if (carrierFreqL > 20000.0f) carrierFreqL = 20000.0f;
        
        float carrierL = 0.0f;
        switch (waveformL) {
            case 1: carrierL = 4.0f * fabsf(fmodf(_carrierPhaseL / (2.0f * M_PI) + 0.75f, 1.0f) - 0.5f) - 1.0f; break;
            case 2: carrierL = (_carrierPhaseL < M_PI) ? 1.0f : -1.0f; break;
            case 3: carrierL = 2.0f * (_carrierPhaseL / (2.0f * M_PI)) - 1.0f; break;
            default: carrierL = sinf(_carrierPhaseL);
        }
        
        // Generate right carrier with frequency modulation
        float carrierFreqR = lfoValue * ma + fr;
        if (carrierFreqR < 20.0f) carrierFreqR = 20.0f;
        if (carrierFreqR > 20000.0f) carrierFreqR = 20000.0f;
        
        float carrierR = 0.0f;
        switch (waveformR) {
            case 1: carrierR = 4.0f * fabsf(fmodf(_carrierPhaseR / (2.0f * M_PI) + 0.75f, 1.0f) - 0.5f) - 1.0f; break;
            case 2: carrierR = (_carrierPhaseR < M_PI) ? 1.0f : -1.0f; break;
            case 3: carrierR = 2.0f * (_carrierPhaseR / (2.0f * M_PI)) - 1.0f; break;
            default: carrierR = sinf(_carrierPhaseR);
        }
        
        carrierL *= volume * _currentGain;
        carrierR *= volume * _currentGain;
        
        // Calculate panning modulation based on panOsc mode
        float panGainL = 1.0f;
        float panGainR = 1.0f;
        
        if (panOsc == 1) {
            // Ping-pong crossfade mode (same as BinauralNode)
            float fullCycleDuration = 2.0f * panOscPeriod;
            float phaseInCycle = fmodf(_panEnvPhaseTime, fullCycleDuration);
            float crossfadeValue = 0.0f; // 0.0 = normal position, 1.0 = swapped
            
            if (phaseInCycle < (panOscPeriod - panOscTrans)) {
                crossfadeValue = 0.0f;
            } else if (phaseInCycle < panOscPeriod) {
                float transitionProgress = (phaseInCycle - (panOscPeriod - panOscTrans)) / panOscTrans;
                crossfadeValue = transitionProgress;
            } else if (phaseInCycle < (2.0f * panOscPeriod - panOscTrans)) {
                crossfadeValue = 1.0f;
            } else {
                float transitionProgress = (phaseInCycle - (2.0f * panOscPeriod - panOscTrans)) / panOscTrans;
                crossfadeValue = 1.0f - transitionProgress;
            }
            
            panGainL = 1.0f - crossfadeValue;
            panGainR = crossfadeValue;
            
            if (!isPaused) _panEnvPhaseTime += dt;
        } else if (panOsc == 2) {
            // Continuous sinusoidal panning (same as BinauralNode)
            float sinValue = sinf(_panOscPhase);
            float crossfadeValue = (sinValue + 1.0f) * 0.5f; // [-1,1] -> [0,1]
            panGainL = 1.0f - crossfadeValue;
            panGainR = crossfadeValue;
            
            if (!isPaused) {
                _panOscPhase += 2.0f * M_PI * dt / panOscPeriod;
                if (_panOscPhase >= 2.0f * M_PI) _panOscPhase -= 2.0f * M_PI;
            }
        } else if (panOsc == 3) {
            // Follow Martigli LFO breathing
            // lfoValue ranges from 0.0 (trough) to 1.0 (peak)
            panGainL = 1.0f - lfoValue;
            panGainR = lfoValue;
        }
        // panOsc == 0: no panning (both gains = 1.0)
        
        // Apply panning by mixing carriers to channels (same as BinauralNode)
        // Normal: leftCarrier→leftChannel (panGainL=1, panGainR=0)
        // Swapped: leftCarrier→rightChannel (panGainL=0, panGainR=1)
        if (numChannels >= 1) bus->getChannel(0)->getData()[i] = carrierL * panGainL + carrierR * panGainR;
        if (numChannels >= 2) bus->getChannel(1)->getData()[i] = carrierR * panGainL + carrierL * panGainR;
        
        // Advance phases
        if (!isPaused) {
            _lfoPhaseTime += dt;
            _carrierPhaseL += 2.0f * M_PI * carrierFreqL * dt;
            if (_carrierPhaseL >= 2.0f * M_PI) _carrierPhaseL -= 2.0f * M_PI;
            _carrierPhaseR += 2.0f * M_PI * carrierFreqR * dt;
            if (_carrierPhaseR >= 2.0f * M_PI) _carrierPhaseR -= 2.0f * M_PI;
            if (_isRamping) _rampElapsedTime += dt;
        }
    }
}

} // namespace audioapi
