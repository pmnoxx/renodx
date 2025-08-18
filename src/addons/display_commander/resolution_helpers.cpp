#include "resolution_helpers.hpp"
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <iomanip>
#include <cmath>

using Microsoft::WRL::ComPtr;

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
            
            std::set<std::pair<int, int>> resolution_set; // Use set to avoid duplicates and maintain order
            
            for (int i = 0; EnumDisplaySettingsW(device_name.c_str(), i, &dm); i++) {
                // Only add valid resolutions (width > 0, height > 0)
                if (dm.dmPelsWidth > 0 && dm.dmPelsHeight > 0) {
                    resolution_set.insert({dm.dmPelsWidth, dm.dmPelsHeight});
                }
            }
            
            // Convert to sorted list
            for (const auto& pair : resolution_set) {
                std::ostringstream oss;
                oss << pair.first << " x " << pair.second;
                labels.push_back(oss.str());
            }
            
            // Sort by width (ascending - lowest first, regular order)
            std::sort(labels.begin(), labels.end(), [](const std::string& a, const std::string& b) {
                int width_a, height_a, width_b, height_b;
                sscanf(a.c_str(), "%d x %d", &width_a, &height_a);
                sscanf(b.c_str(), "%d x %d", &width_b, &height_b);
                return width_a < width_b; // Changed back to < for ascending order
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

        // Try DXGI first for high precision refresh rates
        bool used_dxgi = false;
        ComPtr<IDXGIFactory1> factory;
        if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))) && factory) {
            for (UINT a = 0; ; ++a) {
                ComPtr<IDXGIAdapter1> adapter;
                if (factory->EnumAdapters1(a, &adapter) == DXGI_ERROR_NOT_FOUND) break;
                for (UINT o = 0; ; ++o) {
                    ComPtr<IDXGIOutput> output;
                    if (adapter->EnumOutputs(o, &output) == DXGI_ERROR_NOT_FOUND) break;
                    DXGI_OUTPUT_DESC desc{};
                    if (FAILED(output->GetDesc(&desc))) continue;
                    if (desc.Monitor != hmon) continue;

                    ComPtr<IDXGIOutput1> output1;
                    if (FAILED(output.As(&output1)) || !output1) continue;

                    UINT num_modes = 0;
                    if (FAILED(output1->GetDisplayModeList1(
                            DXGI_FORMAT_R8G8B8A8_UNORM,
                            0,
                            &num_modes,
                            nullptr))) {
                        continue;
                    }
                    std::vector<DXGI_MODE_DESC1> modes(num_modes);
                    if (FAILED(output1->GetDisplayModeList1(
                            DXGI_FORMAT_R8G8B8A8_UNORM,
                            0,
                            &num_modes,
                            modes.data()))) {
                        continue;
                    }

                    std::vector<double> rates;
                    // Store rational refresh rate data for later use
                    static std::map<std::string, std::pair<UINT32, UINT32>> rational_rates;
                    rational_rates.clear();
                    
                    for (const auto& m : modes) {
                        if (static_cast<int>(m.Width) == width && static_cast<int>(m.Height) == height) {
                            if (m.RefreshRate.Denominator != 0) {
                                double hz = static_cast<double>(m.RefreshRate.Numerator) / static_cast<double>(m.RefreshRate.Denominator);
                                // Deduplicate with epsilon (e.g., 59.94 vs 59.9401)
                                bool exists = false;
                                for (double r : rates) {
                                    if (std::fabs(r - hz) < 0.001) { exists = true; break; }
                                }
                                if (!exists) {
                                    rates.push_back(hz);
                                    // Store the rational values
                                    std::ostringstream oss;
                                    oss << std::setprecision(10) << hz;
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
                                    std::string key = rate_str + "Hz";
                                    rational_rates[key] = {m.RefreshRate.Numerator, m.RefreshRate.Denominator};
                                }
                            }
                        }
                    }

                    std::sort(rates.begin(), rates.end()); // ascending
                    for (double r : rates) {
                        std::ostringstream oss;
                        oss << std::setprecision(10) << r;
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
                        std::string key = rate_str + "Hz";
                        labels.push_back(key);
                    }

                    used_dxgi = true;
                    break; // matched output found
                }
                if (used_dxgi) break;
            }
        }

        // Fallback to EnumDisplaySettings if DXGI path failed
        if (!used_dxgi) {
            MONITORINFOEXW mi;
            mi.cbSize = sizeof(mi);
            if (GetMonitorInfoW(hmon, &mi)) {
                std::wstring device_name = mi.szDevice;
                DEVMODEW dm;
                dm.dmSize = sizeof(dm);
                for (int i = 0; EnumDisplaySettingsW(device_name.c_str(), i, &dm); i++) {
                    if (dm.dmPelsWidth == width && dm.dmPelsHeight == height) {
                        std::ostringstream oss;
                        // Note: dmDisplayFrequency is integer; present it as xx Hz (no trailing zeros)
                        oss << std::setprecision(10) << static_cast<double>(dm.dmDisplayFrequency);
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
                        std::string refresh_rate = rate_str + "Hz";
                        bool found = false;
                        for (const auto& existing : labels) {
                            if (existing == refresh_rate) { found = true; break; }
                        }
                        if (!found) labels.push_back(refresh_rate);
                    }
                }

                std::sort(labels.begin(), labels.end(), [](const std::string& a, const std::string& b) {
                    float freq_a = 0.f, freq_b = 0.f;
                    sscanf(a.c_str(), "%fHz", &freq_a);
                    sscanf(b.c_str(), "%fHz", &freq_b);
                    return freq_a < freq_b;
                });
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
        if (sscanf(selected_refresh_rate.c_str(), "%fHz", &out_refresh_rate) == 1) {
            return true;
        }
    }
    return false;
}

// Helper function to get selected refresh rate as rational values
bool GetSelectedRefreshRateRational(int monitor_index, int width, int height, int refresh_rate_index, 
                                   UINT32& out_numerator, UINT32& out_denominator) {
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

        // Try DXGI first for high precision refresh rates
        ComPtr<IDXGIFactory1> factory;
        if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))) && factory) {
            for (UINT a = 0; ; ++a) {
                ComPtr<IDXGIAdapter1> adapter;
                if (factory->EnumAdapters1(a, &adapter) == DXGI_ERROR_NOT_FOUND) break;
                for (UINT o = 0; ; ++o) {
                    ComPtr<IDXGIOutput> output;
                    if (adapter->EnumOutputs(o, &output) == DXGI_ERROR_NOT_FOUND) break;
                    DXGI_OUTPUT_DESC desc{};
                    if (FAILED(output->GetDesc(&desc))) continue;
                    if (desc.Monitor != hmon) continue;

                    ComPtr<IDXGIOutput1> output1;
                    if (FAILED(output.As(&output1)) || !output1) continue;

                    UINT num_modes = 0;
                    if (FAILED(output1->GetDisplayModeList1(
                            DXGI_FORMAT_R8G8B8A8_UNORM,
                            0,
                            &num_modes,
                            nullptr))) {
                        continue;
                    }
                    std::vector<DXGI_MODE_DESC1> modes(num_modes);
                    if (FAILED(output1->GetDisplayModeList1(
                            DXGI_FORMAT_R8G8B8A8_UNORM,
                            0,
                            &num_modes,
                            modes.data()))) {
                        continue;
                    }

                    std::vector<double> rates;
                    std::vector<std::pair<UINT32, UINT32>> rational_rates;
                    
                    for (const auto& m : modes) {
                        if (static_cast<int>(m.Width) == width && static_cast<int>(m.Height) == height) {
                            if (m.RefreshRate.Denominator != 0) {
                                double hz = static_cast<double>(m.RefreshRate.Numerator) / static_cast<double>(m.RefreshRate.Denominator);
                                // Deduplicate with epsilon (e.g., 59.94 vs 59.9401)
                                bool exists = false;
                                for (double r : rates) {
                                    if (std::fabs(r - hz) < 0.001) { exists = true; break; }
                                }
                                if (!exists) {
                                    rates.push_back(hz);
                                    rational_rates.push_back({m.RefreshRate.Numerator, m.RefreshRate.Denominator});
                                }
                            }
                        }
                    }

                    if (refresh_rate_index >= 0 && refresh_rate_index < static_cast<int>(rational_rates.size())) {
                        out_numerator = rational_rates[refresh_rate_index].first;
                        out_denominator = rational_rates[refresh_rate_index].second;
                        return true;
                    }
                    break; // matched output found
                }
                break;
            }
        }
    }
    
    return false;
}

// Helper function to apply display settings using modern API with rational refresh rates
bool ApplyDisplaySettingsModern(int monitor_index, int width, int height, UINT32 refresh_numerator, UINT32 refresh_denominator) {
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

        // Get the path info for this monitor
        MONITORINFOEXW mi;
        mi.cbSize = sizeof(mi);
        if (!GetMonitorInfoW(hmon, &mi)) {
            return false;
        }

        // Try to use modern SetDisplayConfig API
        UINT32 path_elements = 0;
        UINT32 mode_elements = 0;
        if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &path_elements, &mode_elements) == ERROR_SUCCESS) {
            std::vector<DISPLAYCONFIG_PATH_INFO> paths(path_elements);
            std::vector<DISPLAYCONFIG_MODE_INFO> modes(mode_elements);
            
            if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &path_elements, paths.data(), &mode_elements, modes.data(), nullptr) == ERROR_SUCCESS) {
                // Find the path for our monitor
                for (UINT32 i = 0; i < path_elements; i++) {
                    DISPLAYCONFIG_SOURCE_DEVICE_NAME source_name = {};
                    source_name.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
                    source_name.header.size = sizeof(source_name);
                    source_name.header.adapterId = paths[i].sourceInfo.adapterId;
                    source_name.header.id = paths[i].sourceInfo.id;
                    
                    if (DisplayConfigGetDeviceInfo(&source_name.header) == ERROR_SUCCESS) {
                        // Convert monitor device name to wide string for comparison
                        std::wstring monitor_device(mi.szDevice);
                        if (monitor_device == source_name.viewGdiDeviceName) {
                            // Found our monitor path, now modify the mode
                            if (paths[i].sourceInfo.modeInfoIdx < mode_elements) {
                                auto& source_mode = modes[paths[i].sourceInfo.modeInfoIdx].sourceMode;
                                source_mode.width = width;
                                source_mode.height = height;
                                
                                // Set the rational refresh rate
                                paths[i].targetInfo.refreshRate.Numerator = refresh_numerator;
                                paths[i].targetInfo.refreshRate.Denominator = refresh_denominator;
                                
                                // Apply the changes
                                if (SetDisplayConfig(path_elements, paths.data(), mode_elements, modes.data(), 
                                                   SDC_APPLY | SDC_USE_SUPPLIED_DISPLAY_CONFIG) == ERROR_SUCCESS) {
                                    return true;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return false;
}

} // namespace renodx::resolution
