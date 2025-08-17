#include "resolution_helpers.hpp"

namespace renodx::resolution {

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
            
            std::map<int, int> resolution_map; // width -> height mapping to avoid duplicates
            
            for (int i = 0; EnumDisplaySettingsW(device_name.c_str(), i, &dm); i++) {
                // Store the highest height for each width (prefer higher resolutions)
                if (resolution_map.find(dm.dmPelsWidth) == resolution_map.end() || 
                    dm.dmPelsHeight > resolution_map[dm.dmPelsWidth]) {
                    resolution_map[dm.dmPelsWidth] = dm.dmPelsHeight;
                }
            }
            
            // Convert to sorted list
            for (const auto& pair : resolution_map) {
                std::ostringstream oss;
                oss << pair.first << " x " << pair.second;
                labels.push_back(oss.str());
            }
            
            // Sort by width (ascending)
            std::sort(labels.begin(), labels.end(), [](const std::string& a, const std::string& b) {
                int width_a, height_a, width_b, height_b;
                sscanf(a.c_str(), "%d x %d", &width_a, &height_a);
                sscanf(b.c_str(), "%d x %d", &width_b, &height_b);
                return width_a < width_b;
            });
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

// Helper function to get selected resolution
bool GetSelectedResolution(int monitor_index, int resolution_index, int& out_width, int& out_height) {
    auto labels = GetResolutionLabels(monitor_index);
    if (resolution_index >= 0 && resolution_index < static_cast<int>(labels.size())) {
        std::string selected_resolution = labels[resolution_index];
        if (sscanf(selected_resolution.c_str(), "%d x %d", &out_width, &out_height) == 2) {
            return true;
        }
    }
    return false;
}

// Helper function to get selected refresh rate
bool GetSelectedRefreshRate(int monitor_index, int width, int height, int refresh_rate_index, float& out_refresh_rate) {
    auto labels = GetRefreshRateLabels(monitor_index, width, height);
    if (refresh_rate_index >= 0 && refresh_rate_index < static_cast<int>(labels.size())) {
        std::string selected_refresh_rate = labels[refresh_rate_index];
        if (sscanf(selected_refresh_rate.c_str(), "%f Hz", &out_refresh_rate) == 1) {
            return true;
        }
    }
    return false;
}

} // namespace renodx::resolution
