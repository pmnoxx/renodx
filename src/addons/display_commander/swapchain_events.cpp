#include "addon.hpp"

void OnInitSwapchain(reshade::api::swapchain* swapchain, bool resize) {
  // Update last known backbuffer size
  auto* device = swapchain->get_device();
  if (device != nullptr && swapchain->get_back_buffer_count() > 0) {
    auto bb = swapchain->get_back_buffer(0);
    auto desc = device->get_resource_desc(bb);
    g_last_backbuffer_width.store(static_cast<int>(desc.texture.width));
    g_last_backbuffer_height.store(static_cast<int>(desc.texture.height));
    std::ostringstream oss; oss << "OnInitSwapchain(backbuffer=" << desc.texture.width << "x" << desc.texture.height
                                << ", resize=" << (resize ? "true" : "false") << ")";
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
  
  // Reinstall minimize prevention if it was enabled and the window handle changed
  extern float s_prevent_windows_minimize;
  if (s_prevent_windows_minimize >= 0.5f) {
    extern bool InstallMinimizeHook();
    InstallMinimizeHook();
  }
}

// Update composition state after presents (required for valid stats)
static void OnPresentUpdate(
    reshade::api::command_queue* /*queue*/, reshade::api::swapchain* swapchain,
    const reshade::api::rect* /*source_rect*/, const reshade::api::rect* /*dest_rect*/,
    uint32_t /*dirty_rect_count*/, const reshade::api::rect* /*dirty_rects*/) {
  // Throttle queries to ~every 30 presents
  int c = ++g_comp_query_counter;
  
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
  int last = g_comp_last_logged.load();
  if (state != last) {
    g_comp_last_logged.store(state);
    std::ostringstream oss;
    oss << "DXGI Composition State (present): " << DxgiBypassModeToString(mode) << " (" << state << ")";
    LogInfo(oss.str().c_str());
  }
}
