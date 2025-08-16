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
    static bool first_run = true;
    if (first_run) {
        LogInfo("FPS limiter task started - monitoring FPS limits");
        first_run = false;
    }
    
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
        LogInfo(oss.str().c_str());
    }
    
    if (is_background) {
        // Background: apply background FPS limit if enabled
        if (s_fps_limit_background >= 0.f) {
            const float desired_limit = s_fps_limit_background;
            if (renodx::utils::swapchain::fps_limit != desired_limit) {
                const float old_limit = renodx::utils::swapchain::fps_limit;
                renodx::utils::swapchain::fps_limit = desired_limit;
                std::ostringstream oss;
                oss << "FPS limiter: Applied background limit " << desired_limit << " FPS (was " << old_limit << ")";
                LogInfo(oss.str().c_str());
            }
        } else {
            // Background limit disabled - apply foreground limit instead
            const float desired_limit = s_fps_limit;
            if (renodx::utils::swapchain::fps_limit != desired_limit) {
                const float old_limit = renodx::utils::swapchain::fps_limit;
                renodx::utils::swapchain::fps_limit = desired_limit;
                std::ostringstream oss;
                oss << "FPS limiter: Background limit disabled, applied foreground limit " << desired_limit << " FPS (was " << old_limit << ")";
                LogInfo(oss.str().c_str());
            }
        }
    } else {
        // Foreground: apply foreground FPS limit
        const float desired_limit = s_fps_limit;
        if (renodx::utils::swapchain::fps_limit != desired_limit) {
            const float old_limit = renodx::utils::swapchain::fps_limit;
            renodx::utils::swapchain::fps_limit = desired_limit;
            std::ostringstream oss;
            if (desired_limit > 0.0f) {
                oss << "FPS limiter: Applied foreground limit " << desired_limit << " FPS (was " << old_limit << ")";
            } else {
                oss << "FPS limiter: Removed foreground FPS limit (was " << old_limit << ")";
            }
            LogInfo(oss.str().c_str());
        }
    }
}
