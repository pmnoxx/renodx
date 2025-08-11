#include "utils.hpp"

// External declarations needed by utility functions
extern float s_windowed_width;
extern float s_windowed_height;
extern float s_resize_mode;
extern float s_aspect_index;
extern std::vector<MonitorInfo> g_monitors;

// Constant definitions
const int WIDTH_OPTIONS[] = {1280, 1366, 1600, 1920, 2560, 3440, 3840};
const int HEIGHT_OPTIONS[] = {720, 900, 1080, 1200, 1440, 1600, 2160};
const AspectRatio ASPECT_OPTIONS[] = {
  {3, 2},    // 1.5:1
  {4, 3},    // 1.333:1
  {16, 10},  // 1.6:1
  {16, 9},   // 1.778:1
  {19, 9},   // 2.111:1
  {195, 90}, // 2.167:1 (19.5:9)
  {21, 9},   // 2.333:1
  {32, 9},   // 3.556:1
};

// Helper function implementations
RECT RectFromWH(int width, int height) {
    RECT rect = {0, 0, width, height};
    return rect;
}

// Utility function implementations
void LogInfo(const char* msg) { 
    reshade::log::message(reshade::log::level::info, msg); 
}

void LogWarn(const char* msg) { 
    reshade::log::message(reshade::log::level::warning, msg); 
}

void LogDebug(const std::string& s) { 
    reshade::log::message(reshade::log::level::debug, s.c_str()); 
}

std::string FormatLastError() {
    DWORD error = GetLastError();
    if (error == 0) return "No error";
    
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    
    if (size == 0) return "Unknown error";
    
    std::string result(messageBuffer, size);
    LocalFree(messageBuffer);
    return result;
}

bool IsExclusiveFullscreen(HWND hwnd) {
    if (!hwnd) return false;
    
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    if (style & WS_POPUP) return true;
    
    RECT window_rect, client_rect;
    GetWindowRect(hwnd, &window_rect);
    GetClientRect(hwnd, &client_rect);
    
    // Convert client rect to screen coordinates
    POINT client_tl = {client_rect.left, client_rect.top};
    POINT client_br = {client_rect.right, client_rect.bottom};
    ClientToScreen(hwnd, &client_tl);
    ClientToScreen(hwnd, &client_br);
    
    // Check if window rect matches client rect (fullscreen indicator)
    return (window_rect.left == client_tl.x && 
            window_rect.top == client_tl.y && 
            window_rect.right == client_br.x && 
            window_rect.bottom == client_br.y);
}

UINT ComputeSWPFlags(HWND hwnd, bool style_changed) {
    UINT flags = SWP_NOZORDER | SWP_NOACTIVATE;
    if (style_changed) flags |= SWP_FRAMECHANGED;
    return flags;
}

bool IsBorderlessStyleBits(LONG_PTR style) {
    return (style & WS_CAPTION) == 0 && (style & WS_THICKFRAME) == 0;
}

bool IsBorderless(HWND hwnd) {
    if (!hwnd) return false;
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    return IsBorderlessStyleBits(style);
}

void RemoveWindowBorderLocal(HWND window) {
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
}

void ForceWindowToMonitorOrigin(HWND hwnd) {
    if (!hwnd) return;
    
    RECT window_rect;
    GetWindowRect(hwnd, &window_rect);
    
    int x = window_rect.left;
    int y = window_rect.top;
    
    // Move to origin if out of bounds
    if (x < 0 || y < 0) {
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void ForceWindowToMonitorOriginThreaded(HWND hwnd) {
    if (!hwnd) return;
    
    std::thread([hwnd]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ForceWindowToMonitorOrigin(hwnd);
    }).detach();
}

std::vector<std::string> MakeLabels(const int* values, size_t count) {
    std::vector<std::string> labels;
    labels.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        labels.push_back(std::to_string(values[i]));
    }
    return labels;
}

int FindClosestIndex(int value, const int* values, size_t count) {
    if (count == 0) return -1;
    
    int closest_index = 0;
    int min_diff = abs(value - values[0]);
    
    for (size_t i = 1; i < count; ++i) {
        int diff = abs(value - values[i]);
        if (diff < min_diff) {
            min_diff = diff;
            closest_index = static_cast<int>(i);
        }
    }
    
    return closest_index;
}

std::vector<std::string> MakeAspectLabels() {
    std::vector<std::string> labels;
    labels.reserve(8);
    labels.push_back("3:2");
    labels.push_back("4:3");
    labels.push_back("16:10");
    labels.push_back("16:9");
    labels.push_back("19:9");
    labels.push_back("19.5:9");
    labels.push_back("21:9");
    labels.push_back("32:9");
    return labels;
}

AspectRatio GetAspectByIndex(int index) {
    if (index >= 0 && index < 8) {
        return ASPECT_OPTIONS[index];
    }
    return {16, 9}; // Default to 16:9
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

int ComputeHeightFromWidth(int width) {
    if (s_resize_mode < 0.5f) {
        // Manual height mode - return the stored height setting
        return static_cast<int>(s_windowed_height);
    }
    
    // Aspect mode - compute height from width and aspect ratio
    int index = static_cast<int>(s_aspect_index);
    AspectRatio ar = GetAspectByIndex(index);
    
    // Prevent division by zero
    if (ar.w <= 0) ar.w = 1;
    
    // height = round(width * h / w)
    std::int64_t num = static_cast<std::int64_t>(width) * static_cast<std::int64_t>(ar.h);
    int height = static_cast<int>((num + (ar.w / 2)) / ar.w);
    
    // Ensure minimum height of 1
    return (std::max)(height, 1);
}

int ComputeHeightFromWidthAndAspect(int width, const AspectRatio& ar) {
    // Prevent division by zero
    int w = ar.w;
    if (w <= 0) w = 1;
    
    // height = round(width * h / w)
    std::int64_t num = static_cast<std::int64_t>(width) * static_cast<std::int64_t>(ar.h);
    int height = static_cast<int>((num + (w / 2)) / w);
    
    // Ensure minimum height of 1
    return (std::max)(height, 1);
}

std::vector<std::string> MakeMonitorLabels() {
    g_monitors.clear();
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, 0);
    std::vector<std::string> labels;
    labels.reserve(g_monitors.size() + 1);
    labels.emplace_back("Auto (Current)");
    for (size_t i = 0; i < g_monitors.size(); ++i) {
        const auto& mi = g_monitors[i].info;
        const RECT& r = mi.rcMonitor;
        const bool primary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
        std::ostringstream oss;
        oss << i + 1 << ": " << (primary ? "Primary " : "")
            << (r.right - r.left) << "x" << (r.bottom - r.top);
        labels.emplace_back(oss.str());
    }
    return labels;
}

// Monitor enumeration callback
BOOL CALLBACK MonitorEnumProc(HMONITOR hmon, HDC hdc, LPRECT rect, LPARAM lparam) {
    MONITORINFOEXW info;
    info.cbSize = sizeof(MONITORINFOEXW);
    
    if (GetMonitorInfoW(hmon, &info)) {
        MonitorInfo monitor_info;
        monitor_info.handle = hmon;
        monitor_info.info = info;
        g_monitors.push_back(monitor_info);
    }
    
    return TRUE;
}
