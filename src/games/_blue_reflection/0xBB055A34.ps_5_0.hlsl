#include "shared.h" 

// ---- Created with 3Dmigoto v1.4.1 on Fri Jun 27 11:44:42 2025

SamplerState smplScene_s : register(s0);
Texture2D<float4> smplScene_Tex : register(t0);


// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_Position0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  o0.xyzw = smplScene_Tex.Sample(smplScene_s, v1.xy).xyzw;

  o0.xyz = min(4000.f / 203.f, max(-4000.f / 203.f, o0.xyz));

/*
  o0.xyz = log2(o0.xyz);
  o0.xyz = (2.2f) * o0.xyz;
  o0.xyz = exp2(o0.xyz);
*/
  o0.xyz *= RENODX_GRAPHICS_WHITE_NITS / 203.f;

/*
  o0.xyz = log2(o0.xyz);
  o0.xyz = (1/2.2f) * o0.xyz;
  o0.xyz = exp2(o0.xyz);*/

  //
  
  o0.xyz = max(0.f, o0.xyz);
  o0.xyz = renodx::color::bt2020::from::BT709(o0.xyz);
  o0.xyz = max(0.f, min(1.f, renodx::color::pq::Encode(o0.xyz, 203.f)));
  o0.w = 1.f;

  return;
}