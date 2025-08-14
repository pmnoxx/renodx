#include "keyboard_hook.hpp"
#include "addon.hpp"
#include "utils.hpp"
#include <windowsx.h> // For GET_X_LPARAM, GET_Y_LPARAM macros

// Global variables for minimize prevention
static HHOOK g_alt_tab_hook = nullptr;
static WNDPROC g_original_wnd_proc = nullptr;
static HWND g_subclassed_hwnd = nullptr;
static WNDPROC g_original_logging_wnd_proc = nullptr;
static HWND g_logging_subclassed_hwnd = nullptr;

// Global variables for resize enforcement
static WNDPROC g_original_resize_wnd_proc = nullptr;
static HWND g_resize_enforcer_hwnd = nullptr;
static bool g_in_resize_enforce = false;

// Timer-based minimize prevention
static UINT_PTR g_minimize_prevention_timer = 0;
static const UINT MINIMIZE_PREVENTION_TIMER_ID = 1001;
static const UINT MINIMIZE_CHECK_INTERVAL_MS = 100; // Check every 100ms

// Hook procedure for Alt-Tab suppression
LRESULT CALLBACK AltTabHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT* kbStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            
            // Check if Alt+Tab is pressed (Alt = VK_MENU, Tab = VK_TAB)
            if (kbStruct->vkCode == VK_TAB && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
                // Suppress Alt+Tab by returning 1 (indicating we handled the message)
                LogDebug("Alt+Tab suppressed by hook");
                return 1;
            }
        }
    }
    
    // Call the next hook in the chain
    return CallNextHookEx(g_alt_tab_hook, nCode, wParam, lParam);
}

// Install the Alt-Tab suppression hook
bool InstallAltTabHook() {
    if (g_alt_tab_hook != nullptr) {
        LogWarn("Alt-Tab hook already installed");
        return true;
    }
    
    // Install a low-level keyboard hook
    g_alt_tab_hook = SetWindowsHookEx(WH_KEYBOARD_LL, AltTabHookProc, nullptr, 0);
    
    if (g_alt_tab_hook == nullptr) {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to install Alt-Tab hook. Error: " << error;
        LogWarn(oss.str().c_str());
        return false;
    }
    
    LogInfo("Alt-Tab suppression hook installed successfully");
    return true;
}

// Uninstall the Alt-Tab suppression hook
void UninstallAltTabHook() {
    if (g_alt_tab_hook == nullptr) {
        LogDebug("Alt-Tab hook not installed");
        return;
    }
    
    if (UnhookWindowsHookEx(g_alt_tab_hook)) {
        g_alt_tab_hook = nullptr;
        LogInfo("Alt-Tab suppression hook uninstalled successfully");
    } else {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to uninstall Alt-Tab hook. Error: " << error;
        LogWarn(oss.str().c_str());
    }
}

// Window procedure for intercepting minimize commands
LRESULT CALLBACK MinimizeWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Optional: enforce desired window size/position on resize/move
    extern float s_enforce_desired_window;
    if (s_enforce_desired_window >= 0.5f && !g_in_resize_enforce) {
        if (uMsg == WM_SIZING || uMsg == WM_SIZE || uMsg == WM_WINDOWPOSCHANGING) {
            RECT cr{}; RECT wr{};
            GetClientRect(hwnd, &cr);
            GetWindowRect(hwnd, &wr);
            int cur_w = cr.right - cr.left;
            int cur_h = cr.bottom - cr.top;

            int want_w = 0; int want_h = 0; ComputeDesiredSize(want_w, want_h);
            bool need_resize = (cur_w != want_w) || (cur_h != want_h) || (uMsg == WM_SIZING);

            // Always enforce position to desired coordinates
            int pos_x = static_cast<int>(s_windowed_pos_x);
            int pos_y = static_cast<int>(s_windowed_pos_y);

            if (need_resize) {
                g_in_resize_enforce = true;
                WindowStyleMode mode = (s_remove_top_bar >= 0.5f) ? WindowStyleMode::BORDERLESS : WindowStyleMode::OVERLAPPED_WINDOW;
                ApplyWindowChange(hwnd, /*do_resize=*/true, want_w, want_h, /*do_move=*/true, pos_x, pos_y, mode);
                g_in_resize_enforce = false;
                return 0;
            }
        }
    }
    // Check for minimize commands
    if (uMsg == WM_SYSCOMMAND) {
        if ((wParam & 0xFFF0) == SC_MINIMIZE) {
            // Suppress minimize by logging and returning 0 (handled)
            LogDebug("Window minimize suppressed by window subclassing (WM_SYSCOMMAND)");
            return 0;
        }
    }
    
    // Check for ShowWindow minimize calls
    if (uMsg == WM_SHOWWINDOW && wParam == FALSE) {
        // This might be a minimize - check if the window is being hidden
        if (IsIconic(hwnd)) {
            LogDebug("Window minimize suppressed by window subclassing (WM_SHOWWINDOW)");
            return 0;
        }
    }
    
    // Check for off-screen positioning that indicates minimize
    if (uMsg == WM_WINDOWPOSCHANGING) {
        WINDOWPOS* wp = (WINDOWPOS*)lParam;
        // Check if window is being moved off-screen (typical minimize behavior)
        if (wp->x < -10000 || wp->y < -10000 || wp->cx < 10 || wp->cy < 10) {
            // This looks like a minimize - prevent it
            LogDebug("Window minimize suppressed by window subclassing (off-screen positioning)");
            // Keep the window visible and on-screen
            wp->x = 0;
            wp->y = 0;
            wp->cx = max(wp->cx, 100);
            wp->cy = max(wp->cy, 100);
            return 0;
        }
    }
    
    // Check for size changes that indicate minimize
    if (uMsg == WM_SIZE) {
        if (wParam == SIZE_MINIMIZED) {
            // Suppress minimize by logging and returning 0 (handled)
            LogDebug("Window minimize suppressed by window subclassing (WM_SIZE)");
            return 0;
        }
    }
    
    // Check for window position changes that might indicate minimize
    if (uMsg == WM_WINDOWPOSCHANGED) {
        WINDOWPOS* wp = (WINDOWPOS*)lParam;
        // Check if window was moved off-screen (post-minimize)
        if (wp->x < -10000 || wp->y < -10000 || wp->cx < 10 || wp->cy < 10) {
            LogDebug("Window minimize detected after the fact - attempting to restore");
            // Try to restore the window immediately
            SetWindowPos(hwnd, nullptr, 0, 0, max(wp->cx, 100), max(wp->cy, 100), 
                        SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            return 0;
        }
    }
    
    // Check for activation changes that might indicate minimize
    if (uMsg == WM_ACTIVATE) {
        if (LOWORD(wParam) == WA_INACTIVE) {
            // Window is losing activation - check if it's minimized
            if (IsIconic(hwnd)) {
                LogDebug("Window minimize detected via activation change - attempting to restore");
                // Try to restore the window
                ShowWindow(hwnd, SW_RESTORE);
                return 0;
            }
        }
    }
    
    // Call the original window procedure for all other messages
    return CallWindowProc(g_original_wnd_proc, hwnd, uMsg, wParam, lParam);
}

// Window procedure enforcing desired size/position on resize/move
LRESULT CALLBACK ResizeEnforcerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Only act if feature enabled
    extern float s_enforce_desired_window;
    if (s_enforce_desired_window < 0.5f) {
        return CallWindowProc(g_original_resize_wnd_proc, hwnd, uMsg, wParam, lParam);
    }

    switch (uMsg) {
        case WM_SIZING:
        case WM_SIZE:
        case WM_WINDOWPOSCHANGING: {
            // Compute desired client size
            int want_w = 0; int want_h = 0; ComputeDesiredSize(want_w, want_h);

            // Compute desired position
            int pos_x = static_cast<int>(s_windowed_pos_x);
            int pos_y = static_cast<int>(s_windowed_pos_y);

            // Style preference
            WindowStyleMode mode = (s_remove_top_bar >= 0.5f) ? WindowStyleMode::BORDERLESS : WindowStyleMode::OVERLAPPED_WINDOW;

            // Apply enforce without recursion storms: only when size actually diverges or message indicates sizing
            RECT cr{}; RECT wr{};
            GetClientRect(hwnd, &cr);
            GetWindowRect(hwnd, &wr);
            int cur_w = cr.right - cr.left;
            int cur_h = cr.bottom - cr.top;
            bool need_resize = (cur_w != want_w) || (cur_h != want_h) || (uMsg == WM_SIZING);

            // For move
            bool need_move = true;
            // Map target pos to monitor origin handled in ApplyWindowChange

            if (need_resize || need_move) {
                ApplyWindowChange(hwnd, /*do_resize=*/true, want_w, want_h, /*do_move=*/true, pos_x, pos_y, mode);
                return 0; // handled
            }
            break;
        }
        default:
            break;
    }

    return CallWindowProc(g_original_resize_wnd_proc, hwnd, uMsg, wParam, lParam);
}

// Install the Windows minimize prevention using window subclassing
bool InstallMinimizeHook() {
    if (g_subclassed_hwnd != nullptr) {
        LogWarn("Minimize prevention already installed");
        return true;
    }
    
    // Get the current swapchain window handle
    extern std::atomic<HWND> g_last_swapchain_hwnd;
    HWND hwnd = g_last_swapchain_hwnd.load();
    
    if (hwnd == nullptr) {
        // Try to get the foreground window if no swapchain window
        hwnd = GetForegroundWindow();
    }
    
    if (hwnd == nullptr) {
        LogWarn("No window handle available for minimize prevention");
        return false;
    }
    
    // Store the original window procedure
    g_original_wnd_proc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
    if (g_original_wnd_proc == nullptr) {
        LogWarn("Failed to get original window procedure");
        return false;
    }
    
    // Subclass the window
    if (SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)MinimizeWndProc) == 0) {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to subclass window for minimize prevention. Error: " << error;
        LogWarn(oss.str().c_str());
        return false;
    }
    
    g_subclassed_hwnd = hwnd;
    LogInfo("Windows minimize prevention installed via window subclassing");
    
    // Start the timer-based minimize prevention as a backup
    g_minimize_prevention_timer = SetTimer(hwnd, MINIMIZE_PREVENTION_TIMER_ID, MINIMIZE_CHECK_INTERVAL_MS, MinimizePreventionTimerProc);
    if (g_minimize_prevention_timer == 0) {
        LogWarn("Failed to start minimize prevention timer");
    } else {
        LogDebug("Minimize prevention timer started successfully");
    }
    
    return true;
}

// Install the resize enforcer using window subclassing
bool InstallResizeEnforcerHook() {
    if (g_resize_enforcer_hwnd != nullptr) {
        LogWarn("Resize enforcer already installed");
        return true;
    }

    // If minimize hook already subclassed the window, piggyback on it (logic in MinimizeWndProc)
    if (g_subclassed_hwnd != nullptr) {
        LogInfo("Resize enforcer piggybacking on existing minimize subclass");
        return true;
    }

    // Get the current swapchain window handle or foreground
    extern std::atomic<HWND> g_last_swapchain_hwnd;
    HWND hwnd = g_last_swapchain_hwnd.load();
    if (hwnd == nullptr) hwnd = GetForegroundWindow();
    if (hwnd == nullptr) {
        LogWarn("No window handle available for resize enforcer");
        return false;
    }

    g_original_resize_wnd_proc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
    if (g_original_resize_wnd_proc == nullptr) {
        LogWarn("Failed to get original window procedure for resize enforcer");
        return false;
    }

    if (SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)ResizeEnforcerWndProc) == 0) {
        DWORD error = GetLastError();
        std::ostringstream oss; oss << "Failed to subclass window for resize enforcer. Error: " << error;
        LogWarn(oss.str().c_str());
        return false;
    }

    g_resize_enforcer_hwnd = hwnd;
    LogInfo("Resize enforcer installed via window subclassing");
    return true;
}

// Uninstall the resize enforcer
void UninstallResizeEnforcerHook() {
    if (g_resize_enforcer_hwnd == nullptr || g_original_resize_wnd_proc == nullptr) {
        LogDebug("Resize enforcer not installed");
        return;
    }

    if (SetWindowLongPtr(g_resize_enforcer_hwnd, GWLP_WNDPROC, (LONG_PTR)g_original_resize_wnd_proc) != 0) {
        g_resize_enforcer_hwnd = nullptr;
        g_original_resize_wnd_proc = nullptr;
        LogInfo("Resize enforcer uninstalled successfully");
    } else {
        DWORD error = GetLastError();
        std::ostringstream oss; oss << "Failed to uninstall resize enforcer. Error: " << error;
        LogWarn(oss.str().c_str());
    }
}

// Uninstall the Windows minimize prevention
void UninstallMinimizeHook() {
    if (g_subclassed_hwnd == nullptr || g_original_wnd_proc == nullptr) {
        LogWarn("Minimize prevention not installed");
        return;
    }
    
    // Stop the minimize prevention timer
    if (g_minimize_prevention_timer != 0) {
        if (KillTimer(g_subclassed_hwnd, MINIMIZE_PREVENTION_TIMER_ID)) {
            LogDebug("Minimize prevention timer stopped successfully");
        } else {
            LogWarn("Failed to stop minimize prevention timer");
        }
        g_minimize_prevention_timer = 0;
    }
    
    // Restore the original window procedure
    if (SetWindowLongPtr(g_subclassed_hwnd, GWLP_WNDPROC, (LONG_PTR)g_original_wnd_proc) != 0) {
        g_subclassed_hwnd = nullptr;
        g_original_wnd_proc = nullptr;
        LogInfo("Windows minimize prevention uninstalled successfully");
    } else {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to uninstall minimize prevention. Error: " << error;
        LogWarn(oss.str().c_str());
    }
}

// Timer callback for minimize prevention
VOID CALLBACK MinimizePreventionTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    if (idEvent == MINIMIZE_PREVENTION_TIMER_ID && g_subclassed_hwnd != nullptr) {
        // Check if the window is minimized
        if (IsIconic(g_subclassed_hwnd)) {
            LogDebug("Timer detected minimized window - restoring");
            ShowWindow(g_subclassed_hwnd, SW_RESTORE);
        }
        
        // Also check for off-screen positioning
        RECT rect;
        if (GetWindowRect(g_subclassed_hwnd, &rect)) {
            if (rect.left < -10000 || rect.top < -10000 || 
                (rect.right - rect.left) < 10 || (rect.bottom - rect.top) < 10) {
                LogDebug("Timer detected off-screen window - repositioning");
                SetWindowPos(g_subclassed_hwnd, nullptr, 0, 0, 
                           max(rect.right - rect.left, 100), max(rect.bottom - rect.top, 100),
                           SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            }
        }
    }
}

// Helper functions for window state logging
void LogWindowMoveMessage(WPARAM wParam, LPARAM lParam) {
    std::ostringstream oss;
    oss << "Window moved to (" << GET_X_LPARAM(lParam) << ", " << GET_Y_LPARAM(lParam) << ")";
    LogInfo(oss.str().c_str());
}

void LogWindowSizeMessage(WPARAM wParam, LPARAM lParam) {
    std::ostringstream oss;
    oss << "Window resized to " << LOWORD(lParam) << "x" << HIWORD(lParam);
    LogInfo(oss.str().c_str());
}

void LogWindowSystemCommand(WPARAM wParam, LPARAM lParam) {
    switch (wParam & 0xFFF0) {
        case SC_MINIMIZE:
            LogInfo("Window minimize requested");
            break;
        case SC_MAXIMIZE:
            LogInfo("Window maximize requested");
            break;
        case SC_RESTORE:
            LogInfo("Window restore requested");
            break;
        case SC_CLOSE:
            LogInfo("Window close requested");
            break;
    }
}

void LogWindowPositionChanged(WPARAM wParam, LPARAM lParam) {
    WINDOWPOS* wp = (WINDOWPOS*)lParam;
    if (!(wp->flags & SWP_NOMOVE) || !(wp->flags & SWP_NOSIZE)) {
        std::ostringstream oss;
        oss << "Window position/size changed: flags=0x" << std::hex << wp->flags 
            << ", pos=(" << wp->x << "," << wp->y << "), size=" << wp->cx << "x" << wp->cy;
        LogInfo(oss.str().c_str());
    }
}

void LogWindowStyleChanged(WPARAM wParam, LPARAM lParam) {
    std::ostringstream oss;
    oss << "Window style changed: type=" << ((wParam == GWL_STYLE) ? "GWL_STYLE" : "GWL_EXSTYLE")
        << ", style=0x" << std::hex << ((wParam == GWL_STYLE) ? lParam : 0)
        << ", exstyle=0x" << std::hex << ((wParam == GWL_EXSTYLE) ? lParam : 0);
    LogInfo(oss.str().c_str());
}

void LogWindowActivationChanged(WPARAM wParam, LPARAM lParam) {
    std::ostringstream oss;
    oss << "Window activation changed: state=";
    if (LOWORD(wParam) == WA_INACTIVE) {
        oss << "Inactive";
    } else if (LOWORD(wParam) == WA_ACTIVE) {
        oss << "Active";
    } else {
        oss << "Click Active";
    }
    LogInfo(oss.str().c_str());
}

void LogWindowFocusChanged(UINT uMsg) {
    if (uMsg == WM_SETFOCUS) {
        LogInfo("Window gained focus");
    } else if (uMsg == WM_KILLFOCUS) {
        LogInfo("Window lost focus");
    }
}

void LogWindowShowStateChanged(WPARAM wParam, LPARAM lParam) {
    std::ostringstream oss;
    oss << "Window show state changed: show=" << (wParam ? "true" : "false") << ", reason=" << lParam;
    LogInfo(oss.str().c_str());
}

void LogWindowPositionChanging(WPARAM wParam, LPARAM lParam) {
    WINDOWPOS* wp = (WINDOWPOS*)lParam;
    if (!(wp->flags & SWP_NOMOVE) || !(wp->flags & SWP_NOSIZE)) {
        std::ostringstream oss;
        oss << "Window position/size changing: flags=0x" << std::hex << wp->flags 
            << ", pos=(" << wp->x << "," << wp->y << "), size=" << wp->cx << "x" << wp->cy;
        LogInfo(oss.str().c_str());
    }
}

// Window procedure for logging window state changes
LRESULT CALLBACK WindowStateLoggingWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Log relevant window state changes
    switch (uMsg) {
        case WM_MOVE:
            LogWindowMoveMessage(wParam, lParam);
            break;
            
        case WM_SIZE:
            LogWindowSizeMessage(wParam, lParam);
            break;
            
        case WM_SYSCOMMAND:
            LogWindowSystemCommand(wParam, lParam);
            break;
            
        case WM_WINDOWPOSCHANGED:
            LogWindowPositionChanged(wParam, lParam);
            break;
            
        case WM_STYLECHANGED:
            LogWindowStyleChanged(wParam, lParam);
            break;
            
        case WM_ACTIVATE:
            LogWindowActivationChanged(wParam, lParam);
            break;
            
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            LogWindowFocusChanged(uMsg);
            break;
            
        case WM_SHOWWINDOW:
            LogWindowShowStateChanged(wParam, lParam);
            break;
            
        case WM_WINDOWPOSCHANGING:
            LogWindowPositionChanging(wParam, lParam);
            break;
    }
    
    // Call the original window procedure for all messages
    return CallWindowProc(g_original_logging_wnd_proc, hwnd, uMsg, wParam, lParam);
}

// Install the window state change logging hook
bool InstallWindowStateLoggingHook() {
    if (g_logging_subclassed_hwnd != nullptr) {
        LogWarn("Window state logging already installed");
        return true;
    }
    
    // Get the current swapchain window handle
    extern std::atomic<HWND> g_last_swapchain_hwnd;
    HWND hwnd = g_last_swapchain_hwnd.load();
    
    if (hwnd == nullptr) {
        // Try to get the foreground window if no swapchain window
        hwnd = GetForegroundWindow();
    }
    
    if (hwnd == nullptr) {
        LogWarn("No window handle available for window state logging");
        return false;
    }
    
    // Store the original window procedure
    g_original_logging_wnd_proc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
    if (g_original_logging_wnd_proc == nullptr) {
        LogWarn("Failed to get original window procedure for logging");
        return false;
    }
    
    // Subclass the window
    if (SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WindowStateLoggingWndProc) == 0) {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to subclass window for state logging. Error: " << error;
        LogWarn(oss.str().c_str());
        return false;
    }
    
    g_logging_subclassed_hwnd = hwnd;
    LogInfo("Window state change logging installed via window subclassing");
    return true;
}

// Uninstall the window state change logging hook
void UninstallWindowStateLoggingHook() {
    if (g_logging_subclassed_hwnd == nullptr || g_original_logging_wnd_proc == nullptr) {
        LogDebug("Window state logging not installed");
        return;
    }
    
    // Restore the original window procedure
    if (SetWindowLongPtr(g_logging_subclassed_hwnd, GWLP_WNDPROC, (LONG_PTR)g_original_logging_wnd_proc) != 0) {
        g_logging_subclassed_hwnd = nullptr;
        g_original_logging_wnd_proc = nullptr;
        LogInfo("Window state change logging uninstalled successfully");
    } else {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to uninstall window state logging. Error: " << error;
        LogWarn(oss.str().c_str());
    }
}
