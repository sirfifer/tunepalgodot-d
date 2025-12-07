# Tunepal Roadmap

This roadmap captures the vision and development priorities for Tunepal Nua - the Godot-based rebuild of Tunepal.

## Bryan Duggan's Original Vision

> *The following is preserved from [Bryan Duggan's upstream repository](https://github.com/skooter500/TunepalGodot) and represents the guiding vision for this project.*

The goal is to create a new, modern Tunepal that solves a number of problems with the original, while providing a platform for future development and research. The plan is to rebuild Tunepal from the ground up using Godot for its cross-platform capabilities and support for extensions in different languages.

### The New Tunepal Will Have:

- One project for all platforms: Android, iOS and Web
- Support for different screen sizes including tablets
- Fast, offline searches, running on device
- Better accuracy
- Updated corpus with the latest available transcriptions to ABC of the classic sources
- Interactive music scores with playback, repeat sections and slowdown
- Support for full range of instruments, sessions and recordings
- Import new sources
- Europeana sounds integration - connection to the Comhaltas archive, ITMA and other sources
- Backup Tunebooks in the cloud and share across devices
- Share Tunes and sets with other users
- Integration with thesession.org
- Platform for future research and development
- Free and open-source

### Bryan's Priority Tasks:

> "These are the broad tasks!" - Bryan Duggan

1. **Searching by playing performance improvements** - Needs to take a few seconds max
2. **Score visualization** - Probably using ABCJS. Needs a way to render a local HTML page in Godot via a "WebView" type of node
3. **Playback** - Using abc2midi and [Godot MIDI Player](https://godotengine.org/asset-library/asset/1667)
4. **Local tune management**

---

## Current Status

See the [README](README.md#development-status) for detailed status tables.

**Working:** Audio recording, frequency analysis, note recognition, ABC string generation, edit distance search, multi-threaded search, keyword search, SQLite database, menu navigation

**In Progress:** iOS build (ready, needs TestFlight), score visualization, playback, My Tunes persistence, cloud sync

---

## Development Priorities

### Phase 1: Core MVP (Current Focus)

These align directly with Bryan's priority tasks:

| Priority | Feature | Status | Notes |
|----------|---------|--------|-------|
| P0 | Search performance optimization | Not started | Target: <3 seconds |
| P0 | iOS TestFlight release | Ready to test | Extension built, needs device testing |
| P1 | Score visualization (ABCJS) | Not started | Requires WebView integration |
| P1 | Playback (MIDI) | Not started | Godot MIDI Player addon included |
| P2 | Local tune management | Not started | "My Tunes" persistence |

### Phase 2: Feature Parity

Building toward full feature set:

| Feature | Status | Notes |
|---------|--------|-------|
| Android release | Partially tested | Extension builds, needs more testing |
| Web (WASM) release | Not tested | Extension configured |
| Tablet UI optimization | Not started | Support different screen sizes |
| Import tune sources | Not started | User-provided ABC collections |

### Phase 3: Enhanced Features

Future enhancements from Bryan's vision:

| Feature | Status | Notes |
|---------|--------|-------|
| thesession.org integration | Not started | External API integration |
| Europeana sounds integration | Not started | Comhaltas archive, ITMA, etc. |
| Cloud backup | Not started | Sync tunebooks across devices |
| Tune/set sharing | Not started | Share with other users |
| Updated corpus | Not started | Latest ABC transcriptions |

---

## Architecture Notes

### Offline-First Design

The Godot rebuild fundamentally changes the architecture:

| Component | Original Tunepal | Tunepal Nua |
|-----------|-----------------|-------------|
| Tune Search | Server-side (tunepal.org) | On-device (C++ GDExtension) |
| Tune Database | Server MySQL | Embedded SQLite (48MB) |
| Transcription | On-device | On-device |
| Internet Required | Yes, for search | No (core features work offline) |

This means:
- **No server dependency for core functionality**
- Cloud features (backup, sharing) are optional enhancements
- External integrations (thesession.org, Europeana) will be optional

### Server-Side API (Reference)

The original Tunepal API server exists at [skooter500/tunepal-api](https://github.com/skooter500/tunepal-api). This is separate from the Godot rebuild and maintained by Bryan for the legacy system.

---

## Contributing to the Roadmap

### How Issues Become Roadmap Items

1. **Submit an issue** - Use the bug report or feature request template
2. **Triage** - Issues are reviewed and labeled
3. **Promotion** - Significant items get the `roadmap` label
4. **Prioritization** - Roadmap items are ordered by priority
5. **Implementation** - Work begins, issue linked to PR

### Issue Labels

| Label | Purpose |
|-------|---------|
| `bug` | Something isn't working |
| `enhancement` | New feature or improvement |
| `roadmap` | Promoted to roadmap tracking |
| `priority: high` | Critical path item |
| `priority: medium` | Important but not blocking |
| `priority: low` | Nice to have |
| `area: search` | Search/matching functionality |
| `area: ui` | User interface |
| `area: audio` | Audio recording/playback |
| `area: platform` | Platform-specific (iOS, Android, etc.) |
| `good first issue` | Good for new contributors |
| `help wanted` | Looking for contributors |

---

## Upstream Alignment

This fork aims to contribute improvements back to Bryan's [upstream repository](https://github.com/skooter500/TunepalGodot). All roadmap work should:

1. Follow Bryan's stated vision
2. Be contribution-ready (clean, documented)
3. Respect the existing architecture
4. Maintain cross-platform compatibility

---

## References

- [Bryan's Blog: Tunepal being rebuilt in Godot](https://bryanduggan.org/2024/04/01/new-tunepal-using-godot/)
- [Bryan's Blog: Tunepal Nua Launch Announcement](https://bryanduggan.org/2025/03/31/tunepal-nua-to-launch-this-summer/)
- [Upstream Repository](https://github.com/skooter500/TunepalGodot)
- [Original Tunepal](https://tunepal.org)
- [The Session Discussion](https://thesession.org/discussions/49580)
