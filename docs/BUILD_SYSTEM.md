# Build System Documentation

## Overview

RenoDX uses CMake as its primary build system, with support for multiple build configurations and toolchains. The build system is designed to handle complex dependencies including Reshade, shader compilation tools, and game-specific resources.

## Prerequisites

### Required Tools
- **CMake 3.27+**: Build system generator
- **C++20 Compiler**: Visual Studio 2019+ or compatible
- **Windows SDK 10**: For DirectX headers and tools
- **Git Bash**: For build scripts (as specified in user rules)

### Required Binaries
- **FXC.exe**: DirectX Shader Compiler (from Windows SDK)
- **DXC.exe**: DirectX Shader Compiler (from Windows SDK)
- **slangc.exe**: Slang shader compiler (included in bin/)

## Build Configuration

### CMake Configuration
The main `CMakeLists.txt` defines:
- Project structure and dependencies
- Shader compilation pipeline
- Resource embedding system
- Addon generation for each game

### Key Build Variables
```cmake
CMAKE_WINDOWS_KITS_10_DIR    # Windows SDK location
FXC_BIN                      # FXC compiler path
DXC_BIN                      # DXC compiler path
SLANGC_BIN                   # Slang compiler path
```

## Build Process

### 1. Standard Build
```bash
# Using build.cmd (Windows)
./build.cmd

# Using CMake directly
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### 2. 32-bit Build
```bash
# Using build32.ps1
./bd32.ps1

# Manual 32-bit build
mkdir build32 && cd build32
cmake .. -A Win32
cmake --build . --config Release
```

### 3. Development Build
```bash
# Using PowerShell scripts
./bd.ps1        # 64-bit development build
./bd32.ps1      # 32-bit development build
```

## Build Artifacts

### Generated Files
- **Addon DLLs**: `*.addon64` for 64-bit, `*.addon32` for 32-bit
- **Decompiler**: `decomp.exe` for shader analysis
- **Embedded Resources**: Generated `.h` files from HLSL shaders
- **Resource Files**: `.rc` files for addon metadata

### Output Structure
```
bin/
├── *.addon64          # 64-bit addons
├── *.addon32          # 32-bit addons
├── decomp.exe         # Shader decompiler
└── *.dll              # Runtime dependencies
```

## Shader Compilation Pipeline

### HLSL Processing
1. **Dependency Analysis**: CMake analyzes `#include` statements
2. **Compilation**: Shaders compiled with appropriate target profiles
3. **Embedding**: Compiled shaders embedded into C++ headers
4. **Linking**: Embedded shaders linked into addon binaries

### Supported Shader Models
- **Vertex Shaders**: vs_4_0, vs_5_0, vs_5_1
- **Pixel Shaders**: ps_4_0, ps_5_0, ps_5_1
- **Compute Shaders**: cs_4_0, cs_5_0, cs_5_1
- **Geometry Shaders**: gs_4_0, gs_5_0, gs_5_1

## Game-Specific Builds

### Addon Generation
Each game in `src/games/` generates its own addon:
- **Template Processing**: Game-specific templates processed
- **Resource Compilation**: HLSL files compiled and embedded
- **Binary Generation**: Final addon DLL created

### Configuration Options
- **Feature Flags**: Enable/disable specific features per game
- **Shader Variants**: Different shader versions for different hardware
- **Debug Symbols**: Optional debug information inclusion

## Development Workflow

### 1. Adding New Games
1. Create game directory in `src/games/`
2. Add `addon.cpp` and `addon.h` files
3. Add HLSL shader files
4. Update CMakeLists.txt if needed
5. Build and test

### 2. Modifying Existing Games
1. Edit game-specific files in `src/games/<game>/`
2. Rebuild using appropriate build script
3. Test changes in target game

### 3. Debugging Build Issues
```bash
# Verbose CMake output
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON

# Debug build
cmake --build . --config Debug

# Clean rebuild
cmake --build . --target clean
cmake --build . --config Release
```

## Troubleshooting

### Common Issues
1. **Missing Windows SDK**: Ensure Windows SDK 10 is installed
2. **Compiler Not Found**: Check PATH for FXC/DXC binaries
3. **Shader Compilation Errors**: Verify HLSL syntax and target profiles
4. **Linker Errors**: Check for missing dependencies

### Build Scripts
- **bd.ps1**: 64-bit development build with timeout protection
- **bd32.ps1**: 32-bit development build with timeout protection
- **build.cmd**: Standard build process
- **make-sln.cmd**: Generate Visual Studio solution

## Performance Considerations

### Build Optimization
- **Parallel Compilation**: Use `-j` flag for parallel builds
- **Incremental Builds**: CMake caches for faster rebuilds
- **Shader Caching**: Compiled shaders cached between builds

### Runtime Performance
- **Release Builds**: Optimized for production use
- **Debug Builds**: Include debugging information
- **Profile Builds**: Include profiling information

## Continuous Integration

### GitHub Actions
- **Clang Build**: Cross-platform compilation
- **Ninja Build**: Fast incremental builds
- **Visual Studio Build**: Windows-specific testing

### Build Matrix
- **Platforms**: Windows x64, Windows x86
- **Configurations**: Debug, Release, RelWithDebInfo
- **Toolchains**: MSVC, Clang, Ninja








