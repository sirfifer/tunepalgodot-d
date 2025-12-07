# iOS Simulator Development Workflow

This document outlines how to set up an efficient iOS development workflow for Tunepal using the iOS Simulator, including known issues and solutions.

## Known Issues

### SQLite in iOS Simulator

**Problem:** The godot-sqlite addon provides `ios.arm64` binaries, but these are compiled for **iOS devices**, not the **iOS Simulator**. The simulator uses different architectures:
- Apple Silicon Macs: `arm64-simulator` (different SDK than device arm64)
- Intel Macs: `x86_64-simulator`

**Symptoms:**
- App crashes on launch in Simulator
- SQLite operations fail silently
- Database queries return empty/null

**Current Workaround Options:**

1. **Test on Physical Device via TestFlight**
   - This bypasses the Simulator architecture issue entirely
   - The arm64 iOS binaries work on real devices

2. **Build godot-sqlite for Simulator** (Advanced)
   - Clone [godot-sqlite](https://github.com/2shady4u/godot-sqlite)
   - Build with simulator SDK: `xcrun -sdk iphonesimulator clang...`
   - Add simulator entries to `gdsqlite.gdextension`

3. **Use macOS Build for Development**
   - The macOS binary works on Apple Silicon
   - Use `godot --path TunepalGodot` to run directly
   - Faster iteration than Simulator

### Database Path for iOS

The app already handles iOS file system restrictions correctly in `record.gd`:

```gdscript
if OS.get_name() in ["Android", "iOS", "Web"]:
    copy_data_to_user()
    db_name = "user://data/tunepal"
```

This copies the database from `res://` (read-only) to `user://` (writable) on mobile platforms.

---

## MCP Server for iOS Simulator Automation

### ios-simulator-mcp

An MCP server that allows Claude to interact directly with the iOS Simulator for UI testing and screenshots.

**Installation:**

```bash
# Install Facebook IDB (required dependency)
brew install idb-companion

# For Claude Code, add to your MCP configuration
claude mcp add ios-simulator --command npx --args ios-simulator-mcp
```

**Or manually configure** in `~/.config/claude-code/mcp.json`:

```json
{
  "mcpServers": {
    "ios-simulator": {
      "command": "npx",
      "args": ["-y", "ios-simulator-mcp"]
    }
  }
}
```

**Available Tools:**
- `screenshot` - Capture current screen
- `ui_tap` - Simulate tap at coordinates
- `ui_type` - Input text
- `ui_swipe` - Perform swipe gestures
- `ui_describe_all` - Get accessibility tree
- `ui_describe_point` - Inspect element at point
- `launch_app` - Launch installed app
- `install_app` - Install .app bundle
- `record_video` / `stop_recording` - Video capture

**Requirements:**
- macOS with Xcode
- iOS Simulator installed
- Facebook IDB tool (`brew install idb-companion`)
- Node.js

**Usage Example:**
Once configured, Claude can:
- "Take a screenshot of the simulator"
- "Tap on the Keywords button"
- "Describe what's on screen"

---

## Claude Code + IDE Integration

### VS Code Extension

Claude Code has a VS Code extension that provides:
- Real-time diff viewing
- File reference shortcuts (Cmd+Option+K)
- Automatic diagnostic sharing
- Direct integration with terminal Claude Code

**Installation:**
The extension auto-installs when running Claude Code with VS Code open.

### Cursor IDE

Cursor is VS Code-based and supports the extension with a manual workaround:

```bash
cursor --install-extension ~/.claude/local/node_modules/@anthropic-ai/claude-code/vendor/claude-code.vsix
```

### Benefits for UI Development
- See changes in real-time through native GUI
- Access lint/syntax errors automatically
- Navigate code with file references
- Works alongside Cursor's native AI features

---

## Recommended Development Workflow

### For UI Iteration (Current Best Path)

1. **Make changes** in VS Code/Cursor with Claude Code
2. **Test on macOS** using Godot editor (fast iteration)
   ```bash
   cd TunepalGodot
   godot
   ```
3. **Build for iOS** when ready to test mobile-specific features
   ```bash
   scons platform=ios target=template_debug arch=arm64
   ```
4. **Test on real device** via TestFlight (bypasses Simulator SQLite issue)

### For Automated UI Testing (Future)

1. **Configure ios-simulator-mcp** as described above
2. **Build and install** app in Simulator (with SQLite fix applied)
3. **Use Claude Code** to:
   - Take screenshots for review
   - Automate UI interactions
   - Verify accessibility labels
   - Record bug reproduction videos

---

## Future Improvements

### Short-term
- [ ] Build godot-sqlite for iOS Simulator targets
- [ ] Set up ios-simulator-mcp for automated testing
- [ ] Create UI test suite using MCP tools

### Long-term
- [ ] CI/CD pipeline for Simulator testing
- [ ] Automated screenshot comparison
- [ ] Accessibility audit automation

---

## References

- [godot-sqlite](https://github.com/2shady4u/godot-sqlite) - SQLite GDExtension
- [ios-simulator-mcp](https://github.com/joshuayoes/ios-simulator-mcp) - MCP Server for iOS Simulator
- [Claude Code IDE Integrations](https://docs.anthropic.com/en/docs/claude-code/ide-integrations) - Official docs
- [XcodeBuildMCP](https://github.com/cameroncooke/XcodeBuildMCP) - Xcode automation via MCP
