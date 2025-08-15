#include "ui_window_position_settings.hpp"
#include "ui_common.hpp"
#include "../../../utils/settings.hpp"

namespace renodx::ui {

void AddWindowPositionSettings(std::vector<renodx::utils::settings::Setting*>& settings) {
    // Window position (X, Y coordinates)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "WindowPosX",
        .binding = &s_windowed_pos_x,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Window Position X",
        .section = "Display",
        .tooltip = "X coordinate for window positioning (relative to target monitor).",
        .min = -10000.f,
        .max = 10000.f,
        .format = "%d px",
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    settings.push_back(new renodx::utils::settings::Setting{
        .key = "WindowPosY",
        .binding = &s_windowed_pos_y,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Window Position Y",
        .section = "Display",
        .tooltip = "Y coordinate for window positioning (relative to target monitor).",
        .min = -10000.f,
        .max = 10000.f,
        .format = "%d px",
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });
}

} // namespace renodx::ui
