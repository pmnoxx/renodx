#include "borderless_style_enforcement.hpp"
#include "../addon.hpp"
#include "../utils.hpp"
#include <sstream>
#include <thread>
#include <atomic>

// Timer-based borderless style enforcement
UINT_PTR g_borderless_timer_id = 0;
bool g_borderless_timer_active = false;

// Thread-safe flag for pending style changes
std::atomic<bool> g_pending_style_change{false};

// Delayed style application function - runs in separate thread
void ApplyBorderlessStylesDelayed() {
    extern float s_remove_top_bar;
    if (s_remove_top_bar < 0.5f) {
        return; // Borderless mode not enabled
    }
    
    // Get the current swapchain window handle
    extern std::atomic<HWND> g_last_swapchain_hwnd;
    HWND hwnd = g_last_swapchain_hwnd.load();
    if (hwnd == nullptr) {
        return;
    }
    
    // Check if window is already borderless
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    LONG_PTR ex_style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    
    // Check if title bar and borders are already removed
    bool has_title_bar = (style & WS_CAPTION) != 0;
    bool has_borders = (style & (WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU)) != 0;
    bool has_ex_borders = (ex_style & (WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)) != 0;
    
    if (has_title_bar || has_borders || has_ex_borders) {
        // Remove title bar and borders
        LONG_PTR new_style = style & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        LONG_PTR new_ex_style = ex_style & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
        
        // Apply the new styles
        SetWindowLongPtr(hwnd, GWL_STYLE, new_style);
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, new_ex_style);
        
        // Force the window to update with new styles
   //     SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
        
        // Verify the styles were applied
        LONG_PTR verify_style = GetWindowLongPtr(hwnd, GWL_STYLE);
        LONG_PTR verify_ex_style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        
        if (verify_style == new_style && verify_ex_style == new_ex_style) {
            LogDebug("Reflex: Successfully applied borderless styles to existing window");
        } else {
            LogWarn("Reflex: Failed to apply borderless styles - styles not properly set");
        }
    }
    
    // Reset the pending flag
    g_pending_style_change.store(false);
}

// Timer callback for checking window state and scheduling delayed changes
VOID CALLBACK BorderlessStyleTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    // Only check if no pending change is in progress
    if (!g_pending_style_change.load()) {
        extern float s_remove_top_bar;
        if (s_remove_top_bar >= 0.5f) {
            // Check if window needs style changes
            extern std::atomic<HWND> g_last_swapchain_hwnd;
            HWND current_hwnd = g_last_swapchain_hwnd.load();
            if (current_hwnd != nullptr) {
                LONG_PTR style = GetWindowLongPtr(current_hwnd, GWL_STYLE);
                LONG_PTR ex_style = GetWindowLongPtr(current_hwnd, GWL_EXSTYLE);
                
                // Check if title bar and borders are present
                bool has_title_bar = (style & WS_CAPTION) != 0;
                bool has_borders = (style & (WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU)) != 0;
                bool has_ex_borders = (ex_style & (WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)) != 0;
                
                if (has_title_bar || has_borders || has_ex_borders) {
                    // Schedule delayed style change
                    g_pending_style_change.store(true);
                    
                    // Start delayed application in separate thread
                    std::thread([]() {
                        // Small delay to prevent immediate execution
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                        ApplyBorderlessStylesDelayed();
                    }).detach();
                }
            }
        }
    }
    
    // Slow down the timer after the initial aggressive phase
    static int timer_phase_counter = 0;
    if (++timer_phase_counter == 200) { // After 10 seconds (200 * 50ms)
        // Kill the current timer and restart with a slower interval
        KillTimer(nullptr, g_borderless_timer_id);
        g_borderless_timer_id = SetTimer(nullptr, 0, 200, BorderlessStyleTimerProc); // 200ms interval
        if (g_borderless_timer_id != 0) {
            LogDebug("Reflex: Slowed down borderless style enforcement timer to 200ms interval");
        }
    }
}

// Aggressively apply borderless styles to existing windows (now uses delayed approach)
void ApplyBorderlessStylesAggressively() {
    // This function now just schedules a delayed check instead of immediate changes
    if (!g_pending_style_change.load()) {
        g_pending_style_change.store(true);
        
        // Start delayed application in separate thread
        std::thread([]() {
            // Small delay to prevent immediate execution
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ApplyBorderlessStylesDelayed();
        }).detach();
    }
}

// Check all top-level windows and apply borderless styles if needed (now uses delayed approach)
void ApplyBorderlessStylesToAllWindows() {
    extern float s_remove_top_bar;
    if (s_remove_top_bar < 0.5f) {
        return; // Borderless mode not enabled
    }
    
    // Only check if no pending change is in progress
    if (!g_pending_style_change.load()) {
        // EnumWindows callback to check each top-level window
        auto enumWindowsProc = [](HWND hwnd, LPARAM lParam) -> BOOL {
            // Skip invisible windows
            if (!IsWindowVisible(hwnd)) {
                return TRUE;
            }
            
            // Skip windows that are not top-level
            if (GetParent(hwnd) != nullptr) {
                return TRUE;
            }
            
            // Check if this window has a title bar or borders
            LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
            LONG_PTR ex_style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
            
            bool has_title_bar = (style & WS_CAPTION) != 0;
            bool has_borders = (style & (WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU)) != 0;
            bool has_ex_borders = (ex_style & (WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)) != 0;
            
            if (has_title_bar || has_borders || has_ex_borders) {
                // Schedule delayed style change for this window
                std::thread([hwnd, style, ex_style]() {
                    // Small delay to prevent immediate execution
                    std::this_thread::sleep_for(std::chrono::milliseconds(150));
                    
                    // Remove title bar and borders
                    LONG_PTR new_style = style & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
                    LONG_PTR new_ex_style = ex_style & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
                    
                    // Apply the new styles
                    SetWindowLongPtr(hwnd, GWL_STYLE, new_style);
                    SetWindowLongPtr(hwnd, GWL_EXSTYLE, new_ex_style);
                    
                    // Force the window to update with new styles
                  //  SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
                    
                    LogDebug("Reflex: Applied borderless styles to top-level window (delayed)");
                }).detach();
            }
            
            return TRUE;
        };
        
        // Enumerate all top-level windows
        EnumWindows(enumWindowsProc, 0);
    }
}

// Start the borderless style enforcement timer
void StartBorderlessStyleTimer() {
    if (g_borderless_timer_active) {
        return; // Timer already active
    }
    
    extern float s_remove_top_bar;
    if (s_remove_top_bar < 0.5f) {
        return; // Borderless mode not enabled
    }
    
    // Set a timer to check window state every 50ms initially, then slow down
    g_borderless_timer_id = SetTimer(nullptr, 0, 50, BorderlessStyleTimerProc);
    if (g_borderless_timer_id != 0) {
        g_borderless_timer_active = true;
        LogDebug("Reflex: Started borderless style enforcement timer (50ms initial interval, delayed execution)");
    } else {
        LogWarn("Reflex: Failed to start borderless style enforcement timer");
    }
}

// Stop the borderless style enforcement timer
void StopBorderlessStyleTimer() {
    if (g_borderless_timer_active && g_borderless_timer_id != 0) {
        KillTimer(nullptr, g_borderless_timer_id);
        g_borderless_timer_id = 0;
        g_borderless_timer_active = false;
        LogDebug("Reflex: Stopped borderless style enforcement timer");
    }
}
