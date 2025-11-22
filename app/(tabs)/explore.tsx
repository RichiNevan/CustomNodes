import React, { useEffect, useState, useRef } from "react";
import {
  ActivityIndicator,
  StyleSheet,
  View,
  Text,
  ScrollView,
  TouchableOpacity,
} from "react-native";
import Slider from "@react-native-community/slider";
import { Container, Button } from "../components";
import NativeOscillatorModule from "../../specs/NativeOscillatorModule";
import { MartigliNode, BinauralNode, SymmetryNode } from "./types";
import { AudioContext } from "react-native-audio-api";

const MARTIGLI_PRESET = {
  mf0: 250,
  ma: 90,
  mp1: 20,
  md: 600,
  waveformM: 0,
  inhaleDur: 5,
  exhaleDur: 5,
  panOsc: 0,
  panOscPeriod: 120,
  panOscTrans: 20,
};

const BINAURAL_PRESET = {
  fl: 245,
  fr: 260,
  waveformL: 0,
  waveformR: 0,
  panOsc: 3,
  panOscPeriod: 120,
  panOscTrans: 20,
};

const SYMMETRY_PRESET = {
  id: 3,
  nnotes: 2,
  noctaves: 2,
  f0: 200,
  d: 1,
  waveform: 0,
  permfunc: 0,
  type: "Symmetry",
  iniVolume: null,
};

export default function TabTwoScreen() {
  const [isReady, setIsReady] = useState(false);
  const [isPlayingMartigli, setIsPlayingMartigli] = useState(false);
  const [isPlayingBinaural, setIsPlayingBinaural] = useState(false);
  const [isPlayingSymmetry, setIsPlayingSymmetry] = useState(false);
  const [animationValue, setAnimationValue] = useState(0);

  // Current Martigli durations (for live display)
  const [currentInhaleDur, setCurrentInhaleDur] = useState(0);
  const [currentExhaleDur, setCurrentExhaleDur] = useState(0);

  // Base Martigli durations (user-adjustable)
  const [baseInhaleDur, setBaseInhaleDur] = useState(MARTIGLI_PRESET.inhaleDur);
  const [baseExhaleDur, setBaseExhaleDur] = useState(MARTIGLI_PRESET.exhaleDur);
  const [targetPeriod, setTargetPeriod] = useState(MARTIGLI_PRESET.mp1);

  // Volume controls
  const [martigliVolume, setMartigliVolume] = useState(0.3);
  const [binauralVolume, setBinauralVolume] = useState(0.3);
  const [symmetryVolume, setSymmetryVolume] = useState(0.3);

  // Panning test controls
  const [panOscMode, setPanOscMode] = useState(3);
  const [panOscPeriod, setPanOscPeriod] = useState(10);
  const [panOscTrans, setPanOscTrans] = useState(2);

  const audioContext = useRef<AudioContext | null>(null);
  const martigliNode = useRef<MartigliNode | null>(null);
  const binauralNode = useRef<BinauralNode | null>(null);
  const symmetryNode = useRef<SymmetryNode | null>(null);
  const animationInterval = useRef<ReturnType<typeof setInterval> | null>(null);

  useEffect(() => {
    audioContext.current = new AudioContext();
    NativeOscillatorModule?.injectCustomProcessorInstaller();
    setIsReady(true);
  }, []);

  const toggleMartigli = async () => {
    if (!audioContext.current) return;

    if (isPlayingMartigli) {
      martigliNode.current?.stop();
      if (animationInterval.current) clearInterval(animationInterval.current);
      // Wait for fade-out before disconnecting
      setTimeout(() => {
        martigliNode.current?.disconnect();
        martigliNode.current = null;
      }, 1000);
      setIsPlayingMartigli(false);
    } else {
      if (!isPlayingMartigli && !isPlayingBinaural) {
        await audioContext.current.resume();
      }

      const node = new MartigliNode(
        audioContext.current,
        global.createMartigliNode(audioContext.current.context)
      );
      Object.assign(node, MARTIGLI_PRESET, {
        mp0: baseInhaleDur + baseExhaleDur,
        inhaleDur: baseInhaleDur,
        exhaleDur: baseExhaleDur,
        volume: martigliVolume,
      });
      node.connect(audioContext.current.destination);
      node.start();
      martigliNode.current = node;

      // Set this as the active martigli for animation registry
      node.isOn = true;

      animationInterval.current = setInterval(() => {
        setAnimationValue(martigliNode.current?.animationValue || 0);
        // Also update current durations for display
        if (martigliNode.current) {
          const inhale = martigliNode.current.currentInhaleDur;
          const exhale = martigliNode.current.currentExhaleDur;
          const period = martigliNode.current.currentPeriod;

          setCurrentInhaleDur(inhale);
          setCurrentExhaleDur(exhale);

          // Debug: log occasionally to verify
          if (Math.random() < 0.02) {
            // ~1 time per second
            console.log(
              `Martigli timing - Period: ${period.toFixed(
                2
              )}s, Inhale: ${inhale.toFixed(2)}s, Exhale: ${exhale.toFixed(
                2
              )}s, Total: ${(inhale + exhale).toFixed(2)}s`
            );
          }
        }
      }, 16);

      setIsPlayingMartigli(true);
    }
  };

  const toggleBinaural = async () => {
    if (!audioContext.current) return;

    if (isPlayingBinaural) {
      binauralNode.current?.stop();
      // Wait for fade-out before disconnecting
      setTimeout(() => {
        binauralNode.current?.disconnect();
        binauralNode.current = null;
      }, 1000);
      setIsPlayingBinaural(false);
    } else {
      if (!isPlayingMartigli && !isPlayingBinaural) {
        await audioContext.current.resume();
      }

      console.log("Creating BinauralNode...");
      console.log(
        "global.createBinauralNode exists?",
        typeof global.createBinauralNode
      );
      console.log(
        "audioContext.current.context:",
        audioContext.current.context
      );

      const rawNode = global.createBinauralNode(audioContext.current.context);
      console.log("Raw node created:", rawNode);

      const node = new BinauralNode(audioContext.current, rawNode);
      console.log("BinauralNode wrapper created:", node);

      // Apply preset and test controls
      Object.assign(node, BINAURAL_PRESET, {
        volume: binauralVolume,
        panOsc: panOscMode,
        panOscPeriod: panOscPeriod,
        panOscTrans: panOscTrans,
      });
      console.log(
        "Properties assigned - fl:",
        node.fl,
        "fr:",
        node.fr,
        "panOsc:",
        node.panOsc,
        "panOscPeriod:",
        node.panOscPeriod,
        "panOscTrans:",
        node.panOscTrans
      );

      node.connect(audioContext.current.destination);
      console.log("Node connected to destination");

      // Check shouldStart before and after calling start()
      console.log("shouldStart before start():", (rawNode as any).shouldStart);
      node.start();
      console.log("shouldStart after start():", (rawNode as any).shouldStart);
      console.log("Node start() called");

      // Poll to see if shouldStart changes
      setTimeout(() => {
        console.log("shouldStart after 100ms:", (rawNode as any).shouldStart);
        console.log("frameCount after 100ms:", (rawNode as any).frameCount);
      }, 100);

      setTimeout(() => {
        console.log("frameCount after 500ms:", (rawNode as any).frameCount);
      }, 500);

      binauralNode.current = node;

      setIsPlayingBinaural(true);
    }
  };

  const toggleSymmetry = async () => {
    if (!audioContext.current) return;

    if (isPlayingSymmetry) {
      symmetryNode.current?.stop();
      // Wait for fade-out before disconnecting
      setTimeout(() => {
        symmetryNode.current?.disconnect();
        symmetryNode.current = null;
      }, 1000);
      setIsPlayingSymmetry(false);
    } else {
      if (!isPlayingMartigli && !isPlayingBinaural && !isPlayingSymmetry) {
        await audioContext.current.resume();
      }

      console.log("Creating SymmetryNode...");
      const rawNode = global.createSymmetryNode(audioContext.current.context);
      const node = new SymmetryNode(audioContext.current, rawNode);

      Object.assign(node, SYMMETRY_PRESET, { volume: symmetryVolume });
      console.log(
        "Symmetry params - f0:",
        node.f0,
        "nnotes:",
        node.nnotes,
        "d:",
        node.d
      );

      node.connect(audioContext.current.destination);
      node.start();
      console.log("SymmetryNode started");

      symmetryNode.current = node;
      setIsPlayingSymmetry(true);
    }
  };

  return (
    <Container centered>
      <ScrollView
        style={styles.scrollView}
        contentContainerStyle={styles.scrollContent}
        showsVerticalScrollIndicator={true}
      >
        {!isReady && <ActivityIndicator color="#FFF" />}

        <Text style={styles.title}>Audio Voice Tests</Text>

        <View style={styles.section}>
          <Text style={styles.subtitle}>🫁 Martigli Breathing Guide</Text>

          <View style={styles.sliderContainer}>
            <Text style={styles.label}>
              Volume: {(martigliVolume * 100).toFixed(0)}%
            </Text>
            <Slider
              style={styles.slider}
              minimumValue={0}
              maximumValue={1}
              value={martigliVolume}
              onValueChange={(val) => {
                setMartigliVolume(val);
                if (martigliNode.current) martigliNode.current.volume = val;
              }}
              minimumTrackTintColor="#1EB1FC"
              maximumTrackTintColor="#8B8B8B"
              thumbTintColor="#1EB1FC"
              disabled={!isReady}
            />
          </View>

          <Button
            title={isPlayingMartigli ? "Stop Martigli" : "Start Martigli"}
            onPress={toggleMartigli}
            disabled={!isReady}
          />

          {isPlayingMartigli && (
            <>
              <Button
                title={martigliNode.current?.isPaused ? "Resume" : "Pause"}
                onPress={() =>
                  martigliNode.current?.isPaused
                    ? martigliNode.current?.resume()
                    : martigliNode.current?.pause()
                }
              />

              <View style={styles.box}>
                <Text style={styles.label}>
                  Breathing: {(animationValue * 100).toFixed(0)}%
                </Text>
                <View style={styles.bar}>
                  <View
                    style={[styles.fill, { width: `${animationValue * 100}%` }]}
                  />
                </View>
              </View>

              <View style={styles.box}>
                <Text style={styles.label}>Breathing Rhythm Control</Text>
                <Text style={styles.text}>
                  Current Period:{" "}
                  {(currentInhaleDur + currentExhaleDur).toFixed(1)}s
                </Text>
                <Text style={styles.subtext}>
                  Inhale: {currentInhaleDur.toFixed(1)}s | Exhale:{" "}
                  {currentExhaleDur.toFixed(1)}s
                </Text>
                <Text style={styles.subtext}>
                  Base: {baseInhaleDur.toFixed(1)}s in /{" "}
                  {baseExhaleDur.toFixed(1)}s out
                </Text>
                <Text style={styles.subtext}>
                  Target: {targetPeriod.toFixed(1)}s over{" "}
                  {(MARTIGLI_PRESET.md / 60).toFixed(0)} min
                </Text>

                <View style={styles.buttonRow}>
                  <TouchableOpacity
                    style={[
                      styles.adjustButton,
                      (baseInhaleDur < 2 || baseExhaleDur < 2) && styles.disabledButton
                    ]}
                    disabled={baseInhaleDur < 2 || baseExhaleDur < 2}
                    onPress={() => {
                      const factor = 0.9;
                      const newInhale = Math.max(1, baseInhaleDur * factor);
                      const newExhale = Math.max(1, baseExhaleDur * factor);
                      const newMp0 = newInhale + newExhale;
                      const newMp1 =
                        newMp0 *
                        (targetPeriod / (baseInhaleDur + baseExhaleDur));

                      setBaseInhaleDur(newInhale);
                      setBaseExhaleDur(newExhale);
                      setTargetPeriod(newMp1);

                      if (martigliNode.current) {
                        Object.assign(martigliNode.current, {
                          inhaleDur: newInhale,
                          exhaleDur: newExhale,
                          mp0: newMp0,
                          mp1: newMp1,
                        });
                      }
                    }}
                  >
                    <Text style={styles.adjustButtonText}>− 10%</Text>
                    <Text style={styles.adjustButtonLabel}>SLOWER</Text>
                  </TouchableOpacity>

                  <TouchableOpacity
                    style={[
                      styles.adjustButton,
                      (baseInhaleDur > 15 || baseExhaleDur > 15) && styles.disabledButton
                    ]}
                    disabled={baseInhaleDur > 15 || baseExhaleDur > 15}
                    onPress={() => {
                      const factor = 1.1;
                      const newInhale = Math.min(20, baseInhaleDur * factor);
                      const newExhale = Math.min(30, baseExhaleDur * factor);
                      const newMp0 = newInhale + newExhale;
                      const newMp1 =
                        newMp0 *
                        (targetPeriod / (baseInhaleDur + baseExhaleDur));

                      setBaseInhaleDur(newInhale);
                      setBaseExhaleDur(newExhale);
                      setTargetPeriod(newMp1);

                      if (martigliNode.current) {
                        Object.assign(martigliNode.current, {
                          inhaleDur: newInhale,
                          exhaleDur: newExhale,
                          mp0: newMp0,
                          mp1: newMp1,
                        });
                      }
                    }}
                  >
                    <Text style={styles.adjustButtonText}>+ 10%</Text>
                    <Text style={styles.adjustButtonLabel}>FASTER</Text>
                  </TouchableOpacity>
                </View>
              </View>
            </>
          )}
        </View>

        <View style={styles.section}>
          <Text style={styles.subtitle}>🎧 Binaural Beats - Panning Test</Text>

          <View style={styles.sliderContainer}>
            <Text style={styles.label}>
              Volume: {(binauralVolume * 100).toFixed(0)}%
            </Text>
            <Slider
              style={styles.slider}
              minimumValue={0}
              maximumValue={1}
              value={binauralVolume}
              onValueChange={(val) => {
                setBinauralVolume(val);
                if (binauralNode.current) binauralNode.current.volume = val;
              }}
              minimumTrackTintColor="#1EB1FC"
              maximumTrackTintColor="#8B8B8B"
              thumbTintColor="#1EB1FC"
              disabled={!isReady}
            />
          </View>

          {!isPlayingBinaural && (
            <>
              <View style={styles.controlGroup}>
                <Text style={styles.label}>Panning Mode:</Text>
                <View style={styles.buttonRow}>
                  {["None", "Ping-Pong", "Sine", "Martigli"].map(
                    (label, idx) => (
                      <TouchableOpacity
                        key={idx}
                        onPress={() => setPanOscMode(idx)}
                        style={[
                          styles.modeButton,
                          panOscMode === idx && styles.activeButton,
                        ]}
                      >
                        <Text style={styles.buttonText}>{label}</Text>
                      </TouchableOpacity>
                    )
                  )}
                </View>
              </View>

              {(panOscMode === 1 || panOscMode === 2) && (
                <View style={styles.sliderContainer}>
                  <Text style={styles.label}>Period: {panOscPeriod}s</Text>
                  <Slider
                    style={styles.slider}
                    minimumValue={5}
                    maximumValue={30}
                    step={1}
                    value={panOscPeriod}
                    onValueChange={setPanOscPeriod}
                    minimumTrackTintColor="#1EB1FC"
                    maximumTrackTintColor="#8B8B8B"
                    thumbTintColor="#1EB1FC"
                  />
                </View>
              )}

              {panOscMode === 1 && (
                <View style={styles.sliderContainer}>
                  <Text style={styles.label}>Transition: {panOscTrans}s</Text>
                  <Slider
                    style={styles.slider}
                    minimumValue={1}
                    maximumValue={10}
                    step={1}
                    value={panOscTrans}
                    onValueChange={setPanOscTrans}
                    minimumTrackTintColor="#1EB1FC"
                    maximumTrackTintColor="#8B8B8B"
                    thumbTintColor="#1EB1FC"
                  />
                </View>
              )}
            </>
          )}

          <Button
            title={isPlayingBinaural ? "Stop Binaural" : "Start Binaural"}
            onPress={toggleBinaural}
            disabled={!isReady}
          />

          {isPlayingBinaural && (
            <>
              <Button
                title={binauralNode.current?.isPaused ? "Resume" : "Pause"}
                onPress={() =>
                  binauralNode.current?.isPaused
                    ? binauralNode.current?.resume()
                    : binauralNode.current?.pause()
                }
              />

              <View style={styles.box}>
                <Text style={styles.label}>Panning Info</Text>
                <Text style={styles.text}>
                  Mode: {["None", "Ping-Pong", "Sine", "Martigli"][panOscMode]}
                </Text>
                <Text style={styles.text}>
                  {BINAURAL_PRESET.fl}Hz (L) • {BINAURAL_PRESET.fr}Hz (R) •{" "}
                  {Math.abs(BINAURAL_PRESET.fl - BINAURAL_PRESET.fr)}Hz beat
                </Text>
                {panOscMode === 3 && (
                  <Text style={styles.text}>
                    Martigli Sync: {(animationValue * 100).toFixed(0)}%
                    {!isPlayingMartigli && " (Start Martigli!)"}
                  </Text>
                )}
              </View>
            </>
          )}
        </View>

        <View style={styles.section}>
          <Text style={styles.subtitle}>🎵 Symmetry Note Sequences</Text>

          <View style={styles.sliderContainer}>
            <Text style={styles.label}>
              Volume: {(symmetryVolume * 100).toFixed(0)}%
            </Text>
            <Slider
              style={styles.slider}
              minimumValue={0}
              maximumValue={1}
              value={symmetryVolume}
              onValueChange={(val) => {
                setSymmetryVolume(val);
                if (symmetryNode.current) symmetryNode.current.volume = val;
              }}
              minimumTrackTintColor="#1EB1FC"
              maximumTrackTintColor="#8B8B8B"
              thumbTintColor="#1EB1FC"
              disabled={!isReady}
            />
          </View>

          <Button
            title={isPlayingSymmetry ? "Stop Symmetry" : "Start Symmetry"}
            onPress={toggleSymmetry}
            disabled={!isReady}
          />

          {isPlayingSymmetry && (
            <>
              <Button
                title="Pause/Resume"
                onPress={() =>
                  symmetryNode.current?.pause
                    ? symmetryNode.current?.pause()
                    : symmetryNode.current?.resume()
                }
              />

              <View style={styles.box}>
                <Text style={styles.label}>Parameters</Text>
                <Text style={styles.text}>
                  Base Frequency: {SYMMETRY_PRESET.f0}Hz | Notes:{" "}
                  {SYMMETRY_PRESET.nnotes}
                </Text>
                <Text style={styles.text}>
                  Octave Span: {SYMMETRY_PRESET.noctaves} | Loop:{" "}
                  {SYMMETRY_PRESET.d}s
                </Text>
                <Text style={styles.text}>
                  Note Duration:{" "}
                  {(SYMMETRY_PRESET.d / SYMMETRY_PRESET.nnotes / 2).toFixed(2)}s
                  | Separation:{" "}
                  {(SYMMETRY_PRESET.d / SYMMETRY_PRESET.nnotes).toFixed(2)}s
                </Text>
                <Text style={styles.text}>
                  Permutation:{" "}
                  {
                    ["Shuffle", "Rotate Fwd", "Rotate Back", "Reverse", "None"][
                      SYMMETRY_PRESET.permfunc
                    ]
                  }
                </Text>
              </View>
            </>
          )}
        </View>
      </ScrollView>
    </Container>
  );
}

const styles = StyleSheet.create({
  scrollView: {
    flex: 1,
    width: "100%",
  },
  scrollContent: {
    // padding: 3,
    paddingBottom: 40,
  },
  title: {
    color: "#FFF",
    fontSize: 22,
    fontWeight: "bold",
    marginBottom: 20,
    textAlign: "center",
  },
  section: {
    width: "100%",
    marginBottom: 20,
    backgroundColor: "rgba(255,255,255,0.05)",
    borderRadius: 12,
    padding: 16,
  },
  subtitle: {
    color: "#FFF",
    fontSize: 17,
    fontWeight: "600",
    marginBottom: 12,
  },
  box: {
    width: "100%",
    marginTop: 12,
    padding: 16,
    backgroundColor: "rgba(255,255,255,0.08)",
    borderRadius: 8,
  },
  sliderContainer: {
    marginBottom: 16,
    marginTop: 8,
  },
  slider: {
    width: "100%",
    height: 40,
  },
  controlGroup: {
    marginBottom: 12,
  },
  buttonRow: {
    flexDirection: "row",
    justifyContent: "space-between",
    gap: 8,
    marginTop: 8,
  },
  modeButton: {
    flex: 1,
    paddingVertical: 10,
    paddingHorizontal: 8,
    backgroundColor: "rgba(255,255,255,0.1)",
    borderRadius: 8,
    alignItems: "center",
  },
  activeButton: {
    backgroundColor: "#1EB1FC",
  },
  buttonText: {
    color: "#FFF",
    fontSize: 13,
    fontWeight: "600",
  },
  label: {
    color: "#FFF",
    fontSize: 15,
    fontWeight: "600",
    marginBottom: 8,
  },
  bar: {
    width: "100%",
    height: 24,
    backgroundColor: "rgba(0,0,0,0.3)",
    borderRadius: 12,
    overflow: "hidden",
    marginTop: 8,
  },
  fill: {
    height: "100%",
    backgroundColor: "#1EB1FC",
    borderRadius: 12,
  },
  text: {
    color: "rgba(255,255,255,0.75)",
    fontSize: 13,
    marginBottom: 4,
    lineHeight: 18,
  },
  subtext: {
    color: "rgba(255,255,255,0.6)",
    fontSize: 12,
    marginBottom: 4,
  },
  adjustButton: {
    flex: 1,
    paddingVertical: 16,
    paddingHorizontal: 12,
    backgroundColor: "rgba(30, 177, 252, 0.2)",
    borderRadius: 8,
    borderWidth: 1,
    borderColor: "rgba(30, 177, 252, 0.4)",
    alignItems: "center",
    justifyContent: "center",
  },
  disabledButton: {
    backgroundColor: "rgba(100, 100, 100, 0.1)",
    borderColor: "rgba(100, 100, 100, 0.2)",
    opacity: 0.5,
  },
  adjustButtonText: {
    color: "#1EB1FC",
    fontSize: 20,
    fontWeight: "bold",
    marginBottom: 4,
  },
  adjustButtonLabel: {
    color: "rgba(255,255,255,0.7)",
    fontSize: 11,
    fontWeight: "600",
    letterSpacing: 0.5,
  },
});
