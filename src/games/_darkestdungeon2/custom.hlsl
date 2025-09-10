// Custom game color grading/tone mapping helpers

#include "./common.hlsl"

#ifndef CUSTOM_HLSL_H
#define CUSTOM_HLSL_H

#define cmp -

float3 UnityLookupARIDPQ(float4 input_color, float2 texcoord, float4 lut_param, SamplerState s0_s, Texture2D<float4> lut) {
  float4 debug_output = debug_mode(input_color, texcoord, 0.02f);
  float3 lut_output;
  if (CUSTOM_USE_PQ_TONE_MAP_FOR_LUT == 1.f) {
    const float3 bt2020_converted = max(0.f, renodx::color::bt2020::from::BT709(debug_output.xyz));
    float3 lut_input = renodx::color::pq::Encode(bt2020_converted, 100.f);
    lut_output = renodx::lut::SampleTetrahedral(lut, saturate(lut_input));
  } else {
    // incorrect code
    float4 r0 = float4(debug_output.xyz, 0.f);
    float4 r1, r2, r3;
    r0.xyz = lut_param.www * r0.zxy;
    r0.xyz = r0.xyz * float3(5.55555582, 5.55555582, 5.55555582) + float3(0.0479959995, 0.0479959995, 0.0479959995);
    r0.xyz = max(float3(0, 0, 0), r0.xyz);
    r0.xyz = log2(r0.xyz);
    r0.xyz = saturate(r0.xyz * float3(0.0734997839, 0.0734997839, 0.0734997839) + float3(0.386036009, 0.386036009, 0.386036009));
    r0.yzw = lut_param.zzz * r0.xyz;

    r0.y = floor(r0.y);
    r1.xy = float2(0.5, 0.5) * lut_param.xy;
    r1.yz = r0.zw * lut_param.xy + r1.xy;
    r1.x = r0.y * lut_param.y + r1.y;

    // lut query
    r2.xyzw = lut.SampleLevel(s0_s, r1.xz, 0).xyzw;
    r3.x = lut_param.y;
    r3.y = 0;
    r0.zw = r3.xy + r1.xz;
    r1.xyzw = lut.SampleLevel(s0_s, r0.zw, 0).xyzw;
    r0.x = r0.x * lut_param.z + -r0.y;
    r0.yzw = r1.xyz + -r2.xyz;
    r0.xyz = r0.xxx * r0.yzw + r2.xyz;
  }
  return lut_output;
}

float3 UnityLookupSDRsRGB(float3 input_color, float2 texcoord, float lut_strength2, Texture2D<float4> lut_2) {
  float3 untonemappedSign = sign(input_color);
  float3 untonemappedAbs = abs(input_color);
  float3 sdrTonemapped = RestoreHighlightSaturation(input_color);
  
  float3 encoded = renodx::color::srgb::EncodeSafe(sdrTonemapped);
  float3 lutResult = renodx::lut::SampleTetrahedral(lut_2, encoded);
  float3 decoded = renodx::color::srgb::DecodeSafe(lutResult);

  float3 graded = ComputeUntonemappedGraded(untonemappedAbs, decoded, sdrTonemapped);

  float3 result = lerp(untonemappedAbs, graded, lut_strength2);
  result = untonemappedSign * result;
  
  return result;
}

#endif
