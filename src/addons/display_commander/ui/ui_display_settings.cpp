#include "ui_display_settings.hpp"
#include "ui_common.hpp"
#include "../../../utils/settings.hpp"

namespace renodx::ui {

void AddDisplaySettings(std::vector<renodx::utils::settings::Setting*>& settings) {
    // Window width preset slider with labels
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "WindowWidth",
        .binding = &s_windowed_width,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 3.f, // default to 1920
        .label = "Window Width",
        .section = "Display",
        .labels = MakeLabels(WIDTH_OPTIONS, 7),
        .is_visible = []() { return is_basic_tab(s_ui_mode); }, // Show in Basic mode
        .parse = [](float index) {
          int i = static_cast<int>(index);
          i = (std::max)(i, 0);
          int max_i = 6;
          i = (std::min)(i, max_i);
          return static_cast<float>(WIDTH_OPTIONS[i]);
        },
    });

    // Window height preset slider with labels
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "WindowHeight",
        .binding = &s_windowed_height,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 2.f, // default to 1080
        .label = "Window Height",
        .section = "Display",
        .labels = MakeLabels(HEIGHT_OPTIONS, 7),
        .is_visible = []() { return is_basic_tab(s_ui_mode); }, // Show in Basic mode
        .is_enabled = [](){ return s_resize_mode < 0.5f; },
        .parse = [](float index) {
          int i = static_cast<int>(index);
          i = (std::max)(i, 0);
          int max_i = 6;
          i = (std::min)(i, max_i);
          return static_cast<float>(HEIGHT_OPTIONS[i]);
        },
    });

    // Resize mode: width/height vs aspect ratio
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "ResizeMode",
        .binding = &s_resize_mode,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Resize Mode",
        .section = "Display",
        .tooltip = "Choose between manual width/height or aspect ratio-based resizing.",
        .labels = {"Width/Height", "Aspect Ratio"},
        .is_visible = []() { return is_basic_tab(s_ui_mode); }, // Show in Basic mode
    });

    // Aspect Ratio (only when in Aspect mode)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "AspectRatio",
        .binding = &s_aspect_index,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 3.f,
        .label = "Aspect Ratio",
        .section = "Display",
        .tooltip = "Choose the aspect ratio for window resizing. Only applies when Resize Mode is set to Aspect Ratio.",
        .labels = {"3:2", "4:3", "16:10", "16:9", "19:9", "19.5:9", "21:9", "32:9"},
        .min = 0.f,
        .max = 7.f,
        .is_visible = []() { return is_basic_tab(s_ui_mode); }, // Show in Basic mode
    });

    // FPS Limit
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "FPSLimit",
        .binding = &s_fps_limit,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "FPS Limit",
        .section = "Performance",
        .tooltip = "Set FPS limit for the game (0 = no limit).",
        .min = 0.f,
        .max = 300.f,
        .format = "%d FPS",
        .on_change_value = [](float previous, float current){ g_default_fps_limit.store(current); },
        .on_draw = []() -> bool {
            // Sync with current atomic value
            s_fps_limit = g_default_fps_limit.load();
            return false;
        },
        .is_visible = []() { return is_basic_tab(s_ui_mode); }, // Show in Basic mode
    });

    // Remove top bar (title bar, borders)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "RemoveTopBar",
        .binding = &s_remove_top_bar,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 1.f,
        .label = "Remove Top Bar",
        .section = "Display",
        .tooltip = "Remove the window title bar and borders for a cleaner look.",
        .labels = {"Keep", "Remove"},
        .on_change_value = [](float previous, float current) {
            // Note: Borderless style enforcement removed - use continuous monitoring instead
            std::ostringstream oss;
            oss << "Remove top bar setting changed from " << (previous >= 0.5f ? "enabled" : "disabled") << " to " << (current >= 0.5f ? "enabled" : "disabled");
            LogInfo(oss.str().c_str());
        },
        .is_visible = []() { return is_basic_tab(s_ui_mode); }, // Show in Basic mode
    });

    // Target Monitor
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "TargetMonitor",
        .binding = &s_target_monitor_index,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Target Monitor",
        .section = "Display",
        .tooltip = "Choose which monitor to apply size/pos to. 'Auto' uses the current window monitor.",
        .labels = MakeMonitorLabels(),
        .is_visible = []() { return is_basic_tab(s_ui_mode); }, // Show in Basic mode
    });

    // Window alignment when repositioning is needed
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "Alignment",
        .binding = &s_move_to_zero_if_out,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 2.f, // default to top right
        .label = "Alignment",
        .section = "Display",
        .tooltip = "Choose how to align the window when repositioning is needed. 1=Top Left, 2=Top Right, 3=Bottom Left, 4=Bottom Right, 5=Center.",
        .labels = {"None", "Top Left", "Top Right", "Bottom Left", "Bottom Right", "Center"},
        .is_visible = []() { return is_basic_tab(s_ui_mode); }, // Show in Basic mode
    });

    // Apply Changes button
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "ApplyChanges",
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .default_value = 0.f,
        .label = "Apply Changes",
        .section = "Display",
        .tooltip = "Apply the current window size and position settings immediately.",
        .on_click = []() -> bool {
            // Force immediate application of window changes
            extern std::atomic<uint64_t> g_init_apply_generation;
            ::g_init_apply_generation.fetch_add(1);
            LogInfo("Apply Changes button clicked - forcing immediate window update");
            return true;
        },
        .is_visible = []() { return is_basic_tab(s_ui_mode); }, // Show in Basic mode
    });
}

} // namespace renodx::ui
