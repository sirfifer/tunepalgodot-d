# Plan 002 Addendum: General MIDI & iOS Audio Integration

**Date:** December 9, 2025
**Parent Document:** PLAN_002_PLAYBACK_FEATURES.md
**Status:** Planning
**Priority:** P2-P3 (after core playback)

---

## Overview

This addendum expands on the iOS audio integration and General MIDI aspects of the playback plan, providing deeper technical detail for implementation.

---

## Part A: General MIDI Deep Dive

### What is General MIDI?

General MIDI (GM) is a standardized specification that ensures MIDI files play back consistently across different synthesizers and software. Without GM, a MIDI file created on one device might sound completely different on another.

### GM Requirements

| Requirement | Specification |
|-------------|---------------|
| Minimum polyphony | 24 voices (16 melody + 8 percussion) |
| Channels | 16 (Channel 10 reserved for percussion) |
| Instruments | 128 standard programs |
| Drum sounds | 47 percussion instruments (notes 35-81) |
| Controllers | Volume, Pan, Expression, Sustain, etc. |

### The 128 GM Instruments (Program Numbers 0-127)

Instruments are organized into 16 families of 8 instruments each:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ GENERAL MIDI INSTRUMENT MAP                                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│ PIANO (0-7)                      │ CHROMATIC PERCUSSION (8-15)              │
│ 0  Acoustic Grand Piano          │ 8  Celesta                               │
│ 1  Bright Acoustic Piano         │ 9  Glockenspiel                          │
│ 2  Electric Grand Piano          │ 10 Music Box                             │
│ 3  Honky-tonk Piano              │ 11 Vibraphone                            │
│ 4  Electric Piano 1              │ 12 Marimba                               │
│ 5  Electric Piano 2              │ 13 Xylophone                             │
│ 6  Harpsichord                   │ 14 Tubular Bells                         │
│ 7  Clavinet                      │ 15 Dulcimer                              │
│                                  │                                          │
│ ORGAN (16-23)                    │ GUITAR (24-31)                           │
│ 16 Drawbar Organ                 │ 24 Acoustic Guitar (nylon)               │
│ 17 Percussive Organ              │ 25 Acoustic Guitar (steel)   ◄─ Bouzouki │
│ 18 Rock Organ                    │ 26 Electric Guitar (jazz)                │
│ 19 Church Organ                  │ 27 Electric Guitar (clean)               │
│ 20 Reed Organ                    │ 28 Electric Guitar (muted)               │
│ 21 Accordion                     │ 29 Overdriven Guitar                     │
│ 22 Harmonica                     │ 30 Distortion Guitar                     │
│ 23 Tango Accordion               │ 31 Guitar Harmonics                      │
│                                  │                                          │
│ BASS (32-39)                     │ STRINGS (40-47)                          │
│ 32 Acoustic Bass                 │ 40 Violin                    ◄─ Fiddle   │
│ 33 Electric Bass (finger)        │ 41 Viola                                 │
│ 34 Electric Bass (pick)          │ 42 Cello                                 │
│ 35 Fretless Bass                 │ 43 Contrabass                            │
│ 36 Slap Bass 1                   │ 44 Tremolo Strings                       │
│ 37 Slap Bass 2                   │ 45 Pizzicato Strings                     │
│ 38 Synth Bass 1                  │ 46 Orchestral Harp           ◄─ Irish Harp│
│ 39 Synth Bass 2                  │ 47 Timpani                               │
│                                  │                                          │
│ ENSEMBLE (48-55)                 │ BRASS (56-63)                            │
│ 48 String Ensemble 1             │ 56 Trumpet                               │
│ 49 String Ensemble 2             │ 57 Trombone                              │
│ 50 Synth Strings 1               │ 58 Tuba                                  │
│ 51 Synth Strings 2               │ 59 Muted Trumpet                         │
│ 52 Choir Aahs                    │ 60 French Horn                           │
│ 53 Voice Oohs                    │ 61 Brass Section                         │
│ 54 Synth Voice                   │ 62 Synth Brass 1                         │
│ 55 Orchestra Hit                 │ 63 Synth Brass 2                         │
│                                  │                                          │
│ REED (64-71)                     │ PIPE (72-79)                             │
│ 64 Soprano Sax                   │ 72 Piccolo                               │
│ 65 Alto Sax                      │ 73 Flute                     ◄─ Irish Flute│
│ 66 Tenor Sax                     │ 74 Recorder                  ◄─ Tin Whistle│
│ 67 Baritone Sax                  │ 75 Pan Flute                             │
│ 68 Oboe                          │ 76 Blown Bottle                          │
│ 69 English Horn                  │ 77 Shakuhachi                            │
│ 70 Bassoon                       │ 78 Whistle                               │
│ 71 Clarinet                      │ 79 Ocarina                               │
│                                  │                                          │
│ SYNTH LEAD (80-87)               │ SYNTH PAD (88-95)                        │
│ 80 Lead 1 (square)               │ 88 Pad 1 (new age)                       │
│ 81 Lead 2 (sawtooth)             │ 89 Pad 2 (warm)                          │
│ 82 Lead 3 (calliope)             │ 90 Pad 3 (polysynth)                     │
│ 83 Lead 4 (chiff)                │ 91 Pad 4 (choir)                         │
│ 84 Lead 5 (charang)              │ 92 Pad 5 (bowed)                         │
│ 85 Lead 6 (voice)                │ 93 Pad 6 (metallic)                      │
│ 86 Lead 7 (fifths)               │ 94 Pad 7 (halo)                          │
│ 87 Lead 8 (bass + lead)          │ 95 Pad 8 (sweep)                         │
│                                  │                                          │
│ SYNTH EFFECTS (96-103)           │ ETHNIC (104-111)                         │
│ 96  FX 1 (rain)                  │ 104 Sitar                                │
│ 97  FX 2 (soundtrack)            │ 105 Banjo                    ◄─ Banjo    │
│ 98  FX 3 (crystal)               │ 106 Shamisen                             │
│ 99  FX 4 (atmosphere)            │ 107 Koto                                 │
│ 100 FX 5 (brightness)            │ 108 Kalimba                              │
│ 101 FX 6 (goblins)               │ 109 Bagpipe                  ◄─ Uilleann │
│ 102 FX 7 (echoes)                │ 110 Fiddle                   ◄─ Alt Fiddle│
│ 103 FX 8 (sci-fi)                │ 111 Shanai                               │
│                                  │                                          │
│ PERCUSSIVE (112-119)             │ SOUND EFFECTS (120-127)                  │
│ 112 Tinkle Bell                  │ 120 Guitar Fret Noise                    │
│ 113 Agogo                        │ 121 Breath Noise                         │
│ 114 Steel Drums                  │ 122 Seashore                             │
│ 115 Woodblock                    │ 123 Bird Tweet                           │
│ 116 Taiko Drum                   │ 124 Telephone Ring                       │
│ 117 Melodic Tom                  │ 125 Helicopter                           │
│ 118 Synth Drum                   │ 126 Applause                             │
│ 119 Reverse Cymbal               │ 127 Gunshot                              │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Irish Music Instrument Mapping

| Irish Instrument | Best GM Match | Program # | Notes |
|------------------|---------------|-----------|-------|
| **Tin Whistle** | Recorder | 74 | Good match for high whistle |
| **Low Whistle** | Flute | 73 | Better for low D whistle |
| **Irish Flute** | Flute | 73 | Direct match |
| **Fiddle** | Violin | 40 | Direct match |
| **Fiddle (alt)** | Fiddle (Ethnic) | 110 | More "folk" character |
| **Uilleann Pipes** | Bagpipe | 109 | Closest available |
| **Concertina** | Accordion | 21 | Similar sound family |
| **Bodhrán** | Channel 10 | N/A | Use percussion channel |
| **Bones** | Channel 10 | N/A | Woodblock or claves |
| **Bouzouki** | Steel Guitar | 25 | Good for strumming |
| **Guitar** | Nylon Guitar | 24 | Acoustic accompaniment |
| **Irish Harp** | Orchestral Harp | 46 | Direct match |
| **Banjo** | Banjo | 105 | Direct match |
| **Mandolin** | Banjo | 105 | Closest (no mandolin in GM) |

### Percussion Channel (Channel 10) Drum Map

For bodhrán and bones accompaniment:

```
┌─────────────────────────────────────────────────────────────────┐
│ GM DRUM MAP (Channel 10) - Notes 35-81                         │
├─────────────────────────────────────────────────────────────────┤
│ Note │ Instrument          │ TunePal Use                       │
├──────┼─────────────────────┼───────────────────────────────────┤
│  35  │ Acoustic Bass Drum  │ Bodhrán: deep "DUM"               │
│  36  │ Bass Drum 1         │ Bodhrán: medium "DUM"             │
│  38  │ Acoustic Snare      │ Bodhrán: "dum" (rim)              │
│  39  │ Hand Clap           │ Bones: accent                     │
│  42  │ Closed Hi-Hat       │ High whistle ornament click       │
│  44  │ Pedal Hi-Hat        │ Bodhrán: "di" upstroke            │
│  46  │ Open Hi-Hat         │ Bodhrán: open tone                │
│  54  │ Tambourine          │ Bodhrán: jingle effect            │
│  56  │ Cowbell             │ Spoons                            │
│  60  │ Hi Bongo            │ Bodhrán: high "dum"               │
│  61  │ Low Bongo           │ Bodhrán: low "DUM"                │
│  75  │ Claves              │ Bones: click                      │
│  76  │ Hi Wood Block       │ Bones: high                       │
│  77  │ Low Wood Block      │ Bones: low                        │
└──────┴─────────────────────┴───────────────────────────────────┘
```

### Bodhrán Pattern MIDI Implementation

```gdscript
# Bodhrán patterns using GM percussion
const BODHRAN_SAMPLES = {
    "DUM": 36,        # Bass Drum 1 (downstroke, bass)
    "dum": 38,        # Snare (upstroke, rim)
    "di": 44,         # Pedal Hi-Hat (light touch)
    "tip": 60,        # Hi Bongo (finger tip)
}

const BODHRAN_PATTERNS = {
    "jig_6_8": [
        # Beat 1    2    3    4    5    6
        {"note": "DUM", "time": 0.0,   "vel": 100},
        {"note": "di",  "time": 0.33,  "vel": 70},
        {"note": "di",  "time": 0.67,  "vel": 75},
        {"note": "DUM", "time": 1.0,   "vel": 95},
        {"note": "di",  "time": 1.33,  "vel": 70},
        {"note": "di",  "time": 1.67,  "vel": 75},
    ],
    "reel_4_4": [
        {"note": "DUM", "time": 0.0,   "vel": 100},
        {"note": "dum", "time": 0.5,   "vel": 70},
        {"note": "DUM", "time": 1.0,   "vel": 85},
        {"note": "dum", "time": 1.5,   "vel": 70},
        {"note": "DUM", "time": 2.0,   "vel": 100},
        {"note": "dum", "time": 2.5,   "vel": 70},
        {"note": "DUM", "time": 3.0,   "vel": 85},
        {"note": "dum", "time": 3.5,   "vel": 70},
    ],
    "hornpipe_4_4_swing": [
        # Hornpipe has more swing/shuffle
        {"note": "DUM", "time": 0.0,   "vel": 100},
        {"note": "dum", "time": 0.6,   "vel": 65},  # Swung
        {"note": "DUM", "time": 1.0,   "vel": 90},
        {"note": "dum", "time": 1.6,   "vel": 65},
        {"note": "DUM", "time": 2.0,   "vel": 100},
        {"note": "dum", "time": 2.6,   "vel": 65},
        {"note": "DUM", "time": 3.0,   "vel": 90},
        {"note": "dum", "time": 3.6,   "vel": 65},
    ]
}

func generate_bodhran_midi(tune_type: String, bars: int) -> Array:
    var pattern = BODHRAN_PATTERNS.get(tune_type, BODHRAN_PATTERNS["reel_4_4"])
    var events = []
    var bar_length = _get_bar_length(tune_type)

    for bar in range(bars):
        for hit in pattern:
            var time = bar * bar_length + hit.time
            var note = BODHRAN_SAMPLES[hit.note]
            var vel = hit.vel
            events.append({
                "channel": 9,  # Percussion channel (0-indexed)
                "note": note,
                "velocity": vel,
                "time": time,
                "duration": 0.1
            })

    return events
```

---

## Part B: iOS Audio Integration - Detailed Architecture

### Technology Stack Overview

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         iOS AUDIO ECOSYSTEM                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                        HOST APPS                                     │   │
│  │   Loopy Pro    AUM    GarageBand    Cubasis    Logic Pro (iPad)     │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                    ▲                                         │
│                                    │                                         │
│  ┌─────────────────────────────────┼─────────────────────────────────────┐  │
│  │                         AUv3 PLUGINS                                   │  │
│  │   ┌─────────────┐  ┌─────────────┐  ┌─────────────┐                   │  │
│  │   │ Instruments │  │   Effects   │  │MIDI Effects │ ◄─ TunePal here? │  │
│  │   │ (generate   │  │ (process    │  │ (generate/  │                   │  │
│  │   │  audio)     │  │  audio)     │  │  transform  │                   │  │
│  │   │             │  │             │  │  MIDI)      │                   │  │
│  │   └─────────────┘  └─────────────┘  └─────────────┘                   │  │
│  └───────────────────────────────────────────────────────────────────────┘  │
│                                    ▲                                         │
│                                    │                                         │
│  ┌─────────────────────────────────┼─────────────────────────────────────┐  │
│  │                       CORE MIDI                                        │  │
│  │                                                                        │  │
│  │   Virtual Sources ◄──────────────── TunePal (simpler approach)        │  │
│  │   Virtual Destinations                                                 │  │
│  │   Network MIDI                                                         │  │
│  │   Bluetooth MIDI                                                       │  │
│  │                                                                        │  │
│  └────────────────────────────────────────────────────────────────────────┘  │
│                                    ▲                                         │
│                                    │                                         │
│  ┌─────────────────────────────────┼─────────────────────────────────────┐  │
│  │                     CORE AUDIO                                         │  │
│  │   Audio Units (AU)                                                     │  │
│  │   Audio Queues                                                         │  │
│  │   AVAudioEngine                                                        │  │
│  └────────────────────────────────────────────────────────────────────────┘  │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Implementation Levels

#### Level 1: Core MIDI Virtual Source (Recommended First)

The simplest integration - TunePal creates a virtual MIDI output port that other apps can receive.

**Pros:**
- Simplest to implement
- Works with any MIDI-capable app
- No App Store review complications
- Can be enabled/disabled by user

**Cons:**
- User must manually connect in receiving app
- No host transport sync
- No state saving with host project

**Implementation: Godot iOS Plugin**

```
TunePalMIDI/
├── tunepal_midi.gdip              # Plugin descriptor
├── tunepal_midi.gdextension       # Extension config
├── src/
│   ├── TunePalMIDI.swift          # Main Swift class
│   ├── MIDIManager.swift          # Core MIDI wrapper
│   └── TunePalMIDI-Bridging.h     # Obj-C bridge
└── project/
    └── tunepal_midi.xcodeproj
```

**MIDIManager.swift (Core MIDI Virtual Source):**

```swift
import CoreMIDI

class MIDIManager {
    private var midiClient: MIDIClientRef = 0
    private var midiSource: MIDIEndpointRef = 0
    private var isInitialized = false

    let portName = "TunePal Output"

    func initialize() -> Bool {
        guard !isInitialized else { return true }

        // Create MIDI client
        var status = MIDIClientCreate("TunePal" as CFString, nil, nil, &midiClient)
        guard status == noErr else {
            print("Failed to create MIDI client: \(status)")
            return false
        }

        // Create virtual source (output port visible to other apps)
        status = MIDISourceCreate(midiClient, portName as CFString, &midiSource)
        guard status == noErr else {
            print("Failed to create MIDI source: \(status)")
            return false
        }

        isInitialized = true
        print("MIDI initialized: \(portName)")
        return true
    }

    func sendNoteOn(channel: UInt8, note: UInt8, velocity: UInt8) {
        var packet = MIDIPacket()
        packet.timeStamp = 0
        packet.length = 3
        packet.data.0 = 0x90 | (channel & 0x0F)  // Note On + channel
        packet.data.1 = note & 0x7F
        packet.data.2 = velocity & 0x7F

        var packetList = MIDIPacketList(numPackets: 1, packet: packet)
        MIDIReceived(midiSource, &packetList)
    }

    func sendNoteOff(channel: UInt8, note: UInt8) {
        var packet = MIDIPacket()
        packet.timeStamp = 0
        packet.length = 3
        packet.data.0 = 0x80 | (channel & 0x0F)  // Note Off + channel
        packet.data.1 = note & 0x7F
        packet.data.2 = 0  // Release velocity

        var packetList = MIDIPacketList(numPackets: 1, packet: packet)
        MIDIReceived(midiSource, &packetList)
    }

    func sendProgramChange(channel: UInt8, program: UInt8) {
        var packet = MIDIPacket()
        packet.timeStamp = 0
        packet.length = 2
        packet.data.0 = 0xC0 | (channel & 0x0F)  // Program Change + channel
        packet.data.1 = program & 0x7F

        var packetList = MIDIPacketList(numPackets: 1, packet: packet)
        MIDIReceived(midiSource, &packetList)
    }

    func sendControlChange(channel: UInt8, controller: UInt8, value: UInt8) {
        var packet = MIDIPacket()
        packet.timeStamp = 0
        packet.length = 3
        packet.data.0 = 0xB0 | (channel & 0x0F)  // Control Change + channel
        packet.data.1 = controller & 0x7F
        packet.data.2 = value & 0x7F

        var packetList = MIDIPacketList(numPackets: 1, packet: packet)
        MIDIReceived(midiSource, &packetList)
    }

    func shutdown() {
        if midiSource != 0 {
            MIDIEndpointDispose(midiSource)
            midiSource = 0
        }
        if midiClient != 0 {
            MIDIClientDispose(midiClient)
            midiClient = 0
        }
        isInitialized = false
    }
}
```

**GDScript Interface:**

```gdscript
# TunePalMIDIOutput.gd
extends Node

var midi_plugin = null
var midi_enabled: bool = false

signal midi_status_changed(enabled: bool)

func _ready():
    if OS.get_name() == "iOS":
        if Engine.has_singleton("TunePalMIDI"):
            midi_plugin = Engine.get_singleton("TunePalMIDI")
            print("TunePal MIDI plugin loaded")

func enable_midi_output() -> bool:
    if midi_plugin == null:
        push_error("MIDI plugin not available")
        return false

    var result = midi_plugin.initialize()
    midi_enabled = result
    emit_signal("midi_status_changed", midi_enabled)
    return result

func disable_midi_output():
    if midi_plugin:
        midi_plugin.shutdown()
    midi_enabled = false
    emit_signal("midi_status_changed", midi_enabled)

func send_note_on(channel: int, note: int, velocity: int):
    if midi_enabled and midi_plugin:
        midi_plugin.sendNoteOn(channel, note, velocity)

func send_note_off(channel: int, note: int):
    if midi_enabled and midi_plugin:
        midi_plugin.sendNoteOff(channel, note)

func send_program_change(channel: int, program: int):
    if midi_enabled and midi_plugin:
        midi_plugin.sendProgramChange(channel, program)

func send_control_change(channel: int, controller: int, value: int):
    if midi_enabled and midi_plugin:
        midi_plugin.sendControlChange(channel, controller, value)
```

**Integration with MidiPlayer:**

```gdscript
# In PlaybackController.gd

var midi_output: TunePalMIDIOutput

func _ready():
    midi_output = TunePalMIDIOutput.new()
    add_child(midi_output)

    # Connect to MIDI player events
    midi_player.connect("midi_event", _on_midi_event)

func _on_midi_event(channel: MidiPlayer.GodotMIDIPlayerChannelStatus, event):
    if not midi_output.midi_enabled:
        return

    # Forward MIDI events to iOS MIDI output
    match event.type:
        SMF.MIDIEventType.note_on:
            midi_output.send_note_on(channel.number, event.note, event.velocity)
        SMF.MIDIEventType.note_off:
            midi_output.send_note_off(channel.number, event.note)
        SMF.MIDIEventType.program_change:
            midi_output.send_program_change(channel.number, event.number)
        SMF.MIDIEventType.control_change:
            midi_output.send_control_change(channel.number, event.number, event.value)
```

#### Level 2: AUv3 MIDI Generator Extension

More complex but provides tighter integration with host apps.

**What is an AUv3 Extension?**

An AUv3 (Audio Unit v3) is an app extension that can be loaded by host apps like AUM, Loopy Pro, or Logic Pro. For TunePal, we'd create a **MIDI Generator** type AU.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        AUv3 ARCHITECTURE                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│   ┌─────────────────────────────────────────────────────────────────────┐   │
│   │                    HOST APP (e.g., AUM)                              │   │
│   │                                                                      │   │
│   │   ┌───────────────────────────────────────────────────────────┐     │   │
│   │   │             TunePal AUv3 Extension                         │     │   │
│   │   │                                                            │     │   │
│   │   │   ┌─────────────────┐    ┌─────────────────┐              │     │   │
│   │   │   │   Tune Browser  │    │   Playback      │              │     │   │
│   │   │   │   (mini UI)     │───►│   Engine        │──► MIDI Out  │     │   │
│   │   │   │                 │    │                 │              │     │   │
│   │   │   └─────────────────┘    └─────────────────┘              │     │   │
│   │   │                                                            │     │   │
│   │   │   Parameters (exposed to host):                           │     │   │
│   │   │   - Tempo multiplier                                       │     │   │
│   │   │   - Transpose                                              │     │   │
│   │   │   - Play/Stop                                              │     │   │
│   │   │   - Loop A/B points                                        │     │   │
│   │   │                                                            │     │   │
│   │   └────────────────────────────────────────────────────────────┘     │   │
│   │                                                                      │   │
│   │   Host Features:                                                     │   │
│   │   - Transport sync (play/stop with host)                            │   │
│   │   - Tempo sync (follow host BPM)                                    │   │
│   │   - State save/restore (save with host project)                     │   │
│   │   - Parameter automation                                             │   │
│   │                                                                      │   │
│   └──────────────────────────────────────────────────────────────────────┘   │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

**AUv3 Extension Structure:**

```
TunePal.app/
├── Info.plist
├── TunePal (main app)
└── PlugIns/
    └── TunePalAU.appex/              # The AUv3 extension
        ├── Info.plist
        ├── TunePalAU                 # Extension binary
        └── TunePalAU.swift           # AU implementation
```

**Key AUv3 Components:**

```swift
// TunePalAudioUnit.swift
import AudioToolbox
import CoreAudioKit

public class TunePalAudioUnit: AUAudioUnit {

    // MIDI output
    private var midiOutputEventBlock: AUMIDIOutputEventBlock?

    // Parameters
    private var _tempo: AUParameter!
    private var _transpose: AUParameter!
    private var _isPlaying: AUParameter!

    // Playback state
    private var currentTune: TuneData?
    private var playbackPosition: Double = 0

    public override init(
        componentDescription: AudioComponentDescription,
        options: AudioComponentInstantiationOptions = []
    ) throws {
        try super.init(componentDescription: componentDescription, options: options)

        // Set up parameter tree
        let parameterTree = AUParameterTree.createTree(withChildren: [
            AUParameterTree.createParameter(
                withIdentifier: "tempo",
                name: "Tempo",
                address: 0,
                min: 0.25,
                max: 2.0,
                unit: .generic,
                unitName: nil,
                flags: [.flag_IsReadable, .flag_IsWritable],
                valueStrings: nil,
                dependentParameters: nil
            ),
            // ... more parameters
        ])

        self.parameterTree = parameterTree
    }

    // Called by host to render MIDI
    public override var internalRenderBlock: AUInternalRenderBlock {
        return { [weak self] actionFlags, timestamp, frameCount, outputBusNumber,
                  outputData, realtimeEventListHead, pullInputBlock in

            guard let self = self else { return noErr }

            // Generate MIDI events based on playback position
            if self._isPlaying.value > 0.5 {
                self.generateMIDIEvents(
                    frameCount: frameCount,
                    timestamp: timestamp
                )
            }

            return noErr
        }
    }

    private func generateMIDIEvents(frameCount: AUAudioFrameCount, timestamp: UnsafePointer<AudioTimeStamp>) {
        guard let outputBlock = midiOutputEventBlock else { return }

        // Calculate which notes should play in this render cycle
        // and call outputBlock for each MIDI event
    }
}
```

**Effort Estimate for AUv3:**

| Task | Hours |
|------|-------|
| AUv3 extension setup & boilerplate | 16-24 |
| Mini UI for tune selection | 16-24 |
| MIDI generation from playback | 8-12 |
| Parameter exposure (tempo, transpose) | 4-8 |
| Host transport sync | 8-12 |
| State save/restore | 8-12 |
| Testing with multiple hosts | 16-24 |
| App Store review prep | 8-16 |
| **Total** | **84-132 hours** |

### Swift Libraries for MIDI

Several libraries can simplify MIDI implementation:

| Library | License | Features | Link |
|---------|---------|----------|------|
| **[MIDIKit](https://github.com/orchetect/MIDIKit)** | MIT | Modern Swift, MIDI 1.0 & 2.0 | [GitHub](https://github.com/orchetect/MIDIKit) |
| **[MIKMIDI](https://github.com/mixedinkey-opensource/MIKMIDI)** | MIT | Obj-C compatible, mature | [GitHub](https://github.com/mixedinkey-opensource/MIKMIDI) |
| **[AudioKit](https://audiokit.io/)** | MIT | Full audio framework | [Website](https://audiokit.io/) |

**Recommendation:** Use **MIDIKit** for modern Swift codebase with clean API.

---

## Part C: Implementation Roadmap

### Phase 1: Core MIDI Output (Level 1)

**Goal:** TunePal can send MIDI to other iOS apps

| Step | Task | Hours |
|------|------|-------|
| 1 | Create iOS plugin project structure | 4 |
| 2 | Implement MIDIManager.swift with Core MIDI | 8-12 |
| 3 | Create GDScript bridge interface | 4-6 |
| 4 | Hook into MidiPlayer events | 4-6 |
| 5 | Add UI toggle for MIDI output | 2-4 |
| 6 | Test with Loopy Pro, AUM, GarageBand | 8-12 |
| 7 | Handle background/foreground transitions | 4-6 |
| **Total** | | **34-50 hours** |

### Phase 2: Enhanced MIDI Features

| Step | Task | Hours |
|------|------|-------|
| 1 | MIDI input (receive transport from host) | 8-12 |
| 2 | MIDI clock sync (follow external tempo) | 8-12 |
| 3 | MIDI channel routing UI | 4-6 |
| 4 | Program change on tune load | 2-4 |
| **Total** | | **22-34 hours** |

### Phase 3: AUv3 Extension (Optional)

| Step | Task | Hours |
|------|------|-------|
| 1 | AUv3 extension project setup | 8-12 |
| 2 | Audio Unit implementation | 16-24 |
| 3 | Mini tune browser UI | 16-24 |
| 4 | Parameter system | 8-12 |
| 5 | Host transport sync | 8-12 |
| 6 | State persistence | 8-12 |
| 7 | Multi-host testing | 16-24 |
| **Total** | | **80-120 hours** |

---

## Part D: User Experience Design

### MIDI Output Settings UI

```
┌─────────────────────────────────────────────────────────────────┐
│  Settings > MIDI Output                                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  MIDI Output                                                     │
│  ──────────────────────────────────────────────────────────────  │
│                                                                  │
│  [●] Enable MIDI Output                                         │
│                                                                  │
│  When enabled, TunePal will appear as "TunePal Output"          │
│  in other music apps like AUM, Loopy Pro, or GarageBand.        │
│                                                                  │
│  ──────────────────────────────────────────────────────────────  │
│                                                                  │
│  Channel Assignment                                              │
│                                                                  │
│  Melody:        [Channel 1 ▼]                                   │
│  Chords:        [Channel 2 ▼]                                   │
│  Bass:          [Channel 3 ▼]                                   │
│  Percussion:    [Channel 10 ▼] (Standard)                       │
│                                                                  │
│  ──────────────────────────────────────────────────────────────  │
│                                                                  │
│  [●] Send Program Change on tune load                           │
│  [ ] Send MIDI Clock                                            │
│  [ ] Follow external transport                                  │
│                                                                  │
│  ──────────────────────────────────────────────────────────────  │
│                                                                  │
│  Status: Connected to 2 destinations                            │
│    • Loopy Pro                                                   │
│    • AUM                                                         │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### Workflow Example: Practice with Loopy Pro

```
┌────────────────────────────────────────────────────────────────────┐
│  USER WORKFLOW: Practice session with loop recording              │
├────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  1. User opens TunePal, enables MIDI Output in settings            │
│                                                                     │
│  2. User opens Loopy Pro                                           │
│     - In Loopy, set track input to "TunePal Output"               │
│     - Set track to record MIDI (not audio)                         │
│                                                                     │
│  3. User searches for a tune in TunePal                            │
│     - Find "The Kesh Jig"                                          │
│     - Adjust tempo to 75% for learning                             │
│                                                                     │
│  4. User presses Play in TunePal                                   │
│     - MIDI flows to Loopy Pro                                      │
│     - Loopy Pro plays back using its instruments                   │
│     - User plays along on their fiddle/whistle                     │
│                                                                     │
│  5. User arms recording in Loopy Pro                               │
│     - TunePal keeps playing the tune                               │
│     - Loopy records both the MIDI tune AND their instrument        │
│     - Creates layered loop                                          │
│                                                                     │
│  6. User stops TunePal, Loopy keeps playing the loop               │
│     - Practice over the loop indefinitely                          │
│     - Gradually increase tempo                                      │
│                                                                     │
└────────────────────────────────────────────────────────────────────┘
```

---

## Part E: Testing Matrix

### Core MIDI Testing

| Host App | Free? | MIDI Input | Notes |
|----------|-------|------------|-------|
| **AUM** | No ($20) | ✅ | Industry standard, recommended |
| **Loopy Pro** | No ($30) | ✅ | Excellent for practice loops |
| **GarageBand** | Yes | ✅ | Widely available |
| **Cubasis** | No ($50) | ✅ | Full DAW |
| **MIDI Wrench** | Free | ✅ | Debug tool, essential |
| **Midiflow** | Free tier | ✅ | MIDI routing utility |

### Test Cases

| Test | Expected Result |
|------|-----------------|
| Enable MIDI output | "TunePal Output" appears in other apps |
| Play tune | Notes received in host app |
| Stop playback | Note offs sent, no hanging notes |
| Background TunePal | MIDI continues (with background audio entitlement) |
| Change tempo | Host receives notes at new timing |
| Program change | Host selects correct instrument |
| Multiple receivers | All connected apps receive MIDI |

---

## References

### General MIDI
- [General MIDI on Wikipedia](https://en.wikipedia.org/wiki/General_MIDI)
- [GM Instrument Map (Somascape)](http://www.somascape.org/midi/basic/gmins.html)
- [GM Program Numbers (PG Music)](https://www.pgmusic.com/tutorial_gm.htm)

### iOS Development
- [AudioKit AUv3 Tutorial](https://audiokitpro.com/auv3-midi-tutorial-part1/)
- [AudioKit iOS AUv3 Guide](https://audiokitpro.com/iosauv3tutorial/)
- [Apple Core MIDI Documentation](https://developer.apple.com/documentation/coremidi/midi-services)
- [Apple Audio Unit Documentation](https://developer.apple.com/documentation/audiotoolbox/audio_unit_v3_plug-ins/incorporating_audio_effects_and_instruments)
- [MIDIKit (Swift Library)](https://github.com/orchetect/MIDIKit)
- [MIKMIDI (Obj-C Library)](https://github.com/mixedinkey-opensource/MIKMIDI)

### Godot iOS
- [Creating iOS Plugins (Godot 4.4)](https://docs.godotengine.org/en/4.4/tutorials/platform/ios/ios_plugin.html)
- [Godot iOS Plugins Repository](https://github.com/godot-sdk-integrations/godot-ios-plugins)
- [MIDI Output Proposal #2321](https://github.com/godotengine/godot-proposals/issues/2321)

### Apps
- [AUM - Audio Mixer](https://apps.apple.com/app/id1055636344)
- [Loopy Pro](https://loopypro.com/)
- [Midiflow](https://apps.apple.com/us/app/midiflow/id879915554)
