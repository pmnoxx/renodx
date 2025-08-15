# Shader System Documentation

## Overview

RenoDX's shader system is responsible for analyzing, modifying, and injecting HLSL shaders into DirectX games. This system includes shader decompilation, analysis, modification, and runtime injection capabilities.

## Shader Pipeline

### 1. Shader Detection
- **Hook Points**: Intercept shader creation calls
- **Resource Tracking**: Monitor shader resource creation
- **State Analysis**: Analyze current rendering pipeline state

### 2. Shader Analysis
- **Decompilation**: Convert compiled shaders back to HLSL
- **Structure Analysis**: Parse shader input/output signatures
- **Dependency Mapping**: Identify shader dependencies and resources

### 3. Shader Modification
- **Custom Shaders**: Apply game-specific shader modifications
- **Resource Injection**: Inject custom textures and buffers
- **Parameter Modification**: Adjust shader parameters and constants

### 4. Shader Injection
- **Compilation**: Compile modified HLSL to bytecode
- **Resource Binding**: Bind custom resources to shader stages
- **Pipeline Integration**: Integrate into rendering pipeline

## Shader Decompiler (`src/decompiler/`)

### Purpose
The decompiler tool (`decomp.exe`) analyzes compiled shaders and provides insights into their structure and functionality.

### Usage
```bash
# Decompile a shader file
./decomp.exe input_shader.bin output_hlsl.hlsl

# Analyze shader with verbose output
./decomp.exe -v input_shader.bin

# Extract shader resources
./decomp.exe -r input_shader.bin
```

### Output Analysis
- **HLSL Code**: Decompiled shader source code
- **Resource Bindings**: Texture, buffer, and constant buffer bindings
- **Input/Output**: Vertex and pixel shader signatures
- **Techniques**: Shader technique and pass information

## HLSL Processing

### Shader Models Supported
- **Vertex Shaders**: vs_4_0, vs_5_0, vs_5_1
- **Pixel Shaders**: ps_4_0, ps_5_0, ps_5_1
- **Compute Shaders**: cs_4_0, cs_5_0, cs_5_1
- **Geometry Shaders**: gs_4_0, gs_5_0, gs_5_1
- **Hull/Domain Shaders**: hs_5_0, ds_5_0

### Compilation Pipeline
```cpp
// Shader compilation process
reshade::api::pipeline compile_shader(const std::string &hlsl_source, 
                                     reshade::api::shader_stage stage,
                                     const std::string &entry_point)
{
    // 1. Parse HLSL source
    // 2. Validate syntax and semantics
    // 3. Compile to target shader model
    // 4. Create pipeline state object
    // 5. Return compiled pipeline
}
```

### Resource Binding
```cpp
// Bind resources to shader
void bind_shader_resources(reshade::api::command_list *cmd_list,
                          reshade::api::pipeline pipeline,
                          const resource_bindings &bindings)
{
    // Bind textures
    for (const auto &texture : bindings.textures) {
        cmd_list->bind_resource_view(stage, slot, texture.view);
    }
    
    // Bind buffers
    for (const auto &buffer : bindings.buffers) {
        cmd_list->bind_resource_view(stage, slot, buffer.view);
    }
    
    // Bind constant buffers
    for (const auto &cbuffer : bindings.constant_buffers) {
        cmd_list->bind_resource_view(stage, slot, cbuffer.view);
    }
}
```

## Game-Specific Shaders (`src/games/*/`)

### Shader Organization
Each game directory contains:
- **`*.hlsl`**: Custom shader files
- **`addon.cpp`**: Shader injection logic
- **`addon.h`**: Shader declarations and structures

### Shader Patterns
```hlsl
// Common shader modification pattern
#include "common.hlsl"

// Original shader input/output structures
struct VS_INPUT {
    float3 position : POSITION;
    float2 texcoord : TEXCOORD0;
    // ... other inputs
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    // ... other outputs
};

// Custom shader modifications
PS_INPUT vertex_shader(VS_INPUT input)
{
    PS_INPUT output;
    
    // Apply custom vertex transformations
    output.position = mul(world_view_projection, float4(input.position, 1.0));
    output.texcoord = input.texcoord;
    
    // Apply custom effects
    output.texcoord = apply_custom_uv_transform(input.texcoord);
    
    return output;
}

float4 pixel_shader(PS_INPUT input) : SV_Target
{
    // Sample original texture
    float4 original_color = texture0.Sample(sampler0, input.texcoord);
    
    // Apply custom pixel effects
    float4 modified_color = apply_custom_effects(original_color, input.texcoord);
    
    return modified_color;
}
```

## Shader Injection Techniques

### 1. Direct Replacement
```cpp
// Replace existing shader with custom version
void inject_custom_shader(reshade::api::command_list *cmd_list,
                         reshade::api::pipeline original_pipeline,
                         reshade::api::pipeline custom_pipeline)
{
    // Unbind original shader
    cmd_list->bind_pipeline_states(reshade::api::pipeline_stage::all, 0, nullptr);
    
    // Bind custom shader
    cmd_list->bind_pipeline_states(reshade::api::pipeline_stage::all, 1, &custom_pipeline);
}
```

### 2. Conditional Injection
```cpp
// Inject shader based on conditions
void conditional_shader_injection(reshade::api::command_list *cmd_list,
                                 const render_state &state)
{
    if (state.shader_hash == target_shader_hash && 
        settings.enable_modification) {
        // Inject custom shader
        inject_custom_shader(cmd_list, state.pipeline, custom_pipeline);
    } else {
        // Use original shader
        cmd_list->bind_pipeline_states(reshade::api::pipeline_stage::all, 
                                      1, &state.pipeline);
    }
}
```

### 3. Resource Injection
```cpp
// Inject custom resources into shader
void inject_shader_resources(reshade::api::command_list *cmd_list,
                            const shader_resources &resources)
{
    // Inject custom textures
    for (const auto &texture : resources.textures) {
        cmd_list->bind_resource_view(texture.stage, texture.slot, texture.view);
    }
    
    // Inject custom buffers
    for (const auto &buffer : resources.buffers) {
        cmd_list->bind_resource_view(buffer.stage, buffer.slot, buffer.view);
    }
}
```

## Shader Analysis Tools

### Shader Profiling
```cpp
// Profile shader performance
struct shader_profile {
    uint64_t compilation_time;
    uint64_t execution_time;
    uint32_t instruction_count;
    uint32_t texture_samples;
    uint32_t buffer_accesses;
};

shader_profile profile_shader(reshade::api::pipeline pipeline)
{
    shader_profile profile = {};
    
    // Measure compilation time
    auto start_time = std::chrono::high_resolution_clock::now();
    // ... compilation logic
    auto end_time = std::chrono::high_resolution_clock::now();
    profile.compilation_time = std::chrono::duration_cast<std::chrono::microseconds>
                              (end_time - start_time).count();
    
    return profile;
}
```

### Shader Validation
```cpp
// Validate shader compatibility
bool validate_shader(reshade::api::pipeline pipeline, 
                    const shader_requirements &requirements)
{
    // Check shader model compatibility
    if (pipeline.shader_model < requirements.min_shader_model) {
        return false;
    }
    
    // Check resource binding compatibility
    if (!validate_resource_bindings(pipeline, requirements.resources)) {
        return false;
    }
    
    // Check feature level compatibility
    if (pipeline.feature_level < requirements.min_feature_level) {
        return false;
    }
    
    return true;
}
```

## Performance Optimization

### Shader Caching
```cpp
// Cache compiled shaders
class shader_cache {
private:
    std::unordered_map<std::string, reshade::api::pipeline> compiled_shaders;
    
public:
    reshade::api::pipeline get_or_compile(const std::string &hlsl_source,
                                         reshade::api::shader_stage stage)
    {
        std::string cache_key = generate_cache_key(hlsl_source, stage);
        
        auto it = compiled_shaders.find(cache_key);
        if (it != compiled_shaders.end()) {
            return it->second;
        }
        
        // Compile new shader
        reshade::api::pipeline pipeline = compile_shader(hlsl_source, stage);
        compiled_shaders[cache_key] = pipeline;
        
        return pipeline;
    }
};
```

### Resource Pooling
```cpp
// Pool shader resources
class resource_pool {
private:
    std::vector<reshade::api::resource> available_resources;
    
public:
    reshade::api::resource acquire_resource(const resource_desc &desc)
    {
        // Find available resource with matching description
        for (auto &resource : available_resources) {
            if (resource.desc == desc) {
                // Remove from pool and return
                auto result = resource;
                available_resources.erase(
                    std::remove(available_resources.begin(), 
                               available_resources.end(), resource),
                    available_resources.end());
                return result;
            }
        }
        
        // Create new resource if none available
        return device->create_resource(desc);
    }
    
    void release_resource(reshade::api::resource resource)
    {
        available_resources.push_back(resource);
    }
};
```

## Debugging and Development

### Shader Debugging
```cpp
// Enable shader debugging
void enable_shader_debugging(reshade::api::device *device)
{
    // Enable debug layer
    device->enable_debug_layer();
    
    // Enable shader debugging
    device->enable_shader_debugging();
    
    // Set debug callback
    device->set_debug_callback([](reshade::api::debug_source source,
                                 reshade::api::debug_type type,
                                 reshade::api::debug_severity severity,
                                 const char *message) {
        // Log debug messages
        reshade::log_message(reshade::log_level::debug, message);
    });
}
```

### Shader Hot-Reloading
```cpp
// Hot-reload shaders during development
class shader_hot_reloader {
private:
    std::unordered_map<std::string, std::filesystem::file_time_type> file_times;
    
public:
    void check_for_updates()
    {
        for (const auto &shader_file : shader_files) {
            auto current_time = std::filesystem::last_write_time(shader_file);
            
            if (file_times[shader_file] != current_time) {
                // File modified, reload shader
                reload_shader(shader_file);
                file_times[shader_file] = current_time;
            }
        }
    }
};
```

## Best Practices

### Shader Development
1. **Modular Design**: Use include files for common functionality
2. **Error Handling**: Validate shader compilation results
3. **Performance**: Profile shaders for performance impact
4. **Compatibility**: Test with different hardware and drivers

### Resource Management
1. **Memory Efficiency**: Minimize resource allocation overhead
2. **Caching**: Cache frequently used shaders and resources
3. **Cleanup**: Properly release resources to prevent leaks
4. **Pooling**: Use resource pools for frequently allocated resources

### Debugging
1. **Validation**: Use debug layers during development
2. **Logging**: Log shader compilation and execution events
3. **Profiling**: Monitor shader performance in real-time
4. **Hot-Reloading**: Enable shader hot-reloading for faster iteration









