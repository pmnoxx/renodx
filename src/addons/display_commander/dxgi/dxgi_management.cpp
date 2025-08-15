#include "../addon.hpp"

inline DxgiBypassMode GetIndependentFlipState(reshade::api::swapchain* swapchain) {
  // Get or create the failure tracking structure
  IndependentFlipFailures* failures = g_if_failures.load();
  if (failures == nullptr) {
    failures = new IndependentFlipFailures();
    IndependentFlipFailures* expected = nullptr;
    if (!g_if_failures.compare_exchange_strong(expected, failures)) {
      delete failures;
      failures = expected;
    }
  }
  
  if (swapchain == nullptr) {
    LogDebug("DXGI IF state: swapchain is null");
    failures->swapchain_null.store(true);
    return DxgiBypassMode::kUnknown;
  }
  auto* device = swapchain->get_device();
  if (device == nullptr) {
    LogDebug("DXGI IF state: device is null");
    failures->device_null.store(true);
    return DxgiBypassMode::kUnknown;
  }

  const auto api = device->get_api();
  if (api != reshade::api::device_api::d3d10 &&
      api != reshade::api::device_api::d3d11 &&
      api != reshade::api::device_api::d3d12) {
    std::ostringstream oss; oss << "DXGI IF state: non-DXGI device api=" << static_cast<int>(api);
    LogDebug(oss.str());
    failures->non_dxgi_api.store(true);
    return DxgiBypassMode::kUnknown; // Not DXGI-backed
  }

  auto* dxgi_swapchain = reinterpret_cast<IDXGISwapChain*>(swapchain->get_native());
  if (dxgi_swapchain == nullptr) {
    LogDebug("DXGI IF state: native swapchain is null");
    return DxgiBypassMode::kUnknown;
  }

  Microsoft::WRL::ComPtr<IDXGISwapChainMedia> media;
  {
    HRESULT hr = dxgi_swapchain->QueryInterface(IID_PPV_ARGS(&media));
    if (FAILED(hr)) {
      std::ostringstream oss; oss << "DXGI IF state: QI IDXGISwapChainMedia failed hr=0x" << std::hex << hr;
      LogDebug(oss.str());
      failures->swapchain_media_failed.store(true);
      // Log swap effect for diagnostics
      DXGI_SWAP_CHAIN_DESC scd{};
      if (SUCCEEDED(dxgi_swapchain->GetDesc(&scd))) {
        std::ostringstream oss2; oss2 << "DXGI IF state: SwapEffect=" << static_cast<int>(scd.SwapEffect);
        LogDebug(oss2.str());
      }
      return DxgiBypassMode::kUnknown;
    }
  }

  DXGI_FRAME_STATISTICS_MEDIA stats = {};
  {
    HRESULT hr = media->GetFrameStatisticsMedia(&stats);
    if (FAILED(hr)) {
      std::ostringstream oss; oss << "DXGI IF state: GetFrameStatisticsMedia failed hr=0x" << std::hex << hr
                                  << " (call after at least one Present)";
      LogDebug(oss.str());
      failures->frame_stats_failed.store(true);
      // Log swap effect for diagnostics
      DXGI_SWAP_CHAIN_DESC scd{};
      if (SUCCEEDED(dxgi_swapchain->GetDesc(&scd))) {
        std::ostringstream oss2; oss2 << "DXGI IF state: SwapEffect=" << static_cast<int>(scd.SwapEffect);
        LogDebug(oss2.str());
      }
      return DxgiBypassMode::kUnknown; // Call after at least one Present
    }
  }

  switch (stats.CompositionMode) {
    case DXGI_FRAME_PRESENTATION_MODE_COMPOSED: return DxgiBypassMode::kComposed;
    case DXGI_FRAME_PRESENTATION_MODE_OVERLAY: return DxgiBypassMode::kOverlay;
    case DXGI_FRAME_PRESENTATION_MODE_NONE: return DxgiBypassMode::kIndependentFlip;
    default: return DxgiBypassMode::kUnknown;
  }
}

inline const char* DxgiBypassModeToString(DxgiBypassMode mode) {
  switch (mode) {
    case DxgiBypassMode::kComposed: return "Composed";             
    case DxgiBypassMode::kOverlay: return "Hardware Overlay (MPO)";       
    case DxgiBypassMode::kIndependentFlip: return "Independent Flip"; 
    case DxgiBypassMode::kUnknown:
    default: return "Unknown";
  }
}

// Attempt to configure the DXGI swapchain for conditions that allow Independent Flip
// Only uses DXGI APIs (no Win32 window sizing). Returns true if a change was applied.
inline bool SetIndependentFlipState(reshade::api::swapchain* swapchain) {
  if (swapchain == nullptr) {
    LogWarn("SetIndependentFlipState: null swapchain");
    return false;
  }
  auto* device = swapchain->get_device();
  if (device == nullptr) {
    LogWarn("SetIndependentFlipState: null device");
    return false;
  }
  const auto api = device->get_api();
  if (api != reshade::api::device_api::d3d10 &&
      api != reshade::api::device_api::d3d11 &&
      api != reshade::api::device_api::d3d12) {
    LogWarn("SetIndependentFlipState: non-DXGI backend");
    return false;
  }

  IDXGISwapChain* sc = reinterpret_cast<IDXGISwapChain*>(swapchain->get_native());
  if (sc == nullptr) {
    LogWarn("SetIndependentFlipState: native swapchain is null");
    return false;
  }

  Microsoft::WRL::ComPtr<IDXGISwapChain4> sc4;
  if (FAILED(sc->QueryInterface(IID_PPV_ARGS(&sc4)))) {
    LogWarn("SetIndependentFlipState: IDXGISwapChain4 not available");
    return false;
  }

  DXGI_SWAP_CHAIN_DESC1 desc1{};
  if (FAILED(sc4->GetDesc1(&desc1))) {
    LogWarn("SetIndependentFlipState: GetDesc1 failed");
    return false;
  }

  // Must be flip-model for Independent Flip
  if (desc1.SwapEffect != DXGI_SWAP_EFFECT_FLIP_DISCARD &&
      desc1.SwapEffect != DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) {
    LogWarn("SetIndependentFlipState: not a flip-model swapchain (cannot change SwapEffect without recreation)");
    return false;
  }

  // Align backbuffer size to the containing output's resolution
  Microsoft::WRL::ComPtr<IDXGIOutput> output;
  if (FAILED(sc4->GetContainingOutput(&output)) || output == nullptr) {
    LogWarn("SetIndependentFlipState: GetContainingOutput failed");
    return false;
  }

  Microsoft::WRL::ComPtr<IDXGIOutput6> output6;
  if (FAILED(output->QueryInterface(IID_PPV_ARGS(&output6))) || output6 == nullptr) {
    LogWarn("SetIndependentFlipState: IDXGIOutput6 not available");
    return false;
  }

  DXGI_OUTPUT_DESC1 out_desc{};
  if (FAILED(output6->GetDesc1(&out_desc))) {
    LogWarn("SetIndependentFlipState: GetDesc1 on output failed");
    return false;
  }

  const UINT target_width = static_cast<UINT>(out_desc.DesktopCoordinates.right - out_desc.DesktopCoordinates.left);
  const UINT target_height = static_cast<UINT>(out_desc.DesktopCoordinates.bottom - out_desc.DesktopCoordinates.top);

  bool changed = false;
  if (desc1.Width != target_width || desc1.Height != target_height) {
    LogInfo("SetIndependentFlipState: Resizing buffers to match output resolution");
    const HRESULT hr = sc4->ResizeBuffers(
        0, // keep buffer count
        target_width,
        target_height,
        desc1.Format,
        desc1.Flags);
    if (FAILED(hr)) {
      std::ostringstream oss; oss << "SetIndependentFlipState: ResizeBuffers failed hr=0x" << std::hex << hr;
      LogWarn(oss.str().c_str());
      return false;
    }
    changed = true;
  }

  // Optional: Hint tearing allowed when supported (does not enforce IF, but can help latency)
  // This keeps flags as-is to avoid impacting app behavior.

  if (changed) {
    LogInfo("SetIndependentFlipState: Applied DXGI-only changes. Present a few frames to allow promotion.");
  } else {
    LogInfo("SetIndependentFlipState: Already matching output resolution; waiting for promotion conditions.");
  }
  return changed;
}

inline void LogIndependentFlipConditions(reshade::api::swapchain* swapchain) {
  // Get or create the failure tracking structure
  IndependentFlipFailures* failures = g_if_failures.load();
  if (failures == nullptr) {
    failures = new IndependentFlipFailures();
    IndependentFlipFailures* expected = nullptr;
    if (!g_if_failures.compare_exchange_strong(expected, failures)) {
      delete failures;
      failures = expected;
    }
  }
  
  // Reset previous failures
  failures->reset();
  
  if (swapchain == nullptr) {
    LogWarn("IF Conditions: swapchain=nullptr");
    failures->swapchain_null.store(true);
    return;
  }
  auto* device = swapchain->get_device();
  if (device == nullptr) {
    LogWarn("IF Conditions: device=nullptr");
    failures->device_null.store(true);
    return;
  }

  // API
  {
    const auto api = device->get_api();
    std::ostringstream oss; oss << "IF Conditions: device_api=" << static_cast<int>(api);
    LogInfo(oss.str().c_str());
    
    if (api != reshade::api::device_api::d3d10 &&
        api != reshade::api::device_api::d3d11 &&
        api != reshade::api::device_api::d3d12) {
      failures->non_dxgi_api.store(true);
    }
  }

  // DXGI swapchain desc
  IDXGISwapChain* sc = reinterpret_cast<IDXGISwapChain*>(swapchain->get_native());
  if (sc != nullptr) {
    Microsoft::WRL::ComPtr<IDXGISwapChain4> sc4;
    if (SUCCEEDED(sc->QueryInterface(IID_PPV_ARGS(&sc4))) && sc4 != nullptr) {
      DXGI_SWAP_CHAIN_DESC1 d1{};
      if (SUCCEEDED(sc4->GetDesc1(&d1))) {
        std::ostringstream oss;
        oss << "IF Conditions: SwapEffect=" << static_cast<int>(d1.SwapEffect)
            << ", Buffers=" << d1.BufferCount
            << ", Format=" << static_cast<int>(d1.Format)
            << ", Flags=0x" << std::hex << d1.Flags << std::dec
            << ", BB=" << d1.Width << "x" << d1.Height;
        LogInfo(oss.str().c_str());
        
        // Check if it's a flip model swapchain
        if (d1.SwapEffect != DXGI_SWAP_EFFECT_FLIP_DISCARD &&
            d1.SwapEffect != DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) {
          failures->not_flip_model.store(true);
        }
      }
    }
  }

  // Color space
  {
    std::ostringstream oss; oss << "IF Conditions: color_space=" << static_cast<int>(swapchain->get_color_space());
    LogInfo(oss.str().c_str());
  }

  // Window vs monitor sizing and styles
  HWND hwnd = static_cast<HWND>(swapchain->get_hwnd());
  if (hwnd == nullptr) {
    LogWarn("IF Conditions: hwnd=nullptr");
    failures->hwnd_null.store(true);
    return;
  }
  // Get window rectangle (including decorations) and client rectangle (actual rendering area)
  RECT wr{}; GetWindowRect(hwnd, &wr);
  RECT cr{}; GetClientRect(hwnd, &cr);
  
  // Convert client coordinates to screen coordinates for accurate position
  POINT client_origin = {cr.left, cr.top};
  ClientToScreen(hwnd, &client_origin);
  
  HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
  MONITORINFO mi{}; mi.cbSize = sizeof(mi); GetMonitorInfo(hmon, &mi);
  
  const int wnd_w = wr.right - wr.left;
  const int wnd_h = wr.bottom - wr.top;
  const int client_w = cr.right - cr.left;
  const int client_h = cr.bottom - cr.top;
  const int mon_w = mi.rcMonitor.right - mi.rcMonitor.left;
  const int mon_h = mi.rcMonitor.bottom - mi.rcMonitor.top;

  UINT dpi = 0;
#if defined(_WIN32)
  if (auto pGetDpiForWindow = reinterpret_cast<UINT (WINAPI*)(HWND)>(GetProcAddress(GetModuleHandleW(L"user32"), "GetDpiForWindow"))) {
    dpi = pGetDpiForWindow(hwnd);
  }
#endif

  LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
  LONG_PTR ex_style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
  const bool is_borderless = IsBorderlessStyleBits(style);
  const bool is_layered = (ex_style & WS_EX_LAYERED) != 0;
  const bool is_topmost = (ex_style & WS_EX_TOPMOST) != 0;
  const bool is_zoomed = IsZoomed(hwnd) != FALSE;
  const bool is_iconic = IsIconic(hwnd) != FALSE;

  {
    std::ostringstream oss;
    oss << "IF Conditions: wnd=" << wnd_w << "x" << wnd_h
        << " @(" << wr.left << "," << wr.top << ")"
        << ", client=" << client_w << "x" << client_h
        << " @(" << client_origin.x << "," << client_origin.y << ")"
        << ", mon=" << mon_w << "x" << mon_h
        << " @(" << mi.rcMonitor.left << "," << mi.rcMonitor.top << ")"
        << ", dpi=" << dpi
        << ", borderless=" << (is_borderless ? 1 : 0)
        << ", layered=" << (is_layered ? 1 : 0)
        << ", topmost=" << (is_topmost ? 1 : 0)
        << ", zoomed=" << (is_zoomed ? 1 : 0)
        << ", iconic=" << (is_iconic ? 1 : 0);
    LogInfo(oss.str().c_str());
  }

  // Size equality checks
  {
    const int bb_w = g_last_backbuffer_width.load();
    const int bb_h = g_last_backbuffer_height.load();
    const bool want_flip_model = true;
    const bool want_buffers_ge_2 = true;
    const bool want_bb_eq_mon = (bb_w == mon_w && bb_h == mon_h);
    // Check if client area (rendering area) matches monitor size
    const bool want_client_eq_mon = (client_w == mon_w && client_h == mon_h);
    // Check if client area is at the monitor origin (this is what matters for IF)
    const bool want_client_at_origin = (client_origin.x == mi.rcMonitor.left && client_origin.y == mi.rcMonitor.top);
    const bool want_borderless = true; // IF does not require window decorations to be removed; borderless=0 is desired
    const bool want_not_layered = true;
    const bool want_not_topmost = true;
    const bool want_not_zoomed = true;
    const bool want_not_iconic = true;

    std::ostringstream ok;
    ok << "IF Desired: flip_model=1, buffers>=2, bb==mon=1, client==mon=1, client@(0,0)=1, borderless=1, layered=0, topmost=0, zoomed=0, iconic=0";
    LogInfo(ok.str().c_str());

    // Validate and log errors for mismatches and track failures
    auto log_err = [](const char* msg) { LogWarn(msg); };
    if (!want_bb_eq_mon) {
      log_err("IF Error: backbuffer size does not match monitor");
      failures->backbuffer_size_mismatch.store(true);
    }
    if (!want_client_eq_mon) {
      log_err("IF Error: client area size does not match monitor");
      failures->window_size_mismatch.store(true);
    }
    if (!want_client_at_origin) {
      log_err("IF Error: client area not at monitor origin (0,0)");
      failures->window_not_at_origin.store(true);
    }
    if (is_borderless != want_borderless) log_err("IF Error: borderless should be 0");
    if (is_layered && want_not_layered) {
      log_err("IF Error: WS_EX_LAYERED is set");
      failures->window_layered.store(true);
    }
    if (is_topmost && want_not_topmost) {
      log_err("IF Error: WS_EX_TOPMOST is set");
      failures->window_topmost.store(true);
    }
    if (is_zoomed && want_not_zoomed) {
      log_err("IF Error: window is maximized");
      failures->window_maximized.store(true);
    }
    if (is_iconic && want_not_iconic) {
      log_err("IF Error: window is minimized");
      failures->window_minimized.store(true);
    }
  }
}
