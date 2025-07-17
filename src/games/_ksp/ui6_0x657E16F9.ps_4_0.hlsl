// ---- Created with 3Dmigoto v1.4.1 on Thu Jul 17 13:09:23 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb1 : register(b1)
{
  float4 cb1[12];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[7];
}

#include "./shared.h"

// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : TEXCOORD0,
  float3 v2 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.x = dot(v2.xyz, v2.xyz);
  r0.x = rsqrt(r0.x);
  r0.xyz = v2.xyz * r0.xxx;
  r1.x = cb1[9].z;
  r1.y = cb1[10].z;
  r1.z = cb1[11].z;
  r0.x = dot(r0.xyz, r1.xyz);
  r0.y = cb0[3].y * r0.x;
  r0.y = log2(r0.y);
  r0.y = cb0[3].x * r0.y;
  r0.y = exp2(r0.y);
  r0.y = min(1, r0.y);
  r0.xy = float2(1,1) + -r0.xy;
  r0.zw = v1.xy * cb0[2].xy + cb0[2].zw;
  r1.xyzw = t0.Sample(s0_s, r0.zw).xyzw;
  r2.xyz = cb0[5].xyz + -r1.xyz;
  r0.yzw = r0.yyy * r2.xyz + r1.xyz;
  r1.x = cb0[4].w * r1.w;
  o0.xyz = cb0[4].xyz * r0.yzw;
  r0.y = cmp(r0.x >= cb0[3].z);
  r0.x = cmp(cb0[3].z >= r0.x);
  r0.yz = r0.yx ? float2(1,1) : 0;
  r0.y = r0.y * r0.z;
  r0.x = r0.x ? 0 : r0.y;
  r0.x = r0.z + r0.x;
  r0.x = r1.x * r0.x;
  o0.w = cb0[6].x * r0.x;
  // o0.xyz *= RENODX_GRAPHICS_WHITE_NITS / RENODX_DIFFUSE_WHITE_NITS;
  o0.rgb = renodx_adjust_ui_color(o0.rgb);
  return;
}