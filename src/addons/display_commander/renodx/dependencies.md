# RenoDX Dependencies

This document provides a comprehensive list of all RenoDX components and APIs that Display Commander depends on.

## Core Utilities (`renodx::utils`)

### Settings System (`renodx::utils::settings`)
**Purpose**: Configuration management and persistence
**Usage**: 
- `renodx::utils::settings::use_presets` - Enable/disable preset system
- `renodx::utils::settings::global_name` - Global settings namespace
- `renodx::utils::settings::Use()` - Initialize settings system
- `renodx::utils::settings::Setting` - Individual setting definition

**Files**: `src/utils/settings.hpp`

### Swapchain Utilities (`renodx::utils::swapchain`)
**Purpose**: FPS limiting and swapchain operations
**Usage**:
- `renodx::utils::swapchain::fps_limit` - Global FPS limit value
- `renodx::utils::swapchain::Use()` - Initialize swapchain system
- `renodx::utils::swapchain::ResizeBuffer()` - Resize swapchain buffers
- `renodx::utils::swapchain::FastResizeBuffer()` - Fast buffer resizing

**Files**: `src/utils/swapchain.hpp`

### Data Management (`renodx::utils::data`)
**Purpose**: Per-device data storage and retrieval
**Usage**:
- `renodx::utils::data::Get<T>()` - Retrieve device-specific data
- `renodx::utils::data::Delete()` - Clean up device data

**Files**: `src/utils/data.hpp`

## Background System (`renodx::background`)

**Purpose**: Task coordination and background processing
**Usage**:
- `renodx::background::StartBackgroundTasks()` - Start background task system
- Background task coordination for FPS limiting, audio, and window management

**Files**: `src/background/background_task_coordinator.hpp`

## Hooks System (`renodx::hooks`)

**Purpose**: Window message interception and modification
**Usage**:
- `renodx::hooks::InstallAllHooks()` - Install all window hooks
- `renodx::hooks::UninstallAllHooks()` - Clean up installed hooks
- Window subclassing and message filtering

**Files**: `src/hooks/hooks_manager.hpp`

## Swapchain Mods (`renodx::mods::swapchain`)

**Purpose**: Global swapchain state management and modification
**Usage**:
- `renodx::mods::swapchain::prevent_full_screen` - Global fullscreen prevention flag (⚠️ **CONFLICT: RenoDX has its own ReShade event handler that also prevents fullscreen**)
- `renodx::mods::swapchain::force_borderless` - Global borderless mode flag
- `renodx::mods::swapchain::force_screen_tearing` - Global screen tearing flag
- `renodx::mods::swapchain::use_device_proxy` - Device proxy usage flag

**Files**: `src/mods/swapchain.hpp`

**⚠️ IMPORTANT NOTE**: The `prevent_full_screen` variable is used by RenoDX's own ReShade event handler `OnSetFullscreenState` to prevent fullscreen. This creates a **duplication** with Display Commander's fullscreen prevention event handler. We should either:
1. Remove Display Commander's event handler and rely on RenoDX's
2. Remove the RenoDX variable setting and rely on Display Commander's event handler
3. Coordinate between both systems to avoid conflicts

## Direct API Calls

### Current Direct Usage
These are currently called directly and should be moved to the proxy:

```cpp
// In main_entry.cpp
renodx::utils::settings::use_presets = false;
renodx::utils::settings::global_name = "ResolutionOverride";
renodx::background::StartBackgroundTasks();
renodx::hooks::InstallAllHooks();
renodx::utils::swapchain::fps_limit

// In ui_developer_settings.cpp
renodx::mods::swapchain::force_borderless = (current >= 0.5f);
renodx::mods::swapchain::prevent_full_screen = (current >= 0.5f);
```

### Target Proxy Interface
```cpp
namespace renodx::proxy {
    // Settings
    void SetUsePresets(bool use);
    void SetGlobalName(const std::string& name);
    
    // Background tasks
    void StartBackgroundTasks();
    void StopBackgroundTasks();
    
    // Hooks
    void InstallAllHooks();
    void UninstallAllHooks();
    
    // Swapchain mods
    void SetFullscreenPrevention(bool enabled);
    void SetForceBorderless(bool enabled);
    void SetForceScreenTearing(bool enabled);
    
    // Utilities
    float GetFpsLimit();
    void SetFpsLimit(float limit);
}
```

## Migration Priority

### High Priority (Core Functionality)
1. **Settings system** - Critical for configuration
2. **Background tasks** - Essential for FPS limiting and monitoring
3. **Hooks system** - Required for window management

### Medium Priority (Display Features)
1. **Swapchain mods** - Important for fullscreen prevention
2. **Swapchain utilities** - Used for buffer management

### Low Priority (Optimization)
1. **Data management** - Performance optimization
2. **Advanced features** - Nice-to-have functionality

## Replacement Strategies

### Settings System
- **Alternative**: Custom INI/JSON configuration system
- **Complexity**: Medium
- **Impact**: High (affects all settings)

### Background Tasks
- **Alternative**: Custom thread pool with task queue
- **Complexity**: High
- **Impact**: High (affects FPS limiting and monitoring)

### Hooks System
- **Alternative**: Direct Windows API calls with SetWindowsHookEx
- **Complexity**: High
- **Impact**: High (affects window management)

### Swapchain Mods
- **Alternative**: Direct DXGI/D3D API calls
- **Complexity**: Medium
- **Impact**: Medium (affects display behavior)
