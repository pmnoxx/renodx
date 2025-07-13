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
    if (pos.x < 0.01f && pos.y < 0.01f && RENODX_DEBUG_MODE)
        return float4(0.f, 1.f, 0.f, 1.f);
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
    color = renodx::draw::ToneMapPass(color);
    color.xyz = renodx::color::srgb::EncodeSafe(color.xyz);
    return color;
}

// Wrapper for ToneMapPass calls - disabled by default
float3 ToneMapPassWrapper(float3 color) {
    #ifdef ENABLE_TONE_MAP_PASS
        return renodx::draw::ToneMapPass(color);
    #else
        return color;
    #endif
}

float3 ToneMapPassWrapper(float3 untonemapped, float3 graded_sdr_color) {
    #ifdef ENABLE_TONE_MAP_PASS
        return renodx::draw::ToneMapPass(untonemapped, graded_sdr_color);
    #else
        return graded_sdr_color;
    #endif
}

float3 ToneMapPassWrapper(float3 untonemapped, float3 graded_sdr_color, float3 neutral_sdr_color) {
    #ifdef ENABLE_TONE_MAP_PASS
        return renodx::draw::ToneMapPass(untonemapped, graded_sdr_color, neutral_sdr_color);
    #else
        return graded_sdr_color;
    #endif
}
#endif