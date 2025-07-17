#ifndef COMMON_H_
#define COMMON_H_

#include "./shared.h"

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

float4 renodx_opening_tonemap_block(float4 r0, float2 v1, Texture2D<float4> t2, Texture2D<float4> t3, float use_t3, bool useToneMapPass) {

    float4 r1 = 0;
    r1.w = use_t3;
    if (r1.w != 0) {
      r0 = debug_mode(r0, v1, 0.02f);
      float3 untonemapped = r0.xyz;
      float3 sdrTonemapped = renodx::tonemap::renodrt::NeutralSDR(untonemapped);  // tonemap to SDR you can change this to any SDR tonemapper you want
      if (RENODX_TONE_MAP_TYPE != 0) {
        float y = renodx::color::y::from::BT709(untonemapped);
        r0.xyz = saturate(lerp(untonemapped, sdrTonemapped, saturate(y)));
        sdrTonemapped = r0.xyz;
      }
      r0.xyz = renodx::color::srgb::EncodeSafe(r0.xyz);
      r0.xyz = renodx::lut::SampleTetrahedral(t3, r0.xyz);  // 16x16 grey texture, is this used for black-white effect?
      r0.xyz = renodx::color::srgb::DecodeSafe(r0.xyz);
        
      float3 sdrGraded = r0.xyz;
      float3 color = renodx::tonemap::UpgradeToneMap(untonemapped, sdrTonemapped, sdrGraded, 1.f);
      r0.xyz = lerp(untonemapped, color, RENODX_LUT_T3_ENABLE);      
    }
    r0.xyz = max(0.f, renodx::color::bt2020::from::BT709(r0.xyz));
    r0.xyz = renodx::color::pq::Encode(r0.xyz, 100.f);
    r0.xyz = renodx::lut::SampleTetrahedral(t2, r0.xyz);

    if (useToneMapPass) {
        r0.rgb = renodx::draw::ToneMapPass(r0.rgb);
    }
    r0.w = 1;
    r0.xyz = renodx::draw::RenderIntermediatePass(r0.xyz);
    return r0;
}

#endif