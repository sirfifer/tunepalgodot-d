# Tunepal Algorithm Enhancement: Technical Implementation Plan

**Document Type:** Technical Implementation Specification
**Date:** December 2024
**Status:** Ready for Implementation
**License:** MIT (All implementations derived from MIT-licensed or public domain sources)

---

## Executive Summary

This document provides a complete technical specification for implementing three key algorithms to significantly improve Tunepal's pitch detection and tune matching capabilities:

1. **Harmonic Product Spectrum (HPS)** - Pitch detection enhancement
2. **Viterbi Decoding** - Temporal pitch smoothing
3. **Needleman-Wunsch** - Sequence alignment for tune matching

All implementations will be in **C++** for performance, integrated with Godot via GDExtension, and derived exclusively from **MIT-licensed or public domain sources** to maintain Tunepal's MIT license.

---

## Success Criteria (CRITICAL)

This section defines the **measurable success criteria** that must be met for this implementation to be considered complete. These are derived from FolkFriend benchmarks and identified Tunepal improvement opportunities.

### Primary Success Metrics

| Metric | Current Baseline | Target | Minimum Acceptable | Verification Method |
|--------|-----------------|--------|-------------------|---------------------|
| **Total Processing Time** | ~1000-2000ms | **< 300ms** | < 500ms | Automated benchmark suite |
| **Octave Error Rate** | ~30-40% (est.) | **< 10%** | < 15% | Test dataset of 50 known tunes |
| **Pitch Detection Accuracy** | ~60% (est.) | **> 85%** | > 75% | Synthetic + real audio tests |
| **Search Match Rate (Top 10)** | ~70% (est.) | **> 90%** | > 80% | Known tune test suite |
| **Real-time Frame Rate** | N/A | **> 30 fps** | > 20 fps | During-recording benchmark |

### Detailed Success Criteria by Algorithm

#### 1. Harmonic Product Spectrum (HPS)

| Criterion | Target | Measurement |
|-----------|--------|-------------|
| **Fundamental vs Harmonic Confusion** | < 5% errors | Test with 100 harmonic-rich samples |
| **Octave Detection Accuracy** | > 95% correct octave | Test with multi-octave phrases |
| **Processing Latency** | < 5ms per frame | Benchmark on target hardware |
| **Frequency Resolution** | Within ±25 cents | Synthetic tone comparison |
| **Robustness to Noise** | Works at 15dB SNR | Add white noise to test samples |

**Test Cases:**
1. Pure sine wave at 440Hz → Must return 440Hz (±5Hz)
2. Violin A4 (harmonic-rich) → Must return 440Hz, not 880Hz
3. Flute D5 → Must return ~587Hz, not harmonics
4. Whistle ornament (fast notes) → Must track pitch changes

#### 2. Viterbi Decoding

| Criterion | Target | Measurement |
|-----------|--------|-------------|
| **Spurious Note Reduction** | > 50% fewer spurious notes | Compare with/without Viterbi |
| **Octave Jump Correction** | > 80% corrected | Test with synthetic octave errors |
| **Sequence Coherence** | Musical interval distribution | Statistical analysis of output |
| **Processing Time (1000 frames)** | < 50ms | Benchmark |
| **Memory Usage** | < 10MB | Memory profiling |

**Test Cases:**
1. Clean sequence CDEFGABC → Must output unchanged
2. Noisy sequence CDxFGAxC (x=noise) → Must output CDEFGABC
3. Octave error CD'EFGA'BC (')=octave up → Must correct to CDEFGABC
4. Fast jig passage → Must maintain coherent pitch sequence

#### 3. Needleman-Wunsch Alignment

| Criterion | Target | Measurement |
|-----------|--------|-------------|
| **Exact Match Score** | 1.0 (normalized) | Identity string test |
| **Near-Match Detection** | Score > 0.8 for 90% similarity | Synthetic variations |
| **Gap Handling** | Correct insertion/deletion scoring | Gap test patterns |
| **Music-Aware Scoring** | Adjacent notes score higher | C→D vs C→F# test |
| **Search Speed (10K tunes)** | < 300ms | Full database benchmark |

**Test Cases:**
1. "CDEFG" vs "CDEFG" → Score = 1.0
2. "CDEFG" vs "CDXFG" → Score > 0.7 (one mismatch)
3. "CDEFG" vs "CDFG" → Score > 0.75 (one deletion)
4. "CDEFG" vs "FGHIJ" → Score < 0.3 (completely different)

### End-to-End Success Criteria

| Test Scenario | Success Criterion |
|---------------|-------------------|
| **Record "Cooley's Reel" (10 sec)** | Must appear in top 5 results |
| **Record "The Kesh Jig" (10 sec)** | Must appear in top 5 results |
| **Record tune in wrong octave** | Still appears in top 10 results |
| **Record tune with ornaments** | Still appears in top 10 results |
| **Record tune with mistakes** | Still appears in top 15 results |
| **Process 10,000 tune database** | Complete search in < 300ms |

### Benchmark Comparison: FolkFriend

FolkFriend publishes these performance metrics:

| Operation | FolkFriend | Tunepal Target | Notes |
|-----------|------------|----------------|-------|
| Transcription | 57ms | **< 100ms** | Pitch detection + quantization |
| Query/Search | 208ms | **< 200ms** | Database matching |
| **Total** | **265ms** | **< 300ms** | End-to-end processing |

**Our Target: Meet or exceed FolkFriend's 265ms benchmark**

### Accuracy Benchmark Dataset

A test dataset must be created with:

1. **Synthetic Audio (50 samples)**
   - Pure tones at known frequencies
   - Harmonic-rich tones (simulated instruments)
   - Multi-note sequences with known intervals

2. **Real Recordings (50 samples)**
   - 10 fiddle recordings
   - 10 flute recordings
   - 10 whistle recordings
   - 10 concertina/accordion recordings
   - 10 mixed/challenging recordings

3. **Ground Truth**
   - Each recording labeled with correct tune ID
   - Each recording transcribed to note sequence
   - Timing and pitch annotations

### Success Gate Criteria

Implementation is considered **SUCCESSFUL** when:

- [ ] All three algorithms implemented and tested
- [ ] Total processing time < 300ms (average over 100 searches)
- [ ] Octave error rate < 15% on test dataset
- [ ] Top-10 match rate > 80% on known tune test suite
- [ ] Real-time pitch detection achieves > 20 fps
- [ ] No memory leaks detected in 10-minute continuous use
- [ ] Works on desktop (primary) and has fallback for mobile

Implementation is considered **EXCELLENT** when:

- [ ] Total processing time < 200ms
- [ ] Octave error rate < 10%
- [ ] Top-5 match rate > 85%
- [ ] Real-time pitch detection achieves > 30 fps
- [ ] Works natively on iOS and Android

---

## Table of Contents

0. [Success Criteria (CRITICAL)](#success-criteria-critical)
1. [Algorithm Overview](#1-algorithm-overview)
2. [Harmonic Product Spectrum (HPS)](#2-harmonic-product-spectrum-hps)
3. [Viterbi Decoding](#3-viterbi-decoding)
4. [Needleman-Wunsch Alignment](#4-needleman-wunsch-alignment)
5. [C++ Architecture Design](#5-c-architecture-design)
6. [Integration with Existing Code](#6-integration-with-existing-code)
7. [Testing Strategy](#7-testing-strategy)
8. [Performance Targets](#8-performance-targets)
9. [Implementation Phases](#9-implementation-phases)
10. [MIT License Source Documentation](#10-mit-license-source-documentation)

---

## 1. Algorithm Overview

### 1.1 Current State Analysis

**Current Pitch Detection (record.gd:190-238):**
- Scans 100-8000 Hz in 2 Hz steps
- Uses Godot's `AudioEffectSpectrumAnalyzerInstance`
- Selects frequency with highest magnitude
- Maps to nearest fundamental frequency
- Requires 2 consecutive detections for stability

**Current Issues:**
- Picks harmonics (2nd, 3rd) instead of fundamental frequency
- No temporal smoothing between frames
- Octave errors common
- No consideration of musical context

**Current Matching (tunepal.cpp:31-103):**
- Levenshtein substring edit distance
- Fixed 400x400 matrix (limits pattern/text length)
- Multi-threaded across CPU cores
- Wildcard 'Z' support

**Current Issues:**
- Searches entire database (no pre-filtering)
- Equal penalties for all operations
- No gap-specific handling

### 1.2 Proposed Improvements

| Algorithm | Problem Solved | Expected Improvement |
|-----------|----------------|---------------------|
| **HPS** | Harmonic confusion, octave errors | 30-50% fewer octave errors |
| **Viterbi** | Frame-by-frame noise, temporal incoherence | Smoother sequences, fewer spurious notes |
| **Needleman-Wunsch** | Poor handling of insertions/deletions | Better alignment for varied performances |

### 1.3 Algorithm Synergy

```
Audio Input
     │
     ▼
┌─────────────────────────────────────────────────────┐
│  HARMONIC PRODUCT SPECTRUM (C++)                    │
│  • Analyzes spectrum data from Godot                │
│  • Generates pitch candidates with confidence       │
│  • Outputs: [frame_id, pitch_hz, confidence][]      │
└─────────────────────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────────────────────┐
│  VITERBI DECODING (C++)                             │
│  • Takes pitch candidates per frame                 │
│  • Finds globally optimal pitch sequence            │
│  • Penalizes unlikely intervals                     │
│  • Outputs: smoothed pitch sequence                 │
└─────────────────────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────────────────────┐
│  Note Quantization (GDScript - existing)            │
│  • Converts pitches to note names                   │
│  • Groups by duration                               │
│  • Outputs: note string "DDEBBABB..."               │
└─────────────────────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────────────────────┐
│  NEEDLEMAN-WUNSCH (C++)                             │
│  • Aligns note string with database entries         │
│  • Proper gap and mismatch handling                 │
│  • Normalized similarity score [0, 1]               │
│  • Outputs: ranked matches                          │
└─────────────────────────────────────────────────────┘
```

---

## 2. Harmonic Product Spectrum (HPS)

### 2.1 Algorithm Description

The Harmonic Product Spectrum is a pitch detection algorithm that exploits the harmonic structure of musical tones. When a note is played, it produces energy not just at the fundamental frequency (f₀) but also at integer multiples (2f₀, 3f₀, 4f₀, etc.).

**Key Insight:** If we downsample the spectrum by factors of 2, 3, 4... and multiply them together, the peaks at harmonic frequencies will align at the fundamental frequency, producing a strong peak.

**MIT-Licensed Reference:** Algorithm is public domain (Noll, 1969). Implementation guidance from Wikipedia.

### 2.2 Mathematical Formulation

Given a magnitude spectrum |X(f)|, the HPS is computed as:

```
HPS(f) = ∏[h=1 to H] |X(h·f)|

Where:
  f = candidate fundamental frequency
  H = number of harmonics to consider (typically 4-5)
  |X(f)| = magnitude at frequency f
```

The fundamental frequency is:
```
f₀ = argmax_f HPS(f)
```

### 2.3 C++ Implementation Specification

#### 2.3.1 Header File Addition (tunepal.h)

```cpp
// Add to tunepal.h

#include <vector>

// Pitch candidate structure
struct PitchCandidate {
    float frequency;    // Hz
    float confidence;   // 0.0 to 1.0
    int midi_note;      // MIDI note number (0-127)
};

// HPS configuration
struct HPSConfig {
    int num_harmonics = 5;          // Number of harmonics to analyze
    float min_frequency = 80.0f;    // Minimum detectable frequency (Hz)
    float max_frequency = 2000.0f;  // Maximum detectable frequency (Hz)
    float frequency_resolution = 1.0f;  // Hz per bin
    int max_candidates = 5;         // Top N candidates to return
};

class Tunepal : public Node2D {
    // ... existing code ...

public:
    // HPS pitch detection
    godot::Array detect_pitch_hps(
        const godot::PackedFloat32Array& magnitude_spectrum,
        float sample_rate,
        int fft_size
    );

    // Configure HPS parameters
    void set_hps_config(int num_harmonics, float min_freq, float max_freq);

private:
    HPSConfig hps_config_;

    // Internal HPS computation
    std::vector<PitchCandidate> compute_hps(
        const float* spectrum,
        int spectrum_size,
        float bin_frequency_resolution
    );

    // Frequency to MIDI note conversion
    int frequency_to_midi(float frequency);
    float midi_to_frequency(int midi_note);
};
```

#### 2.3.2 Implementation (tunepal.cpp additions)

```cpp
// Harmonic Product Spectrum implementation
// Source: Wikipedia "Pitch detection algorithm" (Public Domain)
// Reference: A.M. Noll, "Pitch determination of human speech by the
//            harmonic product spectrum" (1969)
// NOT derived from FolkFriend (GPL-3.0)

#include <algorithm>
#include <cmath>

// MIDI note conversion constants
constexpr float MIDI_A4_FREQUENCY = 440.0f;
constexpr int MIDI_A4_NOTE = 69;

int Tunepal::frequency_to_midi(float frequency) {
    if (frequency <= 0) return -1;
    return static_cast<int>(std::round(
        12.0f * std::log2(frequency / MIDI_A4_FREQUENCY) + MIDI_A4_NOTE
    ));
}

float Tunepal::midi_to_frequency(int midi_note) {
    return MIDI_A4_FREQUENCY * std::pow(2.0f, (midi_note - MIDI_A4_NOTE) / 12.0f);
}

std::vector<PitchCandidate> Tunepal::compute_hps(
    const float* spectrum,
    int spectrum_size,
    float bin_frequency_resolution
) {
    std::vector<PitchCandidate> candidates;

    // Calculate HPS for each frequency bin
    int min_bin = static_cast<int>(hps_config_.min_frequency / bin_frequency_resolution);
    int max_bin = static_cast<int>(hps_config_.max_frequency / bin_frequency_resolution);

    // Ensure we don't exceed spectrum bounds with harmonic lookups
    max_bin = std::min(max_bin, spectrum_size / hps_config_.num_harmonics);

    std::vector<float> hps_values(max_bin - min_bin + 1);
    float max_hps = 0.0f;

    for (int bin = min_bin; bin <= max_bin; bin++) {
        float product = 1.0f;
        bool valid = true;

        // Multiply magnitudes at each harmonic
        for (int h = 1; h <= hps_config_.num_harmonics; h++) {
            int harmonic_bin = bin * h;
            if (harmonic_bin >= spectrum_size) {
                valid = false;
                break;
            }
            // Use magnitude (ensure positive)
            float mag = std::abs(spectrum[harmonic_bin]);
            product *= (mag + 1e-10f);  // Avoid log(0)
        }

        if (valid) {
            hps_values[bin - min_bin] = product;
            max_hps = std::max(max_hps, product);
        }
    }

    // Find peaks in HPS
    // Simple peak detection: local maximum above threshold
    float threshold = max_hps * 0.1f;  // 10% of max

    for (int i = 1; i < static_cast<int>(hps_values.size()) - 1; i++) {
        float val = hps_values[i];
        if (val > threshold &&
            val > hps_values[i - 1] &&
            val > hps_values[i + 1]) {

            int bin = i + min_bin;
            float frequency = bin * bin_frequency_resolution;
            float confidence = val / max_hps;
            int midi_note = frequency_to_midi(frequency);

            candidates.push_back({frequency, confidence, midi_note});
        }
    }

    // Sort by confidence descending
    std::sort(candidates.begin(), candidates.end(),
        [](const PitchCandidate& a, const PitchCandidate& b) {
            return a.confidence > b.confidence;
        });

    // Return top N candidates
    if (candidates.size() > static_cast<size_t>(hps_config_.max_candidates)) {
        candidates.resize(hps_config_.max_candidates);
    }

    return candidates;
}

godot::Array Tunepal::detect_pitch_hps(
    const godot::PackedFloat32Array& magnitude_spectrum,
    float sample_rate,
    int fft_size
) {
    godot::Array result;

    if (magnitude_spectrum.size() == 0) {
        return result;
    }

    // Calculate frequency resolution
    float bin_resolution = sample_rate / static_cast<float>(fft_size);

    // Get raw pointer for performance
    const float* spectrum_data = magnitude_spectrum.ptr();
    int spectrum_size = magnitude_spectrum.size();

    // Compute HPS
    std::vector<PitchCandidate> candidates = compute_hps(
        spectrum_data, spectrum_size, bin_resolution
    );

    // Convert to Godot Array of Dictionaries
    for (const auto& candidate : candidates) {
        godot::Dictionary dict;
        dict["frequency"] = candidate.frequency;
        dict["confidence"] = candidate.confidence;
        dict["midi_note"] = candidate.midi_note;
        result.push_back(dict);
    }

    return result;
}

void Tunepal::set_hps_config(int num_harmonics, float min_freq, float max_freq) {
    hps_config_.num_harmonics = std::clamp(num_harmonics, 2, 8);
    hps_config_.min_frequency = std::clamp(min_freq, 20.0f, 500.0f);
    hps_config_.max_frequency = std::clamp(max_freq, 500.0f, 4000.0f);
}
```

#### 2.3.3 Method Binding

```cpp
// Add to _bind_methods()
ClassDB::bind_method(D_METHOD("detect_pitch_hps", "magnitude_spectrum",
    "sample_rate", "fft_size"), &Tunepal::detect_pitch_hps);
ClassDB::bind_method(D_METHOD("set_hps_config", "num_harmonics",
    "min_freq", "max_freq"), &Tunepal::set_hps_config);
```

### 2.4 Integration with GDScript

```gdscript
# In record.gd - replacement for current pitch detection

func detect_pitch_frame(spectrum: AudioEffectSpectrumAnalyzerInstance) -> Array:
    # Get magnitude spectrum from Godot
    var fft_size = 2048  # Typical FFT size
    var magnitudes = PackedFloat32Array()

    # Sample the spectrum at each bin
    var sample_rate = 44100.0  # Or get from AudioServer
    var bin_resolution = sample_rate / fft_size

    for i in range(fft_size / 2):
        var freq = i * bin_resolution
        var mag = spectrum.get_magnitude_for_frequency_range(
            freq, freq + bin_resolution
        )
        magnitudes.append(mag.length())

    # Use HPS for pitch detection
    if tunepal != null:
        return tunepal.detect_pitch_hps(magnitudes, sample_rate, fft_size)
    else:
        # Fallback to original method on mobile
        return fallback_pitch_detection(spectrum)
```

---

## 3. Viterbi Decoding

### 3.1 Algorithm Description

The Viterbi algorithm finds the most likely sequence of hidden states given a sequence of observations. For pitch tracking:

- **States:** Possible MIDI pitches (e.g., 48-95 = 4 octaves)
- **Observations:** Pitch candidates from HPS at each frame
- **Transitions:** Probability of moving from one pitch to another
- **Goal:** Find the pitch sequence that maximizes overall likelihood

**MIT-Licensed Reference:** Wikipedia pseudocode (Public Domain)

### 3.2 Mathematical Formulation

For each frame t and pitch state p:

```
V[t][p] = max_q(V[t-1][q] + log(Transition(q→p)) + log(Emission(p|observation_t)))

Where:
  V[t][p] = log probability of best path ending in state p at time t
  Transition(q→p) = probability of pitch transition from q to p
  Emission(p|o) = probability of observing o given true pitch is p
```

**Transition Model (based on music theory):**
```
log(Transition(q→p)) = -λ * |p - q|

Where λ controls preference for small intervals
- λ = 0.05: mild preference (allows octave jumps)
- λ = 0.10: moderate preference
- λ = 0.20: strong preference (mostly stepwise motion)
```

### 3.3 C++ Implementation Specification

#### 3.3.1 Header Additions (tunepal.h)

```cpp
// Viterbi configuration
struct ViterbiConfig {
    int min_midi = 48;          // C2 (~131 Hz)
    int max_midi = 95;          // B6 (~1975 Hz)
    float transition_penalty = 0.08f;  // Interval penalty weight
    float silence_threshold = 0.05f;   // Minimum confidence to consider
    int min_note_frames = 3;    // Minimum frames for valid note
};

// Pitch frame data for Viterbi
struct PitchFrame {
    std::vector<PitchCandidate> candidates;  // From HPS
    float timestamp;                          // Frame time in seconds
};

class Tunepal : public Node2D {
    // ... existing code ...

public:
    // Viterbi decoding
    godot::Array viterbi_decode(const godot::Array& pitch_frames);

    // Configure Viterbi parameters
    void set_viterbi_config(int min_midi, int max_midi, float transition_penalty);

private:
    ViterbiConfig viterbi_config_;

    // Internal Viterbi computation
    std::vector<int> compute_viterbi(const std::vector<PitchFrame>& frames);
};
```

#### 3.3.2 Implementation (tunepal.cpp additions)

```cpp
// Viterbi algorithm for pitch sequence optimization
// Source: Wikipedia "Viterbi algorithm" (Public Domain)
// Reference: A.J. Viterbi, "Error bounds for convolutional codes" (1967)
// NOT derived from FolkFriend (GPL-3.0) or pYIN (GPL)

#include <limits>

std::vector<int> Tunepal::compute_viterbi(const std::vector<PitchFrame>& frames) {
    if (frames.empty()) {
        return {};
    }

    const int num_states = viterbi_config_.max_midi - viterbi_config_.min_midi + 1;
    const int num_frames = static_cast<int>(frames.size());
    const float NEG_INF = -std::numeric_limits<float>::infinity();

    // Viterbi tables: score and backpointer
    // Using vector of vectors for dynamic sizing
    std::vector<std::vector<float>> V(num_frames, std::vector<float>(num_states, NEG_INF));
    std::vector<std::vector<int>> backptr(num_frames, std::vector<int>(num_states, -1));

    // Helper: convert observation to emission score for each state
    auto get_emission_score = [&](const PitchFrame& frame, int midi_note) -> float {
        float best_score = NEG_INF;

        for (const auto& candidate : frame.candidates) {
            if (candidate.confidence < viterbi_config_.silence_threshold) {
                continue;
            }

            // Score based on how close candidate is to state
            int candidate_midi = candidate.midi_note;
            int distance = std::abs(candidate_midi - midi_note);

            if (distance == 0) {
                // Exact match: use full confidence
                best_score = std::max(best_score, std::log(candidate.confidence + 1e-10f));
            } else if (distance <= 1) {
                // Within semitone: partial credit
                float score = std::log((candidate.confidence * 0.5f) + 1e-10f);
                best_score = std::max(best_score, score);
            }
        }

        // If no good candidates, return low score (silence/noise)
        if (best_score == NEG_INF) {
            best_score = std::log(0.01f);  // Low but not impossible
        }

        return best_score;
    };

    // Initialize first frame
    for (int s = 0; s < num_states; s++) {
        int midi_note = s + viterbi_config_.min_midi;
        V[0][s] = get_emission_score(frames[0], midi_note);
    }

    // Forward pass
    for (int t = 1; t < num_frames; t++) {
        for (int s = 0; s < num_states; s++) {
            int midi_note = s + viterbi_config_.min_midi;
            float emission = get_emission_score(frames[t], midi_note);

            float best_score = NEG_INF;
            int best_prev = 0;

            for (int prev_s = 0; prev_s < num_states; prev_s++) {
                // Transition penalty based on interval size
                int interval = std::abs(s - prev_s);
                float transition = -viterbi_config_.transition_penalty * interval;

                // Optional: penalize octave jumps less (harmonics)
                if (interval == 12 || interval == 24) {
                    transition *= 0.5f;  // Octaves more likely than random jumps
                }

                float score = V[t-1][prev_s] + transition + emission;

                if (score > best_score) {
                    best_score = score;
                    best_prev = prev_s;
                }
            }

            V[t][s] = best_score;
            backptr[t][s] = best_prev;
        }
    }

    // Find best final state
    int best_final = 0;
    float best_final_score = NEG_INF;
    for (int s = 0; s < num_states; s++) {
        if (V[num_frames - 1][s] > best_final_score) {
            best_final_score = V[num_frames - 1][s];
            best_final = s;
        }
    }

    // Backtrack to find path
    std::vector<int> path(num_frames);
    path[num_frames - 1] = best_final + viterbi_config_.min_midi;

    for (int t = num_frames - 2; t >= 0; t--) {
        int prev_state = backptr[t + 1][path[t + 1] - viterbi_config_.min_midi];
        path[t] = prev_state + viterbi_config_.min_midi;
    }

    return path;
}

godot::Array Tunepal::viterbi_decode(const godot::Array& pitch_frames) {
    godot::Array result;

    // Convert Godot Array to internal format
    std::vector<PitchFrame> frames;
    frames.reserve(pitch_frames.size());

    for (int i = 0; i < pitch_frames.size(); i++) {
        godot::Dictionary frame_dict = pitch_frames[i];
        godot::Array candidates_array = frame_dict["candidates"];

        PitchFrame frame;
        frame.timestamp = frame_dict["timestamp"];

        for (int j = 0; j < candidates_array.size(); j++) {
            godot::Dictionary cand = candidates_array[j];
            PitchCandidate pc;
            pc.frequency = cand["frequency"];
            pc.confidence = cand["confidence"];
            pc.midi_note = cand["midi_note"];
            frame.candidates.push_back(pc);
        }

        frames.push_back(frame);
    }

    // Run Viterbi
    std::vector<int> path = compute_viterbi(frames);

    // Convert to Godot Array
    for (int midi : path) {
        result.push_back(midi);
    }

    return result;
}

void Tunepal::set_viterbi_config(int min_midi, int max_midi, float transition_penalty) {
    viterbi_config_.min_midi = std::clamp(min_midi, 21, 108);  // Piano range
    viterbi_config_.max_midi = std::clamp(max_midi, min_midi + 12, 108);
    viterbi_config_.transition_penalty = std::clamp(transition_penalty, 0.01f, 0.5f);
}
```

#### 3.3.3 Method Binding

```cpp
// Add to _bind_methods()
ClassDB::bind_method(D_METHOD("viterbi_decode", "pitch_frames"),
    &Tunepal::viterbi_decode);
ClassDB::bind_method(D_METHOD("set_viterbi_config", "min_midi",
    "max_midi", "transition_penalty"), &Tunepal::set_viterbi_config);
```

### 3.4 Integration with GDScript

```gdscript
# In record.gd - after collecting all HPS pitch candidates

var pitch_frames = []

func _physics_process(_delta):
    if timer.get_time_left() > 0:
        var candidates = detect_pitch_frame(spectrum)
        var frame = {
            "timestamp": 10.0 - timer.get_time_left(),
            "candidates": candidates
        }
        pitch_frames.append(frame)

func stop_recording():
    # ... existing code ...

    # Apply Viterbi smoothing
    var smoothed_midi = []
    if tunepal != null and pitch_frames.size() > 0:
        smoothed_midi = tunepal.viterbi_decode(pitch_frames)

    # Convert MIDI notes to note names
    var note_sequence = convert_midi_to_notes(smoothed_midi)

    # Continue with existing grouping and matching...
```

---

## 4. Needleman-Wunsch Alignment

### 4.1 Algorithm Description

Needleman-Wunsch is a dynamic programming algorithm for global sequence alignment. Unlike simple edit distance (Levenshtein), it uses separate scoring for:

- **Match:** Two identical elements align (+score)
- **Mismatch:** Two different elements align (-score)
- **Gap:** Element aligns with nothing (insertion/deletion)

**MIT-Licensed References:**
- Wikipedia pseudocode (Public Domain)
- https://github.com/Orion9/NeedlemanWunsch (MIT)
- https://github.com/Aqcurate/Needleman-Wunsch (MIT)

### 4.2 Mathematical Formulation

Given sequences A (length m) and B (length n), build matrix S where:

```
Initialization:
  S[0][0] = 0
  S[i][0] = i * gap_penalty    (for i = 1..m)
  S[0][j] = j * gap_penalty    (for j = 1..n)

Recurrence:
  S[i][j] = max(
      S[i-1][j-1] + score(A[i], B[j]),   // Match or mismatch
      S[i-1][j] + gap_penalty,            // Gap in B
      S[i][j-1] + gap_penalty             // Gap in A
  )

Where:
  score(a, b) = match_score    if a == b
              = mismatch_score if a != b

Final alignment score: S[m][n]
Normalized similarity: 0.5 * (S[m][n] / min(m, n)) + 0.5
```

### 4.3 Scoring Matrices for Music

For melodic comparison, we can use a music-aware scoring matrix:

```cpp
// Note similarity scoring
// Same note: +2
// Adjacent in scale (C-D, E-F, etc.): +1
// Third apart (C-E): 0
// Otherwise: -2
// Octave equivalent (C vs c): +1

int music_score(char a, char b) {
    if (a == b) return 2;  // Exact match

    // Case-insensitive comparison for octave equivalence
    char a_upper = std::toupper(a);
    char b_upper = std::toupper(b);

    if (a_upper == b_upper) return 1;  // Same note, different octave

    // Scale-based adjacency
    const char* scale = "CDEFGAB";
    int pos_a = -1, pos_b = -1;
    for (int i = 0; i < 7; i++) {
        if (scale[i] == a_upper) pos_a = i;
        if (scale[i] == b_upper) pos_b = i;
    }

    if (pos_a >= 0 && pos_b >= 0) {
        int distance = std::min(std::abs(pos_a - pos_b), 7 - std::abs(pos_a - pos_b));
        if (distance == 1) return 1;   // Adjacent (step)
        if (distance == 2) return 0;   // Third
    }

    return -2;  // Unrelated
}
```

### 4.4 C++ Implementation Specification

#### 4.4.1 Header Additions (tunepal.h)

```cpp
// Needleman-Wunsch configuration
struct NWConfig {
    float match_score = 2.0f;
    float mismatch_score = -2.0f;
    float gap_penalty = -1.0f;
    bool use_music_scoring = true;  // Use music-aware scoring matrix
};

class Tunepal : public Node2D {
    // ... existing code ...

public:
    // Needleman-Wunsch alignment
    float needleman_wunsch(const godot::String& pattern, const godot::String& text);

    // Configure NW parameters
    void set_nw_config(float match, float mismatch, float gap, bool music_scoring);

    // Batch processing for search
    godot::Array nw_search(
        const godot::String& pattern,
        const godot::Array& candidates,  // Array of {id, search_key}
        int max_results
    );

private:
    NWConfig nw_config_;

    // Internal NW computation with space optimization
    float compute_nw(const char* a, int len_a, const char* b, int len_b);

    // Music-aware scoring
    int music_score(char a, char b);
};
```

#### 4.4.2 Implementation (tunepal.cpp additions)

```cpp
// Needleman-Wunsch sequence alignment
// Sources:
//   - Wikipedia "Needleman-Wunsch algorithm" (Public Domain)
//   - https://github.com/Orion9/NeedlemanWunsch (MIT License)
// NOT derived from FolkFriend (GPL-3.0)

int Tunepal::music_score(char a, char b) {
    if (a == b) return static_cast<int>(nw_config_.match_score);

    if (!nw_config_.use_music_scoring) {
        return static_cast<int>(nw_config_.mismatch_score);
    }

    // Case-insensitive for octave equivalence
    char a_upper = std::toupper(static_cast<unsigned char>(a));
    char b_upper = std::toupper(static_cast<unsigned char>(b));

    if (a_upper == b_upper) return 1;  // Same note class

    // Handle wildcards
    if (a == 'Z' || b == 'Z') return static_cast<int>(nw_config_.match_score);

    // Scale-based distance
    static const char* scale = "CDEFGAB";
    int pos_a = -1, pos_b = -1;
    for (int i = 0; i < 7; i++) {
        if (scale[i] == a_upper) pos_a = i;
        if (scale[i] == b_upper) pos_b = i;
    }

    if (pos_a >= 0 && pos_b >= 0) {
        int distance = std::abs(pos_a - pos_b);
        distance = std::min(distance, 7 - distance);  // Circular distance

        if (distance == 1) return 1;   // Step (C-D, B-C)
        if (distance == 2) return 0;   // Third (C-E)
    }

    return static_cast<int>(nw_config_.mismatch_score);
}

float Tunepal::compute_nw(const char* a, int len_a, const char* b, int len_b) {
    if (len_a == 0 || len_b == 0) {
        return 0.0f;
    }

    // Space-optimized: only keep two rows
    std::vector<float> prev(len_b + 1);
    std::vector<float> curr(len_b + 1);

    float gap = nw_config_.gap_penalty;

    // Initialize first row
    for (int j = 0; j <= len_b; j++) {
        prev[j] = j * gap;
    }

    // Fill matrix
    for (int i = 1; i <= len_a; i++) {
        curr[0] = i * gap;

        for (int j = 1; j <= len_b; j++) {
            float match = prev[j - 1] + music_score(a[i - 1], b[j - 1]);
            float delete_gap = prev[j] + gap;
            float insert_gap = curr[j - 1] + gap;

            curr[j] = std::max({match, delete_gap, insert_gap});
        }

        std::swap(prev, curr);
    }

    return prev[len_b];
}

float Tunepal::needleman_wunsch(const godot::String& pattern, const godot::String& text) {
    if (pattern.length() == 0 || text.length() == 0) {
        return 0.0f;
    }

    // Convert Godot strings to std::string for easier handling
    godot::CharString pattern_utf8 = pattern.utf8();
    godot::CharString text_utf8 = text.utf8();

    float raw_score = compute_nw(
        pattern_utf8.get_data(), pattern_utf8.length(),
        text_utf8.get_data(), text_utf8.length()
    );

    // Normalize to [0, 1]
    // Maximum possible score = min(len_a, len_b) * match_score
    int shorter = std::min(static_cast<int>(pattern.length()),
                           static_cast<int>(text.length()));
    float max_score = shorter * nw_config_.match_score;

    // Shift and scale: raw can be negative
    // Map from [min_possible, max_possible] to [0, 1]
    float min_score = shorter * nw_config_.mismatch_score;  // Worst case all mismatches

    float normalized = (raw_score - min_score) / (max_score - min_score);
    return std::clamp(normalized, 0.0f, 1.0f);
}

godot::Array Tunepal::nw_search(
    const godot::String& pattern,
    const godot::Array& candidates,
    int max_results
) {
    godot::Array results;

    // Score all candidates
    std::vector<std::pair<float, int>> scores;  // (score, index)
    scores.reserve(candidates.size());

    for (int i = 0; i < candidates.size(); i++) {
        godot::Dictionary cand = candidates[i];
        godot::String search_key = cand["search_key"];

        if (search_key.length() < 20) continue;  // Skip too short

        float score = needleman_wunsch(pattern, search_key);
        scores.push_back({score, i});
    }

    // Partial sort to get top N
    int n = std::min(max_results, static_cast<int>(scores.size()));
    std::partial_sort(scores.begin(), scores.begin() + n, scores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });

    // Build result array
    for (int i = 0; i < n; i++) {
        godot::Dictionary result;
        godot::Dictionary original = candidates[scores[i].second];

        result["confidence"] = scores[i].first;
        result["id"] = original["id"];
        result["title"] = original["title"];
        result["search_key"] = original["search_key"];
        // Copy other fields as needed...

        results.push_back(result);
    }

    return results;
}

void Tunepal::set_nw_config(float match, float mismatch, float gap, bool music_scoring) {
    nw_config_.match_score = std::clamp(match, 0.0f, 10.0f);
    nw_config_.mismatch_score = std::clamp(mismatch, -10.0f, 0.0f);
    nw_config_.gap_penalty = std::clamp(gap, -10.0f, 0.0f);
    nw_config_.use_music_scoring = music_scoring;
}
```

#### 4.4.3 Method Binding

```cpp
// Add to _bind_methods()
ClassDB::bind_method(D_METHOD("needleman_wunsch", "pattern", "text"),
    &Tunepal::needleman_wunsch);
ClassDB::bind_method(D_METHOD("nw_search", "pattern", "candidates", "max_results"),
    &Tunepal::nw_search);
ClassDB::bind_method(D_METHOD("set_nw_config", "match", "mismatch", "gap",
    "music_scoring"), &Tunepal::set_nw_config);
```

### 4.5 Integration with GDScript

```gdscript
# In record.gd - replace current search function

func search_with_nw(start: int, end: int, thread_id: int) -> Array:
    var info = []

    # Build candidates array for this chunk
    var candidates = []
    for id in range(start, end):
        var tune = query_result[id]
        if tune["search_key"].length() >= 50:
            candidates.append({
                "id": tune["id"],
                "title": tune["title"],
                "search_key": tune["search_key"],
                "notation": tune["notation"],
                "midi_sequence": tune["midi_sequence"],
                "shortName": tune["shortName"],
                "tune_type": tune["tune_type"],
                "key_sig": tune["key_sig"]
            })

    # Use NW batch search
    if tunepal != null:
        var chunk_results = tunepal.nw_search(note_string, candidates, 100)
        for result in chunk_results:
            info.append(result)
    else:
        # Fallback to existing edSubstring
        for tune in candidates:
            var ed = fallback_edit_distance(note_string, tune["search_key"])
            var confidence = 1.0 - (ed / float(note_string.length()))
            tune["confidence"] = confidence
            info.append(tune)

    return info
```

---

## 5. C++ Architecture Design

### 5.1 Complete Header File (tunepal.h)

```cpp
#ifndef TUNEPAL_H
#define TUNEPAL_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <vector>

namespace godot {

// ============================================
// Data Structures
// ============================================

struct PitchCandidate {
    float frequency;
    float confidence;
    int midi_note;
};

struct PitchFrame {
    std::vector<PitchCandidate> candidates;
    float timestamp;
};

struct HPSConfig {
    int num_harmonics = 5;
    float min_frequency = 80.0f;
    float max_frequency = 2000.0f;
    float frequency_resolution = 1.0f;
    int max_candidates = 5;
};

struct ViterbiConfig {
    int min_midi = 48;
    int max_midi = 95;
    float transition_penalty = 0.08f;
    float silence_threshold = 0.05f;
    int min_note_frames = 3;
};

struct NWConfig {
    float match_score = 2.0f;
    float mismatch_score = -2.0f;
    float gap_penalty = -1.0f;
    bool use_music_scoring = true;
};

// ============================================
// Main Tunepal Class
// ============================================

class Tunepal : public Node2D {
    GDCLASS(Tunepal, Node2D)

private:
    HPSConfig hps_config_;
    ViterbiConfig viterbi_config_;
    NWConfig nw_config_;

protected:
    static void _bind_methods();

public:
    Tunepal();
    ~Tunepal();

    void _process(double delta) override;

    // ========================================
    // Legacy Methods (preserved for compatibility)
    // ========================================
    void say_hello();
    int edSubstring(const godot::String pattern, const godot::String text,
                    const int thread_id);

    // ========================================
    // Harmonic Product Spectrum
    // ========================================
    Array detect_pitch_hps(
        const PackedFloat32Array& magnitude_spectrum,
        float sample_rate,
        int fft_size
    );
    void set_hps_config(int num_harmonics, float min_freq, float max_freq);

    // ========================================
    // Viterbi Decoding
    // ========================================
    Array viterbi_decode(const Array& pitch_frames);
    void set_viterbi_config(int min_midi, int max_midi, float transition_penalty);

    // ========================================
    // Needleman-Wunsch Alignment
    // ========================================
    float needleman_wunsch(const String& pattern, const String& text);
    Array nw_search(const String& pattern, const Array& candidates, int max_results);
    void set_nw_config(float match, float mismatch, float gap, bool music_scoring);

private:
    // Internal HPS methods
    std::vector<PitchCandidate> compute_hps(
        const float* spectrum, int spectrum_size, float bin_frequency_resolution);
    int frequency_to_midi(float frequency);
    float midi_to_frequency(int midi_note);

    // Internal Viterbi methods
    std::vector<int> compute_viterbi(const std::vector<PitchFrame>& frames);

    // Internal NW methods
    float compute_nw(const char* a, int len_a, const char* b, int len_b);
    int music_score(char a, char b);
};

} // namespace godot

#endif // TUNEPAL_H
```

### 5.2 File Structure

```
src/
├── tunepal.h                  # Main header (all declarations)
├── tunepal.cpp               # Main implementation + binding
├── algorithms/
│   ├── hps.cpp               # HPS implementation
│   ├── viterbi.cpp           # Viterbi implementation
│   └── needleman_wunsch.cpp  # NW implementation
└── register_types.cpp        # GDExtension registration
```

### 5.3 Build Configuration (SConstruct modifications)

```python
# Add to SConstruct or SCsub

# Algorithm source files
algorithm_sources = [
    "src/algorithms/hps.cpp",
    "src/algorithms/viterbi.cpp",
    "src/algorithms/needleman_wunsch.cpp",
]

# Compile with optimization
env.Append(CXXFLAGS=["-O3", "-march=native"])  # For desktop
# For iOS/Android, use appropriate optimization flags
```

---

## 6. Integration with Existing Code

### 6.1 GDScript Integration Layer

Create a new script to manage the enhanced pipeline:

```gdscript
# TunepalGodot/Scripts/pitch_pipeline.gd
extends Node

var tunepal = null
var hps_enabled = true
var viterbi_enabled = true
var nw_enabled = true

func _ready():
    if ClassDB.class_exists("Tunepal"):
        tunepal = ClassDB.instantiate("Tunepal")
        configure_defaults()

func configure_defaults():
    if tunepal:
        tunepal.set_hps_config(5, 100.0, 1800.0)
        tunepal.set_viterbi_config(48, 96, 0.08)
        tunepal.set_nw_config(2.0, -2.0, -1.0, true)

func process_audio_frame(spectrum: AudioEffectSpectrumAnalyzerInstance,
                          sample_rate: float = 44100.0,
                          fft_size: int = 2048) -> Dictionary:
    var result = {"candidates": [], "timestamp": Time.get_ticks_msec() / 1000.0}

    if tunepal and hps_enabled:
        var magnitudes = extract_magnitudes(spectrum, sample_rate, fft_size)
        result["candidates"] = tunepal.detect_pitch_hps(magnitudes, sample_rate, fft_size)
    else:
        # Fallback to simple peak detection
        result["candidates"] = simple_pitch_detect(spectrum)

    return result

func smooth_pitch_sequence(frames: Array) -> Array:
    if tunepal and viterbi_enabled and frames.size() > 5:
        return tunepal.viterbi_decode(frames)
    else:
        # Fallback: just take highest confidence from each frame
        var result = []
        for frame in frames:
            if frame["candidates"].size() > 0:
                result.append(frame["candidates"][0]["midi_note"])
        return result

func search_tunes(pattern: String, candidates: Array, max_results: int = 50) -> Array:
    if tunepal and nw_enabled:
        return tunepal.nw_search(pattern, candidates, max_results)
    else:
        # Fallback to edSubstring
        return fallback_search(pattern, candidates, max_results)

# Helper functions...
func extract_magnitudes(spectrum, sample_rate, fft_size) -> PackedFloat32Array:
    var magnitudes = PackedFloat32Array()
    var bin_resolution = sample_rate / fft_size

    for i in range(fft_size / 2):
        var freq = i * bin_resolution
        var mag = spectrum.get_magnitude_for_frequency_range(freq, freq + bin_resolution)
        magnitudes.append(mag.length())

    return magnitudes
```

### 6.2 Modified record.gd

```gdscript
# Key modifications to record.gd

@onready var pitch_pipeline = preload("res://Scripts/pitch_pipeline.gd").new()
var pitch_frames = []

func _ready():
    add_child(pitch_pipeline)
    # ... existing initialization ...

func _physics_process(_delta):
    if timer.get_time_left() > 0:
        # Use new pipeline
        var frame = pitch_pipeline.process_audio_frame(spectrum)
        pitch_frames.append(frame)

        # ... existing progress bar update ...

func stop_recording():
    # Apply Viterbi smoothing to get clean MIDI sequence
    var smoothed_midi = pitch_pipeline.smooth_pitch_sequence(pitch_frames)

    # Convert MIDI to note string
    current_notes = midi_sequence_to_notes(smoothed_midi)

    # ... existing grouping code ...

    note_string = create_string(current_notes, average_time)

    # Use NW search
    confidences = []
    var all_candidates = []
    for id in range(query_result.size()):
        var tune = query_result[id]
        if tune["search_key"].length() >= 50:
            all_candidates.append(tune)

    confidences = pitch_pipeline.search_tunes(note_string, all_candidates, 100)

    # ... existing result display code ...
```

### 6.3 Backward Compatibility

The implementation preserves all existing methods:
- `edSubstring()` remains available for mobile platforms
- New methods are optional enhancements
- Fallback paths exist for all new features

---

## 7. Testing Strategy

### 7.1 Unit Tests (C++)

```cpp
// tests/test_algorithms.cpp

#include <gtest/gtest.h>
#include "tunepal.h"

// HPS Tests
TEST(HPSTest, DetectsSimpleSineWave) {
    // Create synthetic spectrum with fundamental at 440Hz
    // Verify HPS returns 440Hz with high confidence
}

TEST(HPSTest, HandlesHarmonicRichTone) {
    // Create spectrum with strong harmonics
    // Verify HPS finds fundamental, not harmonics
}

// Viterbi Tests
TEST(ViterbiTest, SmoothsNoisySequence) {
    // Create sequence with occasional noise notes
    // Verify Viterbi removes spurious notes
}

TEST(ViterbiTest, PenalizesLargeJumps) {
    // Create sequence with octave errors
    // Verify Viterbi corrects octave jumps
}

// Needleman-Wunsch Tests
TEST(NWTest, ExactMatchScoresHighest) {
    // "CDEFG" vs "CDEFG" should score 1.0
}

TEST(NWTest, HandlesGapsCorrectly) {
    // "CDEFG" vs "CDEG" should score reasonably
}

TEST(NWTest, MusicScoringWorks) {
    // "C" vs "D" should score higher than "C" vs "F#"
}
```

### 7.2 Integration Tests (GDScript)

```gdscript
# tests/test_pitch_detection.gd
extends Node

func test_hps_returns_candidates():
    var tunepal = ClassDB.instantiate("Tunepal")
    var test_spectrum = create_test_spectrum(440.0)  # A4
    var candidates = tunepal.detect_pitch_hps(test_spectrum, 44100.0, 2048)

    assert(candidates.size() > 0)
    assert(abs(candidates[0]["frequency"] - 440.0) < 10.0)

func test_viterbi_smooths_sequence():
    # Test with noisy input
    pass

func test_nw_matches_known_tune():
    # Test with actual tune from database
    pass
```

### 7.3 Benchmark Tests

```gdscript
# tests/benchmark.gd

func benchmark_search():
    var start = Time.get_ticks_msec()

    # Run 100 searches with different patterns
    for i in range(100):
        var pattern = generate_random_pattern(30)
        var results = pitch_pipeline.search_tunes(pattern, query_result, 50)

    var elapsed = Time.get_ticks_msec() - start
    print("100 searches completed in %d ms (avg: %d ms/search)" % [elapsed, elapsed/100])

func benchmark_pitch_detection():
    # Measure frame-by-frame pitch detection time
    pass
```

---

## 8. Performance Targets

### 8.1 Latency Targets

| Operation | Current (est.) | Target | Notes |
|-----------|---------------|--------|-------|
| HPS per frame | N/A | < 5ms | Real-time requirement |
| Viterbi (1000 frames) | N/A | < 50ms | Post-recording |
| NW single match | N/A | < 1ms | Per tune comparison |
| Full search (10000 tunes) | ~1000ms | < 300ms | Using pre-filtering |

### 8.2 Memory Targets

| Component | Target | Notes |
|-----------|--------|-------|
| HPS working memory | < 1MB | Per-frame allocation |
| Viterbi tables | < 10MB | 1000 frames × 48 states |
| NW matrix | < 1MB | Space-optimized (2 rows) |

### 8.3 FolkFriend Comparison Target

FolkFriend achieves:
- Transcription: 57ms
- Query: 208ms
- **Total: 265ms**

Tunepal target: **< 300ms** total processing time

---

## 9. Implementation Phases

### Phase 1: Foundation (1-2 weeks)

**Goals:**
- Set up algorithm file structure
- Implement HPS with unit tests
- Basic GDScript integration

**Deliverables:**
- [ ] `src/algorithms/hps.cpp` complete
- [ ] HPS unit tests passing
- [ ] `pitch_pipeline.gd` with HPS integration
- [ ] Benchmark: HPS < 5ms per frame

### Phase 2: Temporal Smoothing (1-2 weeks)

**Goals:**
- Implement Viterbi decoding
- Integrate with pitch detection pipeline
- Test on real recordings

**Deliverables:**
- [ ] `src/algorithms/viterbi.cpp` complete
- [ ] Viterbi unit tests passing
- [ ] End-to-end pitch detection working
- [ ] Octave error rate reduced by 30%+

### Phase 3: Search Enhancement (1-2 weeks)

**Goals:**
- Implement Needleman-Wunsch
- Replace Levenshtein in search pipeline
- Optimize batch processing

**Deliverables:**
- [ ] `src/algorithms/needleman_wunsch.cpp` complete
- [ ] NW unit tests passing
- [ ] Search using NW by default
- [ ] Search time < 300ms for 10000 tunes

### Phase 4: Integration & Polish (1 week)

**Goals:**
- Complete GDScript integration
- Performance optimization
- Documentation

**Deliverables:**
- [ ] All algorithms integrated in record.gd
- [ ] Mobile fallback paths tested
- [ ] Performance targets met
- [ ] User documentation updated

### Phase 5: Validation (1 week)

**Goals:**
- User testing with real musicians
- Compare accuracy with baseline
- Fine-tune parameters

**Deliverables:**
- [ ] Test dataset with known tunes
- [ ] Accuracy metrics documented
- [ ] Parameter tuning complete
- [ ] Ready for release

---

## 10. MIT License Source Documentation

### 10.1 Algorithm Sources

| Algorithm | Primary Source | License | URL |
|-----------|---------------|---------|-----|
| **HPS** | A.M. Noll (1969) | Public Domain (academic) | Original paper |
| **HPS** | Wikipedia | Public Domain | https://en.wikipedia.org/wiki/Pitch_detection_algorithm |
| **Viterbi** | A.J. Viterbi (1967) | Public Domain (academic) | Original paper |
| **Viterbi** | Wikipedia | Public Domain | https://en.wikipedia.org/wiki/Viterbi_algorithm |
| **NW** | Needleman & Wunsch (1970) | Public Domain (academic) | Original paper |
| **NW** | Wikipedia | Public Domain | https://en.wikipedia.org/wiki/Needleman-Wunsch_algorithm |
| **NW** | Orion9/NeedlemanWunsch | MIT | https://github.com/Orion9/NeedlemanWunsch |

### 10.2 Code Attribution Requirements

Each implementation file must include:

```cpp
/**
 * [Algorithm Name] Implementation
 *
 * Sources:
 *   - [Primary academic reference]
 *   - Wikipedia: [URL] (Public Domain)
 *   - [Any MIT reference]: [URL] (MIT License)
 *
 * This implementation is NOT derived from FolkFriend (GPL-3.0).
 *
 * License: MIT (compatible with Tunepal project license)
 */
```

### 10.3 Excluded Sources

The following sources are **GPL-licensed and must NOT be used**:

| Source | License | Reason to Avoid |
|--------|---------|-----------------|
| FolkFriend | GPL-3.0 | Copyleft incompatible with MIT |
| pYIN | GPL | Copyleft incompatible with MIT |
| Aubio | GPL-3.0 | Copyleft incompatible with MIT |
| Essentia | AGPL-3.0 | Copyleft incompatible with MIT |

---

## Appendix A: Quick Reference

### A.1 Key Constants

```cpp
// HPS
constexpr int DEFAULT_NUM_HARMONICS = 5;
constexpr float DEFAULT_MIN_FREQ = 80.0f;   // ~E2
constexpr float DEFAULT_MAX_FREQ = 2000.0f; // ~B6

// Viterbi
constexpr int DEFAULT_MIN_MIDI = 48;   // C3
constexpr int DEFAULT_MAX_MIDI = 95;   // B6
constexpr float DEFAULT_TRANSITION_PENALTY = 0.08f;

// Needleman-Wunsch
constexpr float DEFAULT_MATCH = 2.0f;
constexpr float DEFAULT_MISMATCH = -2.0f;
constexpr float DEFAULT_GAP = -1.0f;
```

### A.2 MIDI Note Reference

```
MIDI 48 = C3  (~130.81 Hz)
MIDI 60 = C4  (~261.63 Hz) - Middle C
MIDI 69 = A4  (~440.00 Hz) - Concert A
MIDI 72 = C5  (~523.25 Hz)
MIDI 84 = C6  (~1046.50 Hz)
MIDI 95 = B6  (~1975.53 Hz)
```

### A.3 API Quick Reference

```cpp
// HPS
Array detect_pitch_hps(PackedFloat32Array spectrum, float sample_rate, int fft_size);
void set_hps_config(int harmonics, float min_freq, float max_freq);

// Viterbi
Array viterbi_decode(Array pitch_frames);
void set_viterbi_config(int min_midi, int max_midi, float penalty);

// Needleman-Wunsch
float needleman_wunsch(String pattern, String text);
Array nw_search(String pattern, Array candidates, int max_results);
void set_nw_config(float match, float mismatch, float gap, bool music_scoring);
```

---

## Document History

| Date | Version | Author | Changes |
|------|---------|--------|---------|
| 2024-12 | 1.0 | Claude | Initial technical specification |

---

*This document provides complete specifications for clean-room implementation of pitch detection and matching algorithms, maintaining MIT license compatibility.*
