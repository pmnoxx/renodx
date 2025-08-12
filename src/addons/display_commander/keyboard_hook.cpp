#include "keyboard_hook.hpp"
#include "addon.hpp"
#include <include/reshade.hpp>

// Global variables for hooks
static HHOOK g_alt_tab_hook = nullptr;
static HHOOK g_minimize_hook = nullptr;

// Hook procedure for Alt-Tab suppression
LRESULT CALLBACK AltTabHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT* kbStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            
            // Check if Alt+Tab is pressed (Alt = VK_MENU, Tab = VK_TAB)
            if (kbStruct->vkCode == VK_TAB && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
                // Suppress Alt+Tab by returning 1 (indicating we handled the message)
                LogDebug("Alt+Tab suppressed by hook");
                return 1;
            }
        }
    }
    
    // Call the next hook in the chain
    return CallNextHookEx(g_alt_tab_hook, nCode, wParam, lParam);
}

// Install the Alt-Tab suppression hook
bool InstallAltTabHook() {
    if (g_alt_tab_hook != nullptr) {
        LogWarn("Alt-Tab hook already installed");
        return true;
    }
    
    // Install a low-level keyboard hook
    g_alt_tab_hook = SetWindowsHookEx(WH_KEYBOARD_LL, AltTabHookProc, nullptr, 0);
    
    if (g_alt_tab_hook == nullptr) {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to install Alt-Tab hook. Error: " << error;
        LogWarn(oss.str().c_str());
        return false;
    }
    
    LogInfo("Alt-Tab suppression hook installed successfully");
    return true;
}

// Uninstall the Alt-Tab suppression hook
void UninstallAltTabHook() {
    if (g_alt_tab_hook == nullptr) {
        LogDebug("Alt-Tab hook not installed");
        return;
    }
    
    if (UnhookWindowsHookEx(g_alt_tab_hook)) {
        g_alt_tab_hook = nullptr;
        LogInfo("Alt-Tab suppression hook uninstalled successfully");
    } else {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to uninstall Alt-Tab hook. Error: " << error;
        LogWarn(oss.str().c_str());
    }
}

// Hook procedure for Windows minimize prevention
LRESULT CALLBACK MinimizeHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_SYSCOMMAND) {
            // Check if the minimize command is being sent
            if ((lParam & 0xFFF0) == SC_MINIMIZE) {
                // Suppress minimize by returning 0 (indicating we handled the message)
                LogDebug("Window minimize suppressed by hook");
                return 0;
            }
        }
    }
    
    // Call the next hook in the chain
    return CallNextHookEx(g_minimize_hook, nCode, wParam, lParam);
}

// Install the Windows minimize prevention hook
bool InstallMinimizeHook() {
    if (g_minimize_hook != nullptr) {
        LogWarn("Minimize hook already installed");
        return true;
    }
    
    // Install a system-wide hook for window messages
    g_minimize_hook = SetWindowsHookEx(WH_GETMESSAGE, MinimizeHookProc, nullptr, 0);
    
    if (g_minimize_hook == nullptr) {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to install minimize hook. Error: " << error;
        LogWarn(oss.str().c_str());
        return false;
    }
    
    LogInfo("Windows minimize prevention hook installed successfully");
    return true;
}

// Uninstall the Windows minimize prevention hook
void UninstallMinimizeHook() {
    if (g_minimize_hook == nullptr) {
        LogDebug("Minimize hook not installed");
        return;
    }
    
    if (UnhookWindowsHookEx(g_minimize_hook)) {
        g_minimize_hook = nullptr;
        LogInfo("Windows minimize prevention hook uninstalled successfully");
    } else {
        DWORD error = GetLastError();
        std::ostringstream oss;
        oss << "Failed to uninstall minimize hook. Error: " << error;
        LogWarn(oss.str().c_str());
    }
}
