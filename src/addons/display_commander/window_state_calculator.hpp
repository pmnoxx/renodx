#pragma once

#include <windows.h>
#include <string>
#include "addon.hpp" // For WindowStyleMode enum

namespace renodx::window {

// Structure to hold the complete desired window state
struct DesiredWindowState {
    int width = 0;
    int height = 0;
    int pos_x = 0;
    int pos_y = 0;
    WindowStyleMode style_mode = WindowStyleMode::KEEP;
    bool should_resize = false;
    bool should_move = false;
    bool should_change_style = false;
    std::string reason = "";
    
    // Helper methods
    bool is_valid() const { return width > 0 && height > 0; }
    std::string to_string() const;
};

// Functions to calculate desired window state
DesiredWindowState CalculateDesiredWindowState(HWND hwnd, const std::string& context = "");
DesiredWindowState CalculateDesiredSize(HWND hwnd);
DesiredWindowState CalculateDesiredPosition(HWND hwnd);
DesiredWindowState CalculateDesiredStyle(HWND hwnd);

// Validation functions
bool ValidateWindowState(const DesiredWindowState& state, HWND hwnd);
bool IsWindowStateChangeAllowed(const DesiredWindowState& current, const DesiredWindowState& desired);

// Error reporting
void LogWindowStateError(const std::string& context, const DesiredWindowState& state, const std::string& error);
void LogWindowStateChange(const std::string& context, const DesiredWindowState& from, const DesiredWindowState& to);

} // namespace renodx::window
