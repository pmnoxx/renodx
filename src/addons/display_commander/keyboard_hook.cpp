#include "keyboard_hook.hpp"
#include "addon.hpp"
#include "utils.hpp"
#include <windowsx.h> // For GET_X_LPARAM, GET_Y_LPARAM macros

// Global variables for minimize prevention
static WNDPROC g_original_wnd_proc = nullptr;
static HWND g_subclassed_hwnd = nullptr;
static WNDPROC g_original_logging_wnd_proc = nullptr;
static HWND g_logging_subclassed_hwnd = nullptr;

// Global variables for resize enforcement
static WNDPROC g_original_resize_wnd_proc = nullptr;
static HWND g_resize_enforcer_hwnd = nullptr;
static bool g_in_resize_enforce = false;

// Global variables for window message hook
static WNDPROC g_original_window_message_wnd_proc = nullptr;
static HWND g_window_message_subclassed_hwnd = nullptr;

// Global variables for window creation hooks
static WNDPROC g_original_create_window_ex_wnd_proc = nullptr;
static HWND g_create_window_hook_hwnd = nullptr;
static bool g_window_creation_hooks_installed = false;

// Direct hook for CreateWindowExW to modify window styles during creation
static HWND (WINAPI* g_original_CreateWindowExW)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) = nullptr;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    // Check if we want borderless mode
    extern float s_remove_top_bar;
    if (s_remove_top_bar >= 0.5f) {
        // Remove title bar and borders from the style
        dwStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        dwExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
        
        LogDebug("Reflex: Modified CreateWindowExW call to use borderless styles");
    }
    
    // Call the original function with modified styles
    return g_original_CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

// Install CreateWindowEx hooks
bool InstallCreateWindowExHooks() {
    if (g_original_CreateWindowExW != nullptr) {
        LogDebug("CreateWindowEx hooks already installed");
        return true;
    }
    
    // Get the address of CreateWindowExW
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        LogWarn("Failed to get user32.dll handle");
        return false;
    }
    
    g_original_CreateWindowExW = (HWND (WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID))GetProcAddress(user32, "CreateWindowExW");
    if (!g_original_CreateWindowExW) {
        LogWarn("Failed to get CreateWindowExW address");
        return false;
    }
    
    // Install the hook using Detours or similar hooking library
    // For now, we'll use a simpler approach by modifying the window after creation
    LogInfo("CreateWindowEx hooks prepared (requires hooking library for full implementation)");
    
    return true;
}

// Uninstall CreateWindowEx hooks
void UninstallCreateWindowExHooks() {
    if (g_original_CreateWindowExW != nullptr) {
        g_original_CreateWindowExW = nullptr;
        LogInfo("CreateWindowEx hooks uninstalled");
    }
}

// Timer-based minimize prevention removed







// Hook for CreateWindowExW to modify window styles during creation
LRESULT CALLBACK CreateWindowHookProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Call the original window procedure first
    LRESULT result = CallWindowProc(g_original_create_window_ex_wnd_proc, hwnd, uMsg, wParam, lParam);
    
    // If this is a window creation message and we want borderless mode
    if (uMsg == WM_CREATE || uMsg == WM_NCCREATE) {
        extern float s_remove_top_bar;
        if (s_remove_top_bar >= 0.5f) {
            // Get current styles
            LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
            LONG_PTR ex_style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
            
            // Remove title bar and borders
            LONG_PTR new_style = style & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
            LONG_PTR new_ex_style = ex_style & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
            
            // Apply the new styles
            SetWindowLongPtr(hwnd, GWL_STYLE, new_style);
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, new_ex_style);
            
            // Force the window to update with new styles
            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
            
            LogDebug("Reflex: Applied borderless styles during window creation");
        }
    }
    
    return result;
}

// Install window creation hooks
bool InstallWindowCreationHooks() {
    if (g_window_creation_hooks_installed) {
        LogDebug("Window creation hooks already installed");
        return true;
    }
    
    // Get the current swapchain window handle or foreground
    extern std::atomic<HWND> g_last_swapchain_hwnd;
    HWND hwnd = g_last_swapchain_hwnd.load();
    if (hwnd == nullptr) hwnd = GetForegroundWindow();
    if (hwnd == nullptr) {
        LogDebug("No window handle available for window creation hooks");
        return false;
    }
    
    // Store the original window procedure
    g_original_create_window_ex_wnd_proc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
    if (g_original_create_window_ex_wnd_proc == nullptr) {
        LogDebug("Failed to get original window procedure for window creation hooks");
        return false;
    }
    
    // Subclass the window
    if (SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)CreateWindowHookProc) == 0) {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to subclass window for window creation hooks. Error: " << error;
        LogDebug(oss.str().c_str());
        return false;
    }
    
    g_create_window_hook_hwnd = hwnd;
    g_window_creation_hooks_installed = true;
    LogInfo("Window creation hooks installed successfully");
    
    return true;
}

// Uninstall window creation hooks
void UninstallWindowCreationHooks() {
    if (!g_window_creation_hooks_installed || g_create_window_hook_hwnd == nullptr || g_original_create_window_ex_wnd_proc == nullptr) {
        LogDebug("Window creation hooks not installed");
        return;
    }
    
    // Restore the original window procedure
    if (SetWindowLongPtr(g_create_window_hook_hwnd, GWLP_WNDPROC, (LONG_PTR)g_original_create_window_ex_wnd_proc) != 0) {
        g_create_window_hook_hwnd = nullptr;
        g_original_create_window_ex_wnd_proc = nullptr;
        g_window_creation_hooks_installed = false;
        LogInfo("Window creation hooks uninstalled successfully");
    } else {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to uninstall window creation hooks. Error: " << error;
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
                ApplyWindowChange(hwnd, /*do_resize=*/true, want_w, want_h, /*do_move=*/true, pos_x, pos_y, mode, "keyboard_hook_resize_enforce");
                g_in_resize_enforce = false;
                return 0;
            }
        }
    }
    // Remove minimize styles/commands with argument modification
    if (uMsg == WM_NCCALCSIZE) {
        // No direct RemoveWindowBorderLocal here; prefer argument/flag modification elsewhere
    }
    if (uMsg == WM_STYLECHANGING) {
        // Modify style arguments before they are applied
        STYLESTRUCT* ss = reinterpret_cast<STYLESTRUCT*>(lParam);
        if (wParam == GWL_STYLE && ss != nullptr) {
            // Always strip minimize to avoid minimize action
            ss->styleNew &= ~WS_MINIMIZEBOX;
            
            // Conditionally strip maximize based on setting
            extern float s_suppress_maximize;
            if (s_suppress_maximize >= 0.5f) {
                ss->styleNew &= ~WS_MAXIMIZEBOX;
            }
            
            // Respect Remove Top Bar: strip caption/thickframe/sysmenu when enabled
            if (s_remove_top_bar >= 0.5f) {
                ss->styleNew &= ~(WS_CAPTION | WS_THICKFRAME | WS_SYSMENU);
            }
        } else if (wParam == GWL_EXSTYLE && ss != nullptr) {
            if (s_remove_top_bar >= 0.5f) {
                ss->styleNew &= ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
            }
        }
        return 0;
    }
    if (uMsg == WM_SYSCOMMAND) {
        UINT cmd = (UINT)(wParam & 0xFFF0);
        if (cmd == SC_MINIMIZE) {
            // Convert minimize to restore; effectively ignore minimize
            LogDebug("Intercepted SC_MINIMIZE; switching to SC_RESTORE");
            return 0;
        }
        
        // Conditionally intercept maximize based on setting
        extern float s_suppress_maximize;
        if (s_suppress_maximize >= 0.5f && cmd == SC_MAXIMIZE) {
            LogDebug("Intercepted SC_MAXIMIZE; ignoring maximize command");
            return 0;
        }
    }
    
    // Ignore show/hide minimize attempts
    if (uMsg == WM_SHOWWINDOW && wParam == FALSE && IsIconic(hwnd)) {
        LogDebug("Ignored minimize via WM_SHOWWINDOW");
        return 0;
    }
    
    // Reject off-screen minimize geometry; ensure frame changed when removing top bar
    if (uMsg == WM_WINDOWPOSCHANGING) {
        WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
        if (wp != nullptr) {
            if (wp->x < -10000 || wp->y < -10000 || wp->cx < 10 || wp->cy < 10) {
                LogDebug("Rejected minimize geometry in WM_WINDOWPOSCHANGING");
                wp->flags |= (SWP_NOSIZE | SWP_NOMOVE);
                return 0;
            }
            if (s_remove_top_bar >= 0.5f) {
                wp->flags |= SWP_FRAMECHANGED;
            }
        }
    }
    
    if (uMsg == WM_SIZE && wParam == SIZE_MINIMIZED) {
        LogDebug("Ignored SIZE_MINIMIZED");
        return 0;
    }
    
    // Conditionally intercept maximize size changes
    extern float s_suppress_maximize;
    if (s_suppress_maximize >= 0.5f && uMsg == WM_SIZE && wParam == SIZE_MAXIMIZED) {
        LogDebug("Ignored SIZE_MAXIMIZED");
        return 0;
    }
    
    if (uMsg == WM_WINDOWPOSCHANGED) {
        WINDOWPOS* wp = (WINDOWPOS*)lParam;
        if (wp && (wp->x < -10000 || wp->y < -10000 || wp->cx < 10 || wp->cy < 10)) {
            LogDebug("Correcting minimize geometry in WM_WINDOWPOSCHANGED");
            SetWindowPos(hwnd, nullptr, 0, 0, (std::max)(wp->cx, 100), (std::max)(wp->cy, 100),
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            return 0;
        }
    }
    
    // Ignore minimize on deactivate
    if (uMsg == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE && IsIconic(hwnd)) {
        LogDebug("Ignored minimize on deactivation");
        ShowWindow(hwnd, SW_RESTORE);
        return 0;
    }
    
    // Intercept double-click on title bar to prevent maximize
    extern float s_suppress_maximize;
    if (s_suppress_maximize >= 0.5f && uMsg == WM_NCLBUTTONDBLCLK) {
        if (wParam == HTCAPTION) {
            LogDebug("Intercepted double-click on title bar; preventing maximize");
            return 0;
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
                ApplyWindowChange(hwnd, /*do_resize=*/true, want_w, want_h, /*do_move=*/true, pos_x, pos_y, mode, "keyboard_hook_resize_enforcer_delayed");
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

// (Timer-based minimize prevention removed)

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

// Window procedure for handling window creation and lifecycle messages
LRESULT CALLBACK WindowMessageHookProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            // Window is being created
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            if (cs) {
                std::ostringstream oss;
                oss << "Window created: " << hwnd 
                    << " size=" << cs->cx << "x" << cs->cy
                    << " pos=(" << cs->x << "," << cs->y << ")";
                LogInfo(oss.str().c_str());
                
                // Modify creation parameters to remove top bar if setting is enabled
                extern float s_remove_top_bar;
                if (s_remove_top_bar >= 0.5f) {
                    // Modify the window style to remove caption, thickframe, and sysmenu
                    cs->style &= ~(WS_CAPTION | WS_THICKFRAME | WS_SYSMENU);
                    // Also remove minimize/maximize buttons
                    cs->style &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
                    LogInfo("Modified WM_CREATE parameters to remove top bar");
                }
            } else {
                LogInfo("Window created");
            }
            break;
        }
        
        case WM_DESTROY: {
            // Window is being destroyed
            LogInfo("Window destroyed");
            break;
        }
        
        case WM_NCCREATE: {
            // Non-client area is being created
            LogDebug("Window non-client area created");
            
            // Early opportunity to modify window creation parameters
            // This is called before WM_CREATE, so we can modify the CREATESTRUCT
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            if (cs) {
                extern float s_remove_top_bar;
                if (s_remove_top_bar >= 0.5f) {
                    // Modify the window style to remove caption, thickframe, and sysmenu
                    cs->style &= ~(WS_CAPTION | WS_THICKFRAME | WS_SYSMENU);
                    // Also remove minimize/maximize buttons
                    cs->style &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
                    LogDebug("Modified WM_NCCREATE parameters to remove top bar");
                }
            }
            break;
        }
        
        case WM_NCDESTROY: {
            // Non-client area is being destroyed
            LogDebug("Window non-client area destroyed");
            break;
        }
        
        case WM_PARENTNOTIFY: {
            // Parent window notification (child window events)
            if (wParam == WM_CREATE) {
                LogDebug("Child window created");
            } else if (wParam == WM_DESTROY) {
                LogDebug("Child window destroyed");
            }
            break;
        }
        
        case WM_ACTIVATEAPP: {
            // Application activation state changed
            bool activated = (wParam != FALSE);
            LogInfo(activated ? "Application activated" : "Application deactivated");
            break;
        }
        
        case WM_WINDOWPOSCHANGING: {
            // Window position/size is about to change
            WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
            if (wp && !(wp->flags & SWP_NOMOVE)) {
                std::ostringstream oss;
                oss << "Window position changing to (" << wp->x << "," << wp->y << ")";
                LogDebug(oss.str().c_str());
            }
            break;
        }
        
        case WM_WINDOWPOSCHANGED: {
            // Window position/size has changed
            WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
            if (wp && !(wp->flags & SWP_NOMOVE)) {
                std::ostringstream oss;
                oss << "Window position changed to (" << wp->x << "," << wp->y << ")";
                LogDebug(oss.str().c_str());
            }
            break;
        }
        
        case WM_ENTERSIZEMOVE: {
            // User started resizing/moving the window
            LogDebug("Window resize/move started");
            break;
        }
        
        case WM_EXITSIZEMOVE: {
            // User finished resizing/moving the window
            LogDebug("Window resize/move finished");
            break;
        }
        
        case WM_GETMINMAXINFO: {
            // Window is querying min/max size info
            LogDebug("Window querying min/max info");
            
            // If top bar removal is enabled, prevent maximization
            extern float s_remove_top_bar;
            if (s_remove_top_bar >= 0.5f) {
                MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
                if (mmi) {
                    // Set max size to current size to prevent maximization
                    RECT rect;
                    if (GetWindowRect(hwnd, &rect)) {
                        mmi->ptMaxSize.x = rect.right - rect.left;
                        mmi->ptMaxSize.y = rect.bottom - rect.top;
                        mmi->ptMaxPosition.x = rect.left;
                        mmi->ptMaxPosition.y = rect.top;
                        LogDebug("Modified WM_GETMINMAXINFO to prevent maximization");
                    }
                }
            }
            break;
        }
        
        case WM_SIZING: {
            // Window is being resized
            LogDebug("Window resizing");
            break;
        }
        
        case WM_MOVING: {
            // Window is being moved
            LogDebug("Window moving");
            break;
        }
    }
    
    // Always call the original window procedure for all messages
    return CallWindowProc(g_original_window_message_wnd_proc, hwnd, uMsg, wParam, lParam);
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

// Install the window message hook
bool InstallWindowMessageHook() {
    if (g_window_message_subclassed_hwnd != nullptr) {
        LogWarn("Window message hook already installed");
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
        LogWarn("No window handle available for window message hook");
        return false;
    }
    
    // Store the original window procedure
    g_original_window_message_wnd_proc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
    if (g_original_window_message_wnd_proc == nullptr) {
        LogWarn("Failed to get original window procedure for window message hook");
        return false;
    }
    
    // Subclass the window
    if (SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WindowMessageHookProc) == 0) {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to subclass window for window message hook. Error: " << error;
        LogWarn(oss.str().c_str());
        return false;
    }
    
    g_window_message_subclassed_hwnd = hwnd;
    LogInfo("Window message hook installed via window subclassing");
    return true;
}

// Uninstall the window message hook
void UninstallWindowMessageHook() {
    if (g_window_message_subclassed_hwnd == nullptr || g_original_window_message_wnd_proc == nullptr) {
        LogDebug("Window message hook not installed");
        return;
    }
    
    // Restore the original window procedure
    if (SetWindowLongPtr(g_window_message_subclassed_hwnd, GWLP_WNDPROC, (LONG_PTR)g_original_window_message_wnd_proc) != 0) {
        g_window_message_subclassed_hwnd = nullptr;
        g_original_window_message_wnd_proc = nullptr;
        LogInfo("Window message hook uninstalled successfully");
    } else {
        DWORD error = GetLastError();

        std::ostringstream oss;
        oss << "Failed to uninstall window message hook. Error: " << error;
        LogWarn(oss.str().c_str());
    }
}

// Install all window hooks
bool InstallAllWindowHooks() {
    bool success = true;
    
    // Try to install window creation hooks first
    if (!InstallWindowCreationHooks()) {
        LogDebug("Window creation hooks failed to install");
        success = false;
    }
    
    // Try to install minimize prevention
    if (!InstallMinimizeHook()) {
        LogDebug("Minimize prevention failed to install");
        success = false;
    }
    
    // Try to install resize enforcer
    if (!InstallResizeEnforcerHook()) {
        LogDebug("Resize enforcer failed to install");
        success = false;
    }
    
    // Try to install CreateWindowEx hooks
    if (!InstallCreateWindowExHooks()) {
        LogDebug("CreateWindowEx hooks failed to install");
        success = false;
    }
    
    if (success) {
        LogInfo("All window hooks installed successfully");
    } else {
        LogWarn("Some window hooks failed to install");
    }
    
    return success;
}

// Uninstall all window hooks
void UninstallAllWindowHooks() {
    UninstallWindowCreationHooks();
    UninstallMinimizeHook();
    UninstallResizeEnforcerHook();
    UninstallCreateWindowExHooks();
    LogInfo("All window hooks uninstalled");
}

// Aggressively apply borderless styles to existing windows
void ApplyBorderlessStylesAggressively() {
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
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
        
        // Verify the styles were applied
        LONG_PTR verify_style = GetWindowLongPtr(hwnd, GWL_STYLE);
        LONG_PTR verify_ex_style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        
        if (verify_style == new_style && verify_ex_style == new_ex_style) {
            LogDebug("Reflex: Successfully applied borderless styles to existing window");
        } else {
            LogWarn("Reflex: Failed to apply borderless styles - styles not properly set");
        }
    }
}

// Check all top-level windows and apply borderless styles if needed
void ApplyBorderlessStylesToAllWindows() {
    extern float s_remove_top_bar;
    if (s_remove_top_bar < 0.5f) {
        return; // Borderless mode not enabled
    }
    
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
            // Remove title bar and borders
            LONG_PTR new_style = style & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
            LONG_PTR new_ex_style = ex_style & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
            
            // Apply the new styles
            SetWindowLongPtr(hwnd, GWL_STYLE, new_style);
            SetWindowLongPtr(hwnd, GWL_EXSTYLE, new_ex_style);
            
            // Force the window to update with new styles
            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
            
            LogDebug("Reflex: Applied borderless styles to top-level window");
        }
        
        return TRUE;
    };
    
    // Enumerate all top-level windows
    EnumWindows(enumWindowsProc, 0);
}

// Timer-based borderless style enforcement
static UINT_PTR g_borderless_timer_id = 0;
static bool g_borderless_timer_active = false;

// Timer callback for enforcing borderless styles
VOID CALLBACK BorderlessStyleTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    // Try to apply borderless styles aggressively to the main window
    ApplyBorderlessStylesAggressively();
    
    // Also check all top-level windows periodically
    static int all_windows_counter = 0;
    if (++all_windows_counter % 50 == 0) { // Every 5 seconds
        ApplyBorderlessStylesToAllWindows();
    }
    
    // Also try to install hooks if they're not working
    static int hook_retry_counter = 0;
    if (++hook_retry_counter % 100 == 0) { // Every 10 seconds
        extern std::atomic<HWND> g_last_swapchain_hwnd;
        HWND current_hwnd = g_last_swapchain_hwnd.load();
        if (current_hwnd != nullptr && !g_window_creation_hooks_installed) {
            LogDebug("Reflex: Retrying to install window creation hooks");
            InstallWindowCreationHooks();
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

// Start the borderless style enforcement timer
void StartBorderlessStyleTimer() {
    if (g_borderless_timer_active) {
        return; // Timer already active
    }
    
    extern float s_remove_top_bar;
    if (s_remove_top_bar < 0.5f) {
        return; // Borderless mode not enabled
    }
    
    // Set a timer to check and apply borderless styles every 50ms initially, then slow down
    g_borderless_timer_id = SetTimer(nullptr, 0, 50, BorderlessStyleTimerProc);
    if (g_borderless_timer_id != 0) {
        g_borderless_timer_active = true;
        LogDebug("Reflex: Started borderless style enforcement timer (50ms initial interval)");
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
