#include "window_position_task.hpp"
#include "../addon.hpp"

// External declarations from globals.cpp
extern std::atomic<HWND> g_last_swapchain_hwnd;
extern float s_move_to_zero_if_out;

// Window position background task
void RunWindowPositionTask() {
    // Check if window position adjustment is needed
    if (s_move_to_zero_if_out >= 0.5f) {
        HWND hwnd = g_last_swapchain_hwnd.load();
        if (hwnd != nullptr && IsWindow(hwnd)) {
            // Check if window is out of bounds and needs repositioning
            RECT window_rect;
            if (GetWindowRect(hwnd, &window_rect)) {
                // If window is off-screen, move it to a safe position
                if (window_rect.left < 0 || window_rect.top < 0 || 
                    window_rect.right < 0 || window_rect.bottom < 0) {
                    // Move window to origin (0,0) on its current monitor
                    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, 
                                SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
                    LogDebug("Window position task: Moved off-screen window to origin");
                }
            }
        }
    }
}
