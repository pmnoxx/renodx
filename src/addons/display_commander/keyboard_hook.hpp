#pragma once

#include <windows.h>

// Alt-Tab suppression functions
bool InstallAltTabHook();
void UninstallAltTabHook();

// Windows minimize prevention functions
bool InstallMinimizeHook();
void UninstallMinimizeHook();

// Internal hook procedure (don't call directly)
LRESULT CALLBACK AltTabHookProc(int nCode, WPARAM wParam, LPARAM lParam);

// Internal minimize hook procedure (don't call directly)
LRESULT CALLBACK MinimizeHookProc(int nCode, WPARAM wParam, LPARAM lParam);
