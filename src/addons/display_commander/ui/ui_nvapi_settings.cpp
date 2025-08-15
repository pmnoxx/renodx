#include "ui_common.hpp"

// External declarations for NVAPI settings
extern float s_nvapi_fullscreen_prevention;
extern float s_nvapi_hdr_logging;

namespace renodx::ui {

void AddNvapiSettings(std::vector<renodx::utils::settings::Setting*>& settings) {
    // NVAPI Fullscreen Prevention
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "NvapiFullscreenPrevention",
        .binding = &s_nvapi_fullscreen_prevention,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "NVAPI Fullscreen Prevention",
        .section = "NVAPI",
        .tooltip = "Use NVAPI to prevent fullscreen mode at the driver level.",
        .labels = {"Disabled", "Enabled"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
    });

    // NVAPI HDR Logging
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "NvapiHdrLogging",
        .binding = &s_nvapi_hdr_logging,
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "NVAPI HDR Logging",
        .section = "NVAPI",
        .tooltip = "Enable HDR monitor information logging via NVAPI.",
        .labels = {"Disabled", "Enabled"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); },
    });
}

} // namespace renodx::ui
