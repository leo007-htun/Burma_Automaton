# Burma Automaton

<iframe src="https://snapcraft.io/roscribe/embedded?button=black" frameborder="0" width="100%" height="380px" style="border: 1px solid #CCC; border-radius: 2px;"></iframe>

**Natural Language-Driven Gazebo World Builder**

Burma Automaton is a native Ubuntu desktop application that enables users to create, visualize, and export Gazebo simulation worlds using natural language commands interpreted by an integrated BitNet LLM.

## Features

- **Natural Language Interface**: Describe simulation worlds in plain English
- **Intelligent Asset Reuse**: Automatically fetches existing models from Gazebo Fuel
- **Real-time 3D Visualization**: Embedded Gazebo rendering engine
- **Interactive Editing**: Modify objects, materials, lighting, and physics parameters
- **RViz Export**: Convert worlds to ROS-compatible map files (.pgm + .yaml)
- **BitNet LLM Integration**: Powered by Microsoft's BitNet for intelligent world generation

## Architecture

```
┌─────────────────────────────────────────────────────┐
│              Qt6 GUI Layer                          │
├─────────────────────────────────────────────────────┤
│  Prompt Input │ 3D Viewport │ Properties │ Assets  │
└─────────────────────────────────────────────────────┘
         │              │              │
         ▼              ▼              ▼
    BitNetClient   RenderWidget   FuelFetcher
         │              │              │
         ▼              │              ▼
    RAG Pipeline       │         SDFBuilder
                       │              │
                       ▼              ▼
                Gazebo Renderer   RvizConverter
```

## Requirements

### Build Dependencies
- CMake >= 3.20
- Qt6 (Core, Widgets, OpenGL, OpenGLWidgets, Network)
- C++17 compiler (GCC 9+ or Clang 10+)
- Gazebo Garden/Harmonic (optional but recommended)
- libcurl
- nlohmann_json (optional)

### Runtime Dependencies
- Ubuntu 22.04+ (or compatible Linux distribution)
- OpenGL 3.3+ capable graphics
- Python 3.8+ with PyTorch (for BitNet server)
- BitNet LLM (Microsoft - runs locally)

## Quick Start

### One-Step Setup & Build

The easiest way to build Burma Automaton and set up BitNet:

```bash
# Clone repository
git clone <repository-url>
cd Burma_Automaton

# Run all-in-one setup (builds app + sets up BitNet + starts server)
./setup-all.sh
```

This script will:
1. Install system dependencies
2. Build Burma Automaton
3. Clone and setup Microsoft BitNet
4. Download the LLM model (~3GB, auto-downloads on first run)
5. Start the inference server

### Manual Building

If you prefer to build components separately:

```bash
# Build Burma Automaton only
./build.sh

# Setup BitNet separately
./build.sh --setup-bitnet

# Start BitNet server
./build.sh --start-bitnet
```

Or using CMake directly:

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install  # optional
```

## Configuration

### BitNet Local Server

Burma Automaton uses a **local** Microsoft BitNet server - no API keys needed!

The server runs on:
```
http://localhost:8080/api/generate
```

To start the server manually:
```bash
cd ~/BitNet
source venv/bin/activate
python3 run_server.py --model microsoft/bitnet-b1.58-2B-4T --port 8080
```

### Gazebo Fuel Cache

Models are cached in:
```
~/.cache/BurmaAutomaton/gazebo_models/
```

## Usage

### 1. Launch Application

```bash
BurmaAutomaton
```

### 2. Describe Your World

Enter a natural language description in the prompt panel:

```
Create a warehouse with shelving units, pallets, and an AMR robot.
Add loading docks and adequate lighting.
```

### 3. Generate & Visualize

Click "Generate World" to process the prompt. The application will:
- Parse your description with BitNet
- Search for existing assets on Gazebo Fuel
- Generate missing models procedurally
- Create a valid SDF world file
- Render the world in real-time

### 4. Refine & Edit

Use the property editor to:
- Adjust object positions, rotations, scales
- Change materials and colors
- Configure physics parameters
- Modify lighting

### 5. Export

Export to RViz format for ROS navigation:
- File → Export to RViz
- Select output directory and resolution
- Generates `.pgm` and `.yaml` files

## Project Structure

```
Burma_Automaton/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp
│   ├── ui/              # Qt UI widgets
│   ├── core/            # Application core
│   ├── modules/         # Backend modules
│   └── utils/           # Utilities
├── include/
│   ├── ui/
│   ├── core/
│   ├── modules/
│   └── utils/
├── resources/           # Icons, shaders, etc.
├── docs/                # Documentation
└── tests/               # Unit tests
```

## Modules

### BitNetClient
- Communicates with BitNet LLM API
- Parses natural language prompts
- Generates structured world plans (JSON)

### FuelFetcher
- Searches Gazebo Fuel for existing models
- Downloads and caches assets
- Manages local model repository

### SDFBuilder
- Converts JSON world plans to SDF XML
- Handles model generation
- Manages world file structure

### RvizConverter
- Generates occupancy grid maps
- Creates RViz-compatible metadata
- Based on: https://github.com/leo007-htun/Gazebo-map-to-Rviz

### MaterialManager
- Applies colors and materials to models
- Manages texture library
- Dynamic visual property modification

## Packaging

### Debian Package (.deb)

```bash
cd build
cpack -G DEB
sudo dpkg -i BurmaAutomaton-1.0.0-Linux.deb
```

### Snap Package

```bash
snapcraft
sudo snap install burma-automaton_1.0.0_amd64.snap --dangerous
```

## Contributing

Contributions are welcome! Please read CONTRIBUTING.md for guidelines.

## License

[Specify your license here]

## References

- [Gazebo Documentation](https://gazebosim.org/libs/)
- [Gazebo Fuel Models](https://app.gazebosim.org/)
- [BitNet (Microsoft)](https://github.com/microsoft/BitNet)
- [Gazebo Color Tutorial](https://classic.gazebosim.org/tutorials?tut=color_model)
- [Gazebo to RViz Converter](https://github.com/leo007-htun/Gazebo-map-to-Rviz)

## Contact

For issues and support, please visit: [GitHub Issues](https://github.com/yourusername/Burma_Automaton/issues)

---

**© 2025 Burma Robotics**
