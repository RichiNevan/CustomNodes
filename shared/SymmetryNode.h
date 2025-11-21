#pragma once

#include <audioapi/core/AudioNode.h>
#include <audioapi/core/BaseAudioContext.h>
#include <vector>
#include <cmath>
#include <random>

namespace audioapi {

class SymmetryNode : public AudioNode {
public:
  explicit SymmetryNode(BaseAudioContext *context);
  ~SymmetryNode() override = default;

  void processNode(const std::shared_ptr<AudioBus> &bus, int framesToProcess) override;

  // Core parameters
  float f0 = 220.0f;           // Base frequency (Hz)
  float noctaves = 1.0f;       // Number of octaves to span
  int nnotes = 8;              // Number of notes in the sequence
  float d = 32.0f;             // Loop duration (seconds)
  int waveform = 0;            // 0=sine, 1=triangle, 2=square, 3=sawtooth
  int permfunc = 0;            // 0=shuffle, 1=rotateForward, 2=rotateBack, 3=reverse, 4=none
  float volume = 0.5f;         // Master volume (0.0 to 1.0)

  // Control flags
  bool shouldStart = false;
  bool shouldStop = false;
  bool shouldPause = false;
  bool shouldResume = false;

  // Debug/monitoring
  int frameCount = 0;

private:
  // Oscillator state
  float _phase = 0.0f;
  
  // Note sequence
  std::vector<float> _notes;
  int _currentNoteIndex = 0;
  
  // Timing state
  float _loopPhaseTime = 0.0f;  // Time within current loop (0 to d)
  float _noteStartTime = 0.0f;  // When current note started
  float _noteSep = 0.0f;        // Time between note starts
  float _noteDur = 0.0f;        // Duration of each note
  bool _useEnvelope = false;    // Whether to use attack/decay envelope
  
  // Envelope state (for long notes)
  float _envelopeGain = 0.0f;
  float _notePhaseTime = 0.0f;  // Time within current note
  static constexpr float ENVELOPE_ATTACK = 2.0f;
  static constexpr float ENVELOPE_DECAY = 2.0f;
  
  // Volume ramping state machine
  enum class RampState {
    IDLE,
    RAMPING_UP,
    PLAYING,
    RAMPING_DOWN,
    PAUSED
  };
  RampState _rampState = RampState::IDLE;
  float _currentGain = 0.0f;
  float _targetGain = 0.0f;
  float _rampDuration = 1.0f;    // Duration of current ramp
  float _rampProgress = 0.0f;    // Progress through current ramp (0 to 1)
  
  // Random number generator for shuffle
  std::mt19937 _rng;
  
  // Helper methods
  void initializeNotes();
  void applyPermutation();
  float generateSample(float frequency);
  float calculateEnvelopeGain();
  void updateVolumeRamp(float deltaTime);
  
  // Permutation functions
  void shuffleNotes();
  void rotateNotesForward();
  void rotateNotesBackward();
  void reverseNotes();
};

} // namespace audioapi
