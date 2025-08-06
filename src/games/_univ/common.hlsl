// common.hlsl v0.4

#include "./shared.h"
#include "./DICE.hlsl"

#ifndef COMMON_H_
#define COMMON_H_


#define INV_REINHARD 2.f

float ReinhardScalable(float color, float channel_max = 1.f, float channel_min = 0.f, float gray_in = 0.18f, float gray_out = 0.18f) {
  /* float exposure = (channel_max * (channel_min * gray_out + channel_min - gray_out));

  float numerator = -channel_max * (channel_min * color + channel_min - color);
  float denominator = (exposure * (channel_max - color));
  retur renodx::math::DivideSafe(numerator, denominator, 3.f); //renodx::math::FLT16_MAX); 
*/
  float numerator = color * (channel_max - gray_in);
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
  float2 dim = float2(0.01f, 0.01f) * RENODX_DEBUG_MODE;

  if (pos.x >= box.x && pos.x <= box.x + dim.x && 
      pos.y >= box.y && pos.y <= box.y + dim.y) {
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

#define cmp -
float3 HueCorrectAP1(float3 incorrect_color_ap1, float3 correct_color_ap1, float hue_correct_strength = 0.5f) {
  float3 incorrect_color_bt709 = renodx::color::bt709::from::AP1(incorrect_color_ap1);
  float3 correct_color_bt709 = renodx::color::bt709::from::AP1(correct_color_ap1);

  float3 corrected_color_bt709 = renodx::color::correct::Hue(incorrect_color_bt709, correct_color_bt709, hue_correct_strength, 2u);
  float3 corrected_color_ap1 = renodx::color::ap1::from::BT709(corrected_color_bt709);
  return corrected_color_ap1;
}

float3 acesTonemap(float3 bt709, float peak_nits = 203.f, float diffuse_white_nits = 203.f) {
  float3 untonemapped_bt709 = bt709;// / 32.f;

  const float ACES_MIN = 0.0001f;
  float aces_min = ACES_MIN / diffuse_white_nits;
  float aces_max = (peak_nits / diffuse_white_nits);

  float3 untonemapped_ap0 = mul(renodx::color::BT709_TO_AP0_MAT, untonemapped_bt709);
  float3 untonemapped_rrt_ap1 = renodx::tonemap::aces::RRT(untonemapped_ap0);
  float3 tonemapped_ap1 = renodx::tonemap::aces::ODT(untonemapped_rrt_ap1, aces_min * 48.f, aces_max * 48.f, renodx::color::IDENTITY_MAT) / 48.f;

  float3 hue_corrected_ap1 = HueCorrectAP1(tonemapped_ap1, untonemapped_rrt_ap1);

  float3 blended_color_ap1 = lerp(hue_corrected_ap1, tonemapped_ap1, saturate(hue_corrected_ap1));

//  float3 blended_color_bt2020 = renodx::color::bt2020::from::AP1(blended_color_ap1);

  return renodx::color::bt709::from::AP1(blended_color_ap1);
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
    if (RENODX_PERCEPTUAL_BOOST_MODE == 0.f) {
        // OFF
        return color;
    } else if (RENODX_PERCEPTUAL_BOOST_MODE == 1.f) {
        float3 original_color = color;
        // Reinhard
        float mid_point = RENODX_PERCEPTUAL_BOOST_REINHARD_MIDPOINT;
        float y = renodx::color::y::from::BT709(color);
        float scale = ReinhardScalable(y, RENODX_PERCEPTUAL_BOOST_CHANNEL_MAX, 0.f, mid_point, mid_point);
        color *= min(10.f, renodx::math::DivideSafe(scale, y, 10.f));
        // Apply strength scaling
        color = lerp(original_color, color, boost_strength * RENODX_PERCEPTUAL_BOOST_REINHARD_STRENGTH);
        return color;
        } else if (RENODX_PERCEPTUAL_BOOST_MODE == 2.f) {
        // XY->PQ (inverse tonemapping)
        float3 xyz = renodx::color::XYZ::from::BT709(color.xyz);
        float mid_point = RENODX_PERCEPTUAL_BOOST_XYPQ_MIDPOINT;
        float new_grey = mid_point;
        if (xyz.y > 0.0000001f)
        {
            float3 newXYZ = sign(xyz) * renodx::color::pq::Decode(
                renodx::color::pq::Encode(abs(xyz), 10000.f) * (1.f + RENODX_PERCEPTUAL_BOOST_XYPQ_PARAM),
                10000.f);
            new_grey = renodx::color::pq::Decode(
                renodx::color::pq::Encode(new_grey, 10000.f) * (1.f + RENODX_PERCEPTUAL_BOOST_XYPQ_PARAM),
                10000.f).r;
            xyz = lerp(
                xyz * (newXYZ.y / xyz.y),
                newXYZ.xyz,
                RENODX_PERCEPTUAL_BOOST_XYPQ_COLOR);
        }
        color.xyz = lerp(color.xyz, renodx::color::bt709::from::XYZ(xyz) * (mid_point/ new_grey), RENODX_PERCEPTUAL_BOOST_XYPQ_STRENGTH * boost_strength);

        
        return color;
        } else if (RENODX_PERCEPTUAL_BOOST_MODE == 3.f) {
        // ICTCP-based inverse tonemapping
        float3 new_color;
        {
            float mid_point = RENODX_PERCEPTUAL_BOOST_ICTCP_MIDPOINT;
            float3 ictcp = renodx::color::ictcp::from::BT709(color.xyz * (10000.f / 10000.f));
            ictcp.x *= 1.f + RENODX_PERCEPTUAL_BOOST_ICTCP_PARAM;
            ictcp.yz *= lerp(1.f, 1.f + RENODX_PERCEPTUAL_BOOST_ICTCP_PARAM, RENODX_PERCEPTUAL_BOOST_ICTCP_COLOR);
            new_color = renodx::color::bt709::from::ICtCp(ictcp);

            float3 ictcp_grey = renodx::color::ictcp::from::BT709(mid_point * (10000.f / 10000.f));
            ictcp_grey.x *= 1.f + RENODX_PERCEPTUAL_BOOST_ICTCP_PARAM;
            ictcp_grey.yz *= lerp(1.f, 1.f + RENODX_PERCEPTUAL_BOOST_ICTCP_PARAM, RENODX_PERCEPTUAL_BOOST_ICTCP_COLOR);
            float3 new_grey = renodx::color::bt709::from::ICtCp(ictcp_grey);
            new_color *= (mid_point / new_grey);


        }


        color.xyz = lerp(color.xyz, new_color, RENODX_PERCEPTUAL_BOOST_ICTCP_STRENGTH * boost_strength);


        return color;
    }

    return color;
}

float3 ComputeUntonemappedGraded(float3 untonemapped, float3 graded_sdr_color, float3 neutral_sdr_color) {
  [branch]
  if (RENODX_COLOR_GRADE_STRENGTH == 0) {
    return untonemapped;
  } else {
    if (RENODX_COLOR_GRADE_PER_CHANNEL_BLOWOUT_RESTORATION != 0.f
        || RENODX_COLOR_GRADE_PER_CHANNEL_HUE_CORRECTION != 0.f
        || RENODX_COLOR_GRADE_PER_CHANNEL_CHROMINANCE_CORRECTION != 0.f) {
      graded_sdr_color = renodx::draw::ApplyPerChannelCorrection(
          untonemapped,
          graded_sdr_color,
          RENODX_COLOR_GRADE_PER_CHANNEL_BLOWOUT_RESTORATION,
          RENODX_COLOR_GRADE_PER_CHANNEL_HUE_CORRECTION,
          RENODX_COLOR_GRADE_PER_CHANNEL_CHROMINANCE_CORRECTION);
    }

    return renodx::tonemap::UpgradeToneMap(
        untonemapped,
        neutral_sdr_color,
        graded_sdr_color,
        RENODX_COLOR_GRADE_STRENGTH,
        RENODX_COLOR_GRADE_TONE_MAP_PASS_AUTOCORRECTION);
  }
}

float3 ApplyPerceptualBoostAndToneMap(float3 color, float scene_type = SCENE_TYPE_UNKNOWN) {

    color.xyz = renodx::color::srgb::DecodeSafe(color.xyz);
    color = ApplyReverseReinhard(color, scene_type);
    if (RENODX_PERCEPTUAL_BOOST_MODE > 0.f) {
  //      color = renodx::draw::ToneMapPass(color);
    }
    color.xyz = renodx::color::srgb::EncodeSafe(color.xyz);
    return color;
}

float3 ToneMapPassWrapper(float3 color) {
  //   color = (color > 0 | color < 0) ? color : 0.f;
    if (RENODX_ENABLE_UI_TONEMAPPASS > 0.f) {
      return color;
    }
    float gamma = RENODX_TONE_MAP_GAMMA * 2.f;
    color.xyz = sign(color.xyz) * pow(abs(color.xyz), 1.f / gamma);
    return renodx::draw::ToneMapPass(color);
}
/*
float3 ToneMapPassWrapper(float3 untonemapped, float3 graded_sdr_color) {
  if (RENODX_ENABLE_UI_TONEMAPPASS > 0.f) {
    return graded_sdr_color;
  }
  float3 color = renodx::draw::ToneMapPass(untonemapped, graded_sdr_color);
  float gamma = RENODX_TONE_MAP_GAMMA * 2.f;
  color.xyz = sign(color.xyz) * pow(abs(color.xyz), 1.f / gamma);
  return color;
}*/

float3 ToneMapPassWrapper(float3 untonemapped, float3 graded_sdr_color, float3 neutral_sdr_color) {
  float3 color = ComputeUntonemappedGraded(untonemapped, graded_sdr_color, neutral_sdr_color);
  if (RENODX_ENABLE_UI_TONEMAPPASS > 0.f) {
    return color;
  }
  float gamma = RENODX_TONE_MAP_GAMMA * 2.f;
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

float3 applyUserTonemap(float3 untonemapped, float userToneMapType) {
  float3 outputColor;
  if (userToneMapType == 0.f) {  // If vanilla is selected
    outputColor = saturate(untonemapped);
  } else {
    outputColor = untonemapped;
  }

  if (userToneMapType != 0) {  // UserColorGrading, pre-tonemap
    outputColor = renodx::color::grade::UserColorGrading(
        outputColor,
        RENODX_TONE_MAP_EXPOSURE,    // exposure
        RENODX_TONE_MAP_HIGHLIGHTS,  // highlights
        RENODX_TONE_MAP_SHADOWS,     // shadows
        RENODX_TONE_MAP_CONTRAST,    // contrast
        1.f,                                   // saturation, we'll do this post-tonemap
        0.f,                                   // dechroma, post tonemapping
        0.f);                                  // hue correction, Post tonemapping
  }

  // Start tonemapper if/else
  if (userToneMapType == 4.f) {  // DICE
    DICESettings DICEconfig = DefaultDICESettings();
    DICEconfig.Type = 3;
    DICEconfig.ShoulderStart = RENODX_DICE_SHOULDER;  // 0.33f;  // Start shoulder

    float dicePaperWhite = RENODX_GRAPHICS_WHITE_NITS / 80.f;
    float dicePeakWhite = RENODX_PEAK_WHITE_NITS / 80.f;

    outputColor = DICETonemap(outputColor * dicePaperWhite, dicePeakWhite, DICEconfig) / dicePaperWhite;
  } else if (userToneMapType == 5.f) {  // Frostbite
    float frostbitePeak = RENODX_PEAK_WHITE_NITS / RENODX_GRAPHICS_WHITE_NITS;
    outputColor = renodx::tonemap::frostbite::BT709(outputColor, frostbitePeak);
  } else if (userToneMapType == 6.f) {  // baby reinhard
    float ReinhardPeak = RENODX_PEAK_WHITE_NITS / RENODX_GRAPHICS_WHITE_NITS;
    outputColor = renodx::tonemap::ReinhardScalable(outputColor, ReinhardPeak);

  } 

  if (userToneMapType != 0) {  // UserColorGrading, post-tonemap
    outputColor = renodx::color::grade::UserColorGrading(
        outputColor,
        1.f,                                       // exposure
        1.f,                                       // highlights
        1.f,                                       // shadows
        1.f,                                       // contrast
        RENODX_TONE_MAP_SATURATION,      // saturation
        0.f,                                       // dechroma, we don't need it
        RENODX_TONE_MAP_HUE_CORRECTION,  // Hue Correction Strength
        renodx::tonemap::Reinhard(untonemapped));  // Hue Correction Type
  }

  outputColor = renodx::color::bt709::clamp::BT2020(outputColor);  // Clamp to BT2020 to avoid negative colors

  return outputColor;
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

float3 ToneMapPassCustom(float3 color, float ignore_tonemap_flag = 0.f) {
  // Apply gamma correction from ColorGradeGamma (0.0-1.0 mapped to gamma 1.0-2.2)
  if (RENODX_ENABLE_UI_TONEMAPPASS > 0.f && ignore_tonemap_flag == 0.f) {
    return color;
  }
  if (RENODX_TONE_MAP_GAMMA != 0.5f) {
    float gamma = RENODX_TONE_MAP_GAMMA * 2.f;
    color.xyz = sign(color.xyz) * pow(abs(color.xyz), 1.f / gamma);
  }
/*
  if (RENODX_DEBUG_MODE3 == 2.0f) {
    float3 untonemapped = color;
    float y = renodx::color::y::from::BT709(color);
    float3 neutral_sdr_color = lerp(untonemapped, renodx::tonemap::renodrt::NeutralSDR(color), saturate(y));
    color = acesTonemap(neutral_sdr_color);
    color = UpgradeToneMapCustom(untonemapped, neutral_sdr_color, color, shader_injection.color_grade_strength, 0.f);
  }*/
  if (RENODX_TONE_MAP_TYPE >= 4.f) {  //
    return applyUserTonemap(color, RENODX_TONE_MAP_TYPE);        // DICE
  }

  return renodx::draw::ToneMapPass(color);
}

float3 ToneMapPassCustom(float3 untonemapped, float3 graded_sdr_color, float3 neutral_sdr_color) {
  float3 color = ComputeUntonemappedGraded(untonemapped, graded_sdr_color, neutral_sdr_color);

  return ToneMapPassCustom(color); ;
}

float3 ToneMapPassCustom(float3 untonemapped, float3 graded_sdr_color) {
  float3 color = ComputeUntonemappedGraded(untonemapped, graded_sdr_color, renodx::tonemap::renodrt::NeutralSDR(untonemapped));

  return ToneMapPassCustom(color); ;
}

#define cmp -



#endif
