#include "ui_display_tab.hpp"
#include "ui_common.hpp"
#include "../../../utils/settings.hpp"
#include "../addon.hpp"
#include <windows.h>
#include <vector>
#include <sstream>
#include <algorithm>

namespace renodx::ui {

// Helper function to get monitor information
std::vector<std::string> GetMonitorLabels() {
    std::vector<std::string> labels;
    
    // Enumerate monitors
    EnumDisplayMonitors(nullptr, nullptr, 
        [](HMONITOR hmon, HDC, LPRECT, LPARAM lparam) -> BOOL {
            auto* labels_ptr = reinterpret_cast<std::vector<std::string>*>(lparam);
            
            MONITORINFOEXW mi;
            mi.cbSize = sizeof(mi);
            if (GetMonitorInfoW(hmon, &mi)) {
                std::wstring device_name = mi.szDevice;
                std::string device_name_str(device_name.begin(), device_name.end());
                
                // Get current display settings for this monitor
                DEVMODEW dm;
                dm.dmSize = sizeof(dm);
                if (EnumDisplaySettingsW(device_name.c_str(), ENUM_CURRENT_SETTINGS, &dm)) {
                    std::ostringstream oss;
                    oss << device_name_str << " (" << dm.dmPelsWidth << "x" << dm.dmPelsHeight 
                        << " @ " << dm.dmDisplayFrequency << "Hz)";
                    labels_ptr->push_back(oss.str());
                } else {
                    labels_ptr->push_back(device_name_str);
                }
            }
            return TRUE;
        }, 
        reinterpret_cast<LPARAM>(&labels));
    
    return labels;
}

// Helper function to get available resolutions for a monitor
std::vector<std::string> GetResolutionLabels(int monitor_index) {
    std::vector<std::string> labels;
    
    // Get monitor handle
    std::vector<HMONITOR> monitors;
    EnumDisplayMonitors(nullptr, nullptr, 
        [](HMONITOR hmon, HDC, LPRECT, LPARAM lparam) -> BOOL {
            auto* monitors_ptr = reinterpret_cast<std::vector<HMONITOR>*>(lparam);
            monitors_ptr->push_back(hmon);
            return TRUE;
        }, 
        reinterpret_cast<LPARAM>(&monitors));
    
    if (monitor_index >= 0 && monitor_index < static_cast<int>(monitors.size())) {
        HMONITOR hmon = monitors[monitor_index];
        
        MONITORINFOEXW mi;
        mi.cbSize = sizeof(mi);
        if (GetMonitorInfoW(hmon, &mi)) {
            std::wstring device_name = mi.szDevice;
            
            // Enumerate all display modes
            DEVMODEW dm;
            dm.dmSize = sizeof(dm);
            
            for (int i = 0; EnumDisplaySettingsW(device_name.c_str(), i, &dm); i++) {
                // Only add unique resolutions
                std::ostringstream oss;
                oss << dm.dmPelsWidth << " x " << dm.dmPelsHeight;
                std::string resolution = oss.str();
                
                // Check if this resolution is already in the list
                bool found = false;
                for (const auto& existing : labels) {
                    if (existing == resolution) {
                        found = true;
                        break;
                    }
                }
                
                if (!found) {
                    labels.push_back(resolution);
                }
            }
        }
    }
    
    return labels;
}

// Helper function to get available refresh rates for a monitor and resolution
std::vector<std::string> GetRefreshRateLabels(int monitor_index, int width, int height) {
    std::vector<std::string> labels;
    
    // Get monitor handle
    std::vector<HMONITOR> monitors;
    EnumDisplayMonitors(nullptr, nullptr, 
        [](HMONITOR hmon, HDC, LPRECT, LPARAM lparam) -> BOOL {
            auto* monitors_ptr = reinterpret_cast<std::vector<HMONITOR>*>(lparam);
            monitors_ptr->push_back(hmon);
            return TRUE;
        }, 
        reinterpret_cast<LPARAM>(&monitors));
    
    if (monitor_index >= 0 && monitor_index < static_cast<int>(monitors.size())) {
        HMONITOR hmon = monitors[monitor_index];
        
        MONITORINFOEXW mi;
        mi.cbSize = sizeof(mi);
        if (GetMonitorInfoW(hmon, &mi)) {
            std::wstring device_name = mi.szDevice;
            
            // Enumerate all display modes
            DEVMODEW dm;
            dm.dmSize = sizeof(dm);
            
            for (int i = 0; EnumDisplaySettingsW(device_name.c_str(), i, &dm); i++) {
                // Only add refresh rates for the selected resolution
                if (dm.dmPelsWidth == width && dm.dmPelsHeight == height) {
                    std::ostringstream oss;
                    oss << dm.dmDisplayFrequency << " Hz";
                    std::string refresh_rate = oss.str();
                    
                    // Check if this refresh rate is already in the list
                    bool found = false;
                    for (const auto& existing : labels) {
                        if (existing == refresh_rate) {
                            found = true;
                            break;
                        }
                    }
                    
                    if (!found) {
                        labels.push_back(refresh_rate);
                    }
                }
            }
        }
    }
    
    return labels;
}

// Helper function to get max monitor index
float GetMaxMonitorIndex() {
    std::vector<HMONITOR> monitors;
    EnumDisplayMonitors(nullptr, nullptr, 
        [](HMONITOR hmon, HDC, LPRECT, LPARAM lparam) -> BOOL {
            auto* monitors_ptr = reinterpret_cast<std::vector<HMONITOR>*>(lparam);
            monitors_ptr->push_back(hmon);
            return TRUE;
        }, 
        reinterpret_cast<LPARAM>(&monitors));
    return static_cast<float>((std::max)(0, static_cast<int>(monitors.size()) - 1));
}

void AddDisplayTabSettings(std::vector<renodx::utils::settings::Setting*>& settings) {
    // Desktop Resolution Override Checkbox
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "OverrideDesktopResolution",
        .binding = &s_override_desktop_resolution,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f, // Disabled by default
        .label = "Override Desktop Resolution",
        .section = "Display Tab",
        .tooltip = "Enable to override the desktop resolution and refresh rate. This affects the entire desktop, not just games.",
        .is_visible = []() { return is_display_tab(s_ui_mode); } // Show in Display tab mode
    });

    // Monitor Selection
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "SelectedMonitor",
        .binding = &s_selected_monitor_index,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f, // Primary monitor by default
        .label = "Monitor",
        .section = "Display Tab",
        .tooltip = "Select which monitor to override. Only available when Desktop Resolution Override is enabled.",
        .labels = GetMonitorLabels(),
        .min = 0.f,
        .max = GetMaxMonitorIndex(),
        .is_enabled = []() { return s_override_desktop_resolution >= 0.5f; },
        .is_visible = []() { return is_display_tab(s_ui_mode); } // Show in Display tab mode
    });

    // Display Resolution Width
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "DesktopWidth",
        .binding = &s_desktop_width,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 1920.f,
        .label = "Desktop Width",
        .section = "Display Tab",
        .tooltip = "Set the desktop width in pixels. Only available when Desktop Resolution Override is enabled.",
        .min = 800.f,
        .max = 7680.f, // Support up to 8K
        .is_enabled = []() { return s_override_desktop_resolution >= 0.5f; },
        .is_visible = []() { return is_display_tab(s_ui_mode); } // Show in Display tab mode
    });

    // Display Resolution Height
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "DesktopHeight",
        .binding = &s_desktop_height,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 1080.f,
        .label = "Desktop Height",
        .section = "Display Tab",
        .tooltip = "Set the desktop height in pixels. Only available when Desktop Resolution Override is enabled.",
        .min = 600.f,
        .max = 4320.f, // Support up to 8K
        .is_enabled = []() { return s_override_desktop_resolution >= 0.5f; },
        .is_visible = []() { return is_display_tab(s_ui_mode); } // Show in Display tab mode
    });

    // Refresh Rate
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "DesktopRefreshRate",
        .binding = &s_desktop_refresh_rate,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 60.f,
        .label = "Refresh Rate",
        .section = "Display Tab",
        .tooltip = "Set the desktop refresh rate in Hz. Only available when Desktop Resolution Override is enabled.",
        .min = 30.f,
        .max = 360.f, // Support up to 360Hz
        .format = "%d Hz",
        .is_enabled = []() { return s_override_desktop_resolution >= 0.5f; },
        .is_visible = []() { return is_display_tab(s_ui_mode); } // Show in Display tab mode
    });

    // Apply Changes Button
    settings.push_back(new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Apply Desktop Changes",
        .section = "Display Tab",
        .tooltip = "Apply the desktop resolution and refresh rate changes. This will affect the entire desktop.",
        .is_enabled = []() { return s_override_desktop_resolution >= 0.5f; },
        .on_click = []() {
            if (s_override_desktop_resolution >= 0.5f) {
                std::thread([](){
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
                            
                            // Create DEVMODE structure
                            DEVMODEW dm;
                            dm.dmSize = sizeof(dm);
                            dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
                            dm.dmPelsWidth = static_cast<DWORD>(s_desktop_width);
                            dm.dmPelsHeight = static_cast<DWORD>(s_desktop_height);
                            dm.dmDisplayFrequency = static_cast<DWORD>(s_desktop_refresh_rate);
                            
                            // Apply the changes
                            LONG result = ChangeDisplaySettingsExW(device_name.c_str(), &dm, nullptr, CDS_UPDATEREGISTRY, nullptr);
                            
                            if (result == DISP_CHANGE_SUCCESSFUL) {
                                LogInfo("Desktop resolution changed successfully");
                            } else {
                                std::ostringstream oss;
                                oss << "Failed to change desktop resolution. Error code: " << result;
                                LogWarn(oss.str().c_str());
                            }
                        }
                    }
                }).detach();
            }
            return false;
        },
        .is_visible = []() { return is_display_tab(s_ui_mode); } // Show in Display tab mode
    });
}

} // namespace renodx::ui
