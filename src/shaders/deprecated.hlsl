#ifndef SRC_SHADERS_DEPRECATED_HLSL_
#define SRC_SHADERS_DEPRECATED_HLSL_

#include "./color.hlsl"
#include "./math.hlsl"
#include "./tonemap.hlsl"

namespace renodx {
namespace color {
namespace srgb {
namespace from {
/// @deprecated
float BT709(float channel) {
  return renodx::color::srgb::Encode(channel);
}
/// @deprecated
float3 BT709(float3 color) {
  return renodx::color::srgb::Encode(color);
}
/// @deprecated - Use Encode
float4 BT709(float4 color) {
  return renodx::color::srgb::Encode(color);
}
}  // namespace from
}  // namespace srgb

namespace bt709 {
namespace from {
/// @deprecated
float SRGB(float channel) {
  return renodx::color::srgb::Decode(channel);
}
/// @deprecated
float3 SRGB(float3 color) {
  return renodx::color::srgb::Decode(color);
}
/// @deprecated - Use Encode
float4 SRGB(float4 color) {
  return renodx::color::srgb::Decode(color);
}
/// @deprecated - Use Encode
float4 SRGBA(float4 color) {
  return renodx::color::srgba::Decode(color);
}
}
}

namespace srgba {
namespace from {
/// @deprecated - Use Decode
float4 BT709(float4 color) {
  return renodx::color::srgba::Decode(color);
}
}  // namespace from
}  // namespace srgba

namespace pq {
namespace from {
/// @deprecated - Use pq::Encode
float3 BT2020(float3 bt2020_color, float scaling = 10000.f) {
  return Encode(bt2020_color, scaling);
}
}  // namespace from
}  // namespace pq

namespace bt2020 {
namespace from {
/// @deprecated - Use pq::Decode
float3 PQ(float3 pq_color, float scaling = 10000.f) {
  return pq::Decode(pq_color, scaling);
}
}  // namespace from
}  // namespace bt2020

}  // namespace color

namespace math {

#define POWSAFE_FUNCTION_GENERATOR(struct)           \
  /* @deprecated */                                  \
  /* Does opiniated SignPow instead of proper pow */ \
  struct PowSafe(struct x, float exponent) {         \
    return SignPow(x, exponent);                     \
  }

POWSAFE_FUNCTION_GENERATOR(float);
POWSAFE_FUNCTION_GENERATOR(float2);
POWSAFE_FUNCTION_GENERATOR(float3);
POWSAFE_FUNCTION_GENERATOR(float4);
#undef POWSAFE_FUNCTION_GENERATOR

#define SQRTSAFE_FUNCTION_GENERATOR(struct) \
  struct SqrtSafe(struct x) {               \
    return SignSqrt(x);                     \
  }

SQRTSAFE_FUNCTION_GENERATOR(float);
SQRTSAFE_FUNCTION_GENERATOR(float2);
SQRTSAFE_FUNCTION_GENERATOR(float3);
SQRTSAFE_FUNCTION_GENERATOR(float4);
#undef SQRTSAFE_FUNCTION_GENERATOR

/// @deprecated
/// Does opiniated SignPow instead of proper pow
float3 SafePow(float3 color, float exponent) {
  return PowSafe(color, exponent);
}

/// @deprecated
/// Does opiniated SignPow instead of proper pow
float SafePow(float color, float exponent) {
  return PowSafe(color, exponent);
}

/// @deprecated - DivideSafe
float SafeDivision(float quotient, float dividend) {
  return (dividend == 0.f)
             ? FLT_MAX * Sign(quotient)
             : (quotient / dividend);
}

/// @deprecated - Use DivideSafe
float SafeDivision(float quotient, float dividend, float fallback) {
  return (dividend == 0.f)
             ? fallback
             : (quotient / dividend);
}

/// @deprecated - Use DivideSafe
float3 SafeDivision(float3 quotient, float3 dividend) {
  return float3(SafeDivision(quotient.x, dividend.x, FLT_MAX * Sign(quotient.x)),
                SafeDivision(quotient.y, dividend.y, FLT_MAX * Sign(quotient.y)),
                SafeDivision(quotient.z, dividend.z, FLT_MAX * Sign(quotient.z)));
}

/// @deprecated - Use DivideSafe
float3 SafeDivision(float3 quotient, float3 dividend, float3 fallback) {
  return float3(SafeDivision(quotient.x, dividend.x, fallback.x),
                SafeDivision(quotient.y, dividend.y, fallback.y),
                SafeDivision(quotient.z, dividend.z, fallback.z));
}

}  // namespace math

namespace tonemap {
namespace config {

float3 ApplyRenoDRT(float3 color, Config tm_config, bool is_sdr) {
  if (is_sdr) {
    tm_config.gamma_correction = 0;
    tm_config.gamma_correction_custom = 0;
    tm_config.peak_nits = 100.f;
    tm_config.game_nits = 100.f;
  }
  return ApplyRenoDRT(color, tm_config);
}

float3 ApplyACES(float3 color, Config tm_config, bool is_sdr) {
  if (is_sdr) {
    tm_config.gamma_correction = 0;
    tm_config.gamma_correction_custom = 0;
    tm_config.peak_nits = 100.f;
    tm_config.game_nits = 100.f;
  }
  return ApplyACES(color, tm_config);
}

}  // namespace config
}  // namespace tonemap
}  // namespace renodx

#endif  // SRC_SHADERS_DEPRECATED_HLSL_