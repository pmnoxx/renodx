#pragma once

#include <windows.h>

// Window creation hook functions
bool InstallWindowCreationHooks();
void UninstallWindowCreationHooks();
bool InstallCreateWindowExHooks();
void UninstallCreateWindowExHooks();

// Hook procedures
LRESULT CALLBACK CreateWindowHookProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
