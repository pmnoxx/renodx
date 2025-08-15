#include "ui_common.hpp"

// External declarations for audio settings
extern float s_audio_volume_percent;
extern float s_audio_mute;
extern float s_mute_in_background;
extern float s_fps_limit_background;

namespace renodx::ui {

void AddAudioSettings(std::vector<renodx::utils::settings::Setting*>& settings) {
    // Audio Volume
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "AudioVolume",
        .binding = &s_audio_volume_percent,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 100.f,
        .label = "Audio Volume",
        .section = "Audio",
        .tooltip = "Set the audio volume percentage.",
        .min = 0.f,
        .max = 100.f,
        .format = "%d%%",
        .is_visible = []() { return is_basic_tab(s_ui_mode); },
    });

    // Audio Mute
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "AudioMute",
        .binding = &s_audio_mute,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Audio Mute",
        .section = "Audio",
        .tooltip = "Mute/unmute audio.",
        .labels = {"Unmuted", "Muted"},
        .is_visible = []() { return is_basic_tab(s_ui_mode); },
    });

    // Mute in Background
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "MuteInBackground",
        .binding = &s_mute_in_background,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Mute in Background",
        .section = "Audio",
        .tooltip = "Mute audio when application loses focus.",
        .labels = {"Disabled", "Enabled"},
        .is_visible = []() { return is_basic_tab(s_ui_mode); },
    });

    // FPS Limit in Background
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "FPSLimitInBackground",
        .binding = &s_fps_limit_background,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 30.f,
        .label = "FPS Limit in Background",
        .section = "Audio",
        .tooltip = "Set FPS limit when application is in background.",
        .min = 1.f,
        .max = 300.f,
        .format = "%d FPS",
        .is_visible = []() { return is_basic_tab(s_ui_mode); },
    });
}

} // namespace renodx::ui
