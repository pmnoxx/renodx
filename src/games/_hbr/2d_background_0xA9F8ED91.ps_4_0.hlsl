#include "./common.h"

// ---- Created with 3Dmigoto v1.4.1 on Tue Jul  1 15:57:23 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);


// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  float4 v2 : TEXCOORD0,
  float4 v3 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  float4 r0;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t0.Sample(s0_s, v2.xy).xyzw;


 // r0.xyz = renodx::color::srgb::DecodeSafe(r0.xyz);


  if (RENODX_TONE_MAP_TYPE >= 1.f) {
    r0.xyz = ApplyReverseReinhard(r0.xyz, 2.f);
    r0.xyz = renodx::draw::ToneMapPass(r0.xyz);
  }
  
 // r0.xyz = renodx::color::srgb::EncodeSafe(r0.xyz);
  
  o0.xyz = v1.xyz * r0.xyz;
  o0.w = v1.w;


  o0.w = saturate(o0.w);
  return;
}