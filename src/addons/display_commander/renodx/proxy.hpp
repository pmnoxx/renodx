#pragma once

#include <windows.h>
#include <string>

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

} // namespace renodx::proxy
