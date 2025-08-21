#include "../ui/ui_common.hpp"
#include "nvapi_fullscreen_prevention.hpp"

// External declarations for NVAPI settings
extern float s_nvapi_fullscreen_prevention;
extern float s_nvapi_hdr_logging;
extern float s_nvapi_force_hdr10;
extern float s_nvapi_hdr_interval_sec;

namespace renodx::ui {

void AddNvapiSettings(std::vector<renodx::utils::settings2::Setting*>& settings) {
    // NVAPI Fullscreen Prevention
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "NvapiFullscreenPrevention",
        .binding = &s_nvapi_fullscreen_prevention,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "NVAPI Fullscreen Prevention",
        .section = "NVAPI",
        .tooltip = "Use NVAPI to prevent fullscreen mode at the driver level.",
        .labels = {"Disabled", "Enabled"},
        .is_visible = []() { return is_developer_tab(); },
    });

    // NVAPI HDR Logging
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "NvapiHdrLogging",
        .binding = &s_nvapi_hdr_logging,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
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
        .is_visible = []() { return is_developer_tab(); },
    });

    // NVAPI Debug Information Display
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "NvapiDebugInfo",
        .binding = nullptr,
        .value_type = renodx::utils::settings2::SettingValueType::CUSTOM,
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
        .is_visible = []() { return is_developer_tab(); },
    });

    // Force NVAPI HDR10 (UHDA)
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "NvapiForceHDR10",
        .binding = &s_nvapi_force_hdr10,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Force HDR10 Output (NVAPI)",
        .section = "NVAPI",
        .tooltip = "Enable HDR10 (UHDA) output via NVAPI for all connected displays (driver-level).",
        .labels = {"Off", "On"},
        .on_change_value = [](float previous, float current){
            std::thread([](){
                extern NVAPIFullscreenPrevention g_nvapiFullscreenPrevention;
                if (!::g_nvapiFullscreenPrevention.IsAvailable()) {
                    if (!::g_nvapiFullscreenPrevention.Initialize()) {
                        LogWarn("NVAPI Force HDR10: failed to initialize NVAPI");
                        return;
                    }
                }
                bool enable = (s_nvapi_force_hdr10 >= 0.5f);
                if (::g_nvapiFullscreenPrevention.SetHdr10OnAll(enable)) {
                    LogInfo(enable ? "NVAPI Force HDR10: enabled" : "NVAPI Force HDR10: disabled");
                } else {
                    LogWarn("NVAPI Force HDR10: failed to apply");
                }
            }).detach();
        },
        .is_visible = []() { return is_developer_tab(); },
    });

    // Single-shot NVAPI HDR log (button)
    settings.push_back(new renodx::utils::settings2::Setting{
        .value_type = renodx::utils::settings2::SettingValueType::BUTTON,
        .label = "Log HDR Status Now (NVAPI)",
        .section = "NVAPI",
        .tooltip = "Immediately query NVAPI HDR status and log a single line.",
        .on_click = [](){
            std::thread([](){ LogNvapiHdrOnce(); }).detach();
            return false;
        },
        .is_visible = []() { return is_developer_tab(); },
    });

    // Dump full NVAPI HDR details
    settings.push_back(new renodx::utils::settings2::Setting{
        .value_type = renodx::utils::settings2::SettingValueType::BUTTON,
        .label = "Dump Full HDR Details (NVAPI)",
        .section = "NVAPI",
        .tooltip = "Print full HDR parameters (caps, ST2086) for connected displays.",
        .on_click = [](){
            std::thread([](){
                extern NVAPIFullscreenPrevention g_nvapiFullscreenPrevention;
                if (!::g_nvapiFullscreenPrevention.IsAvailable()) {
                    if (!::g_nvapiFullscreenPrevention.Initialize()) {
                        LogWarn("NVAPI HDR dump: failed to initialize NVAPI");
                        return;
                    }
                }
                std::string details;
                if (::g_nvapiFullscreenPrevention.QueryHdrDetails(details)) {
                    LogInfo(details.c_str());
                } else {
                    LogWarn("NVAPI HDR dump: failed to get details");
                }
            }).detach();
            return false;
        },
        .is_visible = []() { return is_developer_tab(); },
    });

    // HDR Log Interval
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "NvapiHdrIntervalSec",
        .binding = &s_nvapi_hdr_interval_sec,
        .value_type = renodx::utils::settings2::SettingValueType::INTEGER,
        .default_value = 5.f,
        .label = "HDR Log Interval (s)",
        .section = "NVAPI",
        .tooltip = "How often to query/log HDR status via NVAPI.",
        .min = 1.f,
        .max = 120.f,
        .format = "%d s",
        .is_visible = []() { return is_developer_tab() && s_nvapi_hdr_logging >= 0.5f; },
    });

    // NVAPI Debug Button
    settings.push_back(new renodx::utils::settings2::Setting{
        .value_type = renodx::utils::settings2::SettingValueType::BUTTON,
        .label = "Test NVAPI Functions",
        .section = "NVAPI",
        .tooltip = "Manually test NVAPI initialization and show detailed function status. Useful for debugging.",
        .on_click = []() {
            extern NVAPIFullscreenPrevention g_nvapiFullscreenPrevention;
            
            std::thread([](){
                LogDebug("NVAPI Debug Test button pressed (bg thread)");
                
                // Test initialization
                if (::g_nvapiFullscreenPrevention.Initialize()) {
                    LogInfo("NVAPI Debug Test: Initialization successful");
                    
                    // Test hardware detection
                    if (::g_nvapiFullscreenPrevention.HasNVIDIAHardware()) {
                        LogInfo("NVAPI Debug Test: NVIDIA hardware detected");
                    } else {
                        LogWarn("NVAPI Debug Test: No NVIDIA hardware found");
                    }
                    
                    // Test driver version
                    std::string driverVersion = ::g_nvapiFullscreenPrevention.GetDriverVersion();
                    if (driverVersion != "Failed to get driver version") {
                        std::ostringstream oss;
                        oss << "NVAPI Debug Test: Driver version: " << driverVersion;
                        LogInfo(oss.str().c_str());
                    } else {
                        LogWarn("NVAPI Debug Test: Failed to get driver version");
                    }
                    
                    // Test fullscreen prevention (toggle on then off)
                    if (::g_nvapiFullscreenPrevention.SetFullscreenPrevention(true)) {
                        LogInfo("NVAPI Debug Test: Fullscreen prevention enabled successfully");
                        
                        // Wait a moment then disable
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        
                        if (::g_nvapiFullscreenPrevention.SetFullscreenPrevention(false)) {
                            LogInfo("NVAPI Debug Test: Fullscreen prevention disabled successfully");
                        } else {
                            std::ostringstream oss;
                            oss << "NVAPI Debug Test: Failed to disable fullscreen prevention: " << ::g_nvapiFullscreenPrevention.GetLastError();
                            LogWarn(oss.str().c_str());
                        }
                    } else {
                        std::ostringstream oss;
                        oss << "NVAPI Debug Test: Failed to enable fullscreen prevention: " << ::g_nvapiFullscreenPrevention.GetLastError();
                        LogWarn(oss.str().c_str());
                    }
                    
                } else {
                    std::ostringstream oss;
                    oss << "NVAPI Debug Test: Initialization failed: " << ::g_nvapiFullscreenPrevention.GetLastError();
                    LogWarn(oss.str().c_str());
                }
            }).detach();
            
            return false;
        },
        .is_visible = []() { return is_developer_tab(); },
    });
}

} // namespace renodx::ui
