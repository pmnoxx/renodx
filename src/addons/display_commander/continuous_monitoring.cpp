#include "addon.hpp"
#include "utils.hpp"
#include "background_window.hpp"
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
extern float s_block_input_in_background; // Added this line
extern float s_background_feature_enabled; // Added this line

// ReShade runtime for input blocking
extern std::atomic<reshade::api::effect_runtime*> g_reshade_runtime;

// Background window manager
extern class BackgroundWindowManager g_backgroundWindowManager;

// Input blocking state
static std::atomic<bool> g_input_blocking_active = false;
static std::atomic<bool> g_app_in_background = false;

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
            // BACKGROUND DETECTION: Check if the app is in background
            bool current_background = (GetForegroundWindow() != hwnd);
            bool background_changed = (current_background != g_app_in_background.load());
            
            if (background_changed) {
                g_app_in_background.store(current_background);
                
                if (current_background) {
                    LogInfo("Continuous monitoring: App moved to BACKGROUND - Starting ReShade input blocking");
                    g_input_blocking_active.store(true);
                    
                    // Immediately log the current state of input blocking settings
                    if (s_block_input_in_background >= 0.5f) {
                        LogInfo("Continuous monitoring: Input blocking setting is ENABLED - will block input");
                    } else {
                        LogInfo("Continuous monitoring: Input blocking setting is DISABLED - will NOT block input");
                    }
                    
                    // Check if ReShade runtime is available
                    auto runtime = g_reshade_runtime.load();
                    if (runtime != nullptr) {
                        LogInfo("Continuous monitoring: ReShade runtime is AVAILABLE for input blocking");
                    } else {
                        LogInfo("Continuous monitoring: ReShade runtime is NOT AVAILABLE for input blocking");
                    }
                } else {
                    LogInfo("Continuous monitoring: App moved to FOREGROUND - Stopping ReShade input blocking");
                    g_input_blocking_active.store(false);
                }
            }
            
            // RESHADE INPUT BLOCKING: Use ReShade's input blocking when app is in background
            if (g_input_blocking_active.load() && s_block_input_in_background >= 0.5f) {
                auto runtime = g_reshade_runtime.load();
                if (runtime != nullptr) {
                    // Block input to prevent game from re-capturing mouse
                    runtime->block_input_next_frame();
                    
                    // Also force release any existing mouse confinement
                    ReleaseCapture();
                    ClipCursor(nullptr);
                    
                    // Log only occasionally to avoid spam
                    static int log_counter = 0;
                    if (++log_counter % 60 == 0) { // Log every 60 seconds
                        LogDebug("Continuous monitoring: ReShade input blocking active - preventing mouse re-capture");
                    }
                } else {
                    LogDebug("Continuous monitoring: ReShade runtime not available for input blocking");
                }
            } else {
                // Log why input blocking is not active
                static int debug_counter = 0;
                if (++debug_counter % 30 == 0) { // Log every 30 seconds
                    if (!g_input_blocking_active.load()) {
                        LogDebug("Continuous monitoring: Input blocking inactive - app is in foreground");
                    } else if (s_block_input_in_background < 0.5f) {
                        LogDebug("Continuous monitoring: Input blocking disabled by user setting");
                    }
                }
            }
            
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
                
                // PROACTIVE MOUSE CONFINEMENT RELEASE: Prevent DirectInput from keeping mouse confined
                // Check if mouse is currently captured or cursor is clipped
                HWND captured_window = GetCapture();
                if (captured_window == hwnd) {
                    LogInfo("Continuous monitoring: MOUSE CAPTURE DETECTED - Releasing to prevent confinement");
                    ReleaseCapture();
                }
                
                // Also check if cursor is clipped to this window
                RECT clip_rect;
                if (GetClipCursor(&clip_rect)) {
                    RECT window_rect;
                    GetWindowRect(hwnd, &window_rect);
                    // If cursor is clipped to window bounds, release it
                    if (memcmp(&clip_rect, &window_rect, sizeof(RECT)) == 0) {
                        LogInfo("Continuous monitoring: CURSOR CLIPPING DETECTED - Releasing to prevent confinement");
                        ClipCursor(nullptr);
                    }
                }
            }
            
            // Apply window changes - the function will automatically determine what needs to be changed
            ApplyWindowChange(hwnd, "continuous_monitoring_auto_fix");
            
            // BACKGROUND WINDOW MANAGEMENT: Update background window if feature is enabled
            if (s_background_feature_enabled >= 0.5f) {
                g_backgroundWindowManager.UpdateBackgroundWindow(hwnd);
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
