// Custom game color grading/tone mapping helpers

#include "./common.hlsl"

#ifndef CUSTOM_H_
#define CUSTOM_H_

#define cmp -

float3 ProcessLUTWithUntonemappedGrading(float3 untonemapped, Texture2D<float4> lut_texture, float lut_intensity) {
  float3 untonemappedSign = sign(untonemapped);
  float3 untonemappedAbs = abs(untonemapped);
  float3 sdrTonemapped = RestoreHighlightSaturation(untonemapped);
  
  float3 encoded = renodx::color::srgb::EncodeSafe(sdrTonemapped);
  float3 lutResult = renodx::lut::SampleTetrahedral(lut_texture, encoded);
  float3 decoded = renodx::color::srgb::DecodeSafe(lutResult);

  float3 graded = ComputeUntonemappedGraded(untonemappedAbs, decoded, sdrTonemapped);

  float3 result = lerp(untonemappedAbs, graded, lut_intensity);
  result = untonemappedSign * result;
  
  return result;
}

float4 unity_color_grade_user(float4 color, float2 uv, Texture2D<float4> user_lut, float t3_intensity) {
  float4 r1 = 0;
  if (t3_intensity != 0) {
    color = debug_mode(color, uv, 0.02f);
    float3 untonemapped = color.xyz;
    float3 sdrTonemapped = renodx::tonemap::renodrt::NeutralSDR(untonemapped);  // tonemap to SDR you can change this to any SDR tonemapper you want
    if (RENODX_TONE_MAP_TYPE != 0) {
      float y = renodx::color::y::from::BT709(untonemapped);
      color.xyz = saturate(lerp(untonemapped, sdrTonemapped, saturate(y)));
      sdrTonemapped = color.xyz;
    }
    color.xyz = renodx::color::srgb::EncodeSafe(color.xyz);
    color.xyz = lerp(color.xyz, renodx::lut::SampleTetrahedral(user_lut, color.xyz), t3_intensity);
    color.xyz = renodx::color::srgb::DecodeSafe(color.xyz);

    float3 sdrGraded = color.xyz;
    float3 color = ComputeUntonemappedGraded(untonemapped, sdrGraded, sdrTonemapped);
    color.xyz = color;
  }

  if (RENODX_ENABLE_UI_TONEMAPPASS == 0.f) {
    color.rgb = renodx::draw::ToneMapPass(color.rgb);
  }
  color.w = 1;
  color.xyz = renodx::draw::RenderIntermediatePass(color.xyz);
  return color;
}

#endif
