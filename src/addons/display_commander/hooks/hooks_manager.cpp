#include "hooks_manager.hpp"
#include "window_style_hooks.hpp"
#include "../addon.hpp"
#include <sstream>

namespace renodx::hooks {

// Global state for hooks management
bool g_any_hooks_installed = false;

void InstallAllHooks() {
    if (g_any_hooks_installed) {
        LogWarn("Hooks already installed, skipping installation");
        return;
    }
    
    LogInfo("Installing all hooks...");
    
    // Install window style hooks if remove top bar is enabled
    if (s_remove_top_bar >= 0.5f) {
        InstallWindowStyleHooks();
    }
    
    g_any_hooks_installed = true;
    LogInfo("All hooks installed successfully");
}

void UninstallAllHooks() {
    if (!g_any_hooks_installed) {
        LogWarn("No hooks installed, skipping uninstallation");
        return;
    }
    
    LogInfo("Uninstalling all hooks...");
    
    // Uninstall window style hooks
    UninstallWindowStyleHooks();
    
    g_any_hooks_installed = false;
    LogInfo("All hooks uninstalled successfully");
}

void UpdateHooks() {
    // Check if we need to install or uninstall hooks based on current settings
    bool should_have_style_hooks = (s_remove_top_bar >= 0.5f);
    bool have_style_hooks = AreWindowStyleHooksInstalled();
    
    if (should_have_style_hooks && !have_style_hooks) {
        LogInfo("Settings changed: Installing window style hooks");
        InstallWindowStyleHooks();
    } else if (!should_have_style_hooks && have_style_hooks) {
        LogInfo("Settings changed: Uninstalling window style hooks");
        UninstallWindowStyleHooks();
    }
    
    // Update global state
    g_any_hooks_installed = AreWindowStyleHooksInstalled();
}

bool AreAnyHooksInstalled() {
    return g_any_hooks_installed;
}

} // namespace renodx::hooks
