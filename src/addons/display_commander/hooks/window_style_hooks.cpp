#include "window_style_hooks.hpp"
#include "../addon.hpp"
#include "../window_management/window_management.hpp"
#include <sstream>

namespace renodx::hooks {

// No external declarations needed - variables are accessible from addon.hpp

// Global variables for hook management
HWND g_hooked_window = nullptr;
WNDPROC g_original_window_proc = nullptr;
bool g_hooks_installed = false;

// Hooked window procedure that intercepts style-changing messages and focus loss messages
// NOTE: Do not make any system calls within this function - only log and modify message parameters
// Background tasks will handle the actual window state enforcement later
LRESULT CALLBACK WindowStyleHookProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Disable logic for now.
    if (false) {

        // DEBUG: Log that our hook is active (only for important messages to avoid spam)
        if (uMsg == WM_SYSCOMMAND || uMsg == WM_SIZE || uMsg == WM_GETMINMAXINFO) {
            LogDebug("Window style hook: ACTIVE - Processing message 0x" + std::to_string(uMsg));
        }
        
        // SPOOF FOCUS FEATURE - Prevent games from knowing they lost focus through Win32 message suppression
        if (s_force_continuous_rendering >= 0.5f) {
            switch (uMsg) {
                case WM_KILLFOCUS:
                    // Suppress focus loss messages to keep the game rendering
                    LogDebug("Window style hook: SUPPRESSED WM_KILLFOCUS - Preventing game from knowing it lost focus");
                    
            /*
                    if (s_prevent_always_on_top >= 0.5f) {
                        // Try to release any captured input devices
                        ReleaseCapture(); // Release mouse capture
                        ClipCursor(nullptr); // Release cursor clipping
                        LogDebug("Window style hook: Released mouse capture and cursor clipping to prevent confinement");
                    }*/
                    
                    return 0; // Block the message completely
                    
                case WM_ACTIVATE:
                    if (LOWORD(wParam) == WA_INACTIVE) {
                        // Suppress deactivation messages to keep the game active
                        LogDebug("Window style hook: SUPPRESSED WM_ACTIVATE(WA_INACTIVE) - Keeping game active");
                        
                /*
                        if (s_prevent_always_on_top >= 0.5f) {
                            ReleaseCapture(); // Release mouse capture
                            ClipCursor(nullptr); // Release cursor clipping
                            LogDebug("Window style hook: Released mouse capture and cursor clipping on deactivation");
                        } */
                        
                        return 0; // Block the message completely
                    }
                    break;
                    
                case WM_ACTIVATEAPP:
                    if (wParam == FALSE) {
                        // Suppress app deactivation messages
                        LogDebug("Window style hook: SUPPRESSED WM_ACTIVATEAPP(FALSE) - Preventing app deactivation");
                        return 0; // Block the message completely
                    }
                    break;
                    
                case WM_NCACTIVATE:
                    if (wParam == FALSE) {
                        // Suppress non-client area deactivation messages
                        LogDebug("Window style hook: SUPPRESSED WM_NCACTIVATE(FALSE) - Preventing non-client deactivation");
                        return 0; // Block the message completely
                    }
                    break;
                    
                case WM_SETFOCUS:
                    // Always allow focus gain, but log it
                    LogDebug("Window style hook: ALLOWED WM_SETFOCUS - Game gained focus (this is good)");
                    break;
                    
                case WM_MOUSEACTIVATE:
                    // Prevent mouse activation from deactivating the game
                    if (s_force_continuous_rendering >= 0.5f) {
                        LogDebug("Window style hook: SUPPRESSED WM_MOUSEACTIVATE - Preventing mouse deactivation");
                        return MA_ACTIVATE; // Force activation
                    }
                    break;
                    
                case WM_WINDOWPOSCHANGING:
                    // Prevent window position changes from affecting focus
                    if (s_force_continuous_rendering >= 0.5f) {
                        // Allow position changes but ensure focus stays
                        LogDebug("Window style hook: ALLOWED WM_WINDOWPOSCHANGING - Position change with focus preservation");
                    }
                    break;
            }
        }
        
        // Check if we should prevent style changes
        if (s_remove_top_bar >= 0.5f) {
            switch (uMsg) {
                case WM_WINDOWPOSCHANGING: {
                    LogDebug("Window style hook: WM_WINDOWPOSCHANGING - lParam = " + std::to_string(lParam));
                    // Intercept window position/style changes
                    WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
                    if (wp) {
                        // ENHANCED LOGGING: Log all window position changes to debug top border issues
                        std::ostringstream oss;
                        oss << "Window style hook: WM_WINDOWPOSCHANGING - Flags: 0x" << std::hex << wp->flags 
                            << ", Pos: (" << wp->x << "," << wp->y << "), Size: (" << wp->cx << "x" << wp->cy << ")";
                        
                        if (wp->flags & SWP_FRAMECHANGED) {
                            oss << " [FRAME CHANGE DETECTED - Preventing borders!]";
                            // Prevent frame changes that would add borders
                            wp->flags &= ~SWP_FRAMECHANGED;
                            LogInfo(oss.str().c_str());
                        } else {
                            LogDebug(oss.str().c_str());
                        }
                    }
                    break;
                }
                
                case WM_STYLECHANGING: {
                    LogDebug("Window style hook: WM_STYLECHANGING - lParam = " + std::to_string(lParam));
                    // Intercept style change requests
                    STYLESTRUCT* style = reinterpret_cast<STYLESTRUCT*>(lParam);
                    if (style) {
                        // ENHANCED LOGGING: Log all style changes to debug top border issues
                        DWORD remove_styles = WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
                        DWORD old_style = style->styleOld;
                        DWORD new_style = style->styleNew;
                        
                        std::ostringstream oss;
                        oss << "Window style hook: WM_STYLECHANGING - Old: 0x" << std::hex << old_style 
                            << ", New: 0x" << std::hex << new_style;
                        
                        // Check if any unwanted styles are being added
                        DWORD unwanted_added = (new_style & remove_styles) & ~(old_style & remove_styles);
                        if (unwanted_added) {
                            oss << " [UNWANTED STYLES DETECTED: 0x" << std::hex << unwanted_added << " - Removing!]";
                            LogInfo(oss.str().c_str());
                        } else {
                            LogDebug(oss.str().c_str());
                        }
                        
                        // Remove window styles that add borders
                        style->styleNew &= ~remove_styles;
                    //    style->styleOld &= ~remove_styles;
                    }
                    
                    // PREVENT ALWAYS ON TOP: Block WS_EX_TOPMOST and WS_EX_TOOLWINDOW styles
                    if (s_prevent_always_on_top >= 0.5f) {
                        // Check if always on top styles are being added
                        DWORD remove_ex_styles = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
                        DWORD old_ex_style = style->styleOld;
                        DWORD new_ex_style = style->styleNew;
                        
                        DWORD unwanted_ex_added = (new_ex_style & remove_ex_styles) & ~(old_ex_style & remove_ex_styles);
                        if (unwanted_ex_added) {
                            std::ostringstream oss;
                            oss << "Window style hook: PREVENTING ALWAYS ON TOP - Removing extended styles 0x" << std::hex << unwanted_ex_added;
                            LogInfo(oss.str().c_str());
                            
                            // Remove always on top extended styles
                            style->styleNew &= ~remove_ex_styles;
                        //  style->styleOld &= ~remove_ex_styles;
                        }
                    }
                    break;
                }
                
                case WM_STYLECHANGED: {
                    LogDebug("Window style hook: WM_STYLECHANGED - lParam = " + std::to_string(lParam));
                    // Intercept style change confirmations
                    STYLESTRUCT* style = reinterpret_cast<STYLESTRUCT*>(lParam);
                    if (style) {
                        // Log the style change but don't modify it here
                        // The background task will enforce the borderless style
                        DWORD remove_styles = WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
                        DWORD current_style = style->styleNew;
                        style->styleNew &= ~remove_styles;
                        
                        if (current_style & remove_styles) {
                            std::ostringstream oss;
                            oss << "Window style hook: Style change detected with unwanted styles. Game wants style 0x" << std::hex << current_style << ". Background task will enforce borderless style.";
                            LogDebug(oss.str());
                            
                            // Mark that we need to enforce styles later
                            // Don't call SetWindowLong here - let background task handle it
                        }
                    }
                    break;
                }
                
                case WM_SYSCOMMAND: {
                    LogDebug("Window style hook: WM_SYSCOMMAND - wParam = " + std::to_string(wParam));
                    // AGGRESSIVE: Block ALL maximize commands when s_suppress_maximize is enabled
                    if (s_suppress_maximize >= 0.5f && wParam == SC_MAXIMIZE) {
                        LogDebug("Window style hook: AGGRESSIVELY BLOCKED SC_MAXIMIZE - No maximize allowed at all");
                        return 0; // Block ALL maximize commands completely
                    }
                    
                    // Smart restore: Allow restore only when window is minimized or maximized
                    if (wParam == SC_RESTORE) {
                        // Check current window state
                        WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
                        if (GetWindowPlacement(hwnd, &wp)) {
                            if (wp.showCmd == SW_SHOWMINIMIZED || wp.showCmd == SW_SHOWMAXIMIZED) {
                                // Window is minimized or maximized - allow restore to normal state
                                LogDebug("Window style hook: ALLOWED SC_RESTORE - Window is " + 
                                    std::string(wp.showCmd == SW_SHOWMINIMIZED ? "minimized" : "maximized") + 
                                    ", restoring to normal state");
                                break; // Allow the restore command
                            } else {
                                // Window is already normal - block unnecessary restore
                                LogDebug("Window style hook: Blocked SC_RESTORE - Window is already in normal state");
                        //     return 0; // Block the command
                            }
                        } else {
                            // Couldn't get window state - block to be safe
                            LogDebug("Window style hook: Blocked SC_RESTORE - Couldn't determine window state");
                        // return 0; // Block the command
                        }
                    }
                    
                    // Also handle SC_MINIMIZE to prevent unwanted minimization
                    if (wParam == SC_MINIMIZE) {
                        LogDebug("Window style hook: Blocked SC_MINIMIZE - Preventing unwanted minimization");
                        return 0; // Block minimize commands
                    }
                    break;
                }

                case WM_GETMINMAXINFO: {
                    LogDebug("Window style hook: WM_GETMINMAXINFO - lParam = " + std::to_string(lParam));
                    // Prevent the window from being maximized or restored
                    MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
                    if (mmi) {
                        mmi->ptMaxPosition.x = g_window_state.target_x;
                        mmi->ptMaxPosition.y = g_window_state.target_y;
                        mmi->ptMaxSize.x = g_window_state.target_w;
                        mmi->ptMaxSize.y = g_window_state.target_h;
                    }
                    break;
                }

                case WM_NCCALCSIZE: {
                    LogDebug("Window style hook: WM_NCCALCSIZE - lParam = " + std::to_string(lParam));
                    // Prevent non-client area calculation (title bar, borders)
                    if (s_remove_top_bar >= 0.5f && wParam == TRUE) {
                        NCCALCSIZE_PARAMS* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
                        if (params) {
                            // ENHANCED LOGGING: Log NCCALCSIZE to debug top border removal
                            std::ostringstream oss;
                            oss << "Window style hook: WM_NCCALCSIZE - Window: (" << params->rgrc[1].left << "," << params->rgrc[1].top 
                                << ") to (" << params->rgrc[1].right << "," << params->rgrc[1].bottom << ")";
                            
                            // Calculate desired window state to get current target position
                            CalculateWindowState(hwnd, "hook_nccalcsize");
                            
                            // Use global window position data instead of hardcoding (0,0)
                            // This ensures the client area respects the actual window position
                            params->rgrc[0].left = g_window_state.target_x;
                            params->rgrc[0].top = g_window_state.target_y;
                            params->rgrc[0].right = g_window_state.target_x + g_window_state.target_w;
                            params->rgrc[0].bottom = g_window_state.target_y + g_window_state.target_h;
                            
                            oss << " -> Client: (" << params->rgrc[0].left << "," << params->rgrc[0].top 
                                << ") to (" << params->rgrc[0].right << "," << params->rgrc[0].bottom << ") [TOP BORDER REMOVED with global position!]";
                            LogInfo(oss.str().c_str());
                            
                        //  return 0; // Don't call default handler
                        }
                    } 
                    break;
                }
            }
        }

        // Check if we should suppress move/resize messages
        if (s_suppress_move_resize_messages >= 0.5f) {
            // Suppress messages immediately when enabled
            switch (uMsg) {
                case WM_MOVE: {
                    LogDebug("Window style hook: WM_MOVE - lParam = " + std::to_string(lParam));
                    // Intercept window move messages
                    int new_x = static_cast<int>(static_cast<short>(LOWORD(lParam)));
                    int new_y = static_cast<int>(static_cast<short>(HIWORD(lParam)));
                    
                    // Calculate desired window state to get current target position
                    CalculateWindowState(hwnd, "hook_move_check");
                    
                    // Get our desired position from global window state
                    int desired_x = g_window_state.target_x;
                    int desired_y = g_window_state.target_y;
                    
                    lParam = MAKELONG(desired_x, desired_y);
                    break;
                }
                
                case WM_SIZE: {
                    LogDebug("Window style hook: WM_SIZE - lParam = " + std::to_string(lParam));
                    // Intercept window size messages
                    int new_w = static_cast<int>(LOWORD(lParam));
                    int new_h = static_cast<int>(HIWORD(lParam));
                    WPARAM size_type = wParam;
                    
                    // Check if we should suppress maximize
                    if (s_suppress_maximize >= 0.5f && size_type == SIZE_MAXIMIZED) {
                        LogDebug("Window style hook: SUPPRESSED maximize size message - blocking maximize completely");
                        // Block maximize completely - don't even process the message
                        return 0; // Don't call default handler
                    }
                    
                    // Allow restore from maximized state (SIZE_RESTORED)
                    
                    // Calculate desired window state to get current target size
                    CalculateWindowState(hwnd, "hook_size_check");
                    
                    lParam = MAKELONG(g_window_state.target_w, g_window_state.target_h);
                    if (size_type == SIZE_RESTORED) {
                        LogDebug("Window style hook: ALLOWED SIZE_RESTORED - Window restoring from maximized/minimized state");
                        // Allow the restore message to process normally
                    //   break;
                    }
                    break;
                }
                
                case WM_WINDOWPOSCHANGED: {
                    // Intercept window position/size change confirmations
                    WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
                    LogDebug("Window style hook: WM_WINDOWPOSCHANGED - wp = " + std::to_string(wp->cx) + ", " + std::to_string(wp->cy) + ", " + std::to_string(wp->x) + ", " + std::to_string(wp->y) + ", " + std::to_string(wp->flags));
                    if (wp) {
                        // Calculate desired window state to get current target size
                        CalculateWindowState(hwnd, "hook_windowpos_check");
                        
                        LogDebug("Window style hook: WM_WINDOWPOSCHANGED old wp.(cx, cy, x, y, flags) = (" + std::to_string(wp->cx) + ", " + std::to_string(wp->cy) + ", " + std::to_string(wp->x) + ", " + std::to_string(wp->y) + ", " + std::to_string(wp->flags ) + ")");
            
                        wp -> cx = g_window_state.target_w;
                        wp -> cy = g_window_state.target_h;   
                        wp -> x = g_window_state.target_x;
                        wp -> y = g_window_state.target_y;

                        // TODO: which flags should be changed in wp -> flags?
                        
                        // Update flags to reflect that we're changing position and size
                        // Remove flags that indicate no changes, since we ARE making changes
                        wp->flags &= ~(SWP_NOMOVE | SWP_NOSIZE);
                        
                        // Add flags to indicate our changes are intentional
                        wp->flags |= SWP_NOZORDER | SWP_NOACTIVATE;


                        LogDebug("Window style hook: WM_WINDOWPOSCHANGED new wp.(cx, cy, x, y, flags) = (" + std::to_string(wp->cx) + ", " + std::to_string(wp->cy) + ", " + std::to_string(wp->x) + ", " + std::to_string(wp->y) + ", " + std::to_string(wp->flags ) + ")");
                    }
                    break;
                }
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
