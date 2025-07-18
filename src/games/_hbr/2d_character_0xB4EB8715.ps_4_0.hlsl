#include "./common.h"

// ---- Created with 3Dmigoto v1.4.1 on Tue Jul  1 16:04:16 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[4];
}



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
  r0.w = saturate(r0.w);


  if (RENODX_TONE_MAP_TYPE >= 1.f) {
    r0.xyz = ApplyPerceptualBoostAndToneMap(r0.xyz, SCENE_TYPE_2D_CHARACTER);
  }


  r0.xyzw = cb0[3].xyzw + r0.xyzw;
  o0.xyzw = v1.xyzw * r0.xyzw;
  // saturate fix
  o0.w = saturate(o0.w);
  return;
}