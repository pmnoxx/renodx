#include "../ui/ui_common.hpp"
#include "dxgi_device_info.hpp"
#include <sstream>
#include <iomanip>

namespace renodx::ui {

void AddDxgiDeviceInfoSettings(std::vector<renodx::utils::settings2::Setting*>& settings) {
    // DXGI Device Information Tab
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "DxgiDeviceInfo",
        .value_type = renodx::utils::settings2::SettingValueType::CUSTOM,
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
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Reset HDR metadata for HDR10 displays");

            // Colorspace selector
            ImGui::SameLine();
            if (ImGui::Button("Set Colorspace")) {
                // Colorspace selection will be handled in the dropdown below
            }
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.8f, 1.0f, 0.8f, 1.0f), "Set swapchain colorspace");
            
            // Colorspace dropdown
            static int selected_colorspace = 0;
            const char* colorspace_names[] = {
                "sRGB (Non-Linear)",
                "Extended sRGB (Linear)", 
                "HDR10 (ST2084/PQ)",
                "HDR10 (HLG)"
            };
            const reshade::api::color_space colorspace_values[] = {
                reshade::api::color_space::srgb_nonlinear,
                reshade::api::color_space::extended_srgb_linear,
                reshade::api::color_space::hdr10_st2084,
                reshade::api::color_space::hdr10_hlg
            };
            
            if (ImGui::Combo("Colorspace", &selected_colorspace, colorspace_names, IM_ARRAYSIZE(colorspace_names))) {
                // Apply the selected colorspace
                if (g_dxgiDeviceInfoManager && g_dxgiDeviceInfoManager->IsInitialized()) {
                    bool success = g_dxgiDeviceInfoManager->SetColorspace(colorspace_values[selected_colorspace]);
                    
                    if (success) {
                        LogInfo(("Colorspace changed to: " + std::string(colorspace_names[selected_colorspace])).c_str());
                    } else {
                        LogWarn(("Failed to change colorspace to: " + std::string(colorspace_names[selected_colorspace])).c_str());
                    }
                }
            }

            ImGui::Separator();

            // Display adapter information
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
                                
                                // Resolution and refresh rate
                                int width = output.desktop_coordinates.right - output.desktop_coordinates.left;
                                int height = output.desktop_coordinates.bottom - output.desktop_coordinates.top;
                                ImGui::Text("Resolution: %dx%d", width, height);
                                
                                if (output.refresh_rate.Denominator > 0) {
                                    float refresh = static_cast<float>(output.refresh_rate.Numerator) / static_cast<float>(output.refresh_rate.Denominator);
                                    ImGui::Text("Refresh Rate: %.2f Hz", refresh);
                                }

                                // HDR information
                                if (output.supports_hdr10) {
                                    ImGui::Separator();
                                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "HDR10 Support: ✓ Enabled");
                                    ImGui::Text("Max Luminance: %.1f nits", output.max_luminance);
                                    ImGui::Text("Min Luminance: %.1f nits", output.min_luminance);
                                    ImGui::Text("Max Frame Average Light Level: %.1f nits", output.max_frame_average_light_level);
                                    ImGui::Text("Max Content Light Level: %.1f nits", output.max_content_light_level);
                                } else {
                                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.0f, 1.0f), "HDR10 Support: ✗ Not Supported");
                                }

                                // Color space information
                                ImGui::Separator();
                                ImGui::Text("Color Space: %s", 
                                           output.color_space == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 ? "HDR10" :
                                           output.color_space == DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709 ? "sRGB" :
                                           "Other");
                                ImGui::Text("Wide Color Gamut: %s", output.supports_wide_color_gamut ? "Yes" : "No");

                                // Supported modes count
                                if (!output.supported_modes.empty()) {
                                    ImGui::Text("Supported Modes: %zu", output.supported_modes.size());
                                }

                                ImGui::TreePop();
                            }
                        }
                    }

                    ImGui::TreePop();
                }
            }

            return false;
        },
        .is_visible = []() { return is_device_tab(s_ui_tab); }, // Show only in Device Info mode
    });
}

} // namespace renodx::ui
