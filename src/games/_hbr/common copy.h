#include "./shared.h"

#ifndef COMMON_H_
#define COMMON_H_


#define INV_REINHARD 2.f

float3 PumboAutoHDR(float3 SDRColor, float _PeakWhiteNits, float _PaperWhiteNits, float ShoulderPow = 2.75f) {  // ShoulderPow = 2.75f default
  const float SDRRatio = max(renodx::color::y::from::BT709(SDRColor), 0.f);
  // Limit AutoHDR brightness, it won't look good beyond a certain level.
  // The paper white multiplier is applied later so we account for that.
  const float AutoHDRMaxWhite = max(min(_PeakWhiteNits, 700) / _PaperWhiteNits, 1.f);
  const float AutoHDRShoulderRatio = 1.f - max(1.f - SDRRatio, 0.f);
  const float AutoHDRExtraRatio = pow(max(AutoHDRShoulderRatio, 0.f), ShoulderPow) * (AutoHDRMaxWhite - 1.f);
  const float AutoHDRTotalRatio = SDRRatio + AutoHDRExtraRatio;
  return SDRColor * renodx::math::SafeDivision(AutoHDRTotalRatio, SDRRatio, 1);  // Fallback on a value of 1 in case of division by 0
}

float ReinhardScalable(float color, float channel_max = 1.f, float channel_min = 0.f, float gray_in = 0.18f, float gray_out = 0.18f) {
  float exposure = (channel_max * (channel_min * gray_out + channel_min - gray_out))
                   / (gray_in * (gray_out - channel_max));

  float numerator = -channel_max * (channel_min * color + channel_min - color);
  float denominator = (exposure * (channel_max - color));
  return renodx::math::DivideSafe(numerator, denominator, renodx::math::FLT16_MAX);
}


// Function to apply reverse Reinhard tone mapping
float3 ApplyReverseReinhard(float3 color, float channel_max = INV_REINHARD) {



  color.xyz = renodx::color::srgb::DecodeSafe(color.xyz);
    float y =  renodx::color::y::from::BT709(color);

    if (true) {
        float scale = ReinhardScalable(y, channel_max, 0.f, 0.18f, 0.18f);
        color *= renodx::math::DivideSafe(scale, y, renodx::math::FLT16_MAX);
    } else {

        color = PumboAutoHDR(color, 1000.f, 203.f);
    }


 color.xyz = renodx::color::srgb::EncodeSafe(color.xyz);

  return color;
}

#endif
