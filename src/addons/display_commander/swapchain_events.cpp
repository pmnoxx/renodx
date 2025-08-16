#include "addon.hpp"
#include "reflex/reflex_management.hpp"
#include "dxgi/dxgi_device_info.hpp"
#include <dxgi1_4.h>
#include "../../utils/swapchain.hpp"
#include "utils.hpp"
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

  // Continuous rendering system - runs on EVERY frame when enabled
  if (s_continuous_rendering_enabled >= 0.5f) {
    // Add throttling to prevent excessive processing that could cause focus issues
    static int continuous_frame_counter = 0;
    continuous_frame_counter++;
    
    // Use configurable throttling to prevent excessive processing that could cause focus issues
    int throttle_factor = static_cast<int>(s_continuous_rendering_throttle);
    if (throttle_factor < 1) throttle_factor = 1; // Minimum throttle of 1 (every frame)
    if (throttle_factor > 10) throttle_factor = 10; // Maximum throttle of 10 (every 10th frame)
    
    // Check if force continuous rendering is enabled (runs on every frame)
    bool force_mode = s_force_continuous_rendering >= 0.5f;
    if (force_mode) {
      throttle_factor = 1; // Force to every frame
    }
    
    // Log continuous rendering start on first frame
    static bool continuous_rendering_started = false;
    if (!continuous_rendering_started) {
      std::ostringstream oss;
      oss << "Continuous rendering STARTED - Throttle: every " << throttle_factor << " frames" 
          << (force_mode ? " (FORCE MODE)" : "");
      LogInfo(oss.str().c_str());
      continuous_rendering_started = true;
    }
    
    // Run continuous rendering with throttling, regardless of focus state
    // This is more aggressive and works better with focus spoofing
    if ((continuous_frame_counter % throttle_factor) == 0) {
      // Update frame counter for continuous rendering
      g_current_frame.fetch_add(1);
      
      // Update performance metrics every frame
      static auto last_frame_time = std::chrono::high_resolution_clock::now();
      auto now = std::chrono::high_resolution_clock::now();
      auto frame_duration = std::chrono::duration_cast<std::chrono::microseconds>(now - last_frame_time);
      last_frame_time = now;
      
      // Convert to milliseconds for UI display
      float frame_time_ms = frame_duration.count() / 1000.0f;
      
      // Update latency tracking (similar to Special-K's frame pacing widget)
      g_current_latency_ms.store(frame_time_ms);
      
      // Update rolling average (simple exponential moving average)
      static float avg_latency = 16.67f; // Start with 60 FPS assumption
      avg_latency = avg_latency * 0.95f + frame_time_ms * 0.05f;
      g_average_latency_ms.store(avg_latency);
      
      // Update min/max latency tracking
      static float min_latency = 16.67f;
      static float max_latency = 16.67f;
      
      if (frame_time_ms < min_latency) {
        min_latency = frame_time_ms;
        g_min_latency_ms.store(min_latency);
      }
      if (frame_time_ms > max_latency) {
        max_latency = frame_time_ms;
        g_max_latency_ms.store(max_latency);
      }
      
      // Reset min/max every 1000 frames to prevent stale data
      if ((g_current_frame.load() % 1000) == 0) {
        min_latency = frame_time_ms;
        max_latency = frame_time_ms;
        g_min_latency_ms.store(min_latency);
        g_max_latency_ms.store(max_latency);
      }
      
      // Log continuous rendering status every 300 frames (about 5 seconds at 60 FPS)
      if ((g_current_frame.load() % 300) == 0) {
        std::ostringstream oss;
        oss << "Continuous rendering active - Frame: " << g_current_frame.load() 
            << ", Latency: " << std::fixed << std::setprecision(2) << frame_time_ms << " ms"
            << ", Avg: " << std::fixed << std::setprecision(2) << avg_latency << " ms"
            << ", Min: " << std::fixed << std::setprecision(2) << min_latency << " ms"
            << ", Max: " << std::fixed << std::setprecision(2) << max_latency << " ms";
        LogDebug(oss.str());
      }
      
      // Signal that the rendering pipeline is active
      // This helps the background thread know when the game is actually rendering
      static auto last_active_signal = std::chrono::steady_clock::now();
      last_active_signal = now;
      
      // Log that we're actively processing frames (this helps debug focus loss issues)
      static int active_frame_log_counter = 0;
      active_frame_log_counter++;
      if (active_frame_log_counter % 100 == 0) { // Log every 100 active frames
        LogDebug("Continuous rendering: Game actively rendering frames - Counter: " + std::to_string(active_frame_log_counter));
      }
    }
  }
  
  // DXGI-based continuous rendering - detect focus loss and force rendering to continue
  if (s_continuous_rendering_enabled >= 0.5f) {
    static int present_call_counter = 0;
    auto now = std::chrono::steady_clock::now();
    
    // Update present call counter and last present time
    present_call_counter++;
    g_last_present_time.store(now);
    
    // Log present time updates every 100 frames to help debug focus loss detection
    if (present_call_counter % 100 == 0) {
      std::ostringstream oss;
      oss << "Present time updated - Counter: " << present_call_counter << ", Time: " 
          << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 100000;
      LogDebug(oss.str().c_str());
    }
    
    // Reset focus loss detection since we're getting present calls
    if (g_focus_loss_detected.load()) {
      std::ostringstream oss;
      oss << "Focus restored - present calls resumed after " << g_focus_loss_counter.load() << " cycles";
      LogInfo(oss.str().c_str());
      g_focus_loss_detected.store(false);
      g_focus_loss_counter.store(0);
    }
    
    // Log present call activity every 300 frames (about 5 seconds at 60 FPS)
    if (present_call_counter % 300 == 0) {
      std::ostringstream oss;
      oss << "Continuous rendering: Is it possPresent calls active - Counter: " << present_call_counter;
      LogDebug(oss.str().c_str());
    }
    
      // CONTINUOUS RENDERING FEATURE REMOVED - Focus spoofing is now handled by Win32 hooks
      // This provides a much cleaner and more effective solution
    } // Close the continuous rendering block
  
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
