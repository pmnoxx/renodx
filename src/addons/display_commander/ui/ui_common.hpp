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


// Forward declarations for settings sections
namespace renodx::ui {
    void AddGeneralSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddDisplaySettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddDisplayTabSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddAudioSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
    void AddWindowControls(std::vector<renodx::utils::settings2::Setting*>& settings);
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
