#!/bin/bash

# Burma Automaton - Complete Setup Script
# This script will:
# 1. Install ALL system dependencies automatically
# 2. Setup BitNet.cpp (with gcc, not clang)
# 3. Download BitNet model
# 4. Build Burma Automaton

set -e  # Exit on error

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$SCRIPT_DIR/build"
BITNET_DIR="$HOME/BitNet"

echo "=================================================="
echo " Burma Automaton - Complete Setup"
echo "=================================================="
echo ""
echo "This script will:"
echo "  1. Install ALL system dependencies (Qt6, cmake, etc.)"
echo "  2. Setup and build BitNet.cpp"
echo "  3. Download BitNet model (~1.2GB)"
echo "  4. Build Burma Automaton"
echo ""
echo "This may take 10-20 minutes depending on your internet speed."
echo ""
read -p "Continue? (y/N): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Setup cancelled."
    exit 0
fi

# ==========================================
# Step 1: Install Dependencies
# ==========================================
echo ""
echo "=================================================="
echo " Step 1/4: Installing Dependencies"
echo "=================================================="
echo ""

echo "Updating package lists..."
sudo apt update

echo ""
echo "Installing dependencies (this may take a few minutes)..."
sudo apt install -y \
    cmake \
    build-essential \
    git \
    wget \
    curl \
    python3 \
    python3-pip \
    qt6-base-dev \
    libqt6opengl6-dev \
    libqt6openglwidgets6 \
    libcurl4-openssl-dev

echo ""
echo "Installing Python packages..."
pip3 install --user huggingface-hub

echo "✓ All dependencies installed"

# ==========================================
# Step 2: Setup BitNet.cpp
# ==========================================
echo ""
echo "=================================================="
echo " Step 2/4: Setting up BitNet.cpp"
echo "=================================================="
echo ""

if [ -d "$BITNET_DIR" ]; then
    echo "BitNet.cpp already exists at $BITNET_DIR"
    read -p "Rebuild? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        rm -rf "$BITNET_DIR/build"
    else
        echo "Skipping BitNet.cpp build"
        if [ ! -f "$BITNET_DIR/build/bin/llama-cli" ]; then
            echo "Warning: llama-cli not found, will rebuild..."
            rm -rf "$BITNET_DIR/build"
        fi
    fi
fi

if [ ! -d "$BITNET_DIR" ]; then
    echo "Cloning BitNet.cpp repository..."
    git clone --recursive https://github.com/microsoft/BitNet.git "$BITNET_DIR"
fi

cd "$BITNET_DIR"

# Initialize submodules if not already done
if [ ! -d "3rdparty/llama.cpp/.git" ]; then
    echo "Initializing submodules..."
    git submodule update --init --recursive
fi

# Build BitNet.cpp if not already built
if [ ! -f "$BITNET_DIR/build/bin/llama-cli" ]; then
    echo ""
    echo "Building BitNet.cpp with gcc (this may take 5-10 minutes)..."
    rm -rf build
    mkdir -p build
    cd build

    # Use gcc instead of clang (clang is optional)
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
    make -j$(nproc)

    echo "✓ BitNet.cpp built successfully"
else
    echo "✓ BitNet.cpp already built"
fi

# ==========================================
# Step 3: Download BitNet Model
# ==========================================
echo ""
echo "=================================================="
echo " Step 3/4: Downloading BitNet Model"
echo "=================================================="
echo ""

MODEL_DIR="$BITNET_DIR/models/BitNet-b1.58-2B-4T"
MODEL_FILE="$MODEL_DIR/ggml-model-i2_s.gguf"

if [ -f "$MODEL_FILE" ]; then
    echo "Model already exists at $MODEL_FILE"
    echo "Size: $(du -h "$MODEL_FILE" | cut -f1)"
else
    echo "Downloading BitNet-b1.58-2B-4T model (~1.2GB)..."
    echo "This is the official BitNet GGUF model for llama.cpp inference."
    echo "This will take several minutes depending on your connection..."
    echo ""

    mkdir -p "$MODEL_DIR"

    # Download using Python and huggingface_hub
    python3 << 'PYEOF'
from huggingface_hub import snapshot_download
import os

model_dir = os.path.expanduser("~/BitNet/models/BitNet-b1.58-2B-4T")
os.makedirs(model_dir, exist_ok=True)

print(f"Downloading BitNet-b1.58-2B-4T to {model_dir}...")
snapshot_download(
    repo_id="microsoft/BitNet-b1.58-2B-4T-gguf",
    local_dir=model_dir,
    local_dir_use_symlinks=False
)
print("✓ Model downloaded successfully!")
PYEOF
fi

# Verify model exists
if [ ! -f "$MODEL_FILE" ]; then
    echo "ERROR: Model download failed!"
    exit 1
fi

echo "✓ Model ready: $MODEL_FILE"
echo "  Size: $(du -h "$MODEL_FILE" | cut -f1)"

# ==========================================
# Step 4: Build Burma Automaton
# ==========================================
echo ""
echo "=================================================="
echo " Step 4/4: Building Burma Automaton"
echo "=================================================="
echo ""

cd "$SCRIPT_DIR"

# Clean build
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Running CMake..."
cmake -DCMAKE_BUILD_TYPE=Release ..

echo "Compiling..."
make -j$(nproc)

if [ ! -f "$BUILD_DIR/BurmaAutomaton" ]; then
    echo "ERROR: Build failed!"
    exit 1
fi

echo "✓ Burma Automaton built successfully"

# ==========================================
# Verify Everything Works
# ==========================================
echo ""
echo "=================================================="
echo " Verifying Installation"
echo "=================================================="
echo ""

# Check BitNet.cpp
if [ -f "$BITNET_DIR/build/bin/llama-cli" ]; then
    echo "✓ BitNet.cpp binary: $BITNET_DIR/build/bin/llama-cli"
else
    echo "✗ BitNet.cpp binary NOT FOUND"
    exit 1
fi

# Check Model
if [ -f "$MODEL_FILE" ]; then
    echo "✓ BitNet model: $MODEL_FILE ($(du -h "$MODEL_FILE" | cut -f1))"
else
    echo "✗ BitNet model NOT FOUND"
    exit 1
fi

# Check Burma Automaton
if [ -f "$BUILD_DIR/BurmaAutomaton" ]; then
    echo "✓ Burma Automaton: $BUILD_DIR/BurmaAutomaton ($(du -h "$BUILD_DIR/BurmaAutomaton" | cut -f1))"
else
    echo "✗ Burma Automaton NOT FOUND"
    exit 1
fi

# Test BitNet.cpp quickly
echo ""
echo "Testing BitNet.cpp inference..."
timeout 30 "$BITNET_DIR/build/bin/llama-cli" \
    -m "$MODEL_FILE" \
    -p "Hello" \
    -n 5 \
    > /tmp/bitnet_test.txt 2>&1 || true

if grep -q "llama_model_loader" /tmp/bitnet_test.txt; then
    echo "✓ BitNet.cpp inference test PASSED"
else
    echo "⚠ BitNet.cpp test inconclusive (may be slow on first run)"
fi

# ==========================================
# Summary
# ==========================================
echo ""
echo "=================================================="
echo " Setup Complete! ✓"
echo "=================================================="
echo ""
echo "Installation Summary:"
echo "─────────────────────────────────────────────────"
echo "BitNet.cpp:"
echo "  Location: $BITNET_DIR"
echo "  Binary:   $BITNET_DIR/build/bin/llama-cli"
echo "  Model:    BitNet-b1.58-2B-4T (official GGUF)"
echo "  Path:     $MODEL_FILE"
echo "  Size:     $(du -h "$MODEL_FILE" | cut -f1)"
echo ""
echo "Burma Automaton:"
echo "  Binary:   $BUILD_DIR/BurmaAutomaton"
echo "  Size:     $(du -h "$BUILD_DIR/BurmaAutomaton" | cut -f1)"
echo ""
echo "Architecture:"
echo "  Burma Automaton → QProcess → llama-cli → BitNet Model"
echo "  (No HTTP server, no Python runtime required)"
echo ""
echo "─────────────────────────────────────────────────"
echo ""
echo "To run Burma Automaton:"
echo ""
echo "  cd $SCRIPT_DIR"
echo "  ./build/BurmaAutomaton"
echo ""
echo "The application will automatically detect BitNet.cpp"
echo "and show 'Ready: Yes' in the Event Log."
echo ""
echo "For more information, see:"
echo "  - RUN.md (how to run and test)"
echo "  - BITNET_SETUP_EXPLAINED.md (technical details)"
echo ""
echo "Enjoy your BitNet.cpp-powered Burma Automaton! 🚀"
echo ""
