#include "background_window.hpp"
#include "addon.hpp"
#include "utils.hpp"
#include <dwmapi.h>

// Static instance for window procedure
BackgroundWindowManager* BackgroundWindowManager::s_instance = nullptr;

// Global instance declaration (defined in globals.cpp)
extern BackgroundWindowManager g_backgroundWindowManager;

BackgroundWindowManager::BackgroundWindowManager() 
    : m_background_hwnd(nullptr)
    , m_has_background_window(false) {
    s_instance = this;
}

BackgroundWindowManager::~BackgroundWindowManager() {
    DestroyBackgroundWindow();
    s_instance = nullptr;
}

bool BackgroundWindowManager::RegisterWindowClass() {
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = BackgroundWindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = BACKGROUND_WINDOW_CLASS;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    
    ATOM result = RegisterClassExA(&wc);
    return result != 0;
}

void BackgroundWindowManager::UnregisterWindowClass() {
    UnregisterClassA(BACKGROUND_WINDOW_CLASS, GetModuleHandle(nullptr));
}

LRESULT CALLBACK BackgroundWindowManager::BackgroundWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Fill with black
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_ERASEBKGND:
            return TRUE; // Prevent flickering
            
        case WM_NCHITTEST:
            return HTTRANSPARENT; // Make window transparent to mouse input
            
        default:
            return DefWindowProc(hwnd, msg, wparam, lparam);
    }
}

bool BackgroundWindowManager::CreateBackgroundWindow(HWND game_hwnd) {
    if (m_background_hwnd != nullptr) {
        return true; // Already exists
    }
    
    // Register window class if not already registered
    if (!RegisterWindowClass()) {
        LogInfo("Failed to register background window class");
        return false;
    }
    
    // Get game window position and size
    RECT game_rect;
    if (!GetWindowRect(game_hwnd, &game_rect)) {
        LogInfo("Failed to get game window rect for background window");
        return false;
    }
    
    // Get monitor info for the game window
    HMONITOR monitor = MonitorFromWindow(game_hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitor_info = { sizeof(MONITORINFO) };
    if (!GetMonitorInfo(monitor, &monitor_info)) {
        LogInfo("Failed to get monitor info for background window");
        return false;
    }
    
    // Create background window covering the entire monitor
    m_background_hwnd = CreateWindowExA(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, // Extended styles
        BACKGROUND_WINDOW_CLASS,                           // Window class
        "Renodx Background",                               // Window title
        WS_POPUP,                                          // Window style
        monitor_info.rcMonitor.left,                       // X position
        monitor_info.rcMonitor.top,                        // Y position
        monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,  // Width
        monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,  // Height
        nullptr,                                            // Parent window
        nullptr,                                            // Menu
        GetModuleHandle(nullptr),                          // Instance
        nullptr                                             // Additional data
    );
    
    if (m_background_hwnd == nullptr) {
        LogInfo("Failed to create background window");
        return false;
    }
    
    // Set window transparency (semi-transparent black)
    SetLayeredWindowAttributes(m_background_hwnd, 0, 255, LWA_ALPHA);
    
    // Show the background window
    ShowWindow(m_background_hwnd, SW_SHOW);
    
    // Ensure it's behind the game window
    SetWindowPos(m_background_hwnd, game_hwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    
    m_has_background_window.store(true);
    LogInfo("Background window created successfully");
    
    return true;
}

void BackgroundWindowManager::UpdateBackgroundWindowPosition(HWND game_hwnd) {
    if (m_background_hwnd == nullptr) {
        return;
    }
    
    // Get monitor info for the game window
    HMONITOR monitor = MonitorFromWindow(game_hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitor_info = { sizeof(MONITORINFO) };
    if (!GetMonitorInfo(monitor, &monitor_info)) {
        return;
    }
    
    // Update background window to cover entire monitor
    SetWindowPos(m_background_hwnd, 
                 HWND_BOTTOM, // Place at bottom of Z-order
                 monitor_info.rcMonitor.left,
                 monitor_info.rcMonitor.top,
                 monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                 monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                 SWP_NOACTIVATE);
}

void BackgroundWindowManager::UpdateBackgroundWindow(HWND game_hwnd) {
    if (game_hwnd == nullptr) {
        return;
    }
    
    // Check if background feature is enabled
    extern float s_background_feature_enabled;
    if (s_background_feature_enabled < 0.5f) {
        // Feature disabled, destroy background window if it exists
        if (m_has_background_window.load()) {
            DestroyBackgroundWindow();
        }
        return;
    }
    
    // Feature enabled, create or update background window
    if (!m_has_background_window.load()) {
        CreateBackgroundWindow(game_hwnd);
    } else {
        UpdateBackgroundWindowPosition(game_hwnd);
    }
}

void BackgroundWindowManager::DestroyBackgroundWindow() {
    if (m_background_hwnd != nullptr) {
        DestroyWindow(m_background_hwnd);
        m_background_hwnd = nullptr;
        m_has_background_window.store(false);
        LogInfo("Background window destroyed");
    }
}

bool BackgroundWindowManager::HasBackgroundWindow() const {
    return m_has_background_window.load();
}

HWND BackgroundWindowManager::GetBackgroundWindow() const {
    return m_background_hwnd;
}
