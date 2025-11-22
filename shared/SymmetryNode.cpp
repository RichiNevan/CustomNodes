#include "SymmetryNode.h"
#include <audioapi/utils/AudioBus.h>
#include <audioapi/utils/AudioArray.h>
#include <algorithm>
#include <random>

namespace audioapi {

SymmetryNode::SymmetryNode(BaseAudioContext *context) 
  : AudioNode(context), _rng(std::random_device{}()) {
  
  isInitialized_ = true; // CRITICAL: Required for processNode() to be called
  
  channelCount_ = 2;
  channelCountMode_ = ChannelCountMode::EXPLICIT;
  channelInterpretation_ = ChannelInterpretation::SPEAKERS;
  
  // Initialize note array
  initializeNotes();
}

void SymmetryNode::processNode(const std::shared_ptr<AudioBus> &bus, int framesToProcess) {
  if (framesToProcess == 0 || bus->getNumberOfChannels() < 2) {
    return;
  }
  
  auto leftChannel = bus->getChannel(0);
  auto rightChannel = bus->getChannel(1);
  const float sampleRate = context_->getSampleRate();
  const float deltaTime = 1.0f / sampleRate;
  
  // Handle control flags
  if (shouldStart) {
    shouldStart = false;
    _rampState = RampState::RAMPING_UP;
    _currentGain = 0.0f;
    _targetGain = volume;
    _rampProgress = 0.0f;
    
    // Determine ramp duration based on note separation
    _noteSep = d / static_cast<float>(nnotes);
    _noteDur = _noteSep / 2.0f;
    _useEnvelope = (_noteSep > 10.0f);
    
    if (_useEnvelope) {
      _rampDuration = ENVELOPE_ATTACK; // Use envelope attack time
    } else {
      _rampDuration = 1.5f; // Fixed 1.5 seconds
    }
    
    _loopPhaseTime = 0.0f;
    _noteStartTime = 0.0f;
    _currentNoteIndex = 0;
    _phase = 0.0f;
    _notePhaseTime = 0.0f;
    _envelopeGain = 0.0f;
    
    initializeNotes();
    applyPermutation();
  }
  
  if (shouldStop) {
    shouldStop = false;
    _rampState = RampState::RAMPING_DOWN;
    _targetGain = 0.0f;
    _rampProgress = 0.0f;
    
    if (_useEnvelope) {
      _rampDuration = ENVELOPE_DECAY; // Use envelope decay time
    } else {
      _rampDuration = 1.5f; // Fixed 1.5 seconds
    }
  }
  
  if (shouldPause) {
    shouldPause = false;
    _rampState = RampState::RAMPING_DOWN;
    _targetGain = 0.0f;
    _rampProgress = 0.0f;
    _rampDuration = 1.0f; // Increased to 1s for testing
  }
  
  if (shouldResume) {
    shouldResume = false;
    _rampState = RampState::RAMPING_UP;
    _targetGain = volume;
    _rampProgress = 0.0f;
    _rampDuration = 0.5f; // Fixed 0.5 seconds for resume
  }
  
  // Process audio samples
  for (int i = 0; i < framesToProcess; i++) {
    frameCount++;
    
    // Update volume ramping
    updateVolumeRamp(deltaTime);
    
    float sample = 0.0f;
    
    if (_rampState != RampState::IDLE) {
      // Update loop phase
      _loopPhaseTime += deltaTime;
      _notePhaseTime += deltaTime;
      
      // Check if we've completed a full loop
      if (_loopPhaseTime >= d) {
        _loopPhaseTime -= d;
        _noteStartTime = 0.0f;
        _currentNoteIndex = 0;
        _notePhaseTime = 0.0f;
        applyPermutation();
      }
      
      // Check if it's time to start a new note
      float nextNoteTime = _currentNoteIndex * _noteSep;
      if (_loopPhaseTime >= nextNoteTime && _currentNoteIndex < nnotes) {
        if (_loopPhaseTime >= _noteStartTime + _noteSep) {
          _noteStartTime += _noteSep;
          _currentNoteIndex++;
          _notePhaseTime = 0.0f;
          _phase = 0.0f; // Reset phase for new note
        }
      }
      
      // Generate audio if within note duration
      if (_currentNoteIndex < nnotes && _notePhaseTime < _noteDur) {
        float frequency = _notes[_currentNoteIndex];
        sample = generateSample(frequency);
        
        // ALWAYS apply per-note envelope to prevent clicks
        float envGain = calculateEnvelopeGain();
        sample *= envGain;
        
        // Advance phase
        _phase += (frequency / sampleRate);
        if (_phase >= 1.0f) {
          _phase -= 1.0f;
        }
      }
      
      // Apply volume ramping
      sample *= _currentGain;
    }
    
    // Write to both channels (mono signal)
    leftChannel->getData()[i] = sample;
    rightChannel->getData()[i] = sample;
  }
}

void SymmetryNode::initializeNotes() {
  _notes.clear();
  
  if (nnotes <= 0) {
    nnotes = 1;
  }
  
  // Calculate frequency factor: freqFact = 2^(noctaves/nnotes)
  float freqFact = std::pow(2.0f, noctaves / static_cast<float>(nnotes));
  
  // Generate note sequence
  _notes.push_back(f0);
  for (int i = 1; i < nnotes; i++) {
    _notes.push_back(f0 * std::pow(freqFact, static_cast<float>(i)));
  }
  
  _currentNoteIndex = 0;
}

void SymmetryNode::applyPermutation() {
  switch (permfunc) {
    case 0: shuffleNotes(); break;
    case 1: rotateNotesForward(); break;
    case 2: rotateNotesBackward(); break;
    case 3: reverseNotes(); break;
    case 4: break; // none - do nothing
    default: break;
  }
}

float SymmetryNode::generateSample(float frequency) {
  float sample = 0.0f;
  
  switch (waveform) {
    case 0: // Sine
      sample = std::sin(2.0f * M_PI * _phase);
      break;
      
    case 1: // Triangle
      if (_phase < 0.25f) {
        sample = 4.0f * _phase;
      } else if (_phase < 0.75f) {
        sample = 2.0f - 4.0f * _phase;
      } else {
        sample = 4.0f * _phase - 4.0f;
      }
      break;
      
    case 2: // Square
      sample = (_phase < 0.5f) ? 1.0f : -1.0f;
      break;
      
    case 3: // Sawtooth
      sample = 2.0f * _phase - 1.0f;
      break;
      
    default:
      sample = std::sin(2.0f * M_PI * _phase);
      break;
  }
  
  return sample;
}

float SymmetryNode::calculateEnvelopeGain() {
  float gain = 1.0f;
  
  // Use shorter attack/release times for short notes to prevent clicks
  // For long notes (noteSep > 10s), use 2s attack/decay
  // For short notes, use proportional times (e.g., 10% of note duration)
  float attackTime = ENVELOPE_ATTACK;
  float releaseTime = ENVELOPE_DECAY;
  
  if (!_useEnvelope) {
    // For short notes, use much shorter envelope times
    // Use 10% of note duration or 100ms minimum, whichever is larger
    attackTime = std::max(0.1f, _noteDur * 0.1f);
    releaseTime = std::max(0.1f, _noteDur * 0.1f);
  }
  
  // Attack phase
  if (_notePhaseTime < attackTime) {
    gain = _notePhaseTime / attackTime;
  }
  // Release phase
  else if (_notePhaseTime > (_noteDur - releaseTime)) {
    float timeIntoRelease = _notePhaseTime - (_noteDur - releaseTime);
    gain = 1.0f - (timeIntoRelease / releaseTime);
  }
  // Sustain phase
  else {
    gain = 1.0f;
  }
  
  // Clamp to 0.0 - 1.0
  if (gain < 0.0f) gain = 0.0f;
  if (gain > 1.0f) gain = 1.0f;
  
  return gain;
}

void SymmetryNode::updateVolumeRamp(float deltaTime) {
  if (_rampState == RampState::RAMPING_UP || _rampState == RampState::RAMPING_DOWN) {
    _rampProgress += deltaTime / _rampDuration;
    
    if (_rampProgress >= 1.0f) {
      _rampProgress = 1.0f;
      _currentGain = _targetGain;
      
      if (_rampState == RampState::RAMPING_UP) {
        _rampState = RampState::PLAYING;
      } else { // RAMPING_DOWN
        if (_targetGain == 0.0f) {
          // Check if this was a pause or a stop
          // If shouldResume hasn't been set, assume it's a pause
          _rampState = RampState::PAUSED;
        }
      }
    } else {
      // Linear interpolation
      float startGain = (_rampState == RampState::RAMPING_UP) ? 0.0f : _currentGain;
      _currentGain = startGain + (_targetGain - startGain) * _rampProgress;
    }
  } else if (_rampState == RampState::PLAYING) {
    // Track volume changes while playing
    if (_currentGain != volume) {
      _currentGain = volume;
    }
  }
}

// Permutation functions
void SymmetryNode::shuffleNotes() {
  if (_notes.size() <= 1) return;
  
  // Fisher-Yates shuffle
  for (int i = _notes.size() - 1; i > 0; i--) {
    std::uniform_int_distribution<int> dist(0, i);
    int j = dist(_rng);
    std::swap(_notes[i], _notes[j]);
  }
}

void SymmetryNode::rotateNotesForward() {
  if (_notes.size() <= 1) return;
  
  float last = _notes.back();
  _notes.pop_back();
  _notes.insert(_notes.begin(), last);
}

void SymmetryNode::rotateNotesBackward() {
  if (_notes.size() <= 1) return;
  
  float first = _notes.front();
  _notes.erase(_notes.begin());
  _notes.push_back(first);
}

void SymmetryNode::reverseNotes() {
  if (_notes.size() <= 1) return;
  
  std::reverse(_notes.begin(), _notes.end());
}

} // namespace audioapi
