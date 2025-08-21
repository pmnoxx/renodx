#include "ui_hdr_colorspace_settings.hpp"
#include "ui_common.hpp"
#include "../renodx/settings.hpp"

namespace renodx::ui {

void AddHdrColorspaceSettings(std::vector<renodx::utils::settings2::Setting*>& settings) {
    // HDR10 Colorspace Fix
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "Hdr10ColorspaceFix",
        .binding = &s_fix_hdr10_colorspace,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Fix HDR10 Colorspace",
        .section = "HDR",
        .tooltip = "Automatically fix HDR10 colorspace when swapchain format is RGB10A2 and colorspace is currently sRGB. Only works when the game is using sRGB colorspace.",
        .labels = {"Off", "On"},
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Show in Developer mode
    });
}

} // namespace renodx::ui
