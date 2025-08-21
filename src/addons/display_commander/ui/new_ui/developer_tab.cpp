#include "developer_tab.hpp"
#include "../../addon.hpp"
#include "../../nvapi/nvapi_fullscreen_prevention.hpp"
#include "../../reflex/reflex_management.hpp"
#include "../ui_common.hpp"
#include "../../utils.hpp"
#include <sstream>
#include <iomanip>
#include <thread>

namespace renodx::ui::new_ui {

void DrawDeveloperTab() {
    ImGui::Text("Developer Tab - Advanced Features");
    ImGui::Separator();
    
    // Display settings section
    if (ImGui::CollapsingHeader("Display Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        DrawDisplayDeveloperSettings();
    }
    
    // NVAPI settings section
    if (ImGui::CollapsingHeader("NVAPI Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        DrawNvapiSettings();
    }
    
    // Reflex settings section
    if (ImGui::CollapsingHeader("Reflex Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        DrawReflexSettings();
    }
    
    // Latency display section
    if (ImGui::CollapsingHeader("Latency Information", ImGuiTreeNodeFlags_DefaultOpen)) {
        DrawLatencyDisplaySettings();
    }
    
    // HDR and colorspace section
    if (ImGui::CollapsingHeader("HDR & Colorspace", ImGuiTreeNodeFlags_DefaultOpen)) {
        DrawHdrColorspaceSettings();
    }
}

void DrawDisplayDeveloperSettings() {
    ImGui::Text("Display Control Settings");
    ImGui::Separator();
    
    // Prevent Fullscreen
    bool prevent_fullscreen = (s_prevent_fullscreen >= 0.5f);
    if (ImGui::Checkbox("Prevent Fullscreen", &prevent_fullscreen)) {
        s_prevent_fullscreen = prevent_fullscreen ? 1.0f : 0.0f;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Prevent exclusive fullscreen; keep borderless/windowed for stability and HDR.");
    }
    
    // Spoof Fullscreen State
    const char* spoof_fullscreen_labels[] = {"Disabled", "Spoof as Fullscreen", "Spoof as Windowed"};
    int spoof_fullscreen_state = static_cast<int>(s_spoof_fullscreen_state);
    if (ImGui::Combo("Spoof Fullscreen State", &spoof_fullscreen_state, spoof_fullscreen_labels, 3)) {
        s_spoof_fullscreen_state = static_cast<float>(spoof_fullscreen_state);
        
        // Log the change
        std::ostringstream oss;
        oss << "Fullscreen state spoofing changed to ";
        if (spoof_fullscreen_state < 1) {
            oss << "Disabled";
        } else if (spoof_fullscreen_state < 2) {
            oss << "Spoof as Fullscreen";
        } else {
            oss << "Spoof as Windowed";
        }
        LogInfo(oss.str().c_str());
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Spoof fullscreen state detection for applications that query fullscreen status. Useful for games that change behavior based on fullscreen state.");
    }
    
    // Spoof Window Focus
    const char* spoof_focus_labels[] = {"Disabled", "Spoof as Focused", "Spoof as Unfocused"};
    int spoof_focus_state = static_cast<int>(s_spoof_window_focus);
    if (ImGui::Combo("Spoof Window Focus", &spoof_focus_state, spoof_focus_labels, 3)) {
        s_spoof_window_focus = static_cast<float>(spoof_focus_state);
        
        // Log the change
        std::ostringstream oss;
        oss << "Window focus spoofing changed to ";
        if (spoof_focus_state < 1) {
            oss << "Disabled";
        } else if (spoof_focus_state < 2) {
            oss << "Spoof as Focused";
        } else {
            oss << "Spoof as Unfocused";
        }
        LogInfo(oss.str().c_str());
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Spoof window focus state for applications that query focus status. Useful for games that change behavior based on focus state.");
    }
}

void DrawNvapiSettings() {
    ImGui::Text("NVAPI Configuration");
    ImGui::Separator();
    
    // NVAPI Fullscreen Prevention
    bool nvapi_fullscreen_prevention = (s_nvapi_fullscreen_prevention >= 0.5f);
    if (ImGui::Checkbox("NVAPI Fullscreen Prevention", &nvapi_fullscreen_prevention)) {
        s_nvapi_fullscreen_prevention = nvapi_fullscreen_prevention ? 1.0f : 0.0f;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Use NVAPI to prevent fullscreen mode at the driver level.");
    }
    
    // NVAPI HDR Logging
    bool nvapi_hdr_logging = (s_nvapi_hdr_logging >= 0.5f);
    if (ImGui::Checkbox("NVAPI HDR Logging", &nvapi_hdr_logging)) {
        s_nvapi_hdr_logging = nvapi_hdr_logging ? 1.0f : 0.0f;
        
        if (nvapi_hdr_logging) {
            extern void RunBackgroundNvapiHdrMonitor();
            std::thread(::RunBackgroundNvapiHdrMonitor).detach();
        }
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable HDR monitor information logging via NVAPI.");
    }
    
    // NVAPI Debug Information Display
    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "NVAPI Debug Information:");
    
    extern NVAPIFullscreenPrevention g_nvapiFullscreenPrevention;
    
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
        
        // Function availability check
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
}

void DrawReflexSettings() {
    ImGui::Text("NVIDIA Reflex Configuration");
    ImGui::Separator();
    
    // Enable NVIDIA Reflex
    bool reflex_enabled = (s_reflex_enabled >= 0.5f);
    if (ImGui::Checkbox("Enable NVIDIA Reflex", &reflex_enabled)) {
        s_reflex_enabled = reflex_enabled ? 1.0f : 0.0f;
        
        if (reflex_enabled) {
            InstallReflexHooks();
        } else {
            UninstallReflexHooks();
        }
        // Mark that Reflex settings have changed to force sleep mode update
        extern std::atomic<bool> g_reflex_settings_changed;
        ::g_reflex_settings_changed.store(true);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable NVIDIA Reflex for reduced latency. Requires NVIDIA GPU and drivers.");
    }
    
    if (reflex_enabled) {
        ImGui::Indent();
        
        // Reflex Low Latency Mode (disabled)
        bool low_latency_mode = (s_reflex_low_latency_mode >= 0.5f);
        ImGui::BeginDisabled();
        if (ImGui::Checkbox("Reflex Low Latency Mode", &low_latency_mode)) {
            s_reflex_low_latency_mode = low_latency_mode ? 1.0f : 0.0f;
            extern std::atomic<bool> g_reflex_settings_changed;
            ::g_reflex_settings_changed.store(true);
        }
        ImGui::EndDisabled();
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Enable low latency mode for reduced input lag. (Currently disabled)");
        }
        
        // Reflex Low Latency Boost (disabled)
        bool low_latency_boost = (s_reflex_low_latency_boost >= 0.5f);
        ImGui::BeginDisabled();
        if (ImGui::Checkbox("Reflex Low Latency Boost", &low_latency_boost)) {
            s_reflex_low_latency_boost = low_latency_boost ? 1.0f : 0.0f;
            extern std::atomic<bool> g_reflex_settings_changed;
            ::g_reflex_settings_changed.store(true);
        }
        ImGui::EndDisabled();
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Request maximum GPU clock frequency for lower latency in CPU-limited scenarios. (Currently disabled)");
        }
        
        // Reflex Use Markers (disabled)
        bool use_markers = (s_reflex_use_markers >= 0.5f);
        ImGui::BeginDisabled();
        if (ImGui::Checkbox("Reflex Use Markers", &use_markers)) {
            s_reflex_use_markers = use_markers ? 1.0f : 0.0f;
            extern std::atomic<bool> g_reflex_settings_changed;
            ::g_reflex_settings_changed.store(true);
        }
        ImGui::EndDisabled();
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Allow latency markers to be used for runtime optimizations. (Currently disabled)");
        }
        
        // Reflex Debug Output
        bool debug_output = (s_reflex_debug_output >= 0.5f);
        if (ImGui::Checkbox("Reflex Debug Output", &debug_output)) {
            s_reflex_debug_output = debug_output ? 1.0f : 0.0f;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Enable or disable Reflex debug messages in the log.");
        }
        
        ImGui::Unindent();
    }
}

void DrawLatencyDisplaySettings() {
    ImGui::Text("Real-time Latency Information");
    ImGui::Separator();
    
    // Current Latency Display
    extern std::atomic<float> g_current_latency_ms;
    float latency = ::g_current_latency_ms.load();
    std::ostringstream oss;
    oss << "Current Latency: " << std::fixed << std::setprecision(2) << latency << " ms";
    ImGui::TextUnformatted(oss.str().c_str());
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Current frame latency in milliseconds");
    }
    
    // PCL AV Latency Display (Most Important)
    extern std::atomic<float> g_pcl_av_latency_ms;
    float pcl_latency = ::g_pcl_av_latency_ms.load();
    oss.str("");
    oss.clear();
    oss << "PCL AV Latency: " << std::fixed << std::setprecision(2) << pcl_latency << " ms";
    ImGui::TextUnformatted(oss.str().c_str());
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "(30-frame avg)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("PCL Average Latency (30-frame average) - This is what the NVIDIA overlay should show");
    }
    
    // Reflex Status Display
    extern std::atomic<bool> g_reflex_active;
    extern std::atomic<uint64_t> g_current_frame;
    bool is_active = ::g_reflex_active.load();
    uint64_t frame = ::g_current_frame.load();
    
    oss.str("");
    oss.clear();
    if (is_active) {
        oss << "Reflex Status: Active (Frame " << frame << ")";
    } else {
        oss << "Reflex Status: Inactive";
    }
    ImGui::TextUnformatted(oss.str().c_str());
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Current Reflex status and configuration");
    }
    
    ImGui::Separator();
    
    // Debug Button to show atomic variable values
    if (ImGui::Button("Log All Latency Values")) {
        extern std::atomic<float> g_average_latency_ms;
        extern std::atomic<float> g_min_latency_ms;
        extern std::atomic<float> g_max_latency_ms;
        
        std::ostringstream debug_oss;
        debug_oss << "=== LATENCY DEBUG INFO ===\n";
        debug_oss << "Current Latency: " << std::fixed << std::setprecision(2) << ::g_current_latency_ms.load() << " ms\n";
        debug_oss << "PCL AV Latency: " << std::fixed << std::setprecision(2) << ::g_pcl_av_latency_ms.load() << " ms\n";
        debug_oss << "Average Latency: " << std::fixed << std::setprecision(2) << ::g_average_latency_ms.load() << " ms\n";
        debug_oss << "Min Latency: " << std::fixed << std::setprecision(2) << ::g_min_latency_ms.load() << " ms\n";
        debug_oss << "Max Latency: " << std::fixed << std::setprecision(2) << ::g_max_latency_ms.load() << " ms\n";
        debug_oss << "Current Frame: " << ::g_current_frame.load() << "\n";
        debug_oss << "Reflex Active: " << (::g_reflex_active.load() ? "Yes" : "No") << "\n";
        debug_oss << "========================";
        
        extern void LogInfo(const char* message);
        ::LogInfo(debug_oss.str().c_str());
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Click to log all current latency values for debugging");
    }
}

void DrawHdrColorspaceSettings() {
    ImGui::Text("HDR and Colorspace Configuration");
    ImGui::Separator();
    
    // HDR10 Colorspace Fix
    bool fix_hdr10_colorspace = (s_fix_hdr10_colorspace >= 0.5f);
    if (ImGui::Checkbox("Fix NVAPI HDR10 Colorspace for reshade addon", &fix_hdr10_colorspace)) {
        s_fix_hdr10_colorspace = fix_hdr10_colorspace ? 1.0f : 0.0f;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Automatically fix HDR10 colorspace when swapchain format is RGB10A2 and colorspace is currently sRGB. Only works when the game is using sRGB colorspace.");
    }
}

} // namespace renodx::ui::new_ui
