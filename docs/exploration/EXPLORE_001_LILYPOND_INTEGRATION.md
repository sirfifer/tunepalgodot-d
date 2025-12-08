# Exploration 001: LilyPond Integration for TunePal

**Date:** December 8, 2025
**Status:** Proposal
**Decision:** Recommended (as secondary output format)

---

## Executive Summary

**LilyPond** is a professional-grade music engraving system that produces publication-quality sheet music. This document explores whether adding LilyPond support to TunePal would provide genuine value beyond what ABC notation already offers.

**Recommendation:** YES, add LilyPond as a **secondary output format** for high-quality PDF/print export. Do NOT replace ABC or require users to write LilyPond directly.

---

## What is LilyPond?

### Overview

LilyPond is a free, open-source music engraving program that takes text input and produces beautifully formatted sheet music. It's part of the GNU Project.

| Attribute | Value |
|-----------|-------|
| First Release | 1996 |
| Current Version | 2.24.4 (December 2024) |
| License | GPL-2.0+ |
| Language | C++ with Scheme (Guile) extension |
| Platforms | Windows, macOS, Linux |
| Primary Repository | GitLab (gitlab.com/lilypond/lilypond) |
| Development Status | Active, steady releases |

### The Name

LilyPond was created by Han-Wen Nienhuys and Jan Nieuwenhuizen. The name comes from the concept of "lily pads" - the way notes sit on staff lines like lily pads floating on a pond.

### Design Philosophy

LilyPond's primary goal is to produce output comparable to **professionally engraved scores** - the kind that were hand-engraved by master craftsmen before computers. It rejects the "mechanical, computer-generated look" of most notation software.

---

## LilyPond vs ABC Notation: Key Differences

### Syntax Comparison

**The same melody in both formats:**

ABC Notation:
```abc
X:1
T:Sample Tune
M:6/8
K:G
|:GAB c2B|AGE G2D|GAB c2d|edB A2G:|
```

LilyPond:
```lilypond
\version "2.24"
\header { title = "Sample Tune" }
\relative g' {
  \time 6/8
  \key g \major
  \repeat volta 2 {
    g8 a b c4 b8 | a g e g4 d8 |
    g a b c4 d8 | e d b a4 g8
  }
}
```

### Feature Comparison

| Feature | ABC | LilyPond | Winner |
|---------|-----|----------|--------|
| **Simplicity** | Very simple, human-readable | Verbose, programming-like | ABC |
| **Learning Curve** | Minutes to learn basics | Hours/days to learn | ABC |
| **Engraving Quality** | Basic, depends on renderer | Publication-quality | LilyPond |
| **Multi-voice Scores** | Supported (since 2.1) but awkward | Native, elegant | LilyPond |
| **Orchestral Works** | Impractical | Fully capable | LilyPond |
| **Custom Notation** | Limited | Infinitely extensible (Scheme) | LilyPond |
| **Folk Music Community** | Dominant standard | Rarely used | ABC |
| **File Size** | Compact | Larger | ABC |
| **Parsing** | Ambiguous, implementation-dependent | Well-defined grammar | LilyPond |
| **PDF Output** | Via external tools (abcm2ps) | Native, beautiful | LilyPond |

### What LilyPond Does That ABC Cannot

1. **Optical Font Scaling**
   - Note heads become more rounded at different staff sizes
   - Staff lines thicken appropriately
   - Professional typographic adjustments

2. **Intelligent Spacing**
   - Stem directions affect horizontal spacing
   - Ledger lines shorten near accidentals
   - Proportional spacing options

3. **Complex Scores**
   - Full orchestral scores with 50+ staves
   - Conductor scores with cues
   - Part extraction from master scores

4. **Programmability**
   - Embed Scheme code for algorithmic composition
   - Create custom notation symbols
   - Automate repetitive tasks

5. **Layout Control**
   - Fine-grained control over every element
   - Page breaking optimization
   - Multi-page scores with proper headers/footers

---

## Relevance to Irish Traditional Music

### The Good News

LilyPond has **specific support for Irish traditional music ornamentation**:

- **Grey Larsen Notation System**: A LilyPond library exists implementing symbols for cuts, strikes, slides, rolls, cranns from Grey Larsen's "The Essential Guide to Irish Flute and Tin Whistle"
- **Breizh Partitions**: A major Celtic music sheet music site (celticscores.com) uses LilyPond as its source format
- **Modal Support**: Full support for Dorian, Mixolydian, Aeolian modes common in Celtic music

### Ornamentation Symbols

Irish traditional music uses specific ornaments that can be precisely notated in LilyPond:

| Ornament | Description | LilyPond Capability |
|----------|-------------|---------------------|
| Cut | Grace note above principal | Native grace notes |
| Tap/Strike | Grace note below principal | Native grace notes |
| Long Roll | 5-note ornament (dotted quarter) | Custom symbol available |
| Short Roll | 4-note ornament | Custom symbol available |
| Cran | Multiple grace notes on low D | Custom symbol available |
| Slide | Pitch bend into note | Custom articulation |

### The Reality Check

While LilyPond CAN notate Irish music beautifully, the folk/traditional music community **overwhelmingly uses ABC notation**. Reasons:

1. ABC was designed FOR folk music
2. Massive existing ABC tune libraries (thesession.org, etc.)
3. Simpler to type while learning a tune
4. Community momentum and network effects

---

## Conversion Between Formats

### ABC → LilyPond (abc2ly)

LilyPond includes a built-in converter:

```bash
abc2ly input.abc -o output.ly
```

**Capabilities:**
- Basic melody conversion
- Key signatures and time signatures
- Modal scales (dorian, mixolydian, etc.)
- Repeat structures

**Limitations:**
- Word/note synchronization at line starts
- Some ABC extensions not supported
- Layout/formatting choices differ from original

### LilyPond → ABC (ly2abc)

A third-party tool exists but is **immature**:

- Repository: github.com/aelkiss/ly2abc
- Status: "Heavily a work in progress"
- Missing: Tuplets, grace notes, decorations, multiple voices
- **Not production-ready**

### Practical Implication

**One-way conversion is reliable** (ABC → LilyPond). Round-trip conversion is not practical. This means:

- ABC remains the "source of truth" for tunes
- LilyPond becomes an "export format" for quality output
- No need to store tunes in both formats

---

## Integration Architecture for TunePal

### Proposed Model

```
┌─────────────────────────────────────────────────────────┐
│                    TunePal Database                      │
│                   (ABC as primary)                       │
└─────────────────────────────────────────────────────────┘
                          │
          ┌───────────────┼───────────────┐
          ▼               ▼               ▼
    ┌──────────┐    ┌──────────┐    ┌──────────┐
    │ ABC View │    │ MIDI     │    │ LilyPond │
    │ (edit)   │    │ Playback │    │ Export   │
    └──────────┘    └──────────┘    └──────────┘
                                          │
                                          ▼
                                    ┌──────────┐
                                    │ PDF/SVG  │
                                    │ Output   │
                                    └──────────┘
```

### Implementation Options

**Option A: External LilyPond Binary**
- Call `abc2ly` then `lilypond` as external processes
- Pros: Simple, uses official tools
- Cons: Requires LilyPond installation, large dependency

**Option B: WebAssembly LilyPond**
- Compile LilyPond to WASM for in-app use
- Pros: No external dependency
- Cons: Complex, may not exist yet

**Option C: Server-Side Rendering**
- Send ABC to server, return PDF
- Pros: No client-side dependency
- Cons: Requires server infrastructure, network dependency

**Recommended: Option A** for desktop/native builds, with Option C as fallback for web/mobile.

---

## Value Proposition Analysis

### What LilyPond Adds to TunePal

| Benefit | Description | Value |
|---------|-------------|-------|
| **Print Quality** | Professional PDF output for printing | HIGH |
| **Sheet Music Export** | Share tunes as beautiful PDFs | HIGH |
| **Tunebook Generation** | Compile multiple tunes into formatted books | MEDIUM |
| **Ornamentation Display** | Show Irish ornaments with standard symbols | MEDIUM |
| **Academic Use** | Suitable for publications, theses | LOW-MEDIUM |

### What LilyPond Does NOT Add

| Non-Benefit | Explanation |
|-------------|-------------|
| Better playback | MIDI from ABC is equivalent |
| Easier editing | ABC is simpler to write |
| Community compatibility | ABC is the standard for sharing |
| Tune recognition | ABC already works for this |

### Who Benefits Most?

1. **Teachers** wanting professional handouts
2. **Session organizers** creating tunebooks
3. **Publishers** needing print-ready scores
4. **Performers** wanting elegant lead sheets

### Who Doesn't Need It?

1. Users who just want to learn tunes by ear
2. Users sharing tunes online (ABC is standard)
3. Mobile-first users (LilyPond is heavy)

---

## Risk Assessment

### Technical Risks

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| LilyPond installation issues | Medium | Medium | Provide clear instructions, fallback to ABC-only |
| abc2ly conversion errors | Low | Low | Validate output, show warnings |
| Large binary size | High | Medium | Make LilyPond optional/downloadable |
| Platform compatibility | Low | Medium | Test on all target platforms |

### Strategic Risks

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| Feature creep | Medium | Medium | Keep as export-only, no editing |
| User confusion | Low | Low | Clear UI separation |
| Maintenance burden | Medium | Low | LilyPond is stable, minimal updates needed |

---

## Comparison with Alternatives

### Why Not Just Better ABC Rendering?

Tools like `abcm2ps` can produce decent output, but:
- Not publication quality
- Limited formatting options
- No programmability

### Why Not MusicXML?

- Verbose, not human-readable
- Primarily an interchange format
- No direct PDF output

### Why Not MuseScore?

- GUI application, not embeddable
- Overkill for folk tunes
- Different use case (composition vs. transcription)

---

## Implementation Effort Estimate

### Minimal Viable Integration

| Task | Effort |
|------|--------|
| Add abc2ly wrapper | 2-4 hours |
| Add LilyPond PDF generation | 2-4 hours |
| UI for "Export as PDF" | 4-8 hours |
| Testing and polish | 4-8 hours |
| **Total** | **12-24 hours** |

### Full Integration (Tunebook Generation)

| Task | Additional Effort |
|------|-------------------|
| Multi-tune selection | 4-8 hours |
| Tunebook formatting templates | 8-16 hours |
| Table of contents generation | 4-8 hours |
| **Additional Total** | **16-32 hours** |

---

## Recommendation

### Verdict: YES, Add LilyPond Support

LilyPond provides **genuine, non-overlapping value** to TunePal:

1. **Publication-quality output** that ABC tools cannot match
2. **Professional PDF export** for teachers, publishers, and performers
3. **Tunebook generation** for session organizers
4. **Irish ornamentation symbols** via existing libraries
5. **One-way conversion** from ABC is reliable and mature

### Implementation Strategy

1. **Phase 1**: Add "Export as PDF (LilyPond)" option
   - Use abc2ly for conversion
   - Call lilypond binary for rendering
   - Optional feature, doesn't affect core ABC workflow

2. **Phase 2**: Add tunebook generation
   - Select multiple tunes
   - Generate formatted PDF collection
   - Include table of contents

3. **Do NOT**:
   - Replace ABC as primary format
   - Add LilyPond editing capability
   - Require LilyPond for basic functionality

### Success Criteria

- Users can export any ABC tune as a high-quality PDF
- Export works on Windows, macOS, and Linux
- No impact on users who don't need PDF export
- Clear documentation on LilyPond installation

---

## Appendix: Resources

### Official LilyPond
- Website: https://lilypond.org/
- Documentation: https://lilypond.org/doc/
- GitLab: https://gitlab.com/lilypond/lilypond

### Conversion Tools
- abc2ly (built into LilyPond): https://lilypond.org/doc/v2.25/Documentation/usage/invoking-abc2ly
- ly2abc (experimental): https://github.com/aelkiss/ly2abc

### Irish Music Resources
- Grey Larsen ornamentation symbols: Available on GitHub (search "lilypond grey larsen")
- Breizh Partitions (Celtic scores): https://www.celticscores.com/

### Community
- LilyPond mailing list: lilypond-user@gnu.org
- Frescobaldi (LilyPond editor): https://github.com/frescobaldi/frescobaldi

---

## Sources

- [LilyPond Official Website](https://lilypond.org/)
- [LilyPond Wikipedia](https://en.wikipedia.org/wiki/LilyPond)
- [LilyPond GitHub Mirror](https://github.com/lilypond/lilypond)
- [Breizh Partitions - Celtic Scores](https://www.celticscores.com/)
- [ABC Notation Limitations - Stack Exchange](https://music.stackexchange.com/questions/23841/what-are-the-limitations-of-the-abc-notation-format)
- [Notation Software Comparison - Slant](https://www.slant.co/topics/4512/~music-notation-software)
- [Irish Music Ornamentation - Tradschool](https://www.tradschool.com/en/about-irish-music/ornamentation-in-irish-music/)
