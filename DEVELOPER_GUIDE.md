# Audio Engine Developer Guide

## Overview

This audio engine is built on `react-native-audio-api` with custom C++ audio processing nodes for creating binaural beats, breathing-synchronized tones, symmetrical harmonic patterns, and noise generation. The system is designed to be integrated into existing React Native applications with minimal friction.

---

## Architecture

### Core Components

1. **SessionManager** (`app/audio/SessionManager.js`)
   - Manages audio sessions (start/pause/resume/stop)
   - Handles voice lifecycle and volume control
   - Provides timing and animation data
   - Maintains session state

2. **Custom Audio Nodes** (C++ native modules)
   - `MartigliNode` - Breathing-synchronized tone with modulation
   - `MartigliBinauralNode` - Breathing-synchronized binaural beats with panning
   - `BinauralNode` - Standard binaural beats with optional panning
   - `SymmetryNode` - Symmetrical harmonic patterns
   - `NoiseNode` - White/pink/brown noise generator

3. **Preset System**
   - JSON-based voice configurations
   - Duration and timing parameters
   - Default volume settings per voice type

---

## SessionManager: Core API

### Initialization

Create **one** SessionManager instance and maintain it throughout your app lifecycle:

```javascript
import { SessionManager } from "../audio/SessionManager";

const sessionManager = useRef<SessionManager | null>(null);

useEffect(() => {
  sessionManager.current = new SessionManager();
  
  return () => {
    sessionManager.current?.destroy();
  };
}, []);
```

**Important**: Use `useRef` to persist the manager across re-renders. Create it once, destroy on unmount.

### Multi-Tab/Screen Architecture

The SessionManager should live **above** your tab navigation - typically at the root screen level. Pass it down via:
- Context API
- Props drilling
- Ref forwarding

**Pattern**:
```
Root Screen (sessionManager instance)
  └─ Tab Navigator
      ├─ Session Tab (receives manager)
      ├─ Settings Tab (receives manager)
      └─ Other Tabs (receive manager)
```

This ensures the audio keeps playing when switching between tabs.

---

## Session Lifecycle

### State Machine

```
idle → playing → paused → playing → stopped → idle
       ↑_____________________________↓
```

**States**:
- `idle` - No active session, can load preset
- `playing` - Audio running, timer active
- `paused` - Audio paused, timer stopped
- `stopped` - Fade-out in progress (transitions to idle after 1.5s)

### State Transitions

```javascript
// Load preset (must be in 'idle' state)
sessionManager.loadPreset(presetData);

// Start session
sessionManager.start();  // idle → playing

// Pause/Resume
sessionManager.pause();  // playing → paused
sessionManager.resume(); // paused → playing

// Stop (triggers fade-out)
sessionManager.stop();   // any → stopped → idle (after 1.5s)
```

### Reading Current State

```javascript
const state = sessionManager.getState(); // 'idle' | 'playing' | 'paused' | 'stopped'
```

---

## Timing System

### Getting Time Values

```javascript
const elapsed = sessionManager.getElapsedTime();   // seconds elapsed
const remaining = sessionManager.getRemainingTime(); // seconds remaining
const duration = sessionManager.duration;           // total duration
```

### Timer Callbacks

To receive real-time updates:

```javascript
sessionManager.current.onTimerUpdate = (elapsed, remaining, duration) => {
  // Update UI every 100ms
  setElapsedTime(elapsed);
  setRemainingTime(remaining);
};
```

**Call frequency**: ~10 times per second (every 100ms)

---

## Volume Control

### Master Volume

Affects all voices simultaneously:

```javascript
sessionManager.setMasterVolume(0.5); // 0.0 to 1.0
```

### Individual Voice Volume

Each voice has its own volume multiplied by master:

```javascript
sessionManager.setVoiceVolume(voiceIndex, 0.8);
```

**Final output** = `voiceVolume × masterVolume`

### Getting Voice Information

```javascript
const voices = sessionManager.getVoices();
// Returns: [
//   { index: 0, type: 'Martigli', volume: 0.15 },
//   { index: 1, type: 'Binaural', volume: 0.12 },
//   ...
// ]
```

---

## Breathing Pace Adjustment (+ / - Buttons)

### Understanding the System

Martigli-type voices (`Martigli` and `Martigli-Binaural`) have breathing-synchronized modulation controlled by two parameters:

- **`mp0`** - Initial breathing cycle period (seconds)
- **`mp1`** - Target breathing cycle period (seconds)
- **`currentPeriod`** - The actual current period (animated from mp0 → mp1)

The breathing cycle gradually transitions from `mp0` to `mp1` over time. Both parameters control the same cycle, so adjusting them changes breathing pace.

### Getting Current Values

```javascript
const params = sessionManager.getBreathingParams(voiceIndex);
if (params) {
  console.log(params.currentPeriod); // Current breathing cycle (seconds)
  console.log(params.targetPeriod);  // Target (always equals mp1)
  console.log(params.mp0);           // Initial period
  console.log(params.mp1);           // Target period
  console.log(params.inhaleDur);     // Inhale duration (seconds)
  console.log(params.exhaleDur);     // Exhale duration (seconds)
}
```

Returns `null` if voice is not a Martigli-type or doesn't exist.

### Adjusting Pace

```javascript
// Make breathing faster (decrease period by 15%)
sessionManager.adjustBreathingPace(voiceIndex, 'increase');

// Make breathing slower (increase period by 15%)
sessionManager.adjustBreathingPace(voiceIndex, 'decrease');
```

**Note**: "Increase pace" = decrease period (faster breathing), "decrease pace" = increase period (slower breathing). Both `mp0` and `mp1` are adjusted together to maintain consistent behavior.

### UI Integration Pattern

Display current and target cycle:

```javascript
const params = sessionManager.getBreathingParams(martigliVoiceIndex);

<Text>Current: {params?.currentPeriod?.toFixed(1)}s</Text>
<Text>Target: {params?.targetPeriod?.toFixed(1)}s</Text>

<Button onPress={() => handleAdjustPace('increase')}>+</Button>
<Button onPress={() => handleAdjustPace('decrease')}>-</Button>
```

The `currentPeriod` animates smoothly from `mp0` toward `mp1` over the session duration, so users see gradual changes in breathing pace.

---

## Animation System

### Breathing Animation Value

For syncing visual elements with breathing cycles:

```javascript
sessionManager.current.onAnimationUpdate = (value) => {
  // value: 0.0 to 1.0
  // 0.0 = exhale complete
  // 1.0 = inhale complete
  setAnimationValue(value);
};
```

**Call frequency**: ~60fps (every 16ms)

**Use cases**:
- Breathing circle animations
- Pulsing effects
- Visual breathing guides

---

## Android Build Configuration

### Critical: Gradle Task Dependencies

The custom C++ audio nodes require the `react-native-audio-api` native library to be built **before** your app's native modules compile.

**Already configured in `android/app/build.gradle`**:

```groovy
// After plugin declarations:
evaluationDependsOn(":react-native-audio-api")

// At end of file:
afterEvaluate {
    def audioApiProject = project(":react-native-audio-api")
    
    tasks.matching { it.name.startsWith("buildCMakeDebug") }.configureEach {
        dependsOn(audioApiProject.tasks.named("mergeDebugNativeLibs"))
    }
    tasks.matching { it.name.startsWith("buildCMakeRelWithDebInfo") }.configureEach {
        dependsOn(audioApiProject.tasks.named("mergeReleaseNativeLibs"))
    }
}
```

### Building

With proper configuration, standard build commands work:

```bash
# Clean build
./gradlew clean
./gradlew app:assembleDebug

# Or via Expo
npx expo run:android
```

**You should NOT need to manually build the audio library separately**. Gradle's task dependencies ensure proper build order automatically.

### Troubleshooting Build Issues

If you encounter linking errors about missing `libreact-native-audio-api.so`:

1. **Clean CMake cache**:
   ```bash
   cd android
   ./gradlew clean
   rm -rf app/.cxx
   ```

2. **Verify Gradle configuration** includes the `afterEvaluate` block shown above

3. **Check task dependencies**:
   ```bash
   ./gradlew app:tasks --all | grep buildCMake
   ```

4. **Last resort - manual library build**:
   ```bash
   ./gradlew :react-native-audio-api:assembleDebug
   ```
   Then rebuild your app. But this shouldn't be necessary if Gradle is configured correctly.

---

## Preset Format

Presets are JSON objects defining session configuration:

```javascript
{
  header: {
    d: 900,  // duration in seconds
    // ... other metadata
  },
  voices: [
    {
      type: "Martigli-Binaural",
      fl: 200,
      fr: 210,
      waveformL: 0,  // 0=sine, 1=triangle, 2=square, 3=sawtooth
      waveformR: 0,
      ma: 0.5,       // modulation amplitude
      mp0: 10,       // initial breathing period (seconds)
      mp1: 15,       // target breathing period (seconds)
      md: 20,        // modulation duration (seconds)
      inhaleDur: 4,
      exhaleDur: 6,
      panOsc: 1,     // 0=none, 1=envelope, 2=independent, 3=synced
      panPeriod: 120,
      panTrans: 20,
      iniVolume: 0.15  // optional default volume
    },
    // ... more voices
  ]
}
```

---

## Best Practices

### ✅ Do

- Create SessionManager once at app root level
- Use `useRef` to persist manager across renders
- Always call `destroy()` on unmount
- Check session state before calling start/pause/resume
- Use callbacks (`onTimerUpdate`, `onAnimationUpdate`, `onStateChange`) for reactive UI
- Keep master volume moderate (0.2-0.4 range) for comfortable listening
- Test breathing pace adjustments with real Martigli voices

### ❌ Don't

- Create multiple SessionManager instances
- Call `start()` while already playing
- Load presets while session is active
- Forget to `destroy()` on unmount (causes audio leaks)
- Set volumes above 0.5 without user warning (can be harsh)
- Manually manipulate node properties during playback (use SessionManager methods)

---

## Common Patterns

### Play/Pause Toggle

```javascript
const handlePlayPause = () => {
  if (!sessionManager.current) return;
  
  if (state === 'idle') {
    sessionManager.current.start();
  } else if (state === 'playing') {
    sessionManager.current.pause();
  } else if (state === 'paused') {
    sessionManager.current.resume();
  }
};
```

### Stop with Confirmation

```javascript
const handleStop = () => {
  if (state === 'idle') return;
  
  Alert.alert(
    'Stop Session?',
    'This will end the current session.',
    [
      { text: 'Cancel', style: 'cancel' },
      { 
        text: 'Stop', 
        onPress: () => sessionManager.current?.stop(),
        style: 'destructive'
      }
    ]
  );
};
```

### Breathing Pace UI

```javascript
const handleAdjustPace = (direction: 'increase' | 'decrease') => {
  if (!sessionManager.current) return;
  
  // Find first Martigli voice
  const voices = sessionManager.current.getVoices();
  const martigliIndex = voices.findIndex(v => 
    v.type === 'Martigli' || v.type === 'Martigli-Binaural'
  );
  
  if (martigliIndex >= 0) {
    sessionManager.current.adjustBreathingPace(martigliIndex, direction);
    
    // Update UI
    const params = sessionManager.current.getBreathingParams(martigliIndex);
    setBreathingParams(params);
  }
};
```

---

## TypeScript Support

The SessionManager is JavaScript but can be used in TypeScript with type definitions:

```typescript
import { SessionManager } from "../audio/SessionManager";

interface BreathingParams {
  mp0: number;
  mp1: number;
  inhaleDur: number;
  exhaleDur: number;
  currentPeriod: number;
  targetPeriod: number;
}

type SessionState = 'idle' | 'playing' | 'paused' | 'stopped';

const sessionManager = useRef<SessionManager | null>(null);
```

---

## Performance Considerations

- **Timer callbacks** run at ~10Hz - keep logic lightweight
- **Animation callbacks** run at ~60Hz - avoid heavy computations
- **Volume changes** are immediate - no need to debounce
- **Breathing pace adjustments** are immediate - can be called rapidly
- **Stop operation** includes 1.5s fade-out - don't expect instant silence

---

## Summary

The audio engine provides a high-level API for managing complex multi-voice audio sessions with breathing-synchronized modulation, binaural beats, and noise generation. Key principles:

1. **Single source of truth**: One SessionManager at root level
2. **State-driven**: Check state before operations
3. **Callback-based**: Use callbacks for reactive UI updates
4. **Automatic build**: Gradle handles native library dependencies
5. **Simple integration**: Designed to fit into existing React Native apps

For implementation reference, see the included `index.tsx` example which demonstrates all patterns described in this guide.
