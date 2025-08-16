#include "window_style_hooks.hpp"
#include "../addon.hpp"
#include <sstream>

namespace renodx::hooks {

// Global variables for hook management
HWND g_hooked_window = nullptr;
WNDPROC g_original_window_proc = nullptr;
bool g_hooks_installed = false;

// Hooked window procedure that intercepts style-changing messages
// NOTE: Do not make any system calls within this function - only log and modify message parameters
// Background tasks will handle the actual window state enforcement later
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

    // Check if we should suppress move/resize messages
    if (s_suppress_move_resize_messages >= 0.5f) {
        switch (uMsg) {
            case WM_MOVE: {
                // Intercept window move messages
                int new_x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
                int new_y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
                
                // Get our desired position from window state calculator
                int desired_x = static_cast<int>(s_windowed_pos_x);
                int desired_y = static_cast<int>(s_windowed_pos_y);
                
                // Check if this move matches our desired state
                if (new_x != desired_x || new_y != desired_y) {
                    std::ostringstream oss;
                    oss << "Window style hook: Suppressed move message - position mismatch. Game wants (" << new_x << "," << new_y << "), we want (" << desired_x << "," << desired_y << "). Background task will fix this later.";
                    LogDebug(oss.str());
                    
                    // Modify the message to keep current position (suppress the move)
                    // We'll use the current window position as the "new" position
                    RECT window_rect;
                    if (GetWindowRect(hwnd, &window_rect)) {
                        lParam = MAKELONG(static_cast<short>(window_rect.left), static_cast<short>(window_rect.top));
                    }
                }
                break;
            }
            
            case WM_SIZE: {
                // Intercept window size messages
                int new_w = static_cast<int>(LOWORD(lParam));
                int new_h = static_cast<int>(HIWORD(lParam));
                
                // Get our desired size from window state calculator
                int desired_w = static_cast<int>(s_windowed_width);
                int desired_h = static_cast<int>(s_windowed_height);
                
                // Check if this resize matches our desired state
                if (new_w != desired_w || new_h != desired_h) {
                    std::ostringstream oss;
                    oss << "Window style hook: Suppressed resize message - size mismatch. Game wants " << new_w << "x" << new_h << ", we want " << desired_w << "x" << desired_h << ". Background task will fix this later.";
                    LogDebug(oss.str());
                    
                    // Modify the message to keep current size (suppress the resize)
                    // We'll use the current window size as the "new" size
                    RECT client_rect;
                    if (GetClientRect(hwnd, &client_rect)) {
                        lParam = MAKELONG(static_cast<short>(client_rect.right - client_rect.left), 
                                         static_cast<short>(client_rect.bottom - client_rect.top));
                    }
                }
                break;
            }
            
            case WM_WINDOWPOSCHANGED: {
                // Intercept window position/size change confirmations
                WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
                if (wp) {
                    // Get our desired state from window state calculator
                    int desired_w = static_cast<int>(s_windowed_width);
                    int desired_h = static_cast<int>(s_windowed_height);
                    int desired_x = static_cast<int>(s_windowed_pos_x);
                    int desired_y = static_cast<int>(s_windowed_pos_y);
                    
                    // Check if this change matches our desired state
                    bool position_mismatch = (wp->x != desired_x || wp->y != desired_y);
                    bool size_mismatch = (wp->cx != desired_w || wp->cy != desired_h);
                    
                    if (position_mismatch || size_mismatch) {
                        std::ostringstream oss;
                        oss << "Window style hook: Suppressed windowpos change - mismatch detected. Game wants pos(" << wp->x << "," << wp->y << ") size(" << wp->cx << "x" << wp->cy << "), we want pos(" << desired_x << "," << desired_y << ") size(" << desired_w << "x" << desired_h << "). Background task will fix this later.";
                        LogDebug(oss.str());
                        
                        // Modify the message to keep current state (suppress the change)
                        // We'll use the current window position as the "new" position
                        RECT window_rect;
                        if (GetWindowRect(hwnd, &window_rect)) {
                            wp->x = window_rect.left;
                            wp->y = window_rect.top;
                        }
                        
                        RECT client_rect;
                        if (GetClientRect(hwnd, &client_rect)) {
                            wp->cx = client_rect.right - client_rect.left;
                            wp->cy = client_rect.bottom - client_rect.top;
                        }
                    }
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
