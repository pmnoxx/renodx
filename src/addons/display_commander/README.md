# Display Commander Addon

This is the main ReShade addon for Display Commander, providing comprehensive display management, window control, and performance optimization features.

## Features

- **Window Management**: Automatic window sizing, positioning, and style control
- **FPS Limiting**: Smart FPS limiting with background/foreground detection
- **Audio Management**: Background audio muting and volume control
- **DXGI Integration**: Advanced DXGI device information and composition monitoring
- **NVAPI Support**: Fullscreen prevention and HDR monitoring
- **Reflex Integration**: NVIDIA Reflex latency optimization
- **Continuous Monitoring**: Real-time window state monitoring and adjustment

## Architecture

The addon is organized into modular components:

- `background_tasks/` - Unified background task coordinator for FPS limiting, audio, and window management
- `ui/` - User interface components for all settings
- `window_management/` - Window positioning and style management
- `dxgi/` - DXGI device information and composition handling
- `nvapi/` - NVIDIA API integration
- `reflex/` - NVIDIA Reflex functionality
- `audio/` - Audio session management

## Private Documentation

This addon has associated private documentation, prompts, and research materials available in the `private_docs` submodule at the repository root. The submodule contains:

- **AI Prompts**: Development and debugging prompts used during development
- **Internal Documentation**: Detailed technical specifications and implementation notes
- **Research Materials**: Performance analysis, compatibility testing, and feature research
- **Development Notes**: Iterative development process and decision-making documentation

To access the private documentation:
```bash
# Clone with submodules
git clone --recursive <repository-url>

# Or update existing submodules
git submodule update --init --recursive
```

## Building

See the main repository README for build instructions. This addon requires:
- CMake 3.20+
- C++17 compatible compiler
- Windows SDK
- ReShade SDK
