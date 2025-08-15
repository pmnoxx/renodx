#include "ui_common.hpp"

// External declarations for general settings
extern float s_ui_mode;
extern float s_auto_apply_enabled;
extern float s_auto_apply_delay_sec;
extern float s_auto_apply_init_delay_sec;

namespace renodx::ui {

void AddGeneralSettings(std::vector<renodx::utils::settings::Setting*>& settings) {
    // Basic/Developer/Device Info mode toggle
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "UIMode",
        .binding = &s_ui_mode,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f, // Basic mode by default
        .label = "UI Mode",
        .section = "General",
        .tooltip = "Choose between Basic (minimal features), Developer (all features), Device Info (DXGI device information), and Window Info (window creation details) modes.",
        .labels = {"Basic", "Developer", "Device Info", "Window Info"},
    });

    // Auto-apply toggle and delay (seconds)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "AutoApply",
        .binding = &s_auto_apply_enabled,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Auto Apply",
        .section = "Display",
        .tooltip = "Automatically apply window changes after swapchain initialization.",
        .labels = {"Off", "On"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    settings.push_back(new renodx::utils::settings::Setting{
        .key = "AutoApplyDelay",
        .binding = &s_auto_apply_delay_sec,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 10.f,
        .label = "App Start Delay (s)",
        .section = "Display",
        .tooltip = "Delay after application start before applying window changes.",
        .min = 1.f,
        .max = 60.f,
        .format = "%d s",
        .is_visible = [](){ return is_developer_tab(s_ui_mode); },
    });

    settings.push_back(new renodx::utils::settings::Setting{
        .key = "InitApplyDelay",
        .binding = &s_auto_apply_init_delay_sec,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 1.f,
        .label = "SwapChain Init Delay (s)",
        .section = "Display",
        .tooltip = "Delay after swapchain initialization before applying window changes.",
        .min = 1.f,
        .max = 60.f,
        .format = "%d s",
        .is_visible = [](){ return is_developer_tab(s_ui_mode); },
    });

    // Single-time window initialization setting
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "SingleTimeInit",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 1.f, // Enabled by default
        .label = "Single-Time Window Init",
        .section = "Display",
        .tooltip = "Apply window styles and size once after application starts. This ensures proper window appearance even if hooks fail.",
        .labels = {"Off", "On"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
        .on_change_value = [](float previous, float current) {
            if (current >= 0.5f) {
                LogInfo("Single-time window initialization enabled");
            } else {
                LogInfo("Single-time window initialization disabled");
            }
        }
    });
}

} // namespace renodx::ui
