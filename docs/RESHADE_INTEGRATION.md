# Reshade Integration Documentation

## Overview

RenoDX is built on top of Reshade's add-on system, which provides the foundation for DirectX hooking, resource injection, and runtime modification. Understanding this integration is crucial for developing and modifying RenoDX addons.

## Reshade Add-on System

### Core Concepts
- **Addon DLLs**: Dynamic libraries loaded by Reshade runtime
- **Hook Points**: Specific DirectX API calls that can be intercepted
- **Resource Management**: Texture, buffer, and shader injection capabilities
- **GUI Integration**: ImGui-based user interface system

### Key Components in `external/reshade/`

#### Runtime System (`source/runtime/`)
- **DirectX Hooks**: API interception for D3D11, D3D12, OpenGL, Vulkan
- **Resource Tracking**: Monitor texture, buffer, and shader creation
- **State Management**: Track rendering state and pipeline configuration

#### Add-on Interface (`include/`)
- **reshade.hpp**: Main add-on API header
- **addon.hpp**: Add-on lifecycle management
- **effect.hpp**: Shader effect system integration
- **gui.hpp**: ImGui integration for user interfaces

#### GUI System (`source/runtime_gui.cpp`)
- **ImGui Integration**: User interface rendering
- **Settings Management**: Configuration persistence
- **Overlay System**: In-game overlay capabilities

## Addon Lifecycle

### 1. Loading Phase
```cpp
// Addon is loaded by Reshade runtime
extern "C" __declspec(dllexport) const char *NAME = "RenoDX Addon";
extern "C" __declspec(dllexport) const char *DESCRIPTION = "Game modification addon";
extern "C" __declspec(dllexport) const char *AUTHOR = "RenoDX Team";
```

### 2. Initialization
```cpp
// Called when addon is loaded
bool addon_init(reshade::api::device *device)
{
    // Initialize addon-specific resources
    // Register hooks for DirectX APIs
    // Set up user interface
    return true;
}
```

### 3. Runtime Operation
- **Hook Execution**: Intercept DirectX calls
- **Resource Injection**: Modify textures, buffers, shaders
- **State Monitoring**: Track rendering pipeline state
- **User Interaction**: Handle GUI events and settings

### 4. Cleanup
```cpp
// Called when addon is unloaded
void addon_uninit(reshade::api::device *device)
{
    // Release allocated resources
    // Unregister hooks
    // Save configuration
}
```

## DirectX Hook Points

### D3D11 Hooks
- **CreateDevice**: Monitor device creation
- **CreateTexture2D**: Intercept texture creation
- **CreateBuffer**: Monitor buffer creation
- **CreateVertexShader**: Intercept vertex shader creation
- **CreatePixelShader**: Intercept pixel shader creation
- **DrawIndexed**: Monitor draw calls
- **Present**: Intercept frame presentation

### D3D12 Hooks
- **CreateDevice**: Monitor device creation
- **CreateResource**: Intercept resource creation
- **CreatePipelineState**: Monitor PSO creation
- **DrawIndexedInstanced**: Monitor draw calls
- **Present**: Intercept frame presentation

## Resource Injection System

### Texture Injection
```cpp
// Inject custom texture
reshade::api::resource_view texture_view = 
    device->create_resource_view(resource, usage, format, first_level, levels, first_layer, layers);

// Replace existing texture
device->update_resource_view(original_view, new_view);
```

### Buffer Injection
```cpp
// Create custom buffer
reshade::api::resource buffer = 
    device->create_resource(desc, subresources, usage);

// Inject into pipeline
device->bind_vertex_buffers(slot, 1, &buffer, &offset, &stride);
```

### Shader Injection
```cpp
// Compile and inject shader
reshade::api::pipeline pipeline = 
    device->create_pipeline_layout(param_count, params);

// Replace existing shader
device->bind_pipeline_states(stages, count, pipelines);
```

## GUI Integration

### ImGui Setup
```cpp
// Initialize ImGui context
void addon_present(reshade::api::command_queue *queue, reshade::api::swapchain *swapchain, const reshade::api::rect *dirty_rect, uint32_t buffer_count)
{
    // Render ImGui interface
    if (ImGui::Begin("RenoDX Settings"))
    {
        // Add UI controls
        ImGui::Checkbox("Enable Modifications", &settings.enabled);
        ImGui::SliderFloat("Intensity", &settings.intensity, 0.0f, 1.0f);
    }
    ImGui::End();
}
```

### Settings Management
```cpp
// Load settings
void addon_load_config(reshade::api::effect_runtime *runtime)
{
    // Load configuration from file
    // Apply saved settings
}

// Save settings
void addon_save_config(reshade::api::effect_runtime *runtime)
{
    // Save current configuration
    // Persist user preferences
}
```

## Error Handling and Debugging

### Hook Registration
```cpp
// Register hooks safely
bool register_hooks()
{
    if (!reshade::register_addon("RenoDX", this))
        return false;
    
    // Register specific API hooks
    reshade::register_event<reshade::addon_event::create_device>(on_create_device);
    reshade::register_event<reshade::addon_event::create_resource>(on_create_resource);
    
    return true;
}
```

### Error Recovery
```cpp
// Handle hook failures gracefully
void on_create_device(reshade::api::device *device)
{
    try {
        // Device creation logic
    } catch (const std::exception &e) {
        // Log error and continue
        reshade::log_message(reshade::log_level::error, e.what());
    }
}
```

## Performance Considerations

### Hook Overhead
- **Minimal Impact**: Hooks designed for minimal performance cost
- **Selective Registration**: Only register necessary hooks
- **Efficient Processing**: Optimize hook callback functions

### Resource Management
- **Caching**: Cache frequently used resources
- **Lazy Loading**: Load resources on demand
- **Memory Pooling**: Reuse resource objects when possible

## Compatibility Notes

### API Versions
- **Reshade 5.0+**: Full compatibility
- **DirectX 11/12**: Supported
- **OpenGL/Vulkan**: Limited support
- **Windows 10/11**: Primary target platform

### Game Compatibility
- **Anti-Cheat**: Some games may detect hooking
- **DRM**: Digital rights management may interfere
- **Performance**: Resource-intensive games may be affected

## Development Guidelines

### Best Practices
1. **Minimal Hooks**: Only register necessary hooks
2. **Error Handling**: Always handle potential failures
3. **Resource Cleanup**: Properly release allocated resources
4. **User Feedback**: Provide clear error messages and status updates

### Testing
1. **Multiple Games**: Test with various DirectX games
2. **Performance Profiling**: Monitor impact on frame rates
3. **Stability Testing**: Long-running stability tests
4. **Compatibility Testing**: Test with different Reshade versions



