import React, { useCallback, useEffect, useState, FC, useRef } from "react";
import { ActivityIndicator, StyleSheet, View, Text } from "react-native";
import Slider from "@react-native-community/slider";
import { Container, Button } from "../components";
import NativeOscillatorModule from "../../specs/NativeOscillatorModule";
import { MyOscillatorNode } from "./types";
import { AudioContext } from "react-native-audio-api";

const AudioFile: FC = () => {
  const [isReady, setIsReady] = useState(false);
  const [isOscPlaying, setIsOscPlaying] = useState(false);
  const [frequency, setFrequency] = useState(440);
  const [volume, setVolume] = useState(0.5);

  // Use refs to hold the audio context and node instances
  // This prevents them from being re-created on every render
  const audioContext = useRef<AudioContext | null>(null);
  const oscillatorNode = useRef<MyOscillatorNode | null>(null);

  // Effect to initialize the audio context and inject the custom processor
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

  const toggleOscillator = async () => {
    console.log("Toggling oscillator");
    if (!audioContext.current) return;
    console.log(`Toggling oscillator. Currently playing: ${isOscPlaying}`);

    if (isOscPlaying) {
      // If playing, disconnect and stop
      if (oscillatorNode.current) {
        oscillatorNode.current.disconnect();
        oscillatorNode.current = null;
      }
      await audioContext.current.suspend();
      setIsOscPlaying(false);
    } else {
      // If stopped, resume context and start playing
      await audioContext.current.resume();

      // Create the custom C++ node
      const node = new MyOscillatorNode(
        audioContext.current,
        global.createMyOscillatorNode(audioContext.current.context)
      );

      // Set its frequency and volume, then connect it to the output
      node.frequency = frequency;
      node.volume = volume;
      node.connect(audioContext.current.destination);

      oscillatorNode.current = node;
      setIsOscPlaying(true);
    }
  };

  const handleFrequencyChange = (value: number) => {
    setFrequency(value);
    if (oscillatorNode.current) {
      oscillatorNode.current.frequency = value;
    }
  };

  const handleVolumeChange = (value: number) => {
    setVolume(value);
    if (oscillatorNode.current) {
      oscillatorNode.current.volume = value;
    }
  };

  return (
    <Container centered>
      {!isReady && <ActivityIndicator color="#FFFFFF" />}
      <Button
        title={isOscPlaying ? "Stop Oscillator" : "Play Oscillator"}
        onPress={toggleOscillator}
        disabled={!isReady}
      />

      <View style={styles.controlsContainer}>
        <View style={styles.sliderContainer}>
          <Text style={styles.label}>Frequency: {frequency.toFixed(0)} Hz</Text>
          <Slider
            style={styles.slider}
            minimumValue={20}
            maximumValue={2000}
            value={frequency}
            onValueChange={handleFrequencyChange}
            minimumTrackTintColor="#1EB1FC"
            maximumTrackTintColor="#8B8B8B"
            thumbTintColor="#1EB1FC"
            disabled={!isReady}
          />
        </View>

        <View style={styles.sliderContainer}>
          <Text style={styles.label}>Volume: {(volume * 100).toFixed(0)}%</Text>
          <Slider
            style={styles.slider}
            minimumValue={0}
            maximumValue={1}
            value={volume}
            onValueChange={handleVolumeChange}
            minimumTrackTintColor="#1EB1FC"
            maximumTrackTintColor="#8B8B8B"
            thumbTintColor="#1EB1FC"
            disabled={!isReady}
          />
        </View>
      </View>
    </Container>
  );
};

const styles = StyleSheet.create({
  controlsContainer: {
    width: "100%",
    paddingHorizontal: 20,
    marginTop: 30,
  },
  sliderContainer: {
    marginBottom: 25,
  },
  label: {
    color: "#FFFFFF",
    fontSize: 16,
    fontWeight: "600",
    marginBottom: 10,
  },
  slider: {
    width: "100%",
    height: 40,
  },
});

export default AudioFile;
