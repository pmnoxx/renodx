#include "addon.hpp"
#include "utils.hpp"

// UI/settings
renodx::utils::settings::Settings settings = {
    // Auto-apply toggle and delay (seconds)
    new renodx::utils::settings::Setting{
        .key = "AutoApply",
        .binding = &s_auto_apply_enabled,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Auto Apply",
        .section = "Display",
        .labels = {"Off", "On"},
    },
    new renodx::utils::settings::Setting{
        .key = "AutoApplyDelay",
        .binding = &s_auto_apply_delay_sec,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 10.f,
        .label = "App Start Delay (s)",
        .section = "Display",
        .min = 1.f,
        .max = 60.f,
        .format = "%d s",
        .is_enabled = [](){ return s_auto_apply_enabled >= 0.5f; },
    },
    new renodx::utils::settings::Setting{
        .key = "InitApplyDelay",
        .binding = &s_auto_apply_init_delay_sec,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 1.f,
        .label = "SwapChain Init Delay (s)",
        .section = "Display",
        .min = 1.f,
        .max = 60.f,
        .format = "%d s",
        .is_enabled = [](){ return s_auto_apply_enabled >= 0.5f; },
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
        .is_enabled = [](){ return s_resize_mode < 0.5f; },
        .parse = [](float index) {
          int i = static_cast<int>(index);
          i = (std::max)(i, 0);
          int max_i = 6;
          i = (std::min)(i, max_i);
          return static_cast<float>(HEIGHT_OPTIONS[i]);
        },
    },
    // Resize Mode
    new renodx::utils::settings::Setting{
        .key = "ResizeMode",
        .binding = &s_resize_mode,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Resize Mode",
        .section = "Display",
        .labels = {"Width/Height", "Aspect"},
    },
    // Aspect Ratio (only when in Aspect mode)
    new renodx::utils::settings::Setting{
        .key = "AspectRatio",
        .binding = &s_aspect_index,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 3.f, // 16:9 (now at index 3 after sorting)
        .label = "Aspect Ratio",
        .section = "Display",
        .labels = MakeAspectLabels(),
        .is_enabled = [](){ return s_resize_mode >= 0.5f; },
    },

    // FPS Limit (foreground default)
    new renodx::utils::settings::Setting{
        .key = "FPSLimit",
        .binding = &renodx::utils::swapchain::fps_limit,
        .default_value = 0.f,
        .label = "FPS Limit",
        .section = "Performance",
        .min = 0.f,
        .max = 480.f,
        .format = "%.0f FPS",
        .on_change_value = [](float previous, float current){ g_default_fps_limit.store(current); },
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
    },
    // Volume (0-100)
    new renodx::utils::settings::Setting{
        .key = "Volume",
        .binding = &s_audio_volume_percent,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 100.f,
        .label = "Volume",
        .section = "Audio",
        .min = 0.f,
        .max = 100.f,
        .format = "%d%%",
        .on_change_value = [](float previous, float current){ SetVolumeForCurrentProcess(current); },
    },
    // Mute (manual)
    new renodx::utils::settings::Setting{
        .key = "Mute",
        .binding = &s_audio_mute,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Mute",
        .section = "Audio",
        .labels = {"Off", "On"},
        .on_change_value = [](float previous, float current){ SetMuteForCurrentProcess(current >= 0.5f); },
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
        .is_enabled = [](){ return s_audio_mute < 0.5f; },
        .on_change_value = [](float previous, float current){
          // Reset applied flag so the monitor thread reapplies desired state
          g_muted_applied.store(false);
        },
    },
    // Apply button executes windowed resize
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Apply",
        .section = "Display",
        .tooltip = "Apply the selected window width and height (respects Remove Top Bar).",
        .on_click = []() {
          std::thread([](){
            HWND hwnd = g_last_swapchain_hwnd.load();
            if (hwnd == nullptr) hwnd = GetForegroundWindow();
            if (hwnd == nullptr) return;
            DWORD owner_tid = GetWindowThreadProcessId(hwnd, nullptr);
            DWORD this_tid = GetCurrentThreadId();
            RECT wr{};
            int pos_x = 100;
            int pos_y = 100;
            if (GetWindowRect(hwnd, &wr) != FALSE) {
              pos_x = wr.left;
              pos_y = wr.top;
            }
            WindowStyleMode mode = (s_remove_top_bar >= 0.5f) ? WindowStyleMode::BORDERLESS : WindowStyleMode::OVERLAPPED_WINDOW;
            int want_w = 0; int want_h = 0; ComputeDesiredSize(want_w, want_h);
            
            std::ostringstream oss; oss << "Apply (bg) clicked hwnd=" << hwnd
                                        << " owner_tid=" << owner_tid
                                        << " this_tid=" << this_tid
                                        << " size=" << want_w << "x" << want_h;
            LogDebug(oss.str());
            ApplyWindowChange(
                hwnd,
                /*do_resize=*/true, want_w, want_h,
                /*do_move=*/true, pos_x, pos_y,
                mode);
          }).detach();
          return false; // do not trigger settings save
        },
    },
    // Remove Top Bar (for Force Windowed)
    new renodx::utils::settings::Setting{
        .key = "RemoveTopBar",
        .binding = &s_remove_top_bar,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 1.f,
        .label = "Remove Top Bar",
        .section = "Display",
        .tooltip = "When forcing windowed mode, choose whether to remove the title bar (borderless).",
        .labels = {"Keep", "Remove"},
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
    },
    // Window alignment when repositioning is needed
    new renodx::utils::settings::Setting{
        .key = "Alignment",
        .binding = &s_move_to_zero_if_out,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 1.f, // default to center
        .label = "Alignment",
        .section = "Display",
        .tooltip = "Choose how to align the window when repositioning is needed. TL=Top Left, TR=Top Right, BL=Bottom Left, BR=Bottom Right.",
        .labels = {"None", "Center", "Top Left", "Top Right", "Bottom Left", "Bottom Right"},
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
            
            // Install or uninstall the hook based on the new setting
            if (current >= 0.5f) {
                InstallAltTabHook();
            } else {
                UninstallAltTabHook();
            }
        },
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
    },
    // Force Windowed (Experimental)
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Force Windowed (Experimental)",
        .section = "Display",
        .tooltip = "Attempt to force overlapped window styles and then apply size.",
        .on_click = [](){
          std::thread([](){
            HWND hwnd = g_last_swapchain_hwnd.load();
            if (hwnd == nullptr) hwnd = GetForegroundWindow();
            if (hwnd == nullptr) return;
            LogDebug("Force Windowed button pressed (bg thread)");
            // Only one call: change style per setting, keep current size/pos
            WindowStyleMode mode = (s_remove_top_bar >= 0.5f) ? WindowStyleMode::BORDERLESS : WindowStyleMode::OVERLAPPED_WINDOW;
            ApplyWindowChange(
                hwnd,
                /*do_resize=*/false, 0, 0,
                /*do_move=*/false, 0, 0,
                mode);
          }).detach();
          return false; // do not save on button click
        },
    },

    // Try Independent Flip (Experimental)
    new renodx::utils::settings::Setting{
        .key = "TryIndependentFlip",
        .binding = &s_try_independent_flip,
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .default_value = 0.f,
        .label = "Experimental: Try Fixing Independent Flip",
        .section = "Performance",
        .tooltip = "Attempt to configure swapchain for Independent Flip using DXGI-only changes (no window ops).",
        .on_click = [](){
            auto* sc = g_last_swapchain_ptr.load();
            if (sc != nullptr) {
              SetIndependentFlipState(sc);
              LogIndependentFlipConditions(sc);
            } else {
              LogWarn("Try Independent Flip: no swapchain yet");
            }
            return false;
          },
    },
    new renodx::utils::settings::Setting{
        .key = "ForceWindowToOrigin",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .default_value = 0.f,
        .label = "Force Window to Monitor Origin (experimental)",
        .section = "Performance",
        .tooltip = "Force the window to be positioned so its client area starts at monitor origin (0,0). Required for Independent Flip.",
        .on_click = [](){
            HWND hwnd = g_last_swapchain_hwnd.load();
            if (hwnd != nullptr) {
              ForceWindowToMonitorOriginThreaded(hwnd);
            } else {
              LogWarn("Force Window to Origin: no window handle yet");
            }
            return false;
          },
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
    },
    // Test button for RemoveWindowBorder
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Test RemoveWindowBorderLocal",
        .section = "Display",
        .tooltip = "Test button that calls RemoveWindowBorderLocal in another thread.",
        .on_click = []() {
          std::thread([](){
            HWND hwnd = g_last_swapchain_hwnd.load();
            if (hwnd == nullptr) hwnd = GetForegroundWindow();
            if (hwnd == nullptr) {
              LogWarn("Test RemoveWindowBorder: no window handle available");
              return;
            }
            LogDebug("Test RemoveWindowBorder button pressed (bg thread)");
            RemoveWindowBorderLocal(hwnd);
            LogInfo("Test RemoveWindowBorderLocal completed");
          }).detach();
          return false; // do not save on button click
        },
    },
    // Log Window State Changes
    new renodx::utils::settings::Setting{
        .key = "LogWindowStateChanges",
        .binding = &s_log_window_state_changes,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Log Window State Changes",
        .section = "Debug",
        .tooltip = "Log every window state change (move, resize, minimize, maximize, style changes, etc.) for debugging and monitoring.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current){ 
            // Update the window state logging state
            std::ostringstream oss;
            oss << "Window state change logging changed from " << (previous >= 0.5f ? "enabled" : "disabled") << " to " << (current >= 0.5f ? "enabled" : "disabled");
            LogInfo(oss.str().c_str());
            
            // Install or uninstall the hook based on the new setting
            if (current >= 0.5f) {
                InstallWindowStateLoggingHook();
            } else {
                UninstallWindowStateLoggingHook();
            }
        },
    },
    // Fix HDR10 Colorspace
    new renodx::utils::settings::Setting{
        .key = "FixHDR10Colorspace",
        .binding = &s_fix_hdr10_colorspace,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 1.f,
        .label = "Fix HDR10 Colorspace",
        .section = "Display",
        .tooltip = "Automatically fix HDR10 colorspace when swapchain format is RGB10A2 and colorspace is currently sRGB. Only works when the game is using sRGB colorspace.",
        .labels = {"Off", "On"},
    },
};
