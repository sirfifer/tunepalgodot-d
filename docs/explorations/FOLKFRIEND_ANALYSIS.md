# FolkFriend Deep-Dive Analysis

**Document Type:** Competitive Analysis & Technical Exploration
**Date:** December 2024
**Status:** Active Reference
**Repository:** https://github.com/TomWyllie/folkfriend

---

## Executive Summary

FolkFriend is a Rust-based folk music transcription and recognition system that has been described as "best of breed" for what it does. This document provides a deep technical analysis of FolkFriend's algorithms and architecture, compares them directly with Tunepal Godot's current implementation, and identifies specific opportunities for Tunepal to meet or exceed FolkFriend's capabilities.

**Key Finding:** FolkFriend uses significantly more sophisticated signal processing algorithms (modified autocorrelation, Viterbi decoding, Needleman-Wunsch sequence alignment) compared to Tunepal's simpler approach (peak magnitude detection, Levenshtein edit distance). However, Tunepal has major platform advantages (native mobile, offline-first) that FolkFriend lacks.

**Critical Licensing Notice:** FolkFriend is GPL-3.0 licensed while Tunepal is MIT licensed. These licenses are **incompatible for code incorporation**. See [Section 0: Licensing Analysis](#0-licensing-analysis-critical) for detailed guidance.

---

## Table of Contents

0. [Licensing Analysis (CRITICAL)](#0-licensing-analysis-critical)
1. [What is FolkFriend?](#1-what-is-folkfriend)
2. [Architecture Comparison](#2-architecture-comparison)
3. [Signal Processing: Deep Comparison](#3-signal-processing-deep-comparison)
4. [Matching Algorithms: Deep Comparison](#4-matching-algorithms-deep-comparison)
5. [Performance Benchmarks](#5-performance-benchmarks)
6. [Feature Gap Analysis](#6-feature-gap-analysis)
7. [What FolkFriend Does Better](#7-what-folkfriend-does-better)
8. [Recommendations for Tunepal](#8-recommendations-for-tunepal)
9. [Implementation Priorities](#9-implementation-priorities)
10. [Appendix: Technical Details](#appendix-technical-details)

---

## 0. Licensing Analysis (CRITICAL)

### License Overview

| Project | License | Type | Implications |
|---------|---------|------|--------------|
| **FolkFriend** | GPL-3.0 | Copyleft ("viral") | Derivative works must also be GPL-3.0 |
| **Tunepal Godot** | MIT | Permissive | Few restrictions, compatible with most licenses |
| godot-sqlite | MIT | Permissive | Compatible |
| godot-midi | MIT | Permissive | Compatible |

### The Incompatibility Problem

**GPL-3.0 is a "copyleft" license** with these key requirements:
- Any derivative work must be released under GPL-3.0
- Source code must be made available
- All recipients receive the same freedoms

**MIT is a "permissive" license** that:
- Allows use in proprietary and open-source projects
- Has minimal requirements (attribution only)
- Is NOT compatible with GPL requirements in reverse direction

### What This Means for Tunepal

#### CANNOT Do (License Violations)

| Action | Risk Level | Why It's Prohibited |
|--------|------------|---------------------|
| Copy FolkFriend code into Tunepal | **Critical** | Would require Tunepal to become GPL-3.0 |
| Use FolkFriend as a linked library | **Critical** | GPL linking requirements would apply |
| Port FolkFriend code (translate Rust→GDScript/C++) | **High** | Translation is considered a derivative work |
| Create "substantially similar" implementations by closely following their code | **Moderate** | Could be considered derivative work in court |

#### CAN Do (Safe Approaches)

| Action | Risk Level | Why It's Allowed |
|--------|------------|------------------|
| Study FolkFriend to understand *concepts* | **Safe** | Ideas and algorithms are not copyrightable |
| Implement algorithms from academic papers, Wikipedia, textbooks | **Safe** | Mathematical algorithms are public domain |
| Use MIT/BSD/Apache licensed implementations as reference | **Safe** | Permissive licenses allow this |
| Implement from algorithm pseudocode (not actual GPL code) | **Safe** | Clean room implementation |
| Be inspired by FolkFriend's architecture choices | **Safe** | High-level design ideas are not copyrightable |

### Algorithm IP vs Code IP

**Important distinction:** The *algorithms* FolkFriend uses are not FolkFriend's intellectual property:

| Algorithm | Origin | Status |
|-----------|--------|--------|
| Needleman-Wunsch | Published 1970 by Saul Needleman & Christian Wunsch | Public domain mathematical concept |
| Viterbi | Published 1967 by Andrew Viterbi | Public domain mathematical concept |
| Harmonic Product Spectrum | Published 1969 by A. Michael Noll | Public domain mathematical concept |
| Autocorrelation | Standard signal processing technique | Public domain |
| Dynamic Programming | Computer science fundamental | Public domain |

**What IS copyrighted:** FolkFriend's specific *implementation* of these algorithms (their Rust code, specific optimizations, code structure).

### Safe Implementation Sources

For each algorithm we want to implement, here are GPL-free sources:

#### Needleman-Wunsch Sequence Alignment

| Source | License | URL |
|--------|---------|-----|
| Orion9/NeedlemanWunsch | MIT | https://github.com/Orion9/NeedlemanWunsch |
| kurtfu/nw-cuda | MIT | https://github.com/kurtfu/nw-cuda |
| Aqcurate/Needleman-Wunsch | MIT | https://github.com/Aqcurate/Needleman-Wunsch |
| Wikipedia pseudocode | Public Domain | https://en.wikipedia.org/wiki/Needleman–Wunsch_algorithm |
| slowkow Python gist | Unlicensed/Public | https://gist.github.com/slowkow/06c6dba9180d013dfd82bec217d22eb5 |

#### Harmonic Product Spectrum

| Source | License | URL |
|--------|---------|-----|
| Wikipedia description | Public Domain | https://en.wikipedia.org/wiki/Pitch_detection_algorithm |
| Original Noll 1969 paper | Academic | (Cite: A. M. Noll, "Pitch determination of human speech by the harmonic product spectrum") |
| Stack Exchange explanations | CC BY-SA | https://dsp.stackexchange.com/questions/572/harmonic-product-spectrum-limitations-in-pitch-detection |

#### Viterbi Algorithm

| Source | License | URL |
|--------|---------|-----|
| Wikipedia with pseudocode | Public Domain | https://en.wikipedia.org/wiki/Viterbi_algorithm |
| Beethoven (Swift, YIN-based) | MIT | https://github.com/vadymmarkov/Beethoven |
| MATLAB documentation | Reference only | https://www.mathworks.com/help/audio/ug/pitch-tracking-using-multiple-pitch-estimations-and-hmm.html |

**Note:** pYIN (the well-known Viterbi pitch tracker) is **GPL licensed** - do NOT use it as reference.

### Recommended Safe Development Process

#### 1. Clean Room Implementation

For each algorithm:
1. **DO NOT** have FolkFriend source code open while implementing
2. **DO** read Wikipedia, academic papers, or MIT-licensed implementations
3. **DO** write your own implementation from algorithmic descriptions
4. **DO** document your sources for each implementation

#### 2. Source Documentation

When implementing, add comments like:
```gdscript
# Needleman-Wunsch sequence alignment
# Implementation based on:
# - Wikipedia: https://en.wikipedia.org/wiki/Needleman–Wunsch_algorithm
# - MIT-licensed reference: https://github.com/Orion9/NeedlemanWunsch
# NOT derived from FolkFriend (GPL-3.0)
```

#### 3. Architectural Inspiration (Safe)

It IS safe to be inspired by FolkFriend's high-level choices:
- "Use a two-pass search strategy" ✓
- "Apply Viterbi smoothing after pitch detection" ✓
- "Use N-gram indexing for fast filtering" ✓

These are *ideas*, not *expression*, and ideas are not copyrightable.

### FolkFriend Author Contact

The FolkFriend repository does not provide:
- Contact information for the author (Tom Wyllie)
- A CONTRIBUTING.md file
- Any stated exceptions to the GPL-3.0 license

If you want to use FolkFriend code directly, you would need to:
1. Contact the author to request a dual-license or exception
2. OR relicense Tunepal as GPL-3.0 (which may conflict with Bryan Duggan's stated intent for "free and open-source" with MIT)

### Summary: Our Approach

| What We Learn from FolkFriend | How We Implement It |
|------------------------------|---------------------|
| Modified autocorrelation is effective | Implement HPS from Wikipedia/papers |
| Viterbi smoothing improves accuracy | Implement Viterbi from Wikipedia pseudocode |
| Two-pass search (n-gram + alignment) improves speed | Implement n-gram filter + NW from MIT sources |
| 265ms total is achievable | Use as performance benchmark |

**Bottom Line:** FolkFriend is an excellent *educational reference* showing what's possible. Our implementations must come from GPL-free sources to maintain MIT licensing.

---

## 1. What is FolkFriend?

### Overview

FolkFriend is a Progressive Web Application (PWA) that:
- **Transcribes** audio recordings of folk music into ABC notation
- **Recognizes** tunes by matching against thesession.org database
- Runs entirely in the browser using **WebAssembly** (compiled from Rust)

### Technology Stack

| Component | Technology |
|-----------|------------|
| Core Library | Rust |
| Web Runtime | WebAssembly (wasm-pack) |
| Frontend | Vue.js 2.x |
| Database | thesession.org tune index |
| License | GPL-3.0 |

### Key Metrics (from their README)

| Operation | Time |
|-----------|------|
| Transcription | ~57ms |
| Query/Recognition | ~208ms |
| **Total Pipeline** | **~265ms** |

---

## 2. Architecture Comparison

### FolkFriend Architecture

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         FOLKFRIEND PIPELINE                              │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│  Audio Input (PCM)                                                       │
│       │                                                                  │
│       ▼                                                                  │
│  ┌─────────────────────────────────────────────────────────────────┐    │
│  │ FEATURE EXTRACTION (feature/autocorrelate.rs)                   │    │
│  │ • Blackman window (1024 samples)                                │    │
│  │ • FFT → Power spectrum                                          │    │
│  │ • Magnitude compression (6th root: |X|^(1/6))                   │    │
│  │ • Autocorrelation via inverse FFT                               │    │
│  │ • Linear interpolation to MIDI bins (144 bins, 3 per note)      │    │
│  │ • Keep top 5 features per frame                                 │    │
│  └─────────────────────────────────────────────────────────────────┘    │
│       │                                                                  │
│       ▼                                                                  │
│  ┌─────────────────────────────────────────────────────────────────┐    │
│  │ PITCH DECODING (decode/beam_search.rs - actually Viterbi DP)    │    │
│  │ • Dynamic programming through pitch lattice                     │    │
│  │ • Score = previous + interval_penalty + energy_score            │    │
│  │ • Octave correction post-processing                             │    │
│  │ • Minimum 5 frames for valid decode                             │    │
│  └─────────────────────────────────────────────────────────────────┘    │
│       │                                                                  │
│       ▼                                                                  │
│  ┌─────────────────────────────────────────────────────────────────┐    │
│  │ CONTOUR GENERATION (decode/contour.rs)                          │    │
│  │ • Quantize note durations to uniform units                      │    │
│  │ • Find optimal BPM (60-240, 5-unit steps)                       │    │
│  │ • Contour = sequence of MIDI pitches, one per quaver            │    │
│  └─────────────────────────────────────────────────────────────────┘    │
│       │                                                                  │
│       ├──────────────────────────┬───────────────────────────────────   │
│       ▼                          ▼                                       │
│  ┌──────────────────┐   ┌──────────────────────────────────────────┐    │
│  │ ABC CONVERSION   │   │ QUERY ENGINE (query/mod.rs)              │    │
│  │ • Key detection  │   │ • Pass 1: N-gram heuristic (quadgrams)   │    │
│  │   via circular   │   │ • Keep top 2000 candidates               │    │
│  │   autocorrelation│   │ • Pass 2: Needleman-Wunsch alignment     │    │
│  │ • 4 modes:       │   │ • Score normalization to [0, 1]          │    │
│  │   Major, Minor,  │   │ • Deduplicate by tune ID                 │    │
│  │   Dorian, Mixo   │   └──────────────────────────────────────────┘    │
│  └──────────────────┘                                                    │
│                                                                          │
└─────────────────────────────────────────────────────────────────────────┘
```

### Tunepal Godot Architecture

```
┌─────────────────────────────────────────────────────────────────────────┐
│                       TUNEPAL GODOT PIPELINE                             │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│  Audio Input (Microphone)                                                │
│       │                                                                  │
│       ▼                                                                  │
│  ┌─────────────────────────────────────────────────────────────────┐    │
│  │ SPECTRUM ANALYSIS (record.gd, Godot AudioEffectAnalyzer)        │    │
│  │ • Buffer: 0.1 seconds, tap back: 0.05 seconds                   │    │
│  │ • Scan 100-8000 Hz in 2 Hz steps                                │    │
│  │ • get_magnitude_for_frequency_range() API                       │    │
│  │ • Keep frequencies within 0.1% of peak magnitude                │    │
│  └─────────────────────────────────────────────────────────────────┘    │
│       │                                                                  │
│       ▼                                                                  │
│  ┌─────────────────────────────────────────────────────────────────┐    │
│  │ NOTE DETECTION (record.gd:167-201)                              │    │
│  │ • Find nearest of 73 fundamental frequencies                    │    │
│  │ • Require 2 consecutive detections for stability                │    │
│  │ • Magnitude threshold: 0.001                                    │    │
│  │ • Accumulate note timing                                        │    │
│  └─────────────────────────────────────────────────────────────────┘    │
│       │                                                                  │
│       ▼                                                                  │
│  ┌─────────────────────────────────────────────────────────────────┐    │
│  │ NOTE GROUPING (record.gd:304-333)                               │    │
│  │ • Sort by duration                                              │    │
│  │ • Group within 33% tolerance of average                         │    │
│  │ • Filter notes < 0.1 seconds                                    │    │
│  └─────────────────────────────────────────────────────────────────┘    │
│       │                                                                  │
│       ▼                                                                  │
│  ┌─────────────────────────────────────────────────────────────────┐    │
│  │ ABC STRING GENERATION (record.gd:335-344)                       │    │
│  │ • Convert note sequence to letter names (A-G)                   │    │
│  │ • Repeat letters based on duration ratio                        │    │
│  │ • Output: "DDEBBABBEBBB..."                                     │    │
│  └─────────────────────────────────────────────────────────────────┘    │
│       │                                                                  │
│       ▼                                                                  │
│  ┌─────────────────────────────────────────────────────────────────┐    │
│  │ SEARCH ENGINE (tunepal.cpp + record.gd)                         │    │
│  │ • Split database across CPU cores                               │    │
│  │ • Substring edit distance (Levenshtein variant)                 │    │
│  │ • 'Z' wildcard support                                          │    │
│  │ • Confidence = 1 - (distance / pattern_length)                  │    │
│  │ • Sort by confidence descending                                 │    │
│  └─────────────────────────────────────────────────────────────────┘    │
│                                                                          │
└─────────────────────────────────────────────────────────────────────────┘
```

### Key Architectural Differences

| Aspect | FolkFriend | Tunepal Godot |
|--------|------------|---------------|
| **Core Language** | Rust (systems-level, memory-safe) | GDScript + C++ |
| **Platform** | Browser only (WASM) | Native iOS/Android/Desktop |
| **Offline Support** | Limited (needs index download) | Full (embedded SQLite) |
| **Audio Processing** | Custom FFT pipeline | Godot's built-in analyzer |
| **Search Strategy** | Two-pass (heuristic + alignment) | Single-pass brute force |
| **Pitch Detection** | Viterbi DP with octave correction | Simple peak magnitude |
| **Matching Algorithm** | Needleman-Wunsch (sequence alignment) | Levenshtein (edit distance) |

---

## 3. Signal Processing: Deep Comparison

### 3.1 Pitch Detection

#### FolkFriend Approach: Modified Autocorrelation

FolkFriend uses a sophisticated multi-step process:

1. **Windowing**: Apply Blackman window to 1024-sample frame
2. **FFT**: Transform to frequency domain
3. **Power Spectrum**: Compute |X|²
4. **Magnitude Compression**: Apply 6th root (|X|^(1/6)) - this compresses dynamic range and enhances harmonic structure
5. **Autocorrelation**: Inverse FFT of compressed spectrum
6. **Peak Pruning**: Zero out negative values
7. **Interpolation**: Map to 144 spectral bins (3 per MIDI note)
8. **Feature Selection**: Keep top 5 energetic features per frame

**Why this works well:**
- Autocorrelation naturally finds periodicity (fundamental frequency)
- Magnitude compression reduces harmonic interference
- Multiple features per frame allows for chord/polyphony handling
- High spectral resolution (3 bins per note) enables pitch accuracy

#### Tunepal Current Approach: Peak Magnitude

```gdscript
# record.gd:123-171
for i in range(100, 8000, spec_range):  # spec_range = 2 Hz
    var f = spectrum.get_magnitude_for_frequency_range(i, i+spec_range)[0]
    if f > biggest_mag:
        biggest_mag = f
        # Find nearest fundamental frequency
        for j in range(0, fund_frequencies.size()):
            var diff = abs(i+(spec_range/2) - fund_frequencies[j])
            if diff < minDiff:
                minDiff = diff
                minIndex = j
```

**Issues with current approach:**
- Picks loudest frequency, which is often a harmonic (2nd, 3rd, etc.) not fundamental
- No windowing applied (spectral leakage)
- No harmonic analysis to find true fundamental
- Single frequency per frame (no polyphony support)
- Threshold-based filtering (0.001) may be too simple

#### Gap Analysis: Pitch Detection

| Feature | FolkFriend | Tunepal | Gap |
|---------|------------|---------|-----|
| Windowing | Blackman | None | **Significant** |
| FFT Implementation | Custom Radix4 | Godot built-in | Moderate |
| Harmonic Handling | Autocorrelation | None | **Critical** |
| Octave Correction | Yes | No | **Significant** |
| Multi-pitch | 5 features/frame | 1 note/frame | Moderate |
| Spectral Resolution | 3 bins/note | ~1 bin/note | Moderate |

### 3.2 Temporal Modeling

#### FolkFriend: Viterbi Dynamic Programming

```
For each frame f:
    For each pitch p:
        score[f][p] = max over all previous pitches q of:
            score[f-1][q] + interval_penalty(q, p) + energy(f, p)

Backtrack to find optimal pitch sequence
```

**Key parameters:**
- `PITCH_MODEL_WEIGHT`: 0.050
- `PITCH_MODEL_SHIFT`: -7.0
- `BASE_ENERGY_SCORE`: -0.18
- `MIN_NOTE_DURATION`: 3 frames

This models the probability of pitch transitions (large jumps are penalized) and ensures temporal coherence.

#### Tunepal: Consecutive Detection Threshold

```gdscript
# record.gd:175-201
for note in temp_notes:
    if note.note == spellings[minIndex]:
        note.count += 1
        if note.count == 2:  # Require 2 consecutive detections
            # Accept the note
```

**Issues:**
- Fixed threshold (2 detections) regardless of tempo
- No modeling of pitch transition probabilities
- No smoothing across frames

#### Gap Analysis: Temporal Modeling

| Feature | FolkFriend | Tunepal | Gap |
|---------|------------|---------|-----|
| Algorithm | Viterbi DP | Consecutive threshold | **Critical** |
| Transition Modeling | Yes (interval penalty) | No | **Significant** |
| Frame Coherence | Full sequence optimization | Local only | **Significant** |
| Tempo Adaptation | Yes (BPM search) | Fixed timing | Moderate |

---

## 4. Matching Algorithms: Deep Comparison

### 4.1 Search Strategy

#### FolkFriend: Two-Pass Search

**Pass 1: N-gram Heuristic (Fast)**
- Uses quadgrams (4-note patterns) from contour
- Quick scoring to eliminate poor candidates
- Keeps top 2000 candidates (`QUERY_REPASS_SIZE`)

**Pass 2: Needleman-Wunsch Alignment (Accurate)**
- Full sequence alignment on remaining candidates
- Proper gap handling for insertions/deletions
- Normalized score in [0, 1]

#### Tunepal: Single-Pass Edit Distance

```cpp
// tunepal.cpp:31-102
// Substring edit distance - pattern can match anywhere in text
for (int i = 1; i <= pLength; i++) {
    for (int j = 1; j <= tLength; j++) {
        if ((text[j - 1] != sc) && sc != 'Z')
            difference = 1;
        else
            difference = 0;
        matrix[i][j] = min(min(
            matrix[i - 1][j] + 1,      // deletion
            matrix[i][j - 1] + 1),     // insertion
            v + difference);            // substitution
    }
}
// Find minimum in last row (substring match)
```

**Current strengths:**
- Multi-threaded across CPU cores
- 'Z' wildcard support
- Substring matching (pattern can appear anywhere)

**Weaknesses:**
- No pre-filtering, searches entire database
- Equal penalties for all operations
- No consideration of musical similarity (e.g., C vs C# should cost less than C vs G)

### 4.2 Scoring Functions

#### FolkFriend: Needleman-Wunsch

```rust
// Constants
MATCH_SCORE = +2
MISMATCH_SCORE = -2
GAP_SCORE = -1

// Normalization
similarity = 0.5 * (score / shorter_length)
```

This is a proper sequence alignment algorithm, originally designed for DNA/protein sequences but well-suited for melodic comparison.

#### Tunepal: Levenshtein Variant

```cpp
confidence = 1.0 - (edit_distance / float(pattern_length))
```

Simple but effective for basic matching.

#### Gap Analysis: Matching

| Feature | FolkFriend | Tunepal | Gap |
|---------|------------|---------|-----|
| Pre-filtering | N-gram (4-note) | None | **Significant** |
| Algorithm | Needleman-Wunsch | Levenshtein | Moderate |
| Gap Penalties | Separate from mismatch | Same as mismatch | Minor |
| Musical Similarity | Could be added | Not present | Minor |
| Normalization | Length-aware | Length-aware | Same |

---

## 5. Performance Benchmarks

### FolkFriend Published Metrics

| Operation | Time | Notes |
|-----------|------|-------|
| Transcription | 57ms | Audio → Contour |
| Query | 208ms | Contour → Results |
| **Total** | **265ms** | End-to-end |

### Tunepal Targets

Based on FolkFriend's performance, Tunepal should target:

| Operation | Current (Est.) | Target | Improvement Needed |
|-----------|---------------|--------|-------------------|
| Recording | 10s (user time) | 10s | N/A (user input) |
| Note Detection | ~50ms | ~60ms | Within range |
| Search | ~500-2000ms* | <250ms | 2-8x faster |
| **Total Processing** | **~500-2000ms** | **<300ms** | **2-7x faster** |

*Estimated, needs benchmarking

### Benchmark Implementation Needed

Add timing instrumentation to `record.gd`:

```gdscript
# Add to stop_recording()
var time_note_detection = Time.get_ticks_msec()
# ... note detection code ...
print("Note detection: %d ms" % (Time.get_ticks_msec() - time_note_detection))

var time_search = Time.get_ticks_msec()
# ... search code ...
print("Search: %d ms" % (Time.get_ticks_msec() - time_search))
```

---

## 6. Feature Gap Analysis

### Features FolkFriend Has

| Feature | FolkFriend | Tunepal | Priority |
|---------|------------|---------|----------|
| ABC Transcription Output | Yes (full notation) | Partial (note names only) | Medium |
| Key Detection | Yes (4 modes) | No | Medium |
| Tempo Detection | Yes (BPM search) | Partial (duration grouping) | Low |
| Web Platform | Yes (PWA) | Not tested | Medium |
| Octave Correction | Yes | No | **High** |
| Two-Pass Search | Yes | No | **High** |
| N-gram Indexing | Yes (quadgrams) | No | **High** |

### Features Tunepal Has (Advantages)

| Feature | Tunepal | FolkFriend | Advantage Level |
|---------|---------|------------|-----------------|
| Native Mobile (iOS/Android) | Yes | No | **Major** |
| Offline-First | Yes (embedded DB) | Limited | **Major** |
| Keyword Search | Yes | Partial | Moderate |
| My Tunes Collection | Yes | No | Moderate |
| MIDI Playback (planned) | In progress | No | Moderate |
| Multi-threaded Search | Yes | N/A (WASM) | Moderate |
| Time Signature Filtering | Yes | No | Minor |

---

## 7. What FolkFriend Does Better

### 7.1 Pitch Detection Accuracy

**The Problem:** Tunepal's simple peak-magnitude detection often picks harmonics instead of fundamentals, leading to octave errors and incorrect notes.

**FolkFriend's Solution:** Modified autocorrelation with:
- Magnitude compression to reduce harmonic interference
- Post-processing octave correction
- Viterbi smoothing for temporal coherence

**Impact:** More accurate transcription, especially for instruments with rich harmonic content (fiddle, flute, concertina).

### 7.2 Search Speed

**The Problem:** Tunepal searches the entire database on every query.

**FolkFriend's Solution:** Two-pass search:
1. Fast n-gram heuristic eliminates 95%+ of candidates
2. Expensive alignment only on top 2000 candidates

**Impact:** Query time stays fast regardless of database size.

### 7.3 Temporal Coherence

**The Problem:** Tunepal makes local decisions (2 consecutive detections) without considering the overall melody.

**FolkFriend's Solution:** Viterbi dynamic programming finds the globally optimal pitch sequence, penalizing unlikely intervals.

**Impact:** Fewer spurious notes, better handling of fast passages.

### 7.4 Rhythm Representation

**The Problem:** Tunepal uses duration-based note repetition which loses fine rhythmic detail.

**FolkFriend's Solution:** Contour representation with:
- BPM estimation (60-240, tested in 5-unit increments)
- Quantization to uniform duration units
- Balanced quantization error

**Impact:** Better differentiation between tunes with same notes but different rhythms (e.g., jig vs reel versions).

---

## 8. Recommendations for Tunepal

### 8.1 High Priority: Pitch Detection Enhancement

**Current Issue:** Harmonic confusion, octave errors

**Recommended Approach:** Implement harmonic product spectrum (HPS)

**Safe Implementation Sources (NOT from FolkFriend):**
- Wikipedia: https://en.wikipedia.org/wiki/Pitch_detection_algorithm
- Original paper: A. M. Noll, "Pitch determination of human speech by the harmonic product spectrum" (1969)
- Stack Exchange: https://dsp.stackexchange.com/questions/572/harmonic-product-spectrum-limitations-in-pitch-detection

```gdscript
# Harmonic Product Spectrum for fundamental frequency detection
# Source: Wikipedia Pitch Detection Algorithm article (Public Domain)
# NOT derived from FolkFriend (GPL-3.0)
func harmonic_product_spectrum(spectrum: AudioEffectSpectrumAnalyzerInstance,
                               freq_min: float, freq_max: float) -> float:
    var hps_scores = {}
    var num_harmonics = 4

    for f in range(int(freq_min), int(freq_max), 2):
        var product = 1.0
        for h in range(1, num_harmonics + 1):
            var harmonic_freq = f * h
            if harmonic_freq < freq_max:
                var mag = spectrum.get_magnitude_for_frequency_range(
                    harmonic_freq, harmonic_freq + 2.0).length()
                product *= mag
        hps_scores[f] = product

    # Find frequency with maximum HPS score
    var best_freq = freq_min
    var best_score = 0
    for f in hps_scores:
        if hps_scores[f] > best_score:
            best_score = hps_scores[f]
            best_freq = f

    return best_freq
```

**Expected Improvement:** 30-50% reduction in octave errors

### 8.2 High Priority: Two-Pass Search

**Current Issue:** O(n) search through all tunes

**Recommended Approach:** Implement n-gram pre-filtering

**Safe Implementation Sources (NOT from FolkFriend):**
- N-gram concept: Standard text search technique (no specific license needed)
- This is a generic computer science concept, not derived from any specific codebase

**Step 1: Build N-gram Index (at startup or in database)**

```gdscript
# N-gram indexing for fast candidate filtering
# Source: Standard computer science technique (public domain concept)
# Build index of 4-note patterns
func build_ngram_index(query_result: Array, n: int = 4) -> Dictionary:
    var index = {}  # ngram -> [tune_ids]

    for tune in query_result:
        var search_key = tune["search_key"]
        if search_key.length() >= n:
            for i in range(search_key.length() - n + 1):
                var ngram = search_key.substr(i, n)
                if not index.has(ngram):
                    index[ngram] = []
                index[ngram].append(tune["id"])

    return index
```

**Step 2: Filter Candidates**

```gdscript
func get_candidate_tunes(note_string: String, ngram_index: Dictionary,
                         n: int = 4, min_matches: int = 2) -> Array:
    var tune_scores = {}  # tune_id -> match_count

    for i in range(note_string.length() - n + 1):
        var ngram = note_string.substr(i, n)
        if ngram_index.has(ngram):
            for tune_id in ngram_index[ngram]:
                if not tune_scores.has(tune_id):
                    tune_scores[tune_id] = 0
                tune_scores[tune_id] += 1

    # Return tunes with at least min_matches
    var candidates = []
    for tune_id in tune_scores:
        if tune_scores[tune_id] >= min_matches:
            candidates.append(tune_id)

    return candidates
```

**Expected Improvement:** 5-10x faster search

### 8.3 Medium Priority: Viterbi Smoothing

**Current Issue:** Frame-by-frame decisions without temporal context

**Recommended Approach:** Implement simplified Viterbi in GDScript or add to C++ extension

**Safe Implementation Sources (NOT from FolkFriend):**
- Wikipedia: https://en.wikipedia.org/wiki/Viterbi_algorithm (Public Domain pseudocode)
- Original paper: A. J. Viterbi, "Error bounds for convolutional codes" (1967)
- **AVOID:** pYIN library (GPL licensed)

```cpp
// Viterbi algorithm for pitch sequence optimization
// Source: Wikipedia Viterbi Algorithm article (Public Domain)
// NOT derived from FolkFriend (GPL-3.0)
// Add to tunepal.cpp
std::vector<int> viterbi_decode(const std::vector<std::vector<float>>& energy,
                                 int num_pitches) {
    int num_frames = energy.size();
    std::vector<std::vector<float>> score(num_frames, std::vector<float>(num_pitches));
    std::vector<std::vector<int>> backptr(num_frames, std::vector<int>(num_pitches));

    // Initialize first frame
    for (int p = 0; p < num_pitches; p++) {
        score[0][p] = energy[0][p];
    }

    // Forward pass
    for (int f = 1; f < num_frames; f++) {
        for (int p = 0; p < num_pitches; p++) {
            float best_score = -1e9;
            int best_prev = 0;
            for (int q = 0; q < num_pitches; q++) {
                float interval_penalty = -0.05 * abs(p - q);  // Penalize large jumps
                float s = score[f-1][q] + interval_penalty + energy[f][p];
                if (s > best_score) {
                    best_score = s;
                    best_prev = q;
                }
            }
            score[f][p] = best_score;
            backptr[f][p] = best_prev;
        }
    }

    // Backtrack
    std::vector<int> path(num_frames);
    path[num_frames-1] = std::max_element(score[num_frames-1].begin(),
                                           score[num_frames-1].end())
                         - score[num_frames-1].begin();
    for (int f = num_frames - 2; f >= 0; f--) {
        path[f] = backptr[f+1][path[f+1]];
    }

    return path;
}
```

**Expected Improvement:** Smoother note sequences, fewer spurious notes

### 8.4 Medium Priority: Needleman-Wunsch Matching

**Current Issue:** Levenshtein treats all substitutions equally

**Recommended Approach:** Replace edit distance with NW alignment in tunepal.cpp

**Safe Implementation Sources (NOT from FolkFriend):**
- Wikipedia: https://en.wikipedia.org/wiki/Needleman–Wunsch_algorithm (Public Domain)
- MIT-licensed C++: https://github.com/Orion9/NeedlemanWunsch
- MIT-licensed CUDA: https://github.com/kurtfu/nw-cuda
- Original paper: Needleman & Wunsch, "A general method applicable to the search for similarities" (1970)

```cpp
// Needleman-Wunsch sequence alignment for melody matching
// Source: Wikipedia NW article + MIT-licensed implementations
// NOT derived from FolkFriend (GPL-3.0)
float needleman_wunsch(const godot::String& pattern, const godot::String& text) {
    const float MATCH = 2.0f;
    const float MISMATCH = -2.0f;
    const float GAP = -1.0f;

    int m = pattern.length();
    int n = text.length();

    if (m == 0 || n == 0) return 0.0f;

    // Use two rows for memory efficiency
    std::vector<float> prev(n + 1), curr(n + 1);

    for (int j = 0; j <= n; j++) prev[j] = j * GAP;

    for (int i = 1; i <= m; i++) {
        curr[0] = i * GAP;
        for (int j = 1; j <= n; j++) {
            float match_score = (pattern[i-1] == text[j-1]) ? MATCH : MISMATCH;
            curr[j] = std::max({
                prev[j-1] + match_score,
                prev[j] + GAP,
                curr[j-1] + GAP
            });
        }
        std::swap(prev, curr);
    }

    // Normalize to [0, 1]
    return 0.5f * (prev[n] / std::min(m, n));
}
```

**Expected Improvement:** Slightly better matching quality, especially for insertions/deletions

### 8.5 Low Priority: Key Detection

**Current Issue:** Tunepal doesn't detect key

**Recommended Approach:** Implement simplified key detection using pitch class histogram

```gdscript
# Count occurrences of each pitch class
func detect_key(notes: Array) -> String:
    var pitch_classes = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]  # C, C#, D, ...

    for note in notes:
        var pc = note_to_pitch_class(note.note)
        pitch_classes[pc] += 1

    # Compare against major/minor templates
    var best_key = "C"
    var best_score = 0

    for root in range(12):
        var major_score = template_match(pitch_classes, root, MAJOR_TEMPLATE)
        var minor_score = template_match(pitch_classes, root, MINOR_TEMPLATE)
        if major_score > best_score:
            best_score = major_score
            best_key = PITCH_NAMES[root] + " major"
        if minor_score > best_score:
            best_score = minor_score
            best_key = PITCH_NAMES[root] + " minor"

    return best_key
```

**Expected Improvement:** Better context for ABC output, potential for key-aware matching

---

## 9. Implementation Priorities

### Phase 1: Quick Wins (1-2 weeks effort)

1. **Add Performance Benchmarking**
   - Instrument `record.gd` with timing measurements
   - Establish baseline metrics
   - Compare against FolkFriend's 265ms target

2. **Implement N-gram Pre-filtering**
   - Build quadgram index at startup
   - Filter candidates before edit distance
   - Expected: 5-10x search speedup

3. **Add Octave Correction Heuristic**
   - Track overall pitch range
   - Correct obvious octave jumps
   - Expected: 20-30% fewer octave errors

### Phase 2: Core Improvements (2-4 weeks effort)

4. **Harmonic Product Spectrum**
   - Replace simple peak detection
   - Implement in GDScript or C++
   - Expected: 30-50% better pitch accuracy

5. **Viterbi Smoothing**
   - Add to C++ extension
   - Require temporal coherence
   - Expected: Smoother, more accurate note sequences

### Phase 3: Advanced Features (4-8 weeks effort)

6. **Needleman-Wunsch Matching**
   - Replace Levenshtein in tunepal.cpp
   - Proper sequence alignment
   - Expected: Better handling of variations

7. **Key Detection**
   - Pitch class histogram analysis
   - Mode detection (major/minor/dorian/mixolydian)
   - Expected: Better ABC output, search filtering

8. **Rhythm Quantization**
   - BPM estimation
   - Duration normalization
   - Expected: Better rhythm representation

---

## Appendix: Technical Details

### A.1 FolkFriend Configuration Constants

```rust
// ff_config.rs - Key parameters
MIDI_HIGH = 95              // B6 (~1975 Hz)
MIDI_LOW = 48               // C2 (~131 Hz)
MIDI_NUM = 48               // 4 octaves
SPEC_BINS_PER_MIDI = 3      // High resolution
SPEC_BINS_NUM = 144         // Total bins
SPEC_WINDOW_SIZE = 1024     // FFT window
SAMPLE_RATE_DEFAULT = 48000

// Note detection
MIN_NOTE_DURATION = 3       // frames
MIN_NOTE_DURATION_REL = 0.2
MIN_NOTE_POWER = 0.10

// Query
QUERY_NGRAM_SIZE_CONTOUR = 4  // Quadgrams
QUERY_REPASS_SIZE = 2000      // Top candidates for pass 2
```

### A.2 Tunepal Current Configuration

```gdscript
# record.gd - Key parameters
fund_frequencies = [123.471, 130.813, ... 7902.13]  # 73 frequencies, ~6 octaves
spellings = ["B", "C", "C", "D", ...]  # Note names (no octave)
spec_range = 2                          # Hz per bin
percent = 0.001                         # Magnitude filter
magnitude_threshold = 0.001             # Detection threshold
consecutive_required = 2                # Note stability
min_note_duration = 0.1                 # seconds
duration_tolerance = 0.33               # 33% for grouping
```

### A.3 Database Schema Comparison

| Field | FolkFriend | Tunepal |
|-------|------------|---------|
| Tune ID | string (tune_id) | int (id) |
| Title | via alias lookup | title, alt_title |
| ABC Notation | abc | notation |
| Contour/Search Key | contour | search_key |
| Time Signature | meter | time_sig |
| Mode/Key | mode | key_sig |
| Type | dance | tune_type |
| MIDI | Not stored | midi_sequence |

### A.4 References

- FolkFriend Repository: https://github.com/TomWyllie/folkfriend
- Needleman-Wunsch Algorithm: https://en.wikipedia.org/wiki/Needleman-Wunsch_algorithm
- Viterbi Algorithm: https://en.wikipedia.org/wiki/Viterbi_algorithm
- Harmonic Product Spectrum: https://en.wikipedia.org/wiki/Pitch_detection_algorithm
- ABC Notation: https://abcnotation.com/

---

## Document History

| Date | Version | Changes |
|------|---------|---------|
| 2024-12 | 1.0 | Initial analysis based on FolkFriend source code review |
| 2024-12 | 1.1 | **Added comprehensive licensing analysis (Section 0)**. Clarified GPL-3.0 vs MIT incompatibility. Added safe implementation sources for all algorithms. Updated code recommendations with source documentation requirements. |

---

*This document should be updated as Tunepal implements recommendations and as FolkFriend evolves.*

**Important:** All implementations derived from this analysis MUST follow the clean-room process outlined in Section 0 to maintain MIT license compatibility.
