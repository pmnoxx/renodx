#pragma once

#include <windows.h>

// Borderless style enforcement functions
void StartBorderlessStyleTimer();
void StopBorderlessStyleTimer();
void ApplyBorderlessStylesAggressively();
void ApplyBorderlessStylesToAllWindows();

// Timer-based borderless style enforcement
extern UINT_PTR g_borderless_timer_id;
extern bool g_borderless_timer_active;
