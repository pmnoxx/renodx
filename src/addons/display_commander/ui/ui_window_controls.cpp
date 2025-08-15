#include "ui_common.hpp"

namespace renodx::ui {

void AddWindowControls(std::vector<renodx::utils::settings::Setting*>& settings) {
    // Apply Now Button
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "ApplyNow",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .default_value = 0.f,
        .label = "Apply Now",
        .section = "Controls",
        .tooltip = "Apply window changes immediately on another thread.",
        .on_click = []() -> bool {
            // Get the current swapchain window and apply changes on another thread
            extern std::atomic<HWND> g_last_swapchain_hwnd;
            extern void ApplyWindowChange(HWND hwnd, const char* reason = "unknown");
            extern float s_windowed_width, s_windowed_height, s_windowed_pos_x, s_windowed_pos_y, s_remove_top_bar;
            
            HWND hwnd = ::g_last_swapchain_hwnd.load();
            if (hwnd != nullptr && IsWindow(hwnd)) {
                int want_w = static_cast<int>(::s_windowed_width);
                int want_h = static_cast<int>(::s_windowed_height);
                int pos_x = static_cast<int>(::s_windowed_pos_x);
                int pos_y = static_cast<int>(::s_windowed_pos_y);
                
                // Determine style mode
                WindowStyleMode style_mode = (::s_remove_top_bar >= 0.5f) ? WindowStyleMode::BORDERLESS : WindowStyleMode::OVERLAPPED_WINDOW;
                
                LogInfo("Apply Now: Scheduling window change on another thread");
                
                // Apply changes on another thread to avoid blocking the UI
                std::thread([hwnd, want_w, want_h, pos_x, pos_y, style_mode]() {
                    try {
                        // Small delay to ensure UI is not blocked
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        
                        std::ostringstream oss;
                        oss << "Apply Now: Executing window change - resize to " << want_w << "x" << want_h 
                            << ", move to (" << pos_x << "," << pos_y << "), style mode " << static_cast<int>(style_mode);
                        LogInfo(oss.str().c_str());
                        
                        ::ApplyWindowChange(hwnd, "ui_window_controls_apply_now");
                        
                        LogInfo("Apply Now: Window change completed successfully");
                    } catch (const std::exception& e) {
                        std::ostringstream oss;
                        oss << "Apply Now: Exception during window change: " << e.what();
                        LogWarn(oss.str().c_str());
                    } catch (...) {
                        LogWarn("Apply Now: Unknown exception during window change");
                    }
                }).detach();
                
                LogInfo("Apply Now: Window change scheduled successfully");
            } else {
                LogWarn("Apply Now: No valid swapchain window available");
            }
            return true; // Return true to indicate success
        },
        .is_visible = []() { return is_basic_tab(s_ui_mode); },
    });

    // Reset to Defaults Button
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "ResetToDefaults",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .default_value = 0.f,
        .label = "Reset to Defaults",
        .section = "Controls",
        .tooltip = "Reset all settings to their default values.",
        .on_click = []() -> bool {
            // TODO: Fix namespace issues - Reset all settings to defaults
            /*
            extern void ResetAllSettingsToDefaults();
            ResetAllSettingsToDefaults();
            LogInfo("All settings reset to defaults");
            */
            LogInfo("Reset to Defaults button clicked (functionality temporarily disabled)");
            return true; // Return true to indicate success
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
    });
}

} // namespace renodx::ui
