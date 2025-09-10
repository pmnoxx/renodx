// Custom game color grading/tone mapping helpers

#include "./common.hlsl"

#ifndef CUSTOM_H_
#define CUSTOM_H_

#define cmp -

// UNITY START
#define CUSTOM_USE_PQ_TONE_MAP_FOR_LUT 1.f

float3 UnityLookupPQ(float4 input_color,Texture2D<float4> lut) {
  const float3 bt2020_converted = max(0.f, renodx::color::bt2020::from::BT709(input_color.xyz));
  float3 lut_input = renodx::color::pq::Encode(bt2020_converted, 100.f);
  float3 lut_output = renodx::lut::SampleTetrahedral(lut, saturate(lut_input));
  return lut_output;
}

float3 UnityLookupARID(float4 input_color, float4 lut_param, SamplerState s0_s, Texture2D<float4> lut) {
  // incorrect code
  float4 r0 = float4(input_color.xyz, 0.f);
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
  return r0.xyz;
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

// Unity YRGB Curves LUT Builder - processes color through tone mapping and YRGB LUT system
float4 UnityYRGBCurvesLutBuilder(float3 input_color, SamplerState texSampler, 
                                Texture2D<float4> lut_y, Texture2D<float4> lut_r, 
                                Texture2D<float4> lut_g, Texture2D<float4> lut_b, float4 cb0_5) {
  float3 untonemappedSign = sign(input_color);
  float3 untonemapped = abs(input_color);
  float3 sdrTonemapped = saturate(untonemapped);
  float3 r0 = sdrTonemapped;

  r0.xyz = float3(0.00390625, 0.00390625, 0.00390625) + r0.xyz;  // equals to 1 / 256 = 0.00390625
  float r0_w = 0;

  float3 r1;
  r1.x = lut_y.SampleBias(texSampler, float2(r0.x, r0_w), cb0_5.x).x;
  r1.y = lut_y.SampleBias(texSampler, float2(r0.y, r0_w), cb0_5.x).x;
  r1.z = lut_y.SampleBias(texSampler, float2(r0.z, r0_w), cb0_5.x).x;

  r0.xyz = float3(0.00390625, 0.00390625, 0.00390625) + r1.xyz;
  r0_w = 0;

  float4 output;
  output.x = lut_r.SampleBias(texSampler, float2(r0.x, r0_w), cb0_5.x).x;
  output.y = lut_g.SampleBias(texSampler, float2(r0.y, r0_w), cb0_5.x).x;
  output.z = lut_b.SampleBias(texSampler, float2(r0.z, r0_w), cb0_5.x).x;
  output.w = 1;     

  float3 sdrGraded = output.xyz;
  float3 color;
  if (true) {
    color.r = sdrTonemapped.r < 1.f ? sdrGraded.r : sdrGraded.r * untonemapped.r;
    color.g = sdrTonemapped.g < 1.f ? sdrGraded.g : sdrGraded.g * untonemapped.g;
    color.b = sdrTonemapped.b < 1.f ? sdrGraded.b : sdrGraded.b * untonemapped.b;
  } else { // old tone mapping
    color.r = renodx::tonemap::UpgradeToneMap(untonemapped.r, sdrTonemapped.r, sdrGraded.r, 1.f).r;
    color.g = renodx::tonemap::UpgradeToneMap(untonemapped.g, sdrTonemapped.g, sdrGraded.g, 1.f).r;
    color.b = renodx::tonemap::UpgradeToneMap(untonemapped.b, sdrTonemapped.b, sdrGraded.b, 1.f).r;
  }
  output.rgb = color * untonemappedSign;
  return output;
}

// UNITY END

// Utility function to adjust UI color gamma and scale
float4 renodx_adjust_ui_color(float4 color) {
  if (RENODX_INTERMEDIATE_ENCODING == 1.f) {
    color.rgb = renodx::color::srgb::DecodeSafe(color.rgb);
  } else if (RENODX_INTERMEDIATE_ENCODING == 2.f) {
    color.rgb = renodx::color::gamma::DecodeSafe(color.rgb, 2.2f);
  }
//  color.rgb *= RENODX_GRAPHICS_WHITE_NITS / RENODX_DIFFUSE_WHITE_NITS;
  color.rgb *= RENODX_DIFFUSE_WHITE_NITS / RENODX_GRAPHICS_WHITE_NITS;
  if (RENODX_INTERMEDIATE_ENCODING == 1.f) {
    color.rgb = renodx::color::srgb::EncodeSafe(color.rgb);
  } else if (RENODX_INTERMEDIATE_ENCODING == 2.f) {
    color.rgb = renodx::color::gamma::EncodeSafe(color.rgb, 2.2f);
  }
  return color;
}




#endif