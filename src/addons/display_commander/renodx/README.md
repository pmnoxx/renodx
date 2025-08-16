# RenoDX Proxy Layer

This folder contains a proxy layer that abstracts all RenoDX dependencies for Display Commander. This design allows us to:

1. **Centralize RenoDX interactions** - All calls to RenoDX go through this layer
2. **Document dependencies** - Clear visibility into what we use from RenoDX
3. **Enable future separation** - Easy to replace RenoDX with alternatives
4. **Maintain compatibility** - Single point to handle RenoDX API changes

## What We Use From RenoDX

### Core Systems
- **Settings System** (`renodx::utils::settings`) - Configuration management
- **Swapchain Utilities** (`renodx::utils::swapchain`) - FPS limiting and swapchain operations
- **Background Tasks** (`renodx::background`) - Task coordination system

### Mods
- **Swapchain Mods** (`renodx::mods::swapchain`) - Global swapchain state management
  - `prevent_full_screen` - Global fullscreen prevention flag
  - `force_borderless` - Global borderless mode flag
  - Various swapchain modification functions

### Hooks
- **Window Hooks** (`renodx::hooks`) - Window message interception and modification

## Architecture

```
Display Commander
       ↓
   RenoDX Proxy
       ↓
     RenoDX
```

## Files

- **proxy.hpp** - Main proxy interface declarations
- **proxy.cpp** - Proxy implementation
- **dependencies.md** - Detailed dependency documentation
- **migration_guide.md** - Guide for future RenoDX replacement

## Usage

Instead of calling RenoDX directly:
```cpp
// ❌ Direct call
renodx::mods::swapchain::prevent_full_screen = true;

// ✅ Through proxy
renodx::proxy::SetFullscreenPrevention(true);
```

## Current Issues ⚠️

### Fullscreen Prevention Conflict
Display Commander currently has **two fullscreen prevention systems**:
1. **Display Commander's event handler** (`reshade_events/fullscreen_prevention.cpp`)
2. **RenoDX's built-in prevention** (via `renodx::mods::swapchain::prevent_full_screen`)

This creates duplication and potential conflicts. We need to resolve this by choosing one approach.

## Future Separation

When separating from RenoDX:
1. Replace proxy implementations with alternatives
2. Update Display Commander code to use new implementations
3. No changes needed in main Display Commander code
