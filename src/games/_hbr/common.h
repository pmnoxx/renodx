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


// Helper function to get perceptual boost strength for a given scene type
float GetPerceptualBoostStrength(float scene_type) {
    if (scene_type == SCENE_TYPE_2D_CHARACTER) {
        return RENODX_PERCEPTUAL_BOOST_2D_CHARACTER;
    } else if (scene_type == SCENE_TYPE_2D_BACKGROND) {
        return RENODX_PERCEPTUAL_BOOST_2D_BACKGROUND;
    } else if (scene_type == SCENE_TYPE_3D) {
        return RENODX_PERCEPTUAL_BOOST_3D;
    }
    // For unknown scene types, default to 1.0
    return 1.f;
}

float4 debug_mode(float4 color, float2 pos) {
    if (pos.x < 0.01f * RENODX_DEBUG_MODE / 3.f && pos.y < 0.01f * RENODX_DEBUG_MODE && RENODX_DEBUG_MODE)
        return float4(1.f, 0.f, 0.f, 1.f);
    if (pos.x < 0.01f * RENODX_DEBUG_MODE * 2.f / 3.f && pos.y < 0.01f * RENODX_DEBUG_MODE && RENODX_DEBUG_MODE)
        return float4(0.f, 1.f, 0.f, 1.f);
    if (pos.x < 0.01f * RENODX_DEBUG_MODE * 3.f / 3.f && pos.y < 0.01f * RENODX_DEBUG_MODE && RENODX_DEBUG_MODE)
        return float4(0.f, 0.f, 1.f, 1.f);
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


float3 ApplyPerceptualBoostAndToneMap(float3 color, float scene_type = SCENE_TYPE_UNKNOWN) {

    color.xyz = renodx::color::srgb::DecodeSafe(color.xyz);
    color = ApplyReverseReinhard(color, scene_type);
    if (shader_injection.perceptual_boost_mode > 0.f) {
  //      color = renodx::draw::ToneMapPass(color);
    }
    color.xyz = renodx::color::srgb::EncodeSafe(color.xyz);
    return color;
}

float3 ToneMapPassWrapper(float3 color) {
 //   color = (color > 0 | color < 0) ? color : 0.f;
    if (RENODX_ENABLE_UI_TONEMAPPASS) {
        return color;
    }
    return renodx::draw::ToneMapPass(color);
}

float3 ToneMapPassWrapper(float3 untonemapped, float3 graded_sdr_color) {
    if (RENODX_ENABLE_UI_TONEMAPPASS) {
        return graded_sdr_color;
    }
    return renodx::draw::ToneMapPass(untonemapped, graded_sdr_color);
}

float3 ToneMapPassWrapper(float3 untonemapped, float3 graded_sdr_color, float3 neutral_sdr_color) {
    if (RENODX_ENABLE_UI_TONEMAPPASS) {
        return graded_sdr_color;
    }
    return renodx::draw::ToneMapPass(untonemapped, graded_sdr_color, neutral_sdr_color);
}

#define cmp -

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

float4 ToneMapBlock(float3 untonemapped, float r2_w, float cb42_x, Texture2D<float4> t4_tex, float scene_type_3d) {
  float3 r0_xyz_out = saturate(untonemapped);

  float3 sdrTonemapped = renodx::tonemap::renodrt::NeutralSDR(untonemapped);  // tonemap to SDR you can change this to any SDR tonemapper you want
  if (RENODX_TONE_MAP_TYPE != 0) {
    float y = renodx::color::y::from::BT709(untonemapped);
    r0_xyz_out = lerp(untonemapped, sdrTonemapped, saturate(y));
    if (RENODX_DEBUG_MODE3 < 0.5f) {
      r0_xyz_out = saturate(r0_xyz_out);
    }
    sdrTonemapped = r0_xyz_out;
  }

  float3 s = sign(r0_xyz_out);
  r0_xyz_out = abs(r0_xyz_out);

  r0_xyz_out = renodx::color::srgb::EncodeSafe(r0_xyz_out);
  r0_xyz_out = renodx::lut::SampleTetrahedral(t4_tex, r0_xyz_out);
  r0_xyz_out = renodx::color::srgb::DecodeSafe(r0_xyz_out);

  r0_xyz_out *= s;

  float4 result;
  float3 color = r0_xyz_out;
  if (RENODX_TONE_MAP_TYPE != 0) {
    float3 sdrGraded = r0_xyz_out;
    if (RENODX_DEBUG_MODE2 >= 0.5f) {
      color = UpgradeToneMapCustom(untonemapped, sdrTonemapped, sdrGraded, 1.f);
    } else {
      color = renodx::tonemap::UpgradeToneMap(untonemapped, sdrTonemapped, sdrGraded, 1.f);
    }
    color = ApplyReverseReinhard(color, scene_type_3d);
    color = ToneMapPassWrapper(color);  // all 3 colors are in LINEAR here
  }
 // result.w = dot(ungraded_sdr, float3(0.212672904, 0.715152204, 0.0721750036));
  float r0_w = cmp(0.5 < cb42_x);
  if (r0_w != 0) {
    float3 r1_xyz_sat = saturate(color);
    result.w = dot(r1_xyz_sat, float3(0.212672904, 0.715152204, 0.0721750036));
  } else {
    result.w = saturate(r2_w);
  }
  result.xyz = renodx::draw::RenderIntermediatePass(color);
  return result;
}

#endif