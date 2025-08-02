#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "../../utils/settings.hpp"
#include "./shared.h"
#include "../../mods/swapchain.hpp"

namespace hbr_custom_settings {

// Map of setting key to default value (made mutable for filename overrides)
static std::unordered_map<std::string, float> default_values = {
    {"SettingsMode", 0.f},
    {"ToneMapType", 3.f},
    {"ToneMapPeakNits", 1000.f},
    {"ToneMapGameNits", 203.f},
    {"ToneMapUINits", 203.f},
    {"GammaCorrection", 1.f},
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
    {"ColorGradePerChannelBlowoutRestoration", 0.f},
    {"ColorGradePerChannelHueCorrection", 0.f},
    {"ColorGradePerChannelChrominanceCorrection", 0.f},
    {"ColorGradeClip", 65.f},
    {"ColorGradeGamma", 50.f},
    {"CustomCharacterBrightness", 1.f},
    {"CustomTextBrightness", 1.f},
    {"CustomGameCompatibilityMode", 1.f},
    {"CustomBloom2D", 100.f},
    {"CustomBloom3D", 100.f},
    {"EnableUIToneMapPass", 0.f},
    {"SwapChainCustomColorSpace", 0.f},
    {"IntermediateEncoding", 1.f},
    {"SwapChainDecoding", 1.f},
    {"SwapChainGammaCorrection", 1.f},
    {"SwapChainClampColorSpace", 2.f},
    {"EffectSplitMode", 0.f},
    {"EffectSplitX", 1.f},
    {"EffectSplitAngle", 0.f},
    {"PerceptualBoostMode", 0.f},
    {"PerceptualBoostChannelMax", 2.f},
    {"PerceptualBoostReinhardStrength", 100.f},
    {"PerceptualBoostReinhardMidpoint", 5.f},
    {"PerceptualBoostXYPQParam", 389.f}, // 268
    {"PerceptualBoostXYPQColor", 36.f},
    {"PerceptualBoostXYPQStrength", 100.f},
    {"PerceptualBoostXYPQMidpoint", 1.f},
    {"PerceptualBoostICTCPParam", 268.f},
    {"PerceptualBoostICTCPColor", 20.f},
    {"PerceptualBoostICTCPStrength", 100.f},
    {"PerceptualBoost2DCharacter", 0.3f},
    {"PerceptualBoost2DBackground", 1.f},
    {"PerceptualBoost3D", 0.f},
    {"DebugMode", 0.f},
    {"DebugMode2", 0.f},
    {"DebugMode3", 0.f},
    {"DisplayMapType", 3.f},
    {"DisplayMapSaturation", 0.f},
    {"DisplayMapPeak", 2.f},
    {"DisplayMapShoulder", 0.5f},
    {"UpgradeResourceViewCloning", 0.f}, // 0 = false, 1 = true
    {"PerceptualBoostReinhardMidpoint", 5.f},
    {"PostSwapChainToneMapping", 0.f},
    {"Upgrade_SwapChainCompatibility", 1.f},
    {"DisableD3D9ResourceUpgrade", 1.f},
    {"UseDeviceProxy", 0.f},    
    {"ConstantBufferOffset", 0},
};

namespace {
// Returns the default value for a key, or fallback if not found
inline float get_default_value(const std::string& key, float fallback = 0.f) {
    auto it = default_values.find(key);
    if (it != default_values.end()){
        return it->second;
    }
    return fallback;
}

inline bool get_use_device_proxy() {
    return get_default_value("UseDeviceProxy", 0.f) != 0.f;
}

inline bool get_disable_d3d9_resource_upgrade() {
    return get_default_value("DisableD3D9ResourceUpgrade", 0.f) != 0.f;
}

// Returns the value for global resource view cloning (0 = false, 1 = true)
inline bool get_upgrade_resource_view_cloning() {
    return get_default_value("UpgradeResourceViewCloning", 0.f) != 0.f;
}
} // anonymous namespace

inline int get_constant_buffer_offset() {
    return get_default_value("ConstantBufferOffset", 0);
}

// Apply filename-based settings overrides
void ApplyFilenameBasedOverrides(const std::string& filename) {
    if (filename == "Artisan TD.exe") {
        default_values["ToneMapType"] = 4.f; // DICE
        default_values["SimulateRenderPass"] = 1.f; 
        default_values["PostSwapChainToneMapping"] = 1.f;
    }
    if (filename == "Ixion.exe") {
    //    default_values["ToneMapType"] = 3.f; // ACES
    }
    if (filename == "No Creeps Were Harmed TD.exe") {
        default_values["ToneMapType"] = 4.f; // DICE
        default_values["SimulateRenderPass"] = 1.f; 
        default_values["PostSwapChainToneMapping"] = 1.f;
    }
    if (filename == "Phantom Brigade.exe") {
        default_values["WhiteClip"] = 25.f;
        default_values["PerChannelBlowoutRestoration"] = 0.75f;
      //  default_values["ToneMapType"] = 2.f; // ACES
    } else if (filename == "This War of Mine.exe") {
        default_values["DisableD3D9ResourceUpgrade"] = 0.f;
        default_values["UseDeviceProxy"] = 1.f;
        default_values["ConstantBufferOffset"] = 50 * 4;
        default_values["ToneMapType"] = 4.f; // DICE
        default_values["SimulateRenderPass"] = 1.f; 
        default_values["PostSwapChainToneMapping"] = 1.f;
    }
}

std::vector<renodx::utils::settings::Setting*> GenerateCustomGameSettingsSection(ShaderInjectData& shader_injection, float& current_settings_mode) {
    return {
        new renodx::utils::settings::Setting{
            .key = "CustomCharacterBrightness",
            .binding = &shader_injection.custom_character_brightness,
            .default_value = 1.0f,
            .label = "Character Brightness",
            .section = "Custom Color Grading",
            .tooltip = "Adjusts the brightness of custom character rendering.",
            .min = 0.0f,
            .max = 2.0f,
            .format = "%.2f",
            .is_visible = [&current_settings_mode]() { return current_settings_mode >= 3; },
        },
        new renodx::utils::settings::Setting{
            .key = "CustomTextBrightness",
            .binding = &shader_injection.custom_text_brightness,
            .default_value = 1.0f,
            .label = "Text Brightness",
            .section = "Custom Color Grading",
            .tooltip = "Adjusts the brightness of custom text rendering.",
            .min = 0.0f,
            .max = 2.0f,
            .format = "%.2f",
            .is_visible = [&current_settings_mode]() { return current_settings_mode >= 3; },
        },
        new renodx::utils::settings::Setting{
            .key = "CustomGameCompatibilityMode",
            .binding = &shader_injection.custom_game_compatiblity_mode,
            .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
            .default_value = hbr_custom_settings::get_default_value("CustomGameCompatibilityMode", 1.f),
            .label = "Game Compatibility Mode",
            .section = "Custom Color Grading",
            .tooltip = "Disable broken in-game features for better compatibility.",
            .labels = {"Off", "On"},
            .is_visible = [&current_settings_mode]() { return current_settings_mode >= 1; },
        },
    };
}


const std::unordered_map<std::string, std::pair<reshade::api::format, float>> UPGRADE_TARGETS = {
    {"R8G8B8A8_TYPELESS", {reshade::api::format::r8g8b8a8_typeless, 2.f}},
    {"B8G8R8A8_TYPELESS", {reshade::api::format::b8g8r8a8_typeless, 2.f}},
    {"R8G8B8A8_UNORM", {reshade::api::format::r8g8b8a8_unorm, 2.f}},
    {"B8G8R8A8_UNORM", {reshade::api::format::b8g8r8a8_unorm, 2.f}},
    {"R8G8B8A8_SNORM", {reshade::api::format::r8g8b8a8_snorm, 2.f}},
    {"R8G8B8A8_UNORM_SRGB", {reshade::api::format::r8g8b8a8_unorm_srgb, 2.f}},
    {"B8G8R8A8_UNORM_SRGB", {reshade::api::format::b8g8r8a8_unorm_srgb, 2.f}},
    {"R10G10B10A2_TYPELESS", {reshade::api::format::r10g10b10a2_typeless, 2.f}},
    {"R10G10B10A2_UNORM", {reshade::api::format::r10g10b10a2_unorm, 2.f}},
    {"B10G10R10A2_UNORM", {reshade::api::format::b10g10r10a2_unorm, 2.f}},
    {"R11G11B10_FLOAT", {reshade::api::format::r11g11b10_float, 2.f}},
    {"R16G16B16A16_TYPELESS", {reshade::api::format::r16g16b16a16_typeless, 0.f}},
};

inline void AddCustomResourceUpgrades() {
    renodx::mods::swapchain::swap_chain_upgrade_targets.push_back({
        .old_format = reshade::api::format::r8g8b8a8_unorm,
        .new_format = reshade::api::format::r16g16b16a16_typeless,
        .use_resource_view_cloning = false,
        .dimensions = {.width=1024, .height=32},
    });
    renodx::mods::swapchain::swap_chain_upgrade_targets.push_back({
        .old_format = reshade::api::format::r8g8b8a8_unorm,
        .new_format = reshade::api::format::r16g16b16a16_typeless,
        .use_resource_view_cloning = false,
        .dimensions = {.width=256, .height=16},
    });
    renodx::mods::swapchain::swap_chain_upgrade_targets.push_back({
        .old_format = reshade::api::format::r8g8b8a8_typeless,
        .new_format = reshade::api::format::r16g16b16a16_typeless,
        .use_resource_view_cloning = false,
        .dimensions = {.width=1024, .height=32},
    });
    renodx::mods::swapchain::swap_chain_upgrade_targets.push_back({
        .old_format = reshade::api::format::r8g8b8a8_typeless,
        .new_format = reshade::api::format::r16g16b16a16_typeless,
        .use_resource_view_cloning = false,
        .dimensions = {.width=256, .height=16},
    });
}

} // namespace hbr_custom_settings 