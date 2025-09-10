#include "../common.hlsl"

// ---- Created with 3Dmigoto v1.4.1 on Tue Jul  1 21:19:49 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[132];
}



// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  o0.xyzw = t0.SampleLevel(s0_s, v1.xy, cb0[131].x).xyzw;

  
 // o0.xyz = renodx::color::srgb::DecodeSafe(o0.xyz);

 // o0.xyz = ApplyReverseReinhard(o0.xyz);

 // o0.xyz = renodx::color::srgb::EncodeSafe(o0.xyz);
  

  o0.w = saturate(o0.w);
  return;
}
