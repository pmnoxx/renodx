#include "window_style_hooks.hpp"
#include "../addon.hpp"
#include <sstream>

namespace renodx::hooks {

// Global variables for hook management
HWND g_hooked_window = nullptr;
WNDPROC g_original_window_proc = nullptr;
bool g_hooks_installed = false;

// Hooked window procedure that intercepts style-changing messages
LRESULT CALLBACK WindowStyleHookProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Check if we should prevent style changes
    if (s_remove_top_bar >= 0.5f) {
        switch (uMsg) {
            case WM_WINDOWPOSCHANGING: {
                // Intercept window position/style changes
                WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
                if (wp && (wp->flags & SWP_FRAMECHANGED)) {
                    // Prevent frame changes that would add borders
                    wp->flags &= ~SWP_FRAMECHANGED;
                    LogDebug("Window style hook: Prevented frame change");
                }
                break;
            }
            
            case WM_STYLECHANGING: {
                // Intercept style change requests
                STYLESTRUCT* style = reinterpret_cast<STYLESTRUCT*>(lParam);
                if (style) {
                    // Remove window styles that add borders
                    DWORD remove_styles = WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
                    style->styleNew &= ~remove_styles;
                    style->styleOld &= ~remove_styles;
                    LogDebug("Window style hook: Modified style change request");
                }
                break;
            }
            
            case WM_STYLECHANGED: {
                // Intercept style change confirmations
                STYLESTRUCT* style = reinterpret_cast<STYLESTRUCT*>(lParam);
                if (style) {
                    // Force borderless style
                    DWORD remove_styles = WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
                    DWORD current_style = GetWindowLong(hwnd, GWL_STYLE);
                    DWORD new_style = current_style & ~remove_styles;
                    
                    if (current_style != new_style) {
                        SetWindowLong(hwnd, GWL_STYLE, new_style);
                        LogDebug("Window style hook: Forced borderless style");
                    }
                }
                break;
            }
            
            case WM_SYSCOMMAND: {
                // Prevent system menu commands that could restore borders
                if (wParam == SC_RESTORE || wParam == SC_MAXIMIZE) {
                    LogDebug("Window style hook: Blocked system command");
                    return 0; // Block the command
                }
                break;
            }
        }
    }
    
    // Call the original window procedure
    if (g_original_window_proc) {
        return CallWindowProc(g_original_window_proc, hwnd, uMsg, wParam, lParam);
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void InstallWindowStyleHooks() {
    if (g_hooks_installed) {
        LogWarn("Window style hooks already installed");
        return;
    }
    
    // Get the current swapchain window
    HWND hwnd = g_last_swapchain_hwnd.load();
    if (hwnd == nullptr) {
        hwnd = GetForegroundWindow();
    }
    
    if (hwnd == nullptr || !IsWindow(hwnd)) {
        LogWarn("No valid window found for style hooks");
        return;
    }
    
    // Store the window handle
    g_hooked_window = hwnd;
    
    // Get the current window procedure
    g_original_window_proc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hwnd, GWLP_WNDPROC));
    if (g_original_window_proc == nullptr) {
        LogWarn("Failed to get original window procedure");
        return;
    }
    
    // Install the hook
    if (SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowStyleHookProc)) == 0) {
        LogWarn("Failed to install window style hook");
        return;
    }
    
    g_hooks_installed = true;
    
    std::ostringstream oss;
    oss << "Window style hooks installed for window " << hwnd;
    LogInfo(oss.str().c_str());
    
    // Force immediate style application
    if (s_remove_top_bar >= 0.5f) {
        DWORD current_style = GetWindowLong(hwnd, GWL_STYLE);
        DWORD remove_styles = WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
        DWORD new_style = current_style & ~remove_styles;
        
        if (current_style != new_style) {
            SetWindowLong(hwnd, GWL_STYLE, new_style);
            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            LogInfo("Window style hooks: Applied borderless style immediately");
        }
    }
}

void UninstallWindowStyleHooks() {
    if (!g_hooks_installed) {
        LogWarn("Window style hooks not installed");
        return;
    }
    
    if (g_hooked_window && g_original_window_proc) {
        // Restore the original window procedure
        SetWindowLongPtr(g_hooked_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_original_window_proc));
        
        std::ostringstream oss;
        oss << "Window style hooks uninstalled from window " << g_hooked_window;
        LogInfo(oss.str().c_str());
    }
    
    // Reset state
    g_hooked_window = nullptr;
    g_original_window_proc = nullptr;
    g_hooks_installed = false;
}

bool AreWindowStyleHooksInstalled() {
    return g_hooks_installed;
}

} // namespace renodx::hooks
