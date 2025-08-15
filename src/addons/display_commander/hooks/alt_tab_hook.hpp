#pragma once

#include <windows.h>

// Alt-Tab suppression hook functions
bool InstallAltTabHook();
void UninstallAltTabHook();

// Hook procedure for Alt-Tab suppression
LRESULT CALLBACK AltTabHookProc(int nCode, WPARAM wParam, LPARAM lParam);
