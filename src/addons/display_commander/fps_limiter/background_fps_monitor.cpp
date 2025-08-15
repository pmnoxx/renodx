#include "background_fps_monitor.hpp"
#include "../addon.hpp"
#include <chrono>
#include <sstream>

// Global state
std::atomic<bool> g_background_fps_monitor_running{false};
std::thread g_background_fps_monitor_thread;

// External declarations from globals.cpp
extern std::atomic<HWND> g_last_swapchain_hwnd;
extern float s_fps_limit;
extern float s_fps_limit_background;

// Background FPS monitoring thread function
void BackgroundFpsMonitorThread() {
    LogInfo("Background FPS monitor thread started");
    
    // Store the last known state to avoid unnecessary updates
    bool last_was_foreground = true;
    float last_foreground_fps = 0.0f;
    
    while (g_background_fps_monitor_running.load()) {
        // Get the current swapchain window
        HWND hwnd = g_last_swapchain_hwnd.load();
        
        if (hwnd != nullptr && IsWindow(hwnd)) {
            // Check if the window is in foreground
            HWND foreground_hwnd = GetForegroundWindow();
            bool is_foreground = (hwnd == foreground_hwnd);
            
            // Only update if the state has changed
            if (is_foreground != last_was_foreground) {
                if (is_foreground) {
                    // Window came to foreground - restore the user's FPS limit
                    if (s_fps_limit > 0.0f) {
                        renodx::utils::swapchain::fps_limit = s_fps_limit;
                        std::string msg = "Background FPS monitor: Window in foreground, FPS limit restored to " + std::to_string(s_fps_limit);
                        LogInfo(msg.c_str());
                    } else {
                        // User has no FPS limit set, disable it
                        renodx::utils::swapchain::fps_limit = 0.0f;
                        LogInfo("Background FPS monitor: Window in foreground, FPS limit disabled");
                    }
                } else {
                    // Window went to background - apply background FPS limit
                    if (s_fps_limit_background > 0.0f) {
                        // Store the current foreground FPS limit if we haven't already
                        if (last_was_foreground) {
                            last_foreground_fps = renodx::utils::swapchain::fps_limit;
                        }
                        renodx::utils::swapchain::fps_limit = s_fps_limit_background;
                        std::string msg = "Background FPS monitor: Window in background, FPS limit set to " + std::to_string(s_fps_limit_background);
                        LogInfo(msg.c_str());
                    } else {
                        // Background FPS limit is disabled, keep current setting
                        LogInfo("Background FPS monitor: Window in background, FPS limit unchanged (background limit disabled)");
                    }
                }
                
                last_was_foreground = is_foreground;
            }
        }
        
        // Sleep for 100ms to avoid excessive CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    LogInfo("Background FPS monitor thread stopped");
}

void StartBackgroundFpsMonitor() {
    if (g_background_fps_monitor_running.load()) {
        LogWarn("Background FPS monitor is already running");
        return;
    }
    
    g_background_fps_monitor_running.store(true);
    g_background_fps_monitor_thread = std::thread(BackgroundFpsMonitorThread);
    
    LogInfo("Background FPS monitor started");
}

void StopBackgroundFpsMonitor() {
    if (!g_background_fps_monitor_running.load()) {
        LogWarn("Background FPS monitor is not running");
        return;
    }
    
    g_background_fps_monitor_running.store(false);
    
    if (g_background_fps_monitor_thread.joinable()) {
        g_background_fps_monitor_thread.join();
    }
    
    LogInfo("Background FPS monitor stopped");
}
