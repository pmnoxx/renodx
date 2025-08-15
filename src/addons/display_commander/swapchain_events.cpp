#include "addon.hpp"
#include "reflex_management.hpp"
#include "dxgi_device_info.hpp"
#include <dxgi1_4.h>
#include "../../utils/swapchain.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

void OnInitSwapchain(reshade::api::swapchain* swapchain, bool resize) {
  // Update last known backbuffer size and colorspace
  auto* device = swapchain->get_device();
  if (device != nullptr && swapchain->get_back_buffer_count() > 0) {
    auto bb = swapchain->get_back_buffer(0);
    auto desc = device->get_resource_desc(bb);
    g_last_backbuffer_width.store(static_cast<int>(desc.texture.width));
    g_last_backbuffer_height.store(static_cast<int>(desc.texture.height));
    
    // Store current colorspace for UI display
    g_current_colorspace = swapchain->get_color_space();
    
    std::ostringstream oss; oss << "OnInitSwapchain(backbuffer=" << desc.texture.width << "x" << desc.texture.height
                                << ", resize=" << (resize ? "true" : "false") << ", colorspace=" << static_cast<int>(g_current_colorspace) << ")";
    LogDebug(oss.str());
    
    // Schedule auto-apply even on resizes (generation counter ensures only latest runs)
  }

  // Schedule auto-apply even on resizes (generation counter ensures only latest runs)
  HWND hwnd = static_cast<HWND>(swapchain->get_hwnd());
  g_last_swapchain_hwnd.store(hwnd);
  g_last_swapchain_ptr.store(swapchain);
  if (hwnd == nullptr) return;
  // Update DXGI composition state if possible
  {
    DxgiBypassMode mode = GetIndependentFlipState(swapchain);
    switch (mode) {
      case DxgiBypassMode::kUnknown: s_dxgi_composition_state = 0.f; break;
      case DxgiBypassMode::kComposed: s_dxgi_composition_state = 1.f; break;
      case DxgiBypassMode::kOverlay: s_dxgi_composition_state = 2.f; break;
      case DxgiBypassMode::kIndependentFlip: s_dxgi_composition_state = 3.f; break;
    }
    std::ostringstream oss2;
    oss2 << "DXGI Composition State (onSwapChainInit): " << DxgiBypassModeToString(mode)
         << " (" << static_cast<int>(s_dxgi_composition_state) << ")";
    LogInfo(oss2.str().c_str());
  }
  
  // Log Independent Flip conditions to update failure tracking
  LogIndependentFlipConditions(swapchain);
  LogDebug(resize ? "Schedule auto-apply on swapchain init (resize)"
                  : "Schedule auto-apply on swapchain init");
  ScheduleAutoApplyOnInit(hwnd);
  
  // Fix HDR10 colorspace if enabled and needed
  if (s_fix_hdr10_colorspace >= 0.5f) {
    FixHDR10Colorspace(swapchain);
  }
  
  // Note: Minimize hook removed - use continuous monitoring instead

  // Set Reflex sleep mode and latency markers if enabled
  if (s_reflex_enabled >= 0.5f) {
    SetReflexSleepMode(swapchain);
    SetReflexLatencyMarkers(swapchain);
    // Also call sleep to start the frame properly
    extern std::unique_ptr<ReflexManager> g_reflexManager;
    if (g_reflexManager && g_reflexManager->IsAvailable()) {
      g_reflexManager->CallSleep(swapchain);
    }
  }
}

// Update composition state after presents (required for valid stats)
static void OnPresentUpdate(
    reshade::api::command_queue* /*queue*/, reshade::api::swapchain* swapchain,
    const reshade::api::rect* /*source_rect*/, const reshade::api::rect* /*dest_rect*/,
    uint32_t /*dirty_rect_count*/, const reshade::api::rect* /*dirty_rects*/) {
  // Throttle queries to ~every 30 presents
  int c = ++g_comp_query_counter;
  
  // Call Reflex functions on EVERY frame (not throttled)
  if (s_reflex_enabled >= 0.5f) {
    extern std::unique_ptr<ReflexManager> g_reflexManager;
    extern std::atomic<bool> g_reflex_settings_changed;
    
    if (g_reflexManager && g_reflexManager->IsAvailable()) {
      // Force sleep mode update if settings have changed (required for NVIDIA overlay detection)
      if (g_reflex_settings_changed.load()) {
        SetReflexSleepMode(swapchain);
        g_reflex_settings_changed.store(false);
        LogDebug("Sleep mode updated due to settings change");
      } else if ((c % 60) == 0) { // Added periodic refresh
        // Refresh sleep mode every 60 frames to maintain Reflex state
        SetReflexSleepMode(swapchain);
      }
      // Call sleep at frame start (this is crucial for Reflex to work)
      g_reflexManager->CallSleep(swapchain);
      // Set full latency markers (SIMULATION, RENDERSUBMIT, INPUT) as PCLStats ETW events
      // PRESENT markers are sent separately to NVAPI only
      SetReflexLatencyMarkers(swapchain);
      // Set PRESENT markers to wrap the Present call (NVAPI only, no ETW)
      SetReflexPresentMarkers(swapchain);
    }
  }
  
  if ((c % 30) != 0) return;
  
  
  DxgiBypassMode mode = GetIndependentFlipState(swapchain);
  int state = 0;
  switch (mode) {
    case DxgiBypassMode::kComposed: state = 1; break;
    case DxgiBypassMode::kOverlay: state = 2; break;
    case DxgiBypassMode::kIndependentFlip: state = 3; break;
    case DxgiBypassMode::kUnknown:
    default: state = 0; break;
  }
  s_dxgi_composition_state = static_cast<float>(state);
  
  // Update colorspace info (throttled to avoid excessive calls)
  if ((c % 30) == 0) {
    g_current_colorspace = swapchain->get_color_space();
    
    // Enumerate DXGI devices during present (throttled to avoid excessive calls)
    extern std::unique_ptr<DXGIDeviceInfoManager> g_dxgiDeviceInfoManager;
    if (g_dxgiDeviceInfoManager && g_dxgiDeviceInfoManager->IsInitialized()) {
      g_dxgiDeviceInfoManager->EnumerateDevicesOnPresent();
    }
  }
  
  int last = g_comp_last_logged.load();
  if (state != last) {
    g_comp_last_logged.store(state);
    std::ostringstream oss;
    oss << "DXGI Composition State (present): " << DxgiBypassModeToString(mode) << " (" << state << ")";
    LogInfo(oss.str().c_str());
  }


}

// Fix HDR10 colorspace when swapchain format is RGB10A2 and colorspace is not already HDR10
void FixHDR10Colorspace(reshade::api::swapchain* swapchain) {
  if (swapchain == nullptr) return;
  
  // Update timestamp
  auto now = std::chrono::system_clock::now();
  auto time_t = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
  g_hdr10_override_timestamp = ss.str();
  
  // Get the device to check backbuffer format
  auto* device = swapchain->get_device();
  if (device == nullptr || swapchain->get_back_buffer_count() == 0) return;
  
  // Get backbuffer description to check format
  auto bb = swapchain->get_back_buffer(0);
  auto desc = device->get_resource_desc(bb);
  
  // Only proceed if format is RGB10A2 (R10G10B10A2_UNORM)
  if (desc.texture.format != reshade::api::format::r10g10b10a2_unorm) {
    return;
  }
  
  // Only proceed if current colorspace is sRGB (srgb_nonlinear)
  auto current_colorspace = swapchain->get_color_space();
  if (current_colorspace != reshade::api::color_space::srgb_nonlinear) {
    g_hdr10_override_status = "Skipped: colorspace not sRGB";
    LogDebug("Skipping HDR10 colorspace fix: current colorspace is not sRGB");
    return;
  }
  
  // Use the proper ChangeColorSpace function from utils
  // This handles all the DXGI interface management and runtime updates properly
  if (renodx::utils::swapchain::ChangeColorSpace(swapchain, reshade::api::color_space::hdr10_st2084)) {
    LogInfo("Successfully changed colorspace from sRGB to HDR10 ST2084");
    
    // Verify the change took effect
    auto new_colorspace = swapchain->get_color_space();
    if (new_colorspace == reshade::api::color_space::hdr10_st2084) {
      g_hdr10_override_status = "Success: changed to HDR10 ST2084";
      LogInfo("Colorspace change verified: now using HDR10 ST2084");
    } else {
      g_hdr10_override_status = "Warning: change may not have taken effect";
      std::ostringstream oss;
      oss << "Colorspace change may not have taken effect. Current: " << static_cast<int>(new_colorspace);
      LogWarn(oss.str().c_str());
    }
  } else {
    g_hdr10_override_status = "Failed: ChangeColorSpace returned false";
    LogWarn("Failed to change colorspace to HDR10 ST2084");
  }
}
