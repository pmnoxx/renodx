#include "ui_window_controls.hpp"
#include "ui_common.hpp"
#include "../renodx/settings.hpp"

namespace renodx::ui {

void AddWindowControls(std::vector<renodx::utils::settings2::Setting*>& settings) {
    // Window Control Buttons (Minimize and Restore side by side)
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "WindowControlButtons",
        .binding = nullptr,
        .value_type = renodx::utils::settings2::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "Window Controls",
        .section = "Display",
        .tooltip = "Minimize and restore the current game window. Useful for testing window management.",
        .on_draw = []() -> bool {
            bool changed = false;
            
            // Draw buttons side by side
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
                changed = true;
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
                changed = true;
            }
            
            ImGui::EndGroup();
            
            return changed;
        },
        .is_visible = []() { return is_basic_tab(s_ui_mode); }, // Only show in Basic mode
    });
}

} // namespace renodx::ui
