# Tuning the Experimental Detection System

**Document Type:** Parameter Guide
**Date:** 2025-12-09
**Status:** Active Development

---

## Table of Contents

1. [Overview](#1-overview)
2. [YIN Parameters](#2-yin-parameters)
3. [Frequency Range](#3-frequency-range)
4. [Buffer Size](#4-buffer-size)
5. [DTW Parameters](#5-dtw-parameters)
6. [Using the Debug UI](#6-using-the-debug-ui)
7. [Recommended Presets](#7-recommended-presets)

---

## 1. Overview

The experimental detection system has several tunable parameters that affect accuracy, speed, and noise rejection. This guide explains each parameter and how to adjust them for optimal performance.

### Quick Reference

| Parameter | Default | Range | Effect |
|-----------|---------|-------|--------|
| YIN Threshold | 0.15 | 0.05-0.30 | Detection strictness |
| Min Frequency | 80 Hz | 50-200 Hz | Lowest detectable pitch |
| Max Frequency | 2000 Hz | 500-4000 Hz | Highest detectable pitch |
| Buffer Size | 2048 | 512-4096 | Accuracy vs latency |
| Sample Rate | 44100 Hz | Fixed | Audio sample rate |

---

## 2. YIN Parameters

### YIN Threshold

The threshold determines how strict the pitch detection is.

```gdscript
detector.set_yin_threshold(0.15)  # Default
var current = detector.get_yin_threshold()
```

**How It Works:**

The YIN algorithm computes a "cumulative mean normalized difference function" (CMNDF). A pitch is detected when this function falls below the threshold.

```
CMNDF Value:
1.0 ─────────────────────────
        ╲
         ╲
          ╲
           ╲─────╮
                 │ ← Detection when below threshold
───────────────────────────── 0.15 (threshold)
          │     │
          │     └─ Period detected here
          └─ No detection here
0.0 ─────────────────────────
```

**Effects:**

| Threshold | Detection | False Positives | Missed Notes |
|-----------|-----------|-----------------|--------------|
| **0.05-0.10** | Very strict | Very few | More missed |
| **0.10-0.15** | Balanced | Low | Occasional |
| **0.15-0.20** | Moderate | Some | Few |
| **0.20-0.30** | Lenient | More | Very few |

**Recommendations:**

| Scenario | Threshold | Reason |
|----------|-----------|--------|
| Quiet recording environment | 0.10-0.15 | Can afford strictness |
| Noisy environment | 0.08-0.12 | Reject noise |
| Weak signal (distant mic) | 0.18-0.22 | Catch faint pitches |
| Professional recording | 0.12-0.15 | Balanced |

---

## 3. Frequency Range

### Min Frequency

Sets the lowest detectable pitch.

```gdscript
detector.set_pitch_config(44100.0, 2048, 80.0, 2000.0)
#                                        ^^^^
#                                        min_freq
```

**Reference Frequencies:**

| Note | Frequency | Instrument |
|------|-----------|------------|
| E2 | 82 Hz | Low guitar string |
| G2 | 98 Hz | Low violin string |
| C3 | 131 Hz | Viola lowest |
| G3 | 196 Hz | Violin lowest |

**Effects:**
- Lower values allow deeper pitches but may pick up rumble
- Higher values reject low-frequency noise but miss bass notes

### Max Frequency

Sets the highest detectable pitch.

```gdscript
detector.set_pitch_config(44100.0, 2048, 80.0, 2000.0)
#                                              ^^^^^^
#                                              max_freq
```

**Reference Frequencies:**

| Note | Frequency | Instrument |
|------|-----------|------------|
| C6 | 1047 Hz | High whistle |
| E6 | 1319 Hz | High fiddle |
| A6 | 1760 Hz | Very high |
| C7 | 2093 Hz | Extreme |

**Effects:**
- Higher values detect more notes but process more frequencies
- Lower values are faster but may miss high ornaments

---

## 4. Buffer Size

The number of audio samples analyzed per frame.

```gdscript
detector.set_pitch_config(44100.0, 2048, 80.0, 2000.0)
#                                  ^^^^
#                                  buffer_size
```

**Trade-offs:**

| Buffer Size | Samples | Time @ 44.1kHz | Accuracy | Latency |
|-------------|---------|----------------|----------|---------|
| 512 | Short | ~12 ms | Lower | Very low |
| 1024 | Medium | ~23 ms | Good | Low |
| **2048** | Standard | ~46 ms | **High** | **Medium** |
| 4096 | Long | ~93 ms | Very high | High |

**Mathematical Constraint:**

To detect a frequency F, you need at least 2 periods in the buffer:

```
minimum_buffer_size = 2 * sample_rate / min_frequency

Example for 80 Hz at 44100 Hz:
  2 * 44100 / 80 = 1102.5 samples
  → Need at least 1024 (or better, 2048)
```

**Recommendations:**

| Use Case | Buffer Size | Notes |
|----------|-------------|-------|
| Real-time display | 1024 | Lower latency |
| Recording analysis | 2048 | Best balance |
| Offline processing | 4096 | Maximum accuracy |
| Low-frequency instruments | 4096 | Needed for bass |

---

## 5. DTW Parameters

### Window Size (Sakoe-Chiba Band)

Controls how much timing variation is allowed during matching.

The DTW window limits how far the alignment can deviate from the diagonal:

```
Pattern:  C D E F G
          │ │ │ │ │
Text:     C C D E E F G G
          │   │   │   │
          └───┴───┴───┘
             Window allows flexible matching
```

**Current Implementation:**
- Window size: 50 (hardcoded)
- Future: Expose via `set_dtw_window()`

**Effects:**
- Larger window: More timing flexibility, slower
- Smaller window: Stricter timing, faster

---

## 6. Using the Debug UI

The debug overlay provides real-time feedback for tuning parameters.

### Enabling Debug Mode

1. Go to **Preferences** menu
2. Enable **Debug Mode** toggle
3. Return to **Record** screen
4. Debug panel appears as floating overlay

### Debug Panel Features

```
┌─────────────────────────────────────┐
│ Detection Debug                  [X]│
├─────────────────────────────────────┤
│ Status: Recording                   │  ← Current state
│                                     │
│ Pitch: 440.0 Hz (A4)               │  ← Detected pitch
│ Confidence: ████████░░ 0.82        │  ← How certain
│ MIDI: 69                            │  ← MIDI note number
│                                     │
│ YIN Threshold     [====|====] 0.15 │  ← Adjust in real-time
│ Min Frequency     [====|====] 80   │
│ Max Frequency     [====|====] 2000 │
│                                     │
│ Notes: C D E F G A                  │  ← Recent detections
└─────────────────────────────────────┘
```

### Tuning Workflow

1. **Start with defaults:**
   ```
   YIN Threshold: 0.15
   Min Frequency: 80 Hz
   Max Frequency: 2000 Hz
   ```

2. **Play a known note** (e.g., A4 = 440 Hz)

3. **Observe:**
   - Is the pitch detected correctly?
   - What's the confidence level?
   - Are there false detections?

4. **Adjust:**
   - Too many false detections → Lower threshold (0.10-0.12)
   - Missing notes → Raise threshold (0.18-0.20)
   - Wrong octave → Check frequency range

5. **Test with actual tunes:**
   - Record a familiar tune
   - Check if notes match expected
   - Adjust as needed

---

## 7. Recommended Presets

### Preset: Irish Traditional (Default)

```gdscript
detector.set_pitch_config(44100.0, 2048, 80.0, 2000.0)
detector.set_yin_threshold(0.15)
```

- Covers: Tin whistle, fiddle, flute, pipes
- Frequency range: D4 (294 Hz) to D7 (2349 Hz)
- Good for most traditional Irish instruments

### Preset: Fiddle/Violin

```gdscript
detector.set_pitch_config(44100.0, 2048, 196.0, 2500.0)
detector.set_yin_threshold(0.12)
```

- Range: G3 (196 Hz) to B6 (1976 Hz)
- Stricter threshold for cleaner violin tone

### Preset: Low Whistle / Flute

```gdscript
detector.set_pitch_config(44100.0, 2048, 200.0, 1500.0)
detector.set_yin_threshold(0.18)
```

- Range: Low D (294 Hz) to high range
- More lenient for breathy instruments

### Preset: Uilleann Pipes

```gdscript
detector.set_pitch_config(44100.0, 4096, 100.0, 1200.0)
detector.set_yin_threshold(0.20)
```

- Larger buffer for drones
- Wide frequency range for regulators
- More lenient for complex harmonics

### Preset: Noisy Environment

```gdscript
detector.set_pitch_config(44100.0, 2048, 150.0, 1800.0)
detector.set_yin_threshold(0.08)
```

- Stricter threshold rejects noise
- Narrower range focuses on melody
- May miss some notes

---

## Diagnostic Tips

### Problem: Octave Errors

**Symptom:** Detected note is one octave off (e.g., A4 instead of A3)

**Causes:**
1. Strong harmonics in signal
2. Min frequency too high

**Solutions:**
- Lower min_frequency to include fundamental
- YIN should handle this better than spectrum analysis

### Problem: Unstable Detection

**Symptom:** Pitch jumps around rapidly

**Causes:**
1. Buffer too small
2. Threshold too high
3. Noisy signal

**Solutions:**
- Increase buffer size (2048 → 4096)
- Lower threshold (0.15 → 0.10)
- Improve microphone placement

### Problem: Missed Notes

**Symptom:** Some notes not detected

**Causes:**
1. Threshold too strict
2. Notes outside frequency range
3. Quiet signal

**Solutions:**
- Raise threshold (0.15 → 0.20)
- Expand frequency range
- Check input level

### Problem: False Detections

**Symptom:** Detection during silence or noise

**Causes:**
1. Threshold too lenient
2. Background noise

**Solutions:**
- Lower threshold (0.15 → 0.10)
- Add amplitude gate in calling code:
  ```gdscript
  if get_rms_amplitude(buffer) > 0.01:
      var pitch = detector.detect_pitch_pyin(buffer)
  ```

---

## Related Documentation

- [README.md](README.md) - Overview and quick start
- [ARCHITECTURE.md](ARCHITECTURE.md) - Technical design
- [API_REFERENCE.md](API_REFERENCE.md) - API documentation
- [BUILDING.md](BUILDING.md) - Build instructions
