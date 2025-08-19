#include "background_task_coordinator.hpp"
#include "../addon.hpp"
#include <chrono>

// Global state
std::atomic<bool> renodx::background::g_background_tasks_running{false};
std::thread renodx::background::g_background_tasks_thread;

// Include individual background task headers
#include "audio_management_task.hpp"
#include "hooks_monitor_task.hpp"

// Main background task coordinator thread function
void BackgroundTasksThread() {
    LogInfo("Background tasks coordinator thread started");
    
    while (renodx::background::g_background_tasks_running.load()) {
        // Run all background tasks
        RunAudioManagementTask();
        renodx::background::RunHooksMonitorTask();
        
        // Sleep for 300ms between task cycles
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    
    LogInfo("Background tasks coordinator thread stopped");
}

void renodx::background::StartBackgroundTasks() {
    if (g_background_tasks_running.load()) {
        LogWarn("Background tasks are already running");
        return;
    }
    
    g_background_tasks_running.store(true);
    g_background_tasks_thread = std::thread(BackgroundTasksThread);
    
    LogInfo("Background tasks coordinator started");
}

void renodx::background::StopBackgroundTasks() {
    if (!g_background_tasks_running.load()) {
        LogWarn("Background tasks are not running");
        return;
    }
    
    g_background_tasks_running.store(false);
    
    if (g_background_tasks_thread.joinable()) {
        g_background_tasks_thread.join();
    }
    
    LogInfo("Background tasks coordinator stopped");
}
