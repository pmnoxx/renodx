// Custom game color grading/tone mapping helpers

#include "./common.hlsl"

#ifndef CUSTOM_H_
#define CUSTOM_H_

#define cmp -

/*
float4 ToneMapBlock(float3 untonemapped, float r2_w, float cb42_x, Texture2D<float4> t4_tex, float scene_type_3d) {
  float3 r0_xyz_out = saturate(untonemapped);

  float3 sdrTonemapped = RestoreHighlightSaturation(untonemapped);

  float3 s = sign(r0_xyz_out);
  r0_xyz_out = lerp(abs(r0_xyz_out), saturate(r0_xyz_out), DISPLAY_MAP_SATURATION);

  r0_xyz_out = renodx::color::srgb::EncodeSafe(r0_xyz_out);
  r0_xyz_out = renodx::lut::SampleTetrahedral(t4_tex, r0_xyz_out);
  r0_xyz_out = renodx::color::srgb::DecodeSafe(r0_xyz_out);

  r0_xyz_out *= s;

  float4 result;
  float3 color = r0_xyz_out;
  if (RENODX_TONE_MAP_TYPE != 0) {
    float3 sdrGraded = r0_xyz_out;
    color = ComputeUntonemappedGraded(untonemapped, sdrGraded, sdrTonemapped);
    color = ApplyReverseReinhard(color, scene_type_3d);
    color = ToneMapPassWrapper(color);  // all 3 colors are in LINEAR here
  }
 // result.w = dot(ungraded_sdr, float3(0.212672904, 0.715152204, 0.0721750036));
  float r0_w = cmp(0.5 < cb42_x);
  if (r0_w != 0) {
    float3 r1_xyz_sat = saturate(color);
    result.w = dot(r1_xyz_sat, float3(0.212672904, 0.715152204, 0.0721750036));
  } else {
    result.w = saturate(r2_w);
  }
  result.xyz = renodx::draw::RenderIntermediatePass(color);
  return result;
} */

#endif
