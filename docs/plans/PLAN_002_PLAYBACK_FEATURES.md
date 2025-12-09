# Plan 002: Comprehensive Playback Features

**Date:** December 9, 2025
**Status:** Planning
**Priority:** P1 (Critical Path)
**Depends On:** Score Visualization (WebView integration)

---

## Executive Summary

This plan addresses the complete playback experience for TunePal Nua, synthesizing Bryan Duggan's vision, existing MIDI infrastructure, and modern best practices into a flexible, expandable architecture.

**Core Principle:** Build a flexible foundation that supports future expansion rather than a rigid implementation that limits options.

### Key Requirements (from discussion)

1. **Score Visualization** - Not static; zoomable, configurable display
2. **Chord Display** - Optional display AND optional playback of chords
3. **Looping** - A-B loop for phrase practice
4. **Instrument Mix** - Per-instrument volume control
5. **High-Quality Instruments** - Professional SoundFont support
6. **Expandability** - Architecture that allows advanced features later

---

## Part 1: Score Visualization Architecture

### The Challenge

Bryan's vision specifies "Interactive music scores with playback" using ABCJS. The challenge is:
1. Godot has no native WebView
2. Score must be interactive (zoom, scroll, tap to seek)
3. Score must synchronize with playback
4. Must work on iOS, Android, and Desktop

### Option Analysis

| Option | Pros | Cons | Flexibility |
|--------|------|------|-------------|
| **A: ABCJS + WebView** | Industry standard, full features, JS ecosystem | Requires WebView addon, platform complexity | High |
| **B: abc2svg + WebView** | Lighter weight, pure SVG output | Less interactive features | Medium |
| **C: Native GDScript Renderer** | No dependencies, full control | Massive effort, limited features | Very High |
| **D: Hybrid (SVG Export + Native)** | Best of both worlds | Complex architecture | Very High |

### Recommended: Option A (ABCJS + WebView) with Option D Future Path

**Phase 1:** Use ABCJS via WebView for rapid development and full feature set
**Future:** Evaluate native renderer if WebView performance is insufficient

### WebView Integration Options (2024-2025)

Based on current ecosystem research:

| Plugin | Platforms | Maturity | License |
|--------|-----------|----------|---------|
| **[godot-webview](https://godotwebview.com/)** | Win/Mac/Linux | Active (v0.0.3, Jan 2025) | MIT |
| **[Godot WRY](https://github.com/doceazedo/godot_wry)** | Cross-platform | Active (Jun 2025) | MIT |
| **[gdcef](https://github.com/Lecrapouille/gdcef)** | Win/Mac/Linux | Mature (v0.17.0) | MIT |
| **Android WebView Plugin** | Android only | Available | MIT |
| **iOS WKWebView** | iOS only | Needs integration | N/A |

**Recommendation:** Start with **godot-webview** for desktop development, plan platform-specific solutions for mobile.

### Score Visualization Features

#### Core Features (MVP)
```
┌─────────────────────────────────────────────────────────────────┐
│  Score View                                        [−] [□] [×]  │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  The Kesh Jig                                    Key: G Major   │
│  ─────────────────────────────────────────────────────────────  │
│                                                                  │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │ 𝄞 6/8  ♪ ♪ ♪ | ♩. ♪ ♪ | ♪ ♪ ♪ | ♩. ♪ ♪ |             │   │
│  │        G       D        Em       D                        │   │
│  │        ▲ current position                                 │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                  │
│  Zoom: [−] ━━━━●━━━━━━━ [+]   100%                             │
│                                                                  │
│  Display:  [✓] Chords  [✓] Lyrics  [ ] Fingerings              │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

#### Interactive Features

| Feature | Implementation | ABCJS Support |
|---------|----------------|---------------|
| **Zoom** | CSS transform + SVG scaling | ✅ Native |
| **Scroll** | scrollHorizontal parameter | ✅ Native |
| **Tap to Seek** | Click handler → MIDI position | ✅ Via callbacks |
| **Playback Cursor** | CursorControl API | ✅ Native |
| **Chord Display Toggle** | %%gchordon/off directive | ✅ ABC directive |
| **Note Highlighting** | CSS classes on SVG elements | ✅ Native |

#### ABCJS Configuration for TunePal

```javascript
// Proposed ABCJS configuration
const abcjsParams = {
  // Rendering
  responsive: "resize",
  viewportHorizontal: true,
  scrollHorizontal: true,

  // Visual
  scale: 1.0,  // Controlled by zoom slider
  staffwidth: 800,
  wrap: { minSpacing: 1.5, maxSpacing: 2.7 },

  // Chords
  tablature: [{ instrument: 'guitar' }],  // Optional chord diagrams

  // Interaction
  clickListener: (abcElem, tuneNumber, classes) => {
    // Send position to Godot for seeking
    godotBridge.seekToPosition(abcElem.startChar);
  },

  // Animation/Cursor
  cursorControl: {
    onStart: () => {},
    onBeat: (beatInfo) => {
      // Sync with MIDI playback
      godotBridge.onBeat(beatInfo);
    },
    onEvent: (event) => {
      // Highlight current note
    }
  }
};
```

### Score ↔ Playback Synchronization Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                      GDScript (Godot)                           │
│  ┌─────────────────┐       ┌─────────────────────────────────┐  │
│  │   MidiPlayer    │◄─────►│     PlaybackController          │  │
│  │  (existing)     │       │  - position tracking            │  │
│  │  - play/stop    │       │  - tempo management             │  │
│  │  - seek         │       │  - loop handling                │  │
│  │  - channel ctrl │       │  - sync bridge                  │  │
│  └─────────────────┘       └──────────┬────────────────────┬─┘  │
│                                       │                    │     │
│                                       ▼                    │     │
│                            ┌──────────────────┐           │     │
│                            │ JavaScript Bridge│           │     │
│                            │ (WebView ↔ GD)   │           │     │
│                            └────────┬─────────┘           │     │
│                                     │                      │     │
└─────────────────────────────────────┼──────────────────────┼─────┘
                                      │                      │
                                      ▼                      │
┌─────────────────────────────────────────────────────────────────┐
│                     WebView (HTML/JS)                           │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                      ABCJS                               │   │
│  │  - renderAbc()       - Score rendering                  │   │
│  │  - TimingCallbacks   - Beat synchronization             │   │
│  │  - CursorControl     - Visual cursor                    │   │
│  │  - clickListener     - User interaction                 │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

### Communication Protocol

**Godot → WebView:**
```gdscript
# From PlaybackController.gd
func _on_position_changed(position_beats: float):
    webview.execute_js("updateCursorPosition(%f)" % position_beats)

func _on_display_settings_changed(settings: Dictionary):
    var json = JSON.stringify(settings)
    webview.execute_js("updateDisplaySettings(%s)" % json)
```

**WebView → Godot:**
```javascript
// From score.js
function onNoteClicked(beatPosition) {
    // Call back to Godot
    window.godotCallback("seek", { position: beatPosition });
}

function onZoomChanged(zoomLevel) {
    window.godotCallback("zoom", { level: zoomLevel });
}
```

---

## Part 2: Playback Control Architecture

### Existing Infrastructure (MidiPlayer.gd)

The Godot MIDI Player addon provides excellent foundations:

| Feature | Current Status | API |
|---------|----------------|-----|
| **Tempo Control** | ✅ Implemented | `play_speed: float (0.0-100.0)` |
| **Transposition** | ✅ Implemented | `key_shift: int` |
| **Looping** | ✅ Basic | `loop: bool`, `loop_start: float` |
| **Per-Channel Volume** | ✅ Implemented | `channel.volume: float` |
| **Per-Channel Mute** | ✅ Implemented | `channel.mute: bool` |
| **Per-Channel Pan** | ✅ Implemented | `channel.pan: float` |
| **Reverb/Chorus** | ✅ Implemented | Per-channel effects |
| **Seek** | ✅ Implemented | `seek(position: float)` |
| **SoundFont** | ✅ Implemented | `soundfont: String` |
| **Polyphony** | ✅ 96 voices | `max_polyphony: int` |

### Missing Features to Implement

| Feature | Priority | Effort | Notes |
|---------|----------|--------|-------|
| **Loop End Point (B)** | P1 | 4-8 hrs | A-B loop completion |
| **Visual Timeline** | P1 | 8-16 hrs | Waveform/bar visualization |
| **Chord Playback Toggle** | P1 | 4-8 hrs | Mute chord channel |
| **Instrument Selection** | P2 | 8-12 hrs | Change melody instrument |
| **Solo Mode** | P2 | 2-4 hrs | Solo single channel |
| **Lilt/Humanization** | P2 | 24-40 hrs | See EXPLORE_003 |
| **Accompaniment Generation** | P3 | 40-80 hrs | See EXPLORE_003 |

### Proposed PlaybackController API

```gdscript
class_name PlaybackController
extends Node

## Signals
signal position_changed(position_beats: float, position_seconds: float)
signal loop_triggered(loop_count: int)
signal tempo_changed(bpm: float)
signal playback_finished()

## Core playback
var midi_player: MidiPlayer
var current_position: float = 0.0
var duration: float = 0.0

## Tempo
@export_range(0.25, 2.0) var tempo_multiplier: float = 1.0
var original_bpm: float = 120.0

## Looping (A-B)
var loop_enabled: bool = false
var loop_start: float = 0.0   # Point A (beats)
var loop_end: float = -1.0    # Point B (beats), -1 = end of tune
var loop_count: int = 0

## Channel control
var channel_config: Array[ChannelConfig] = []

class ChannelConfig:
    var name: String
    var enabled: bool = true
    var volume: float = 1.0
    var pan: float = 0.5
    var solo: bool = false
    var instrument_id: int = 0

## Methods
func play(from_position: float = 0.0) -> void
func pause() -> void
func stop() -> void
func seek(position_beats: float) -> void
func set_loop_points(start: float, end: float) -> void
func clear_loop() -> void

## Channel control
func set_channel_volume(channel: int, volume: float) -> void
func set_channel_mute(channel: int, muted: bool) -> void
func set_channel_solo(channel: int, solo: bool) -> void
func set_channel_instrument(channel: int, instrument_id: int) -> void
```

---

## Part 3: A-B Loop Implementation

### Visual Design

```
┌─────────────────────────────────────────────────────────────────┐
│  Timeline                                                        │
│                                                                  │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │    A              ●                    B                 │    │
│  │    ▼              ▼                    ▼                 │    │
│  │    ├──────────────┼────────────────────┤                │    │
│  │    │▓▓▓▓▓▓▓▓▓▓▓▓▓▓│▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓│                │    │
│  │────┴──────────────┴────────────────────┴────────────────│    │
│  │ 0:00             0:12                 0:24         0:36 │    │
│  └─────────────────────────────────────────────────────────┘    │
│                                                                  │
│  [Set A]  [Set B]  [Clear]     Loop: 3 / ∞     [Snap to Bar]   │
│                                                                  │
│  Options:                                                        │
│  [✓] Count-in (1 bar before A)                                  │
│  [ ] Auto-advance tempo (+5% each loop)                         │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### Implementation

```gdscript
# In PlaybackController.gd

func _process(delta: float) -> void:
    if not midi_player.playing:
        return

    current_position = midi_player.position
    emit_signal("position_changed", current_position, _beats_to_seconds(current_position))

    # Check for loop point B
    if loop_enabled and loop_end > 0:
        if current_position >= loop_end:
            loop_count += 1
            emit_signal("loop_triggered", loop_count)

            # Handle auto-advance tempo
            if auto_advance_tempo:
                tempo_multiplier = min(tempo_multiplier + 0.05, target_tempo)

            # Seek back to A (with optional count-in)
            var seek_position = loop_start
            if count_in_enabled:
                seek_position = max(0, loop_start - _bars_to_beats(1))

            midi_player.seek(seek_position)

func set_loop_points(start_beats: float, end_beats: float) -> void:
    loop_start = start_beats
    loop_end = end_beats
    loop_enabled = true
    loop_count = 0

    # Update MIDI player's native loop
    midi_player.loop = true
    midi_player.loop_start = loop_start

func snap_to_bar(position_beats: float) -> float:
    # Snap position to nearest bar line
    var beats_per_bar = _get_beats_per_bar()
    return round(position_beats / beats_per_bar) * beats_per_bar
```

---

## Part 4: Instrument & Mix Control

### Channel Architecture

For TunePal, we'll use a semantic channel mapping:

| Channel | Purpose | Default Instrument | Default State |
|---------|---------|-------------------|---------------|
| 0 | **Melody** | Tin Whistle (Recorder, #74) | Enabled, 100% |
| 1 | **Guitar Chords** | Acoustic Guitar (#25) | Enabled, 70% |
| 2 | **Bass** | Acoustic Bass (#33) | Disabled |
| 9 | **Bodhrán/Percussion** | Standard Kit | Disabled |
| 3-8, 10-15 | Reserved for future | Various | Disabled |

### Mixer UI Design

```
┌─────────────────────────────────────────────────────────────────┐
│  Mixer                                              [Presets ▼] │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐            │
│  │    █    │  │    █    │  │    ░    │  │    ░    │  Master    │
│  │    █    │  │    ▓    │  │    ░    │  │    ░    │    █       │
│  │    █    │  │    ▓    │  │    ░    │  │    ░    │    █       │
│  │    ▓    │  │    ░    │  │    ░    │  │    ░    │    █       │
│  │    ▓    │  │    ░    │  │    ░    │  │    ░    │    ▓       │
│  │    ░    │  │    ░    │  │    ░    │  │    ░    │    ░       │
│  │  ──●──  │  │  ──●──  │  │  ──●──  │  │  ──●──  │  ──●──     │
│  │   100%  │  │   70%   │  │   0%    │  │   0%    │   85%      │
│  │         │  │         │  │         │  │         │            │
│  │  [S][M] │  │  [S][M] │  │  [S][M] │  │  [S][M] │            │
│  │         │  │         │  │         │  │         │            │
│  │ Melody  │  │ Chords  │  │  Bass   │  │ Bodhrán │            │
│  │[Whistle▼]│  │[Guitar▼]│  │ [Bass▼] │  │  [Kit▼] │            │
│  └─────────┘  └─────────┘  └─────────┘  └─────────┘            │
│                                                                  │
│  Presets: [Melody Only] [Duo] [Full Session] [Custom]          │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### Mixer Presets

```gdscript
const MIXER_PRESETS = {
    "melody_only": {
        "melody": { "enabled": true, "volume": 1.0 },
        "chords": { "enabled": false, "volume": 0.0 },
        "bass": { "enabled": false, "volume": 0.0 },
        "percussion": { "enabled": false, "volume": 0.0 }
    },
    "duo": {
        "melody": { "enabled": true, "volume": 1.0 },
        "chords": { "enabled": true, "volume": 0.7 },
        "bass": { "enabled": false, "volume": 0.0 },
        "percussion": { "enabled": false, "volume": 0.0 }
    },
    "full_session": {
        "melody": { "enabled": true, "volume": 0.9 },
        "chords": { "enabled": true, "volume": 0.6 },
        "bass": { "enabled": true, "volume": 0.5 },
        "percussion": { "enabled": true, "volume": 0.4 }
    }
}
```

### Instrument Selection

The user should be able to change the melody instrument:

```gdscript
const MELODY_INSTRUMENTS = {
    "Tin Whistle": 74,      # Recorder
    "Irish Flute": 73,      # Flute
    "Fiddle": 40,           # Violin
    "Uilleann Pipes": 109,  # Bagpipe
    "Concertina": 21,       # Accordion
    "Banjo": 105,           # Banjo
    "Mandolin": 105,        # Banjo (closest)
    "Piano": 0,             # Acoustic Grand Piano
}

const ACCOMPANIMENT_INSTRUMENTS = {
    "Acoustic Guitar": 25,
    "Bouzouki": 25,         # Steel-string acoustic
    "Piano": 0,
    "Harp": 46,
}
```

---

## Part 5: High-Quality Instruments (SoundFonts)

### Current State

The MidiPlayer supports SoundFonts:
```gdscript
@export_file ("*.sf2") var soundfont:String = ""
```

But no SoundFont is currently bundled with TunePal.

### SoundFont Options

| Option | Size | Quality | License | Irish Instruments |
|--------|------|---------|---------|-------------------|
| **FluidR3_GM** | ~140MB | Good | MIT | Basic |
| **MuseScore_General** | ~40MB | Good | MIT | Basic |
| **Folk Music Instruments** | ~543MB | High | Commercial | Excellent |
| **Sonatina Symphonic** | ~500MB | High | CC-BY | Orchestral |
| **Custom Curated** | ~20-50MB | High | Mixed | Targeted |

### Recommendation: Tiered Approach

**Tier 1: Bundled (Small, ~10-20MB)**
- Curated subset of FluidR3_GM or MuseScore_General
- Focus on: Recorder, Flute, Violin, Guitar, Piano, Basic Drums
- Ships with app

**Tier 2: Downloadable (Optional, ~50-100MB)**
- Higher quality versions
- Additional Irish instruments
- Downloaded on first use or user request

**Tier 3: User Custom**
- Allow users to load their own SF2 files
- Advanced feature for musicians with preferences

### SoundFont Manager

```gdscript
class_name SoundFontManager
extends Node

enum SoundFontTier { BUNDLED, DOWNLOADABLE, CUSTOM }

var current_soundfont: String = ""
var available_soundfonts: Dictionary = {}

signal download_progress(percent: float)
signal soundfont_ready(path: String)

func _ready():
    # Register bundled soundfont
    available_soundfonts["basic"] = {
        "path": "res://assets/soundfonts/tunepal_basic.sf2",
        "tier": SoundFontTier.BUNDLED,
        "size_mb": 15,
        "description": "Basic instruments (bundled)"
    }

    # Register downloadable soundfonts
    available_soundfonts["hq_folk"] = {
        "url": "https://tunepal.org/assets/soundfonts/folk_hq.sf2",
        "tier": SoundFontTier.DOWNLOADABLE,
        "size_mb": 80,
        "description": "High-quality folk instruments"
    }

func load_soundfont(name: String) -> void:
    var sf_info = available_soundfonts.get(name)
    if sf_info == null:
        push_error("Unknown soundfont: " + name)
        return

    match sf_info.tier:
        SoundFontTier.BUNDLED:
            _load_from_path(sf_info.path)
        SoundFontTier.DOWNLOADABLE:
            await _download_and_load(sf_info)
        SoundFontTier.CUSTOM:
            _load_from_path(sf_info.path)

func _load_from_path(path: String) -> void:
    midi_player.soundfont = path
    current_soundfont = path
    emit_signal("soundfont_ready", path)
```

---

## Part 6: Chord Display & Playback

### ABC Chord Handling

ABC notation includes chords inline:
```abc
|: "G"GABc "D"d2 ef | "Em"g2 fe "D"d2 cB |
```

### Two Independent Controls

1. **Display chords in score** - Visual toggle
2. **Play chords** - Audio toggle (mute chord channel)

These should be **independent** - user might want to see chords but not hear them (to practice adding their own accompaniment).

### Implementation

```gdscript
# In ScoreSettings
var show_chords: bool = true      # Visual display
var play_chords: bool = true      # Audio playback

func _on_show_chords_toggled(enabled: bool):
    show_chords = enabled
    # Update ABCJS rendering
    webview.execute_js("setChordVisibility(%s)" % str(enabled).to_lower())

func _on_play_chords_toggled(enabled: bool):
    play_chords = enabled
    # Mute/unmute chord channel
    playback_controller.set_channel_mute(CHANNEL_CHORDS, not enabled)
```

### ABCJS Chord Display Control

```javascript
function setChordVisibility(visible) {
    if (visible) {
        // Re-render with chords
        ABCJS.renderAbc("score", abcString, {
            ...baseParams,
            // Chords enabled
        });
    } else {
        // Re-render with %%gchordfont none
        const abcWithoutChords = "%%gchordfont none\n" + abcString;
        ABCJS.renderAbc("score", abcWithoutChords, baseParams);
    }
}
```

---

## Part 7: Expandability Architecture

### Plugin/Module System

Design the playback system as modular components that can be extended:

```
PlaybackSystem/
├── Core/
│   ├── PlaybackController.gd      # Main controller
│   ├── MidiPlayer (addon)         # MIDI synthesis
│   └── TimelineManager.gd         # Position/sync
│
├── Visualization/
│   ├── ScoreRenderer.gd           # ABCJS WebView bridge
│   ├── TimelineView.gd            # Visual timeline
│   └── MixerView.gd               # Channel mixer UI
│
├── Features/
│   ├── LoopController.gd          # A-B loop logic
│   ├── TempoController.gd         # Tempo + practice mode
│   └── TransposeController.gd     # Key shifting
│
├── Advanced/ (Future)
│   ├── LiltEngine.gd              # Humanization
│   ├── AccompanimentGenerator.gd  # Dynamic backing
│   └── MidiRouter.gd              # iOS inter-app audio
│
└── Assets/
    ├── SoundFonts/
    └── Presets/
```

### Extension Points

| Extension Point | Purpose | Future Features |
|-----------------|---------|-----------------|
| `_on_note_played` | Hook into note events | Lilt, visualization |
| `_on_beat` | Beat synchronization | Metronome, light show |
| `_on_chord_change` | Chord progression | Theory hints, suggestions |
| `_process_abc` | Pre-process ABC | Auto-chord inference |
| `_render_custom` | Custom rendering | Tablature, notation styles |

### Signal Bus

```gdscript
# PlaybackEventBus.gd - Central signal routing
extends Node

signal playback_started()
signal playback_stopped()
signal playback_paused()
signal position_changed(beats: float, seconds: float)
signal tempo_changed(bpm: float)
signal note_on(channel: int, note: int, velocity: int)
signal note_off(channel: int, note: int)
signal chord_changed(chord: String)
signal measure_changed(measure: int)
signal loop_triggered(count: int)

# Any component can connect to these signals
# Allows loose coupling and easy extension
```

---

## Part 8: Implementation Phases

### Phase 1: Foundation (40-60 hours)

**Goal:** Basic playback with score visualization

| Task | Hours | Dependencies |
|------|-------|--------------|
| WebView integration (godot-webview) | 8-12 | None |
| ABCJS integration | 8-12 | WebView |
| Score ↔ Playback sync bridge | 8-12 | ABCJS |
| Basic playback UI | 8-12 | Sync bridge |
| Tempo control UI | 4-8 | Playback UI |
| Testing & polish | 4-8 | All above |

**Deliverables:**
- Score renders from ABC
- Playback works with cursor following
- Tempo can be adjusted
- Click on score seeks playback

### Phase 2: Looping & Mix (30-45 hours)

**Goal:** A-B loop and channel control

| Task | Hours | Dependencies |
|------|-------|--------------|
| A-B loop implementation | 8-12 | Phase 1 |
| Visual timeline with markers | 8-12 | Phase 1 |
| Per-channel mixer UI | 8-12 | Phase 1 |
| Chord display toggle | 4-6 | ABCJS |
| Chord playback toggle | 2-4 | Mixer |
| Testing & polish | 4-8 | All above |

**Deliverables:**
- Set A and B points visually
- Loop plays between points
- Mix individual channels
- Toggle chord visibility/playback

### Phase 3: Quality & Polish (25-40 hours)

**Goal:** High-quality instruments and presets

| Task | Hours | Dependencies |
|------|-------|--------------|
| SoundFont selection/bundling | 8-12 | None |
| SoundFont download system | 8-12 | None |
| Instrument selection UI | 4-8 | Mixer |
| Mixer presets | 4-6 | Mixer |
| Transposition UI | 4-6 | Phase 1 |
| Testing & polish | 4-8 | All above |

**Deliverables:**
- Better instrument sounds
- Change melody instrument
- Save/load mixer presets
- Transpose for different keys

### Phase 4: Advanced Features (60-100+ hours)

**Goal:** Differentiation features (optional)

| Task | Hours | Dependencies |
|------|-------|--------------|
| Lilt/humanization engine | 24-40 | Phase 1 |
| Accompaniment generation | 40-60 | Phase 2 |
| iOS MIDI routing | 36-54 | Phase 1 |
| Practice mode (auto-accelerate) | 4-8 | Phase 2 |
| Advanced score features | 20-30 | Phase 1 |

**Deliverables:**
- Playback sounds more human/Irish
- Auto-generate backing tracks
- Route MIDI to other iOS apps
- Graduated practice modes

---

## Part 9: Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| WebView performance on mobile | Medium | High | Test early, have fallback plan |
| ABCJS sync latency | Low | Medium | Buffer/predict cursor position |
| SoundFont file size too large | Medium | Medium | Tiered approach, compression |
| iOS WebView restrictions | Medium | High | Research WKWebView early |
| Cross-platform consistency | High | Medium | Test on all platforms often |

---

## Part 10: Success Metrics

| Metric | Target | Measurement |
|--------|--------|-------------|
| **Score render time** | <500ms | Time from ABC to visible score |
| **Sync accuracy** | <50ms | Cursor position vs audio |
| **Loop precision** | <10ms | Time to loop back to A |
| **UI responsiveness** | 60fps | No dropped frames during playback |
| **SoundFont quality** | Subjective | Musician feedback |
| **Feature completeness** | 100% of MVP | Checklist verification |

---

## References

### Documentation Sources
- `docs/exploration/EXPLORE_003_LEARNING_AND_PLAYBACK.md` - Detailed feature exploration
- `ROADMAP.md` - Bryan Duggan's vision
- `TunepalGodot/addons/midi/MidiPlayer.gd` - Existing MIDI infrastructure

### External Resources
- [ABCJS Documentation](https://paulrosen.github.io/abcjs/) - Score rendering library
- [ABCJS Interactive Configurator](https://configurator.abcjs.net/interactive/) - Test configurations
- [godot-webview](https://godotwebview.com/) - WebView addon for Godot
- [Godot WRY](https://github.com/doceazedo/godot_wry) - Cross-platform WebView
- [gdcef](https://github.com/Lecrapouille/gdcef) - Chromium Embedded Framework for Godot
- [Musical Artifacts SF2](https://musical-artifacts.com/artifacts?formats=sf2) - SoundFont repository
- [Soundfonts 4U](https://sites.google.com/site/soundfonts4u/) - High-quality SoundFonts

---

## Appendix A: ABC → MIDI Channel Mapping

When converting ABC to MIDI for multi-channel playback:

```
ABC Input:
|: "G"GABc "D"d2 ef | "Em"g2 fe "D"d2 cB |

MIDI Output:
Channel 0 (Melody): G A B c d d e f | g g f e d d c B |
Channel 1 (Chords): [G,B,D] - - - [D,F#,A] - - - | [E,G,B] - - - [D,F#,A] - - - |
Channel 2 (Bass):   G - - - D - - - | E - - - D - - - |
Channel 9 (Drums):  Per tune-type pattern
```

---

## Appendix B: Tune Type Detection

For accompaniment generation, detect tune type from ABC:

```gdscript
func detect_tune_type(abc: String) -> String:
    # Check M: (meter) field
    var meter_match = regex_meter.search(abc)
    if meter_match:
        var meter = meter_match.get_string(1)
        match meter:
            "6/8": return "jig"
            "9/8": return "slip_jig"
            "4/4", "C":
                # Could be reel or hornpipe - check R: field
                var rhythm_match = regex_rhythm.search(abc)
                if rhythm_match:
                    return rhythm_match.get_string(1).to_lower()
                return "reel"  # Default 4/4 to reel
            "2/4": return "polka"
            "3/4": return "waltz"
    return "unknown"
```

---

## Appendix C: Display Settings Schema

```gdscript
const DEFAULT_DISPLAY_SETTINGS = {
    "score": {
        "zoom": 1.0,
        "show_chords": true,
        "show_lyrics": true,
        "show_fingerings": false,
        "show_tablature": false,
        "note_color": "#000000",
        "highlight_color": "#4A90D9"
    },
    "playback": {
        "show_cursor": true,
        "scroll_follow": true,
        "measures_ahead": 2
    },
    "timeline": {
        "show_waveform": false,
        "show_measures": true,
        "show_loop_markers": true
    }
}
```
