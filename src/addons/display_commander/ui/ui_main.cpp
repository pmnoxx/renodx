#include "ui_common.hpp"
#include "ui_general_settings.hpp"
#include "ui_display_settings.hpp"
#include "ui_audio_settings.hpp"
#include "ui_window_controls.hpp"
#include "ui_nvapi_settings.hpp"
#include "ui_device_info.hpp"
#include "ui_window_info.hpp"
#include "../ui_settings.hpp"
#include "../settings.hpp"

// UI/settings - main vector that includes all sections
renodx::utils::settings::Settings settings = {};

// Flag to prevent multiple initializations
static bool ui_settings_initialized = false;

// Initialize all settings sections
void InitializeUISettings() {
    // Prevent multiple initializations
    if (ui_settings_initialized) {
        return;
    }
    
    // Clear any existing settings to prevent duplicates
    settings.clear();
    
    // Add all settings sections
    renodx::ui::AddGeneralSettings(settings);
    renodx::ui::AddDisplaySettings(settings);
    renodx::ui::AddAudioSettings(settings);
    renodx::ui::AddWindowControls(settings);
    renodx::ui::AddNvapiSettings(settings);
    renodx::ui::AddDeviceInfoSettings(settings);
    renodx::ui::AddWindowInfoSettings(settings);
    
    // Add UI settings
    AddUISettings(settings);
    
    // Mark as initialized
    ui_settings_initialized = true;
}

