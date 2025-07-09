#include "./shared.h"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);
float4 main(float4 vpos: SV_POSITION, float2 uv: TEXCOORD0)
    : SV_TARGET {

  float4 linearColor = t0.Sample(s0, uv);

  bool applyEffect = true;
  if (shader_injection.effect_split_mode == 1) {
    // Left/Right
    applyEffect = (uv.x < shader_injection.effect_split_x);
  } else if (shader_injection.effect_split_mode == 2) {
    // Top/Bottom
    applyEffect = (uv.y < shader_injection.effect_split_x);
  } else if (shader_injection.effect_split_mode == 3) {
    // Diagonal \
    applyEffect = (uv.y < uv.x - (shader_injection.effect_split_x - 0.5f) * 2.0f);
  } else if (shader_injection.effect_split_mode == 4) {
    // Diagonal /
    applyEffect = (uv.y < (shader_injection.effect_split_x * 2.0f - uv.x));
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
