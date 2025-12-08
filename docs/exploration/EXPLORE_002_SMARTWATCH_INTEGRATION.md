# Exploration 002: Secondary Device Support (Smartwatch Integration)

**Date:** December 8, 2025
**Status:** Proposal
**Priority:** High
**Decision:** Strongly Recommended

---

## Executive Summary

Adding smartwatch support to TunePal would enable musicians to **capture and identify tunes with a quick glance at their wrist** - no fumbling for a phone during a session. This is a compelling use case with clear user value.

**MVP Target:** Apple Watch
**Secondary Target:** Wear OS (Android watches)
**Potential Future:** Other wearables, dedicated capture devices

**Key Finding:** This is technically feasible. Both watchOS and Wear OS support audio recording with sufficient quality for TunePal's recognition algorithm. The main architectural decision is whether to process on-watch, on-phone, or directly to server.

---

## The Use Case

### The Problem

In a traditional music session:
- A tune starts that you don't know
- By the time you pull out your phone, unlock it, open TunePal, and hit record...
- ...the tune is half over or you've missed the characteristic opening phrase
- Holding up a phone in a session can feel intrusive or awkward

### The Solution

Glance at your watch → Tap once → Recording starts → Tune identified → Results on your wrist

**This is exactly how Shazam works on Apple Watch** - and it's wildly popular for good reason.

---

## Platform Analysis

### Apple Watch (watchOS) - MVP Target

| Capability | Status | Notes |
|------------|--------|-------|
| Microphone Access | ✅ Available | Built-in on all Apple Watch models |
| Audio Recording | ✅ Available | AVAudioRecorder, AVAudioEngine |
| Real-time Audio Buffers | ✅ Available | AVAudioInputNode with installTapOnBus |
| On-device FFT | ✅ Available | Accelerate framework vDSP |
| Network Access | ✅ Available | WiFi, Bluetooth, Cellular (some models) |
| Independent Apps | ✅ Available | Since watchOS 6 |
| Background Audio | ⚠️ Limited | Recording only in foreground |

#### Audio API Details

```swift
// Real-time audio buffer access is possible
let inputNode = audioEngine.inputNode
inputNode.installTap(onBus: 0, bufferSize: 2048, format: format) { buffer, time in
    // Process audio samples here
    // Can do pitch detection, FFT, etc.
}
```

**Key Constraints:**
- Recording only works when app is **in foreground**
- No AudioUnit or custom buffer duration control
- Battery drain with continuous microphone use
- Dedicated audio session required

#### Existing Watch Apps for Reference

| App | Function | Architecture |
|-----|----------|--------------|
| **Shazam** | Song recognition | Records on watch → Fingerprint → Server match |
| **SoundHound** | Song recognition + lyrics | Similar to Shazam |
| **Voice Memos** | Audio recording | Records to file on watch |

**Shazam proves the model works** for music recognition on Apple Watch.

---

### Wear OS (Android Watches) - Secondary Target

| Capability | Status | Notes |
|------------|--------|-------|
| Microphone Access | ✅ Available | Most Wear OS devices have mics |
| Audio Recording | ✅ Available | MediaRecorder, AudioRecord APIs |
| Real-time Audio Buffers | ✅ Available | AudioRecord with read() |
| Network Access | ✅ Available | WiFi, Bluetooth, Cellular (some models) |
| Independent Apps | ✅ Available | Wear OS 3+ |
| Background Audio | ⚠️ Limited | Similar constraints to watchOS |

#### Audio API Details

```kotlin
// MediaRecorder for simple recording
val recorder = MediaRecorder().apply {
    setAudioSource(MediaRecorder.AudioSource.MIC)
    setOutputFormat(MediaRecorder.OutputFormat.AAC_ADTS)
    setAudioEncoder(MediaRecorder.AudioEncoder.AAC)
    setAudioSamplingRate(44100)
}

// AudioRecord for real-time buffer access
val audioRecord = AudioRecord(
    MediaRecorder.AudioSource.MIC,
    44100,
    AudioFormat.CHANNEL_IN_MONO,
    AudioFormat.ENCODING_PCM_16BIT,
    bufferSize
)
```

**Key Constraints:**
- Device fragmentation (various watch manufacturers)
- Some devices have audio encoding bugs
- Battery efficiency varies by device
- 44100Hz is only guaranteed sample rate

---

## Godot Compatibility

### Current Status: No Native Support

Godot does **not** currently support Apple Watch (watchOS) or Wear OS as export targets. This means:

- **Cannot deploy Godot app directly to watch**
- **Must build native watch apps** in Swift/Kotlin
- Watch apps communicate with Godot phone app via platform bridges

### Architectural Implication

```
┌─────────────────────────────────────────────────────────────────┐
│                        WATCH (Native)                            │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │  Swift (watchOS) / Kotlin (Wear OS)                      │    │
│  │  - Audio recording                                        │    │
│  │  - Basic UI (tap to record, show results)                │    │
│  │  - Communication with phone                               │    │
│  └─────────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────┘
                              │
                    Watch Connectivity /
                    Wear Data Layer API
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                        PHONE (Godot)                             │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │  Native Plugin (Swift/Kotlin)                            │    │
│  │  - Receives audio/data from watch                        │    │
│  │  - Bridges to Godot via GDExtension                      │    │
│  └─────────────────────────────────────────────────────────┘    │
│                              │                                   │
│                              ▼                                   │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │  Godot TunePal App                                       │    │
│  │  - Audio processing                                       │    │
│  │  - Server communication                                   │    │
│  │  - Results display                                        │    │
│  └─────────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────┘
```

---

## TunePal Recognition Requirements

Understanding what TunePal needs helps determine watch architecture:

| Requirement | Value | Implication |
|-------------|-------|-------------|
| Audio Duration | ~12 seconds | Short recording, reasonable for watch |
| Audio Quality | Melody-focused | Watch mic quality is sufficient |
| Processing | STFT + Pitch Detection | Could be on-watch or off-watch |
| Matching | Edit distance against corpus | Requires server or large local DB |
| Instruments | Legato (whistle, flute, fiddle) | Watch mic can capture these |
| Noise Tolerance | High (designed for sessions) | Good fit for watch environment |

### Algorithm Overview (MATT2)

1. **Record** ~12 seconds of audio
2. **Transcribe** to ABC notation via STFT + pitch spelling
3. **Match** against corpus using edit distance
4. **Return** top 10 matches

Steps 1-2 could potentially happen on-watch. Step 3 requires server access.

---

## Architecture Options

### Option A: Watch → Phone → Server (Recommended for MVP)

```
Watch                    Phone                    Server
  │                        │                        │
  │  Record 12s audio      │                        │
  │  ───────────────►      │                        │
  │  Send audio file       │                        │
  │  ════════════════►     │                        │
  │                        │  Forward audio         │
  │                        │  ═══════════════════►  │
  │                        │                        │  Process
  │                        │       Results          │
  │                        │  ◄═══════════════════  │
  │      Results           │                        │
  │  ◄════════════════     │                        │
  │  Display on watch      │                        │
```

**Pros:**
- Simplest watch app (just record + display)
- Phone handles complex logic
- Works with existing TunePal infrastructure
- Battery efficient on watch

**Cons:**
- Requires phone nearby (for non-cellular watches)
- Two hops adds latency

### Option B: Watch → Server (Direct)

```
Watch                                          Server
  │                                              │
  │  Record 12s audio                            │
  │  Send directly via WiFi/Cellular             │
  │  ══════════════════════════════════════════► │
  │                                              │  Process
  │                        Results               │
  │  ◄══════════════════════════════════════════ │
  │  Display on watch                            │
```

**Pros:**
- Works without phone nearby
- Lower latency (one hop)
- True independence

**Cons:**
- Requires cellular watch OR known WiFi
- More complex watch app
- Phone app doesn't see results (unless synced later)

### Option C: On-Watch Processing + Server Match

```
Watch                                          Server
  │                                              │
  │  Record audio                                │
  │  Perform STFT + Pitch Detection              │
  │  Generate ABC-like query                     │
  │  Send compact query                          │
  │  ══════════════════════════════════════════► │
  │                                              │  Match query
  │                        Results               │
  │  ◄══════════════════════════════════════════ │
```

**Pros:**
- Minimal data transfer (send query, not audio)
- Faster server response
- Works on slow connections

**Cons:**
- Complex on-watch signal processing
- Battery drain from DSP operations
- Harder to debug/maintain

### Recommendation: Start with Option A

**Option A is the pragmatic MVP choice:**
- Minimal watch app complexity
- Leverages existing TunePal phone app
- Can evolve to Option B/C later
- Fastest time to market

---

## Implementation Plan

### Phase 1: Apple Watch MVP

**Scope:** Basic tune capture and identification from watch

| Component | Technology | Effort |
|-----------|------------|--------|
| watchOS App | Swift + SwiftUI | 20-40 hours |
| Watch Connectivity | WCSession | 8-16 hours |
| iOS Native Plugin | Swift | 16-24 hours |
| Godot Bridge | GDExtension | 8-16 hours |
| Testing & Polish | - | 16-24 hours |
| **Total** | | **68-120 hours** |

**Watch App Features:**
- Single-tap recording
- Recording progress indicator
- Results display (tune name, confidence)
- Complication for quick access
- History of recent identifications

**Phone Integration:**
- Receive audio from watch
- Process through existing TunePal flow
- Send results back to watch
- Sync history

### Phase 2: Wear OS Support

| Component | Technology | Effort |
|-----------|------------|--------|
| Wear OS App | Kotlin + Compose | 24-40 hours |
| Data Layer API | Wear OS SDK | 8-16 hours |
| Android Native Plugin | Kotlin | 16-24 hours |
| Testing & Polish | - | 16-24 hours |
| **Total** | | **64-104 hours** |

### Phase 3: Enhanced Features

- Direct server communication (Option B)
- On-watch pitch preview (Option C partial)
- Watch complications with tune-of-the-day
- Session mode (continuous listening with notifications)
- Offline queue (record now, identify later)

---

## Battery Considerations

### Apple Watch Battery Impact

| Activity | Impact | Mitigation |
|----------|--------|------------|
| 12s recording | Minimal | One-shot, not continuous |
| Bluetooth to phone | Low | Native, optimized |
| WiFi/Cellular | Moderate | Only when phone unavailable |
| Screen on during recording | Moderate | Short duration acceptable |

**Comparison:** Shazam on Apple Watch is widely used without battery complaints for occasional song identification. TunePal's use case is similar.

### Recommendations

1. **Don't** implement continuous listening (like Noise app)
2. **Do** make it tap-to-record only
3. **Do** prefer Bluetooth to phone over direct WiFi/Cellular
4. **Do** turn off screen after recording starts (optional)

---

## User Experience Design

### Watch Interface (MVP)

```
┌─────────────────────┐
│    🎵 TunePal       │
│                     │
│   ┌─────────────┐   │
│   │             │   │
│   │   🎤 TAP    │   │
│   │  TO RECORD  │   │
│   │             │   │
│   └─────────────┘   │
│                     │
│  Last: The Kesh     │
└─────────────────────┘

       ↓ (tap)

┌─────────────────────┐
│    🎵 TunePal       │
│                     │
│   ┌─────────────┐   │
│   │  🔴          │   │
│   │  Listening   │   │
│   │  ████░░░░░   │   │
│   │  8s          │   │
│   └─────────────┘   │
│                     │
│  Keep playing...    │
└─────────────────────┘

       ↓ (after ~12s)

┌─────────────────────┐
│    🎵 TunePal       │
│                     │
│   Found it! 🎉      │
│                     │
│   The Kesh Jig      │
│   (Jig, G Major)    │
│                     │
│   [View on Phone]   │
│   [Save]  [Again]   │
└─────────────────────┘
```

### Complication

A watch face complication allows one-tap access:
- Small: TunePal icon
- Tap: Opens directly to recording screen

---

## Risk Assessment

### Technical Risks

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| Watch mic quality insufficient | Low | High | Testing shows Shazam/SoundHound work fine |
| Watch Connectivity unreliable | Medium | Medium | Implement retry logic, offline queue |
| Battery drain complaints | Low | Medium | Use tap-to-record, not continuous |
| Godot bridge complexity | Medium | Medium | Start with simple data passing |
| App Store rejection | Low | High | Follow all Apple/Google guidelines |

### Platform Risks

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| watchOS API changes | Low | Medium | Use stable APIs, test with betas |
| Wear OS fragmentation | Medium | Low | Target recent Wear OS versions |
| Market size too small | Low | Low | Apple Watch has 50M+ users |

---

## Market Validation

### Comparable Apps

| App | Platform | Monthly Users | Rating |
|-----|----------|---------------|--------|
| Shazam | watchOS | Millions | 4.8★ |
| SoundHound | watchOS | Hundreds of thousands | 4.6★ |
| Voice Memos | watchOS | Built-in | N/A |

**Shazam proves there's strong demand for music recognition on watches.**

### TunePal's Unique Position

Unlike Shazam/SoundHound:
- TunePal recognizes **live playing**, not just recordings
- Works with **any instrument** playing the tune
- Designed for **noisy session environments**
- Returns **sheet music and ABC notation**, not just song ID

**This is genuinely differentiated** - no competitor offers traditional tune recognition on watch.

---

## Recommendation

### Verdict: STRONGLY RECOMMENDED

Secondary device support, starting with Apple Watch, should be a **high priority** for TunePal:

1. **Clear user value** - Capture tunes without phone fumbling
2. **Technically feasible** - Audio APIs support the use case
3. **Proven market** - Shazam validates watch music recognition
4. **Differentiated** - No competitor does traditional tune recognition on watch
5. **Reasonable effort** - MVP in 70-120 hours

### Suggested Roadmap

```
Q1: Apple Watch MVP (Option A architecture)
    - Tap to record
    - Send to phone
    - Display results on watch

Q2: Wear OS Support
    - Port watch app to Kotlin
    - Integrate with Android TunePal

Q3: Enhanced Independence
    - Direct server communication
    - Offline queue
    - Session mode

Q4: Advanced Features
    - On-watch pitch preview
    - Complications
    - Siri/Assistant integration
```

---

## Appendix: Technical Resources

### Apple Watch / watchOS

- [Apple Watch Audio Recording Documentation](https://developer.apple.com/documentation/watchkit/wkinterfacecontroller/audio_recording_options)
- [AVAudioEngine on watchOS](https://developer.apple.com/documentation/avfaudio/avaudioengine)
- [Watch Connectivity Framework](https://developer.apple.com/documentation/watchconnectivity)
- [Creating Independent watchOS Apps](https://developer.apple.com/documentation/watchos-apps/creating-independent-watchos-apps)

### Wear OS

- [Wear OS Samples (GitHub)](https://github.com/android/wear-os-samples)
- [MediaRecorder Documentation](https://developer.android.com/reference/kotlin/android/media/MediaRecorder)
- [Wear OS Voice Input](https://developer.android.com/training/wearables/user-input/voice)

### Reference Apps

- [Shazam on Apple Watch](https://support.apple.com/guide/shazam/use-shazam-music-recognition-on-apple-watch-dev6fc829d35/web)
- [SoundHound App](https://www.soundhound.com/soundhound)

### TunePal Recognition

- [TunePal Research Paper](https://www.researchgate.net/publication/220418444_Tunepal_Searching_a_Digital_Library_of_Traditional_Music_Scores)
- [MATT2 System Overview](https://bryanduggan.org/projects/tunepal/)

---

## Sources

- [Apple Watch Audio Recording - Stack Overflow](https://stackoverflow.com/questions/29939704/audio-recording-capabilites-on-apple-watch)
- [Wear OS Audio Recording - Stack Overflow](https://stackoverflow.com/questions/47954616/how-to-record-audio-on-android-wear-works-on-phone-not-on-watch)
- [Shazam on Apple Watch - Apple Support](https://support.apple.com/guide/shazam/use-shazam-music-recognition-on-apple-watch-dev6fc829d35/web)
- [Independent watchOS Apps - CapTech](https://www.captechconsulting.com/blogs/independent-apps-in-watchos)
- [Watch Connectivity Tutorial - Kodeco](https://www.kodeco.com/books/watchos-with-swiftui-by-tutorials/v1.0/chapters/4-watch-connectivity)
- [TunePal Help](https://tunepal.org/tunepal/help.php)
- [Godot Wearable Discussion](https://forum.godotengine.org/t/making-wear-os-games/55421)
