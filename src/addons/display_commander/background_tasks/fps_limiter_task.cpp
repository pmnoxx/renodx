#include "fps_limiter_task.hpp"
#include "../addon.hpp"
#include <sstream>

// External declarations from globals.cpp
extern std::atomic<HWND> g_last_swapchain_hwnd;
extern float s_fps_limit;
extern float s_fps_limit_background;
extern std::atomic<float> g_default_fps_limit;

// FPS limiting background task
void RunFpsLimiterTask() {
    HWND hwnd = g_last_swapchain_hwnd.load();
    if (hwnd == nullptr) hwnd = GetForegroundWindow();
    
    // Use actual focus state for more reliable FPS limiting
    const bool is_background = (hwnd != nullptr && GetForegroundWindow() != hwnd);
    
    // Log current state for debugging
    static int last_log_counter = 0;
    if (++last_log_counter % 100 == 0) { // Log every 30 seconds (100 * 300ms)
        std::ostringstream oss;
        oss << "FPS limiter task: hwnd=" << hwnd << ", is_background=" << (is_background ? "true" : "false");
        oss << ", current_fps_limit=" << renodx::utils::swapchain::fps_limit;
        oss << ", desired_background=" << s_fps_limit_background;
        oss << ", desired_foreground=" << g_default_fps_limit.load();
        LogDebug(oss.str());
    }
    
    if (is_background) {
        // Background: apply background FPS limit if enabled
        if (s_fps_limit_background >= 0.f) {
            const float desired_limit = s_fps_limit_background;
            if (renodx::utils::swapchain::fps_limit != desired_limit) {
                renodx::utils::swapchain::fps_limit = desired_limit;
                std::ostringstream oss;
                oss << "FPS limiter: Applied background limit " << desired_limit << " FPS";
                LogInfo(oss.str().c_str());
            }
        }
    } else {
        // Foreground: restore default FPS limit
        if (g_default_fps_limit.load() == 0.0f) {
            if (renodx::utils::swapchain::fps_limit != 0.0f) {
                renodx::utils::swapchain::fps_limit = 0.0f;
                LogInfo("FPS limiter: Removed FPS limit (foreground)");
            }
        } else {
            if (renodx::utils::swapchain::fps_limit != g_default_fps_limit.load()) {
                renodx::utils::swapchain::fps_limit = g_default_fps_limit.load();
                std::ostringstream oss;
                oss << "FPS limiter: Restored default FPS limit " << g_default_fps_limit.load() << " FPS";
                LogInfo(oss.str().c_str());
            }
        }
    }
}
