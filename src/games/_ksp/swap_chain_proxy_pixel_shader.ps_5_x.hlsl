#include "./shared.h"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);
float4 main(float4 vpos: SV_POSITION, float2 uv: TEXCOORD0)
    : SV_TARGET {

  float4 linearColor = t0.Sample(s0, uv);

  // Convert angle to radians
  float angle_rad = radians(shader_injection.effect_split_angle);

  // Compute the split line normal
  float2 normal = float2(cos(angle_rad), sin(angle_rad));

  // Vector from center to current pixel
  float2 from_center = float2(uv.x - 0.5, uv.y - 0.5);

  // Side of the line: dot > 0 is one side, < 0 is the other
  bool applyEffect = true;
  if (shader_injection.effect_split_mode != 0) {
      applyEffect = dot(from_center, normal) < shader_injection.effect_split_x;
  }

  if (applyEffect) {
    if (RENODX_TONE_MAP_TYPE > 0.f) {
      if (RENODX_PIXEL_SHADER_DECODE_MODE == 1.f) {
        linearColor.xyz = pow(linearColor.xyz, 2.2f);
      } else if (RENODX_PIXEL_SHADER_DECODE_MODE == 2.f) {
        linearColor.xyz = renodx::color::srgb::DecodeSafe(linearColor.xyz);
      }
    }

    linearColor.xyz = renodx_ksp_apply_tonemap_and_boost(linearColor.xyz);
  }
  linearColor.xyz *= RENODX_DIFFUSE_WHITE_NITS / RENODX_GRAPHICS_WHITE_NITS;

  return renodx::draw::SwapChainPass(linearColor);
}
