#include "../addon.hpp"
#include "../utils.hpp"
#include "../resolution_helpers.hpp"
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
    g_window_state.show_cmd = wp.showCmd;
  }
  
  // Calculate new borderless styles
  LONG_PTR new_style = current_style & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
  LONG_PTR new_ex_style = current_ex_style & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
  
  // Check if styles actually changed
  g_window_state.style_changed = (new_style != current_style) || (new_ex_style != current_ex_style);
  
  // PROACTIVE ALWAYS ON TOP DETECTION: Force style changes if always on top is detected
  if (s_prevent_always_on_top >= 0.5f) {
    if (current_ex_style & (WS_EX_TOPMOST | WS_EX_TOOLWINDOW)) {
      std::ostringstream oss;
      oss << "CalculateWindowState: ALWAYS ON TOP DETECTED - Forcing style change to remove extended styles 0x" << std::hex << (current_ex_style & (WS_EX_TOPMOST | WS_EX_TOOLWINDOW));
      LogInfo(oss.str().c_str());
      
      // Force style change to remove always on top
      g_window_state.style_changed = true;
    }
  }
  
  g_window_state.style_mode = WindowStyleMode::BORDERLESS;

  // First, determine the target monitor based on game's intended display
  HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
  MONITORINFOEXW mi{};
  mi.cbSize = sizeof(mi);
  
  // Use target monitor if specified, or Display tab monitor if desktop resolution override is enabled
  if (s_override_desktop_resolution >= 0.5f) {
    // Use the monitor selected in the Display tab
    const int index = static_cast<int>(s_selected_monitor_index);
    g_monitors.clear();
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&g_monitors));
    if (index >= 0 && index < static_cast<int>(g_monitors.size())) {
      hmon = g_monitors[index].handle;
      mi = g_monitors[index].info;
      
      std::ostringstream oss;
      oss << "CalculateWindowState: Using Display tab monitor " << index << " for desktop resolution override";
      LogDebug(oss.str());
    }
  } else if (s_target_monitor_index > 0.5f) {
    // Use the legacy target monitor setting
    const int index = static_cast<int>(s_target_monitor_index) - 1;
    g_monitors.clear();
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&g_monitors));
    if (index >= 0 && index < static_cast<int>(g_monitors.size())) {
      hmon = g_monitors[index].handle;
      mi = g_monitors[index].info;
    }
  } else {
    // When target monitor is 0, find the monitor where the game is currently on
    // by comparing window position against each monitor's dimensions
    g_monitors.clear();
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&g_monitors));
    
    // Find which monitor contains the game window
    const int window_center_x = (wr_current.left + wr_current.right) / 2;
    const int window_center_y = (wr_current.top + wr_current.bottom) / 2;
    
    bool found_monitor = false;
    for (const auto& monitor : g_monitors) {
      const RECT& mr = monitor.info.rcMonitor;
      if (window_center_x >= mr.left && window_center_x < mr.right &&
          window_center_y >= mr.top && window_center_y < mr.bottom) {
        hmon = monitor.handle;
        mi = monitor.info;
        found_monitor = true;
        
        std::ostringstream oss;
        oss << "CalculateWindowState: Game window is on monitor " << (&monitor - &g_monitors[0]) 
            << " (position: " << window_center_x << "," << window_center_y << ")";
        LogDebug(oss.str());
        break;
      }
    }
    
    if (!found_monitor) {
      // Fallback: use the monitor closest to the window
      hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
      GetMonitorInfoW(hmon, &mi);
      
      std::ostringstream oss;
      oss << "CalculateWindowState: Could not determine exact monitor, using closest monitor as fallback";
      LogWarn(oss.str().c_str());
    }
  }
  
  if (mi.cbSize != sizeof(mi)) {
    GetMonitorInfoW(hmon, &mi);
  }
  
  // Get desired dimensions and position from global settings
  if (s_override_desktop_resolution >= 0.5f) {
    // Use the new dynamic resolution system for desktop resolution override
    int width, height;
    if (renodx::resolution::GetSelectedResolution(static_cast<int>(s_selected_monitor_index), 
                                 static_cast<int>(s_selected_resolution_index), 
                                 width, height)) {
      g_window_state.desired_width = width;
      g_window_state.desired_height = height;
      
      std::ostringstream oss;
      oss << "CalculateWindowState: Desktop resolution override enabled - using " << width << "x" << height;
      LogDebug(oss.str());
    } else {
      // Log the failure and use default values
      LogWarn("CalculateWindowState: Failed to get resolution from new system, using default 1920x1080");
      g_window_state.desired_width = 1920;
      g_window_state.desired_height = 1080;
    }
  } else {
    // Use manual or aspect ratio mode
    ComputeDesiredSize(g_window_state.desired_width, g_window_state.desired_height);
  }
  
  // Clamp desired size to fit within the target monitor's dimensions
  const int monitor_width = mi.rcMonitor.right - mi.rcMonitor.left;
  const int monitor_height = mi.rcMonitor.bottom - mi.rcMonitor.top;
  
  if (g_window_state.desired_width > monitor_width) {
    std::ostringstream oss;
    oss << "CalculateWindowState: Desired width " << g_window_state.desired_width << " exceeds monitor width " << monitor_width << ", clamping";
    LogInfo(oss.str().c_str());
    g_window_state.desired_width = monitor_width;
  }
  
  if (g_window_state.desired_height > monitor_height) {
    std::ostringstream oss;
    oss << "CalculateWindowState: Desired height " << g_window_state.desired_height << " exceeds monitor height " << monitor_height << ", clamping";
    LogInfo(oss.str().c_str());
    g_window_state.desired_height = monitor_height;
  }
  
  // Calculate target dimensions
  RECT client_rect = RectFromWH(g_window_state.desired_width, g_window_state.desired_height);
  if (AdjustWindowRectEx(&client_rect, static_cast<DWORD>(new_style), FALSE, static_cast<DWORD>(new_ex_style)) == FALSE) {
    LogWarn("AdjustWindowRectEx failed for CalculateWindowState.");
    return;
  }
  g_window_state.target_w = client_rect.right - client_rect.left;
  g_window_state.target_h = client_rect.bottom - client_rect.top;
  
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
            << " state=" << (g_window_state.show_cmd == SW_SHOWMAXIMIZED ? "MAXIMIZED" : 
                           g_window_state.show_cmd == SW_SHOWMINIMIZED ? "MINIMIZED" : 
                           g_window_state.show_cmd == SW_SHOWNORMAL ? "RESTORED" : "UNKNOWN");
    LogInfo(log_oss.str().c_str());
  }
  
}

// Second function: Apply the calculated window changes
void ApplyWindowChange(HWND hwnd, const char* reason, bool force_apply) {
  if (hwnd == nullptr) return;
  
  if (g_window_state.show_cmd == SW_SHOWMAXIMIZED) {
    ShowWindow(hwnd, SW_RESTORE);
    return;
  }
  
  // First calculate the desired window state
  CalculateWindowState(hwnd, reason);

  
  // Get current styles
  LONG_PTR current_style = GetWindowLongPtrW(hwnd, GWL_STYLE);
  LONG_PTR current_ex_style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
  
  // Calculate new borderless styles
  LONG_PTR new_style = current_style & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
  LONG_PTR new_ex_style = current_ex_style & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
  
  // PREVENT ALWAYS ON TOP: Remove WS_EX_TOPMOST and WS_EX_TOOLWINDOW styles
  if (s_prevent_always_on_top >= 0.5f && (new_ex_style & (WS_EX_TOPMOST | WS_EX_TOOLWINDOW))) {
    new_ex_style &= ~(WS_EX_TOPMOST | WS_EX_TOOLWINDOW);
    
    // Log if we're removing always on top styles
    if ((current_ex_style & (WS_EX_TOPMOST | WS_EX_TOOLWINDOW)) != 0) {
      std::ostringstream oss;
      oss << "ApplyWindowChange: PREVENTING ALWAYS ON TOP - Removing extended styles 0x" << std::hex << (current_ex_style & (WS_EX_TOPMOST | WS_EX_TOOLWINDOW));
      LogInfo(oss.str().c_str());
    }
  }
  
  bool style_changed = false;
  bool ex_style_changed = false;

  
  // Check if any changes are needed
  if (g_window_state.needs_resize || g_window_state.needs_move) {
    //LogDebug("ApplyWindowChange: No changes needed");
    if (g_window_state.target_w <= 0 || g_window_state.target_h <= 0) {
      std::ostringstream oss;
      oss << "ApplyWindowChange: Invalid target size " << g_window_state.target_w << "x" << g_window_state.target_h << ", skipping";
      LogWarn(oss.str().c_str());
      return;
    }

    // Apply all changes in a single SetWindowPos call
    // UINT flags = SWP_NOZORDER | SWP_NOACTIVATE;
    //if (!g_window_state.needs_resize) flags |= SWP_NOSIZE;
    // if (!g_window_state.needs_move) flags |= SWP_NOMOVE;
    //if (style_changed || ex_style_changed) flags |= SWP_FRAMECHANGED;
    UINT flags = SWP_NOZORDER | SWP_NOOWNERZORDER;
    
    SetWindowPos(hwnd, nullptr, g_window_state.target_x, g_window_state.target_y, 
      g_window_state.target_w, g_window_state.target_h, flags);
  }
  
  if (current_style != new_style) {
    SetWindowLongPtrW(hwnd, GWL_STYLE, new_style);
    g_window_state.style_changed = true;
  }
  if (current_ex_style != new_ex_style) {
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, new_ex_style);
    g_window_state.style_changed = true;
  }

  /*
    if (!window) return;
    
    LONG_PTR style = GetWindowLongPtr(window, GWL_STYLE);
    LONG_PTR ex_style = GetWindowLongPtr(window, GWL_EXSTYLE);
    
    // Remove title bar and borders
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
    ex_style &= ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
    
    SetWindowLongPtr(window, GWL_STYLE, style);
    SetWindowLongPtr(window, GWL_EXSTYLE, ex_style);
    
    // Force window update
    SetWindowPos(window, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
  */
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
 //   if (size_matches) {
 //     LogDebug("AutoApply skipped: backbuffer matches desired state");
 //     return;
 //   }
    // Aplways do it once
    // Some games start window in fullscreen size, but if they don't get resize event their internal buffers are not updated.
    ApplyWindowChange(hwnd, "schedule_auto_apply_on_init", true);
  }).detach();
}
