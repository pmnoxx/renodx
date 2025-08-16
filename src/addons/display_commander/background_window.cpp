#include "background_window.hpp"
#include "addon.hpp"
#include "utils.hpp"
#include <dwmapi.h>
#include <windows.h>
#include <iostream>
#include <sstream>

// Include for logging
extern void LogInfo(const char* message);
extern void LogDebug(const char* message);

// Static instance for window procedure
BackgroundWindowManager* BackgroundWindowManager::s_instance = nullptr;

// Global instance declaration (defined in globals.cpp)
extern BackgroundWindowManager g_backgroundWindowManager;

BackgroundWindowManager::BackgroundWindowManager() 
    : m_background_hwnd(nullptr)
    , m_has_background_window(false) {
    s_instance = this;
    // Register the window class
    RegisterWindowClass();
}

BackgroundWindowManager::~BackgroundWindowManager() {
    DestroyBackgroundWindow();
    
    // Ensure message pump thread is joined
    if (m_message_pump_thread.joinable()) {
        m_message_pump_thread.join();
    }
    
    s_instance = nullptr;
    UnregisterWindowClass();
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
    if (result == 0) {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to register background window class - RegisterClassExA failed with error code: " << error;
        LogInfo(oss.str().c_str());
        return false;
    }
    
    LogInfo("Background window class registered successfully");
    return true;
}

void BackgroundWindowManager::UnregisterWindowClass() {
    UnregisterClassA(BACKGROUND_WINDOW_CLASS, GetModuleHandle(nullptr));
}

LRESULT CALLBACK BackgroundWindowManager::BackgroundWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Get the game window position and size for transparency
            HWND game_hwnd = nullptr;
            if (s_instance != nullptr) {
                // Try to get the last known game window
                extern std::atomic<HWND> g_last_swapchain_hwnd;
                game_hwnd = g_last_swapchain_hwnd.load();
            }
            
            // Fill with black, but make game area transparent
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            if (game_hwnd != nullptr && IsWindow(game_hwnd)) {
                // Get game window position relative to screen
                RECT game_rect;
                GetWindowRect(game_hwnd, &game_rect);
                
                // Get monitor info for the background window
                HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                MONITORINFO monitor_info;
                monitor_info.cbSize = sizeof(MONITORINFO);
                GetMonitorInfo(monitor, &monitor_info);
                
                // Calculate game area relative to monitor
                RECT game_monitor_rect;
                game_monitor_rect.left = game_rect.left - monitor_info.rcMonitor.left;
                game_monitor_rect.top = game_rect.top - monitor_info.rcMonitor.top;
                game_monitor_rect.right = game_rect.right - monitor_info.rcMonitor.left;
                game_monitor_rect.bottom = game_rect.bottom - monitor_info.rcMonitor.top;
                
                // Fill entire area with black first
                FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
                
                // Create a transparent region for the game area
                HRGN game_region = CreateRectRgn(
                    game_monitor_rect.left,
                    game_monitor_rect.top,
                    game_monitor_rect.right,
                    game_monitor_rect.bottom
                );
                
                // Create a region for the entire background window
                HRGN background_region = CreateRectRgn(0, 0, rect.right, rect.bottom);
                
                // Create a region that excludes the game area (background - game)
                HRGN final_region = CreateRectRgn(0, 0, 0, 0);
                CombineRgn(final_region, background_region, game_region, RGN_DIFF);
                
                // Fill only the non-game areas with black
                FillRgn(hdc, final_region, (HBRUSH)GetStockObject(BLACK_BRUSH));
                
                // Clean up regions
                DeleteObject(game_region);
                DeleteObject(background_region);
                DeleteObject(final_region);
            } else {
                // No game window info, just fill with black
                FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_ERASEBKGND:
            return TRUE; // Prevent flickering
            
        case WM_NCHITTEST:
            return HTTRANSPARENT; // Make window transparent to mouse input
            
        // Essential Windows messages that must be handled to prevent "unresponsive" status
        case WM_GETMINMAXINFO:
        case WM_WINDOWPOSCHANGING:
        case WM_WINDOWPOSCHANGED:
        case WM_SIZE:
        case WM_MOVE:
        case WM_STYLECHANGING:
        case WM_STYLECHANGED:
        case WM_SYSCOMMAND:
        case WM_NCACTIVATE:
        case WM_NCCALCSIZE:
        // Block input and focus events
        case WM_ACTIVATE:
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        case WM_MOUSEACTIVATE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEMOVE:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
            return 0; // Block all input and focus events
            
        // Additional messages to prevent unresponsiveness
        case WM_QUERYNEWPALETTE:
        case WM_PALETTECHANGED:
        case WM_DISPLAYCHANGE:
        case WM_THEMECHANGED:
        case WM_SETTINGCHANGE:
        // Keep window alive messages
        case WM_TIMER:
        case WM_USER:
        case WM_APP:
            return 0; // Handle but don't process
            
        default:
            return DefWindowProc(hwnd, msg, wparam, lparam);
    }
}

bool BackgroundWindowManager::CreateBackgroundWindow(HWND game_hwnd) {
    if (m_background_hwnd != nullptr) {
        return true; // Already exists
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
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, // Extended styles - added WS_EX_TOPMOST
        BACKGROUND_WINDOW_CLASS,           // Window class
        "Renodx Background",               // Window title
        WS_POPUP | WS_VISIBLE,             // Window style - added WS_VISIBLE
        monitor_info.rcMonitor.left,       // X position
        monitor_info.rcMonitor.top,        // Y position
        monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,  // Width
        monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,  // Height
        nullptr,                            // Parent window
        nullptr,                            // Menu
        GetModuleHandle(nullptr),          // Instance
        nullptr                             // Additional data
    );
    
    if (m_background_hwnd == nullptr) {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to create background window - CreateWindowExA failed with error code: " << error;
        LogInfo(oss.str().c_str());
        return false;
    }
    
    // Set window transparency (semi-transparent black)
    SetLayeredWindowAttributes(m_background_hwnd, 0, 255, LWA_ALPHA);
    
    // Ensure the background window cannot receive focus or input
    SetWindowLongPtr(m_background_hwnd, GWL_EXSTYLE, 
                     GetWindowLongPtr(m_background_hwnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
    
    // Show the background window
    ShowWindow(m_background_hwnd, SW_SHOW);
    
    // Set a timer to keep the window responsive
    SetTimer(m_background_hwnd, 1, 1000, nullptr); // 1 second timer
    
    // Ensure it's on top of everything
    SetWindowPos(m_background_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    
    // Start a message pump thread to keep the window responsive
    if (m_message_pump_thread.joinable()) {
        m_message_pump_thread.join();
    }
    
    m_message_pump_thread = std::thread([this]() {
        MSG msg;
        while (m_has_background_window.load()) {
            // Process Windows messages to keep the window responsive
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                // Only process messages for our background window or general system messages
                if (msg.hwnd == m_background_hwnd || msg.hwnd == nullptr) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            
            // Also process any pending messages for our specific window
            if (m_background_hwnd != nullptr) {
                while (PeekMessage(&msg, m_background_hwnd, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }
    });
    
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
    
    // Update background window to cover entire monitor, but ensure it stays behind the game window
    SetWindowPos(m_background_hwnd, 
                 game_hwnd, // Place behind the game window specifically
                 monitor_info.rcMonitor.left,
                 monitor_info.rcMonitor.top,
                 monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                 monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                 SWP_NOACTIVATE);
}

void BackgroundWindowManager::UpdateBackgroundWindow(HWND game_hwnd) {
    if (game_hwnd == nullptr || !IsWindow(game_hwnd)) {
        // No valid game window, destroy background window if it exists
        if (m_has_background_window.load()) {
            DestroyBackgroundWindow();
        }
        return;
    }
    
    // If background window doesn't exist, create it
    if (!m_has_background_window.load()) {
        if (CreateBackgroundWindow(game_hwnd)) {
            LogInfo("Background window created successfully");
        } else {
            LogInfo("Failed to create background window");
        }
    } else {
        // Update position of existing background window
        UpdateBackgroundWindowPosition(game_hwnd);
    }
}

void BackgroundWindowManager::DestroyBackgroundWindow() {
    if (m_background_hwnd != nullptr) {
        // Kill the timer
        KillTimer(m_background_hwnd, 1);
        
        // Ensure message pump thread is joined
        if (m_message_pump_thread.joinable()) {
            m_message_pump_thread.join();
        }
        
        // Destroy the window
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
