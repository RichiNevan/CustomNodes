import React, { useCallback, useEffect, useState, FC, useRef } from "react";
import {
  ActivityIndicator,
  StyleSheet,
  View,
  Text,
  ScrollView,
} from "react-native";
import { Container, Button } from "../components";
import NativeOscillatorModule from "../../specs/NativeOscillatorModule";
import { MartigliNode } from "./types";
import { AudioContext } from "react-native-audio-api";

export default function TabTwoScreen() {
  const [isReady, setIsReady] = useState(false);
  const [isPlaying, setIsPlaying] = useState(false);
  const [animationValue, setAnimationValue] = useState(0);

  const audioContext = useRef<AudioContext | null>(null);
  const martigliNode = useRef<MartigliNode | null>(null);
  const animationInterval = useRef<ReturnType<typeof setInterval> | null>(null);

  // Example preset from prompts.txt
  const examplePreset = {
    id: 1.2,
    mf0: 250,
    waveformM: 0,
    ma: 90,
    mp0: undefined as number | undefined,
    inhaleDur: 3,
    exhaleDur: 8,
    mp1: 20,
    md: 600,
    type: "Martigli",
    panOsc: 0,
    panOscPeriod: 120,
    panOscTrans: 20,
    isOn: true,
    iniVolume: null,
  };

  useEffect(() => {
    if (audioContext.current) return;

    audioContext.current = new AudioContext();

    if (NativeOscillatorModule) {
      console.log("NativeOscillatorModule is available");
      console.log("Injecting custom processor installer");
      NativeOscillatorModule.injectCustomProcessorInstaller();
    } else {
      console.log("NativeOscillatorModule is not available");
    }
    setIsReady(true);
  }, []);

  const toggleMartigli = async () => {
    if (!audioContext.current) return;
    console.log(`Toggling Martigli. Currently playing: ${isPlaying}`);

    if (isPlaying) {
      // Stop
      if (martigliNode.current) {
        martigliNode.current.disconnect();
        martigliNode.current = null;
      }
      if (animationInterval.current) {
        clearInterval(animationInterval.current);
        animationInterval.current = null;
      }
      await audioContext.current.suspend();
      setIsPlaying(false);
    } else {
      // Start
      await audioContext.current.resume();

      // Create the Martigli node
      const node = new MartigliNode(
        audioContext.current,
        global.createMartigliNode(audioContext.current.context)
      );

      // Apply the example preset
      node.mf0 = examplePreset.mf0;
      node.ma = examplePreset.ma;
      
      // Set asymmetric breathing if specified
      if (examplePreset.inhaleDur && examplePreset.exhaleDur) {
        node.inhaleDur = examplePreset.inhaleDur;
        node.exhaleDur = examplePreset.exhaleDur;
        // Calculate mp0 from inhale + exhale durations
        node.mp0 = examplePreset.inhaleDur + examplePreset.exhaleDur;
      } else {
        node.mp0 = examplePreset.mp0 || 10;
      }
      
      node.mp1 = examplePreset.mp1;
      node.md = examplePreset.md;
      node.waveformM = examplePreset.waveformM;
      node.panOsc = examplePreset.panOsc;
      node.panOscPeriod = examplePreset.panOscPeriod;
      node.panOscTrans = examplePreset.panOscTrans;
      node.volume = 0.3; // Set a reasonable volume

      // Connect to destination
      node.connect(audioContext.current.destination);

      // Start the node
      node.start();

      martigliNode.current = node;

      // Start animation value polling
      animationInterval.current = setInterval(() => {
        if (martigliNode.current) {
          setAnimationValue(martigliNode.current.animationValue);
        }
      }, 16); // ~60fps

      setIsPlaying(true);
    }
  };

  const handlePauseResume = () => {
    if (martigliNode.current) {
      if (martigliNode.current.isPaused) {
        martigliNode.current.resume();
      } else {
        martigliNode.current.pause();
      }
    }
  };

  return (
    <Container centered>
      {!isReady && <ActivityIndicator color="#FFFFFF" />}

      <View style={styles.content}>
        <Text style={styles.title}>Martigli Breathing Guide</Text>

        <Button
          title={isPlaying ? "Stop Martigli" : "Start Martigli"}
          onPress={toggleMartigli}
          disabled={!isReady}
        />

        {isPlaying && (
          <>
            <Button
              title={martigliNode.current?.isPaused ? "Resume" : "Pause"}
              onPress={handlePauseResume}
              disabled={!isReady || !martigliNode.current}
            />

            <View style={styles.animationContainer}>
              <Text style={styles.label}>Breathing Animation Value:</Text>
              <View style={styles.progressBar}>
                <View
                  style={[
                    styles.progressFill,
                    { width: `${animationValue * 100}%` },
                  ]}
                />
              </View>
              <Text style={styles.valueText}>
                {(animationValue * 100).toFixed(1)}%
              </Text>
            </View>

            <View style={styles.presetInfo}>
              <Text style={styles.infoLabel}>Preset Parameters:</Text>
              <Text style={styles.infoText}>
                Base Frequency: {examplePreset.mf0} Hz
              </Text>
              <Text style={styles.infoText}>
                Modulation Amount: {examplePreset.ma}
              </Text>
              {examplePreset.inhaleDur && examplePreset.exhaleDur ? (
                <>
                  <Text style={styles.infoText}>
                    Inhale Duration: {examplePreset.inhaleDur}s
                  </Text>
                  <Text style={styles.infoText}>
                    Exhale Duration: {examplePreset.exhaleDur}s
                  </Text>
                  <Text style={styles.infoText}>
                    Initial Period: {examplePreset.inhaleDur + examplePreset.exhaleDur}s
                  </Text>
                </>
              ) : (
                <Text style={styles.infoText}>
                  Initial Period: {examplePreset.mp0}s
                </Text>
              )}
              <Text style={styles.infoText}>
                Final Period: {examplePreset.mp1}s
              </Text>
              <Text style={styles.infoText}>
                Ramp Duration: {examplePreset.md}s
              </Text>
              <Text style={styles.infoText}>
                Panning:{" "}
                {examplePreset.panOsc === 0
                  ? "None"
                  : `Mode ${examplePreset.panOsc}`}
              </Text>
            </View>
          </>
        )}
      </View>
    </Container>
  );
}

const styles = StyleSheet.create({
  content: {
    width: "100%",
    paddingHorizontal: 20,
    alignItems: "center",
  },
  title: {
    color: "#FFFFFF",
    fontSize: 24,
    fontWeight: "bold",
    marginBottom: 30,
    textAlign: "center",
  },
  animationContainer: {
    width: "100%",
    marginTop: 30,
    padding: 20,
    backgroundColor: "rgba(255, 255, 255, 0.1)",
    borderRadius: 10,
  },
  label: {
    color: "#FFFFFF",
    fontSize: 16,
    fontWeight: "600",
    marginBottom: 10,
  },
  progressBar: {
    width: "100%",
    height: 30,
    backgroundColor: "rgba(0, 0, 0, 0.3)",
    borderRadius: 15,
    overflow: "hidden",
    marginBottom: 10,
  },
  progressFill: {
    height: "100%",
    backgroundColor: "#1EB1FC",
    borderRadius: 15,
  },
  valueText: {
    color: "#FFFFFF",
    fontSize: 18,
    fontWeight: "bold",
    textAlign: "center",
  },
  presetInfo: {
    width: "100%",
    marginTop: 20,
    padding: 15,
    backgroundColor: "rgba(255, 255, 255, 0.05)",
    borderRadius: 10,
  },
  infoLabel: {
    color: "#FFFFFF",
    fontSize: 16,
    fontWeight: "600",
    marginBottom: 10,
  },
  infoText: {
    color: "#CCCCCC",
    fontSize: 14,
    marginBottom: 5,
  },
});
