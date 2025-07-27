/*
- 0.04
perceptual boost for displayoutput
- 0.05 
fixed default of highlights restoration
- 0.08
add tooltip to post swapchain tone mapping
 - 0.09
add midpoint settings for perceptual boost
- 0.10
add disable swap chain upgrade setting
- 0.11
add aces tonemap sdr effect
- 0.12
add dice and frostbite tonemappers
- 0.13
add simulate render pass setting
- 0.14
add random seed setting for debandingbd
- 0.15
add custom.h for tone mapping
- 0.16
increase max nits for tone mapping
- 0.17
add dump lut shaders setting    
- 0.18
add version display on top
- 0.19
change default output folder to renodx-dev/dump
- 0.20
change default dump folder to dump
- 0.21
fix setting visibility for resource upgrades
- 0.22
disable automatic shader dumping
- 0.23
optimize shader dumping
- 0.24
print exe name and title
- 0.25
change defaults for Unity
*/

 constexpr const char* RENODX_VERSION = "0.25";

 #define ImTextureID ImU64

 #define DEBUG_LEVEL_0
 
   #include <deps/imgui/imgui.h>
  #include <include/reshade.hpp>
  #include <cstring>
  
  #include <embed/shaders.h>
 
 #include "../../mods/shader.hpp"
 #include "../../mods/swapchain.hpp"
#include "../../utils/platform.hpp"
#include "../../utils/random.hpp"
 #include "../../utils/settings.hpp"
 #include "../../utils/shader.hpp"
 #include "../../utils/shader_dump.hpp"
 #include "../../utils/swapchain.hpp"
   #include "../../utils/resource.hpp"
  #include <crc32_hash.hpp>
   #include "./shared.h"
  #include "custom_settings.cpp"
 
 
namespace {
 
    renodx::mods::shader::CustomShaders custom_shaders = {
       __ALL_CUSTOM_SHADERS
    };
    
    ShaderInjectData shader_injection;
    
         float g_dump_shaders = 0;
     float g_autodump_lutbuilders = 0;
     std::unordered_set<uint32_t> g_dumped_shaders = {};
     
     float current_settings_mode = 0;

    // Check if shader data contains binary representation of the lutbuilder float
    static bool ConstainsFloat(const std::span<const uint8_t>& shader_data, float target_float) {
      const uint8_t* target_bytes = reinterpret_cast<const uint8_t*>(&target_float);
      
      if (shader_data.size() >= 4) {
        for (size_t i = 0; i <= shader_data.size() - 4; ++i) {
          if (std::memcmp(&shader_data[i], target_bytes, 4) == 0) {
            return true;
          }
        }
      }
      return false;
    }

    static std::optional<std::string> DumpShaderPrefix(const std::span<const uint8_t>& shader_data) {
        if (g_autodump_lutbuilders != 0 && ConstainsFloat(shader_data, 0.070841603f)) {
            return "lutbuilder_";
        }
        if (g_autodump_lutbuilders != 0 && ConstainsFloat(shader_data, 0.947867334)) {
            return "uber_srgb_";
        }
        if (g_dump_shaders != 0) {
            return "shader_";
        }

        return std::nullopt;
    }

    // Scan existing dumped shaders on application start for performance
    static void ScanExistingDumpedShaders() {
      auto dump_path = renodx::utils::path::GetOutputPath();
      
      if (!std::filesystem::exists(dump_path)) return;
      dump_path /= renodx::utils::shader::dump::default_dump_folder;
      if (!std::filesystem::exists(dump_path)) return;
      
      size_t shaders_found = 0;
      
      for (const auto& entry : std::filesystem::directory_iterator(dump_path)) {
        if (!entry.is_regular_file()) continue;
        const auto& entry_path = entry.path();
        
        // Check various possible extensions (.cso, .hlsl, etc.)
        const auto extension = entry_path.extension().string();
        if (extension != ".cso" && extension != ".hlsl" && extension != ".ps_5_0" && 
            extension != ".vs_5_0" && extension != ".cs_5_0" && extension != ".gs_5_0") continue;
            
        const auto& basename = entry_path.stem().string();
        if (basename.length() < 10) continue;  // 0x + 8 hex chars minimum
        if (!basename.starts_with("0x")) continue;
        
        // Extract the 8-character hex hash
        auto hash_string = basename.substr(2, 8);
        uint32_t shader_hash;
        try {
          shader_hash = std::stoul(hash_string, nullptr, 16);
        } catch (const std::exception& e) {
          // Skip invalid hash files
          continue;
        }
        
        if (g_dumped_shaders.insert(shader_hash).second) {
          shaders_found++;
        }
      }
      
      if (shaders_found > 0) {
        std::stringstream s;
        s << "Loaded " << shaders_found << " existing dumped shader hashes for performance optimization";
        reshade::log::message(reshade::log::level::info, s.str().c_str());
      }
    }
 
    
 void OnInitPipeline(
     reshade::api::device* device,
     reshade::api::pipeline_layout layout,
     uint32_t subobject_count,
     const reshade::api::pipeline_subobject* subobjects,
     reshade::api::pipeline pipeline) {
   if (pipeline.handle == 0u) return;
   if (layout.handle == 0u) return;

   // Check if any dumping is enabled
   if (g_dump_shaders == 0 && g_autodump_lutbuilders == 0) return;

   // Extract and dump any new shaders from pipeline subobjects
   for (uint32_t i = 0; i < subobject_count; ++i) {
    const auto& subobject = subobjects[i];
    
    // Check if this is a shader subobject
    if (subobject.type == reshade::api::pipeline_subobject_type::pixel_shader) {
      
      if (subobject.data == nullptr) continue;
      
      // Get shader data
      const auto* shader_desc = reinterpret_cast<const reshade::api::shader_desc*>(subobject.data);
      
      if (shader_desc == nullptr || shader_desc->code_size == 0) continue;
      
             // Calculate shader hash
       auto shader_hash = compute_crc32(
           reinterpret_cast<const uint8_t*>(shader_desc->code), 
           shader_desc->code_size);
      
      // Skip if we've already dumped this shader
      if (g_dumped_shaders.contains(shader_hash)) continue;
      
             // Skip if this is a custom shader we injected
     //  if (custom_shaders.contains(shader_hash)) continue;
       
       g_dumped_shaders.emplace(shader_hash);
       
               try {
          std::vector<uint8_t> shader_data(
              reinterpret_cast<const uint8_t*>(shader_desc->code),
              reinterpret_cast<const uint8_t*>(shader_desc->code) + shader_desc->code_size);
          
          auto shader_version = renodx::utils::shader::compiler::directx::DecodeShaderVersion(shader_data);
          if (shader_version.GetMajor() == 0) {
            // No shader information found
            continue;
          }
          
                     // Check if shader should be dumped and get prefix
           auto dump_prefix = DumpShaderPrefix(shader_data);
           
           if (!dump_prefix.has_value()) continue;
           
           std::string prefix = dump_prefix.value();
           
          reshade::log::message(
              reshade::log::level::info,
              std::format("Dumping pipeline shader: 0x{:08x} ({})", shader_hash, prefix).c_str());
           
           renodx::utils::path::default_output_folder = "renodx-dev";
           renodx::utils::shader::dump::default_dump_folder = "dump";
          
          renodx::utils::shader::dump::DumpShader(
              shader_hash,
              shader_data,
              subobject.type,
              prefix,
              "");
            
      } catch (...) {
        std::stringstream s;
        s << "utils::shader::dump(Failed to dump pipeline shader: ";
        s << PRINT_CRC32(shader_hash);
        s << ")";
        reshade::log::message(reshade::log::level::warning, s.str().c_str());
      }
    }
  }
  
             // Dump any pending shaders (ignoring custom shaders and already dumped shaders)
     renodx::utils::shader::dump::DumpAllPending(custom_shaders, g_dumped_shaders, DumpShaderPrefix);
}
 // Sectioned settings generator functions
 std::vector<renodx::utils::settings::Setting*> GenerateSettingsModeSection() {
     return {
         new renodx::utils::settings::Setting{
             .key = "SettingsMode",
             .binding = &current_settings_mode,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("SettingsMode", 0.f),
             .can_reset = false,
             .label = "Settings Mode",
             .labels = {"Simple", "Intermediate", "Advanced", "Developer"},
             .is_global = true,
         },
     };
 }
 
 std::vector<renodx::utils::settings::Setting*> GenerateToneMappingSection() {
     return {
         new renodx::utils::settings::Setting{
             .key = "ToneMapType",
             .binding = &shader_injection.tone_map_type,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("ToneMapType", 3.f),
             .can_reset = true,
             .label = "Tone Mapper",
             .section = "Tone Mapping",
             .tooltip = "Sets the tone mapper type",
             .labels = {"Vanilla", "None", "ACES", "RenoDRT", "DICE", "Frostbite"},
             .is_visible = []() { return current_settings_mode >= 1; },
         },
         new renodx::utils::settings::Setting{
             .key = "ToneMapPeakNits",
             .binding = &shader_injection.peak_white_nits,
             .default_value = hbr_custom_settings::get_default_value("ToneMapPeakNits", 203.f),
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
             .default_value = hbr_custom_settings::get_default_value("ToneMapGameNits", 203.f),
             .label = "Game Brightness",
             .section = "Tone Mapping",
             .tooltip = "Sets the value of 100% white in nits",
             .min = 48.f,
             .max = 1000.f,
         },
         new renodx::utils::settings::Setting{
             .key = "ToneMapUINits2",
             .binding = &shader_injection.graphics_white_nits,
             .default_value = hbr_custom_settings::get_default_value("ToneMapUINits", 203.f),
             .label = "UI Brightness",
             .section = "Tone Mapping",
             .tooltip = "Sets the brightness of UI and HUD elements in nits",
             .min = 48.f,
             .max = 1000.f,
         },
         new renodx::utils::settings::Setting{
             .key = "GammaCorrection",
             .binding = &shader_injection.gamma_correction,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("GammaCorrection", 0.f),
             .label = "Gamma Correction",
             .section = "Tone Mapping",
             .tooltip = "Emulates a display EOTF.",
             .labels = {"Off", "2.2", "BT.1886"},
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "ToneMapScaling",
             .binding = &shader_injection.tone_map_per_channel,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("ToneMapScaling", 0.f),
             .label = "Scaling",
             .section = "Tone Mapping",
             .tooltip = "Luminance scales colors consistently while per-channel saturates and blows out sooner",
             .labels = {"Luminance", "Per Channel"},
             .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "ToneMapWorkingColorSpace",
             .binding = &shader_injection.tone_map_working_color_space,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("ToneMapWorkingColorSpace", 0.f),
             .label = "Working Color Space",
             .section = "Tone Mapping",
             .labels = {"BT709", "BT2020", "AP1"},
             .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "ToneMapHueProcessor",
             .binding = &shader_injection.tone_map_hue_processor,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("ToneMapHueProcessor", 0.f),
             .label = "Hue Processor",
             .section = "Tone Mapping",
             .tooltip = "Selects hue processor",
             .labels = {"OKLab", "ICtCp", "darkTable UCS"},
             .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "ToneMapHueCorrection",
             .binding = &shader_injection.tone_map_hue_correction,
             .default_value = hbr_custom_settings::get_default_value("ToneMapHueCorrection", 100.f),
             .label = "Hue Correction",
             .section = "Tone Mapping",
             .tooltip = "Hue retention strength.",
             .min = 0.f,
             .max = 200.f,
             .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "ToneMapHueShift",
             .binding = &shader_injection.tone_map_hue_shift,
             .default_value = hbr_custom_settings::get_default_value("ToneMapHueShift", 50.f),
             .label = "Hue Shift",
             .section = "Tone Mapping",
             .tooltip = "Hue-shift emulation strength.",
             .min = 0.f,
             .max = 100.f,
             .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "ToneMapClampColorSpace",
             .binding = &shader_injection.tone_map_clamp_color_space,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("ToneMapClampColorSpace", 2.f),
             .label = "Clamp Color Space",
             .section = "Tone Mapping",
             .tooltip = "Hue-shift emulation strength.",
             .labels = {"None", "BT709", "BT2020", "AP1"},
             .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
             .parse = [](float value) { return value - 1.f; },
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "ToneMapClampPeak",
             .binding = &shader_injection.tone_map_clamp_peak,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("ToneMapClampPeak", 0.f),
             .label = "Clamp Peak",
             .section = "Tone Mapping",
             .tooltip = "Hue-shift emulation strength.",
             .labels = {"None", "BT709", "BT2020", "AP1"},
             .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
             .parse = [](float value) { return value - 1.f; },
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "PostSwapChainToneMapping",
             .binding = &shader_injection.enable_tone_map_pass,
             .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
             .default_value = hbr_custom_settings::get_default_value("PostSwapChainToneMapping", 0.f),
             .label = "Post-SwapChain Tone Mapping",
             .section = "Tone Mapping",
             .tooltip = "Enables tone mapping pass to run after SwapChainPass for additional processing. Not recommended as this will apply tone mapping to UI elements. Useful when FakeHDR is enabled.",
             .labels = {"Off", "On"},
             .is_visible = []() { return current_settings_mode >= 2; },
         },
         new renodx::utils::settings::Setting{
             .key = "SimulateRenderPass",
             .binding = &shader_injection.simulate_render_pass,
             .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
             .default_value = hbr_custom_settings::get_default_value("SimulateRenderPass", 0.f),
             .label = "Simulate Render Pass",
             .section = "Tone Mapping",
             .tooltip = "Simulates render pass processing for intermediate color space conversion.",
             .labels = {"Off", "On"},
             .is_visible = []() { return current_settings_mode >= 3; },
         },
     };
 }
 
 std::vector<renodx::utils::settings::Setting*> GenerateColorGradingSection() {
     return {
         new renodx::utils::settings::Setting{
             .key = "ColorGradeExposure",
             .binding = &shader_injection.tone_map_exposure,
             .default_value = hbr_custom_settings::get_default_value("ColorGradeExposure", 1.f),
             .label = "Exposure",
             .section = "Color Grading",
             .max = 2.f,
             .format = "%.2f",
             .is_visible = []() { return current_settings_mode >= 1; },
         },
         new renodx::utils::settings::Setting{
             .key = "ColorGradeHighlights",
             .binding = &shader_injection.tone_map_highlights,
             .default_value = hbr_custom_settings::get_default_value("ColorGradeHighlights", 50.f),
             .label = "Highlights",
             .section = "Color Grading",
             .max = 100.f,
             .parse = [](float value) { return value * 0.02f; },
             .is_visible = []() { return current_settings_mode >= 1; },
         },
         new renodx::utils::settings::Setting{
             .key = "ColorGradeShadows",
             .binding = &shader_injection.tone_map_shadows,
             .default_value = hbr_custom_settings::get_default_value("ColorGradeShadows", 50.f),
             .label = "Shadows",
             .section = "Color Grading",
             .max = 100.f,
             .parse = [](float value) { return value * 0.02f; },
             .is_visible = []() { return current_settings_mode >= 1; },
         },
         new renodx::utils::settings::Setting{
             .key = "ColorGradeContrast",
             .binding = &shader_injection.tone_map_contrast,
             .default_value = hbr_custom_settings::get_default_value("ColorGradeContrast", 50.f),
             .label = "Contrast",
             .section = "Color Grading",
             .max = 100.f,
             .parse = [](float value) { return value * 0.02f; },
         },
         new renodx::utils::settings::Setting{
             .key = "ColorGradeSaturation",
             .binding = &shader_injection.tone_map_saturation,
             .default_value = hbr_custom_settings::get_default_value("ColorGradeSaturation", 50.f),
             .label = "Saturation",
             .section = "Color Grading",
             .max = 100.f,
             .parse = [](float value) { return value * 0.02f; },
         },
         new renodx::utils::settings::Setting{
             .key = "ColorGradeHighlightSaturation",
             .binding = &shader_injection.tone_map_highlight_saturation,
             .default_value = hbr_custom_settings::get_default_value("ColorGradeHighlightSaturation", 50.f),
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
             .default_value = hbr_custom_settings::get_default_value("ColorGradeBlowout", 0.f),
             .label = "Blowout",
             .section = "Color Grading",
             .tooltip = "Controls highlight desaturation due to overexposure.",
             .max = 100.f,
             .parse = [](float value) { return value * 0.01f; },
         },
         new renodx::utils::settings::Setting{
             .key = "ColorGradeFlare",
             .binding = &shader_injection.tone_map_flare,
             .default_value = hbr_custom_settings::get_default_value("ColorGradeFlare", 0.f),
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
             .default_value = hbr_custom_settings::get_default_value("ColorGradeScene", 100.f),
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
             .default_value = hbr_custom_settings::get_default_value("ColorGradeClip", 65.f),
             .label = "RenoDX White Clip",
             .section = "Color Grading",
             .tooltip = "Clip point for white in nits",
             .min = 1.f,
             .max = 100.f,
             .is_enabled = []() { return shader_injection.tone_map_type == 3.f; },
             .is_visible = []() { return current_settings_mode >= 1; },
         },
         new renodx::utils::settings::Setting{
             .key = "DiceShoulder",
             .binding = &shader_injection.dice_shoulder,
             .default_value = 0.33f,
             .can_reset = true,
             .label = "DICE Shoulder",
             .section = "Color Grading",
             .tooltip = "Adjusts the shoulder start point for DICE tone mapping",
             .min = 0.0f,
             .max = 1.0f,
             .format = "%.2f",
             .is_enabled = []() { return shader_injection.tone_map_type == 4.f; },
             .is_visible = []() { return current_settings_mode >= 1; },
         },
         new renodx::utils::settings::Setting{
             .key = "ColorGradePerChannelBlowoutRestoration",
             .binding = &shader_injection.color_grade_per_channel_blowout_restoration,
             .value_type = renodx::utils::settings::SettingValueType::FLOAT,
             .default_value = hbr_custom_settings::get_default_value("ColorGradePerChannelBlowoutRestoration", 0.f),
             .can_reset = true,
             .label = "Per Channel Blowout Restoration",
             .section = "Color Grading",
             .tooltip = "Restores blown out highlights on a per-channel basis.",
             .max = 100.f,
             .is_enabled = []() { return shader_injection.tone_map_type > 0; },
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 2; },
         },
         new renodx::utils::settings::Setting{
             .key = "ColorGradePerChannelHueCorrection",
             .binding = &shader_injection.color_grade_per_channel_hue_correction,
             .value_type = renodx::utils::settings::SettingValueType::FLOAT,
             .default_value = hbr_custom_settings::get_default_value("ColorGradePerChannelHueCorrection", 0.f),
             .can_reset = true,
             .label = "Per Channel Hue Correction",
             .section = "Color Grading",
             .tooltip = "Applies hue correction on a per-channel basis.",
             .max = 100.f,
             .is_enabled = []() { return shader_injection.tone_map_type > 0; },
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 2; },
         },
         new renodx::utils::settings::Setting{
             .key = "ColorGradePerChannelChrominanceCorrection",
             .binding = &shader_injection.color_grade_per_channel_chrominance_correction,
             .value_type = renodx::utils::settings::SettingValueType::FLOAT,
             .default_value = hbr_custom_settings::get_default_value("ColorGradePerChannelChrominanceCorrection", 0.f),
             .can_reset = true,
             .label = "Per Channel Chrominance Correction",
             .section = "Color Grading",
             .tooltip = "Applies chrominance correction on a per-channel basis.",
             .min = 0.f,
             .max = 100.f,
             .is_enabled = []() { return shader_injection.tone_map_type > 0; },
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 2; },
         },
         new renodx::utils::settings::Setting{
             .key = "ColorGradeGamma",
             .binding = &shader_injection.tone_map_gamma, // Make sure this field exists in ShaderInjectData
             .default_value = hbr_custom_settings::get_default_value("ColorGradeGamma", 50.f),
             .label = "Gamma",
             .section = "Color Grading",
             .tooltip = "Adjusts the gamma curve for color grading.",
             .min = 0.f,
             .max = 100.f,
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 1; },
         },
     };
 }
 
 std::vector<renodx::utils::settings::Setting*> GenerateDisplayOutputSection() {
     return {
         new renodx::utils::settings::Setting{
             .key = "SwapChainCustomColorSpace",
             .binding = &shader_injection.swap_chain_custom_color_space,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("SwapChainCustomColorSpace", 0.f),
             .label = "Custom Color Space (recommended JPN Modern on 6500k monitor)",
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
         },
         new renodx::utils::settings::Setting{
             .key = "IntermediateEncoding",
             .binding = &shader_injection.intermediate_encoding,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("IntermediateEncoding", 2.f),
             .label = "Intermediate Encoding",
             .section = "Display Output",
             .labels = {"Auto", "None", "SRGB", "2.2", "2.4"},
         //    .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
             .parse = [](float value) {
                 if (value == 0) return shader_injection.gamma_correction + 1.f;
                 return value - 1.f; },
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "SwapChainDecoding",
             .binding = &shader_injection.swap_chain_decoding,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("SwapChainDecoding", 3.f),
             .label = "Swapchain Decoding",
             .section = "Display Output",
             .labels = {"Auto", "None", "SRGB", "2.2", "2.4"},
      //       .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
             .parse = [](float value) {
                 if (value == 0) return shader_injection.intermediate_encoding;
                 return value - 1.f; },
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "SwapChainGammaCorrection",
             .binding = &shader_injection.swap_chain_gamma_correction,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("SwapChainGammaCorrection", 0.f),
             .label = "Gamma Correction",
             .section = "Display Output",
             .labels = {"None", "2.2", "2.4"},
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "SwapChainClampColorSpace",
             .binding = &shader_injection.swap_chain_clamp_color_space,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("SwapChainClampColorSpace", 2.f),
             .label = "Clamp Color Space",
             .section = "Display Output",
             .labels = {"None", "BT709", "BT2020", "AP1"},
             .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
             .parse = [](float value) { return value - 1.f; },
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "EffectSplitMode",
             .binding = &shader_injection.effect_split_mode,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("EffectSplitMode", 0.f),
             .label = "Split Mode",
             .section = "Display Output",
             .tooltip = "Choose the split mode for effect application.",
             .labels = {"Off", "On"},
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "EffectSplitX",
             .binding = &shader_injection.effect_split_x,
             .default_value = hbr_custom_settings::get_default_value("EffectSplitX", 1.f), // Default to half of 1920 width, adjust as needed
             .label = "Effect Split X",
             .section = "Display Output",
             .tooltip = "Only apply effects to the left of this screen X coordinate.",
             .min = -1.f,
             .max = 1.f, // Or your max screen width
             .format = "%.3f",
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "EffectSplitAngle",
             .binding = &shader_injection.effect_split_angle,
             .default_value = hbr_custom_settings::get_default_value("EffectSplitAngle", 0.f),
             .label = "Split Angle",
             .section = "Display Output",
             .tooltip = "Angle (in degrees) for the split line (0 = vertical, 90 = horizontal, etc.)",
             .min = 0.f,
             .max = 360.f,
             .is_visible = []() { return current_settings_mode >= 3 && shader_injection.effect_split_mode != 0; },
         },
         new renodx::utils::settings::Setting{
             .key = "DisableSwapChainUpgrade",
             .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
             .default_value = hbr_custom_settings::get_default_value("DisableSwapChainUpgrade", 0.f),
             .label = "Disable Swap Chain Upgrade",
             .section = "Display Output",
             .tooltip = "When enabled, forces resize buffer mode for swap chain handling",
             .labels = {"Off", "On"},
             .on_change_value = [](float previous, float current) { 
                 if (current == 1.f) {
                     renodx::mods::swapchain::use_resize_buffer_on_demand = true;
                     renodx::mods::swapchain::use_resize_buffer = true;
                 }
             },
             .is_visible = []() { return current_settings_mode >= 2; },
         },
     };
 }
 
 std::vector<renodx::utils::settings::Setting*> GeneratePerceptualBoostSection() {
     return {
         new renodx::utils::settings::Setting{
             .key = "PerceptualBoostMode",
             .binding = &shader_injection.perceptual_boost_mode,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoostMode", 0.f),
             .label = "Perceptual Boost",
             .section = "Perceptual Boost",
             .tooltip = "Selects the method for perceptual boost.",
             .labels = {"OFF", "Reinhard", "XY->PQ", "ICTCP"},
             .is_visible = []() { return current_settings_mode >= 1; },
         },
         new renodx::utils::settings::Setting{
             .key = "PerceptualBoostChannelMax",
             .binding = &shader_injection.perceptual_boost_channel_max,
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoostChannelMax", 2.f),
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
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoostReinhardStrength", 100.f),
             .label = "Reinhard Strength",
             .section = "Perceptual Boost",
             .tooltip = "Controls the overall strength of Reinhard perceptual boost effect (0% to 100%)",
             .min = 0.f,
             .max = 100.f,
             .is_enabled = []() { return shader_injection.perceptual_boost_mode == 1.f; },
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 1; },
         },
         new renodx::utils::settings::Setting{
             .key = "PerceptualBoostReinhardMidpoint",
             .binding = &shader_injection.perceptual_boost_reinhard_midpoint,
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoostReinhardMidpoint", 5.f),
             .label = "Reinhard Midpoint",
             .section = "Perceptual Boost",
             .tooltip = "Controls the midpoint for Reinhard perceptual boost (0.01 to 1.0, default: 0.18)",
             .min = 1.f,
             .max = 100.f,
             .is_enabled = []() { return shader_injection.perceptual_boost_mode == 1.f; },
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 2; },
         },
         // XY->PQ mode settings
         new renodx::utils::settings::Setting{
             .key = "PerceptualBoostXYPQParam",
             .binding = &shader_injection.perceptual_boost_xypq_param,
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoostXYPQParam", 389.f),
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
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoostXYPQColor", 36.f),
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
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoostXYPQStrength", 100.f),
             .label = "XY->PQ Strength",
             .section = "Perceptual Boost",
             .tooltip = "Controls the overall strength of XY->PQ perceptual boost effect (0% to 100%)",
             .min = 0.f,
             .max = 100.f,
             .is_enabled = []() { return shader_injection.perceptual_boost_mode == 2.f; },
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 1; },
         },
         new renodx::utils::settings::Setting{
             .key = "PerceptualBoostXYPQMidpoint",
             .binding = &shader_injection.perceptual_boost_xypq_midpoint,
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoostXYPQMidpoint", 1.f),
             .label = "XY->PQ Midpoint",
             .section = "Perceptual Boost",
             .tooltip = "Controls the midpoint for XY->PQ perceptual boost (0.01 to 1.0, default: 0.18)",
             .min = 1.f,
             .max = 100.f,
             .is_enabled = []() { return shader_injection.perceptual_boost_mode == 2.f; },
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 2; },
         },
         // ICTCP mode settings
         new renodx::utils::settings::Setting{
             .key = "PerceptualBoostICTCPParam",
             .binding = &shader_injection.perceptual_boost_ictcp_param,
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoostICTCPParam", 268.f),
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
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoostICTCPColor", 20.f),
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
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoostICTCPStrength", 100.f),
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
             .key = "PerceptualBoostICTCPMidpoint",
             .binding = &shader_injection.perceptual_boost_ictcp_midpoint,
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoostICTCPMidpoint", 18.f),
             .label = "ICTCP Midpoint",
             .section = "Perceptual Boost",
             .tooltip = "Controls the midpoint for ICTCP perceptual boost (0.01 to 1.0, default: 0.18)",
             .min = 1.f,
             .max = 100.f,
             .is_enabled = []() { return shader_injection.perceptual_boost_mode == 3.f; },
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 2; },
         },
         new renodx::utils::settings::Setting{
             .key = "PerceptualBoost2DCharacter",
             .binding = &shader_injection.perceptual_boost_2d_character,
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoost2DCharacter", 0.f),
             .label = "2D Character Scenes",
             .section = "Perceptual Boost",
             .tooltip = "Perceptual boost strength for 2D character scenes (0.0 = disabled, 1.0 = normal, 10.0 = maximum)",
             .min = 0.f,
             .max = 100.f,
             .is_enabled = []() { return shader_injection.perceptual_boost_mode > 0.f; },
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 1; },
         },
         new renodx::utils::settings::Setting{
             .key = "PerceptualBoost2DBackground",
             .binding = &shader_injection.perceptual_boost_2d_background,
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoost2DBackground", 0.f),
             .label = "2D Background Scenes",
             .section = "Perceptual Boost",
             .tooltip = "Perceptual boost strength for 2D background scenes (0.0 = disabled, 1.0 = normal, 10.0 = maximum)",
             .min = 0.f,
             .max = 100.f,
             .is_enabled = []() { return shader_injection.perceptual_boost_mode > 0.f; },
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 1; },
         },
         new renodx::utils::settings::Setting{
             .key = "PerceptualBoost3D",
             .binding = &shader_injection.perceptual_boost_3d,
             .default_value = hbr_custom_settings::get_default_value("PerceptualBoost3D", 0.f),
             .label = "3D Scenes",
             .section = "Perceptual Boost",
             .tooltip = "Perceptual boost strength for 3D scenes (0.0 = disabled, 1.0 = normal, 10.0 = maximum)",
             .min = 0.f,
             .max = 100.f,
             .is_enabled = []() { return shader_injection.perceptual_boost_mode > 0.f; },
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 1; },
         },
         new renodx::utils::settings::Setting{
             .key = "PerceptualBoostDisplayOutput",
             .binding = &shader_injection.perceptual_boost_display_output,
             .default_value = 100.f,
             .label = "Display Output",
             .section = "Perceptual Boost",
             .tooltip = "Perceptual boost strength for display output (0.0 = disabled, 1.0 = normal, 10.0 = maximum)",
             .min = 0.f,
             .max = 100.f,
             .is_enabled = []() { return shader_injection.perceptual_boost_mode > 0.f; },
             .parse = [](float value) { return value * 0.01f; },
             .is_visible = []() { return current_settings_mode >= 1; },
         },
     };
 }
 
 
   std::vector<renodx::utils::settings::Setting*> GenerateDebugSection() {
      return {
          new renodx::utils::settings::Setting{
              .key = "DebugMode",
              .binding = &shader_injection.debug_mode,
              .default_value = hbr_custom_settings::get_default_value("DebugMode", 0.f),
              .label = "Debug Mode",
              .section = "Debug",
              .tooltip = "Debug mode for development and testing (0.0-1.0)",
              .min = 0.f,
              .max = 1.f,
              .format = "%.2f",
              .is_visible = []() { return current_settings_mode >= 3; },
          },
         new renodx::utils::settings::Setting{
             .key = "DebugMode2",
             .binding = &shader_injection.debug_mode2,
             .default_value = hbr_custom_settings::get_default_value("DebugMode2", 1.f),
             .label = "Debug Mode 2",
             .section = "Debug",
             .tooltip = "Debug mode 2 for development and testing (0.0-1.0)",
             .min = 0.f,
             .max = 1.f,
             .format = "%.2f",
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "DebugMode3",
             .binding = &shader_injection.debug_mode3,
             .default_value = hbr_custom_settings::get_default_value("DebugMode3", 1.f),
             .label = "Debug Mode 3",
             .section = "Debug",
             .tooltip = "Debug mode 3 for development and testing (0.0-1.0)",
             .min = 0.f,
             .max = 1.f,
             .format = "%.2f",
             .is_visible = []() { return current_settings_mode >= 3; },
         },
     };
 }
 
 std::vector<renodx::utils::settings::Setting*> GenerateHighlightSaturationRestorationSection() {
     return {
         new renodx::utils::settings::Setting{
             .key = "DisplayMapType",
             .binding = &shader_injection.display_map_type,
             .value_type = renodx::utils::settings::SettingValueType::INTEGER,
             .default_value = hbr_custom_settings::get_default_value("DisplayMapType", 3.f),
             .can_reset = true,
             .label = "Display Map Type",
             .section = "Highlight Saturation Restoration",
             .tooltip = "Sets the Display mapper used",
             .labels = {"None", "DICE", "Frostbite", "RenoDRT NeutralSDR", "ToneMapMaxCLL"},
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "DisplayMapSaturation",
             .binding = &shader_injection.display_map_saturation,
             .value_type = renodx::utils::settings::SettingValueType::FLOAT,
             .default_value = hbr_custom_settings::get_default_value("DisplayMapSaturation", 0.f),
             .can_reset = true,
             .label = "Display Map Saturation",
             .section = "Highlight Saturation Restoration",
             .tooltip = "Controls the saturation strength of the display mapping effect.",
             .min = 0.f,
             .max = 1.f,
             .format = "%.2f",
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "DisplayMapPeak",
             .binding = &shader_injection.display_map_peak,
             .value_type = renodx::utils::settings::SettingValueType::FLOAT,
             .default_value = hbr_custom_settings::get_default_value("DisplayMapPeak", 2.f),
             .can_reset = true,
             .label = "Display Map Peak",
             .section = "Highlight Saturation Restoration",
             .tooltip = "What nit value we want to display map down to -- 2.f is solid",
             .min = 1.f,
             .max = 5.f,
             .format = "%.2f",
             .is_visible = []() { return current_settings_mode >= 3; },
         },
         new renodx::utils::settings::Setting{
             .key = "DisplayMapShoulder",
             .binding = &shader_injection.display_map_shoulder,
             .value_type = renodx::utils::settings::SettingValueType::FLOAT,
             .default_value = hbr_custom_settings::get_default_value("DisplayMapShoulder", 0.5f),
             .can_reset = true,
             .label = "Display Map Shoulder",
             .section = "Highlight Saturation Restoration",
             .tooltip = "Determines where the highlights curve (shoulder) starts in the display mapper.",
             .max = 1.f,
             .format = "%.2f",
             .is_visible = []() { return current_settings_mode >= 3; },
         },
     };
 }
 
 // The global settings vector
 renodx::utils::settings::Settings settings;
 
 void AddAdvancedSettings() {
   auto* swapchain_setting = new renodx::utils::settings::Setting{
       .key = "Upgrade_SwapChainCompatibility",
       .value_type = renodx::utils::settings::SettingValueType::INTEGER,
       .default_value = hbr_custom_settings::get_default_value("Upgrade_SwapChainCompatibility", 0.f),
       .label = "Swap Chain Compatibility Mode",
       .section = "About",
       .tooltip = "Enhances support for third-party addons to read the swap chain.",
       .labels = {
           "Off",
           "On",
       },
       .is_global = true,
       .is_visible = []() { return current_settings_mode >= 3; },
   };
   reshade::get_config_value(nullptr, renodx::utils::settings::global_name.c_str(), "Upgrade_SwapChainCompatibility", swapchain_setting->value_as_int);
   renodx::mods::swapchain::swapchain_proxy_compatibility_mode = swapchain_setting->GetValue() != 0;
   settings.push_back(swapchain_setting);
 }
 
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
 void InitializeSettings() {
     settings.clear();
     
     // Add version display at the top
     settings.push_back(new renodx::utils::settings::Setting{
         .value_type = renodx::utils::settings::SettingValueType::TEXT,
         .label = std::string("Version: ") + RENODX_VERSION,
         .section = "About",
         .tooltip = std::string("RenoDX Universal Template Version ") + RENODX_VERSION,
     });
     
     // Add each section's settings
     auto add_section = [](const std::vector<renodx::utils::settings::Setting*>& section) {
         settings.insert(settings.end(), section.begin(), section.end());
     };
     add_section(GenerateSettingsModeSection());
     add_section(GenerateToneMappingSection());
     add_section(GenerateColorGradingSection());
     add_section(GenerateDisplayOutputSection());
     add_section(GeneratePerceptualBoostSection());
     add_section(GenerateDebugSection());
     add_section(GenerateHighlightSaturationRestorationSection());
     add_section(hbr_custom_settings::GenerateCustomGameSettingsSection(shader_injection, current_settings_mode)); // Use external function
     AddAdvancedSettings();
     settings.push_back(new renodx::utils::settings::Setting{
         .key = "UpgradeResourceViewCloning",
         .value_type = renodx::utils::settings::SettingValueType::INTEGER,
         .default_value = hbr_custom_settings::get_default_value("UpgradeResourceViewCloning", 0.f),
         .label = "Resource View Cloning (Global)",
         .section = "Resource Upgrades",
         .tooltip = "If enabled, use resource view cloning for all resource upgrades. (May fix or cause issues depending on the game)",
         .labels = {"Disabled", "Enabled"},
         .is_global = true,
         .is_visible = []() { return current_settings_mode >= 3; },
     });
 }
 
 }  // namespace
 
 
 
 extern "C" __declspec(dllexport) constexpr const char* NAME = "RenoDX";
 extern "C" __declspec(dllexport) constexpr const char* DESCRIPTION = "RenoDX (Generic)";
 
 BOOL APIENTRY DllMain(HMODULE h_module, DWORD fdw_reason, LPVOID lpv_reserved) {
     
   switch (fdw_reason) {
     case DLL_PROCESS_ATTACH:
       if (!reshade::register_addon(h_module)) return FALSE;
 
       if (!initialized) {
                   // Print exe name and title
          auto process_path = renodx::utils::platform::GetCurrentProcessPath();
          auto filename = process_path.filename().string();
          auto product_name = renodx::utils::platform::GetProductName(process_path);
          bool is_no_creeps_were_harmed = filename == "No Creeps Were Harmed TD.exe";
          reshade::log::message(reshade::log::level::info, std::format("RenoDX Universal loaded for {} ({}) {}.", filename, product_name, is_no_creeps_were_harmed ? "No Creeps Were Harmed TD" : "").c_str());

        // Apply filename-based settings overrides
         hbr_custom_settings::ApplyFilenameBasedOverrides(filename);

         InitializeSettings();
         hbr_custom_settings::AddCustomResourceUpgrades();
 
         renodx::mods::shader::force_pipeline_cloning = true;
         renodx::mods::shader::expected_constant_buffer_space = 50;
         renodx::mods::shader::expected_constant_buffer_index = 13;
         renodx::mods::shader::allow_multiple_push_constants = true;
 
         renodx::mods::swapchain::expected_constant_buffer_index = 13;
         renodx::mods::swapchain::expected_constant_buffer_space = 50;
         renodx::mods::swapchain::use_resource_cloning = true;
         renodx::utils::random::binds.push_back(&shader_injection.random_seed);
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
              .key = "AutomaticShaderDumping",
              .binding = &g_dump_shaders,
              .value_type = renodx::utils::settings::SettingValueType::INTEGER,
              .default_value = 0.f,
              .label = "Automatic Shader Dumping",
              .section = "Debug",
              .tooltip = "Automatically dump shaders into renodx-dump folder.",
              .labels = {
                  "Off",
                  "On",
              },
              .is_global = true,
              .is_visible = []() { return current_settings_mode >= 3; }
             };
             renodx::utils::settings::LoadSetting(renodx::utils::settings::global_name, setting);
             settings.push_back(setting);
             g_dump_shaders = setting->GetValue();
          }
          {
            auto* setting = new renodx::utils::settings::Setting{
              .key = "AutodumpLutbuilders",
              .binding = &g_autodump_lutbuilders,
              .value_type = renodx::utils::settings::SettingValueType::INTEGER,
              .default_value = 0.f,
              .label = "Autodump Lutbuilders",
              .section = "Debug",
              .tooltip = "Automatically dump only lutbuilder shaders. Works independently of general shader dumping.",
              .labels = {
                  "Off",
                  "On",
              },
              .is_global = true,
              .is_visible = []() { return current_settings_mode >= 3; }
             };
             renodx::utils::settings::LoadSetting(renodx::utils::settings::global_name, setting);
             settings.push_back(setting);
             g_autodump_lutbuilders = setting->GetValue();
         }
         {
           auto* setting = new renodx::utils::settings::Setting{
               .key = "SwapChainForceBorderless",
               .value_type = renodx::utils::settings::SettingValueType::INTEGER,
               .default_value = hbr_custom_settings::get_default_value("SwapChainForceBorderless", 0.f),
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
               .default_value = hbr_custom_settings::get_default_value("SwapChainPreventFullscreen", 0.f),
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
               .default_value = hbr_custom_settings::get_default_value("SwapChainEncoding", 5.f),
               .label = "Encoding",
               .section = "Display Output",
               .labels = {"None", "SRGB", "2.2", "2.4", "HDR10", "scRGB"},
           //    .is_enabled = []() { return shader_injection.tone_map_type >= 1; },
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
 
         for (const auto& [key, format_pair] : hbr_custom_settings::UPGRADE_TARGETS) {
           auto* setting = new renodx::utils::settings::Setting{
               .key = "Upgrade_" + key,
               .value_type = renodx::utils::settings::SettingValueType::INTEGER,
               .default_value = hbr_custom_settings::get_default_value("Upgrade_" + key, format_pair.second),
               .label = key,
               .section = "Resource Upgrades",
               .labels = {
                   "Off",
                   "Output size",
                   "Output ratio",
                   "Any size",
               },
               .is_global = true,
               .is_visible = []() { return current_settings_mode >= 3; },
           };
           renodx::utils::settings::LoadSetting(renodx::utils::settings::global_name, setting);
           settings.push_back(setting);
 
           auto value = setting->GetValue();
           if (value > 0) {
             renodx::mods::swapchain::swap_chain_upgrade_targets.push_back({
                 .old_format = format_pair.first,
                 .new_format = reshade::api::format::r16g16b16a16_float,
                 .ignore_size = (value == UPGRADE_TYPE_ANY),
                 .use_resource_view_cloning = hbr_custom_settings::get_upgrade_resource_view_cloning(),
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
                   {
           std::stringstream s;
   
           s << "g_dump_shaders " << g_dump_shaders << ", g_autodump_lutbuilders " << g_autodump_lutbuilders;
           reshade::log::message(reshade::log::level::info, s.str().c_str());
          }
          if (g_dump_shaders != 0.f || g_autodump_lutbuilders != 0.f) {
            // Scan existing dumped shaders for performance optimization
            ScanExistingDumpedShaders();
            
            renodx::utils::swapchain::Use(fdw_reason);
            renodx::utils::shader::Use(fdw_reason);
            renodx::utils::shader::use_shader_cache = true;
            renodx::utils::shader::dump::Use(fdw_reason);
            renodx::utils::resource::Use(fdw_reason);
          //  reshade::register_event<reshade::addon_event::init_pipeline>(OnInitPipelineTrackAddons);
            reshade::register_event<reshade::addon_event::init_pipeline>(OnInitPipeline);
            
            if (g_dump_shaders != 0.f && g_autodump_lutbuilders != 0.f) {
              reshade::log::message(reshade::log::level::info, "Shader dumping and lutbuilder dumping enabled.");
            } else if (g_dump_shaders != 0.f) {
              reshade::log::message(reshade::log::level::info, "General shader dumping enabled.");
            } else {
              reshade::log::message(reshade::log::level::info, "Lutbuilder dumping enabled.");
            }
          }
   
         initialized = true;
       }
       break;
     case DLL_PROCESS_DETACH:
       renodx::utils::shader::Use(fdw_reason);
       renodx::utils::shader::dump::Use(fdw_reason);
       renodx::utils::swapchain::Use(fdw_reason);
       renodx::utils::resource::Use(fdw_reason);
    //   reshade::unregister_event<reshade::addon_event::init_pipeline>(OnInitPipelineTrackAddons);
       reshade::unregister_event<reshade::addon_event::init_pipeline>(OnInitPipeline);
     //  reshade::unregister_event<reshade::addon_event::draw>(OnDrawForLUTDump);
       reshade::unregister_addon(h_module);
       break;
   }
 
   renodx::utils::settings::Use(fdw_reason, &settings, &OnPresetOff);
   renodx::mods::swapchain::Use(fdw_reason, &shader_injection);
   renodx::mods::shader::Use(fdw_reason, custom_shaders, &shader_injection);
 
   return TRUE;
 }
 



