#pragma once

#include "../addon.hpp"
#include <vector>

// External declarations for settings


extern float s_reflex_debug_output;

// Helper functions for tab visibility
inline bool is_basic_tab(float ui_mode) { return ui_mode < 0.5f; }
inline bool is_developer_tab(float ui_mode) { return ui_mode >= 0.5f && ui_mode < 2.0f; }
inline bool is_device_tab(float ui_mode) { return ui_mode >= 2.0f && ui_mode < 3.0f; }
inline bool is_window_info_tab(float ui_mode) { return ui_mode >= 3.0f; }
inline bool is_main_tab(float ui_mode) { return ui_mode < 2.0f; } // Show in Basic and Developer modes, but not Device Info or Window Info

// Forward declarations for settings sections
namespace renodx::ui {
    void AddGeneralSettings(std::vector<renodx::utils::settings::Setting*>& settings);
    void AddDisplaySettings(std::vector<renodx::utils::settings::Setting*>& settings);
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
}
