# API Reference Documentation

## Overview

This document provides comprehensive API reference for RenoDX's core systems, including Reshade integration, shader management, resource handling, and utility functions.

## Core API Components

### Reshade Integration API

#### Addon Lifecycle Functions
```cpp
// Addon initialization
bool addon_init(reshade::api::device *device);

// Addon cleanup
void addon_uninit(reshade::api::device *device);

// Configuration loading
void addon_load_config(reshade::api::effect_runtime *runtime);

// Configuration saving
void addon_save_config(reshade::api::effect_runtime *runtime);
```

#### Hook Registration
```cpp
// Register addon with Reshade
bool reshade::register_addon(const char *name, void *addon);

// Register event hooks
template<typename Event>
void reshade::register_event(Event callback);

// Unregister event hooks
template<typename Event>
void reshade::unregister_event(Event callback);
```

#### Logging API
```cpp
// Log levels
enum class log_level {
    error,
    warning,
    info,
    debug
};

// Log message
void reshade::log_message(log_level level, const char *format, ...);
```

### Device and Resource Management

#### Device API
```cpp
// Device creation
reshade::api::device *create_device(reshade::api::device_api api, void *native_device);

// Device destruction
void destroy_device(reshade::api::device *device);

// Get device capabilities
reshade::api::device_caps get_device_caps(reshade::api::device *device);
```

#### Resource Creation
```cpp
// Create texture resource
reshade::api::resource create_texture_2d(reshade::api::device *device,
                                        uint32_t width, uint32_t height,
                                        reshade::api::format format,
                                        uint32_t mip_levels = 1,
                                        uint32_t array_size = 1,
                                        reshade::api::resource_usage usage = reshade::api::resource_usage::shader_resource);

// Create buffer resource
reshade::api::resource create_buffer(reshade::api::device *device,
                                    uint64_t size,
                                    reshade::api::resource_usage usage = reshade::api::resource_usage::shader_resource);

// Create resource view
reshade::api::resource_view create_resource_view(reshade::api::device *device,
                                                reshade::api::resource resource,
                                                reshade::api::resource_usage usage,
                                                reshade::api::format format,
                                                uint32_t first_level = 0,
                                                uint32_t levels = 1,
                                                uint32_t first_layer = 0,
                                                uint32_t layers = 1);
```

#### Resource Destruction
```cpp
// Destroy resource
void destroy_resource(reshade::api::device *device, reshade::api::resource resource);

// Destroy resource view
void destroy_resource_view(reshade::api::device *device, reshade::api::resource_view view);
```

### Command List API

#### Command List Management
```cpp
// Create command list
reshade::api::command_list *create_command_list(reshade::api::device *device);

// Destroy command list
void destroy_command_list(reshade::api::device *device, reshade::api::command_list *cmd_list);

// Reset command list
void reset_command_list(reshade::api::command_list *cmd_list);

// Close command list
void close_command_list(reshade::api::command_list *cmd_list);
```

#### Drawing Commands
```cpp
// Draw indexed
void draw_indexed(reshade::api::command_list *cmd_list,
                 uint32_t index_count,
                 uint32_t instance_count = 1,
                 uint32_t first_index = 0,
                 int32_t vertex_offset = 0,
                 uint32_t first_instance = 0);

// Draw
void draw(reshade::api::command_list *cmd_list,
         uint32_t vertex_count,
         uint32_t instance_count = 1,
         uint32_t first_vertex = 0,
         uint32_t first_instance = 0);
```

#### Resource Binding
```cpp
// Bind vertex buffers
void bind_vertex_buffers(reshade::api::command_list *cmd_list,
                        uint32_t first_slot,
                        uint32_t count,
                        const reshade::api::resource *buffers,
                        const uint64_t *offsets,
                        const uint32_t *strides);

// Bind index buffer
void bind_index_buffer(reshade::api::command_list *cmd_list,
                      reshade::api::resource buffer,
                      uint64_t offset,
                      uint32_t index_size);

// Bind resource views
void bind_resource_view(reshade::api::command_list *cmd_list,
                       reshade::api::shader_stage stage,
                       uint32_t slot,
                       reshade::api::resource_view view);

// Bind pipeline states
void bind_pipeline_states(reshade::api::command_list *cmd_list,
                         reshade::api::pipeline_stage stages,
                         uint32_t count,
                         const reshade::api::pipeline *pipelines);
```

### Shader Management API

#### Shader Compilation
```cpp
// Compile shader from HLSL source
reshade::api::pipeline compile_shader(reshade::api::device *device,
                                     const std::string &hlsl_source,
                                     reshade::api::shader_stage stage,
                                     const std::string &entry_point = "main",
                                     const std::vector<std::string> &defines = {});

// Compile shader from file
reshade::api::pipeline compile_shader_from_file(reshade::api::device *device,
                                               const std::string &file_path,
                                               reshade::api::shader_stage stage,
                                               const std::string &entry_point = "main");

// Destroy shader pipeline
void destroy_pipeline(reshade::api::device *device, reshade::api::pipeline pipeline);
```

#### Shader Analysis
```cpp
// Get shader information
struct shader_info {
    std::string entry_point;
    reshade::api::shader_stage stage;
    std::vector<std::string> input_signature;
    std::vector<std::string> output_signature;
    std::vector<std::string> resource_bindings;
};

shader_info get_shader_info(reshade::api::pipeline pipeline);

// Analyze shader dependencies
std::vector<std::string> get_shader_dependencies(const std::string &hlsl_source);
```

### Utility Functions

#### Hash Functions
```cpp
// CRC32 hash calculation
uint32_t crc32_hash(const void *data, size_t size);

// String hash
uint32_t string_hash(const std::string &str);

// Resource hash
uint64_t resource_hash(const reshade::api::resource_desc &desc);
```

#### File Operations
```cpp
// Read file contents
std::vector<uint8_t> read_file(const std::string &file_path);

// Write file contents
bool write_file(const std::string &file_path, const std::vector<uint8_t> &data);

// Check file exists
bool file_exists(const std::string &file_path);

// Get file size
uint64_t get_file_size(const std::string &file_path);
```

#### Memory Management
```cpp
// Allocate aligned memory
void *aligned_alloc(size_t size, size_t alignment);

// Free aligned memory
void aligned_free(void *ptr);

// Memory copy with alignment
void aligned_memcpy(void *dst, const void *src, size_t size, size_t alignment);
```

### Configuration API

#### Settings Management
```cpp
// Load settings from file
bool load_settings(const std::string &file_path, std::unordered_map<std::string, std::string> &settings);

// Save settings to file
bool save_settings(const std::string &file_path, const std::unordered_map<std::string, std::string> &settings);

// Get setting value
template<typename T>
T get_setting(const std::unordered_map<std::string, std::string> &settings,
              const std::string &key,
              const T &default_value = T{});

// Set setting value
template<typename T>
void set_setting(std::unordered_map<std::string, std::string> &settings,
                const std::string &key,
                const T &value);
```

### Performance Monitoring API

#### Timing Functions
```cpp
// High-resolution timer
class high_resolution_timer {
public:
    void start();
    void stop();
    uint64_t elapsed_microseconds() const;
    double elapsed_milliseconds() const;
    double elapsed_seconds() const;
    
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    bool is_running = false;
};
```

#### Performance Counters
```cpp
// Performance counter
class performance_counter {
public:
    void begin_frame();
    void end_frame();
    void begin_event(const std::string &name);
    void end_event();
    
    // Get statistics
    double get_average_frame_time() const;
    double get_min_frame_time() const;
    double get_max_frame_time() const;
    uint64_t get_frame_count() const;
    
private:
    high_resolution_timer frame_timer;
    std::vector<double> frame_times;
    uint64_t frame_count = 0;
};
```

### Debug and Development API

#### Debug Utilities
```cpp
// Enable debug layer
void enable_debug_layer(reshade::api::device *device);

// Set debug callback
void set_debug_callback(reshade::api::device *device,
                       std::function<void(reshade::api::debug_source,
                                        reshade::api::debug_type,
                                        reshade::api::debug_severity,
                                        const char *)> callback);

// Debug break
void debug_break();

// Assert macro
#define RENODX_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            reshade::log_message(reshade::log_level::error, "Assertion failed: %s", message); \
            debug_break(); \
        } \
    } while(0)
```

#### Development Tools
```cpp
// Shader hot-reloading
class shader_hot_reloader {
public:
    void add_shader_file(const std::string &file_path);
    void remove_shader_file(const std::string &file_path);
    void check_for_updates();
    
private:
    std::unordered_map<std::string, std::filesystem::file_time_type> file_times;
    std::vector<std::string> shader_files;
};

// Resource leak detection
class resource_leak_detector {
public:
    void track_resource(reshade::api::resource resource, const std::string &allocation_site);
    void untrack_resource(reshade::api::resource resource);
    void report_leaks();
    
private:
    std::unordered_map<reshade::api::resource, std::string> tracked_resources;
};
```

## Error Handling

### Error Codes
```cpp
// Error codes
enum class error_code {
    success = 0,
    invalid_parameter,
    resource_not_found,
    compilation_failed,
    device_lost,
    out_of_memory,
    not_supported,
    unknown_error
};

// Error information
struct error_info {
    error_code code;
    std::string message;
    std::string function;
    std::string file;
    uint32_t line;
};
```

### Exception Handling
```cpp
// RenoDX exception
class renodx_exception : public std::exception {
public:
    renodx_exception(error_code code, const std::string &message);
    
    error_code get_error_code() const;
    const char *what() const noexcept override;
    
private:
    error_code code;
    std::string message;
};
```

## Best Practices

### API Usage Guidelines
1. **Error Checking**: Always check return values and handle errors appropriately
2. **Resource Management**: Use RAII patterns for resource management
3. **Performance**: Minimize API calls and use batching when possible
4. **Threading**: Be aware of threading requirements for different APIs
5. **Compatibility**: Check feature support before using advanced features

### Memory Management
1. **Allocation**: Use appropriate allocation strategies for different resource types
2. **Cleanup**: Always release resources in the correct order
3. **Caching**: Cache frequently used resources to improve performance
4. **Pooling**: Use resource pools for frequently allocated objects

### Debugging
1. **Validation**: Enable validation layers during development
2. **Logging**: Use appropriate log levels for different types of information
3. **Profiling**: Profile performance-critical code paths
4. **Testing**: Test with different hardware configurations







