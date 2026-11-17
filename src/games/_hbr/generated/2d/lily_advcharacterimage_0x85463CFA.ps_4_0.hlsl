#include "../../common.hlsl"
// ---- Created with 3Dmigoto v1.4.1 on Tue Nov 17 09:33:35 2026
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[5];
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
  float4 r0,r1;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xy = cmp(v3.xy >= cb0[4].xy);
  r0.zw = cmp(cb0[4].zw >= v3.xy);
  r0.xyzw = r0.xyzw ? float4(1,1,1,1) : 0;
  r0.xy = r0.xy * r0.zw;
  r0.x = r0.x * r0.y;
  r1.xyzw = t0.Sample(s0_s, v2.xy).xyzw;
  r1.xyzw = cb0[3].xyzw + r1.xyzw;
  r1.xyzw = v1.xyzw * r1.xyzw;
  o0.w = r1.w * r0.x;
  o0.xyz = r1.xyz;
  {
  o0.w = saturate(o0.w);
  if (RENODX_TONE_MAP_TYPE >= 1.f) {
    o0.xyz = ApplyPerceptualBoostAndToneMap(o0.xyz, SCENE_TYPE_2D_CHARACTER);
  }
    return;
  }
}