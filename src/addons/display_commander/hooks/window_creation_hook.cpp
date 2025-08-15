#include "window_creation_hook.hpp"
#include "../addon.hpp"
#include "../utils.hpp"
#include <sstream>
#include <thread>

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

// Hook for CreateWindowExW to modify window styles during creation (now uses delayed approach)
LRESULT CALLBACK CreateWindowHookProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Call the original window procedure first
    LRESULT result = CallWindowProc(g_original_create_window_ex_wnd_proc, hwnd, uMsg, wParam, lParam);
    
    // If this is a window creation message and we want borderless mode
    if (uMsg == WM_CREATE || uMsg == WM_NCCREATE) {
        extern float s_remove_top_bar;
        if (s_remove_top_bar >= 0.5f) {
            // Schedule delayed style change instead of immediate application
            std::thread([hwnd]() {
                // Small delay to prevent immediate execution within the hook
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                
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
                
                LogDebug("Reflex: Applied borderless styles during window creation (delayed)");
            }).detach();
        }
    }
    
    return result;
}

// Install window creation hooks (now uses delayed approach for extra safety)
bool InstallWindowCreationHooks() {
    if (g_window_creation_hooks_installed) {
        LogDebug("Window creation hooks already installed");
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
            LogDebug("No window handle available for window creation hooks");
            return;
        }
        
        // Store the original window procedure
        g_original_create_window_ex_wnd_proc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
        if (g_original_create_window_ex_wnd_proc == nullptr) {
            LogDebug("Failed to get original window procedure for window creation hooks");
            return;
        }
        
        // Subclass the window
        if (SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)CreateWindowHookProc) == 0) {
            DWORD error = GetLastError();
            std::ostringstream oss;
            oss << "Failed to subclass window for window creation hooks. Error: " << error;
            LogDebug(oss.str().c_str());
            return;
        }
        
        g_create_window_hook_hwnd = hwnd;
        g_window_creation_hooks_installed = true;
        LogInfo("Window creation hooks installed successfully (delayed)");
    }).detach();
    
    return true; // Return true immediately, actual installation happens in background
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
