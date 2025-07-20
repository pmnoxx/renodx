#ifndef COMMON_H_
#define COMMON_H_

#include "./DICE.hlsl"
#include "./shared.h"

float4 debug_mode(float4 color, float2 pos, float shift_y = 0.f) {
  float2 box = float2(0.99f, 0.99f - shift_y);
  float2 dim = float2(0.01f, 0.01f);

  if (length(pos >= box && pos <= box + dim) > 1.f) {
    float part = (pos.x - box.x) / dim.x;
    if (part < 1. / 3.f) {
      return float4(1.f, 0.f, 0.f, 1.f);
    }
    if (part < 2. / 3.f) {
      return float4(0.f, 1.f, 0.f, 1.f);
    }
    return float4(0.f, 0.f, 1.f, 1.f);
  }

  return color;
}

float3 applyUserTonemap(float3 untonemapped, float userToneMapType) {
  float3 outputColor;
  if (userToneMapType == 0.f) {  // If vanilla is selected
    outputColor = saturate(untonemapped);
  } else {
    outputColor = untonemapped;
  }

  if (userToneMapType == 5.f) {  // DefaultToneMapper
    return renodx::draw::ToneMapPass(untonemapped);
  }

  if (userToneMapType != 0) {  // UserColorGrading, pre-tonemap
    outputColor = renodx::color::grade::UserColorGrading(
        outputColor,
        shader_injection.tone_map_exposure,    // exposure
        shader_injection.tone_map_highlights,  // highlights
        shader_injection.tone_map_shadows,     // shadows
        shader_injection.tone_map_contrast,    // contrast
        1.f,                                   // saturation, we'll do this post-tonemap
        0.f,                                   // dechroma, post tonemapping
        0.f);                                  // hue correction, Post tonemapping
  }

  // Start tonemapper if/else
  if (userToneMapType == 2.f) {  // DICE
    DICESettings DICEconfig = DefaultDICESettings();
    DICEconfig.Type = 3;
    DICEconfig.ShoulderStart = RENODX_DICE_SHOULDER; //0.33f;  // Start shoulder

    float dicePaperWhite = shader_injection.graphics_white_nits / 80.f;
    float dicePeakWhite = shader_injection.peak_white_nits / 80.f;

    outputColor = DICETonemap(outputColor * dicePaperWhite, dicePeakWhite, DICEconfig) / dicePaperWhite;
  } else if (userToneMapType == 3.f) {  // baby reinhard
    float ReinhardPeak = shader_injection.peak_white_nits / shader_injection.graphics_white_nits;
    outputColor = renodx::tonemap::ReinhardScalable(outputColor, ReinhardPeak);

  } else if (userToneMapType == 4.f) {  // Frostbite
    float frostbitePeak = shader_injection.peak_white_nits / shader_injection.graphics_white_nits;
    outputColor = renodx::tonemap::frostbite::BT709(outputColor, frostbitePeak);
  }

  if (userToneMapType != 0) {  // UserColorGrading, post-tonemap
    outputColor = renodx::color::grade::UserColorGrading(
        outputColor,
        1.f,                                       // exposure
        1.f,                                       // highlights
        1.f,                                       // shadows
        1.f,                                       // contrast
        shader_injection.tone_map_saturation,      // saturation
        0.f,                                       // dechroma, we don't need it
        shader_injection.tone_map_hue_correction,  // Hue Correction Strength
        renodx::tonemap::Reinhard(untonemapped));  // Hue Correction Type
  }

  outputColor = renodx::color::bt709::clamp::BT2020(outputColor);  // Clamp to BT2020 to avoid negative colors

  return outputColor;
}

float3 extendedTonemapPass(float3 color, float2 uv) {
  //  color = debug_mode(float4(color, 1.f), uv).xyz;
  if (shader_injection.tone_map_type == 4.f) {  // DICE
    color = applyUserTonemap(color, 2.f);
  } else if (shader_injection.tone_map_type == 5.f) {  // Frostbite
    color = applyUserTonemap(color, 4.f);
  } else {
    color.xyz = renodx::draw::ToneMapPass(color.xyz);
  }
  return color;
}

float3 renodx_ksp_apply_tonemap_and_boost(float3 linearColor, float2 uv)
{
    if (RENODX_TONE_MAP_TYPE > 0.f)
    {
        if (RENODX_PERCEPTUAL_BOOST_METHOD == RENODX_PERCEPTUAL_BOOST_METHOD_XY_PQ && RENODX_PERCEPTUAL_BOOST_STRENGTH > 0.f) {
          // XY->PQ method
          float3 xyz = renodx::color::XYZ::from::BT709(linearColor.xyz);
          float new_grey = 0.18f;
          if (xyz.y > 0.0000001f)
          {
              float3 newXYZ = sign(xyz) * renodx::color::pq::Decode(
                  renodx::color::pq::Encode(abs(xyz), 10000.f) * (1.f + RENODX_PERCEPTUAL_BOOST_PARAM),
                  10000.f);
              new_grey = renodx::color::pq::Decode(
                  renodx::color::pq::Encode(new_grey, 10000.f) * (1.f + RENODX_PERCEPTUAL_BOOST_PARAM),
                  10000.f).r;
              xyz = lerp(
                  xyz * (newXYZ.y / xyz.y),
                  newXYZ.xyz,
                  RENODX_PERCEPTUAL_BOOST_COLOR);
          }
          linearColor.xyz = lerp(linearColor.xyz, renodx::color::bt709::from::XYZ(xyz) * (0.18f / new_grey), RENODX_PERCEPTUAL_BOOST_STRENGTH);
        } else if (RENODX_PERCEPTUAL_BOOST_METHOD == RENODX_PERCEPTUAL_BOOST_METHOD_ICTCP) {
            // ICTCP-based perceptual boost
            float3 ictcp = renodx::color::ictcp::from::BT709(linearColor * (10000.f / 10000.f));
            ictcp.x *= (1.f + RENODX_PERCEPTUAL_BOOST_PARAM);
            ictcp.yz *= (1.f + RENODX_PERCEPTUAL_BOOST_COLOR);
            float3 new_color = renodx::color::bt709::from::ICtCp(ictcp);

            float3 ictcp_grey = renodx::color::ictcp::from::BT709(0.18f * (10000.f / 10000.f));
            ictcp_grey.x *= (1.f +RENODX_PERCEPTUAL_BOOST_PARAM);
            ictcp_grey.yz *= (1. + RENODX_PERCEPTUAL_BOOST_COLOR);
            float3 new_grey = renodx::color::bt709::from::ICtCp(ictcp_grey);
            linearColor = lerp(linearColor, new_color * (0.18f / new_grey), RENODX_PERCEPTUAL_BOOST_STRENGTH);
        }
        linearColor = extendedTonemapPass(linearColor, uv);
    }
    else
    {
        linearColor = saturate(linearColor);
    }
    return linearColor;
}



// Utility function to adjust UI color gamma and scale
float4 renodx_adjust_ui_color(float4 color) {
    if (RENODX_INTERMEDIATE_ENCODING == 1.f) {
        color.rgb = renodx::color::srgb::DecodeSafe(color.rgb);
    } else if (RENODX_INTERMEDIATE_ENCODING == 2.f) {
        color.rgb = renodx::color::gamma::DecodeSafe(color.rgb, 2.2f);
    }
    color.rgb *= RENODX_GRAPHICS_WHITE_NITS / RENODX_DIFFUSE_WHITE_NITS;
    if (RENODX_INTERMEDIATE_ENCODING == 1.f) {
        color.rgb = renodx::color::srgb::EncodeSafe(color.rgb);
    } else if (RENODX_INTERMEDIATE_ENCODING == 2.f) {
        color.rgb = renodx::color::gamma::EncodeSafe(color.rgb, 2.2f);
    }
    return color;
}

float4 ProcessColor(float3 untonemapped, float3 graded) {
  float4 color;
  float midGray = 0.18f;

  if (RENODX_TONE_MAP_TYPE != 0.f) {
    // untonemapped.rgb *= midGray / 0.18f; // Adjust midgray, RenoDRT except 0.18f

    color.rgb = renodx::draw::ToneMapPass(untonemapped, graded);
    color.rgb = renodx::color::correct::GammaSafe(color.rgb);
    color.rgb *= RENODX_GAME_NITS / RENODX_UI_NITS;
    color.rgb = renodx::color::correct::GammaSafe(color.rgb, true);
  } else {
    color.rgb = graded;
    color.rgb = renodx::color::correct::GammaSafe(color.rgb);
    color.rgb *= RENODX_GAME_NITS / RENODX_UI_NITS;
    color.rgb = renodx::color::correct::GammaSafe(color.rgb, true);
  }

  color.w = 1.f;

  return color;
}

// RenoDRTSmoothClamp from FFX, swapped up to use Reinhard
// Added whiteclip
// STILL A WIP
float3 RenoDRTSmoothClamp(float3 untonemapped, float peak = 100.f, float whiteclip = 100.f, float highlightSaturation = 1.f, bool per_channel = false) {
  renodx::tonemap::renodrt::Config renodrt_config =
      renodx::tonemap::renodrt::config::Create();
  renodrt_config.nits_peak = peak;
  renodrt_config.mid_gray_value = 0.18f;
  renodrt_config.mid_gray_nits = 18.f;
  renodrt_config.exposure = 1.f;
  renodrt_config.highlights = 1.f;
  renodrt_config.shadows = 1.f;
  renodrt_config.contrast = 1.f;
  renodrt_config.saturation = 1.f;
  renodrt_config.dechroma = 0.f;
  renodrt_config.flare = 0.f;
  renodrt_config.blowout = -1.f * (highlightSaturation - 1.f);  // Highlight saturation
  renodrt_config.hue_correction_strength = 0.f;
  renodrt_config.tone_map_method =
      renodx::tonemap::renodrt::config::tone_map_method::REINHARD;
  renodrt_config.working_color_space = 1u;  // might need to be 0/1u -- can test
  renodrt_config.white_clip = whiteclip;
  renodrt_config.per_channel = per_channel;

  return renodx::tonemap::renodrt::BT709(untonemapped, renodrt_config);
}

/// Applies Exponential Roll-Off tonemapping using the maximum channel.
/// Used to fit the color into a 0–output_max range for SDR LUT compatibility.
float3 ToneMapMaxCLL(float3 color, float rolloff_start = 0.375f, float output_max = 1.f) {
  color = min(color, 100.f);
  float peak = max(color.r, max(color.g, color.b));
  peak = min(peak, 100.f);
  float log_peak = log2(peak);

  // Apply exponential shoulder in log space
  float log_mapped = renodx::tonemap::ExponentialRollOff(log_peak, log2(rolloff_start), log2(output_max));
  float scale = exp2(log_mapped - log_peak);  // How much to compress all channels

  return min(output_max, color * scale);
}

// A banaid fix Adrian found using a Display Mapper (DICE/Frostbite) to restore highlight saturation
// That was lost running TonemapPass
// We run this function right after untonemapped, and display map the rest of the sdr path down to ~2.f
float3 RestoreHighlightSaturation(float3 color) {
  if (RENODX_TONE_MAP_TYPE != 0.f && DISPLAY_MAP_TYPE != 0.f) {
    float color_y = renodx::color::y::from::BT709(color);

    [branch]
    if (DISPLAY_MAP_TYPE == 1.f) {  // Dice

      float dice_peak = DISPLAY_MAP_PEAK;          // 2.f default
      float dice_shoulder = DISPLAY_MAP_SHOULDER;  // 0.5f default
      float3 dice_color = renodx::tonemap::dice::BT709(color, dice_peak, dice_shoulder);
      color = lerp(color, dice_color, saturate(color_y));

    } else if (DISPLAY_MAP_TYPE == 2.f) {  // Frostbite

      float frostbite_peak = DISPLAY_MAP_PEAK;          // 2.f default
      float frostbite_shoulder = DISPLAY_MAP_SHOULDER;  // 0.5f default
      float frostbite_saturation = 1.f;                 // Hardcode to 1.f
      float3 frostbite_color = renodx::tonemap::frostbite::BT709(color, frostbite_peak, frostbite_shoulder, frostbite_saturation);
      color = lerp(color, frostbite_color, saturate(color_y));

    } else if (DISPLAY_MAP_TYPE == 3.f) {  // RenoDRT NeutralSDR
      float3 neutral_sdr_color = renodx::tonemap::renodrt::NeutralSDR(color);
      color = lerp(color, neutral_sdr_color, saturate(color_y));
    } else if (DISPLAY_MAP_TYPE == 4.f) {  // ToneMapMaxCLL
      color = lerp(color, ToneMapMaxCLL(color), saturate(color_y));
    }

  } else {
    // We dont want to Display Map if the tonemapper is vanilla/preset off or display map is none
    color = color;
  }

  return color;
}
#endif