// ---- Created with 3Dmigoto v1.4.1 on Tue Jul  1 17:29:13 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb1 : register(b1)
{
  float4 cb1[2];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[160];
}




// 3Dmigoto declarations
#define cmp -

#include "./common.h"

void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : TEXCOORD0,
  float4 v2 : TEXCOORD5,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xy = v2.xy * cb1[0].xy + cb1[0].zw;
  r0.xyzw = t0.Sample(s0_s, r0.xy).xyzw;

  r0.xyz = ApplyReverseReinhard(r0.xyz, 1.0f);

  r0.xyzw = cb1[1].xyzw * r0.xyzw;
  r1.xyz = cb0[154].xyz * cb0[149].xyz;
  r2.xyz = cb0[159].xyz + cb0[155].xyz;
  o0.xyz = r0.xyz * r1.xyz + r2.xyz;
  o0.w = r0.w;
  return;
}