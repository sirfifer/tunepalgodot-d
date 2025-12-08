#!/bin/bash
# One-command iOS Simulator testing for Tunepal
#
# Usage:
#   ./scripts/test-ios-simulator.sh           - Launch existing build
#   ./scripts/test-ios-simulator.sh --rebuild - Full rebuild and launch
#   ./scripts/test-ios-simulator.sh --help    - Show this help
#
# Requirements:
#   - Xcode with iOS Simulator
#   - Godot 4.5.1+ with iOS export templates
#   - Previous successful build (unless using --rebuild)

set -e

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
GODOT_PROJECT="$PROJECT_ROOT/TunepalGodot"
EXPORT_DIR="$PROJECT_ROOT/export"
BUNDLE_ID="org.tunepal.app"
SIMULATOR="iPhone 17 Pro"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

show_help() {
    echo "iOS Simulator Test Script for Tunepal"
    echo ""
    echo "Usage:"
    echo "  $0           - Launch existing build in Simulator"
    echo "  $0 --rebuild - Export from Godot, build in Xcode, then launch"
    echo "  $0 --help    - Show this help message"
    echo ""
    echo "Options:"
    echo "  --rebuild    Full rebuild: export from Godot and compile with Xcode"
    echo "  --simulator  Specify simulator name (default: iPhone 17 Pro)"
    echo ""
    echo "Examples:"
    echo "  $0                                    # Quick launch"
    echo "  $0 --rebuild                          # Full rebuild"
    echo "  $0 --simulator 'iPad Pro 13-inch'     # Use different simulator"
}

# Parse arguments
REBUILD=false
while [[ $# -gt 0 ]]; do
    case $1 in
        --rebuild)
            REBUILD=true
            shift
            ;;
        --simulator)
            SIMULATOR="$2"
            shift 2
            ;;
        --help|-h)
            show_help
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            show_help
            exit 1
            ;;
    esac
done

echo -e "${GREEN}=== Tunepal iOS Simulator Test ===${NC}"
echo "Simulator: $SIMULATOR"
echo ""

if [ "$REBUILD" = true ]; then
    echo -e "${YELLOW}=== Step 1/3: Exporting from Godot ===${NC}"
    cd "$GODOT_PROJECT"
    godot --headless --export-debug "iOS" ../export/tunepal-ios.xcodeproj 2>&1 | grep -E "(ERROR|DONE|error)" || true

    echo ""
    echo -e "${YELLOW}=== Step 2/3: Building for Simulator ===${NC}"
    cd "$EXPORT_DIR"
    xcodebuild -project tunepal-ios.xcodeproj \
      -scheme tunepal-ios \
      -destination "platform=iOS Simulator,name=$SIMULATOR" \
      -configuration Debug \
      build 2>&1 | grep -E "(BUILD|error:|warning:)" | tail -10

    if [ ${PIPESTATUS[0]} -ne 0 ]; then
        echo -e "${RED}Build failed! Check Xcode for details.${NC}"
        exit 1
    fi
    echo ""
fi

echo -e "${YELLOW}=== Launching in Simulator ===${NC}"

# Boot simulator if not running
xcrun simctl boot "$SIMULATOR" 2>/dev/null || true

# Find the built app
APP_PATH=$(find ~/Library/Developer/Xcode/DerivedData/tunepal-ios-*/Build/Products/Debug-iphonesimulator -name "tunepal-ios.app" -type d 2>/dev/null | head -1)

if [ -z "$APP_PATH" ]; then
    echo -e "${RED}Error: No build found. Run with --rebuild first.${NC}"
    exit 1
fi

echo "Installing from: $APP_PATH"

# Install and launch
xcrun simctl install booted "$APP_PATH"
open -a Simulator
sleep 1
xcrun simctl launch booted "$BUNDLE_ID"

echo ""
echo -e "${GREEN}=== Tunepal launched in $SIMULATOR ===${NC}"
echo ""
echo "To verify SQLite is working:"
echo "  1. Tap on 'Keywords' in the app"
echo "  2. Check that tune data appears"
echo ""
