# Exploration 003: Learning & Playback Features

**Date:** December 8, 2025
**Status:** Proposal
**Priority:** High
**Decision:** Strongly Recommended - Aligns with Original Vision

---

## Executive Summary

This document explores enhanced learning and playback features for TunePal, with careful attention to **aligning with Bryan Duggan's original vision** while extending capabilities for richer practice experiences.

**Key Finding:** The features proposed here are **100% aligned** with Bryan Duggan's documented plans. The TunepalGodot README explicitly mentions "interactive music scores with playback, repeat sections and slowdown" and "support for full range of instruments, sessions and recordings."

**Even Better:** The existing Godot MIDI Player addon already has much of the infrastructure we need - tempo control, transposition, and per-channel volume/mute are already implemented.

---

## Bryan Duggan's Vision (Source of Truth)

### From TunepalGodot README

> "Interactive music scores with playback, repeat sections and slowdown"

> "Support for full range of instruments, sessions and recordings"

### From Current TunePal App

The existing TunePal already supports:
- **128 high quality MIDI instruments**
- **Speed up, slow down or transpose playback**
- **Transcription fundamental adjustment** for non-concert pitch instruments

### From Research Papers

TunePal's documented features include:
> "The ability to play a section of a tune and the ability to speed up or slow down a tune"

---

## Proposed Features & Alignment

| Feature | Bryan's Vision | Alignment | Status |
|---------|----------------|-----------|--------|
| **Tempo Control** | "slowdown" | ✅ Perfect | Extends existing |
| **Transposition** | "transpose playback" | ✅ Perfect | Already exists |
| **Multi-Instrument Playback** | "full range of instruments, sessions" | ✅ Perfect | New implementation |
| **Per-Instrument Mixing** | Implied by "sessions" | ✅ Aligned | New implementation |
| **A-B Loop/Repeat** | "repeat sections" | ✅ Perfect | Extends existing |
| **iOS Audio/MIDI Routing** | Cross-platform support | ✅ Essential | New implementation |
| **Lilt/Expression** | Authentic trad sound | ✅ Essential | New implementation |

**Verdict:** All proposed features directly align with or extend Bryan Duggan's documented vision. No conflicts detected.

---

## Feature 1: Tempo Control (Enhanced)

### Current State

The existing MIDI player already has:
```gdscript
## 再生速度 (Playback speed)
@export_range (0.0, 100.0) var play_speed:float = 1.0
```

The current TunePal also has speed control.

### Proposed Enhancement

| Capability | Current | Proposed |
|------------|---------|----------|
| Speed range | 0-100% | 25%-200% with finer control |
| UI | Basic slider | Preset buttons + slider |
| Pitch preservation | ✅ Yes | ✅ Yes |
| Gradual acceleration | ❌ No | ✅ Yes ("practice mode") |

### Gradual Acceleration Mode (New)

For learners, offer a "practice mode" that:
1. Starts at 50% tempo
2. Automatically increases tempo by 5% after each successful loop
3. Continues until reaching target tempo (100%)

**Use Case:** Learning a difficult passage by starting slow and gradually building speed.

### UI Design

```
┌─────────────────────────────────────────────────────────┐
│  Tempo Control                                          │
│  ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐      │
│  │ 50% │ │ 75% │ │ 100%│ │ 125%│ │ 150%│ │ 200%│      │
│  └─────┘ └─────┘ └─────┘ └─────┘ └─────┘ └─────┘      │
│                                                         │
│  Fine control: ━━━━━━━━━━━●━━━━━━━━━  85%              │
│                                                         │
│  [ ] Practice Mode: Auto-accelerate from 50% → 100%    │
└─────────────────────────────────────────────────────────┘
```

### Implementation Effort

| Task | Effort |
|------|--------|
| UI for tempo presets | 2-4 hours |
| Fine-grained slider | 1-2 hours |
| Gradual acceleration logic | 4-6 hours |
| Testing & polish | 2-4 hours |
| **Total** | **9-16 hours** |

---

## Feature 2: Transposition

### Current State

The MIDI player already has:
```gdscript
## キーシフト (Key shift)
@export var key_shift:int = 0
```

The current TunePal supports transposition and transcription fundamental adjustment.

### Proposed Enhancement

| Capability | Current | Proposed |
|------------|---------|----------|
| Semitone range | Unknown | ±12 semitones (full octave) |
| Key display | None | Show resulting key (e.g., "G Major → A Major") |
| Instrument presets | Manual | Preset for common transposing instruments |
| Capo simulation | ❌ No | ✅ Yes (for guitar accompaniment) |

### Transposing Instrument Presets

| Instrument | Transposition | Common Use |
|------------|---------------|------------|
| Bb Clarinet | +2 semitones | Play along with clarinet |
| Eb Saxophone | +3 semitones | Alto sax in sessions |
| F French Horn | +5 semitones | Horn arrangements |
| C Flute/Whistle | 0 (concert) | Default |

### UI Design

```
┌─────────────────────────────────────────────────────────┐
│  Transposition                                          │
│                                                         │
│  Original Key: G Major                                  │
│                                                         │
│  Transpose: ━━━━━━━━●━━━━━━━━━━  +2 semitones          │
│             -12        0        +12                     │
│                                                         │
│  Result: A Major                                        │
│                                                         │
│  Quick Presets:                                         │
│  ┌────────────┐ ┌────────────┐ ┌────────────┐          │
│  │ Bb Clarinet│ │ Eb Alto Sax│ │ Capo 2     │          │
│  └────────────┘ └────────────┘ └────────────┘          │
└─────────────────────────────────────────────────────────┘
```

### Implementation Effort

| Task | Effort |
|------|--------|
| Key detection from ABC | 2-4 hours |
| Transpose UI with key display | 2-4 hours |
| Instrument presets | 2-3 hours |
| Testing & polish | 2-3 hours |
| **Total** | **8-14 hours** |

---

## Feature 3: Multi-Instrument Playback

### Bryan's Vision

> "Support for full range of instruments, sessions and recordings"

This is the most significant new feature - allowing users to practice with a **full accompaniment**, not just melody.

### Concept: Session Simulation

Instead of hearing just the melody on a single instrument, users can:
- Add **bodhrán** for rhythmic pulse
- Add **guitar** for chord accompaniment
- Add **bouzouki** for harmonic support
- Keep or mute the **melody instrument**

**Result:** Practice feels like playing in an actual session.

### Technical Approach

The ABC notation contains chord symbols (e.g., `"G"`, `"D"`, `"Em"`). We can:

1. **Parse chord symbols** from ABC
2. **Generate accompaniment patterns** based on tune type (jig, reel, hornpipe)
3. **Assign to MIDI channels** (melody on channel 0, bodhrán on 9, guitar on 1, etc.)
4. **Allow independent control** of each instrument

### Accompaniment Patterns by Tune Type

| Tune Type | Time Sig | Bodhrán Pattern | Guitar Strumming |
|-----------|----------|-----------------|------------------|
| Reel | 4/4 | DUM-dum-DUM-dum | Down-up-down-up |
| Jig | 6/8 | DUM-di-di-DUM-di-di | Down-down-up |
| Slip Jig | 9/8 | DUM-di-di-DUM-di-di-DUM-di-di | Complex |
| Hornpipe | 4/4 (dotted) | Swing feel | Syncopated |
| Polka | 2/4 | DUM-dum-DUM-dum | Boom-chick |

### MIDI Channel Assignment

| Channel | Purpose | Default Instrument |
|---------|---------|-------------------|
| 0 | Melody | Tin Whistle (Recorder) |
| 1 | Guitar (chords) | Acoustic Guitar (Nylon) |
| 2 | Bouzouki | Acoustic Guitar (Steel) |
| 9 | Percussion (Bodhrán) | Drum Kit |

### UI Design

```
┌─────────────────────────────────────────────────────────┐
│  Session Instruments                                    │
│                                                         │
│  ┌─────────────────────────────────────────────────┐   │
│  │ ● Melody (Tin Whistle)          Vol: ━━━━●━━  │   │
│  │   [Change Instrument ▼]                        │   │
│  └─────────────────────────────────────────────────┘   │
│                                                         │
│  ┌─────────────────────────────────────────────────┐   │
│  │ ☑ Guitar (Acoustic)             Vol: ━━●━━━━  │   │
│  │   [Change Instrument ▼]                        │   │
│  └─────────────────────────────────────────────────┘   │
│                                                         │
│  ┌─────────────────────────────────────────────────┐   │
│  │ ☑ Bodhrán                       Vol: ━━━●━━━  │   │
│  │   Pattern: [Jig ▼]                             │   │
│  └─────────────────────────────────────────────────┘   │
│                                                         │
│  ┌─────────────────────────────────────────────────┐   │
│  │ ☐ Bouzouki                      Vol: ━━━━━━━  │   │
│  └─────────────────────────────────────────────────┘   │
│                                                         │
│  Preset: [Pub Session ▼]  [Full Band] [Melody Only]   │
└─────────────────────────────────────────────────────────┘
```

### Presets

| Preset | Instruments | Use Case |
|--------|-------------|----------|
| **Melody Only** | Tin whistle | Learning notes |
| **Melody + Rhythm** | Melody + bodhrán | Feel the groove |
| **Duo** | Melody + guitar | Intimate practice |
| **Full Session** | All instruments | Performance prep |

### Implementation Complexity

This is the most complex feature and can be phased:

**Phase 1: Simple Accompaniment**
- Bodhrán pattern generation (based on time signature)
- Guitar drone/chord root only
- Effort: 24-32 hours

**Phase 2: Chord-Based Accompaniment**
- Parse chord symbols from ABC
- Generate chord voicings
- Strumming patterns
- Effort: 32-48 hours

**Phase 3: Smart Accompaniment**
- Infer chords from melody (when not specified)
- Dynamic patterns based on tune structure
- Effort: 40-60 hours

### Implementation Effort (Phase 1 MVP)

| Task | Effort |
|------|--------|
| ABC chord symbol parsing | 4-8 hours |
| Bodhrán pattern generation | 8-12 hours |
| Guitar root note accompaniment | 4-6 hours |
| Multi-channel MIDI output | 4-8 hours |
| UI for instrument selection | 8-12 hours |
| Testing & polish | 8-12 hours |
| **Total (Phase 1)** | **36-58 hours** |

---

## Feature 4: Per-Instrument Mixing

### Current State

The MIDI player already supports per-channel:
```gdscript
## 音量値 (Volume)
var volume:float = 100.0 / 127.0
## チャンネルミュート (Channel mute)
var mute:bool
```

And applies volume per channel:
```gdscript
func _apply_channel_volume( channel:GodotMIDIPlayerChannelStatus ) -> void:
    AudioServer.set_bus_volume_db(...)
```

### Proposed Enhancement

| Capability | Current | Proposed |
|------------|---------|----------|
| Per-channel volume | ✅ Exists | Expose in UI |
| Per-channel mute | ✅ Exists | Expose in UI |
| Per-channel pan | ✅ Exists | Expose in UI |
| Master volume | ✅ Exists | Expose in UI |
| Solo mode | ❌ No | Add |

### UI Design (Integrated with Multi-Instrument)

The mixing controls are integrated into the instrument panel shown above. Each instrument has:
- **Enable/Disable toggle** (checkbox)
- **Volume slider** (0-100%)
- **Instrument selection** (dropdown)

Advanced mixer view (optional):

```
┌─────────────────────────────────────────────────────────┐
│  Mixer                                                  │
│                                                         │
│   Melody    Guitar   Bodhrán  Bouzouki   Master        │
│     │         │         │         │         │          │
│     █         █         █         ░         █          │
│     █         █         █         ░         █          │
│     █         ▓         █         ░         █          │
│     █         ▓         ▓         ░         █          │
│     █         ▓         ▓         ░         █          │
│     ▓         ░         ▓         ░         █          │
│     ▓         ░         ░         ░         ▓          │
│     ░         ░         ░         ░         ░          │
│   ──●──     ──●──     ──●──     ──●──     ──●──        │
│   100%       70%       80%       0%       100%         │
│                                                         │
│   [S] [M]   [S] [M]   [S] [M]   [S] [M]                │
│                                                         │
│   S = Solo, M = Mute                                    │
└─────────────────────────────────────────────────────────┘
```

### Implementation Effort

| Task | Effort |
|------|--------|
| Volume slider UI | 2-4 hours |
| Mute/Solo buttons | 2-4 hours |
| Connect to MIDI player | 2-4 hours |
| Testing | 2-4 hours |
| **Total** | **8-16 hours** |

---

## Feature 5: A-B Loop (Phrase Practice)

### Bryan's Vision

> "repeat sections"

### Current State

The MIDI player has basic loop support:
```gdscript
## ループフラグ (Loop flag)
@export var loop:bool = false
## ループ開始位置 (Loop start position)
@export var loop_start:float = 0.0
```

And emits a signal when looping:
```gdscript
## ループ時 (When looped)
signal looped
```

### Proposed Enhancement

| Capability | Current | Proposed |
|------------|---------|----------|
| Loop entire tune | ✅ Yes | ✅ Yes |
| Set custom start | ✅ Yes | ✅ Yes |
| Set custom end | ❌ No | ✅ Yes (A-B points) |
| Visual markers | ❌ No | ✅ Yes |
| Snap to bar lines | ❌ No | ✅ Yes |
| Loop count display | ❌ No | ✅ Yes |

### A-B Loop Workflow

1. **Play tune** normally
2. **Tap "A" button** at loop start point
3. **Tap "B" button** at loop end point
4. **Playback loops** between A and B
5. **Count display** shows loop repetitions
6. **Clear A-B** to return to normal playback

### UI Design

```
┌─────────────────────────────────────────────────────────┐
│  Playback Controls                                      │
│                                                         │
│         ┌───────────────────────────────────────┐       │
│  Time:  │  A────────●════════════════B─────────│       │
│         │  0:04    0:12              0:20  0:32│       │
│         └───────────────────────────────────────┘       │
│                                                         │
│    [|◄]  [◄◄]  [  ▶  ]  [►►]  [►|]                     │
│                                                         │
│    [Set A]  [Set B]  [Clear A-B]   Loop: 3 / ∞         │
│                                                         │
│    [ ] Snap to bar lines                                │
│    [ ] Count-in (1 bar before A)                        │
└─────────────────────────────────────────────────────────┘
```

### Advanced Features

1. **Snap to Bar Lines**: A and B points automatically align to musical boundaries
2. **Count-In**: Play one bar before A point to prepare
3. **Gradual Tempo**: Combine with tempo control for progressive practice
4. **Loop Counter**: Track how many times you've practiced the section

### Implementation Effort

| Task | Effort |
|------|--------|
| A-B point selection | 4-6 hours |
| Loop end position handling | 2-4 hours |
| Visual timeline with markers | 6-10 hours |
| Snap to bar functionality | 4-6 hours |
| Count-in feature | 2-4 hours |
| Loop counter display | 1-2 hours |
| Testing & polish | 4-6 hours |
| **Total** | **23-38 hours** |

---

## Feature 6: iOS Audio & MIDI Interoperability

### Why This Matters

TunePal should integrate seamlessly with the iOS music creation ecosystem. Users want to:
- **Send MIDI to Loopy Pro** to use tunes as backing tracks
- **Route audio to other apps** for recording or effects
- **Receive MIDI control** from external controllers
- **Use TunePal as a source** for music production workflows

### iOS Audio Technologies (2024 Standards)

| Technology | Status | Purpose |
|------------|--------|---------|
| **AUv3** (Audio Unit v3) | Current standard | Plugin format for instruments/effects |
| **Core MIDI** | Active | Virtual MIDI ports between apps |
| **Inter-App Audio (IAA)** | Deprecated | Legacy audio routing |
| **Audiobus** | Still used | Third-party audio routing |

**AUv3 is the current standard.** It allows:
- Multiple instances of the same plugin
- State saving with host session
- Both audio AND MIDI routing

### Proposed Implementation

#### Level 1: Core MIDI Output (MVP)

TunePal sends MIDI to a virtual port that other apps can receive:

```
TunePal → Virtual MIDI Port → Loopy Pro / GarageBand / AUM
```

**User workflow:**
1. Open TunePal, select a tune
2. Open Loopy Pro, set input to "TunePal MIDI"
3. Press play in TunePal → Loopy Pro receives MIDI
4. Record in Loopy Pro with your own instrument layered

**Implementation:** Use Godot's platform-specific plugin to access Core MIDI APIs.

#### Level 2: AUv3 MIDI Generator

TunePal as an AUv3 plugin that hosts can load:

```
Host App (AUM, Loopy Pro, GarageBand)
    └── TunePal AUv3 (MIDI Generator)
            └── Outputs MIDI to any channel
```

**Benefits:**
- Tight integration with host transport (play/stop)
- Tempo sync with host
- Save/load state with host project

**Complexity:** High - requires building separate AUv3 extension.

#### Level 3: Audio Output (Optional)

Route TunePal's audio to other apps:

```
TunePal → Audio Output → Loopy Pro (Audio Input)
```

This allows recording the actual sound, not just MIDI. Lower priority since MIDI is more flexible.

### Technical Architecture

```
┌─────────────────────────────────────────────────────────┐
│                     TunePal App                          │
│  ┌───────────────────────────────────────────────────┐  │
│  │              Core MIDI Module                      │  │
│  │  - Create virtual MIDI output port                 │  │
│  │  - Send note on/off, CC, program change           │  │
│  │  - Sync with internal playback                    │  │
│  └───────────────────────────────────────────────────┘  │
│                          │                               │
│                          ▼                               │
│  ┌───────────────────────────────────────────────────┐  │
│  │         Platform Plugin (iOS/macOS)               │  │
│  │  - CoreMIDI framework integration                 │  │
│  │  - MIDIClientCreate, MIDISourceCreate             │  │
│  │  - MIDIPacketListAdd, MIDISend                    │  │
│  └───────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
                          │
                          ▼
              ┌───────────────────────┐
              │   Virtual MIDI Bus     │
              │   "TunePal Output"     │
              └───────────────────────┘
                          │
          ┌───────────────┼───────────────┐
          ▼               ▼               ▼
    ┌──────────┐    ┌──────────┐    ┌──────────┐
    │Loopy Pro │    │ AUM      │    │GarageBand│
    └──────────┘    └──────────┘    └──────────┘
```

### Use Cases

| Use Case | Workflow |
|----------|----------|
| **Practice with looper** | TunePal MIDI → Loopy Pro → Layer your own playing |
| **Record backing track** | TunePal MIDI → GarageBand → Record with software instruments |
| **Live performance** | TunePal MIDI → AUM → Route to multiple synths |
| **Transcription aid** | TunePal MIDI → Notation app → See staff notation |

### Implementation Effort

| Task | Effort |
|------|--------|
| Core MIDI platform plugin (iOS) | 16-24 hours |
| MIDI output from playback engine | 8-12 hours |
| Settings UI for MIDI output | 4-6 hours |
| Testing with Loopy Pro, AUM | 8-12 hours |
| **Total (Level 1)** | **36-54 hours** |

| AUv3 Extension (Level 2) | Additional Effort |
|--------------------------|-------------------|
| AUv3 app extension setup | 16-24 hours |
| State save/restore | 8-12 hours |
| Host transport sync | 8-12 hours |
| Testing & certification | 16-24 hours |
| **Total (Level 2)** | **48-72 hours** |

---

## Feature 7: Lilt & Expression (Humanization)

### The Problem

MIDI playback of traditional Irish music sounds **flat, mechanical, and lifeless** without proper expression. Musicians who know this music will immediately notice and be put off.

> "Lilt is an essential ingredient in playing Irish music."
> — The Celtic Room

### What is Lilt?

Lilt is defined as "a pleasant gentle swinging rhythm, a characteristic rising and falling in the tune."

**Key components:**
- **Timing variations** - Notes slightly ahead or behind the beat
- **Velocity variations** - Dynamic emphasis on certain beats
- **Note length variations** - Some notes held longer than written
- **Swing/triplet feel** - Especially in jigs and hornpipes

### Tune-Type Specific Patterns

| Tune Type | Time | Lilt Characteristics |
|-----------|------|---------------------|
| **Reel** | 4/4 | Driving, straight feel with slight push on beat 1 |
| **Jig** | 6/8 | Natural triplet bounce, emphasis on 1 and 4 |
| **Slip Jig** | 9/8 | Rolling feel, 3+3+3 grouping |
| **Hornpipe** | 4/4 | Heavy swing, almost dotted-eighth feel |
| **Polka** | 2/4 | Bouncy, strong backbeat |

### Technical Approach: Humanization Parameters

#### 1. Timing Variation (Microsift)

Shift notes slightly forward or backward from the grid:

```
Grid (mechanical):    |    |    |    |    |
                      ●    ●    ●    ●    ●

With lilt:           |    |    |    |    |
                     ●     ●  ●      ●   ●
                    (early)(late)(early)(on)(late)
```

**Parameters:**
- `timing_variance`: 0-100% (how much notes can shift)
- `swing_amount`: 0-100% (bias toward pushing off-beats)

#### 2. Velocity Variation (Dynamics)

Emphasize certain beats and vary individual note velocities:

```
Jig pattern (6/8):
Beat:      1    2    3    4    5    6
Velocity: 100  70   80   95   70   75
          ▓▓▓  ░░   ▓    ▓▓   ░░   ░░
```

**Parameters:**
- `velocity_variance`: 0-100% (random variation amount)
- `accent_strength`: 0-100% (how strong the beat emphasis is)

#### 3. Swing Amount

For hornpipes and some jigs, apply swing to eighth notes:

```
Straight eighths:  ♪ ♪ ♪ ♪  (equal duration)
                   50-50

Swung eighths:     ♪. ♬ ♪. ♬  (long-short pattern)
                   60-40 or 67-33 (triplet feel)
```

**Parameters:**
- `swing_ratio`: 50% (straight) to 75% (heavy swing)

#### 4. Articulation

Traditional music uses varied articulation:
- **Legato** - Notes connected smoothly
- **Staccato** - Notes separated with space
- **Accents** - Strong attack on specific notes

**Parameters:**
- `articulation_style`: smooth / bouncy / punchy
- `note_separation`: 0-100% (gap between notes)

### User Interface: The Lilt Slider

A single **"Lilt" slider** that adjusts all humanization parameters together, with optional advanced controls for fine-tuning:

```
┌─────────────────────────────────────────────────────────┐
│  Expression                                             │
│                                                         │
│  Lilt:  ━━━━━━━━━━━━●━━━━━━━━━━━━  65%                 │
│         Square               Expressive                 │
│                                                         │
│  Tune Type: [Jig ▼]  (auto-detected from time sig)     │
│                                                         │
│  [Advanced ▼]                                           │
│  ┌─────────────────────────────────────────────────┐   │
│  │ Timing Variance:  ━━━━━●━━━━━  40%              │   │
│  │ Swing Amount:     ━━━━━━●━━━━  55%              │   │
│  │ Velocity Variance:━━━━━━━●━━━  60%              │   │
│  │ Accent Strength:  ━━━━●━━━━━━  35%              │   │
│  └─────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
```

### Preset Lilt Profiles

| Profile | Settings | Use Case |
|---------|----------|----------|
| **Square** (0%) | No humanization | Learning exact notes |
| **Light Lilt** (25%) | Subtle timing/velocity | Early practice |
| **Session Feel** (50%) | Moderate swing and dynamics | Normal practice |
| **Expressive** (75%) | Strong lilt, clear accents | Performance prep |
| **Wild** (100%) | Maximum variation | Experienced players |

### Technical Implementation

In the MIDI player, apply transformations before sending notes:

```gdscript
func apply_lilt(note: MIDINote, lilt_amount: float, tune_type: String) -> MIDINote:
    var profile = get_lilt_profile(tune_type)

    # Timing shift
    var timing_shift = randf_range(-1, 1) * profile.timing_variance * lilt_amount
    timing_shift += get_swing_offset(note.beat_position, profile.swing_ratio)
    note.time += timing_shift

    # Velocity variation
    var velocity_base = get_accent_velocity(note.beat_position, profile.accent_pattern)
    var velocity_variance = randf_range(-1, 1) * profile.velocity_variance * lilt_amount
    note.velocity = clamp(velocity_base + velocity_variance, 1, 127)

    # Note length adjustment
    note.duration *= lerp(1.0, profile.articulation_factor, lilt_amount)

    return note
```

### Per-Tune-Type Profiles

```gdscript
var LILT_PROFILES = {
    "reel": {
        "timing_variance": 0.03,  # 3% of beat
        "swing_ratio": 0.50,      # straight
        "accent_pattern": [1.0, 0.7, 0.85, 0.7],  # 4/4 pattern
        "velocity_variance": 0.15
    },
    "jig": {
        "timing_variance": 0.04,
        "swing_ratio": 0.55,      # slight swing
        "accent_pattern": [1.0, 0.7, 0.75, 0.95, 0.7, 0.75],  # 6/8 pattern
        "velocity_variance": 0.18
    },
    "hornpipe": {
        "timing_variance": 0.05,
        "swing_ratio": 0.62,      # pronounced swing
        "accent_pattern": [1.0, 0.65, 0.9, 0.65],
        "velocity_variance": 0.20
    }
}
```

### Research Foundation

This approach is based on industry-standard MIDI humanization techniques:

> "Even slight timing variations add immense realism. MIDI note offsetting involves shifting notes ahead or behind the beat, creating a natural 'push and pull' feeling."

> "If you're manually setting a MIDI randomizer, try setting the timing and velocity to around 3-5%; this is a good starting point."

> "Swing is a term used by musicians to define slight variations in timing aimed to give liveliness and unpredictability to a rhythm."

### Implementation Effort

| Task | Effort |
|------|--------|
| Lilt algorithm core | 12-16 hours |
| Per-tune-type profiles | 8-12 hours |
| UI slider and presets | 4-6 hours |
| Advanced controls panel | 4-6 hours |
| Testing with musicians | 8-12 hours |
| Refinement based on feedback | 8-12 hours |
| **Total** | **44-64 hours** |

### Success Criteria

A musician familiar with Irish traditional music should:
1. **Not be jarred** by mechanical-sounding playback
2. **Feel the groove** appropriate to the tune type
3. **Be able to adjust** the lilt to their preference
4. **Recognize the difference** between a jig feel and a reel feel

---

## Comparison with Industry Standards

### Anytune

Anytune is the gold standard for music practice apps:
- A-B loop with visual waveform ✅ (we propose this)
- Tempo change without pitch shift ✅ (already exists)
- EQ and effects ❌ (not proposed - overkill for trad)
- Import any audio file ❌ (we generate from ABC)

### Music Looper

- Seamless A-B loop ✅ (we propose this)
- Mixer AI to isolate instruments ❌ (not needed - we generate instruments)
- Hands-free control ❌ (could add later)

### Moises

- AI instrument separation ❌ (not needed)
- BPM detection ✅ (we have exact tempo from MIDI)
- Section detection ❌ (could add based on ABC structure)

### Online Academy of Irish Music (OAIM)

Most directly relevant reference:
- Play-along backing tracks ✅ (we propose this)
- Multiple speeds (50/75/100%) ✅ (we propose this)
- Melody only or melody + accompaniment ✅ (we propose this)
- Guitar chords displayed ⚠️ (could add)

**TunePal Advantage:** We generate accompaniment from ABC, so users get backing tracks for **any tune**, not just pre-recorded ones.

---

## Implementation Roadmap

### Phase 1: Core Practice Features (MVP)

| Feature | Effort | Priority |
|---------|--------|----------|
| Enhanced tempo UI | 9-16 hours | P1 |
| A-B loop basics | 15-24 hours | P1 |
| Transposition UI | 8-14 hours | P1 |
| **Phase 1 Total** | **32-54 hours** | |

### Phase 2: Lilt & Expression (Critical for Authenticity)

| Feature | Effort | Priority |
|---------|--------|----------|
| Lilt algorithm core | 12-16 hours | P2 |
| Per-tune-type profiles | 8-12 hours | P2 |
| UI slider and presets | 4-6 hours | P2 |
| Testing with musicians | 8-12 hours | P2 |
| **Phase 2 Total** | **32-46 hours** | |

### Phase 3: Multi-Instrument

| Feature | Effort | Priority |
|---------|--------|----------|
| Bodhrán patterns | 8-12 hours | P3 |
| Basic chord accompaniment | 12-20 hours | P3 |
| Instrument selection UI | 8-12 hours | P3 |
| Per-instrument mixing | 8-16 hours | P3 |
| **Phase 3 Total** | **36-60 hours** | |

### Phase 4: iOS MIDI Interoperability

| Feature | Effort | Priority |
|---------|--------|----------|
| Core MIDI platform plugin | 16-24 hours | P4 |
| MIDI output from playback | 8-12 hours | P4 |
| Settings UI for MIDI output | 4-6 hours | P4 |
| Testing with Loopy Pro, AUM | 8-12 hours | P4 |
| **Phase 4 Total** | **36-54 hours** | |

### Phase 5: Advanced Features

| Feature | Effort | Priority |
|---------|--------|----------|
| Gradual acceleration mode | 4-6 hours | P5 |
| Smart chord inference | 20-30 hours | P5 |
| Advanced accompaniment patterns | 20-30 hours | P5 |
| Snap to bar lines | 4-6 hours | P5 |
| AUv3 extension (optional) | 48-72 hours | P5 |
| **Phase 5 Total** | **96-144 hours** | |

### Total Estimated Effort

| Phase | Hours | Description |
|-------|-------|-------------|
| Phase 1 | 32-54 | Core practice features |
| Phase 2 | 32-46 | Lilt & expression (authenticity) |
| Phase 3 | 36-60 | Multi-instrument accompaniment |
| Phase 4 | 36-54 | iOS MIDI interoperability |
| Phase 5 | 96-144 | Advanced features |
| **Total** | **232-358 hours** | |

### Priority Rationale

**Phase 2 (Lilt) is elevated to high priority** because:
1. Without it, playback sounds mechanical and off-putting
2. Musicians familiar with trad will reject square-sounding playback
3. It's a differentiator - no other ABC player does this well
4. Relatively low effort for high impact

---

## Technical Notes

### Existing Infrastructure

The Godot MIDI Player already provides:

```gdscript
# Tempo control
@export_range (0.0, 100.0) var play_speed:float = 1.0

# Transposition
@export var key_shift:int = 0

# Looping
@export var loop:bool = false
@export var loop_start:float = 0.0

# Per-channel control
var volume:float = 100.0 / 127.0
var mute:bool
var pan:float = 0.5

# Seek to position
func seek( to_position:float ) -> void

# Signals
signal looped
signal finished
```

**This is excellent!** Most of the core playback functionality already exists. We're building UI and enhancing, not starting from scratch.

### ABC Chord Parsing

ABC notation includes chord symbols:
```
|: "G"GABc "D"d2 ef | "Em"g2 fe "D"d2 cB |
```

We need to parse these and generate accompaniment.

### Accompaniment Generation

For bodhrán, we need patterns based on:
- Time signature (6/8 for jig, 4/4 for reel)
- Beat emphasis (downbeat, upbeat)
- Fills at phrase boundaries

For guitar, we need:
- Chord voicings mapped to MIDI notes
- Strumming patterns (note timing within beat)
- Bass note handling

---

## Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| ABC chord parsing complexity | Medium | Medium | Start with simple patterns, add complexity |
| Accompaniment sounds mechanical | High | Medium | Humanization (timing variation, velocity variation) |
| Too many features overwhelm users | Medium | Medium | Progressive disclosure, presets |
| Performance with multiple instruments | Low | Medium | MIDI player already handles 96 polyphony |

---

## Recommendation

### Verdict: STRONGLY RECOMMENDED

These features:
1. **Align perfectly** with Bryan Duggan's documented vision
2. **Build on existing infrastructure** in the Godot MIDI player
3. **Differentiate TunePal** from simple tune recognition apps
4. **Address real user needs** for learning and practice
5. **Enable session simulation** - unique in the market

### Suggested Approach

1. **Start with Phase 1** (tempo, A-B loop, transposition)
2. **Validate with users** before Phase 2
3. **Multi-instrument is the differentiator** - prioritize once Phase 1 is solid

### Key Differentiator

**No other app generates backing tracks dynamically from ABC notation.** OAIM has pre-recorded tracks for ~150 tunes. TunePal could have backing tracks for **24,000+ tunes** - because we generate them on the fly.

---

## Sources

### Bryan Duggan's Vision
- [TunepalGodot GitHub](https://github.com/skooter500/TunepalGodot) - "Interactive music scores with playback, repeat sections and slowdown"
- [Tunepal Blog Posts](https://bryanduggan.org/tag/tunepal/)
- [TunePal on Google Play](https://play.google.com/store/apps/details?id=org.tunepal) - "128 MIDI instruments, speed up/slow down, transpose"

### Music Practice Apps
- [Anytune](https://www.anytune.app/) - Industry standard practice app
- [Music Looper](https://apps.apple.com/us/app/music-looper-pitch-slow/id1189818821) - A-B loop specialist
- [Moises](https://moises.ai/) - AI-powered practice tool
- [Transcribe+](https://apps.apple.com/us/app/slow-down-music-transcribe/id1048119179) - Slow-down specialist

### Traditional Music Learning
- [Online Academy of Irish Music (OAIM)](https://oaim.ie/about/play-along-backing-tracks/) - Play-along backing tracks
- [Bodhrán Metronomes - Jon Antonsson](https://jonantonsson.bandcamp.com/album/bodhr-n-metronomes-for-practicing-irish-traditional-music)
- [Traditional Irish Music Accompaniment - McNeela Music](https://blog.mcneelamusic.com/irish-music-accompaniment-how-to-accompany-traditional-irish-music/)

### iOS Audio & MIDI Interoperability
- [AUv3 MIDI Tutorial - AudioKit](https://audiokitpro.com/auv3-midi-tutorial-part1/) - How to build AUv3 MIDI extensions
- [AUv3 for MIDI Routing - Loopy Pro Forum](https://forum.loopypro.com/discussion/32491/auv3-for-sending-midi-between-apps)
- [Audio Unit Extensions - Midiflow Blog](https://www.midiflow.com/blog/audio-unit-extensions.html)
- [What is AUv3 - Loopy Pro Forum](https://forum.loopypro.com/discussion/26091/what-is-auv3)
- [Audiobus, IAA and AUv3 Differences - FutureSonic](https://futuresonic.io/discussion/audiobus-iaa-and-auv3-whats-the-difference/)
- [Loopy Pro Manual](https://loopypro.com/manual/)

### Irish Music Lilt & Expression
- [The Lilt of Irish Music - The Celtic Room](https://www.thecelticroom.org/playing-irish-music/irish-music-lilt.html)
- [Lift, Lilt and Swing Discussion - Chiff and Fipple](https://forums.chiffandfipple.com/viewtopic.php?t=33816)
- [Irish Tune Types - Tradschool](https://www.tradschool.com/en/irish-music/tune-types-in-irish-traditional-music/)
- [Understanding Irish Tune Types - McNeela Music](https://blog.mcneelamusic.com/understanding-the-five-main-types-of-irish-tunes/)

### MIDI Humanization Techniques
- [How to Humanize MIDI - Unison Audio](https://unison.audio/how-to-humanize-midi/)
- [6 Ways to Humanize Beats - Logic Pro X](https://www.macprovideo.com/article/audio-software/7-ways-to-humanize-beats-and-midi-regions-in-logic-pro-x)
- [Humanizing MIDI Drums - MixElite](https://mixelite.com/blog/humanizing-midi-drums/)
- [5 Ways to Humanize Productions - Black Ghost Audio](https://www.blackghostaudio.com/blog/5-ways-to-humanize-your-productions)
