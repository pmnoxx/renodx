
// Custom game color grading/tone mapping helpers

#include "./common.hlsl"

#ifndef CUSTOM_H_
#define CUSTOM_H_

#define cmp -

float4 renodx_opening_tonemap_block(float4 untonemapped, float4 aces_output, float2 v1, Texture2D<float4> t2, Texture2D<float4> t3, float t3_intensity, bool useToneMapPass) {

    float4 r1 = 0;

    float4 r0 = aces_output;
    if (RENODX_DEBUG_MODE3 > 0.5f) {
        float3 sdrTonemapped_first = renodx::tonemap::renodrt::NeutralSDR(untonemapped.rgb);  // tonemap to SDR you can change this to any SDR tonemapper you want
        if (RENODX_TONE_MAP_TYPE != 0) {
          float y = renodx::color::y::from::BT709(untonemapped.rgb);
          sdrTonemapped_first.xyz = lerp(untonemapped.xyz, sdrTonemapped_first.xyz, saturate(y));
        }

        float3 color = ComputeUntonemappedGraded(untonemapped.rgb, r0.rgb, sdrTonemapped_first);
        r0.xyz = color;
        untonemapped.xyz = color;

        if (t3_intensity != 0) {
          float3 sdrTonemapped = renodx::tonemap::renodrt::NeutralSDR(untonemapped.rgb);  // tonemap to SDR you can change this to any SDR tonemapper you want
          if (RENODX_TONE_MAP_TYPE != 0) {
            float y = renodx::color::y::from::BT709(untonemapped.rgb);
            r0.xyz = saturate(lerp(untonemapped.rgb, sdrTonemapped, saturate(y)));
              sdrTonemapped = r0.xyz;
            }

            r0.xyz = renodx::color::srgb::EncodeSafe(r0.xyz);
            r0.xyz = lerp(r0.xyz, renodx::lut::SampleTetrahedral(t3, r0.xyz), t3_intensity);  
            r0.xyz = renodx::color::srgb::DecodeSafe(r0.xyz);

            // preserved brightness changes in case t3 lut changes brightness
            r0.xyz = ComputeUntonemappedGraded(untonemapped.rgb, r0.xyz, sdrTonemapped);
        }

        r0.xyz = max(0.f, renodx::color::bt2020::from::BT709(r0.xyz));
        r0.xyz = renodx::color::pq::Encode(r0.xyz, 100.f);
        r0.xyz = renodx::lut::SampleTetrahedral(t2, r0.xyz);
    } else {
        if (t3_intensity != 0) {
            untonemapped = debug_mode(untonemapped, v1, 0.02f);

            r0.xyz = renodx::color::srgb::EncodeSafe(r0.xyz);
            r0.xyz = lerp(r0.xyz, renodx::lut::SampleTetrahedral(t3, r0.xyz), t3_intensity);  
            r0.xyz = renodx::color::srgb::DecodeSafe(r0.xyz);
        }
        float3 color = ComputeUntonemappedGraded(untonemapped.rgb, r0.rgb, renodx::tonemap::renodrt::NeutralSDR(untonemapped.rgb));
        r0.xyz = color;

        r0.xyz = max(0.f, renodx::color::bt2020::from::BT709(r0.xyz));
        r0.xyz = renodx::color::pq::Encode(r0.xyz, 100.f);
        r0.xyz = renodx::lut::SampleTetrahedral(t2, r0.xyz);

    }

    if (RENODX_ENABLE_UI_TONEMAPPASS == 0.f) { 
        if (useToneMapPass) {
            r0.rgb = renodx::draw::ToneMapPass(r0.rgb);
        }
    }
    r0.w = 1;
    r0.xyz = renodx::draw::RenderIntermediatePass(r0.xyz);
    return r0;
}

float4 renodx_opening_tonemap_block(float4 r0, float2 v1, Texture2D<float4> t2, Texture2D<float4> t3, float t3_intensity, bool useToneMapPass) {

    float4 r1 = 0;
    if (t3_intensity != 0) {
      r0 = debug_mode(r0, v1, 0.02f);
      float3 untonemapped = r0.xyz;
      float3 sdrTonemapped = renodx::tonemap::renodrt::NeutralSDR(untonemapped);  // tonemap to SDR you can change this to any SDR tonemapper you want
      if (RENODX_TONE_MAP_TYPE != 0) {
        float y = renodx::color::y::from::BT709(untonemapped);
        r0.xyz = saturate(lerp(untonemapped, sdrTonemapped, saturate(y)));
        sdrTonemapped = r0.xyz;
      }
      r0.xyz = renodx::color::srgb::EncodeSafe(r0.xyz);
      r0.xyz = lerp(r0.xyz, renodx::lut::SampleTetrahedral(t3, r0.xyz), t3_intensity);  
      r0.xyz = renodx::color::srgb::DecodeSafe(r0.xyz);

      float3 sdrGraded = r0.xyz;
      float3 color = ComputeUntonemappedGraded(untonemapped, sdrGraded, sdrTonemapped);
      r0.xyz = color;      
    }
    r0.xyz = max(0.f, renodx::color::bt2020::from::BT709(r0.xyz));
    r0.xyz = renodx::color::pq::Encode(r0.xyz, 100.f);
    r0.xyz = renodx::lut::SampleTetrahedral(t2, r0.xyz);

    if (RENODX_ENABLE_UI_TONEMAPPASS == 0.f) { 
        if (useToneMapPass) {
            r0.rgb = renodx::draw::ToneMapPass(r0.rgb);
        }
    }
    r0.w = 1;
    r0.xyz = renodx::draw::RenderIntermediatePass(r0.xyz);
    return r0;
}

#endif