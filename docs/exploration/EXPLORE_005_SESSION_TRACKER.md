# Exploration: Session Tracker ("The Set List")

> [!NOTE]
> **Status:** FEASIBLE (with constraints)
> **Goal:** Continuously identify tunes played at a session to generate a "set list" history.

## The Concept
Instead of the "Record -> Stop -> Identify" workflow, the user hits "Start Session". The phone sits on the table and continuously listens, populating a list of tunes played:

```
[20:14] The Kesh Jig
[20:17] The Morrison's Jig
[20:21] Drowsy Maggie
...
```

## Technical Feasibility Analysis

### 1. The "Continuous" Challenge
Mobile devices restrict "always listening" (battery/privacy). However, an active "Session Mode" (like a workout tracker or voice recorder) where the app is foregrounded (or a foreground service) is standard and acceptable.

**Battery Impact:** 
- **High.** Continuous FFT + Edit Distance search is CPU expensive.
- **Mitigation:** Don't search every frame. Buffer audio, search every 15-30 seconds.

### 2. The "Set" Challenge (Segmentation)
Tunepal's current algorithm (`edSubstring`) looks for the *recording* inside the *database tune*.
- If you record a Set (Tune A + Tune B), the pattern `A...B` will **fail** to match `A` (too many extra notes) and **fail** to match `B`.
- **Solution:** **Sliding Window / Chunking**.
    - We cannot feed a 5-minute audio buffer to `edSubstring`.
    - We must feed short, tune-length chunks (e.g., 10-15 seconds).

### 3. Proposed Algorithm: "The Rolling Recognizer"

Instead of "magic" segmentation, we use disjoint or overlapping windows.

1.  **Buffer:** Circular buffer holding notes.
2.  **Trigger:** Every 10 seconds:
    - Take the notes from `T-15s` to `T`.
    - Run `search()` (linear scan of 24,000 tunes).
    - **Result:** "Top Match: The Kesh Jig (Confidence 0.9)"
3.  **State Machine (The "Set Logic"):**
    - Store the result history: `[Kesh, Kesh, Kesh, <LowConf>, Morrison's, Morrison's]`
    - **Smoothing:** 
        - If we see `Tune A` for 3 consecutive windows -> **Confirmed**. Add to UI.
        - If we see `Tune A` then `Tune B` -> **Transition**.
        - If we see `Low Confidence` -> **Noodling/Tuning/Chatter**. Ignore.

### 4. Performance Optimization
Running 20 threads to search 24,000 tunes every 10 seconds might heat up the phone.
- **Optimization A:** **Candidate Filtering.** Only run full `edSubstring` on tunes with matching Key Signature or Time Signature (if detectable).
- **Optimization B:** **Common Tune Cache.** Check the "Top 1000 Session Tunes" first.
- **Optimization C:** **Geolocated Prioritization.** (Future) Check tunes popular *in this location*.

## User Experience (UX)

- **UI:** A simple list that grows.
- **Feedback:** "Listening..." pulsing indicator. 
- **Intervention:** User can swipe to delete a false positive.
- **Privacy:** **Audio is discarded.** Only the detected notes (ABC/text) are transiently processed. No audio saved to disk.

## Recommendation

**Start with a "Sliding Window" Prototype.**
1.  Modify `record.gd` to have a "Continuous" checkbox.
2.  Instead of stopping after 10s, have it loop:
    - Record 12s.
    - Spawn search threads.
    - *Immediately* start recording next 12s buffer (double buffering).
    - When search returns, update a "Live Log" UI.
3.  Test with a recorded set from YouTube (played into the mic).

## References
- **Google "Now Playing":** Uses low-power DSP. We don't have access to that, but "Foreground Service" is the Android equivalent for active apps.
- **Shazam "Auto":** Uses fingerprinting. We are using Melody Transcription. Transcription is heavier but works for *versions* of tunes (unlike fingerprinting which needs exact audio matches).
