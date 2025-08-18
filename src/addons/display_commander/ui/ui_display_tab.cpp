#include "ui_display_tab.hpp"
#include "ui_common.hpp"
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
#include <atomic>

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
            // Convert wide string to narrow string for UI
            std::string friendly_name(display->friendly_name.begin(), display->friendly_name.end());
            oss << friendly_name << " (" << display->GetCurrentResolutionString() 
                << " @ " << display->GetCurrentRefreshRateString() << ")";
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
    
    // Format the display info using cache data with high-precision refresh rate
    std::ostringstream oss;
    std::string friendly_name(display->friendly_name.begin(), display->friendly_name.end());
    oss << friendly_name << " - " << display->GetCurrentResolutionString() 
        << " @ ";
    
    // Format refresh rate with high precision (up to 10 digits, no trailing zeros)
    oss << std::setprecision(10) << display->current_refresh_rate.ToHz();
    std::string rate_str = oss.str();
    
    // Remove trailing zeros after decimal point
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
    
    return rate_str + "Hz";
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
            // Reset display cache every 60 frames to keep it fresh
            static int frame_counter = 0;
            frame_counter++;
            if (frame_counter >= 60) {
                renodx::display_cache::g_displayCache.Refresh();
                frame_counter = 0;
            }
            
            // Get current monitor labels
            auto monitor_labels = GetMonitorLabelsFromCache();
            if (monitor_labels.empty()) {
                ImGui::Text("No monitors detected");
                return false;
            }
            
            // Auto-detect current display resolution and set as default if not already set
            static bool first_run = true;
            if (first_run) {
                first_run = false;
                
                // Get current display info for the selected monitor
                HWND hwnd = g_last_swapchain_hwnd.load();
                if (hwnd) {
                    HMONITOR current_monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                    if (current_monitor) {
                                                 // Find which monitor index this corresponds to
                         for (int i = 0; i < static_cast<int>(monitor_labels.size()); i++) {
                             const auto* display = renodx::display_cache::g_displayCache.GetDisplay(i);
                             if (display && display->monitor_handle == current_monitor) {
                                 s_selected_monitor_index = static_cast<float>(i);
                                 
                                 // Get current resolution and find its index
                                 std::string current_resolution = display->GetCurrentResolutionString();
                                 auto resolution_labels = renodx::display_cache::g_displayCache.GetResolutionLabels(i);
                                 
                                 for (int j = 0; j < static_cast<int>(resolution_labels.size()); j++) {
                                     if (resolution_labels[j] == current_resolution) {
                                         s_selected_resolution_index = static_cast<float>(j);
                                         break;
                                     }
                                 }
                                 
                                 // Get current refresh rate and find its index
                                 std::string current_refresh_rate = display->GetCurrentRefreshRateString();
                                 auto refresh_rate_labels = renodx::display_cache::g_displayCache.GetRefreshRateLabels(i, s_selected_resolution_index);
                                 
                                 for (int j = 0; j < static_cast<int>(refresh_rate_labels.size()); j++) {
                                     if (refresh_rate_labels[j] == current_refresh_rate) {
                                         s_selected_refresh_rate_index = static_cast<float>(j);
                                         break;
                                     }
                                 }
                                 break;
                             }
                         }
                    }
                }
            }
            
            // Monitor selection
            if (ImGui::BeginCombo("Monitor", monitor_labels[static_cast<int>(s_selected_monitor_index)].c_str())) {
                for (int i = 0; i < static_cast<int>(monitor_labels.size()); i++) {
                    const bool is_selected = (i == static_cast<int>(s_selected_monitor_index));
                    if (ImGui::Selectable(monitor_labels[i].c_str(), is_selected)) {
                        s_selected_monitor_index = static_cast<float>(i);
                        s_selected_resolution_index = 0.f; // Reset resolution when monitor changes
                        s_selected_refresh_rate_index = 0.f; // Reset refresh rate when monitor changes
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            
            // Resolution selection
            auto resolution_labels = renodx::display_cache::g_displayCache.GetResolutionLabels(static_cast<int>(s_selected_monitor_index));
            if (!resolution_labels.empty()) {
                if (ImGui::BeginCombo("Resolution", resolution_labels[static_cast<int>(s_selected_resolution_index)].c_str())) {
                    for (int i = 0; i < static_cast<int>(resolution_labels.size()); i++) {
                        const bool is_selected = (i == static_cast<int>(s_selected_resolution_index));
                        if (ImGui::Selectable(resolution_labels[i].c_str(), is_selected)) {
                            s_selected_resolution_index = static_cast<float>(i);
                            s_selected_refresh_rate_index = 0.f; // Reset refresh rate when resolution changes
                        }
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }
            
            // Refresh rate selection
            if (s_selected_resolution_index >= 0 && s_selected_resolution_index < static_cast<int>(resolution_labels.size())) {
                auto refresh_rate_labels = renodx::display_cache::g_displayCache.GetRefreshRateLabels(static_cast<int>(s_selected_monitor_index), static_cast<int>(s_selected_resolution_index));
                if (!refresh_rate_labels.empty()) {
                    if (ImGui::BeginCombo("Refresh Rate", refresh_rate_labels[static_cast<int>(s_selected_refresh_rate_index)].c_str())) {
                        for (int i = 0; i < static_cast<int>(refresh_rate_labels.size()); i++) {
                            const bool is_selected = (i == static_cast<int>(s_selected_refresh_rate_index));
                            if (ImGui::Selectable(refresh_rate_labels[i].c_str(), is_selected)) {
                                s_selected_refresh_rate_index = static_cast<float>(i);
                            }
                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                }
            }
            
            // Apply Changes Button
            if (ImGui::Button("Apply Desktop Changes")) {
                // Apply the changes using the display cache
                std::thread([](){
                    // Get the selected resolution from the cache
                    auto resolution_labels = renodx::display_cache::g_displayCache.GetResolutionLabels(static_cast<int>(s_selected_monitor_index));
                    if (s_selected_resolution_index >= 0 && s_selected_resolution_index < static_cast<int>(resolution_labels.size())) {
                        std::string selected_resolution = resolution_labels[static_cast<int>(s_selected_resolution_index)];
                        int width, height;
                        sscanf(selected_resolution.c_str(), "%d x %d", &width, &height);
                        
                        // Get the selected refresh rate from the cache
                        auto refresh_rate_labels = renodx::display_cache::g_displayCache.GetRefreshRateLabels(static_cast<int>(s_selected_monitor_index), static_cast<int>(s_selected_resolution_index));
                        if (s_selected_refresh_rate_index >= 0 && s_selected_refresh_rate_index < static_cast<int>(refresh_rate_labels.size())) {
                            std::string selected_refresh_rate = refresh_rate_labels[static_cast<int>(s_selected_refresh_rate_index)];
                            
                            // Try to get rational refresh rate values from the cache
                            renodx::display_cache::RationalRefreshRate refresh_rate;
                            bool has_rational = renodx::display_cache::g_displayCache.GetRationalRefreshRate(
                                static_cast<int>(s_selected_monitor_index), 
                                static_cast<int>(s_selected_resolution_index),
                                static_cast<int>(s_selected_refresh_rate_index), 
                                refresh_rate);
                            
                            if (has_rational) {
                                // Use modern API with rational refresh rates
                                if (renodx::resolution::ApplyDisplaySettingsModern(
                                    static_cast<int>(s_selected_monitor_index), width, height, 
                                    refresh_rate.numerator, refresh_rate.denominator)) {
                                    
                                    double refresh_rate_hz = refresh_rate.ToHz();
                                    std::ostringstream oss;
                                    oss << "Display changes applied successfully using modern API: " << width << "x" << height 
                                        << " @ " << std::fixed << std::setprecision(3) << refresh_rate_hz << "Hz";
                                    LogInfo(oss.str().c_str());
                                } else {
                                    // Fallback to legacy API
                                    float refresh_rate;
                                    sscanf(selected_refresh_rate.c_str(), "%fHz", &refresh_rate);
                                    
                                    // Get monitor handle
                                    std::vector<HMONITOR> monitors;
                                    EnumDisplayMonitors(nullptr, nullptr, 
                                        [](HMONITOR hmon, HDC, LPRECT, LPARAM lparam) -> BOOL {
                                            auto* monitors_ptr = reinterpret_cast<std::vector<HMONITOR>*>(lparam);
                                            monitors_ptr->push_back(hmon);
                                            return TRUE;
                                        }, 
                                        reinterpret_cast<LPARAM>(&monitors));
                                    
                                    if (s_selected_monitor_index >= 0 && s_selected_monitor_index < static_cast<int>(monitors.size())) {
                                        HMONITOR hmon = monitors[static_cast<int>(s_selected_monitor_index)];
                                        
                                        MONITORINFOEXW mi;
                                        mi.cbSize = sizeof(mi);
                                        if (GetMonitorInfoW(hmon, &mi)) {
                                            std::wstring device_name = mi.szDevice;
                                            
                                            // Create DEVMODE structure with selected resolution and refresh rate
                                            DEVMODEW dm;
                                            dm.dmSize = sizeof(dm);
                                            dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
                                            dm.dmPelsWidth = width;
                                            dm.dmPelsHeight = height;
                                            dm.dmDisplayFrequency = static_cast<DWORD>(refresh_rate);
                                            
                                            // Apply the changes
                                            LONG result = ChangeDisplaySettingsExW(device_name.c_str(), &dm, nullptr, CDS_UPDATEREGISTRY, nullptr);
                                            
                                            if (result == DISP_CHANGE_SUCCESSFUL) {
                                                std::ostringstream oss;
                                                oss << "Display changes applied successfully using legacy API: " << width << "x" << height 
                                                    << " @ " << std::fixed << std::setprecision(2) << refresh_rate << "Hz";
                                                LogInfo(oss.str().c_str());
                                            } else {
                                                std::ostringstream oss;
                                                oss << "Failed to apply display changes. Error code: " << result;
                                                LogWarn(oss.str().c_str());
                                            }
                                        }
                                    }
                                }
                            } else {
                                // No rational refresh rate available, use legacy API
                                float refresh_rate;
                                sscanf(selected_refresh_rate.c_str(), "%fHz", &refresh_rate);
                                
                                // Get monitor handle
                                std::vector<HMONITOR> monitors;
                                EnumDisplayMonitors(nullptr, nullptr, 
                                    [](HMONITOR hmon, HDC, LPRECT, LPARAM lparam) -> BOOL {
                                        auto* monitors_ptr = reinterpret_cast<std::vector<HMONITOR>*>(lparam);
                                        monitors_ptr->push_back(hmon);
                                        return TRUE;
                                    }, 
                                    reinterpret_cast<LPARAM>(&monitors));
                                
                                if (s_selected_monitor_index >= 0 && s_selected_monitor_index < static_cast<int>(monitors.size())) {
                                    HMONITOR hmon = monitors[static_cast<int>(s_selected_monitor_index)];
                                    
                                    MONITORINFOEXW mi;
                                    mi.cbSize = sizeof(mi);
                                    if (GetMonitorInfoW(hmon, &mi)) {
                                        std::wstring device_name = mi.szDevice;
                                        
                                        // Create DEVMODE structure with selected resolution and refresh rate
                                        DEVMODEW dm;
                                        dm.dmSize = sizeof(dm);
                                        dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
                                        dm.dmPelsWidth = width;
                                        dm.dmPelsHeight = height;
                                        dm.dmDisplayFrequency = static_cast<DWORD>(refresh_rate);
                                        
                                        // Apply the changes
                                        LONG result = ChangeDisplaySettingsExW(device_name.c_str(), &dm, nullptr, CDS_UPDATEREGISTRY, nullptr);
                                        
                                        if (result == DISP_CHANGE_SUCCESSFUL) {
                                            std::ostringstream oss;
                                            oss << "Display changes applied successfully using legacy API: " << width << "x" << height 
                                                << " @ " << std::fixed << std::setprecision(2) << refresh_rate << "Hz";
                                            LogInfo(oss.str().c_str());
                                        } else {
                                            std::ostringstream oss;
                                            oss << "Failed to apply display changes. Error code: " << result;
                                            LogWarn(oss.str().c_str());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }).detach();
            }
            
            return false; // No value change
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
        .tooltip = "Shows the current resolution and refresh rate of the display where the game is running.",
        .on_draw = []() -> bool {
            std::string display_info = GetCurrentDisplayInfoFromCache();
            ImGui::TextUnformatted(display_info.c_str());
            return false; // No value change
        },
        .is_visible = []() { return is_basic_tab(s_ui_mode) || is_display_tab(s_ui_mode); } // Show in Simple and Display modes
    });

}

} // namespace renodx::ui
