
// Custom game color grading/tone mapping helpers

#include "./common.hlsl"

#ifndef CUSTOM_H_
#define CUSTOM_H_

#define cmp -
/*
float4 CustomToneMapBlock(float3 color, Texture2D<float4> t1, SamplerState s0_s, float2 uv) {
  color = debug_mode(float4(color, 1.f), uv).rgb;

  const float3 bt2020_converted = max(0.f, renodx::color::bt2020::from::BT709(color));
  color = renodx::color::pq::Encode(bt2020_converted, 1.f);

  float3 result = renodx::lut::SampleTetrahedral(t1, color);
  result = ApplyReverseReinhard(result, SCENE_TYPE_3D);
  result = ToneMapPassWrapper(result);
  float3 finalColor = renodx::draw::RenderIntermediatePass(result);
  return float4(finalColor, 1);
}*/


// unity color grading with custom_lut + lutbuilderldr
float3 custom_color_grading(float3 untonemapped, float2 v1, Texture2D<float4> t2, Texture2D<float4> t3, float t3_intensity, bool useToneMapPass) {
  untonemapped = debug_mode(float4(untonemapped.rgb, 1.f), v1).rgb;

  float3 r0 = untonemapped;
  if (t3_intensity != 0) {
    float3 s = sign(r0.xyz);
    r0.xyz = abs(r0.xyz);

    float3 sdrTonemapped = RestoreHighlightSaturation(untonemapped.rgb);

    r0.xyz = renodx::color::srgb::EncodeSafe(r0.xyz);
    r0.xyz = lerp(r0.xyz, renodx::lut::SampleTetrahedral(t3, r0.xyz), t3_intensity);
    r0.xyz = renodx::color::srgb::DecodeSafe(r0.xyz);

    r0.xyz = ComputeUntonemappedGraded(untonemapped.rgb, r0.xyz, sdrTonemapped);

    r0.xyz *= s; // WCG restoration
  }

  r0.xyz = max(0.f, renodx::color::bt2020::from::BT709(r0.xyz));
  r0.xyz = renodx::color::pq::Encode(r0.xyz, 100.f);
  r0.xyz = renodx::lut::SampleTetrahedral(t2, r0.xyz);


  if (RENODX_ENABLE_UI_TONEMAPPASS == 0.f) {
    if (useToneMapPass) {
      r0.rgb = ToneMapPassCustom(r0.rgb);
    }
  }
  r0.xyz = renodx::draw::RenderIntermediatePass(r0.xyz);
  return r0;
}

#endif