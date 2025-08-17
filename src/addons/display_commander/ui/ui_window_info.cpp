#include "ui_window_info.hpp"
#include "ui_common.hpp"
#include "../window_management/window_state_calculator.hpp"
#include "../addon.hpp"
#include "../window_management/window_management.hpp"

// External declarations for window info
extern std::atomic<HWND> g_last_swapchain_hwnd;
extern std::atomic<int> g_last_backbuffer_width;
extern std::atomic<int> g_last_backbuffer_height;

namespace renodx::ui {

void AddWindowInfoSettings(std::vector<renodx::utils::settings::Setting*>& settings) {
    // Window Information Section
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "WindowInfoHeader",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "=== Window Information ===",
        .section = "Window Info",
        .tooltip = "Current window state and information",
        .on_draw = []() {
            // Get current window info
            HWND hwnd = g_last_swapchain_hwnd.load();
            int bb_w = g_last_backbuffer_width.load();
            int bb_h = g_last_backbuffer_height.load();
            
            if (hwnd != nullptr) {
                // Get current window styles
                LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
                LONG_PTR ex_style = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
                
                // Get window rect
                RECT window_rect, client_rect;
                GetWindowRect(hwnd, &window_rect);
                GetClientRect(hwnd, &client_rect);
                
                // Display window information
                ImGui::Text("Window Handle: %p", hwnd);
                ImGui::Text("Window Rect: (%ld,%ld) to (%ld,%ld)", 
                           window_rect.left, window_rect.top, 
                           window_rect.right, window_rect.bottom);
                ImGui::Text("Client Rect: (%ld,%ld) to (%ld,%ld)", 
                           client_rect.left, client_rect.top, 
                           client_rect.right, client_rect.bottom);
                ImGui::Text("Window Size: %ldx%ld", 
                           window_rect.right - window_rect.left, 
                           window_rect.bottom - window_rect.top);
                ImGui::Text("Client Size: %ldx%ld", 
                           client_rect.right - client_rect.left, 
                           client_rect.bottom - client_rect.top);
                
                ImGui::Separator();
                ImGui::Text("Backbuffer Size: %dx%d", bb_w, bb_h);
                
                ImGui::Separator();
                ImGui::Text("Window Styles:");
                ImGui::Text("  Style: 0x%08X", static_cast<unsigned int>(style));
                ImGui::Text("  ExStyle: 0x%08X", static_cast<unsigned int>(ex_style));
                
                // Style analysis
                bool has_caption = (style & WS_CAPTION) != 0;
                bool has_border = (style & WS_BORDER) != 0;
                bool has_thickframe = (style & WS_THICKFRAME) != 0;
                bool has_minimizebox = (style & WS_MINIMIZEBOX) != 0;
                bool has_maximizebox = (style & WS_MAXIMIZEBOX) != 0;
                bool has_sysmenu = (style & WS_SYSMENU) != 0;
                bool is_popup = (style & WS_POPUP) != 0;
                bool is_child = (style & WS_CHILD) != 0;
                
                ImGui::Text("  Has Caption: %s", has_caption ? "Yes" : "No");
                ImGui::Text("  Has Border: %s", has_border ? "Yes" : "No");
                ImGui::Text("  Has ThickFrame: %s", has_thickframe ? "Yes" : "No");
                ImGui::Text("  Has MinimizeBox: %s", has_minimizebox ? "Yes" : "No");
                ImGui::Text("  Has MaximizeBox: %s", has_maximizebox ? "Yes" : "No");
                ImGui::Text("  Has SysMenu: %s", has_sysmenu ? "Yes" : "No");
                ImGui::Text("  Is Popup: %s", is_popup ? "Yes" : "No");
                ImGui::Text("  Is Child: %s", is_child ? "Yes" : "No");
                
                // Window state
                bool is_visible = IsWindowVisible(hwnd) != FALSE;
                bool is_iconic = IsIconic(hwnd) != FALSE;
                bool is_zoomed = IsZoomed(hwnd) != FALSE;
                bool is_enabled = IsWindowEnabled(hwnd) != FALSE;
                
                ImGui::Separator();
                ImGui::Text("Window State:");
                ImGui::Text("  Visible: %s", is_visible ? "Yes" : "No");
                ImGui::Text("  Iconic (Minimized): %s", is_iconic ? "Yes" : "No");
                ImGui::Text("  Zoomed (Maximized): %s", is_zoomed ? "Yes" : "No");
                ImGui::Text("  Enabled: %s", is_enabled ? "Yes" : "No");
                
                // Calculate desired state
                auto desired_state = renodx::window::CalculateDesiredWindowState(hwnd, "ui_display");
                if (desired_state.is_valid()) {
                    ImGui::Separator();
                    ImGui::Text("Desired State:");
                    ImGui::Text("  Size: %dx%d", desired_state.width, desired_state.height);
                    ImGui::Text("  Position: (%d,%d)", desired_state.pos_x, desired_state.pos_y);
                }
                
                // Global Window State Information
                ImGui::Separator();
                ImGui::Text("=== Global Window State ===");
                
                // Get current global window state (already declared in addon.hpp)
                
                ImGui::Text("Current State:");
                ImGui::Text("  Is Maximized: %s", g_window_state.is_maximized ? "YES" : "No");
                ImGui::Text("  Is Minimized: %s", g_window_state.is_minimized ? "YES" : "No");
                ImGui::Text("  Is Restored: %s", g_window_state.is_restored ? "YES" : "No");
                
                // Additional window properties that affect mouse behavior
                bool is_topmost = (GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
                bool is_layered = (GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_LAYERED) != 0;
                bool is_transparent = (GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_TRANSPARENT) != 0;
                
                ImGui::Separator();
                ImGui::Text("Window Properties (Mouse Behavior):");
                ImGui::Text("  Always On Top: %s", is_topmost ? "YES" : "No");
                ImGui::Text("  Layered: %s", is_layered ? "YES" : "No");
                ImGui::Text("  Transparent: %s", is_transparent ? "YES" : "No");
                
                // Check for mouse confinement properties
                bool has_system_menu = (GetWindowLongPtr(hwnd, GWL_STYLE) & WS_SYSMENU) != 0;
                bool has_minimize_box = (GetWindowLongPtr(hwnd, GWL_STYLE) & WS_MINIMIZEBOX) != 0;
                bool has_maximize_box = (GetWindowLongPtr(hwnd, GWL_STYLE) & WS_MAXIMIZEBOX) != 0;
                
                ImGui::Text("Mouse & Input Properties:");
                ImGui::Text("  System Menu: %s", has_system_menu ? "YES" : "No");
                ImGui::Text("  Minimize Box: %s", has_minimize_box ? "YES" : "No");
                ImGui::Text("  Maximize Box: %s", has_maximize_box ? "YES" : "No");
                
                // Check for cursor confinement and focus
                bool is_foreground = (GetForegroundWindow() == hwnd);
                bool is_active = (GetActiveWindow() == hwnd);
                bool is_focused = (GetFocus() == hwnd);
                
                ImGui::Text("Focus & Input State:");
                ImGui::Text("  Is Foreground: %s", is_foreground ? "YES" : "No");
                ImGui::Text("  Is Active: %s", is_active ? "YES" : "No");
                ImGui::Text("  Is Focused: %s", is_focused ? "YES" : "No");
                
                // Check for cursor confinement
                POINT cursor_pos;
                GetCursorPos(&cursor_pos);
                bool cursor_in_window = (cursor_pos.x >= window_rect.left && cursor_pos.x <= window_rect.right &&
                                       cursor_pos.y >= window_rect.top && cursor_pos.y <= window_rect.bottom);
                
                ImGui::Text("Cursor Information:");
                ImGui::Text("  Cursor Pos: (%ld, %ld)", cursor_pos.x, cursor_pos.y);
                ImGui::Text("  Cursor In Window: %s", cursor_in_window ? "YES" : "No");
                ImGui::Text("  Window Bounds: (%ld,%ld) to (%ld,%ld)", 
                           window_rect.left, window_rect.top, window_rect.right, window_rect.bottom);
                
                ImGui::Separator();
                ImGui::Text("Target State:");
                ImGui::Text("  Target Size: %dx%d", g_window_state.target_w, g_window_state.target_h);
                ImGui::Text("  Target Position: (%d,%d)", g_window_state.target_x, g_window_state.target_y);
                
                ImGui::Text("Change Requirements:");
                ImGui::Text("  Needs Resize: %s", g_window_state.needs_resize ? "YES" : "No");
                ImGui::Text("  Needs Move: %s", g_window_state.needs_move ? "YES" : "No");
                ImGui::Text("  Style Changed: %s", g_window_state.style_changed ? "YES" : "No");
                
                ImGui::Text("Style Mode: %s", 
                    g_window_state.style_mode == WindowStyleMode::BORDERLESS ? "BORDERLESS" :
                    g_window_state.style_mode == WindowStyleMode::OVERLAPPED_WINDOW ? "WINDOWED" :
                    "KEEP");
                
                ImGui::Text("Last Reason: %s", g_window_state.reason.c_str());
                
                // Add a refresh button
                if (ImGui::Button("Refresh Global State")) {
                    // Force a recalculation of the global window state
                    CalculateWindowState(hwnd, "ui_refresh");
                }
                ImGui::SameLine();
                if (ImGui::Button("Apply Changes")) {
                    // Force application of the current global window state
                    ApplyWindowChange(hwnd, "ui_apply");
                }
            } else {
                ImGui::Text("No window available");
            }
            
            return true;
        }
    });

    // Refresh Window Info Button
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "RefreshWindowInfo",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .default_value = 0.f,
        .label = "Refresh Window Info",
        .section = "Window Info",
        .tooltip = "Refresh window information display",
        .on_click = []() -> bool {
            // Force refresh of window information
            LogInfo("Window information refreshed");
            return true;
        }
    });

    // Window Debug Mode
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "WindowDebugMode",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Window Debug Mode",
        .section = "Window Info",
        .tooltip = "Enable detailed window debugging information",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current) {
            if (current >= 0.5f) {
                LogInfo("Window debug mode enabled");
            } else {
                LogInfo("Window debug mode disabled");
            }
        },
        .is_visible = []() { return is_window_info_tab(s_ui_mode); }
    });
}

} // namespace renodx::ui
