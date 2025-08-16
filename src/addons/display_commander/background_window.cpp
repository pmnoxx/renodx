#include "background_window.hpp"
#include "addon.hpp"
#include <sstream>

// Global instance is defined in globals.cpp
extern BackgroundWindowManager g_backgroundWindowManager;

BackgroundWindowManager::BackgroundWindowManager() 
    : m_background_hwnd(nullptr)
    , m_has_background_window(false)
    , m_frame_counter(0)
    , m_ignore_focus_events(true) { // Start with focus events ignored
}

BackgroundWindowManager::~BackgroundWindowManager() {
    DestroyBackgroundWindow();
}

bool BackgroundWindowManager::RegisterWindowClass() {
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = DefWindowProc; // Use default window procedure
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

bool BackgroundWindowManager::CreateBackgroundWindow(HWND game_hwnd) {
    if (m_background_hwnd != nullptr) {
        return true; // Already exists
    }
    
    // Try custom class first, fallback to existing Windows class
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
    
    // Get monitor info for the game window
    HMONITOR monitor = MonitorFromWindow(game_hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitor_info = { sizeof(MONITORINFO) };
    if (!GetMonitorInfo(monitor, &monitor_info)) {
        LogInfo("Failed to get monitor info for background window");
        return false;
    }
    
    // Create background window covering the entire monitor
    m_background_hwnd = CreateWindowExA(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW, // Extended styles
        window_class_to_use,               // Window class
        "RENODX BACKGROUND WINDOW",       // Window title
        WS_POPUP | WS_VISIBLE,            // Window style
        monitor_info.rcMonitor.left,      // X position
        monitor_info.rcMonitor.top,       // Y position
        monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,  // Width
        monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top, // Height
        nullptr,                          // Parent window
        nullptr,                          // Menu
        GetModuleHandle(nullptr),         // Instance
        nullptr                           // Additional data
    );
    
    if (m_background_hwnd == nullptr) {
        LogInfo("Failed to create background window");
        return false;
    }
    
    // Set window transparency
    SetLayeredWindowAttributes(m_background_hwnd, RGB(0, 0, 0), 255, LWA_COLORKEY);
    
    // Ensure the background window cannot receive focus or input
    SetWindowLongPtr(m_background_hwnd, GWL_EXSTYLE, 
                     GetWindowLongPtr(m_background_hwnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
    
    // Start the dedicated message processing thread
    StartBackgroundThread(game_hwnd);
    
    // Enable focus events after window is fully created
    m_ignore_focus_events.store(false);
    
    m_has_background_window.store(true);
    LogInfo("Background window created successfully");
    
    return true;
}

void BackgroundWindowManager::StartBackgroundThread(HWND game_hwnd) {
    // Stop existing thread if any
    if (m_background_thread.joinable()) {
        m_background_thread.join();
    }
    
    // Start dedicated message processing thread
    m_background_thread = std::thread([this, game_hwnd]() {
        LogInfo("[BG-WINDOW-THREAD] Background window message thread started");
        
        // Set a timer for color cycling
        SetTimer(m_background_hwnd, 1, 100, nullptr);
        
        // Message processing loop
        MSG msg;
        while (m_has_background_window && IsWindow(m_background_hwnd)) {
            // Get and process messages
            if (GetMessage(&msg, nullptr, 0, 0)) {
                // Handle specific messages
                switch (msg.message) {
                    case WM_PAINT: {
                        // Handle painting with color cycling
                        PAINTSTRUCT ps;
                        HDC hdc = BeginPaint(m_background_hwnd, &ps);
                        
                        if (hdc) {
                            // Get current color based on frame counter
                            int color_index = (m_frame_counter / 10) % COLOR_COUNT;
                            COLORREF current_color = COLORS[color_index];
                            
                            // Create brush and fill the window
                            HBRUSH brush = CreateSolidBrush(current_color);
                            FillRect(hdc, &ps.rcPaint, brush);
                            DeleteObject(brush);
                        }
                        
                        EndPaint(m_background_hwnd, &ps);
                        break;
                    }
                    
                    case WM_TIMER: {
                        // Handle timer for color cycling
                        if (msg.wParam == 1) { // Our timer
                            m_frame_counter.fetch_add(1);
                            
                            // Log every 100 frames
                            if (m_frame_counter % 100 == 0) {
                                int color_index = (m_frame_counter / 10) % COLOR_COUNT;
                                std::ostringstream oss;
                                oss << "[BG-WINDOW-THREAD] Background window: Frame " << m_frame_counter << ", Color RGB(" 
                                    << (color_index & 1 ? "true" : "false") << "," 
                                    << (color_index & 2 ? "true" : "false") << "," 
                                    << (color_index & 4 ? "true" : "false") << ")";
                                LogInfo(oss.str().c_str());
                            }
                            
                            // Trigger repaint
                            InvalidateRect(m_background_hwnd, nullptr, FALSE);
                            UpdateWindow(m_background_hwnd);
                        }
                        break;
                    }
                    
                    case WM_SETCURSOR: {
                        // Show "not allowed" cursor
                        SetCursor(LoadCursor(nullptr, IDC_NO));
                        break;
                    }
                    
                    case WM_LBUTTONDOWN:
                    case WM_RBUTTONDOWN:
                    case WM_MBUTTONDOWN: {
                        // Close window on any click
                        LogInfo("[BG-WINDOW-THREAD] Background window clicked - closing");
                        m_has_background_window.store(false);
                        break;
                    }
                    
                    case WM_SETFOCUS:
                    case WM_ACTIVATE: {
                        // Only close window when it gains focus if we're not ignoring focus events
                        if (!m_ignore_focus_events) {
                            LogInfo("[BG-WINDOW-THREAD] Background window gained focus - closing");
                            m_has_background_window.store(false);
                        } else {
                            LogInfo("[BG-WINDOW-THREAD] Background window focus event ignored during creation");
                        }
                        break;
                    }
                    
                    default:
                        // Let Windows handle other messages
                        DefWindowProc(m_background_hwnd, msg.message, msg.wParam, msg.lParam);
                        break;
                }
            } else {
                // WM_QUIT received, exit the loop
                break;
            }
        }
        
        // Cleanup
        KillTimer(m_background_hwnd, 1);
        
        LogInfo("[BG-WINDOW-THREAD] Background window message thread exiting");
    });
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
        // Create the background window
        CreateBackgroundWindow(game_hwnd);
        return;
    }
    
    // Update position of existing window
    UpdateBackgroundWindowPosition(game_hwnd);
}

void BackgroundWindowManager::DestroyBackgroundWindow() {
    if (m_background_hwnd != nullptr) {
        // Stop the background thread
        m_has_background_window.store(false);
        
        // Wait for thread to finish
        if (m_background_thread.joinable()) {
            m_background_thread.join();
        }
        
        // Destroy the window
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
