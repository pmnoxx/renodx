#include "../shared.h"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);
float4 main(float4 vpos: SV_POSITION, float2 uv: TEXCOORD0)
    : SV_TARGET {
  float4 color = t0.Sample(s0, uv);
  if (RENODX_ENABLE_UI_TONEMAPPASS > 0.f) {
    color.xyz = renodx::draw::InvertIntermediatePass(color.xyz);

    color.xyz = renodx::draw::ToneMapPass(color.xyz);  // game applies post effects to UI, which exceed peak nits.

    color.xyz = renodx::draw::RenderIntermediatePass(color.xyz);
  }
  return renodx::draw::SwapChainPass(color);
}
