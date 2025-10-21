#!/bin/bash

# Build Debian package for Burma Automaton

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

echo "=================================================="
echo " Building Debian Package for Burma Automaton"
echo "=================================================="
echo ""

# Check dependencies
echo "Checking build dependencies..."
MISSING_DEPS=""

dpkg -l | grep -q debhelper || MISSING_DEPS="$MISSING_DEPS debhelper"
dpkg -l | grep -q build-essential || MISSING_DEPS="$MISSING_DEPS build-essential"
dpkg -l | grep -q cmake || MISSING_DEPS="$MISSING_DEPS cmake"
dpkg -l | grep -q qt6-base-dev || MISSING_DEPS="$MISSING_DEPS qt6-base-dev"

if [ -n "$MISSING_DEPS" ]; then
    echo "ERROR: Missing build dependencies:$MISSING_DEPS"
    echo "Please install with:"
    echo "  sudo apt update"
    echo "  sudo apt install -y$MISSING_DEPS"
    exit 1
fi

echo "✓ Build dependencies ready"
echo ""

# Clean previous builds
echo "Cleaning previous builds..."
rm -f ../*.deb ../*.build ../*.changes ../*.buildinfo
rm -rf debian/burma-automaton debian/.debhelper debian/files debian/debhelper-build-stamp

echo "✓ Clean complete"
echo ""

# Build package
echo "Building Debian package..."
dpkg-buildpackage -us -uc -b

echo ""
echo "=================================================="
echo " Build Complete!"
echo "=================================================="
echo ""

# Find the .deb file
DEB_FILE=$(ls -t ../*.deb 2>/dev/null | head -1)

if [ -f "$DEB_FILE" ]; then
    echo "Debian package created:"
    echo "  $DEB_FILE"
    echo "  Size: $(du -h "$DEB_FILE" | cut -f1)"
    echo ""
    echo "To install:"
    echo "  sudo dpkg -i $DEB_FILE"
    echo "  sudo apt-get install -f  # Fix dependencies if needed"
    echo ""
    echo "To upload to Ubuntu Software Center:"
    echo "  1. Create account at https://snapcraft.io"
    echo "  2. Build snap: snapcraft"
    echo "  3. Upload: snapcraft upload burma-automaton_1.0.0_amd64.snap"
else
    echo "ERROR: Debian package not found!"
    exit 1
fi
