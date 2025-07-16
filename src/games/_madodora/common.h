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


float4 debug_mode(float4 color, float2 pos) {
    if (1.f - pos.x < 0.01f * RENODX_DEBUG_MODE / 3.f && 1.f - pos.y < 0.01f * RENODX_DEBUG_MODE && RENODX_DEBUG_MODE)
        return float4(1.f, 0.f, 0.f, 1.f);
    if (1.f - pos.x < 0.01f * RENODX_DEBUG_MODE * 2.f / 3.f && 1.f - pos.y < 0.01f * RENODX_DEBUG_MODE && RENODX_DEBUG_MODE)
        return float4(0.f, 1.f, 0.f, 1.f);
    if (1.f - pos.x < 0.01f * RENODX_DEBUG_MODE * 3.f / 3.f && 1.f - pos.y < 0.01f * RENODX_DEBUG_MODE && RENODX_DEBUG_MODE)
        return float4(0.f, 0.f, 1.f, 1.f);
    return color;
}
  


float3 ToneMapPassWrapper(float3 color) {
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
#endif