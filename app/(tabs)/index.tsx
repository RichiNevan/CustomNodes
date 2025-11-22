import React, { useEffect, useState, useRef } from "react";
import { StyleSheet, View, Text, ScrollView, Pressable, ActivityIndicator } from "react-native";
import Slider from "@react-native-community/slider";
import { Container, Button } from "../components";
import { SessionManager } from "../audio/SessionManager";
import { presets } from "../../testPresets";
import NativeOscillatorModule from "../../specs/NativeOscillatorModule";
import { DEFAULT_MASTER_VOLUME } from "../audio/AudioConfig";

const SessionTest = () => {
  const sessionManager = useRef<SessionManager | null>(null);
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

  // Initialize session manager
  useEffect(() => {
    // First inject the custom processor installer
    if (NativeOscillatorModule) {
      NativeOscillatorModule.injectCustomProcessorInstaller();
    }
    
    // Then create the session manager
    sessionManager.current = new SessionManager();
    const manager = sessionManager.current;

    manager.onStateChange = (newState: string) => {
      setState(newState);
    };

    manager.onTimerUpdate = (elapsed: number, remaining: number, total: number) => {
      setElapsed(elapsed);
      setRemaining(remaining);
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
    }
  };

  const handlePause = () => {
    if (sessionManager.current && state === "playing") {
      sessionManager.current.pause();
    }
  };

  const handleStop = () => {
    if (sessionManager.current && state !== "idle") {
      sessionManager.current.stop();
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

  const formatTime = (seconds: number) => {
    const mins = Math.floor(seconds / 60);
    const secs = seconds % 60;
    return `${mins}:${secs.toString().padStart(2, "0")}`;
  };

  return (
    <Container>
      <ScrollView style={styles.scrollView} contentContainerStyle={styles.content}>
        <Text style={styles.title}>Session Manager Test</Text>

        {!isReady && <ActivityIndicator color="#FFFFFF" size="large" />}

        {isReady && (
          <>
            {/* Preset Selection */}
            <View style={styles.section}>
              <Text style={styles.label}>Preset: {presetNames[selectedPresetIndex]}</Text>
          <View style={styles.selectorRow}>
            <Pressable
              style={[styles.arrowButton, state !== "idle" && styles.disabledButton]}
              onPress={() => setSelectedPresetIndex((selectedPresetIndex - 1 + presetNames.length) % presetNames.length)}
              disabled={state !== "idle"}
            >
              <Text style={styles.arrowText}>◀</Text>
            </Pressable>
            <View style={styles.selectorValue}>
              <Text style={styles.selectorText}>{presetNames[selectedPresetIndex]}</Text>
            </View>
            <Pressable
              style={[styles.arrowButton, state !== "idle" && styles.disabledButton]}
              onPress={() => setSelectedPresetIndex((selectedPresetIndex + 1) % presetNames.length)}
              disabled={state !== "idle"}
            >
              <Text style={styles.arrowText}>▶</Text>
            </Pressable>
          </View>
        </View>

        {/* Duration Selection */}
        <View style={styles.section}>
          <Text style={styles.label}>Duration: {durationOptions[durationIndex].label}</Text>
          <View style={styles.selectorRow}>
            <Pressable
              style={[styles.arrowButton, state !== "idle" && styles.disabledButton]}
              onPress={() => setDurationIndex((durationIndex - 1 + durationOptions.length) % durationOptions.length)}
              disabled={state !== "idle"}
            >
              <Text style={styles.arrowText}>◀</Text>
            </Pressable>
            <View style={styles.selectorValue}>
              <Text style={styles.selectorText}>{durationOptions[durationIndex].label}</Text>
            </View>
            <Pressable
              style={[styles.arrowButton, state !== "idle" && styles.disabledButton]}
              onPress={() => setDurationIndex((durationIndex + 1) % durationOptions.length)}
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
              onValueChange={handleMasterVolumeChange}
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
                  {index + 1}. {voice.type} - {Math.round(voice.volume * 100)}%
                </Text>
                <Slider
                  style={styles.slider}
                  minimumValue={0}
                  maximumValue={0.4}
                  value={voice.volume}
                  onValueChange={(value) => handleVoiceVolumeChange(index, value)}
                  minimumTrackTintColor="#1EB1FC"
                  maximumTrackTintColor="#8B8B8B"
                  thumbTintColor="#1EB1FC"
                />
              </View>
            ))}
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
  slider: {
    width: "100%",
    height: 40,
  },
});

export default SessionTest;
