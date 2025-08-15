#include "fps_limiter.hpp"
#include "../addon.hpp"
#include <chrono>
#include <thread>

// External declarations from globals.cpp
extern std::atomic<HWND> g_last_swapchain_hwnd;
extern float s_fps_limit;
extern float s_fps_limit_background;
extern std::atomic<float> g_default_fps_limit;

// FPS limiting functions
void ApplyFpsLimit() {
    HWND hwnd = g_last_swapchain_hwnd.load();
    if (hwnd == nullptr) hwnd = GetForegroundWindow();
    
    // Use spoofed focus state instead of actual focus state
    const bool is_background = (hwnd != nullptr && !GetSpoofedWindowFocus(hwnd));
    
    if (is_background) {
        // Background: apply background FPS limit if enabled
        if (s_fps_limit_background >= 0.f) {
            const float desired_limit = s_fps_limit_background;
            if (renodx::utils::swapchain::fps_limit != desired_limit) {
                renodx::utils::swapchain::fps_limit = desired_limit;
                LogDebug("FPS limiter: Applied background limit");
            }
        }
    } else {
        // Foreground: restore default FPS limit
        if (g_default_fps_limit.load() == 0.0f) {
            if (renodx::utils::swapchain::fps_limit != 0.0f) {
                renodx::utils::swapchain::fps_limit = 0.0f;
                LogDebug("FPS limiter: Removed FPS limit (foreground)");
            }
        } else {
            if (renodx::utils::swapchain::fps_limit != g_default_fps_limit.load()) {
                renodx::utils::swapchain::fps_limit = g_default_fps_limit.load();
                LogDebug("FPS limiter: Restored default FPS limit");
            }
        }
    }
}

void RunFpsLimiterMonitor() {
    while (!g_shutdown.load()) {
        ApplyFpsLimit();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}
