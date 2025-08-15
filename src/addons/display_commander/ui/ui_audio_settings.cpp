#include "ui_audio_settings.hpp"
#include "ui_common.hpp"
#include "../../../utils/settings.hpp"

namespace renodx::ui {

void AddAudioSettings(std::vector<renodx::utils::settings::Setting*>& settings) {
    // Volume (0-100)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "AudioVolume",
        .binding = &s_audio_volume_percent,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 100.f,
        .label = "Audio Volume (%)",
        .section = "Audio",
        .tooltip = "Master audio volume control (0-100%).",
        .min = 0.f,
        .max = 100.f,
        .format = "%d%%",
        .is_visible = []() { return is_basic_tab(s_ui_mode); }, // Show in Basic mode
    });

    // Mute (manual)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "AudioMute",
        .binding = &s_audio_mute,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Audio Mute",
        .section = "Audio",
        .tooltip = "Manually mute/unmute audio.",
        .labels = {"Unmuted", "Muted"},
        .on_change_value = [](float previous, float current){
            // Reset applied flag so the monitor thread reapplies desired state
            ::g_muted_applied.store(false);
        },
        .is_visible = []() { return is_basic_tab(s_ui_mode); }, // Show in Basic mode
    });

    // Mute in Background (placed after Mute; disabled if Mute is ON)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "MuteInBackground",
        .binding = &s_mute_in_background,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Mute In Background",
        .section = "Audio",
        .tooltip = "Mute the game's audio when it is not the foreground window.",
        .labels = {"Off", "On"},
        .on_change_value = [](float previous, float current){
          // Reset applied flag so the monitor thread reapplies desired state
          ::g_muted_applied.store(false);
        },
        .is_visible = [](){ return is_basic_tab(s_ui_mode); }, // Show in Basic mode
    });


}

} // namespace renodx::ui
