#include "./shared.h"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);
float4 main(float4 vpos: SV_POSITION, float2 uv: TEXCOORD0)
    : SV_TARGET {

  float4 linearColor = t0.Sample(s0, uv);
#if defined(RENODX_PIXEL_SHADER_DECODE_MODE)
  if (RENODX_PIXEL_SHADER_DECODE_MODE == 1.f) {
    linearColor.xyz = pow(linearColor.xyz, 2.2f);
  }
#endif

  linearColor.xyz = renodx_ksp_apply_tonemap_and_boost(linearColor.xyz);
  
  linearColor.xyz *= RENODX_DIFFUSE_WHITE_NITS / RENODX_GRAPHICS_WHITE_NITS;
  return renodx::draw::SwapChainPass(linearColor);
}
