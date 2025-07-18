#include "./common.h"

// ---- Created with 3Dmigoto v1.4.1 on Mon Jul  7 17:53:31 2025

SamplerState BlitSampler_s : register(s0);
Texture2D<float4> BlitTexture : register(t0);


// 3Dmigoto declarations
#define cmp -

void main(
    float2 v0: TEXCOORD0,
    float4 v1: SV_POSITION0,
    out float4 o0: SV_Target0)
{
  o0.xyzw = BlitTexture.Sample(BlitSampler_s, v0.xy).xyzw;
  //o0.xyz = (3.f, 3.f, 3.f);

  if (RENODX_ENABLE_UI_TONEMAPPASS > 0.f) {
    o0.xyz = renodx::draw::InvertIntermediatePass(o0.xyz);


    o0.xyz = renodx::draw::ToneMapPass(o0.xyz); // game applies post effects to UI, which exceed peak nits.
    
    o0.xyz = renodx::draw::RenderIntermediatePass(o0.xyz);
  }



  return;
}