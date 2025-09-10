// Custom game color grading/tone mapping helpers

#include "./common.hlsl"

#ifndef CUSTOM_H_
#define CUSTOM_H_

// Utility function to adjust UI color gamma and scale
float4 renodx_adjust_ui_color(float4 color) {
  if (RENODX_INTERMEDIATE_ENCODING == 1.f) {
    color.rgb = renodx::color::srgb::DecodeSafe(color.rgb);
  } else if (RENODX_INTERMEDIATE_ENCODING == 2.f) {
    color.rgb = renodx::color::gamma::DecodeSafe(color.rgb, 2.2f);
  }
  color.rgb *= RENODX_GRAPHICS_WHITE_NITS / RENODX_DIFFUSE_WHITE_NITS;
  if (RENODX_INTERMEDIATE_ENCODING == 1.f) {
    color.rgb = renodx::color::srgb::EncodeSafe(color.rgb);
  } else if (RENODX_INTERMEDIATE_ENCODING == 2.f) {
    color.rgb = renodx::color::gamma::EncodeSafe(color.rgb, 2.2f);
  }
  return color;
}

#endif