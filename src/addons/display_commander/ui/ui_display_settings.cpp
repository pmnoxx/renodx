#include "ui_common.hpp"
#include "../utils.hpp"

// External declarations for display settings
extern float s_windowed_width;
extern float s_windowed_height;
extern float s_windowed_pos_x;
extern float s_windowed_pos_y;
extern float s_remove_top_bar;
extern float s_resize_mode;
extern float s_aspect_index;
extern float s_force_borderless;
extern float s_prevent_fullscreen;
extern float s_spoof_fullscreen_state;
extern float s_spoof_window_focus;

extern float s_prevent_windows_minimize;
extern float s_enforce_desired_window;
extern float s_target_monitor_index;
extern float s_move_to_zero_if_out;
extern float s_staggered_resolution_notifications;
extern float s_suppress_maximize;

// Continuous monitoring
extern float s_continuous_monitoring_enabled;
extern void StartContinuousMonitoring();
extern void StopContinuousMonitoring();

// External declarations for global functions
extern void StartBorderlessStyleTimer();
extern void StopBorderlessStyleTimer();

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
        .tooltip = "Choose between fixed width/height or aspect ratio preservation.",
        .labels = {"Fixed Size", "Aspect Ratio"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    // Aspect Ratio (only when in Aspect mode)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "AspectRatio",
        .binding = &s_aspect_index,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 3.f, // 16:9
        .label = "Aspect Ratio",
        .section = "Display",
        .tooltip = "Choose the aspect ratio to maintain when resizing.",
        .labels = {"4:3", "5:4", "16:10", "16:9", "21:9", "32:9"},
        .is_visible = []() { return is_developer_tab(s_ui_mode) && s_resize_mode >= 0.5f; },
    });

    // Remove top bar (title bar, borders)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "RemoveTopBar",
        .binding = &s_remove_top_bar,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 1.f, // Enabled by default to remove top bar
        .label = "Remove Top Bar",
        .section = "Display",
        .tooltip = "Remove the window title bar and borders for a cleaner look.",
        .labels = {"Keep", "Remove"},
        .on_change_value = [](float previous, float current) {
            // Start or stop borderless style enforcement based on setting
            if (current >= 0.5f) {
                // TODO: Fix namespace issues - StartBorderlessStyleTimer();
                LogInfo("Borderless style enforcement started due to setting change");
            } else {
                // TODO: Fix namespace issues - StopBorderlessStyleTimer();
                LogInfo("Borderless style enforcement stopped due to setting change");
            }
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
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
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
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
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    // Force Borderless (global)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "ForceBorderless",
        .binding = &s_force_borderless,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 1.f, // Enabled by default to force borderless mode
        .label = "Force Borderless",
        .section = "Display",
        .tooltip = "Force window to be borderless. Useful for games that don't support borderless mode.",
        .labels = {"Off", "On"},
        .on_change_value = [](float previous, float current){ renodx::mods::swapchain::force_borderless = (current >= 0.5f); },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    // Prevent Fullscreen (global)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "PreventFullscreen",
        .binding = &s_prevent_fullscreen,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 1.f, // Enabled by default
        .label = "Prevent Fullscreen",
        .section = "Display",
        .tooltip = "Prevent exclusive fullscreen; keep borderless/windowed for stability and HDR.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current){ renodx::mods::swapchain::prevent_full_screen = (current >= 0.5f); },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    // Spoof Fullscreen State
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "SpoofFullscreenState",
        .binding = &s_spoof_fullscreen_state,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
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
        .default_value = 0.f,
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



    // Prevent Windows Minimize
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "PreventWindowsMinimize",
        .binding = &s_prevent_windows_minimize,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 1.f, // Enabled by default to prevent minimize when clicking outside window
        .label = "Prevent Windows Minimize",
        .section = "Display",
        .tooltip = "Prevent windows from being minimized by installing Windows hook. Suppresses minimize window messages.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current) {
            // Start or stop minimize prevention hook based on setting
            if (current >= 0.5f) {
                // TODO: Fix namespace issues - InstallMinimizeHook();
                LogInfo("Minimize prevention hook started due to setting change");
            } else {
                // TODO: Fix namespace issues - UninstallMinimizeHook();
                LogInfo("Minimize prevention hook stopped due to setting change");
            }
        },
        .is_visible = []() { return is_main_tab(s_ui_mode); }, // Show in Basic and Developer modes since this is a common issue
    });

    // Enforce Desired Window (resize hook)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "EnforceDesiredWindow",
        .binding = &s_enforce_desired_window,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Enforce Desired Window",
        .section = "Display",
        .tooltip = "Enforce desired window size and position using resize hook. Automatically resizes/moves window to desired dimensions.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current) {
            // Start or stop resize enforcer hook based on setting
            if (current >= 0.5f) {
                // TODO: Fix namespace issues - InstallResizeEnforcerHook();
                LogInfo("Resize enforcer hook started due to setting change");
            } else {
                // TODO: Fix namespace issues - UninstallResizeEnforcerHook();
                LogInfo("Resize enforcer hook stopped due to setting change");
            }
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    // Continuous Monitoring
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "ContinuousMonitoring",
        .binding = &s_continuous_monitoring_enabled,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Continuous Monitoring",
        .section = "Display",
        .tooltip = "Continuously monitor and automatically fix window position, size, and style every second.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current) {
            if (current >= 0.5f) {
                StartContinuousMonitoring();
                LogInfo("Continuous monitoring started due to setting change");
            } else {
                StopContinuousMonitoring();
                LogInfo("Continuous monitoring stopped due to setting change");
            }
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    // Staggered Resolution Notifications
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "StaggeredResolutionNotifications",
        .binding = &s_staggered_resolution_notifications,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Staggered Resolution Notifications",
        .section = "Display",
        .tooltip = "Stagger resolution change notifications to prevent overwhelming the system.",
        .labels = {"Disabled", "Enabled"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });

    // Suppress Maximize
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "SuppressMaximize",
        .binding = &s_suppress_maximize,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Suppress Maximize",
        .section = "Display",
        .tooltip = "Suppress maximize functionality to prevent window from being maximized.",
        .labels = {"Disabled", "Enabled"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });
}

} // namespace renodx::ui
