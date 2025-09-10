#ifndef COMMON_H_
#define COMMON_H_

#include "./shared.h"
#include "./DICE.hlsl"

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



float3 applyUserTonemap(float3 untonemapped, float2 v1) {
    float3 outputColor;
    untonemapped = debug_mode(float4(untonemapped, 1.f), v1).xyz;
    if (shader_injection.userToneMapType == 0.f) {  // If vanilla is selected
      outputColor = saturate(untonemapped);
    } else {
      outputColor = untonemapped;
    }

    if (shader_injection.userToneMapType == 5.f) {  // DefaultToneMapper
        return renodx::draw::ToneMapPass(untonemapped);
    }
  
    if (shader_injection.userToneMapType != 0) {  // UserColorGrading, pre-tonemap
      outputColor = renodx::color::grade::UserColorGrading(
          outputColor,
          shader_injection.tone_map_exposure,    // exposure
          shader_injection.tone_map_highlights,  // highlights
          shader_injection.tone_map_shadows,     // shadows
          shader_injection.tone_map_contrast,    // contrast
          1.f,                                // saturation, we'll do this post-tonemap
          0.f,                                // dechroma, post tonemapping
          0.f);                               // hue correction, Post tonemapping
    }
  
    // Start tonemapper if/else
    if (shader_injection.userToneMapType == 2.f) {  // DICE
      DICESettings DICEconfig = DefaultDICESettings();
      DICEconfig.Type = 3;
      DICEconfig.ShoulderStart = 0.33f;  // Start shoulder
  
      float dicePaperWhite = shader_injection.graphics_white_nits / 80.f;
      float dicePeakWhite = shader_injection.peak_white_nits / 80.f;
  
      outputColor = DICETonemap(outputColor * dicePaperWhite, dicePeakWhite, DICEconfig) / dicePaperWhite;
    } else if (shader_injection.userToneMapType == 3.f) {  // baby reinhard
      float ReinhardPeak = shader_injection.peak_white_nits / shader_injection.graphics_white_nits;
      outputColor = renodx::tonemap::ReinhardScalable(outputColor, ReinhardPeak);
  
    } else if (shader_injection.userToneMapType == 4.f) {  // Frostbite
      float frostbitePeak = shader_injection.peak_white_nits / shader_injection.graphics_white_nits;
      outputColor = renodx::tonemap::frostbite::BT709(outputColor, frostbitePeak);
    } 
  
    if (shader_injection.userToneMapType != 0) {  // UserColorGrading, post-tonemap
      outputColor = renodx::color::grade::UserColorGrading(
          outputColor,
          1.f,                                       // exposure
          1.f,                                       // highlights
          1.f,                                       // shadows
          1.f,                                       // contrast
          shader_injection.tone_map_saturation,         // saturation
          0.f,                                       // dechroma, we don't need it
          shader_injection.tone_map_hue_correction,         // Hue Correction Strength
          renodx::tonemap::Reinhard(untonemapped));  // Hue Correction Type
    }
  
    outputColor = renodx::color::bt709::clamp::BT2020(outputColor);  // Clamp to BT2020 to avoid negative colors
  
    return outputColor;
  }

#endif