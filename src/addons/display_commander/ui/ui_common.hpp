#pragma once

#include "../addon.hpp"
#include <vector>
#include "new_ui/new_ui_tabs.hpp"

// External declarations for settings


extern float s_reflex_debug_output;





// Helper functions for tab visibility
// These functions now only use the new UI system
inline bool is_basic_tab(float ui_tab = 0) { 
    // Check if new UI Main tab (index 0) is selected
    // When Main tab is active, show all basic settings (old UI)
    return renodx::ui::new_ui::g_tab_manager.GetActiveTab() == 0;
}
inline bool is_developer_tab(float ui_tab = 0) { 
    // Check if new UI Developer tab (index 1) is selected
    return renodx::ui::new_ui::g_tab_manager.GetActiveTab() == 1;
}
inline bool is_device_tab(float ui_tab = 0) { 
    // Check if new UI Device Info tab (index 2) is selected
    return renodx::ui::new_ui::g_tab_manager.GetActiveTab() == 2;
}
inline bool is_window_info_tab(float ui_tab = 0) { 
    // Check if new UI Window Info tab (index 3) is selected
    return renodx::ui::new_ui::g_tab_manager.GetActiveTab() == 3;
}
inline bool is_swapchain_tab(float ui_tab = 0) { 
    // Check if new UI Swapchain tab (index 4) is selected
    return renodx::ui::new_ui::g_tab_manager.GetActiveTab() == 4;
}


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
    void AddDxgiDeviceInfoSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
}
