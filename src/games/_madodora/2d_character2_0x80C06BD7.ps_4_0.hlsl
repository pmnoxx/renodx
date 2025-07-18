#include "./common.h"

// ---- Created with 3Dmigoto v1.4.1 on Wed Jul 16 12:54:29 2025
Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb1 : register(b1)
{
  float4 cb1[7];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[13];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  float2 v2 : TEXCOORD0,
  float2 w2 : TEXCOORD3,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4,r5;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t0.Sample(s1_s, v2.xy).xyzw;
  r1.xyz = cb0[9].xyz * r0.xyz;
  r2.xyz = r0.xyz * cb0[9].xyz + cb0[10].xyz;
  r0.xyz = -r1.xyz * cb0[10].xyz + r2.xyz;
  r1.xyzw = v1.xyzw * r0.xyzw;
  r0.w = cmp(cb0[2].x != 0.000000);
  if (r0.w != 0) {
    r2.xy = v0.xy / cb1[6].xy;
    r0.w = -cb1[6].y / cb1[6].x;
    r3.x = 0.00332999998 * r0.w;
    r3.y = 0.00332999998;
    r2.zw = saturate(r3.xy + r2.xy);
    r3.xyzw = t1.Sample(s0_s, r2.xy).xyzw;
    r4.xyzw = t1.Sample(s0_s, r2.zw).xyzw;
    r0.w = -r4.x + r3.x;
    r0.w = cmp(r0.w >= 0.5);
    r0.w = r0.w ? 1.000000 : 0;
    r2.xz = cmp(cb0[2].xx == float2(1,2));
    r3.xyz = -cb0[5].xyz + cb0[4].xyz;
    r3.xyz = r2.yyy * r3.xyz + cb0[5].xyz;
    r4.xyz = r2.xxx ? cb0[3].xyz : 0;
    r3.xyz = r2.xxx ? r3.xyz : 0;
    r5.xyz = -cb0[8].xyz + cb0[7].xyz;
    r2.xyw = r2.yyy * r5.xyz + cb0[8].xyz;
    r4.xyz = r2.zzz ? cb0[6].xyz : r4.xyz;
    r2.xyz = r2.zzz ? r2.xyw : r3.xyz;
    r0.xyz = -r0.xyz * v1.xyz + float3(1,1,1);
    r0.xyz = r0.xyz + r0.xyz;
    r3.xyz = float3(1,1,1) + -r2.xyz;
    r0.xyz = -r0.xyz * r3.xyz + float3(1,1,1);
    r3.xyz = r2.xyz * r1.xyz;
    r3.xyz = r3.xyz + r3.xyz;
    r2.xyz = cmp(float3(0.5,0.5,0.5) >= r2.xyz);
    r5.xyz = r2.xyz ? float3(1,1,1) : 0;
    r2.xyz = r2.xyz ? float3(0,0,0) : float3(1,1,1);
    r0.xyz = r2.xyz * r0.xyz;
    r0.xyz = r3.xyz * r5.xyz + r0.xyz;
    r2.xyz = float3(1,1,1) + -r4.xyz;
    r3.xyz = float3(1,1,1) + -r0.xyz;
    r2.xyz = -r2.xyz * r3.xyz + float3(1,1,1);
    r2.xyz = r2.xyz + -r0.xyz;
    r1.xyz = r0.www * r2.xyz + r0.xyz;
  }
  r1.xyz = r1.xyz * r1.www;
  r0.xy = cb0[12].yz * cb0[12].ww;
  r0.xy = cmp(w2.xy >= r0.xy);
  r0.xy = r0.xy ? float2(1,1) : 0;
  r0.x = r0.x * r0.y;
  r0.yz = cb0[12].yz * cb0[12].ww + cb0[12].ww;
  r0.yz = cmp(r0.yz >= w2.xy);
  r0.yz = r0.yz ? float2(1,1) : 0;
  r0.x = r0.x * r0.y;
  r0.x = r0.x * r0.z;
  r2.xyzw = cmp(cb0[12].xxxx == float4(0,1,2,3));
  r2.xyzw = r2.xyzw ? float4(1,1,1,1) : 0;
  r0.xyzw = r2.xyzw * r0.xxxx;
  r2.xyzw = t2.Sample(s2_s, w2.xy).xyzw;
  r0.xy = r2.xy * r0.xy;
  r0.x = r0.x + r0.y;
  r0.x = r2.z * r0.z + r0.x;
  r0.x = r2.w * r0.w + r0.x;
  r0.xyzw = r1.xyzw * r0.xxxx;
  o0.xyzw = cb0[11].xxxx * r0.xyzw;

  o0.w = saturate(o0.w);
  return;
}