#pragma once

#include <windows.h>

// Alt-Tab suppression functions
bool InstallAltTabHook();
void UninstallAltTabHook();

// Windows minimize prevention functions (using window subclassing)
bool InstallMinimizeHook();
void UninstallMinimizeHook();

// Window resize enforcement (using window subclassing)
bool InstallResizeEnforcerHook();
void UninstallResizeEnforcerHook();

// Internal hook procedure (don't call directly)
LRESULT CALLBACK AltTabHookProc(int nCode, WPARAM wParam, LPARAM lParam);

// Internal window procedure for minimize prevention (don't call directly)
LRESULT CALLBACK MinimizeWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Timer callback for minimize prevention (don't call directly)
VOID CALLBACK MinimizePreventionTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

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
