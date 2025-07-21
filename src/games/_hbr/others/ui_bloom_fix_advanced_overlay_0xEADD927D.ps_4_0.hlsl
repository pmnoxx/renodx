
// ---- Created with 3Dmigoto v1.4.1 on Fri Jul 11 17:50:46 2025
Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[136];
}




// 3Dmigoto declarations
#define cmp -

#include "../common.hlsl"

void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xy = v1.xy * cb0[134].zw + cb0[134].xy;
  r0.xyzw = t1.SampleBias(s1_s, r0.xy, cb0[5].x).xyzw;
  r0.w = saturate(r0.w);
 // r0.w = renodx::color::y::from::BT709(r0.xyz);
  

  r1.xyzw = cb0[135].xyzw * r0.xyzw;
  r0.xyz = -r0.xyz * cb0[135].xyz + float3(1,1,1);
  r2.xyzw = t0.SampleBias(s0_s, v1.xy, cb0[5].x).xyzw;
  r2.w = saturate(r2.w);
 // r2.w = renodx::color::y::from::BT709(r2.xyz);

  r3.xyz = float3(1,1,1) + -r2.xyz;
  r3.xyz = r3.xyz + r3.xyz;
  r0.xyz = -r3.xyz * r0.xyz + float3(1,1,1);
  r3.xyz = r2.xyz + r2.xyz;
  r1.xyz = r3.xyz * r1.xyz + -r0.xyz;
  r0.w = r2.w * r1.w;
  r3.xyz = cmp(float3(0.5,0.5,0.5) >= r2.xyz);
  r3.xyz = r3.xyz ? float3(1,1,1) : 0;
  r0.xyz = r3.xyz * r1.xyz + r0.xyz;
  r0.xyz = r0.xyz + -r2.xyz;
  o0.xyz = r0.www * r0.xyz + r2.xyz;


  o0.xyz = lerp(t0.SampleBias(s0_s, v1.xy, cb0[5].x).xyz, o0.xyz, RENODX_2D_BLOOM);
//  r0 *= RENODX_2D_BLOOM;

  o0.w = 1;
  return;
}
