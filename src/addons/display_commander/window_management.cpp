#include "addon.hpp"
#include "utils.hpp"
#include <thread>
#include <algorithm>
#include <sstream>

// Forward declaration
void ComputeDesiredSize(int& out_w, int& out_h);

// Helper function to send resolution change notifications from another thread
void SendResolutionChangeNotifications(HWND hwnd, int client_width, int client_height, int target_x, int target_y, int target_w, int target_h, UINT flags) {
    if (hwnd == nullptr) return;
    
    // Send messages from another thread to avoid potential deadlocks
    std::thread([hwnd, client_width, client_height, target_x, target_y, target_w, target_h, flags]() {
        // Small delay to ensure SetWindowPos has completed
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        // Send WM_SIZE message to notify about client area size change
        // This is the most important message for games to update their internal buffers
        SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(client_width, client_height));
        
        // Send WM_WINDOWPOSCHANGED to notify about window position/size change
        WINDOWPOS wp = {};
        wp.hwnd = hwnd;
        wp.x = target_x;
        wp.y = target_y;
        wp.cx = target_w;
        wp.cy = target_h;
        wp.flags = flags;
        SendMessage(hwnd, WM_WINDOWPOSCHANGED, 0, reinterpret_cast<LPARAM>(&wp));
        
        // Send WM_PAINT to trigger a repaint
        InvalidateRect(hwnd, nullptr, TRUE);
        UpdateWindow(hwnd);
        
        // Additional messages that games might need for resolution changes
        SendMessage(hwnd, WM_GETMINMAXINFO, 0, 0);
        SendMessage(hwnd, WM_NCCALCSIZE, FALSE, 0);
        
        // For DirectX games, send additional messages that might be needed
        // Some games listen for these messages to update their swapchain
        SendMessage(hwnd, WM_DISPLAYCHANGE, 32, MAKELPARAM(client_width, client_height));
        
        // Force a redraw of the entire window
        RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
        
        LogInfo("Sent comprehensive resolution change notifications to game from separate thread");
    }).detach();
}

// Additional function for games that need staggered resolution change notifications
void SendStaggeredResolutionNotifications(HWND hwnd, int client_width, int client_height) {
    if (hwnd == nullptr) return;
    
    // Some games need multiple notifications over time to properly update their buffers
    std::thread([hwnd, client_width, client_height]() {
        // First notification
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(client_width, client_height));
        
        // Second notification after a longer delay
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(client_width, client_height));
        
        // Final notification with additional messages
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        SendMessage(hwnd, WM_SIZE, SIZE_RESTORED, MAKELPARAM(client_width, client_height));
        SendMessage(hwnd, WM_PAINT, 0, 0);
        
        LogInfo("Sent staggered resolution change notifications to game");
    }).detach();
}

void ApplyWindowChange(HWND hwnd,
                       bool do_resize, int client_width, int client_height,
                       bool do_move, int pos_x, int pos_y,
                       WindowStyleMode style_mode, const char* reason) {
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
      // Set borderless styles directly
      // current_style and current_ex_style are already declared at function level
      
      LONG_PTR new_style = current_style;
      LONG_PTR new_ex_style = current_ex_style;
      
      // Remove title bar and borders
      new_style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
      new_ex_style &= ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
      
      // Check if styles actually changed
      style_changed = (new_style != current_style) || (new_ex_style != current_ex_style);
      
      if (style_changed) {
        std::ostringstream oss;
        oss << "Applying borderless styles - Current style: 0x" << std::hex << current_style 
            << ", New style: 0x" << new_style 
            << ", Current ex_style: 0x" << current_ex_style 
            << ", New ex_style: 0x" << new_ex_style;
        LogDebug(oss.str().c_str());
        
        SetWindowLongPtrW(hwnd, GWL_STYLE, new_style);
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, new_ex_style);
        
        // Verify the styles were set correctly
        LONG_PTR verify_style = GetWindowLongPtrW(hwnd, GWL_STYLE);
        LONG_PTR verify_ex_style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        
        if (verify_style != new_style || verify_ex_style != new_ex_style) {
          std::ostringstream oss;
          oss << "Failed to set window styles - Expected style: 0x" << std::hex << new_style 
              << ", Got: 0x" << verify_style 
              << ", Expected ex_style: 0x" << new_ex_style 
              << ", Got: 0x" << verify_ex_style;
          LogWarn(oss.str().c_str());
        } else {
          LogDebug("Window styles set successfully");
        }
      } else {
        LogDebug("Window styles already match desired borderless state");
      }
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
  // Always apply alignment when enabled, not just when out of bounds
  if (s_move_to_zero_if_out > 0) { // 0 = None, 1+ = different alignment options
    // Get monitor bounds
    const RECT& mr = mi.rcMonitor;
    
    // Apply alignment based on setting value
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
    repositioned = true;
    effective_do_move = true;
    
          // Check if the aligned position is still out of bounds and clamp if needed
      bool out_of_bounds = (target_x < mr.left) || (target_y < mr.top) ||
                           (target_x + target_w > mr.right) || (target_y + target_h > mr.bottom);
      if (out_of_bounds) {
        // Clamp to monitor bounds
        if (target_x < mr.left) target_x = mr.left;
        if (target_x + target_w > mr.right) target_x = mr.right - target_w;
        if (target_y < mr.top) target_y = mr.top;
        if (target_y + target_h > mr.bottom) target_y = mr.bottom - target_h;
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
            << " reason=" << reason
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
      
  // Send proper window messages to notify the game about resolution change
  // This ensures the game updates its internal buffers and rendering pipeline
  if (do_resize) {
    // Check if staggered notifications are enabled
    extern float s_staggered_resolution_notifications;
    if (s_staggered_resolution_notifications >= 0.5f) {
      SendStaggeredResolutionNotifications(hwnd, client_width, client_height);
    } else {
      SendResolutionChangeNotifications(hwnd, client_width, client_height, target_x, target_y, target_w, target_h, flags);
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
                      mode, "schedule_auto_apply_on_init");
  }).detach();
}
