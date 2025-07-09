#include "./shared.h"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);
float4 main(float4 vpos: SV_POSITION, float2 uv: TEXCOORD0)
    : SV_TARGET {

  float4 linearColor = t0.Sample(s0, uv);
  if (RENODX_TONE_MAP_TYPE > 0.f) {
    if (RENODX_PIXEL_SHADER_DECODE_MODE == 1.f) {
      linearColor.xyz = pow(linearColor.xyz, 2.2f);
    } else if (RENODX_PIXEL_SHADER_DECODE_MODE == 2.f) {
      linearColor.xyz = renodx::color::srgb::DecodeSafe(linearColor.xyz);
    }
  }

  linearColor.xyz = renodx_ksp_apply_tonemap_and_boost(linearColor.xyz);
  
  linearColor.xyz *= RENODX_DIFFUSE_WHITE_NITS / RENODX_GRAPHICS_WHITE_NITS;
  return renodx::draw::SwapChainPass(linearColor);
}
