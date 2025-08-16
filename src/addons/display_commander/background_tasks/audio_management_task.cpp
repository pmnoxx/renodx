#include "audio_management_task.hpp"
#include "../addon.hpp"

// External declarations from globals.cpp
extern std::atomic<HWND> g_last_swapchain_hwnd;
extern float s_audio_mute;
extern float s_mute_in_background;
extern float s_audio_volume_percent;
extern std::atomic<bool> g_muted_applied;
extern std::atomic<bool> g_volume_applied;

// Audio management background task
void RunAudioManagementTask() {
    bool want_mute = false;
    
    // Check if manual mute is enabled - if so, always mute regardless of background state
    if (s_audio_mute >= 0.5f) {
        want_mute = true;
    }
    // Only apply background mute logic if manual mute is OFF
    else if (s_mute_in_background >= 0.5f) {
        HWND hwnd = g_last_swapchain_hwnd.load();
        if (hwnd == nullptr) hwnd = GetForegroundWindow();
        // Use actual focus state instead of spoofed focus state
        want_mute = (hwnd != nullptr && GetForegroundWindow() != hwnd);
    }

    const bool applied = g_muted_applied.load();
    if (want_mute != applied) {
        if (SetMuteForCurrentProcess(want_mute)) {
            g_muted_applied.store(want_mute);
        }
    }
    
    // Handle volume changes (only when not muted)
    if (!want_mute) {
        static float last_volume = -1.0f;
        if (s_audio_volume_percent != last_volume) {
            if (SetVolumeForCurrentProcess(s_audio_volume_percent)) {
                last_volume = s_audio_volume_percent;
                LogDebug("Background audio task: Applied volume setting");
            }
        }
    }
}
