#define ImTextureID ImU64

#define DEBUG_LEVEL_0

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <deps/imgui/imgui.h>
#include <include/reshade.hpp>

#include <sstream>
#include <thread>
#include <atomic>
#include <chrono>

#include "../../utils/settings.hpp"

namespace {

// No persistent window handle; resolve on demand

// Style backup for borderless toggle
struct WindowStyleBackup {
  LONG_PTR style = 0;
  LONG_PTR ex_style = 0;
  RECT window_rect{0, 0, 0, 0};
  bool valid = false;
};
WindowStyleBackup g_style_backup{};

// Settings bindings
float s_borderless_enabled = 0.f; // 0 off, 1 on
float s_auto_apply_enabled = 0.f; // 0 off, 1 on
float s_auto_apply_delay_sec = 10.f; // 1..60 seconds (app start delay)
float s_auto_apply_init_delay_sec = 1.f; // 1..60 seconds (swapchain init delay)

// Selected numeric windowed size
float s_windowed_width = 1280.f;
float s_windowed_height = 720.f;
// Current window position baseline
float s_windowed_pos_x = 100.f;
float s_windowed_pos_y = 100.f;

// Helpers
inline RECT RectFromWH(LONG w, LONG h) {
  RECT r{0, 0, w, h};
  return r;
}

void LogInfo(const char* msg) { reshade::log::message(reshade::log::level::info, msg); }
void LogWarn(const char* msg) { reshade::log::message(reshade::log::level::warning, msg); }
void LogDebug(const std::string& s) { reshade::log::message(reshade::log::level::debug, s.c_str()); }

std::string FormatLastError() {
  DWORD err = GetLastError();
  if (err == 0) return std::string();
  LPSTR buf = nullptr;
  DWORD len = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&buf), 0, nullptr);
  std::string out = (len != 0 && buf != nullptr) ? std::string(buf, buf + len) : std::string("Unknown error");
  if (buf != nullptr) LocalFree(buf);
  std::ostringstream oss; oss << "GLE=0x" << std::hex << err << ": " << out;
  return oss.str();
}

void ApplyBorderless(HWND hwnd, bool use_work_area) {
  if (hwnd == nullptr) return;
  LogDebug("ApplyBorderless: begin");
  DWORD owner_tid = GetWindowThreadProcessId(hwnd, nullptr);
  DWORD this_tid = GetCurrentThreadId();
  {
    std::ostringstream tlog; tlog << "ApplyBorderless: owner_tid=" << owner_tid << " this_tid=" << this_tid;
    LogDebug(tlog.str());
  }
  HMONITOR h_mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
  if (h_mon == nullptr) {
    LogWarn("ApplyBorderless: MonitorFromWindow returned null");
    return;
  }

  MONITORINFO mi{sizeof(mi)};
  if (GetMonitorInfoW(h_mon, &mi) == FALSE) {
    LogWarn("ApplyBorderless: GetMonitorInfoW failed");
    return;
  }

  const RECT target = mi.rcMonitor; // fullscreen target

  // If we're not on the window thread, avoid style changes to prevent deadlocks.
  if (owner_tid != this_tid) {
    RECT target = mi.rcMonitor;
    std::ostringstream oss; oss << "ApplyBorderless (fallback): hwnd=" << hwnd
                                << " target=" << target.left << "," << target.top << " "
                                << (target.right - target.left) << "x" << (target.bottom - target.top);
    LogDebug(oss.str());
    UINT uflags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS | SWP_NOSENDCHANGING | SWP_SHOWWINDOW;
    SetWindowPos(hwnd, HWND_TOPMOST, target.left, target.top,
                 target.right - target.left, target.bottom - target.top, uflags);
    LogInfo("Applied borderless (fallback, no style change).");
    return;
  }

  // Backup original styles/rect once
  if (!g_style_backup.valid) {
    LogDebug("ApplyBorderless: backing up original window styles");
    g_style_backup.style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    g_style_backup.ex_style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    GetWindowRect(hwnd, &g_style_backup.window_rect);
    g_style_backup.valid = true;
  }

  LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
  LONG_PTR ex_style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

  std::ostringstream os_before; os_before << "ApplyBorderless: current style=0x" << std::hex << style
                                          << " ex=0x" << ex_style;
  LogDebug(os_before.str());

  LONG_PTR new_style = style;
  LONG_PTR new_ex_style = ex_style;
  new_style &= ~(WS_BORDER | WS_THICKFRAME | WS_DLGFRAME);
  new_style |= WS_POPUP; // keep other flags as-is (preserve WS_VISIBLE, MINIMIZEBOX etc.)
  new_ex_style &= ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE);

  std::ostringstream os_after; os_after << "ApplyBorderless: new style=0x" << std::hex << new_style
                                         << " ex=0x" << new_ex_style;
  LogDebug(os_after.str());

  const bool style_changed = (new_style != style) || (new_ex_style != ex_style);
  if (style_changed) {
    SetWindowLongPtrW(hwnd, GWL_STYLE, new_style);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, new_ex_style);
  }

  RECT wnd = target;
  if (AdjustWindowRectEx(&wnd, static_cast<DWORD>(new_style), FALSE, static_cast<DWORD>(new_ex_style)) == FALSE) {
    LogWarn("AdjustWindowRectEx failed for borderless.");
    return;
  }

  std::ostringstream oss; oss << "ApplyBorderless hwnd=" << hwnd
                              << " target=" << wnd.left << "," << wnd.top << " "
                              << (wnd.right - wnd.left) << "x" << (wnd.bottom - wnd.top);
  LogDebug(oss.str());

  // Check if position/size already matches
  RECT cur{};
  bool need_move = true;
  if (GetWindowRect(hwnd, &cur) != FALSE) {
    need_move = (cur.left != wnd.left) || (cur.top != wnd.top) ||
                ((cur.right - cur.left) != (wnd.right - wnd.left)) ||
                ((cur.bottom - cur.top) != (wnd.bottom - wnd.top));
  }

  if (!style_changed && !need_move) {
    LogDebug("ApplyBorderless: no changes needed");
    return;
  }

  UINT uflags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS | SWP_NOSENDCHANGING;
  if (style_changed) uflags |= SWP_FRAMECHANGED;
  if (IsWindowVisible(hwnd) == FALSE) uflags |= SWP_SHOWWINDOW;

  SetWindowPos(
      hwnd, nullptr,
      wnd.left, wnd.top,
      wnd.right - wnd.left,
      wnd.bottom - wnd.top,
      uflags);

  LogInfo("Applied borderless window.");
}

void RestoreWindowFromBorderless(HWND hwnd) {
  if (hwnd == nullptr) return;
  if (!g_style_backup.valid) return;

  SetWindowLongPtrW(hwnd, GWL_STYLE, g_style_backup.style);
  SetWindowLongPtrW(hwnd, GWL_EXSTYLE, g_style_backup.ex_style);

  const RECT r = g_style_backup.window_rect;
  std::ostringstream oss; oss << "RestoreWindowFromBorderless hwnd=" << hwnd
                              << " rect=" << r.left << "," << r.top << " "
                              << (r.right - r.left) << "x" << (r.bottom - r.top);
  LogDebug(oss.str());

  // Only set frame changed if style actually changed
  UINT uflags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS | SWP_NOSENDCHANGING;
  // Frame changed already implied by SetWindowLongPtr above; include to notify if styles changed
  uflags |= SWP_FRAMECHANGED;
  if (IsWindowVisible(hwnd) == FALSE) uflags |= SWP_SHOWWINDOW;

  SetWindowPos(
      hwnd, nullptr,
      r.left, r.top,
      r.right - r.left,
      r.bottom - r.top,
      uflags);

  LogInfo("Restored window styles from borderless.");
}

void ApplyWindowedMove(HWND hwnd, int client_width, int client_height, int pos_x, int pos_y) {
  if (hwnd == nullptr) return;

  LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
  LONG_PTR ex_style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

  RECT client = RectFromWH(client_width, client_height);
  if (AdjustWindowRectEx(&client, static_cast<DWORD>(style), FALSE, static_cast<DWORD>(ex_style)) == FALSE) {
    LogWarn("AdjustWindowRectEx failed for windowed move.");
    return;
  }

  std::ostringstream oss; oss << "ApplyWindowedMove hwnd=" << hwnd
                              << " client=" << client_width << "x" << client_height
                              << " pos=" << pos_x << "," << pos_y
                              << " wndsz=" << (client.right - client.left) << "x" << (client.bottom - client.top);
  LogDebug(oss.str());

  SetWindowPos(
      hwnd, nullptr,
      pos_x, pos_y,
      client.right - client.left,
      client.bottom - client.top,
      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_ASYNCWINDOWPOS | SWP_NOSENDCHANGING);

  LogInfo("Applied windowed move/resize.");
}

// No desktop mode changes in this addon per requirements

// Auto-apply on swapchain init scheduler
std::atomic<uint64_t> g_init_apply_generation{0};
std::chrono::steady_clock::time_point g_attach_time;
std::atomic<int> g_last_backbuffer_width{0};
std::atomic<int> g_last_backbuffer_height{0};

bool ShouldApplyWindowedForBackbuffer(int desired_w, int desired_h) {
  const int bb_w = g_last_backbuffer_width.load();
  const int bb_h = g_last_backbuffer_height.load();
  std::ostringstream oss;
  oss << "Backbuffer check (desired=" << desired_w << "x" << desired_h
      << ", current=" << bb_w << "x" << bb_h << ")";
  LogDebug(oss.str());
  if (bb_w <= 0 || bb_h <= 0) return true; // Unknown → apply
  return !(bb_w == desired_w && bb_h == desired_h);
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

  // Capture current desired size/mode at schedule time
  const bool want_borderless = (s_borderless_enabled >= 0.5f);
  const int want_w = static_cast<int>(s_windowed_width);
  const int want_h = static_cast<int>(s_windowed_height);
  std::thread([my_gen, delay_ms, hwnd, want_borderless, want_w, want_h]() {
    if (delay_ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    if (g_init_apply_generation.load() != my_gen) return;
    if (hwnd == nullptr) return;
    std::ostringstream oss; oss << "AutoApply on init after " << delay_ms << "ms: mode="
                                << (want_borderless ? "borderless" : "windowed")
                                << " size=" << want_w << "x" << want_h;
    LogDebug(oss.str());
    if (want_borderless) {
      ApplyBorderless(hwnd, false);
    } else {
      const int bb_w = g_last_backbuffer_width.load();
      const int bb_h = g_last_backbuffer_height.load();
      if (bb_w == want_w && bb_h == want_h) {
        LogDebug("AutoApply skipped: backbuffer matches desired size");
        return;
      }
      RECT wr{};
      int pos_x = 100;
      int pos_y = 100;
      if (GetWindowRect(hwnd, &wr) != FALSE) {
        pos_x = wr.left;
        pos_y = wr.top;
      }
      ApplyWindowedMove(hwnd, want_w, want_h, pos_x, pos_y);
    }
  }).detach();
}

// Preset lists and helpers for labeled width/height
const int WIDTH_OPTIONS[] = {1280, 1366, 1600, 1920, 2560, 3440, 3840};
const int HEIGHT_OPTIONS[] = {720, 900, 1080, 1200, 1440, 1600, 2160};

std::vector<std::string> MakeLabels(const int* values, size_t count) {
  std::vector<std::string> labels;
  labels.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    labels.emplace_back(std::to_string(values[i]));
  }
  return labels;
}

int FindClosestIndex(int value, const int* values, size_t count) {
  int best_index = 0;
  int best_delta = INT_MAX;
  for (size_t i = 0; i < count; ++i) {
    int d = std::abs(values[i] - value);
    if (d < best_delta) {
      best_delta = d;
      best_index = static_cast<int>(i);
    }
  }
  return best_index;
}

// UI/settings
renodx::utils::settings::Settings settings = {
    // Borderless toggle
    new renodx::utils::settings::Setting{
        .key = "Borderless",
        .binding = &s_borderless_enabled,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Borderless",
        .section = "Display",
        .labels = {"Off", "On"},
        .on_change = []() {
          // Do nothing automatically. Apply button executes changes.
        },
    },
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
        .label = "Auto Apply Delay (s)",
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
        .label = "Init Apply Delay (s)",
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
        .labels = MakeLabels(WIDTH_OPTIONS, std::size(WIDTH_OPTIONS)),
        .is_enabled = []() { return s_borderless_enabled < 0.5f; },
        .parse = [](float index) {
          int i = static_cast<int>(index);
          i = (std::max)(i, 0);
          int max_i = static_cast<int>(std::size(WIDTH_OPTIONS)) - 1;
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
        .labels = MakeLabels(HEIGHT_OPTIONS, std::size(HEIGHT_OPTIONS)),
        .is_enabled = []() { return s_borderless_enabled < 0.5f; },
        .parse = [](float index) {
          int i = static_cast<int>(index);
          i = (std::max)(i, 0);
          int max_i = static_cast<int>(std::size(HEIGHT_OPTIONS)) - 1;
          i = (std::min)(i, max_i);
          return static_cast<float>(HEIGHT_OPTIONS[i]);
        },
    },
    // Apply button executes either fullscreen borderless or windowed resize
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Apply",
        .section = "Display",
        .tooltip = "Apply the selected window width and height.",
        .on_click = []() {
          HWND hwnd = GetForegroundWindow();
          if (hwnd == nullptr) return false;
          DWORD owner_tid = GetWindowThreadProcessId(hwnd, nullptr);
          DWORD this_tid = GetCurrentThreadId();
          std::ostringstream oss; oss << "Apply clicked hwnd=" << hwnd
                                      << " owner_tid=" << owner_tid
                                      << " this_tid=" << this_tid
                                      << " mode=" << (s_borderless_enabled >= 0.5f ? "borderless" : "windowed")
                                      << " size=" << static_cast<int>(s_windowed_width) << "x" << static_cast<int>(s_windowed_height);
          LogDebug(oss.str());

          if (s_borderless_enabled >= 0.5f) {
            // Apply borderless
            ApplyBorderless(hwnd, false);
          } else {
            // Restore window styles and apply selected windowed size, keeping current top-left
            RestoreWindowFromBorderless(hwnd);
            RECT wr{};
            int pos_x = 100;
            int pos_y = 100;
            if (GetWindowRect(hwnd, &wr) != FALSE) {
              pos_x = wr.left;
              pos_y = wr.top;
            }
            if (!ShouldApplyWindowedForBackbuffer(static_cast<int>(s_windowed_width), static_cast<int>(s_windowed_height))) {
              LogDebug("Manual Apply skipped: backbuffer matches desired size");
              return false;
            }
            ApplyWindowedMove(
                hwnd,
                static_cast<int>(s_windowed_width),
                static_cast<int>(s_windowed_height),
                pos_x,
                pos_y);
          }
          return false; // do not trigger settings save
        },
    },
};

}  // namespace

extern "C" __declspec(dllexport) constexpr const char* NAME = "Resolution Override";
extern "C" __declspec(dllexport) constexpr const char* DESCRIPTION = "RenoDX Resolution/Window Override (Win32)";

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
  }

  // Schedule auto-apply even on resizes (generation counter ensures only latest runs)
  HWND hwnd = static_cast<HWND>(swapchain->get_hwnd());
  if (hwnd == nullptr) return;
  LogDebug(resize ? "Schedule auto-apply on swapchain init (resize)"
                  : "Schedule auto-apply on swapchain init");
  ScheduleAutoApplyOnInit(hwnd);
}

BOOL APIENTRY DllMain(HMODULE h_module, DWORD fdw_reason, LPVOID lpv_reserved) {
  switch (fdw_reason) {
    case DLL_PROCESS_ATTACH:
      if (!reshade::register_addon(h_module)) return FALSE;
      reshade::register_event<reshade::addon_event::init_swapchain>(OnInitSwapchain);

      renodx::utils::settings::use_presets = false;
      renodx::utils::settings::global_name = "ResolutionOverride";
      g_attach_time = std::chrono::steady_clock::now();
      break;
    case DLL_PROCESS_DETACH:
      reshade::unregister_event<reshade::addon_event::init_swapchain>(OnInitSwapchain);
      reshade::unregister_addon(h_module);
      break;
  }

  renodx::utils::settings::Use(fdw_reason, &settings);

  return TRUE;
}


