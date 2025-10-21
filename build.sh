#!/bin/bash

# Burma Automaton Build Script

set -e  # Exit on error

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$SCRIPT_DIR/build"
BITNET_DIR="$HOME/BitNet"

echo "================================"
echo " Burma Automaton Build Script"
echo "================================"
echo ""

# Parse command line arguments
CLEAN_BUILD=false
INSTALL=false
PACKAGE=false
BUILD_TYPE="Release"
SETUP_BITNET=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --install)
            INSTALL=true
            shift
            ;;
        --package)
            PACKAGE=true
            shift
            ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --setup-bitnet)
            SETUP_BITNET=true
            shift
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Build Options:"
            echo "  --clean          Clean build directory before building"
            echo "  --install        Install after building"
            echo "  --package        Create package after building"
            echo "  --debug          Build in debug mode (default: Release)"
            echo ""
            echo "BitNet Options:"
            echo "  --setup-bitnet   Setup and build Microsoft BitNet.cpp"
            echo ""
            echo "Examples:"
            echo "  $0                        # Build Burma Automaton"
            echo "  $0 --clean --debug        # Clean debug build"
            echo "  $0 --setup-bitnet         # Setup BitNet.cpp"
            echo ""
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# ==========================================
# BitNet.cpp Setup Function
# ==========================================

setup_bitnet() {
    echo ""
    echo "================================"
    echo " Setting up BitNet.cpp"
    echo "================================"
    echo ""

    # Check if already exists
    if [ -d "$BITNET_DIR" ]; then
        echo "BitNet.cpp already exists at $BITNET_DIR"
        read -p "Rebuild? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            echo "Skipping BitNet.cpp setup"
            return
        fi
        rm -rf "$BITNET_DIR/build"
    else
        # Clone BitNet.cpp repository
        echo "Cloning BitNet.cpp repository..."
        git clone --recursive https://github.com/microsoft/BitNet.git "$BITNET_DIR"
    fi

    cd "$BITNET_DIR"

    # Download and setup model
    echo ""
    echo "Setting up BitNet model..."
    echo "This will download microsoft/BitNet-b1.58-2B-4T model (~2GB)"
    echo ""

    python3 setup_env.py --hf-repo microsoft/BitNet-b1.58-2B-4T -q i2_s

    # Build BitNet.cpp
    echo ""
    echo "Building BitNet.cpp..."
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)

    echo ""
    echo "✓ BitNet.cpp built successfully"
    echo "  Location: $BITNET_DIR"
    echo "  Model: models/BitNet-b1.58-2B-4T/"
}

# ==========================================
# Execute BitNet Operations
# ==========================================

if [ "$SETUP_BITNET" = true ]; then
    setup_bitnet
fi

# ==========================================
# Build Burma Automaton
# ==========================================

# Clean build directory if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Run CMake
echo "Running CMake..."
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

# Build
echo "Building Burma Automaton ($BUILD_TYPE)..."
make -j$(nproc)

echo ""
echo "Build complete!"

# Install if requested
if [ "$INSTALL" = true ]; then
    echo ""
    echo "Installing..."
    sudo make install
    echo "Installation complete!"
fi

# Create package if requested
if [ "$PACKAGE" = true ]; then
    echo ""
    echo "Creating package..."
    cpack -G DEB
    echo "Package created!"
fi

echo ""
echo "================================"
echo " Build Summary"
echo "================================"
echo "Build Type: $BUILD_TYPE"
echo "Build Directory: $BUILD_DIR"
if [ "$INSTALL" = true ]; then
    echo "Status: Built and installed"
elif [ "$PACKAGE" = true ]; then
    echo "Status: Built and packaged"
else
    echo "Status: Built successfully"
fi

# Check BitNet status
if [ -d "$BITNET_DIR/build/bin" ]; then
    echo ""
    echo "BitNet.cpp Status:"
    echo "  Installed: Yes ($BITNET_DIR)"
    echo "  Binary: $BITNET_DIR/build/bin/llama-cli"
    echo "  Model: BitNet-b1.58-2B-4T (i2_s quantized)"
else
    echo ""
    echo "BitNet.cpp: Not installed"
    echo "  To setup: ./build.sh --setup-bitnet"
fi

echo ""
echo "================================"
echo " Quick Start"
echo "================================"
echo "Run application:"
echo "  $BUILD_DIR/BurmaAutomaton"
echo ""
echo "For more info, see:"
echo "  - README.md"
echo "  - GETTING_STARTED.md"
echo ""
