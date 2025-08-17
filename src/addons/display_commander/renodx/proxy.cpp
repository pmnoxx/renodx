#include "proxy.hpp"
#include "../addon.hpp"

// Forward declarations for RenoDX includes
#include "../../../utils/settings.hpp"
#include "../../../utils/swapchain.hpp"
#include "../background_tasks/background_task_coordinator.hpp"
#include "../hooks/hooks_manager.hpp"
#include "../../../mods/swapchain.hpp"

namespace renodx::proxy {

// ============================================================================
// Settings System Proxy Implementation
// ============================================================================

void SetUsePresets(bool use) {
    renodx::utils::settings::use_presets = use;
}

void SetGlobalName(const std::string& name) {
    renodx::utils::settings::global_name = name;
}

void InitializeSettings(DWORD fdw_reason, void* settings_vector) {
    auto* settings = static_cast<renodx::utils::settings::Settings*>(settings_vector);
    renodx::utils::settings::Use(fdw_reason, settings);
}

// ============================================================================
// Background Task System Proxy Implementation
// ============================================================================

void StartBackgroundTasks() {
    renodx::background::StartBackgroundTasks();
}

void StopBackgroundTasks() {
    // Note: RenoDX doesn't have a StopBackgroundTasks function yet
    // This would need to be implemented in RenoDX or we'd need to track tasks ourselves
}

// ============================================================================
// Hooks System Proxy Implementation
// ============================================================================

void InstallAllHooks() {
    renodx::hooks::InstallAllHooks();
}

void UninstallAllHooks() {
    renodx::hooks::UninstallAllHooks();
}

// ============================================================================
// Swapchain Mods Proxy Implementation
// ============================================================================

void SetFullscreenPrevention(bool enabled) {
    renodx::mods::swapchain::prevent_full_screen = enabled;
}

void SetForceBorderless(bool enabled) {
    renodx::mods::swapchain::force_borderless = enabled;
}

void SetForceScreenTearing(bool enabled) {
    renodx::mods::swapchain::force_screen_tearing = enabled;
}

void SetUseDeviceProxy(bool enabled) {
    renodx::mods::swapchain::use_device_proxy = enabled;
}

bool GetFullscreenPrevention() {
    return renodx::mods::swapchain::prevent_full_screen;
}

bool GetForceBorderless() {
    return renodx::mods::swapchain::force_borderless;
}

bool GetForceScreenTearing() {
    return renodx::mods::swapchain::force_screen_tearing;
}

bool GetUseDeviceProxy() {
    return renodx::mods::swapchain::use_device_proxy;
}

// ============================================================================
// Swapchain Utilities Proxy Implementation
// ============================================================================

float GetFpsLimit() {
    return renodx::utils::swapchain::fps_limit;
}

void SetFpsLimit(float limit) {
    renodx::utils::swapchain::fps_limit = limit;
}

void InitializeSwapchain(DWORD fdw_reason) {
    // DISABLED FOR NOW, due to potencia lcrashes
    // renodx::utils::swapchain::Use(fdw_reason);
}

void ResizeSwapchainBuffers(void* swapchain, int target_format, int target_color_space) {
    // Convert void* to reshade::api::swapchain*
    auto* reshade_swapchain = static_cast<reshade::api::swapchain*>(swapchain);
    
    // Convert int parameters to proper types
    auto format = static_cast<reshade::api::format>(target_format);
    auto color_space = static_cast<reshade::api::color_space>(target_color_space);
    
    renodx::utils::swapchain::ResizeBuffer(reshade_swapchain, format, color_space);
}

void FastResizeSwapchainBuffers(void* swapchain) {
    auto* reshade_swapchain = static_cast<reshade::api::swapchain*>(swapchain);
    // Note: FastResizeBuffer doesn't exist in current RenoDX
    // Using regular ResizeBuffer instead
    renodx::utils::swapchain::ResizeBuffer(reshade_swapchain, reshade::api::format::r16g16b16a16_float, reshade::api::color_space::extended_srgb_linear);
}

// ============================================================================
// Data Management Proxy Implementation
// ============================================================================

template<typename T>
T* GetDeviceData(void* device) {
    auto* reshade_device = static_cast<reshade::api::device*>(device);
    return renodx::utils::data::Get<T>(reshade_device);
}

void DeleteDeviceData(void* device) {
    auto* reshade_device = static_cast<reshade::api::device*>(device);
    // Note: This requires the data type to be specified
    // For now, we'll use void* as a placeholder
    // In practice, you'd need to know the specific type
    // renodx::utils::data::Delete<void>(reshade_device);
}

// Note: Template instantiation for void* is problematic
// In practice, specific types would be used

// ============================================================================
// System Initialization
// ============================================================================

void Initialize(DWORD fdw_reason, void* settings_vector) {
    InitializeSettings(fdw_reason, settings_vector);
    InitializeSwapchain(fdw_reason);
}

void Shutdown(DWORD fdw_reason) {
    // Cleanup is handled by individual systems
    // This function provides a centralized shutdown point for future use
}

} // namespace renodx::proxy
