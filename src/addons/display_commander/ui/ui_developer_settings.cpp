#include "ui_developer_settings.hpp"
#include "ui_common.hpp"
#include "../../../utils/settings.hpp"
#include "../renodx/proxy.hpp"

namespace renodx::ui {

void AddDeveloperSettings(std::vector<renodx::utils::settings::Setting*>& settings) {


    // Prevent Fullscreen (global)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "PreventFullscreen",
        .binding = &s_prevent_fullscreen,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Prevent Fullscreen",
        .section = "Display",
        .tooltip = "Prevent exclusive fullscreen; keep borderless/windowed for stability and HDR.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current){ 
            // The setting is handled by our event handler in reshade_events/fullscreen_prevention.cpp
            // We do NOT set the RenoDX variable to avoid conflicts
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    // Spoof Fullscreen State
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "SpoofFullscreenState",
        .binding = &s_spoof_fullscreen_state,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
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
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    // Spoof Window Focus
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "SpoofWindowFocus",
        .binding = &s_spoof_window_focus,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
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
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    // Suppress Move/Resize Messages
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "SuppressMoveResizeMessages",
        .binding = &s_suppress_move_resize_messages,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Suppress Move/Resize Messages",
        .section = "Display",
        .tooltip = "Suppress window move/resize messages that don't match our desired state. Prevents games from overriding window positioning.",
        .labels = {"Disabled", "Enabled"},
        .is_enabled = []() { return false; }, // Disabled
        .on_change_value = [](float previous, float current){ 
            std::ostringstream oss;
            oss << "Suppress move/resize messages changed from " << (previous >= 0.5f ? "enabled" : "disabled") << " to " << (current >= 0.5f ? "enabled" : "disabled");
            LogInfo(oss.str().c_str());
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });



    // Suppress Top Bar/Border Messages
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "SuppressTopBarBorderMessages",
        .binding = &s_remove_top_bar,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f, // Disabled by default due to bug
        .label = "Suppress Top Bar/Border Messages",
        .section = "Display",
        .tooltip = "Suppress window messages that would add title bars and borders. Prevents games from restoring unwanted window decorations. ⚠️ WARNING: This feature has bugs and is disabled by default.",
        .labels = {"Disabled", "Enabled"},
        .is_enabled = []() { return false; }, // Disabled
        .on_change_value = [](float previous, float current){ 
            std::ostringstream oss;
            oss << "Suppress top bar/border messages changed from " << (previous >= 0.5f ? "enabled" : "disabled") << " to " << (current >= 0.5f ? "enabled" : "disabled");
            LogInfo(oss.str().c_str());
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    // Suppress Maximize Messages
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "SuppressMaximizeMessages",
        .binding = &s_suppress_maximize,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 1.f,
        .label = "Suppress Maximize Messages",
        .section = "Display",
        .tooltip = "Suppress window maximize messages. Prevents games from maximizing their windows unexpectedly.",
        .labels = {"Disabled", "Enabled"},
        .is_enabled = []() { return false; }, // Disabled
        .on_change_value = [](float previous, float current){ 
            std::ostringstream oss;
            oss << "Suppress maximize messages changed from " << (previous >= 0.5f ? "enabled" : "disabled") << " to " << (current >= 0.5f ? "enabled" : "disabled");
            LogInfo(oss.str().c_str());
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });
}

} // namespace renodx::ui
