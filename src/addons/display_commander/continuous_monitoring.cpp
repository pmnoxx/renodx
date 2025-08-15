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

// Function to check if a window needs adjustment
bool NeedsWindowAdjustment(HWND hwnd, int& out_width, int& out_height, int& out_pos_x, int& out_pos_y, WindowStyleMode& out_style_mode) {
    if (hwnd == nullptr) return false;
    
    // Get current window state
    RECT window_rect;
    if (!GetWindowRect(hwnd, &window_rect)) return false;
    
    RECT client_rect;
    if (!GetClientRect(hwnd, &client_rect)) return false;
    
    // Get current window styles
    LONG_PTR current_style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR current_ex_style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    
    // Compute desired size and position
    int desired_width = 0, desired_height = 0;
    ComputeDesiredSize(desired_width, desired_height);
    
    int desired_pos_x = static_cast<int>(s_windowed_pos_x);
    int desired_pos_y = static_cast<int>(s_windowed_pos_y);
    
    // Determine desired style mode
    WindowStyleMode desired_style_mode = (s_remove_top_bar >= 0.5f) ? WindowStyleMode::BORDERLESS : WindowStyleMode::OVERLAPPED_WINDOW;
    
    // Check if window is maximized or minimized
    if (IsZoomed(hwnd) || IsIconic(hwnd)) {
        return false; // Don't interfere with maximized/minimized windows
    }
    
    // Check if size needs adjustment
    bool needs_resize = false;
    if (desired_width > 0 && desired_height > 0) {
        int current_client_width = client_rect.right - client_rect.left;
        int current_client_height = client_rect.bottom - client_rect.top;
        
        // Allow small tolerance (5 pixels)
        if (abs(current_client_width - desired_width) > 5 || abs(current_client_height - desired_height) > 5) {
            needs_resize = true;
        }
    }
    
    // Check if position needs adjustment
    bool needs_move = false;
    if (desired_pos_x != 0 || desired_pos_y != 0) {
        int current_x = window_rect.left;
        int current_y = window_rect.top;
        
        // Allow small tolerance (5 pixels)
        if (abs(current_x - desired_pos_x) > 5 || abs(current_y - desired_pos_y) > 5) {
            needs_move = true;
        }
    }
    
    // Check if style needs adjustment
    bool needs_style_change = false;
    if (desired_style_mode == WindowStyleMode::BORDERLESS) {
        // Check if window has borderless styles
        bool has_borderless_style = !(current_style & WS_CAPTION) && !(current_style & WS_THICKFRAME);
        if (!has_borderless_style) {
            needs_style_change = true;
        }
    } else if (desired_style_mode == WindowStyleMode::OVERLAPPED_WINDOW) {
        // Check if window has overlapped window styles
        bool has_overlapped_style = (current_style & WS_CAPTION) && (current_style & WS_THICKFRAME);
        if (!has_overlapped_style) {
            needs_style_change = true;
        }
    }
    
    // Set output parameters if adjustment is needed
    if (needs_resize || needs_move || needs_style_change) {
        out_width = desired_width;
        out_height = desired_height;
        out_pos_x = desired_pos_x;
        out_pos_y = desired_pos_y;
        out_style_mode = desired_style_mode;
        return true;
    }
    
    return false;
}

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
            // Check if window needs adjustment
            int desired_width, desired_height, desired_pos_x, desired_pos_y;
            WindowStyleMode desired_style_mode;
            
            if (NeedsWindowAdjustment(hwnd, desired_width, desired_height, desired_pos_x, desired_pos_y, desired_style_mode)) {
                // Apply the necessary changes
                ApplyWindowChange(hwnd, true, desired_width, desired_height, true, desired_pos_x, desired_pos_y, 
                                desired_style_mode, "continuous_monitoring_auto_fix");
                
                LogDebug("Continuous monitoring: Applied window adjustments");
            }
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
