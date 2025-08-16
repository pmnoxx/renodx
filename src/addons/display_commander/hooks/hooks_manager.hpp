#pragma once

#include <windows.h>

namespace renodx::hooks {

// Hooks manager functions
void InstallAllHooks();
void UninstallAllHooks();
void UpdateHooks();

// Check if any hooks are installed
bool AreAnyHooksInstalled();

// Individual hook management
void InstallWindowStyleHooks();
void UninstallWindowStyleHooks();

} // namespace renodx::hooks
