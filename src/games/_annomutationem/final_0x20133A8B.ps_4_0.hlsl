#include "./shared.h"

SamplerState BlitSampler_s : register(s0);
Texture2D<float4> BlitTexture : register(t0);

#define cmp -

void main(
    float2 v0: TEXCOORD0,
    float4 v1: SV_POSITION0,
    out float4 o0: SV_Target0) {
  o0.xyzw = BlitTexture.Sample(BlitSampler_s, v0.xy).xyzw;
  o0.xyz = renodx::draw::ToneMapPass(o0.xyz);
  o0.xyz = renodx::draw::RenderIntermediatePass(o0.xyz);
  o0.xyzw = renodx::draw::SwapChainPass(o0.xyzw);
  return;
}
