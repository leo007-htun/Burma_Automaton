# Snap Build Notes

## Current Issue

The snap build is failing with Qt6OpenGLWidgets CMake configuration not found in the core22 build environment.

### Error Details

```
Failed to find Qt component "OpenGLWidgets".
Expected Config file at "/usr/lib/x86_64-linux-gnu/cmake/Qt6OpenGLWidgets/Qt6OpenGLWidgetsConfig.cmake" exists
```

### Root Cause

The snap build environment (LXD container with core22 base) doesn't have the complete Qt6 development files, specifically the CMake config for Qt6OpenGLWidgets component.

## Solutions

### Solution 1: Use core24 base (Updated in snapcraft.yaml)

Changed `base: core22` to `base: core24` in snapcraft.yaml. Core24 has newer Qt6 packages that include OpenGLWidgets CMake configs.

### Solution 2: Build locally without snap confinement

For immediate testing and distribution, you can:

1. **Build native binary:**
   ```bash
   ./build.sh
   ```

2. **Create tarball distribution:**
   ```bash
   tar -czf burma-automaton-1.0.0-linux-amd64.tar.gz \
       build/BurmaAutomaton \
       resources/icons/burma-automaton.png \
       QUICKSTART.md \
       RUN.md \
       LICENSE
   ```

3. **Users extract and run:**
   ```bash
   tar -xzf burma-automaton-1.0.0-linux-amd64.tar.gz
   cd burma-automaton-1.0.0-linux-amd64
   ./BurmaAutomaton
   ```

### Solution 3: Use AppImage (Alternative to Snap)

AppImage is another universal Linux package format that doesn't require snap infrastructure.

Create `AppImageBuilder.yml`:

```yaml
version: 1
AppDir:
  path: ./AppDir
  app_info:
    id: com.burma.automaton
    name: burma-automaton
    icon: burma-automaton
    version: 1.0.0
    exec: usr/bin/BurmaAutomaton
    exec_args: $@

  apt:
    arch: amd64
    sources:
      - sourceline: 'deb http://archive.ubuntu.com/ubuntu/ jammy main universe'
    include:
      - libqt6core6
      - libqt6gui6
      - libqt6widgets6
      - libqt6opengl6
      - libqt6openglwidgets6
      - libqt6network6
      - libcurl4
      - libgl1

  files:
    include:
      - /usr/lib/x86_64-linux-gnu/libQt6*.so*
      - /usr/lib/x86_64-linux-gnu/libcurl.so*

  runtime:
    env:
      QT_QPA_PLATFORM: wayland;xcb

  test:
    debian:
      image: appimagecrafters/tests-env:debian-stable
      command: ./AppRun
      use_host_x: true
```

Build AppImage:
```bash
# Install appimage-builder
sudo apt install -y python3-pip
pip3 install appimage-builder

# Build
appimage-builder --recipe AppImageBuilder.yml
```

Result: `burma-automaton-1.0.0-x86_64.AppImage` (single executable file)

## Recommendation

**For Ubuntu Software Center:**
- Try snap build with core24 base
- If that fails, consider AppImage as alternative

**For immediate distribution:**
- Use tarball (Solution 2) for simplest approach
- Include setup script for BitNet.cpp installation

## Next Steps

1. Try snap build with core24: `snapcraft`
2. If successful, upload to Snap Store
3. If fails, switch to AppImage or tarball distribution
4. Update documentation with working method

## Alternative Distribution Channels

If Snap Store upload is blocked:

1. **GitHub Releases** - Upload binary tarball
2. **Flathub** - Alternative to Snap Store (uses Flatpak)
3. **Direct download** - Host on website/GitHub Pages
4. **PPA** - Personal Package Archive for Ubuntu users
