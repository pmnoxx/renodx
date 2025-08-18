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
    
    // Get current monitor labels (now with precise refresh rates and raw rational values)
    auto monitor_labels = GetMonitorLabelsFromCache();
            if (monitor_labels.empty()) {
                ImGui::Text("No monitors detected");
                return false;
            }
            
            // Auto-detect current display resolution and set as default if not already set
    if (!s_initial_auto_selection_done) {
        s_initial_auto_selection_done = true;
                
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
    ImGui::Text("Apply Display Changes:");
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Legacy API: Rounds refresh rates to integers (e.g., 143.999Hz → 144Hz)\nModern API: Preserves exact fractional refresh rates (e.g., 143.999Hz → 143.999Hz)");
    }
    
    if (ImGui::Button("Apply Desktop Changes (Legacy)")) {
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
                    
                    // Always use the reliable legacy API for now, but with high-precision refresh rate from cache
                    // Get the exact refresh rate from the rational values
                    double exact_refresh_rate = refresh_rate.ToHz();
                    
                    // Log the values we're trying to apply
                    std::ostringstream debug_oss;
                    debug_oss << "Attempting to apply display changes: Monitor=" << s_selected_monitor_index 
                              << ", Resolution=" << width << "x" << height 
                              << ", Refresh Rate=" << std::fixed << std::setprecision(3) << exact_refresh_rate << "Hz"
                              << " (Rational: " << refresh_rate.numerator << "/" << refresh_rate.denominator << ")";
                    LogInfo(debug_oss.str().c_str());
                    
                    // Get monitor handle from the display cache
                    const auto* display = renodx::display_cache::g_displayCache.GetDisplay(static_cast<int>(s_selected_monitor_index));
                    if (display) {
                        // Get monitor handle
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
                            
                            // Round the refresh rate to the nearest integer for DEVMODE
                            // This gives us the closest refresh rate rather than just truncating
                            dm.dmDisplayFrequency = static_cast<DWORD>(std::round(exact_refresh_rate));
                            
                            // Apply the changes
                            LONG result = ChangeDisplaySettingsExW(device_name.c_str(), &dm, nullptr, CDS_UPDATEREGISTRY, nullptr);
                            
                            if (result == DISP_CHANGE_SUCCESSFUL) {
                                std::ostringstream oss;
                                oss << "Display changes applied successfully: " << width << "x" << height 
                                    << " @ " << std::fixed << std::setprecision(3) << exact_refresh_rate << "Hz";
                                LogInfo(oss.str().c_str());
                                
                                // Log a note about rounding if the refresh rate was rounded
                                if (exact_refresh_rate != static_cast<double>(dm.dmDisplayFrequency)) {
                                    std::ostringstream info_oss;
                                    info_oss << "Note: Refresh rate was rounded from " << std::fixed << std::setprecision(3) 
                                             << exact_refresh_rate << "Hz to " << dm.dmDisplayFrequency << "Hz for compatibility";
                                    LogInfo(info_oss.str().c_str());
                                }
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
    
    ImGui::Spacing();
    
    // API Status Section
    ImGui::Text("API Status:");
    ImGui::SameLine();
    
    // Modern API Status
    bool modern_api_available = renodx::resolution::IsModernDisplayAPIAvailable();
    ImGui::Text("SetDisplayConfig: ");
    ImGui::SameLine();
    if (modern_api_available) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Available");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not Available");
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Modern display API (SetDisplayConfig) is not available on this system.\nThis could be due to:\n- Windows version < 8\n- Graphics driver limitations\n- System policy restrictions");
        }
    }
    
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
    
    // Check if modern API is available and show button state
    if (!modern_api_available) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
    }
    
    if (ImGui::Button("Apply with Modern API (Fractional)")) {
        // Apply the changes using the modern API for fractional refresh rates
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
                    
                    // Get rational refresh rate values from the cache
                    renodx::display_cache::RationalRefreshRate refresh_rate;
                    bool has_rational = renodx::display_cache::g_displayCache.GetRationalRefreshRate(
                        static_cast<int>(s_selected_monitor_index), 
                        static_cast<int>(s_selected_resolution_index),
                        static_cast<int>(s_selected_refresh_rate_index), 
                        refresh_rate);
                    
                    if (has_rational) {
                        // Log the values we're trying to apply with modern API
                        std::ostringstream debug_oss;
                        debug_oss << "Attempting to apply display changes with MODERN API: Monitor=" << s_selected_monitor_index 
                                  << ", Resolution=" << width << "x" << height 
                                  << ", Refresh Rate=" << std::fixed << std::setprecision(10) << refresh_rate.ToHz() << "Hz"
                                  << " (Rational: " << refresh_rate.numerator << "/" << refresh_rate.denominator << ")";
                        LogInfo(debug_oss.str().c_str());
                        
                        // Try modern API first for fractional refresh rates
                        if (renodx::resolution::ApplyDisplaySettingsModern(
                            static_cast<int>(s_selected_monitor_index),
                            width, height,
                            refresh_rate.numerator, refresh_rate.denominator)) {
                            
                            std::ostringstream oss;
                            oss << "Modern API SUCCESS: " << width << "x" << height 
                                << " @ " << std::fixed << std::setprecision(10) << refresh_rate.ToHz() << "Hz"
                                << " (Exact fractional refresh rate applied)";
                            LogInfo(oss.str().c_str());
                        } else {
                            // Fallback to legacy API if modern API fails
                            LogWarn("Modern API failed, falling back to legacy API");
                            
                            // Get the last Windows error for debugging
                            DWORD error = GetLastError();
                            if (error != 0) {
                                LPSTR messageBuffer = nullptr;
                                size_t size = FormatMessageA(
                                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                    NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
                                
                                if (size > 0) {
                                    std::ostringstream error_oss;
                                    error_oss << "Modern API error details: " << std::string(messageBuffer, size);
                                    LogWarn(error_oss.str().c_str());
                                    LocalFree(messageBuffer);
                                }
                            }
                            
                            // Get monitor handle from the display cache
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
                        LogWarn("Failed to get rational refresh rate from cache for modern API");
                    }
                }
            }
        }).detach();
    }
    
    // Restore button colors if modern API is not available
    if (!modern_api_available) {
        ImGui::PopStyleColor(3);
    }
    
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
                sscanf(selected_resolution.c_str(), "%d x %d", &width, &height);
                
                // Get the selected refresh rate from the cache
                auto refresh_rate_labels = renodx::display_cache::g_displayCache.GetRefreshRateLabels(static_cast<int>(s_selected_monitor_index), static_cast<int>(s_selected_resolution_index));
                    if (s_selected_refresh_rate_index >= 0 && s_selected_refresh_rate_index < static_cast<int>(refresh_rate_labels.size())) {
                        std::string selected_refresh_rate = refresh_rate_labels[static_cast<int>(s_selected_refresh_rate_index)];
                    
                    // Get rational refresh rate values from the cache
                    renodx::display_cache::RationalRefreshRate refresh_rate;
                    bool has_rational = renodx::display_cache::g_displayCache.GetRationalRefreshRate(
                        static_cast<int>(s_selected_monitor_index), 
                        static_cast<int>(s_selected_resolution_index),
                        static_cast<int>(s_selected_refresh_rate_index), 
                        refresh_rate);
                    
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
                }
            }).detach();
    }
    
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
