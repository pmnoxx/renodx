#include "background_window.hpp"
#include "addon.hpp"
#include "utils.hpp"
#include <dwmapi.h>
#include <sstream>

// Static instance for window procedure
BackgroundWindowManager* BackgroundWindowManager::s_instance = nullptr;

// Global instance declaration (defined in globals.cpp)
extern BackgroundWindowManager g_backgroundWindowManager;

BackgroundWindowManager::BackgroundWindowManager() 
    : m_background_hwnd(nullptr)
    , m_has_background_window(false)
    , m_frame_counter(0) {
    s_instance = this;
}

BackgroundWindowManager::~BackgroundWindowManager() {
    DestroyBackgroundWindow();
    
    // No more message pump thread to join
    
    s_instance = nullptr;
}

bool BackgroundWindowManager::RegisterWindowClass() {
    // HYBRID APPROACH: Try custom class first, fallback to existing Windows class
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = BackgroundWindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = BACKGROUND_WINDOW_CLASS;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    
    ATOM result = RegisterClassExA(&wc);
    if (result != 0) {
        LogInfo("Custom background window class registered successfully");
        return true;
    }
    
    // FALLBACK: Use existing Windows class if custom registration fails
    DWORD error = GetLastError();
    std::ostringstream oss;
    oss << "Custom window class registration failed with error: " << error << " - Using fallback approach";
    LogInfo(oss.str().c_str());
    
    // Don't return false - we'll use fallback in CreateBackgroundWindow
    return false;
}

void BackgroundWindowManager::UnregisterWindowClass() {
    UnregisterClassA(BACKGROUND_WINDOW_CLASS, GetModuleHandle(nullptr));
}

LRESULT CALLBACK BackgroundWindowManager::BackgroundWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // Check if instance is valid
    if (s_instance == nullptr) {
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    
    // Only handle WM_PAINT, let Windows handle everything else
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        
        if (hdc) {
            // Get current color based on frame counter
            int color_index = (s_instance->m_frame_counter / 10) % COLOR_COUNT;
            COLORREF current_color = COLORS[color_index];
            
            // Create brush and fill the window
            HBRUSH brush = CreateSolidBrush(current_color);
            FillRect(hdc, &ps.rcPaint, brush);
            DeleteObject(brush);
        }
        
        EndPaint(hwnd, &ps);
        return 0;  // Message handled
    }
    
    // Handle timer for color cycling
    if (msg == WM_TIMER) {
        if (s_instance) {
            // Increment frame counter for color cycling
            s_instance->m_frame_counter.fetch_add(1);
            
            // Trigger a repaint to show new color
            InvalidateRect(hwnd, nullptr, FALSE);
            UpdateWindow(hwnd);
        }
        return 0;  // Message handled
    }
    
    // Handle cursor to show "not allowed" when mouse is over background window
    if (msg == WM_SETCURSOR) {
        SetCursor(LoadCursor(nullptr, IDC_NO)); // Show "not allowed" cursor
        return TRUE; // We handled it
    }
    
    // Handle clicks to close background window
    if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN) {
        if (s_instance) {
            LogInfo("[BG-WINDOW-THREAD] Background window clicked - closing");
            s_instance->DestroyBackgroundWindow();
        }
        return 0; // Message handled
    }
    
    // Handle focus to close background window
    if (msg == WM_SETFOCUS || msg == WM_ACTIVATE) {
        if (s_instance) {
            LogInfo("[BG-WINDOW-THREAD] Background window gained focus - closing");
            s_instance->DestroyBackgroundWindow();
        }
        return 0; // Message handled
    }
    
    // Let Windows handle all other messages
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool BackgroundWindowManager::CreateBackgroundWindow(HWND game_hwnd) {
    if (m_background_hwnd != nullptr) {
        return true; // Already exists
    }
    
    // HYBRID APPROACH: Try custom class first, fallback to existing Windows class
    bool custom_class_registered = RegisterWindowClass();
    const char* window_class_to_use;
    
    if (custom_class_registered) {
        window_class_to_use = BACKGROUND_WINDOW_CLASS;
        LogInfo("Using custom background window class");
    } else {
        // FALLBACK: Use existing Windows class that we know works
        window_class_to_use = "Static"; // Windows built-in static control class
        LogInfo("Using fallback Windows class: Static");
    }
    
    // NUCLEAR FALLBACK: If Static class also fails, try the most basic Windows class
    if (window_class_to_use == "Static") {
        // Test if Static class works by trying to create a temporary window
        HWND test_hwnd = CreateWindowExA(0, "Static", "Test", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
        if (test_hwnd == nullptr) {
            window_class_to_use = "#32770"; // Windows dialog class - guaranteed to exist
            LogInfo("Static class failed, using nuclear fallback: Windows dialog class");
            if (test_hwnd != nullptr) {
                DestroyWindow(test_hwnd);
            }
        } else {
            DestroyWindow(test_hwnd);
            LogInfo("Static class test successful, proceeding with Static class");
        }
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
        WS_EX_LAYERED | WS_EX_TOOLWINDOW, // Extended styles - removed WS_EX_TOPMOST to prevent issues
        window_class_to_use,           // Window class
        "RENODX BACKGROUND WINDOW - DEBUG MODE", // Window title - MADE VISIBLE FOR DEBUGGING
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
        LogInfo("Failed to create background window");
        return false;
    }
    
    // Set window transparency (BRIGHT RED FOR DEBUGGING - IMPOSSIBLE TO MISS)
    SetLayeredWindowAttributes(m_background_hwnd, RGB(255, 0, 0), 255, LWA_COLORKEY | LWA_ALPHA);
    
    // Force the window to be visible and on top temporarily for debugging
    SetWindowPos(m_background_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    
    // Ensure the background window cannot receive focus or input
    SetWindowLongPtr(m_background_hwnd, GWL_EXSTYLE, 
                     GetWindowLongPtr(m_background_hwnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
    
    // NO MORE DRAWING THREAD - Only draw in BackgroundWindowProc to prevent crashes
    // Set a timer to trigger repaints for color cycling
    SetTimer(m_background_hwnd, 1, 100, nullptr); // 100ms timer for color cycling
    
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
    
    if (!m_has_background_window) {
        // Create the background window directly (working approach)
        CreateBackgroundWindow(game_hwnd);
        return;
    }
    
    // Update position of existing window
    UpdateBackgroundWindowPosition(game_hwnd);
    
    // No more message thread management needed
}

void BackgroundWindowManager::DestroyBackgroundWindow() {
    if (m_background_hwnd != nullptr) {
        // Stop the background window
        m_has_background_window.store(false);
        
        // Kill the timer
        KillTimer(m_background_hwnd, 1);
        
        // No more message pump thread to wait for
        
        DestroyWindow(m_background_hwnd);
        m_background_hwnd = nullptr;
        LogInfo("Background window destroyed");
    }
}

bool BackgroundWindowManager::HasBackgroundWindow() const {
    return m_has_background_window.load();
}

HWND BackgroundWindowManager::GetBackgroundWindow() const {
    return m_background_hwnd;
}
