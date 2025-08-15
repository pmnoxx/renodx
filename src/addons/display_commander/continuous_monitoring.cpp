#include "addon.hpp"
#include "utils.hpp"
#include <thread>
#include <chrono>
#include <sstream>

// Forward declarations
void ComputeDesiredSize(int& out_w, int& out_h);

// Global variables for monitoring
extern float s_continuous_monitoring_enabled;
extern std::atomic<bool> g_monitoring_thread_running;
extern std::thread g_monitoring_thread;
extern std::atomic<HWND> g_last_swapchain_hwnd;

// Additional global variables needed for monitoring
extern float s_windowed_pos_x;
extern float s_windowed_pos_y;
extern float s_remove_top_bar;

// Main monitoring thread function
void ContinuousMonitoringThread() {
    LogInfo("Continuous monitoring thread started");
    
    while (g_monitoring_thread_running.load()) {
        // Check if monitoring is still enabled
        if (s_continuous_monitoring_enabled < 0.5f) {
            break;
        }
        
        // Get the current swapchain window
        HWND hwnd = g_last_swapchain_hwnd.load();
        if (hwnd != nullptr && IsWindow(hwnd)) {
            // Apply window changes - the function will automatically determine what needs to be changed
            ApplyWindowChange(hwnd, "continuous_monitoring_auto_fix");
        }
        
        // Sleep for 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    LogInfo("Continuous monitoring thread stopped");
}

// Start continuous monitoring
void StartContinuousMonitoring() {
    if (g_monitoring_thread_running.load()) {
        LogDebug("Continuous monitoring already running");
        return;
    }
    
    if (s_continuous_monitoring_enabled < 0.5f) {
        LogDebug("Continuous monitoring disabled, not starting");
        return;
    }
    
    g_monitoring_thread_running.store(true);
    
    // Start the monitoring thread
    if (g_monitoring_thread.joinable()) {
        g_monitoring_thread.join();
    }
    
    g_monitoring_thread = std::thread(ContinuousMonitoringThread);
    
    LogInfo("Continuous monitoring started");
}

// Stop continuous monitoring
void StopContinuousMonitoring() {
    if (!g_monitoring_thread_running.load()) {
        LogDebug("Continuous monitoring not running");
        return;
    }
    
    g_monitoring_thread_running.store(false);
    
    // Wait for thread to finish
    if (g_monitoring_thread.joinable()) {
        g_monitoring_thread.join();
    }
    
    LogInfo("Continuous monitoring stopped");
}
