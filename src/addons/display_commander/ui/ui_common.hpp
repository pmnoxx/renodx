#pragma once

#include "../addon.hpp"
#include <vector>

// External declarations for settings


extern float s_reflex_debug_output;

// Tab labels constant - update this list to automatically update the UI mode slider
const std::vector<std::string> TAB_LABELS = {
    "Simple",      // 0 - Basic settings
    "Developer",   // 1 - Advanced settings
    "Device Info", // 2 - Device information
    "Window Info", // 3 - Window debugging
    "Swapchain",   // 4 - Swapchain information
    
};

// Helper functions for tab visibility
inline bool is_basic_tab(float ui_tab) { return static_cast<int>(ui_tab) == 0; }
inline bool is_developer_tab(float ui_tab) { return static_cast<int>(ui_tab) == 1; }
inline bool is_device_tab(float ui_tab) { return static_cast<int>(ui_tab) == 2; }
inline bool is_window_info_tab(float ui_tab) { return static_cast<int>(ui_tab) == 3; }
inline bool is_swapchain_tab(float ui_tab) { return static_cast<int>(ui_tab) == 4; }


// Forward declarations for settings sections
namespace renodx::ui {
    void AddGeneralSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddDisplaySettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddDisplayTabSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddAudioSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddWindowControls(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddDeveloperSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddWindowPositionSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddHdrColorspaceSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddNvapiSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddDeviceInfoSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddWindowInfoSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddDxgiDeviceInfoDetailedSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddDxgiCompositionInfoSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddIndependentFlipFailuresSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddReflexSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddNvapiSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddDxgiDeviceInfoSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
}
