#include "./shared.h"

#ifndef COMMON_H_
#define COMMON_H_


#define INV_REINHARD 2.f


float ReinhardScalable(float color, float channel_max = 1.f, float channel_min = 0.f, float gray_in = 0.18f, float gray_out = 0.18f) {
  /*float exposure = (channel_max * (channel_min * gray_out + channel_min - gray_out));
                   / max(gray_in * (gray_out - channel_max));

  float numerator = -channel_max * (channel_min * color + channel_min - color);
  float denominator = (exposure * (channel_max - color));
  return renodx::math::DivideSafe(numerator, denominator, renodx::math::FLT16_MAX); */

  float numerator = color * (channel_max - 0.18f);
  float denominator = max(.0001f, channel_max - color);
  return renodx::math::DivideSafe(numerator, denominator, 10.f);
}


// Function to apply reverse Reinhard tone mapping
float3 ApplyReverseReinhard(float3 color, float channel_max = INV_REINHARD) {
    if (RENODX_TONE_MAP_TYPE != 0) {
      // Use global toggle and channel_max from shader_injection
      if (shader_injection.perceptual_boost_mode == 0.f) {
          // OFF
          return color;
      } else if (shader_injection.perceptual_boost_mode == 1.f) {
          // Reinhard
          color.xyz = renodx::color::srgb::DecodeSafe(color.xyz);
          float y = renodx::color::y::from::BT709(color);
          float scale = ReinhardScalable(y, shader_injection.perceptual_boost_channel_max, 0.f, 0.18f, 0.18f);
          color *= min(10.f, renodx::math::DivideSafe(scale, y, 10.f));
          color.xyz = renodx::color::srgb::EncodeSafe(color.xyz);
          return color;
      } 
    }
    return color;
}

#endif
