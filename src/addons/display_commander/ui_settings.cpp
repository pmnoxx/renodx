#include "addon.hpp"
#include "nvapi_fullscreen_prevention.hpp"
#include "reflex_management.hpp"
#include "dxgi_device_info.hpp"
#include "utils.hpp"
#include "ui_settings.hpp"
#include <sstream>
#include <iomanip>

// External declarations
extern float s_reflex_debug_output;

// Continuous monitoring function declarations
extern void StartContinuousMonitoring();
extern void StopContinuousMonitoring();

// Helper functions for tab visibility
inline bool is_basic_tab(float ui_mode) { return ui_mode < 0.5f; }
inline bool is_developer_tab(float ui_mode) { return ui_mode >= 0.5f && ui_mode < 2.0f; }
inline bool is_device_tab(float ui_mode) { return ui_mode >= 2.0f; }

// Function to add UI settings to an existing settings vector
void AddUISettings(renodx::utils::settings::Settings& settings) {
    // DXGI Device Information Tab
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "DxgiDeviceInfo",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "",
        .section = "DXGI",
        .tooltip = "Detailed DXGI adapter and output information including HDR capabilities.",
        .on_draw = [](){
            if (!g_dxgiDeviceInfoManager || !g_dxgiDeviceInfoManager->IsInitialized()) {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "DXGI Device Info Manager not initialized");
                return false;
            }

            // Force device enumeration when this tab is first opened
            static bool first_open = true;
            if (first_open) {
                g_dxgiDeviceInfoManager->RefreshDeviceInfo();
                first_open = false;
            }

            const auto& adapters = g_dxgiDeviceInfoManager->GetAdapters();
            if (adapters.empty()) {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No DXGI adapters found yet. Device enumeration happens automatically during present operations.");
                ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "If you're still not seeing adapters, try refreshing or check if a game/application is running.");
                return false;
            }

            // Add refresh button
            if (ImGui::Button("Refresh Device Info")) {
                g_dxgiDeviceInfoManager->RefreshDeviceInfo();
            }
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Click to refresh device information");
            
            // Add force re-enumeration button
            ImGui::SameLine();
            if (ImGui::Button("Force Re-enumeration")) {
                g_dxgiDeviceInfoManager->RefreshDeviceInfo();
            }
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Force re-enumeration of all devices");
            
            // Add HDR metadata reset button
            ImGui::SameLine();
            if (ImGui::Button("Reset HDR Metadata")) {
                // Find first HDR10-capable output and reset its metadata
                const auto& adapters = g_dxgiDeviceInfoManager->GetAdapters();
                for (const auto& adapter : adapters) {
                    for (const auto& output : adapter.outputs) {
                        if (output.supports_hdr10) {
                            if (g_dxgiDeviceInfoManager->ResetHDRMetadataOnPresent(output.device_name)) {
                                LogInfo(("HDR metadata reset initiated for: " + output.device_name).c_str());
                            } else {
                                LogWarn(("HDR metadata reset failed for: " + output.device_name).c_str());
                            }
                            break; // Only reset the first HDR10 output found
                        }
                    }
                }
            }
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Reset HDR metadata on next present");
            
            ImGui::Separator();
            
            // Display adapter information
            for (size_t i = 0; i < adapters.size(); ++i) {
                const auto& adapter = adapters[i];
                
                if (ImGui::CollapsingHeader(("Adapter " + std::to_string(i) + ": " + adapter.name).c_str())) {
                    ImGui::Text("Description: %s", adapter.description.c_str());
                    ImGui::Text("Dedicated Video Memory: %zu MB", adapter.dedicated_video_memory / (1024 * 1024));
                    ImGui::Text("Dedicated System Memory: %zu MB", adapter.dedicated_system_memory / (1024 * 1024));
                    ImGui::Text("Shared System Memory: %zu MB", adapter.shared_system_memory / (1024 * 1024));
                    ImGui::Text("Software Adapter: %s", adapter.is_software ? "Yes" : "No");
                    
                    // Display output information
                    if (ImGui::TreeNode(("Outputs (" + std::to_string(adapter.outputs.size()) + ")").c_str())) {
                        for (size_t j = 0; j < adapter.outputs.size(); ++j) {
                            const auto& output = adapter.outputs[j];
                            
                            if (ImGui::TreeNode(("Output " + std::to_string(j) + ": " + output.device_name).c_str())) {
                                ImGui::Text("Device Name: %s", output.device_name.c_str());
                                ImGui::Text("Monitor Name: %s", output.monitor_name.c_str());
                                ImGui::Text("Attached to Desktop: %s", output.is_attached ? "Yes" : "No");
                                ImGui::Text("Supports HDR10: %s", output.supports_hdr10 ? "Yes" : "No");
                                
                                if (output.is_attached) {
                                    ImGui::Text("Desktop Coordinates: (%ld, %ld) to (%ld, %ld)",
                                               output.desktop_coordinates.left, output.desktop_coordinates.top,
                                               output.desktop_coordinates.right, output.desktop_coordinates.bottom);
                                    
                                    // Display supported modes
                                    if (ImGui::TreeNode(("Supported Modes (" + std::to_string(output.supported_modes.size()) + ")").c_str())) {
                                        for (size_t k = 0; k < output.supported_modes.size(); ++k) {
                                            const auto& mode = output.supported_modes[k];
                                            ImGui::Text("Mode %zu: %ux%u @ %u Hz", k, mode.Width, mode.Height, mode.RefreshRate.Numerator / mode.RefreshRate.Denominator);
                                        }
                                        ImGui::TreePop();
                                    }
                                }
                                
                                ImGui::TreePop();
                            }
                        }
                        ImGui::TreePop();
                    }
                }
            }
            
            return false;
        },
        .is_visible = []() { return is_device_tab(s_ui_mode); } // Only show in Device mode
    });

    // DXGI Composition and Backbuffer Information
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "DxgiCompositionInfo",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "",
        .section = "DXGI",
        .tooltip = "Current DXGI composition mode and backbuffer information.",
        .on_draw = [](){
            const char* mode_str = "Unknown";
            switch (static_cast<int>(s_dxgi_composition_state)) {
                case 1: mode_str = "Composed Flip"; break;
                case 2: mode_str = "Modern Independent Flip"; break;
                case 3: mode_str = "Legacy Independent Flip"; break;
                default: mode_str = "Unknown"; break;
            }
            
            // Check exclusive fullscreen status
            HWND hwnd = g_last_swapchain_hwnd.load();
            if (hwnd == nullptr) hwnd = GetForegroundWindow();
            const bool is_exclusive_fullscreen = GetSpoofedFullscreenState(hwnd);
            
            // Get backbuffer format
            std::string format_str = "Unknown";
            if (g_last_swapchain_ptr.load() != nullptr) {
                auto* device = g_last_swapchain_ptr.load()->get_device();
                if (device != nullptr && g_last_swapchain_ptr.load()->get_back_buffer_count() > 0) {
                    auto bb = g_last_swapchain_ptr.load()->get_back_buffer(0);
                    auto desc = device->get_resource_desc(bb);
                    switch (desc.texture.format) {
                        case reshade::api::format::r8g8b8a8_unorm: format_str = "R8G8B8A8_UNORM"; break;
                        case reshade::api::format::r8g8b8a8_unorm_srgb: format_str = "R8G8B8A8_UNORM_SRGB"; break;
                        case reshade::api::format::r10g10b10a2_unorm: format_str = "R10G10B10A2_UNORM"; break;
                        case reshade::api::format::r16g16b16a16_float: format_str = "R16G16B16A16_FLOAT"; break;
                        case reshade::api::format::r11g11b10_float: format_str = "R11G11B10_FLOAT"; break;
                        case reshade::api::format::b8g8r8a8_unorm: format_str = "B8G8R8A8_UNORM"; break;
                        case reshade::api::format::b8g8r8a8_unorm_srgb: format_str = "B8G8R8A8_UNORM_SRGB"; break;
                        case reshade::api::format::r8g8b8_unorm: format_str = "R8G8B8_UNORM"; break;
                        case reshade::api::format::r8g8b8_unorm_srgb: format_str = "R8G8B8_UNORM_SRGB"; break;
                        case reshade::api::format::b5g6r5_unorm: format_str = "B5G6R5_UNORM"; break;
                        case reshade::api::format::b5g5r5a1_unorm: format_str = "B5G5R5A1_UNORM"; break;
                        case reshade::api::format::b4g4r4a4_unorm: format_str = "B4G4R4A4_UNORM"; break;
                        default: format_str = "Format_" + std::to_string(static_cast<int>(desc.texture.format)); break;
                    }
                }
            }
            
            // Get colorspace string
            std::string colorspace_str = "Unknown";
            switch (g_current_colorspace) {
                case reshade::api::color_space::unknown: colorspace_str = "Unknown"; break;
                case reshade::api::color_space::srgb_nonlinear: colorspace_str = "sRGB"; break;
                case reshade::api::color_space::extended_srgb_linear: colorspace_str = "Extended sRGB Linear"; break;
                case reshade::api::color_space::hdr10_st2084: colorspace_str = "HDR10 ST2084"; break;
                case reshade::api::color_space::hdr10_hlg: colorspace_str = "HDR10 HLG"; break;
                default: colorspace_str = "ColorSpace_" + std::to_string(static_cast<int>(g_current_colorspace)); break;
            }
            
            ImGui::Separator();
            ImGui::Text("DXGI Composition: %s | Backbuffer: %dx%d | Format: %s | Colorspace: %s",
                        mode_str,
                        g_last_backbuffer_width.load(),
                        g_last_backbuffer_height.load(),
                        format_str.c_str(),
                        colorspace_str.c_str());
            
            // Display HDR10 override status
            ImGui::Text("HDR10 Colorspace Override: %s (Last: %s)", g_hdr10_override_status.c_str(), g_hdr10_override_timestamp.c_str());
            
            // Display current window position and size
            if (hwnd != nullptr) {
                RECT wr{};
                RECT cr{};
                if (GetWindowRect(hwnd, &wr) != FALSE && GetClientRect(hwnd, &cr) != FALSE) {
                    int window_width = wr.right - wr.left;
                    int window_height = wr.bottom - wr.top;
                    int client_width = cr.right - cr.left;
                    int client_height = cr.bottom - cr.top;
                    
                    // Convert client coordinates to screen coordinates
                    POINT client_origin = {cr.left, cr.top};
                    ClientToScreen(hwnd, &client_origin);
                    
                    ImGui::Text("Window Frame: %dx%d at (%ld, %ld)", window_width, window_height, wr.left, wr.top);
                    ImGui::Text("Client Area: %dx%d at (%ld, %ld)", client_width, client_height, client_origin.x, client_origin.y);
                    ImGui::Text("Decorations: %dx%d offset", window_width - client_width, window_height - client_height);
                }
                
                // Display current monitor resolution
                HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                if (hmon != nullptr) {
                    MONITORINFOEXW mi{};
                    mi.cbSize = sizeof(mi);
                    if (GetMonitorInfoW(hmon, &mi) != FALSE) {
                        int monitor_width = mi.rcMonitor.right - mi.rcMonitor.left;
                        int monitor_height = mi.rcMonitor.bottom - mi.rcMonitor.top;
                        ImGui::Text("Monitor: %dx%d at (%ld, %ld)", 
                                   monitor_width, monitor_height, mi.rcMonitor.left, mi.rcMonitor.top);
                    }
                }
            }
            
            return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); } // Only show in Developer mode
    });

    // Independent Flip failure reasons (text only)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "IndependentFlipFailures",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "",
        .section = "Performance",
        .tooltip = "Reasons why Independent Flip is not working.",
        .on_draw = [](){
            IndependentFlipFailures* failures = g_if_failures.load();
            if (failures == nullptr) return false;
            
            // Check if there are any failures
            bool has_failures = failures->swapchain_null.load() ||
                               failures->device_null.load() ||
                               failures->non_dxgi_api.load() ||
                               failures->swapchain_media_failed.load() ||
                               failures->frame_stats_failed.load() ||
                               failures->not_flip_model.load() ||
                               failures->backbuffer_size_mismatch.load() ||
                               failures->window_size_mismatch.load() ||
                               failures->window_not_at_origin.load() ||
                               failures->window_layered.load() ||
                               failures->window_topmost.load() ||
                               failures->window_maximized.load() ||
                               failures->window_minimized.load() ||
                               failures->hwnd_null.load();
            
            if (has_failures) {
                ImGui::Separator();
                ImGui::Text("Independent Flip Debug Info:");
                
                if (failures->swapchain_null.load()) ImGui::BulletText("Swapchain is null");
                if (failures->device_null.load()) ImGui::BulletText("Device is null");
                if (failures->non_dxgi_api.load()) ImGui::BulletText("Non-DXGI API (OpenGL/Vulkan)");
                if (failures->swapchain_media_failed.load()) ImGui::BulletText("Swapchain media interface failed");
                if (failures->frame_stats_failed.load()) ImGui::BulletText("Frame statistics failed");
                if (failures->not_flip_model.load()) ImGui::BulletText("Not a flip-model swapchain");
                if (failures->backbuffer_size_mismatch.load()) ImGui::BulletText("Backbuffer size doesn't match monitor");
                if (failures->window_size_mismatch.load()) ImGui::BulletText("Window size doesn't match monitor");
                if (failures->window_not_at_origin.load()) ImGui::BulletText("Window not at monitor origin (0,0)");
                if (failures->window_layered.load()) ImGui::BulletText("Window has WS_EX_LAYERED style");
                if (failures->window_topmost.load()) ImGui::BulletText("Window has WS_EX_TOPMOST style");
                if (failures->window_maximized.load()) ImGui::BulletText("Window is maximized");
                if (failures->window_minimized.load()) ImGui::BulletText("Window is minimized");
                if (failures->hwnd_null.load()) ImGui::BulletText("Window handle is null");
            }
            
            return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); } // Only show in Developer mode
    });
}
