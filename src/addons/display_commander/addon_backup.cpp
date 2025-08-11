
#include "addon.hpp"

// Global variable definitions
float s_auto_apply_enabled = 0.f; // 0 off, 1 on
float s_auto_apply_delay_sec = 10.f; // 1..60 seconds (app start delay)
float s_auto_apply_init_delay_sec = 1.f; // 1..60 seconds (swapchain init delay)

// Selected numeric windowed size
float s_windowed_width = 1280.f;
float s_windowed_height = 720.f;
// Current window position baseline
float s_windowed_pos_x = 100.f;
float s_windowed_pos_y = 100.f;

// Style preference
float s_remove_top_bar = 1.f; // 0 keep title bar, 1 remove (borderless)
// Resize mode: 0 = width/height, 1 = aspect
float s_resize_mode = 0.f;
// Aspect selection index when in aspect mode
float s_aspect_index = 3.f; // default to 16:9 (now at index 3 after sorting)
// Window alignment when repositioning is needed
float s_move_to_zero_if_out = 1.f; // 0 = None, 1 = Center, 2 = Top Left, 3 = Top Right, 4 = Bottom Left, 5 = Bottom Right
// Global swapchain behaviors
float s_force_borderless = 0.f;
float s_prevent_fullscreen = 1.f;

// Audio: mute when in background
float s_mute_in_background = 0.f;
// Audio: master volume (0..100) and manual mute toggle
float s_audio_volume_percent = 100.f;
float s_audio_mute = 0.f;
// Performance: background FPS cap
float s_fps_limit_background = 30.f;
// Monitor targeting (0 = Auto/current). Values > 0 map to enumerated monitors.
float s_target_monitor_index = 0.f;
// UI: DXGI composition state (0 Unknown, 1 Composed, 2 Overlay, 3 IndependentFlip)
float s_dxgi_composition_state = 0.f;
std::atomic<int> g_comp_query_counter{0};
std::atomic<int> g_comp_last_logged{-1};
// Try to configure swapchain for Independent Flip using DXGI-only changes
float s_try_independent_flip = 0.f;

// Prevent minimize (workaround for full solution)
float s_prevent_minimize = 0.f;

// Track frames since last minimize restore to prevent excessive calls
std::atomic<int> g_frames_since_last_restore{0};

// Colorspace override - forces specific DXGI colorspace for swapchain
float s_colorspace_override = 0.f; // 0 = None, 1+ = specific colorspace index

std::atomic<reshade::api::swapchain*> g_last_swapchain_ptr{nullptr};

// No persistent window handle; resolve on demand

// Implementation of IndependentFlipFailures::reset()
void IndependentFlipFailures::reset() {
    swapchain_null.store(false);
    device_null.store(false);
    non_dxgi_api.store(false);
    swapchain_media_failed.store(false);
    frame_stats_failed.store(false);
    not_flip_model.store(false);
    backbuffer_size_mismatch.store(false);
    window_size_mismatch.store(false);
    window_not_at_origin.store(false);
    window_layered.store(false);
    window_topmost.store(false);
    window_maximized.store(false);
    window_minimized.store(false);
    hwnd_null.store(false);
  }

std::atomic<IndependentFlipFailures*> g_if_failures{nullptr};

// Additional global variable definitions
std::atomic<uint64_t> g_init_apply_generation{0};
std::chrono::steady_clock::time_point g_attach_time;
std::atomic<HWND> g_last_swapchain_hwnd{nullptr};
std::atomic<bool> g_shutdown{false};
std::atomic<bool> g_muted_applied{false};
std::atomic<float> g_default_fps_limit{0.0f};

std::vector<MonitorInfo> g_monitors;

// Constants moved to utils.cpp

// Forward decl
void ComputeDesiredSize(int& out_w, int& out_h);

// RectFromWH is now defined in the header file

// IsExclusiveFullscreen moved to utils.cpp

// ComputeSWPFlags moved to utils.cpp

// Logging functions moved to utils.cpp

// Borderless functions moved to utils.cpp

// Global variable definitions
std::atomic<int> g_last_backbuffer_width{0};
std::atomic<int> g_last_backbuffer_height{0};

// DxgiBypassMode is now defined in the header file

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

// RemoveWindowBorder moved to utils.cpp

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

// Borderless functions moved to utils.cpp

// MonitorInfo is now defined in the header file
// g_monitors is now defined at the top of the file

// MonitorEnumProc moved to utils.cpp

std::vector<std::string> MakeMonitorLabels();

// WindowStyleMode is now defined in the header file

void ApplyWindowChange(HWND hwnd,
                       bool do_resize, int client_width, int client_height,
                       bool do_move, int pos_x, int pos_y,
                       WindowStyleMode style_mode) {
  if (hwnd == nullptr) return;

  // If window is maximized, restore it so size/style changes take effect
  const bool was_maximized = IsZoomed(hwnd) != FALSE;
  if (was_maximized) {
    ShowWindow(hwnd, SW_RESTORE);
  }

  LONG_PTR current_style = GetWindowLongPtrW(hwnd, GWL_STYLE);
  LONG_PTR current_ex_style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

  LONG_PTR new_style = current_style;
  LONG_PTR new_ex_style = current_ex_style;

  bool style_changed = false;
  switch (style_mode) {
    case WindowStyleMode::KEEP:
      break;
    case WindowStyleMode::BORDERLESS: {
      // Use the RemoveWindowBorderLocal function directly
      RemoveWindowBorderLocal(hwnd);
      style_changed = false; // Always mark as changed since RemoveWindowBorderLocal handles the logic
      break;
    }
    case WindowStyleMode::OVERLAPPED_WINDOW: {
      new_style &= ~WS_POPUP;
      new_style |= WS_OVERLAPPEDWINDOW;
      new_ex_style &= ~WS_EX_TOPMOST;
      style_changed = (new_style != current_style) || (new_ex_style != current_ex_style);
      if (style_changed) {
        SetWindowLongPtrW(hwnd, GWL_STYLE, new_style);
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, new_ex_style);
      }
      break;
    }
  }

  RECT wr_current{};
  GetWindowRect(hwnd, &wr_current);

  // Resolve target monitor
  HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
  MONITORINFOEXW mi{};
  mi.cbSize = sizeof(mi);
  if (s_target_monitor_index > 0.5f) {
    // Use enumerated monitor if available
    const int index = static_cast<int>(s_target_monitor_index) - 1; // labels start at 1
    g_monitors.clear();
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&g_monitors));
    if (index >= 0 && index < static_cast<int>(g_monitors.size())) {
      hmon = g_monitors[index].handle;
      mi = g_monitors[index].info;
    }
  }
  if (mi.cbSize != sizeof(mi)) {
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(hmon, &mi);
  }

  int target_w = 0;
  int target_h = 0;
  int target_x = 0;
  int target_y = 0;

  if (do_resize) {
    RECT client_rect = RectFromWH(client_width, client_height);
    if (AdjustWindowRectEx(&client_rect, static_cast<DWORD>(new_style), FALSE, static_cast<DWORD>(new_ex_style)) == FALSE) {
      LogWarn("AdjustWindowRectEx failed for ApplyWindowChange.");
      return;
    }
    target_w = client_rect.right - client_rect.left;
    target_h = client_rect.bottom - client_rect.top;
  }

  if (do_move) {
    // If targeting a specific monitor, offset desired pos by monitor origin
    target_x = pos_x + mi.rcMonitor.left;
    target_y = pos_y + mi.rcMonitor.top;
  } else {
    // Keep current placement
    target_x = wr_current.left;
    target_y = wr_current.top;
  }

  // Ensure we have dimensions to check bounds when not resizing
  if (!do_resize) {
    target_w = wr_current.right - wr_current.left;
    target_h = wr_current.bottom - wr_current.top;
  }

  bool repositioned = false;
  bool effective_do_move = do_move;
  if (s_move_to_zero_if_out > 0) { // 0 = None, 1+ = different alignment options
    // First check against chosen monitor
    const RECT& mr = mi.rcMonitor;
    bool out_of_bounds = (target_x < mr.left) || (target_y < mr.top) ||
                         (target_x + target_w > mr.right) || (target_y + target_h > mr.bottom);
    if (out_of_bounds) {
      // Apply alignment based on setting value
      switch (static_cast<int>(s_move_to_zero_if_out)) {
        case 1: // Center
          target_x = mr.left + (mr.right - mr.left - target_w) / 2;
          target_y = mr.top + (mr.bottom - mr.top - target_h) / 2;
          break;
        case 2: // Top Left
          target_x = mr.left;
          target_y = mr.top;
          break;
        case 3: // Top Right
          target_x = mr.right - target_w;
          target_y = mr.top;
          break;
        case 4: // Bottom Left
          target_x = mr.left;
          target_y = mr.bottom - target_h;
          break;
        case 5: // Bottom Right
          target_x = mr.right - target_w;
          target_y = mr.bottom - target_h;
          break;
        default: // Fallback to top left
          target_x = mr.left;
          target_y = mr.top;
          break;
      }
      repositioned = true;
      effective_do_move = true;
    }
    // As a fallback, clamp to virtual screen if still invalid
    const int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    out_of_bounds = (target_x < vx) || (target_y < vy) ||
                    (target_x + target_w > vx + vw) || (target_y + target_h > vy + vh);
    if (out_of_bounds) {
      // Apply same alignment logic to virtual screen
      switch (static_cast<int>(s_move_to_zero_if_out)) {
        case 1: // Center
          target_x = vx + (vw - target_w) / 2;
          target_y = vy + (vh - target_h) / 2;
          break;
        case 2: // Top Left
          target_x = vx;
          target_y = vy;
          break;
        case 3: // Top Right
          target_x = vx + vw - target_w;
          target_y = vy;
          break;
        case 4: // Bottom Left
          target_x = vx;
          target_y = vy + vh - target_h;
          break;
        case 5: // Bottom Right
          target_x = vx + vw - target_w;
          target_y = vy + vh - target_h;
          break;
        default: // Fallback to top left
          target_x = vx;
          target_y = vy;
          break;
      }
      repositioned = true;
      effective_do_move = true;
    }
  }

  UINT flags = ComputeSWPFlags(hwnd, style_changed);
  if (!do_resize) { flags |= SWP_NOSIZE; }
  if (!effective_do_move) { flags |= SWP_NOMOVE; }

  {
    std::ostringstream log_oss;
    const char* style_mode_str = "";
    if (style_mode == WindowStyleMode::KEEP) {
      style_mode_str = "Keep";
    } else if (style_mode == WindowStyleMode::BORDERLESS) {
      style_mode_str = "Borderless";
    } else {
      style_mode_str = "OverlappedWindow";
    }
    log_oss << "ApplyWindowChange hwnd=" << hwnd
            << " style_mode=" << style_mode_str
            << " do_resize=" << (do_resize ? "1" : "0")
            << " client=" << client_width << "x" << client_height
            << " do_move=" << (do_move ? "1" : "0")
            << " pos=" << target_x << "," << target_y
            << " flags=0x" << std::hex << flags
            << std::dec
            << " was_maximized=" << (was_maximized ? "1" : "0")
            << " style_changed=" << (style_changed ? "1" : "0")
            << " repositioned=" << (repositioned ? "1" : "0");
    LogInfo(log_oss.str().c_str());
  }

  SetWindowPos(
      hwnd, nullptr,
      target_x, target_y,
      do_resize ? target_w : 0,
      do_resize ? target_h : 0,
      flags);
}

// No desktop mode changes in this addon per requirements

// Fullscreen detection/exit removed

// Auto-apply on swapchain init scheduler
// Global variables are now defined at the top of the file

bool SetMuteForCurrentProcess(bool mute) {
  const DWORD target_pid = GetCurrentProcessId();
  HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  const bool did_init = SUCCEEDED(hr);
  if (!did_init && hr != RPC_E_CHANGED_MODE) {
    LogWarn("CoInitializeEx failed for audio mute control");
    return false;
  }

  bool success = false;
  IMMDeviceEnumerator* device_enumerator = nullptr;
  IMMDevice* device = nullptr;
  IAudioSessionManager2* session_manager = nullptr;
  IAudioSessionEnumerator* session_enumerator = nullptr;

  do {
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&device_enumerator));
    if (FAILED(hr) || device_enumerator == nullptr) break;
    hr = device_enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device);
    if (FAILED(hr) || device == nullptr) break;
    hr = device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&session_manager));
    if (FAILED(hr) || session_manager == nullptr) break;
    hr = session_manager->GetSessionEnumerator(&session_enumerator);
    if (FAILED(hr) || session_enumerator == nullptr) break;
    int count = 0; session_enumerator->GetCount(&count);
    for (int i = 0; i < count; ++i) {
      IAudioSessionControl* session_control = nullptr;
      if (FAILED(session_enumerator->GetSession(i, &session_control)) || session_control == nullptr) continue;
      IAudioSessionControl2* session_control2 = nullptr;
      if (SUCCEEDED(session_control->QueryInterface(&session_control2)) && session_control2 != nullptr) {
        DWORD pid = 0; session_control2->GetProcessId(&pid);
        if (pid == target_pid) {
          ISimpleAudioVolume* simple_volume = nullptr;
          if (SUCCEEDED(session_control->QueryInterface(&simple_volume)) && simple_volume != nullptr) {
            simple_volume->SetMute(mute ? TRUE : FALSE, nullptr);
            simple_volume->Release();
            success = true;
          }
        }
        session_control2->Release();
      }
      session_control->Release();
    }
  } while(false);

  if (session_enumerator != nullptr) session_enumerator->Release();
  if (session_manager != nullptr) session_manager->Release();
  if (device != nullptr) device->Release();
  if (device_enumerator != nullptr) device_enumerator->Release();
  if (did_init && hr != RPC_E_CHANGED_MODE) CoUninitialize();

  std::ostringstream oss; oss << "BackgroundMute apply mute=" << (mute ? "1" : "0") << " success=" << (success ? "1" : "0");
  LogInfo(oss.str().c_str());
  return success;
}

bool SetVolumeForCurrentProcess(float volume_0_100) {
  float clamped = (std::max)(0.0f, (std::min)(volume_0_100, 100.0f));
  const float scalar = clamped / 100.0f;
  const DWORD target_pid = GetCurrentProcessId();
  HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  const bool did_init = SUCCEEDED(hr);
  if (!did_init && hr != RPC_E_CHANGED_MODE) {
    LogWarn("CoInitializeEx failed for audio volume control");
    return false;
  }

  bool success = false;
  IMMDeviceEnumerator* device_enumerator = nullptr;
  IMMDevice* device = nullptr;
  IAudioSessionManager2* session_manager = nullptr;
  IAudioSessionEnumerator* session_enumerator = nullptr;

  do {
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&device_enumerator));
    if (FAILED(hr) || device_enumerator == nullptr) break;
    hr = device_enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device);
    if (FAILED(hr) || device == nullptr) break;
    hr = device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&session_manager));
    if (FAILED(hr) || session_manager == nullptr) break;
    hr = session_manager->GetSessionEnumerator(&session_enumerator);
    if (FAILED(hr) || session_enumerator == nullptr) break;
    int count = 0; session_enumerator->GetCount(&count);
    for (int i = 0; i < count; ++i) {
      IAudioSessionControl* session_control = nullptr;
      if (FAILED(session_enumerator->GetSession(i, &session_control)) || session_control == nullptr) continue;
      IAudioSessionControl2* session_control2 = nullptr;
      if (SUCCEEDED(session_control->QueryInterface(&session_control2)) && session_control2 != nullptr) {
        DWORD pid = 0; session_control2->GetProcessId(&pid);
        if (pid == target_pid) {
          ISimpleAudioVolume* simple_volume = nullptr;
          if (SUCCEEDED(session_control->QueryInterface(&simple_volume)) && simple_volume != nullptr) {
            simple_volume->SetMasterVolume(scalar, nullptr);
            simple_volume->Release();
            success = true;
          }
        }
        session_control2->Release();
      }
      session_control->Release();
    }
  } while(false);

  if (session_enumerator != nullptr) session_enumerator->Release();
  if (session_manager != nullptr) session_manager->Release();
  if (device != nullptr) device->Release();
  if (device_enumerator != nullptr) device_enumerator->Release();
  if (did_init && hr != RPC_E_CHANGED_MODE) CoUninitialize();

  std::ostringstream oss; oss << "BackgroundVolume set percent=" << clamped << " success=" << (success ? "1" : "0");
  LogInfo(oss.str().c_str());
  return success;
}

void RunBackgroundAudioMonitor() {
  while (!g_shutdown.load()) {
    bool want_mute = false;
    if (s_mute_in_background >= 0.5f) {
      HWND hwnd = g_last_swapchain_hwnd.load();
      if (hwnd == nullptr) hwnd = GetForegroundWindow();
      HWND fg = GetForegroundWindow();
      want_mute = (hwnd != nullptr && fg != hwnd);
    }

    const bool applied = g_muted_applied.load();
    if (want_mute != applied) {
      if (SetMuteForCurrentProcess(want_mute)) {
        g_muted_applied.store(want_mute);
      }
    }

    // Background FPS limit handling (foreground uses the default FPSLimit)
    if (s_fps_limit_background >= 0.f) {
      HWND hwnd = g_last_swapchain_hwnd.load();
      if (hwnd == nullptr) hwnd = GetForegroundWindow();
      HWND fg = GetForegroundWindow();
      const bool is_background = (hwnd != nullptr && fg != hwnd);
      if (is_background) {
        const float desired_limit = s_fps_limit_background;
        if (renodx::utils::swapchain::fps_limit != desired_limit) {
          renodx::utils::swapchain::fps_limit = desired_limit;
        }
      } else {
        // Foreground: if the default is zero, ensure no FPS limit
        if (g_default_fps_limit.load() == 0.0f) {
          if (renodx::utils::swapchain::fps_limit != 0.0f) {
            renodx::utils::swapchain::fps_limit = 0.0f;
          }
        } else {
          if (renodx::utils::swapchain::fps_limit != g_default_fps_limit.load()) {
            renodx::utils::swapchain::fps_limit = g_default_fps_limit.load();
          }
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
  }
}

// ForceWindowToMonitorOrigin functions moved to utils.cpp

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


bool ShouldApplyWindowedForBackbuffer(int desired_w, int desired_h) {
  const int bb_w = g_last_backbuffer_width.load();
  const int bb_h = g_last_backbuffer_height.load();
  std::ostringstream oss;
  oss << "Backbuffer check (desired=" << desired_w << "x" << desired_h
      << ", current=" << bb_w << "x" << bb_h << ")";
  LogDebug(oss.str());
  if (bb_w <= 0 || bb_h <= 0) return true; // Unknown → apply
  return bb_w != desired_w || bb_h != desired_h;
}

void ScheduleAutoApplyOnInit(HWND hwnd) {
  if (s_auto_apply_enabled < 0.5f) return;
  const uint64_t my_gen = ++g_init_apply_generation;
  // Compute absolute target time: max(app_start + X, init_time + Y)
  const auto now = std::chrono::steady_clock::now();
  auto t_app = g_attach_time + std::chrono::seconds(static_cast<int>(s_auto_apply_delay_sec));
  auto t_init = now + std::chrono::seconds(static_cast<int>(s_auto_apply_init_delay_sec));
  auto target = (t_app > t_init) ? t_app : t_init;
  int delay_ms = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(target - now).count());
  delay_ms = (std::max)(delay_ms, 0);

  // Capture current desired size at schedule time
  int want_w = 0; int want_h = 0; ComputeDesiredSize(want_w, want_h);
  const int target_pos_x = static_cast<int>(s_windowed_pos_x);
  const int target_pos_y = static_cast<int>(s_windowed_pos_y);
  const WindowStyleMode mode = (s_remove_top_bar >= 0.5f) ? WindowStyleMode::BORDERLESS : WindowStyleMode::OVERLAPPED_WINDOW;
  std::thread([my_gen, delay_ms, hwnd, want_w, want_h, target_pos_x, target_pos_y, mode]() {
    if (delay_ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    if (g_init_apply_generation.load() != my_gen) return;
    if (hwnd == nullptr) return;
    std::ostringstream oss; oss << "AutoApply on init after " << delay_ms << "ms: size=" << want_w << "x" << want_h;
    LogDebug(oss.str());
    const int bb_w = g_last_backbuffer_width.load();
    const int bb_h = g_last_backbuffer_height.load();
    // Apply if style needs change or size differs
    const bool size_matches = (bb_w == want_w && bb_h == want_h);
    const bool style_matches = (mode != WindowStyleMode::BORDERLESS) || IsBorderless(hwnd);
    if (size_matches && style_matches) {
      LogDebug("AutoApply skipped: backbuffer and style match desired state");
      return;
    }
    ApplyWindowChange(hwnd,
                      /*do_resize=*/true, want_w, want_h,
                      /*do_move=*/true, target_pos_x, target_pos_y,
                      mode);
  }).detach();
}

// Utility functions moved to utils.cpp

// ComputeDesiredSize functions moved to utils.cpp

// MakeMonitorLabels moved to utils.cpp

// UI/settings
renodx::utils::settings::Settings settings = {
    // Auto-apply toggle and delay (seconds)
    new renodx::utils::settings::Setting{
        .key = "AutoApply",
        .binding = &s_auto_apply_enabled,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Auto Apply",
        .section = "Display",
        .labels = {"Off", "On"},
    },
    new renodx::utils::settings::Setting{
        .key = "AutoApplyDelay",
        .binding = &s_auto_apply_delay_sec,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 10.f,
        .label = "App Start Delay (s)",
        .section = "Display",
        .min = 1.f,
        .max = 60.f,
        .format = "%d s",
        .is_enabled = [](){ return s_auto_apply_enabled >= 0.5f; },
    },
    new renodx::utils::settings::Setting{
        .key = "InitApplyDelay",
        .binding = &s_auto_apply_init_delay_sec,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 1.f,
        .label = "SwapChain Init Delay (s)",
        .section = "Display",
        .min = 1.f,
        .max = 60.f,
        .format = "%d s",
        .is_enabled = [](){ return s_auto_apply_enabled >= 0.5f; },
    },
    // Window width preset slider with labels
    new renodx::utils::settings::Setting{
        .key = "WindowWidth",
        .binding = &s_windowed_width,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 3.f, // default to 1920
        .label = "Window Width",
        .section = "Display",
        .labels = MakeLabels(WIDTH_OPTIONS, 7),
        .parse = [](float index) {
          int i = static_cast<int>(index);
          i = (std::max)(i, 0);
          int max_i = 6;
          i = (std::min)(i, max_i);
          return static_cast<float>(WIDTH_OPTIONS[i]);
        },
    },
    // Window height preset slider with labels
    new renodx::utils::settings::Setting{
        .key = "WindowHeight",
        .binding = &s_windowed_height,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 2.f, // default to 1080
        .label = "Window Height",
        .section = "Display",
        .labels = MakeLabels(HEIGHT_OPTIONS, 7),
        .is_enabled = [](){ return s_resize_mode < 0.5f; },
        .parse = [](float index) {
          int i = static_cast<int>(index);
          i = (std::max)(i, 0);
          int max_i = 6;
          i = (std::min)(i, max_i);
          return static_cast<float>(HEIGHT_OPTIONS[i]);
        },
    },
    // Resize Mode
    new renodx::utils::settings::Setting{
        .key = "ResizeMode",
        .binding = &s_resize_mode,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Resize Mode",
        .section = "Display",
        .labels = {"Width/Height", "Aspect"},
    },
    // Aspect Ratio (only when in Aspect mode)
    new renodx::utils::settings::Setting{
        .key = "AspectRatio",
        .binding = &s_aspect_index,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 3.f, // 16:9 (now at index 3 after sorting)
        .label = "Aspect Ratio",
        .section = "Display",
        .labels = MakeAspectLabels(),
        .is_enabled = [](){ return s_resize_mode >= 0.5f; },
    },

    // FPS Limit (foreground default)
    new renodx::utils::settings::Setting{
        .key = "FPSLimit",
        .binding = &renodx::utils::swapchain::fps_limit,
        .default_value = 0.f,
        .label = "FPS Limit",
        .section = "Performance",
        .min = 0.f,
        .max = 480.f,
        .format = "%.0f FPS",
        .on_change_value = [](float previous, float current){ g_default_fps_limit.store(current); },
    },
    // Background FPS Limit
    new renodx::utils::settings::Setting{
        .key = "FPSLimitBackground",
        .binding = &s_fps_limit_background,
        .default_value = 30.f,
        .label = "Background FPS Limit",
        .section = "Performance",
        .tooltip = "FPS cap when the game window is not in the foreground.",
        .min = 0.f,
        .max = 240.f,
        .format = "%.0f FPS",
    },
    // Volume (0-100)
    new renodx::utils::settings::Setting{
        .key = "Volume",
        .binding = &s_audio_volume_percent,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 100.f,
        .label = "Volume",
        .section = "Audio",
        .min = 0.f,
        .max = 100.f,
        .format = "%d%%",
        .on_change_value = [](float previous, float current){ SetVolumeForCurrentProcess(current); },
    },
    // Mute (manual)
    new renodx::utils::settings::Setting{
        .key = "Mute",
        .binding = &s_audio_mute,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Mute",
        .section = "Audio",
        .labels = {"Off", "On"},
        .on_change_value = [](float previous, float current){ SetMuteForCurrentProcess(current >= 0.5f); },
    },
    // Mute in Background (placed after Mute; disabled if Mute is ON)
    new renodx::utils::settings::Setting{
        .key = "MuteInBackground",
        .binding = &s_mute_in_background,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Mute In Background",
        .section = "Audio",
        .tooltip = "Mute the game's audio when it is not the foreground window.",
        .labels = {"Off", "On"},
        .is_enabled = [](){ return s_audio_mute < 0.5f; },
        .on_change_value = [](float previous, float current){
          // Reset applied flag so the monitor thread reapplies desired state
          g_muted_applied.store(false);
        },
    },
    // Apply button executes windowed resize
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Apply",
        .section = "Display",
        .tooltip = "Apply the selected window width and height (respects Remove Top Bar).",
        .on_click = []() {
          std::thread([](){
            HWND hwnd = g_last_swapchain_hwnd.load();
            if (hwnd == nullptr) hwnd = GetForegroundWindow();
            if (hwnd == nullptr) return;
            DWORD owner_tid = GetWindowThreadProcessId(hwnd, nullptr);
            DWORD this_tid = GetCurrentThreadId();
            RECT wr{};
            int pos_x = 100;
            int pos_y = 100;
            if (GetWindowRect(hwnd, &wr) != FALSE) {
              pos_x = wr.left;
              pos_y = wr.top;
            }
            WindowStyleMode mode = (s_remove_top_bar >= 0.5f) ? WindowStyleMode::BORDERLESS : WindowStyleMode::OVERLAPPED_WINDOW;
            int want_w = 0; int want_h = 0; ComputeDesiredSize(want_w, want_h);
            
            std::ostringstream oss; oss << "Apply (bg) clicked hwnd=" << hwnd
                                        << " owner_tid=" << owner_tid
                                        << " this_tid=" << this_tid
                                        << " size=" << want_w << "x" << want_h;
            LogDebug(oss.str());
            ApplyWindowChange(
                hwnd,
                /*do_resize=*/true, want_w, want_h,
                /*do_move=*/true, pos_x, pos_y,
                mode);
          }).detach();
          return false; // do not trigger settings save
        },
    },
    // Remove Top Bar (for Force Windowed)
    new renodx::utils::settings::Setting{
        .key = "RemoveTopBar",
        .binding = &s_remove_top_bar,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 1.f,
        .label = "Remove Top Bar",
        .section = "Display",
        .tooltip = "When forcing windowed mode, choose whether to remove the title bar (borderless).",
        .labels = {"Keep", "Remove"},
    },
    // Target Monitor
    new renodx::utils::settings::Setting{
        .key = "TargetMonitor",
        .binding = &s_target_monitor_index,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Target Monitor",
        .section = "Display",
        .tooltip = "Choose which monitor to apply size/pos to. 'Auto' uses the current window monitor.",
        .labels = MakeMonitorLabels(),
    },
    // Window alignment when repositioning is needed
    new renodx::utils::settings::Setting{
        .key = "Alignment",
        .binding = &s_move_to_zero_if_out,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 1.f, // default to center
        .label = "Alignment",
        .section = "Display",
        .tooltip = "Choose how to align the window when repositioning is needed. TL=Top Left, TR=Top Right, BL=Bottom Left, BR=Bottom Right.",
        .labels = {"None", "Center", "Top Left", "Top Right", "Bottom Left", "Bottom Right"},
    },
    // Force Borderless (global)
    new renodx::utils::settings::Setting{
        .key = "ForceBorderless",
        .binding = &s_force_borderless,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Force Borderless",
        .section = "Display",
        .tooltip = "Force borderless presentation when game requests fullscreen.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current){ renodx::mods::swapchain::force_borderless = (current >= 0.5f); },
    },
    // Prevent Fullscreen (global)
    new renodx::utils::settings::Setting{
        .key = "PreventFullscreen",
        .binding = &s_prevent_fullscreen,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 1.f,
        .label = "Prevent Fullscreen",
        .section = "Display",
        .tooltip = "Prevent exclusive fullscreen; keep borderless/windowed for stability and HDR.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current){ renodx::mods::swapchain::prevent_full_screen = (current >= 0.5f); },
    },
    // Prevent Minimize (workaround)
    new renodx::utils::settings::Setting{
        .key = "PreventMinimize",
        .binding = &s_prevent_minimize,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Prevent Minimize (workaround)",
        .section = "Display",
        .tooltip = "Prevent window from being minimized by checking every 1 second and restoring if needed. This is a workaround for a full solution.",
        .labels = {"Disabled", "Enabled"},
    },
    // Colorspace Override
    new renodx::utils::settings::Setting{
        .key = "ColorspaceOverride",
        .binding = &s_colorspace_override,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Colorspace Override (requires restart)",
        .section = "Display",
        .tooltip = "Force specific DXGI colorspace for swapchain. Useful for HDR, wide gamut, and color accuracy.",
        .labels = {"None", "BT709 Full", "BT2020 Full", "HDR10", "BT709 Studio", "BT2020 Studio", "HDR10 Studio"},
    },
    // Force Windowed (Experimental)
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Force Windowed (Experimental)",
        .section = "Display",
        .tooltip = "Attempt to force overlapped window styles and then apply size.",
        .on_click = [](){
          std::thread([](){
            HWND hwnd = g_last_swapchain_hwnd.load();
            if (hwnd == nullptr) hwnd = GetForegroundWindow();
            if (hwnd == nullptr) return;
            LogDebug("Force Windowed button pressed (bg thread)");
            // Only one call: change style per setting, keep current size/pos
            WindowStyleMode mode = (s_remove_top_bar >= 0.5f) ? WindowStyleMode::BORDERLESS : WindowStyleMode::OVERLAPPED_WINDOW;
            ApplyWindowChange(
                hwnd,
                /*do_resize=*/false, 0, 0,
                /*do_move=*/false, 0, 0,
                mode);
          }).detach();
          return false; // do not save on button click
        },
    },

    // Try Independent Flip (Experimental)
    new renodx::utils::settings::Setting{
        .key = "TryIndependentFlip",
        .binding = &s_try_independent_flip,
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .default_value = 0.f,
        .label = "Experimental: Try Fixing Independent Flip",
        .section = "Performance",
        .tooltip = "Attempt to configure swapchain for Independent Flip using DXGI-only changes (no window ops).",
                                  .on_click = [](){
                            auto* sc = g_last_swapchain_ptr.load();
                            if (sc != nullptr) {
                              SetIndependentFlipState(sc);
                              LogIndependentFlipConditions(sc);
    } else {
                              LogWarn("Try Independent Flip: no swapchain yet");
                            }
                            return false;
                          },
                        },
                        new renodx::utils::settings::Setting{
                          .key = "ForceWindowToOrigin",
                          .binding = nullptr,
                          .value_type = renodx::utils::settings::SettingValueType::BUTTON,
                          .default_value = 0.f,
                          .label = "Force Window to Monitor Origin (experimental)",
                          .section = "Performance",
                          .tooltip = "Force the window to be positioned so its client area starts at monitor origin (0,0). Required for Independent Flip.",
                          .on_click = [](){
                            HWND hwnd = g_last_swapchain_hwnd.load();
                            if (hwnd != nullptr) {
                              ForceWindowToMonitorOriginThreaded(hwnd);
                            } else {
                              LogWarn("Force Window to Origin: no window handle yet");
                            }
                            return false;
                          },
    },
    // DXGI composition/backbuffer info (text only) — placed at bottom
    new renodx::utils::settings::Setting{
        .key = "DxgiInfo",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "",
        .section = "Performance",
        .tooltip = "DXGI composition mode, current backbuffer size, and fullscreen status.",
        .on_draw = [](){
          const char* mode_str = "Unknown";
          switch (static_cast<int>(s_dxgi_composition_state)) {
            case 1: mode_str = "Composed Flip"; break;
            case 2: mode_str = "Modern Independent Flip"; break;
            case 3: mode_str = "Legacy Independent Flip"; break;
            default: mode_str = "Unknown"; break;
          }
          
          // Check exclusive fullscreen status
          HWND hwnd = g_last_swapchain_hwnd.load();
          if (hwnd == nullptr) hwnd = GetForegroundWindow();
          const bool is_exclusive_fullscreen = IsExclusiveFullscreen(hwnd);
          
          // Get backbuffer format
          std::string format_str = "Unknown";
          if (g_last_swapchain_ptr.load() != nullptr) {
            auto* device = g_last_swapchain_ptr.load()->get_device();
            if (device != nullptr && g_last_swapchain_ptr.load()->get_back_buffer_count() > 0) {
              auto bb = g_last_swapchain_ptr.load()->get_back_buffer(0);
              auto desc = device->get_resource_desc(bb);
              switch (desc.texture.format) {
                case reshade::api::format::r8g8b8a8_unorm: format_str = "R8G8B8A8_UNORM"; break;
                case reshade::api::format::r8g8b8a8_unorm_srgb: format_str = "R8G8B8A8_UNORM_SRGB"; break;
                case reshade::api::format::r10g10b10a2_unorm: format_str = "R10G10B10A2_UNORM"; break;
                case reshade::api::format::r16g16b16a16_float: format_str = "R16G16B16A16_FLOAT"; break;
                case reshade::api::format::r11g11b10_float: format_str = "R11G11B10_FLOAT"; break;
                case reshade::api::format::b8g8r8a8_unorm: format_str = "B8G8R8A8_UNORM"; break;
                case reshade::api::format::b8g8r8a8_unorm_srgb: format_str = "B8G8R8A8_UNORM_SRGB"; break;
                case reshade::api::format::r8g8b8_unorm: format_str = "R8G8B8_UNORM"; break;
                case reshade::api::format::r8g8b8_unorm_srgb: format_str = "R8G8B8_UNORM_SRGB"; break;
                case reshade::api::format::b5g6r5_unorm: format_str = "B5G6R5_UNORM"; break;
                case reshade::api::format::b5g5r5a1_unorm: format_str = "B5G5R5A1_UNORM"; break;
                case reshade::api::format::b4g4r4a4_unorm: format_str = "B4G4R4A4_UNORM"; break;
                default: format_str = "Format_" + std::to_string(static_cast<int>(desc.texture.format)); break;
              }
            }
          }
          
          // Display colorspace override status
          std::string colorspace_str = "None";
          if (s_colorspace_override > 0.f) {
            switch (static_cast<int>(s_colorspace_override)) {
              case 1: colorspace_str = "BT709 Full"; break;
              case 2: colorspace_str = "BT2020 Full"; break;
              case 3: colorspace_str = "HDR10"; break;
              case 4: colorspace_str = "BT709 Studio"; break;
              case 5: colorspace_str = "BT2020 Studio"; break;
              case 6: colorspace_str = "HDR10 Studio"; break;
              default: colorspace_str = "Unknown"; break;
            }
          }
          
          ImGui::Separator();
          ImGui::Text("DXGI Composition: %s | Backbuffer: %dx%d | Format: %s",
                      mode_str,
                      g_last_backbuffer_width.load(),
                      g_last_backbuffer_height.load(),
                      format_str.c_str());
          ImGui::Text("Fullscreen Mode: %s | Colorspace Override: %s", 
                      is_exclusive_fullscreen ? "Exclusive" : "Windowed/Borderless",
                      colorspace_str.c_str());
          
                                      // Display current window position and size
                            if (hwnd != nullptr) {
                              RECT wr{};
                              RECT cr{};
                              if (GetWindowRect(hwnd, &wr) != FALSE && GetClientRect(hwnd, &cr) != FALSE) {
                                int window_width = wr.right - wr.left;
                                int window_height = wr.bottom - wr.top;
                                int client_width = cr.right - cr.left;
                                int client_height = cr.bottom - cr.top;
                                
                                // Convert client coordinates to screen coordinates
                                POINT client_origin = {cr.left, cr.top};
                                ClientToScreen(hwnd, &client_origin);
                                
                                ImGui::Text("Window Frame: %dx%d at (%ld, %ld)", window_width, window_height, wr.left, wr.top);
                                ImGui::Text("Client Area: %dx%d at (%ld, %ld)", client_width, client_height, client_origin.x, client_origin.y);
                                ImGui::Text("Decorations: %dx%d offset", window_width - client_width, window_height - client_height);
                              }
                              
                              // Display current monitor resolution
                              HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                              if (hmon != nullptr) {
                                MONITORINFOEXW mi{};
                                mi.cbSize = sizeof(mi);
                                if (GetMonitorInfoW(hmon, &mi) != FALSE) {
                                  int monitor_width = mi.rcMonitor.right - mi.rcMonitor.left;
                                  int monitor_height = mi.rcMonitor.bottom - mi.rcMonitor.top;
                                  ImGui::Text("Monitor: %dx%d at (%ld, %ld)", 
                                             monitor_width, monitor_height, mi.rcMonitor.left, mi.rcMonitor.top);
                                }
                              }
                            }
          
          return false;
        },
    },
    // Independent Flip failure reasons (text only)
    new renodx::utils::settings::Setting{
        .key = "IndependentFlipFailures",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "",
        .section = "Performance",
        .tooltip = "Reasons why Independent Flip is not working.",
        .on_draw = [](){
          IndependentFlipFailures* failures = g_if_failures.load();
          if (failures == nullptr) return false;
          
          // Check if there are any failures
          bool has_failures = failures->swapchain_null.load() ||
                             failures->device_null.load() ||
                             failures->non_dxgi_api.load() ||
                             failures->swapchain_media_failed.load() ||
                             failures->frame_stats_failed.load() ||
                             failures->not_flip_model.load() ||
                             failures->backbuffer_size_mismatch.load() ||
                             failures->window_size_mismatch.load() ||
                             failures->window_not_at_origin.load() ||
                             failures->window_layered.load() ||
                             failures->window_topmost.load() ||
                             failures->window_maximized.load() ||
                             failures->window_minimized.load() ||
                             failures->hwnd_null.load();
          
          if (has_failures) {
            ImGui::Separator();
            ImGui::Text("Independent Flip Blocked:");
            
            if (failures->swapchain_null.load()) ImGui::BulletText("Swapchain is null");
            if (failures->device_null.load()) ImGui::BulletText("Device is null");
            if (failures->non_dxgi_api.load()) ImGui::BulletText("Non-DXGI API (OpenGL/Vulkan)");
            if (failures->swapchain_media_failed.load()) ImGui::BulletText("Swapchain media interface failed");
            if (failures->frame_stats_failed.load()) ImGui::BulletText("Frame statistics failed");
            if (failures->not_flip_model.load()) ImGui::BulletText("Not a flip-model swapchain");
            if (failures->backbuffer_size_mismatch.load()) ImGui::BulletText("Backbuffer size doesn't match monitor");
            if (failures->window_size_mismatch.load()) ImGui::BulletText("Window size doesn't match monitor");
            if (failures->window_not_at_origin.load()) ImGui::BulletText("Window not at monitor origin (0,0)");
            if (failures->window_layered.load()) ImGui::BulletText("Window has WS_EX_LAYERED style");
            if (failures->window_topmost.load()) ImGui::BulletText("Window has WS_EX_TOPMOST style");
            if (failures->window_maximized.load()) ImGui::BulletText("Window is maximized");
            if (failures->window_minimized.load()) ImGui::BulletText("Window is minimized");
            if (failures->hwnd_null.load()) ImGui::BulletText("Window handle is null");
          }
          
          return false;
        },
    },
    // Test button for RemoveWindowBorder
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Test RemoveWindowBorderLocal",
        .section = "Display",
        .tooltip = "Test button that calls RemoveWindowBorderLocal in another thread.",
        .on_click = []() {
          std::thread([](){
            HWND hwnd = g_last_swapchain_hwnd.load();
            if (hwnd == nullptr) hwnd = GetForegroundWindow();
            if (hwnd == nullptr) {
              LogWarn("Test RemoveWindowBorder: no window handle available");
              return;
            }
            LogDebug("Test RemoveWindowBorder button pressed (bg thread)");
            RemoveWindowBorderLocal(hwnd);
            LogInfo("Test RemoveWindowBorderLocal completed");
          }).detach();
          return false; // do not save on button click
        },
    },
};

// Namespace removed - all functions are now in global scope

extern "C" __declspec(dllexport) constexpr const char* NAME = "Resolution Override";
extern "C" __declspec(dllexport) constexpr const char* DESCRIPTION =
    "RenoDX Resolution/Window Override (Win32). Note: Applying window operations from the main thread can crash some apps; this addon performs them on a background thread.";

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
    
    // Apply colorspace override if enabled
    if (s_colorspace_override > 0.f) {
      try {
        // Get the underlying DXGI swapchain interface
        auto* dxgi_swapchain = static_cast<IDXGISwapChain3*>(reinterpret_cast<void*>(swapchain->get_native()));
        if (dxgi_swapchain != nullptr) {
          // Map setting index to DXGI colorspace
          DXGI_COLOR_SPACE_TYPE colorspace = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709; // Default
          
          switch (static_cast<int>(s_colorspace_override)) {
            case 1: colorspace = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709; break;      // BT709 Full
            case 2: colorspace = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P2020; break;     // BT2020 Full
            case 3: colorspace = DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020; break;   // HDR10
            case 4: colorspace = DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P709; break;    // BT709 Studio
            case 5: colorspace = DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P2020; break;   // BT2020 Studio
            case 6: colorspace = DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020; break; // HDR10 Studio
            default: colorspace = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709; break;
          }
          
          HRESULT hr = dxgi_swapchain->SetColorSpace1(colorspace);
          if (SUCCEEDED(hr)) {
            std::ostringstream oss2; oss2 << "Colorspace Override: Applied colorspace " << static_cast<int>(colorspace);
            LogInfo(oss2.str().c_str());
          } else {
            std::ostringstream oss2; oss2 << "Colorspace Override: Failed to set colorspace, HRESULT: 0x" << std::hex << hr;
            LogWarn(oss2.str().c_str());
          }
        }
      } catch (...) {
        LogWarn("Colorspace Override: Exception occurred while setting colorspace");
      }
    }
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
}

// Update composition state after presents (required for valid stats)
static void OnPresentUpdate(
    reshade::api::command_queue* /*queue*/, reshade::api::swapchain* swapchain,
    const reshade::api::rect* /*source_rect*/, const reshade::api::rect* /*dest_rect*/,
    uint32_t /*dirty_rect_count*/, const reshade::api::rect* /*dirty_rects*/) {
  // Throttle queries to ~every 30 presents
  int c = ++g_comp_query_counter;
  // Check for minimized window and restore if needed (prevent minimize feature)
  if (s_prevent_minimize >= 0.5f) {
    // Only restore if we haven't restored in the last 30 frames
      HWND hwnd = g_last_swapchain_hwnd.load();
      if (hwnd != nullptr && IsIconic(hwnd) && g_frames_since_last_restore.load() >= 1) {
        // Window is minimized, restore it in a separate thread to avoid blocking render thread
        std::thread([hwnd]() {
          std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Small delay to ensure UI thread is not blocked
          ShowWindow(hwnd, SW_SHOWNOACTIVATE); // should restore but not bring focus back
          LogDebug("Prevent Minimize: restored minimized window (bg thread)");
        }).detach();
        
        // Reset the counter after initiating restore
        g_frames_since_last_restore.store(0);
      }
  }
  
  // Increment frame counter for prevent minimize feature
  g_frames_since_last_restore.fetch_add(1);
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

BOOL APIENTRY DllMain(HMODULE h_module, DWORD fdw_reason, LPVOID lpv_reserved) {
  switch (fdw_reason) {
    case DLL_PROCESS_ATTACH:
      if (!reshade::register_addon(h_module)) return FALSE;
      reshade::register_event<reshade::addon_event::init_swapchain>(OnInitSwapchain);
      // Intercept SetFullscreenState; return true to skip original call
      reshade::register_event<reshade::addon_event::set_fullscreen_state>(
          [](reshade::api::swapchain* swapchain, bool fullscreen, void* hmonitor) -> bool {
            // log the event (reshade::log::message does not support printf-style formatting)
            {
              std::ostringstream oss;
              oss << "[Resolution Override] SetFullscreenState: fullscreen=" << (fullscreen ? 1 : 0)
                  << ", hmonitor=" << hmonitor;
              reshade::log::message(reshade::log::level::info, oss.str().c_str());
            }

            return false; // not handled; call original
          });

      renodx::utils::settings::use_presets = false;
      renodx::utils::settings::global_name = "ResolutionOverride";
      g_attach_time = std::chrono::steady_clock::now();
      g_shutdown.store(false);
      std::thread(RunBackgroundAudioMonitor).detach();
      // Seed default fps limit snapshot
      g_default_fps_limit.store(renodx::utils::swapchain::fps_limit);
      reshade::register_event<reshade::addon_event::present>(OnPresentUpdate);
      break;
    case DLL_PROCESS_DETACH:
      // Clean up the failure tracking structure
      IndependentFlipFailures* failures = g_if_failures.load();
      if (failures != nullptr) {
        delete failures;
        g_if_failures.store(nullptr);
      }
      
      reshade::unregister_event<reshade::addon_event::present>(OnPresentUpdate);
      reshade::unregister_event<reshade::addon_event::set_fullscreen_state>(
          [](reshade::api::swapchain*, bool, void*) { return false; });
      reshade::unregister_event<reshade::addon_event::init_swapchain>(OnInitSwapchain);
      reshade::unregister_addon(h_module);
      g_shutdown.store(true);
      break;
  }

  renodx::utils::settings::Use(fdw_reason, &settings);
  renodx::utils::swapchain::Use(fdw_reason);

  return TRUE;
}


