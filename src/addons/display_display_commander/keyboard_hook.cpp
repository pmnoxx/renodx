// Timer callback for enforcing borderless styles
VOID CALLBACK BorderlessStyleTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    // Try to apply borderless styles aggressively to the main window
    ApplyBorderlessStylesAggressively();
    
    // Also check all top-level windows periodically
    static int all_windows_counter = 0;
    if (++all_windows_counter % 50 == 0) { // Every 5 seconds
        ApplyBorderlessStylesToAllWindows();
    }
    
    // Also try to install hooks if they're not working
    static int hook_retry_counter = 0;
    if (++hook_retry_counter % 100 == 0) { // Every 10 seconds
        extern std::atomic<HWND> g_last_swapchain_hwnd;
        HWND current_hwnd = g_last_swapchain_hwnd.load();
        if (current_hwnd != nullptr && !g_window_creation_hooks_installed) {
            LogDebug("Reflex: Retrying to install window creation hooks");
            InstallWindowCreationHooks();
        }
    }
    
    // Slow down the timer after the initial aggressive phase
    static int timer_phase_counter = 0;
    if (++timer_phase_counter == 200) { // After 10 seconds (200 * 50ms)
        // Kill the current timer and restart with a slower interval
        KillTimer(nullptr, g_borderless_timer_id);
        g_borderless_timer_id = SetTimer(nullptr, 0, 200, BorderlessStyleTimerProc); // 200ms interval
        if (g_borderless_timer_id != 0) {
            LogDebug("Reflex: Slowed down borderless style enforcement timer to 200ms interval");
        }
    }
}
