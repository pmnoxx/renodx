#include "window_creation_detection.hpp"
#include "../addon.hpp"
#include "../utils.hpp"
#include <algorithm>
#include <psapi.h>
#include <sstream>

// Global window tracking
std::map<HWND, WindowCreationInfo> g_window_creation_history;
std::mutex g_window_creation_mutex;

// Hook function pointers
static HWND (WINAPI* g_original_CreateWindowExW)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) = nullptr;
static HWND (WINAPI* g_original_CreateWindowW)(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) = nullptr;
static ATOM (WINAPI* g_original_RegisterClassExW)(const WNDCLASSEXW* lpWndClassEx) = nullptr;
static LONG_PTR (WINAPI* g_original_SetWindowLongPtrW)(HWND hWnd, int nIndex, LONG_PTR dwNewLong) = nullptr;



// Hooked CreateWindowW
HWND WINAPI CreateWindowW_Hook(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    // Call the original function first
    HWND hwnd = g_original_CreateWindowW(lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    
    if (hwnd != nullptr) {
        // Record window creation (similar to CreateWindowExW)
        std::lock_guard<std::mutex> lock(g_window_creation_mutex);
        
        WindowCreationInfo info;
        info.creation_time = std::chrono::steady_clock::now();
        info.original_style = dwStyle;
        info.original_ex_style = 0; // CreateWindowW doesn't have extended styles
        info.current_style = dwStyle;
        info.current_ex_style = 0;
        
        // Detect API and method
        info.api = WindowCreationAPI::WIN32_CREATEWINDOW;
        info.api_name = "Win32 CreateWindowW";
        info.method = WindowCreationMethod::DIRECT_CREATION;
        info.method_name = "Direct Creation";
        
        // Get window class name
        if (lpClassName != nullptr) {
            char class_name[256];
            if (WideCharToMultiByte(CP_UTF8, 0, lpClassName, -1, class_name, sizeof(class_name), nullptr, nullptr) > 0) {
                info.window_class = class_name;
            }
        }
        
        // Get process information
        DWORD process_id = GetCurrentProcessId();
        info.process_id = process_id;
        
        // Get process name
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, process_id);
        if (hProcess != nullptr) {
            char process_name[MAX_PATH];
            if (GetModuleFileNameExA(hProcess, nullptr, process_name, MAX_PATH)) {
                std::string full_path(process_name);
                size_t last_slash = full_path.find_last_of("/\\");
                if (last_slash != std::string::npos) {
                    info.process_name = full_path.substr(last_slash + 1);
                } else {
                    info.process_name = full_path;
                }
            }
            CloseHandle(hProcess);
        }
        
        // Check parent/child relationships
        info.parent_window = hWndParent;
        info.is_child = (hWndParent != nullptr);
        info.is_owned = false;
        
        // Store the information
        g_window_creation_history[hwnd] = info;
        
        LogDebug("Window creation detected: " + info.window_class + " via " + info.api_name);
    }
    
    return hwnd;
}

// Hooked RegisterClassExW
ATOM WINAPI RegisterClassExW_Hook(const WNDCLASSEXW* lpWndClassEx) {
    // Call the original function first
    ATOM result = g_original_RegisterClassExW(lpWndClassEx);
    
    if (result != 0 && lpWndClassEx != nullptr) {
        // Get class name
        char class_name[256];
        if (WideCharToMultiByte(CP_UTF8, 0, lpWndClassEx->lpszClassName, -1, class_name, sizeof(class_name), nullptr, nullptr) > 0) {
            LogDebug("Window class registered: " + std::string(class_name) + " with style: 0x" + std::to_string(lpWndClassEx->style));
        }
    }
    
    return result;
}

// Detect window creation API and method
WindowCreationInfo DetectWindowCreationInfo(HWND hwnd) {
    if (hwnd == nullptr) {
        WindowCreationInfo info;
        return info;
    }
    
    std::lock_guard<std::mutex> lock(g_window_creation_mutex);
    
    // Check if we have recorded information for this window
    auto it = g_window_creation_history.find(hwnd);
    if (it != g_window_creation_history.end()) {
        return it->second;
    }
    
    // If not found, try to detect from current state
    WindowCreationInfo info;
    
    // Get process information
    DWORD process_id = 0;
    GetWindowThreadProcessId(hwnd, &process_id);
    info.process_id = process_id;
    
    // Get process name
    if (process_id != 0) {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, process_id);
        if (hProcess != nullptr) {
            char process_name[MAX_PATH];
            if (GetModuleFileNameExA(hProcess, nullptr, process_name, MAX_PATH)) {
                std::string full_path(process_name);
                size_t last_slash = full_path.find_last_of("/\\");
                if (last_slash != std::string::npos) {
                    info.process_name = full_path.substr(last_slash + 1);
                } else {
                    info.process_name = full_path;
                }
            }
            CloseHandle(hProcess);
        }
    }
    
    // Get window class
    char class_name[256];
    if (GetClassNameA(hwnd, class_name, sizeof(class_name)) > 0) {
        info.window_class = class_name;
    }
    
    // Check parent/child relationships
    info.parent_window = GetParent(hwnd);
    info.is_child = (info.parent_window != nullptr);
    info.is_owned = (GetWindow(hwnd, GW_OWNER) != nullptr);
    
    // Get current styles
    info.current_style = GetWindowLongPtr(hwnd, GWL_STYLE);
    info.current_ex_style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    
    // Detect API based on window class and process name
    std::string lower_class = info.window_class;
    std::string lower_process = info.process_name;
    std::transform(lower_class.begin(), lower_class.end(), lower_class.begin(), ::tolower);
    std::transform(lower_process.begin(), lower_process.end(), lower_process.begin(), ::tolower);
    
    // DirectX detection
    if (lower_class.find("d3d") != std::string::npos || 
        lower_class.find("dxgi") != std::string::npos ||
        lower_class.find("swapchain") != std::string::npos) {
        info.api = WindowCreationAPI::DIRECTX_SWAPCHAIN;
        info.api_name = "DirectX SwapChain";
    }
    // OpenGL detection
    else if (lower_class.find("ogl") != std::string::npos ||
             lower_class.find("gl") != std::string::npos ||
             lower_class.find("wgl") != std::string::npos) {
        info.api = WindowCreationAPI::OPENGL_WGL;
        info.api_name = "OpenGL WGL";
    }
    // Vulkan detection
    else if (lower_class.find("vk") != std::string::npos ||
             lower_class.find("vulkan") != std::string::npos) {
        info.api = WindowCreationAPI::VULKAN_SURFACE;
        info.api_name = "Vulkan Surface";
    }
    // SDL2 detection
    else if (lower_class.find("sdl") != std::string::npos ||
             lower_process.find("sdl") != std::string::npos) {
        info.api = WindowCreationAPI::SDL2;
        info.api_name = "SDL2";
    }
    // GLFW detection
    else if (lower_class.find("glfw") != std::string::npos ||
             lower_process.find("glfw") != std::string::npos) {
        info.api = WindowCreationAPI::GLFW;
        info.api_name = "GLFW";
    }
    // Qt detection
    else if (lower_class.find("qt") != std::string::npos ||
             lower_class.find("qwidget") != std::string::npos ||
             lower_process.find("qt") != std::string::npos) {
        info.api = WindowCreationAPI::QT;
        info.api_name = "Qt";
    }
    // WPF detection
    else if (lower_class.find("hwndwrapper") != std::string::npos ||
             lower_class.find("wpf") != std::string::npos) {
        info.api = WindowCreationAPI::WPF;
        info.api_name = "WPF";
    }
    // UWP detection
    else if (lower_class.find("applicationframe") != std::string::npos ||
             lower_class.find("windows.ui") != std::string::npos) {
        info.api = WindowCreationAPI::UWP;
        info.api_name = "UWP";
    }
    // Standard Win32
    else {
        info.api = WindowCreationAPI::WIN32_CREATEWINDOW;
        info.api_name = "Win32 CreateWindow";
    }
    
    // Detect creation method
    WNDPROC current_proc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
    if (current_proc != DefWindowProc) {
        info.method = WindowCreationMethod::SUBCLASSED;
        info.method_name = "Subclassed";
        info.additional_info = "Custom window procedure detected";
    }
    // Check if window has hooks installed
    else if (info.current_ex_style & WS_EX_LAYERED || info.current_ex_style & WS_EX_TRANSPARENT) {
        info.method = WindowCreationMethod::HOOKED;
        info.method_name = "Hooked";
        info.additional_info = "Layered or transparent styles suggest hooks";
    }
    // Check if window inherits from another
    else if (info.is_child || info.is_owned) {
        info.method = WindowCreationMethod::INHERITED;
        info.method_name = "Inherited";
        info.additional_info = "Child or owned window";
    }
    // Standard creation
    else {
        info.method = WindowCreationMethod::DIRECT_CREATION;
        info.method_name = "Direct Creation";
        info.additional_info = "Standard Win32 window creation";
    }
    
    return info;
}

// Get detailed window information for UI display
std::string GetDetailedWindowInfo(HWND hwnd) {
    if (hwnd == nullptr) return "No window handle available";
    
    WindowCreationInfo info = DetectWindowCreationInfo(hwnd);
    
    std::ostringstream oss;
    oss << "=== Window Creation Information ===\n";
    oss << "Process: " << info.process_name << " (PID: " << info.process_id << ")\n";
    oss << "Window Class: " << info.window_class << "\n";
    oss << "Creation API: " << info.api_name << "\n";
    oss << "Creation Method: " << info.method_name << "\n";
    
    if (info.is_child) {
        oss << "Parent Window: " << std::hex << (uintptr_t)info.parent_window << std::dec << "\n";
    }
    if (info.is_owned) {
        oss << "Owner Window: " << std::hex << (uintptr_t)GetWindow(hwnd, GW_OWNER) << std::dec << "\n";
    }
    
    // Get window styles
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    LONG_PTR ex_style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    
    oss << "\n=== Window Styles ===\n";
    oss << "Style: 0x" << std::hex << style << std::dec << "\n";
    oss << "ExStyle: 0x" << std::hex << ex_style << std::dec << "\n";
    
    // Decode common styles
    oss << "\nStyle Breakdown:\n";
    if (style & WS_POPUP) oss << "  WS_POPUP\n";
    if (style & WS_OVERLAPPED) oss << "  WS_OVERLAPPED\n";
    if (style & WS_CAPTION) oss << "  WS_CAPTION\n";
    if (style & WS_SYSMENU) oss << "  WS_SYSMENU\n";
    if (style & WS_THICKFRAME) oss << "  WS_THICKFRAME\n";
    if (style & WS_MINIMIZEBOX) oss << "  WS_MINIMIZEBOX\n";
    if (style & WS_MAXIMIZEBOX) oss << "  WS_MAXIMIZEBOX\n";
    if (style & WS_VISIBLE) oss << "  WS_VISIBLE\n";
    if (style & WS_DISABLED) oss << "  WS_DISABLED\n";
    
    oss << "\nExtended Style Breakdown:\n";
    if (ex_style & WS_EX_TOPMOST) oss << "  WS_EX_TOPMOST\n";
    if (ex_style & WS_EX_LAYERED) oss << "  WS_EX_LAYERED\n";
    if (ex_style & WS_EX_TRANSPARENT) oss << "  WS_EX_TRANSPARENT\n";
    if (ex_style & WS_EX_TOOLWINDOW) oss << "  WS_EX_TOOLWINDOW\n";
    if (ex_style & WS_EX_APPWINDOW) oss << "  WS_EX_APPWINDOW\n";
    
    // Get window position and size
    RECT window_rect, client_rect;
    if (GetWindowRect(hwnd, &window_rect) && GetClientRect(hwnd, &client_rect)) {
        oss << "\n=== Window Geometry ===\n";
        oss << "Window Rect: (" << window_rect.left << ", " << window_rect.top 
            << ") to (" << window_rect.right << ", " << window_rect.bottom << ")\n";
        oss << "Window Size: " << (window_rect.right - window_rect.left) 
            << " x " << (window_rect.bottom - window_rect.top) << "\n";
        oss << "Client Size: " << (client_rect.right - client_rect.left) 
            << " x " << (client_rect.bottom - client_rect.top) << "\n";
        
        // Calculate border sizes
        int border_width = (window_rect.right - window_rect.left) - (client_rect.right - client_rect.left);
        int border_height = (window_rect.bottom - window_rect.top) - (client_rect.bottom - client_rect.top);
        oss << "Border Size: " << border_width << " x " << border_height << "\n";
    }
    
    // Get window state
    oss << "\n=== Window State ===\n";
    if (IsWindowVisible(hwnd)) oss << "Visible: Yes\n";
    else oss << "Visible: No\n";
    
    if (IsIconic(hwnd)) oss << "Minimized: Yes\n";
    else if (IsZoomed(hwnd)) oss << "Maximized: Yes\n";
    else oss << "State: Normal\n";
    
    if (IsWindowEnabled(hwnd)) oss << "Enabled: Yes\n";
    else oss << "Enabled: No\n";
    
    // Get window procedure information
    WNDPROC current_proc = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_WNDPROC);
    oss << "\n=== Window Procedure ===\n";
    oss << "Current WNDPROC: 0x" << std::hex << (uintptr_t)current_proc << std::dec << "\n";
    if (current_proc == DefWindowProc) {
        oss << "Status: Default window procedure\n";
    } else {
        oss << "Status: Custom window procedure\n";
    }
    
    if (!info.additional_info.empty()) {
        oss << "\n=== Additional Info ===\n";
        oss << info.additional_info << "\n";
    }
    
    return oss.str();
}

// Get window creation summary for UI display
std::string GetWindowCreationSummary(HWND hwnd) {
    if (hwnd == nullptr) return "No window handle available";
    
    WindowCreationInfo info = DetectWindowCreationInfo(hwnd);
    
    std::ostringstream oss;
    oss << "Window: " << info.window_class << "\n";
    oss << "API: " << info.api_name << "\n";
    oss << "Method: " << info.method_name << "\n";
    oss << "Process: " << info.process_name << "\n";
    
    // Get current styles
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    LONG_PTR ex_style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    
    oss << "Style: 0x" << std::hex << style << std::dec << "\n";
    oss << "ExStyle: 0x" << std::hex << ex_style << std::dec << "\n";
    
    // Highlight fullscreen indicators
    if (style & WS_POPUP) oss << "*** FULLSCREEN INDICATOR: WS_POPUP ***\n";
    if (ex_style & WS_EX_TOPMOST) oss << "*** FULLSCREEN INDICATOR: WS_EX_TOPMOST ***\n";
    if (IsZoomed(hwnd)) oss << "*** FULLSCREEN INDICATOR: Maximized ***\n";
    
    return oss.str();
}

// Install window creation detection hooks
bool InstallWindowCreationDetectionHooks() {
    // Get the addresses of the functions to hook
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        LogWarn("Failed to get user32.dll handle for window creation hooks");
        return false;
    }
    
    // Get CreateWindowExW
    g_original_CreateWindowExW = (HWND (WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID))GetProcAddress(user32, "CreateWindowExW");
    if (!g_original_CreateWindowExW) {
        LogWarn("Failed to get CreateWindowExW address");
        return false;
    }
    
    // Get CreateWindowW
    g_original_CreateWindowW = (HWND (WINAPI*)(LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID))GetProcAddress(user32, "CreateWindowW");
    if (!g_original_CreateWindowW) {
        LogWarn("Failed to get CreateWindowW address");
        return false;
    }
    
    // Get RegisterClassExW
    g_original_RegisterClassExW = (ATOM (WINAPI*)(const WNDCLASSEXW*))GetProcAddress(user32, "RegisterClassExW");
    if (!g_original_RegisterClassExW) {
        LogWarn("Failed to get RegisterClassExW address");
        return false;
    }
    
    // Get SetWindowLongPtrW
    g_original_SetWindowLongPtrW = (LONG_PTR (WINAPI*)(HWND, int, LONG_PTR))GetProcAddress(user32, "SetWindowLongPtrW");
    if (!g_original_SetWindowLongPtrW) {
        LogWarn("Failed to get SetWindowLongPtrW address");
        return false;
    }
    
    // For now, we'll use a simpler approach - just track window creation
    // without actually hooking the functions (which requires more complex memory manipulation)
    // The borderless style enforcement timer will handle applying styles after creation
    
    LogInfo("Window creation detection hooks prepared - using timer-based enforcement");
    
    return true;
}

// Uninstall window creation detection hooks
void UninstallWindowCreationDetectionHooks() {
    g_original_CreateWindowExW = nullptr;
    g_original_CreateWindowW = nullptr;
    g_original_RegisterClassExW = nullptr;
    
    // Clear window history
    std::lock_guard<std::mutex> lock(g_window_creation_mutex);
    g_window_creation_history.clear();
    
    LogInfo("Window creation detection hooks uninstalled");
}

// Track window style changes to detect fullscreen transitions (now uses delayed approach)
void TrackWindowStyleChange(HWND hwnd, LONG_PTR old_style, LONG_PTR new_style, LONG_PTR old_ex_style, LONG_PTR new_ex_style, const std::string& reason) {
    // Schedule delayed style change tracking to prevent locks
    std::thread([hwnd, old_style, new_style, old_ex_style, new_ex_style, reason]() {
        // Small delay to prevent immediate execution
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        
        std::lock_guard<std::mutex> lock(g_window_creation_mutex);
        
        auto it = g_window_creation_history.find(hwnd);
        if (it != g_window_creation_history.end()) {
            WindowCreationInfo& info = it->second;
            
            // Update current styles
            info.current_style = new_style;
            info.current_ex_style = new_ex_style;
            info.last_style_change = std::chrono::steady_clock::now();
            
            // Record the style change
            WindowCreationInfo::StyleChange change;
            change.timestamp = info.last_style_change;
            change.old_style = old_style;
            change.new_style = new_style;
            change.old_ex_style = old_ex_style;
            change.new_ex_style = new_ex_style;
            change.reason = reason;
            
            info.style_changes.push_back(change);
            
            // Check for fullscreen indicators
            bool was_fullscreen = (old_style & WS_POPUP) || (old_ex_style & WS_EX_TOPMOST);
            bool is_fullscreen = (new_style & WS_POPUP) || (new_ex_style & WS_EX_TOPMOST);
            
            if (!was_fullscreen && is_fullscreen) {
                std::ostringstream oss; oss << "FULLSCREEN DETECTED: Window " << info.window_class << " transitioned to fullscreen via " << reason; LogWarn(oss.str().c_str());
                std::ostringstream oss1; oss1 << "Style change: 0x" << std::hex << old_style << " -> 0x" << new_style; LogWarn(oss1.str().c_str());
                std::ostringstream oss2; oss2 << "ExStyle change: 0x" << std::hex << old_ex_style << " -> 0x" << new_ex_style; LogWarn(oss2.str().c_str());
            } else if (was_fullscreen && !is_fullscreen) {
                std::ostringstream oss; oss << "FULLSCREEN EXIT: Window " << info.window_class << " exited fullscreen via " << reason; LogInfo(oss.str().c_str());
            }
        }
    }).detach();
}

// Hook for SetWindowLongPtr to track style changes (now uses delayed approach)
LONG_PTR WINAPI SetWindowLongPtrW_Hook(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
    if (hWnd != nullptr && (nIndex == GWL_STYLE || nIndex == GWL_EXSTYLE)) {
        // Get current styles before change
        LONG_PTR old_style = GetWindowLongPtr(hWnd, GWL_STYLE);
        LONG_PTR old_ex_style = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
        
        // Call original function
        LONG_PTR result = g_original_SetWindowLongPtrW(hWnd, nIndex, dwNewLong);
        
        if (result != 0) {
            // Get new styles after change
            LONG_PTR new_style = GetWindowLongPtr(hWnd, GWL_STYLE);
            LONG_PTR new_ex_style = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
            
            // Track the style change with delay to prevent locks
            std::string reason = (nIndex == GWL_STYLE) ? "SetWindowLongPtr(GWL_STYLE)" : "SetWindowLongPtr(GWL_EXSTYLE)";
            TrackWindowStyleChange(hWnd, old_style, new_style, old_ex_style, new_ex_style, reason);
        }
        
        return result;
    }
    
    // Call original function for other cases
    return g_original_SetWindowLongPtrW(hWnd, nIndex, dwNewLong);
}
