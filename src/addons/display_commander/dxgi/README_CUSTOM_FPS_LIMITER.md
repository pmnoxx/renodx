# Custom FPS Limiter System

This directory contains a complete custom FPS limiting system that replaces RenoDX's built-in FPS limiter with your own implementation.

## 🎯 **What This System Provides**

- **Complete Control**: Your own FPS limiting logic in the `OnPresent` event
- **Multiple Strategies**: Choose from different sleep/timing approaches
- **Performance Monitoring**: Real-time FPS tracking and statistics
- **Easy Integration**: Simple API to integrate into your addon
- **Conflict Prevention**: Automatically disables RenoDX's built-in limiter

## 📁 **File Structure**

```
dxgi/
├── custom_fps_limiter.hpp          # Main FPS limiter class header
├── custom_fps_limiter.cpp          # Main FPS limiter implementation
├── custom_fps_limiter_events.hpp   # ReShade event integration header
├── custom_fps_limiter_events.cpp   # ReShade event integration implementation
├── custom_fps_limiter_manager.hpp  # System management header
├── custom_fps_limiter_manager.cpp  # System management implementation
├── custom_fps_limiter_example.cpp  # Usage examples
└── README_CUSTOM_FPS_LIMITER.md    # This file
```

## 🚀 **Quick Start**

### **1. Basic Integration**

```cpp
#include "dxgi/custom_fps_limiter_manager.hpp"

// In your DLL_PROCESS_ATTACH
if (renodx::dxgi::fps_limiter::InitializeCustomFpsLimiterSystem()) {
    // Set target FPS
    renodx::dxgi::fps_limiter::SetCustomFpsLimiterTarget(28.0f);
    
    // Enable the limiter
    renodx::dxgi::fps_limiter::EnableCustomFpsLimiter(true);
}
```

### **2. Advanced Usage**

```cpp
// Get the manager instance
auto& manager = renodx::dxgi::fps_limiter::g_customFpsLimiterManager;

// Configure advanced settings
manager.SetLoggingEnabled(true);
manager.SetMinSleepTime(std::chrono::microseconds(500));  // 0.5ms
manager.SetMaxSleepTime(std::chrono::microseconds(5000)); // 5ms

// Choose a strategy
manager.SetStrategy(renodx::dxgi::fps_limiter::LimiterStrategy::ADAPTIVE_SLEEP);
```

### **3. Clean Shutdown**

```cpp
// In your DLL_PROCESS_DETACH
renodx::dxgi::fps_limiter::ShutdownCustomFpsLimiterSystem();
```

## 🎮 **FPS Limiting Strategies**

### **1. SLEEP_ONLY**
- **Pros**: Simple, low CPU usage
- **Cons**: Less precise timing
- **Best for**: Basic FPS limiting, battery saving

### **2. HYBRID_SLEEP_YIELD** (Recommended)
- **Pros**: Good precision, moderate CPU usage
- **Cons**: Slightly more complex
- **Best for**: Most use cases, balanced performance

### **3. ADAPTIVE_SLEEP**
- **Pros**: Learns from system performance, very precise
- **Cons**: Higher CPU usage, more complex
- **Best for**: High-precision requirements, performance-critical applications

### **4. CUSTOM_CALLBACK**
- **Pros**: Complete control over timing logic
- **Cons**: Requires custom implementation
- **Best for**: Advanced users, custom timing requirements

## 📊 **Performance Monitoring**

The system provides real-time performance statistics:

```cpp
auto& manager = renodx::dxgi::fps_limiter::g_customFpsLimiterManager;

float current_fps = manager.GetCurrentFps();
float average_fps = manager.GetAverageFps();
bool is_enabled = manager.IsEnabled();
```

## ⚙️ **Configuration Options**

### **Basic Configuration**
```cpp
renodx::dxgi::fps_limiter::LimiterConfig config;
config.target_fps = 60.0f;
config.strategy = LimiterStrategy::HYBRID_SLEEP_YIELD;
config.enable_logging = true;
config.min_sleep_time = std::chrono::microseconds(1000);  // 1ms
config.max_sleep_time = std::chrono::microseconds(10000); // 10ms
config.adaptive_learning_rate = 0.1f;

g_customFpsLimiter.SetConfig(config);
```

### **Dynamic FPS Adjustment**
```cpp
// Change FPS limit at runtime
renodx::dxgi::fps_limiter::SetCustomFpsLimiterTarget(30.0f);  // 30 FPS
renodx::dxgi::fps_limiter::SetCustomFpsLimiterTarget(120.0f); // 120 FPS
renodx::dxgi::fps_limiter::SetCustomFpsLimiterTarget(0.0f);   // No limit
```

## 🔧 **Integration with Existing Code**

### **Replace RenoDX's Built-in Limiter**

The system automatically:
1. **Disables** RenoDX's built-in FPS limiter (`renodx::utils::swapchain::fps_limit = 0`)
2. **Registers** your custom `OnPresent` event handler
3. **Manages** the transition between systems

### **Use with Display Commander**

```cpp
// In your main entry point
#include "dxgi/custom_fps_limiter_manager.hpp"

// Initialize after settings are loaded
if (renodx::dxgi::fps_limiter::InitializeCustomFpsLimiterSystem()) {
    // Set FPS limit from your settings
    float fps_limit = GetFpsLimitFromSettings();
    renodx::dxgi::fps_limiter::SetCustomFpsLimiterTarget(fps_limit);
    renodx::dxgi::fps_limiter::EnableCustomFpsLimiter(fps_limit > 0.0f);
}
```

## 🧪 **Testing and Debugging**

### **Run Examples**
```cpp
// Run all examples to test the system
renodx::dxgi::fps_limiter::example::RunAllExamples();
```

### **Enable Logging**
```cpp
// Enable detailed logging
g_customFpsLimiterManager.SetLoggingEnabled(true);
```

### **Monitor Performance**
```cpp
// Check if system is working
if (g_customFpsLimiterManager.IsInitialized()) {
    float fps = g_customFpsLimiterManager.GetCurrentFps();
    LogInfo(("Current FPS: " + std::to_string(fps)).c_str());
}
```

## ⚠️ **Important Notes**

1. **Initialization Order**: Initialize after ReShade is ready and settings are loaded
2. **Conflict Prevention**: The system automatically disables RenoDX's built-in limiter
3. **Performance Impact**: Different strategies have different CPU usage characteristics
4. **Thread Safety**: All operations are thread-safe using atomic variables
5. **Error Handling**: The system includes comprehensive error handling and logging

## 🎯 **Why Use This System?**

- **No More Race Conditions**: Complete control over FPS limiting logic
- **Custom Strategies**: Implement your own timing algorithms
- **Better Performance**: Optimize for your specific use case
- **Easy Debugging**: Full visibility into what's happening
- **Future-Proof**: Easy to extend and modify

## 🚀 **Next Steps**

1. **Integrate** the system into your addon
2. **Configure** the FPS limiting strategy
3. **Test** with different FPS targets
4. **Monitor** performance and adjust settings
5. **Customize** the timing logic if needed

The system is designed to be production-ready and includes comprehensive error handling, logging, and performance monitoring. Start with the basic integration and gradually explore the advanced features as needed.
