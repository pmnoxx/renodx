#ifndef SRC_TEMPLATE_SHARED_H_
#define SRC_TEMPLATE_SHARED_H_



struct ShaderInjectData {

  // Debug Mode
  float debug_mode; // Debug mode for development and testing (0.0-1.0) - shader_injection[0][0]
  float debug_mode2; // Debug mode 2 for development and testing (0.0-1.0) - shader_injection[0][1]
  float debug_mode3; // Debug mode 3 for development and testing (0.0-1.0) - shader_injection[0][2]
  float debug_mode4; // Debug mode 4 for development and testing (0.0-1.0) - shader_injection[0][3]

  float peak_white_nits;              // 1000.f - shader_injection[1][0]
  float diffuse_white_nits;           // shader_injection[1][1]
  float graphics_white_nits;          // shader_injection[1][2]
  float color_grade_strength;         // shader_injection[1][3]
  float tone_map_type;                // shader_injection[2][0]
  float tone_map_exposure;            // shader_injection[2][1]
  float tone_map_highlights;          // shader_injection[2][2]
  float tone_map_shadows;             // shader_injection[2][3]
  float tone_map_contrast;            // shader_injection[3][0]
  float tone_map_saturation;          // shader_injection[3][1]
  float tone_map_highlight_saturation; // shader_injection[3][2]
  float tone_map_blowout;             // shader_injection[3][3]
  float tone_map_flare;               // shader_injection[4][0]
  float tone_map_hue_correction;      // shader_injection[4][1]
  float tone_map_hue_shift;           // shader_injection[4][2]
  float tone_map_working_color_space; // shader_injection[4][3]
  float tone_map_clamp_color_space;   // shader_injection[5][0]
  float tone_map_clamp_peak;          // shader_injection[5][1]
  float tone_map_hue_processor;       // shader_injection[5][2]
  float tone_map_per_channel;         // shader_injection[5][3]
  float gamma_correction;             // shader_injection[6][0]
  float tone_map_gamma;               // For ColorGradeGamma setting (0.0-1.0) - shader_injection[6][1]
  float dice_shoulder;                // shader_injection[6][2]
  float intermediate_scaling;         // shader_injection[6][3]
  float intermediate_encoding;        // shader_injection[7][0]
  float intermediate_color_space;     // shader_injection[7][1]
  float swap_chain_decoding;          // shader_injection[7][2]
  float swap_chain_gamma_correction;  // shader_injection[7][3]
  float swap_chain_custom_color_space; // shader_injection[8][0]
  float swap_chain_clamp_color_space; // shader_injection[8][1]
  float swap_chain_encoding;          // shader_injection[8][2]
  float swap_chain_encoding_color_space; // shader_injection[8][3]
  float reno_drt_white_clip;          // shader_injection[9][0]
  float perceptual_boost_method;      // shader_injection[9][1]
  float perceptual_boost_param;       // shader_injection[9][2]
  float perceptual_boost_color;       // shader_injection[9][3]
  float perceptual_boost_strength;    // shader_injection[10][0]

  float effect_split_x;               // shader_injection[10][1]
  float effect_split_mode;            // shader_injection[10][2]
  float effect_split_angle;           // shader_injection[10][3]

  float color_grade_per_channel_blowout_restoration;     // 0.f - shader_injection[11][0]
  float color_grade_per_channel_hue_correction;          // 0.f - shader_injection[11][1]
  float color_grade_per_channel_chrominance_correction;  // 0.f - shader_injection[11][2]
  float color_grade_tone_map_pass_autocorrection;        // TODO add slider - shader_injection[11][3]
  
  // Perceptual Boost
  float perceptual_boost_mode; // 0 = OFF, 1 = Reinhard, 2 = XY->PQ, 3 = ICTCP - shader_injection[12][0]
  float perceptual_boost_channel_max; // Used for Reinhard, user adjustable - shader_injection[12][1]
  // Reinhard mode parameters
  float perceptual_boost_reinhard_strength; // Used for Reinhard method - shader_injection[12][2]
  float perceptual_boost_reinhard_midpoint; // Midpoint for Reinhard perceptual boost (default: 0.18) - shader_injection[12][3]
  // XY->PQ mode parameters
  float perceptual_boost_xypq_param; // Used for XY->PQ method - shader_injection[13][0]
  float perceptual_boost_xypq_color; // Used for XY->PQ method - shader_injection[13][1]
  float perceptual_boost_xypq_strength; // Used for XY->PQ method - shader_injection[13][2]
  float perceptual_boost_xypq_midpoint; // Midpoint for XY->PQ perceptual boost (default: 0.18) - shader_injection[13][3]
  // ICTCP mode parameters
  float perceptual_boost_ictcp_param; // Used for ICTCP method - shader_injection[14][0]
  float perceptual_boost_ictcp_color; // Used for ICTCP method - shader_injection[14][1]
  float perceptual_boost_ictcp_strength; // Used for ICTCP method - shader_injection[14][2]
  float perceptual_boost_ictcp_midpoint; // Midpoint for ICTCP perceptual boost (default: 0.18) - shader_injection[14][3]
  // Scene Type Perceptual Boost Strength
  float perceptual_boost_2d_character; // Perceptual boost strength for 2D character scenes (0.0-10.0) - shader_injection[15][0]
  float perceptual_boost_2d_background; // Perceptual boost strength for 2D background scenes (0.0-10.0) - shader_injection[15][1]
  float perceptual_boost_3d; // Perceptual boost strength for 3D scenes (0.0-10.0) - shader_injection[15][2]
  float perceptual_boost_display_output; // shader_injection[15][3]


  float display_map_type;             // shader_injection[16][3]
  float display_map_peak;             // shader_injection[17][0]
  float display_map_shoulder;         // shader_injection[17][1]
  float display_map_saturation;       // shader_injection[17][2]

  // Tone Map Pass Control
  float enable_tone_map_pass; // Enable/disable tone mapping pass (0.0 = off, 1.0 = on) - shader_injection[17][3]

  // Render Pass Simulation
  float simulate_render_pass; // Simulate render pass processing (0.0 = off, 1.0 = on) - shader_injection[18][0]
  float random_seed;          // shader_injection[18][1]

  // CUSTOM GAME SETTINGS
  
  float custom_character_brightness; // Custom character brightness for UI slider - shader_injection[18][2]
  float custom_text_brightness;      // shader_injection[18][3]
  float custom_game_compatiblity_mode; // Custom game compatibility mode (disable broken in-game features) - shader_injection[19][0]
};

#ifndef __cplusplus
#if (__SHADER_TARGET_MAJOR == 3)

float4 shader_injection[20] : register(c50);
// Debug Mode
#define RENODX_DEBUG_MODE                      shader_injection[0][0]
#define RENODX_DEBUG_MODE2                     shader_injection[0][1]
#define RENODX_DEBUG_MODE3                     shader_injection[0][2]
#define RENODX_DEBUG_MODE4                     shader_injection[0][3]

#define RENODX_PEAK_WHITE_NITS               shader_injection[1][0]
#define RENODX_DIFFUSE_WHITE_NITS            shader_injection[1][1]
#define RENODX_GRAPHICS_WHITE_NITS           shader_injection[1][2]
#define RENODX_COLOR_GRADE_STRENGTH          shader_injection[1][3]
#define RENODX_TONE_MAP_TYPE                 shader_injection[2][0]
#define RENODX_TONE_MAP_EXPOSURE             shader_injection[2][1]
#define RENODX_TONE_MAP_HIGHLIGHTS           shader_injection[2][2]
#define RENODX_TONE_MAP_SHADOWS              shader_injection[2][3]
#define RENODX_TONE_MAP_CONTRAST             shader_injection[3][0]
#define RENODX_TONE_MAP_SATURATION           shader_injection[3][1]
#define RENODX_TONE_MAP_HIGHLIGHT_SATURATION shader_injection[3][2]
#define RENODX_TONE_MAP_BLOWOUT              shader_injection[3][3]
#define RENODX_TONE_MAP_FLARE                shader_injection[4][0]
#define RENODX_TONE_MAP_HUE_CORRECTION       shader_injection[4][1]
#define RENODX_TONE_MAP_HUE_SHIFT            shader_injection[4][2]
#define RENODX_TONE_MAP_WORKING_COLOR_SPACE  shader_injection[4][3]
#define RENODX_TONE_MAP_CLAMP_COLOR_SPACE    shader_injection[5][0]
#define RENODX_TONE_MAP_CLAMP_PEAK           shader_injection[5][1]
#define RENODX_TONE_MAP_HUE_PROCESSOR        shader_injection[5][2]
#define RENODX_TONE_MAP_PER_CHANNEL          shader_injection[5][3]
#define RENODX_GAMMA_CORRECTION              shader_injection[6][0]
#define RENODX_TONE_MAP_GAMMA                shader_injection[6][1]
#define RENODX_DICE_SHOULDER                 shader_injection[6][2]
#define RENODX_INTERMEDIATE_SCALING          shader_injection[6][3]
#define RENODX_INTERMEDIATE_ENCODING           shader_injection[7][0]
#define RENODX_INTERMEDIATE_COLOR_SPACE        shader_injection[7][1]
#define RENODX_SWAP_CHAIN_DECODING             shader_injection[7][2]
#define RENODX_SWAP_CHAIN_GAMMA_CORRECTION     shader_injection[7][3]
#define RENODX_SWAP_CHAIN_CUSTOM_COLOR_SPACE   shader_injection[8][0]
#define RENODX_SWAP_CHAIN_CLAMP_COLOR_SPACE    shader_injection[8][1]
#define RENODX_SWAP_CHAIN_ENCODING             shader_injection[8][2]
#define RENODX_SWAP_CHAIN_ENCODING_COLOR_SPACE shader_injection[8][3]

#define RENODX_RENO_DRT_WHITE_CLIP               shader_injection[9][0]
#define RENODX_PERCEPTUAL_BOOST_PARAM          shader_injection[9][1]
#define RENODX_PERCEPTUAL_BOOST_COLOR         shader_injection[9][2]
#define RENODX_PERCEPTUAL_BOOST_STRENGTH          shader_injection[9][3]
#define RENODX_PERCEPTUAL_BOOST_METHOD           shader_injection[9][1]

#define RENODX_EFFECT_SPLIT_X                    shader_injection[10][1]
#define RENODX_EFFECT_SPLIT_MODE                 shader_injection[10][2]
#define RENODX_EFFECT_SPLIT_ANGLE                shader_injection[10][3]
#define RENODX_PERCEPTUAL_BOOST_METHOD_XY_PQ 1
#define RENODX_PERCEPTUAL_BOOST_METHOD_ICTCP 2
#define RENODX_COLOR_GRADE_PER_CHANNEL_BLOWOUT_RESTORATION shader_injection[11][0]
#define RENODX_COLOR_GRADE_PER_CHANNEL_HUE_CORRECTION shader_injection[11][1]
#define RENODX_COLOR_GRADE_PER_CHANNEL_CHROMINANCE_CORRECTION shader_injection[11][2]
#define RENODX_COLOR_GRADE_TONE_MAP_PASS_AUTOCORRECTION shader_injection[11][3]

#define RENODX_PERCEPTUAL_BOOST_MODE             shader_injection[12][0]
#define RENODX_PERCEPTUAL_BOOST_CHANNEL_MAX      shader_injection[12][1]
#define RENODX_PERCEPTUAL_BOOST_REINHARD_STRENGTH shader_injection[12][2]
#define RENODX_PERCEPTUAL_BOOST_REINHARD_MIDPOINT shader_injection[12][3]
// XY->PQ mode parameters
#define RENODX_PERCEPTUAL_BOOST_XYPQ_PARAM     shader_injection[13][0]
#define RENODX_PERCEPTUAL_BOOST_XYPQ_MIDPOINT  shader_injection[13][3]
// ICTCP mode parameters
#define RENODX_PERCEPTUAL_BOOST_ICTCP_MIDPOINT shader_injection[14][3]
#define RENODX_PERCEPTUAL_BOOST_XYPQ_COLOR     shader_injection[13][1]
#define RENODX_PERCEPTUAL_BOOST_XYPQ_STRENGTH  shader_injection[13][2]
// ICTCP mode parameters
#define RENODX_PERCEPTUAL_BOOST_ICTCP_PARAM    shader_injection[14][0]
#define RENODX_PERCEPTUAL_BOOST_ICTCP_COLOR    shader_injection[14][1]
#define RENODX_PERCEPTUAL_BOOST_ICTCP_STRENGTH shader_injection[14][2]
#define RENODX_PERCEPTUAL_BOOST_2D_CHARACTER   shader_injection[15][0]
#define RENODX_PERCEPTUAL_BOOST_2D_BACKGROUND  shader_injection[15][1]
#define RENODX_PERCEPTUAL_BOOST_3D             shader_injection[15][2]
#define RENODX_PERCEPTUAL_BOOST_DISPLAY_OUTPUT shader_injection[15][3]


#define DISPLAY_MAP_TYPE                       shader_injection[16][3]
#define DISPLAY_MAP_PEAK                       shader_injection[17][0]
#define DISPLAY_MAP_SHOULDER                   shader_injection[17][1]
#define DISPLAY_MAP_SATURATION                 shader_injection[17][2]

#define COLOR_GRADE_PER_CHANNEL_BLOWOUT_RESTORATION     shader_injection[11][0]
#define COLOR_GRADE_PER_CHANNEL_HUE_CORRECTION          shader_injection[11][1]
#define COLOR_GRADE_PER_CHANNEL_CHROMINANCE_CORRECTION  shader_injection[11][2]
#define COLOR_GRADE_TONE_MAP_PASS_AUTOCORRECTION        shader_injection[11][3]


#define RENODX_ENABLE_UI_TONEMAPPASS          shader_injection[17][3]
#define RENODX_SIMULATE_RENDER_PASS           shader_injection[18][0]
#define RENODX_RANDOM_SEED                   shader_injection[18][1]

// CUSTOM GAME SETTINGS
#define CUSTOM_CHARACTER_BRIGHTNESS      shader_injection[18][2]
#define CUSTOM_TEXT_BRIGHTNESS           shader_injection[18][3]
#define CUSTOM_GAME_COMPATIBLITY_MODE    shader_injection[19][0]

#define RENODX_RENO_DRT_TONE_MAP_METHOD renodx::tonemap::renodrt::config::tone_map_method::REINHARD
#else
#if ((__SHADER_TARGET_MAJOR == 5 && __SHADER_TARGET_MINOR >= 1) || __SHADER_TARGET_MAJOR >= 6)
cbuffer shader_injection : register(b13, space50) {
#elif (__SHADER_TARGET_MAJOR < 5) || ((__SHADER_TARGET_MAJOR == 5) && (__SHADER_TARGET_MINOR < 1))
cbuffer shader_injection : register(b13) {
#endif
  ShaderInjectData shader_injection : packoffset(c0);
}

// Don't use tonemapper in tonemappass if tone_map_type is 4 or 5
#define RENODX_TONE_MAP_TYPE                 (shader_injection.tone_map_type < 4.f ? shader_injection.tone_map_type : 1.f)
// Consider moving these to the shader inject data
#define RENODX_DICE_SHOULDER                 shader_injection.dice_shoulder
#define RENODX_PEAK_WHITE_NITS               shader_injection.peak_white_nits
#define RENODX_DIFFUSE_WHITE_NITS            shader_injection.diffuse_white_nits
#define RENODX_GRAPHICS_WHITE_NITS           shader_injection.graphics_white_nits
#define RENODX_GAMMA_CORRECTION              shader_injection.gamma_correction
#define RENODX_TONE_MAP_PER_CHANNEL          shader_injection.tone_map_per_channel
#define RENODX_TONE_MAP_WORKING_COLOR_SPACE  shader_injection.tone_map_working_color_space
#define RENODX_TONE_MAP_HUE_PROCESSOR        shader_injection.tone_map_hue_processor
#define RENODX_TONE_MAP_HUE_CORRECTION       shader_injection.tone_map_hue_correction
#define RENODX_TONE_MAP_HUE_SHIFT            shader_injection.tone_map_hue_shift
#define RENODX_TONE_MAP_CLAMP_COLOR_SPACE    shader_injection.tone_map_clamp_color_space
#define RENODX_TONE_MAP_CLAMP_PEAK           shader_injection.tone_map_clamp_peak
#define RENODX_TONE_MAP_EXPOSURE             shader_injection.tone_map_exposure
#define RENODX_TONE_MAP_HIGHLIGHTS           shader_injection.tone_map_highlights
#define RENODX_TONE_MAP_SHADOWS              shader_injection.tone_map_shadows
#define RENODX_TONE_MAP_CONTRAST             shader_injection.tone_map_contrast
#define RENODX_TONE_MAP_SATURATION           shader_injection.tone_map_saturation
#define RENODX_TONE_MAP_HIGHLIGHT_SATURATION shader_injection.tone_map_highlight_saturation
#define RENODX_TONE_MAP_BLOWOUT              shader_injection.tone_map_blowout
#define RENODX_TONE_MAP_FLARE                shader_injection.tone_map_flare
#define RENODX_COLOR_GRADE_STRENGTH          shader_injection.color_grade_strength
#define RENODX_INTERMEDIATE_ENCODING         shader_injection.intermediate_encoding
#define RENODX_SWAP_CHAIN_DECODING           shader_injection.swap_chain_decoding
#define RENODX_SWAP_CHAIN_GAMMA_CORRECTION   shader_injection.swap_chain_gamma_correction
// #define RENODX_SWAP_CHAIN_DECODING_COLOR_SPACE shader_injection.swap_chain_decoding_color_space
#define RENODX_SWAP_CHAIN_CUSTOM_COLOR_SPACE shader_injection.swap_chain_custom_color_space
// #define RENODX_SWAP_CHAIN_SCALING_NITS         shader_injection.swap_chain_scaling_nits
// #define RENODX_SWAP_CHAIN_CLAMP_NITS           shader_injection.swap_chain_clamp_nits
#define RENODX_SWAP_CHAIN_CLAMP_COLOR_SPACE    shader_injection.swap_chain_clamp_color_space
#define RENODX_SWAP_CHAIN_ENCODING             shader_injection.swap_chain_encoding
#define RENODX_SWAP_CHAIN_ENCODING_COLOR_SPACE shader_injection.swap_chain_encoding_color_space
#define RENODX_RENO_DRT_TONE_MAP_METHOD        renodx::tonemap::renodrt::config::tone_map_method::REINHARD

#define RENODX_RENO_DRT_WHITE_CLIP               shader_injection.reno_drt_white_clip
#define RENODX_PERCEPTUAL_BOOST_MODE             shader_injection.perceptual_boost_mode

#define RENODX_PERCEPTUAL_BOOST_PARAM          shader_injection.perceptual_boost_param
#define RENODX_PERCEPTUAL_BOOST_COLOR         shader_injection.perceptual_boost_color
#define RENODX_PERCEPTUAL_BOOST_STRENGTH          shader_injection.perceptual_boost_strength
#define RENODX_PERCEPTUAL_BOOST_METHOD           shader_injection.perceptual_boost_method
#define RENODX_PERCEPTUAL_BOOST_METHOD_XY_PQ 1
#define RENODX_PERCEPTUAL_BOOST_METHOD_ICTCP 2
#define RENODX_PIXEL_SHADER_DECODE_MODE shader_injection.pixel_shader_decode_mode


#define RENODX_PERCEPTUAL_BOOST_REINHARD_STRENGTH shader_injection.perceptual_boost_reinhard_strength
#define RENODX_PERCEPTUAL_BOOST_REINHARD_MIDPOINT shader_injection.perceptual_boost_reinhard_midpoint
// XY->PQ mode parameters
#define RENODX_PERCEPTUAL_BOOST_XYPQ_PARAM     shader_injection.perceptual_boost_xypq_param
#define RENODX_PERCEPTUAL_BOOST_XYPQ_MIDPOINT  shader_injection.perceptual_boost_xypq_midpoint
// ICTCP mode parameters
#define RENODX_PERCEPTUAL_BOOST_ICTCP_MIDPOINT shader_injection.perceptual_boost_ictcp_midpoint
#define RENODX_PERCEPTUAL_BOOST_XYPQ_COLOR     shader_injection.perceptual_boost_xypq_color
#define RENODX_PERCEPTUAL_BOOST_XYPQ_STRENGTH  shader_injection.perceptual_boost_xypq_strength
// ICTCP mode parameters
#define RENODX_PERCEPTUAL_BOOST_ICTCP_PARAM    shader_injection.perceptual_boost_ictcp_param
#define RENODX_PERCEPTUAL_BOOST_ICTCP_COLOR    shader_injection.perceptual_boost_ictcp_color
#define RENODX_PERCEPTUAL_BOOST_ICTCP_STRENGTH shader_injection.perceptual_boost_ictcp_strength
#define RENODX_PERCEPTUAL_BOOST_2D_CHARACTER   shader_injection.perceptual_boost_2d_character
#define RENODX_PERCEPTUAL_BOOST_2D_BACKGROUND  shader_injection.perceptual_boost_2d_background
#define RENODX_PERCEPTUAL_BOOST_3D             shader_injection.perceptual_boost_3d
#define RENODX_PERCEPTUAL_BOOST_DISPLAY_OUTPUT shader_injection.perceptual_boost_display_output

// Debug Mode
#define RENODX_DEBUG_MODE                      shader_injection.debug_mode
#define RENODX_DEBUG_MODE2                     shader_injection.debug_mode2
#define RENODX_DEBUG_MODE3                     shader_injection.debug_mode3

#define DISPLAY_MAP_TYPE                       shader_injection.display_map_type
#define DISPLAY_MAP_PEAK                       shader_injection.display_map_peak
#define DISPLAY_MAP_SHOULDER                   shader_injection.display_map_shoulder
#define DISPLAY_MAP_SATURATION                 shader_injection.display_map_saturation

#define COLOR_GRADE_PER_CHANNEL_BLOWOUT_RESTORATION     shader_injection.color_grade_per_channel_blowout_restoration
#define COLOR_GRADE_PER_CHANNEL_HUE_CORRECTION          shader_injection.color_grade_per_channel_hue_correction
#define COLOR_GRADE_PER_CHANNEL_CHROMINANCE_CORRECTION  shader_injection.color_grade_per_channel_chrominance_correction


#define RENODX_ENABLE_UI_TONEMAPPASS          shader_injection.enable_tone_map_pass
#define RENODX_SIMULATE_RENDER_PASS           shader_injection.simulate_render_pass

#define RENODX_COLOR_GRADE_PER_CHANNEL_BLOWOUT_RESTORATION shader_injection.color_grade_per_channel_blowout_restoration
#define RENODX_COLOR_GRADE_PER_CHANNEL_HUE_CORRECTION shader_injection.color_grade_per_channel_hue_correction
#define RENODX_COLOR_GRADE_PER_CHANNEL_CHROMINANCE_CORRECTION shader_injection.color_grade_per_channel_chrominance_correction
#define RENODX_COLOR_GRADE_TONE_MAP_PASS_AUTOCORRECTION shader_injection.color_grade_tone_map_pass_autocorrection

#define RENODX_TONE_MAP_GAMMA shader_injection.tone_map_gamma
#define RENODX_PERCEPTUAL_BOOST_REINHARD_MIDPOINT shader_injection.perceptual_boost_reinhard_midpoint
#define RENODX_PERCEPTUAL_BOOST_REINHARD_STRENGTH shader_injection.perceptual_boost_reinhard_strength
#define RENODX_PERCEPTUAL_BOOST_CHANNEL_MAX shader_injection.perceptual_boost_channel_max
#define RENODX_PERCEPTUAL_BOOST_XYPQ_MIDPOINT shader_injection.perceptual_boost_xypq_midpoint
#define RENODX_PERCEPTUAL_BOOST_XYPQ_PARAM shader_injection.perceptual_boost_xypq_param
#define RENODX_PERCEPTUAL_BOOST_XYPQ_COLOR shader_injection.perceptual_boost_xypq_color
#define RENODX_PERCEPTUAL_BOOST_XYPQ_STRENGTH shader_injection.perceptual_boost_xypq_strength
#define RENODX_PERCEPTUAL_BOOST_ICTCP_MIDPOINT shader_injection.perceptual_boost_ictcp_midpoint
#define RENODX_PERCEPTUAL_BOOST_ICTCP_PARAM shader_injection.perceptual_boost_ictcp_param
#define RENODX_PERCEPTUAL_BOOST_ICTCP_COLOR shader_injection.perceptual_boost_ictcp_color
#define RENODX_PERCEPTUAL_BOOST_ICTCP_STRENGTH shader_injection.perceptual_boost_ictcp_strength

// CUSTOM GAME SETTINGS

#define CUSTOM_CHARACTER_BRIGHTNESS      shader_injection.custom_character_brightness
#define CUSTOM_TEXT_BRIGHTNESS           shader_injection.custom_text_brightness
#define CUSTOM_GAME_COMPATIBLITY_MODE    shader_injection.custom_game_compatiblity_mode

#endif

#include "../../shaders/renodx.hlsl"

#endif

#endif  // SRC_TEMPLATE_SHARED_H_
