#pragma once

#include <windows.h>
#include <string>
#include <functional>

namespace renodx::proxy {

// ============================================================================
// Settings System Proxy
// ============================================================================

/**
 * Configure whether to use preset system
 * @param use True to enable presets, false to disable
 */
void SetUsePresets(bool use);

/**
 * Set the global settings namespace
 * @param name The namespace identifier
 */
void SetGlobalName(const std::string& name);

/**
 * Initialize the settings system
 * @param fdw_reason DLL attach/detach reason
 * @param settings_vector Vector of settings to register
 */
void InitializeSettings(DWORD fdw_reason, void* settings_vector);

// ============================================================================
// Background Task System Proxy
// ============================================================================

/**
 * Start the background task system
 * Initializes FPS limiting, audio monitoring, and window management tasks
 */
void StartBackgroundTasks();

/**
 * Stop the background task system
 * Cleans up all background tasks and threads
 */
void StopBackgroundTasks();

// ============================================================================
// Hooks System Proxy
// ============================================================================

/**
 * Install all window hooks
 * Sets up window subclassing and message filtering
 */
void InstallAllHooks();

/**
 * Uninstall all window hooks
 * Cleans up installed hooks and subclasses
 */
void UninstallAllHooks();

// ============================================================================
// Swapchain Mods Proxy
// ============================================================================

/**
 * Set global fullscreen prevention flag
 * @param enabled True to prevent fullscreen, false to allow
 */
void SetFullscreenPrevention(bool enabled);

/**
 * Set global borderless mode flag
 * @param enabled True to force borderless, false to allow normal windows
 */
void SetForceBorderless(bool enabled);

/**
 * Set global screen tearing flag
 * @param enabled True to allow screen tearing, false to prevent
 */
void SetForceScreenTearing(bool enabled);

/**
 * Set device proxy usage flag
 * @param enabled True to use device proxy, false to use direct device
 */
void SetUseDeviceProxy(bool enabled);

/**
 * Get current fullscreen prevention state
 * @return True if fullscreen prevention is enabled
 */
bool GetFullscreenPrevention();

/**
 * Get current borderless mode state
 * @return True if borderless mode is forced
 */
bool GetForceBorderless();

/**
 * Get current screen tearing state
 * @return True if screen tearing is allowed
 */
bool GetForceScreenTearing();

/**
 * Get current device proxy usage state
 * @return True if device proxy is enabled
 */
bool GetUseDeviceProxy();

// ============================================================================
// Swapchain Utilities Proxy
// ============================================================================

/**
 * Get current FPS limit
 * @return Current FPS limit value (0 = unlimited)
 */
float GetFpsLimit();

/**
 * Set FPS limit
 * @param limit FPS limit value (0 = unlimited)
 */
void SetFpsLimit(float limit);

/**
 * Initialize swapchain system
 * @param fdw_reason DLL attach/detach reason
 */
void InitializeSwapchain(DWORD fdw_reason);

/**
 * Resize swapchain buffers
 * @param swapchain The swapchain to resize
 * @param target_format Target format for the new buffers
 * @param target_color_space Target color space
 */
void ResizeSwapchainBuffers(void* swapchain, int target_format, int target_color_space);

/**
 * Fast resize swapchain buffers
 * @param swapchain The swapchain to resize
 */
void FastResizeSwapchainBuffers(void* swapchain);

// ============================================================================
// Data Management Proxy
// ============================================================================

/**
 * Get device-specific data
 * @tparam T The data type to retrieve
 * @param device The device to get data for
 * @return Pointer to device data, or nullptr if not found
 */
template<typename T>
T* GetDeviceData(void* device);

/**
 * Delete device-specific data
 * @param device The device to clean up data for
 */
void DeleteDeviceData(void* device);

// ============================================================================
// System Initialization
// ============================================================================

/**
 * Initialize all RenoDX proxy systems
 * @param fdw_reason DLL attach/detach reason
 * @param settings_vector Vector of settings to register
 */
void Initialize(DWORD fdw_reason, void* settings_vector);

/**
 * Shutdown all RenoDX proxy systems
 * @param fdw_reason DLL attach/detach reason
 */
void Shutdown(DWORD fdw_reason);

} // namespace renodx::proxy
