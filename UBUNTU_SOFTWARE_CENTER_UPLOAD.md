# Upload Burma Automaton to Ubuntu Software Center

## Current Status - Updated

**Snap Build Issue**: The snap build with core22 base encountered Qt6OpenGLWidgets CMake configuration issues. Updated to core24 base in snapcraft.yaml.

**Alternative Distribution**: Created tarball distribution script (build-tarball.sh) for immediate distribution while snap build is being resolved.

All packaging files have been created and are ready for use:

- `snapcraft.yaml` - Snap package configuration
- `debian/control` - Debian package metadata
- `debian/rules` - Build rules
- `debian/changelog` - Version history
- `debian/compat` - Debhelper compatibility
- `build-deb.sh` - Automated Debian package builder

## Required System Dependencies

Before building packages, install these dependencies:

```bash
sudo apt update
sudo apt install -y debhelper build-essential cmake \
    qt6-base-dev libqt6opengl6-dev libcurl4-openssl-dev
```

## Build Options

### Option 1: Tarball Distribution (Quickest - Ready Now)

This creates a portable .tar.gz file that works on any Linux system with Qt6.

#### Build the tarball:

```bash
./build-tarball.sh
```

This creates: `burma-automaton-1.0.0-linux-amd64.tar.gz`

**Contents:**
- BurmaAutomaton binary
- Resources and icons
- Documentation (QUICKSTART.md, RUN.md)
- run.sh launcher script with BitNet.cpp checks
- INSTALL.txt installation guide

**Distribution options:**
- Upload to GitHub Releases
- Host on website for direct download
- Share via file sharing services

**User installation:**
```bash
tar -xzf burma-automaton-1.0.0-linux-amd64.tar.gz
cd burma-automaton-1.0.0-linux-amd64
./run.sh
```

### Option 2: Snap Package (For Ubuntu Software Center)

Snap is the preferred format for Ubuntu Software Center distribution.

#### Build the snap:

```bash
snapcraft
```

This creates: `burma-automaton_1.0.0_amd64.snap`

#### Test locally:

```bash
sudo snap install --dangerous burma-automaton_1.0.0_amd64.snap
burma-automaton
```

**Note**: Updated snapcraft.yaml to use `base: core24` to resolve Qt6OpenGLWidgets CMake configuration issues encountered with core22. See SNAP_BUILD_NOTES.md for details.

### Option 3: Debian Package

For traditional .deb package:

```bash
./build-deb.sh
```

This creates: `../burma-automaton_1.0.0_amd64.deb`

#### Test locally:

```bash
sudo dpkg -i ../burma-automaton_1.0.0_amd64.deb
sudo apt-get install -f  # Fix any dependency issues
burma-automaton
```

## Upload to Ubuntu Software Center

### Step 1: Create Snapcraft Account

1. Go to https://snapcraft.io
2. Click "Sign up"
3. Use Ubuntu One account or create new account
4. Verify email address

### Step 2: Login via CLI

```bash
snapcraft login
```

Enter your Ubuntu One credentials when prompted.

### Step 3: Register the snap name

```bash
snapcraft register burma-automaton
```

If the name is already taken, you'll need to choose a different name and update `snapcraft.yaml`:

```yaml
name: burma-automaton-ai  # Example alternative name
```

### Step 4: Upload the snap

```bash
snapcraft upload burma-automaton_1.0.0_amd64.snap --release=stable
```

Available channels:
- `stable` - Production release (recommended for first upload)
- `candidate` - Pre-release testing
- `beta` - Beta testing
- `edge` - Development builds

### Step 5: Monitor Review Process

After upload, your snap goes through automatic review:

1. **Automatic review** (1-5 minutes)
   - Checks for security issues
   - Validates snap structure
   - Most snaps pass automatically

2. **Manual review** (if flagged, 1-3 business days)
   - Human review for security-sensitive features
   - You'll receive email notification

3. **Check status:**
   - Visit: https://snapcraft.io/burma-automaton/releases
   - Or run: `snapcraft status burma-automaton`

### Step 6: Make it Public

Once approved, your snap appears in:
- Ubuntu Software Center
- Snap Store website
- `snap find burma-automaton` command

Users can install with:

```bash
sudo snap install burma-automaton
```

## Important Notes

### BitNet.cpp Runtime Dependency

Burma Automaton requires BitNet.cpp to be installed separately. Users need to:

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

**Consider adding this to the snap description or documentation.**

### Expected Paths

The application expects:
- BitNet binary: `~/BitNet/build/bin/llama-cli`
- Model: `~/BitNet/models/BitNet-b1.58-2B-4T/ggml-model-i2_s.gguf`

### Fallback Behavior

If BitNet.cpp is not found or fails:
- Application still runs
- Falls back to simple geometric world generation (red box)
- User can still explore UI and export functionality

## Updating Your Snap

To release updates:

1. Update version in `snapcraft.yaml`:
   ```yaml
   version: '1.0.1'
   ```

2. Update `debian/changelog`:
   ```
   burma-automaton (1.0.1) focal; urgency=medium

     * Bug fixes and improvements
     * Updated BitNet.cpp integration

    -- Your Name <email>  Date
   ```

3. Rebuild:
   ```bash
   snapcraft
   ```

4. Upload new version:
   ```bash
   snapcraft upload burma-automaton_1.0.1_amd64.snap --release=stable
   ```

## Troubleshooting

### "Name already registered"

Choose a different name in `snapcraft.yaml` and try again.

### "Missing dependencies"

Ensure all build dependencies are installed (see top of this document).

### "Snap build fails"

Clean and retry:
```bash
snapcraft clean
snapcraft
```

### "Manual review required"

Some features trigger manual review:
- Custom interfaces
- System access
- Network access (Burma Automaton uses network for Fuel API)

Wait for review notification email (usually 1-3 days).

### Testing before upload

Always test your snap locally before uploading:

```bash
# Install
sudo snap install --dangerous burma-automaton_1.0.0_amd64.snap

# Run
burma-automaton

# Check logs if issues
snap logs burma-automaton

# Uninstall
sudo snap remove burma-automaton
```

## Support and Documentation

After publishing, consider:

1. **Adding store listing assets:**
   - Screenshots (recommended: 1-5 images)
   - Icon (already included: resources/icons/burma-automaton.png)
   - Detailed description
   - Website/GitHub link (https://github.com/leo007-htun/Burma_Automaton)

2. **Store metadata** (edit at snapcraft.io):
   - Summary: "AI-powered 3D world generator for robotics simulation"
   - Description: Include features, requirements, and quick start guide
   - Categories: Graphics, Development, Science

3. **User documentation:**
   - Link to QUICKSTART.md
   - Link to RUN.md
   - BitNet.cpp setup instructions

## Quick Reference

```bash
# Build snap
snapcraft

# Login
snapcraft login

# Register name
snapcraft register burma-automaton

# Upload
snapcraft upload burma-automaton_1.0.0_amd64.snap --release=stable

# Check status
snapcraft status burma-automaton

# View releases
snapcraft list-revisions burma-automaton
```

## Next Steps

1. Install build dependencies (see "Required System Dependencies")
2. Build the snap package with `snapcraft`
3. Test locally with `sudo snap install --dangerous burma-automaton_1.0.0_amd64.snap`
4. Create Snapcraft account at https://snapcraft.io
5. Follow upload steps above
6. Wait for review approval
7. Update store listing with screenshots and detailed description
8. Share with the community!
