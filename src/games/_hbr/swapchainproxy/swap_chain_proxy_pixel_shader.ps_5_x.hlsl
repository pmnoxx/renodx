#include "../shared.h"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);
float4 main(float4 vpos: SV_POSITION, float2 uv: TEXCOORD0)
    : SV_TARGET {
  float4 o0 = t0.Sample(s0, uv);


  if (RENODX_ENABLE_UI_TONEMAPPASS > 0.f) {
    o0.xyz = renodx::draw::InvertIntermediatePass(o0.xyz);

    o0.xyz = renodx::draw::ToneMapPass(o0.xyz);  // game applies post effects to UI, which exceed peak nits.

    o0.xyz = renodx::draw::RenderIntermediatePass(o0.xyz);
  }

  return renodx::draw::SwapChainPass(o0);
}
