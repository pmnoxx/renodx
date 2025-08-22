#pragma once

#include "settings_wrapper.hpp"
#include <vector>

namespace renodx::ui::new_ui {

// Settings manager for the main new tab
class MainNewTabSettings {
public:
    MainNewTabSettings();
    ~MainNewTabSettings() = default;
    
    // Load all settings from Reshade config
    void LoadSettings();
    
    // Get all settings for loading
    std::vector<SettingBase*> GetAllSettings();
    
    // Display Settings
    ComboSetting window_width;
    ComboSetting window_height;
    ComboSetting resize_mode;
    ComboSetting aspect_index;
    ComboSetting target_monitor_index;
    BoolSetting remove_top_bar;
    ComboSetting alignment;
    
    // FPS Settings
    FloatSetting fps_limit;
    FloatSetting fps_limit_background;
    
    // Audio Settings
    FloatSetting audio_volume_percent;
    BoolSetting audio_mute;
    BoolSetting mute_in_background;
    
    // Reflex Settings
    BoolSetting reflex_enabled;

private:
    std::vector<SettingBase*> all_settings_;
};

// Global instance
extern MainNewTabSettings g_main_new_tab_settings;

} // namespace renodx::ui::new_ui
