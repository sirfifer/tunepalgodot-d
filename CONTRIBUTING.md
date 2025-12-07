# Contributing to Tunepal Godot

Thank you for your interest in contributing to Tunepal! This project aims to serve the traditional Irish music community, and contributions are welcome.

## Original Vision

This project follows the vision of Dr. Bryan Duggan, the original creator of Tunepal. Please review the [README](README.md) and Bryan's [blog post](https://bryanduggan.org/2024/04/01/new-tunepal-using-godot/) to understand the goals.

### Core Principles

1. **Free and Open Source** - Tunepal should remain free for all traditional musicians
2. **Offline-First** - No dependency on backend servers; everything runs on-device
3. **Cross-Platform** - One codebase for iOS, Android, and Web
4. **Faithful to Tradition** - Respect the traditional music community this serves

## Development Setup

### Prerequisites

- [Godot 4.3+](https://godotengine.org/download) (Standard or .NET version)
- Python 3.x with SCons (`pip install scons`)
- C++ compiler (GCC, Clang, or MSVC)
- For iOS: macOS with Xcode
- For Android: Android NDK r23c

### Building the C++ Extension

```bash
# Initialize submodules
git submodule update --init --recursive

# Build for your platform
scons platform=linux target=template_debug -j4    # Linux
scons platform=macos target=template_debug -j4    # macOS
scons platform=ios target=template_debug arch=arm64 -j4  # iOS
scons platform=windows target=template_debug -j4  # Windows
scons platform=android target=template_debug arch=arm64 -j4  # Android
```

### Running Tests

Open `TunepalGodot/Scenes/test_tunepal.tscn` in Godot and run it to execute the test suite.

## Code Style

- **GDScript**: Follow [Godot's GDScript style guide](https://docs.godotengine.org/en/stable/tutorials/scripting/gdscript/gdscript_styleguide.html)
- **C++**: Use the existing code style in `src/`
- Keep changes focused and minimal
- Document non-obvious code with comments

## Pull Request Process

1. Fork the repository
2. Create a feature branch from `main`
3. Make your changes
4. Run the test suite to ensure nothing is broken
5. Submit a pull request with a clear description

### Commit Messages

- Use clear, descriptive commit messages
- Reference issues when applicable (e.g., "Fixes #123")
- Keep commits focused on single changes

## Priority Areas

Current areas where contributions are especially welcome:

1. **Score Visualization** - Integrating ABCJS or similar for music notation display
2. **Playback** - Wiring up the Godot MIDI Player addon
3. **iOS Testing** - Testing on real iOS devices via TestFlight
4. **Performance** - Improving search speed (target: <3 seconds)
5. **UI/UX** - Improving the mobile interface

## Questions?

- Open an issue for bugs or feature discussions
- Check Bryan's blog for context on design decisions
- Be respectful of the traditional music community this project serves

## License

By contributing, you agree that your contributions will be licensed under the same license as the project.

---

*Go raibh maith agat!* (Thank you!)
