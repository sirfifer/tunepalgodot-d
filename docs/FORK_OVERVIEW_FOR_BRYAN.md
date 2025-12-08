# A Letter to Bryan Duggan: Fork Overview & Collaboration Proposal

**Date:** December 8, 2025
**From:** A Fellow Traditional Musician
**To:** Dr. Bryan Duggan, Creator of Tunepal

---

## Dear Bryan,

First and foremost, I want to express my profound gratitude for the incredible gift you've given to the traditional Irish music community. Tunepal has been a companion at sessions, a teacher during practice, and a bridge connecting musicians to tunes they might never have discovered otherwise.

Since 2009, your creation has served approximately 100,000 musicians and facilitated millions of tune searches. That's not just software - that's cultural preservation. That's keeping the tradition alive. As a traditional musician myself, I've experienced firsthand the magic of hearing a tune at a session, pulling out Tunepal, and having that moment of "Ah! That's what it's called!"

**Go raibh mile maith agat** for all of it.

---

## Why This Fork Exists

I forked your TunepalGodot repository not to compete with or replace your work, but to help. My goal is simple: **preserve your vision, respect your intent, and contribute back to what you've built**.

Everything I've done is with the following principles in mind:
1. **This is your project** - I'm just trying to help
2. **Open source and free** - exactly as you intended
3. **Contribution-ready** - all work is structured to merge back upstream
4. **Cross-platform first** - respecting your architecture decisions
5. **Community-oriented** - serving traditional musicians, not commercial interests

**I would be honored to:**
- Merge everything back into your upstream repository
- Work directly out of your repo instead of this fork
- Collaborate in whatever way works best for you
- Simply hand over any useful work and step back

Whatever serves the project and the community best - I'm completely flexible.

---

## Who I Am

I've been a musician my whole life, but Irish traditional music found me around 1989 and never let go. My most intense involvement was during the 1990s when I lived in Seattle, Washington - I was playing music and involved in set and step dancing pretty much six nights a week. Sessions, ceilis, festivals, the whole community. I've never quite matched that level of intensity since, but I've stayed connected to the tradition ever since, and I still aspire to do more.

My primary instrument is the Irish flute, though I also play tin whistle (of course), bodhrán, bones, and I mess around on keyboard when no one's listening.

There's an interesting wrinkle to my story right now: I'm in the process of reinventing my career. I've always been in tech, but I'm going all-in on AI. Many of the companies I'm most interested in - and the jobs I'm actively pursuing - are in Dublin. Others are elsewhere in Europe. If things go the way I hope, I'll be Ireland-adjacent within the next year.

So when I say I care about this project, it's not abstract. Tunepal has been part of my musical life. Traditional Irish music is part of who I am. And if I'm lucky enough to land in Ireland, I'll be back at sessions regularly, with Tunepal on my phone (or watch, if we build that feature).

---

## Full Transparency: My Development Process

**I want to be completely upfront about how I've been doing this work.**

Since forking your repository, the majority of my contributions have been created through direct collaboration with **Claude Code** (Anthropic's AI coding assistant), primarily using the **Opus 4.5** model. This means:

- I describe what I want to explore, build, or document
- Claude helps research, write code, create documentation, and analyze options
- I review, guide, iterate, and make decisions
- The AI does much of the heavy lifting on implementation

### Why I'm Working This Way

This is part of a deliberate journey I'm on. I'm reinventing my career around AI - not just using AI tools, but developing deep expertise in *effective collaboration with AI*. The jobs I'm pursuing (many in Dublin) require demonstrating real skill in this area.

So when I picked up this project, it was partly because:
1. I genuinely love traditional music and want Tunepal to succeed
2. It's a real-world project that lets me build and demonstrate AI collaboration skills
3. I can potentially contribute something meaningful while learning

### Why I'm Telling You This

**Transparency matters.** You deserve to know exactly what you're looking at:

- The exploration documents, code analysis, and documentation have significant AI involvement
- My role has been direction, judgment, review, and iteration - not line-by-line authorship
- The quality and depth would not have been achievable in this timeframe without AI assistance

### If You Have Concerns

**I completely respect that you may have reservations about AI-assisted development.** Some people:
- Prefer human-authored contributions only
- Have concerns about AI training on open source code
- Simply prefer traditional development workflows
- Have other reasons I haven't considered

**If any of this is a problem for you, please tell me.** I will:
- Gladly adapt my approach to whatever you're comfortable with
- Accept any constraints you set on how AI tools are used
- Abandon this fork entirely if that's your preference

The project is yours. The community is what matters. I have no ego invested in having my contributions accepted - I genuinely just want to help in whatever way is welcome.

### A Note on Quality

Despite the AI involvement, I believe the work stands on its own merits:
- Research is factual and verifiable
- Code builds and runs
- Analysis is thorough and considers multiple perspectives
- Everything is documented for future maintainability

But I wanted you to have the full picture before deciding whether this collaboration makes sense for you.

### A Thought on AI as a Feature

I have no current plan to add AI as a feature within Tunepal itself - I won't force AI into the product without a compelling use case. That said, if a genuinely valuable application emerged (and I don't have one in mind today), I'd be enthusiastic about exploring it. This would align with my broader learning goals and could potentially benefit the project. Just putting it on the table as something to think about, not a proposal.

---

## Summary of Work Done

Here's a concise overview of what's been added to this fork since departing from your repository. All of this is available for you to review, cherry-pick, or adopt wholesale.

### 1. Build & Test Infrastructure

| What | Where | Description |
|------|-------|-------------|
| **CI/CD Pipeline** | `.github/workflows/build.yml` | GitHub Actions builds for Linux, Windows, macOS, iOS, Android |
| **Test Suite** | `TunepalGodot/Scripts/test_tunepal.gd` | GDScript tests for the edit distance algorithm |
| **Test in CI** | `.github/workflows/build.yml:99-136` | Automated test execution on every push |
| **SessionStart Hook** | `.claude/install-godot.sh` | Auto-installs Godot 4.3 for development sessions |
| **Issue Templates** | `.github/ISSUE_TEMPLATE/` | Bug report and feature request templates |
| **Labels Config** | `.github/labels.yml` | Standardized issue labels |

**Why:** Automated builds and tests reduce friction for contributors and catch issues early. The CI builds extension binaries for all platforms automatically.

### 2. iOS Platform Support

| What | Where | Description |
|------|-------|-------------|
| **Tunepal GDExtension** | `TunepalGodot/addons/tunepal/bin/tunepal.gdextension:24-25` | Added iOS arm64 entries |
| **SQLite xcframework** | `TunepalGodot/addons/godot-sqlite/gdsqlite.gdextension:18-19` | Updated to use xcframework for iOS |
| **Export Presets** | `TunepalGodot/export_presets.cfg` | iOS export template with microphone permissions |
| **Simulator Workflow** | `docs/IOS_SIMULATOR_WORKFLOW.md` | Documentation for iOS simulator development |
| **Simulator Build Plan** | `docs/plans/PLAN_001_SQLITE_IOS_SIMULATOR.md` | Step-by-step plan to build godot-sqlite for simulator |

**Why:** Your blog post mentions wanting iOS support and a launch at Willie Clancy 2025. This lays the foundation for iOS deployment.

### 3. Documentation

| Document | Location | Purpose |
|----------|----------|---------|
| **README Expansion** | `README.md` | Full attribution to you, development status tables, contributor list |
| **ROADMAP** | `ROADMAP.md` | Preserves your vision verbatim, adds phase-based priorities |
| **CONTRIBUTING** | `CONTRIBUTING.md` | Guidelines emphasizing upstream alignment with your repo |
| **LICENSE** | `LICENSE` | MIT license (matching your "free and open source" intent) |

**Why:** Clear documentation helps contributors understand the project's direction and ensures credit goes where it belongs - to you.

### 4. Exploration Documents

These are research documents exploring features aligned with your stated vision. They're meant to inform future development decisions, not commit to any particular path.

| Document | Location | Summary |
|----------|----------|---------|
| **LilyPond Integration** | `docs/exploration/EXPLORE_001_LILYPOND_INTEGRATION.md` | Analysis of LilyPond for publication-quality PDF export |
| **Smartwatch Integration** | `docs/exploration/EXPLORE_002_SMARTWATCH_INTEGRATION.md` | Apple Watch and Wear OS for quick tune capture at sessions |
| **Learning & Playback** | `docs/exploration/EXPLORE_003_LEARNING_AND_PLAYBACK.md` | Tempo control, A-B loops, multi-instrument accompaniment, lilt/expression |
| **FolkFriend Analysis** | `docs/exploration/EXPLORE_004_FOLKFRIEND_ANALYSIS.md` | Deep competitive analysis of FolkFriend tune recognition - what it does better, licensing implications (GPL vs MIT), clean-room implementation roadmap |
| **EasyABC Analysis** | `docs/analysis/easyabc-ecosystem-analysis.md` | State of the EasyABC project and its forks |

**Why:** Your README mentions "interactive music scores with playback, repeat sections and slowdown" and "support for full range of instruments, sessions and recordings." These explorations dive deep into what that might look like. The FolkFriend analysis specifically looks at what "best in class" tune recognition looks like and how Tunepal could match or exceed it.

---

## Detailed File Reference

Here's every file added or significantly modified, so you can review at your leisure:

### New Files

```
.github/
  workflows/build.yml         # CI/CD pipeline
  labels.yml                  # Issue label definitions
  ISSUE_TEMPLATE/
    bug_report.md            # Bug report template
    feature_request.md       # Feature request template
    config.yml               # Issue template config

.claude/
  settings.json              # Claude Code settings with GitHub whitelist
  install-godot.sh           # SessionStart hook for Godot installation

docs/
  FORK_OVERVIEW_FOR_BRYAN.md # This document - overview for the original author
  IOS_SIMULATOR_WORKFLOW.md  # iOS simulator development guide
  plans/
    PLAN_001_SQLITE_IOS_SIMULATOR.md  # iOS simulator SQLite build plan
  exploration/
    EXPLORE_001_LILYPOND_INTEGRATION.md      # LilyPond research
    EXPLORE_002_SMARTWATCH_INTEGRATION.md    # Smartwatch research
    EXPLORE_003_LEARNING_AND_PLAYBACK.md     # Learning features research
    EXPLORE_004_FOLKFRIEND_ANALYSIS.md       # FolkFriend competitive analysis & implementation roadmap
  analysis/
    easyabc-ecosystem-analysis.md            # EasyABC ecosystem state

TunepalGodot/Scripts/
  test_tunepal.gd            # Edit distance test suite

CONTRIBUTING.md              # Contributor guidelines
LICENSE                      # MIT License
ROADMAP.md                   # Development roadmap
```

### Modified Files

```
README.md                    # Expanded with attribution, status tables
.gitignore                   # Added built extension binaries
TunepalGodot/addons/tunepal/bin/tunepal.gdextension  # Added iOS entries
TunepalGodot/addons/godot-sqlite/gdsqlite.gdextension # Updated to xcframework
```

---

## What's Working

Building on your foundation, here's the current state:

| Feature | Status | Your Implementation | This Fork's Additions |
|---------|--------|---------------------|----------------------|
| Audio Recording | Working | Your code | - |
| Frequency Analysis | Working | Your code | - |
| Note Recognition | Working | Your code | - |
| ABC String Generation | Working | Your code | - |
| Edit Distance Search | Working | Your C++ GDExtension | Added test suite |
| Multi-threaded Search | Working | Your code | - |
| Keyword Search | Working | Your code | Fixed font rendering |
| SQLite Database | Working | Your integration | iOS xcframework support |
| iOS Build | Ready | Configured | Added to CI, export presets |
| CI/CD | New | - | Full pipeline for all platforms |

---

## What's In Progress / Planned

These are directions I'm exploring, aligned with your documented priorities:

### Your Priority 1: "Score visualization - Probably using ABCJS"
**Status:** Research complete in exploration docs
**Next Step:** WebView integration research

### Your Priority 2: "Playback - Using abc2midi and Godot MIDI Player"
**Status:** Deep exploration in `EXPLORE_003_LEARNING_AND_PLAYBACK.md`
**Highlights:**
- The Godot MIDI Player addon already has tempo control, transposition, per-channel volume
- Proposed enhancements: A-B loops, multi-instrument accompaniment, "lilt" for authentic Irish sound
- All aligned with your vision of "repeat sections and slowdown"

### Your Priority 3: "Local tune management"
**Status:** Not yet addressed

### Your Priority 4: "Searching by playing performance improvements"
**Status:** Not yet addressed

---

## The Exploration Documents in Detail

I want to call special attention to the exploration documents, as they represent significant research effort:

### LilyPond Integration (`EXPLORE_001_LILYPOND_INTEGRATION.md`)
- **Recommendation:** Add as secondary PDF export format, not primary
- **Why:** Publication-quality output for teachers, session organizers, publishers
- **Effort estimate:** 12-24 hours for basic integration
- **Respects:** ABC remains the source of truth (per your architecture)

### Smartwatch Integration (`EXPLORE_002_SMARTWATCH_INTEGRATION.md`)
- **Use Case:** Tap your watch at a session, identify the tune without fumbling for phone
- **Technical:** Both watchOS and Wear OS support audio recording with sufficient quality
- **Architecture:** Watch records, phone processes (leveraging your existing codebase)
- **Differentiator:** No competitor does traditional tune recognition on watch

### Learning & Playback (`EXPLORE_003_LEARNING_AND_PLAYBACK.md`)
This is the most extensive exploration, covering:

1. **Tempo Control** - Building on existing MIDI player capabilities
2. **Transposition** - Already exists, proposed UI improvements
3. **A-B Loop** - "repeat sections" from your vision
4. **Multi-Instrument Playback** - "sessions" from your vision
   - Dynamically generate bodhrán, guitar, bouzouki accompaniment
   - Per-instrument mixing
   - No competitor generates backing tracks from ABC for 24,000+ tunes
5. **Lilt/Expression** - Critical for authentic Irish sound
   - Timing variations, swing, velocity dynamics
   - Per-tune-type profiles (reel vs jig vs hornpipe)
6. **iOS MIDI Interoperability** - Send MIDI to Loopy Pro, GarageBand, AUM

**Key Finding:** The Godot MIDI Player already has much of the infrastructure. We're extending, not rebuilding.

### EasyABC Analysis (`easyabc-ecosystem-analysis.md`)
Analysis of whether to fork EasyABC for ABC editing capabilities. **Conclusion:** Don't fork - contribute upstream instead. Main repo is actively maintained.

---

## What I'm NOT Doing

To be clear about boundaries:

1. **Not competing** - This fork exists to contribute, not to replace
2. **Not commercializing** - Everything remains free and open source
3. **Not deviating** - All work follows your stated architecture and vision
4. **Not rushing** - Quality over speed, contribution-ready code
5. **Not claiming credit** - This is your project; I'm just helping

---

## Proposed Next Steps

I'd love to hear your thoughts on any of the following:

### Option A: Merge Back to Upstream
I can prepare PRs for your repository with any/all of this work:
- CI/CD pipeline
- iOS configuration
- Test suite
- Documentation improvements
- Exploration documents

### Option B: Collaborate on Your Repo
I'm happy to work directly in your repository instead of this fork:
- You maintain control
- I contribute as a regular contributor
- Fork becomes unnecessary

### Option C: Continue Parallel Development
If you prefer, I can:
- Keep this fork for experimental features
- Contribute stable work upstream
- Keep you informed of explorations

### Option D: Something Else Entirely
I'm open to whatever works best for you and the project.

---

## How to Reach Me

I'd be honored to discuss any of this with you. Whether it's a quick email, a video call, or meeting at a session somewhere - whatever works for you.

The goal is simple: serve the traditional music community, honor your incredible work, and help Tunepal Nua reach its full potential.

---

## Closing Thoughts

Tunepal has been part of my musical journey for years. The moment I discovered your Godot rebuild was public, I knew I wanted to help. Not because I think I can do it better - you clearly know what you're doing - but because the traditional music community deserves to have this tool thrive.

Your vision of "one project for all platforms," "offline-first," and "free and open-source" resonates deeply. These aren't just technical requirements; they're expressions of respect for musicians who might be playing tunes in places without reliable internet, on devices they can afford, at sessions where every moment counts.

Whatever form our collaboration takes - or even if you'd prefer I step back entirely - I'm grateful for the opportunity to contribute to something that matters.

*Slan go foill,*

A Fellow Traditional Musician

---

## Appendix: Quick Links

| Resource | URL |
|----------|-----|
| Your Upstream Repo | https://github.com/skooter500/TunepalGodot |
| This Fork | https://github.com/sirfifer/tunepalgodot-d |
| Your Blog: Godot Rebuild | https://bryanduggan.org/2024/04/01/new-tunepal-using-godot/ |
| Your Blog: Nua Launch | https://bryanduggan.org/2025/03/31/tunepal-nua-to-launch-this-summer/ |
| The Session Discussion | https://thesession.org/discussions/49580 |
| Original Tunepal | https://tunepal.org |

---

*This document: `docs/FORK_OVERVIEW_FOR_BRYAN.md`*
*Last updated: December 8, 2025*
*Updated to include: AI workflow transparency section, FolkFriend competitive analysis*
