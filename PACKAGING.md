# Burma Automaton Packaging Guide

This guide explains how to package Burma Automaton for distribution on Ubuntu.

## Prerequisites

Before building packages, ensure all build dependencies are installed:

```bash
sudo apt update
sudo apt install -y debhelper build-essential cmake qt6-base-dev \
    libqt6opengl6-dev libcurl4-openssl-dev snapcraft
```

## Option 1: Build Debian Package (.deb)

### Build the package

```bash
./build-deb.sh
```

### Install locally for testing

```bash
# Find the built package
DEB_FILE=$(ls -t ../*.deb 2>/dev/null | head -1)

# Install it
sudo dpkg -i "$DEB_FILE"

# Fix dependencies if needed
sudo apt-get install -f
```

### Test the installation

```bash
burma-automaton
```

## Option 2: Build Snap Package (Recommended for Ubuntu Software Center)

### Build the snap

```bash
snapcraft
```

This will create `burma-automaton_1.0.0_amd64.snap`

### Install locally for testing

```bash
sudo snap install --dangerous burma-automaton_1.0.0_amd64.snap
```

### Test the snap

```bash
burma-automaton
```

## Publishing to Ubuntu Software Center

### 1. Create Snapcraft Account

- Go to https://snapcraft.io
- Click "Sign up" and create an account
- Verify your email address

### 2. Login via CLI

```bash
snapcraft login
```

### 3. Register the snap name

```bash
snapcraft register burma-automaton
```

### 4. Upload the snap

```bash
snapcraft upload burma-automaton_1.0.0_amd64.snap --release=stable
```

### 5. Monitor the review process

- Automatic reviews usually complete within minutes
- Manual reviews (if needed) can take 1-3 business days
- Check status at: https://snapcraft.io/burma-automaton/releases

## Package Contents

The package includes:

- **Binary**: `/usr/bin/burma-automaton`
- **Desktop file**: `/usr/share/applications/burma-automaton.desktop`
- **Icon**: `/usr/share/pixmaps/burma-automaton.png`
- **Documentation**: `/usr/share/doc/burma-automaton/`
  - QUICKSTART.md
  - RUN.md

## Runtime Requirements

Users must have BitNet.cpp installed separately:

```bash
# Clone and build BitNet
git clone --recursive https://github.com/microsoft/BitNet.git ~/BitNet
cd ~/BitNet

# Download model
python3 setup_env.py --hf-repo microsoft/BitNet-b1.58-2B-4T -q i2_s

# Build (using gcc instead of clang if needed)
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
make -j$(nproc)
```

The application expects:
- BitNet binary: `~/BitNet/build/bin/llama-cli`
- Model file: `~/BitNet/models/BitNet-b1.58-2B-4T/ggml-model-i2_s.gguf`

## Troubleshooting

### Build fails with "debhelper not found"

```bash
sudo apt install debhelper
```

### Build fails with Qt6 errors

```bash
sudo apt install qt6-base-dev libqt6opengl6-dev libqt6openglwidgets6
```

### Snap build fails

```bash
# Clean previous builds
snapcraft clean

# Rebuild
snapcraft
```

### Application doesn't start

Check BitNet is properly installed:

```bash
ls -la ~/BitNet/build/bin/llama-cli
ls -la ~/BitNet/models/BitNet-b1.58-2B-4T/ggml-model-i2_s.gguf
```

## Version Updates

To release a new version:

1. Update version in `debian/changelog`
2. Update version in `snapcraft.yaml`
3. Rebuild package
4. Upload new snap with `snapcraft upload`
