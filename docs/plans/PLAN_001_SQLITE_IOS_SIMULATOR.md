# Plan 001: Build godot-sqlite for iOS Simulator

**Status:** Ready for local execution
**Requires:** Local Claude Code session on macOS with Xcode
**Estimated effort:** 30-60 minutes
**Priority:** High (blocks iOS Simulator development)

---

## Problem Statement

The godot-sqlite addon provides `ios.arm64` binaries compiled for **iOS devices**, but the iOS Simulator requires binaries compiled for the **Simulator SDK**:

| Target | Architecture | SDK | Current Status |
|--------|--------------|-----|----------------|
| iOS Device | arm64 | iphoneos | ✅ Provided |
| iOS Simulator (Apple Silicon) | arm64 | iphonesimulator | ❌ Missing |
| iOS Simulator (Intel Mac) | x86_64 | iphonesimulator | ❌ Missing |

The difference is subtle but critical: device arm64 and simulator arm64 use different SDKs and are not interchangeable.

---

## Solution Overview

1. Clone the godot-sqlite repository
2. Build for iOS Simulator targets (arm64-simulator and x86_64-simulator)
3. Update the gdextension file to include simulator entries
4. Test in iOS Simulator

---

## Detailed Steps

### Step 1: Prerequisites Check

```bash
# Verify Xcode is installed
xcode-select -p
# Should output: /Applications/Xcode.app/Contents/Developer

# Verify iOS Simulator SDK is available
xcrun --sdk iphonesimulator --show-sdk-path
# Should output something like: /Applications/Xcode.app/.../iPhoneSimulator.sdk

# Check available simulators
xcrun simctl list devices available
```

### Step 2: Clone godot-sqlite

```bash
# Create a working directory (outside the project)
cd ~/Development
git clone https://github.com/2shady4u/godot-sqlite.git
cd godot-sqlite

# Initialize submodules (includes godot-cpp)
git submodule update --init --recursive
```

### Step 3: Build for iOS Simulator

The key is using the `iphonesimulator` SDK instead of `iphoneos`.

```bash
# Build for Apple Silicon Simulator (arm64)
scons platform=ios target=template_debug arch=arm64 ios_simulator=yes -j4
scons platform=ios target=template_release arch=arm64 ios_simulator=yes -j4

# Build for Intel Mac Simulator (x86_64)
scons platform=ios target=template_debug arch=x86_64 ios_simulator=yes -j4
scons platform=ios target=template_release arch=x86_64 ios_simulator=yes -j4
```

**Note:** If `ios_simulator=yes` flag doesn't exist in the SConstruct, you may need to modify the build script or use manual compilation (see Alternative Approach below).

### Step 4: Alternative Approach (Manual Compilation)

If the scons build doesn't support simulator targets directly:

```bash
# Find the SDK path
SIMULATOR_SDK=$(xcrun --sdk iphonesimulator --show-sdk-path)

# Build godot-cpp for simulator first
cd godot-cpp
scons platform=ios target=template_debug arch=arm64 ios_simulator=yes -j4
cd ..

# Then build godot-sqlite
# The exact command depends on the SConstruct file structure
# May need to examine and modify SConstruct to add simulator support
```

### Step 5: Copy Built Libraries

After successful build, copy the libraries to the project:

```bash
# From godot-sqlite build directory
cp bin/libgdsqlite.ios.template_debug.arm64.simulator.dylib \
   ~/path/to/tunepalgodot-d/TunepalGodot/addons/godot-sqlite/bin/

cp bin/libgdsqlite.ios.template_release.arm64.simulator.dylib \
   ~/path/to/tunepalgodot-d/TunepalGodot/addons/godot-sqlite/bin/

# For Intel Mac support (optional but recommended)
cp bin/libgdsqlite.ios.template_debug.x86_64.simulator.dylib \
   ~/path/to/tunepalgodot-d/TunepalGodot/addons/godot-sqlite/bin/

cp bin/libgdsqlite.ios.template_release.x86_64.simulator.dylib \
   ~/path/to/tunepalgodot-d/TunepalGodot/addons/godot-sqlite/bin/
```

### Step 6: Update gdextension File

Edit `TunepalGodot/addons/godot-sqlite/gdsqlite.gdextension`:

```ini
[libraries]
# ... existing entries ...

# ADD THESE NEW ENTRIES for iOS Simulator
ios.arm64.simulator = "res://addons/godot-sqlite/bin/libgdsqlite.ios.template_debug.arm64.simulator.dylib"
ios.template_release.arm64.simulator = "res://addons/godot-sqlite/bin/libgdsqlite.ios.template_release.arm64.simulator.dylib"
ios.x86_64.simulator = "res://addons/godot-sqlite/bin/libgdsqlite.ios.template_debug.x86_64.simulator.dylib"
ios.template_release.x86_64.simulator = "res://addons/godot-sqlite/bin/libgdsqlite.ios.template_release.x86_64.simulator.dylib"

[dependencies]
# ... existing entries ...

# ADD THESE
ios.arm64.simulator = {}
ios.template_release.arm64.simulator = {}
ios.x86_64.simulator = {}
ios.template_release.x86_64.simulator = {}
```

**Important:** The exact key format for simulator may vary. Check Godot 4.3 documentation for the correct gdextension format for simulator targets. It might be:
- `ios.arm64.simulator`
- `ios.simulator.arm64`
- `ios_simulator.arm64`

### Step 7: Test in iOS Simulator

```bash
# Open Godot editor
cd ~/path/to/tunepalgodot-d/TunepalGodot
godot

# In Godot:
# 1. Go to Project > Export
# 2. Select iOS preset
# 3. Export to Xcode project

# In Xcode:
# 1. Open the exported project
# 2. Select an iOS Simulator target
# 3. Build and run
# 4. Verify app launches and database queries work
```

---

## Troubleshooting

### "Library not found" in Simulator

- Verify the library architecture: `lipo -info libgdsqlite.*.dylib`
- Should show `arm64` for simulator, NOT just device arm64
- Use `file` command to check: `file libgdsqlite.*.dylib`
  - Device: `Mach-O 64-bit dynamically linked shared library arm64`
  - Simulator: Should mention simulator or show different load commands

### Build Errors in godot-sqlite

- Ensure godot-cpp is built for simulator first
- Check that you're using the correct SDK path
- May need to modify SConstruct to add simulator platform detection

### Godot Doesn't Recognize Simulator Library

- Check gdextension key format matches what Godot expects
- Try different key formats (ios.simulator.arm64, ios.arm64.simulator, etc.)
- Check Godot export logs for which library paths it's looking for

---

## Alternative: Use Pre-built Binaries

Check if someone has already built simulator binaries:

1. Check [godot-sqlite releases](https://github.com/2shady4u/godot-sqlite/releases) for simulator builds
2. Search GitHub issues for "simulator" - may have community solutions
3. Check Godot Asset Library for alternative SQLite plugins with simulator support

---

## Success Criteria

- [ ] iOS Simulator launches Tunepal app without crash
- [ ] Keywords view shows tune data (database queries work)
- [ ] Search functionality works
- [ ] Can use ios-simulator-mcp for automated testing

---

## Notes for Local Claude Code Session

When executing this plan locally:

1. Start in the godot-sqlite clone directory
2. Examine the `SConstruct` file to understand build options
3. If `ios_simulator=yes` isn't supported, you may need to:
   - Add simulator support to SConstruct
   - Or manually invoke clang with correct SDK
4. The key difference is: `-sdk iphonesimulator` instead of `-sdk iphoneos`

---

## Follow-up Tasks

After this is working:
- [ ] Document the exact build commands that worked
- [ ] Consider contributing simulator support to godot-sqlite upstream
- [ ] Set up ios-simulator-mcp for automated UI testing
- [ ] Add simulator testing to CI/CD pipeline

---

*Created: 2025-12-07*
*For execution in: Local Claude Code session on macOS*
