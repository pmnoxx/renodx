# Custom FPS Limiter vs RenoDX Built-in: Detailed Comparison

## Overview

This document analyzes the differences between our Custom FPS Limiter implementation in Display Commander and RenoDX's built-in FPS limiter. Both use the same core algorithm but differ in several key aspects.

## Core Algorithm Similarities

Both implementations use identical core logic:

1. **Hybrid Sleep + Spin-Lock Strategy**
2. **Adaptive Sleep Duration Calculation**
3. **Sleep Latency History Management**
4. **Spin-Lock Duration Optimization**
5. **Failure Recovery Mechanisms**
6. **Precision Spin-Lock for Final Timing**

## Key Differences

### 1. **Architecture & Scope**

#### **RenoDX Built-in**
- **Location**: `src/utils/swapchain.hpp` (internal namespace)
- **Scope**: Global, system-wide FPS limiting
- **Integration**: Direct ReShade event hook
- **State**: Static variables, single instance per process

#### **Custom FPS Limiter**
- **Location**: `src/addons/display_commander/dxgi/`
- **Scope**: Addon-specific, user-controllable
- **Integration**: Through Display Commander's event system
- **State**: Instance-based, multiple instances possible

### 2. **Event Integration**

#### **RenoDX Built-in**
```cpp
// Direct ReShade event registration
reshade::register_event<reshade::addon_event::present>(internal::OnPresent);

// Called automatically every frame
static void OnPresent(reshade::api::command_queue* queue, ...) {
    // FPS limiting logic
}
```

#### **Custom FPS Limiter**
```cpp
// Indirect integration through Display Commander
void OnPresentUpdate(...) {
    if (s_custom_fps_limiter_enabled > 0.5f) {
        auto& limiter = g_customFpsLimiterManager.GetFpsLimiter();
        if (limiter.IsEnabled()) {
            limiter.LimitFrameRate();  // Calls UpdateTiming()
        }
    }
}
```

### 3. **Frame Lifecycle Tracking**

#### **RenoDX Built-in**
- **No frame lifecycle tracking**
- **Timing based solely on `OnPresent` calls**
- **Assumes consistent frame timing**

#### **Custom FPS Limiter**
```cpp
// Frame lifecycle hooks
void OnFrameBegin() {
    m_frame_start_time = std::chrono::high_resolution_clock::now();
    m_frame_in_progress = true;
}

void OnFrameEnd() {
    m_frame_end_time = std::chrono::high_resolution_clock::now();
    m_frame_in_progress = false;
}
```

**Benefits of Frame Lifecycle Tracking:**
- More accurate timing calculations
- Better understanding of actual rendering time
- Potential for future optimizations based on frame complexity

### 4. **State Management**

#### **RenoDX Built-in**
```cpp
namespace internal {
    static std::chrono::high_resolution_clock::time_point last_time_point;
    static float last_fps_limit;
    static auto spin_lock_duration = std::chrono::nanoseconds(0);
    static std::uint32_t spin_lock_failures = 0;
    static std::deque<std::chrono::nanoseconds> sleep_latency_history;
    static const size_t MAX_LATENCY_HISTORY_SIZE = 1000;
}
```

#### **Custom FPS Limiter**
```cpp
class CustomFpsLimiter {
private:
    // Instance-based state
    std::chrono::high_resolution_clock::time_point m_last_present_time;
    float m_last_fps_limit = 0.0f;
    std::chrono::nanoseconds m_spin_lock_duration{0};
    int m_spin_lock_failures = 0;
    std::deque<std::chrono::nanoseconds> m_sleep_latency_history;
    static constexpr size_t MAX_LATENCY_HISTORY_SIZE = 100;  // Reduced from 1000
};
```

### 5. **Configuration & Control**

#### **RenoDX Built-in**
- **Global control**: `renodx::utils::swapchain::fps_limit`
- **No enable/disable**: Always active when `fps_limit > 0`
- **No UI integration**: Must be set programmatically

#### **Custom FPS Limiter**
- **Instance-based control**: `m_enabled` flag
- **UI integration**: Toggle and slider in Display Commander
- **Dynamic enable/disable**: Can be turned on/off at runtime
- **Independent operation**: Works alongside or instead of built-in

### 6. **Memory Usage**

#### **RenoDX Built-in**
- **MAX_LATENCY_HISTORY_SIZE = 1000**
- **Fixed memory overhead**: ~8KB per process
- **Global memory**: Shared across all swapchains

#### **Custom FPS Limiter**
- **MAX_LATENCY_HISTORY_SIZE = 100**
- **Reduced memory overhead**: ~800 bytes per instance
- **Instance memory**: Per-limiter instance

### 7. **Initialization & Lifecycle**

#### **RenoDX Built-in**
```cpp
// Automatic during DLL initialization
static void Use(DWORD fdw_reason) {
    case DLL_PROCESS_ATTACH:
        reshade::register_event<reshade::addon_event::present>(internal::OnPresent);
}
```

#### **Custom FPS Limiter**
```cpp
// Manual initialization through manager
bool InitializeCustomFpsLimiterSystem() {
    renodx::utils::swapchain::fps_limit = 0.0f;  // Disable built-in
    return true;
}

void ShutdownCustomFpsLimiterSystem() {
    renodx::utils::swapchain::fps_limit = 60.0f;  // Re-enable built-in
}
```

### 8. **Error Handling & Logging**

#### **RenoDX Built-in**
- **Minimal logging**: Basic info messages
- **No error handling**: Assumes stable operation
- **System-level integration**: Part of core RenoDX

#### **Custom FPS Limiter**
- **Enhanced logging**: Warning messages for user actions
- **User feedback**: Clear indication of enable/disable state
- **Addon-level integration**: Part of Display Commander

## Performance Implications

### **Memory Usage**
- **RenoDX**: 1000 samples × 8 bytes = 8KB (fixed)
- **Custom**: 100 samples × 8 bytes = 800 bytes (per instance)
- **Custom advantage**: 10x less memory per instance

### **CPU Overhead**
- **RenoDX**: Minimal, optimized for system-wide use
- **Custom**: Slightly higher due to additional frame lifecycle tracking
- **Trade-off**: More information vs. slightly higher overhead

### **Timing Accuracy**
- **RenoDX**: Based on present timing only
- **Custom**: Based on present timing + frame lifecycle awareness
- **Custom advantage**: More context for future optimizations

## Compatibility & Integration

### **RenoDX Built-in**
- **Universal**: Works with any DirectX application
- **Automatic**: No configuration required
- **System-level**: Affects entire process

### **Custom FPS Limiter**
- **Addon-specific**: Only active when Display Commander is loaded
- **User-controlled**: Can be enabled/disabled by user
- **Coexistence**: Can work alongside or replace built-in

## Future Enhancement Potential

### **RenoDX Built-in**
- **Limited**: Core system component, changes affect all users
- **Stable**: Well-tested, production-ready
- **Performance-focused**: Optimized for minimal overhead

### **Custom FPS Limiter**
- **Extensible**: Easy to add new features
- **User-specific**: Can implement user preferences
- **Experimental**: Platform for testing new approaches

## Recommendations

### **When to Use RenoDX Built-in**
- **Simple FPS limiting needs**
- **System-wide consistency**
- **Minimal resource usage**
- **Production environments**

### **When to Use Custom FPS Limiter**
- **Advanced FPS limiting features**
- **User-specific customization**
- **Frame lifecycle awareness**
- **Development and testing**
- **Addon-specific requirements**

## Conclusion

The Custom FPS Limiter successfully clones RenoDX's proven algorithm while adding several enhancements:

1. **Frame Lifecycle Awareness**: Better understanding of rendering timing
2. **User Control**: Enable/disable and configuration through UI
3. **Reduced Memory**: 10x less memory usage per instance
4. **Addon Integration**: Seamless integration with Display Commander
5. **Coexistence**: Can work alongside or replace built-in limiter

The core algorithm remains identical, ensuring the same level of timing accuracy and performance optimization. The differences are primarily architectural and provide additional flexibility and user control without sacrificing the proven effectiveness of RenoDX's approach.

## Technical Notes

### **Spin-Lock Usage**
Both implementations use spin-lock for final precision timing, which is acceptable for:
- Microsecond-level precision requirements
- Low-latency applications
- Systems where CPU efficiency is secondary to timing accuracy

### **Memory Optimization**
The Custom FPS Limiter's reduced history size (100 vs 1000) may slightly reduce adaptation speed but significantly reduces memory overhead, making it suitable for addon use where multiple instances might be created.
