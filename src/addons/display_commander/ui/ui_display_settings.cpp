#include "ui_display_settings.hpp"
#include "ui_common.hpp"
#include "../renodx/settings.hpp"
#include "../addon.hpp"
#include "../dxgi/custom_fps_limiter.hpp"
#include "../dxgi/custom_fps_limiter_manager.hpp"
#include <sstream>

// Global variable declaration
extern std::unique_ptr<renodx::dxgi::fps_limiter::CustomFpsLimiterManager> g_customFpsLimiterManager;

namespace renodx::ui {

void AddDisplaySettings(std::vector<renodx::utils::settings2::Setting*>& settings) {
    // Window width preset slider with labels
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "WindowWidth",
        .binding = &s_windowed_width,
        .value_type = renodx::utils::settings2::SettingValueType::INTEGER,
        .default_value = 3.f, // default to 1920
        .label = "Window Width",
        .section = "Display",
        .labels = {"Current Display", "1280", "1366", "1600", "1920", "2560", "3440", "3840"},
        .parse = [](float index) {
          int i = static_cast<int>(index);
          i = (std::max)(i, 0);
          int max_i = 7; // Now 7 (0-7)
          i = (std::min)(i, max_i);
          
          // If option 0 is selected, return current monitor width
          if (i == 0) {
            return static_cast<float>(GetCurrentMonitorWidth());
          }
          
          return static_cast<float>(WIDTH_OPTIONS[i]);
        },
        .is_visible = []() { return is_basic_tab(s_ui_tab); } // Show in Basic mode
    });

    // Window height preset slider with labels
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "WindowHeight",
        .binding = &s_windowed_height,
        .value_type = renodx::utils::settings2::SettingValueType::INTEGER,
        .default_value = 2.f, // default to 1080
        .label = "Window Height",
        .section = "Display",
        .labels = {"Current Display", "720", "900", "1080", "1200", "1440", "1600", "2160"},
        .is_enabled = [](){ return s_resize_mode < 0.5f; },
        .parse = [](float index) {
          int i = static_cast<int>(index);
          i = (std::max)(i, 0);
          int max_i = 7; // Now 7 (0-7)
          i = (std::min)(i, max_i);
          
          // If option 0 is selected, return current monitor height
          if (i == 0) {
            return static_cast<float>(GetCurrentMonitorHeight());
          }
          
          return static_cast<float>(HEIGHT_OPTIONS[i]);
        },
        .is_visible = []() { return is_basic_tab(s_ui_tab); } // Show in Basic mode
    });

    // Resize mode: width/height vs aspect ratio
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "ResizeMode",
        .binding = &s_resize_mode,
        .value_type = renodx::utils::settings2::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Resize Mode",
        .section = "Display",
        .tooltip = "Choose between manual width/height or aspect ratio-based resizing.",
        .labels = {"Width/Height", "Aspect Ratio"},
        .is_visible = []() { return is_basic_tab(s_ui_tab); } // Show in Basic mode
    });

    // Aspect Ratio (only when in Aspect mode)
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "AspectRatio",
        .binding = &s_aspect_index,
        .value_type = renodx::utils::settings2::SettingValueType::INTEGER,
        .default_value = 3.f,
        .label = "Aspect Ratio",
        .section = "Display",
        .tooltip = "Choose the aspect ratio for window resizing. Only applies when Resize Mode is set to Aspect Ratio.",
        .labels = {"3:2", "4:3", "16:10", "16:9", "19:9", "19.5:9", "21:9", "32:9"},
        .min = 0.f,
        .max = 7.f,
        .is_visible = []() { return is_basic_tab(s_ui_tab); } // Show in Basic mode
    });

    // Target Monitor
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "TargetMonitor",
        .binding = &s_target_monitor_index,
        .value_type = renodx::utils::settings2::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Target Monitor",
        .section = "Display",
        .tooltip = "Choose which monitor to apply size/pos to. 'Auto' uses the current window monitor.",
        .labels = MakeMonitorLabels(),
        .is_visible = []() { return is_basic_tab(s_ui_tab); }, // Show in Basic mode
    });

    // Window alignment when repositioning is needed
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "Alignment",
        .binding = &s_move_to_zero_if_out,
        .value_type = renodx::utils::settings2::SettingValueType::INTEGER,
        .default_value = 2.f, // default to top right
        .label = "Alignment",
        .section = "Display",
        .tooltip = "Choose how to align the window when repositioning is needed. 1=Top Left, 2=Top Right, 3=Bottom Left, 4=Bottom Right, 5=Center.",
        .labels = {"None", "Top Left", "Top Right", "Bottom Left", "Bottom Right", "Center"},
        .is_visible = []() { return is_basic_tab(s_ui_tab); }, // Show in Basic mode
    });

    // Apply Changes button
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "ApplyChanges",
        .value_type = renodx::utils::settings2::SettingValueType::BUTTON,
        .default_value = 0.f,
        .label = "Apply Changes",
        .section = "Display",
        .tooltip = "Apply the current window size and position settings immediately.",
        .on_click = []() -> bool {
            // Force immediate application of window changes
            extern std::atomic<uint64_t> g_init_apply_generation;
            ::g_init_apply_generation.fetch_add(1);
            LogInfo("Apply Changes button clicked - forcing immediate window update");
            return true;
        },
        .is_visible = []() { return is_basic_tab(s_ui_tab); }, // Show in Basic mode
    });

    
    // FPS Limit (Now uses Custom FPS Limiter)
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "FPSLimit",
        .binding = &s_fps_limit,
        .value_type = renodx::utils::settings2::SettingValueType::FLOAT,
        .default_value = 0.f,
        .label = "FPS Limit",
        .section = "Performance",
        .tooltip = "Set FPS limit for the game (0 = no limit). Now uses the new Custom FPS Limiter system.",
        .min = 0.f,
        .max = 300.f,
        .format = "%.2f FPS",
        .is_enabled = []() { return true; }, // Always enabled
        .on_change_value = [](float previous, float current){ 
            if (current > 0.0f) {
                // Custom FPS Limiter is always enabled, just initialize if needed
                if (g_customFpsLimiterManager && g_customFpsLimiterManager->InitializeCustomFpsLimiterSystem()) {
                    LogWarn("Custom FPS Limiter system auto-initialized");
                } else {
                    LogWarn("Failed to initialize Custom FPS Limiter system");
                    return;
                }
                
                // Update the custom FPS limiter
                if (g_customFpsLimiterManager) {
                    auto& limiter = g_customFpsLimiterManager->GetFpsLimiter();
                    limiter.SetTargetFps(current);
                    limiter.SetEnabled(true);
                    
                    std::ostringstream oss;
                    oss << "FPS limit applied: " << static_cast<int>(current) << " FPS (via Custom FPS Limiter)";
                    LogInfo(oss.str().c_str());
                }
            } else {
                // FPS limit set to 0, disable the limiter
                if (g_customFpsLimiterManager) {
                    auto& limiter = g_customFpsLimiterManager->GetFpsLimiter();
                    limiter.SetEnabled(false);
                    LogInfo("FPS limit removed (no limit)");
                }
            }
        },
        .on_draw = []() -> bool {
            // Sync with current atomic value
            s_fps_limit = g_default_fps_limit.load();
            return false;
        },
        .is_visible = []() { return is_basic_tab(s_ui_tab); } // Show in Basic mode
    });





    // Background FPS Limit (Now uses Custom FPS Limiter)
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "FPSLimitBackground",
        .binding = &s_fps_limit_background,
        .value_type = renodx::utils::settings2::SettingValueType::FLOAT,
        .default_value = 30.f,
        .label = "Background FPS Limit",
        .section = "Performance",
        .tooltip = "FPS cap when the game window is not in the foreground. Now uses the new Custom FPS Limiter system.",
        .min = 0.f,
        .max = 240.f,
        .format = "%.2f FPS",
        .is_enabled = []() { return true; }, // Always enabled
        .on_change_value = [](float previous, float current){
            if (current > 0.0f) {
                // Custom FPS Limiter is always enabled, just initialize if needed
                if (g_customFpsLimiterManager && g_customFpsLimiterManager->InitializeCustomFpsLimiterSystem()) {
                    LogWarn("Custom FPS Limiter system auto-initialized");
                } else {
                    LogWarn("Failed to initialize Custom FPS Limiter system");
                    return;
                }
                
                // Apply background FPS limit immediately if currently in background
                HWND hwnd = g_last_swapchain_hwnd.load();
                if (hwnd == nullptr) hwnd = GetForegroundWindow();
                const bool is_background = (hwnd != nullptr && GetForegroundWindow() != hwnd);
                
                if (is_background && current >= 0.f) {
                    if (g_customFpsLimiterManager) {
                        auto& limiter = g_customFpsLimiterManager->GetFpsLimiter();
                        limiter.SetTargetFps(current);
                        limiter.SetEnabled(true);
                        
                        std::ostringstream oss;
                        oss << "Background FPS limit applied immediately: " << static_cast<int>(current) << " FPS (via Custom FPS Limiter)";
                        LogInfo(oss.str().c_str());
                    }
                }
            } else {
                // Background FPS limit set to 0, disable the limiter
                if (g_customFpsLimiterManager) {
                    auto& limiter = g_customFpsLimiterManager->GetFpsLimiter();
                    limiter.SetEnabled(false);
                    LogInfo("Background FPS limit removed (no limit)");
                }
            }
        },
        .is_visible = []() { return is_basic_tab(s_ui_tab); } // Show in Basic mode
    });

}

} // namespace renodx::ui
