#include "monitor_settings.hpp"
#include "../ui_display_tab.hpp"
#include "../../display_cache.hpp"
#include "../../resolution_helpers.hpp"
#include "../../utils.hpp"
#include <sstream>
#include <thread>
#include <iomanip>
#include <deps/imgui/imgui.h>

// External declarations
extern std::atomic<HWND> g_last_swapchain_hwnd;
extern float s_selected_monitor_index;
extern float s_selected_resolution_index;
extern float s_selected_refresh_rate_index;
extern bool s_initial_auto_selection_done;


namespace renodx::ui::monitor_settings {

// Handle display cache refresh logic (every 60 frames)
void HandleDisplayCacheRefresh() {
    // Reset display cache every 60 frames to keep it fresh
    static int frame_counter = 0;
    frame_counter++;
    if (frame_counter >= 60) {
        renodx::display_cache::g_displayCache.Refresh();
        frame_counter = 0;
        
        // Debug: Log current display info after refresh
        HWND hwnd = g_last_swapchain_hwnd.load();
        if (hwnd) {
            HMONITOR current_monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            if (current_monitor) {
                const auto* display = renodx::display_cache::g_displayCache.GetDisplayByHandle(current_monitor);
                if (display) {
                    std::ostringstream debug_oss;
                    debug_oss << "Cache refreshed - Current: " << display->GetCurrentResolutionString() 
                              << " @ " << display->GetCurrentRefreshRateString()
                              << " [Raw: " << display->current_refresh_rate.numerator << "/" 
                              << display->current_refresh_rate.denominator << "]";
                    LogInfo(debug_oss.str().c_str());
                }
            }
        }
    }
}

// Handle auto-detection of current display settings
void HandleAutoDetection() {
    if (!s_initial_auto_selection_done) {
        s_initial_auto_selection_done = true;
        
        // Get current display info for the selected monitor
        HWND hwnd = g_last_swapchain_hwnd.load();
        if (hwnd) {
            HMONITOR current_monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            if (current_monitor) {
                // Get monitor labels to find which monitor index this corresponds to
                auto monitor_labels = renodx::ui::GetMonitorLabelsFromCache();
                
                // Find which monitor index this corresponds to
                for (int i = 0; i < static_cast<int>(monitor_labels.size()); i++) {
                    const auto* display = renodx::display_cache::g_displayCache.GetDisplay(i);
                    if (display && display->monitor_handle == current_monitor) {
                        s_selected_monitor_index = static_cast<float>(i);
                        
                        // Use the new methods to find closest supported modes to current settings
                        auto closest_resolution_index = display->FindClosestResolutionIndex();
                        if (closest_resolution_index.has_value()) {
                            s_selected_resolution_index = static_cast<float>(closest_resolution_index.value());
                            
                            // Find closest refresh rate within this resolution
                            auto closest_refresh_rate_index = display->FindClosestRefreshRateIndex(closest_resolution_index.value());
                            if (closest_refresh_rate_index.has_value()) {
                                s_selected_refresh_rate_index = static_cast<float>(closest_refresh_rate_index.value());
                                
                                // Debug: Log what we found
                                std::ostringstream found_oss;
                                found_oss << "Auto-detected: Resolution " << s_selected_resolution_index 
                                          << " (closest to current " << display->GetCurrentResolutionString() 
                                          << "), Refresh Rate " << s_selected_refresh_rate_index 
                                          << " (closest to current " << display->GetCurrentRefreshRateString() << ")";
                                LogInfo(found_oss.str().c_str());
                            } else {
                                // Fallback to first refresh rate if no match found
                                s_selected_refresh_rate_index = 0.0f;
                                LogWarn("No refresh rate match found, using first available");
                            }
                        } else {
                            // Fallback to first resolution if no match found
                            s_selected_resolution_index = 0.0f;
                            s_selected_refresh_rate_index = 0.0f;
                            LogWarn("No resolution match found, using first available");
                        }
                        break;
                    }
                }
            }
        }
    }
}

// Handle monitor selection UI
void HandleMonitorSelection(const std::vector<std::string>& monitor_labels) {
    // Monitor selection
    if (ImGui::BeginCombo("Monitor", monitor_labels[static_cast<int>(s_selected_monitor_index)].c_str())) {
        for (int i = 0; i < static_cast<int>(monitor_labels.size()); i++) {
            const bool is_selected = (i == static_cast<int>(s_selected_monitor_index));
            if (ImGui::Selectable(monitor_labels[i].c_str(), is_selected)) {
                s_selected_monitor_index = static_cast<float>(i);
                
                // Auto-select closest resolution and refresh rate for the newly selected monitor
                const auto* display = renodx::display_cache::g_displayCache.GetDisplay(i);
                if (display) {
                    // Find closest supported resolution to current settings
                    auto closest_resolution_index = display->FindClosestResolutionIndex();
                    if (closest_resolution_index.has_value()) {
                        s_selected_resolution_index = static_cast<float>(closest_resolution_index.value());
                        
                        // Find closest refresh rate within this resolution
                        auto closest_refresh_rate_index = display->FindClosestRefreshRateIndex(closest_resolution_index.value());
                        if (closest_refresh_rate_index.has_value()) {
                            s_selected_refresh_rate_index = static_cast<float>(closest_refresh_rate_index.value());
                            
                            // Debug: Log what we auto-selected
                            std::ostringstream auto_select_oss;
                            auto_select_oss << "Auto-selected for monitor " << i << ": Resolution " 
                                           << s_selected_resolution_index << " (closest to current " 
                                           << display->GetCurrentResolutionString() << "), Refresh Rate " 
                                           << s_selected_refresh_rate_index << " (closest to current " 
                                           << display->GetCurrentRefreshRateString() << ")";
                            LogInfo(auto_select_oss.str().c_str());
                        } else {
                            // Fallback to first refresh rate if no match found
                            s_selected_refresh_rate_index = 0.0f;
                            LogWarn("No refresh rate match found for auto-selection, using first available");
                        }
                    } else {
                        // Fallback to first resolution if no match found
                        s_selected_resolution_index = 0.0f;
                        s_selected_refresh_rate_index = 0.0f;
                        LogWarn("No resolution match found for auto-selection, using first available");
                    }
                }
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

// Handle resolution selection UI
void HandleResolutionSelection(int selected_monitor_index) {
    auto resolution_labels = renodx::display_cache::g_displayCache.GetResolutionLabels(selected_monitor_index);
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
}

// Handle refresh rate selection UI
void HandleRefreshRateSelection(int selected_monitor_index, int selected_resolution_index) {
    if (s_selected_resolution_index >= 0) {
        auto resolution_labels = renodx::display_cache::g_displayCache.GetResolutionLabels(selected_monitor_index);
        if (s_selected_resolution_index < static_cast<int>(resolution_labels.size())) {
            auto refresh_rate_labels = renodx::display_cache::g_displayCache.GetRefreshRateLabels(selected_monitor_index, selected_resolution_index);
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
}







// Handle the "Apply with DXGI API" button
void HandleDXGIAPIApplyButton() {
    // DXGI API Button (Alternative Fractional Method)
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Uses DXGI SetFullscreenState + ResizeTarget to set fractional refresh rates.\nThis method creates a temporary swap chain to apply the mode.");
    }
    ImGui::SameLine();
    if (ImGui::Button("Apply with DXGI API")) {
        // Apply the changes using the DXGI API for fractional refresh rates
        std::thread([](){
            // Get the selected resolution from the cache
            auto resolution_labels = renodx::display_cache::g_displayCache.GetResolutionLabels(static_cast<int>(s_selected_monitor_index));
            if (s_selected_resolution_index >= 0 && s_selected_resolution_index < static_cast<int>(resolution_labels.size())) {
                std::string selected_resolution = resolution_labels[static_cast<int>(s_selected_resolution_index)];
                int width, height;
                sscanf_s(selected_resolution.c_str(), "%d x %d", &width, &height);
                
                // Get the selected refresh rate from the cache
                renodx::display_cache::RationalRefreshRate refresh_rate;
                bool has_rational = false;
                
                // Use selected refresh rate from cache
                auto refresh_rate_labels = renodx::display_cache::g_displayCache.GetRefreshRateLabels(static_cast<int>(s_selected_monitor_index), static_cast<int>(s_selected_resolution_index));
                if (s_selected_refresh_rate_index >= 0 && s_selected_refresh_rate_index < static_cast<int>(refresh_rate_labels.size())) {
                    std::string selected_refresh_rate = refresh_rate_labels[static_cast<int>(s_selected_refresh_rate_index)];
                    
                    // Get rational refresh rate values from the cache
                    has_rational = renodx::display_cache::g_displayCache.GetRationalRefreshRate(
                        static_cast<int>(s_selected_monitor_index), 
                        static_cast<int>(s_selected_resolution_index),
                        static_cast<int>(s_selected_refresh_rate_index), 
                        refresh_rate);
                }
                
                if (has_rational) {
                    // Log the values we're trying to apply with DXGI API
                    std::ostringstream debug_oss;
                    debug_oss << "Attempting to apply display changes with DXGI API: Monitor=" << s_selected_monitor_index 
                              << ", Resolution=" << width << "x" << height 
                              << ", Refresh Rate=" << std::fixed << std::setprecision(10) << refresh_rate.ToHz() << "Hz"
                              << " (Rational: " << refresh_rate.numerator << "/" << refresh_rate.denominator << ")";
                    LogInfo(debug_oss.str().c_str());
                    
                    // Try DXGI API for fractional refresh rates
                    if (renodx::resolution::ApplyDisplaySettingsDXGI(
                        static_cast<int>(s_selected_monitor_index),
                        width, height,
                        refresh_rate.numerator, refresh_rate.denominator)) {
                        
                        std::ostringstream oss;
                        oss << "DXGI API SUCCESS: " << width << "x" << height 
                            << " @ " << std::fixed << std::setprecision(10) << refresh_rate.ToHz() << "Hz"
                            << " (Exact fractional refresh rate applied via DXGI)";
                        LogInfo(oss.str().c_str());
                    } else {
                        // DXGI API failed, fall back to legacy API
                        LogWarn("DXGI API failed, falling back to legacy API");
                        
                        // Get the last Windows error for debugging
                        DWORD error = GetLastError();
                        if (error != 0) {
                            LPSTR messageBuffer = nullptr;
                            size_t size = FormatMessageA(
                                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
                            
                            if (size > 0) {
                                std::ostringstream error_oss;
                                error_oss << "DXGI API error details: " << std::string(messageBuffer, size);
                                LogWarn(error_oss.str().c_str());
                                LocalFree(messageBuffer);
                            }
                        }
                        
                        // Fallback to legacy API
                        const auto* display = renodx::display_cache::g_displayCache.GetDisplay(static_cast<int>(s_selected_monitor_index));
                        if (display) {
                            HMONITOR hmon = display->monitor_handle;
                            
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
                                
                                // Round the refresh rate to the nearest integer for DEVMODE fallback
                                dm.dmDisplayFrequency = static_cast<DWORD>(std::round(refresh_rate.ToHz()));
                                
                                // Apply the changes using legacy API
                                LONG result = ChangeDisplaySettingsExW(device_name.c_str(), &dm, nullptr, CDS_UPDATEREGISTRY, nullptr);
                                
                                if (result == DISP_CHANGE_SUCCESSFUL) {
                                    std::ostringstream oss;
                                    oss << "Legacy API fallback SUCCESS: " << width << "x" << height 
                                        << " @ " << std::fixed << std::setprecision(3) << refresh_rate.ToHz() << "Hz"
                                        << " (Note: Refresh rate was rounded for compatibility)";
                                    LogInfo(oss.str().c_str());
                                } else {
                                    std::ostringstream oss;
                                    oss << "Legacy API fallback also failed. Error code: " << result;
                                    LogWarn(oss.str().c_str());
                                }
                            }
                        }
                    }
                } else {
                    LogWarn("Failed to get rational refresh rate from cache for DXGI API");
                }
            }
        }).detach();
    }
}

// Display API status information
void DisplayAPIStatus() {
    // API Status Section
    ImGui::Text("API Status:");
    ImGui::SameLine();
    
    // DXGI API Status
    ImGui::SameLine();
    ImGui::Text("DXGI: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Available");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("DXGI API is available on all Windows systems with DirectX support.\nThis method creates a temporary swap chain to apply display modes.");
    }
    
    // Modern API Button (Fractional Refresh Rates)
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        if (renodx::resolution::IsModernDisplayAPIAvailable()) {
            ImGui::SetTooltip("Uses SetDisplayConfig API to preserve exact fractional refresh rates.\nFalls back to legacy API if modern API fails.");
        } else {
            ImGui::SetTooltip("Modern API not available on this system.\nWill fall back to legacy API automatically.");
        }
    }
    ImGui::SameLine();
}

} // namespace renodx::ui::monitor_settings
