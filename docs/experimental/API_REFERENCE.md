# Experimental Detection API Reference

**Document Type:** API Documentation
**Date:** 2025-12-09
**Status:** Active Development

---

## Table of Contents

1. [Class Overview](#1-class-overview)
2. [Pitch Detection Methods](#2-pitch-detection-methods)
3. [Sequence Matching Methods](#3-sequence-matching-methods)
4. [Configuration Methods](#4-configuration-methods)
5. [Debug/Tuning Methods](#5-debugtuning-methods)
6. [Integration Examples](#6-integration-examples)

---

## 1. Class Overview

### TunepalExperimental

The main class providing experimental detection algorithms.

```gdscript
# Check availability
if ClassDB.class_exists("TunepalExperimental"):
    var detector = ClassDB.instantiate("TunepalExperimental")
```

**Inheritance:** `Node2D`

**Availability:**
- macOS: Yes (debug and release)
- iOS Device: Planned
- iOS Simulator: Planned
- Windows: Planned
- Linux: Planned

---

## 2. Pitch Detection Methods

### detect_pitch_pyin

Detects the fundamental frequency from an audio buffer using the YIN algorithm.

```gdscript
float detect_pitch_pyin(PackedFloat32Array audio_buffer)
```

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `audio_buffer` | `PackedFloat32Array` | Audio samples, normalized to -1.0 to 1.0 |

**Returns:** `float`
- Detected frequency in Hz (e.g., 440.0 for A4)
- Returns `-1.0` if no pitch detected or buffer too small

**Example:**

```gdscript
var detector = ClassDB.instantiate("TunepalExperimental")

# Get audio samples from recording
var audio_buffer = PackedFloat32Array()
audio_buffer.resize(2048)
# ... fill with audio samples ...

var frequency = detector.detect_pitch_pyin(audio_buffer)

if frequency > 0:
    print("Detected frequency: ", frequency, " Hz")
    print("Note: ", frequency_to_note(frequency))
else:
    print("No pitch detected")
```

**Notes:**
- Minimum buffer size: 64 samples
- Recommended buffer size: 2048 samples at 44100 Hz
- Call `get_last_confidence()` after to check detection quality

---

### detect_pitch_sequence

Detects a sequence of pitches from longer audio data with specified hop size.

```gdscript
Array detect_pitch_sequence(PackedFloat32Array audio_data, int hop_size)
```

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `audio_data` | `PackedFloat32Array` | Full audio recording |
| `hop_size` | `int` | Samples to advance between frames (e.g., 512) |

**Returns:** `Array` of `Dictionary`

Each dictionary contains:
| Key | Type | Description |
|-----|------|-------------|
| `"frequency"` | `float` | Detected frequency in Hz |
| `"confidence"` | `float` | Detection confidence (0.0-1.0) |
| `"midi"` | `int` | MIDI note number (69 = A4) |

**Example:**

```gdscript
var detector = ClassDB.instantiate("TunepalExperimental")
detector.set_pitch_config(44100.0, 2048, 80.0, 2000.0)

# Process 5 seconds of audio
var audio_data = get_recorded_audio()  # PackedFloat32Array
var hop_size = 512  # ~11.6ms at 44100Hz

var pitches = detector.detect_pitch_sequence(audio_data, hop_size)

for frame in pitches:
    if frame["frequency"] > 0 and frame["confidence"] > 0.5:
        print("Frame: ", frame["frequency"], " Hz, confidence: ", frame["confidence"])
```

---

### detect_pitch_ensemble

Detects pitch using an ensemble of algorithms (future feature).

```gdscript
float detect_pitch_ensemble(PackedFloat32Array audio_buffer)
```

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `audio_buffer` | `PackedFloat32Array` | Audio samples |

**Returns:** `float` - Detected frequency or -1.0

**Note:** Currently uses YIN only. Will combine MPM, YIN, and HPS in future versions.

---

## 3. Sequence Matching Methods

### dtw_distance

Computes the raw Dynamic Time Warping distance between two numeric sequences.

```gdscript
float dtw_distance(PackedFloat32Array seq1, PackedFloat32Array seq2)
```

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `seq1` | `PackedFloat32Array` | First sequence (e.g., recorded pitches) |
| `seq2` | `PackedFloat32Array` | Second sequence (e.g., database tune) |

**Returns:** `float`
- Raw DTW distance (lower = more similar)
- Returns `INF` if either sequence is empty

**Example:**

```gdscript
var detector = ClassDB.instantiate("TunepalExperimental")

# MIDI note sequences
var recorded = PackedFloat32Array([60, 62, 64, 65, 67])  # C D E F G
var database = PackedFloat32Array([60, 62, 64, 65, 67, 69])  # C D E F G A

var distance = detector.dtw_distance(recorded, database)
print("DTW distance: ", distance)
```

---

### dtw_similarity

Computes a normalized similarity score using DTW with subsequence matching.

```gdscript
float dtw_similarity(String pattern, String text)
```

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `pattern` | `String` | Short note pattern (e.g., "CDEFG") |
| `text` | `String` | Longer note sequence to search in |

**Returns:** `float`
- Similarity score from 0.0 (different) to 1.0 (identical)
- Handles timing variations (repeated notes)

**Note String Format:**
- Use letters A-G for notes
- Case insensitive
- Non-note characters are ignored

**Example:**

```gdscript
var detector = ClassDB.instantiate("TunepalExperimental")

# Recorded melody
var recorded = "CDEFGAB"

# Database tune (might have timing differences)
var tune = "CCDEEFGGABB"

var similarity = detector.dtw_similarity(recorded, tune)
print("Similarity: ", similarity)  # e.g., 0.85

if similarity > 0.7:
    print("Good match!")
```

---

### dtw_search

Searches an array of candidates and returns the best matches.

```gdscript
Array dtw_search(String pattern, Array candidates, int max_results)
```

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `pattern` | `String` | Note pattern to search for |
| `candidates` | `Array` of `String` | Database of tune note sequences |
| `max_results` | `int` | Maximum number of results to return |

**Returns:** `Array` of `Dictionary`

Each dictionary contains:
| Key | Type | Description |
|-----|------|-------------|
| `"index"` | `int` | Index in candidates array |
| `"similarity"` | `float` | Match score (0.0-1.0) |

**Example:**

```gdscript
var detector = ClassDB.instantiate("TunepalExperimental")

var recorded = "ADBGGABG"

# Get search_key from all tunes in database
var candidates = []
for tune in query_result:
    candidates.append(tune["search_key"])

var results = detector.dtw_search(recorded, candidates, 50)

for result in results:
    var tune = query_result[result["index"]]
    print(tune["title"], ": ", result["similarity"])
```

---

### needleman_wunsch

Computes edit distance similar to Bryan's original algorithm (for comparison).

```gdscript
float needleman_wunsch(String pattern, String text)
```

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `pattern` | `String` | Short pattern |
| `text` | `String` | Longer text to search |

**Returns:** `float` - Edit distance (number of operations needed)

**Note:** This is a simple implementation for comparison purposes. Use Bryan's original `edSubstring()` for production.

---

## 4. Configuration Methods

### set_pitch_config

Configures the pitch detection parameters.

```gdscript
void set_pitch_config(float sample_rate, int buffer_size, float min_freq, float max_freq)
```

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `sample_rate` | `float` | 44100.0 | Audio sample rate in Hz |
| `buffer_size` | `int` | 2048 | Analysis window size in samples |
| `min_freq` | `float` | 80.0 | Minimum detectable frequency (Hz) |
| `max_freq` | `float` | 2000.0 | Maximum detectable frequency (Hz) |

**Example:**

```gdscript
var detector = ClassDB.instantiate("TunepalExperimental")

# Configure for violin range
detector.set_pitch_config(
    44100.0,  # Sample rate
    2048,     # Buffer size
    196.0,    # G3 (lowest violin note)
    3520.0    # A7 (very high)
)
```

---

### set_pitch_algorithm

Sets the pitch detection algorithm (for future use).

```gdscript
void set_pitch_algorithm(int algorithm)
```

**Parameters:**

| Value | Algorithm | Description |
|-------|-----------|-------------|
| 0 | PYIN | Probabilistic YIN (default) |
| 1 | MPM | McLeod Pitch Method (planned) |
| 2 | YIN | Standard YIN |
| 3 | ENSEMBLE | Combined voting (planned) |

---

## 5. Debug/Tuning Methods

### get_last_confidence

Returns the confidence level of the most recent pitch detection.

```gdscript
float get_last_confidence()
```

**Returns:** `float`
- Confidence from 0.0 (uncertain) to 1.0 (very confident)
- Based on the CMNDF minimum value

**Example:**

```gdscript
var detector = ClassDB.instantiate("TunepalExperimental")

var frequency = detector.detect_pitch_pyin(audio_buffer)
var confidence = detector.get_last_confidence()

if confidence > 0.8:
    print("High confidence: ", frequency, " Hz")
elif confidence > 0.5:
    print("Medium confidence: ", frequency, " Hz")
else:
    print("Low confidence, might be noise")
```

---

### set_yin_threshold

Sets the YIN algorithm detection threshold.

```gdscript
void set_yin_threshold(float threshold)
```

**Parameters:**

| Name | Type | Range | Description |
|------|------|-------|-------------|
| `threshold` | `float` | 0.05-0.30 | Detection sensitivity |

**Effect:**
- **Lower (0.05-0.10):** Stricter detection, fewer false positives
- **Higher (0.20-0.30):** More lenient, may detect more weak signals

**Default:** 0.15

---

### get_yin_threshold

Gets the current YIN detection threshold.

```gdscript
float get_yin_threshold()
```

**Returns:** `float` - Current threshold value

---

### say_hello

Prints a hello message (for testing library loading).

```gdscript
void say_hello()
```

---

### get_version

Returns the library version string.

```gdscript
String get_version()
```

**Returns:** `String` - Version (e.g., "0.1.0-experimental")

---

## 6. Integration Examples

### Example 1: Basic Integration with record.gd

```gdscript
# In record.gd or a new detection script

var tunepal_exp = null

func _ready():
    if ClassDB.class_exists("TunepalExperimental"):
        tunepal_exp = ClassDB.instantiate("TunepalExperimental")
        tunepal_exp.set_pitch_config(44100.0, 2048, 80.0, 2000.0)
        print("Experimental detection available: ", tunepal_exp.get_version())

func detect_with_experimental(audio_samples: PackedFloat32Array) -> Dictionary:
    if tunepal_exp == null:
        return {"error": "Experimental detector not available"}

    var frequency = tunepal_exp.detect_pitch_pyin(audio_samples)
    var confidence = tunepal_exp.get_last_confidence()

    return {
        "frequency": frequency,
        "confidence": confidence,
        "midi": frequency_to_midi(frequency) if frequency > 0 else -1
    }

func frequency_to_midi(freq: float) -> int:
    if freq <= 0:
        return -1
    return int(round(69.0 + 12.0 * log(freq / 440.0) / log(2.0)))
```

### Example 2: A/B Testing Both Systems

```gdscript
func compare_detection(audio_samples: PackedFloat32Array):
    # Original system (spectrum-based)
    var original_freq = detect_with_spectrum(audio_samples)

    # Experimental system (YIN-based)
    var exp_result = detect_with_experimental(audio_samples)

    print("Original: ", original_freq, " Hz")
    print("Experimental: ", exp_result["frequency"], " Hz (confidence: ", exp_result["confidence"], ")")

    # Check for octave errors
    if original_freq > 0 and exp_result["frequency"] > 0:
        var ratio = original_freq / exp_result["frequency"]
        if abs(ratio - 2.0) < 0.1 or abs(ratio - 0.5) < 0.1:
            print("WARNING: Possible octave error between systems")
```

### Example 3: Full Search Integration

```gdscript
func search_experimental(note_string: String) -> Array:
    if tunepal_exp == null:
        return []

    # Get all search keys from database
    var candidates = []
    for tune in query_result:
        candidates.append(tune["search_key"])

    # Use DTW search
    var results = tunepal_exp.dtw_search(note_string, candidates, 50)

    # Build result array with full tune info
    var full_results = []
    for result in results:
        var tune = query_result[result["index"]]
        full_results.append({
            "title": tune["title"],
            "similarity": result["similarity"],
            "notation": tune["notation"],
            "tune_type": tune["tune_type"]
        })

    return full_results
```

---

## Related Documentation

- [README.md](README.md) - Overview and quick start
- [ARCHITECTURE.md](ARCHITECTURE.md) - Technical design
- [BUILDING.md](BUILDING.md) - Build instructions
- [TUNING_GUIDE.md](TUNING_GUIDE.md) - Parameter tuning
