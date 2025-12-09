# Exploration: Session Tracker ("The Set List")

> [!NOTE]
> **Status:** FEASIBLE (with constraints)
> **Goal:** Continuously identify tunes played at a session to generate a "set list" history, with set grouping and personal tracking.

---

## The Vision

Instead of the "Record -> Stop -> Identify" workflow, the user hits "Start Session". The phone sits on the table and continuously listens, populating a list of tunes played:

```
Session at Matt Molloy's - Dec 9, 2025
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Set 1 (8:42 PM)
  ☑ The Kesh Jig          [I played this]
  ☑ Morrison's Jig        [I played this]
  ☐ Apples in Winter      [Just heard]

Set 2 (8:51 PM)
  ☑ The Bucks of Oranmore [I played this]
  ☐ The Maid Behind the Bar [Just heard]
...
```

### Key Features
- **Automatic tune recognition** via periodic sampling (not continuous)
- **Set detection** - group tunes played back-to-back without pause
- **Personal tracking** - mark which tunes you actually played vs. just heard
- **Geo-location** - identify the session/venue automatically
- **Community sync** - share setlists, see what's being played where

---

## Market Gap Analysis

### What Exists Today

| App | Tune Recognition | Session Tracking | Set Detection | Notes |
|-----|------------------|------------------|---------------|-------|
| **TunePal** | ✅ 10-second samples | ❌ Single-tune only | ❌ None | Your app - great accuracy |
| **FolkFriend** | ✅ 10-second samples | ❌ Single-tune only | ❌ None | Works in noisy 40+ person sessions |
| **Ceol.io** | ❌ No recognition | ✅ Manual logging | ❌ Manual | Users manually enter tune names |
| **The Session** | ❌ No recognition | ❌ None | ✅ Manual sets | 10,472+ user-created sets, but manually entered |

**Key Finding**:
- **Nobody has automated session tracking**
- **Nobody has automated set detection in a consumer app**
- Ceol.io does manual session logging but no auto-recognition
- The Session has sets but they're manually created, not detected from live audio

### Prior Art: Google Pixel "Now Playing"

The closest analogy for the sampling approach:
- Listens every **60 seconds** for ambient music
- Uses on-device database of ~10,000 song fingerprints (~500MB)
- **Negligible battery impact** through smart duty cycling
- Completely offline/private - no audio sent to cloud
- Maintains a history log of recognized songs

**This proves the periodic sampling approach is viable on mobile.**

---

## Technical Feasibility Analysis

### 1. The "Continuous" Challenge

Mobile devices restrict "always listening" (battery/privacy). However, an active "Session Mode" (like a workout tracker or voice recorder) where the app is foregrounded (or a foreground service) is standard and acceptable.

**Battery Impact Estimate:**
- Google Pixel "Now Playing" at 60-second intervals: "negligible"
- AmbientSense research: 13.75 hours with **continuous** recognition at 16kHz
- Our approach (30-second intervals, 10-second samples): very manageable

For a 3-hour session with 30-second sampling:
- 360 samples × 10 seconds = 60 minutes of audio processing
- But spread over 3 hours with 20-second sleep periods between
- **Mitigation:** Skip sampling during detected silence/speech

### 2. The "Set" Challenge (Segmentation)

**The Problem:**
Tunepal's current algorithm (`edSubstring`) looks for the *recording* inside the *database tune*.
- If you record a Set (Tune A + Tune B), the pattern `A...B` will **fail** to match `A` (too many extra notes) and **fail** to match `B`.
- Tunes in sets are played **segue (without pause)**, making boundaries hard to detect.

**Critical Discovery: Bryan Duggan's Research**

Duggan (TunePal's creator) has already researched this exact problem:

#### MATS Algorithm (2008)
- Paper: ["Machine Annotation of Sets of Traditional Irish Dance Tunes"](https://arrow.tudublin.ie/scschcomcon/22/)
- Authors: Bryan Duggan, Mikel Gainza, Pádraig Cunningham
- Presented at ISMIR 2008, Philadelphia

**Key findings:**
- "A set in traditional Irish music is a sequence of two or more dance tunes in the same time signature, where each tune is repeated an arbitrary number of times"
- "A turn in a set represents the point at which either a tune repeats or a new tune is introduced"
- "Tunes in sets are played in a segue (without a pause) and so detecting the turn is a significant challenge"
- **96% accuracy** in identifying tunes within sets
- Works on monophonic flute and tin-whistle recordings

#### TANSEY Algorithm
- "Turn ANnotation from SEts using SimilaritY profiles"
- Named after traditional flute player Seamus Tansey
- Validated on **130 real-world field recordings** from sessions, classes, concerts
- Tested on **solo and ensemble playing** on various instruments
- Tested in **noisy public session environments**

**Implication**: The hard research problem of set boundary detection has been solved academically. The challenge is implementing it in a mobile app context.

### 3. Proposed Algorithm: "The Rolling Recognizer"

Instead of "magic" segmentation, we use disjoint or overlapping windows.

#### Core Loop
```
1. Buffer: Circular buffer holding notes
2. Trigger: Every 10-15 seconds:
   - Take the notes from T-15s to T
   - Run search() (linear scan of 24,000 tunes)
   - Result: "Top Match: The Kesh Jig (Confidence 0.9)"
3. State Machine (The "Set Logic"):
   - Store the result history: [Kesh, Kesh, Kesh, <LowConf>, Morrison's, Morrison's]
   - Smoothing:
     - If we see Tune A for 3 consecutive windows -> **Confirmed**. Add to UI.
     - If we see Tune A then Tune B with no silence -> **Same Set**
     - If we see Tune A then silence then Tune B -> **New Set**
     - If we see Low Confidence -> **Noodling/Tuning/Chatter**. Ignore.
```

#### Set Boundary Detection Approaches

**Approach 1: Silence-Based (MVP)**
- Detect silence gaps (>2 seconds) between tune groups
- Simple RMS energy threshold
- Silence = set boundary, continuous music = same set

**Approach 2: Post-Processing Analysis (Recommended)**
```
During Session:
├── Sample every 20-30 seconds
├── Store: {timestamp, detected_tune, confidence}
├── Track silence periods (>2 seconds)

Post-Session Analysis:
├── Group consecutive detections of same tune → "Tune A played 3x"
├── Silence gaps → set boundaries
├── Non-silence tune changes → same set
└── Output: Set 1: [Tune A, Tune B], Set 2: [Tune C, Tune D, Tune E]
```

**Approach 3: Advanced (MATS/TANSEY-style)**
- Track melodic similarity between consecutive samples
- Detect "turn points" where tune identity changes using chroma features
- More complex but handles segue transitions better

### 4. Sampling Strategy

#### Irish Session Timing
| Element | Typical Duration |
|---------|-----------------|
| Single tune (1x through) | 30-60 seconds |
| Tune repetitions | 2-3x typical |
| Total per tune | 1-3 minutes |
| Set (2-3 tunes) | 3-9 minutes |
| Pause between sets | 10 seconds - 5 minutes (talking, drinks) |

#### Sampling Scenarios

**Conservative: Sample every 20 seconds, 10-second capture**
- Guaranteed to catch every tune (shortest tune = 30 sec)
- 3-hour session = 540 samples = 90 minutes processing
- Higher battery use but maximum coverage

**Balanced: Sample every 30 seconds, 10-second capture** (Recommended)
- Should catch every tune (may miss extremely short pieces)
- 3-hour session = 360 samples = 60 minutes processing
- Reasonable battery use

**Aggressive: Sample every 45 seconds, 10-second capture**
- Might miss short tunes or brief tune changes
- 3-hour session = 240 samples = 40 minutes processing
- Lower battery use

### 5. Performance Optimization

Running 20 threads to search 24,000 tunes every 10 seconds might heat up the phone.

**Optimization A: Candidate Filtering**
Only run full `edSubstring` on tunes with matching Key Signature or Time Signature (if detectable).

**Optimization B: Common Tune Cache**
Check the "Top 1000 Session Tunes" first (data available from irishtune.info).

**Optimization C: N-gram Pre-filtering**
Build index of 4-note patterns at startup, filter candidates before edit distance (5-10x speedup, per FolkFriend analysis).

**Optimization D: Geolocated Prioritization (Future)**
Check tunes popular *at this venue* first.

---

## The Session API Integration

### Current Capabilities

The Session has a **read-only API** ([documentation](https://thesession.org/api)):

**Can Query:**
- Tunes (search, details, popularity)
- Sessions (locations, times, nearby)
- Events, Recordings
- Members' tunebooks
- Sets (user-created tune combinations)

**Cannot Do (via API):**
- Submit new tunes
- Upload setlists
- Post session recordings
- Create sets programmatically

### Integration Options

**Option A: Read-Only Integration (MVP)**
- Query The Session for tune metadata when we identify a tune
- Link detected tunes to their Session page
- Show if a tune is popular in sessions generally

**Option B: Community Sharing (Workaround)**
- Generate shareable setlist (text, image, or link)
- User manually posts to The Session discussions or social media
- Build our own community platform parallel to The Session

**Option C: Future Partnership**
- Approach The Session about API expansion
- Propose automated setlist submission feature
- Could be mutually beneficial - more data for the community

---

## User Interaction: "Tunes I Played"

### The Problem
Automatic detection tells us what tunes were played at the session, but not which ones the user personally played vs. just heard.

### Solution: Easy Post-Session Tagging

After session ends, show the detected tune list with quick toggles:

```
Session Summary - Matt Molloy's, Dec 9 2025
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Set 1 (8:42 PM)
  ☑ The Kesh Jig          [I played this]
  ☑ Morrison's Jig        [I played this]
  ☐ Apples in Winter      [Just heard]

Set 2 (8:51 PM)
  ☑ The Bucks of Oranmore [I played this]
  ☐ The Maid Behind the Bar [Just heard]

[Mark all as played] [Mark none as played]
```

### Additional Tagging Options
- "I know this tune" vs "New to me"
- "Want to learn"
- Star/favorite
- Add notes ("great variation on the B part")

### Personal Stats Over Time
- Tunes played at sessions: 47
- Tunes heard but don't know: 123
- Most played tune: The Kesh Jig (12 sessions)
- Sessions attended: 8
- Favorite venues: Matt Molloy's (4), The Crane (2)

---

## Technical Architecture

### Already Have (TunePal Codebase)
- Audio capture via Godot `AudioEffectCapture`
- Spectrum-based pitch detection ([record.gd](../../TunepalGodot/Scripts/record.gd))
- YIN pitch detection (experimental: [yin_detector.h](../../src_experimental/algorithms/yin_detector.h))
- DTW sequence matching (experimental: [dtw_matcher.h](../../src_experimental/algorithms/dtw_matcher.h))
- Tune database (~23,000 tunes) and search
- Multi-threaded search via C++ extension

### Need to Build

| Component | Purpose | Complexity |
|-----------|---------|------------|
| Session Manager | Start/stop session, state machine | Low |
| Sampling Timer | Periodic 10-sec captures every 20-30 sec | Low |
| Silence/Speech Detector | Skip non-music, detect set boundaries | Medium |
| Session Storage | SQLite table for session/tune history | Low |
| Geo-Location | GPS for venue identification | Low (platform API) |
| Set Grouping Algorithm | Group tunes into sets post-session | Medium |
| Session Summary UI | Display results, tune tagging | Medium |
| Sync/Export | Share to Session, export setlists | Medium |

### Data Model

```
Session
├── id (UUID)
├── start_time
├── end_time
├── location_lat
├── location_lng
├── venue_name (user-entered or geocoded)
└── notes

SessionTune
├── id
├── session_id (FK)
├── tune_id (FK to tune database)
├── tune_name
├── detected_at (timestamp)
├── confidence (0.0-1.0)
├── set_number (which set in the session)
├── set_position (position within set)
├── user_played (boolean - did user play this?)
├── user_knows (boolean - does user know this tune?)
└── notes

Set (computed/grouped)
├── session_id
├── set_number
├── start_time
├── tunes[] (ordered list)
```

### Platform Considerations

**iOS Background Execution:**
- Background audio mode requires proper entitlements
- App must be "playing audio" to stay active (can play silence)
- Alternative: require foreground with screen lock disabled
- Best: native plugin for proper background handling

**Android Background Service:**
- WorkManager for periodic tasks
- Foreground service with notification for continuous
- More flexible than iOS

**Godot Limitations:**
- Godot doesn't have great background execution support
- May need native plugins for iOS/Android
- Alternative: keep app in foreground, use screen wake lock

**MVP Approach:**
For proof-of-concept, accept foreground-only operation:
- User keeps phone out with app open
- Screen stays on (wake lock)
- Validate the concept before investing in background execution

---

## User Experience (UX)

### During Session
- **UI:** A simple list that grows in real-time
- **Feedback:** "Listening..." pulsing indicator
- **Set Markers:** Visual separation when silence detected
- **Intervention:** User can swipe to delete a false positive

### After Session
- Session summary with set groupings
- Quick toggles for "I played this"
- Export/share options
- Save to session history

### Privacy
- **Audio is discarded** - Only detected notes (ABC/text) are transiently processed
- **No audio saved to disk**
- **Local processing only** - no audio sent to servers
- **User-initiated** - explicit "Start Session" action

---

## Community & Social Features (Future)

### Session Sharing
- Generate shareable setlist image
- Export to text/JSON for posting
- Link to The Session tune pages

### Venue Profiles
- Aggregate data: "At Matt Molloy's, the most common tunes are..."
- Session frequency: "Usually sessions here on Tuesdays"
- Anonymous aggregation across users

### Personal Journey
- "Your session history"
- "Tunes you've heard most"
- "Tunes to learn next" (heard often but don't know)

### Community Setlist Database
If The Session doesn't support uploads, we could:
- Build our own setlist sharing platform
- Let users publish session setlists with venue/date
- Create a "what's being played where" map

---

## Risks & Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| Background execution not allowed | High | MVP: foreground-only, later: native plugins |
| Battery drain complaints | Medium | Smart duty cycling, user-configurable intervals |
| Low accuracy in noisy sessions | Medium | FolkFriend tested with 40+ people successfully |
| Set detection inaccurate | Medium | Let user manually adjust set boundaries |
| Privacy concerns | Medium | Local processing, no audio storage, clear consent |
| False positives accumulate | Low | Confidence threshold, user can delete mistakes |
| The Session doesn't allow uploads | Low | Build own sharing, or partnership opportunity |

---

## Proof of Concept Scope

### Phase 1: MVP Feature Set
1. "Session Mode" toggle in app
2. Modify `record.gd` to have a "Continuous" checkbox
3. Instead of stopping after 10s, loop:
   - Record 12s
   - Spawn search threads
   - *Immediately* start recording next 12s buffer (double buffering)
   - When search returns, update a "Live Log" UI
4. Simple silence detection (RMS threshold) for set boundaries
5. End session → show summary with set groupings
6. Toggle "I played this" on each tune
7. Save to local SQLite database
8. Test with a recorded set from YouTube (played into the mic)

### Not in MVP
- Background execution (foreground only)
- Geo-location (manual venue entry)
- The Session integration
- Community sharing
- Advanced set boundary detection (MATS/TANSEY)
- Personal stats/analytics

### Success Criteria
- Detects 80%+ of tunes at a real session
- Correctly groups 70%+ of sets
- Battery usage acceptable for 2-hour session
- User finds value in the session log

---

## Recommendations

### Start with a "Sliding Window" Prototype

1. Modify `record.gd` to have a "Continuous" checkbox
2. Instead of stopping after 10s, loop:
   - Record 12s
   - Spawn search threads
   - Immediately start recording next 12s buffer (double buffering)
   - When search returns, update a "Live Log" UI
3. Test with a recorded set from YouTube (played into the mic)

This would validate:
- Whether the sampling approach catches all tunes
- Battery impact in real usage
- Accuracy in real session conditions
- User experience considerations

---

## Research Sources

### Bryan Duggan's Work (TunePal Creator)
- [Machine Annotation of Sets of Traditional Irish Dance Tunes](https://arrow.tudublin.ie/scschcomcon/22/) - MATS algorithm, 96% tune identification in sets
- [MATT2 and TANSEY](https://arrow.dit.ie/argcon/46/) - Set annotation algorithms
- [TunePal: The Traditional Musician's Toolbox](https://arrow.tudublin.ie/scschcomcon/102/) - Original TunePal research
- [A Corpus of Annotated Irish Traditional Dance Music Recordings](https://www.semanticscholar.org/paper/A-Corpus-of-Annotated-Irish-Traditional-Dance-Music-Beauguitte-Duggan/ee457ef34b0d55d0559e2aa05b1cc2742035024e) - 130 real-world field recordings tested

### Similar Apps & Services
- [FolkFriend](https://github.com/TomWyllie/folkfriend) - Open source, tested at 40-person sessions
- [FolkFriend Discussion on The Session](https://thesession.org/discussions/44025) - Real user experiences
- [Ceol.io](https://www.ceol.io/) - Manual session logging (no auto-recognition)
- [The Session API](https://thesession.org/api) - Read-only, 10,472+ user-created sets

### Technical References
- [Google Pixel Now Playing](https://support.google.com/pixelphone/answer/7535326) - Ambient recognition approach
- [AmbientSense Research](https://www.researchgate.net/publication/261467304) - Battery-efficient continuous recognition
- [Shazam Algorithm Paper](https://www.ee.columbia.edu/~dpwe/papers/Wang03-shazam.pdf) - Audio fingerprinting fundamentals
- [Music Boundary Detection with Neural Networks](https://www.ofai.at/~jan.schlueter/pubs/2015_eusipco.pdf) - CNN approaches

---

## Open Questions

1. **Background execution strategy**: Native plugin vs. foreground-only vs. accept platform limitations?

2. **Set boundary detection accuracy target**: Is 70% acceptable, or is manual adjustment too annoying?

3. **The Session partnership**: Worth pursuing API expansion for setlist uploads?

4. **Community platform**: Build our own, or wait for The Session integration?

5. **Privacy messaging**: How prominently to message "no audio stored" to users and session participants?

---

## Related Documents

- [EXPLORE_004_FOLKFRIEND_ANALYSIS.md](EXPLORE_004_FOLKFRIEND_ANALYSIS.md) - Technical analysis of FolkFriend's algorithms (relevant for optimization)
