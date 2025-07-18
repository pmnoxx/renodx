/*
 * Copyright (C) 2024 Carlos Lopez
 * SPDX-License-Identifier: MIT
 */

#define ImTextureID ImU64

#define DEBUG_LEVEL_0

#include <deps/imgui/imgui.h>
#include <include/reshade.hpp>

#include <embed/shaders.h>

#include "../../mods/shader.hpp"
#include "../../mods/swapchain.hpp"
#include "../../utils/settings.hpp"
#include "./shared.h"

namespace {

renodx::mods::shader::CustomShaders custom_shaders = {
        CustomShaderEntry(0x29B13A00), // uber2
        CustomShaderEntry(0xA833F91D), // uber
        CustomShaderEntry(0xC85DC52C), // uber_3d
        CustomShaderEntry(0xFBCF41E6), // uber_park
        CustomShaderEntry(0x97379D6B), // uber_walking_base
        CustomShaderEntry(0x699FABE4), // uber_battle
        CustomShaderEntry(0xC39AD9C3), // uber_battle2
        CustomShaderEntry(0xE5E4F46C), // uber_beach
        CustomShaderEntry(0x99F14892), // uber_beach3
        CustomShaderEntry(0x0EC3291B), // 2d_popup_character
        CustomShaderEntry(0x8E2521B8), // 2d_art2
        CustomShaderEntry(0x35097DF4), // blit_copy
        CustomShaderEntry(0xBC821AE4), // sprints_normal_last_msaa
        CustomShaderEntry(0x20133A8B), // final
        CustomShaderEntry(0xA9F8ED91), // 2d_background
        CustomShaderEntry(0x066C98CB), // 2d_battle_ready_screen
        CustomShaderEntry(0xB4EB8715), // 2d_character
        CustomShaderEntry(0xEADD927D), // advanced_overlay
        CustomShaderEntry(0xEA8CA6F6), // lily_renderer
        CustomShaderEntry(0xFE185665), // bloom_3d_p1
        CustomShaderEntry(0xA1FBDC24), // bloom_3d_p2
        CustomShaderEntry(0x43207502), // bloom_3d_p3
        CustomShaderEntry(0xA8B64286), // uber_battle4
        CustomShaderEntry(0xF2218191), // sky_renderer
        CustomShaderEntry(0xF52A2A61), // uber_battle6
        CustomShaderEntry(0x242BCAA5), // uber_battle5
        CustomShaderEntry(0x09BE2D97), // uber_battle7
        CustomShaderEntry(0x97910609), // uber_battle8
        CustomShaderEntry(0xB054E953), // uber_battle9
        CustomShaderEntry(0x915C6643), // text_draw
        CustomShaderEntry(0x55B0DCB7), // text_draw2
        CustomShaderEntry(0x4F036BB9), // text_draw3

  //  CustomShaderEntry(0x45A96F2D),
   /// CustomShaderEntry(0xD8341E94),
    // CustomSwapchainShader(0x00000000),
    // BypassShaderEntry(0x00000000)
};



ShaderInjectData shader_injection;

float current_settings_mode = 0;

renodx::utils::settings::Settings settings = {
    new renodx::utils::settings::Setting{
        .key = "SettingsMode",
        .binding = &current_settings_mode,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .can_reset = false,
        .label = "Settings Mode",
        .labels = {"Simple", "Intermediate", "Advanced"},
        .is_global = true,
    },
    new renodx::utils::settings::Setting{
        .key = "ToneMapType",
        .binding = &shader_injection.tone_map_type,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 3.f,
        .can_reset = true,
        .label = "Tone Mapper",
        .section = "Tone Mapping",
        .tooltip = "Sets the tone mapper type",
        .labels = {"Vanilla", "None", "ACES", "RenoDRT"},
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "ToneMapPeakNits",
        .binding = &shader_injection.peak_white_nits,
        .default_value = 1000.f,
        .can_reset = false,
        .label = "Peak Brightness",
        .section = "Tone Mapping",
        .tooltip = "Sets the value of peak white in nits",
        .min = 48.f,
        .max = 4000.f,
    },
    new renodx::utils::settings::Setting{
        .key = "ToneMapGameNits",
        .binding = &shader_injection.diffuse_white_nits,
        .default_value = 203.f,
        .label = "Game Brightness",
        .section = "Tone Mapping",
        .tooltip = "Sets the value of 100% white in nits",
        .min = 48.f,
        .max = 500.f,
    },
    new renodx::utils::settings::Setting{
        .key = "ToneMapUINits",
        .binding = &shader_injection.graphics_white_nits,
        .default_value = 203.f,
        .label = "UI Brightness",
        .section = "Tone Mapping",
        .tooltip = "Sets the brightness of UI and HUD elements in nits",
        .min = 48.f,
        .max = 500.f,
    },
    new renodx::utils::settings::Setting{
        .key = "GammaCorrection",
        .binding = &shader_injection.gamma_correction,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Gamma Correction",
        .section = "Tone Mapping",
        .tooltip = "Emulates a display EOTF.",
        .labels = {"Off", "2.2", "BT.1886"},
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "ToneMapScaling",
        .binding = &shader_injection.tone_map_per_channel,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Scaling",
        .section = "Tone Mapping",
        .tooltip = "Luminance scales colors consistently while per-channel saturates and blows out sooner",
        .labels = {"Luminance", "Per Channel"},
        .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
        .is_visible = []() { return current_settings_mode >= 2; },
    },
    new renodx::utils::settings::Setting{
        .key = "ToneMapWorkingColorSpace",
        .binding = &shader_injection.tone_map_working_color_space,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Working Color Space",
        .section = "Tone Mapping",
        .labels = {"BT709", "BT2020", "AP1"},
        .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
        .is_visible = []() { return current_settings_mode >= 2; },
    },
    new renodx::utils::settings::Setting{
        .key = "ToneMapHueProcessor",
        .binding = &shader_injection.tone_map_hue_processor,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Hue Processor",
        .section = "Tone Mapping",
        .tooltip = "Selects hue processor",
        .labels = {"OKLab", "ICtCp", "darkTable UCS"},
        .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
        .is_visible = []() { return current_settings_mode >= 2; },
    },
    new renodx::utils::settings::Setting{
        .key = "ToneMapHueCorrection",
        .binding = &shader_injection.tone_map_hue_correction,
        .default_value = 100.f,
        .label = "Hue Correction",
        .section = "Tone Mapping",
        .tooltip = "Hue retention strength.",
        .min = 0.f,
        .max = 200.f,
        .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
        .parse = [](float value) { return value * 0.01f; },
        .is_visible = []() { return current_settings_mode >= 2; },
    },
    new renodx::utils::settings::Setting{
        .key = "ToneMapHueShift",
        .binding = &shader_injection.tone_map_hue_shift,
        .default_value = 50.f,
        .label = "Hue Shift",
        .section = "Tone Mapping",
        .tooltip = "Hue-shift emulation strength.",
        .min = 0.f,
        .max = 100.f,
        .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
        .parse = [](float value) { return value * 0.01f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "ToneMapClampColorSpace",
        .binding = &shader_injection.tone_map_clamp_color_space,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Clamp Color Space",
        .section = "Tone Mapping",
        .tooltip = "Hue-shift emulation strength.",
        .labels = {"None", "BT709", "BT2020", "AP1"},
        .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
        .parse = [](float value) { return value - 1.f; },
        .is_visible = []() { return current_settings_mode >= 2; },
    },
    new renodx::utils::settings::Setting{
        .key = "ToneMapClampPeak",
        .binding = &shader_injection.tone_map_clamp_peak,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Clamp Peak",
        .section = "Tone Mapping",
        .tooltip = "Hue-shift emulation strength.",
        .labels = {"None", "BT709", "BT2020", "AP1"},
        .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
        .parse = [](float value) { return value - 1.f; },
        .is_visible = []() { return current_settings_mode >= 2; },
    },
    /*
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoostMethod",
        .binding = &shader_injection.perceptual_boost_method,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Perceptual Boost Method",
        .section = "Perceptual Boost",
        .tooltip = "Selects the method for perceptual boost. XY->PQ is the current implementation. TODO for future methods.",
        .labels = {"None", "XY->PQ", "ICTCP"},
    },
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoostParam2",
        .binding = &shader_injection.perceptual_boost_param,
        .default_value = 268.f,
        .label = "Curve Adjustment",
        .section = "Perceptual Boost",
        .tooltip = "Adjusts the perceptual boost curve shape for better color reproduction",
        .min = 000.f,
        .max = 1500.f,
        .parse = [](float value) { return value * 0.001f; },
        .is_visible = []() { return shader_injection.perceptual_boost_method >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoostColor2",
        .binding = &shader_injection.perceptual_boost_color,
        .default_value = 33.f,
        .label = "Color Boost",
        .section = "Perceptual Boost",
        .tooltip = "Controls the intensity of color enhancement (0% to 100%)",
        .min = 0.f,
        .max = 100.f,
        .parse = [](float value) { return value * 0.01f; },
        .is_visible = []() { return shader_injection.perceptual_boost_method >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoostStrength2",
        .binding = &shader_injection.perceptual_boost_strength,
        .default_value = 50.f,
        .label = "Strength",
        .section = "Perceptual Boost",
        .tooltip = "Controls the overall strength of perceptual boost effect (0% to 100%)",
        .min = 0.f,
        .max = 100.f,
        .parse = [](float value) { return value * 0.01f; },
        .is_visible = []() { return shader_injection.perceptual_boost_method >= 1; },
    },
    */
    new renodx::utils::settings::Setting{
        .key = "ColorGradeExposure",
        .binding = &shader_injection.tone_map_exposure,
        .default_value = 1.f,
        .label = "Exposure",
        .section = "Color Grading",
        .max = 2.f,
        .format = "%.2f",
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "ColorGradeHighlights",
        .binding = &shader_injection.tone_map_highlights,
        .default_value = 50.f,
        .label = "Highlights",
        .section = "Color Grading",
        .max = 100.f,
        .parse = [](float value) { return value * 0.02f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "ColorGradeShadows",
        .binding = &shader_injection.tone_map_shadows,
        .default_value = 50.f,
        .label = "Shadows",
        .section = "Color Grading",
        .max = 100.f,
        .parse = [](float value) { return value * 0.02f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "ColorGradeContrast",
        .binding = &shader_injection.tone_map_contrast,
        .default_value = 50.f,
        .label = "Contrast",
        .section = "Color Grading",
        .max = 100.f,
        .parse = [](float value) { return value * 0.02f; },
    },
    new renodx::utils::settings::Setting{
        .key = "ColorGradeSaturation",
        .binding = &shader_injection.tone_map_saturation,
        .default_value = 50.f,
        .label = "Saturation",
        .section = "Color Grading",
        .max = 100.f,
        .parse = [](float value) { return value * 0.02f; },
    },
    new renodx::utils::settings::Setting{
        .key = "ColorGradeHighlightSaturation",
        .binding = &shader_injection.tone_map_highlight_saturation,
        .default_value = 50.f,
        .label = "Highlight Saturation",
        .section = "Color Grading",
        .tooltip = "Adds or removes highlight color.",
        .max = 100.f,
        .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
        .parse = [](float value) { return value * 0.02f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "ColorGradeBlowout",
        .binding = &shader_injection.tone_map_blowout,
        .default_value = 0.f,
        .label = "Blowout",
        .section = "Color Grading",
        .tooltip = "Controls highlight desaturation due to overexposure.",
        .max = 100.f,
        .parse = [](float value) { return value * 0.01f; },
    },
    new renodx::utils::settings::Setting{
        .key = "ColorGradeFlare",
        .binding = &shader_injection.tone_map_flare,
        .default_value = 0.f,
        .label = "Flare",
        .section = "Color Grading",
        .tooltip = "Flare/Glare Compensation",
        .max = 100.f,
        .is_enabled = []() { return shader_injection.tone_map_type == 3; },
        .parse = [](float value) { return value * 0.02f; },
    },
    new renodx::utils::settings::Setting{
        .key = "ColorGradeScene",
        .binding = &shader_injection.color_grade_strength,
        .default_value = 100.f,
        .label = "Scene Grading",
        .section = "Color Grading",
        .tooltip = "Scene grading as applied by the game",
        .max = 100.f,
        .is_enabled = []() { return shader_injection.tone_map_type > 0; },
        .parse = [](float value) { return value * 0.01f; },
    },
    new renodx::utils::settings::Setting{
        .key = "ColorGradeClip",
        .binding = &shader_injection.reno_drt_white_clip,
        .default_value = 65.f,
        .label = "White Clip",
        .section = "Custom Color Grading",
        .tooltip = "Clip point for white in nits",
        .min = 1.f,
        .max = 100.f,
        .is_enabled = []() { return shader_injection.tone_map_type == 3; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "Bloom2D",
        .binding = &shader_injection.bloom_2d,
        .default_value = 100.f,
        .label = "2D Bloom",
        .section = "Custom Color Grading",
        .tooltip = "Controls the strength of bloom effect for 2D scenes (0% = disabled, 100% = full strength)",
        .min = 0.f,
        .max = 100.f,
        .parse = [](float value) { return value * 0.01f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "Bloom3D",
        .binding = &shader_injection.bloom_3d,
        .default_value = 100.f,
        .label = "3D Bloom",
        .section = "Custom Color Grading",
        .tooltip = "Controls the strength of bloom effect for 3D scenes (0% = disabled, 100% = full strength)",
        .min = 0.f,
        .max = 100.f,
        .parse = [](float value) { return value * 0.01f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "TextBrightnessCoef",
        .binding = &shader_injection.text_brightness_coef,
        .default_value = 1.f,
        .label = "Text Brightness Coefficient",
        .section = "Custom Color Grading",
        .tooltip = "Controls the brightness coefficient for text and UI elements (1.0 = normal brightness)",
        .min = 0.1f,
        .max = 3.f,
        .format = "%.2f",
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "EnableUIToneMapPass",
        .binding = &shader_injection.enable_tone_map_pass,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "UI Tone Mapping Pass",
        .section = "Custom Color Grading",
        .tooltip = "Enable or disable the tone mapping pass in the final shader. (This applies tonemapping to UI effects, which can exceed peak nits.)",
        .labels = {"Off", "On"},
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "SwapChainCustomColorSpace",
        .binding = &shader_injection.swap_chain_custom_color_space,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 1.f,
        .label = "Custom Color Space",
        .section = "Display Output",
        .tooltip = "Selects output color space"
                   "\nUS Modern for BT.709 D65."
                   "\nJPN Modern for BT.709 D93."
                   "\nUS CRT for BT.601 (NTSC-U)."
                   "\nJPN CRT for BT.601 ARIB-TR-B9 D93 (NTSC-J)."
                   "\nDefault: US CRT",
        .labels = {
            "US Modern",
            "JPN Modern",
            "US CRT",
            "JPN CRT",
        },
        .is_visible = []() { return settings[0]->GetValue() >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "IntermediateEncoding",
        .binding = &shader_injection.intermediate_encoding,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 2.f,
        .label = "Intermediate Encoding",
        .section = "Display Output",
        .labels = {"Auto", "None", "SRGB", "2.2", "2.4"},
        .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
        .parse = [](float value) {
            if (value == 0) return shader_injection.gamma_correction + 1.f;
            return value - 1.f; },
        .is_visible = []() { return current_settings_mode >= 2; },
    },
    new renodx::utils::settings::Setting{
        .key = "SwapChainDecoding",
        .binding = &shader_injection.swap_chain_decoding,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 2.f,
        .label = "Swapchain Decoding",
        .section = "Display Output",
        .labels = {"Auto", "None", "SRGB", "2.2", "2.4"},
        .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
        .parse = [](float value) {
            if (value == 0) return shader_injection.intermediate_encoding;
            return value - 1.f; },
        .is_visible = []() { return current_settings_mode >= 2; },
    },
    new renodx::utils::settings::Setting{
        .key = "SwapChainGammaCorrection",
        .binding = &shader_injection.swap_chain_gamma_correction,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Gamma Correction",
        .section = "Display Output",
        .labels = {"None", "2.2", "2.4"},
        .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
        .is_visible = []() { return current_settings_mode >= 2; },
    },
    new renodx::utils::settings::Setting{
        .key = "SwapChainClampColorSpace",
        .binding = &shader_injection.swap_chain_clamp_color_space,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 2.f,
        .label = "Clamp Color Space",
        .section = "Display Output",
        .labels = {"None", "BT709", "BT2020", "AP1"},
        .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
        .parse = [](float value) { return value - 1.f; },
        .is_visible = []() { return current_settings_mode >= 2; },
    },
    new renodx::utils::settings::Setting{
        .key = "EffectSplitMode",
        .binding = &shader_injection.effect_split_mode,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Split Mode",
        .section = "Display Output",
        .tooltip = "Choose the split mode for effect application.",
        .labels = {"Off", "On"},
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "EffectSplitX",
        .binding = &shader_injection.effect_split_x,
        .default_value = 1.f, // Default to half of 1920 width, adjust as needed
        .label = "Effect Split X",
        .section = "Display Output",
        .tooltip = "Only apply effects to the left of this screen X coordinate.",
        .min = -1.f,
        .max = 1.f, // Or your max screen width
        .format = "%.3f",
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "EffectSplitAngle",
        .binding = &shader_injection.effect_split_angle,
        .default_value = 0.f,
        .label = "Split Angle",
        .section = "Display Output",
        .tooltip = "Angle (in degrees) for the split line (0 = vertical, 90 = horizontal, etc.)",
        .min = 0.f,
        .max = 360.f,
        .is_visible = []() { return shader_injection.effect_split_mode != 0; },
    },



    
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoostMode",
        .binding = &shader_injection.perceptual_boost_mode,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Perceptual Boost",
        .section = "Perceptual Boost",
        .tooltip = "Selects the method for perceptual boost.",
        .labels = {"OFF", "Reinhard", "XY->PQ", "ICTCP"},
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoostChannelMax",
        .binding = &shader_injection.perceptual_boost_channel_max,
        .default_value = 2.f,
        .label = "Perceptual Boost Channel Max",
        .section = "Perceptual Boost",
        .tooltip = "Sets the channel_max parameter for Reinhard Perceptual Boost.",
        .min = 1.f,
        .max = 10.f,
        .format = "%.2f",
        .is_enabled = []() { return shader_injection.perceptual_boost_mode == 1.f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    // Reinhard mode settings
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoostReinhardStrength",
        .binding = &shader_injection.perceptual_boost_reinhard_strength,
        .default_value = 100.f,
        .label = "Reinhard Strength",
        .section = "Perceptual Boost",
        .tooltip = "Controls the overall strength of Reinhard perceptual boost effect (0% to 100%)",
        .min = 0.f,
        .max = 100.f,
        .is_enabled = []() { return shader_injection.perceptual_boost_mode == 1.f; },
        .parse = [](float value) { return value * 0.01f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    // XY->PQ mode settings
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoostXYPQParam",
        .binding = &shader_injection.perceptual_boost_xypq_param,
        .default_value = 268.f,
        .label = "XY->PQ Curve Adjustment",
        .section = "Perceptual Boost",
        .tooltip = "Adjusts the XY->PQ perceptual boost curve shape for better color reproduction",
        .min = 0.f,
        .max = 1500.f,
        .is_enabled = []() { return shader_injection.perceptual_boost_mode == 2.f; },
        .parse = [](float value) { return value * 0.001f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoostXYPQColor",
        .binding = &shader_injection.perceptual_boost_xypq_color,
        .default_value = 20.f,
        .label = "XY->PQ Color Boost",
        .section = "Perceptual Boost",
        .tooltip = "Controls the intensity of XY->PQ color enhancement (0% to 100%)",
        .min = 0.f,
        .max = 100.f,
        .is_enabled = []() { return shader_injection.perceptual_boost_mode == 2.f; },
        .parse = [](float value) { return value * 0.01f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoostXYPQStrength",
        .binding = &shader_injection.perceptual_boost_xypq_strength,
        .default_value = 100.f,
        .label = "XY->PQ Strength",
        .section = "Perceptual Boost",
        .tooltip = "Controls the overall strength of XY->PQ perceptual boost effect (0% to 100%)",
        .min = 0.f,
        .max = 100.f,
        .is_enabled = []() { return shader_injection.perceptual_boost_mode == 2.f; },
        .parse = [](float value) { return value * 0.01f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    // ICTCP mode settings
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoostICTCPParam",
        .binding = &shader_injection.perceptual_boost_ictcp_param,
        .default_value = 268.f,
        .label = "ICTCP Curve Adjustment",
        .section = "Perceptual Boost",
        .tooltip = "Adjusts the ICTCP perceptual boost curve shape for better color reproduction",
        .min = 0.f,
        .max = 1500.f,
        .is_enabled = []() { return shader_injection.perceptual_boost_mode == 3.f; },
        .parse = [](float value) { return value * 0.001f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoostICTCPColor",
        .binding = &shader_injection.perceptual_boost_ictcp_color,
        .default_value = 20.f,
        .label = "ICTCP Color Boost",
        .section = "Perceptual Boost",
        .tooltip = "Controls the intensity of ICTCP color enhancement (0% to 100%)",
        .min = 0.f,
        .max = 100.f,
        .is_enabled = []() { return shader_injection.perceptual_boost_mode == 3.f; },
        .parse = [](float value) { return value * 0.01f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoostICTCPStrength",
        .binding = &shader_injection.perceptual_boost_ictcp_strength,
        .default_value = 100.f,
        .label = "ICTCP Strength",
        .section = "Perceptual Boost",
        .tooltip = "Controls the overall strength of ICTCP perceptual boost effect (0% to 100%)",
        .min = 0.f,
        .max = 100.f,
        .is_enabled = []() { return shader_injection.perceptual_boost_mode == 3.f; },
        .parse = [](float value) { return value * 0.01f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoost2DCharacter",
        .binding = &shader_injection.perceptual_boost_2d_character,
        .default_value = 0.3f,
        .label = "2D Character Scenes",
        .section = "Perceptual Boost",
        .tooltip = "Perceptual boost strength for 2D character scenes (0.0 = disabled, 1.0 = normal, 10.0 = maximum)",
        .min = 0.f,
        .max = 10.f,
        .format = "%.3f",
        .is_enabled = []() { return shader_injection.perceptual_boost_mode > 0.f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoost2DBackground",
        .binding = &shader_injection.perceptual_boost_2d_background,
        .default_value = 1.f,
        .label = "2D Background Scenes",
        .section = "Perceptual Boost",
        .tooltip = "Perceptual boost strength for 2D background scenes (0.0 = disabled, 1.0 = normal, 10.0 = maximum)",
        .min = 0.f,
        .max = 10.f,
        .format = "%.3f",
        .is_enabled = []() { return shader_injection.perceptual_boost_mode > 0.f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "PerceptualBoost3D",
        .binding = &shader_injection.perceptual_boost_3d,
        .default_value = 0.f,
        .label = "3D Scenes",
        .section = "Perceptual Boost",
        .tooltip = "Perceptual boost strength for 3D scenes (0.0 = disabled, 1.0 = normal, 10.0 = maximum)",
        .min = 0.f,
        .max = 10.f,
        .format = "%.3f",
        .is_enabled = []() { return shader_injection.perceptual_boost_mode > 0.f; },
        .is_visible = []() { return current_settings_mode >= 1; },
    },
    new renodx::utils::settings::Setting{
        .key = "DebugMode",
        .binding = &shader_injection.debug_mode,
        .default_value = 0.f,
        .label = "Debug Mode",
        .section = "Development",
        .tooltip = "Debug mode for development and testing (0.0 = disabled, 1.0 = enabled)",
        .min = 0.f,
        .max = 1.f,
        .format = "%.2f",
        .is_visible = []() { return current_settings_mode >= 2; },
    },
    
   /* new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "Reset",
        .section = "Color Grading Templates",
        .group = "button-line-1",
        .tint = 0xAD5729,
        .on_change = []() {
          renodx::utils::settings::UpdateSetting("toneMapType", 3.f);
          renodx::utils::settings::UpdateSetting("toneMapPerChannel", 1.f);
          renodx::utils::settings::UpdateSetting("toneMapColorSpace", 2.f);
          renodx::utils::settings::UpdateSetting("toneMapHueProcessor", 2.f);
          renodx::utils::settings::UpdateSetting("toneMapHueShift", 50.f);
          renodx::utils::settings::UpdateSetting("toneMapHueCorrection", 0.f);
          renodx::utils::settings::UpdateSetting("colorGradeExposure", 1.f);
          renodx::utils::settings::UpdateSetting("colorGradeHighlights", 50.f);
          renodx::utils::settings::UpdateSetting("colorGradeShadows", 50.f);
          renodx::utils::settings::UpdateSetting("colorGradeContrast", 50.f);
          renodx::utils::settings::UpdateSetting("colorGradeSaturation", 50.f);
          renodx::utils::settings::UpdateSetting("colorGradeBlowout", 50.f);
          renodx::utils::settings::UpdateSetting("colorGradeDechroma", 0.f);
          renodx::utils::settings::UpdateSetting("colorGradeFlare", 0.f);
          renodx::utils::settings::UpdateSetting("colorGradeClip", 100.f);
          renodx::utils::settings::UpdateSetting("colorGradeLUTStrength", 100.f);
          renodx::utils::settings::UpdateSetting("colorGradeLUTSampling", 1.f);
          renodx::utils::settings::UpdateSetting("colorGradeLUTShaper", 1.f);
        },
    },
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "HDR Look",
        .section = "Color Grading Templates",
        .group = "button-line-1",
        .tint = 0xB88151,
        .on_change = []() {
            renodx::utils::settings::UpdateSetting("toneMapType", 3.f);
            renodx::utils::settings::UpdateSetting("toneMapPerChannel", 1.f);
            renodx::utils::settings::UpdateSetting("toneMapColorSpace", 2.f);
            renodx::utils::settings::UpdateSetting("toneMapHueProcessor", 1.f);
            renodx::utils::settings::UpdateSetting("toneMapHueCorrection", 0.f);
            renodx::utils::settings::UpdateSetting("colorGradeExposure", 1.f);
            renodx::utils::settings::UpdateSetting("colorGradeHighlights", 50.f);
            renodx::utils::settings::UpdateSetting("colorGradeShadows", 47.f);
            renodx::utils::settings::UpdateSetting("colorGradeContrast", 50.f);
            renodx::utils::settings::UpdateSetting("colorGradeSaturation", 55.f);
            renodx::utils::settings::UpdateSetting("colorGradeBlowout", 35.f);
            renodx::utils::settings::UpdateSetting("colorGradeDechroma", 0.f);
            renodx::utils::settings::UpdateSetting("colorGradeFlare", 50.f);
            renodx::utils::settings::UpdateSetting("colorGradeClip", 100.f);
          renodx::utils::settings::UpdateSetting("colorGradeLUTStrength", 100.f);
          renodx::utils::settings::UpdateSetting("colorGradeLUTSampling", 1.f);
          renodx::utils::settings::UpdateSetting("colorGradeLUTShaper", 1.f); },
        //.is_visible = []() { return shader_injection.toneMapType == 3.f; },
    },
    new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::BUTTON,
        .label = "HDR Look 2",
        .section = "Color Grading Templates",
        .group = "button-line-1",
        .tint = 0xB88151,
        .on_change = []() {
          renodx::utils::settings::UpdateSetting("toneMapType", 4.f);
          renodx::utils::settings::UpdateSetting("toneMapPerChannel", 0.f);
          renodx::utils::settings::UpdateSetting("toneMapColorSpace", 2.f);
          renodx::utils::settings::UpdateSetting("toneMapHueProcessor", 1.f);
          renodx::utils::settings::UpdateSetting("toneMapHueShift", 100.f);
          renodx::utils::settings::UpdateSetting("toneMapHueCorrection", 100.f);
          renodx::utils::settings::UpdateSetting("colorGradeExposure", 1.f);
          renodx::utils::settings::UpdateSetting("colorGradeHighlights", 50.f);
          renodx::utils::settings::UpdateSetting("colorGradeShadows", 40.f);
          renodx::utils::settings::UpdateSetting("colorGradeContrast", 42.f);
          renodx::utils::settings::UpdateSetting("colorGradeSaturation", 86.f);
          renodx::utils::settings::UpdateSetting("colorGradeBlowout", 35.f);
          renodx::utils::settings::UpdateSetting("colorGradeDechroma", 65.f);
          renodx::utils::settings::UpdateSetting("colorGradeFlare", 50.f);
          renodx::utils::settings::UpdateSetting("colorGradeClip", 100.f);
          renodx::utils::settings::UpdateSetting("colorGradeLUTStrength", 100.f);
          renodx::utils::settings::UpdateSetting("colorGradeLUTSampling", 1.f);
          renodx::utils::settings::UpdateSetting("colorGradeLUTShaper", 1.f); },
    },*/
};

void AddAdvancedSettings() {
  auto* swapchain_setting = new renodx::utils::settings::Setting{
      .key = "Upgrade_SwapChainCompatibility",
      .value_type = renodx::utils::settings::SettingValueType::INTEGER,
      .default_value = 0.f,
      .label = "Swap Chain Compatibility Mode",
      .section = "About",
      .tooltip = "Enhances support for third-party addons to read the swap chain.",
      .labels = {
          "Off",
          "On",
      },
      .is_global = true,
  };
  reshade::get_config_value(nullptr, renodx::utils::settings::global_name.c_str(), "Upgrade_SwapChainCompatibility", swapchain_setting->value_as_int);
  renodx::mods::swapchain::swapchain_proxy_compatibility_mode = swapchain_setting->GetValue() != 0;
  settings.push_back(swapchain_setting);
}

const std::unordered_map<std::string, std::pair<reshade::api::format, float>> UPGRADE_TARGETS = {
    {"R8G8B8A8_TYPELESS", {reshade::api::format::r8g8b8a8_typeless, 3.f}},
    {"B8G8R8A8_TYPELESS", {reshade::api::format::b8g8r8a8_typeless, 0.f}},
    {"R8G8B8A8_UNORM", {reshade::api::format::r8g8b8a8_unorm, 0.f}},
    {"B8G8R8A8_UNORM", {reshade::api::format::b8g8r8a8_unorm, 0.f}},
    {"R8G8B8A8_SNORM", {reshade::api::format::r8g8b8a8_snorm, 0.f}},
    {"R8G8B8A8_UNORM_SRGB", {reshade::api::format::r8g8b8a8_unorm_srgb, 0.f}},
    {"B8G8R8A8_UNORM_SRGB", {reshade::api::format::b8g8r8a8_unorm_srgb, 0.f}},
    {"R10G10B10A2_TYPELESS", {reshade::api::format::r10g10b10a2_typeless, 0.f}},
    {"R10G10B10A2_UNORM", {reshade::api::format::r10g10b10a2_unorm, 0.f}},
    {"B10G10R10A2_UNORM", {reshade::api::format::b10g10r10a2_unorm, 0.f}},
    {"R11G11B10_FLOAT", {reshade::api::format::r11g11b10_float, 3.f}},
    {"R16G16B16A16_TYPELESS", {reshade::api::format::r16g16b16a16_typeless, 0.f}},
};

void OnPresetOff() {
  //   renodx::utils::settings::UpdateSetting("toneMapType", 0.f);
  //   renodx::utils::settings::UpdateSetting("toneMapPeakNits", 203.f);
  //   renodx::utils::settings::UpdateSetting("toneMapGameNits", 203.f);
  //   renodx::utils::settings::UpdateSetting("toneMapUINits", 203.f);
  //   renodx::utils::settings::UpdateSetting("toneMapGammaCorrection", 0);
  //   renodx::utils::settings::UpdateSetting("colorGradeExposure", 1.f);
  //   renodx::utils::settings::UpdateSetting("colorGradeHighlights", 50.f);
  //   renodx::utils::settings::UpdateSetting("colorGradeShadows", 50.f);
  //   renodx::utils::settings::UpdateSetting("colorGradeContrast", 50.f);
  //   renodx::utils::settings::UpdateSetting("colorGradeSaturation", 50.f);
  //   renodx::utils::settings::UpdateSetting("colorGradeLUTStrength", 100.f);
  //   renodx::utils::settings::UpdateSetting("colorGradeLUTScaling", 0.f);
}

const auto UPGRADE_TYPE_NONE = 0.f;
const auto UPGRADE_TYPE_OUTPUT_SIZE = 1.f;
const auto UPGRADE_TYPE_OUTPUT_RATIO = 2.f;
const auto UPGRADE_TYPE_ANY = 3.f;

bool initialized = false;

}  // namespace

extern "C" __declspec(dllexport) constexpr const char* NAME = "RenoDX";
extern "C" __declspec(dllexport) constexpr const char* DESCRIPTION = "RenoDX (Generic)";

BOOL APIENTRY DllMain(HMODULE h_module, DWORD fdw_reason, LPVOID lpv_reserved) {
    
  switch (fdw_reason) {
    case DLL_PROCESS_ATTACH:
      if (!reshade::register_addon(h_module)) return FALSE;

      if (!initialized) {
        renodx::mods::shader::force_pipeline_cloning = true;
        renodx::mods::shader::expected_constant_buffer_space = 50;
        renodx::mods::shader::expected_constant_buffer_index = 13;
        renodx::mods::shader::allow_multiple_push_constants = true;

        renodx::mods::swapchain::expected_constant_buffer_index = 13;
        renodx::mods::swapchain::expected_constant_buffer_space = 50;
        renodx::mods::swapchain::use_resource_cloning = true;
        renodx::mods::swapchain::swap_chain_proxy_shaders = {
            {
                reshade::api::device_api::d3d11,
                {
                    .vertex_shader = __swap_chain_proxy_vertex_shader_dx11,
                    .pixel_shader = __swap_chain_proxy_pixel_shader_dx11,
                },
            },
            {
                reshade::api::device_api::d3d12,
                {
                    .vertex_shader = __swap_chain_proxy_vertex_shader_dx12,
                    .pixel_shader = __swap_chain_proxy_pixel_shader_dx12,
                },
            },
        };
        renodx::mods::swapchain::ignored_device_apis = { reshade::api::device_api::d3d9 }; // needed to prevent crash

        {
          auto* setting = new renodx::utils::settings::Setting{
              .key = "SwapChainForceBorderless",
              .value_type = renodx::utils::settings::SettingValueType::INTEGER,
              .default_value = 0.f,
              .label = "Force Borderless",
              .section = "Display Output",
              .tooltip = "Forces fullscreen to be borderless for proper HDR",
              .labels = {
                  "Disabled",
                  "Enabled",
              },
              .on_change_value = [](float previous, float current) { renodx::mods::swapchain::force_borderless = (current == 1.f); },
              .is_global = true,
              .is_visible = []() { return current_settings_mode >= 2; },
          };
          renodx::utils::settings::LoadSetting(renodx::utils::settings::global_name, setting);
          renodx::mods::swapchain::force_borderless = (setting->GetValue() == 1.f);
          settings.push_back(setting);
        }

        {
          auto* setting = new renodx::utils::settings::Setting{
              .key = "SwapChainPreventFullscreen",
              .value_type = renodx::utils::settings::SettingValueType::INTEGER,
              .default_value = 0.f,
              .label = "Prevent Fullscreen",
              .section = "Display Output",
              .tooltip = "Prevent exclusive fullscreen for proper HDR",
              .labels = {
                  "Disabled",
                  "Enabled",
              },
              .on_change_value = [](float previous, float current) { renodx::mods::swapchain::prevent_full_screen = (current == 1.f); },
              .is_global = true,
              .is_visible = []() { return current_settings_mode >= 2; },
          };
          renodx::utils::settings::LoadSetting(renodx::utils::settings::global_name, setting);
          renodx::mods::swapchain::prevent_full_screen = (setting->GetValue() == 1.f);
          settings.push_back(setting);
        }

        {
          auto* setting = new renodx::utils::settings::Setting{
              .key = "SwapChainEncoding",
              .binding = &shader_injection.swap_chain_encoding,
              .value_type = renodx::utils::settings::SettingValueType::INTEGER,
              .default_value = 5.f,
              .label = "Encoding",
              .section = "Display Output",
              .labels = {"None", "SRGB", "2.2", "2.4", "HDR10", "scRGB"},
              .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
              .on_change_value = [](float previous, float current) {
                bool is_hdr10 = current == 4;
                shader_injection.swap_chain_encoding_color_space = (is_hdr10 ? 1.f : 0.f);
                // return void
              },
              .is_global = true,
              .is_visible = []() { return current_settings_mode >= 2; },
          };
          renodx::utils::settings::LoadSetting(renodx::utils::settings::global_name, setting);
          bool is_hdr10 = setting->GetValue() == 4;
          renodx::mods::swapchain::SetUseHDR10(is_hdr10);
          renodx::mods::swapchain::use_resize_buffer = setting->GetValue() < 4;
          shader_injection.swap_chain_encoding_color_space = is_hdr10 ? 1.f : 0.f;
          settings.push_back(setting);
        }

        for (const auto& [key, format_pair] : UPGRADE_TARGETS) {
          auto* setting = new renodx::utils::settings::Setting{
              .key = "Upgrade_" + key,
              .value_type = renodx::utils::settings::SettingValueType::INTEGER,
              .default_value = format_pair.second,
              .label = key,
              .section = "Resource Upgrades",
              .labels = {
                  "Off",
                  "Output size",
                  "Output ratio",
                  "Any size",
              },
              .is_global = true,
              .is_visible = []() { return settings[0]->GetValue() >= 2; },
          };
          renodx::utils::settings::LoadSetting(renodx::utils::settings::global_name, setting);
          settings.push_back(setting);

          auto value = setting->GetValue();
          if (value > 0) {
            renodx::mods::swapchain::swap_chain_upgrade_targets.push_back({
                .old_format = format_pair.first,
                .new_format = reshade::api::format::r16g16b16a16_float,
                .ignore_size = (value == UPGRADE_TYPE_ANY),
                .use_resource_view_cloning = false, // needed to prevent crash
              //  .use_resource_view_cloning = true, // needed to prevent crash
                .aspect_ratio = static_cast<float>((value == UPGRADE_TYPE_OUTPUT_RATIO)
                                                       ? renodx::mods::swapchain::SwapChainUpgradeTarget::BACK_BUFFER
                                                       : renodx::mods::swapchain::SwapChainUpgradeTarget::ANY),
                .usage_include = reshade::api::resource_usage::render_target,
            });
            std::stringstream s;
            s << "Applying user resource upgrade for ";
            s << format_pair.first << ": " << value;
            reshade::log::message(reshade::log::level::info, s.str().c_str());
          }
        }
        AddAdvancedSettings();
        initialized = true;
      }

      break;
    case DLL_PROCESS_DETACH:
      reshade::unregister_addon(h_module);
      break;
  }

  renodx::utils::settings::Use(fdw_reason, &settings, &OnPresetOff);
  renodx::mods::swapchain::Use(fdw_reason, &shader_injection);
  renodx::mods::shader::Use(fdw_reason, custom_shaders, &shader_injection);

  return TRUE;
}
