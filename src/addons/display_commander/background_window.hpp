#pragma once

#include <windows.h>
#include <atomic>

// Background window management
class BackgroundWindowManager {
public:
    BackgroundWindowManager();
    ~BackgroundWindowManager();
    
    // Create or update background window
    void UpdateBackgroundWindow(HWND game_hwnd);
    
    // Destroy background window
    void DestroyBackgroundWindow();
    
    // Check if background window exists
    bool HasBackgroundWindow() const;
    
    // Get background window handle
    HWND GetBackgroundWindow() const;

private:
    // Create the background window
    bool CreateBackgroundWindow(HWND game_hwnd);
    
    // Update background window position and size
    void UpdateBackgroundWindowPosition(HWND game_hwnd);
    
    // Background window handle
    HWND m_background_hwnd;
    
    // Flag to track if background window exists
    std::atomic<bool> m_has_background_window;
    
    // Window class name for background window
    static constexpr const char* BACKGROUND_WINDOW_CLASS = "RenodxBackgroundWindow";
    
    // Window procedure for background window
    static LRESULT CALLBACK BackgroundWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    
    // Register window class
    static bool RegisterWindowClass();
    
    // Unregister window class
    static void UnregisterWindowClass();
    
    // Static instance for window procedure
    static BackgroundWindowManager* s_instance;
};

// Global instance
extern BackgroundWindowManager g_backgroundWindowManager;
