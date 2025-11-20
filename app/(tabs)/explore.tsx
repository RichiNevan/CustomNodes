import React, { useEffect, useState, useRef } from "react";
import { ActivityIndicator, StyleSheet, View, Text } from "react-native";
import { Container, Button } from "../components";
import NativeOscillatorModule from "../../specs/NativeOscillatorModule";
import { MartigliNode, BinauralNode } from "./types";
import { AudioContext } from "react-native-audio-api";

const MARTIGLI_PRESET = {
  mf0: 250,
  ma: 90,
  mp1: 20,
  md: 600,
  waveformM: 0,
  inhaleDur: 3,
  exhaleDur: 8,
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

export default function TabTwoScreen() {
  const [isReady, setIsReady] = useState(false);
  const [isPlayingMartigli, setIsPlayingMartigli] = useState(false);
  const [isPlayingBinaural, setIsPlayingBinaural] = useState(false);
  const [animationValue, setAnimationValue] = useState(0);
  const audioContext = useRef<AudioContext | null>(null);
  const martigliNode = useRef<MartigliNode | null>(null);
  const binauralNode = useRef<BinauralNode | null>(null);
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
        mp0: MARTIGLI_PRESET.inhaleDur + MARTIGLI_PRESET.exhaleDur,
        volume: 0.3,
      });
      node.connect(audioContext.current.destination);
      node.start();
      martigliNode.current = node;

      animationInterval.current = setInterval(() => {
        setAnimationValue(martigliNode.current?.animationValue || 0);
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
      console.log("global.createBinauralNode exists?", typeof global.createBinauralNode);
      console.log("audioContext.current.context:", audioContext.current.context);
      
      const rawNode = global.createBinauralNode(audioContext.current.context);
      console.log("Raw node created:", rawNode);
      
      const node = new BinauralNode(audioContext.current, rawNode);
      console.log("BinauralNode wrapper created:", node);
      
      Object.assign(node, BINAURAL_PRESET, { volume: 0.3 });
      console.log("Properties assigned - fl:", node.fl, "fr:", node.fr, "volume:", node.volume);
      
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

  return (
    <Container centered>
      {!isReady && <ActivityIndicator color="#FFF" />}

      <Text style={styles.title}>Audio Voice Tests</Text>

      <View style={styles.section}>
        <Text style={styles.subtitle}>Martigli Breathing Guide</Text>
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
              <Text style={styles.label}>Parameters</Text>
              <Text style={styles.text}>
                Frequency: {MARTIGLI_PRESET.mf0}Hz | Modulation:{" "}
                {MARTIGLI_PRESET.ma}
              </Text>
              <Text style={styles.text}>
                Inhale: {MARTIGLI_PRESET.inhaleDur}s | Exhale:{" "}
                {MARTIGLI_PRESET.exhaleDur}s
              </Text>
              <Text style={styles.text}>
                Period:{" "}
                {MARTIGLI_PRESET.inhaleDur + MARTIGLI_PRESET.exhaleDur}s →{" "}
                {MARTIGLI_PRESET.mp1}s over {MARTIGLI_PRESET.md}s
              </Text>
            </View>
          </>
        )}
      </View>

      <View style={styles.section}>
        <Text style={styles.subtitle}>Binaural Beats</Text>
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
              <Text style={styles.label}>Parameters</Text>
              <Text style={styles.text}>
                Left: {BINAURAL_PRESET.fl}Hz | Right: {BINAURAL_PRESET.fr}Hz
              </Text>
              <Text style={styles.text}>
                Beat Frequency: {Math.abs(BINAURAL_PRESET.fl - BINAURAL_PRESET.fr)}Hz
              </Text>
              <Text style={styles.text}>
                Pan Mode: {BINAURAL_PRESET.panOsc} | Period:{" "}
                {BINAURAL_PRESET.panOscPeriod}s
              </Text>
            </View>
          </>
        )}
      </View>
    </Container>
  );
}

const styles = StyleSheet.create({
  title: { color: "#FFF", fontSize: 24, fontWeight: "bold", marginBottom: 30 },
  section: { width: "100%", marginBottom: 30 },
  subtitle: { color: "#FFF", fontSize: 18, fontWeight: "600", marginBottom: 15 },
  box: {
    width: "100%",
    marginTop: 20,
    padding: 20,
    backgroundColor: "rgba(255,255,255,0.1)",
    borderRadius: 10,
  },
  label: { color: "#FFF", fontSize: 16, fontWeight: "600", marginBottom: 10 },
  bar: {
    width: "100%",
    height: 30,
    backgroundColor: "rgba(0,0,0,0.3)",
    borderRadius: 15,
    overflow: "hidden",
  },
  fill: { height: "100%", backgroundColor: "#1EB1FC", borderRadius: 15 },
  text: { color: "#CCC", fontSize: 14, marginBottom: 5 },
});
