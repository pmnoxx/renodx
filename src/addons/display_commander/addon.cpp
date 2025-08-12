#include "addon.hpp"

// Global variables moved to globals.cpp
#include "globals.cpp"

// Constants moved to utils.cpp

// Forward decl
void ComputeDesiredSize(int& out_w, int& out_h);

// RectFromWH is now defined in the header file

// IsExclusiveFullscreen moved to utils.cpp

// ComputeSWPFlags moved to utils.cpp

// Logging functions moved to utils.cpp

// Borderless functions moved to utils.cpp

// Window management functions moved to window_management.cpp
#include "window_management.cpp"

// DXGI management functions moved to dxgi_management.cpp
#include "dxgi_management.cpp"

// Audio management functions moved to audio_management.cpp
#include "audio_management.cpp"

// Swapchain event handlers moved to swapchain_events.cpp
#include "swapchain_events.cpp"

// Keyboard hook functions moved to keyboard_hook.cpp
#include "keyboard_hook.cpp"

// UI settings moved to ui_settings.cpp
#include "ui_settings.cpp"

// Main entry point moved to main_entry.cpp
#include "main_entry.cpp"

// g_last_backbuffer_width and g_last_backbuffer_height moved to globals.cpp

// DxgiBypassMode is now defined in the header file

// GetIndependentFlipState function moved to dxgi_management.cpp

// DxgiBypassModeToString function moved to dxgi_management.cpp

// RemoveWindowBorder moved to utils.cpp

// SetIndependentFlipState function moved to dxgi_management.cpp

// Borderless functions moved to utils.cpp

// MonitorInfo is now defined in the header file
// g_monitors is now defined at the top of the file

// MonitorEnumProc moved to utils.cpp

std::vector<std::string> MakeMonitorLabels();

// WindowStyleMode is now defined in the header file

// ApplyWindowChange function moved to window_management.cpp

// No desktop mode changes in this addon per requirements

// Fullscreen detection/exit removed

// Auto-apply on swapchain init scheduler
// Global variables are now defined at the top of the file

// SetMuteForCurrentProcess function moved to audio_management.cpp

// SetVolumeForCurrentProcess function moved to audio_management.cpp

// RunBackgroundAudioMonitor function moved to audio_management.cpp

// ForceWindowToMonitorOrigin functions moved to utils.cpp

// LogIndependentFlipConditions function moved to dxgi_management.cpp


// ShouldApplyWindowedForBackbuffer function moved to window_management.cpp

// ScheduleAutoApplyOnInit function moved to window_management.cpp

// Utility functions moved to utils.cpp

// ComputeDesiredSize functions moved to utils.cpp

// MakeMonitorLabels moved to utils.cpp

extern "C" __declspec(dllexport) constexpr const char* NAME = "Resolution Override";
extern "C" __declspec(dllexport) constexpr const char* DESCRIPTION =
    "RenoDX Resolution/Window Override (Win32). Note: Applying window operations from the main thread can crash some apps; this addon performs them on a background thread.";

// OnInitSwapchain function moved to swapchain_events.cpp

// OnPresentUpdate function moved to swapchain_events.cpp

// DllMain function moved to main_entry.cpp


