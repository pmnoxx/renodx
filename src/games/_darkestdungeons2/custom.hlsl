// Custom game color grading/tone mapping helpers

#include "./common.hlsl"

#ifndef CUSTOM_HLSL_H
#define CUSTOM_HLSL_H

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

float3 ProcessBT2020PQWithLUT(float3 color, Texture2D<float4> lut_texture, Texture2D<float4> grading_lut, float grading_intensity) {
  float3 lut_input;

  if (CUSTOM_USE_PQ_TONE_MAP_FOR_LUT == 1.f) {
    const float3 bt2020_converted = max(0.f, renodx::color::bt2020::from::BT709(color));
    lut_input = renodx::color::pq::Encode(bt2020_converted, 100.f);
  } else {
    float4 r0 = float4(color, 0.f);
    r0.xyz = r0.xyz * float3(5.55555582, 5.55555582, 5.55555582) + float3(0.0479959995, 0.0479959995, 0.0479959995);
    r0.xyz = max(float3(0, 0, 0), r0.xyz);
    r0.xyz = log2(r0.xyz);
    r0.xyz = saturate(r0.xyz * float3(0.0734997839, 0.0734997839, 0.0734997839) + float3(0.386036009, 0.386036009, 0.386036009));
    lut_input = r0.yzw;
  }
  float3 lut_result = renodx::lut::SampleTetrahedral(lut_texture, saturate(lut_input));
  
  return ProcessLUTWithUntonemappedGrading(lut_result, grading_lut, grading_intensity);
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
