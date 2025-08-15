#include "../addon.hpp"
#include "../utils.hpp"
#include <thread>
#include <algorithm>
#include <sstream>

// Forward declaration
void ComputeDesiredSize(int& out_w, int& out_h);





void ApplyWindowChange(HWND hwnd, const char* reason) {
  if (hwnd == nullptr) return;

  // Get current window state
  RECT wr_current{};
  GetWindowRect(hwnd, &wr_current);
  
  // Check if maximized (needed for restoration)
  const bool was_maximized = IsZoomed(hwnd) != FALSE;
  
  // Get current styles
  LONG_PTR current_style = GetWindowLongPtrW(hwnd, GWL_STYLE);
  LONG_PTR current_ex_style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
  
  // Calculate new borderless styles
  LONG_PTR new_style = current_style & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
  LONG_PTR new_ex_style = current_ex_style & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
  
  // Check if styles actually changed
  bool style_changed = (new_style != current_style) || (new_ex_style != current_ex_style);

  // Get desired dimensions and position from global settings
  int desired_width = 0, desired_height = 0;
  ComputeDesiredSize(desired_width, desired_height);
  
  int target_w = 0;
  int target_h = 0;
  int target_x = 0;
  int target_y = 0;

  // Calculate target dimensions
  RECT client_rect = RectFromWH(desired_width, desired_height);
  if (AdjustWindowRectEx(&client_rect, static_cast<DWORD>(new_style), FALSE, static_cast<DWORD>(new_ex_style)) == FALSE) {
    LogWarn("AdjustWindowRectEx failed for ApplyWindowChange.");
    return;
  }
  target_w = client_rect.right - client_rect.left;
  target_h = client_rect.bottom - client_rect.top;

  // Calculate target position
  int pos_x = static_cast<int>(s_windowed_pos_x);
  int pos_y = static_cast<int>(s_windowed_pos_y);
  
  // Get monitor info efficiently
  HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
  MONITORINFOEXW mi{};
  mi.cbSize = sizeof(mi);
  
  // Use target monitor if specified
  if (s_target_monitor_index > 0.5f) {
    const int index = static_cast<int>(s_target_monitor_index) - 1;
    g_monitors.clear();
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&g_monitors));
    if (index >= 0 && index < static_cast<int>(g_monitors.size())) {
      hmon = g_monitors[index].handle;
      mi = g_monitors[index].info;
    }
  }
  
  if (mi.cbSize != sizeof(mi)) {
    GetMonitorInfoW(hmon, &mi);
  }
  
  // Calculate target position with monitor offset
  target_x = pos_x + mi.rcMonitor.left;
  target_y = pos_y + mi.rcMonitor.top;
  
  // Apply alignment if enabled
  if (s_move_to_zero_if_out > 0) {
    const RECT& mr = mi.rcMonitor;
    
    // Apply alignment based on setting
    switch (static_cast<int>(s_move_to_zero_if_out)) {
      case 1: // Top Left
        target_x = mr.left;
        target_y = mr.top;
        break;
      case 2: // Top Right
        target_x = mr.right - target_w;
        target_y = mr.top;
        break;
      case 3: // Bottom Left
        target_x = mr.left;
        target_y = mr.bottom - target_h;
        break;
      case 4: // Bottom Right
        target_x = mr.right - target_w;
        target_y = mr.bottom - target_h;
        break;
      case 5: // Center
        target_x = mr.left + (mr.right - mr.left - target_w) / 2;
        target_y = mr.top + (mr.bottom - mr.top - target_h) / 2;
        break;
      default: // Fallback to top left
        target_x = mr.left;
        target_y = mr.top;
        break;
    }
    
    // Clamp to monitor bounds
    if (target_x < mr.left) target_x = mr.left;
    if (target_x + target_w > mr.right) target_x = mr.right - target_w;
    if (target_y < mr.top) target_y = mr.top;
    if (target_y + target_h > mr.bottom) target_y = mr.bottom - target_h;
    
    // Fallback to virtual screen if still out of bounds
    const int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    
    bool out_of_bounds = (target_x < vx) || (target_y < vy) ||
                        (target_x + target_w > vx + vw) || (target_y + target_h > vy + vh);
    if (out_of_bounds) {
      // Apply same alignment to virtual screen
      switch (static_cast<int>(s_move_to_zero_if_out)) {
        case 1: // Top Left
          target_x = vx;
          target_y = vy;
          break;
        case 2: // Top Right
          target_x = vx + vw - target_w;
          target_y = vy;
          break;
        case 3: // Bottom Left
          target_x = vx;
          target_y = vy + vh - target_h;
          break;
        case 4: // Bottom Right
          target_x = vx + vw - target_w;
          target_y = vy + vh - target_h;
          break;
        case 5: // Center
          target_x = vx + (vw - target_w) / 2;
          target_y = vy + (vh - target_h) / 2;
          break;
        default: // Fallback to top left
          target_x = vx;
          target_y = vy;
          break;
      }
    }
  }

  // Check if any changes are actually needed
  bool needs_resize = (target_w != (wr_current.right - wr_current.left)) || 
                     (target_h != (wr_current.bottom - wr_current.top));
  bool needs_move = (target_x != wr_current.left) || (target_y != wr_current.top);
  
  if (!needs_resize && !needs_move && !style_changed) {
    LogDebug("ApplyWindowChange: No changes needed");
    return;
  }

  // Log the operation
  {
    std::ostringstream log_oss;
    log_oss << "ApplyWindowChange hwnd=" << hwnd
            << " reason=" << reason
            << " style_mode=Borderless"
            << " resize=" << (needs_resize ? "1" : "0")
            << " move=" << (needs_move ? "1" : "0")
            << " target_size=" << target_w << "x" << target_h
            << " target_pos=" << target_x << "," << target_y
            << " was_maximized=" << (was_maximized ? "1" : "0")
            << " style_changed=" << (style_changed ? "1" : "0");
    LogInfo(log_oss.str().c_str());
  }

  // Apply all changes in a single SetWindowPos call
  UINT flags = SWP_NOZORDER | SWP_NOACTIVATE;
  if (!needs_resize) flags |= SWP_NOSIZE;
  if (!needs_move) flags |= SWP_NOMOVE;
  if (style_changed) flags |= SWP_FRAMECHANGED;
  
  // Restore window if it was maximized (needed for style changes to take effect)
  if (was_maximized) {
    ShowWindow(hwnd, SW_RESTORE);
  }
  
  // Apply style changes first if needed
  if (style_changed) {
    SetWindowLongPtrW(hwnd, GWL_STYLE, new_style);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, new_ex_style);
  }
  
  // Apply position and size changes
  SetWindowPos(hwnd, nullptr, target_x, target_y, target_w, target_h, flags);
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
  std::thread([my_gen, delay_ms, hwnd, want_w, want_h, target_pos_x, target_pos_y]() {
    if (delay_ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    if (g_init_apply_generation.load() != my_gen) return;
    if (hwnd == nullptr) return;
    std::ostringstream oss; oss << "AutoApply on init after " << delay_ms << "ms: size=" << want_w << "x" << want_h;
    LogDebug(oss.str());
    const int bb_w = g_last_backbuffer_width.load();
    const int bb_h = g_last_backbuffer_height.load();
    // Apply if size differs (style is always borderless now)
    const bool size_matches = (bb_w == want_w && bb_h == want_h);
    if (size_matches) {
      LogDebug("AutoApply skipped: backbuffer matches desired state");
      return;
    }
    ApplyWindowChange(hwnd,
                      "schedule_auto_apply_on_init");
  }).detach();
}
