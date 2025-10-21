#!/bin/bash

set -e

VERSION="1.0.0"
APP_NAME="burma-automaton"
TARBALL_NAME="${APP_NAME}-${VERSION}-linux-amd64"

echo "=================================================="
echo " Building Burma Automaton Tarball Distribution"
echo "=================================================="
echo ""

# Check if binary exists
if [ ! -f "build/BurmaAutomaton" ]; then
    echo "Error: build/BurmaAutomaton not found"
    echo "Please run ./build.sh first to build the application"
    exit 1
fi

echo "Creating distribution directory..."
mkdir -p "${TARBALL_NAME}"

# Copy binary
echo "Copying application binary..."
cp build/BurmaAutomaton "${TARBALL_NAME}/"

# Copy resources
echo "Copying resources..."
mkdir -p "${TARBALL_NAME}/resources/icons"
cp -r resources/icons/* "${TARBALL_NAME}/resources/icons/" 2>/dev/null || true

# Copy documentation
echo "Copying documentation..."
cp QUICKSTART.md "${TARBALL_NAME}/" 2>/dev/null || true
cp RUN.md "${TARBALL_NAME}/" 2>/dev/null || true
cp LICENSE "${TARBALL_NAME}/" 2>/dev/null || true
cp README.md "${TARBALL_NAME}/" 2>/dev/null || true

# Create run script
echo "Creating launcher script..."
cat > "${TARBALL_NAME}/run.sh" << 'EOF'
#!/bin/bash
# Burma Automaton Launcher

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Check for BitNet.cpp
BITNET_CLI="$HOME/BitNet/build/bin/llama-cli"
BITNET_MODEL="$HOME/BitNet/models/BitNet-b1.58-2B-4T/ggml-model-i2_s.gguf"

if [ ! -f "$BITNET_CLI" ] || [ ! -f "$BITNET_MODEL" ]; then
    echo "WARNING: BitNet.cpp not found or not properly configured"
    echo ""
    echo "Expected files:"
    echo "  - $BITNET_CLI"
    echo "  - $BITNET_MODEL"
    echo ""
    echo "The application will run with fallback world generation."
    echo "For AI-powered world generation, please install BitNet.cpp:"
    echo ""
    echo "  git clone --recursive https://github.com/microsoft/BitNet.git ~/BitNet"
    echo "  cd ~/BitNet"
    echo "  python3 setup_env.py --hf-repo microsoft/BitNet-b1.58-2B-4T -q i2_s"
    echo "  mkdir -p build && cd build"
    echo "  cmake .. -DCMAKE_BUILD_TYPE=Release"
    echo "  make -j\$(nproc)"
    echo ""
    read -p "Press Enter to continue anyway..."
fi

./BurmaAutomaton
EOF

chmod +x "${TARBALL_NAME}/run.sh"

# Create installation instructions
echo "Creating installation instructions..."
cat > "${TARBALL_NAME}/INSTALL.txt" << 'EOF'
Burma Automaton Installation Instructions
=========================================

## Quick Start

1. Extract this archive to your desired location
2. Run the application:
   ./run.sh

   Or directly:
   ./BurmaAutomaton

## BitNet.cpp Setup (Optional but Recommended)

For AI-powered world generation, install BitNet.cpp:

```bash
# Clone BitNet repository
git clone --recursive https://github.com/microsoft/BitNet.git ~/BitNet
cd ~/BitNet

# Download model
python3 setup_env.py --hf-repo microsoft/BitNet-b1.58-2B-4T -q i2_s

# Build
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## System Requirements

- Ubuntu 22.04 or newer (or compatible Linux distribution)
- Qt6 libraries (install with: sudo apt install qt6-base-dev libqt6opengl6-dev)
- OpenGL support
- 4GB RAM minimum
- BitNet.cpp: 2GB disk space for model

## Dependencies

If the application fails to start due to missing libraries:

```bash
sudo apt update
sudo apt install -y libqt6core6 libqt6gui6 libqt6widgets6 \
    libqt6opengl6 libqt6openglwidgets6 libqt6network6 \
    libcurl4 libgl1
```

## Usage

See QUICKSTART.md for usage instructions.

## Support

For issues and questions:
- GitHub: https://github.com/leo007-htun/Burma_Automaton
- Documentation: See RUN.md for detailed usage

## License

See LICENSE file for licensing information.
EOF

# Create tarball
echo ""
echo "Creating tarball..."
tar -czf "${TARBALL_NAME}.tar.gz" "${TARBALL_NAME}"

# Calculate size and checksum
TARBALL_SIZE=$(du -h "${TARBALL_NAME}.tar.gz" | cut -f1)
TARBALL_SHA256=$(sha256sum "${TARBALL_NAME}.tar.gz" | cut -d' ' -f1)

# Clean up temp directory
rm -rf "${TARBALL_NAME}"

echo ""
echo "✓ Tarball created successfully!"
echo ""
echo "File: ${TARBALL_NAME}.tar.gz"
echo "Size: ${TARBALL_SIZE}"
echo "SHA256: ${TARBALL_SHA256}"
echo ""
echo "To test:"
echo "  tar -xzf ${TARBALL_NAME}.tar.gz"
echo "  cd ${TARBALL_NAME}"
echo "  ./run.sh"
echo ""
echo "To distribute:"
echo "  - Upload to GitHub Releases"
echo "  - Share direct download link"
echo "  - Host on website"
