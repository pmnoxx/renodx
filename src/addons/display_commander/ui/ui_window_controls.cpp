#include "ui_window_controls.hpp"
#include "ui_common.hpp"
#include "../renodx/settings.hpp"

namespace renodx::ui {

void AddWindowControls(std::vector<renodx::utils::settings2::Setting*>& settings) {
    // Minimize Window
    settings.push_back(new renodx::utils::settings2::Setting{
        .value_type = renodx::utils::settings2::SettingValueType::BUTTON,
        .label = "Minimize Window",
        .section = "Display",
        .tooltip = "Minimize the current game window. Useful for testing window management.",
        .on_click = []() {
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
            return false;
        },
        .is_visible = []() { return is_basic_tab(s_ui_mode); }, // Only show in Basic mode
    });

    // Restore Window
    settings.push_back(new renodx::utils::settings2::Setting{
        .value_type = renodx::utils::settings2::SettingValueType::BUTTON,
        .label = "Restore Window",
        .section = "Display",
        .tooltip = "Restore the current game window to normal size. Useful for testing window management.",
        .on_click = []() {
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
            return false;
        },
        .is_visible = []() { return is_basic_tab(s_ui_mode); }, // Only show in Basic mode
    });
}

} // namespace renodx::ui
