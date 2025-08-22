#include "main_new_tab.hpp"
#include "main_new_tab_settings.hpp"
#include "../../addon.hpp"
#include "../../dxgi/custom_fps_limiter.hpp"
#include "../../dxgi/custom_fps_limiter_manager.hpp"
#include "../ui_display_tab.hpp"
#include <sstream>
#include <thread>
#include <atomic>

// Global variable declaration
extern std::unique_ptr<renodx::dxgi::fps_limiter::CustomFpsLimiterManager> g_customFpsLimiterManager;

// External constants for width and height options (unused here)

namespace renodx::ui::new_ui {

void DrawMainNewTab() {
    // Load saved settings once and sync legacy globals
    static bool settings_loaded_once = false;
    if (!settings_loaded_once) {
        g_main_new_tab_settings.LoadSettings();
        {
            int idx = g_main_new_tab_settings.window_width.GetValue();
            idx = (std::max)(idx, 0);
            int max_idx = 7;
            idx = (std::min)(idx, max_idx);
            s_windowed_width = (idx == 0) ? static_cast<float>(GetCurrentMonitorWidth())
                                          : static_cast<float>(WIDTH_OPTIONS[idx]);
        }
        {
            int idx = g_main_new_tab_settings.window_height.GetValue();
            idx = (std::max)(idx, 0);
            int max_idx = 7;
            idx = (std::min)(idx, max_idx);
            s_windowed_height = (idx == 0) ? static_cast<float>(GetCurrentMonitorHeight())
                                           : static_cast<float>(HEIGHT_OPTIONS[idx]);
        }
        s_resize_mode = static_cast<float>(g_main_new_tab_settings.resize_mode.GetValue());
        s_aspect_index = static_cast<float>(g_main_new_tab_settings.aspect_index.GetValue());
        s_target_monitor_index = static_cast<float>(g_main_new_tab_settings.target_monitor_index.GetValue());
        s_remove_top_bar = g_main_new_tab_settings.remove_top_bar.GetValue() ? 1.0f : 0.0f;
        s_move_to_zero_if_out = static_cast<float>(g_main_new_tab_settings.alignment.GetValue());
        s_fps_limit = g_main_new_tab_settings.fps_limit.GetValue();
        s_fps_limit_background = g_main_new_tab_settings.fps_limit_background.GetValue();
        s_audio_volume_percent = g_main_new_tab_settings.audio_volume_percent.GetValue();
        s_audio_mute = g_main_new_tab_settings.audio_mute.GetValue() ? 1.0f : 0.0f;
        s_mute_in_background = g_main_new_tab_settings.mute_in_background.GetValue() ? 1.0f : 0.0f;
        s_reflex_enabled = g_main_new_tab_settings.reflex_enabled.GetValue() ? 1.0f : 0.0f;
        settings_loaded_once = true;
    }
    ImGui::Text("Main Tab - Basic Settings");
    ImGui::Separator();
    
    // Display Settings Section
    DrawDisplaySettings();
    
    ImGui::Spacing();
    ImGui::Separator();
    
    // Monitor/Display Resolution Settings Section
    DrawMonitorDisplaySettings();
    
    ImGui::Spacing();
    ImGui::Separator();
    
    // Audio Settings Section
    DrawAudioSettings();
    
    ImGui::Spacing();
    ImGui::Separator();
    
    // Window Controls Section
    DrawWindowControls();
    
    ImGui::Spacing();
    ImGui::Separator();
    
    // Basic Reflex Settings Section
    DrawBasicReflexSettings();
}

void DrawDisplaySettings() {
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "=== Display Settings ===");
    
    // Window Width dropdown (with persistent setting)
    if (ComboSettingWrapper(g_main_new_tab_settings.window_width, "Window Width")) {
        int idx = g_main_new_tab_settings.window_width.GetValue();
        idx = (std::max)(idx, 0);
        int max_idx = 7;
        idx = (std::min)(idx, max_idx);
        s_windowed_width = (idx == 0) ? static_cast<float>(GetCurrentMonitorWidth())
                                      : static_cast<float>(WIDTH_OPTIONS[idx]);
        LogInfo("Window width changed");
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Choose the window width. 'Current Display' uses the current monitor's width.");
    }
    
    // Window Height dropdown (disabled when in aspect ratio mode)
    bool height_enabled = (s_resize_mode < 0.5f);
    if (!height_enabled) {
        ImGui::BeginDisabled();
    }
    if (ComboSettingWrapper(g_main_new_tab_settings.window_height, "Window Height")) {
        int idx = g_main_new_tab_settings.window_height.GetValue();
        idx = (std::max)(idx, 0);
        int max_idx = 7;
        idx = (std::min)(idx, max_idx);
        s_windowed_height = (idx == 0) ? static_cast<float>(GetCurrentMonitorHeight())
                                       : static_cast<float>(HEIGHT_OPTIONS[idx]);
        LogInfo("Window height changed");
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Choose the window height. 'Current Display' uses the current monitor's height. Disabled when using Aspect Ratio mode.");
    }
    if (!height_enabled) {
        ImGui::EndDisabled();
    }
    
    // Resize Mode combo (persistent)
    if (ComboSettingWrapper(g_main_new_tab_settings.resize_mode, "Resize Mode")) {
        s_resize_mode = static_cast<float>(g_main_new_tab_settings.resize_mode.GetValue());
        LogInfo("Resize mode changed");
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Choose between manual width/height or aspect ratio-based resizing.");
    }
    
    // Aspect Ratio dropdown (only visible when in Aspect mode)
    if (s_resize_mode >= 0.5f) {
        if (ComboSettingWrapper(g_main_new_tab_settings.aspect_index, "Aspect Ratio")) {
            s_aspect_index = static_cast<float>(g_main_new_tab_settings.aspect_index.GetValue());
            LogInfo("Aspect ratio changed");
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Choose the aspect ratio for window resizing. Only applies when Resize Mode is set to Aspect Ratio.");
        }
    }
    
    // Target Monitor dropdown
    std::vector<std::string> monitor_labels = MakeMonitorLabels();
    std::vector<const char*> monitor_c_labels;
    monitor_c_labels.reserve(monitor_labels.size());
    for (const auto& label : monitor_labels) {
        monitor_c_labels.push_back(label.c_str());
    }
    int monitor_index = static_cast<int>(s_target_monitor_index);
    if (ImGui::Combo("Target Monitor", &monitor_index, monitor_c_labels.data(), static_cast<int>(monitor_c_labels.size()))) {
        s_target_monitor_index = static_cast<float>(monitor_index);
        g_main_new_tab_settings.target_monitor_index.SetValue(monitor_index);
        LogInfo("Target monitor changed");
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Choose which monitor to apply size/pos to. 'Auto' uses the current window monitor.");
    }
    
    // Remove Top Bar checkbox
    if (CheckboxSetting(g_main_new_tab_settings.remove_top_bar, "Remove Top Bar")) {
        s_remove_top_bar = g_main_new_tab_settings.remove_top_bar.GetValue() ? 1.0f : 0.0f;
        LogInfo("Remove top bar setting changed");
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Remove the window title bar for a borderless appearance.");
    }
    
    // Window Alignment dropdown
    if (ComboSettingWrapper(g_main_new_tab_settings.alignment, "Alignment")) {
        s_move_to_zero_if_out = static_cast<float>(g_main_new_tab_settings.alignment.GetValue());
        LogInfo("Window alignment changed");
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Choose how to align the window when repositioning is needed. 1=Top Left, 2=Top Right, 3=Bottom Left, 4=Bottom Right, 5=Center.");
    }
    
    // Apply Changes button
    if (ImGui::Button("Apply Changes")) {
        // Force immediate application of window changes
        extern std::atomic<uint64_t> g_init_apply_generation;
        ::g_init_apply_generation.fetch_add(1);
        LogInfo("Apply Changes button clicked - forcing immediate window update");
        std::ostringstream oss;
        // All global settings on this tab are handled by the settings wrapper
        oss << "Apply Changes button clicked - forcing immediate window update";
        LogInfo(oss.str().c_str());
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Apply the current window size and position settings immediately.");
    }
    
    ImGui::Spacing();
    
    // FPS Limit slider
    float fps_limit = s_fps_limit;
    if (ImGui::SliderFloat("FPS Limit", &fps_limit, 0.0f, 240.0f, fps_limit > 0.0f ? "%.0f FPS" : "No Limit")) {
        s_fps_limit = fps_limit;
        
        if (fps_limit > 0.0f) {
            // Custom FPS Limiter is always enabled, just initialize if needed
            if (g_customFpsLimiterManager && g_customFpsLimiterManager->InitializeCustomFpsLimiterSystem()) {
                LogWarn("Custom FPS Limiter system auto-initialized");
            } else {
                LogWarn("Failed to initialize Custom FPS Limiter system");
                return;
            }
            
            // Update the custom FPS limiter
            if (g_customFpsLimiterManager) {
                auto& limiter = g_customFpsLimiterManager->GetFpsLimiter();
                limiter.SetTargetFps(fps_limit);
                limiter.SetEnabled(true);
                
                std::ostringstream oss;
                oss << "FPS limit applied: " << static_cast<int>(fps_limit) << " FPS (via Custom FPS Limiter)";
                LogInfo(oss.str().c_str());
            }
        } else {
            // FPS limit set to 0, disable the limiter
            if (g_customFpsLimiterManager) {
                auto& limiter = g_customFpsLimiterManager->GetFpsLimiter();
                limiter.SetEnabled(false);
                LogInfo("FPS limit removed (no limit)");
            }
        }
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Set FPS limit for the game (0 = no limit). Now uses the new Custom FPS Limiter system.");
    }
    
    // Background FPS Limit slider
    float fps_limit_bg = s_fps_limit_background;
    if (ImGui::SliderFloat("Background FPS Limit", &fps_limit_bg, 0.0f, 240.0f, fps_limit_bg > 0.0f ? "%.0f FPS" : "No Limit")) {
        s_fps_limit_background = fps_limit_bg;
        
        if (fps_limit_bg > 0.0f) {
            // Custom FPS Limiter is always enabled, just initialize if needed
            if (g_customFpsLimiterManager && g_customFpsLimiterManager->InitializeCustomFpsLimiterSystem()) {
                LogWarn("Custom FPS Limiter system auto-initialized");
            } else {
                LogWarn("Failed to initialize Custom FPS Limiter system");
                return;
            }
            
            // Apply background FPS limit immediately if currently in background
            HWND hwnd = g_last_swapchain_hwnd.load();
            if (hwnd == nullptr) hwnd = GetForegroundWindow();
            const bool is_background = (hwnd != nullptr && GetForegroundWindow() != hwnd);
            
            if (is_background && fps_limit_bg >= 0.f) {
                if (g_customFpsLimiterManager) {
                    auto& limiter = g_customFpsLimiterManager->GetFpsLimiter();
                    limiter.SetTargetFps(fps_limit_bg);
                    limiter.SetEnabled(true);
                    
                    std::ostringstream oss;
                    oss << "Background FPS limit applied immediately: " << static_cast<int>(fps_limit_bg) << " FPS (via Custom FPS Limiter)";
                    LogInfo(oss.str().c_str());
                }
            }
        } else {
            // Background FPS limit set to 0, disable the limiter
            if (g_customFpsLimiterManager) {
                auto& limiter = g_customFpsLimiterManager->GetFpsLimiter();
                limiter.SetEnabled(false);
                LogInfo("Background FPS limit removed (no limit)");
            }
        }
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("FPS cap when the game window is not in the foreground. Now uses the new Custom FPS Limiter system.");
    }
}

void DrawAudioSettings() {
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "=== Audio Settings ===");
    
    // Audio Volume slider
    float volume = s_audio_volume_percent;
    if (ImGui::SliderFloat("Audio Volume (%)", &volume, 0.0f, 100.0f, "%.0f%%")) {
        s_audio_volume_percent = volume;
        
        // Apply volume change immediately
        if (::SetVolumeForCurrentProcess(volume)) {
            std::ostringstream oss;
            oss << "Audio volume changed to " << static_cast<int>(volume) << "%";
            LogInfo(oss.str().c_str());
        } else {
            std::ostringstream oss;
            oss << "Failed to set audio volume to " << static_cast<int>(volume) << "%";
            LogWarn(oss.str().c_str());
        }
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Master audio volume control (0-100%%)");
    }
    
    // Audio Mute checkbox
    bool audio_mute = (s_audio_mute >= 0.5f);
    if (ImGui::Checkbox("Audio Mute", &audio_mute)) {
        s_audio_mute = audio_mute ? 1.0f : 0.0f;
        
        // Apply mute/unmute immediately
        if (::SetMuteForCurrentProcess(audio_mute)) {
            ::g_muted_applied.store(audio_mute);
            std::ostringstream oss;
            oss << "Audio " << (audio_mute ? "muted" : "unmuted") << " successfully";
            LogInfo(oss.str().c_str());
        } else {
            std::ostringstream oss;
            oss << "Failed to " << (audio_mute ? "mute" : "unmute") << " audio";
            LogWarn(oss.str().c_str());
        }
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Manually mute/unmute audio.");
    }
    
    // Mute in Background checkbox (disabled if Mute is ON)
    bool mute_in_bg = (s_mute_in_background >= 0.5f);
    if (s_audio_mute >= 0.5f) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Checkbox("Mute In Background", &mute_in_bg)) {
        s_mute_in_background = mute_in_bg ? 1.0f : 0.0f;
        
        // Reset applied flag so the monitor thread reapplies desired state
        ::g_muted_applied.store(false);
        // Also apply the current mute state immediately if manual mute is off
        if (s_audio_mute < 0.5f) {
            bool want_mute = (mute_in_bg && GetForegroundWindow() != g_last_swapchain_hwnd.load());
            if (::SetMuteForCurrentProcess(want_mute)) {
                ::g_muted_applied.store(want_mute);
                std::ostringstream oss;
                oss << "Background mute " << (mute_in_bg ? "enabled" : "disabled");
                LogInfo(oss.str().c_str());
            }
        }
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Mute the game's audio when it is not the foreground window.");
    }
    if (s_audio_mute >= 0.5f) {
        ImGui::EndDisabled();
    }
}

void DrawWindowControls() {
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.8f, 1.0f), "=== Window Controls ===");
    
    // Window Control Buttons (Minimize and Restore side by side)
    ImGui::BeginGroup();
    
    // Minimize Window Button
    if (ImGui::Button("Minimize Window", ImVec2(ImGui::GetContentRegionAvail().x * 0.48f, 0))) {
        std::thread([](){
            HWND hwnd = g_last_swapchain_hwnd.load();
            if (hwnd == nullptr) hwnd = GetForegroundWindow();
            if (hwnd == nullptr) {
                LogWarn("Minimize Window: no window handle available");
                return;
            }
            LogDebug("Minimize Window button pressed (bg thread)");
            if (ShowWindow(hwnd, SW_MINIMIZE)) {
                LogInfo("Window minimized successfully");
            } else {
                DWORD error = GetLastError();
                std::ostringstream oss;
                oss << "Failed to minimize window. Error: " << error;
                LogWarn(oss.str().c_str());
            }
        }).detach();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Minimize the current game window.");
    }
    
    ImGui::SameLine();
    
    // Restore Window Button
    if (ImGui::Button("Restore Window", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
        std::thread([](){
            HWND hwnd = g_last_swapchain_hwnd.load();
            if (hwnd == nullptr) hwnd = GetForegroundWindow();
            if (hwnd == nullptr) {
                LogWarn("Restore Window: no window handle available");
                return;
            }
            LogDebug("Restore Window button pressed (bg thread)");
            if (ShowWindow(hwnd, SW_RESTORE)) {
                LogInfo("Window restored successfully");
            } else {
                DWORD error = GetLastError();
                std::ostringstream oss;
                oss << "Failed to restore window. Error: " << error;
                LogWarn(oss.str().c_str());
            }
        }).detach();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Restore the minimized game window.");
    }
    
    ImGui::EndGroup();
}

void DrawMonitorDisplaySettings() {
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "=== Monitor & Display Resolution ===");
    
    // Initialize the display cache when first accessed
    static bool cache_initialized = false;
    if (!cache_initialized) {
        InitializeDisplayCache();
        cache_initialized = true;
    }
    
    // Dynamic Monitor Settings
    if (ImGui::CollapsingHeader("Dynamic Monitor Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        HandleMonitorSettingsUI();
    }
    
    ImGui::Spacing();
    
    // Current Display Info
    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.0f), "Current Display Info:");
    std::string display_info = GetCurrentDisplayInfoFromCache();
    ImGui::TextWrapped("%s", display_info.c_str());
}

void DrawBasicReflexSettings() {
    ImGui::TextColored(ImVec4(0.8f, 1.0f, 0.8f, 1.0f), "=== Basic Reflex Settings ===");
    
    // Enable NVIDIA Reflex checkbox
    bool reflex_enabled = (s_reflex_enabled >= 0.5f);
    if (ImGui::Checkbox("Enable NVIDIA Reflex", &reflex_enabled)) {
        s_reflex_enabled = reflex_enabled ? 1.0f : 0.0f;
        
        std::ostringstream oss;
        oss << "NVIDIA Reflex " << (reflex_enabled ? "enabled" : "disabled");
        LogInfo(oss.str().c_str());
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Enable NVIDIA Reflex for reduced latency. This setting appears in the basic tab for easy access.");
    }
}

} // namespace renodx::ui::new_ui
