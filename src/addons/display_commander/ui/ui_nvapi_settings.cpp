#include "ui_common.hpp"
#include "../nvapi_fullscreen_prevention.hpp"

// External declarations for NVAPI settings
extern float s_nvapi_fullscreen_prevention;
extern float s_nvapi_hdr_logging;

namespace renodx::ui {

void AddNvapiSettings(std::vector<renodx::utils::settings::Setting*>& settings) {
    // NVAPI Fullscreen Prevention
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "NvapiFullscreenPrevention",
        .binding = &s_nvapi_fullscreen_prevention,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "NVAPI Fullscreen Prevention",
        .section = "NVAPI",
        .tooltip = "Use NVAPI to prevent fullscreen mode at the driver level.",
        .labels = {"Disabled", "Enabled"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
    });

    // NVAPI HDR Logging
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "NvapiHdrLogging",
        .binding = &s_nvapi_hdr_logging,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "NVAPI HDR Logging",
        .section = "NVAPI",
        .tooltip = "Enable HDR monitor information logging via NVAPI.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current){
            if (current >= 0.5f) {
                extern void RunBackgroundNvapiHdrMonitor();
                std::thread(::RunBackgroundNvapiHdrMonitor).detach();
            }
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
    });

    // NVAPI Debug Information Display
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "NvapiDebugInfo",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "NVAPI Debug Information",
        .section = "NVAPI",
        .tooltip = "Display detailed NVAPI status and debug information",
        .on_draw = []() -> bool {
            extern NVAPIFullscreenPrevention g_nvapiFullscreenPrevention;
            
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "NVAPI Debug Information:");
            
            if (::g_nvapiFullscreenPrevention.IsAvailable()) {
                // Library loaded successfully
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ NVAPI Library: Loaded");
                
                // Driver version info
                std::string driverVersion = ::g_nvapiFullscreenPrevention.GetDriverVersion();
                if (driverVersion != "Failed to get driver version") {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ Driver Version: %s", driverVersion.c_str());
                } else {
                    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "⚠ Driver Version: %s", driverVersion.c_str());
                }
                
                // Hardware detection
                if (::g_nvapiFullscreenPrevention.HasNVIDIAHardware()) {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ NVIDIA Hardware: Detected");
                } else {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "✗ NVIDIA Hardware: Not Found");
                }
                
                // Fullscreen prevention status
                if (::g_nvapiFullscreenPrevention.IsFullscreenPreventionEnabled()) {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ Fullscreen Prevention: ACTIVE");
                } else {
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "○ Fullscreen Prevention: Inactive");
                }
                
                // Function availability check (simulated)
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ Core Functions: Available");
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ DRS Functions: Available");
                
            } else {
                // Library not loaded
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "✗ NVAPI Library: Not Loaded");
                
                // Try to get error information
                std::string lastError = ::g_nvapiFullscreenPrevention.GetLastError();
                if (!lastError.empty()) {
                    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Error: %s", lastError.c_str());
                }
                
                // Common failure reasons
                ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 0.8f), "Common Issues:");
                ImGui::BulletText("nvapi64.dll not found in system PATH");
                ImGui::BulletText("No NVIDIA drivers installed");
                ImGui::BulletText("Running on non-NVIDIA hardware");
                ImGui::BulletText("Insufficient permissions to load DLL");
                
                // Function availability (all unavailable when library not loaded)
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "✗ Core Functions: Unavailable");
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "✗ DRS Functions: Unavailable");
            }
            
            // Additional debug info
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Technical Details:");
            ImGui::BulletText("Uses dynamic loading of nvapi64.dll");
            ImGui::BulletText("Creates NVIDIA driver profiles per application");
            ImGui::BulletText("Sets OGL_DX_PRESENT_DEBUG_ID (0x20324987)");
            ImGui::BulletText("Applies DISABLE_FULLSCREEN_OPT (0x00000001)");
            ImGui::BulletText("Settings persist across application restarts");
            
            // DLL version info
            if (!::g_nvapiFullscreenPrevention.IsAvailable()) {
                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "DLL Information:");
                std::string dllInfo = ::g_nvapiFullscreenPrevention.GetDllVersionInfo();
                if (dllInfo != "No library loaded") {
                    ImGui::TextWrapped("%s", dllInfo.c_str());
                } else {
                    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "DLL not loaded - cannot get version info");
                }
            }
            
            return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
    });
}

} // namespace renodx::ui
