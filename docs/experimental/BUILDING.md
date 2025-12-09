# Building the Experimental Detection Library

**Document Type:** Build Instructions
**Date:** 2025-12-09
**Status:** Active Development

---

## Table of Contents

1. [Prerequisites](#1-prerequisites)
2. [Quick Build Commands](#2-quick-build-commands)
3. [Platform-Specific Instructions](#3-platform-specific-instructions)
4. [Verifying the Build](#4-verifying-the-build)
5. [Troubleshooting](#5-troubleshooting)

---

## 1. Prerequisites

### Required Software

| Software | Version | Purpose |
|----------|---------|---------|
| Python | 3.9+ | SCons build system |
| SCons | 4.0+ | Build automation |
| Clang/GCC | Latest | C++ compiler |
| Godot Engine | 4.2+ | Testing |

### Godot-CPP Submodule

The `godot-cpp` submodule must be initialized:

```bash
cd /path/to/tunepalgodot-d
git submodule update --init --recursive
```

### Verify Prerequisites

```bash
# Check Python
python3 --version

# Check SCons
scons --version

# Check compiler (macOS)
clang++ --version

# Check godot-cpp
ls godot-cpp/
```

---

## 2. Quick Build Commands

### Build Original Library Only (Default)

```bash
scons platform=macos target=template_debug
```

### Build Experimental Library

```bash
BUILD_EXPERIMENTAL=1 scons platform=macos target=template_debug experimental
```

### Build Both Libraries

```bash
# First build original
scons platform=macos target=template_debug

# Then build experimental
BUILD_EXPERIMENTAL=1 scons platform=macos target=template_debug experimental
```

### Clean Build

```bash
# Clean all build artifacts
scons -c

# Rebuild
BUILD_EXPERIMENTAL=1 scons platform=macos target=template_debug experimental
```

---

## 3. Platform-Specific Instructions

### macOS (Development)

**Debug Build:**
```bash
BUILD_EXPERIMENTAL=1 scons platform=macos target=template_debug experimental
```

**Release Build:**
```bash
BUILD_EXPERIMENTAL=1 scons platform=macos target=template_release experimental
```

**Output:**
```
TunepalGodot/addons/tunepal_experimental/bin/
└── libtunepal_experimental.macos.template_debug.framework/
    └── libtunepal_experimental.macos.template_debug
```

**Architecture:** Universal binary (x86_64 + arm64)

---

### iOS Device

**Debug Build:**
```bash
BUILD_EXPERIMENTAL=1 scons platform=ios arch=arm64 target=template_debug experimental
```

**Release Build:**
```bash
BUILD_EXPERIMENTAL=1 scons platform=ios arch=arm64 target=template_release experimental
```

**Output:**
```
TunepalGodot/addons/tunepal_experimental/bin/
└── libtunepal_experimental.ios.template_debug.arm64.dylib
```

---

### iOS Simulator

**ARM64 Simulator (Apple Silicon Mac):**
```bash
BUILD_EXPERIMENTAL=1 scons platform=ios arch=arm64 ios_simulator=yes target=template_debug experimental
```

**Output:**
```
TunepalGodot/addons/tunepal_experimental/bin/
└── libtunepal_experimental.ios.template_debug.arm64.simulator.dylib
```

---

### Windows (Planned)

```bash
BUILD_EXPERIMENTAL=1 scons platform=windows target=template_debug experimental
```

---

### Linux (Planned)

```bash
BUILD_EXPERIMENTAL=1 scons platform=linux target=template_debug experimental
```

---

## 4. Verifying the Build

### Check Library Exists

```bash
ls -la TunepalGodot/addons/tunepal_experimental/bin/
```

Expected output:
```
libtunepal_experimental.macos.template_debug.framework/
tunepal_experimental.gdextension
```

### Check Library Architecture (macOS)

```bash
file TunepalGodot/addons/tunepal_experimental/bin/libtunepal_experimental.macos.template_debug.framework/libtunepal_experimental.macos.template_debug
```

Expected output:
```
Mach-O universal binary with 2 architectures:
[x86_64:Mach-O 64-bit dynamically linked shared library x86_64]
[arm64:Mach-O 64-bit dynamically linked shared library arm64]
```

### Check Entry Point Symbol

```bash
nm TunepalGodot/addons/tunepal_experimental/bin/libtunepal_experimental.macos.template_debug.framework/libtunepal_experimental.macos.template_debug | grep tunepal
```

Expected output:
```
00000000000007d4 T _tunepal_experimental_library_init
```

### Test in Godot Editor

1. Open the TunepalGodot project in Godot
2. Check Output panel for errors during "Verifying GDExtensions..."
3. Run this script to test:

```gdscript
# Create a new script or run in _ready()
func _ready():
    if ClassDB.class_exists("TunepalExperimental"):
        var exp = ClassDB.instantiate("TunepalExperimental")
        exp.say_hello()
        print("Version: ", exp.get_version())
        print("YIN Threshold: ", exp.get_yin_threshold())
    else:
        print("ERROR: TunepalExperimental not found!")
```

---

## 5. Troubleshooting

### Problem: "experimental=yes" Ignored

**Symptom:** Warning "Unknown SCons variables were passed and will be ignored"

**Solution:** Use the environment variable approach:
```bash
BUILD_EXPERIMENTAL=1 scons platform=macos target=template_debug experimental
```

---

### Problem: Library Not Found at Runtime

**Symptom:** GDScript reports `ClassDB.class_exists("TunepalExperimental")` returns false

**Possible Causes:**

1. **GDExtension file missing or incorrect:**
   ```bash
   cat TunepalGodot/addons/tunepal_experimental/bin/tunepal_experimental.gdextension
   ```
   Verify `entry_symbol = "tunepal_experimental_library_init"`

2. **Library path mismatch:**
   Check that the `.gdextension` file paths match the actual built files

3. **Library not built for current platform:**
   Rebuild for your platform

---

### Problem: Compilation Errors

**Symptom:** Compiler errors about missing headers

**Solution:** Ensure godot-cpp is built first:
```bash
cd godot-cpp
scons platform=macos target=template_debug
cd ..
BUILD_EXPERIMENTAL=1 scons platform=macos target=template_debug experimental
```

---

### Problem: iOS Simulator Build Fails

**Symptom:** Wrong architecture or codesigning issues

**Solution:**
1. Build with correct architecture:
   ```bash
   BUILD_EXPERIMENTAL=1 scons platform=ios arch=arm64 ios_simulator=yes target=template_debug experimental
   ```

2. Re-sign the dylib:
   ```bash
   codesign -f -s - TunepalGodot/addons/tunepal_experimental/bin/libtunepal_experimental.ios.template_debug.arm64.simulator.dylib
   ```

---

### Problem: Godot Editor Hangs on Load

**Symptom:** Godot freezes when opening project

**Solution:**
1. Remove the experimental addon temporarily:
   ```bash
   mv TunepalGodot/addons/tunepal_experimental TunepalGodot/addons/tunepal_experimental.bak
   ```
2. Open Godot to diagnose
3. Check build matches Godot version (4.2+)

---

## Build Configuration Reference

### SConstruct Variables

| Variable | Description |
|----------|-------------|
| `platform` | Target platform (macos, ios, windows, linux) |
| `target` | Build type (template_debug, template_release) |
| `arch` | Architecture (arm64, x86_64) |
| `ios_simulator` | Build for iOS Simulator (yes/no) |

### Environment Variables

| Variable | Description |
|----------|-------------|
| `BUILD_EXPERIMENTAL` | Set to "1" to build experimental library |

### Output Locations

| Platform | Output Path |
|----------|-------------|
| macOS | `addons/tunepal_experimental/bin/libtunepal_experimental.macos.*.framework/` |
| iOS | `addons/tunepal_experimental/bin/libtunepal_experimental.ios.*.dylib` |
| Windows | `addons/tunepal_experimental/bin/libtunepal_experimental.windows.*.dll` |
| Linux | `addons/tunepal_experimental/bin/libtunepal_experimental.linux.*.so` |

---

## Related Documentation

- [README.md](README.md) - Overview and quick start
- [ARCHITECTURE.md](ARCHITECTURE.md) - Technical design
- [API_REFERENCE.md](API_REFERENCE.md) - API documentation
- [TUNING_GUIDE.md](TUNING_GUIDE.md) - Parameter tuning
