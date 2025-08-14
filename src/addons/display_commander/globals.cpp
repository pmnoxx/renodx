#include "addon.hpp"

// Global variable definitions
float s_auto_apply_enabled = 0.f; // 0 off, 1 on
float s_auto_apply_delay_sec = 10.f; // 1..60 seconds (app start delay)
float s_auto_apply_init_delay_sec = 1.f; // 1..60 seconds (swapchain init delay)

// Selected numeric windowed size
float s_windowed_width = 1280.f;
float s_windowed_height = 720.f;
// Current window position baseline
float s_windowed_pos_x = 100.f;
float s_windowed_pos_y = 100.f;

// Style preference
float s_remove_top_bar = 1.f; // 0 keep title bar, 1 remove (borderless)
// Resize mode: 0 = width/height, 1 = aspect
float s_resize_mode = 0.f;
// Aspect selection index when in aspect mode
float s_aspect_index = 3.f; // default to 16:9 (now at index 3 after sorting)
// Window alignment when repositioning is needed
float s_move_to_zero_if_out = 1.f; // 0 = None, 1 = Center, 2 = Top Left, 3 = Top Right, 4 = Bottom Left, 5 = Bottom Right
// Global swapchain behaviors
float s_force_borderless = 0.f;
// Force Windowed (Experimental)
float s_force_windowed = 0.f; // 0 = Off, 1 = On

// Prevent Fullscreen
float s_prevent_fullscreen = 0.f; // 0 = Off, 1 = On

// NVAPI Fullscreen Prevention
float s_nvapi_fullscreen_prevention = 1.f; // 0 = Off, 1 = On (enabled by default)

// Spoof Fullscreen State (for applications that query fullscreen status)
float s_spoof_fullscreen_state = 0.f; // 0 = Off, 1 = On (spoof as fullscreen), 2 = On (spoof as windowed)

// Audio: mute when in background
float s_mute_in_background = 0.f;
// Audio: master volume (0..100) and manual mute toggle
float s_audio_volume_percent = 100.f;
float s_audio_mute = 0.f;
// Performance: background FPS cap
float s_fps_limit_background = 30.f;
// Monitor targeting (0 = Auto/current). Values > 0 map to enumerated monitors.
float s_target_monitor_index = 0.f;
// UI: DXGI composition state (0 Unknown, 1 Composed, 2 Overlay, 3 IndependentFlip)
float s_dxgi_composition_state = 0.f;
std::atomic<int> g_comp_query_counter{0};
std::atomic<int> g_comp_last_logged{-1};
// Try to configure swapchain for Independent Flip using DXGI-only changes
// float s_try_independent_flip = 0.f; // Removed - no longer used

// Suppress Alt-Tab (by installing Windows hook)
float s_suppress_alt_tab = 0.f; // 0 = Off, 1 = On

// Suppress Alt modifier keys (when Alt is used with other keys)
float s_suppress_alt_modifier = 0.f; // 0 = Off, 1 = On

// Suppress Tab key (standalone Tab key presses)
float s_suppress_tab = 0.f; // 0 = Off, 1 = On

// Alt suppression methods
float s_alt_suppression_method = 0.f; // 0 = Hook only, 1 = Hook + Window subclass, 2 = Hook + DLL injection, 3 = Hook + Raw Input, 4 = All methods

// Additional Alt suppression via window subclassing
float s_alt_suppression_window_subclass = 0.f; // 0 = Off, 1 = On

// Additional Alt suppression via DLL injection (experimental)
float s_alt_suppression_dll_injection = 0.f; // 0 = Off, 1 = On

// Intercept Raw Input for Alt suppression (for games that bypass Windows hooks)
float s_alt_suppression_raw_input = 0.f; // 0 = Off, 1 = On

// Alt-Tab suppression counter
std::atomic<uint64_t> g_alt_tab_suppressed_count{0};

// Prevent Windows Minimize (by installing Windows hook)
float s_prevent_windows_minimize = 0.f; // 0 = Off, 1 = On

// Spoof Window Focus (for applications that query focus status)
float s_spoof_window_focus = 0.f; // 0 = Off, 1 = On (spoof as focused), 2 = On (spoof as unfocused)

// Display: Fix HDR10 color space via DXGI SetColorSpace1 when using RGB10A2
float s_fix_hdr10_colorspace = 1.f; // 0 = Off, 1 = On

// Current swapchain colorspace for UI display
reshade::api::color_space g_current_colorspace = reshade::api::color_space::unknown;

// HDR10 colorspace override status for UI display
std::string g_hdr10_override_status = "Not attempted";

// HDR10 colorspace override timestamp for UI display
std::string g_hdr10_override_timestamp = "Never";

// Last known swapchain pointer (for composition state queries)
std::atomic<reshade::api::swapchain*> g_last_swapchain_ptr{nullptr};

// No persistent window handle; resolve on demand

// Implementation of IndependentFlipFailures::reset()
void IndependentFlipFailures::reset() {
    swapchain_null.store(false);
    device_null.store(false);
    non_dxgi_api.store(false);
    swapchain_media_failed.store(false);
    frame_stats_failed.store(false);
    not_flip_model.store(false);
    backbuffer_size_mismatch.store(false);
    window_size_mismatch.store(false);
    window_not_at_origin.store(false);
    window_layered.store(false);
    window_topmost.store(false);
    window_maximized.store(false);
    window_minimized.store(false);
    hwnd_null.store(false);
}

std::atomic<IndependentFlipFailures*> g_if_failures{nullptr};

// Additional global variable definitions
std::atomic<uint64_t> g_init_apply_generation{0};
std::chrono::steady_clock::time_point g_attach_time;
std::atomic<HWND> g_last_swapchain_hwnd{nullptr};
std::atomic<bool> g_shutdown{false};
std::atomic<bool> g_muted_applied{false};
std::atomic<float> g_default_fps_limit{0.0f};

std::vector<MonitorInfo> g_monitors;

// Global variable definitions
std::atomic<int> g_last_backbuffer_width{0};
std::atomic<int> g_last_backbuffer_height{0};
