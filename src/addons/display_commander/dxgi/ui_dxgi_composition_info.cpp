#include "ui_dxgi_composition_info.hpp"
#include "../ui/ui_common.hpp"
#include "../../../utils/settings.hpp"
#include "../utils.hpp"
#include <windows.h>

namespace renodx::ui {

void AddDxgiCompositionInfoSettings(std::vector<renodx::utils::settings2::Setting*>& settings) {
    // DXGI Composition and Backbuffer Information
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "DxgiCompositionInfo",
        .value_type = renodx::utils::settings2::SettingValueType::CUSTOM,
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
}

} // namespace renodx::ui
