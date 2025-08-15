#pragma once

#include <windows.h>

// Window subclassing hook functions
bool InstallMinimizeHook();
void UninstallMinimizeHook();
bool InstallResizeEnforcerHook();
void UninstallResizeEnforcerHook();

// Hook procedures
LRESULT CALLBACK MinimizeWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ResizeEnforcerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
