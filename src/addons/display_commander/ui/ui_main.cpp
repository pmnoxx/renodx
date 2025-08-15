#include "ui_common.hpp"
#include "ui_general_settings.hpp"
#include "ui_display_settings.hpp"
#include "ui_audio_settings.hpp"
#include "ui_window_controls.hpp"
#include "ui_nvapi_settings.hpp"
#include "ui_device_info.hpp"
#include "ui_window_info.hpp"

// UI/settings - main vector that includes all sections
renodx::utils::settings::Settings settings = {};

// Initialize all settings sections
void InitializeUISettings() {
    // Add all settings sections
    renodx::ui::AddGeneralSettings(settings);
    renodx::ui::AddDisplaySettings(settings);
    renodx::ui::AddAudioSettings(settings);
    renodx::ui::AddWindowControls(settings);
    renodx::ui::AddNvapiSettings(settings);
    renodx::ui::AddDeviceInfoSettings(settings);
    renodx::ui::AddWindowInfoSettings(settings);
}

