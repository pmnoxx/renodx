#pragma once

#include "../addon.hpp"
#include <vector>

// External declarations for settings


extern float s_reflex_debug_output;

// Tab labels constant - update this list to automatically update the UI mode slider
const std::vector<std::string> TAB_LABELS = {
    "Simple",      // 0 - Basic settings
    "Display",     // 1 - Desktop resolution override
    "Developer",   // 2 - Advanced settings
    "Device Info", // 3 - Device information
    "Window Info", // 4 - Window debugging
    
};

// Helper functions for tab visibility
inline bool is_basic_tab(float ui_mode) { return static_cast<int>(ui_mode) == 0; }
inline bool is_display_tab(float ui_mode) { return static_cast<int>(ui_mode) == 1; }
inline bool is_developer_tab(float ui_mode) { return static_cast<int>(ui_mode) == 2; }
inline bool is_device_tab(float ui_mode) { return static_cast<int>(ui_mode) == 3; }
inline bool is_window_info_tab(float ui_mode) { return static_cast<int>(ui_mode) == 4; }

inline bool is_main_tab(float ui_mode) { return static_cast<int>(ui_mode) < 3; } // Show in Simple (0), Display (1), and Developer (2) modes, but not Device Info (3) or Window Info (4)

// Forward declarations for settings sections
namespace renodx::ui {
    void AddGeneralSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddDisplaySettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddDisplayTabSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddAudioSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddWindowControls(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddDeveloperSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddWindowPositionSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddHdrColorspaceSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddNvapiSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddDeviceInfoSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddWindowInfoSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddDxgiDeviceInfoDetailedSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddDxgiCompositionInfoSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddIndependentFlipFailuresSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddReflexSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddNvapiSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddDxgiDeviceInfoSettings(std::vector<renodx::utils::settings::Setting*>& settings);
}
