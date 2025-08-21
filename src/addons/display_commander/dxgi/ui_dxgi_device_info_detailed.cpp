#include "ui_dxgi_device_info_detailed.hpp"
#include "../ui/ui_common.hpp"
#include "../renodx/settings.hpp"
#include "dxgi_device_info.hpp"
#include "../utils.hpp"

namespace renodx::ui {

void AddDxgiDeviceInfoDetailedSettings(std::vector<renodx::utils::settings2::Setting*>& settings) {
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
        .is_visible = []() { return is_device_tab(); } // Only show in Device mode
    });
}

} // namespace renodx::ui
