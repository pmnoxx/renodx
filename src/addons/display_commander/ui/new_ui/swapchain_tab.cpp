#include "swapchain_tab.hpp"
#include "../../addon.hpp"
#include "../../dxgi/dxgi_device_info.hpp"
#include "../../utils.hpp"
#include <deps/imgui/imgui.h>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace renodx::ui::new_ui {

void DrawSwapchainTab() {
    ImGui::Text("Swapchain Tab - DXGI Information");
    ImGui::Separator();
    
    // Draw all swapchain-related sections
    DrawSwapchainInfo();
    ImGui::Spacing();
    DrawAdapterInfo();
    ImGui::Spacing();
    DrawDxgiCompositionInfo();
}

void DrawSwapchainInfo() {
    if (ImGui::CollapsingHeader("Swapchain Information", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Get current swapchain info from ReShade
        extern std::atomic<reshade::api::swapchain*> g_last_swapchain_ptr;
        extern reshade::api::color_space g_current_colorspace;
        
        auto* swapchain = ::g_last_swapchain_ptr.load();
        if (swapchain) {
            // Colorspace information
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Current Colorspace:");
            switch (::g_current_colorspace) {
                case reshade::api::color_space::srgb_nonlinear:
                    ImGui::Text("  sRGB (Non-Linear)");
                    break;
                case reshade::api::color_space::extended_srgb_linear:
                    ImGui::Text("  Extended sRGB (Linear)");
                    break;
                case reshade::api::color_space::hdr10_st2084:
                    ImGui::Text("  HDR10 (ST2084/PQ)");
                    break;
                case reshade::api::color_space::hdr10_hlg:
                    ImGui::Text("  HDR10 (HLG)");
                    break;
                default:
                    ImGui::Text("  Unknown (%d)", static_cast<int>(::g_current_colorspace));
                    break;
            }

            // Backbuffer information
            if (swapchain->get_back_buffer_count() > 0) {
                auto bb = swapchain->get_back_buffer(0);
                auto* device = swapchain->get_device();
                if (device) {
                    auto desc = device->get_resource_desc(bb);
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Backbuffer:");
                    ImGui::Text("  Resolution: %dx%d", desc.texture.width, desc.texture.height);
                    ImGui::Text("  Format: %s", 
                        desc.texture.format == reshade::api::format::r8g8b8a8_unorm ? "R8G8B8A8_UNORM" :
                        desc.texture.format == reshade::api::format::r10g10b10a2_unorm ? "R10G10B10A2_UNORM" :
                        desc.texture.format == reshade::api::format::r16g16b16a16_float ? "R16G16B16A16_FLOAT" :
                        "Other");
                    ImGui::Text("  Backbuffer Count: %d", swapchain->get_back_buffer_count());
                }
            }

            // Window information
            HWND hwnd = static_cast<HWND>(swapchain->get_hwnd());
            if (hwnd) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Window:");
                RECT rect;
                if (GetWindowRect(hwnd, &rect)) {
                    ImGui::Text("  Position: (%ld, %ld) to (%ld, %ld)", rect.left, rect.top, rect.right, rect.bottom);
                    ImGui::Text("  Size: %ldx%ld", rect.right - rect.left, rect.bottom - rect.top);
                }
                
                // Window state
                LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
                ImGui::Text("  Fullscreen: %s", (style & WS_POPUP) ? "Yes" : "No");
                ImGui::Text("  Borderless: %s", (style & WS_POPUP) && !(style & WS_CAPTION) ? "Yes" : "No");
            }
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No active swapchain available");
        }
    }
}

void DrawAdapterInfo() {
    if (ImGui::CollapsingHeader("Adapter Information", ImGuiTreeNodeFlags_DefaultOpen)) {
        extern std::unique_ptr<DXGIDeviceInfoManager> g_dxgiDeviceInfoManager;
        
        if (::g_dxgiDeviceInfoManager && ::g_dxgiDeviceInfoManager->IsInitialized()) {
            const auto& adapters = ::g_dxgiDeviceInfoManager->GetAdapters();
            
            for (size_t i = 0; i < adapters.size(); ++i) {
                const auto& adapter = adapters[i];
                
                // Adapter header
                std::string adapter_title = adapter.name + " - " + adapter.description;
                if (ImGui::TreeNodeEx(adapter_title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                    
                    // Adapter details
                    ImGui::Text("Description: %s", adapter.description.c_str());
                    ImGui::Text("Dedicated Video Memory: %.1f GB", adapter.dedicated_video_memory / (1024.0 * 1024.0 * 1024.0));
                    ImGui::Text("Dedicated System Memory: %.1f GB", adapter.dedicated_system_memory / (1024.0 * 1024.0 * 1024.0));
                    ImGui::Text("Shared System Memory: %.1f GB", adapter.shared_system_memory / (1024.0 * 1024.0 * 1024.0));
                    ImGui::Text("Software Adapter: %s", adapter.is_software ? "Yes" : "No");
                    
                    // LUID info
                    std::ostringstream luid_oss;
                    luid_oss << "Adapter LUID: 0x" << std::hex << adapter.adapter_luid.HighPart << "_" << adapter.adapter_luid.LowPart;
                    ImGui::Text("%s", luid_oss.str().c_str());

                    // Outputs
                    if (!adapter.outputs.empty()) {
                        ImGui::Separator();
                        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Outputs (%zu):", adapter.outputs.size());
                        
                        for (size_t j = 0; j < adapter.outputs.size(); ++j) {
                            const auto& output = adapter.outputs[j];
                            std::string output_title = "Output " + std::to_string(j) + " - " + output.device_name;
                            
                            if (ImGui::TreeNodeEx(output_title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                                
                                // Basic output info
                                ImGui::Text("Device Name: %s", output.device_name.c_str());
                                ImGui::Text("Monitor Name: %s", output.monitor_name.c_str());
                                ImGui::Text("Attached: %s", output.is_attached ? "Yes" : "No");
                                ImGui::Text("Desktop Coordinates: (%ld, %ld) to (%ld, %ld)", 
                                           output.desktop_coordinates.left, output.desktop_coordinates.top,
                                           output.desktop_coordinates.right, output.desktop_coordinates.bottom);
                                
                                // Resolution info
                                if (output.supported_modes.size() > 0) {
                                    ImGui::Separator();
                                    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Supported Resolutions (%zu):", output.supported_modes.size());
                                    
                                    // Show first few resolutions
                                    size_t max_show = (std::min)(output.supported_modes.size(), size_t(5));
                                    for (size_t k = 0; k < max_show; ++k) {
                                        const auto& mode = output.supported_modes[k];
                                        ImGui::Text("  %ux%u", mode.Width, mode.Height);
                                    }
                                    
                                    if (output.supported_modes.size() > max_show) {
                                        ImGui::Text("  ... and %zu more", output.supported_modes.size() - max_show);
                                    }
                                }
                                
                                ImGui::TreePop();
                            }
                        }
                    }
                    
                    ImGui::TreePop();
                }
            }
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "DXGI Device Info Manager not initialized");
        }
    }
}

void DrawDxgiCompositionInfo() {
    if (ImGui::CollapsingHeader("DXGI Composition Information", ImGuiTreeNodeFlags_DefaultOpen)) {
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
        
        ImGui::Text("DXGI Composition: %s", mode_str);
        ImGui::Text("Backbuffer: %dx%d", g_last_backbuffer_width.load(), g_last_backbuffer_height.load());
        ImGui::Text("Format: %s", format_str.c_str());
        ImGui::Text("Colorspace: %s", colorspace_str.c_str());
        
        // Display HDR10 override status
        ImGui::Text("HDR10 Colorspace Override: %s (Last: %s)", g_hdr10_override_status.c_str(), g_hdr10_override_timestamp.c_str());
    }
}

} // namespace renodx::ui::new_ui
