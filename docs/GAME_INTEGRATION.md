# Game Integration Documentation

## Overview

RenoDX's game integration system allows for creating custom addons for specific games. Each game has its own directory in `src/games/` containing game-specific shaders, injection logic, and configuration.

## Game Addon Structure

### Directory Organization
```
src/games/<game_name>/
├── addon.cpp          # Main addon implementation
├── addon.h            # Header file with declarations
├── *.hlsl             # Custom shader files
├── *.hlsli            # Shader include files
├── *.txt              # Configuration files
└── subdirectories/    # Additional resources
```

### Example Game Structure
```
src/games/cyberpunk2077/
├── addon.cpp          # CP2077-specific addon logic
├── addon.h            # CP2077 addon declarations
├── lighting.hlsl      # Custom lighting shaders
├── post_process.hlsl  # Post-processing effects
├── common.hlsli       # Shared shader includes
└── config.txt         # Game-specific configuration
```

## Addon Implementation

### Basic Addon Template
```cpp
// addon.h
#pragma once
#include "reshade.hpp"

// Game-specific settings structure
struct game_settings {
    bool enable_modifications = true;
    float intensity = 1.0f;
    // ... other settings
};

// Game-specific addon class
class game_addon {
private:
    game_settings settings;
    reshade::api::device *device = nullptr;
    
public:
    bool init(reshade::api::device *dev);
    void uninit();
    void on_present(reshade::api::command_queue *queue, 
                   reshade::api::swapchain *swapchain);
    void on_create_resource(reshade::api::device *dev,
                           const reshade::api::resource_desc &desc,
                           const reshade::api::subresource_data *subresources,
                           reshade::api::resource_usage usage,
                           reshade::api::resource *resource);
    // ... other hook methods
};

// Global addon instance
extern game_addon g_addon;
```

### Addon Implementation
```cpp
// addon.cpp
#include "addon.h"

// Export addon information
extern "C" __declspec(dllexport) const char *NAME = "Game Addon";
extern "C" __declspec(dllexport) const char *DESCRIPTION = "Game-specific modifications";
extern "C" __declspec(dllexport) const char *AUTHOR = "RenoDX Team";

// Global addon instance
game_addon g_addon;

// Addon initialization
bool addon_init(reshade::api::device *device)
{
    return g_addon.init(device);
}

// Addon cleanup
void addon_uninit(reshade::api::device *device)
{
    g_addon.uninit();
}

// Present hook
void addon_present(reshade::api::command_queue *queue,
                  reshade::api::swapchain *swapchain,
                  const reshade::api::rect *dirty_rect,
                  uint32_t buffer_count)
{
    g_addon.on_present(queue, swapchain);
}

// Resource creation hook
void addon_create_resource(reshade::api::device *device,
                          const reshade::api::resource_desc &desc,
                          const reshade::api::subresource_data *subresources,
                          reshade::api::resource_usage usage,
                          reshade::api::resource *resource)
{
    g_addon.on_create_resource(device, desc, subresources, usage, resource);
}
```

## Game-Specific Shaders

### Shader Organization
```hlsl
// common.hlsli - Shared includes
#ifndef COMMON_HLSLI
#define COMMON_HLSLI

// Common structures
struct VS_INPUT {
    float3 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

// Common functions
float3 apply_custom_lighting(float3 color, float3 normal, float3 light_dir);
float4 apply_post_process(float4 color, float2 texcoord);

#endif
```

### Custom Shader Implementation
```hlsl
// lighting.hlsl - Custom lighting shaders
#include "common.hlsli"

// Custom lighting parameters
cbuffer LightingParams : register(b0) {
    float3 light_direction;
    float light_intensity;
    float3 ambient_color;
    float padding;
};

// Vertex shader
PS_INPUT vertex_shader(VS_INPUT input)
{
    PS_INPUT output;
    
    // Transform position
    output.position = mul(world_view_projection, float4(input.position, 1.0));
    output.texcoord = input.texcoord;
    output.normal = mul(world, float4(input.normal, 0.0)).xyz;
    output.color = input.color;
    
    return output;
}

// Pixel shader
float4 pixel_shader(PS_INPUT input) : SV_Target
{
    // Sample original texture
    float4 original_color = texture0.Sample(sampler0, input.texcoord);
    
    // Apply custom lighting
    float3 lit_color = apply_custom_lighting(original_color.rgb, 
                                           input.normal, 
                                           light_direction);
    
    // Apply ambient lighting
    lit_color += ambient_color * original_color.rgb;
    
    return float4(lit_color, original_color.a);
}
```

## Game Detection and Configuration

### Process Detection
```cpp
// Detect target game process
bool is_target_game()
{
    DWORD process_id = GetCurrentProcessId();
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process_id);
    
    if (process) {
        wchar_t process_name[MAX_PATH];
        GetModuleFileNameEx(process, NULL, process_name, MAX_PATH);
        CloseHandle(process);
        
        // Check if process name matches target game
        std::wstring name(process_name);
        return name.find(L"game.exe") != std::wstring::npos;
    }
    
    return false;
}
```

### Configuration Loading
```cpp
// Load game-specific configuration
void load_game_config()
{
    std::ifstream config_file("game_config.txt");
    if (config_file.is_open()) {
        std::string line;
        while (std::getline(config_file, line)) {
            // Parse configuration line
            auto pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                // Apply configuration
                if (key == "enable_modifications") {
                    settings.enable_modifications = (value == "true");
                } else if (key == "intensity") {
                    settings.intensity = std::stof(value);
                }
            }
        }
        config_file.close();
    }
}
```

## Shader Injection Strategies

### 1. Hash-Based Injection
```cpp
// Inject shaders based on hash matching
void inject_by_hash(reshade::api::command_list *cmd_list,
                   uint64_t shader_hash)
{
    static const std::unordered_map<uint64_t, reshade::api::pipeline> shader_map = {
        {0x1234567890ABCDEF, custom_vertex_shader},
        {0xFEDCBA0987654321, custom_pixel_shader},
        // ... more shader mappings
    };
    
    auto it = shader_map.find(shader_hash);
    if (it != shader_map.end()) {
        cmd_list->bind_pipeline_states(reshade::api::pipeline_stage::all, 
                                      1, &it->second);
    }
}
```

### 2. Resource-Based Injection
```cpp
// Inject based on resource characteristics
void inject_by_resource(reshade::api::command_list *cmd_list,
                       const reshade::api::resource_desc &desc)
{
    // Check if resource matches target characteristics
    if (desc.type == reshade::api::resource_type::texture_2d &&
        desc.texture.width == 1920 &&
        desc.texture.height == 1080) {
        
        // Inject custom shader for this resource
        inject_custom_shader(cmd_list);
    }
}
```

### 3. State-Based Injection
```cpp
// Inject based on rendering state
void inject_by_state(reshade::api::command_list *cmd_list,
                    const render_state &state)
{
    // Check current rendering state
    if (state.render_target_count > 0 &&
        state.depth_stencil_state.depth_enable) {
        
        // Inject post-processing shader
        inject_post_process_shader(cmd_list);
    }
}
```

## Performance Optimization

### Shader Caching
```cpp
// Cache game-specific shaders
class game_shader_cache {
private:
    std::unordered_map<std::string, reshade::api::pipeline> shaders;
    
public:
    reshade::api::pipeline get_shader(const std::string &name)
    {
        auto it = shaders.find(name);
        if (it != shaders.end()) {
            return it->second;
        }
        
        // Compile and cache shader
        reshade::api::pipeline pipeline = compile_game_shader(name);
        shaders[name] = pipeline;
        return pipeline;
    }
};
```

### Resource Management
```cpp
// Manage game-specific resources
class game_resource_manager {
private:
    std::vector<reshade::api::resource> game_resources;
    
public:
    void add_resource(reshade::api::resource resource)
    {
        game_resources.push_back(resource);
    }
    
    void cleanup_resources()
    {
        for (auto &resource : game_resources) {
            device->destroy_resource(resource);
        }
        game_resources.clear();
    }
};
```

## User Interface Integration

### Game-Specific Settings
```cpp
// Render game-specific UI
void render_game_ui()
{
    if (ImGui::Begin("Game Settings")) {
        ImGui::Checkbox("Enable Modifications", &settings.enable_modifications);
        
        if (settings.enable_modifications) {
            ImGui::SliderFloat("Lighting Intensity", &settings.lighting_intensity, 0.0f, 2.0f);
            ImGui::SliderFloat("Color Saturation", &settings.color_saturation, 0.0f, 2.0f);
            ImGui::ColorEdit3("Ambient Color", &settings.ambient_color.x);
        }
        
        if (ImGui::Button("Reset to Defaults")) {
            reset_settings();
        }
    }
    ImGui::End();
}
```

### Configuration Persistence
```cpp
// Save game-specific settings
void save_game_settings()
{
    std::ofstream config_file("game_settings.txt");
    if (config_file.is_open()) {
        config_file << "enable_modifications=" << (settings.enable_modifications ? "true" : "false") << std::endl;
        config_file << "lighting_intensity=" << settings.lighting_intensity << std::endl;
        config_file << "color_saturation=" << settings.color_saturation << std::endl;
        config_file.close();
    }
}
```

## Debugging and Development

### Game-Specific Debugging
```cpp
// Enable game-specific debugging
void enable_game_debugging()
{
    // Enable debug logging for this game
    reshade::log_message(reshade::log_level::info, "Game addon loaded");
    
    // Register debug callbacks
    reshade::register_event<reshade::addon_event::create_resource>(
        [](reshade::api::device *device, const reshade::api::resource_desc &desc,
           const reshade::api::subresource_data *subresources,
           reshade::api::resource_usage usage, reshade::api::resource *resource) {
            reshade::log_message(reshade::log_level::debug, 
                               "Resource created: %dx%d", desc.texture.width, desc.texture.height);
        });
}
```

### Performance Monitoring
```cpp
// Monitor game-specific performance
class game_performance_monitor {
private:
    std::chrono::high_resolution_clock::time_point frame_start;
    uint64_t frame_count = 0;
    float average_frame_time = 0.0f;
    
public:
    void begin_frame()
    {
        frame_start = std::chrono::high_resolution_clock::now();
    }
    
    void end_frame()
    {
        auto frame_end = std::chrono::high_resolution_clock::now();
        auto frame_time = std::chrono::duration_cast<std::chrono::microseconds>
                         (frame_end - frame_start).count();
        
        frame_count++;
        average_frame_time = (average_frame_time * (frame_count - 1) + frame_time) / frame_count;
        
        if (frame_count % 60 == 0) {
            reshade::log_message(reshade::log_level::info, 
                               "Average frame time: %.2f ms", average_frame_time / 1000.0f);
        }
    }
};
```

## Best Practices

### Game-Specific Development
1. **Isolation**: Keep game-specific code isolated from core systems
2. **Configuration**: Use external configuration files for easy modification
3. **Performance**: Monitor performance impact on target game
4. **Compatibility**: Test with different game versions and patches

### Shader Development
1. **Modularity**: Use include files for shared functionality
2. **Validation**: Validate shader compatibility with game's rendering pipeline
3. **Optimization**: Profile shaders for performance impact
4. **Fallbacks**: Provide fallback shaders for unsupported hardware

### Resource Management
1. **Cleanup**: Properly release game-specific resources
2. **Caching**: Cache frequently used resources
3. **Memory**: Monitor memory usage and prevent leaks
4. **Validation**: Validate resource compatibility before injection







