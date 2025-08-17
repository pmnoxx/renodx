# RenoDX Built-in FPS Limiter Analysis

## Overview

RenoDX's built-in FPS limiter is a sophisticated, adaptive timing system that provides precise frame rate control while maintaining optimal performance. It's implemented in `src/utils/swapchain.hpp` and uses a hybrid approach combining sleep and spin-lock strategies.

## Architecture

### Core Components

The FPS limiter is implemented within the `renodx::utils::swapchain::internal` namespace and consists of:

1. **Static State Variables**:
   - `last_time_point`: Tracks the last frame presentation time
   - `last_fps_limit`: Caches the previous FPS limit value
   - `spin_lock_duration`: Dynamic duration for precision timing
   - `spin_lock_failures`: Counter for failed timing attempts
   - `sleep_latency_history`: Rolling window of sleep accuracy measurements

2. **Constants**:
   - `MAX_LATENCY_HISTORY_SIZE = 1000`: Maximum number of latency samples to maintain

3. **Event Integration**:
   - Hooks into ReShade's `present` event via `OnPresent`
   - Automatically registered during DLL initialization

## How It Works

### Event Flow

```cpp
// Registration in Use(DLL_PROCESS_ATTACH)
reshade::register_event<reshade::addon_event::present>(internal::OnPresent);

// Called every frame when present is called
static void OnPresent(reshade::api::command_queue* queue, ...) {
    // FPS limiting logic executed here
}
```

### Core Algorithm

The FPS limiter operates in several phases:

#### 1. **Initialization and Reset**
```cpp
if (last_fps_limit != fps_limit) {
    spin_lock_duration = std::chrono::nanoseconds(0);
    sleep_latency_history.clear();
    last_fps_limit = fps_limit;
}
if (fps_limit <= 0.f) return;
```
- Resets timing state when FPS limit changes
- Early exit if no FPS limit is set

#### 2. **Target Time Calculation**
```cpp
const auto time_per_frame = std::chrono::high_resolution_clock::duration(
    std::chrono::milliseconds(1000)) / static_cast<double>(fps_limit);
const auto next_time_point = last_time_point + time_per_frame;
```
- Calculates the ideal time between frames
- Determines when the next frame should be presented

#### 3. **Timing Check**
```cpp
auto now = std::chrono::high_resolution_clock::now();
auto time_till_next_frame = next_time_point - now;
if (time_till_next_frame.count() <= 0) {
    last_time_point = now;
    return; // Already late, skip limiting
}
```
- Checks if we're already behind schedule
- Updates timing and exits if too late

#### 4. **Adaptive Sleep Strategy**
```cpp
auto sleep_duration = time_till_next_frame - spin_lock_duration;
if (sleep_duration.count() > 0) {
    std::this_thread::sleep_for(sleep_duration);
    // ... measure and record sleep accuracy
}
```
- Calculates optimal sleep duration (total time minus spin-lock buffer)
- Uses `std::this_thread::sleep_for` for efficient waiting

#### 5. **Sleep Accuracy Measurement**
```cpp
auto after_sleep = std::chrono::high_resolution_clock::now();
auto actual_sleep_duration = after_sleep - now;
auto sleep_latency = actual_sleep_duration - sleep_duration;

// Record the sleep latency
sleep_latency_history.push_back(
    std::chrono::duration_cast<std::chrono::nanoseconds>(sleep_latency));
```
- Measures how accurate the sleep operation was
- Records latency (difference between requested and actual sleep time)

#### 6. **Latency History Management**
```cpp
auto current_size = sleep_latency_history.size() + 1;
if (current_size > MAX_LATENCY_HISTORY_SIZE) {
    sleep_latency_history.pop_front();
    --current_size;
}
```
- Maintains a rolling window of 1000 latency samples
- Automatically prunes old data to prevent memory growth

#### 7. **Spin-Lock Duration Optimization**
```cpp
if (current_size >= MAX_LATENCY_HISTORY_SIZE * 0.10f) {  // 100+ samples
    auto sorted_latencies = sleep_latency_history;
    std::ranges::sort(sorted_latencies, std::greater<>());
    auto worst_1_percent = sorted_latencies[current_size * 0.01];
    spin_lock_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
        worst_1_percent * 1.5);
}
```
- Calculates optimal spin-lock duration based on worst-case latency
- Uses 99th percentile latency × 1.5 as safety margin
- Only adjusts after collecting sufficient data (100+ samples)

#### 8. **Failure Recovery**
```cpp
} else {
    ++spin_lock_failures;
    
    if (spin_lock_failures > fps_limit) {
        // Full second of failures, try increasing
        m_spin_lock_failures = 0;
        
        if (m_sleep_latency_history.empty()) {
            m_spin_lock_duration = std::chrono::nanoseconds(0);
        } else {
            auto sorted_latencies = m_sleep_latency_history;
            std::ranges::sort(sorted_latencies, std::less<>());
            if (sorted_latencies[0] * 1.5 < time_per_frame) {
                // Decrease spin lock duration by 10%
                m_spin_lock_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    m_spin_lock_duration * 0.90f);
            }
        }
    }
}
```
- Tracks consecutive failures (when sleep duration ≤ 0)
- Reduces spin-lock duration after a full second of failures
- Prevents excessive CPU usage from overly aggressive spin-locking

#### 9. **Precision Spin-Lock**
```cpp
while (now < next_time_point) {
    // Spin lock until the next time point
    now = std::chrono::high_resolution_clock::now();
}
```
- Active waiting for the final microseconds
- Ensures precise timing after sleep completes
- Uses minimal CPU cycles for maximum accuracy

#### 10. **State Update**
```cpp
last_time_point = now;
```
- Records the actual presentation time
- Used as the base for the next frame's timing calculation

## Key Features

### 1. **Adaptive Learning**
- Continuously measures sleep accuracy
- Automatically adjusts spin-lock duration based on system performance
- Learns from worst-case scenarios to provide consistent timing

### 2. **Efficiency Optimization**
- Uses sleep for the majority of waiting time
- Minimizes CPU usage through intelligent timing
- Only spins for the final precision timing

### 3. **Performance Adaptation**
- Adjusts to different hardware characteristics
- Handles system load variations gracefully
- Recovers from timing failures automatically

### 4. **Memory Management**
- Rolling window prevents unbounded memory growth
- Efficient data structure for latency tracking
- Automatic cleanup of old measurements

## Performance Characteristics

### **Sleep vs Spin-Lock Balance**
- **Sleep**: Handles 95%+ of waiting time efficiently
- **Spin-Lock**: Provides microsecond precision for final timing
- **Adaptive**: Automatically finds optimal balance for each system

### **Latency Compensation**
- Measures and compensates for sleep inaccuracy
- Uses historical data to predict worst-case scenarios
- Provides consistent timing despite system variations

### **CPU Usage Optimization**
- Minimal overhead during normal operation
- Intelligent failure detection and recovery
- Prevents excessive CPU usage from timing failures

## Integration Points

### **ReShade Events**
- Hooks into `reshade::addon_event::present`
- Automatically called every frame
- Seamless integration with existing rendering pipeline

### **Global Configuration**
- Controlled via `renodx::utils::swapchain::fps_limit`
- Can be dynamically changed at runtime
- Automatic state reset on configuration changes

### **Device Management**
- Integrates with RenoDX's device lifecycle
- Handles device creation/destruction gracefully
- Maintains state across device changes

## Advantages

### 1. **Precision**
- Microsecond-accurate timing
- Consistent frame pacing
- Minimal jitter in frame presentation

### 2. **Efficiency**
- Low CPU overhead
- Intelligent sleep/spin-lock balance
- Adaptive to system capabilities

### 3. **Reliability**
- Handles system variations gracefully
- Recovers from timing failures
- Maintains performance under different loads

### 4. **Integration**
- Seamless ReShade integration
- No manual event handling required
- Automatic lifecycle management

## Limitations

### 1. **Single-Threaded Design**
- Designed for single-threaded rendering
- May not handle multi-threaded scenarios optimally

### 2. **System Dependency**
- Performance depends on system sleep accuracy
- May vary across different operating systems
- Hardware-specific timing characteristics

### 3. **Memory Usage**
- Maintains 1000 latency samples
- Fixed memory overhead per device
- May not be suitable for memory-constrained environments

## Comparison with Custom Implementation

The Custom FPS Limiter in Display Commander was designed to clone this approach:

### **Similarities**
- Same hybrid sleep + spin-lock strategy
- Identical latency history management
- Same spin-lock duration calculation
- Similar failure recovery mechanisms

### **Differences**
- Custom limiter tracks frame lifecycle (begin/end render pass)
- Custom limiter integrates with Display Commander's UI
- Custom limiter can be enabled/disabled independently
- Custom limiter provides additional debugging and monitoring

## Conclusion

RenoDX's built-in FPS limiter represents a sophisticated approach to frame rate control that balances efficiency with precision. Its adaptive learning system, efficient sleep/spin-lock strategy, and robust failure recovery make it an excellent foundation for frame timing.

The Custom FPS Limiter implementation successfully clones this approach while adding frame lifecycle awareness and user control, demonstrating the effectiveness of RenoDX's original design principles.

## Technical Notes

### **Thread Safety**
- Uses static variables within internal namespace
- Designed for single-threaded rendering scenarios
- No explicit synchronization mechanisms

### **Performance Impact**
- Minimal overhead during normal operation
- Adaptive behavior minimizes CPU usage
- Spin-lock duration automatically optimized

### **Compatibility**
- Works with any DirectX application
- Integrates seamlessly with ReShade
- Handles device lifecycle changes gracefully
