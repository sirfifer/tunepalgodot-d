# iOS Development & Testing Workflow

This document covers testing Tunepal on iOS - both in the Simulator and on physical devices.

---

## Quick Start

### Test in iOS Simulator (One Command)

```bash
# Launch existing build
./scripts/test-ios-simulator.sh

# Full rebuild and launch
./scripts/test-ios-simulator.sh --rebuild

# Use a different simulator
./scripts/test-ios-simulator.sh --simulator "iPad Pro 13-inch"
```

### Test on Physical Device

```bash
# Build and deploy via Xcode
open export/tunepal-ios.xcodeproj
# Select your device, then Cmd+R to run
```

---

## iOS Simulator Setup

### Prerequisites

| Tool | Required Version | Check Command |
|------|------------------|---------------|
| Xcode | 15+ | `xcodebuild -version` |
| Godot | 4.5.1+ | `godot --version` |
| iOS Simulator | iOS 14+ | Open Xcode > Window > Devices |

### First-Time Setup

1. **Install iOS Simulator** (if not present):
   ```bash
   xcodebuild -downloadPlatform iOS
   ```

2. **Verify Godot iOS export templates**:
   ```bash
   ls ~/Library/Application\ Support/Godot/export_templates/4.5.1.stable/ios.zip
   ```

3. **Initial build** (required before first test):
   ```bash
   ./scripts/test-ios-simulator.sh --rebuild
   ```

---

## Current Status

### What Works

| Feature | Status | Notes |
|---------|--------|-------|
| SQLite Database | **Working** | 24,000+ tunes load from Norbeck source |
| Keywords View | **Working** | Displays tune list, search works |
| Navigation | **Working** | All tab navigation functional |
| Recording UI | **Working** | Progress bar shows during 10-sec recording |
| Audio Recording | Partial | Simulator mic support varies |
| Edit Distance Matching | **Not Working** | Tunepal GDExtension not built for iOS yet |

### Known Limitation: Tunepal GDExtension

The Tunepal C++ extension (for edit distance matching) is **not yet available on iOS**. The app loads and runs, but tune matching after recording won't work until the extension is built as an xcframework. This is tracked for future work.

---

## SQLite Support

### Status: RESOLVED

SQLite now works in the iOS Simulator. The solution involved:

1. Building custom Godot iOS templates with arm64 simulator support
2. Building godot-sqlite with merged godot-cpp symbols as xcframeworks
3. Using `libtool -static` to properly merge static libraries
4. **Database copy function** in `record.gd` that copies 46MB database from PCK to `user://data/` on first launch

### Technical Details

The godot-sqlite xcframeworks are located at:
- `TunepalGodot/addons/godot-sqlite/bin/libgdsqlite.ios.template_debug.xcframework`
- `TunepalGodot/addons/godot-sqlite/bin/libgdsqlite.ios.template_release.xcframework`

Each xcframework contains:
- **Device slice**: arm64
- **Simulator slice**: arm64 + x86_64 (fat binary)

To verify SQLite is working:
1. Launch app in Simulator
2. Navigate to Keywords view
3. Tune data should load (this proves database queries work)

---

## Workflow Details

### Development Iteration

| Task | Command | Time |
|------|---------|------|
| Quick UI test | `./scripts/test-ios-simulator.sh` | ~5 sec |
| After code changes | `./scripts/test-ios-simulator.sh --rebuild` | ~2 min |
| Test on device | Open Xcode, Cmd+R | ~3 min |

### When to Rebuild

Use `--rebuild` when you've changed:
- Any GDScript files
- Scene files (.tscn)
- Resources
- Project settings

No rebuild needed for:
- Just re-testing the same build
- Switching simulators

---

## Physical Device Testing

### Via Xcode

1. Connect your iOS device via USB
2. Open the Xcode project:
   ```bash
   open export/tunepal-ios.xcodeproj
   ```
3. Select your device in the scheme dropdown
4. Press Cmd+R to build and run

### Via TestFlight

For broader testing:
1. Archive in Xcode (Product > Archive)
2. Upload to App Store Connect
3. Distribute via TestFlight

---

## Troubleshooting

### App won't launch in Simulator

```bash
# Check if simulator is running
xcrun simctl list devices booted

# Boot simulator manually
xcrun simctl boot "iPhone 17 Pro"

# Check if app is installed
xcrun simctl listapps booted | grep tunepal
```

### Build fails with undefined symbols

This usually means the xcframeworks need rebuilding. The fix:
```bash
# Full rebuild of godot-sqlite xcframeworks
# (Contact maintainer - this is a one-time setup)
```

### Simulator shows black screen

```bash
# Terminate and relaunch
xcrun simctl terminate booted org.tunepal.app
xcrun simctl launch booted org.tunepal.app
```

### Database queries return empty

Check that:
1. The database file exists: `user://data/tunepal.db`
2. SQLite library is loaded (check Xcode console for errors)
3. Keywords view attempts to load data

---

## Architecture Reference

### Build Artifacts

| File | Purpose |
|------|---------|
| `export/tunepal-ios.xcodeproj` | Xcode project (regenerated on export) |
| `export/tunepal-ios.pck` | Game data package |
| `export/tunepal-ios.xcframework` | Godot engine library |
| `TunepalGodot/addons/godot-sqlite/bin/*.xcframework` | SQLite extension |

### Platform Detection

```bash
# Check if library is for simulator (platform 7) or device (platform 2)
otool -l <library.a> | grep -A5 "LC_BUILD_VERSION"
```

### Architecture Verification

```bash
# Check architectures in fat binary
lipo -info <library.a>
# Expected for simulator: "x86_64 arm64"
```

---

## Advanced: Claude Code Integration

### ios-simulator-mcp

For automated UI testing via Claude Code:

```bash
# Install IDB
brew install idb-companion

# Add MCP server
claude mcp add ios-simulator --command npx --args ios-simulator-mcp
```

Available tools:
- `screenshot` - Capture screen
- `ui_tap` - Tap at coordinates
- `ui_type` - Input text
- `launch_app` - Launch installed app

---

## References

- [godot-sqlite](https://github.com/2shady4u/godot-sqlite) - SQLite GDExtension
- [godot-ios-plugins](https://github.com/godot-sdk-integrations/godot-ios-plugins) - iOS plugin build reference
- [Godot iOS Export](https://docs.godotengine.org/en/stable/tutorials/export/exporting_for_ios.html) - Official docs
