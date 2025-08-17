# Custom Frame Limiter Implementation

## Overview

The Custom Frame Limiter is a sophisticated FPS limiting system implemented in the Display Commander addon that provides precise frame rate control while maintaining game responsiveness. It replaces RenoDX's built-in FPS limiter with a more advanced approach that tracks the complete frame lifecycle.

## Architecture

### Core Components

1. **`CustomFpsLimiter`** (`src/addons/display_commander/dxgi/custom_fps_limiter.hpp/cpp`)
   - Main limiter class implementing the FPS limiting logic
   - Handles timing calculations, sleep strategies, and performance adaptation

2. **`CustomFpsLimiterManager`** (`src/addons/display_commander/dxgi/custom_fps_limiter_manager.hpp/cpp`)
   - Manages the lifecycle of the custom FPS limiter system
   - Handles initialization, shutdown, and RenoDX built-in limiter coordination

3. **Frame Lifecycle Hooks** (`src/addons/display_commander/swapchain_events.cpp`)
   - `OnBeginRenderPass`: Marks the start of frame rendering
   - `OnEndRenderPass`: Marks the end of frame rendering
   - `OnPresentUpdate`: Applies FPS limiting between frames

## How It Works

### Frame Lifecycle Tracking

The system tracks the complete frame lifecycle using ReShade's render pass events:

```cpp
void OnBeginRenderPass(...) {
    // Record frame start time
    limiter.OnFrameBegin();
}

void OnEndRenderPass(...) {
    // Record frame end time
    limiter.OnFrameEnd();
}

void OnPresentUpdate(...) {
    // Apply FPS limiting between frames
    limiter.LimitFrameRate();
}
```

### Timing Strategy

The limiter uses a sophisticated hybrid approach combining sleep and spin-lock:

1. **Target Frame Time Calculation**:
   ```cpp
   const auto time_per_frame = std::chrono::milliseconds(1000) / target_fps;
   const auto next_time_point = last_present_time + time_per_frame;
   ```

2. **Adaptive Sleep Duration**:
   - Calculates optimal sleep time: `sleep_duration = time_till_next_frame - spin_lock_duration`
   - Uses historical latency data to optimize sleep accuracy
   - Maintains a rolling window of sleep latency measurements

3. **Spin-Lock for Precision**:
   - After sleep, uses active waiting for the final microseconds
   - Adapts spin-lock duration based on performance characteristics
   - Prevents oversleeping while maintaining accuracy

### Performance Learning

The system continuously learns and adapts:

- **Sleep Latency History**: Tracks how accurate sleep operations are
- **Spin-Lock Duration Adjustment**: Dynamically adjusts spin-lock time based on worst-case latency
- **Failure Recovery**: Reduces spin-lock duration if too many failures occur

## Key Features

### 1. Adaptive Timing
- Automatically adjusts sleep and spin-lock durations
- Learns from system performance characteristics
- Optimizes for different hardware configurations

### 2. Frame-Aware Limiting
- Respects actual rendering time
- Doesn't interfere with frame processing
- Maintains consistent frame pacing

### 3. Performance Monitoring
- Tracks sleep accuracy over time
- Adapts to system variations
- Self-tuning for optimal performance

### 4. RenoDX Integration
- Automatically disables built-in FPS limiter when enabled
- Restores built-in limiter when disabled
- Seamless integration with existing systems

## Implementation Details

### Core Algorithm

```cpp
void CustomFpsLimiter::UpdateTiming() {
    auto now = std::chrono::high_resolution_clock::now();
    
    // Calculate target frame time
    const auto time_per_frame = std::chrono::milliseconds(1000) / target_fps;
    const auto next_time_point = last_present_time + time_per_frame;
    
    // Calculate time until next frame
    auto time_till_next_frame = next_time_point - now;
    
    if (time_till_next_frame.count() <= 0) {
        last_present_time = now;
        return;
    }
    
    // Apply adaptive sleep
    auto sleep_duration = time_till_next_frame - spin_lock_duration;
    if (sleep_duration.count() > 0) {
        std::this_thread::sleep_for(sleep_duration);
        
        // Measure actual sleep accuracy
        auto after_sleep = std::chrono::high_resolution_clock::now();
        auto actual_sleep_duration = after_sleep - now;
        auto sleep_latency = actual_sleep_duration - sleep_duration;
        
        // Update latency history
        UpdateLatencyHistory(sleep_latency);
        
        // Adjust spin-lock duration based on performance
        AdjustSpinLockDuration();
        
        now = after_sleep;
    }
    
    // Final precision with spin-lock
    while (now < next_time_point) {
        now = std::chrono::high_resolution_clock::now();
    }
    
    last_present_time = now;
}
```

### Performance Optimization

1. **Latency History Management**:
   - Maintains rolling window of recent sleep latencies
   - Uses worst-case latency to determine spin-lock duration
   - Automatically prunes old data to maintain performance

2. **Spin-Lock Duration Calculation**:
   ```cpp
   if (current_size >= MAX_LATENCY_HISTORY_SIZE * 0.10f) {
       auto sorted_latencies = sleep_latency_history;
       std::ranges::sort(sorted_latencies, std::greater<>());
       auto worst_1_percent = sorted_latencies[current_size * 0.01];
       spin_lock_duration = worst_1_percent * 1.5;
   }
   ```

3. **Failure Recovery**:
   ```cpp
   if (spin_lock_failures > target_fps) {
       spin_lock_failures = 0;
       // Reduce spin-lock duration to prevent excessive CPU usage
       spin_lock_duration *= 0.90f;
   }
   ```

## Usage

### Enabling the Custom FPS Limiter

1. **UI Control**: Use the "Custom FPS Limiter" toggle in Developer mode
2. **FPS Setting**: Adjust the "Custom FPS Limit" slider (0-300 FPS)
3. **Automatic Management**: System automatically handles RenoDX limiter coordination

### Configuration

- **Target FPS**: Set via UI slider (0 = no limit)
- **Adaptive Behavior**: Automatically enabled and optimized
- **Performance Monitoring**: Built-in learning and adaptation

## Benefits Over Simple Sleep

### 1. **Accuracy**
- Combines sleep for efficiency with spin-lock for precision
- Adapts to system characteristics automatically
- Maintains consistent frame timing

### 2. **Responsiveness**
- Doesn't block during frame rendering
- Respects actual frame processing time
- Provides smooth, consistent frame pacing

### 3. **Efficiency**
- Minimizes CPU usage through adaptive sleep
- Uses spin-lock only when necessary
- Learns and optimizes over time

### 4. **Reliability**
- Handles system variations gracefully
- Recovers from timing failures
- Maintains performance under different loads

## Technical Considerations

### Thread Safety
- All timing operations are thread-safe
- Uses atomic operations where appropriate
- Designed for single-threaded game rendering

### Performance Impact
- Minimal overhead during normal operation
- Adaptive behavior minimizes CPU usage
- Spin-lock duration is automatically optimized

### Compatibility
- Works with any DirectX 11/12 application
- Integrates seamlessly with ReShade
- Automatically coordinates with RenoDX systems

## Troubleshooting

### Common Issues

1. **Game Unresponsiveness**:
   - Check if custom limiter is enabled
   - Verify FPS limit is reasonable (not too low)
   - Monitor system performance

2. **Inaccurate FPS**:
   - System may need time to learn optimal timing
   - Check for background processes affecting timing
   - Verify target FPS is achievable

3. **Performance Issues**:
   - Spin-lock duration may be too high
   - System will automatically adjust over time
   - Consider reducing target FPS if issues persist

### Debug Information

The system provides detailed logging when enabled:
- Sleep duration calculations
- Latency measurements
- Spin-lock adjustments
- Performance statistics

## Future Enhancements

### Potential Improvements

1. **Multi-Threading Support**: Handle multi-threaded rendering scenarios
2. **Advanced Strategies**: Additional limiting algorithms
3. **Performance Profiling**: Detailed performance analysis tools
4. **Dynamic Adaptation**: Real-time strategy switching based on performance

### Extensibility

The modular design allows for:
- Custom limiting strategies
- Performance monitoring plugins
- Integration with other systems
- Custom UI controls

## Conclusion

The Custom Frame Limiter provides a robust, adaptive solution for FPS limiting that maintains game responsiveness while ensuring accurate frame timing. By combining the efficiency of sleep with the precision of spin-lock, and continuously learning from system performance, it delivers a superior user experience compared to simple sleep-based approaches.

The system's integration with RenoDX and ReShade ensures seamless operation while providing advanced features for power users and developers.
