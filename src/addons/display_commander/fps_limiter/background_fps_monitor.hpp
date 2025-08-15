#pragma once

#include <windows.h>
#include <atomic>
#include <thread>

// Background FPS monitoring functions
void StartBackgroundFpsMonitor();
void StopBackgroundFpsMonitor();

// Global state
extern std::atomic<bool> g_background_fps_monitor_running;
extern std::thread g_background_fps_monitor_thread;
