#include "MartigliNode.h"
#include "AnimationValueRegistry.h"
#include <audioapi/core/BaseAudioContext.h>
#include <audioapi/utils/AudioBus.h>
#include <audioapi/utils/AudioArray.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace audioapi {

MartigliNode::MartigliNode(BaseAudioContext *context)
    : AudioNode(context), _context(context) {
    isInitialized_ = true;
}

void MartigliNode::start() {
    _isRamping = true;
    _rampElapsedTime = 0.0f;
    isPaused = false;
    // Start volume fade-in
    _currentGain = 0.0f;
    _targetGain = 1.0f;
    _rampDuration = 1.0f;
    _rampElapsed = 0.0f;
    _isVolumeRamping = true;
    
    // Initialize cycle durations
    _lfoPhaseTime = 0.0f;
    _lastPhase = 0.0f;
    if (inhaleDur > 0.0f && exhaleDur > 0.0f) {
        float scale = mp0 / (inhaleDur + exhaleDur);
        _currentCycleInhale = inhaleDur * scale;
        _currentCycleExhale = exhaleDur * scale;
    } else {
        _currentCycleInhale = _currentCycleExhale = mp0 * 0.5f;
    }
}

void MartigliNode::pause() {
    isPaused = true;
    // Quick fade to silence
    _targetGain = 0.0f;
    _rampDuration = 0.5f;
    _rampElapsed = 0.0f;
    _isVolumeRamping = true;
}

void MartigliNode::resume() {
    isPaused = false;
    // Quick fade back to full volume
    _targetGain = 1.0f;
    _rampDuration = 0.5f;
    _rampElapsed = 0.0f;
    _isVolumeRamping = true;
}

void MartigliNode::stop() {
    // Start fade-out
    _targetGain = 0.0f;
    _rampDuration = 1.0f;
    _rampElapsed = 0.0f;
    _isVolumeRamping = true;
}

void MartigliNode::processNode(const std::shared_ptr<AudioBus> &bus, int framesToProcess) {
    auto sampleRate = _context->getSampleRate();
    float dt = 1.0f / sampleRate;
    
    // Handle control flags
    if (shouldStart) { start(); shouldStart = false; }
    if (shouldPause) { pause(); shouldPause = false; }
    if (shouldResume) { resume(); shouldResume = false; }
    if (shouldStop) { stop(); shouldStop = false; }
    
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
            } else {
                // Linear interpolation
                _currentGain = _currentGain + (_targetGain - _currentGain) * t;
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
        
        // Generate carrier waveform with frequency modulation
        float carrierFreq = lfoValue * ma + mf0;
        if (carrierFreq < 20.0f) carrierFreq = 20.0f;
        if (carrierFreq > 20000.0f) carrierFreq = 20000.0f;
        
        float carrier = 0.0f;
        switch (waveformM) {
            case 1: carrier = 4.0f * fabsf(fmod(_carrierPhase / (2.0f * M_PI) + 0.75f, 1.0f) - 0.5f) - 1.0f; break;
            case 2: carrier = (_carrierPhase < M_PI) ? 1.0f : -1.0f; break;
            case 3: carrier = 2.0f * (_carrierPhase / (2.0f * M_PI)) - 1.0f; break;
            default: carrier = sinf(_carrierPhase);
        }
        
        carrier *= volume * _currentGain;
        
        // Calculate panning
        float panValue = 0.0f;
        if (panOsc == 1) {
            float panPhase = fmod(_panEnvPhaseTime, panOscPeriod * 2.0f);
            panValue = (panPhase < panOscTrans) ? panPhase / panOscTrans :
                      (panPhase < panOscTrans + panOscPeriod) ? 1.0f :
                      (panPhase < panOscTrans * 2.0f + panOscPeriod) ? 1.0f - (panPhase - panOscTrans - panOscPeriod) / panOscTrans : 0.0f;
            panValue = panValue * 2.0f - 1.0f;
            if (!isPaused) _panEnvPhaseTime += dt;
        } else if (panOsc == 2) {
            panValue = sinf(_panOscPhase);
            if (!isPaused) {
                _panOscPhase += 2.0f * M_PI * dt / panOscPeriod;
                if (_panOscPhase >= 2.0f * M_PI) _panOscPhase -= 2.0f * M_PI;
            }
        } else if (panOsc == 3) {
            panValue = lfoValue;
        }
        
        float leftGain = (numChannels >= 2) ? (1.0f + panValue) * 0.5f : 1.0f;
        float rightGain = (numChannels >= 2) ? (1.0f - panValue) * 0.5f : 0.0f;
        
        if (numChannels >= 1) bus->getChannel(0)->getData()[i] = carrier * leftGain;
        if (numChannels >= 2) bus->getChannel(1)->getData()[i] = carrier * rightGain;
        
        // Advance phases
        if (!isPaused) {
            _lfoPhaseTime += dt;
            _carrierPhase += 2.0f * M_PI * carrierFreq * dt;
            if (_carrierPhase >= 2.0f * M_PI) _carrierPhase -= 2.0f * M_PI;
            if (_isRamping) _rampElapsedTime += dt;
        }
    }
}

} // namespace audioapi
