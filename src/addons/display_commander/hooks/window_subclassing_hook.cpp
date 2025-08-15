#include "window_subclassing_hook.hpp"
#include "../addon.hpp"
#include "../utils.hpp"
#include <sstream>
#include <thread>

// Global variables for minimize prevention
static WNDPROC g_original_wnd_proc = nullptr;
static HWND g_subclassed_hwnd = nullptr;

// Global variables for resize enforcement
static WNDPROC g_original_resize_wnd_proc = nullptr;
static HWND g_resize_enforcer_hwnd = nullptr;
static bool g_in_resize_enforce = false;

// Window procedure for intercepting minimize commands (now uses delayed approach)
LRESULT CALLBACK MinimizeWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Optional: enforce desired window size/position on resize/move
    extern float s_enforce_desired_window;
    if (s_enforce_desired_window >= 0.5f && !g_in_resize_enforce) {
        if (uMsg == WM_SIZING || uMsg == WM_SIZE || uMsg == WM_WINDOWPOSCHANGING) {
            // Schedule delayed window enforcement instead of immediate application
            std::thread([hwnd, uMsg]() {
                // Small delay to prevent immediate execution within the hook
                std::this_thread::sleep_for(std::chrono::milliseconds(75));
                
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
           //         ApplyWindowChange(hwnd, /*do_resize=*/true, want_w, want_h, /*do_move=*/true, pos_x, pos_y, mode, "window_subclassing_hook_resize_enforce");
                    g_in_resize_enforce = false;
                }
            }).detach();
            
            return 0;
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
            wParam = (wParam & 0xFFFF0000) | SC_RESTORE;
        }
    }
    
    // Call the original window procedure
    return CallWindowProc(g_original_wnd_proc, hwnd, uMsg, wParam, lParam);
}

// Window procedure enforcing desired size/position on resize/move (now uses delayed approach)
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
            // Schedule delayed window enforcement instead of immediate application
            std::thread([hwnd, uMsg]() {
                // Small delay to prevent immediate execution within the hook
                std::this_thread::sleep_for(std::chrono::milliseconds(75));
                
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
            //        ApplyWindowChange(hwnd, /*do_resize=*/true, want_w, want_h, /*do_move=*/true, pos_x, pos_y, mode, "resize_enforcer_wndproc_delayed");
                }
            }).detach();
            
            return 0; // handled
        }
        default:
            break;
    }

    return CallWindowProc(g_original_resize_wnd_proc, hwnd, uMsg, wParam, lParam);
}

// Install the Windows minimize prevention using window subclassing (now uses delayed approach for extra safety)
bool InstallMinimizeHook() {
    if (g_subclassed_hwnd != nullptr) {
        LogWarn("Minimize prevention already installed");
        return true;
    }
    
    // Schedule delayed hook installation instead of immediate installation
    std::thread([]() {
        // Small delay to prevent immediate execution
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // Get the current swapchain window handle
        extern std::atomic<HWND> g_last_swapchain_hwnd;
        HWND hwnd = g_last_swapchain_hwnd.load();
        
        if (hwnd == nullptr) {
            // Try to get the foreground window if no swapchain window
            hwnd = GetForegroundWindow();
        }
        
        if (hwnd == nullptr) {
            LogWarn("No window handle available for minimize prevention");
            return;
        }
        
        // Store the original window procedure
        g_original_wnd_proc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
        if (g_original_wnd_proc == nullptr) {
            LogWarn("Failed to get original window procedure");
            return;
        }
        
        // Subclass the window
        if (SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)MinimizeWndProc) == 0) {
            DWORD error = GetLastError();
            std::ostringstream oss;
            oss << "Failed to subclass window for minimize prevention. Error: " << error;
            LogWarn(oss.str().c_str());
            return;
        }
        
        g_subclassed_hwnd = hwnd;
        LogInfo("Windows minimize prevention installed via window subclassing (delayed)");
    }).detach();
    
    return true; // Return true immediately, actual installation happens in background
}

// Install the resize enforcer using window subclassing (now uses delayed approach for extra safety)
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

    // Schedule delayed hook installation instead of immediate installation
    std::thread([]() {
        // Small delay to prevent immediate execution
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // Get the current swapchain window handle or foreground
        extern std::atomic<HWND> g_last_swapchain_hwnd;
        HWND hwnd = g_last_swapchain_hwnd.load();
        if (hwnd == nullptr) hwnd = GetForegroundWindow();
        if (hwnd == nullptr) {
            LogWarn("No window handle available for resize enforcer");
            return;
        }

        g_original_resize_wnd_proc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
        if (g_original_resize_wnd_proc == nullptr) {
            LogWarn("Failed to get original window procedure for resize enforcer");
            return;
        }

        if (SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)ResizeEnforcerWndProc) == 0) {
            DWORD error = GetLastError();
            std::ostringstream oss; oss << "Failed to subclass window for resize enforcer. Error: " << error;
            LogWarn(oss.str().c_str());
            return;
        }

        g_resize_enforcer_hwnd = hwnd;
        LogInfo("Resize enforcer installed via window subclassing (delayed)");
    }).detach();
    
    return true; // Return true immediately, actual installation happens in background
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
