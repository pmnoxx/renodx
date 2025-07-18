#ifndef COMMON_H_
#define COMMON_H_

#include "./DICE.hlsl"
#include "./shared.h"

float3 applyUserTonemap(float3 untonemapped, float2 v1, float userToneMapType) {
  float3 outputColor;
// .. untonemapped = debug_mode(float4(untonemapped, 1.f), v1).xyz;
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

float3 renodx_ksp_apply_tonemap_and_boost(float3 linearColor)
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
        if (shader_injection.tone_map_type == 4.f) {  // DICE
          linearColor = applyUserTonemap(linearColor, float2(0.f, 0.f), 2.f);
        } else if (shader_injection.tone_map_type == 5.f) {  // Frostbite
          linearColor = applyUserTonemap(linearColor, float2(0.f, 0.f), 4.f);
        } else {
          linearColor.xyz = renodx::draw::ToneMapPass(linearColor.xyz);
        }
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
#endif