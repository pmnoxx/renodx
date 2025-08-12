#pragma once

#define ImTextureID ImU64

#define DEBUG_LEVEL_0

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windef.h>
#include <dxgi1_3.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <deps/imgui/imgui.h>
#include <include/reshade.hpp>

#include <sstream>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cmath>
#include <vector>

#include "../../utils/settings.hpp"
#include "../../utils/swapchain.hpp"
#include "../../mods/swapchain.hpp"
#include "utils.hpp"

// WASAPI per-app volume control
#include <mmdeviceapi.h>
#include <audiopolicy.h>

// Audio management functions
bool SetMuteForCurrentProcess(bool mute);
bool SetVolumeForCurrentProcess(float volume_0_100);
void RunBackgroundAudioMonitor();

// Forward declarations
void ComputeDesiredSize(int& out_w, int& out_h);
int ComputeHeightFromWidth(int width);
int ComputeHeightFromWidthAndAspect(int width, const struct AspectRatio& ar);
std::vector<std::string> MakeMonitorLabels();

// External declarations
extern std::atomic<int> g_last_backbuffer_width;
extern std::atomic<int> g_last_backbuffer_height;

// Enums
enum class DxgiBypassMode : std::uint8_t { kUnknown, kComposed, kOverlay, kIndependentFlip };
enum class WindowStyleMode : std::uint8_t { KEEP, BORDERLESS, OVERLAPPED_WINDOW };

// Forward declarations that depend on enums
DxgiBypassMode GetIndependentFlipState(reshade::api::swapchain* swapchain);

// Alt-Tab suppression functions
bool InstallAltTabHook();
void UninstallAltTabHook();

// Windows minimize prevention functions (using window subclassing)
bool InstallMinimizeHook();
void UninstallMinimizeHook();

// Window state change logging functions
bool InstallWindowStateLoggingHook();
void UninstallWindowStateLoggingHook();

// Structs
struct IndependentFlipFailures {
  std::atomic<bool> swapchain_null{false};
  std::atomic<bool> device_null{false};
  std::atomic<bool> non_dxgi_api{false};
  std::atomic<bool> swapchain_media_failed{false};
  std::atomic<bool> frame_stats_failed{false};
  std::atomic<bool> not_flip_model{false};
  std::atomic<bool> backbuffer_size_mismatch{false};
  std::atomic<bool> window_size_mismatch{false};
  std::atomic<bool> window_not_at_origin{false};
  std::atomic<bool> window_layered{false};
  std::atomic<bool> window_topmost{false};
  std::atomic<bool> window_maximized{false};
  std::atomic<bool> window_minimized{false};
  std::atomic<bool> hwnd_null{false};
  
  void reset();
};

// Note: AspectRatio and MonitorInfo structs are defined in utils.hpp

// Global variables (extern declarations)
extern float s_auto_apply_enabled;
extern float s_auto_apply_delay_sec;
extern float s_auto_apply_init_delay_sec;
extern float s_windowed_width;
extern float s_windowed_height;
extern float s_windowed_pos_x;
extern float s_windowed_pos_y;
extern float s_remove_top_bar;
extern float s_resize_mode;
extern float s_aspect_index;
extern float s_move_to_zero_if_out;
extern float s_force_borderless;
extern float s_prevent_fullscreen;
extern float s_spoof_fullscreen_state;
extern float s_mute_in_background;
extern float s_audio_volume_percent;
extern float s_audio_mute;
extern float s_fps_limit_background;
extern float s_target_monitor_index;
extern float s_dxgi_composition_state;
extern float s_try_independent_flip;
extern float s_suppress_alt_tab;
extern float s_prevent_windows_minimize;
extern float s_spoof_window_focus;
extern float s_log_window_state_changes;

extern std::atomic<int> g_comp_query_counter;
extern std::atomic<int> g_comp_last_logged;
// Last known swapchain pointer (for composition state queries)
extern std::atomic<reshade::api::swapchain*> g_last_swapchain_ptr;
extern std::atomic<IndependentFlipFailures*> g_if_failures;
extern std::atomic<uint64_t> g_init_apply_generation;
extern std::chrono::steady_clock::time_point g_attach_time;
extern std::atomic<HWND> g_last_swapchain_hwnd;
extern std::atomic<bool> g_shutdown;
extern std::atomic<bool> g_muted_applied;
extern std::atomic<float> g_default_fps_limit;

extern std::vector<MonitorInfo> g_monitors;



// Function declarations
const char* DxgiBypassModeToString(DxgiBypassMode mode);
bool SetIndependentFlipState(reshade::api::swapchain* swapchain);
void ApplyWindowChange(HWND hwnd, bool do_resize, int client_width, int client_height, bool do_move, int pos_x, int pos_y, WindowStyleMode style_mode);
bool ShouldApplyWindowedForBackbuffer(int desired_w, int desired_h);
void ScheduleAutoApplyOnInit(HWND hwnd);
void LogIndependentFlipConditions(reshade::api::swapchain* swapchain);

// Swapchain event handlers
void OnInitSwapchain(reshade::api::swapchain* swapchain, bool resize);
void OnPresentUpdate(reshade::api::command_queue* queue, reshade::api::swapchain* swapchain, const reshade::api::rect* source_rect, const reshade::api::rect* dest_rect, uint32_t dirty_rect_count, const reshade::api::rect* dirty_rects);

// Note: GetIndependentFlipState is implemented in the .cpp file as it's complex
