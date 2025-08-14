#include "addon.hpp"
#include "reflex_management.hpp"

// Global variables
// UI mode (0 = Basic, 1 = Developer)
float s_ui_mode = 0.f; // Basic mode by default

// Auto-apply settings
float s_auto_apply_enabled = 0.f;
float s_auto_apply_delay_sec = 10.f;
float s_auto_apply_init_delay_sec = 1.f;

// Window settings
float s_windowed_width = 1920.f;
float s_windowed_height = 1080.f;
float s_windowed_pos_x = 0.f;
float s_windowed_pos_y = 0.f;
float s_remove_top_bar = 0.f;
float s_resize_mode = 0.f;
float s_aspect_index = 3.f; // 16:9

// Window alignment when repositioning is needed (0 = None, 1 = Top Left, 2 = Top Right, 3 = Bottom Left, 4 = Bottom Right)
float s_move_to_zero_if_out = 2.f; // default to top right
float s_force_borderless = 0.f;

// Prevent Fullscreen
float s_prevent_fullscreen = 0.f;

// NVAPI Fullscreen Prevention
float s_nvapi_fullscreen_prevention = 1.f; // enabled by default
// NVAPI HDR logging
float s_nvapi_hdr_logging = 0.f;
float s_nvapi_hdr_interval_sec = 5.f;
// NVAPI Force HDR10
float s_nvapi_force_hdr10 = 0.f;

// Spoof Fullscreen State (for applications that query fullscreen status)
float s_spoof_fullscreen_state = 0.f;
float s_mute_in_background = 0.f;
float s_audio_volume_percent = 100.f;
float s_audio_mute = 0.f;

// Performance: background FPS cap
float s_fps_limit_background = 30.f;

// FPS limit for foreground
float s_fps_limit = 0.f;

// Monitor and display settings
float s_target_monitor_index = 0.f;
float s_dxgi_composition_state = 0.f;
float s_suppress_alt_tab = 0.f;
float s_prevent_windows_minimize = 0.f;
float s_spoof_window_focus = 0.f;
float s_enforce_desired_window = 0.f;

// Fix HDR10 color space when backbuffer is RGB10A2
float s_fix_hdr10_colorspace = 0.f;

// Reflex settings
float s_reflex_enabled = 1.f; // Enabled by default
float s_reflex_low_latency_mode = 1.f; // Low latency mode enabled by default
float s_reflex_low_latency_boost = 0.f; // Boost disabled by default
float s_reflex_use_markers = 1.f; // Use markers enabled by default

// Atomic variables
std::atomic<int> g_comp_query_counter{0};
std::atomic<int> g_comp_last_logged{0};
std::atomic<reshade::api::swapchain*> g_last_swapchain_ptr{nullptr};
std::atomic<IndependentFlipFailures*> g_if_failures{nullptr};
std::atomic<uint64_t> g_init_apply_generation{0};
std::chrono::steady_clock::time_point g_attach_time{std::chrono::steady_clock::now()};
std::atomic<HWND> g_last_swapchain_hwnd{nullptr};
std::atomic<bool> g_shutdown{false};
std::atomic<bool> g_muted_applied{false};
std::atomic<float> g_default_fps_limit{0.f};

// Reflex-related atomic variables
std::atomic<uint64_t> g_reflex_frame_id{0};
std::atomic<bool> g_reflex_hooks_installed{false};

// Global Reflex manager instance
std::unique_ptr<ReflexManager> g_reflexManager;

// Global flag for Reflex settings changes
std::atomic<bool> g_reflex_settings_changed{false};

// Global DXGI Device Info Manager instance
std::unique_ptr<DXGIDeviceInfoManager> g_dxgiDeviceInfoManager;

// Direct atomic variables for latency tracking (UI access)
std::atomic<float> g_current_latency_ms{16.67f};
std::atomic<float> g_pcl_av_latency_ms{16.67f};
std::atomic<float> g_average_latency_ms{16.67f};
std::atomic<float> g_min_latency_ms{16.67f};
std::atomic<float> g_max_latency_ms{16.67f};
std::atomic<uint64_t> g_current_frame{0};
std::atomic<bool> g_reflex_active{false};

// Backbuffer dimensions
std::atomic<int> g_last_backbuffer_width{0};
std::atomic<int> g_last_backbuffer_height{0};

// Vector variables
std::vector<MonitorInfo> g_monitors;

// Colorspace variables
reshade::api::color_space g_current_colorspace = reshade::api::color_space::unknown;
std::string g_hdr10_override_status = "Not applied";
std::string g_hdr10_override_timestamp = "Never";

// Function implementations
const char* DxgiBypassModeToString(DxgiBypassMode mode) {
  switch (mode) {
    case DxgiBypassMode::kUnknown: return "Unknown";
    case DxgiBypassMode::kComposed: return "Composed";
    case DxgiBypassMode::kOverlay: return "Overlay";
    case DxgiBypassMode::kIndependentFlip: return "Independent Flip";
    default: return "Invalid";
  }
}

bool SetIndependentFlipState(reshade::api::swapchain* swapchain) {
  // This is a placeholder implementation
  // The actual implementation would be in a separate file
  return false;
}

void LogIndependentFlipConditions(reshade::api::swapchain* swapchain) {
  // This is a placeholder implementation
  // The actual implementation would be in a separate file
}

DxgiBypassMode GetIndependentFlipState(reshade::api::swapchain* swapchain) {
  // This is a placeholder implementation
  // The actual implementation would be in a separate file
  return DxgiBypassMode::kUnknown;
}