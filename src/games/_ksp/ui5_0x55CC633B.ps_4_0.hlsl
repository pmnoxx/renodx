// ---- Created with 3Dmigoto v1.4.1 on Thu Jul 17 13:09:23 2025
Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[2];
}
#include "./common.hlsl"

// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  float2 v2 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t1.Sample(s1_s, v2.xy).xyzw;
  r1.xyzw = t0.Sample(s0_s, v2.xy).xyzw;
  r0.x = -r1.w + r0.x;
  r1.w = cb0[1].z * r0.x + r1.w;
  r0.xyzw = v1.xyzw * r1.xyzw;
  o0.xyz = r0.xyz * r0.www;
  o0.w = r0.w;
  // o0.xyz *= RENODX_GRAPHICS_WHITE_NITS / RENODX_DIFFUSE_WHITE_NITS;
  o0.rgba = renodx_adjust_ui_color(o0.rgba);
  return;
}