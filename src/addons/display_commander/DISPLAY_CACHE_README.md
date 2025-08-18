# Display Cache System

## Overview

The Display Cache system provides an efficient, centralized way to store and retrieve display information including resolutions, refresh rates, and display capabilities. It replaces the need for repeated queries to the Windows display APIs and provides rational refresh rate support for precise timing.

## Key Features

- **Efficient Caching**: Store all display information in memory to avoid repeated API calls
- **Rational Refresh Rates**: Support for precise refresh rates using numerator/denominator pairs
- **Modern API Support**: Uses DXGI for high-precision refresh rates with fallback to legacy APIs
- **Display Capabilities**: Tracks HDR and VRR support
- **Thread-Safe**: Designed for use in UI and background threads

## Data Structures

### RationalRefreshRate
```cpp
struct RationalRefreshRate {
    UINT32 numerator;
    UINT32 denominator;
    
    double ToHz() const;           // Convert to double (e.g., 143.999Hz)
    std::string ToString() const;  // Convert to string (e.g., "143.999Hz")
};
```

### Resolution
```cpp
struct Resolution {
    int width;
    int height;
    std::vector<RationalRefreshRate> refresh_rates;
    
    std::string ToString() const;  // Convert to string (e.g., "1920 x 1080")
};
```

### DisplayInfo
```cpp
struct DisplayInfo {
    HMONITOR monitor_handle;
    std::wstring device_name;
    std::wstring friendly_name;
    std::vector<Resolution> resolutions;
    
    // Current settings
    int current_width;
    int current_height;
    RationalRefreshRate current_refresh_rate;
    
    // Display capabilities
    bool supports_hdr;
    bool supports_vrr;
    
    // Helper methods
    std::string GetCurrentResolutionString() const;
    std::string GetCurrentRefreshRateString() const;
    std::optional<size_t> FindResolutionIndex(int width, int height) const;
    std::vector<std::string> GetResolutionLabels() const;
    std::vector<std::string> GetRefreshRateLabels(size_t resolution_index) const;
};
```

## Usage Examples

### 1. Initialize the Cache
```cpp
#include "display_cache.hpp"

// Initialize the global cache
if (!renodx::display_cache::g_displayCache.IsInitialized()) {
    renodx::display_cache::g_displayCache.Initialize();
}
```

### 2. Get Display Information
```cpp
// Get number of displays
size_t display_count = renodx::display_cache::g_displayCache.GetDisplayCount();

// Get specific display
const auto* display = renodx::display_cache::g_displayCache.GetDisplay(0);
if (display) {
    std::wcout << L"Display: " << display->friendly_name << std::endl;
    std::cout << "Current: " << display->GetCurrentResolutionString() 
              << " @ " << display->GetCurrentRefreshRateString() << std::endl;
}
```

### 3. Get Resolution Labels (for UI)
```cpp
// Get all available resolutions for a display
std::vector<std::string> resolutions = 
    renodx::display_cache::g_displayCache.GetResolutionLabels(0);

// Example output: ["1920 x 1080", "2560 x 1440", "3840 x 2160"]
```

### 4. Get Refresh Rate Labels (for UI)
```cpp
// Get refresh rates for a specific resolution
std::vector<std::string> refresh_rates = 
    renodx::display_cache::g_displayCache.GetRefreshRateLabels(0, 0); // display 0, resolution 0

// Example output: ["60Hz", "120Hz", "143.999Hz", "144Hz"]
```

### 5. Get Rational Refresh Rate
```cpp
// Get precise refresh rate values
renodx::display_cache::RationalRefreshRate rate;
if (renodx::display_cache::g_displayCache.GetRationalRefreshRate(0, 0, 2, rate)) {
    // rate.numerator = 143999, rate.denominator = 1000
    double hz = rate.ToHz();  // 143.999
    std::string str = rate.ToString();  // "143.999Hz"
}
```

### 6. Find Display by Handle
```cpp
// Find display index from monitor handle
HMONITOR monitor = /* ... */;
const auto* display = renodx::display_cache::g_displayCache.GetDisplayByHandle(monitor);
if (display) {
    // Use display information
}
```

## Migration from Old System

### Old Way (repeated API calls)
```cpp
// Old: GetResolutionLabels
std::vector<std::string> resolutions = 
    renodx::resolution::GetResolutionLabels(monitor_index);

// Old: GetRefreshRateLabels  
std::vector<std::string> refresh_rates = 
    renodx::resolution::GetRefreshRateLabels(monitor_index, width, height);

// Old: GetSelectedRefreshRate
float refresh_rate;
renodx::resolution::GetSelectedRefreshRate(monitor_index, width, height, 
                                          refresh_rate_index, refresh_rate);
```

### New Way (using cache)
```cpp
// New: GetResolutionLabels
std::vector<std::string> resolutions = 
    renodx::display_cache::g_displayCache.GetResolutionLabels(monitor_index);

// New: GetRefreshRateLabels
const auto* display = renodx::display_cache::g_displayCache.GetDisplay(monitor_index);
auto resolution_index = display->FindResolutionIndex(width, height);
std::vector<std::string> refresh_rates = 
    renodx::display_cache::g_displayCache.GetRefreshRateLabels(monitor_index, *resolution_index);

// New: Get rational refresh rate
renodx::display_cache::RationalRefreshRate rate;
renodx::display_cache::g_displayCache.GetRationalRefreshRate(monitor_index, 
                                                            *resolution_index, 
                                                            refresh_rate_index, 
                                                            rate);
float refresh_rate = static_cast<float>(rate.ToHz());
```

## Benefits

1. **Performance**: No repeated API calls, all data cached in memory
2. **Precision**: Rational refresh rates maintain exact timing (143.999Hz vs 143Hz)
3. **Consistency**: All display queries use the same data source
4. **Extensibility**: Easy to add new display properties (HDR, VRR, etc.)
5. **Maintainability**: Centralized display logic instead of scattered API calls

## Cache Management

### Refresh Cache
```cpp
// Force refresh of all display information
renodx::display_cache::g_displayCache.Refresh();
```

### Clear Cache
```cpp
// Clear all cached data
renodx::display_cache::g_displayCache.Clear();
```

### Check Status
```cpp
// Check if cache is initialized
if (renodx::display_cache::g_displayCache.IsInitialized()) {
    // Cache is ready to use
}
```

## Thread Safety

The display cache is designed to be thread-safe for read operations. Multiple threads can safely call getter methods simultaneously. However, initialization and refresh operations should be synchronized if called from multiple threads.

## Error Handling

All methods return safe defaults or empty results when errors occur:
- Invalid display indices return `nullptr` or empty vectors
- Failed API calls fall back to legacy methods
- Missing refresh rates return empty lists

## Future Enhancements

- **Dynamic Updates**: Monitor for display changes and update cache automatically
- **Advanced Capabilities**: More detailed HDR, VRR, and color space information
- **Performance Metrics**: Track cache hit rates and optimization opportunities
- **Configuration Persistence**: Save/restore display preferences
