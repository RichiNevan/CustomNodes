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
    float currentPeriod = mp0;
    if (_isRamping && md > 0.0f) {
        float progress = std::min(_rampElapsedTime / md, 1.0f);
        currentPeriod = mp0 + (mp1 - mp0) * progress;
        if (progress >= 1.0f) _isRamping = false;
    } else {
        currentPeriod = mp1;
    }
    
    // Store current period for JS access
    this->currentPeriod = currentPeriod;
    
    // Calculate inhale/exhale durations
    float inhale, exhale;
    if (inhaleDur > 0.0f && exhaleDur > 0.0f) {
        float scale = currentPeriod / (inhaleDur + exhaleDur);
        inhale = inhaleDur * scale;
        exhale = exhaleDur * scale;
    } else {
        inhale = exhale = currentPeriod * 0.5f;
    }
    
    // Store current inhale/exhale for JS access
    this->currentInhaleDur = inhale;
    this->currentExhaleDur = exhale;
    
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
        
        // Calculate LFO value using piecewise cosine
        float totalPeriod = inhale + exhale;
        float phase = fmod(_lfoPhaseTime, totalPeriod);
        float lfoValue = (phase < inhale) 
            ? -cosf(M_PI * phase / inhale)
            : cosf(M_PI * (phase - inhale) / exhale);
        
        animationValue = (lfoValue + 1.0f) * 0.5f;
        
        // Publish animation value to registry if this is the active martigli
        AnimationValueRegistry::getInstance().setMartigliAnimationValue(animationValue, isOn);
        
        // Modulation: lfoValue * ma + mf0 = carrier frequency
        float carrierFreq = std::clamp(lfoValue * ma + mf0, 20.0f, 20000.0f);
        
        // Generate carrier waveform
        float carrier = 0.0f;
        switch (waveformM) {
            case 1: carrier = 4.0f * fabsf(fmod(_carrierPhase / (2.0f * M_PI) + 0.75f, 1.0f) - 0.5f) - 1.0f; break; // Triangle
            case 2: carrier = (_carrierPhase < M_PI) ? 1.0f : -1.0f; break; // Square
            case 3: carrier = 2.0f * (_carrierPhase / (2.0f * M_PI)) - 1.0f; break; // Sawtooth
            default: carrier = sinf(_carrierPhase); // Sine
        }
        
        // Apply volume and volume ramping
        carrier *= volume * _currentGain;
        
        // Calculate panning
        float panValue = 0.0f;
        if (panOsc == 1) { // Envelope panning
            float loopPeriod = panOscPeriod * 2.0f;
            float panPhase = fmod(_panEnvPhaseTime, loopPeriod);
            if (panPhase < panOscTrans) panValue = panPhase / panOscTrans;
            else if (panPhase < panOscTrans + panOscPeriod) panValue = 1.0f;
            else if (panPhase < panOscTrans * 2.0f + panOscPeriod) panValue = 1.0f - (panPhase - panOscTrans - panOscPeriod) / panOscTrans;
            panValue = panValue * 2.0f - 1.0f;
            if (!isPaused) _panEnvPhaseTime += dt;
        } else if (panOsc == 2) { // Independent sine
            panValue = sinf(_panOscPhase);
            if (!isPaused) {
                _panOscPhase += 2.0f * M_PI * dt / panOscPeriod;
                if (_panOscPhase >= 2.0f * M_PI) _panOscPhase -= 2.0f * M_PI;
            }
        } else if (panOsc == 3) { // Synced to LFO
            panValue = lfoValue;
        }
        
        float leftGain = (numChannels >= 2) ? (1.0f + panValue) * 0.5f : 1.0f;
        float rightGain = (numChannels >= 2) ? (1.0f - panValue) * 0.5f : 0.0f;
        
        // Write output
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
