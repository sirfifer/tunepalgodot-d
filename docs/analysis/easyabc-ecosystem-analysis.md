# EasyABC Ecosystem Analysis

**Date:** December 8, 2025
**Purpose:** State of the union assessment of the EasyABC project and its forks

---

## Executive Summary

**EasyABC is actively maintained** with recent commits as of March 2025 and issues being closed through November 2025. The project has **388 commits**, **125 stars**, and **46 forks**. The maintainer (jwdj - JW de Jong) is responsive to pull requests and actively merges contributions. However, there are **35 open issues** and only **3 open PRs** awaiting review.

**Key Finding:** No fork has "run away with it" and solved all problems. The main repository remains the canonical, most active version. The ecosystem is healthy but fragmented, with contributors working in their own forks rather than consolidating efforts upstream.

---

## Main Repository Analysis

### Repository: [jwdj/EasyABC](https://github.com/jwdj/EasyABC)

| Metric | Value |
|--------|-------|
| Stars | 125 |
| Forks | 46 |
| Open Issues | 35 |
| Open PRs | 3 |
| Closed PRs | 45 |
| Total Commits | 388 |
| Language | Python (99.5%) |
| License | GPL-2.0 |
| Platforms | Windows, macOS, Linux |

### Recent Activity (2025)

The project shows **strong activity** in 2025:

- **March 2025:** Multiple PR merges including:
  - Copy/paste improvements (#125)
  - Tune list menu (#124)
  - User-defined colors (#120)
  - xml2abc update to v157 (#119)
  - Playback fixes (#118)
  - macOS shortcuts (#115)
  - Note highlighting improvements (#113)
  - Python 3.13 compatibility (#122)

- **November 2025:** Issues being actively triaged and closed:
  - #138: Modified ABC text detection
  - #136: Note highlighting lag
  - #90: Chord playback in selection
  - #77: Multi-voice selection issues

### Current Issues (35 Open)

**Bug Reports:**
- MIDI playback issues (#127, #135)
- Print rendering problems
- Python compatibility errors
- Runtime exceptions

**Feature Requests:**
- Dark theme support (#139)
- Linux packaging improvements (#126, #140)
- UI improvements (#130 - duplicate quit buttons)

**Pending PRs:**
1. **#134** - Debian package (bomm) - Sep 2025
2. **#132** - Composition from copilot (KaiGodZ) - Jun 2025
3. **#131** - German translation update (bomm) - May 2025, 22 comments

---

## Fork Analysis

### Overview of 46 Forks

Most forks are **personal experiments or learning projects** with 0 stars and no downstream activity. Only a handful show meaningful development:

### Tier 1: Active Contributors (Upstream Focus)

#### 1. [aupfred/EasyABC](https://github.com/aupfred/EasyABC) - Frédéric Aupépin
- **Stars:** 4 (most starred fork)
- **Focus:** 64-bit macOS builds
- **Status:** Active contributor to main repo
- **Last Push:** March 2025
- **Note:** Major contributor - many 2025 commits to main repo are from this contributor

#### 2. [bomm/EasyABC](https://github.com/bomm/EasyABC)
- **Focus:** Debian packaging + German translation
- **Status:** Has 2 open PRs on main repo
- **Last Push:** Active in 2025
- **Purpose:** "Provide bugfixes or improvements for the original repository"

### Tier 2: Specialized Variants

#### 3. [lutzhamel/EasyABCLite](https://github.com/lutzhamel/EasyABCLite)
- **Commits:** 391
- **Last Push:** May 2025
- **Purpose:** Simplified/streamlined variant
- **Note:** Created May 2025, appears to be experimental simplification

#### 4. [l-rettberg/EasyABC](https://github.com/l-rettberg/EasyABC)
- **Last Push:** November 2025
- **Branch:** Uses `main` instead of `master`
- **Note:** Recent activity but unclear purpose

### Tier 3: Historical/Dormant

#### 5. [louis-barman/EasyABC](https://github.com/louis-barman/EasyABC)
- **Commits:** 139
- **Status:** Dormant
- **Note:** May be the original author's fork before jwdj took over

#### 6. [bwl21/EasyABC](https://github.com/bwl21/EasyABC)
- **Last Push:** August 2016
- **Status:** Abandoned
- **Note:** Historically significant but long dormant

#### 7. [SpotlightKid/EasyABC](https://github.com/SpotlightKid/EasyABC)
- **Commits:** 157
- **Status:** Inactive (0 stars, 0 forks)

#### 8. [jw35/EasyABC](https://github.com/jw35/EasyABC)
- **Status:** "Superseded by the work of Frédéric Aupepin (aupfred)"
- **Note:** Explicitly points to aupfred as the active contributor

---

## Key Contributors

Based on commit history and PR activity:

1. **JW de Jong (jwdj)** - Current maintainer, merges PRs
2. **Frédéric Aupépin (aupfred)** - Major contributor, macOS focus
3. **bomm** - Packaging and translations
4. **Nils Liberg** - Original creator (historical)

---

## Technical Architecture

### Core Dependencies
- **wxPython** - Cross-platform UI framework
- **Scintilla** - Text editor component for ABC code
- **FluidSynth** - SoundFont-based MIDI playback
- **pygame/portmidi** - Real-time MIDI input
- **python-midi** - MIDI file parsing

### External Tools (bundled/required)
- **abcm2ps** - ABC to PostScript/SVG conversion
- **abc2midi** - ABC to MIDI conversion
- **xml2abc** - MusicXML to ABC conversion
- **abc2xml** - ABC to MusicXML conversion

### Supported Formats
- **Import:** ABC, MusicXML, MIDI, Noteworthy Composer
- **Export:** MIDI, SVG, PDF (single tune or tunebook)

---

## Ecosystem Health Assessment

### Strengths

1. **Active Maintainer:** jwdj is responsive and merging PRs in 2025
2. **Python 3.13 Compatibility:** Recently updated
3. **Cross-Platform:** Works on Windows, macOS, Linux
4. **GPL-2.0 License:** Permissive for forking
5. **Good Documentation:** Build scripts for all platforms
6. **Contributor Base:** Active contributors (aupfred, bomm)

### Weaknesses

1. **No Official Releases:** No GitHub releases/tags published
2. **35 Open Issues:** Some dating back years
3. **Fragmented Effort:** Contributors work in forks rather than upstream
4. **No CI/CD:** No automated testing visible
5. **Packaging Issues:** Linux installation remains problematic (#126)
6. **Dark Theme Missing:** Common modern feature request (#139)

### Opportunities

1. **Debian Packaging:** PR #134 ready for merge
2. **Translation Updates:** German translation PR #131 waiting
3. **Consolidation:** Could unify fork efforts upstream
4. **Modern Python:** Already supports Python 3.13

### Threats

1. **Single Maintainer Risk:** jwdj appears to be sole maintainer
2. **wxPython Dependency:** Framework has its own maintenance challenges
3. **Competing Solutions:** Web-based ABC editors emerging

---

## Comparison: Should You Fork?

### Arguments FOR Forking

1. **35 open issues** means known problems exist
2. **GPL-2.0** makes forking legally straightforward
3. **Python codebase** is accessible and modifiable
4. **Your specific needs** may diverge from general ABC editing
5. **Integration opportunity** with your Godot/Tunepal project

### Arguments AGAINST Forking

1. **Main repo is active** - better to contribute upstream
2. **No fork has succeeded** in becoming a better alternative
3. **Maintenance burden** - you'd inherit all issues plus your changes
4. **Community fragmentation** - yet another fork dilutes effort

### Recommendation

**Don't fork unless you have specific needs that diverge from EasyABC's direction.**

Instead, consider:
1. **Contributing upstream** - PRs are being merged
2. **Using as a library** - Extract specific modules (abc2midi, xml2abc)
3. **Building integration** - Connect your Godot project to EasyABC tools without forking the whole editor

If you DO fork:
- Start from `jwdj/EasyABC` (canonical, most active)
- Consider incorporating PR #134 (Debian packaging)
- Focus on specific improvements, contribute back upstream

---

## Relevant Modules for TunePal/Godot Integration

Based on the codebase, these modules might be most relevant:

| Module | Purpose | Potential Use |
|--------|---------|---------------|
| `abc2midi` | ABC to MIDI conversion | Playback in Godot |
| `xml2abc` | MusicXML to ABC | Import from other notation software |
| `abc2xml` | ABC to MusicXML | Export to standard format |
| `midi2abc` | MIDI to ABC | Transcription support |
| `fluidsynth.py` | SoundFont playback | Audio rendering |

---

## Alternative ABC Tools

From broader ecosystem research:

- **abctool** - Single-file Python ABC handler
- **The Craic** - Mobile ABC app (iOS/Android)
- **ABCPig** - Web-based ABC editor
- **DaCapo Editor** - ABC-based sheet music editor
- **MusicRaft** - Qt-based (PySide2/PyQt5) music composition

---

## Conclusion

The EasyABC ecosystem is **healthier than expected** with active maintenance in 2025. The main repository at `jwdj/EasyABC` is the clear canonical version. No fork has emerged as a superior alternative - the most active contributors (aupfred, bomm) are working to improve the main repo rather than competing.

**For your TunePal/Godot project:**
1. Consider the ABC conversion tools as standalone utilities
2. Contribute fixes upstream if you find bugs
3. Only fork if you need fundamentally different architecture

---

## Sources

- [jwdj/EasyABC](https://github.com/jwdj/EasyABC) - Main repository
- [aupfred/EasyABC](https://github.com/aupfred/EasyABC) - Most starred fork
- [bomm/EasyABC](https://github.com/bomm/EasyABC) - Packaging focus
- [lutzhamel/EasyABCLite](https://github.com/lutzhamel/EasyABCLite) - Lite variant
- [ABC Notation Software](https://abcnotation.com/software) - Ecosystem overview
- [GitHub ABC Notation Topic](https://github.com/topics/abc-notation) - Related projects
