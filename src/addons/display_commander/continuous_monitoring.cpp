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

extern float s_remove_top_bar;
extern float s_prevent_always_on_top;

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
            // PROACTIVE ALWAYS ON TOP CHECK: Force style enforcement if always on top is detected
            if (s_prevent_always_on_top >= 0.5f) {
                LONG_PTR current_ex_style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
                bool needs_fix = false;
                
                // Check if window has always on top styles
                if (current_ex_style & (WS_EX_TOPMOST | WS_EX_TOOLWINDOW)) {
                    needs_fix = true;
                }
                
                // Also check if window is actually in the foreground Z-order (HWND_TOPMOST)
                HWND foreground_window = GetForegroundWindow();
                if (foreground_window == hwnd) {
                    // Check if this window is actually the topmost window
                    HWND top_window = GetWindow(hwnd, GW_HWNDNEXT);
                    if (top_window == nullptr || top_window == GetDesktopWindow()) {
                        needs_fix = true;
                    }
                }
                
                if (needs_fix) {
                    std::ostringstream oss;
                    if (current_ex_style & (WS_EX_TOPMOST | WS_EX_TOOLWINDOW)) {
                        oss << "Continuous monitoring: ALWAYS ON TOP DETECTED - Forcing immediate style enforcement to remove extended styles 0x" << std::hex << (current_ex_style & (WS_EX_TOPMOST | WS_EX_TOOLWINDOW));
                    } else {
                        oss << "Continuous monitoring: FOREGROUND Z-ORDER DETECTED - Forcing window to normal Z-order";
                    }
                    LogInfo(oss.str().c_str());
                    
                    // AGGRESSIVE APPROACH: Use SetWindowPos with HWND_NOTOPMOST to force window out of always on top
                    // This is more effective than just changing styles
                    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                    
                    // Also remove the extended styles as backup
                    LONG_PTR new_ex_style = current_ex_style & ~(WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
                    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, new_ex_style);
                    
                    // Force window update
                    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                    
                    LogInfo("Continuous monitoring: Always on top styles removed and window forced to normal Z-order");
                    continue; // Skip normal ApplyWindowChange since we just fixed the issue
                }
            }
            
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
