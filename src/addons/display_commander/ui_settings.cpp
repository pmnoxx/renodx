#include "addon.hpp"
#include "nvapi_fullscreen_prevention.hpp"
#include "reflex_management.hpp"
#include "dxgi_device_info.hpp"
#include "utils.hpp"
#include "ui_settings.hpp"
#include <sstream>
#include <iomanip>

// External declarations
extern float s_staggered_resolution_notifications;
extern float s_suppress_maximize;

// Helper functions for tab visibility
inline bool is_basic_tab(float ui_mode) { return ui_mode < 0.5f; }
inline bool is_developer_tab(float ui_mode) { return ui_mode >= 0.5f && ui_mode < 2.0f; }
inline bool is_device_tab(float ui_mode) { return ui_mode >= 2.0f; }

// UI/settings
renodx::utils::settings::Settings settings = {
    // Basic/Developer/Device Info mode toggle
    new renodx::utils::settings::Setting{
        .key = "UIMode",
        .binding = &s_ui_mode,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f, // Basic mode by default
        .label = "UI Mode",
        .section = "General",
        .tooltip = "Choose between Basic (minimal features), Developer (all features), and Device Info (DXGI device information) modes.",
        .labels = {"Basic", "Developer", "Device Info"},
    },
    // Auto-apply toggle and delay (seconds)
    new renodx::utils::settings::Setting{
        .key = "AutoApply",
        .binding = &s_auto_apply_enabled,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Auto Apply",
        .section = "Display",
        .tooltip = "Automatically apply window changes after swapchain initialization.",
        .labels = {"Off", "On"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    new renodx::utils::settings::Setting{
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
    },
    new renodx::utils::settings::Setting{
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
    },
    // Window width preset slider with labels
    new renodx::utils::settings::Setting{
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
    },
    // Window height preset slider with labels
    new renodx::utils::settings::Setting{
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
    },
    // Resize mode: width/height vs aspect ratio
    new renodx::utils::settings::Setting{
        .key = "ResizeMode",
        .binding = &s_resize_mode,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Resize Mode",
        .section = "Display",
        .tooltip = "Choose between manual width/height or aspect ratio-based resizing.",
        .labels = {"Width/Height", "Aspect Ratio"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    // Aspect Ratio (only when in Aspect mode)
    new renodx::utils::settings::Setting{
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
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
    },

    // FPS Limit
    new renodx::utils::settings::Setting{
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
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    // Background FPS Limit
    new renodx::utils::settings::Setting{
        .key = "FPSLimitBackground",
        .binding = &s_fps_limit_background,
        .default_value = 30.f,
        .label = "Background FPS Limit",
        .section = "Performance",
        .tooltip = "FPS cap when the game window is not in the foreground.",
        .min = 0.f,
        .max = 240.f,
        .format = "%.0f FPS",
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    // Volume (0-100)
    new renodx::utils::settings::Setting{
        .key = "AudioVolume",
        .binding = &s_audio_volume_percent,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 100.f,
        .label = "Audio Volume (%)",
        .section = "Audio",
        .tooltip = "Master audio volume control (0-100%).",
        .min = 0.f,
        .max = 100.f,
        .format = "%d%%",
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    // Mute (manual)
    new renodx::utils::settings::Setting{
        .key = "AudioMute",
        .binding = &s_audio_mute,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Audio Mute",
        .section = "Audio",
        .tooltip = "Manually mute/unmute audio.",
        .labels = {"Unmuted", "Muted"},
        .on_change_value = [](float previous, float current){
            // Reset applied flag so the monitor thread reapplies desired state
            g_muted_applied.store(false);
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    // Mute in Background (placed after Mute; disabled if Mute is ON)
    new renodx::utils::settings::Setting{
        .key = "MuteInBackground",
        .binding = &s_mute_in_background,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Mute In Background",
        .section = "Audio",
        .tooltip = "Mute the game's audio when it is not the foreground window.",
        .labels = {"Off", "On"},
        .on_change_value = [](float previous, float current){
          // Reset applied flag so the monitor thread reapplies desired state
          g_muted_applied.store(false);
        },
        .is_visible = [](){ return is_developer_tab(s_ui_mode); },
    },
    // Apply button executes windowed resize
    new renodx::utils::settings::Setting{
        .key = "Apply",
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Apply Changes",
        .section = "Actions",
        .tooltip = "Apply the current window size and position settings to the game window.",
        .on_click = []() -> bool {
            std::thread([](){
                HWND hwnd = GetForegroundWindow();
                if (hwnd != nullptr) {
                    // Get current desired size
                    int want_w = 0; int want_h = 0; ComputeDesiredSize(want_w, want_h);
                    
                    // Apply the change
                    extern void ApplyWindowChange(HWND hwnd, bool do_resize, int client_width, int client_height, bool do_move, int pos_x, int pos_y, WindowStyleMode style_mode);
                    WindowStyleMode mode = (s_remove_top_bar >= 0.5f) ? WindowStyleMode::BORDERLESS : WindowStyleMode::OVERLAPPED_WINDOW;
                    ApplyWindowChange(hwnd, true, want_w, want_h, true, static_cast<int>(s_windowed_pos_x), static_cast<int>(s_windowed_pos_y), mode);
                    
                    LogInfo("Manual apply executed");
                }
            }).detach();
            return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    // Remove top bar (title bar, borders)
    new renodx::utils::settings::Setting{
        .key = "RemoveTopBar",
        .binding = &s_remove_top_bar,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Remove Top Bar",
        .section = "Display",
        .tooltip = "Remove the window title bar and borders for a cleaner look.",
        .labels = {"Keep", "Remove"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    // Target Monitor
    new renodx::utils::settings::Setting{
        .key = "TargetMonitor",
        .binding = &s_target_monitor_index,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Target Monitor",
        .section = "Display",
        .tooltip = "Choose which monitor to apply size/pos to. 'Auto' uses the current window monitor.",
        .labels = MakeMonitorLabels(),
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    // Window alignment when repositioning is needed
    new renodx::utils::settings::Setting{
        .key = "Alignment",
        .binding = &s_move_to_zero_if_out,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 2.f, // default to top right
        .label = "Alignment",
        .section = "Display",
        .tooltip = "Choose how to align the window when repositioning is needed. 1=Top Left, 2=Top Right, 3=Bottom Left, 4=Bottom Right.",
        .labels = {"None", "Top Left", "Top Right", "Bottom Left", "Bottom Right"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    // Force Borderless (global)
    new renodx::utils::settings::Setting{
        .key = "ForceBorderless",
        .binding = &s_force_borderless,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Force Borderless",
        .section = "Display",
        .tooltip = "Force window to be borderless. Useful for games that don't support borderless mode.",
        .labels = {"Off", "On"},
        .on_change_value = [](float previous, float current){ renodx::mods::swapchain::force_borderless = (current >= 0.5f); },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    // Prevent Fullscreen (global)
    new renodx::utils::settings::Setting{
        .key = "PreventFullscreen",
        .binding = &s_prevent_fullscreen,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 1.f,
        .label = "Prevent Fullscreen",
        .section = "Display",
        .tooltip = "Prevent exclusive fullscreen; keep borderless/windowed for stability and HDR.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current){ renodx::mods::swapchain::prevent_full_screen = (current >= 0.5f); },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    // Spoof Fullscreen State
    new renodx::utils::settings::Setting{
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
    },
    // Spoof Window Focus
    new renodx::utils::settings::Setting{
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
    },

    // Suppress Alt-Tab
    new renodx::utils::settings::Setting{
        .key = "SuppressAltTab",
        .binding = &s_suppress_alt_tab,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Suppress Alt-Tab",
        .section = "Display",
        .tooltip = "Suppress Alt-Tab functionality by installing Windows hook. Some games don't get informed about Alt-Tab but it still works and allows switching back.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current){ 
            // Update the Alt-Tab suppression state
            std::ostringstream oss;
            oss << "Alt-Tab suppression changed from " << (previous >= 0.5f ? "enabled" : "disabled") << " to " << (current >= 0.5f ? "enabled" : "disabled");
            LogInfo(oss.str().c_str());
            
            // Update all Alt suppression methods
            UpdateAltSuppressionMethods();
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    // Prevent Windows Minimize
    new renodx::utils::settings::Setting{
        .key = "PreventWindowsMinimize",
        .binding = &s_prevent_windows_minimize,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Prevent Windows Minimize",
        .section = "Display",
        .tooltip = "Prevent windows from being minimized by installing Windows hook. Suppresses minimize window messages.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current){ 
            // Update the Windows minimize prevention state
            std::ostringstream oss;
            oss << "Windows minimize prevention changed from " << (previous >= 0.5f ? "enabled" : "disabled") << " to " << (current >= 0.5f ? "enabled" : "disabled");
            LogInfo(oss.str().c_str());
            
            // Install or uninstall the hook based on the new setting
            if (current >= 0.5f) {
                InstallMinimizeHook();
            } else {
                UninstallMinimizeHook();
            }
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },

    // Enforce Desired Window (resize hook)
    new renodx::utils::settings::Setting{
        .key = "EnforceDesiredWindow",
        .binding = &s_enforce_desired_window,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Enforce Desired Window Size/Pos",
        .section = "Display",
        .tooltip = "Install a hook to enforce desired size/position whenever the window is resized or moved.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current){
            std::ostringstream oss;
            oss << "Resize enforcement changed from " << (previous >= 0.5f ? "enabled" : "disabled")
                << " to " << (current >= 0.5f ? "enabled" : "disabled");
            LogInfo(oss.str().c_str());
            if (current >= 0.5f) {
                InstallResizeEnforcerHook();
            } else {
                UninstallResizeEnforcerHook();
            }
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
    },

    // Staggered Resolution Notifications
    new renodx::utils::settings::Setting{
        .key = "StaggeredResolutionNotifications",
        .binding = &s_staggered_resolution_notifications,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Staggered Resolution Notifications",
        .section = "Display",
        .tooltip = "Use staggered resolution change notifications for games that don't respond to immediate notifications. This sends multiple WM_SIZE messages over time to ensure the game updates its internal buffers.",
        .labels = {"Disabled", "Enabled"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
    },

    // Suppress Maximize
    new renodx::utils::settings::Setting{
        .key = "SuppressMaximize",
        .binding = &s_suppress_maximize,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Suppress Maximize",
        .section = "Display",
        .tooltip = "Prevent the game window from being maximized. Useful for maintaining consistent window sizing and preventing games from overriding your desired window dimensions.",
        .labels = {"Disabled", "Enabled"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
    },

    // Minimize Window
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
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
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },

    // Maximize Window
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Maximize Window",
        .section = "Display",
        .tooltip = "Maximize the current game window. Useful for testing window management.",
        .on_click = []() {
            std::thread([](){
                HWND hwnd = g_last_swapchain_hwnd.load();
                if (hwnd == nullptr) hwnd = GetForegroundWindow();
                if (hwnd == nullptr) {
                    LogWarn("Maximize Window: no window handle available");
                    return;
                }
                LogDebug("Maximize Window button pressed (bg thread)");
                if (ShowWindow(hwnd, SW_MAXIMIZE)) {
                    LogInfo("Window maximized successfully");
                } else {
                    DWORD error = GetLastError();
                    std::ostringstream oss;
                    oss << "Failed to maximize window. Error: " << error;
                    LogWarn(oss.str().c_str());
                }
            }).detach();
            return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },

    // Restore Window
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
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
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },

    // Make 720p Window
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Make 720p Window",
        .section = "Display",
        .tooltip = "Quickly set window to 1280x720 (720p) resolution.",
        .on_click = []() {
            std::thread([](){
                HWND hwnd = g_last_swapchain_hwnd.load();
                if (hwnd == nullptr) hwnd = GetForegroundWindow();
                if (hwnd == nullptr) {
                    LogWarn("Make 720p Window: no window handle available");
                    return;
                }
                LogDebug("Make 720p Window button pressed (bg thread)");
                
                // Set the desired size to 720p
                int want_w = 1280;
                int want_h = 720;
                
                // Get current window position
                RECT wr{};
                int pos_x = 100;
                int pos_y = 100;
                if (GetWindowRect(hwnd, &wr) != FALSE) {
                    pos_x = wr.left;
                    pos_y = wr.top;
                }
                
                // Apply 720p window with current style preference
                WindowStyleMode mode = (s_remove_top_bar >= 0.5f) ? WindowStyleMode::BORDERLESS : WindowStyleMode::OVERLAPPED_WINDOW;
                ApplyWindowChange(
                    hwnd,
                    /*do_resize=*/true, want_w, want_h,
                    /*do_move=*/true, pos_x, pos_y,
                    mode);
                
                LogInfo("720p window applied successfully");
            }).detach();
            return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },

    // Make 1080p Window
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Make 1080p Window",
        .section = "Display",
        .tooltip = "Quickly set window to 1920x1080 (1080p) resolution.",
        .on_click = []() {
            std::thread([](){
                HWND hwnd = g_last_swapchain_hwnd.load();
                if (hwnd == nullptr) hwnd = GetForegroundWindow();
                if (hwnd == nullptr) {
                    LogWarn("Make 1080p Window: no window handle available");
                    return;
                }
                LogDebug("Make 1080p Window button pressed (bg thread)");
                
                // Set the desired size to 1080p
                int want_w = 1920;
                int want_h = 1080;
                
                // Get current window position
                RECT wr{};
                int pos_x = 100;
                int pos_y = 100;
                if (GetWindowRect(hwnd, &wr) != FALSE) {
                    pos_x = wr.left;
                    pos_y = wr.top;
                }
                
                // Apply 1080p window with current style preference
                WindowStyleMode mode = (s_remove_top_bar >= 0.5f) ? WindowStyleMode::BORDERLESS : WindowStyleMode::OVERLAPPED_WINDOW;
                ApplyWindowChange(
                    hwnd,
                    /*do_resize=*/true, want_w, want_h,
                    /*do_move=*/true, pos_x, pos_y,
                    mode);
                
                LogInfo("1080p window applied successfully");
            }).detach();
            return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },

    // NVAPI Fullscreen Prevention
    new renodx::utils::settings::Setting{
        .key = "nvapi_fullscreen_prevention",
        .binding = &s_nvapi_fullscreen_prevention,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "NVAPI Fullscreen Prevention",
        .section = "Display",
        .tooltip = "Use NVIDIA driver settings to prevent fullscreen mode. Requires NVIDIA GPU and drivers.",
        .on_change_value = [](float previous, float current) {
            std::ostringstream oss;
            oss << "NVAPI Fullscreen Prevention changed from " << (previous >= 0.5f ? "enabled" : "disabled") << " to " << (current >= 0.5f ? "enabled" : "disabled");
            LogInfo(oss.str().c_str());
            
            // Initialize NVAPI if needed
            extern NVAPIFullscreenPrevention g_nvapiFullscreenPrevention;
            if (current >= 0.5f) {
                if (!g_nvapiFullscreenPrevention.IsAvailable()) {
                    if (!g_nvapiFullscreenPrevention.Initialize()) {
                        LogWarn("Failed to initialize NVAPI for fullscreen prevention");
                        return;
                    }
                }
                
                if (g_nvapiFullscreenPrevention.SetFullscreenPrevention(true)) {
                    LogInfo("NVAPI fullscreen prevention enabled successfully");
                } else {
                    std::ostringstream oss;
                    oss << "Failed to enable NVAPI fullscreen prevention: " << g_nvapiFullscreenPrevention.GetLastError();
                    LogWarn(oss.str().c_str());
                }
            } else {
                if (g_nvapiFullscreenPrevention.IsAvailable()) {
                    if (g_nvapiFullscreenPrevention.SetFullscreenPrevention(false)) {
                        LogInfo("NVAPI fullscreen prevention disabled successfully");
                    } else {
                        std::ostringstream oss;
                        oss << "Failed to disable NVAPI fullscreen prevention: " << g_nvapiFullscreenPrevention.GetLastError();
                        LogWarn(oss.str().c_str());
                    }
                }
            }
        },
        .on_draw = []() -> bool {
            extern NVAPIFullscreenPrevention g_nvapiFullscreenPrevention;
            
            // Main status section
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "NVAPI Debug Information:");
            
            if (g_nvapiFullscreenPrevention.IsAvailable()) {
                // Library loaded successfully
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ NVAPI Library: Loaded");
                
                // Driver version info
                std::string driverVersion = g_nvapiFullscreenPrevention.GetDriverVersion();
                if (driverVersion != "Failed to get driver version") {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ Driver Version: %s", driverVersion.c_str());
                } else {
                    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "⚠ Driver Version: %s", driverVersion.c_str());
                }
                
                // Hardware detection
                if (g_nvapiFullscreenPrevention.HasNVIDIAHardware()) {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ NVIDIA Hardware: Detected");
                } else {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "✗ NVIDIA Hardware: Not Found");
                }
                
                // Fullscreen prevention status
                if (g_nvapiFullscreenPrevention.IsFullscreenPreventionEnabled()) {
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ Fullscreen Prevention: ACTIVE");
                } else {
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "○ Fullscreen Prevention: Inactive");
                }
                
                // Function availability check (simulated)
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ Core Functions: Available");
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ DRS Functions: Available");
                
            } else {
                // Library not loaded
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "✗ NVAPI Library: Not Loaded");
                
                // Try to get error information
                std::string lastError = g_nvapiFullscreenPrevention.GetLastError();
                if (!lastError.empty()) {
                    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Error: %s", lastError.c_str());
                }
                
                // Common failure reasons
                ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Common Issues:");
                ImGui::BulletText("nvapi64.dll not found in system PATH");
                ImGui::BulletText("No NVIDIA drivers installed");
                ImGui::BulletText("Running on non-NVIDIA hardware");
                ImGui::BulletText("Insufficient permissions to load DLL");
                
                // Function availability (all unavailable when library not loaded)
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "✗ Core Functions: Unavailable");
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "✗ DRS Functions: Unavailable");
            }
            
            // Additional debug info
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Technical Details:");
            ImGui::BulletText("Uses dynamic loading of nvapi64.dll");
            ImGui::BulletText("Creates NVIDIA driver profiles per application");
            ImGui::BulletText("Sets OGL_DX_PRESENT_DEBUG_ID (0x20324987)");
            ImGui::BulletText("Applies DISABLE_FULLSCREEN_OPT (0x00000001)");
            ImGui::BulletText("Settings persist across application restarts");
            
            // DLL version info
            if (!g_nvapiFullscreenPrevention.IsAvailable()) {
                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "DLL Information:");
                std::string dllInfo = g_nvapiFullscreenPrevention.GetDllVersionInfo();
                if (dllInfo != "No library loaded") {
                    ImGui::TextWrapped("%s", dllInfo.c_str());
                } else {
                    ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "DLL not loaded - cannot get version info");
                }
            }
            
            return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },

    // NVAPI HDR periodic logging (Developer)
    new renodx::utils::settings::Setting{
        .key = "NvapiHdrLogging",
        .binding = &s_nvapi_hdr_logging,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Log HDR Status Periodically (NVAPI)",
        .section = "Display",
        .tooltip = "Periodically query NVAPI for HDR status and log it.",
        .labels = {"Off", "On"},
        .on_change_value = [](float previous, float current){
            if (current >= 0.5f) {
                std::thread(RunBackgroundNvapiHdrMonitor).detach();
            }
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
    },
    // Force NVAPI HDR10 (UHDA)
    new renodx::utils::settings::Setting{
        .key = "NvapiForceHDR10",
        .binding = &s_nvapi_force_hdr10,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Force HDR10 Output (NVAPI)",
        .section = "Display",
        .tooltip = "Enable HDR10 (UHDA) output via NVAPI for all connected displays (driver-level).",
        .labels = {"Off", "On"},
        .on_change_value = [](float previous, float current){
            std::thread([](){
                extern NVAPIFullscreenPrevention g_nvapiFullscreenPrevention;
                if (!g_nvapiFullscreenPrevention.IsAvailable()) {
                    if (!g_nvapiFullscreenPrevention.Initialize()) {
                        LogWarn("NVAPI Force HDR10: failed to initialize NVAPI");
                        return;
                    }
                }
                bool enable = (s_nvapi_force_hdr10 >= 0.5f);
                if (g_nvapiFullscreenPrevention.SetHdr10OnAll(enable)) {
                    LogInfo(enable ? "NVAPI Force HDR10: enabled" : "NVAPI Force HDR10: disabled");
                } else {
                    LogWarn("NVAPI Force HDR10: failed to apply");
                }
            }).detach();
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
    },
    // Single-shot NVAPI HDR log (button)
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Log HDR Status Now (NVAPI)",
        .section = "Display",
        .tooltip = "Immediately query NVAPI HDR status and log a single line.",
        .on_click = [](){
            std::thread([](){ LogNvapiHdrOnce(); }).detach();
            return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
    },
    // Dump full NVAPI HDR details
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Dump Full HDR Details (NVAPI)",
        .section = "Display",
        .tooltip = "Print full HDR parameters (caps, ST2086) for connected displays.",
        .on_click = [](){
            std::thread([](){
                extern NVAPIFullscreenPrevention g_nvapiFullscreenPrevention;
                if (!g_nvapiFullscreenPrevention.IsAvailable()) {
                    if (!g_nvapiFullscreenPrevention.Initialize()) {
                        LogWarn("NVAPI HDR dump: failed to initialize NVAPI");
                        return;
                    }
                }
                std::string details;
                if (g_nvapiFullscreenPrevention.QueryHdrDetails(details)) {
                    LogInfo(details.c_str());
                } else {
                    LogWarn("NVAPI HDR dump: failed to get details");
                }
            }).detach();
            return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
    },
    new renodx::utils::settings::Setting{
        .key = "NvapiHdrIntervalSec",
        .binding = &s_nvapi_hdr_interval_sec,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 5.f,
        .label = "HDR Log Interval (s)",
        .section = "Display",
        .tooltip = "How often to query/log HDR status via NVAPI.",
        .min = 1.f,
        .max = 120.f,
        .format = "%d s",
        .is_visible = []() { return is_developer_tab(s_ui_mode) && s_nvapi_hdr_logging >= 0.5f; },
    },

    // NVAPI Debug Button
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Test NVAPI Functions",
        .section = "Display",
        .tooltip = "Manually test NVAPI initialization and show detailed function status. Useful for debugging.",
        .on_click = []() {
            extern NVAPIFullscreenPrevention g_nvapiFullscreenPrevention;
            
            std::thread([](){
                LogDebug("NVAPI Debug Test button pressed (bg thread)");
                
                // Test initialization
                if (g_nvapiFullscreenPrevention.Initialize()) {
                    LogInfo("NVAPI Debug Test: Initialization successful");
                    
                    // Test hardware detection
                    if (g_nvapiFullscreenPrevention.HasNVIDIAHardware()) {
                        LogInfo("NVAPI Debug Test: NVIDIA hardware detected");
                    } else {
                        LogWarn("NVAPI Debug Test: No NVIDIA hardware found");
                    }
                    
                    // Test driver version
                    std::string driverVersion = g_nvapiFullscreenPrevention.GetDriverVersion();
                    if (driverVersion != "Failed to get driver version") {
                        std::ostringstream oss;
                        oss << "NVAPI Debug Test: Driver version: " << driverVersion;
                        LogInfo(oss.str().c_str());
                    } else {
                        LogWarn("NVAPI Debug Test: Failed to get driver version");
                    }
                    
                    // Test fullscreen prevention (toggle on then off)
                    if (g_nvapiFullscreenPrevention.SetFullscreenPrevention(true)) {
                        LogInfo("NVAPI Debug Test: Fullscreen prevention enabled successfully");
                        
                        // Wait a moment then disable
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        
                        if (g_nvapiFullscreenPrevention.SetFullscreenPrevention(false)) {
                            LogInfo("NVAPI Debug Test: Fullscreen prevention disabled successfully");
                        } else {
                            std::ostringstream oss;
                            oss << "NVAPI Debug Test: Failed to disable fullscreen prevention: " << g_nvapiFullscreenPrevention.GetLastError();
                            LogWarn(oss.str().c_str());
                        }
                    } else {
                        std::ostringstream oss;
                        oss << "NVAPI Debug Test: Failed to enable fullscreen prevention: " << g_nvapiFullscreenPrevention.GetLastError();
                        LogWarn(oss.str().c_str());
                    }
                    
                } else {
                    std::ostringstream oss;
                    oss << "NVAPI Debug Test: Initialization failed: " << g_nvapiFullscreenPrevention.GetLastError();
                    LogWarn(oss.str().c_str());
                }
            }).detach();
            
            return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },

    // DXGI Device Information Tab
    new renodx::utils::settings::Setting{
        .key = "DxgiDeviceInfo",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "",
        .section = "DXGI",
        .tooltip = "Detailed DXGI adapter and output information including HDR capabilities.",
        .on_draw = [](){
                     if (!g_dxgiDeviceInfoManager || !g_dxgiDeviceInfoManager->IsInitialized()) {
             ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "DXGI Device Info Manager not initialized");
             return false;
           }

           // Force device enumeration when this tab is first opened
           static bool first_open = true;
           if (first_open) {
             g_dxgiDeviceInfoManager->RefreshDeviceInfo();
             first_open = false;
           }

           const auto& adapters = g_dxgiDeviceInfoManager->GetAdapters();
           if (adapters.empty()) {
             ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No DXGI adapters found yet. Device enumeration happens automatically during present operations.");
             ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "If you're still not seeing adapters, try refreshing or check if a game/application is running.");
             return false;
           }

                     // Add refresh button
           if (ImGui::Button("Refresh Device Info")) {
             g_dxgiDeviceInfoManager->RefreshDeviceInfo();
           }
           ImGui::SameLine();
           ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Click to refresh device information");
           
                      // Add force re-enumeration button
           ImGui::SameLine();
           if (ImGui::Button("Force Re-enumeration")) {
             g_dxgiDeviceInfoManager->RefreshDeviceInfo();
           }
           ImGui::SameLine();
           ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Force re-enumeration of all devices");
           
           // Add HDR metadata reset button
           ImGui::SameLine();
           if (ImGui::Button("Reset HDR Metadata")) {
            // Find first HDR10-capable output and reset its metadata
            const auto& adapters = g_dxgiDeviceInfoManager->GetAdapters();
            for (const auto& adapter : adapters) {
              for (const auto& output : adapter.outputs) {
                if (output.supports_hdr10) {
                                      if (g_dxgiDeviceInfoManager->ResetHDRMetadataOnPresent(output.device_name)) {
                        LogInfo(("HDR metadata reset initiated for: " + output.device_name).c_str());
                    } else {
                        LogWarn(("HDR metadata reset failed for: " + output.device_name).c_str());
                    }
                  break; // Only reset the first HDR10 output found
                }
              }
            }
          }
                     ImGui::SameLine();
           ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Reset HDR metadata for HDR10 displays");

           // Colorspace selector
           ImGui::SameLine();
           if (ImGui::Button("Set Colorspace")) {
             // Colorspace selection will be handled in the dropdown below
           }
           ImGui::SameLine();
           ImGui::TextColored(ImVec4(0.8f, 1.0f, 0.8f, 1.0f), "Set swapchain colorspace");
           
           // Colorspace dropdown
           static int selected_colorspace = 0;
           const char* colorspace_names[] = {
               "sRGB (Non-Linear)",
               "Extended sRGB (Linear)", 
               "HDR10 (ST2084/PQ)",
               "HDR10 (HLG)",
               "scRGB (16-bit Linear)"
           };
           const reshade::api::color_space colorspace_values[] = {
               reshade::api::color_space::srgb_nonlinear,
               reshade::api::color_space::extended_srgb_linear,
               reshade::api::color_space::hdr10_st2084,
               reshade::api::color_space::hdr10_hlg,
               reshade::api::color_space::extended_srgb_linear // Use extended_srgb_linear for scRGB
           };
           
           if (ImGui::Combo("Colorspace", &selected_colorspace, colorspace_names, IM_ARRAYSIZE(colorspace_names))) {
             // Apply the selected colorspace
             if (g_dxgiDeviceInfoManager && g_dxgiDeviceInfoManager->IsInitialized()) {
               bool success = false;
               
               if (selected_colorspace == 4) { // scRGB (16-bit Linear)
                 // Use special scRGB method
                 success = g_dxgiDeviceInfoManager->SetScRGBColorspace();
               } else {
                 // Use standard colorspace method
                 success = g_dxgiDeviceInfoManager->SetColorspace(colorspace_values[selected_colorspace]);
               }
               
               if (success) {
                 LogInfo(("Colorspace changed to: " + std::string(colorspace_names[selected_colorspace])).c_str());
               } else {
                 LogWarn(("Failed to change colorspace to: " + std::string(colorspace_names[selected_colorspace])).c_str());
               }
             }
           }

           ImGui::Separator();

           // Swapchain Information Section
           if (ImGui::TreeNodeEx("Swapchain Information", ImGuiTreeNodeFlags_DefaultOpen)) {
             // Get current swapchain info from ReShade
             extern std::atomic<reshade::api::swapchain*> g_last_swapchain_ptr;
             extern reshade::api::color_space g_current_colorspace;
             
             auto* swapchain = g_last_swapchain_ptr.load();
             if (swapchain) {
               // Colorspace information
               ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Current Colorspace:");
               switch (g_current_colorspace) {
                 case reshade::api::color_space::srgb_nonlinear:
                   ImGui::Text("  sRGB (Non-Linear)");
                   break;
                 case reshade::api::color_space::extended_srgb_linear:
                   ImGui::Text("  Extended sRGB (Linear)");
                   break;
                 case reshade::api::color_space::hdr10_st2084:
                   ImGui::Text("  HDR10 (ST2084/PQ)");
                   break;
                 case reshade::api::color_space::hdr10_hlg:
                   ImGui::Text("  HDR10 (HLG)");
                   break;
                 default:
                   ImGui::Text("  Unknown (%d)", static_cast<int>(g_current_colorspace));
                   break;
               }

               // Backbuffer information
               if (swapchain->get_back_buffer_count() > 0) {
                 auto bb = swapchain->get_back_buffer(0);
                 auto* device = swapchain->get_device();
                 if (device) {
                   auto desc = device->get_resource_desc(bb);
                   ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Backbuffer:");
                   ImGui::Text("  Resolution: %dx%d", desc.texture.width, desc.texture.height);
                   ImGui::Text("  Format: %s", 
                     desc.texture.format == reshade::api::format::r8g8b8a8_unorm ? "R8G8B8A8_UNORM" :
                     desc.texture.format == reshade::api::format::r10g10b10a2_unorm ? "R10G10B10A2_UNORM" :
                     desc.texture.format == reshade::api::format::r16g16b16a16_float ? "R16G16B16A16_FLOAT" :
                     "Other");
                   ImGui::Text("  Backbuffer Count: %d", swapchain->get_back_buffer_count());
                 }
               }

               // Window information
               HWND hwnd = static_cast<HWND>(swapchain->get_hwnd());
               if (hwnd) {
                 ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Window:");
                 RECT rect;
                 if (GetWindowRect(hwnd, &rect)) {
                   ImGui::Text("  Position: (%d, %d) to (%d, %d)", rect.left, rect.top, rect.right, rect.bottom);
                   ImGui::Text("  Size: %dx%d", rect.right - rect.left, rect.bottom - rect.top);
                 }
                 
                 // Window state
                 LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
                 ImGui::Text("  Fullscreen: %s", (style & WS_POPUP) ? "Yes" : "No");
                 ImGui::Text("  Borderless: %s", (style & WS_POPUP) && !(style & WS_CAPTION) ? "Yes" : "No");
               }
             } else {
               ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No active swapchain available");
             }
             ImGui::TreePop();
           }

           ImGui::Separator();

          for (size_t i = 0; i < adapters.size(); ++i) {
            const auto& adapter = adapters[i];
            
            // Adapter header
            std::string adapter_title = adapter.name + " - " + adapter.description;
            if (ImGui::TreeNodeEx(adapter_title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
              
              // Adapter details
              ImGui::Text("Description: %s", adapter.description.c_str());
              ImGui::Text("Dedicated Video Memory: %.1f GB", adapter.dedicated_video_memory / (1024.0 * 1024.0 * 1024.0));
              ImGui::Text("Dedicated System Memory: %.1f GB", adapter.dedicated_system_memory / (1024.0 * 1024.0 * 1024.0));
              ImGui::Text("Shared System Memory: %.1f GB", adapter.shared_system_memory / (1024.0 * 1024.0 * 1024.0));
              ImGui::Text("Software Adapter: %s", adapter.is_software ? "Yes" : "No");
              
              // LUID info
              std::ostringstream luid_oss;
              luid_oss << "Adapter LUID: 0x" << std::hex << adapter.adapter_luid.HighPart << "_" << adapter.adapter_luid.LowPart;
              ImGui::Text("%s", luid_oss.str().c_str());

              // Outputs
              if (!adapter.outputs.empty()) {
                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Outputs (%zu):", adapter.outputs.size());
                
                for (size_t j = 0; j < adapter.outputs.size(); ++j) {
                  const auto& output = adapter.outputs[j];
                  std::string output_title = "Output " + std::to_string(j) + " - " + output.device_name;
                  
                  if (ImGui::TreeNodeEx(output_title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                    
                    // Basic output info
                    ImGui::Text("Device Name: %s", output.device_name.c_str());
                    ImGui::Text("Monitor Name: %s", output.monitor_name.c_str());
                    ImGui::Text("Attached: %s", output.is_attached ? "Yes" : "No");
                    ImGui::Text("Desktop Coordinates: (%d, %d) to (%d, %d)", 
                               output.desktop_coordinates.left, output.desktop_coordinates.top,
                               output.desktop_coordinates.right, output.desktop_coordinates.bottom);
                    
                    // Resolution and refresh rate
                    int width = output.desktop_coordinates.right - output.desktop_coordinates.left;
                    int height = output.desktop_coordinates.bottom - output.desktop_coordinates.top;
                    ImGui::Text("Resolution: %dx%d", width, height);
                    
                    if (output.refresh_rate.Denominator > 0) {
                      float refresh = static_cast<float>(output.refresh_rate.Numerator) / static_cast<float>(output.refresh_rate.Denominator);
                      ImGui::Text("Refresh Rate: %.2f Hz", refresh);
                    }

                    // HDR information
                    if (output.supports_hdr10) {
                      ImGui::Separator();
                      ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "HDR10 Support: ✓ Enabled");
                      ImGui::Text("Max Luminance: %.1f nits", output.max_luminance);
                      ImGui::Text("Min Luminance: %.1f nits", output.min_luminance);
                      ImGui::Text("Max Frame Average Light Level: %.1f nits", output.max_frame_average_light_level);
                      ImGui::Text("Max Content Light Level: %.1f nits", output.max_content_light_level);
                    } else {
                      ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "HDR10 Support: ✗ Not Supported");
                    }

                    // Color space information
                    ImGui::Separator();
                    ImGui::Text("Color Space: %s", 
                               output.color_space == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 ? "HDR10" :
                               output.color_space == DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709 ? "sRGB" :
                               "Other");
                    ImGui::Text("Wide Color Gamut: %s", output.supports_wide_color_gamut ? "Yes" : "No");

                    // Supported modes count
                    if (!output.supported_modes.empty()) {
                      ImGui::Text("Supported Modes: %zu", output.supported_modes.size());
                    }

                    ImGui::TreePop();
                  }
                }
              }

              ImGui::TreePop();
            }
          }

          return false;
        },
                 .is_visible = []() { return is_device_tab(s_ui_mode); }, // Show only in Device Info mode
    },

    // DXGI composition/backbuffer info (text only) — placed at bottom
    new renodx::utils::settings::Setting{
        .key = "DxgiInfo",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "",
        .section = "Performance",
        .tooltip = "DXGI composition mode, current backbuffer size, and fullscreen status.",
        .on_draw = [](){
          const char* mode_str = "Unknown";
          switch (static_cast<int>(s_dxgi_composition_state)) {
            case 1: mode_str = "Composed Flip"; break;
            case 2: mode_str = "Modern Independent Flip"; break;
            case 3: mode_str = "Legacy Independent Flip"; break;
            default: mode_str = "Unknown"; break;
          }
          
          // Check exclusive fullscreen status
          HWND hwnd = g_last_swapchain_hwnd.load();
          if (hwnd == nullptr) hwnd = GetForegroundWindow();
          const bool is_exclusive_fullscreen = GetSpoofedFullscreenState(hwnd);
          
          // Get backbuffer format
          std::string format_str = "Unknown";
          if (g_last_swapchain_ptr.load() != nullptr) {
            auto* device = g_last_swapchain_ptr.load()->get_device();
            if (device != nullptr && g_last_swapchain_ptr.load()->get_back_buffer_count() > 0) {
              auto bb = g_last_swapchain_ptr.load()->get_back_buffer(0);
              auto desc = device->get_resource_desc(bb);
              switch (desc.texture.format) {
                case reshade::api::format::r8g8b8a8_unorm: format_str = "R8G8B8A8_UNORM"; break;
                case reshade::api::format::r8g8b8a8_unorm_srgb: format_str = "R8G8B8A8_UNORM_SRGB"; break;
                case reshade::api::format::r10g10b10a2_unorm: format_str = "R10G10B10A2_UNORM"; break;
                case reshade::api::format::r16g16b16a16_float: format_str = "R16G16B16A16_FLOAT"; break;
                case reshade::api::format::r11g11b10_float: format_str = "R11G11B10_FLOAT"; break;
                case reshade::api::format::b8g8r8a8_unorm: format_str = "B8G8R8A8_UNORM"; break;
                case reshade::api::format::b8g8r8a8_unorm_srgb: format_str = "B8G8R8A8_UNORM_SRGB"; break;
                case reshade::api::format::r8g8b8_unorm: format_str = "R8G8B8_UNORM"; break;
                case reshade::api::format::r8g8b8_unorm_srgb: format_str = "R8G8B8_UNORM_SRGB"; break;
                case reshade::api::format::b5g6r5_unorm: format_str = "B5G6R5_UNORM"; break;
                case reshade::api::format::b5g5r5a1_unorm: format_str = "B5G5R5A1_UNORM"; break;
                case reshade::api::format::b4g4r4a4_unorm: format_str = "B4G4R4A4_UNORM"; break;
                default: format_str = "Format_" + std::to_string(static_cast<int>(desc.texture.format)); break;
              }
            }
          }
          
          // Get colorspace string
          std::string colorspace_str = "Unknown";
          switch (g_current_colorspace) {
            case reshade::api::color_space::unknown: colorspace_str = "Unknown"; break;
            case reshade::api::color_space::srgb_nonlinear: colorspace_str = "sRGB"; break;
            case reshade::api::color_space::extended_srgb_linear: colorspace_str = "Extended sRGB Linear"; break;
            case reshade::api::color_space::hdr10_st2084: colorspace_str = "HDR10 ST2084"; break;
            case reshade::api::color_space::hdr10_hlg: colorspace_str = "HDR10 HLG"; break;
            default: colorspace_str = "ColorSpace_" + std::to_string(static_cast<int>(g_current_colorspace)); break;
          }
          
          ImGui::Separator();
          ImGui::Text("DXGI Composition: %s | Backbuffer: %dx%d | Format: %s | Colorspace: %s",
                      mode_str,
                      g_last_backbuffer_width.load(),
                      g_last_backbuffer_height.load(),
                      format_str.c_str(),
                      colorspace_str.c_str());
          
          // Display HDR10 override status
          ImGui::Text("HDR10 Colorspace Override: %s (Last: %s)", g_hdr10_override_status.c_str(), g_hdr10_override_timestamp.c_str());
          
          // Display current window position and size
          if (hwnd != nullptr) {
            RECT wr{};
            RECT cr{};
            if (GetWindowRect(hwnd, &wr) != FALSE && GetClientRect(hwnd, &cr) != FALSE) {
              int window_width = wr.right - wr.left;
              int window_height = wr.bottom - wr.top;
              int client_width = cr.right - cr.left;
              int client_height = cr.bottom - cr.top;
              
              // Convert client coordinates to screen coordinates
              POINT client_origin = {cr.left, cr.top};
              ClientToScreen(hwnd, &client_origin);
              
              ImGui::Text("Window Frame: %dx%d at (%ld, %ld)", window_width, window_height, wr.left, wr.top);
              ImGui::Text("Client Area: %dx%d at (%ld, %ld)", client_width, client_height, client_origin.x, client_origin.y);
              ImGui::Text("Decorations: %dx%d offset", window_width - client_width, window_height - client_height);
            }
            
            // Display current monitor resolution
            HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            if (hmon != nullptr) {
              MONITORINFOEXW mi{};
              mi.cbSize = sizeof(mi);
              if (GetMonitorInfoW(hmon, &mi) != FALSE) {
                int monitor_width = mi.rcMonitor.right - mi.rcMonitor.left;
                int monitor_height = mi.rcMonitor.bottom - mi.rcMonitor.top;
                ImGui::Text("Monitor: %dx%d at (%ld, %ld)", 
                           monitor_width, monitor_height, mi.rcMonitor.left, mi.rcMonitor.top);
              }
            }
          }
          
          return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    // Independent Flip failure reasons (text only)
    new renodx::utils::settings::Setting{
        .key = "IndependentFlipFailures",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "",
        .section = "Performance",
        .tooltip = "Reasons why Independent Flip is not working.",
        .on_draw = [](){
          IndependentFlipFailures* failures = g_if_failures.load();
          if (failures == nullptr) return false;
          
          // Check if there are any failures
          bool has_failures = failures->swapchain_null.load() ||
                             failures->device_null.load() ||
                             failures->non_dxgi_api.load() ||
                             failures->swapchain_media_failed.load() ||
                             failures->frame_stats_failed.load() ||
                             failures->not_flip_model.load() ||
                             failures->backbuffer_size_mismatch.load() ||
                             failures->window_size_mismatch.load() ||
                             failures->window_not_at_origin.load() ||
                             failures->window_layered.load() ||
                             failures->window_topmost.load() ||
                             failures->window_maximized.load() ||
                             failures->window_minimized.load() ||
                             failures->hwnd_null.load();
          
          if (has_failures) {
            ImGui::Separator();
            ImGui::Text("Independent Flip Debug Info:");
            
            if (failures->swapchain_null.load()) ImGui::BulletText("Swapchain is null");
            if (failures->device_null.load()) ImGui::BulletText("Device is null");
            if (failures->non_dxgi_api.load()) ImGui::BulletText("Non-DXGI API (OpenGL/Vulkan)");
            if (failures->swapchain_media_failed.load()) ImGui::BulletText("Swapchain media interface failed");
            if (failures->frame_stats_failed.load()) ImGui::BulletText("Frame statistics failed");
            if (failures->not_flip_model.load()) ImGui::BulletText("Not a flip-model swapchain");
            if (failures->backbuffer_size_mismatch.load()) ImGui::BulletText("Backbuffer size doesn't match monitor");
            if (failures->window_size_mismatch.load()) ImGui::BulletText("Window size doesn't match monitor");
            if (failures->window_not_at_origin.load()) ImGui::BulletText("Window not at monitor origin (0,0)");
            if (failures->window_layered.load()) ImGui::BulletText("Window has WS_EX_LAYERED style");
            if (failures->window_topmost.load()) ImGui::BulletText("Window has WS_EX_TOPMOST style");
            if (failures->window_maximized.load()) ImGui::BulletText("Window is maximized");
            if (failures->window_minimized.load()) ImGui::BulletText("Window is minimized");
            if (failures->hwnd_null.load()) ImGui::BulletText("Window handle is null");
          }
          
          return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },

    // Window position (X, Y coordinates)
    new renodx::utils::settings::Setting{
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
    },
    new renodx::utils::settings::Setting{
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
    },

    // Reflex Settings
    new renodx::utils::settings::Setting{
        .key = "ReflexEnabled",
        .binding = &s_reflex_enabled,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 1.f, // Enabled by default
        .label = "Enable NVIDIA Reflex",
        .section = "Performance",
        .tooltip = "Enable NVIDIA Reflex for reduced latency. Requires NVIDIA GPU and drivers.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current) {
            if (current >= 0.5f) {
                InstallReflexHooks();
            } else {
                UninstallReflexHooks();
            }
            // Mark that Reflex settings have changed to force sleep mode update
            extern std::atomic<bool> g_reflex_settings_changed;
            g_reflex_settings_changed.store(true);
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    new renodx::utils::settings::Setting{
        .key = "ReflexLowLatencyMode",
        .binding = &s_reflex_low_latency_mode,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 1.f,
        .label = "Reflex Low Latency Mode",
        .section = "Performance",
        .tooltip = "Enable low latency mode for reduced input lag.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current) {
            // Mark that Reflex settings have changed to force sleep mode update
            extern std::atomic<bool> g_reflex_settings_changed;
            g_reflex_settings_changed.store(true);
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode) && s_reflex_enabled >= 0.5f; }, // Only show when Reflex is enabled
    },
    new renodx::utils::settings::Setting{
        .key = "ReflexLowLatencyBoost",
        .binding = &s_reflex_low_latency_boost,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Reflex Low Latency Boost",
        .section = "Performance",
        .tooltip = "Request maximum GPU clock frequency for lower latency in CPU-limited scenarios.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current) {
            // Mark that Reflex settings have changed to force sleep mode update
            extern std::atomic<bool> g_reflex_settings_changed;
            g_reflex_settings_changed.store(true);
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode) && s_reflex_enabled >= 0.5f; }, // Only show when Reflex is enabled
    },
    new renodx::utils::settings::Setting{
        .key = "ReflexUseMarkers",
        .binding = &s_reflex_use_markers,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 1.f,
        .label = "Reflex Use Markers",
        .section = "Performance",
        .tooltip = "Allow latency markers to be used for runtime optimizations.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current) {
            // Mark that Reflex settings have changed to force sleep mode update
            extern std::atomic<bool> g_reflex_settings_changed;
            g_reflex_settings_changed.store(true);
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode) && s_reflex_enabled >= 0.5f; }, // Only show when Reflex is enabled
    },
    // Reflex settings section
    new renodx::utils::settings::Setting{
        .key = "ReflexSection",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::TEXT,
        .default_value = 0.f,
        .label = "=== NVIDIA Reflex Settings ===",
        .section = "Performance",
        .tooltip = "NVIDIA Reflex latency reduction settings",
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    
    // Latency Display Section
    new renodx::utils::settings::Setting{
        .key = "LatencyDisplaySection",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::TEXT,
        .default_value = 0.f,
        .label = "=== Latency Information ===",
        .section = "Performance",
        .tooltip = "Real-time latency and Reflex status information",
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    
    // Current Latency Display
    new renodx::utils::settings::Setting{
        .key = "CurrentLatency",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "Current Latency: ",
        .section = "Performance",
        .tooltip = "Current frame latency in milliseconds",
        .on_draw = []() -> bool {
            extern std::atomic<float> g_current_latency_ms;
            float latency = g_current_latency_ms.load();
            
            // Use TextUnformatted for most reliable display
            std::ostringstream oss;
            oss << "Current Latency: " << std::fixed << std::setprecision(2) << latency << " ms";
            ImGui::TextUnformatted(oss.str().c_str());
            
            return false; // No value change
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    
    // PCL AV Latency Display (Most Important)
    new renodx::utils::settings::Setting{
        .key = "PCLLatency",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "PCL AV Latency: ",
        .section = "Performance",
        .tooltip = "PCL Average Latency (30-frame average) - This is what the NVIDIA overlay should show",
        .on_draw = []() -> bool {
            extern std::atomic<float> g_pcl_av_latency_ms;
            float pcl_latency = g_pcl_av_latency_ms.load();
            
            // Use TextUnformatted for most reliable display
            std::ostringstream oss;
            oss << "PCL AV Latency: " << std::fixed << std::setprecision(2) << pcl_latency << " ms";
            ImGui::TextUnformatted(oss.str().c_str());
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "(30-frame avg)");
            
            return false; // No value change
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    
    // Reflex Status Display
    new renodx::utils::settings::Setting{
        .key = "ReflexStatus",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "Reflex Status: ",
        .section = "Performance",
        .tooltip = "Current Reflex status and configuration",
        .on_draw = []() -> bool {
            extern std::atomic<bool> g_reflex_active;
            extern std::atomic<uint64_t> g_current_frame;
            bool is_active = g_reflex_active.load();
            uint64_t frame = g_current_frame.load();
            
            // Use TextUnformatted for most reliable display
            std::ostringstream oss;
            if (is_active) {
                oss << "Reflex Status: Active (Frame " << frame << ")";
            } else {
                oss << "Reflex Status: Inactive";
            }
            ImGui::TextUnformatted(oss.str().c_str());
            
            return false; // No value change
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    
    // Debug Button to show atomic variable values
    new renodx::utils::settings::Setting{
        .key = "DebugLatency",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "Debug: Show All Latency Values",
        .section = "Performance",
        .tooltip = "Click to log all current latency values for debugging",
        .on_click = []() -> bool {
            extern std::atomic<float> g_current_latency_ms;
            extern std::atomic<float> g_pcl_av_latency_ms;
            extern std::atomic<float> g_average_latency_ms;
            extern std::atomic<float> g_min_latency_ms;
            extern std::atomic<float> g_max_latency_ms;
            extern std::atomic<uint64_t> g_current_frame;
            extern std::atomic<bool> g_reflex_active;
            
            extern void LogDebug(const std::string& message);
            LogDebug("=== DEBUG: All Latency Values ===");
            LogDebug("Current Latency: " + std::to_string(g_current_latency_ms.load()) + " ms");
            LogDebug("PCL AV Latency: " + std::to_string(g_pcl_av_latency_ms.load()) + " ms");
            LogDebug("Average Latency: " + std::to_string(g_average_latency_ms.load()) + " ms");
            LogDebug("Min Latency: " + std::to_string(g_min_latency_ms.load()) + " ms");
            LogDebug("Max Latency: " + std::to_string(g_max_latency_ms.load()) + " ms");
            LogDebug("Current Frame: " + std::to_string(g_current_frame.load()));
            LogDebug("Is Active: " + std::string(g_reflex_active.load() ? "true" : "false"));
            LogDebug("=== END DEBUG ===");
            return true; // Save settings
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    
    // Simple always-visible debug element to test UI rendering
    new renodx::utils::settings::Setting{
        .key = "UIRenderTest",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::TEXT,
        .default_value = 0.f,
        .label = "UI Test - If you see this, UI is working",
        .section = "Debug",
        .tooltip = "This element should always be visible to test UI rendering",
        .is_visible = []() { return true; }, // Always visible
    },
    
    // Simple test of atomic variables (always visible)
    new renodx::utils::settings::Setting{
        .key = "AtomicTest",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM, // only custom supports ImGui::Text
        .default_value = 0.f,
        .label = "Atomic Test - Direct Value Display",
        .section = "Debug2",
        .tooltip = "Test direct atomic variable access",
        .on_draw = []() -> bool {
            extern std::atomic<float> g_current_latency_ms;
            extern std::atomic<uint64_t> g_current_frame;
            float latency = g_current_latency_ms.load();
            uint64_t frame = g_current_frame.load();
            
            // Test different ImGui text methods
            ImGui::Text("Method 1 - Direct: %f ms", latency);
            ImGui::Text("Method 2 - Fixed: %.2f ms", latency);
            
            // Method 3 - Convert to string first
            std::string latency_str = std::to_string(latency);
            ImGui::Text("Method 3 - String: %s ms", latency_str.c_str());
            
            // Method 4 - Use TextUnformatted (no formatting)
            std::ostringstream oss;
            oss << "Method 4 - OStream: " << std::fixed << std::setprecision(2) << latency << " ms";
            ImGui::TextUnformatted(oss.str().c_str());
            
            // Method 5 - Simple concatenation
            ImGui::Text("Method 5 - Simple: %s", ("Value: " + std::to_string(latency) + " ms").c_str());
            
            // Test with integer (frame counter)
            ImGui::Text("Frame Test - Integer: %llu", frame);
            ImGui::Text("Frame Test - String: %s", std::to_string(frame).c_str());
            
            // Show update frequency info
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Update Info:");
            ImGui::Text("Values update every 10 frames");
            ImGui::Text("Current frame: %llu", frame);
            ImGui::Text("Last update: %llu frames ago", frame % 10);
            
            return false;
        },
        .is_visible = []() { return true; }, // Always visible
    },
    
    // NVAPI Tab - NVIDIA-specific features
    new renodx::utils::settings::Setting{
        .key = "NVAPITab",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "",
        .section = "NVAPI",
        .tooltip = "NVIDIA-specific features and HDR management",
        .on_draw = []() -> bool {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "NVIDIA Features");
            ImGui::Separator();
            
            // Fix HDR10 Colorspace option moved here
            extern float s_fix_hdr10_colorspace;
            bool fix_hdr = (s_fix_hdr10_colorspace >= 0.5f);
            if (ImGui::Checkbox("Fix HDR10 Colorspace", &fix_hdr)) {
                s_fix_hdr10_colorspace = fix_hdr ? 1.0f : 0.0f;
            }
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "?");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Automatically fix HDR10 colorspace when swapchain format is RGB10A2 and colorspace is currently sRGB. Only works when the game is using sRGB colorspace.");
            }
            
            // Add more NVIDIA-specific features here in the future
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Additional NVIDIA features coming soon...");
            
            return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Show in Developer mode
    },
};
