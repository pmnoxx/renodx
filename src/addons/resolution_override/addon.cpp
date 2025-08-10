
#define ImTextureID ImU64

#define DEBUG_LEVEL_0

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windef.h>
#include <deps/imgui/imgui.h>
#include <include/reshade.hpp>

#include <sstream>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cmath>

#include "../../utils/settings.hpp"
#include "../../utils/swapchain.hpp"

namespace {

// No persistent window handle; resolve on demand

// Settings bindings
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
float s_aspect_index = 1.f; // default to 16:9
// Reposition if window would be out of screen
float s_move_to_zero_if_out = 1.f; // 0 off, 1 on

// Forward decl
void ComputeDesiredSize(int& out_w, int& out_h);

// Helpers
inline RECT RectFromWH(LONG w, LONG h) {
  RECT r{0, 0, w, h};
  return r;
}

// Derive conservative SetWindowPos flags from current window state to preserve behavior
UINT ComputeSWPFlags(HWND hwnd, bool style_changed) {
  UINT flags = 0;
  flags |= SWP_NOZORDER; // preserve z-order
  if (GetForegroundWindow() != hwnd) flags |= SWP_NOACTIVATE; // don't steal focus
  if (style_changed) flags |= SWP_FRAMECHANGED; // notify style changes
  if (IsWindowVisible(hwnd) == FALSE) flags |= SWP_SHOWWINDOW; // ensure visible if hidden
  return flags;
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

// Unified window operation
enum class WindowStyleMode : std::uint8_t { KEEP, BORDERLESS, OVERLAPPED_WINDOW };

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
      new_style &= ~(WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
      new_style |= WS_POPUP;
      new_ex_style &= ~WS_EX_TOPMOST;
      style_changed = (new_style != current_style) || (new_ex_style != current_ex_style);
      break;
    }
    case WindowStyleMode::OVERLAPPED_WINDOW: {
      new_style &= ~WS_POPUP;
      new_style |= WS_OVERLAPPEDWINDOW;
      new_ex_style &= ~WS_EX_TOPMOST;
      style_changed = (new_style != current_style) || (new_ex_style != current_ex_style);
      break;
    }
  }

  if (style_changed) {
    SetWindowLongPtrW(hwnd, GWL_STYLE, new_style);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, new_ex_style);
  }

  RECT wr_current{};
  GetWindowRect(hwnd, &wr_current);

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
    target_x = pos_x;
    target_y = pos_y;
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
  if (s_move_to_zero_if_out >= 0.5f) {
    const int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    const bool out_of_bounds = (target_x < vx) || (target_y < vy) ||
                               (target_x + target_w > vx + vw) || (target_y + target_h > vy + vh);
    if (out_of_bounds) {
      target_x = 0;
      target_y = 0;
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
std::atomic<uint64_t> g_init_apply_generation{0};
std::chrono::steady_clock::time_point g_attach_time;
std::atomic<int> g_last_backbuffer_width{0};
std::atomic<int> g_last_backbuffer_height{0};
std::atomic<HWND> g_last_swapchain_hwnd{nullptr};

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
    if (bb_w == want_w && bb_h == want_h) {
      LogDebug("AutoApply skipped: backbuffer matches desired size");
      return;
    }
    ApplyWindowChange(hwnd,
                      /*do_resize=*/true, want_w, want_h,
                      /*do_move=*/true, target_pos_x, target_pos_y,
                      mode);
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

// Aspect ratio list (width:height)
struct AspectRatio { int w; int h; };
const AspectRatio ASPECT_OPTIONS[] = {
  {4, 3},
  {16, 9},
  {16, 10},
  {21, 9},
  {32, 9},
  {19, 9},
  {195, 90}, // 19.5:9
  {3, 2},
};

std::vector<std::string> MakeAspectLabels() {
  std::vector<std::string> labels;
  labels.reserve(std::size(ASPECT_OPTIONS));
  for (size_t i = 0; i < std::size(ASPECT_OPTIONS); ++i) {
    const auto ar = ASPECT_OPTIONS[i];
    std::ostringstream oss; oss << ar.w << ":" << ar.h;
    labels.emplace_back(oss.str());
  }
  return labels;
}

AspectRatio GetAspectByIndex(int index) {
  index = (std::max)(index, 0);
  int max_i = static_cast<int>(std::size(ASPECT_OPTIONS)) - 1;
  index = (std::min)(index, max_i);
  return ASPECT_OPTIONS[index];
}

void ComputeDesiredSize(int& out_w, int& out_h) {
  const int want_w = static_cast<int>(s_windowed_width);
  if (s_resize_mode < 0.5f) {
    out_w = want_w;
    out_h = static_cast<int>(s_windowed_height);
    return;
  }
  // Aspect mode
  int index = static_cast<int>(s_aspect_index);
  AspectRatio ar = GetAspectByIndex(index);
  // height = round(width * h / w)
  // prevent division by zero
  if (ar.w <= 0) ar.w = 1;
  std::int64_t num = static_cast<std::int64_t>(want_w) * static_cast<std::int64_t>(ar.h);
  int want_h = static_cast<int>((num + (ar.w / 2)) / ar.w);
  out_w = want_w;
  out_h = (std::max)(want_h, 1);
}

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
        .is_enabled = [](){ return s_resize_mode < 0.5f; },
        .parse = [](float index) {
          int i = static_cast<int>(index);
          i = (std::max)(i, 0);
          int max_i = static_cast<int>(std::size(HEIGHT_OPTIONS)) - 1;
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
        .default_value = 1.f, // 16:9
        .label = "Aspect Ratio",
        .section = "Display",
        .labels = MakeAspectLabels(),
        .is_enabled = [](){ return s_resize_mode >= 0.5f; },
    },
    // FPS Limit
    new renodx::utils::settings::Setting{
        .key = "FPSLimit",
        .binding = &renodx::utils::swapchain::fps_limit,
        .default_value = 0.f,
        .label = "FPS Limit",
        .section = "Performance",
        .min = 0.f,
        .max = 480.f,
        .format = "%.0f FPS",
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
            std::ostringstream oss; oss << "Apply (bg) clicked hwnd=" << hwnd
                                        << " owner_tid=" << owner_tid
                                        << " this_tid=" << this_tid
                                        << " size=" << static_cast<int>(s_windowed_width) << "x" << static_cast<int>(s_windowed_height);
            LogDebug(oss.str());

            RECT wr{};
            int pos_x = 100;
            int pos_y = 100;
            if (GetWindowRect(hwnd, &wr) != FALSE) {
              pos_x = wr.left;
              pos_y = wr.top;
            }
            WindowStyleMode mode = (s_remove_top_bar >= 0.5f) ? WindowStyleMode::BORDERLESS : WindowStyleMode::OVERLAPPED_WINDOW;
            int want_w = 0; int want_h = 0; ComputeDesiredSize(want_w, want_h);
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
    // Move to (0,0) if result is out of screen
    new renodx::utils::settings::Setting{
        .key = "RepositionIfOut",
        .binding = &s_move_to_zero_if_out,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 1.f,
        .label = "Reposition If Out (0,0)",
        .section = "Display",
        .tooltip = "If the target window size/position would be outside the virtual screen, move it to (0,0).",
        .labels = {"Off", "On"},
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
};

}  // namespace

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
  }

  // Schedule auto-apply even on resizes (generation counter ensures only latest runs)
  HWND hwnd = static_cast<HWND>(swapchain->get_hwnd());
  g_last_swapchain_hwnd.store(hwnd);
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
  renodx::utils::swapchain::Use(fdw_reason);

  return TRUE;
}


