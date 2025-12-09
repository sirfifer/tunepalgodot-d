# Experimental Detection System Architecture

**Document Type:** Technical Documentation
**Date:** 2025-12-09
**Status:** Active Development

---

## Table of Contents

1. [System Overview](#1-system-overview)
2. [Directory Structure](#2-directory-structure)
3. [YIN Pitch Detection](#3-yin-pitch-detection)
4. [DTW Sequence Matching](#4-dtw-sequence-matching)
5. [GDExtension Integration](#5-gdextension-integration)
6. [Comparison with Original System](#6-comparison-with-original-system)
7. [License Compliance](#7-license-compliance)

---

## 1. System Overview

The experimental detection system is a **parallel implementation** that coexists with Bryan's original Tunepal detection engine. Both systems can be loaded simultaneously, enabling A/B testing and gradual migration.

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                        Godot Engine                                  │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │                   GDScript Layer                             │    │
│  │                                                              │    │
│  │  record.gd                                                   │    │
│  │  ├── _physics_process() → Spectrum analysis (original)       │    │
│  │  ├── stop_recording() → tunepal.edSubstring() (original)     │    │
│  │  └── [future] → experimental_detector.detect() (new)         │    │
│  │                                                              │    │
│  │  detector_factory.gd (planned)                               │    │
│  │  ├── create("legacy") → LegacyDetector                       │    │
│  │  └── create("experimental") → ExperimentalDetector           │    │
│  └─────────────────────────────────────────────────────────────┘    │
│                              │                                       │
│              ┌───────────────┴───────────────┐                      │
│              ▼                               ▼                      │
│  ┌─────────────────────────┐    ┌─────────────────────────────┐    │
│  │   tunepal.gdextension   │    │ tunepal_experimental        │    │
│  │   (Original Library)    │    │    .gdextension             │    │
│  │                         │    │   (New Library)             │    │
│  │  class Tunepal          │    │  class TunepalExperimental  │    │
│  │  ├── edSubstring()      │    │  ├── detect_pitch_pyin()    │    │
│  │  └── say_hello()        │    │  ├── dtw_similarity()       │    │
│  │                         │    │  └── get_last_confidence()  │    │
│  └─────────────────────────┘    └─────────────────────────────┘    │
│              │                               │                      │
│              ▼                               ▼                      │
│  ┌─────────────────────────┐    ┌─────────────────────────────┐    │
│  │      src/               │    │   src_experimental/         │    │
│  │  ├── tunepal.cpp        │    │  ├── tunepal_experimental   │    │
│  │  ├── tunepal.h          │    │  │      .cpp/.h             │    │
│  │  └── register_types.cpp │    │  ├── algorithms/            │    │
│  │                         │    │  │   ├── yin_detector.h     │    │
│  │  (DO NOT MODIFY)        │    │  │   └── dtw_matcher.h      │    │
│  │                         │    │  └── register_types.cpp     │    │
│  └─────────────────────────┘    └─────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────────┘
```

### Design Principles

1. **Zero Risk to Original Code** - The experimental system is completely separate
2. **Simultaneous Loading** - Both libraries load without conflict
3. **Easy Rollback** - Delete `src_experimental/` and `addons/tunepal_experimental/` to remove
4. **MIT License Only** - No GPL dependencies

---

## 2. Directory Structure

```
tunepalgodot-d/
├── src/                                    # ORIGINAL - Bryan's code (DO NOT MODIFY)
│   ├── tunepal.cpp                         # Edit distance algorithm
│   ├── tunepal.h                           # Original class definition
│   └── register_types.cpp                  # GDExtension registration
│
├── src_experimental/                       # NEW - Experimental algorithms
│   ├── tunepal_experimental.cpp            # Main implementation
│   ├── tunepal_experimental.h              # Class definition
│   ├── register_types.cpp                  # GDExtension registration
│   ├── register_types.h
│   │
│   ├── algorithms/                         # Algorithm implementations
│   │   ├── yin_detector.h                  # YIN pitch detection (header-only)
│   │   └── dtw_matcher.h                   # DTW matching (header-only)
│   │
│   └── third_party/                        # External references
│       └── pitch-detection/                # sevagh/pitch-detection (MIT, reference)
│
├── TunepalGodot/
│   └── addons/
│       ├── tunepal/bin/                    # Original library binaries
│       │   ├── tunepal.gdextension
│       │   └── libtunepal.*.framework/
│       │
│       └── tunepal_experimental/bin/       # Experimental library binaries
│           ├── tunepal_experimental.gdextension
│           └── libtunepal_experimental.*.framework/
│
└── SConstruct                              # Build script (handles both libraries)
```

---

## 3. YIN Pitch Detection

### Algorithm Overview

YIN (named after Yin-Yang) is a fundamental frequency estimation algorithm published by Alain de Cheveigne and Hideki Kawahara in 2002. It's known for high accuracy and relatively low computational cost.

### Algorithm Steps

```
┌─────────────────────────────────────────────────────────────┐
│                    YIN Algorithm Flow                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. Difference Function                                     │
│     ┌─────────────────────────────────────────────────┐    │
│     │ For each lag τ (tau):                           │    │
│     │   d(τ) = Σ (x[i] - x[i+τ])²                     │    │
│     │                                                 │    │
│     │ Measures how similar signal is to shifted self  │    │
│     └─────────────────────────────────────────────────┘    │
│                         │                                   │
│                         ▼                                   │
│  2. Cumulative Mean Normalized Difference (CMNDF)           │
│     ┌─────────────────────────────────────────────────┐    │
│     │ d'(τ) = d(τ) * τ / Σ d(j) for j=1 to τ          │    │
│     │                                                 │    │
│     │ Normalizes to handle amplitude variations       │    │
│     │ d'(0) = 1 by definition                        │    │
│     └─────────────────────────────────────────────────┘    │
│                         │                                   │
│                         ▼                                   │
│  3. Absolute Threshold                                      │
│     ┌─────────────────────────────────────────────────┐    │
│     │ Find first τ where d'(τ) < threshold            │    │
│     │                                                 │    │
│     │ Default threshold: 0.15                         │    │
│     │ Lower = stricter (fewer false positives)        │    │
│     │ Higher = more lenient (more detections)         │    │
│     └─────────────────────────────────────────────────┘    │
│                         │                                   │
│                         ▼                                   │
│  4. Parabolic Interpolation                                 │
│     ┌─────────────────────────────────────────────────┐    │
│     │ Refine τ estimate using neighbors               │    │
│     │ τ_refined = τ + (d'[τ+1] - d'[τ-1]) /          │    │
│     │                 (2 * (2*d'[τ] - d'[τ+1] - d'[τ-1]))│    │
│     │                                                 │    │
│     │ Provides sub-sample accuracy                    │    │
│     └─────────────────────────────────────────────────┘    │
│                         │                                   │
│                         ▼                                   │
│  5. Convert to Frequency                                    │
│     ┌─────────────────────────────────────────────────┐    │
│     │ frequency = sample_rate / τ_refined             │    │
│     │                                                 │    │
│     │ confidence = 1.0 - d'(τ)                        │    │
│     └─────────────────────────────────────────────────┘    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Implementation (yin_detector.h)

```cpp
// Key parameters
float threshold = 0.15f;      // Detection sensitivity
float sample_rate = 44100.0f; // Audio sample rate
float min_frequency = 80.0f;  // ~E2 (low limit)
float max_frequency = 2000.0f;// ~B6 (high limit)

// Returns
struct YinResult {
    float frequency;     // Hz, or -1 if no pitch
    float confidence;    // 0.0 to 1.0
    int period_samples;  // Detected period in samples
};
```

### Why YIN vs Spectrum Analysis?

| Aspect | Godot Spectrum Analyzer | YIN Algorithm |
|--------|------------------------|---------------|
| **Octave Errors** | Common (picks harmonics) | Rare (finds fundamental) |
| **Noise Handling** | Sensitive | More robust |
| **Computational Cost** | Low | Medium |
| **Real-time** | Yes | Yes (with proper buffer) |
| **Polyphonic** | Partial | Monophonic only |

---

## 4. DTW Sequence Matching

### Algorithm Overview

Dynamic Time Warping (DTW) finds the optimal alignment between two sequences that may vary in speed or timing. Unlike edit distance, DTW handles timing variations naturally.

### Algorithm Steps

```
┌─────────────────────────────────────────────────────────────┐
│                    DTW Algorithm Flow                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Input: Two note sequences                                  │
│  Pattern: [C, D, E, F, G]                                   │
│  Text:    [C, C, D, E, E, F, G, G]                          │
│                                                             │
│  1. Build Cost Matrix                                       │
│     ┌─────────────────────────────────────────────────┐    │
│     │          C   C   D   E   E   F   G   G          │    │
│     │     ┌───────────────────────────────────────┐   │    │
│     │   C │ 0   0   2   4   4   5   7   7         │   │    │
│     │   D │ 2   2   0   2   2   3   5   5         │   │    │
│     │   E │ 4   4   2   0   0   1   3   3         │   │    │
│     │   F │ 5   5   3   1   1   0   2   2         │   │    │
│     │   G │ 7   7   5   3   3   2   0   0         │   │    │
│     │     └───────────────────────────────────────┘   │    │
│     │                                                 │    │
│     │ cost(i,j) = |pattern[i] - text[j]| +           │    │
│     │             min(cost(i-1,j-1),                 │    │
│     │                 cost(i-1,j),                   │    │
│     │                 cost(i,j-1))                   │    │
│     └─────────────────────────────────────────────────┘    │
│                         │                                   │
│                         ▼                                   │
│  2. Find Optimal Path                                       │
│     ┌─────────────────────────────────────────────────┐    │
│     │ Trace back from bottom-right corner             │    │
│     │ Distance = final cell value                     │    │
│     └─────────────────────────────────────────────────┘    │
│                         │                                   │
│                         ▼                                   │
│  3. Normalize & Convert to Similarity                       │
│     ┌─────────────────────────────────────────────────┐    │
│     │ normalized = distance / (len1 + len2)           │    │
│     │ similarity = exp(-normalized / 2.0)             │    │
│     │                                                 │    │
│     │ Result: 0.0 (different) to 1.0 (identical)      │    │
│     └─────────────────────────────────────────────────┘    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Subsequence DTW

For melody search, we use **subsequence DTW** which finds the best match of a short pattern within a longer sequence:

```cpp
// Regular DTW: Aligns entire sequences
float dtw_distance(seq1, seq2);

// Subsequence DTW: Finds pattern within text
// - Can start matching anywhere in text
// - Can end matching anywhere in text
float subsequence_match(pattern, text);
```

### Why DTW vs Edit Distance?

| Aspect | Edit Distance | DTW |
|--------|--------------|-----|
| **Timing Variations** | Penalizes | Handles naturally |
| **Note Duration** | Ignored | Can incorporate |
| **Transposition** | Fixed penalty | Proportional distance |
| **Computational Cost** | O(n*m) | O(n*m) with window |
| **Existing Code** | Bryan's implementation | New implementation |

---

## 5. GDExtension Integration

### Registration Flow

```cpp
// src_experimental/register_types.cpp

extern "C" {
GDExtensionBool GDE_EXPORT tunepal_experimental_library_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    const GDExtensionClassLibraryPtr p_library,
    GDExtensionInitialization *r_initialization
) {
    // Initialize godot-cpp bindings
    GDExtensionBinding::InitObject init_obj(...);

    // Register our module initializer
    init_obj.register_initializer(initialize_tunepal_experimental_module);
    init_obj.register_terminator(uninitialize_tunepal_experimental_module);

    return init_obj.init();
}
}

void initialize_tunepal_experimental_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) return;

    // Register TunepalExperimental class with Godot
    ClassDB::register_class<TunepalExperimental>();
}
```

### GDExtension Configuration

```ini
; tunepal_experimental.gdextension

[configuration]
entry_symbol = "tunepal_experimental_library_init"
compatibility_minimum = "4.2"

[libraries]
macos.debug = "res://addons/tunepal_experimental/bin/libtunepal_experimental.macos.template_debug.framework"
ios.debug.simulator.arm64 = "res://addons/tunepal_experimental/bin/libtunepal_experimental.ios.template_debug.arm64.simulator.dylib"
```

### Method Binding

```cpp
void TunepalExperimental::_bind_methods() {
    // Pitch detection
    ClassDB::bind_method(D_METHOD("detect_pitch_pyin", "audio_buffer"),
                         &TunepalExperimental::detect_pitch_pyin);

    // DTW matching
    ClassDB::bind_method(D_METHOD("dtw_similarity", "pattern", "text"),
                         &TunepalExperimental::dtw_similarity);

    // Debug/tuning
    ClassDB::bind_method(D_METHOD("set_yin_threshold", "threshold"),
                         &TunepalExperimental::set_yin_threshold);
    ClassDB::bind_method(D_METHOD("get_last_confidence"),
                         &TunepalExperimental::get_last_confidence);
}
```

---

## 6. Comparison with Original System

### Data Flow Comparison

**Original System:**
```
Microphone → AudioEffectCapture → Godot Spectrum Analyzer
    → Find peak frequency → Map to note → Build note string
    → edSubstring() → Edit distance → Rank results
```

**Experimental System:**
```
Microphone → AudioEffectCapture → Raw PCM samples
    → YIN Algorithm → Frequency + confidence → Map to note
    → Build note string → DTW → Similarity score → Rank results
```

### Key Differences

| Component | Original | Experimental |
|-----------|----------|--------------|
| **Pitch Source** | FFT magnitude peaks | Autocorrelation-based YIN |
| **Octave Handling** | Manual harmonic filtering | Built into YIN |
| **Matching Algorithm** | Edit distance (400x400 matrix) | DTW with windowing |
| **Timing Tolerance** | Fixed (note count) | Flexible (time warping) |
| **Confidence Metric** | Magnitude threshold | CMNDF-based confidence |

---

## 7. License Compliance

### MIT-Licensed Components

All experimental code is either:
1. Clean-room implementation based on published algorithms
2. Derived from MIT-licensed sources

| Component | Source | License | Notes |
|-----------|--------|---------|-------|
| `yin_detector.h` | Original implementation | MIT | Based on YIN paper (2002) |
| `dtw_matcher.h` | Original implementation | MIT | Standard DTW algorithm |
| Reference code | sevagh/pitch-detection | MIT | Not directly used, for reference |

### NOT Used (GPL/Restrictive)

- FolkFriend pYIN implementation (GPL)
- Aubio library (GPL)
- Essentia library (AGPL)
- Any code from TunepalLibrary (original license unclear)

### Verification

```bash
# Check for GPL references
grep -r "GPL" src_experimental/
# Should return nothing

# Verify MIT headers
head -20 src_experimental/algorithms/*.h
# Should show MIT license comments
```

---

## Related Documentation

- [README.md](README.md) - Overview and quick start
- [API_REFERENCE.md](API_REFERENCE.md) - Complete API documentation
- [BUILDING.md](BUILDING.md) - Build instructions
- [TUNING_GUIDE.md](TUNING_GUIDE.md) - Parameter tuning
