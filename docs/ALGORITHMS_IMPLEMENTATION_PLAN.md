# Tunepal Algorithm Enhancement: Technical Implementation Plan

**Document Type:** Technical Implementation Specification
**Date:** December 2024 (Updated)
**Status:** Ready for Implementation
**License:** MIT (All implementations derived from MIT-licensed or public domain sources)

---

## Executive Summary

This document provides a complete technical specification for implementing advanced algorithms to significantly improve Tunepal's pitch detection and tune matching capabilities, with special focus on **harmonic-rich instruments** (guitar, accordion, fiddle, concertina).

### Primary Algorithm Stack (Recommended)

| Component | Algorithm | Source | Why |
|-----------|-----------|--------|-----|
| **Pitch Detection** | pYIN | sevagh/pitch-detection (MIT) | 97.5% accuracy, 0.5-1.7% octave errors |
| **Temporal Smoothing** | Integrated Viterbi | Built into pYIN | No separate implementation needed |
| **Sequence Matching** | DTW + N-gram | asardaes/dtw-cpp (MIT) | Better for music than NW |

### Alternative Stack (Maximum Robustness)

| Component | Algorithm | Why |
|-----------|-----------|-----|
| **Pitch Detection** | Ensemble (HPS + MPM + YIN) | Research shows ensemble beats neural networks |
| **Temporal Smoothing** | Median filtering | Simpler, comparable results |
| **Sequence Matching** | DTW primary, NW fallback | Handles both continuous and discrete |

All implementations are **C++** for performance, integrated with Godot via GDExtension, and derived from **MIT-licensed sources**.

---

## Success Criteria (CRITICAL)

This section defines the **measurable success criteria** that must be met for this implementation to be considered complete.

### Primary Success Metrics

| Metric | Current Baseline | Target | Minimum Acceptable | Verification Method |
|--------|-----------------|--------|-------------------|---------------------|
| **Total Processing Time** | ~1000-2000ms | **< 300ms** | < 500ms | Automated benchmark suite |
| **Octave Error Rate** | ~30-40% (est.) | **< 5%** | < 10% | Test dataset of 50 known tunes |
| **Pitch Detection Accuracy** | ~60% (est.) | **> 90%** | > 85% | Synthetic + real audio tests |
| **Search Match Rate (Top 10)** | ~70% (est.) | **> 90%** | > 80% | Known tune test suite |
| **Real-time Frame Rate** | N/A | **> 30 fps** | > 20 fps | During-recording benchmark |

**Note:** Targets updated based on pYIN's proven 97.5% accuracy and 0.5-1.7% octave error rate.

### Detailed Success Criteria by Algorithm

#### 1. Pitch Detection (pYIN / Ensemble)

| Criterion | Target | Measurement |
|-----------|--------|-------------|
| **Fundamental vs Harmonic Confusion** | < 2% errors | Test with 100 harmonic-rich samples |
| **Octave Detection Accuracy** | > 98% correct octave | Test with multi-octave phrases |
| **Processing Latency** | < 5ms per frame | Benchmark on target hardware |
| **Frequency Resolution** | Within ±25 cents | Synthetic tone comparison |
| **Robustness to Noise** | Works at 15dB SNR | Add white noise to test samples |
| **Harmonic-rich Instrument Accuracy** | > 90% | Guitar, accordion, fiddle tests |

**Test Cases:**
1. Pure sine wave at 440Hz → Must return 440Hz (±5Hz)
2. Violin A4 (harmonic-rich) → Must return 440Hz, not 880Hz
3. Guitar G3 (very harmonic-rich) → Must return ~196Hz correctly
4. Accordion chord → Must identify melody note
5. Whistle ornament (fast notes) → Must track pitch changes
6. Fiddle with vibrato → Must track center pitch

#### 2. Temporal Smoothing (pYIN-integrated / Median)

| Criterion | Target | Measurement |
|-----------|--------|-------------|
| **Spurious Note Reduction** | > 50% fewer spurious notes | Compare with/without smoothing |
| **Octave Jump Correction** | > 90% corrected | Test with synthetic octave errors |
| **Sequence Coherence** | Musical interval distribution | Statistical analysis of output |
| **Processing Time (1000 frames)** | < 50ms | Benchmark |
| **Memory Usage** | < 10MB | Memory profiling |

**Test Cases:**
1. Clean sequence CDEFGABC → Must output unchanged
2. Noisy sequence CDxFGAxC (x=noise) → Must output CDEFGABC
3. Octave error CD'EFGA'BC (')=octave up → Must correct to CDEFGABC
4. Fast jig passage → Must maintain coherent pitch sequence

#### 3. Sequence Matching (DTW Primary / NW Fallback)

| Criterion | Target | Measurement |
|-----------|--------|-------------|
| **Exact Match Score** | 1.0 (normalized) | Identity string test |
| **Near-Match Detection** | Score > 0.8 for 90% similarity | Synthetic variations |
| **Tempo Variation Handling** | Matches at ±30% tempo | DTW warping test |
| **Music-Aware Scoring** | Adjacent notes score higher | C→D vs C→F# test |
| **Search Speed (10K tunes)** | < 300ms | Full database benchmark |

**Test Cases:**
1. "CDEFG" vs "CDEFG" → Score = 1.0
2. "CDEFG" vs "CDXFG" → Score > 0.7 (one mismatch)
3. "CDEFG" vs "CDFG" → Score > 0.75 (one deletion)
4. "CDEFG" vs "CDEFG" at different tempos → Score > 0.9 (DTW handles)
5. "CDEFG" vs "FGHIJ" → Score < 0.3 (completely different)

### End-to-End Success Criteria

| Test Scenario | Success Criterion |
|---------------|-------------------|
| **Record "Cooley's Reel" (10 sec) on whistle** | Must appear in top 3 results |
| **Record "The Kesh Jig" (10 sec) on flute** | Must appear in top 3 results |
| **Record tune on guitar (harmonic-rich)** | Must appear in top 5 results |
| **Record tune on accordion** | Must appear in top 5 results |
| **Record tune in wrong octave** | Still appears in top 5 results |
| **Record tune with ornaments** | Still appears in top 5 results |
| **Record tune with mistakes** | Still appears in top 10 results |
| **Process 10,000 tune database** | Complete search in < 300ms |

### Benchmark Comparison: FolkFriend

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
   - 10 flute/whistle recordings
   - 10 guitar recordings (harmonic-rich challenge)
   - 10 concertina/accordion recordings (harmonic-rich challenge)
   - 10 mixed/challenging recordings

3. **Ground Truth**
   - Each recording labeled with correct tune ID
   - Each recording transcribed to note sequence
   - Timing and pitch annotations

### Success Gate Criteria

Implementation is considered **SUCCESSFUL** when:

- [ ] Primary algorithms implemented and tested (pYIN + DTW)
- [ ] Total processing time < 300ms (average over 100 searches)
- [ ] Octave error rate < 10% on test dataset
- [ ] Top-10 match rate > 80% on known tune test suite
- [ ] Real-time pitch detection achieves > 20 fps
- [ ] Harmonic-rich instruments (guitar/accordion) achieve > 80% accuracy
- [ ] No memory leaks detected in 10-minute continuous use
- [ ] Works on desktop (primary) and has fallback for mobile

Implementation is considered **EXCELLENT** when:

- [ ] Total processing time < 200ms
- [ ] Octave error rate < 5%
- [ ] Top-5 match rate > 85%
- [ ] Real-time pitch detection achieves > 30 fps
- [ ] Harmonic-rich instruments achieve > 90% accuracy
- [ ] Works natively on iOS and Android

---

## Table of Contents

0. [Success Criteria (CRITICAL)](#success-criteria-critical)
1. [Algorithm Overview](#1-algorithm-overview)
2. [Pitch Detection: pYIN (Primary)](#2-pitch-detection-pyin-primary)
3. [Pitch Detection: Ensemble (Alternative)](#3-pitch-detection-ensemble-alternative)
4. [Sequence Matching: DTW (Primary)](#4-sequence-matching-dtw-primary)
5. [Sequence Matching: Needleman-Wunsch (Fallback)](#5-sequence-matching-needleman-wunsch-fallback)
6. [C++ Architecture Design](#6-c-architecture-design)
7. [Integration with Existing Code](#7-integration-with-existing-code)
8. [Testing Strategy](#8-testing-strategy)
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
- **Especially problematic for guitar, accordion, fiddle**
- No temporal smoothing between frames
- Octave errors common (~30-40% estimated)
- No consideration of musical context

**Current Matching (tunepal.cpp:31-103):**
- Levenshtein substring edit distance
- Fixed 400x400 matrix (limits pattern/text length)
- Multi-threaded across CPU cores
- Wildcard 'Z' support

**Current Issues:**
- Searches entire database (no pre-filtering)
- Equal penalties for all operations
- No handling for tempo variations

### 1.2 Why Original Plan Needed Updates

Research revealed better alternatives to HPS + standalone Viterbi + NW:

| Original | Updated | Reason |
|----------|---------|--------|
| HPS | **pYIN** | HPS is "prone to noise" and struggles with harmonic-rich instruments |
| Standalone Viterbi | **Integrated in pYIN** | Combined approach is better tuned |
| Needleman-Wunsch only | **DTW primary** | DTW is standard for music, handles tempo variations |

### 1.3 Updated Algorithm Architecture

```
Audio Input
     │
     ▼
┌─────────────────────────────────────────────────────┐
│  pYIN PITCH DETECTION (C++ - sevagh/pitch-detection)│
│  • YIN-based autocorrelation (handles harmonics)    │
│  • Probabilistic multiple candidates                │
│  • Built-in Viterbi HMM smoothing                   │
│  • Proven 97.5% accuracy, 0.5-1.7% octave errors    │
│  • Outputs: smoothed MIDI pitch sequence            │
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
│  N-GRAM PRE-FILTERING (GDScript)                    │
│  • Extract quadgrams from query                     │
│  • Fast candidate filtering (10000 → ~500)          │
│  • Hash-based O(1) lookups                          │
└─────────────────────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────────────────────┐
│  DTW SEQUENCE MATCHING (C++ - asardaes/dtw-cpp)     │
│  • Dynamic Time Warping on candidates               │
│  • Handles tempo variations naturally               │
│  • Normalized similarity score [0, 1]               │
│  • Outputs: ranked matches                          │
└─────────────────────────────────────────────────────┘
```

### 1.4 Alternative: Ensemble Architecture (Maximum Robustness)

Research finding: "A simple combination of 3 estimators by median filtering outperformed all other approaches including CREPE (neural network)"

```
Audio Input
     │
     ├──────────────────┬──────────────────┐
     ▼                  ▼                  ▼
┌─────────┐      ┌─────────┐      ┌─────────┐
│   HPS   │      │   MPM   │      │   YIN   │
│ (freq)  │      │ (time)  │      │ (time)  │
└────┬────┘      └────┬────┘      └────┬────┘
     │                │                │
     └────────────────┼────────────────┘
                      ▼
              ┌─────────────┐
              │Median Filter│
              │(per frame)  │
              └──────┬──────┘
                     ▼
          ┌───────────────────┐
          │ Median Smoothing  │
          │ (temporal window) │
          └─────────┬─────────┘
                    ▼
           Note Quantization
                    ▼
          N-gram + DTW Search
```

---

## 2. Pitch Detection: pYIN (Primary)

### 2.1 Why pYIN Over HPS

| Feature | HPS | pYIN |
|---------|-----|------|
| **Algorithm Type** | Frequency domain | Time domain (autocorrelation) |
| **Harmonic Handling** | Multiplies (can amplify errors) | Pattern matching (robust) |
| **Published Accuracy** | ~70-80% | **97.5%** |
| **Octave Error Rate** | ~10-20% | **0.5-1.7%** |
| **Viterbi Integration** | Separate step needed | Built-in |
| **MIT Implementation** | Custom needed | sevagh/pitch-detection |

### 2.2 Algorithm Description

pYIN (Probabilistic YIN) is a two-stage pitch detector:

**Stage 1: YIN-based Candidate Generation**
- Autocorrelation-based fundamental frequency estimation
- Generates multiple pitch candidates with probabilities
- Robust to harmonics (key for guitar/accordion)

**Stage 2: Viterbi HMM Decoding**
- Hidden Markov Model over pitch candidates
- Finds globally optimal pitch sequence
- Penalizes unlikely pitch transitions

### 2.3 MIT-Licensed Source

**Repository:** https://github.com/sevagh/pitch-detection
**License:** MIT (confirmed)
**Language:** C++ with O(N log N) complexity

The library includes:
- `pitch::pyin<float>()` - Probabilistic YIN with Viterbi
- `pitch::yin<float>()` - Standard YIN
- `pitch::mpm<float>()` - McLeod Pitch Method

### 2.4 C++ Integration Specification

#### 2.4.1 Header File (tunepal.h)

```cpp
#ifndef TUNEPAL_H
#define TUNEPAL_H

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <vector>

// Include pitch-detection library
#include "pitch_detection.h"

namespace godot {

// Configuration for pitch detection
struct PitchConfig {
    float sample_rate = 44100.0f;
    int buffer_size = 2048;
    float min_frequency = 80.0f;   // ~E2
    float max_frequency = 2000.0f; // ~B6
    enum Algorithm { PYIN, MPM, YIN, ENSEMBLE } algorithm = PYIN;
};

class Tunepal : public Node2D {
    GDCLASS(Tunepal, Node2D)

private:
    PitchConfig pitch_config_;

    // Pitch detection allocators (reusable for performance)
    std::unique_ptr<pitch_alloc::Yin<float>> yin_alloc_;
    std::unique_ptr<pitch_alloc::Mpm<float>> mpm_alloc_;

protected:
    static void _bind_methods();

public:
    Tunepal();
    ~Tunepal();

    // ========================================
    // Legacy Methods (preserved)
    // ========================================
    void say_hello();
    int edSubstring(const godot::String pattern, const godot::String text,
                    const int thread_id);

    // ========================================
    // Pitch Detection (pYIN primary)
    // ========================================
    float detect_pitch_pyin(const PackedFloat32Array& audio_buffer);
    Array detect_pitch_pyin_sequence(const PackedFloat32Array& audio_data,
                                      int hop_size);

    // Alternative: ensemble detection
    float detect_pitch_ensemble(const PackedFloat32Array& audio_buffer);

    // Configuration
    void set_pitch_config(float sample_rate, int buffer_size,
                          float min_freq, float max_freq);
    void set_pitch_algorithm(int algorithm);  // 0=PYIN, 1=MPM, 2=YIN, 3=ENSEMBLE

    // ========================================
    // DTW Sequence Matching (primary)
    // ========================================
    float dtw_distance(const PackedFloat32Array& seq1,
                       const PackedFloat32Array& seq2);
    float dtw_similarity(const String& pattern, const String& text);
    Array dtw_search(const String& pattern, const Array& candidates,
                     int max_results);

    // ========================================
    // Needleman-Wunsch (fallback)
    // ========================================
    float needleman_wunsch(const String& pattern, const String& text);
    Array nw_search(const String& pattern, const Array& candidates,
                    int max_results);

private:
    // Internal helpers
    int frequency_to_midi(float frequency);
    float midi_to_frequency(int midi_note);
    std::vector<float> string_to_pitch_sequence(const String& note_string);
};

} // namespace godot

#endif // TUNEPAL_H
```

#### 2.4.2 Implementation (tunepal.cpp)

```cpp
/**
 * Pitch Detection Implementation using pYIN
 *
 * Sources:
 *   - sevagh/pitch-detection: https://github.com/sevagh/pitch-detection (MIT)
 *   - pYIN paper: Mauch & Dixon, "pYIN: A Fundamental Frequency Estimator" (2014)
 *
 * This implementation uses MIT-licensed code from sevagh/pitch-detection.
 * NOT derived from FolkFriend (GPL-3.0) or original pYIN (GPL).
 */

#include "tunepal.h"
#include <algorithm>
#include <cmath>
#include <numeric>

using namespace godot;

// MIDI conversion constants
constexpr float MIDI_A4_FREQUENCY = 440.0f;
constexpr int MIDI_A4_NOTE = 69;

Tunepal::Tunepal() {
    // Initialize pitch detection allocators
    yin_alloc_ = std::make_unique<pitch_alloc::Yin<float>>(
        pitch_config_.buffer_size);
    mpm_alloc_ = std::make_unique<pitch_alloc::Mpm<float>>(
        pitch_config_.buffer_size);
}

Tunepal::~Tunepal() = default;

int Tunepal::frequency_to_midi(float frequency) {
    if (frequency <= 0) return -1;
    return static_cast<int>(std::round(
        12.0f * std::log2(frequency / MIDI_A4_FREQUENCY) + MIDI_A4_NOTE
    ));
}

float Tunepal::midi_to_frequency(int midi_note) {
    return MIDI_A4_FREQUENCY * std::pow(2.0f, (midi_note - MIDI_A4_NOTE) / 12.0f);
}

float Tunepal::detect_pitch_pyin(const PackedFloat32Array& audio_buffer) {
    if (audio_buffer.size() < pitch_config_.buffer_size) {
        return -1.0f;
    }

    // Convert to std::vector for pitch-detection library
    std::vector<float> buffer(audio_buffer.ptr(),
                               audio_buffer.ptr() + pitch_config_.buffer_size);

    // Use pYIN (probabilistic YIN with Viterbi)
    float pitch = pitch::pyin<float>(buffer, pitch_config_.sample_rate);

    // Validate pitch is in expected range
    if (pitch < pitch_config_.min_frequency ||
        pitch > pitch_config_.max_frequency) {
        return -1.0f;  // Invalid/silence
    }

    return pitch;
}

float Tunepal::detect_pitch_ensemble(const PackedFloat32Array& audio_buffer) {
    if (audio_buffer.size() < pitch_config_.buffer_size) {
        return -1.0f;
    }

    std::vector<float> buffer(audio_buffer.ptr(),
                               audio_buffer.ptr() + pitch_config_.buffer_size);

    // Get pitch estimates from multiple algorithms
    float pitch_yin = pitch::yin<float>(buffer, pitch_config_.sample_rate);
    float pitch_mpm = pitch::mpm<float>(buffer, pitch_config_.sample_rate);

    // Simple HPS for third estimate
    // (Could also use pitch::pyin for a stronger ensemble)
    float pitch_pyin = pitch::pyin<float>(buffer, pitch_config_.sample_rate);

    // Collect valid pitches
    std::vector<float> pitches;
    if (pitch_yin > pitch_config_.min_frequency &&
        pitch_yin < pitch_config_.max_frequency) {
        pitches.push_back(pitch_yin);
    }
    if (pitch_mpm > pitch_config_.min_frequency &&
        pitch_mpm < pitch_config_.max_frequency) {
        pitches.push_back(pitch_mpm);
    }
    if (pitch_pyin > pitch_config_.min_frequency &&
        pitch_pyin < pitch_config_.max_frequency) {
        pitches.push_back(pitch_pyin);
    }

    if (pitches.empty()) {
        return -1.0f;
    }

    // Return median
    std::sort(pitches.begin(), pitches.end());
    return pitches[pitches.size() / 2];
}

Array Tunepal::detect_pitch_pyin_sequence(const PackedFloat32Array& audio_data,
                                           int hop_size) {
    Array result;

    if (audio_data.size() < pitch_config_.buffer_size) {
        return result;
    }

    int num_frames = (audio_data.size() - pitch_config_.buffer_size) / hop_size + 1;

    for (int i = 0; i < num_frames; i++) {
        int start = i * hop_size;

        // Extract frame
        PackedFloat32Array frame;
        frame.resize(pitch_config_.buffer_size);
        for (int j = 0; j < pitch_config_.buffer_size; j++) {
            frame[j] = audio_data[start + j];
        }

        float pitch;
        if (pitch_config_.algorithm == PitchConfig::ENSEMBLE) {
            pitch = detect_pitch_ensemble(frame);
        } else {
            pitch = detect_pitch_pyin(frame);
        }

        Dictionary frame_result;
        frame_result["frame"] = i;
        frame_result["time"] = static_cast<float>(start) / pitch_config_.sample_rate;
        frame_result["frequency"] = pitch;
        frame_result["midi_note"] = (pitch > 0) ? frequency_to_midi(pitch) : -1;

        result.push_back(frame_result);
    }

    return result;
}

void Tunepal::set_pitch_config(float sample_rate, int buffer_size,
                                float min_freq, float max_freq) {
    pitch_config_.sample_rate = std::clamp(sample_rate, 8000.0f, 96000.0f);
    pitch_config_.buffer_size = std::clamp(buffer_size, 512, 8192);
    pitch_config_.min_frequency = std::clamp(min_freq, 20.0f, 500.0f);
    pitch_config_.max_frequency = std::clamp(max_freq, 200.0f, 4000.0f);

    // Reinitialize allocators with new buffer size
    yin_alloc_ = std::make_unique<pitch_alloc::Yin<float>>(
        pitch_config_.buffer_size);
    mpm_alloc_ = std::make_unique<pitch_alloc::Mpm<float>>(
        pitch_config_.buffer_size);
}

void Tunepal::set_pitch_algorithm(int algorithm) {
    pitch_config_.algorithm = static_cast<PitchConfig::Algorithm>(
        std::clamp(algorithm, 0, 3));
}
```

#### 2.4.3 Method Binding

```cpp
void Tunepal::_bind_methods() {
    // Legacy methods
    ClassDB::bind_method(D_METHOD("say_hello"), &Tunepal::say_hello);
    ClassDB::bind_method(D_METHOD("edSubstring", "pattern", "text", "thread_id"),
                         &Tunepal::edSubstring);

    // Pitch detection
    ClassDB::bind_method(D_METHOD("detect_pitch_pyin", "audio_buffer"),
                         &Tunepal::detect_pitch_pyin);
    ClassDB::bind_method(D_METHOD("detect_pitch_pyin_sequence", "audio_data", "hop_size"),
                         &Tunepal::detect_pitch_pyin_sequence);
    ClassDB::bind_method(D_METHOD("detect_pitch_ensemble", "audio_buffer"),
                         &Tunepal::detect_pitch_ensemble);
    ClassDB::bind_method(D_METHOD("set_pitch_config", "sample_rate", "buffer_size",
                                   "min_freq", "max_freq"),
                         &Tunepal::set_pitch_config);
    ClassDB::bind_method(D_METHOD("set_pitch_algorithm", "algorithm"),
                         &Tunepal::set_pitch_algorithm);

    // DTW matching
    ClassDB::bind_method(D_METHOD("dtw_distance", "seq1", "seq2"),
                         &Tunepal::dtw_distance);
    ClassDB::bind_method(D_METHOD("dtw_similarity", "pattern", "text"),
                         &Tunepal::dtw_similarity);
    ClassDB::bind_method(D_METHOD("dtw_search", "pattern", "candidates", "max_results"),
                         &Tunepal::dtw_search);

    // NW matching (fallback)
    ClassDB::bind_method(D_METHOD("needleman_wunsch", "pattern", "text"),
                         &Tunepal::needleman_wunsch);
    ClassDB::bind_method(D_METHOD("nw_search", "pattern", "candidates", "max_results"),
                         &Tunepal::nw_search);
}
```

---

## 3. Pitch Detection: Ensemble (Alternative)

### 3.1 When to Use Ensemble

Use the ensemble approach when:
- pYIN alone isn't meeting accuracy targets
- Maximum robustness is required
- Testing on particularly challenging instruments

### 3.2 Research Backing

From "Traditional Machine Learning for Pitch Detection" (arXiv:1903.01290):
> "A simple combination of the 3 estimators by median filtering outperformed all other approaches (including CREPE)"

### 3.3 Ensemble Components

| Algorithm | Type | Strength | MIT Source |
|-----------|------|----------|------------|
| **HPS** | Frequency domain | Simple, fast | Custom (public domain) |
| **MPM** | Time domain | Fast, good for instruments | sevagh/pitch-detection |
| **YIN** | Time domain | Accurate | sevagh/pitch-detection |

### 3.4 HPS Implementation (for Ensemble)

```cpp
/**
 * Harmonic Product Spectrum for ensemble pitch detection
 * Source: Wikipedia (Public Domain), Noll 1969 paper
 */
float Tunepal::compute_hps(const float* spectrum, int spectrum_size,
                            float bin_resolution, int num_harmonics) {
    float max_hps = 0.0f;
    float best_freq = 0.0f;

    int min_bin = static_cast<int>(pitch_config_.min_frequency / bin_resolution);
    int max_bin = std::min(
        static_cast<int>(pitch_config_.max_frequency / bin_resolution),
        spectrum_size / num_harmonics
    );

    for (int bin = min_bin; bin <= max_bin; bin++) {
        float product = 1.0f;

        for (int h = 1; h <= num_harmonics; h++) {
            int harmonic_bin = bin * h;
            if (harmonic_bin < spectrum_size) {
                product *= (spectrum[harmonic_bin] + 1e-10f);
            }
        }

        if (product > max_hps) {
            max_hps = product;
            best_freq = bin * bin_resolution;
        }
    }

    return best_freq;
}
```

---

## 4. Sequence Matching: DTW (Primary)

### 4.1 Why DTW Over Needleman-Wunsch

| Feature | Needleman-Wunsch | DTW |
|---------|-----------------|-----|
| **Designed for** | DNA/protein sequences | Time series, audio, music |
| **Data type** | Discrete symbols only | Continuous values supported |
| **Tempo variations** | Poor (fixed gaps) | Excellent (warps time) |
| **Music adoption** | Rare | Standard in MIR |
| **Gaps** | Inserts gaps (extends sequence) | Collapses/stretches |

### 4.2 Algorithm Description

Dynamic Time Warping finds the optimal alignment between two sequences by warping the time axis:

```
Given sequences A = [a₁, a₂, ..., aₘ] and B = [b₁, b₂, ..., bₙ]

DTW Distance Matrix D:
  D[0][0] = 0
  D[i][0] = ∞ for i > 0
  D[0][j] = ∞ for j > 0

  D[i][j] = distance(aᵢ, bⱼ) + min(D[i-1][j], D[i][j-1], D[i-1][j-1])

Final distance: D[m][n]
```

### 4.3 MIT-Licensed Source

**Repository:** https://github.com/asardaes/dtw-cpp
**License:** MIT
**Language:** C++11

### 4.4 C++ Implementation

```cpp
/**
 * Dynamic Time Warping for melodic sequence matching
 * Sources:
 *   - asardaes/dtw-cpp: https://github.com/asardaes/dtw-cpp (MIT)
 *   - Wikipedia: https://en.wikipedia.org/wiki/Dynamic_time_warping (Public Domain)
 */

#include <cmath>
#include <limits>

float Tunepal::dtw_distance(const PackedFloat32Array& seq1,
                             const PackedFloat32Array& seq2) {
    int m = seq1.size();
    int n = seq2.size();

    if (m == 0 || n == 0) return std::numeric_limits<float>::infinity();

    // DTW matrix (space-optimized: only keep two rows)
    std::vector<float> prev(n + 1, std::numeric_limits<float>::infinity());
    std::vector<float> curr(n + 1, std::numeric_limits<float>::infinity());

    prev[0] = 0.0f;

    for (int i = 1; i <= m; i++) {
        curr[0] = std::numeric_limits<float>::infinity();

        for (int j = 1; j <= n; j++) {
            // Distance between elements (can be customized for music)
            float cost = std::abs(seq1[i - 1] - seq2[j - 1]);

            // DTW recurrence
            curr[j] = cost + std::min({prev[j],      // insertion
                                        curr[j - 1],  // deletion
                                        prev[j - 1]}); // match
        }

        std::swap(prev, curr);
    }

    return prev[n];
}

// Convert note string to MIDI sequence for DTW
std::vector<float> Tunepal::string_to_pitch_sequence(const String& note_string) {
    std::vector<float> pitches;

    static const std::map<char, int> note_to_midi = {
        {'C', 60}, {'D', 62}, {'E', 64}, {'F', 65},
        {'G', 67}, {'A', 69}, {'B', 71}
    };

    godot::CharString utf8 = note_string.utf8();
    for (int i = 0; i < utf8.length(); i++) {
        char c = std::toupper(static_cast<unsigned char>(utf8[i]));
        auto it = note_to_midi.find(c);
        if (it != note_to_midi.end()) {
            pitches.push_back(static_cast<float>(it->second));
        }
    }

    return pitches;
}

float Tunepal::dtw_similarity(const String& pattern, const String& text) {
    auto seq1 = string_to_pitch_sequence(pattern);
    auto seq2 = string_to_pitch_sequence(text);

    if (seq1.empty() || seq2.empty()) return 0.0f;

    // Convert to PackedFloat32Array
    PackedFloat32Array arr1, arr2;
    arr1.resize(seq1.size());
    arr2.resize(seq2.size());
    for (size_t i = 0; i < seq1.size(); i++) arr1[i] = seq1[i];
    for (size_t i = 0; i < seq2.size(); i++) arr2[i] = seq2[i];

    float distance = dtw_distance(arr1, arr2);

    // Normalize to [0, 1] similarity
    // Max possible distance = 12 * min(m, n) (octave difference for each note)
    float max_distance = 12.0f * std::min(seq1.size(), seq2.size());
    float similarity = 1.0f - (distance / max_distance);

    return std::clamp(similarity, 0.0f, 1.0f);
}

Array Tunepal::dtw_search(const String& pattern, const Array& candidates,
                           int max_results) {
    Array results;

    std::vector<std::pair<float, int>> scores;
    scores.reserve(candidates.size());

    auto pattern_seq = string_to_pitch_sequence(pattern);
    if (pattern_seq.empty()) return results;

    PackedFloat32Array pattern_arr;
    pattern_arr.resize(pattern_seq.size());
    for (size_t i = 0; i < pattern_seq.size(); i++) {
        pattern_arr[i] = pattern_seq[i];
    }

    for (int i = 0; i < candidates.size(); i++) {
        Dictionary cand = candidates[i];
        String search_key = cand["search_key"];

        if (search_key.length() < 20) continue;

        float score = dtw_similarity(pattern, search_key);
        scores.push_back({score, i});
    }

    // Partial sort for top N
    int n = std::min(max_results, static_cast<int>(scores.size()));
    std::partial_sort(scores.begin(), scores.begin() + n, scores.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });

    for (int i = 0; i < n; i++) {
        Dictionary result;
        Dictionary original = candidates[scores[i].second];

        result["confidence"] = scores[i].first;
        result["id"] = original["id"];
        result["title"] = original["title"];
        result["search_key"] = original["search_key"];
        // Copy other fields...

        results.push_back(result);
    }

    return results;
}
```

---

## 5. Sequence Matching: Needleman-Wunsch (Fallback)

### 5.1 When to Use NW

Keep NW available for:
- Backward compatibility with existing code
- Cases where discrete note comparison is preferred
- ABC notation string comparison
- Mobile fallback (simpler algorithm)

### 5.2 Implementation

(See original implementation in Section 4.4 of previous document version - preserved for fallback)

---

## 6. C++ Architecture Design

### 6.1 File Structure

```
src/
├── tunepal.h                    # Main header
├── tunepal.cpp                  # Core implementation + bindings
├── algorithms/
│   ├── pitch_pyin.cpp           # pYIN wrapper (uses sevagh library)
│   ├── pitch_ensemble.cpp       # Ensemble (HPS + MPM + YIN + median)
│   ├── dtw.cpp                  # DTW implementation
│   └── needleman_wunsch.cpp     # NW fallback
├── third_party/
│   ├── pitch-detection/         # sevagh/pitch-detection (MIT)
│   │   ├── include/
│   │   └── src/
│   └── dtw-cpp/                 # asardaes/dtw-cpp (MIT)
└── register_types.cpp           # GDExtension registration
```

### 6.2 Dependencies

| Library | Purpose | License | Integration |
|---------|---------|---------|-------------|
| sevagh/pitch-detection | pYIN, MPM, YIN | MIT | Header-only, copy into third_party |
| asardaes/dtw-cpp | DTW | MIT | Header-only, copy into third_party |
| ffts (optional) | FFT for pitch-detection | BSD | System library or bundled |

### 6.3 Build Configuration

```python
# SConstruct additions

# Third-party includes
env.Append(CPPPATH=[
    "src/third_party/pitch-detection/include",
    "src/third_party/dtw-cpp/include",
])

# Source files
sources = [
    "src/tunepal.cpp",
    "src/algorithms/pitch_pyin.cpp",
    "src/algorithms/pitch_ensemble.cpp",
    "src/algorithms/dtw.cpp",
    "src/algorithms/needleman_wunsch.cpp",
    "src/register_types.cpp",
]

# Optimization
env.Append(CXXFLAGS=["-O3", "-march=native"])  # Desktop
```

---

## 7. Integration with Existing Code

### 7.1 GDScript Pipeline

```gdscript
# TunepalGodot/Scripts/pitch_pipeline.gd
extends Node

var tunepal = null
var use_dtw = true
var use_ensemble = false

func _ready():
    if ClassDB.class_exists("Tunepal"):
        tunepal = ClassDB.instantiate("Tunepal")
        tunepal.set_pitch_config(44100.0, 2048, 80.0, 2000.0)
        tunepal.set_pitch_algorithm(0 if not use_ensemble else 3)

func detect_pitch(audio_buffer: PackedFloat32Array) -> float:
    if tunepal:
        if use_ensemble:
            return tunepal.detect_pitch_ensemble(audio_buffer)
        else:
            return tunepal.detect_pitch_pyin(audio_buffer)
    else:
        return fallback_pitch_detection(audio_buffer)

func detect_pitch_sequence(audio_data: PackedFloat32Array,
                            hop_size: int = 512) -> Array:
    if tunepal:
        return tunepal.detect_pitch_pyin_sequence(audio_data, hop_size)
    else:
        return fallback_sequence_detection(audio_data, hop_size)

func search_tunes(pattern: String, candidates: Array,
                   max_results: int = 50) -> Array:
    if tunepal:
        if use_dtw:
            return tunepal.dtw_search(pattern, candidates, max_results)
        else:
            return tunepal.nw_search(pattern, candidates, max_results)
    else:
        return fallback_search(pattern, candidates, max_results)
```

### 7.2 Modified record.gd

```gdscript
# Key changes to record.gd

@onready var pitch_pipeline = preload("res://Scripts/pitch_pipeline.gd").new()

func _ready():
    add_child(pitch_pipeline)
    # ... existing init ...

func stop_recording():
    # Get raw audio from recorder
    var audio_data = get_recorded_audio()

    # Detect pitch sequence using pYIN
    var pitch_sequence = pitch_pipeline.detect_pitch_sequence(audio_data, 512)

    # Convert to notes
    current_notes = convert_pitches_to_notes(pitch_sequence)

    # ... existing grouping code ...

    note_string = create_string(current_notes, average_time)

    # Search using DTW
    confidences = pitch_pipeline.search_tunes(note_string, query_result, 100)

    # ... existing display code ...
```

---

## 8. Testing Strategy

### 8.1 Pitch Detection Tests

```gdscript
# tests/test_pitch_detection.gd

func test_pyin_detects_pure_tone():
    var audio = generate_sine_wave(440.0, 44100, 2048)
    var pitch = tunepal.detect_pitch_pyin(audio)
    assert(abs(pitch - 440.0) < 5.0, "Should detect A4")

func test_pyin_handles_guitar():
    var audio = load_test_audio("guitar_g3.wav")
    var pitch = tunepal.detect_pitch_pyin(audio)
    assert(abs(pitch - 196.0) < 10.0, "Should detect G3 fundamental, not harmonics")

func test_ensemble_handles_accordion():
    var audio = load_test_audio("accordion_melody.wav")
    tunepal.set_pitch_algorithm(3)  # Ensemble
    var pitches = tunepal.detect_pitch_pyin_sequence(audio, 512)
    # Verify expected melody
```

### 8.2 DTW Tests

```gdscript
func test_dtw_exact_match():
    var score = tunepal.dtw_similarity("CDEFG", "CDEFG")
    assert(score > 0.99, "Exact match should score ~1.0")

func test_dtw_handles_tempo():
    # Same melody, different speeds
    var score = tunepal.dtw_similarity("CDEFG", "CCDEEFFGG")
    assert(score > 0.8, "DTW should handle tempo variation")
```

---

## 9. Implementation Phases

### Phase 1: Core Pitch Detection (1-2 weeks)

- [ ] Integrate sevagh/pitch-detection library
- [ ] Implement pYIN wrapper in tunepal.cpp
- [ ] Add GDScript bindings
- [ ] Test on synthetic tones
- [ ] Test on harmonic-rich instruments (guitar, accordion)

### Phase 2: DTW Search (1 week)

- [ ] Integrate asardaes/dtw-cpp
- [ ] Implement DTW wrapper
- [ ] Add N-gram pre-filtering
- [ ] Benchmark search speed

### Phase 3: Integration (1 week)

- [ ] Update record.gd to use new pipeline
- [ ] Implement fallbacks for mobile
- [ ] End-to-end testing

### Phase 4: Optimization & Polish (1 week)

- [ ] Performance tuning
- [ ] Add ensemble mode
- [ ] Parameter tuning for folk instruments
- [ ] Documentation

### Phase 5: Validation (1 week)

- [ ] Test with real musicians
- [ ] Benchmark against baseline
- [ ] Benchmark against FolkFriend times
- [ ] Final parameter tuning

---

## 10. MIT License Source Documentation

### 10.1 Primary Sources

| Component | Source | License | URL |
|-----------|--------|---------|-----|
| **pYIN/MPM/YIN** | sevagh/pitch-detection | MIT | https://github.com/sevagh/pitch-detection |
| **DTW** | asardaes/dtw-cpp | MIT | https://github.com/asardaes/dtw-cpp |
| **HPS** | Wikipedia / Noll 1969 | Public Domain | Wikipedia article |
| **NW** | Wikipedia / Orion9 | Public Domain / MIT | Wikipedia + GitHub |

### 10.2 Excluded Sources (GPL - DO NOT USE)

| Source | License | Reason |
|--------|---------|--------|
| FolkFriend | GPL-3.0 | Copyleft |
| Original pYIN (soundsoftware) | GPL | Copyleft |
| LibPyin | GPL-3.0 | Copyleft |
| Aubio | GPL-3.0 | Copyleft |
| Essentia | AGPL-3.0 | Copyleft |
| Cycfi Q (Hz pitch detection) | GPL-3.0 | Copyleft |

### 10.3 Attribution Template

```cpp
/**
 * [Component] Implementation
 *
 * MIT-Licensed Sources:
 *   - sevagh/pitch-detection: https://github.com/sevagh/pitch-detection
 *   - asardaes/dtw-cpp: https://github.com/asardaes/dtw-cpp
 *
 * Public Domain Sources:
 *   - Wikipedia pitch detection algorithms
 *   - Original academic papers (Noll 1969, Viterbi 1967, etc.)
 *
 * NOT derived from GPL-licensed code (FolkFriend, pYIN original, etc.)
 */
```

---

## Document History

| Date | Version | Changes |
|------|---------|---------|
| 2024-12 | 1.0 | Initial specification (HPS + Viterbi + NW) |
| 2024-12 | 2.0 | **Major update:** Replaced HPS with pYIN, added DTW, integrated Viterbi into pYIN, added ensemble option. Based on research findings. |

---

*This document provides complete specifications for implementing pitch detection and matching algorithms optimized for folk music instruments, including harmonic-rich instruments like guitar and accordion.*
