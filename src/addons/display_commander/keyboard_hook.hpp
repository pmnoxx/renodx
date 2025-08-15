#pragma once

#include <windows.h>



// Windows minimize prevention functions (using window subclassing)
bool InstallMinimizeHook();
void UninstallMinimizeHook();

// Window resize enforcement (using window subclassing)
bool InstallResizeEnforcerHook();
void UninstallResizeEnforcerHook();



// Internal window procedure for minimize prevention (don't call directly)
LRESULT CALLBACK MinimizeWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// (Timer-based minimize prevention removed)

// Window state change logging functions
bool InstallWindowStateLoggingHook();
void UninstallWindowStateLoggingHook();

// Helper functions for window state logging
void LogWindowMoveMessage(WPARAM wParam, LPARAM lParam);
void LogWindowSizeMessage(WPARAM wParam, LPARAM lParam);
void LogWindowSystemCommand(WPARAM wParam, LPARAM lParam);
void LogWindowPositionChanged(WPARAM wParam, LPARAM lParam);
void LogWindowStyleChanged(WPARAM wParam, LPARAM lParam);
void LogWindowActivationChanged(WPARAM wParam, LPARAM lParam);
void LogWindowFocusChanged(UINT uMsg);
void LogWindowShowStateChanged(WPARAM wParam, LPARAM lParam);
void LogWindowPositionChanging(WPARAM wParam, LPARAM lParam);

// Window message hook functions (for window creation and lifecycle)
bool InstallWindowMessageHook();
void UninstallWindowMessageHook();

// Borderless style enforcement functions
void StartBorderlessStyleTimer();
void StopBorderlessStyleTimer();
void ApplyBorderlessStylesAggressively();
void ApplyBorderlessStylesToAllWindows();
bool InstallAllWindowHooks();
void UninstallAllWindowHooks();
