#include "window_state_calculator.hpp"
#include "addon.hpp"
#include "utils.hpp"
#include <sstream>
#include <algorithm>

namespace renodx::window {

std::string DesiredWindowState::to_string() const {
    std::ostringstream oss;
    oss << "DesiredWindowState{";
    oss << "width=" << width << ", height=" << height;
    oss << ", pos_x=" << pos_x << ", pos_y=" << pos_y;
    oss << ", style_mode=" << static_cast<int>(style_mode);
    oss << ", should_resize=" << (should_resize ? "true" : "false");
    oss << ", should_move=" << (should_move ? "true" : "false");
    oss << ", should_change_style=" << (should_change_style ? "true" : "false");
    oss << ", reason=\"" << reason << "\"";
    oss << "}";
    return oss.str();
}

DesiredWindowState CalculateDesiredSize(HWND hwnd) {
    DesiredWindowState state;
    state.reason = "CalculateDesiredSize";
    
    // Get current window size
    RECT window_rect;
    if (!GetClientRect(hwnd, &window_rect)) {
        LogWarn("CalculateDesiredSize: Failed to get window rect");
        return state;
    }
    
    int current_w = window_rect.right - window_rect.left;
    int current_h = window_rect.bottom - window_rect.top;
    
    // Get desired size from settings (global namespace)
    extern float s_windowed_width, s_windowed_height;
    int desired_w = static_cast<int>(::s_windowed_width);
    int desired_h = static_cast<int>(::s_windowed_height);
    
    // Validate desired size
    if (desired_w <= 0 || desired_h <= 0) {
        LogWarn("CalculateDesiredSize: Invalid desired size from settings");
        return state;
    }
    
    // Check if resize is needed
    bool needs_resize = (current_w != desired_w || current_h != desired_h);
    
    if (needs_resize) {
        state.width = desired_w;
        state.height = desired_h;
        state.should_resize = true;
        
        std::ostringstream oss;
        oss << "Resize needed: " << current_w << "x" << current_h << " -> " << desired_w << "x" << desired_h;
        LogInfo(oss.str().c_str());
    } else {
        state.width = current_w;
        state.height = current_h;
        state.should_resize = false;
        LogDebug("CalculateDesiredSize: No resize needed");
    }
    
    return state;
}

DesiredWindowState CalculateDesiredPosition(HWND hwnd) {
    DesiredWindowState state;
    state.reason = "CalculateDesiredPosition";
    
    // Get current window position
    RECT window_rect;
    if (!GetWindowRect(hwnd, &window_rect)) {
        LogWarn("CalculateDesiredPosition: Failed to get window rect");
        return state;
    }
    
    int current_x = window_rect.left;
    int current_y = window_rect.top;
    
    // Get desired position from settings (global namespace)
    extern float s_windowed_pos_x, s_windowed_pos_y;
    int desired_x = static_cast<int>(::s_windowed_pos_x);
    int desired_y = static_cast<int>(::s_windowed_pos_y);
    
    // Check if move is needed
    bool needs_move = (current_x != desired_x || current_y != desired_y);
    
    if (needs_move) {
        state.pos_x = desired_x;
        state.pos_y = desired_y;
        state.should_move = true;
        
        std::ostringstream oss;
        oss << "Move needed: (" << current_x << "," << current_y << ") -> (" << desired_x << "," << desired_y << ")";
        LogInfo(oss.str().c_str());
    } else {
        state.pos_x = current_x;
        state.pos_y = current_y;
        state.should_move = false;
        LogDebug("CalculateDesiredPosition: No move needed");
    }
    
    return state;
}

DesiredWindowState CalculateDesiredStyle(HWND hwnd) {
    DesiredWindowState state;
    state.reason = "CalculateDesiredStyle";
    
    // Get current window styles
    LONG_PTR current_style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR current_ex_style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    
    // Get desired style mode from settings (global namespace)
    extern float s_remove_top_bar;
    WindowStyleMode desired_mode = (::s_remove_top_bar >= 0.5f) ? WindowStyleMode::BORDERLESS : WindowStyleMode::OVERLAPPED_WINDOW;
    
    // Check if style change is needed
    bool needs_style_change = false;
    
    if (desired_mode == WindowStyleMode::BORDERLESS) {
        // Check if title bar and borders are present
        bool has_title_bar = (current_style & WS_CAPTION) != 0;
        bool has_borders = (current_style & (WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU)) != 0;
        bool has_ex_borders = (current_ex_style & (WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)) != 0;
        
        needs_style_change = has_title_bar || has_borders || has_ex_borders;
    }
    
    if (needs_style_change) {
        state.style_mode = desired_mode;
        state.should_change_style = true;
        
        std::ostringstream oss;
        oss << "Style change needed: Current style: 0x" << std::hex << current_style 
            << ", Current ex_style: 0x" << current_ex_style 
            << " -> Desired mode: " << (desired_mode == WindowStyleMode::BORDERLESS ? "BORDERLESS" : "OVERLAPPED_WINDOW");
        LogInfo(oss.str().c_str());
    } else {
        state.style_mode = desired_mode;
        state.should_change_style = false;
        LogDebug("CalculateDesiredStyle: No style change needed");
    }
    
    return state;
}

DesiredWindowState CalculateDesiredWindowState(HWND hwnd, const std::string& context) {
    DesiredWindowState state;
    state.reason = "CalculateDesiredWindowState: " + context;
    
    if (hwnd == nullptr) {
        LogWarn("CalculateDesiredWindowState: Invalid HWND");
        return state;
    }
    
    // Calculate all desired states
    DesiredWindowState size_state = CalculateDesiredSize(hwnd);
    DesiredWindowState pos_state = CalculateDesiredPosition(hwnd);
    DesiredWindowState style_state = CalculateDesiredStyle(hwnd);
    
    // Combine the states
    state.width = size_state.width;
    state.height = size_state.height;
    state.pos_x = pos_state.pos_x;
    state.pos_y = pos_state.pos_y;
    state.style_mode = style_state.style_mode;
    state.should_resize = size_state.should_resize;
    state.should_move = pos_state.should_move;
    state.should_change_style = style_state.should_change_style;
    
    // Log the combined state
    std::ostringstream oss;
    oss << "Combined desired state: " << state.to_string();
    LogDebug(oss.str().c_str());
    
    return state;
}

bool ValidateWindowState(const DesiredWindowState& state, HWND hwnd) {
    if (!state.is_valid()) {
        LogWarn("ValidateWindowState: Invalid state - missing dimensions");
        return false;
    }
    
    if (hwnd == nullptr) {
        LogWarn("ValidateWindowState: Invalid HWND");
        return false;
    }
    
    // Validate dimensions
    if (state.width <= 0 || state.height <= 0) {
        LogWarn("ValidateWindowState: Invalid dimensions");
        return false;
    }
    
    // Validate position (allow negative for multi-monitor setups)
    // Additional validation could be added here
    
    return true;
}

bool IsWindowStateChangeAllowed(const DesiredWindowState& current, const DesiredWindowState& desired) {
    // Check if the change is reasonable
    if (desired.should_resize) {
        // Check if the size change is reasonable (e.g., not too extreme)
        int size_diff_w = abs(desired.width - current.width);
        int size_diff_h = abs(desired.height - current.height);
        
        // If size change is more than 50% of current size, log a warning
        if (current.width > 0 && size_diff_w > current.width / 2) {
            LogWarn("IsWindowStateChangeAllowed: Large width change detected");
        }
        if (current.height > 0 && size_diff_h > current.height / 2) {
            LogWarn("IsWindowStateChangeAllowed: Large height change detected");
        }
    }
    
    return true; // For now, allow all changes but log warnings
}

void LogWindowStateError(const std::string& context, const DesiredWindowState& state, const std::string& error) {
    std::ostringstream oss;
    oss << "WindowStateError [" << context << "]: " << error << " - State: " << state.to_string();
    LogWarn(oss.str().c_str());
}

void LogWindowStateChange(const std::string& context, const DesiredWindowState& from, const DesiredWindowState& to) {
    std::ostringstream oss;
    oss << "WindowStateChange [" << context << "]: " << from.to_string() << " -> " << to.to_string();
    LogInfo(oss.str().c_str());
}

} // namespace renodx::window
