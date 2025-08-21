#include "ui_developer_settings.hpp"
#include "ui_common.hpp"
#include "../renodx/settings.hpp"


namespace renodx::ui {

void AddDeveloperSettings(std::vector<renodx::utils::settings2::Setting*>& settings) {


    // Prevent Fullscreen (global)
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "PreventFullscreen",
        .binding = &s_prevent_fullscreen,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Prevent Fullscreen",
        .section = "Display",
        .tooltip = "Prevent exclusive fullscreen; keep borderless/windowed for stability and HDR.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current){ 
            // The setting is handled by our event handler in reshade_events/fullscreen_prevention.cpp
            // We do NOT set the RenoDX variable to avoid conflicts
        },
        .is_visible = []() { return is_developer_tab(s_ui_tab); }, // Only show in Developer mode
    });

    // Spoof Fullscreen State
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "SpoofFullscreenState",
        .binding = &s_spoof_fullscreen_state,
        .value_type = renodx::utils::settings2::SettingValueType::INTEGER,
        .default_value = 1.f,
        .label = "Spoof Fullscreen State",
        .section = "Display",
        .tooltip = "Spoof fullscreen state detection for applications that query fullscreen status. Useful for games that change behavior based on fullscreen state.",
        .labels = {"Disabled", "Spoof as Fullscreen", "Spoof as Windowed"},
        .on_change_value = [](float previous, float current){ 
            // Update the global spoofing state
            // This will be used by the fullscreen state interception logic
            std::ostringstream oss;
            oss << "Fullscreen state spoofing changed from " << previous << " to " << current;
            if (current < 0.5f) {
                oss << " (Disabled)";
            } else if (current < 1.5f) {
                oss << " (Spoof as Fullscreen)";
            } else {
                oss << " (Spoof as Windowed)";
            }
            LogInfo(oss.str().c_str());
        },
        .is_visible = []() { return is_developer_tab(s_ui_tab); }, // Only show in Developer mode
    });

    // Spoof Window Focus
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "SpoofWindowFocus",
        .binding = &s_spoof_window_focus,
        .value_type = renodx::utils::settings2::SettingValueType::INTEGER,
        .default_value = 1.f,
        .label = "Spoof Window Focus",
        .section = "Display",
        .tooltip = "Spoof window focus state for applications that query focus status. Useful for games that change behavior based on focus state.",
        .labels = {"Disabled", "Spoof as Focused", "Spoof as Unfocused"},
        .on_change_value = [](float previous, float current){ 
            // Update the global focus spoofing state
            std::ostringstream oss;
            oss << "Window focus spoofing changed from " << previous << " to " << current;
            if (current < 0.5f) {
                oss << " (Disabled)";
            } else if (current < 1.5f) {
                oss << " (Spoof as Focused)";
            } else {
                oss << " (Spoof as Unfocused)";
            }
            LogInfo(oss.str().c_str());
        },
        .is_visible = []() { return is_developer_tab(s_ui_tab); }, // Only show in Developer mode
    });

}

} // namespace renodx::ui
