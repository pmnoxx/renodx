#include "display_cache.hpp"
#include <iostream>

// Example of how to use the display cache to replace existing queries
namespace renodx::display_cache_example {

// Example 1: Replace GetResolutionLabels
std::vector<std::string> GetResolutionLabelsExample(int monitor_index) {
    // Old way: renodx::resolution::GetResolutionLabels(monitor_index)
    // New way: Use the cache
    if (!renodx::display_cache::g_displayCache.IsInitialized()) {
        renodx::display_cache::g_displayCache.Initialize();
    }
    
    return renodx::display_cache::g_displayCache.GetResolutionLabels(static_cast<size_t>(monitor_index));
}

// Example 2: Replace GetRefreshRateLabels
std::vector<std::string> GetRefreshRateLabelsExample(int monitor_index, int width, int height) {
    // Old way: renodx::resolution::GetRefreshRateLabels(monitor_index, width, height)
    // New way: Use the cache
    
    if (!renodx::display_cache::g_displayCache.IsInitialized()) {
        renodx::display_cache::g_displayCache.Initialize();
    }
    
    const auto* display = renodx::display_cache::g_displayCache.GetDisplay(static_cast<size_t>(monitor_index));
    if (!display) return {};
    
    // Find the resolution index
    auto resolution_index = display->FindResolutionIndex(width, height);
    if (!resolution_index) return {};
    
    return renodx::display_cache::g_displayCache.GetRefreshRateLabels(static_cast<size_t>(monitor_index), *resolution_index);
}

// Example 3: Replace GetSelectedResolution
bool GetSelectedResolutionExample(int monitor_index, int resolution_index, int& out_width, int& out_height) {
    // Old way: renodx::resolution::GetSelectedResolution(monitor_index, resolution_index, out_width, out_height)
    // New way: Use the cache
    
    if (!renodx::display_cache::g_displayCache.IsInitialized()) {
        renodx::display_cache::g_displayCache.Initialize();
    }
    
    const auto* display = renodx::display_cache::g_displayCache.GetDisplay(static_cast<size_t>(monitor_index));
    if (!display) return false;
    
    if (resolution_index < 0 || static_cast<size_t>(resolution_index) >= display->resolutions.size()) {
        return false;
    }
    
    const auto& res = display->resolutions[static_cast<size_t>(resolution_index)];
    out_width = res.width;
    out_height = res.height;
    return true;
}

// Example 4: Replace GetSelectedRefreshRate
bool GetSelectedRefreshRateExample(int monitor_index, int width, int height, int refresh_rate_index, float& out_refresh_rate) {
    // Old way: renodx::resolution::GetSelectedRefreshRate(monitor_index, width, height, refresh_rate_index, out_refresh_rate)
    // New way: Use the cache
    
    if (!renodx::display_cache::g_displayCache.IsInitialized()) {
        renodx::display_cache::g_displayCache.Initialize();
    }
    
    const auto* display = renodx::display_cache::g_displayCache.GetDisplay(static_cast<size_t>(monitor_index));
    if (!display) return false;
    
    // Find the resolution index
    auto resolution_index = display->FindResolutionIndex(width, height);
    if (!resolution_index) return false;
    
    if (refresh_rate_index < 0 || static_cast<size_t>(refresh_rate_index) >= display->resolutions[*resolution_index].refresh_rates.size()) {
        return false;
    }
    
    const auto& rate = display->resolutions[*resolution_index].refresh_rates[static_cast<size_t>(refresh_rate_index)];
    out_refresh_rate = static_cast<float>(rate.ToHz());
    return true;
}

// Example 5: Replace GetSelectedRefreshRateRational
bool GetSelectedRefreshRateRationalExample(int monitor_index, int width, int height, int refresh_rate_index, 
                                          UINT32& out_numerator, UINT32& out_denominator) {
    // Old way: renodx::resolution::GetSelectedRefreshRateRational(monitor_index, width, height, refresh_rate_index, out_numerator, out_denominator)
    // New way: Use the cache
    
    if (!renodx::display_cache::g_displayCache.IsInitialized()) {
        renodx::display_cache::g_displayCache.Initialize();
    }
    
    const auto* display = renodx::display_cache::g_displayCache.GetDisplay(static_cast<size_t>(monitor_index));
    if (!display) return false;
    
    // Find the resolution index
    auto resolution_index = display->FindResolutionIndex(width, height);
    if (!resolution_index) return false;
    
    renodx::display_cache::RationalRefreshRate refresh_rate;
    if (!renodx::display_cache::g_displayCache.GetRationalRefreshRate(static_cast<size_t>(monitor_index), 
                                              *resolution_index,
                                              static_cast<size_t>(refresh_rate_index), 
                                              refresh_rate)) {
        return false;
    }
    
    out_numerator = refresh_rate.numerator;
    out_denominator = refresh_rate.denominator;
    return true;
}

// Example 6: Get current display information
void PrintCurrentDisplayInfo(int monitor_index) {
    if (!renodx::display_cache::g_displayCache.IsInitialized()) {
        renodx::display_cache::g_displayCache.Initialize();
    }
    
    const auto* display = renodx::display_cache::g_displayCache.GetDisplay(static_cast<size_t>(monitor_index));
    if (!display) {
        std::cout << "Display not found" << std::endl;
        return;
    }
    
    std::wcout << L"Display: " << display->friendly_name << std::endl;
    std::cout << "Current Resolution: " << display->GetCurrentResolutionString() << std::endl;
    std::cout << "Current Refresh Rate: " << display->GetCurrentRefreshRateString() << std::endl;
    std::cout << "Supports HDR: " << (display->supports_hdr ? "Yes" : "No") << std::endl;
    std::cout << "Supports VRR: " << (display->supports_vrr ? "Yes" : "No") << std::endl;
    std::cout << "Available Resolutions: " << display->resolutions.size() << std::endl;
    
    for (size_t i = 0; i < display->resolutions.size(); ++i) {
        const auto& res = display->resolutions[i];
        std::cout << "  " << res.ToString() << " - " << res.refresh_rates.size() << " refresh rates" << std::endl;
        
        for (size_t j = 0; j < res.refresh_rates.size(); ++j) {
            const auto& rate = res.refresh_rates[j];
            std::cout << "    " << rate.ToString() << " (" << rate.numerator << "/" << rate.denominator << ")" << std::endl;
        }
    }
}

// Example 7: Monitor selection helper
std::vector<std::string> GetMonitorLabelsExample() {
    if (!renodx::display_cache::g_displayCache.IsInitialized()) {
        renodx::display_cache::g_displayCache.Initialize();
    }
    
    std::vector<std::string> labels;
    size_t count = renodx::display_cache::g_displayCache.GetDisplayCount();
    
    for (size_t i = 0; i < count; ++i) {
        const auto* display = renodx::display_cache::g_displayCache.GetDisplay(i);
        if (display) {
            // Convert wide string to narrow string for UI
            std::string name(display->friendly_name.begin(), display->friendly_name.end());
            labels.push_back(name);
        }
    }
    
    return labels;
}

// Example 8: Find monitor index by handle
int GetMonitorIndexByHandleExample(HMONITOR monitor) {
    if (!renodx::display_cache::g_displayCache.IsInitialized()) {
        renodx::display_cache::g_displayCache.Initialize();
    }
    
    size_t count = renodx::display_cache::g_displayCache.GetDisplayCount();
    for (size_t i = 0; i < count; ++i) {
        const auto* display = renodx::display_cache::g_displayCache.GetDisplay(i);
        if (display && display->monitor_handle == monitor) {
            return static_cast<int>(i);
        }
    }
    
    return -1;
}

} // namespace renodx::display_cache_example
