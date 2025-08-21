#include "ui_general_settings.hpp"
#include "ui_common.hpp"
#include "../renodx/settings.hpp"
#include "../addon.hpp"

// External declarations for general settings
extern float s_block_input_in_background;
extern float s_background_feature_enabled; // Added this line

    // CONTINUOUS RENDERING FUNCTIONS REMOVED - Focus spoofing is now handled by Win32 hooks

namespace renodx::ui {

void AddGeneralSettings(std::vector<renodx::utils::settings2::Setting*>& settings) {
    // UI Mode selector
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "UIMode",
        .binding = &s_ui_mode,
        .value_type = renodx::utils::settings2::SettingValueType::INTEGER,
        .default_value = 0.f, // Basic mode by default
        .label = "UI Mode",
        .section = "General",
        .tooltip = "Choose between Simple (minimal features), Display (desktop resolution override), Developer (all features), Device Info (DXGI device information), and Window Info (window creation details) modes.",
        .labels = TAB_LABELS,
        .min = 0.f,
        .max = static_cast<float>(TAB_LABELS.size() - 1),
        .is_visible = []() { return true; }, // Always visible
    });

    // Continuous monitoring toggle
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "ContinuousMonitoring",
        .binding = &s_continuous_monitoring_enabled,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
        .default_value = 1.f, // Enabled by default to match globals.cpp
        .label = "Continuous Monitoring",
        .section = "Display",
        .tooltip = "Continuously monitor and automatically fix window position, size, and style every second.",
        .labels = {"Off", "On"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    // Prevent Always On Top toggle
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "PreventAlwaysOnTop",
        .binding = &s_prevent_always_on_top,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
        .default_value = 1.f, // Enabled by default to match globals.cpp
        .label = "Prevent Always On Top",
        .section = "Display",
        .tooltip = "Prevents windows from becoming always on top, even if they are moved or resized.",
        .labels = {"Off", "On"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
        // .on_change_value removed due to linter errors, functionality handled by continuous monitoring
    });

    // Block Input When In Background toggle
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "BlockInputInBackground",
        .binding = &s_block_input_in_background,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
        .default_value = 1.f, // Enabled by default
        .label = "Block Input When In Background",
        .section = "Display",
        .tooltip = "Uses ReShade's input blocking to prevent mouse confinement when the app is not focused.",
        .labels = {"Off", "On"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    // Background Feature toggle
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "BackgroundFeature",
        .binding = &s_background_feature_enabled,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
        .default_value = 0.f, // Disabled by default
        .label = "Background Feature",
        .section = "Display",
        .tooltip = "Creates a black background window behind the game when it doesn't cover the full screen.",
        .labels = {"Off", "On"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    // CONTINUOUS RENDERING UI SETTINGS REMOVED - Focus spoofing is now handled by Win32 hooks
}

} // namespace renodx::ui
