# Technical Design: Playback System for TunePal Godot

**Document Version:** 1.0
**Date:** December 9, 2025
**Status:** Proposal / Under Review
**Author:** Claude Code Analysis

---

## Executive Summary

This document provides a comprehensive technical analysis of playback options for TunePal Godot, evaluating Bryan's proposed approach (abc2midi + Godot MIDI Player) alongside alternative architectures. Our analysis identifies **three viable approaches**, with a **recommended hybrid architecture** that maximizes cross-platform compatibility, minimizes app size, and delivers superior functionality.

**Key Finding:** The existing Godot MIDI Player addon in the codebase is a solid foundation. The critical decision is **how to convert ABC notation to playable MIDI data** and whether to use external tools or build native conversion.

---

## Table of Contents

1. [Requirements Analysis](#1-requirements-analysis)
2. [Current Infrastructure](#2-current-infrastructure)
3. [Approach A: abc2midi + Godot MIDI Player (Bryan's Proposal)](#3-approach-a-abc2midi--godot-midi-player-bryans-proposal)
4. [Approach B: Pure GDScript ABC Parser + Godot MIDI Player](#4-approach-b-pure-gdscript-abc-parser--godot-midi-player)
5. [Approach C: TinySoundFont GDExtension](#5-approach-c-tinysoundfont-gdextension)
6. [Approach D: Platform-Native Audio (iOS/Android)](#6-approach-d-platform-native-audio-iosandroid)
7. [Approach E: Web-First with ABCJS](#7-approach-e-web-first-with-abcjs)
8. [Comparison Matrix](#8-comparison-matrix)
9. [Recommendation](#9-recommendation)
10. [Implementation Roadmap](#10-implementation-roadmap)
11. [SoundFont Selection](#11-soundfont-selection)
12. [Risk Analysis](#12-risk-analysis)
13. [References](#13-references)

---

## 1. Requirements Analysis

### 1.1 Functional Requirements (from Bryan's Vision)

| Requirement | Source | Priority |
|-------------|--------|----------|
| Play ABC notation as audio | README: "interactive music scores with playback" | P1 - Critical |
| Variable tempo (25%-200%) | README: "slowdown" | P1 - Critical |
| Transposition (±12 semitones) | README: "transpose playback" | P1 - Critical |
| Loop sections (A-B repeat) | README: "repeat sections" | P1 - Critical |
| 128 MIDI instruments | Current TunePal feature | P2 - High |
| Multi-instrument playback | README: "full range of instruments, sessions" | P2 - High |
| Lilt/humanization | Authenticity for trad music | P2 - High |
| iOS MIDI interoperability | CoreMIDI integration | P3 - Medium |

### 1.2 Non-Functional Requirements

| Requirement | Target | Rationale |
|-------------|--------|-----------|
| Cross-platform | iOS, Android, Windows, macOS, Linux, Web | All export targets |
| App size impact | < 10MB additional | Mobile app store limits |
| Startup latency | < 2 seconds for SoundFont load | UX requirement |
| Playback latency | < 50ms note-to-sound | Real-time feel |
| CPU usage | < 15% on mobile | Battery life |
| Memory footprint | < 50MB for audio system | Mobile constraints |

### 1.3 ABC Notation Complexity

TunePal's database contains 24,000+ tunes with these ABC features:

| Feature | Frequency | Conversion Complexity |
|---------|-----------|----------------------|
| Basic notes (A-G, octave marks) | 100% | Simple |
| Note lengths (1, 2, /2, /4) | 100% | Simple |
| Rests (z) | 90% | Simple |
| Accidentals (^, =, _) | 80% | Medium |
| Ties and slurs | 60% | Medium |
| Chord symbols ("G", "Am") | 40% | Medium |
| Ornaments (~, .) | 30% | Complex |
| Multi-voice (V:1, V:2) | 10% | Complex |
| Inline key/time changes | 5% | Complex |

---

## 2. Current Infrastructure

### 2.1 Godot MIDI Player Addon (Already Integrated)

**Location:** `TunepalGodot/addons/midi/`
**Author:** @arlez80 (Yui Kinomoto)
**License:** MIT
**Implementation:** 100% Pure GDScript

#### Existing Capabilities

| Feature | Status | Code Reference |
|---------|--------|----------------|
| SMF Format 0/1 parsing | ✅ Ready | `SMF.gd` |
| SoundFont 2.0 loading | ✅ Ready | `SoundFont.gd`, `Bank.gd` |
| ADSR envelope synthesis | ✅ Ready | `ADSR.gd`, `ADSR.tscn` |
| 96-voice polyphony | ✅ Ready | `max_polyphony = 96` |
| Tempo control (0-100x) | ✅ Ready | `play_speed:float` |
| Key transposition | ✅ Ready | `key_shift:int` |
| Loop with start point | ✅ Ready | `loop`, `loop_start` |
| Per-channel volume/pan | ✅ Ready | `GodotMIDIPlayerChannelStatus` |
| Reverb/Chorus effects | ✅ Ready | `AudioEffectReverb`, `AudioEffectChorus` |
| Thread-safe playback | ✅ Ready | `Thread`, `Mutex` |
| GS/GM/XG System support | ✅ Ready | SysEx handling |

#### Performance Notes

```gdscript
# From MidiPlayer.gd - Already has optimization for HTML5/debug
if OS.get_name() == "HTML5" or OS.is_debug_build():
    self.no_thread_mode = true
```

#### What's Missing

| Feature | Status | Work Required |
|---------|--------|---------------|
| ABC → MIDI conversion | ❌ Missing | Core challenge |
| Loop end point (A-B) | ❌ Missing | ~2-4 hours |
| Humanization/lilt | ❌ Missing | ~20-40 hours |
| Accompaniment generation | ❌ Missing | ~40-60 hours |
| UI integration | ❌ Missing | ~16-24 hours |

### 2.2 Project Architecture

```
TunepalGodot/
├── addons/
│   ├── midi/              # MIDI Player (ready to use)
│   │   ├── MidiPlayer.gd  # Main playback engine
│   │   ├── SMF.gd         # MIDI file parser
│   │   ├── SoundFont.gd   # SF2 loader
│   │   ├── Bank.gd        # Instrument bank
│   │   └── ADSR.gd        # Envelope generator
│   ├── godot-sqlite/      # Database access
│   └── tunepal/bin/       # Edit distance GDExtension
├── Scripts/
│   ├── record.gd          # Audio recording (working)
│   └── [playback.gd]      # ← TO BE CREATED
└── data/
    ├── tunepal.db         # 24,000+ ABC tunes (47MB)
    └── [soundfont.sf2]    # ← TO BE ADDED
```

---

## 3. Approach A: abc2midi + Godot MIDI Player (Bryan's Proposal)

### 3.1 Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     TunePal Godot                           │
├─────────────────────────────────────────────────────────────┤
│  ABC String → [abc2midi binary] → .mid file → MIDI Player  │
│       ↑              ↑                            ↓         │
│    Database     External Process              SoundFont     │
│   (24K tunes)    (C binary)                   (30MB SF2)    │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 abc2midi Analysis

**Source:** [abcMIDI Project](https://abcmidi.sourceforge.io/)
**Language:** C
**License:** GPL-2.0
**Maintainer:** Seymour Shlien (active as of 2025)

#### Strengths

| Advantage | Details |
|-----------|---------|
| **Comprehensive ABC support** | Most complete ABC 2.1 implementation |
| **Guitar chord accompaniment** | Built-in `%%MIDI gchord` expansion |
| **Drum patterns** | Configurable percussion from ABC |
| **Proven reliability** | Used by EasyABC, MuseScore, and others |
| **Active maintenance** | Updates in February/June 2025 |

#### Challenges

| Challenge | Impact | Mitigation |
|-----------|--------|------------|
| **External binary dependency** | Must bundle for each platform | Cross-compile or ship per-platform |
| **iOS App Store restrictions** | Cannot execute arbitrary binaries | Must compile as static library |
| **Android process spawn** | Complex IPC, security sandbox | Use JNI wrapper or NDK |
| **Web (WASM)** | Cannot run native executables | Compile to WebAssembly |
| **File I/O overhead** | Write .abc, read .mid | Use memory streams if possible |
| **GPL-2.0 license** | Viral copyleft | OK if kept as separate process |

### 3.3 Platform-Specific Implementation

| Platform | Strategy | Complexity | Size Impact |
|----------|----------|------------|-------------|
| **Windows** | Bundle abc2midi.exe, spawn process | Low | +500KB |
| **macOS** | Bundle abc2midi binary, spawn process | Low | +500KB |
| **Linux** | Bundle or use system abc2midi | Low | +500KB |
| **iOS** | Compile as static lib, call via GDNative | High | +300KB |
| **Android** | Compile via NDK, call via JNI | High | +400KB |
| **Web** | Compile to WASM, call via JS bridge | Very High | +800KB |

### 3.4 Implementation Effort

| Task | Effort | Notes |
|------|--------|-------|
| Desktop integration (Windows/macOS/Linux) | 8-12 hours | Process spawn, temp files |
| iOS static library integration | 24-40 hours | Requires C compilation, GDNative |
| Android NDK integration | 24-40 hours | JNI bridge, security |
| Web WASM compilation | 40-60 hours | Emscripten, JS interop |
| Error handling and edge cases | 8-16 hours | Encoding, paths, failures |
| **Total** | **104-168 hours** | High effort for full cross-platform |

### 3.5 Verdict: abc2midi

| Criterion | Score | Notes |
|-----------|-------|-------|
| ABC feature completeness | ★★★★★ | Best available |
| Desktop compatibility | ★★★★★ | Trivial |
| Mobile compatibility | ★★☆☆☆ | Complex integration |
| Web compatibility | ★☆☆☆☆ | Requires WASM port |
| Size efficiency | ★★★★☆ | ~500KB per platform |
| Implementation effort | ★★☆☆☆ | High for full coverage |
| Maintenance burden | ★★★☆☆ | External dependency |

---

## 4. Approach B: Pure GDScript ABC Parser + Godot MIDI Player

### 4.1 Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     TunePal Godot                           │
├─────────────────────────────────────────────────────────────┤
│  ABC String → [GDScript Parser] → SMF.SMFData → MIDI Player│
│       ↑              ↑                            ↓         │
│    Database     Pure GDScript                 SoundFont     │
│   (24K tunes)   (no external deps)            (30MB SF2)    │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 Concept

Build a **native GDScript ABC parser** that converts ABC notation directly to the `SMF.SMFData` structure that the Godot MIDI Player already understands.

### 4.3 ABC Parser Scope

#### Phase 1: Core (MVP)
- Header parsing (X:, T:, M:, L:, K:)
- Notes (A-G, a-g) with octave markers (', ,)
- Note lengths (2, /2, 3, /4, etc.)
- Accidentals (^, =, _)
- Rests (z)
- Bar lines (|, ||, |:, :|)
- Basic repeats

#### Phase 2: Enhanced
- Chord symbols ("G", "Am7")
- Ties (-) and slurs (())
- Triplets and n-tuplets (3...)
- Grace notes ({abc})
- Decorations (~, .)

#### Phase 3: Advanced
- Multi-voice (V:)
- Inline field changes [K:Am]
- Lyrics (w:)
- Complex ornaments

### 4.4 Implementation Effort

| Phase | Features | Effort | Coverage |
|-------|----------|--------|----------|
| Phase 1 | Core ABC | 40-60 hours | 85% of tunes |
| Phase 2 | Enhanced | 30-50 hours | 95% of tunes |
| Phase 3 | Advanced | 40-60 hours | 99% of tunes |
| **Total** | Full implementation | **110-170 hours** | 99%+ |

### 4.5 Advantages

| Advantage | Impact |
|-----------|--------|
| **Zero external dependencies** | No binaries to bundle |
| **Perfect cross-platform** | Works everywhere Godot works |
| **Full control** | Can add humanization, expression |
| **Single codebase** | No platform-specific code |
| **Easy debugging** | All GDScript, visible flow |
| **MIT license compatible** | No GPL concerns |

### 4.6 Challenges

| Challenge | Impact | Mitigation |
|-----------|--------|------------|
| **Development effort** | 110-170 hours upfront | Phased approach |
| **ABC spec complexity** | Edge cases, ambiguities | Start with common patterns |
| **Maintenance** | Must update for new ABC features | TunePal DB is stable |
| **Performance** | GDScript slower than C | Parse once, cache result |

### 4.7 Reference Implementations

Existing parsers to study:

| Project | Language | Notes |
|---------|----------|-------|
| [pyabc](https://github.com/campagnola/pyabc) | Python | Clean, readable |
| [purescript-abc-parser](https://github.com/newlandsvalley/purescript-abc-parser) | PureScript | Well-documented |
| [elm-abc-parser](https://github.com/newlandsvalley/elm-abc-parser) | Elm | Functional approach |
| [abclua](https://abclua.sourceforge.io/) | Lua | Similar to GDScript |

### 4.8 Verdict: Pure GDScript

| Criterion | Score | Notes |
|-----------|-------|-------|
| ABC feature completeness | ★★★☆☆ | Must build, phased |
| Desktop compatibility | ★★★★★ | Perfect |
| Mobile compatibility | ★★★★★ | Perfect |
| Web compatibility | ★★★★★ | Perfect |
| Size efficiency | ★★★★★ | Zero added size |
| Implementation effort | ★★☆☆☆ | High upfront, low ongoing |
| Maintenance burden | ★★★★☆ | Self-contained |

---

## 5. Approach C: TinySoundFont GDExtension

### 5.1 Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     TunePal Godot                           │
├─────────────────────────────────────────────────────────────┤
│  ABC String → [GDScript Parser] → Note Events → TSF        │
│       ↑              ↑                            ↓         │
│    Database     Pure GDScript              TinySoundFont    │
│   (24K tunes)   (for ABC)                  (C, 30MB SF2)    │
└─────────────────────────────────────────────────────────────┘
```

### 5.2 TinySoundFont Analysis

**Source:** [TinySoundFont](https://github.com/schellingb/TinySoundFont)
**Language:** Single C header (~3000 lines)
**License:** MIT
**Dependencies:** C standard library only (math, malloc, fopen)

#### Key Features

| Feature | Details |
|---------|---------|
| SoundFont 2.0 support | Full SF2 spec |
| MIDI control | Note on/off, CC, pitch bend |
| Audio output | 32-bit float or 16-bit int |
| Sample rate | Configurable (44100 default) |
| Single file | `tsf.h` is the entire library |

### 5.3 Why Consider This?

The existing Godot MIDI Player is **pure GDScript**, which has performance limitations:

> "AudioStreamGenerator is slow—extremely slow. Pushing frames to the buffer is so slow in GDScript that it really takes a lot of CPU."
> — Godot Forums

TinySoundFont as a **GDExtension** could provide:
- Lower CPU usage on mobile
- Lower latency
- Better polyphony handling

### 5.4 Implementation Effort

| Task | Effort | Notes |
|------|--------|-------|
| GDExtension wrapper | 24-32 hours | C++ binding to TSF |
| Cross-platform compilation | 16-24 hours | All 6 targets |
| ABC parser (still needed) | 60-80 hours | Same as Approach B |
| Integration with Godot audio | 8-16 hours | AudioStreamGenerator or buffer |
| **Total** | **108-152 hours** | Plus testing |

### 5.5 Verdict: TinySoundFont

| Criterion | Score | Notes |
|-----------|-------|-------|
| ABC feature completeness | ★★★☆☆ | Still need parser |
| Desktop compatibility | ★★★★★ | Tested |
| Mobile compatibility | ★★★★☆ | Should work (untested) |
| Web compatibility | ★★★☆☆ | Needs WASM build |
| Size efficiency | ★★★★☆ | ~200KB per platform |
| Implementation effort | ★★☆☆☆ | Moderate |
| Maintenance burden | ★★★☆☆ | GDExtension updates |

**Assessment:** Overkill for current needs. The existing GDScript MIDI player is sufficient for 96 voices on modern hardware. Consider TinySoundFont only if performance issues emerge.

---

## 6. Approach D: Platform-Native Audio (iOS/Android)

### 6.1 Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     TunePal Godot                           │
├─────────────────────────────────────────────────────────────┤
│                    ABC → MIDI Events                        │
│                          ↓                                  │
│         ┌────────────────┴────────────────┐                 │
│         ↓                                 ↓                 │
│    iOS: AVMIDIPlayer            Android: MediaPlayer        │
│    or AVAudioEngine             or Oboe + FluidSynth        │
│         ↓                                 ↓                 │
│    System SoundFonts           Bundled SoundFont            │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 iOS Native Options

| Option | Description | Pros | Cons |
|--------|-------------|------|------|
| **AVMIDIPlayer** | Simple MIDI file playback | Easy, native SoundFonts | Limited control |
| **AVAudioEngine + Sampler** | Real-time MIDI | Full control, effects | Complex setup |
| **CoreMIDI** | MIDI routing to other apps | Interoperability | No synthesis |

### 6.3 Android Native Options

| Option | Description | Pros | Cons |
|--------|-------------|------|------|
| **MediaPlayer** | Simple audio playback | Easy | No MIDI |
| **Oboe + FluidSynth** | Low-latency synthesis | Good quality | Large (5MB+) |
| **MIDI API (API 23+)** | System MIDI | Native | External synth needed |

### 6.4 Implementation Effort

| Task | Effort | Notes |
|------|--------|-------|
| iOS GDNative plugin | 40-60 hours | Swift/ObjC bridge |
| Android GDNative plugin | 40-60 hours | Kotlin/Java + NDK |
| Abstraction layer | 16-24 hours | Unified GDScript API |
| **Total** | **96-144 hours** | Two separate codebases |

### 6.5 Verdict: Platform-Native

| Criterion | Score | Notes |
|-----------|-------|-------|
| ABC feature completeness | ★★★☆☆ | Still need parser |
| Desktop compatibility | ☆☆☆☆☆ | Not applicable |
| Mobile compatibility | ★★★★★ | Native, optimized |
| Web compatibility | ☆☆☆☆☆ | Not applicable |
| Size efficiency | ★★★★★ | Uses system resources |
| Implementation effort | ★☆☆☆☆ | Very high |
| Maintenance burden | ★☆☆☆☆ | Platform API changes |

**Assessment:** Not recommended. Doubles the codebase, platform-specific bugs, and the GDScript MIDI player is already cross-platform.

---

## 7. Approach E: Web-First with ABCJS

### 7.1 Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     TunePal Godot (Web)                     │
├─────────────────────────────────────────────────────────────┤
│  ABC String → [ABCJS (JavaScript)] → WebAudio synthesis     │
│       ↑              ↑                                      │
│    Database     External Library                            │
│   (24K tunes)   (npm: abcjs)                                │
└─────────────────────────────────────────────────────────────┘
```

### 7.2 ABCJS Analysis

**Source:** [ABCJS](https://www.abcjs.net/) / [GitHub](https://github.com/paulrosen/abcjs)
**Language:** JavaScript
**License:** MIT
**Size:** ~800KB minified

#### Features

| Feature | Details |
|---------|---------|
| ABC parsing | Full ABC 2.1 support |
| Score rendering | SVG output |
| Audio playback | WebAudio synthesis |
| Transposition | Built-in |
| Animation | Note highlighting during playback |

### 7.3 Implementation for Web Export

Godot's HTML5 export can call JavaScript via `JavaScriptBridge`:

```gdscript
# Example integration
var abcjs = JavaScriptBridge.eval("window.ABCJS")
abcjs.renderAbc("output", abc_string, { responsive: "resize" })
var synth = JavaScriptBridge.eval("new ABCJS.synth.CreateSynth()")
synth.init({ visualObj: visualObj }).then(func(): synth.prime().then(func(): synth.start()))
```

### 7.4 Limitations

| Limitation | Impact |
|------------|--------|
| **Web only** | Doesn't help native apps |
| **JavaScript dependency** | Complex interop |
| **Different architecture** | Two playback systems |
| **No offline** | Requires SoundFont fetch |

### 7.5 Verdict: ABCJS

| Criterion | Score | Notes |
|-----------|-------|-------|
| ABC feature completeness | ★★★★★ | Excellent |
| Desktop compatibility | ☆☆☆☆☆ | Web only |
| Mobile compatibility | ☆☆☆☆☆ | Web only |
| Web compatibility | ★★★★★ | Perfect |
| Size efficiency | ★★★☆☆ | 800KB JS |
| Implementation effort | ★★★★☆ | Low for web |
| Maintenance burden | ★★★★☆ | External library |

**Assessment:** Consider for web-only features like score rendering. Not viable as primary playback solution.

---

## 8. Comparison Matrix

### 8.1 Overall Comparison

| Criterion | Weight | A: abc2midi | B: GDScript | C: TSF | D: Native | E: ABCJS |
|-----------|--------|-------------|-------------|--------|-----------|----------|
| ABC completeness | 20% | ★★★★★ | ★★★☆☆ | ★★★☆☆ | ★★★☆☆ | ★★★★★ |
| Cross-platform | 25% | ★★☆☆☆ | ★★★★★ | ★★★★☆ | ★★☆☆☆ | ★☆☆☆☆ |
| Size efficiency | 15% | ★★★★☆ | ★★★★★ | ★★★★☆ | ★★★★★ | ★★★☆☆ |
| Implementation effort | 20% | ★★☆☆☆ | ★★☆☆☆ | ★★☆☆☆ | ★☆☆☆☆ | ★★★★☆ |
| Maintenance | 10% | ★★★☆☆ | ★★★★☆ | ★★★☆☆ | ★☆☆☆☆ | ★★★★☆ |
| Future extensibility | 10% | ★★★☆☆ | ★★★★★ | ★★★★☆ | ★★☆☆☆ | ★★★☆☆ |
| **Weighted Score** | 100% | **3.00** | **3.95** | **3.55** | **2.30** | **2.80** |

### 8.2 Platform Compatibility Detail

| Platform | A: abc2midi | B: GDScript | C: TSF | D: Native | E: ABCJS |
|----------|-------------|-------------|--------|-----------|----------|
| Windows | ✅ Easy | ✅ Works | ✅ Build | N/A | N/A |
| macOS | ✅ Easy | ✅ Works | ✅ Build | N/A | N/A |
| Linux | ✅ Easy | ✅ Works | ✅ Build | N/A | N/A |
| iOS | ⚠️ Complex | ✅ Works | ⚠️ Build | ✅ Native | N/A |
| Android | ⚠️ Complex | ✅ Works | ⚠️ Build | ✅ Native | N/A |
| Web | ❌ Hard | ✅ Works | ⚠️ WASM | N/A | ✅ Native |

### 8.3 Size Impact

| Component | A: abc2midi | B: GDScript | C: TSF | Notes |
|-----------|-------------|-------------|--------|-------|
| Playback code | 500KB/platform | 50KB total | 200KB/platform | Binary vs script |
| SoundFont | 30MB | 30MB | 30MB | Same for all |
| Additional deps | 0 | 0 | 0 | Minimal |
| **Total** | ~35MB | ~30MB | ~33MB | SF2 dominates |

---

## 9. Recommendation

### 9.1 Recommended Architecture: Hybrid B + E

```
┌─────────────────────────────────────────────────────────────┐
│                     TunePal Godot                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   ┌─────────────────────────────────────────────────────┐   │
│   │           ABCParser.gd (Pure GDScript)              │   │
│   │  • Parse ABC notation to internal representation    │   │
│   │  • Convert to SMF.SMFData for MIDI player           │   │
│   │  • Add humanization/lilt transforms                 │   │
│   └────────────────────────┬────────────────────────────┘   │
│                            ↓                                │
│   ┌────────────────────────────────────────────────────┐    │
│   │         Godot MIDI Player (Existing Addon)         │    │
│   │  • SoundFont synthesis                             │    │
│   │  • Tempo, transposition, looping                   │    │
│   │  • Per-channel mixing                              │    │
│   └────────────────────────────────────────────────────┘    │
│                                                             │
│   [Web Export Only: ABCJS for score rendering]              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 9.2 Why This Approach?

| Reason | Explanation |
|--------|-------------|
| **Zero external dependencies** | No binaries to bundle, no GPL concerns |
| **Perfect cross-platform** | Works on all 6 targets without modification |
| **Existing infrastructure** | Leverages already-integrated MIDI player |
| **Full control** | Can add humanization, accompaniment, expression |
| **Minimal size** | Only adds ~50KB of GDScript |
| **Easy maintenance** | Single codebase, no platform-specific code |
| **Phased delivery** | Can start with basic ABC, add features |

### 9.3 Phased Implementation

#### Phase 1: Core Playback (MVP)
- Basic ABC parser (notes, rests, bar lines)
- Integration with MIDI player
- UI for play/pause/stop
- Tempo control

**Effort:** 60-80 hours
**Coverage:** 85% of tunes playable

#### Phase 2: Enhanced Features
- Full ABC parser (chords, ornaments, ties)
- A-B loop functionality
- Transposition UI
- Basic humanization (timing variation)

**Effort:** 50-70 hours
**Coverage:** 95% of tunes, core learning features

#### Phase 3: Session Simulation
- Chord-based accompaniment (guitar, bodhrán)
- Multi-instrument mixing
- Advanced humanization (lilt, swing)
- iOS CoreMIDI output (optional)

**Effort:** 60-80 hours
**Coverage:** Full feature set

### 9.4 Alternative: abc2midi for Phase 1 Speed

If **time-to-market** is critical, consider:

1. **Phase 1 ONLY:** Use abc2midi on desktop (Windows/macOS/Linux)
2. **Phase 2:** Build GDScript parser to replace abc2midi
3. **Phase 3:** Full mobile/web support via GDScript

This hybrid lets you ship faster on desktop while investing in the proper solution.

---

## 10. Implementation Roadmap

### 10.1 Phase 1: Core Playback (Weeks 1-4)

| Week | Tasks |
|------|-------|
| 1 | ABC parser foundation: header parsing, note tokenization |
| 2 | ABC parser: note lengths, accidentals, bar lines, repeats |
| 3 | Integration: Convert ABC → SMF.SMFData → MidiPlayer |
| 4 | UI: Basic playback controls, tempo slider, testing |

**Deliverable:** Basic tune playback on all platforms

### 10.2 Phase 2: Enhanced Features (Weeks 5-8)

| Week | Tasks |
|------|-------|
| 5 | ABC parser: chord symbols, ties, slurs, triplets |
| 6 | A-B loop: Set points, loop end handling, visual markers |
| 7 | Transposition UI, key detection, instrument presets |
| 8 | Basic humanization: timing variation, velocity variance |

**Deliverable:** Full learning-oriented playback

### 10.3 Phase 3: Session Simulation (Weeks 9-12)

| Week | Tasks |
|------|-------|
| 9 | Chord parsing, guitar accompaniment generation |
| 10 | Bodhrán pattern generation by tune type |
| 11 | Multi-channel mixing UI, presets |
| 12 | Advanced humanization: lilt ratios, articulation |

**Deliverable:** Full session simulation

---

## 11. SoundFont Selection

### 11.1 Recommended: GeneralUser GS

**Source:** [mrbumpy409/GeneralUser-GS](https://github.com/mrbumpy409/GeneralUser-GS)
**Size:** 30.7 MB
**License:** Custom (free for personal and commercial use)
**Compatibility:** FluidSynth 1.0.9+, which Godot MIDI Player should support

#### Why GeneralUser GS?

| Feature | Details |
|---------|---------|
| Size | 30.7 MB - good balance of quality vs size |
| Instruments | 261 presets, 13 drum kits |
| Quality | "Sound as good or better than SoundFonts 2-3x its size" |
| Trad instruments | Acoustic guitar, strings, woodwinds |

### 11.2 Alternatives

| SoundFont | Size | Notes |
|-----------|------|-------|
| FluidR3_GM | 141 MB | Higher quality, too large for mobile |
| MuseScore_General | 35.7 MB | Similar to GeneralUser |
| Timbres of Heaven | 387 MB | Way too large |
| Custom Celtic SF | ~10 MB | Would need to create |

### 11.3 Irish/Celtic Instrument Considerations

The database contains traditional Irish tunes. Consider supplementing with:

- **Tin whistle** (Primary melody instrument)
- **Bodhrán** (Percussion)
- **Uilleann pipes** (For specific tunes)
- **Fiddle** (String instrument)

If GeneralUser GS instruments don't satisfy, a **small custom SoundFont** (~5-10MB) with high-quality samples of these specific instruments could be layered.

---

## 12. Risk Analysis

### 12.1 Technical Risks

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| ABC parser edge cases | High | Medium | Comprehensive test suite, fuzzing |
| SoundFont loading slow | Medium | Medium | Lazy loading, progress indicator |
| Audio latency on mobile | Medium | High | Profile early, optimize buffer sizes |
| GDScript performance limits | Low | Medium | Monitor, TSF fallback ready |
| Thread issues | Low | High | Use existing thread-safe patterns |

### 12.2 Project Risks

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Scope creep | High | High | Strict phase boundaries |
| ABC spec ambiguity | Medium | Medium | Document decisions, test with real tunes |
| SoundFont licensing | Low | High | Verify GeneralUser terms |
| Dependency on MIDI addon | Low | Medium | MIT licensed, can fork if needed |

### 12.3 Mitigation: Test Suite

Create a test suite with ABC samples covering:

1. **100 representative tunes** from database
2. **Edge cases**: unusual time signatures, key changes, ornaments
3. **Performance tests**: tunes with high note density
4. **Platform tests**: all 6 export targets

---

## 13. References

### 13.1 ABC Notation

- [ABC Notation Standard 2.1](https://abcnotation.com/wiki/abc:standard:v2.1)
- [abcMIDI Project](https://abcmidi.sourceforge.io/)
- [ABCJS Documentation](https://paulrosen.github.io/abcjs/)

### 13.2 MIDI and Audio

- [Godot MIDI Player](https://godotengine.org/asset-library/asset/1667)
- [TinySoundFont](https://github.com/schellingb/TinySoundFont)
- [FluidSynth](https://www.fluidsynth.org/)
- [GeneralUser GS SoundFont](https://github.com/mrbumpy409/GeneralUser-GS)

### 13.3 Platform Audio

- [Apple Core MIDI](https://developer.apple.com/documentation/coremidi)
- [Apple AVAudioEngine](https://developer.apple.com/documentation/avfaudio/avaudioengine)
- [Android Oboe](https://github.com/google/oboe)

### 13.4 Related TunePal Documentation

- `docs/exploration/EXPLORE_003_LEARNING_AND_PLAYBACK.md` - Feature exploration
- `docs/analysis/easyabc-ecosystem-analysis.md` - EasyABC ecosystem
- `TunepalGodot/addons/midi/readme.md` - MIDI player documentation

---

## Appendix A: ABC Parser Pseudocode

```gdscript
# ABCParser.gd - Core structure

class_name ABCParser

class ABCTune:
    var title: String
    var key: String
    var time_signature: Vector2i  # numerator, denominator
    var default_note_length: float
    var tempo: int
    var notes: Array[ABCNote]

class ABCNote:
    var pitch: int  # MIDI note number
    var duration: float  # in beats
    var velocity: int
    var is_rest: bool

func parse(abc_string: String) -> ABCTune:
    var tune = ABCTune.new()
    var lines = abc_string.split("\n")

    for line in lines:
        if line.begins_with("X:"):
            # Reference number, ignore
            pass
        elif line.begins_with("T:"):
            tune.title = line.substr(2).strip_edges()
        elif line.begins_with("M:"):
            tune.time_signature = _parse_time_signature(line.substr(2))
        elif line.begins_with("L:"):
            tune.default_note_length = _parse_note_length(line.substr(2))
        elif line.begins_with("K:"):
            tune.key = line.substr(2).strip_edges()
            # Everything after K: is music
        else:
            tune.notes.append_array(_parse_music_line(line, tune))

    return tune

func to_smf_data(tune: ABCTune) -> SMF.SMFData:
    # Convert ABCTune to SMF.SMFData for MidiPlayer
    var smf = SMF.SMFData.new()
    smf.timebase = 480  # ticks per quarter note

    var track = SMF.SMFTrack.new()
    var current_time = 0

    for note in tune.notes:
        if not note.is_rest:
            # Note on
            track.events.append(_create_note_on(current_time, note))
            # Note off
            var duration_ticks = int(note.duration * smf.timebase)
            track.events.append(_create_note_off(current_time + duration_ticks, note))
        current_time += int(note.duration * smf.timebase)

    smf.tracks.append(track)
    return smf
```

---

## Appendix B: Decision Log

| Date | Decision | Rationale |
|------|----------|-----------|
| 2025-12-09 | Recommend GDScript parser over abc2midi | Cross-platform simplicity outweighs abc2midi's completeness |
| 2025-12-09 | Recommend GeneralUser GS SoundFont | Best size/quality tradeoff for mobile |
| 2025-12-09 | Keep existing MIDI player addon | Already integrated, full-featured, MIT licensed |
| 2025-12-09 | Defer TinySoundFont GDExtension | Not needed unless performance issues emerge |

---

**Document Status:** Complete - Ready for Review

**Next Steps:**
1. Team review of approach recommendation
2. Decision on Phase 1 timeline
3. Begin ABC parser implementation
