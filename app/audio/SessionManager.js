// ============================================
// SESSION MANAGER
// ============================================
// Simplified audio engine for AVS sessions
// ============================================

import { AudioContext } from "react-native-audio-api";
import {
  MartigliNode,
  MartigliBinauralNode,
  BinauralNode,
  SymmetryNode,
  NoiseNode,
} from "../(tabs)/types";
import { DEFAULT_MASTER_VOLUME, getDefaultVolume } from "./AudioConfig";

export class SessionManager {
  constructor() {
    this.audioContext = new AudioContext();
    this.voices = []; // {node, volume}
    this.preset = null;
    this.duration = 900;
    this.state = "idle";

    // Timing
    this.startTime = null;
    this.pausedTime = 0;
    this.timerId = null;

    // Animation
    this.animationId = null;
    this.animationValue = 0;

    // Volume
    this.masterVolume = DEFAULT_MASTER_VOLUME;

    // Callbacks
    this.onTimerUpdate = null;
    this.onAnimationUpdate = null;
    this.onStateChange = null;
  }

  // ============================================
  // PUBLIC API
  // ============================================

  loadPreset(preset) {
    if (this.state !== "idle") {
      console.warn("Cannot load preset while session is active");
      return;
    }
    this.preset = preset;
    this.duration = preset?.header?.d ?? 900;
  }

  start() {
    if (this.state !== "idle" || !this.preset) return;

    this._createVoices();
    this._startVoices();
    this.startTime = Date.now();
    this.pausedTime = 0;
    this._startTimer();
    this._startAnimation();
    this._setState("playing");
  }

  pause() {
    if (this.state !== "playing") return;

    this._stopTimer();
    this._pauseVoices();
    this.pausedTime += Date.now() - this.startTime;
    this._setState("paused");
  }

  resume() {
    if (this.state !== "paused") return;

    this._resumeVoices();
    this.startTime = Date.now();
    this._startTimer();
    this._setState("playing");
  }

  stop() {
    if (this.state === "idle") return;

    this._stopTimer();
    this._stopAnimation();
    this._stopVoices();

    // Wait for fade-out (1.5s), then cleanup
    setTimeout(() => {
      this._cleanup();
    }, 1500);

    this._setState("stopped");
  }

  setMasterVolume(volume) {
    this.masterVolume = Math.max(0, Math.min(1, volume));
    this._updateVolumes();
  }

  setVoiceVolume(index, volume) {
    if (this.voices[index]) {
      this.voices[index].volume = Math.max(0, Math.min(1, volume));
      this._updateVolumes();
    }
  }

  getElapsedTime() {
    if (this.state === "idle") return 0;
    const current = this.state === "playing" ? Date.now() - this.startTime : 0;
    return Math.floor((this.pausedTime + current) / 1000);
  }

  getRemainingTime() {
    return Math.max(0, this.duration - this.getElapsedTime());
  }

  getAnimationValue() {
    return this.animationValue;
  }

  getState() {
    return this.state;
  }

  getVoices() {
    return this.voices.map((v, i) => ({
      index: i,
      type: v.node.constructor.name.replace("Node", ""),
      volume: v.volume,
    }));
  }

  getBreathingParams(voiceIndex) {
    const voice = this.voices[voiceIndex];
    if (!voice) return null;

    const node = voice.node;
    if (node.mp0 === undefined) return null; // Not a Martigli-type node

    return {
      mp0: node.mp0 ?? 0,
      mp1: node.mp1 ?? 1,
      inhaleDur: node.inhaleDur ?? 4,
      exhaleDur: node.exhaleDur ?? 6,
      currentPeriod: node.currentPeriod ?? 10,
      targetPeriod: node.mp1 ?? 10,  // Target is always mp1
    };
  }

  adjustBreathingPace(voiceIndex, direction) {
    const voice = this.voices[voiceIndex];
    if (!voice) return;

    const node = voice.node;
    if (node.mp0 === undefined) return; // Not a Martigli-type node

    const factor = direction === 'increase' ? 0.85 : 1.15; // 15% change

    // mp0 and mp1 are the breathing cycle period in SECONDS
    // To make breathing faster, we decrease the period (multiply by < 1)
    // To make breathing slower, we increase the period (multiply by > 1)
    node.mp0 = Math.max(1, node.mp0 * factor);  // Min 1 second
    node.mp1 = Math.max(1, node.mp1 * factor);  // Min 1 second
  }

  destroy() {
    this.stop();
    this.audioContext = null;
  }

  // ============================================
  // PRIVATE METHODS
  // ============================================

  _createVoices() {
    this.voices = [];
    if (!this.preset?.voices) return;

    this.preset.voices.forEach((settings) => {
      const node = this._createNode(settings);
      if (!node) return;

      node.connect(this.audioContext.destination);
      const volume = getDefaultVolume(settings.type, settings.iniVolume);
      this.voices.push({ node, volume });
    });
  }

  _createNode(settings) {
    const ctx = this.audioContext;
    let node = null;

    switch (settings.type) {
      case "Martigli":
        node = new MartigliNode(ctx, global.createMartigliNode(ctx.context));
        node.f = settings.f ?? 200;
        node.waveform = settings.waveform ?? 0;
        node.ma = settings.ma ?? 0.5;
        node.inhaleDur = settings.inhaleDur ?? 4;
        node.exhaleDur = settings.exhaleDur ?? 6;
        break;

      case "Martigli-Binaural":
        node = new MartigliBinauralNode(
          ctx,
          global.createMartigliBinauralNode(ctx.context)
        );
        node.fl = settings.fl ?? 200;
        node.fr = settings.fr ?? 210;
        node.waveformL = settings.waveformL ?? 0;
        node.waveformR = settings.waveformR ?? 0;
        node.ma = settings.ma ?? 0.5;
        node.mp0 = settings.mp0 ?? 0;
        node.mp1 = settings.mp1 ?? 1;
        node.md = settings.md ?? 10;
        node.inhaleDur = settings.inhaleDur ?? 4;
        node.exhaleDur = settings.exhaleDur ?? 6;
        node.panOsc = settings.panOsc ?? 0;
        node.panPeriod = settings.panPeriod ?? 5;
        node.panTrans = settings.panTrans ?? 0;
        break;

      case "Binaural":
        node = new BinauralNode(ctx, global.createBinauralNode(ctx.context));
        node.fl = settings.fl ?? 200;
        node.fr = settings.fr ?? 210;
        node.waveformL = settings.waveformL ?? 0;
        node.waveformR = settings.waveformR ?? 0;
        node.panOsc = settings.panOsc ?? 0;
        node.panPeriod = settings.panPeriod ?? 5;
        node.panTrans = settings.panTrans ?? 0;
        break;

      case "Symmetry":
        node = new SymmetryNode(ctx, global.createSymmetryNode(ctx.context));
        node.f0 = settings.f0 ?? 200;
        node.noctaves = settings.noctaves ?? 2;
        node.nnotes = settings.nnotes ?? 2;
        node.d = settings.d ?? 1;
        node.waveform = settings.waveform ?? 0;
        node.permfunc = settings.permfunc ?? 0;
        break;

      case "Noise":
        node = new NoiseNode(ctx, global.createNoiseNode(ctx.context));
        node.noiseColor = settings.noiseColor ?? 0; // 0=white, 1=pink, 2=brown
        break;
    }

    return node;
  }

  _startVoices() {
    this.voices.forEach(({ node }) => {
      node.start();
      // Set isOn for Martigli-type nodes
      if (node.isOn !== undefined) {
        node.isOn = true;
      }
    });
    this._updateVolumes();
  }

  _stopVoices() {
    this.voices.forEach(({ node }) => {
      if (node.isOn !== undefined) {
        node.isOn = false;
      }
      node.stop();
    });
  }

  _pauseVoices() {
    this.voices.forEach(({ node }) => {
      if (node.pause) {
        node.pause();
      }
    });
  }

  _resumeVoices() {
    this.voices.forEach(({ node }) => {
      if (node.resume) {
        node.resume();
      }
    });
  }

  _updateVolumes() {
    this.voices.forEach(({ node, volume }) => {
      node.volume = volume * this.masterVolume;
    });
  }

  _startTimer() {
    this.timerId = setInterval(() => {
      const elapsed = this.getElapsedTime();
      const remaining = this.getRemainingTime();

      if (this.onTimerUpdate) {
        this.onTimerUpdate(elapsed, remaining, this.duration);
      }

      if (remaining <= 0) {
        this.stop();
      }
    }, 100);
  }

  _stopTimer() {
    if (this.timerId) {
      clearInterval(this.timerId);
      this.timerId = null;
    }
  }

  _startAnimation() {
    this.animationId = setInterval(() => {
      // Find first active Martigli-type voice
      const martigliVoice = this.voices.find(
        ({ node }) =>
          node.isOn !== undefined && node.animationValue !== undefined
      );

      if (martigliVoice) {
        this.animationValue = martigliVoice.node.animationValue ?? 0;
        if (this.onAnimationUpdate) {
          this.onAnimationUpdate(this.animationValue);
        }
      }
    }, 16); // ~60fps
  }

  _stopAnimation() {
    if (this.animationId) {
      clearInterval(this.animationId);
      this.animationId = null;
    }
  }

  _cleanup() {
    this.voices.forEach(({ node }) => {
      try {
        node.disconnect();
      } catch (e) {
        // Already disconnected
      }
    });
    this.voices = [];
    this.preset = null;
    this.startTime = null;
    this.pausedTime = 0;
    this._setState("idle");
  }

  _setState(newState) {
    if (this.state === newState) return;
    this.state = newState;
    if (this.onStateChange) {
      this.onStateChange(newState);
    }
  }
}
