#include <string>
#include <unordered_map>
#include <vector>
#include "../../utils/settings.hpp"
#include "./shared.h"

namespace hbr_custom_settings {

// Map of setting key to default value
static const std::unordered_map<std::string, float> default_values = {
    {"SettingsMode", 0.f},
    {"ToneMapType", 3.f},
    {"ToneMapPeakNits", 1000.f},
    {"ToneMapGameNits", 203.f},
    {"ToneMapUINits", 203.f},
    {"GammaCorrection", 0.f},
    {"ToneMapScaling", 0.f},
    {"ToneMapWorkingColorSpace", 0.f},
    {"ToneMapHueProcessor", 0.f},
    {"ToneMapHueCorrection", 100.f},
    {"ToneMapHueShift", 50.f},
    {"ToneMapClampColorSpace", 2.f},
    {"ToneMapClampPeak", 0.f},
    {"ColorGradeExposure", 1.f},
    {"ColorGradeHighlights", 50.f},
    {"ColorGradeShadows", 50.f},
    {"ColorGradeContrast", 50.f},
    {"ColorGradeSaturation", 50.f},
    {"ColorGradeHighlightSaturation", 50.f},
    {"ColorGradeBlowout", 0.f},
    {"ColorGradeFlare", 0.f},
    {"ColorGradeScene", 100.f},
    {"ColorGradePerChannelBlowoutRestoration", 100.f},
    {"ColorGradePerChannelHueCorrection", 100.f},
    {"ColorGradePerChannelChrominanceCorrection", 100.f},
    {"ColorGradeClip", 65.f},
    {"ColorGradeGamma", 50.f},
    {"CustomBloom2D", 100.f},
    {"CustomBloom3D", 100.f},
    {"CustomTextBrightnessCoef", 1.f},
    {"EnableUIToneMapPass", 0.f},
    {"SwapChainCustomColorSpace", 0.f},
    {"IntermediateEncoding", 2.f},
    {"SwapChainDecoding", 3.f},
    {"SwapChainGammaCorrection", 0.f},
    {"SwapChainClampColorSpace", 2.f},
    {"EffectSplitMode", 0.f},
    {"EffectSplitX", 1.f},
    {"EffectSplitAngle", 0.f},
    {"PerceptualBoostMode", 0.f},
    {"PerceptualBoostChannelMax", 2.f},
    {"PerceptualBoostReinhardStrength", 100.f},
    {"PerceptualBoostXYPQParam", 268.f},
    {"PerceptualBoostXYPQColor", 20.f},
    {"PerceptualBoostXYPQStrength", 100.f},
    {"PerceptualBoostICTCPParam", 268.f},
    {"PerceptualBoostICTCPColor", 20.f},
    {"PerceptualBoostICTCPStrength", 100.f},
    {"PerceptualBoost2DCharacter", 0.3f},
    {"PerceptualBoost2DBackground", 1.f},
    {"PerceptualBoost3D", 0.f},
    {"DebugMode", 0.f},
    {"DebugMode2", 1.f},
    {"DebugMode3", 1.f},
    {"DisplayMapType", 3.f},
    {"DisplayMapSaturation", 0.f},
    {"DisplayMapPeak", 2.f},
    {"DisplayMapShoulder", 0.5f},
    {"UpgradeResourceViewCloning", 0.f}, // 0 = false, 1 = true
};

// Returns the default value for a key, or fallback if not found
inline float get_default_value(const std::string& key, float fallback = 0.f) {
    auto it = default_values.find(key);
    if (it != default_values.end())
        return it->second;
    return fallback;
}

// Returns the value for global resource view cloning (0 = false, 1 = true)
inline bool get_upgrade_resource_view_cloning() {
    return get_default_value("UpgradeResourceViewCloning", 0.f) != 0.f;
}

std::vector<renodx::utils::settings::Setting*> GenerateCustomGameSettingsSection(ShaderInjectData& shader_injection, float& current_settings_mode) {
    return {
        new renodx::utils::settings::Setting{
            .key = "CustomBloom2D",
            .binding = &shader_injection.custom_bloom_2d,
            .default_value = get_default_value("CustomBloom2D", 100.f),
            .label = "2D Bloom",
            .section = "Custom Game Settings",
            .tooltip = "Controls the strength of bloom effect for 2D scenes (0% = disabled, 100% = full strength)",
            .min = 0.f,
            .max = 100.f,
            .parse = [](float value) { return value * 0.01f; },
            .is_visible = [&current_settings_mode]() { return current_settings_mode >= 3; },
        },
        new renodx::utils::settings::Setting{
            .key = "CustomBloom3D",
            .binding = &shader_injection.custom_bloom_3d,
            .default_value = get_default_value("CustomBloom3D", 100.f),
            .label = "3D Bloom",
            .section = "Custom Game Settings",
            .tooltip = "Controls the strength of bloom effect for 3D scenes (0% = disabled, 100% = full strength)",
            .min = 0.f,
            .max = 100.f,
            .parse = [](float value) { return value * 0.01f; },
            .is_visible = [&current_settings_mode]() { return current_settings_mode >= 3; },
        },
        new renodx::utils::settings::Setting{
            .key = "CustomTextBrightnessCoef",
            .binding = &shader_injection.custom_text_brightness_coef,
            .default_value = get_default_value("CustomTextBrightnessCoef", 1.f),
            .label = "Text Brightness Coefficient",
            .section = "Custom Game Settings",
            .tooltip = "Controls the brightness coefficient for text and UI elements (1.0 = normal brightness)",
            .min = 0.1f,
            .max = 3.f,
            .format = "%.2f",
            .is_visible = [&current_settings_mode]() { return current_settings_mode >= 1; },
        },
    };
}

} // namespace hbr_custom_settings 