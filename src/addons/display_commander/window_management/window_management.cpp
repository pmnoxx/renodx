#include "../addon.hpp"
#include "../utils.hpp"
#include "window_management.hpp"
#include <thread>
#include <algorithm>
#include <sstream>

// Forward declaration
void ComputeDesiredSize(int& out_w, int& out_h);





// First function: Calculate and update global window state
void CalculateWindowState(HWND hwnd, const char* reason) {
  if (hwnd == nullptr) return;
  
  // Reset global state
  g_window_state.reset();
  g_window_state.reason = reason;
  
  // Get current window state
  RECT wr_current{};
  GetWindowRect(hwnd, &wr_current);
  
  // Get current styles
  LONG_PTR current_style = GetWindowLongPtrW(hwnd, GWL_STYLE);
  LONG_PTR current_ex_style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
  
  // Detect window state (maximized, minimized, restored)
  WINDOWPLACEMENT wp = { sizeof(WINDOWPLACEMENT) };
  if (GetWindowPlacement(hwnd, &wp)) {
    g_window_state.is_maximized = (wp.showCmd == SW_SHOWMAXIMIZED);
    g_window_state.is_minimized = (wp.showCmd == SW_SHOWMINIMIZED);
    g_window_state.is_restored = (wp.showCmd == SW_SHOWNORMAL);
    
    // Log window state detection
    if (g_window_state.is_maximized) {
      LogDebug("CalculateWindowState: Window is MAXIMIZED - will need restoration");
    } else if (g_window_state.is_minimized) {
      LogDebug("CalculateWindowState: Window is MINIMIZED - will need restoration");
    } else if (g_window_state.is_restored) {
      LogDebug("CalculateWindowState: Window is RESTORED (normal state)");
    }
  } else {
    LogWarn("CalculateWindowState: Could not get window placement");
  }
  
  // Calculate new borderless styles
  LONG_PTR new_style = current_style & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
  LONG_PTR new_ex_style = current_ex_style & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
  
  // Check if styles actually changed
  g_window_state.style_changed = (new_style != current_style) || (new_ex_style != current_ex_style);
  g_window_state.style_mode = WindowStyleMode::BORDERLESS;

  // Get desired dimensions and position from global settings
  ComputeDesiredSize(g_window_state.desired_width, g_window_state.desired_height);
  
  // Calculate target dimensions
  RECT client_rect = RectFromWH(g_window_state.desired_width, g_window_state.desired_height);
  if (AdjustWindowRectEx(&client_rect, static_cast<DWORD>(new_style), FALSE, static_cast<DWORD>(new_ex_style)) == FALSE) {
    LogWarn("AdjustWindowRectEx failed for CalculateWindowState.");
    return;
  }
  g_window_state.target_w = client_rect.right - client_rect.left;
  g_window_state.target_h = client_rect.bottom - client_rect.top;

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
  
  // Calculate target position - start with monitor top-left
  g_window_state.target_x = mi.rcMonitor.left;
  g_window_state.target_y = mi.rcMonitor.top;
  
  // Apply alignment if enabled
  if (s_move_to_zero_if_out > 0) {
    const RECT& mr = mi.rcMonitor;
    
    // Apply alignment based on setting
    switch (static_cast<int>(s_move_to_zero_if_out)) {
      case 1: // Top Left
        g_window_state.target_x = mr.left;
        g_window_state.target_y = mr.top;
        break;
      case 2: // Top Right
        g_window_state.target_x = mr.right - g_window_state.target_w;
        g_window_state.target_y = mr.top;
        break;
      case 3: // Bottom Left
        g_window_state.target_x = mr.left;
        g_window_state.target_y = mr.bottom - g_window_state.target_h;
        break;
      case 4: // Bottom Right
        g_window_state.target_x = mr.right - g_window_state.target_w;
        g_window_state.target_y = mr.bottom - g_window_state.target_h;
        break;
      case 5: // Center
        g_window_state.target_x = mr.left + (mr.right - mr.left - g_window_state.target_w) / 2;
        g_window_state.target_y = mr.top + (mr.bottom - mr.top - g_window_state.target_h) / 2;
        break;
      default: // Fallback to top left
        g_window_state.target_x = mr.left;
        g_window_state.target_y = mr.top;
        break;
    }
    
    // Clamp to monitor bounds
    if (g_window_state.target_x < mr.left) g_window_state.target_x = mr.left;
    if (g_window_state.target_x + g_window_state.target_w > mr.right) g_window_state.target_x = mr.right - g_window_state.target_w;
    if (g_window_state.target_y < mr.top) g_window_state.target_y = mr.top;
    if (g_window_state.target_y + g_window_state.target_h > mr.bottom) g_window_state.target_y = mr.bottom - g_window_state.target_h;
    
    // Fallback to virtual screen if still out of bounds
    const int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    
    bool out_of_bounds = (g_window_state.target_x < vx) || (g_window_state.target_y < vy) ||
                        (g_window_state.target_x + g_window_state.target_w > vx + vw) || (g_window_state.target_y + g_window_state.target_h > vy + vh);
    if (out_of_bounds) {
      // Apply same alignment to virtual screen
      switch (static_cast<int>(s_move_to_zero_if_out)) {
        case 1: // Top Left
          g_window_state.target_x = vx;
          g_window_state.target_y = vy;
          break;
        case 2: // Top Right
          g_window_state.target_x = vx + vw - g_window_state.target_w;
          g_window_state.target_y = vy;
          break;
        case 3: // Bottom Left
          g_window_state.target_x = vx;
          g_window_state.target_y = vy + vh - g_window_state.target_h;
          break;
        case 4: // Bottom Right
          g_window_state.target_x = vx + vw - g_window_state.target_w;
          g_window_state.target_y = vy + vh - g_window_state.target_h;
          break;
        case 5: // Center
          g_window_state.target_x = vx + (vw - g_window_state.target_w) / 2;
          g_window_state.target_y = vy + (vh - g_window_state.target_h) / 2;
          break;
        default: // Fallback to top left
          g_window_state.target_x = vx;
          g_window_state.target_y = vy;
          break;
      }
    }
  }

  // Check if any changes are actually needed
  g_window_state.needs_resize = (g_window_state.target_w != (wr_current.right - wr_current.left)) || 
                                (g_window_state.target_h != (wr_current.bottom - wr_current.top));
  g_window_state.needs_move = (g_window_state.target_x != wr_current.left) || (g_window_state.target_y != wr_current.top);
  
  if (!g_window_state.needs_resize && !g_window_state.needs_move && !g_window_state.style_changed) {
    LogDebug("CalculateWindowState: No changes needed");
    return;
  }

  // Log the calculated state
  {
    std::ostringstream log_oss;
    log_oss << "CalculateWindowState hwnd=" << hwnd
            << " reason=" << reason
            << " style_mode=Borderless"
            << " resize=" << (g_window_state.needs_resize ? "1" : "0")
            << " move=" << (g_window_state.needs_move ? "1" : "0")
            << " target_size=" << g_window_state.target_w << "x" << g_window_state.target_h
            << " target_pos=" << g_window_state.target_x << "," << g_window_state.target_y
            << " state=" << (g_window_state.is_maximized ? "MAXIMIZED" : 
                           g_window_state.is_minimized ? "MINIMIZED" : "RESTORED");
    LogInfo(log_oss.str().c_str());
  }
  
  // AUTO-RESTORE: If window is maximized and we're suppressing maximize, restore it automatically
  if (s_suppress_maximize >= 0.5f && g_window_state.is_maximized) {
    LogInfo("CalculateWindowState: AUTO-RESTORE - Window is maximized, sending restore command");
    
    // Post a restore command to bring it back to normal
    PostMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    
    // Also try to force the window size to our desired state
    SetWindowPos(hwnd, nullptr, 
               g_window_state.target_x, g_window_state.target_y,
               g_window_state.target_w, g_window_state.target_h,
               SWP_NOZORDER | SWP_NOACTIVATE);
    
    std::ostringstream restore_oss;
    restore_oss << "CalculateWindowState: AUTO-RESTORE - Forced window to target size " 
               << g_window_state.target_w << "x" << g_window_state.target_h;
    LogInfo(restore_oss.str().c_str());
  }
}

// Second function: Apply the calculated window changes
void ApplyWindowChange(HWND hwnd, const char* reason) {
  if (hwnd == nullptr) return;
  
  // First calculate the desired window state
  CalculateWindowState(hwnd, reason);
  
  // Check if any changes are needed
  if (!g_window_state.needs_resize && !g_window_state.needs_move && !g_window_state.style_changed) {
    LogDebug("ApplyWindowChange: No changes needed");
    return;
  }
  
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
  
  // Apply all changes in a single SetWindowPos call
  UINT flags = SWP_NOZORDER | SWP_NOACTIVATE;
  if (!g_window_state.needs_resize) flags |= SWP_NOSIZE;
  if (!g_window_state.needs_move) flags |= SWP_NOMOVE;
  if (g_window_state.style_changed) flags |= SWP_FRAMECHANGED;
  
  // Restore window if it was maximized (needed for style changes to take effect)
  if (was_maximized) {
    ShowWindow(hwnd, SW_RESTORE);
  }
  
  // Apply style changes first if needed
  if (g_window_state.style_changed) {
    SetWindowLongPtrW(hwnd, GWL_STYLE, new_style);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, new_ex_style);
  }
  
  // Apply position and size changes
  SetWindowPos(hwnd, nullptr, g_window_state.target_x, g_window_state.target_y, g_window_state.target_w, g_window_state.target_h, flags);
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
  std::thread([my_gen, delay_ms, hwnd, want_w, want_h]() {
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
    ApplyWindowChange(hwnd, "schedule_auto_apply_on_init");
  }).detach();
}
