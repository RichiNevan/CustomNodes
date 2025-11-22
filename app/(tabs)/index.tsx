import React, { useEffect, useState, useRef } from "react";
import {
  StyleSheet,
  View,
  Text,
  ScrollView,
  Pressable,
  ActivityIndicator,
} from "react-native";
import Slider from "@react-native-community/slider";
import { Container, Button } from "../components";
import { SessionManager } from "../audio/SessionManager";
import { presets } from "../../testPresets";
import NativeCustomNodesModule from "../../specs/NativeCustomNodesModule";
import { DEFAULT_MASTER_VOLUME } from "../audio/AudioConfig";
import { NoiseNode } from "./types";
import { AudioContext } from "react-native-audio-api";

const SessionTest = () => {
  const sessionManager = useRef<SessionManager | null>(null);
  const noiseNode = useRef<NoiseNode | null>(null);
  const [isReady, setIsReady] = useState(false);

  const presetNames = Object.keys(presets);
  const durationOptions = [
    { value: 60, label: "1 minute" },
    { value: 300, label: "5 minutes" },
    { value: 600, label: "10 minutes" },
    { value: 900, label: "15 minutes" },
  ];

  // Preset and duration selection
  const [selectedPresetIndex, setSelectedPresetIndex] = useState(0);
  const [durationIndex, setDurationIndex] = useState(3); // 15 min default

  // Session state
  const [state, setState] = useState("idle");
  const [elapsed, setElapsed] = useState(0);
  const [remaining, setRemaining] = useState(0);

  // Volume control
  const [masterVolume, setMasterVolume] = useState(DEFAULT_MASTER_VOLUME);
  const [voices, setVoices] = useState<any[]>([]);

  // Animation value from Martigli breathing
  const [animationValue, setAnimationValue] = useState(0);

  // Breathing parameters (for first Martigli voice)
  const [breathingParams, setBreathingParams] = useState<any>(null);

  // Noise toggle state
  const [noiseEnabled, setNoiseEnabled] = useState(false);
  const [noiseColor, setNoiseColor] = useState(0); // 0=white, 1=pink, 2=brown
  const [noiseVolume, setNoiseVolume] = useState(0.08);

  // Initialize session manager
  useEffect(() => {
    // First inject the custom processor installer
    if (NativeCustomNodesModule) {
      NativeCustomNodesModule.injectCustomProcessorInstaller();
    }

    // Then create the session manager
    sessionManager.current = new SessionManager();
    const manager = sessionManager.current;

    manager.onStateChange = (newState: string) => {
      setState(newState);
    };

    manager.onTimerUpdate = (
      elapsed: number,
      remaining: number,
      total: number
    ) => {
      setElapsed(elapsed);
      setRemaining(remaining);
    };

    manager.onAnimationUpdate = (value: number) => {
      setAnimationValue(value);
    };

    setIsReady(true);

    return () => {
      if (sessionManager.current) {
        sessionManager.current.destroy();
      }
    };
  }, []);

  const handlePlay = () => {
    const manager = sessionManager.current;
    if (!manager) return;

    if (state === "idle") {
      const presetName = presetNames[selectedPresetIndex];
      const preset = (presets as any)[presetName];
      if (!preset) return;

      // Load preset with custom duration
      const customPreset = {
        ...preset,
        header: {
          ...preset.header,
          d: durationOptions[durationIndex].value,
        },
      };

      manager.loadPreset(customPreset);
      manager.start();
      setVoices(manager.getVoices());
    } else if (state === "paused") {
      manager.resume();
      // Also resume noise if it was active
      if (noiseEnabled && noiseNode.current) {
        noiseNode.current.resume();
      }
    }
  };

  const handlePause = () => {
    if (sessionManager.current && state === "playing") {
      sessionManager.current.pause();
      // Also pause noise if active
      if (noiseEnabled && noiseNode.current) {
        noiseNode.current.pause();
      }
    }
  };

  const handleStop = () => {
    if (sessionManager.current && state !== "idle") {
      sessionManager.current.stop();
      // Also stop noise if active
      if (noiseEnabled && noiseNode.current) {
        noiseNode.current.stop();
        setTimeout(() => {
          noiseNode.current?.disconnect();
          noiseNode.current = null;
        }, 400);
        setNoiseEnabled(false);
      }
      // Wait for cleanup before allowing new preset selection
      setTimeout(() => {
        setVoices([]);
        setElapsed(0);
        setRemaining(0);
      }, 1600); // Slightly longer than the fade-out (1.5s)
    }
  };

  const handleMasterVolumeChange = (value: number) => {
    setMasterVolume(value);
    if (sessionManager.current) {
      sessionManager.current.setMasterVolume(value);
    }
  };

  const handleVoiceVolumeChange = (index: number, value: number) => {
    if (sessionManager.current) {
      sessionManager.current.setVoiceVolume(index, value);
      setVoices(sessionManager.current.getVoices());
    }
  };

  const handleVoiceVolumeSliding = (index: number, value: number) => {
    // Update visual state immediately without updating the audio
    setVoices((prev) =>
      prev.map((v, i) => (i === index ? { ...v, volume: value } : v))
    );
  };

  const handleBreathingAdjust = (direction: "increase" | "decrease") => {
    if (!sessionManager.current) return;

    // Find first Martigli-type voice
    const martigliIndex = voices.findIndex(
      (v) => v.type === "Martigli" || v.type === "MartigliBinaural"
    );

    if (martigliIndex >= 0) {
      sessionManager.current.adjustBreathingPace(martigliIndex, direction);
      // Force update of breathing params
      const params = sessionManager.current.getBreathingParams(martigliIndex);
      setBreathingParams(params);
    }
  };

  const updateBreathingParams = () => {
    if (!sessionManager.current) return;

    const martigliIndex = voices.findIndex(
      (v) => v.type === "Martigli" || v.type === "MartigliBinaural"
    );

    if (martigliIndex >= 0) {
      const params = sessionManager.current.getBreathingParams(martigliIndex);
      setBreathingParams(params);
    }
  };

  // Update breathing params periodically when playing
  useEffect(() => {
    if (state === "playing") {
      const interval = setInterval(updateBreathingParams, 100);
      return () => clearInterval(interval);
    }
  }, [state, voices]);

  const handleNoiseToggle = () => {
    if (!sessionManager.current) return;

    if (noiseEnabled) {
      // Turn off noise
      if (noiseNode.current) {
        noiseNode.current.stop();
        setTimeout(() => {
          noiseNode.current?.disconnect();
          noiseNode.current = null;
        }, 400); // Wait for 0.3s fade + buffer
      }
      setNoiseEnabled(false);
    } else {
      // Turn on noise
      const ctx = sessionManager.current.audioContext;
      const node = new NoiseNode(ctx, global.createNoiseNode(ctx.context));
      node.noiseColor = noiseColor;
      node.volume = noiseVolume;
      node.connect(ctx.destination);
      node.start();
      noiseNode.current = node;
      setNoiseEnabled(true);
    }
  };

  const handleNoiseColorChange = (color: number) => {
    setNoiseColor(color);
    if (noiseNode.current) {
      noiseNode.current.noiseColor = color;
    }
  };

  const handleNoiseVolumeChange = (volume: number) => {
    setNoiseVolume(volume);
    if (noiseNode.current) {
      noiseNode.current.volume = volume;
    }
  };

  const formatTime = (seconds: number) => {
    const mins = Math.floor(seconds / 60);
    const secs = seconds % 60;
    return `${mins}:${secs.toString().padStart(2, "0")}`;
  };

  return (
    <Container>
      <ScrollView
        style={styles.scrollView}
        contentContainerStyle={styles.content}
      >
        <Text style={styles.title}>Session Manager Test</Text>

        {!isReady && <ActivityIndicator color="#FFFFFF" size="large" />}

        {isReady && (
          <>
            {/* Preset Selection */}
            <View style={styles.section}>
              <Text style={styles.label}>
                Preset: {presetNames[selectedPresetIndex]}
              </Text>
              <View style={styles.selectorRow}>
                <Pressable
                  style={[
                    styles.arrowButton,
                    state !== "idle" && styles.disabledButton,
                  ]}
                  onPress={() =>
                    setSelectedPresetIndex(
                      (selectedPresetIndex - 1 + presetNames.length) %
                        presetNames.length
                    )
                  }
                  disabled={state !== "idle"}
                >
                  <Text style={styles.arrowText}>◀</Text>
                </Pressable>
                <View style={styles.selectorValue}>
                  <Text style={styles.selectorText}>
                    {presetNames[selectedPresetIndex]}
                  </Text>
                </View>
                <Pressable
                  style={[
                    styles.arrowButton,
                    state !== "idle" && styles.disabledButton,
                  ]}
                  onPress={() =>
                    setSelectedPresetIndex(
                      (selectedPresetIndex + 1) % presetNames.length
                    )
                  }
                  disabled={state !== "idle"}
                >
                  <Text style={styles.arrowText}>▶</Text>
                </Pressable>
              </View>
            </View>

            {/* Duration Selection */}
            <View style={styles.section}>
              <Text style={styles.label}>
                Duration: {durationOptions[durationIndex].label}
              </Text>
              <View style={styles.selectorRow}>
                <Pressable
                  style={[
                    styles.arrowButton,
                    state !== "idle" && styles.disabledButton,
                  ]}
                  onPress={() =>
                    setDurationIndex(
                      (durationIndex - 1 + durationOptions.length) %
                        durationOptions.length
                    )
                  }
                  disabled={state !== "idle"}
                >
                  <Text style={styles.arrowText}>◀</Text>
                </Pressable>
                <View style={styles.selectorValue}>
                  <Text style={styles.selectorText}>
                    {durationOptions[durationIndex].label}
                  </Text>
                </View>
                <Pressable
                  style={[
                    styles.arrowButton,
                    state !== "idle" && styles.disabledButton,
                  ]}
                  onPress={() =>
                    setDurationIndex(
                      (durationIndex + 1) % durationOptions.length
                    )
                  }
                  disabled={state !== "idle"}
                >
                  <Text style={styles.arrowText}>▶</Text>
                </Pressable>
              </View>
            </View>

            {/* Control Buttons */}
            <View style={styles.buttonRow}>
              {state === "idle" && (
                <Button title="▶ Play" onPress={handlePlay} width={150} />
              )}
              {state === "playing" && (
                <>
                  <Button title="⏸ Pause" onPress={handlePause} width={120} />
                  <Button title="⏹ Stop" onPress={handleStop} width={120} />
                </>
              )}
              {state === "paused" && (
                <>
                  <Button title="▶ Resume" onPress={handlePlay} width={120} />
                  <Button title="⏹ Stop" onPress={handleStop} width={120} />
                </>
              )}
            </View>

            {/* Timer Display */}
            {state !== "idle" && (
              <View style={styles.timerSection}>
                <Text style={styles.timerText}>
                  {formatTime(elapsed)} / {formatTime(elapsed + remaining)}
                </Text>
                <Text style={styles.stateText}>State: {state}</Text>
              </View>
            )}

            {/* Breathing Animation Indicator */}
            {state === "playing" && (
              <View style={styles.animationSection}>
                <Text style={styles.animationLabel}>Breathing Guide:</Text>
                <View style={styles.breathingCircleContainer}>
                  <View
                    style={[
                      styles.breathingCircle,
                      {
                        transform: [
                          {
                            scale: 0.5 + animationValue * 0.5, // Scale from 0.5 to 1.0
                          },
                        ],
                        opacity: 0.6 + animationValue * 0.4, // Opacity from 0.6 to 1.0
                      },
                    ]}
                  />
                  <Text style={styles.breathingText}>
                    {animationValue < 0.5 ? "Breathe In" : "Breathe Out"}
                  </Text>
                </View>
              </View>
            )}

            {/* Breathing Pace Control */}
            {state === "playing" && breathingParams && (
              <View style={styles.section}>
                <Text style={styles.sectionTitle}>Breathing Pace:</Text>
                <View style={styles.breathingParamsContainer}>
                  <View style={styles.paceButtonRow}>
                    <Pressable
                      style={styles.paceButton}
                      onPress={() => handleBreathingAdjust("decrease")}
                    >
                      <Text style={styles.paceButtonText}>- Slower</Text>
                    </Pressable>
                    <Pressable
                      style={styles.paceButton}
                      onPress={() => handleBreathingAdjust("increase")}
                    >
                      <Text style={styles.paceButtonText}>+ Faster</Text>
                    </Pressable>
                  </View>

                  <View style={styles.cycleInfoContainer}>
                    <View style={styles.cycleInfoRow}>
                      <Text style={styles.cycleLabel}>Current Cycle:</Text>
                      <Text style={styles.cycleValue}>
                        {breathingParams.currentPeriod.toFixed(2)}s
                      </Text>
                    </View>
                    <View style={styles.cycleInfoRow}>
                      <Text style={styles.cycleLabel}>Target Cycle:</Text>
                      <Text style={styles.cycleValue}>
                        {breathingParams.targetPeriod.toFixed(2)}s
                      </Text>
                    </View>
                  </View>
                </View>
              </View>
            )}

            {/* Master Volume */}
            {state !== "idle" && (
              <View style={styles.section}>
                <Text style={styles.label}>
                  Master Volume: {Math.round(masterVolume * 100)}%
                </Text>
                <Slider
                  style={styles.slider}
                  minimumValue={0}
                  maximumValue={0.6}
                  value={masterVolume}
                  onValueChange={setMasterVolume}
                  onSlidingComplete={handleMasterVolumeChange}
                  minimumTrackTintColor="#1EB1FC"
                  maximumTrackTintColor="#8B8B8B"
                  thumbTintColor="#1EB1FC"
                />
              </View>
            )}

            {/* Voice Volumes */}
            {voices.length > 0 && (
              <View style={styles.section}>
                <Text style={styles.sectionTitle}>Voice Volumes:</Text>
                {voices.map((voice, index) => (
                  <View key={index} style={styles.voiceControl}>
                    <Text style={styles.voiceLabel}>
                      {index + 1}. {voice.type} -{" "}
                      {Math.round(voice.volume * 100)}%
                    </Text>
                    <Slider
                      style={styles.slider}
                      minimumValue={0}
                      maximumValue={0.4}
                      value={voice.volume}
                      onValueChange={(value) =>
                        handleVoiceVolumeSliding(index, value)
                      }
                      onSlidingComplete={(value) =>
                        handleVoiceVolumeChange(index, value)
                      }
                      minimumTrackTintColor="#1EB1FC"
                      maximumTrackTintColor="#8B8B8B"
                      thumbTintColor="#1EB1FC"
                    />
                  </View>
                ))}
              </View>
            )}

            {/* Noise Toggle */}
            {state !== "idle" && (
              <View style={styles.section}>
                <Text style={styles.sectionTitle}>Noise Generator:</Text>
                <View style={styles.noiseContainer}>
                  <View style={styles.noiseToggleRow}>
                    <Text style={styles.label}>Enable Noise</Text>
                    <Pressable
                      style={[
                        styles.toggleButton,
                        noiseEnabled && styles.toggleButtonActive,
                      ]}
                      onPress={handleNoiseToggle}
                    >
                      <Text style={styles.toggleButtonText}>
                        {noiseEnabled ? "ON" : "OFF"}
                      </Text>
                    </Pressable>
                  </View>

                  {noiseEnabled && (
                    <>
                      <View style={styles.noiseColorRow}>
                        <Text style={styles.label}>Color:</Text>
                        {["White", "Pink", "Brown"].map((color, idx) => (
                          <Pressable
                            key={idx}
                            style={[
                              styles.colorButton,
                              noiseColor === idx && styles.colorButtonActive,
                            ]}
                            onPress={() => handleNoiseColorChange(idx)}
                          >
                            <Text style={styles.colorButtonText}>{color}</Text>
                          </Pressable>
                        ))}
                      </View>

                      <View style={styles.sliderContainer}>
                        <Text style={styles.label}>
                          Volume: {Math.round(noiseVolume * 100)}%
                        </Text>
                        <Slider
                          style={styles.slider}
                          minimumValue={0}
                          maximumValue={0.3}
                          value={noiseVolume}
                          onValueChange={setNoiseVolume}
                          onSlidingComplete={handleNoiseVolumeChange}
                          minimumTrackTintColor="#1EB1FC"
                          maximumTrackTintColor="#8B8B8B"
                          thumbTintColor="#1EB1FC"
                        />
                      </View>
                    </>
                  )}
                </View>
              </View>
            )}
          </>
        )}
      </ScrollView>
    </Container>
  );
};

const styles = StyleSheet.create({
  scrollView: {
    width: "100%",
  },
  content: {
    padding: 20,
  },
  title: {
    color: "#FFFFFF",
    fontSize: 24,
    fontWeight: "700",
    marginBottom: 30,
    textAlign: "center",
  },
  section: {
    marginBottom: 20,
  },
  label: {
    color: "#FFFFFF",
    fontSize: 16,
    fontWeight: "600",
    marginBottom: 8,
  },
  selectorRow: {
    flexDirection: "row",
    alignItems: "center",
    gap: 10,
  },
  arrowButton: {
    backgroundColor: "#333",
    padding: 12,
    borderRadius: 8,
    minWidth: 50,
    alignItems: "center",
  },
  disabledButton: {
    opacity: 0.3,
  },
  arrowText: {
    color: "#1EB1FC",
    fontSize: 20,
    fontWeight: "700",
  },
  selectorValue: {
    flex: 1,
    backgroundColor: "#333",
    padding: 12,
    borderRadius: 8,
    alignItems: "center",
  },
  selectorText: {
    color: "#FFFFFF",
    fontSize: 16,
  },
  buttonRow: {
    flexDirection: "row",
    gap: 10,
    marginVertical: 20,
    justifyContent: "center",
  },
  timerSection: {
    padding: 15,
    backgroundColor: "#222",
    borderRadius: 8,
    marginBottom: 20,
  },
  timerText: {
    color: "#1EB1FC",
    fontSize: 20,
    fontWeight: "700",
    textAlign: "center",
    marginBottom: 5,
  },
  stateText: {
    color: "#FFFFFF",
    fontSize: 14,
    textAlign: "center",
  },
  animationSection: {
    padding: 20,
    backgroundColor: "#1a1a1a",
    borderRadius: 12,
    marginBottom: 20,
    alignItems: "center",
  },
  animationLabel: {
    color: "#1EB1FC",
    fontSize: 16,
    fontWeight: "600",
    marginBottom: 15,
  },
  breathingCircleContainer: {
    alignItems: "center",
    justifyContent: "center",
    height: 150,
  },
  breathingCircle: {
    width: 100,
    height: 100,
    borderRadius: 50,
    backgroundColor: "#1EB1FC",
    position: "absolute",
  },
  breathingText: {
    color: "#FFFFFF",
    fontSize: 18,
    fontWeight: "600",
    marginTop: 120,
  },
  sectionTitle: {
    color: "#FFFFFF",
    fontSize: 18,
    fontWeight: "700",
    marginBottom: 15,
  },
  voiceControl: {
    marginBottom: 15,
    padding: 10,
    backgroundColor: "#222",
    borderRadius: 8,
  },
  voiceLabel: {
    color: "#FFFFFF",
    fontSize: 14,
    marginBottom: 8,
  },
  breathingParamsContainer: {
    marginBottom: 20,
    padding: 15,
    backgroundColor: "#222",
    borderRadius: 8,
  },
  paceButtonRow: {
    flexDirection: "row",
    gap: 10,
    marginBottom: 15,
  },
  paceButton: {
    flex: 1,
    backgroundColor: "#1EB1FC",
    padding: 15,
    borderRadius: 8,
    alignItems: "center",
  },
  paceButtonText: {
    color: "#FFFFFF",
    fontSize: 16,
    fontWeight: "700",
  },
  cycleInfoContainer: {
    gap: 8,
  },
  cycleInfoRow: {
    flexDirection: "row",
    justifyContent: "space-between",
    alignItems: "center",
  },
  cycleLabel: {
    color: "#FFFFFF",
    fontSize: 14,
  },
  cycleValue: {
    color: "#1EB1FC",
    fontSize: 16,
    fontWeight: "700",
  },
  noiseContainer: {
    padding: 15,
    backgroundColor: "#222",
    borderRadius: 8,
  },
  noiseToggleRow: {
    flexDirection: "row",
    justifyContent: "space-between",
    alignItems: "center",
    marginBottom: 15,
  },
  toggleButton: {
    backgroundColor: "#333",
    paddingVertical: 8,
    paddingHorizontal: 20,
    borderRadius: 8,
    minWidth: 60,
    alignItems: "center",
  },
  toggleButtonActive: {
    backgroundColor: "#1EB1FC",
  },
  toggleButtonText: {
    color: "#FFFFFF",
    fontSize: 14,
    fontWeight: "700",
  },
  noiseColorRow: {
    flexDirection: "row",
    alignItems: "center",
    gap: 8,
    marginBottom: 15,
  },
  colorButton: {
    flex: 1,
    backgroundColor: "#333",
    paddingVertical: 10,
    borderRadius: 8,
    alignItems: "center",
  },
  colorButtonActive: {
    backgroundColor: "#1EB1FC",
  },
  colorButtonText: {
    color: "#FFFFFF",
    fontSize: 13,
    fontWeight: "600",
  },
  sliderContainer: {
    marginTop: 5,
  },
  slider: {
    width: "100%",
    height: 40,
  },
});

export default SessionTest;
