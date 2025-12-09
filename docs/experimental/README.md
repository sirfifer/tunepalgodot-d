# Experimental Detection System

**Document Type:** Overview & Quick Start
**Date:** 2025-12-09
**Status:** Active Development

---

## What Is This?

The **Experimental Detection System** is a parallel implementation of tune recognition algorithms that runs alongside Bryan Duggan's original Tunepal detection engine. It enables A/B testing of new detection approaches without modifying the proven original code.

### Why Two Systems?

| System | Pitch Detection | Sequence Matching | Status |
|--------|-----------------|-------------------|--------|
| **Original (Bryan's)** | Godot Spectrum Analyzer | Edit Distance (Needleman-Wunsch variant) | Production |
| **Experimental** | YIN Algorithm | Dynamic Time Warping (DTW) | Testing |

The experimental system aims to:
- Reduce octave errors in pitch detection
- Improve matching accuracy for melodic variations
- Handle timing variations better with DTW

**Important:** The original system remains the default. The experimental system is opt-in and requires Bryan's approval before any production use.

---

## Quick Start

### 1. Check if Experimental Library is Available

```gdscript
if ClassDB.class_exists("TunepalExperimental"):
    print("Experimental detection available!")
    var detector = ClassDB.instantiate("TunepalExperimental")
    detector.say_hello()  # Prints version info
```

### 2. Basic Pitch Detection

```gdscript
var detector = ClassDB.instantiate("TunepalExperimental")

# Configure for your audio setup
detector.set_pitch_config(44100.0, 2048, 80.0, 2000.0)

# Detect pitch from audio buffer
var audio_buffer = get_audio_samples()  # PackedFloat32Array
var frequency = detector.detect_pitch_pyin(audio_buffer)

if frequency > 0:
    print("Detected: ", frequency, " Hz")
    print("Confidence: ", detector.get_last_confidence())
```

### 3. DTW Melody Matching

```gdscript
var detector = ClassDB.instantiate("TunepalExperimental")

# Compare note sequences
var recorded = "CDEFGAB"
var database_tune = "CDEFGABCDEFG"

var similarity = detector.dtw_similarity(recorded, database_tune)
print("Similarity: ", similarity)  # 0.0 to 1.0
```

---

## Build Instructions

The experimental library is built separately from the original Tunepal library.

### macOS (Development)

```bash
cd /path/to/tunepalgodot-d
BUILD_EXPERIMENTAL=1 scons platform=macos target=template_debug experimental
```

### iOS Simulator

```bash
BUILD_EXPERIMENTAL=1 scons platform=ios arch=arm64 ios_simulator=yes target=template_debug experimental
```

### Verify Build

Check that the library was created:
```bash
ls TunepalGodot/addons/tunepal_experimental/bin/
# Should show: libtunepal_experimental.macos.template_debug.framework/
```

For detailed build instructions, see [BUILDING.md](BUILDING.md).

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────┐
│                     GDScript Layer                          │
│  record.gd → DetectorFactory → detector_interface.gd        │
└─────────────────────┬───────────────────┬───────────────────┘
                      │                   │
         ┌────────────▼────────┐ ┌────────▼────────────┐
         │  Original Tunepal   │ │ TunepalExperimental │
         │  (Bryan's Code)     │ │ (New Algorithms)    │
         │                     │ │                     │
         │ • Spectrum Analyzer │ │ • YIN Pitch Detect  │
         │ • Edit Distance     │ │ • DTW Matching      │
         │ • 400x400 Matrix    │ │ • Ensemble (future) │
         └─────────────────────┘ └─────────────────────┘
                │                         │
         src/tunepal.cpp          src_experimental/
```

For detailed architecture, see [ARCHITECTURE.md](ARCHITECTURE.md).

---

## API Reference

### Pitch Detection

| Method | Description |
|--------|-------------|
| `detect_pitch_pyin(buffer)` | Detect pitch using YIN algorithm |
| `detect_pitch_sequence(data, hop)` | Detect pitch sequence with hop size |
| `get_last_confidence()` | Get confidence of last detection (0.0-1.0) |

### Sequence Matching

| Method | Description |
|--------|-------------|
| `dtw_distance(seq1, seq2)` | Raw DTW distance between sequences |
| `dtw_similarity(pattern, text)` | Normalized similarity score (0.0-1.0) |
| `dtw_search(pattern, candidates, max)` | Search candidates, return top matches |

### Configuration

| Method | Description |
|--------|-------------|
| `set_pitch_config(rate, buffer, min_f, max_f)` | Configure pitch detection |
| `set_yin_threshold(threshold)` | Set YIN detection threshold |
| `get_yin_threshold()` | Get current threshold |

For complete API documentation, see [API_REFERENCE.md](API_REFERENCE.md).

---

## Tuning Parameters

The experimental detector has several adjustable parameters:

| Parameter | Default | Range | Effect |
|-----------|---------|-------|--------|
| YIN Threshold | 0.15 | 0.05-0.30 | Lower = stricter detection |
| Min Frequency | 80 Hz | 50-200 Hz | Lowest detectable pitch |
| Max Frequency | 2000 Hz | 500-4000 Hz | Highest detectable pitch |
| Buffer Size | 2048 | 512-4096 | Larger = more accurate, slower |

For tuning guidance, see [TUNING_GUIDE.md](TUNING_GUIDE.md).

---

## Debug UI

A debug overlay panel is available for real-time observation and tuning:

1. Enable "Debug Mode" in Preferences
2. Open the Record screen
3. The debug panel shows:
   - Current pitch detection (Hz, note name)
   - Confidence level
   - Adjustable parameter sliders
   - Recent detected notes

---

## License Compliance

The experimental system uses **MIT-licensed** algorithms only:

| Component | Source | License |
|-----------|--------|---------|
| YIN Algorithm | Clean-room implementation | MIT |
| DTW Algorithm | Clean-room implementation | MIT |
| Reference Code | sevagh/pitch-detection | MIT |

**NOT used:** Any GPL-licensed code (FolkFriend, original Aubio pYIN, etc.)

---

## Related Documentation

- [ARCHITECTURE.md](ARCHITECTURE.md) - Technical deep-dive
- [API_REFERENCE.md](API_REFERENCE.md) - Complete API documentation
- [BUILDING.md](BUILDING.md) - Build instructions for all platforms
- [TUNING_GUIDE.md](TUNING_GUIDE.md) - Parameter adjustment guide
- [../ALGORITHMS_IMPLEMENTATION_PLAN.md](../ALGORITHMS_IMPLEMENTATION_PLAN.md) - Original design spec

---

## Contact

For questions about the experimental system, refer to the project's GitHub issues or contact the development team.
