#pragma once

#include <windows.h>

// Alt-Tab suppression functions
bool InstallAltTabHook();
void UninstallAltTabHook();

// Internal hook procedure (don't call directly)
LRESULT CALLBACK AltTabHookProc(int nCode, WPARAM wParam, LPARAM lParam);
