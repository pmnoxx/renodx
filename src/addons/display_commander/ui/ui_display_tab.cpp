#include "ui_display_tab.hpp"
#include "ui_common.hpp"
#include "../../../utils/settings.hpp"
#include "../addon.hpp"
#include "../resolution_helpers.hpp"
#include <windows.h>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map> // Added for resolution_map
#include <iomanip> // Added for std::fixed and std::setprecision
#include <atomic>

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

// Helper function to get current display info based on game position
std::string GetCurrentDisplayInfo() {
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
    
    // Get monitor information
    MONITORINFOEXW mi;
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(monitor, &mi)) {
        return "Failed to get monitor info";
    }
    
    // Get current display settings for this monitor
    DEVMODEW dm;
    dm.dmSize = sizeof(dm);
    if (!EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm)) {
        return "Failed to get display settings";
    }
    
    // Convert device name to string
    std::wstring device_name = mi.szDevice;
    std::string device_name_str(device_name.begin(), device_name.end());
    
    // Format the display info in one line
    std::ostringstream oss;
    oss << device_name_str << " - " << dm.dmPelsWidth << "x" << dm.dmPelsHeight 
        << " @ " << dm.dmDisplayFrequency << "Hz";
    
    return oss.str();
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
            // Get current monitor labels
            auto monitor_labels = GetMonitorLabels();
            if (monitor_labels.empty()) {
                ImGui::Text("No monitors detected");
                return false;
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
            auto resolution_labels = renodx::resolution::GetResolutionLabels(static_cast<int>(s_selected_monitor_index));
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
                std::string selected_resolution = resolution_labels[static_cast<int>(s_selected_resolution_index)];
                int width, height;
                if (sscanf(selected_resolution.c_str(), "%d x %d", &width, &height) == 2) {
                    auto refresh_rate_labels = renodx::resolution::GetRefreshRateLabels(static_cast<int>(s_selected_monitor_index), width, height);
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
            }
            
            // Apply Changes Button
            if (ImGui::Button("Apply Desktop Changes")) {
                // Apply the changes using the existing logic
                if (s_override_desktop_resolution >= 0.5f) {
                    std::thread([](){
                        // Get the selected resolution from the dynamic list
                        auto resolution_labels = renodx::resolution::GetResolutionLabels(static_cast<int>(s_selected_monitor_index));
                        if (s_selected_resolution_index >= 0 && s_selected_resolution_index < static_cast<int>(resolution_labels.size())) {
                            std::string selected_resolution = resolution_labels[static_cast<int>(s_selected_resolution_index)];
                            int width, height;
                            sscanf(selected_resolution.c_str(), "%d x %d", &width, &height);
                            
                            // Get the selected refresh rate from the dynamic list
                            auto refresh_rate_labels = renodx::resolution::GetRefreshRateLabels(static_cast<int>(s_selected_monitor_index), width, height);
                            if (s_selected_refresh_rate_index >= 0 && s_selected_refresh_rate_index < static_cast<int>(refresh_rate_labels.size())) {
                                std::string selected_refresh_rate = refresh_rate_labels[static_cast<int>(s_selected_refresh_rate_index)];
                                float refresh_rate;
                                sscanf(selected_refresh_rate.c_str(), "%f Hz", &refresh_rate);
                                
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
                                            oss << "Display changes applied successfully: " << width << "x" << height 
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
                    }).detach();
                }
            }
            
            return false; // No value change
        },
        .is_visible = []() { return is_basic_tab(s_ui_mode) || is_display_tab(s_ui_mode); } // Show in Simple and Display modes
    });

    // Old UI:


    // Monitor Selection
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "SelectedMonitor",
        .binding = &s_selected_monitor_index,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f, // Primary monitor by default
        .label = "Monitor",
        .section = "Display Tab",
        .tooltip = "Select which monitor to view and configure. Available options are always shown regardless of override setting.",
        .labels = GetMonitorLabels(),
        .min = 0.f,
        .max = GetMaxMonitorIndex(),
        .is_enabled = []() { return true; }, // Always enabled for selection
        .is_visible = []() { return is_display_tab(s_ui_mode); } // Show in Display tab mode
    });

    // Resolution Selection (dynamically populated based on selected monitor)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "SelectedResolution",
        .binding = &s_selected_resolution_index,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f, // Default to first available resolution
        .label = "Resolution",
        .section = "Display Tab",
        .tooltip = "Select resolution from the list of supported resolutions for the selected monitor. Available options are always shown regardless of override setting.",
        .labels = renodx::resolution::GetResolutionLabels(static_cast<int>(s_selected_monitor_index)),
        .min = 0.f,
        .max = []() { 
            auto labels = renodx::resolution::GetResolutionLabels(static_cast<int>(s_selected_monitor_index));
            return static_cast<float>((std::max)(0, static_cast<int>(labels.size()) - 1));
        }(),
        .is_enabled = []() { return true; }, // Always enabled for selection
        .is_visible = []() { return is_display_tab(s_ui_mode); } // Show in Display tab mode
    });

    // Refresh Rate Selection (dynamically populated based on selected monitor and resolution)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "SelectedRefreshRate",
        .binding = &s_selected_refresh_rate_index,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f, // Default to first available refresh rate
        .label = "Refresh Rate",
        .section = "Display Tab",
        .tooltip = "Select refresh rate from the list of supported refresh rates for the selected resolution. Available options are always shown regardless of override setting.",
        .labels = []() {
            // Get the selected resolution to find refresh rates
            auto resolution_labels = renodx::resolution::GetResolutionLabels(static_cast<int>(s_selected_monitor_index));
            if (s_selected_resolution_index >= 0 && s_selected_resolution_index < static_cast<int>(resolution_labels.size())) {
                std::string selected_resolution = resolution_labels[static_cast<int>(s_selected_resolution_index)];
                int width, height;
                sscanf(selected_resolution.c_str(), "%d x %d", &width, &height);
                return renodx::resolution::GetRefreshRateLabels(static_cast<int>(s_selected_monitor_index), width, height);
            }
            return std::vector<std::string>(); // Return empty if no resolution selected
        }(),
        .min = 0.f,
        .max = []() { 
            auto resolution_labels = renodx::resolution::GetResolutionLabels(static_cast<int>(s_selected_monitor_index));
            if (s_selected_resolution_index >= 0 && s_selected_resolution_index < static_cast<int>(resolution_labels.size())) {
                std::string selected_resolution = resolution_labels[static_cast<int>(s_selected_resolution_index)];
                int width, height;
                sscanf(selected_resolution.c_str(), "%d x %d", &width, &height);
                auto refresh_labels = renodx::resolution::GetRefreshRateLabels(static_cast<int>(s_selected_monitor_index), width, height);
                return static_cast<float>((std::max)(0, static_cast<int>(refresh_labels.size()) - 1));
            }
            return 0.f;
        }(),
        .is_enabled = []() { return true; }, // Always enabled for selection
        .is_visible = []() { return is_display_tab(s_ui_mode); } // Show in Display tab mode
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
            std::string display_info = GetCurrentDisplayInfo();
            ImGui::TextUnformatted(display_info.c_str());
            return false; // No value change
        },
        .is_visible = []() { return is_basic_tab(s_ui_mode) || is_display_tab(s_ui_mode); } // Show in Simple and Display modes
    });


    // Apply Changes Button
    settings.push_back(new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Apply Desktop Changes",
        .section = "Display Tab",
        .tooltip = "Apply the selected resolution and refresh rate to the desktop. This button is only available when Desktop Resolution Override is enabled.",
        .on_click = []() {
            std::thread([](){
                                // Get the selected resolution from the dynamic list
            auto resolution_labels = renodx::resolution::GetResolutionLabels(static_cast<int>(s_selected_monitor_index));
            if (s_selected_resolution_index >= 0 && s_selected_resolution_index < static_cast<int>(resolution_labels.size())) {
                std::string selected_resolution = resolution_labels[static_cast<int>(s_selected_resolution_index)];
                int width, height;
                sscanf(selected_resolution.c_str(), "%d x %d", &width, &height);
                
                // Get the selected refresh rate from the dynamic list
                auto refresh_rate_labels = renodx::resolution::GetRefreshRateLabels(static_cast<int>(s_selected_monitor_index), width, height);
                    if (s_selected_refresh_rate_index >= 0 && s_selected_refresh_rate_index < static_cast<int>(refresh_rate_labels.size())) {
                        std::string selected_refresh_rate = refresh_rate_labels[static_cast<int>(s_selected_refresh_rate_index)];
                        float refresh_rate;
                        sscanf(selected_refresh_rate.c_str(), "%f Hz", &refresh_rate);
                        
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
                                    oss << "Display changes applied successfully: " << width << "x" << height 
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
            }).detach();
            return false;
        },
        .is_visible = []() { return is_display_tab(s_ui_mode); } // Show in Display tab mode
    });
}

} // namespace renodx::ui
