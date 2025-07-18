// ---- Created with 3Dmigoto v1.4.1 on Thu Jul 17 13:22:48 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb1 : register(b1)
{
  float4 cb1[5];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[15];
}
#include "./common.hlsl"

// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : TEXCOORD0,
  float4 v2 : TEXCOORD1,
  float4 v3 : TEXCOORD2,
  float4 v4 : TEXCOORD3,
  float4 v5 : TEXCOORD6,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.x = v2.w;
  r0.y = v3.w;
  r0.z = v4.w;
  r1.xyz = cb1[4].xyz + -r0.xyz;
  r0.xyz = -cb0[6].xyz + r0.xyz;
  r0.x = dot(r0.xyz, r0.xyz);
  r0.x = sqrt(r0.x);
  r0.y = dot(r1.xyz, r1.xyz);
  r0.y = rsqrt(r0.y);
  r0.yzw = r1.xyz * r0.yyy;
  r1.xyz = v3.xyz * r0.zzz;
  r1.xyz = v2.xyz * r0.yyy + r1.xyz;
  r0.yzw = v4.xyz * r0.www + r1.xyz;
  r0.y = dot(r0.yzw, r0.yzw);
  r0.y = rsqrt(r0.y);
  r0.y = saturate(r0.w * r0.y);
  r0.y = 1 + -r0.y;
  r0.z = 1 + -r0.y;
  r0.y = log2(r0.y);
  r0.y = cb0[11].y * r0.y;
  r0.y = exp2(r0.y);
  r1.xyz = cb0[12].xyz * r0.yyy;
  r0.y = log2(r0.z);
  r0.y = cb0[10].w * r0.y;
  r0.y = exp2(r0.y);
  r0.z = 2 + -cb0[11].x;
  r0.z = r0.z + -r0.y;
  r0.y = cb0[11].x + r0.y;
  r0.y = r0.y * r0.z;
  r2.xyzw = t0.Sample(s0_s, v1.xy).xyzw;
  r2.xyzw = cb0[14].xyzw * r2.xyzw;
  r0.z = cb0[4].w * r2.w;
  r0.z = cb0[10].z * r0.z;
  r0.y = r0.z * r0.y;
  r0.z = -cb0[9].x + r0.x;
  r0.x = saturate(cb0[9].w * r0.x);
  r0.z = saturate(cb0[10].x * r0.z);
  r0.xz = cb0[9].yz * r0.xz;
  r0.w = cb0[10].y * cb0[7].w;
  r0.z = -r0.w * r0.z + 1;
  r0.x = r0.w * r0.x;
  o0.w = r0.y * r0.z;
  r0.yzw = cb0[13].xyz * cb0[13].www;
  r0.yzw = r1.xyz * cb0[12].www + r0.yzw;
  r0.yzw = cb0[4].xyz * r2.xyz + r0.yzw;
  r1.xyz = cb0[8].xyz + -r0.yzw;
  o0.xyz = r0.xxx * r1.xyz + r0.yzw;
  o0.rgba = renodx_adjust_ui_color(o0.rgba);
  return;
}