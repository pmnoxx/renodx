// common.hlsl v0.2

#include "./shared.h"

#ifndef COMMON_H_
#define COMMON_H_


#define INV_REINHARD 2.f


float ReinhardScalable(float color, float channel_max = 1.f, float channel_min = 0.f, float gray_in = 0.18f, float gray_out = 0.18f) {
  /*float exposure = (channel_max * (channel_min * gray_out + channel_min - gray_out));
                   / max(gray_in * (gray_out - channel_max));

  float numerator = -channel_max * (channel_min * color + channel_min - color);
  float denominator = (exposure * (channel_max - color));
  return renodx::math::DivideSafe(numerator, denominator, renodx::math::FLT16_MAX); */

  float numerator = color * (channel_max - 0.18f);
  float denominator = max(.0001f, channel_max - color);
  return renodx::math::DivideSafe(numerator, denominator, 10.f);
}

#define SCENE_TYPE_UNKNOWN 0
#define SCENE_TYPE_2D_CHARACTER 1
#define SCENE_TYPE_2D_BACKGROND 2
#define SCENE_TYPE_3D 3
#define SCENE_TYPE_DISPLAY_OUTPUT 4


// Helper function to get perceptual boost strength for a given scene type
float GetPerceptualBoostStrength(float scene_type) {
    if (scene_type == SCENE_TYPE_2D_CHARACTER) {
        return RENODX_PERCEPTUAL_BOOST_2D_CHARACTER;
    } else if (scene_type == SCENE_TYPE_2D_BACKGROND) {
        return RENODX_PERCEPTUAL_BOOST_2D_BACKGROUND;
    } else if (scene_type == SCENE_TYPE_3D) {
        return RENODX_PERCEPTUAL_BOOST_3D;
    } else if (scene_type == SCENE_TYPE_DISPLAY_OUTPUT) {
        return RENODX_PERCEPTUAL_BOOST_DISPLAY_OUTPUT;
    }
    // For unknown scene types, default to 1.0
    return 1.f;
}

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

// Function to apply reverse Reinhard tone mapping
float3 ApplyReverseReinhard(float3 color, float scene_type = SCENE_TYPE_UNKNOWN) {
    if (RENODX_TONE_MAP_TYPE == 0) {
      return color;
    }
        // Get perceptual boost strength for this scene type
    float boost_strength = GetPerceptualBoostStrength(scene_type);
    
    // If strength is 0, return original color
    if (boost_strength <= 0.f) {
        return color;
    }
    
    // Use global toggle and channel_max from shader_injection
    if (shader_injection.perceptual_boost_mode == 0.f) {
        // OFF
        return color;
    } else if (shader_injection.perceptual_boost_mode == 1.f) {
        // Reinhard
        float y = renodx::color::y::from::BT709(color);
        float scale = ReinhardScalable(y, shader_injection.perceptual_boost_channel_max, 0.f, 0.18f, 0.18f);
        color *= min(10.f, renodx::math::DivideSafe(scale, y, 10.f));
        // Apply strength scaling
        color = lerp(color, color * (1.f + shader_injection.perceptual_boost_reinhard_strength), boost_strength);
        return color;
    } else if (shader_injection.perceptual_boost_mode == 2.f) {
        // XY->PQ (inverse tonemapping)
        float3 xyz = renodx::color::XYZ::from::BT709(color.xyz);
        float new_grey = 0.18f;
        if (xyz.y > 0.0000001f)
        {
            float3 newXYZ = sign(xyz) * renodx::color::pq::Decode(
                renodx::color::pq::Encode(abs(xyz), 10000.f) * (1.f + shader_injection.perceptual_boost_xypq_param),
                10000.f);
            new_grey = renodx::color::pq::Decode(
                renodx::color::pq::Encode(new_grey, 10000.f) * (1.f + shader_injection.perceptual_boost_xypq_param),
                10000.f).r;
            xyz = lerp(
                xyz * (newXYZ.y / xyz.y),
                newXYZ.xyz,
                shader_injection.perceptual_boost_xypq_color);
        }
        color.xyz = lerp(color.xyz, renodx::color::bt709::from::XYZ(xyz) * (0.18f / new_grey), shader_injection.perceptual_boost_xypq_strength * boost_strength);

        
        return color;
    } else if (shader_injection.perceptual_boost_mode == 3.f) {
        // ICTCP-based inverse tonemapping
        float3 new_color;
        {
            float3 ictcp = renodx::color::ictcp::from::BT709(color.xyz * (10000.f / 10000.f));
            ictcp.x *= 1.f + shader_injection.perceptual_boost_ictcp_param;
            ictcp.yz *= lerp(1.f, 1.f + shader_injection.perceptual_boost_ictcp_param, shader_injection.perceptual_boost_ictcp_color);
            new_color = renodx::color::bt709::from::ICtCp(ictcp);

            float3 ictcp_grey = renodx::color::ictcp::from::BT709(0.18f * (10000.f / 10000.f));
            ictcp_grey.x *= 1.f + shader_injection.perceptual_boost_ictcp_param;
            ictcp_grey.yz *= lerp(1.f, 1.f + shader_injection.perceptual_boost_ictcp_param, shader_injection.perceptual_boost_ictcp_color);
            float3 new_grey = renodx::color::bt709::from::ICtCp(ictcp_grey);
            new_color *= (0.18f / new_grey);


        }


        color.xyz = lerp(color.xyz, new_color, shader_injection.perceptual_boost_ictcp_strength * boost_strength);


        return color;
    }

    return color;
}

float3 ComputeUntonemappedGraded(float3 untonemapped, float3 graded_sdr_color, float3 neutral_sdr_color) {
  [branch]
  if (shader_injection.color_grade_strength == 0) {
    return untonemapped;
  } else {
    if (shader_injection.color_grade_per_channel_blowout_restoration != 0.f
        || shader_injection.color_grade_per_channel_hue_correction != 0.f
        || shader_injection.color_grade_per_channel_chrominance_correction != 0.f) {
      graded_sdr_color = renodx::draw::ApplyPerChannelCorrection(
          untonemapped,
          graded_sdr_color,
          shader_injection.color_grade_per_channel_blowout_restoration,
          shader_injection.color_grade_per_channel_hue_correction,
          shader_injection.color_grade_per_channel_chrominance_correction);
    }

    return renodx::tonemap::UpgradeToneMap(
        untonemapped,
        neutral_sdr_color,
        graded_sdr_color,
        shader_injection.color_grade_strength,
        shader_injection.color_grade_tone_map_pass_autocorrection);
  }
}

float3 ApplyPerceptualBoostAndToneMap(float3 color, float scene_type = SCENE_TYPE_UNKNOWN) {

    color.xyz = renodx::color::srgb::DecodeSafe(color.xyz);
    color = ApplyReverseReinhard(color, scene_type);
    if (shader_injection.perceptual_boost_mode > 0.f) {
  //      color = renodx::draw::ToneMapPass(color);
    }
    color.xyz = renodx::color::srgb::EncodeSafe(color.xyz);
    return color;
}

float3 ToneMapPassCustom(float3 color) {
  // Apply gamma correction from ColorGradeGamma (0.0-1.0 mapped to gamma 1.0-2.2)
  if (RENODX_ENABLE_UI_TONEMAPPASS > 0.f) {
    return color;
  }
  float gamma = shader_injection.tone_map_gamma * 2.f;
  color.xyz = sign(color.xyz) * pow(abs(color.xyz), 1.f / gamma);

  return renodx::draw::ToneMapPass(color);
}

float3 ToneMapPassCustom2(float3 color) {
  // Apply gamma correction from ColorGradeGamma (0.0-1.0 mapped to gamma 1.0-2.2)
  float gamma = shader_injection.tone_map_gamma * 2.f;
  color.xyz = sign(color.xyz) * pow(abs(color.xyz), 1.f / gamma);

  return renodx::draw::ToneMapPass(color);
}

float3 ToneMapPassWrapper(float3 color) {
 //   color = (color > 0 | color < 0) ? color : 0.f;
    if (RENODX_ENABLE_UI_TONEMAPPASS) {
      return color;
    }
    float gamma = shader_injection.tone_map_gamma * 2.f;
    color.xyz = sign(color.xyz) * pow(abs(color.xyz), 1.f / gamma);
    return renodx::draw::ToneMapPass(color);
}
/*
float3 ToneMapPassWrapper(float3 untonemapped, float3 graded_sdr_color) {
  if (RENODX_ENABLE_UI_TONEMAPPASS) {
    return graded_sdr_color;
  }
  float3 color = renodx::draw::ToneMapPass(untonemapped, graded_sdr_color);
  float gamma = shader_injection.tone_map_gamma * 2.f;
  color.xyz = sign(color.xyz) * pow(abs(color.xyz), 1.f / gamma);
  return color;
}*/

float3 ToneMapPassWrapper(float3 untonemapped, float3 graded_sdr_color, float3 neutral_sdr_color) {
  float3 color = ComputeUntonemappedGraded(untonemapped, graded_sdr_color, neutral_sdr_color);
  if (RENODX_ENABLE_UI_TONEMAPPASS) {
    return color;
  }
  float gamma = shader_injection.tone_map_gamma * 2.f;
  color.xyz = sign(color.xyz) * pow(abs(color.xyz), 1.f / gamma);
  color = renodx::draw::ToneMapPass(color);
  return color;
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

  float3 UpgradeToneMapCustom(
      float3 color_untonemapped,
      float3 color_tonemapped,
      float3 color_tonemapped_graded,
      float post_process_strength = 1.f,
      float auto_correction = 0.f) {
    float ratio = 1.f;

    float y_untonemapped = renodx::color::y::from::BT709(color_untonemapped);
    float y_tonemapped = renodx::color::y::from::BT709(color_tonemapped);
    float y_tonemapped_graded = renodx::color::y::from::BT709(color_tonemapped_graded);

    if (y_untonemapped < y_tonemapped) {
      // If substracting (user contrast or paperwhite) scale down instead
      // Should only apply on mismatched HDR
      ratio = y_untonemapped / y_tonemapped;
    } else {
      // float y_delta = y_untonemapped - y_tonemapped;
      float y_delta = y_untonemapped / y_tonemapped;
      // y_delta = max(0, y_delta);  // Cleans up NaN
      y_delta = max(1, y_delta);  // Cleans up NaN
                                  // const float y_new = y_tonemapped_graded + y_delta;
      const float y_new = y_tonemapped_graded * y_delta;

      const bool y_valid = (y_tonemapped_graded > 0);  // Cleans up NaN and ignore black
      ratio = y_valid ? (y_new / y_tonemapped_graded) : 0;
    }
    float auto_correct_ratio = lerp(1.f, ratio, saturate(y_untonemapped));
    ratio = lerp(ratio, auto_correct_ratio, auto_correction);

    float3 color_scaled = color_tonemapped_graded * ratio;
    // Match hue
    color_scaled = renodx::color::correct::Hue(color_scaled, color_tonemapped_graded);
    return lerp(color_untonemapped, color_scaled, post_process_strength);
  }

#define cmp -


#endif
