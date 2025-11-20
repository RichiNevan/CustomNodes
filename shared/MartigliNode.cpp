#include "MartigliNode.h"
#include <audioapi/core/BaseAudioContext.h>
#include <audioapi/utils/AudioBus.h>
#include <audioapi/utils/AudioArray.h>
#include <cmath>
#include <cstdio>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace audioapi {

MartigliNode::MartigliNode(BaseAudioContext *context)
    : AudioNode(context), _context(context) {
    isInitialized_ = true;
    // Start at phase 90 degrees (bottom of wave, start of inhale)
    _lfoPhaseTime = 0.0f;
}

void MartigliNode::start() {
    _isRamping = true;
    _rampElapsedTime = 0.0f;
    isPaused = false;
    printf("MartigliNode: Started\n");
}

void MartigliNode::pause() {
    isPaused = true;
    printf("MartigliNode: Paused\n");
}

void MartigliNode::resume() {
    isPaused = false;
    printf("MartigliNode: Resumed\n");
}

float MartigliNode::getCurrentPeriod() {
    if (!_isRamping || md <= 0.0f) {
        return mp1;
    }
    
    float progress = _rampElapsedTime / md;
    if (progress >= 1.0f) {
        _isRamping = false;
        return mp1;
    }
    
    // Linear interpolation from mp0 to mp1
    return mp0 + (mp1 - mp0) * progress;
}

float MartigliNode::calculateLFOValue(float phaseTime, float currentInhaleDur, float currentExhaleDur) {
    float totalPeriod = currentInhaleDur + currentExhaleDur;
    
    // Wrap phase time
    while (phaseTime >= totalPeriod) {
        phaseTime -= totalPeriod;
    }
    
    if (phaseTime < currentInhaleDur) {
        // Inhale phase: -1 to +1
        float t_norm = phaseTime / currentInhaleDur;
        return -cosf(M_PI * t_norm); // Goes from -1 to +1
    } else {
        // Exhale phase: +1 to -1
        float t_norm = (phaseTime - currentInhaleDur) / currentExhaleDur;
        return cosf(M_PI * t_norm); // Goes from +1 to -1
    }
}

void MartigliNode::updatePanning(float* leftGain, float* rightGain, float lfoValue, int framesToProcess) {
    auto sampleRate = _context->getSampleRate();
    float dt = 1.0f / sampleRate;
    
    if (panOsc == 0) {
        // No panning - center (equal on both channels)
        *leftGain = 0.5f;
        *rightGain = 0.5f;
    } else if (panOsc == 1) {
        // Envelope-based panning
        float totalLoopPeriod = panOscPeriod * 2.0f;
        
        // Wrap phase time
        while (_panEnvPhaseTime >= totalLoopPeriod) {
            _panEnvPhaseTime -= totalLoopPeriod;
        }
        
        float envValue = 0.0f;
        
        if (_panEnvPhaseTime < panOscTrans) {
            // Attack phase: 0 to 1
            envValue = _panEnvPhaseTime / panOscTrans;
        } else if (_panEnvPhaseTime < panOscTrans + panOscPeriod) {
            // Sustain phase: hold at 1
            envValue = 1.0f;
        } else if (_panEnvPhaseTime < panOscTrans * 2.0f + panOscPeriod) {
            // Release phase: 1 to 0
            float releaseTime = _panEnvPhaseTime - (panOscTrans + panOscPeriod);
            envValue = 1.0f - (releaseTime / panOscTrans);
        } else {
            // Hold at 0
            envValue = 0.0f;
        }
        
        // Scale from [0,1] to [-1,+1]
        float panValue = envValue * 2.0f - 1.0f;
        
        // Left gets panValue, right gets -panValue
        *leftGain = (1.0f + panValue) * 0.5f;
        *rightGain = (1.0f - panValue) * 0.5f;
        
        if (!isPaused) {
            _panEnvPhaseTime += dt * framesToProcess;
        }
        
    } else if (panOsc == 2) {
        // Independent sine oscillator
        float panValue = sinf(_panOscPhase);
        *leftGain = (1.0f + panValue) * 0.5f;
        *rightGain = (1.0f - panValue) * 0.5f;
        
        if (!isPaused) {
            float panFreq = 1.0f / panOscPeriod;
            _panOscPhase += 2.0f * M_PI * panFreq * dt * framesToProcess;
            if (_panOscPhase >= 2.0f * M_PI) {
                _panOscPhase -= 2.0f * M_PI;
            }
        }
        
    } else if (panOsc == 3) {
        // Synced to LFO
        float panValue = lfoValue;
        *leftGain = (1.0f + panValue) * 0.5f;
        *rightGain = (1.0f - panValue) * 0.5f;
    }
}

void MartigliNode::processNode(const std::shared_ptr<AudioBus> &bus,
                              int framesToProcess) {
    auto sampleRate = _context->getSampleRate();
    float dt = 1.0f / sampleRate;
    
    // Handle control flags
    if (shouldStart) {
        start();
        shouldStart = false;
    }
    if (shouldPause) {
        pause();
        shouldPause = false;
    }
    if (shouldResume) {
        resume();
        shouldResume = false;
    }
    
    // Determine current breathing period
    float currentPeriod = getCurrentPeriod();
    
    // Calculate current inhale and exhale durations
    float currentInhaleDur, currentExhaleDur;
    if (inhaleDur > 0.0f && exhaleDur > 0.0f) {
        // Asymmetric breathing
        float initialPeriod = inhaleDur + exhaleDur;
        float scaleRatio = currentPeriod / initialPeriod;
        currentInhaleDur = inhaleDur * scaleRatio;
        currentExhaleDur = exhaleDur * scaleRatio;
    } else {
        // Symmetric breathing
        currentInhaleDur = currentPeriod / 2.0f;
        currentExhaleDur = currentPeriod / 2.0f;
    }
    
    // Get panning gains
    float leftGain = 0.5f;
    float rightGain = 0.5f;
    
    // Process each frame
    for (int i = 0; i < framesToProcess; ++i) {
        // Calculate LFO value
        float lfoValue = calculateLFOValue(_lfoPhaseTime, currentInhaleDur, currentExhaleDur);
        
        // Update animation value (normalize to 0-1)
        animationValue = (lfoValue + 1.0f) * 0.5f;
        
        // Apply modulation chain: lfoValue * ma + mf0 = carrier frequency
        float carrierFrequency = lfoValue * ma + mf0;
        
        // Clamp frequency to reasonable range
        if (carrierFrequency < 20.0f) carrierFrequency = 20.0f;
        if (carrierFrequency > 20000.0f) carrierFrequency = 20000.0f;
        
        // Generate carrier waveform
        float carrierValue = 0.0f;
        switch (waveformM) {
            case 0: // Sine
                carrierValue = sinf(_carrierPhase);
                break;
            case 1: // Triangle
                {
                    float t = _carrierPhase / (2.0f * M_PI);
                    carrierValue = 4.0f * fabsf(t - floorf(t + 0.5f)) - 1.0f;
                }
                break;
            case 2: // Square
                carrierValue = (_carrierPhase < M_PI) ? 1.0f : -1.0f;
                break;
            case 3: // Sawtooth
                carrierValue = 2.0f * (_carrierPhase / (2.0f * M_PI)) - 1.0f;
                break;
            default:
                carrierValue = sinf(_carrierPhase);
        }
        
        // Apply volume
        carrierValue *= volume;
        
        // Update panning for this frame (only once per buffer for efficiency)
        if (i == 0) {
            updatePanning(&leftGain, &rightGain, lfoValue, framesToProcess);
        }
        
        // Apply panning to stereo channels
        int numChannels = bus->getNumberOfChannels();
        if (numChannels >= 2) {
            bus->getChannel(0)->getData()[i] = carrierValue * leftGain;
            bus->getChannel(1)->getData()[i] = carrierValue * rightGain;
            // If more channels, fill them with silence or copies
            for (int j = 2; j < numChannels; ++j) {
                bus->getChannel(j)->getData()[i] = 0.0f;
            }
        } else if (numChannels == 1) {
            // Mono output - mix left and right
            bus->getChannel(0)->getData()[i] = carrierValue * (leftGain + rightGain) * 0.5f;
        }
        
        // Advance phases
        if (!isPaused) {
            _lfoPhaseTime += dt;
            _carrierPhase += 2.0f * M_PI * carrierFrequency * dt;
            if (_carrierPhase >= 2.0f * M_PI) {
                _carrierPhase -= 2.0f * M_PI;
            }
        }
    }
    
    // Update ramp elapsed time
    if (!isPaused && _isRamping) {
        _rampElapsedTime += dt * framesToProcess;
    }
}

} // namespace audioapi
