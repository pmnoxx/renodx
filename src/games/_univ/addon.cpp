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
- 0.26
fix dump shaders setting
- 0.27
add log resource creation setting
- 0.28
Totalwar Warhammer III support
- 0.29
Added Darkest Dungeons II support
- 0.30
Added Against the Storm support
*/

constexpr const char* RENODX_VERSION = "0.30";

#define ImTextureID ImU64

#define DEBUG_LEVEL_0

#include <deps/imgui/imgui.h>

#include "../../mods/shader.hpp"
#include "../../mods/swapchain.hpp"
#include "../../utils/platform.hpp"
#include "../../utils/random.hpp"
#include "../../utils/resource.hpp"
#include "../../utils/settings.hpp"
#include "../../utils/shader.hpp"
#include "../../utils/shader_dump.hpp"
#include "../../utils/swapchain.hpp"
#include "./shared.h"
#include "custom_settings.cpp"

#include <embed/shaders.h>
#include <algorithm>
#include <crc32_hash.hpp>
#include <cstring>
#include <include/reshade.hpp>
#include <map>
#include <mutex>

// Slot tracking for first 10 slots
reshade::api::resource_view g_bound_slots[10] = {};
// Track bound slots for first 10 slots
inline void OnPushDescriptors(
    reshade::api::command_list* cmd_list,
    reshade::api::shader_stage stages,
    reshade::api::pipeline_layout layout,
    uint32_t layout_param,
    const reshade::api::descriptor_table_update& update) {
    if (update.count == 0u) return;

    for (uint32_t i = 0; i < update.count; i++) {
        // Track both SRVs and UAVs for the first 10 slots
        if (update.type == reshade::api::descriptor_type::texture_shader_resource_view ||
            update.type == reshade::api::descriptor_type::texture_unordered_access_view) {

            uint32_t slot = update.binding + i;
            if (slot < 10) {  // Only track first 10 slots
                const reshade::api::resource_view* views = static_cast<const reshade::api::resource_view*>(update.descriptors);
                g_bound_slots[slot] = views[i];
            }
        }
    }
}
/*
// Function version of UpgradeSRVReplaceShader
inline std::pair<uint32_t, renodx::mods::shader::CustomShader> CreateUpgradeSRVReplaceShaderExperimental(uint32_t value, int slot) {
    return {
        value,
        {
            .crc32 = value,
            .code = {}, // This will be set by the macro expansion
            .on_draw = [slot](auto* cmd_list) {
                // Get the SRV from slot `slot` (first shader resource view slot)
                if (g_bound_slots[slot].handle != 0u) {
                    bool changed = renodx::mods::swapchain::ActivateCloneHotSwap(cmd_list->get_device(), g_bound_slots[slot]);
                    if (changed) {
                        renodx::mods::swapchain::FlushDescriptors(cmd_list);
                        renodx::mods::swapchain::RewriteRenderTargets(cmd_list, 1, &g_bound_slots[slot], {0});
                        // Note: We might need to implement a RewriteShaderResourceViews function
                        // similar to RewriteRenderTargets for SRVs
                    }
                }
                return true;
            },
        }
    };
}

// Function version of UpgradeRTVShader
inline std::pair<uint32_t, renodx::mods::shader::CustomShader> CreateUpgradeRTVShaderExperimental(uint32_t value) {
    return {
        value,
        {
            .crc32 = value,
            .on_draw = [](auto* cmd_list) {
                auto rtvs = renodx::utils::swapchain::GetRenderTargets(cmd_list);
                bool changed = false;
                for (auto rtv : rtvs) {
                    changed = renodx::mods::swapchain::ActivateCloneHotSwap(cmd_list->get_device(), rtv);
                }
                if (changed) {
                    renodx::mods::swapchain::FlushDescriptors(cmd_list);
                    renodx::mods::swapchain::RewriteRenderTargets(cmd_list, rtvs.size(), rtvs.data(), {0});
                }
                return true;
            },
        }
    };
}

// Function version of UpgradeRTVReplaceShader for Ixion
inline std::pair<uint32_t, renodx::mods::shader::CustomShader> CreateUpgradeRTVReplaceShaderExperimental(uint32_t value) {
    return {
        value,
        {
            .crc32 = value,
            .code = {}, // This will be set by the macro expansion
            .on_draw = [](auto* cmd_list) {
                auto rtvs = renodx::utils::swapchain::GetRenderTargets(cmd_list);
                bool changed = false;
                for (auto rtv : rtvs) {
                    changed = renodx::mods::swapchain::ActivateCloneHotSwap(cmd_list->get_device(), rtv);
                }
                if (changed) {
                    renodx::mods::swapchain::FlushDescriptors(cmd_list);
                    renodx::mods::swapchain::RewriteRenderTargets(cmd_list, rtvs.size(), rtvs.data(), {0});
                }
                return true;
            },
        }
    };
} */

#define CreateUpgradeSRVReplaceShader(value, slot) \
    {\
        value, \
        { \
            .crc32 = value,\
            .code = __##value, \
            .on_draw = [](auto* cmd_list) {\
                if (g_bound_slots[slot].handle != 0u) {\
                    bool changed = renodx::mods::swapchain::ActivateCloneHotSwap(cmd_list->get_device(), g_bound_slots[slot]);\
                    if (changed) {\
                        renodx::mods::swapchain::FlushDescriptors(cmd_list);\
                        renodx::mods::swapchain::RewriteRenderTargets(cmd_list, 1, &g_bound_slots[slot], {0});\
                    }\
                }\
                return true;\
            },\
        },\
    }



#define UpgradeRTVShader(value)                                                                                 \
    {                                                                                                           \
        value,                                                                                                  \
        {                                                                                                       \
            .crc32 = value,                                                                                     \
            .on_draw =                                                                                          \
                [](auto* cmd_list) {                                                                            \
                    auto rtvs = renodx::utils::swapchain::GetRenderTargets(cmd_list);                           \
                    bool changed = false;                                                                       \
                    for (auto rtv : rtvs) {                                                                     \
                        changed = renodx::mods::swapchain::ActivateCloneHotSwap(cmd_list->get_device(), rtv);   \
                    }                                                                                           \
                    if (changed) {                                                                              \
                        renodx::mods::swapchain::FlushDescriptors(cmd_list);                                    \
                        renodx::mods::swapchain::RewriteRenderTargets(cmd_list, rtvs.size(), rtvs.data(), {0}); \
                    }                                                                                           \
                    return true;                                                                                \
                },                                                                                              \
        },                                                                                                      \
    }

#define UpgradeRTVReplaceShader(value)                                                                          \
    {                                                                                                           \
        value,                                                                                                  \
        {                                                                                                       \
            .crc32 = value,                                                                                     \
            .code = __##value,                                                                                  \
            .on_draw =                                                                                          \
                [](auto* cmd_list) {                                                                            \
                    auto rtvs = renodx::utils::swapchain::GetRenderTargets(cmd_list);                           \
                    bool changed = false;                                                                       \
                    for (auto rtv : rtvs) {                                                                     \
                        changed = renodx::mods::swapchain::ActivateCloneHotSwap(cmd_list->get_device(), rtv);   \
                    }                                                                                           \
                    if (changed) {                                                                              \
                        renodx::mods::swapchain::FlushDescriptors(cmd_list);                                    \
                        renodx::mods::swapchain::RewriteRenderTargets(cmd_list, rtvs.size(), rtvs.data(), {0}); \
                    }                                                                                           \
                    return true;                                                                                \
                },                                                                                              \
        },                                                                                                      \
    }

namespace {

renodx::mods::shader::CustomShaders custom_shaders = {
    //  CustomShaderEntry(0xC22C6A84),
    __ALL_CUSTOM_SHADERS};

ShaderInjectData shader_injection;

float g_dump_shaders = 0;
float g_upgrade_copy_destinations = 0;
float g_autodump_lutbuilders = 0;
std::unordered_set<uint32_t> g_dumped_shaders = {};

float current_settings_mode = 0;
float g_log_resource_creation = 0;

// Resolution and bit depth tracking
struct ResolutionBitDepthStats {
    std::map<std::pair<int, int>, std::map<std::string, int>> before_upgrade;  // (width, height) -> bit_depth -> count
    std::map<std::pair<int, int>, std::map<std::string, int>> upgraded;        // (width, height) -> bit_depth -> count
    std::map<std::pair<int, int>, std::map<std::string, int>> after_upgrade;   // (width, height) -> bit_depth -> count
    std::mutex mutex;
} g_resolution_bitdepth_stats;

// Callback to track resource creation for resolution/bit depth statistics
void OnResourceCreated(renodx::utils::resource::ResourceInfo* resource_info) {
    if (resource_info == nullptr) return;

    const auto& desc = resource_info->desc;

    // Only track 2D textures and surfaces
    if (desc.type != reshade::api::resource_type::texture_2d && desc.type != reshade::api::resource_type::surface) {
        return;
    }

    // Skip if no dimensions
    if (desc.texture.width == 0 || desc.texture.height == 0) {
        return;
    }

    // Create resolution pair for proper sorting
    std::pair<int, int> resolution = {desc.texture.width, desc.texture.height};

    // Determine format category based on upgrade targets
    std::string bit_depth = "Unknown";
    switch (desc.texture.format) {
        // R8G8B8A8 formats
        case reshade::api::format::r8g8b8a8_typeless:   bit_depth = "rgba8_t"; break;
        case reshade::api::format::r8g8b8a8_unorm:      bit_depth = "rgba8_u"; break;
        case reshade::api::format::r8g8b8a8_snorm:      bit_depth = "rgba8_s"; break;
        case reshade::api::format::r8g8b8a8_unorm_srgb: bit_depth = "rgba8_srgb"; break;

        // B8G8R8A8 formats
        case reshade::api::format::b8g8r8a8_typeless:   bit_depth = "bgra8_t"; break;
        case reshade::api::format::b8g8r8a8_unorm:      bit_depth = "bgra8_u"; break;
        case reshade::api::format::b8g8r8a8_unorm_srgb: bit_depth = "bgra8_srgb"; break;

        // 10/11-bit formats
        case reshade::api::format::r10g10b10a2_typeless: bit_depth = "rgb10a2_t"; break;
        case reshade::api::format::r10g10b10a2_unorm:    bit_depth = "rgb10a2_u"; break;
        case reshade::api::format::b10g10r10a2_unorm:    bit_depth = "bgr10a2_u"; break;
        case reshade::api::format::r11g11b10_float:      bit_depth = "r11g11b10_f"; break;

        // 16-bit formats
        case reshade::api::format::r16g16b16a16_typeless: bit_depth = "rgba16_tuf"; break;
        case reshade::api::format::r16g16b16a16_unorm:
        case reshade::api::format::r16g16b16a16_snorm:
        case reshade::api::format::r16g16b16a16_uint:
        case reshade::api::format::r16g16b16a16_sint:
        case reshade::api::format::r16g16b16a16_float:    bit_depth = "rgba16_tuf"; break;

        // Skip 32-bit formats as requested
        default:
            // Check if it's a 32-bit format and skip it
            if (static_cast<int>(desc.texture.format) >= static_cast<int>(reshade::api::format::r32_float)
                && static_cast<int>(desc.texture.format) <= static_cast<int>(reshade::api::format::r32g32b32a32_sint)) {
                return;  // Skip 32-bit formats
            }
            break;
    }

    // Log resource creation if enabled
    if (g_log_resource_creation != 0) {
        // TODO: add (resource_info->upgraded)
        std::string original_format = "Unknown(" + std::to_string(static_cast<int>(desc.texture.format)) + ")";
        if (resource_info->upgraded) {
            // Get the original format from the fallback description
            const auto& original_desc = resource_info->fallback_desc;
            switch (original_desc.texture.format) {
                // R8G8B8A8 formats
                case reshade::api::format::r8g8b8a8_typeless:   original_format = "r8g8b8a8_typeless"; break;
                case reshade::api::format::r8g8b8a8_unorm:      original_format = "r8g8b8a8_unorm"; break;
                case reshade::api::format::r8g8b8a8_snorm:      original_format = "r8g8b8a8_snorm"; break;
                case reshade::api::format::r8g8b8a8_uint:       original_format = "r8g8b8a8_uint"; break;
                case reshade::api::format::r8g8b8a8_sint:       original_format = "r8g8b8a8_sint"; break;
                case reshade::api::format::r8g8b8a8_unorm_srgb: original_format = "r8g8b8a8_unorm_srgb"; break;

                // B8G8R8A8 formats
                case reshade::api::format::b8g8r8a8_typeless:   original_format = "b8g8r8a8_typeless"; break;
                case reshade::api::format::b8g8r8a8_unorm:      original_format = "b8g8r8a8_unorm"; break;
                case reshade::api::format::b8g8r8a8_unorm_srgb: original_format = "b8g8r8a8_unorm_srgb"; break;

                // B8G8R8X8 formats
                case reshade::api::format::b8g8r8x8_typeless:   original_format = "b8g8r8x8_typeless"; break;
                case reshade::api::format::b8g8r8x8_unorm:      original_format = "b8g8r8x8_unorm"; break;
                case reshade::api::format::b8g8r8x8_unorm_srgb: original_format = "b8g8r8x8_unorm_srgb"; break;

                // R8G8B8 formats
                case reshade::api::format::r8g8b8_unorm: original_format = "r8g8b8_unorm"; break;
                case reshade::api::format::r8g8b8_snorm: original_format = "r8g8b8_snorm"; break;
                case reshade::api::format::r8g8b8_uint:  original_format = "r8g8b8_uint"; break;
                case reshade::api::format::r8g8b8_sint:  original_format = "r8g8b8_sint"; break;

                // R8G8 formats
                case reshade::api::format::r8g8_unorm: original_format = "r8g8_unorm"; break;
                case reshade::api::format::r8g8_snorm: original_format = "r8g8_snorm"; break;
                case reshade::api::format::r8g8_uint:  original_format = "r8g8_uint"; break;
                case reshade::api::format::r8g8_sint:  original_format = "r8g8_sint"; break;

                // R8 formats
                case reshade::api::format::r8_unorm: original_format = "r8_unorm"; break;
                case reshade::api::format::r8_snorm: original_format = "r8_snorm"; break;
                case reshade::api::format::r8_uint:  original_format = "r8_uint"; break;
                case reshade::api::format::r8_sint:  original_format = "r8_sint"; break;

                // 10/11-bit formats
                case reshade::api::format::r10g10b10a2_typeless: original_format = "r10g10b10a2_typeless"; break;
                case reshade::api::format::r10g10b10a2_unorm:    original_format = "r10g10b10a2_unorm"; break;
                case reshade::api::format::r10g10b10a2_uint:     original_format = "r10g10b10a2_uint"; break;
                case reshade::api::format::b10g10r10a2_unorm:    original_format = "b10g10r10a2_unorm"; break;
                case reshade::api::format::r11g11b10_float:      original_format = "r11g11b10_float"; break;

                // 16-bit formats
                case reshade::api::format::r16_typeless: original_format = "r16_typeless"; break;
                case reshade::api::format::r16_unorm:    original_format = "r16_unorm"; break;
                case reshade::api::format::r16_snorm:    original_format = "r16_snorm"; break;
                case reshade::api::format::r16_uint:     original_format = "r16_uint"; break;
                case reshade::api::format::r16_sint:     original_format = "r16_sint"; break;
                case reshade::api::format::r16_float:    original_format = "r16_float"; break;

                case reshade::api::format::r16g16_typeless: original_format = "r16g16_typeless"; break;
                case reshade::api::format::r16g16_unorm:    original_format = "r16g16_unorm"; break;
                case reshade::api::format::r16g16_snorm:    original_format = "r16g16_snorm"; break;
                case reshade::api::format::r16g16_uint:     original_format = "r16g16_uint"; break;
                case reshade::api::format::r16g16_sint:     original_format = "r16g16_sint"; break;
                case reshade::api::format::r16g16_float:    original_format = "r16g16_float"; break;

                case reshade::api::format::r16g16b16_typeless: original_format = "r16g16b16_typeless"; break;
                case reshade::api::format::r16g16b16_unorm:    original_format = "r16g16b16_unorm"; break;
                case reshade::api::format::r16g16b16_snorm:    original_format = "r16g16b16_snorm"; break;
                case reshade::api::format::r16g16b16_uint:     original_format = "r16g16b16_uint"; break;
                case reshade::api::format::r16g16b16_sint:     original_format = "r16g16b16_sint"; break;
                case reshade::api::format::r16g16b16_float:    original_format = "r16g16b16_float"; break;

                case reshade::api::format::r16g16b16a16_typeless: original_format = "r16g16b16a16_typeless"; break;
                case reshade::api::format::r16g16b16a16_unorm:    original_format = "r16g16b16a16_unorm"; break;
                case reshade::api::format::r16g16b16a16_snorm:    original_format = "r16g16b16a16_snorm"; break;
                case reshade::api::format::r16g16b16a16_uint:     original_format = "r16g16b16a16_uint"; break;
                case reshade::api::format::r16g16b16a16_sint:     original_format = "r16g16b16a16_sint"; break;
                case reshade::api::format::r16g16b16a16_float:    original_format = "r16g16b16a16_float"; break;

                // Depth formats
                case reshade::api::format::d16_unorm:         original_format = "d16_unorm"; break;
                case reshade::api::format::d24_unorm_s8_uint: original_format = "d24_unorm_s8_uint"; break;
                case reshade::api::format::d32_float:         original_format = "d32_float"; break;
                case reshade::api::format::d32_float_s8_uint: original_format = "d32_float_s8_uint"; break;

                // Compressed formats
                case reshade::api::format::bc1_unorm:      original_format = "bc1_unorm"; break;
                case reshade::api::format::bc1_unorm_srgb: original_format = "bc1_unorm_srgb"; break;
                case reshade::api::format::bc2_unorm:      original_format = "bc2_unorm"; break;
                case reshade::api::format::bc2_unorm_srgb: original_format = "bc2_unorm_srgb"; break;
                case reshade::api::format::bc3_unorm:      original_format = "bc3_unorm"; break;
                case reshade::api::format::bc3_unorm_srgb: original_format = "bc3_unorm_srgb"; break;
                case reshade::api::format::bc4_unorm:      original_format = "bc4_unorm"; break;
                case reshade::api::format::bc4_snorm:      original_format = "bc4_snorm"; break;
                case reshade::api::format::bc5_unorm:      original_format = "bc5_unorm"; break;
                case reshade::api::format::bc5_snorm:      original_format = "bc5_snorm"; break;
                case reshade::api::format::bc7_unorm:      original_format = "bc7_unorm"; break;
                case reshade::api::format::bc7_unorm_srgb: original_format = "bc7_unorm_srgb"; break;

                default:
                    original_format = "Unknown(" + std::to_string(static_cast<int>(original_desc.texture.format)) + ")";
                    break;
            }
        }

        std::string final_format = "Unknown";
        switch (desc.texture.format) {
            // R8G8B8A8 formats
            case reshade::api::format::r8g8b8a8_typeless:   final_format = "r8g8b8a8_typeless"; break;
            case reshade::api::format::r8g8b8a8_unorm:      final_format = "r8g8b8a8_unorm"; break;
            case reshade::api::format::r8g8b8a8_snorm:      final_format = "r8g8b8a8_snorm"; break;
            case reshade::api::format::r8g8b8a8_uint:       final_format = "r8g8b8a8_uint"; break;
            case reshade::api::format::r8g8b8a8_sint:       final_format = "r8g8b8a8_sint"; break;
            case reshade::api::format::r8g8b8a8_unorm_srgb: final_format = "r8g8b8a8_unorm_srgb"; break;

            // B8G8R8A8 formats
            case reshade::api::format::b8g8r8a8_typeless:   final_format = "b8g8r8a8_typeless"; break;
            case reshade::api::format::b8g8r8a8_unorm:      final_format = "b8g8r8a8_unorm"; break;
            case reshade::api::format::b8g8r8a8_unorm_srgb: final_format = "b8g8r8a8_unorm_srgb"; break;

            // B8G8R8X8 formats
            case reshade::api::format::b8g8r8x8_typeless:   final_format = "b8g8r8x8_typeless"; break;
            case reshade::api::format::b8g8r8x8_unorm:      final_format = "b8g8r8x8_unorm"; break;
            case reshade::api::format::b8g8r8x8_unorm_srgb: final_format = "b8g8r8x8_unorm_srgb"; break;

            // R8G8B8 formats
            case reshade::api::format::r8g8b8_unorm: final_format = "r8g8b8_unorm"; break;
            case reshade::api::format::r8g8b8_snorm: final_format = "r8g8b8_snorm"; break;
            case reshade::api::format::r8g8b8_uint:  final_format = "r8g8b8_uint"; break;
            case reshade::api::format::r8g8b8_sint:  final_format = "r8g8b8_sint"; break;

            // R8G8 formats
            case reshade::api::format::r8g8_unorm: final_format = "r8g8_unorm"; break;
            case reshade::api::format::r8g8_snorm: final_format = "r8g8_snorm"; break;
            case reshade::api::format::r8g8_uint:  final_format = "r8g8_uint"; break;
            case reshade::api::format::r8g8_sint:  final_format = "r8g8_sint"; break;

            // R8 formats
            case reshade::api::format::r8_unorm: final_format = "r8_unorm"; break;
            case reshade::api::format::r8_snorm: final_format = "r8_snorm"; break;
            case reshade::api::format::r8_uint:  final_format = "r8_uint"; break;
            case reshade::api::format::r8_sint:  final_format = "r8_sint"; break;

            // 10/11-bit formats
            case reshade::api::format::r10g10b10a2_typeless: final_format = "r10g10b10a2_typeless"; break;
            case reshade::api::format::r10g10b10a2_unorm:    final_format = "r10g10b10a2_unorm"; break;
            case reshade::api::format::r10g10b10a2_uint:     final_format = "r10g10b10a2_uint"; break;
            case reshade::api::format::b10g10r10a2_unorm:    final_format = "b10g10r10a2_unorm"; break;
            case reshade::api::format::r11g11b10_float:      final_format = "r11g11b10_float"; break;

            // 16-bit formats
            case reshade::api::format::r16_typeless: final_format = "r16_typeless"; break;
            case reshade::api::format::r16_unorm:    final_format = "r16_unorm"; break;
            case reshade::api::format::r16_snorm:    final_format = "r16_snorm"; break;
            case reshade::api::format::r16_uint:     final_format = "r16_uint"; break;
            case reshade::api::format::r16_sint:     final_format = "r16_sint"; break;
            case reshade::api::format::r16_float:    final_format = "r16_float"; break;

            case reshade::api::format::r16g16_typeless: final_format = "r16g16_typeless"; break;
            case reshade::api::format::r16g16_unorm:    final_format = "r16g16_unorm"; break;
            case reshade::api::format::r16g16_snorm:    final_format = "r16g16_snorm"; break;
            case reshade::api::format::r16g16_uint:     final_format = "r16g16_uint"; break;
            case reshade::api::format::r16g16_sint:     final_format = "r16g16_sint"; break;
            case reshade::api::format::r16g16_float:    final_format = "r16g16_float"; break;

            case reshade::api::format::r16g16b16_typeless: final_format = "r16g16b16_typeless"; break;
            case reshade::api::format::r16g16b16_unorm:    final_format = "r16g16b16_unorm"; break;
            case reshade::api::format::r16g16b16_snorm:    final_format = "r16g16b16_snorm"; break;
            case reshade::api::format::r16g16b16_uint:     final_format = "r16g16b16_uint"; break;
            case reshade::api::format::r16g16b16_sint:     final_format = "r16g16b16_sint"; break;
            case reshade::api::format::r16g16b16_float:    final_format = "r16g16b16_float"; break;

            case reshade::api::format::r16g16b16a16_typeless: final_format = "r16g16b16a16_typeless"; break;
            case reshade::api::format::r16g16b16a16_unorm:    final_format = "r16g16b16a16_unorm"; break;
            case reshade::api::format::r16g16b16a16_snorm:    final_format = "r16g16b16a16_snorm"; break;
            case reshade::api::format::r16g16b16a16_uint:     final_format = "r16g16b16a16_uint"; break;
            case reshade::api::format::r16g16b16a16_sint:     final_format = "r16g16b16a16_sint"; break;
            case reshade::api::format::r16g16b16a16_float:    final_format = "r16g16b16a16_float"; break;

            // Depth formats
            case reshade::api::format::d16_unorm:         final_format = "d16_unorm"; break;
            case reshade::api::format::d24_unorm_s8_uint: final_format = "d24_unorm_s8_uint"; break;
            case reshade::api::format::d32_float:         final_format = "d32_float"; break;
            case reshade::api::format::d32_float_s8_uint: final_format = "d32_float_s8_uint"; break;

            // Compressed formats
            case reshade::api::format::bc1_unorm:      final_format = "bc1_unorm"; break;
            case reshade::api::format::bc1_unorm_srgb: final_format = "bc1_unorm_srgb"; break;
            case reshade::api::format::bc2_unorm:      final_format = "bc2_unorm"; break;
            case reshade::api::format::bc2_unorm_srgb: final_format = "bc2_unorm_srgb"; break;
            case reshade::api::format::bc3_unorm:      final_format = "bc3_unorm"; break;
            case reshade::api::format::bc3_unorm_srgb: final_format = "bc3_unorm_srgb"; break;
            case reshade::api::format::bc4_unorm:      final_format = "bc4_unorm"; break;
            case reshade::api::format::bc4_snorm:      final_format = "bc4_snorm"; break;
            case reshade::api::format::bc5_unorm:      final_format = "bc5_unorm"; break;
            case reshade::api::format::bc5_snorm:      final_format = "bc5_snorm"; break;
            case reshade::api::format::bc7_unorm:      final_format = "bc7_unorm"; break;
            case reshade::api::format::bc7_unorm_srgb: final_format = "bc7_unorm_srgb"; break;

            default: final_format = "Unknown(" + std::to_string(static_cast<int>(desc.texture.format)) + ")"; break;
        }

        std::string upgrade_status = resource_info->upgraded ? "YES" : "NO";
        std::string log_message = "Resource: " + std::to_string(resolution.first) + "x"
                                  + std::to_string(resolution.second) + " | Before: " + original_format
                                  + " | After: " + final_format + " | Upgraded: " + upgrade_status;
        reshade::log::message(reshade::log::level::info, log_message.c_str());
    }

    // Update statistics
    {
        std::lock_guard<std::mutex> lock(g_resolution_bitdepth_stats.mutex);

        // Always increment the "after upgrade" count (this represents the final state)
        g_resolution_bitdepth_stats.after_upgrade[resolution][bit_depth]++;

        // If this resource was upgraded, track the original format and upgrade counts
        if (resource_info->upgraded) {
            // Get the original format from the fallback description
            const auto& original_desc = resource_info->fallback_desc;
            std::string original_bit_depth = "Unknown";

            // Determine original format category from the fallback format
            switch (original_desc.texture.format) {
                // R8G8B8A8 formats
                case reshade::api::format::r8g8b8a8_typeless:   original_bit_depth = "rgba8_t"; break;
                case reshade::api::format::r8g8b8a8_unorm:      original_bit_depth = "rgba8_u"; break;
                case reshade::api::format::r8g8b8a8_snorm:      original_bit_depth = "rgba8_s"; break;
                case reshade::api::format::r8g8b8a8_unorm_srgb: original_bit_depth = "rgba8_srgb"; break;

                // B8G8R8A8 formats
                case reshade::api::format::b8g8r8a8_typeless:   original_bit_depth = "bgra8_t"; break;
                case reshade::api::format::b8g8r8a8_unorm:      original_bit_depth = "bgra8_u"; break;
                case reshade::api::format::b8g8r8a8_unorm_srgb: original_bit_depth = "bgra8_srgb"; break;

                // 10/11-bit formats
                case reshade::api::format::r10g10b10a2_typeless: original_bit_depth = "rgb10a2_t"; break;
                case reshade::api::format::r10g10b10a2_unorm:    original_bit_depth = "rgb10a2_u"; break;
                case reshade::api::format::b10g10r10a2_unorm:    original_bit_depth = "bgr10a2_u"; break;
                case reshade::api::format::r11g11b10_float:      original_bit_depth = "r11g11b10_f"; break;

                // 16-bit formats
                case reshade::api::format::r16g16b16a16_typeless:
                case reshade::api::format::r16g16b16a16_unorm:
                case reshade::api::format::r16g16b16a16_snorm:
                case reshade::api::format::r16g16b16a16_uint:
                case reshade::api::format::r16g16b16a16_sint:
                case reshade::api::format::r16g16b16a16_float:    original_bit_depth = "rgba16_tuf"; break;

                // Skip 32-bit formats as requested
                default:
                    // Check if it's a 32-bit format and skip it
                    if (static_cast<int>(original_desc.texture.format)
                            >= static_cast<int>(reshade::api::format::r32_float)
                        && static_cast<int>(original_desc.texture.format)
                               <= static_cast<int>(reshade::api::format::r32g32b32a32_sint)) {
                        return;  // Skip 32-bit formats
                    }
                    break;
            }

            // Track the original format before upgrade
            g_resolution_bitdepth_stats.before_upgrade[resolution][original_bit_depth]++;

            // Track that this original format was upgraded
            g_resolution_bitdepth_stats.upgraded[resolution][original_bit_depth]++;
        }
    }
}

// Check if shader data contains binary representation of the lutbuilder float
bool ConstainsFloat(const std::span<const uint8_t>& shader_data, float target_float) {
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

std::optional<std::string> DumpShaderPrefix(const std::span<const uint8_t>& shader_data) {
    if ((g_dump_shaders != 0 || g_autodump_lutbuilders != 0) && ConstainsFloat(shader_data, 0.070841603f)) {
        return "lutbuilder_";
    }
    if ((g_dump_shaders != 0 || g_autodump_lutbuilders != 0) && ConstainsFloat(shader_data, 0.947867334)) {
        return "uber_srgb_";
    }
    if (g_dump_shaders != 0) {
        return "shader_";
    }

    return std::nullopt;
}

// Scan existing dumped shaders on application start for performance
void ScanExistingDumpedShaders() {
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
        if (extension != ".cso" && extension != ".hlsl" && extension != ".ps_5_0" && extension != ".vs_5_0"
            && extension != ".cs_5_0" && extension != ".gs_5_0")
            continue;

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

void OnInitPipeline(reshade::api::device* device, reshade::api::pipeline_layout layout, uint32_t subobject_count,
                    const reshade::api::pipeline_subobject* subobjects, reshade::api::pipeline pipeline) {
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
            auto shader_hash =
                compute_crc32(reinterpret_cast<const uint8_t*>(shader_desc->code), shader_desc->code_size);

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

                renodx::utils::shader::dump::DumpShader(shader_hash, shader_data, subobject.type, prefix);

            } catch (...) {
                std::stringstream s;
                s << "utils::shader::dump(Failed to dump pipeline shader: ";
                s << PRINT_CRC32(shader_hash);
                s << ")";
                reshade::log::message(reshade::log::level::warning, s.str().c_str());
            }
        }
    }

    // todo: replace with `There's draw, draw indirect, dispatch and drawindirector dispatch`
    // Dump any pending shaders (ignoring custom shaders and already dumped shaders)
    renodx::utils::shader::dump::DumpAllPending();  // custom_shaders, g_dumped_shaders, DumpShaderPrefix);
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
            .labels = {"Vanilla", "None", "ACES", "RenoDRT", "DICE", "Frostbite", "BabyReinhard", "ACES (v2)"},
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
            .tooltip = "Enables tone mapping pass to run after SwapChainPass for additional processing. Not "
                       "recommended as this will apply tone mapping to UI elements. Useful when FakeHDR is enabled.",
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
            .binding = &shader_injection.tone_map_gamma,  // Make sure this field exists in ShaderInjectData
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
            .labels =
                {
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
            .parse =
                [](float value) {
                    if (value == 0) return shader_injection.gamma_correction + 1.f;
                    return value - 1.f;
                },
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
            .parse =
                [](float value) {
                    if (value == 0) return shader_injection.intermediate_encoding;
                    return value - 1.f;
                },
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
            .default_value = hbr_custom_settings::get_default_value(
                "EffectSplitX", 1.f),  // Default to half of 1920 width, adjust as needed
            .label = "Effect Split X",
            .section = "Display Output",
            .tooltip = "Only apply effects to the left of this screen X coordinate.",
            .min = -1.f,
            .max = 1.f,  // Or your max screen width
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
            .key = "IsUpsideDown",
            .binding = &shader_injection.is_upside_down,
            .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
            .default_value = hbr_custom_settings::get_default_value("IsUpsideDown", 0.f),
            .label = "Flip Image Vertically",
            .section = "Display Output",
            .tooltip = "Flips the image vertically (useful for games that render upside down)",
            .labels = {"Off", "On"},
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
            .tooltip =
                "Perceptual boost strength for 2D character scenes (0.0 = disabled, 1.0 = normal, 10.0 = maximum)",
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
            .tooltip =
                "Perceptual boost strength for 2D background scenes (0.0 = disabled, 1.0 = normal, 10.0 = maximum)",
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
            .max = 10.f,
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
            .max = 10.f,
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
            .max = 10.f,
            .format = "%.2f",
            .is_visible = []() { return current_settings_mode >= 3; },
        },
        new renodx::utils::settings::Setting{
            .key = "AutomaticShaderDumping",
            .binding = &g_dump_shaders,
            .value_type = renodx::utils::settings::SettingValueType::INTEGER,
            .default_value = 0.f,
            .label = "Automatic Shader Dumping",
            .section = "Debug",
            .tooltip = "Automatically dump shaders into renodx-dump folder.",
            .labels =
                {
                    "Off",
                    "On",
                },
            .is_global = true,
            .is_visible = []() { return current_settings_mode >= 3; },
        },
        new renodx::utils::settings::Setting{
            .key = "AutodumpLutbuilders",
            .binding = &g_autodump_lutbuilders,
            .value_type = renodx::utils::settings::SettingValueType::INTEGER,
            .default_value = 0.f,
            .label = "Autodump Lutbuilders",
            .section = "Debug",
            .tooltip = "Automatically dump only lutbuilder shaders. Works independently of general shader dumping.",
            .labels =
                {
                    "Off",
                    "On",
                },
            .is_global = true,
            .is_visible = []() { return current_settings_mode >= 3; },
        },
        new renodx::utils::settings::Setting{
            .key = "LogResourceCreation",
            .binding = &g_log_resource_creation,
            .default_value = 0.f,
            .label = "Log Resource Creation",
            .section = "Debug",
            .tooltip =
                "Enable detailed logging of resource creation (resolution, before/after formats, upgrade status)",
            .min = 0.f,
            .max = 1.f,
            .format = "%.0f",
            .is_visible = []() { return current_settings_mode >= 3; },
        },
        new renodx::utils::settings::Setting{
            .key = "ResolutionBitDepthTable",
            .binding = nullptr,
            .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
            .default_value = 0.f,
            .label = "Resolution & Bit Depth Statistics",
            .section = "Debug",
            .tooltip = "Shows statistics of created resources by resolution and bit depth",
            .on_draw =
                []() {
                    std::lock_guard<std::mutex> lock(g_resolution_bitdepth_stats.mutex);

                    if (g_resolution_bitdepth_stats.after_upgrade.empty()) {
                        ImGui::Text("No resources tracked yet.");
                        return true;
                    }

                    // Column order based on upgrade targets (shorter names)
                    std::vector<std::string> column_order = {"rgba8_t",   "bgra8_t",    "rgba8_u",     "bgra8_u",
                                                             "rgba8_s",   "rgba8_srgb", "bgra8_srgb",  "rgb10a2_t",
                                                             "rgb10a2_u", "bgr10a2_u",  "r11g11b10_f", "rgba16_tuf"};

                    // Create sorted list of resolutions (sorted by width, then height - highest first)
                    std::vector<std::pair<int, int>> sorted_resolutions;
                    sorted_resolutions.reserve(g_resolution_bitdepth_stats.after_upgrade.size());
                    for (const auto& [resolution, bit_depth_map] : g_resolution_bitdepth_stats.after_upgrade) {
                        sorted_resolutions.emplace_back(resolution);
                    }
                    std::sort(sorted_resolutions.begin(), sorted_resolutions.end(),
                              std::greater<std::pair<int, int>>());

                    // Create table
                    int column_count = static_cast<int>(column_order.size()) + 2;  // +2 for resolution and aspect ratio
                    if (ImGui::BeginTable(
                            "ResolutionBitDepthTable", column_count,
                            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
                        // Header row
                        ImGui::TableSetupColumn("Resolution", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                        ImGui::TableSetupColumn("Aspect", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                        for (const auto& bit_depth : column_order) {
                            ImGui::TableSetupColumn(bit_depth.c_str(), ImGuiTableColumnFlags_WidthFixed, 100.0f);
                        }
                        ImGui::TableHeadersRow();

                        // Data rows (now in sorted order)
                        for (const auto& resolution : sorted_resolutions) {
                            ImGui::TableNextRow();

                            // Resolution column
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("%dx%d", resolution.first, resolution.second);

                            // Aspect ratio column
                            ImGui::TableSetColumnIndex(1);
                            float aspect_ratio =
                                static_cast<float>(resolution.first) / static_cast<float>(resolution.second);
                            ImGui::Text("%.2f", aspect_ratio);

                            // Get the bit depth maps for this resolution
                            auto after_it = g_resolution_bitdepth_stats.after_upgrade.find(resolution);
                            auto before_it = g_resolution_bitdepth_stats.before_upgrade.find(resolution);
                            auto upgraded_it = g_resolution_bitdepth_stats.upgraded.find(resolution);

                            if (after_it != g_resolution_bitdepth_stats.after_upgrade.end()) {
                                const auto& after_map = after_it->second;
                                const auto& before_map = (before_it != g_resolution_bitdepth_stats.before_upgrade.end())
                                                             ? before_it->second
                                                             : std::map<std::string, int>{};
                                const auto& upgraded_map = (upgraded_it != g_resolution_bitdepth_stats.upgraded.end())
                                                               ? upgraded_it->second
                                                               : std::map<std::string, int>{};

                                // Bit depth columns (using fixed order) - show before/upgraded/after triplets
                                int col_index = 2;  // Start at 2 because we have Resolution (0) and Aspect (1) columns
                                for (const auto& bit_depth : column_order) {
                                    ImGui::TableSetColumnIndex(col_index);

                                    auto after_bit_it = after_map.find(bit_depth);
                                    auto before_bit_it = before_map.find(bit_depth);
                                    auto upgraded_bit_it = upgraded_map.find(bit_depth);

                                    int after_count = (after_bit_it != after_map.end()) ? after_bit_it->second : 0;
                                    int before_count = (before_bit_it != before_map.end()) ? before_bit_it->second : 0;
                                    int upgraded_count =
                                        (upgraded_bit_it != upgraded_map.end()) ? upgraded_bit_it->second : 0;

                                    if (after_count > 0 || before_count > 0 || upgraded_count > 0) {
                                        ImGui::Text("%d/%d/%d", before_count, upgraded_count, after_count);
                                    } else {
                                        ImGui::Text("-");
                                    }
                                    col_index++;
                                }
                            }
                        }

                        ImGui::EndTable();
                    }

                    return true;
                },
            .is_visible = []() { return current_settings_mode >= 3; }},
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
        .labels =
            {
                "Off",
                "On",
            },
        .is_global = true,
        .is_visible = []() { return current_settings_mode >= 3; },
    };
    reshade::get_config_value(nullptr, renodx::utils::settings::global_name.c_str(), "Upgrade_SwapChainCompatibility",
                              swapchain_setting->value_as_int);
    renodx::mods::swapchain::swapchain_proxy_compatibility_mode = swapchain_setting->GetValue() != 0;
    settings.push_back(swapchain_setting);
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "DisableSwapChainUpgrade",
        .value_type = renodx::utils::settings::SettingValueType::BOOLEAN,
        .default_value = hbr_custom_settings::get_default_value("DisableSwapChainUpgrade", 0.f),
        .label = "Disable Swap Chain Upgrade",
        .section = "Display Output",
        .tooltip = "When enabled, forces resize buffer mode for swap chain handling",
        .labels = {"Off", "On"},
        .on_change_value =
            [](float previous, float current) {
                if (current == 1.f) {
                    renodx::mods::swapchain::use_resize_buffer_on_demand = true;
                    renodx::mods::swapchain::use_resize_buffer = true;
                }
            },
        .is_global = true,
        .is_visible = []() { return current_settings_mode >= 2; },
    });
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


inline bool OnCopyTextureRegionDummy(
    reshade::api::command_list* cmd_list,
    reshade::api::resource source,
    uint32_t source_subresource,
    const reshade::api::subresource_box* source_box,
    reshade::api::resource dest,
    uint32_t dest_subresource,
    const reshade::api::subresource_box* dest_box,
    reshade::api::filter_mode filter) {
    return true;
}

bool initialized = false;
void InitializeSettings() {
    settings.clear();
    settings.push_back(new renodx::utils::settings::Setting{
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .label = std::string("Version: ") + RENODX_VERSION,
        .section = "About",
        .tooltip = std::string("RenoDX Universal Template Version ") + RENODX_VERSION,
        .on_draw = []() {
            ImGui::Text("Version: %s", RENODX_VERSION);
            ImGui::Separator();
            ImGui::Text("Support RenoDX development:");
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 120, 220, 255)); // blue
            if (ImGui::Selectable("ko-fi.com/pmnox", false, ImGuiSelectableFlags_SpanAllColumns)) {
                // Open link in browser (Windows only)
                #ifdef _WIN32
                ShellExecuteA(nullptr, "open", "https://ko-fi.com/pmnox", nullptr, nullptr, SW_SHOWNORMAL);
                #endif
            }
            ImGui::PopStyleColor();
            ImGui::SetItemTooltip("Click to open Ko-fi page in your browser");
            return true;
        },
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

    // Load settings for moved Debug section settings
    {
        auto* setting =
            new renodx::utils::settings::Setting{.key = "AutomaticShaderDumping",
                                                 .binding = &g_dump_shaders,
                                                 .value_type = renodx::utils::settings::SettingValueType::INTEGER,
                                                 .default_value = 0.f,
                                                 .label = "Automatic Shader Dumping",
                                                 .section = "Debug",
                                                 .tooltip = "Automatically dump shaders into renodx-dump folder.",
                                                 .labels =
                                                     {
                                                         "Off",
                                                         "On",
                                                     },
                                                 .is_global = true,
                                                 .is_visible = []() { return current_settings_mode >= 3; }};
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
            .labels =
                {
                    "Off",
                    "On",
                },
            .is_global = true,
            .is_visible = []() { return current_settings_mode >= 3; }};
        renodx::utils::settings::LoadSetting(renodx::utils::settings::global_name, setting);
        settings.push_back(setting);
        g_autodump_lutbuilders = setting->GetValue();
    }

    add_section(GenerateHighlightSaturationRestorationSection());
    add_section(hbr_custom_settings::GenerateCustomGameSettingsSection(
        shader_injection, current_settings_mode));  // Use external function
    AddAdvancedSettings();
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "UpgradeResourceViewCloning",
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = hbr_custom_settings::get_default_value("UpgradeResourceViewCloning", 0.f),
        .label = "Resource View Cloning (Global)",
        .section = "Resource Upgrades",
        .tooltip = "If enabled, use resource view cloning for all resource upgrades. (May fix or cause issues "
                   "depending on the game)",
        .labels = {"Disabled", "Enabled"},
        .is_global = true,
        .is_visible = []() { return current_settings_mode >= 3; },
    });
}

}  // namespace

extern "C" __declspec(dllexport) constexpr const char* NAME = "RenoDX";
extern "C" __declspec(dllexport) constexpr const char* DESCRIPTION = "RenoDX (Generic)";

BOOL APIENTRY DllMain(HMODULE h_module, DWORD fdw_reason, LPVOID lpv_reserved) {
    auto process_path = renodx::utils::platform::GetCurrentProcessPath();
    auto filename = process_path.filename().string();
    switch (fdw_reason) {
        case DLL_PROCESS_ATTACH:
            if (!reshade::register_addon(h_module)) return FALSE;

            if (!initialized) {
                // Print exe name and title
                auto product_name = renodx::utils::platform::GetProductName(process_path);
                bool is_no_creeps_were_harmed = filename == "No Creeps Were Harmed TD.exe";
                reshade::log::message(reshade::log::level::info,
                                      std::format("RenoDX Universal loaded for {} ({}) {}.", filename, product_name,
                                                  is_no_creeps_were_harmed ? "No Creeps Were Harmed TD" : "")
                                          .c_str());

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

                // #ifdef USE_CB50
                renodx::mods::shader::constant_buffer_offset = hbr_custom_settings::get_constant_buffer_offset();
                // 50 * 4;
                //   #endif

                renodx::mods::swapchain::use_resource_cloning = true;
                renodx::mods::swapchain::use_device_proxy = hbr_custom_settings::get_use_device_proxy();
                renodx::utils::random::binds.push_back(&shader_injection.random_seed);

                // Register callback for resource creation and upgrade tracking
                renodx::utils::resource::store->on_init_resource_info_callbacks.emplace_back(&OnResourceCreated);

                g_upgrade_copy_destinations = hbr_custom_settings::get_upgrade_copy_destinations();


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

                if (filename == "TheSwapper.exe") {
                    renodx::mods::swapchain::prevent_full_screen = false;
                    renodx::mods::swapchain::force_screen_tearing = false;
                    renodx::mods::swapchain::set_color_space = false;
                    renodx::mods::swapchain::use_device_proxy = true;
                } else if (filename == "Darkest Dungeon II.exe") {
                    custom_shaders = {
                        // shaders not included
                        UpgradeRTVShader(0x4D57C441), // CameraMotionBlur
                        UpgradeRTVShader(0x0EC60300), // Bloom
                        UpgradeRTVShader(0xB4BAC5C6), // Bloom2


                        // darkest_dungeons2/others
                        UpgradeRTVReplaceShader(0x2805E1FA),
                        UpgradeRTVReplaceShader(0xA8B1BA98),
                        UpgradeRTVReplaceShader(0xD02B91C8),
                        UpgradeRTVReplaceShader(0xD00B5B47),
                        UpgradeRTVReplaceShader(0xD8341E94), //lut1
                        UpgradeRTVReplaceShader(0x64031CB8), //lut2

                        // darkest_dungeons2/lutbuilders
                        UpgradeRTVReplaceShader(0x20D6EA4D),// lutbuilder

                        // darkest_dungeons2/ubers
                        UpgradeRTVReplaceShader(0xC2976820), // uber1
                        UpgradeRTVReplaceShader(0x721D4F40), // uber2
                        UpgradeRTVReplaceShader(0x6C71F0B5), // uber3
                        UpgradeRTVReplaceShader(0x450C7E5A), // uber4
                        UpgradeRTVReplaceShader(0xC2976820), // uber5
                        UpgradeRTVReplaceShader(0x65A1E707), // uber6
                        UpgradeRTVReplaceShader(0x262EEB5C), // uber7
                        UpgradeRTVReplaceShader(0xA991AB1F), // uber8
                        UpgradeRTVReplaceShader(0xC7F7285F), // uber9
                    };
                    auto value = UPGRADE_TYPE_ANY;
                    g_upgrade_copy_destinations = 1.f;
                    renodx::mods::swapchain::swap_chain_upgrade_targets.push_back({
                        .old_format = reshade::api::format::r8g8b8a8_typeless,
                        .new_format = reshade::api::format::r16g16b16a16_float,
                        .ignore_size = (value == UPGRADE_TYPE_ANY),
                        .use_resource_view_cloning = true,
                        .use_resource_view_hot_swap = true,
                        .aspect_ratio =
                            static_cast<float>((value == UPGRADE_TYPE_OUTPUT_RATIO)
                                                   ? renodx::mods::swapchain::SwapChainUpgradeTarget::BACK_BUFFER
                                                   : renodx::mods::swapchain::SwapChainUpgradeTarget::ANY),
                        .usage_include =
                            reshade::api::resource_usage::render_target
                            | (g_upgrade_copy_destinations == 0.f ? reshade::api::resource_usage::undefined
                                                                  : reshade::api::resource_usage::copy_dest),
                    });
                    renodx::mods::swapchain::swap_chain_upgrade_targets.push_back({
                        .old_format = reshade::api::format::r11g11b10_float,
                        .new_format = reshade::api::format::r16g16b16a16_float,
                        .ignore_size = (value == UPGRADE_TYPE_ANY),
                        .use_resource_view_cloning = true,
                        .use_resource_view_hot_swap = true,
                        .aspect_ratio =
                            static_cast<float>((value == UPGRADE_TYPE_OUTPUT_RATIO)
                                                   ? renodx::mods::swapchain::SwapChainUpgradeTarget::BACK_BUFFER
                                                   : renodx::mods::swapchain::SwapChainUpgradeTarget::ANY),
                        .usage_include =
                            reshade::api::resource_usage::render_target
                            | (g_upgrade_copy_destinations == 0.f ? reshade::api::resource_usage::undefined
                                                                  : reshade::api::resource_usage::copy_dest),
                    });
                } else if (filename == "Warhammer3.exe") {
                    custom_shaders = {
                        UpgradeRTVReplaceShader(0x1EDCBE3A),
                      /// CreateUpgradeSRVReplaceShader(0xFE8E2C85, 0),
                        CustomShaderEntry(0xFE8E2C85),
                    };

                    auto value = UPGRADE_TYPE_OUTPUT_SIZE;
                    g_upgrade_copy_destinations = 1.f;
                    renodx::mods::swapchain::swap_chain_upgrade_targets.push_back({
                        .old_format = reshade::api::format::r8g8b8a8_unorm,
                        .new_format = reshade::api::format::r16g16b16a16_float,
                        .ignore_size = (value == UPGRADE_TYPE_ANY),
                        .use_resource_view_cloning = true,
                        .use_resource_view_hot_swap = true,
                        .aspect_ratio =
                            static_cast<float>((value == UPGRADE_TYPE_OUTPUT_RATIO)
                                                   ? renodx::mods::swapchain::SwapChainUpgradeTarget::BACK_BUFFER
                                                   : renodx::mods::swapchain::SwapChainUpgradeTarget::ANY),
                        .usage_include =
                            reshade::api::resource_usage::render_target
                            | (g_upgrade_copy_destinations == 0.f ? reshade::api::resource_usage::undefined
                                                                  : reshade::api::resource_usage::copy_dest),
                    });
                    /*
                    renodx::mods::swapchain::swap_chain_upgrade_targets.push_back({
                        .old_format = reshade::api::format::r8g8b8a8_unorm,
                        .new_format = reshade::api::format::r16g16b16a16_float,
                        .ignore_size = (value == UPGRADE_TYPE_ANY),
                        .use_resource_view_cloning = true,
                     //   .use_resource_view_hot_swap = true,
                        .aspect_ratio =
                            static_cast<float>((value == UPGRADE_TYPE_OUTPUT_RATIO)
                                                   ? renodx::mods::swapchain::SwapChainUpgradeTarget::BACK_BUFFER
                                                   : renodx::mods::swapchain::SwapChainUpgradeTarget::ANY),
                        .usage_include =
                            reshade::api::resource_usage::copy_dest
                    });
                    */
                } else if (filename == "Against the Storm.exe") {
                    custom_shaders = {
                        // shaders not included
                        UpgradeRTVShader(0x440EAE28), // Blit
                        UpgradeRTVShader(0x30356900), // Antialiasing

                        // lutbuilders
                        UpgradeRTVReplaceShader(0xED457D04),

                        // others
                        UpgradeRTVReplaceShader(0x48AAC250),
                        UpgradeRTVReplaceShader(0xC9D8FA56),

                        // ubers
                        UpgradeRTVReplaceShader(0x53F75ED5),
                        UpgradeRTVReplaceShader(0xCF4215DF),
                        UpgradeRTVReplaceShader(0x83BB5283),
                        UpgradeRTVReplaceShader(0x1A7177D2),
                    };
                    auto value = UPGRADE_TYPE_ANY;
                    g_upgrade_copy_destinations = 1.f;
                    renodx::mods::swapchain::swap_chain_upgrade_targets.push_back({
                        .old_format = reshade::api::format::r8g8b8a8_typeless,
                        .new_format = reshade::api::format::r16g16b16a16_float,
                        .ignore_size = (value == UPGRADE_TYPE_ANY),
                        .use_resource_view_cloning = true,
                        .use_resource_view_hot_swap = true,
                        .aspect_ratio =
                            static_cast<float>((value == UPGRADE_TYPE_OUTPUT_RATIO)
                                                   ? renodx::mods::swapchain::SwapChainUpgradeTarget::BACK_BUFFER
                                                   : renodx::mods::swapchain::SwapChainUpgradeTarget::ANY),
                        .usage_include =
                            reshade::api::resource_usage::render_target
                            | (g_upgrade_copy_destinations == 0.f ? reshade::api::resource_usage::undefined
                                                                  : reshade::api::resource_usage::copy_dest),
                    });
                    value = UPGRADE_TYPE_ANY;
                    g_upgrade_copy_destinations = 1.f;
                    renodx::mods::swapchain::swap_chain_upgrade_targets.push_back({
                        .old_format = reshade::api::format::r8g8b8a8_typeless,
                        .new_format = reshade::api::format::r16g16b16a16_float,
                        .ignore_size = (value == UPGRADE_TYPE_ANY),
                        .use_resource_view_cloning = true,
                        .aspect_ratio =
                            static_cast<float>((value == UPGRADE_TYPE_OUTPUT_RATIO)
                                                   ? renodx::mods::swapchain::SwapChainUpgradeTarget::BACK_BUFFER
                                                   : renodx::mods::swapchain::SwapChainUpgradeTarget::ANY),
                        .usage_include =reshade::api::resource_usage::copy_dest,
                    });
                } else if (filename == "Ixion.exe") {
                    renodx::mods::swapchain::swap_chain_proxy_shaders.clear();
                    custom_shaders = {
                        // shaders not included
                        UpgradeRTVShader(0x066C98CB),

                        // Lutbuilders
                        UpgradeRTVReplaceShader(0x6811A33B),  // lutbuilder3d - 3D LUT builder compute shader

                        // others
                        UpgradeRTVReplaceShader(0x91D61E3F),  // finalpost - final post-processing shader
                        UpgradeRTVReplaceShader(0x0F01C9E1),  // blit - blit operation shader
                        UpgradeRTVReplaceShader(0x54C575B8),  // bloom_p1 - bloom pass 1 compute shader
                   //     CustomShaderEntry(0x57AA0427),  // cs1 - compute shader 1
                    //    CustomShaderEntry(0xC3C09926),  // cs2 - compute shader 2
                    //    CustomShaderEntry(0x78CC9692),  // trees - trees rendering pixel shader
                     //   CustomShaderEntry(0xDACD56EB),  // trees2 - trees rendering variant 2 pixel shader
                    };

                    auto value = UPGRADE_TYPE_ANY;
                    g_upgrade_copy_destinations = 1.f;
                    renodx::mods::swapchain::swap_chain_upgrade_targets.push_back({
                        .old_format = reshade::api::format::r8g8b8a8_typeless,
                        .new_format = reshade::api::format::r16g16b16a16_float,
                        .ignore_size = (value == UPGRADE_TYPE_ANY),
                        .use_resource_view_cloning = true,
                        .use_resource_view_hot_swap = true,
                        .aspect_ratio =
                            static_cast<float>((value == UPGRADE_TYPE_OUTPUT_RATIO)
                                                   ? renodx::mods::swapchain::SwapChainUpgradeTarget::BACK_BUFFER
                                                   : renodx::mods::swapchain::SwapChainUpgradeTarget::ANY),
                        .usage_include =
                            reshade::api::resource_usage::render_target
                            | (g_upgrade_copy_destinations == 0.f ? reshade::api::resource_usage::undefined
                                                                  : reshade::api::resource_usage::copy_dest),
                    });
                }

                // add settings to disable d3d9 resource upgrade
                if (hbr_custom_settings::get_disable_d3d9_resource_upgrade()) {
                    renodx::mods::swapchain::ignored_device_apis = {
                        reshade::api::device_api::d3d9};  // needed to prevent crash
                }
                {
                    auto* setting = new renodx::utils::settings::Setting{
                        .key = "SwapChainForceBorderless",
                        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
                        .default_value = hbr_custom_settings::get_default_value("SwapChainForceBorderless", 0.f),
                        .label = "Force Borderless",
                        .section = "Display Output",
                        .tooltip = "Forces fullscreen to be borderless for proper HDR",
                        .labels =
                            {
                                "Disabled",
                                "Enabled",
                            },
                        .on_change_value =
                            [](float previous, float current) {
                                renodx::mods::swapchain::force_borderless = (current == 1.f);
                            },
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
                        .labels =
                            {
                                "Disabled",
                                "Enabled",
                            },
                        .on_change_value =
                            [](float previous, float current) {
                                renodx::mods::swapchain::prevent_full_screen = (current == 1.f);
                            },
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
                        .on_change_value =
                            [](float previous, float current) {
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
                        .labels =
                            {
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
                            .aspect_ratio =
                                static_cast<float>((value == UPGRADE_TYPE_OUTPUT_RATIO)
                                                       ? renodx::mods::swapchain::SwapChainUpgradeTarget::BACK_BUFFER
                                                       : renodx::mods::swapchain::SwapChainUpgradeTarget::ANY),
                            .usage_include =
                                reshade::api::resource_usage::render_target
                                | (g_upgrade_copy_destinations == 0.f ? reshade::api::resource_usage::undefined
                                                                      : reshade::api::resource_usage::copy_dest),
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
                        reshade::log::message(reshade::log::level::info,
                                              "Shader dumping and lutbuilder dumping enabled.");
                    } else if (g_dump_shaders != 0.f) {
                        reshade::log::message(reshade::log::level::info, "General shader dumping enabled.");
                    } else {
                        reshade::log::message(reshade::log::level::info, "Lutbuilder dumping enabled.");
                    }
                }
                reshade::register_event<reshade::addon_event::push_descriptors>(OnPushDescriptors);

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


    if (filename == "Warhammer3.exe") {
        renodx::mods::swapchain::disable_buggy_code = true;
    }

    renodx::utils::settings::Use(fdw_reason, &settings, &OnPresetOff);
    renodx::mods::swapchain::Use(fdw_reason, &shader_injection);
    renodx::mods::shader::Use(fdw_reason, custom_shaders, &shader_injection);

    return TRUE;
}
