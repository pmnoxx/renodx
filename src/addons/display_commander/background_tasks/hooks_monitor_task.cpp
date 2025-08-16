#include "hooks_monitor_task.hpp"
#include "../hooks/hooks_manager.hpp"
#include "../addon.hpp"
#include <sstream>

namespace renodx::background {

// Hooks monitoring background task
void RunHooksMonitorTask() {
    static float last_remove_top_bar = -1.0f;
    static float last_suppress_move_resize_messages = -1.0f;

    // Check if the remove top bar setting has changed
    if (s_remove_top_bar != last_remove_top_bar) {
        std::ostringstream oss;
        oss << "Hooks monitor: Remove top bar setting changed from " << (last_remove_top_bar >= 0.5f ? "enabled" : "disabled");
        oss << " to " << (s_remove_top_bar >= 0.5f ? "enabled" : "disabled");
        LogInfo(oss.str().c_str());

        // Update hooks based on new setting
        renodx::hooks::UpdateHooks();

        // Update our tracking variable
        last_remove_top_bar = s_remove_top_bar;
    }

    // Check if the suppress move/resize messages setting has changed
    if (s_suppress_move_resize_messages != last_suppress_move_resize_messages) {
        std::ostringstream oss;
        oss << "Hooks monitor: Suppress move/resize messages setting changed from " << (last_suppress_move_resize_messages >= 0.5f ? "enabled" : "disabled");
        oss << " to " << (s_suppress_move_resize_messages >= 0.5f ? "enabled" : "disabled");
        LogInfo(oss.str().c_str());

        // Update hooks based on new setting
        renodx::hooks::UpdateHooks();

        // Update our tracking variable
        last_suppress_move_resize_messages = s_suppress_move_resize_messages;
    }

    // Check if we need to reinstall hooks for a new window
    static HWND last_monitored_hwnd = nullptr;
    HWND current_hwnd = g_last_swapchain_hwnd.load();

    if (current_hwnd != nullptr && current_hwnd != last_monitored_hwnd) {
        // New window detected, check if we need to install hooks
        if ((s_remove_top_bar >= 0.5f || s_suppress_move_resize_messages >= 0.5f) && !renodx::hooks::AreAnyHooksInstalled()) {
            std::ostringstream oss;
            oss << "Hooks monitor: New window detected " << current_hwnd << ", installing hooks";
            LogInfo(oss.str().c_str());

            renodx::hooks::InstallAllHooks();
        }

        last_monitored_hwnd = current_hwnd;
    }
}

} // namespace renodx::background
