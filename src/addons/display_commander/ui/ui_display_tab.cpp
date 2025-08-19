#include "ui_display_tab.hpp"
#include "ui_common.hpp"
#include "monitor_settings/monitor_settings.hpp"
#include "../../../utils/settings.hpp"
#include "../addon.hpp"
#include "../resolution_helpers.hpp"
#include "../display_cache.hpp"
#include <windows.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map> // Added for resolution_map
#include <iomanip> // Added for std::fixed and std::setprecision
#include <cmath> // Added for std::round
#include <atomic>

// External variables
extern float s_selected_monitor_index;
extern float s_selected_resolution_index;
extern float s_selected_refresh_rate_index;
extern bool s_initial_auto_selection_done;

namespace renodx::ui {

// Initialize the display cache for the UI
void InitializeDisplayCache() {
    if (!renodx::display_cache::g_displayCache.IsInitialized()) {
        renodx::display_cache::g_displayCache.Initialize();
    }
}

// Helper function to get monitor information using the display cache
std::vector<std::string> GetMonitorLabelsFromCache() {
    std::vector<std::string> labels;
    
    if (!renodx::display_cache::g_displayCache.IsInitialized()) {
        renodx::display_cache::g_displayCache.Initialize();
    }
    
    size_t display_count = renodx::display_cache::g_displayCache.GetDisplayCount();
    labels.reserve(display_count);
    
        for (size_t i = 0; i < display_count; ++i) {
            const auto* display = renodx::display_cache::g_displayCache.GetDisplay(i);
            if (display) {
                std::ostringstream oss;
                
                // Convert friendly name to string for user-friendly display
                std::string friendly_name(display->friendly_name.begin(), display->friendly_name.end());
                
                // Get high-precision refresh rate with full precision
                double exact_refresh_rate = display->current_refresh_rate.ToHz();
                std::ostringstream rate_oss;
                rate_oss << std::setprecision(10) << exact_refresh_rate;
                std::string rate_str = rate_oss.str();
                
                // Remove trailing zeros after decimal point but keep meaningful precision
                size_t decimal_pos = rate_str.find('.');
                if (decimal_pos != std::string::npos) {
                    size_t last_nonzero = rate_str.find_last_not_of('0');
                    if (last_nonzero == decimal_pos) {
                        // All zeros after decimal, remove decimal point too
                        rate_str = rate_str.substr(0, decimal_pos);
                    } else if (last_nonzero > decimal_pos) {
                        // Remove trailing zeros but keep some precision
                        rate_str = rate_str.substr(0, last_nonzero + 1);
                    }
                }
                
                // Format: [DeviceID] Friendly Name - Resolution @ PreciseRefreshRateHz [Raw: num/den]
                std::string device_name(display->device_name.begin(), display->device_name.end());
                oss << "[" << device_name << "] " << friendly_name << " - " << display->GetCurrentResolutionString() 
                    << " @ " << rate_str << "Hz [Raw: " 
                    << display->current_refresh_rate.numerator << "/" 
                    << display->current_refresh_rate.denominator << "]";
                labels.push_back(oss.str());
            }
        }
    
    return labels;
}

// Helper function to get max monitor index using the display cache
float GetMaxMonitorIndexFromCache() {
    if (!renodx::display_cache::g_displayCache.IsInitialized()) {
        renodx::display_cache::g_displayCache.Initialize();
    }
    
    size_t display_count = renodx::display_cache::g_displayCache.GetDisplayCount();
    return static_cast<float>((std::max)(0, static_cast<int>(display_count) - 1));
}

// Helper function to get current display info based on game position using the display cache
std::string GetCurrentDisplayInfoFromCache() {
    HWND hwnd = g_last_swapchain_hwnd.load();
    
    if (!hwnd) {
        return "No game window detected";
    }
    
    // Get window position
    RECT window_rect;
    if (!GetWindowRect(hwnd, &window_rect)) {
        return "Failed to get window position";
    }
    
    // Find which monitor the game is running on
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        return "Failed to determine monitor";
    }
    
    // Use the display cache to get monitor information
    if (!renodx::display_cache::g_displayCache.IsInitialized()) {
        renodx::display_cache::g_displayCache.Initialize();
    }
    
    const auto* display = renodx::display_cache::g_displayCache.GetDisplayByHandle(monitor);
    if (!display) {
        return "Failed to get display info from cache";
    }
    
    // Use the new comprehensive display info method that shows current vs supported modes
    std::string friendly_name(display->friendly_name.begin(), display->friendly_name.end());
    std::ostringstream oss;
    oss << friendly_name << " - " << display->GetCurrentDisplayInfoString();
    return oss.str();
}

// Legacy functions for backward compatibility (can be removed later)
std::vector<std::string> GetMonitorLabels() {
    return GetMonitorLabelsFromCache();
}

float GetMaxMonitorIndex() {
    return GetMaxMonitorIndexFromCache();
}

std::string GetCurrentDisplayInfo() {
    return GetCurrentDisplayInfoFromCache();
}

// Handle monitor settings UI (extracted from on_draw lambda)
bool HandleMonitorSettingsUI() {
    // Handle display cache refresh logic (every 60 frames)
    renodx::ui::monitor_settings::HandleDisplayCacheRefresh();
    
    // Get current monitor labels (now with precise refresh rates and raw rational values)
    auto monitor_labels = GetMonitorLabelsFromCache();
            if (monitor_labels.empty()) {
                ImGui::Text("No monitors detected");
                return false;
            }
            
        // Handle auto-detection of current display settings
    renodx::ui::monitor_settings::HandleAutoDetection();
    
        // Handle monitor selection UI
    renodx::ui::monitor_settings::HandleMonitorSelection(monitor_labels);
    
    // Handle resolution selection UI
    renodx::ui::monitor_settings::HandleResolutionSelection(static_cast<int>(s_selected_monitor_index));
    
    // Handle refresh rate selection UI
    renodx::ui::monitor_settings::HandleRefreshRateSelection(static_cast<int>(s_selected_monitor_index), static_cast<int>(s_selected_resolution_index));
    

    
    // Apply Changes Button
    ImGui::Text("Apply Display Changes:");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Legacy API: Rounds refresh rates to integers (e.g., 143.999Hz → 144Hz)\nModern API: Preserves exact fractional refresh rates (e.g., 143.999Hz → 143.999Hz)");
    }
    
    // Handle the Legacy Apply Button

    
    ImGui::Spacing();
    
    // Display API Status
    renodx::ui::monitor_settings::DisplayAPIStatus();
    

    
    // Handle the DXGI API Apply Button
    renodx::ui::monitor_settings::HandleDXGIAPIApplyButton();
    
    return false; // No value change
}

void AddDisplayTabSettings(std::vector<renodx::utils::settings::Setting*>& settings) {
    // Initialize the display cache when the display tab is first accessed
    static bool cache_initialized = false;
    if (!cache_initialized) {
        InitializeDisplayCache();
        cache_initialized = true;
    }
    
    // Desktop Resolution Override Checkbox
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "OverrideDesktopResolution",
        .binding = &s_override_desktop_resolution,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f, // Disabled by default
        .label = "Override Desktop Resolution",
        .section = "Display Tab",
        .tooltip = "Enable to apply the selected resolution and refresh rate to the desktop. When disabled, you can still browse available options.",
        .is_visible = []() { return is_display_tab(s_ui_mode); } // Show in Display tab mode
    });

    settings.push_back(new renodx::utils::settings::Setting{
        .key = "MonitorSettingsCustom",
        .binding = nullptr, // No direct binding, just for display
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "Dynamic Monitor Settings",
        .section = "Display",
        .tooltip = "Interactive monitor, resolution, and refresh rate selection with real-time updates.",
        .on_draw = []() -> bool {
            // Call the new function for the main logic
            bool result = HandleMonitorSettingsUI();
            
            // Keep any additional logic that was in the original lambda
            // ...
            
            return result;
        },
        .is_visible = []() { return is_basic_tab(s_ui_mode) || is_display_tab(s_ui_mode); } // Show in Simple and Display modes
    });

    // Current Display Info
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "CurrentDisplayInfo",
        .binding = nullptr, // No direct binding, just for display
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "Current Display Info",
        .section = "Display",
        .tooltip = "Shows the current resolution and refresh rate of the display where the game is running, with closest supported mode if different.",
        .on_draw = []() -> bool {
            std::string display_info = GetCurrentDisplayInfoFromCache();
            ImGui::TextUnformatted(display_info.c_str());
            return false; // No value change
        },
        .is_visible = []() { return is_basic_tab(s_ui_mode) || is_display_tab(s_ui_mode); } // Show in Simple and Display modes
    });

}

} // namespace renodx::ui
