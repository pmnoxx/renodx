
// Custom game color grading/tone mapping helpers

#include "./common.hlsl"

#ifndef CUSTOM_H_
#define CUSTOM_H_

#define cmp -

float4 CustomToneMapBlock(float3 color, Texture2D<float4> t1, SamplerState s0_s) {
/*  float3 untonemappedSign = sign(color.xyz);
  float3 untonemapped = abs(color.xyz);
  float3 sdrTonemapped = RestoreHighlightSaturation(untonemapped);
*/
  const float3 bt2020_converted = max(0.f, renodx::color::bt2020::from::BT709(color));
  color = renodx::color::pq::Encode(bt2020_converted, 1.f);

  float3 result = renodx::lut::SampleTetrahedral(t1, color);
/*  float3 graded = color;
  float3 result = ComputeUntonemappedGraded(untonemapped, graded, sdrTonemapped);
  result.xyz *= untonemappedSign;
*/
  result = ApplyReverseReinhard(result, SCENE_TYPE_3D);
  result = ToneMapPassWrapper(result);
  float3 finalColor = renodx::draw::RenderIntermediatePass(result);
  return float4(finalColor, 1);
}

#endif