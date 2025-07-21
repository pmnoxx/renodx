// Custom game color grading/tone mapping helpers

#include "./common.hlsl"

#ifndef CUSTOM_H_
#define CUSTOM_H_

#define cmp -

float3 UpgradeToneMapCustom(
    float3 color_untonemapped,
    float3 color_tonemapped,
    float3 color_tonemapped_graded,
    float post_process_strength = 1.f,
    float auto_correction = 0.f) {
  float ratio = 1.f;

  float y_untonemapped = renodx::color::y::from::BT709(color_untonemapped);
  float y_tonemapped = renodx::color::y::from::BT709(color_tonemapped);
  float y_tonemapped_graded = renodx::color::y::from::BT709(color_tonemapped_graded);

  if (y_untonemapped < y_tonemapped) {
    // If substracting (user contrast or paperwhite) scale down instead
    // Should only apply on mismatched HDR
    ratio = y_untonemapped / y_tonemapped;
  } else {
    // float y_delta = y_untonemapped - y_tonemapped;
    float y_delta = y_untonemapped / y_tonemapped;
    // y_delta = max(0, y_delta);  // Cleans up NaN
    y_delta = max(1, y_delta);  // Cleans up NaN
                                // const float y_new = y_tonemapped_graded + y_delta;
    const float y_new = y_tonemapped_graded * y_delta;

    const bool y_valid = (y_tonemapped_graded > 0);  // Cleans up NaN and ignore black
    ratio = y_valid ? (y_new / y_tonemapped_graded) : 0;
  }
  float auto_correct_ratio = lerp(1.f, ratio, saturate(y_untonemapped));
  ratio = lerp(ratio, auto_correct_ratio, auto_correction);

  float3 color_scaled = color_tonemapped_graded * ratio;
  // Match hue
  color_scaled = renodx::color::correct::Hue(color_scaled, color_tonemapped_graded);
  return lerp(color_untonemapped, color_scaled, post_process_strength);
}

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
    if (RENODX_DEBUG_MODE2 >= 0.5f) {
      color = ComputeUntonemappedGraded(untonemapped, sdrTonemapped, sdrGraded);
    } else {
      color = renodx::tonemap::UpgradeToneMap(untonemapped, sdrTonemapped, sdrGraded, shader_injection.color_grade_strength);
    }
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
} 

#endif