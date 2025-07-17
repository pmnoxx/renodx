// ---- Created with 3Dmigoto v1.4.1 on Thu Jul 17 13:32:57 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[7];
}

#include "./shared.h"

// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  float4 v2 : TEXCOORD0,
  float4 v3 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyz = float3(1,1,1) + -v1.xyz;
  r0.xyz = r0.xyz + r0.xyz;
  r1.xyzw = t0.Sample(s0_s, v2.xy).xyzw;
  r1.xyzw = cb0[3].xyzw + r1.xyzw;
  r0.w = dot(r1.xyz, float3(0.300000012,0.589999974,0.109999999));
  r1.w = v1.w * r1.w;
  r2.x = -r0.w * cb0[6].x + 1;
  r1.xyz = -r0.xyz * r2.xxx + float3(1,1,1);
  r2.w = r1.w;
  r0.xyz = v1.xyz * r0.www;
  r0.w = cmp(0.5 < r0.w);
  r2.xyz = r0.xyz + r0.xyz;
  r0.xyzw = r0.wwww ? r1.xyzw : r2.xyzw;
  r1.xy = cmp(v3.xy >= cb0[5].xy);
  r1.zw = cmp(cb0[5].zw >= v3.xy);
  r1.xyzw = r1.xyzw ? float4(1,1,1,1) : 0;
  r1.xy = r1.xy * r1.zw;
  r1.x = r1.x * r1.y;
  r1.xyzw = r1.xxxx * r0.xyzw;
  r0.xyzw = cb0[4].xxxx ? r1.xyzw : r0.xyzw;
  r1.x = -0.00100000005 + r0.w;
  o0.xyzw = r0.xyzw;
  r0.x = cmp(r1.x < 0);
  r0.y = cmp(asint(cb0[6].y) != 0);
  r0.x = r0.y ? r0.x : 0;
  o0.rgb = renodx_adjust_ui_color(o0.rgb);
  if (r0.x != 0) discard;
  return;
}