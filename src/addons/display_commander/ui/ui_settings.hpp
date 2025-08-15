#pragma once

#include <windows.h>
#include <vector>
#include <string>

// Forward declarations
namespace renodx::utils::settings {
    struct Setting;
}

// External declarations for settings


extern float s_reflex_debug_output;
extern float s_remove_top_bar;
extern float s_ui_mode;
extern float s_auto_apply_enabled;
extern float s_auto_apply_delay_sec;
extern float s_auto_apply_init_delay_sec;
extern float s_windowed_width;
extern float s_windowed_height;
extern float s_resize_mode;
extern float s_windowed_pos_x;
extern float s_windowed_pos_y;
extern float s_move_to_zero_if_out;
extern float s_force_borderless;
extern float s_nvapi_fullscreen_prevention;
extern float s_audio_volume_percent;
extern float s_audio_mute;
extern float s_mute_in_background;
extern float s_fps_limit_background;
extern float s_window_info_display;

// Helper functions for tab visibility
inline bool is_basic_tab(float ui_mode);
inline bool is_developer_tab(float ui_mode);
inline bool is_device_tab(float ui_mode);
inline bool is_window_info_tab(float ui_mode);

// Settings vector declaration
extern std::vector<renodx::utils::settings::Setting*> settings;

// Helper functions for labels
std::vector<std::string> MakeLabels(const std::vector<int>& options, int default_index);
std::vector<std::string> MakeMonitorLabels();
void ComputeDesiredSize(int& width, int& height);

// Window management functions
enum class WindowStyleMode;
void ApplyWindowChange(HWND hwnd, bool do_resize, int client_width, int client_height, bool do_move, int pos_x, int pos_y, WindowStyleMode style_mode, const char* reason = "unknown");

// Audio debugging functions
void DebugAudioSessions();
void DebugAudioSessionForProcess(DWORD process_id);
