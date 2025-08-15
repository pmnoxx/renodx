#pragma once

#include <windows.h>
#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <mutex>

// Window creation API detection
enum class WindowCreationAPI {
    UNKNOWN,
    WIN32_CREATEWINDOW,
    WIN32_CREATEWINDOWEX,
    WIN32_REGISTERCLASS,
    DIRECTX_SWAPCHAIN,
    OPENGL_WGL,
    VULKAN_SURFACE,
    SDL2,
    GLFW,
    QT,
    WPF,
    UWP
};

// Window creation method detection
enum class WindowCreationMethod {
    UNKNOWN,
    DIRECT_CREATION,
    SUBCLASSED,
    HOOKED,
    INHERITED,
    WRAPPER
};

// Structure to hold window creation information
struct WindowCreationInfo {
    WindowCreationAPI api = WindowCreationAPI::UNKNOWN;
    WindowCreationMethod method = WindowCreationMethod::UNKNOWN;
    std::string api_name = "Unknown";
    std::string method_name = "Unknown";
    std::string window_class = "Unknown";
    std::string process_name = "Unknown";
    DWORD process_id = 0;
    HWND parent_window = nullptr;
    bool is_child = false;
    bool is_owned = false;
    std::string additional_info = "";
    
    // Window style information
    LONG_PTR original_style = 0;
    LONG_PTR original_ex_style = 0;
    LONG_PTR current_style = 0;
    LONG_PTR current_ex_style = 0;
    
    // Creation timing
    std::chrono::steady_clock::time_point creation_time;
    std::chrono::steady_clock::time_point first_style_change;
    std::chrono::steady_clock::time_point last_style_change;
    
    // Style change history
    struct StyleChange {
        std::chrono::steady_clock::time_point timestamp;
        LONG_PTR old_style;
        LONG_PTR new_style;
        LONG_PTR old_ex_style;
        LONG_PTR new_ex_style;
        std::string reason;
    };
    std::vector<StyleChange> style_changes;
};

// Window creation detection functions
WindowCreationInfo DetectWindowCreationInfo(HWND hwnd);
std::string GetDetailedWindowInfo(HWND hwnd);
std::string GetWindowCreationSummary(HWND hwnd);

// Window style change tracking
void TrackWindowStyleChange(HWND hwnd, LONG_PTR old_style, LONG_PTR new_style, LONG_PTR old_ex_style, LONG_PTR new_ex_style, const std::string& reason);

// Hook installation functions
bool InstallWindowCreationDetectionHooks();
void UninstallWindowCreationDetectionHooks();

// Global window tracking
extern std::map<HWND, WindowCreationInfo> g_window_creation_history;
extern std::mutex g_window_creation_mutex;
