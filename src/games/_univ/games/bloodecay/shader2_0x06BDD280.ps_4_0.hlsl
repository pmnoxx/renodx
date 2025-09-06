#include "../../custom.hlsl"

// ---- Created with 3Dmigoto v1.4.1 on Wed Sep  3 17:38:11 2025
Texture2D<float4> t4 : register(t4);

Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[1407];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t0.Sample(s0_s, v1.xy).xyzw;
  r1.xyz = float3(0.0773993805,0.0773993805,0.0773993805) * r0.xyz;
  r2.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r0.xyz;
  r2.xyz = float3(0.947867334,0.947867334,0.947867334) * r2.xyz;
  r2.xyz = log2(abs(r2.xyz));
  r2.xyz = float3(2.4000001,2.4000001,2.4000001) * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r0.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r0.xyz);
  r0.xyz = r0.xyz ? r1.xyz : r2.xyz;
  r1.xyzw = t1.Sample(s0_s, v1.xy).xyzw;
  r1.xyz = r1.xyz * r1.xyz;
  r0.w = cmp(0 < cb0[1399].x);
  if (r0.w != 0) {
    r2.xyz = r1.xyz * r1.www;
    r1.xyz = float3(8,8,8) * r2.xyz;
  }
  r1.xyz = cb0[1398].xxx * r1.xyz;
  r0.xyz = r1.xyz * cb0[1398].yzw + r0.xyz;
  r2.xy = v1.xy * cb0[1400].xy + cb0[1400].zw;
  r2.xyzw = t2.Sample(s0_s, r2.xy).xyzw;
  r2.xyz = cb0[1401].xxx * r2.xyz;
  r0.xyz = r2.xyz * r1.xyz + r0.xyz;
  r0.w = cmp(0 < cb0[1406].z);
  if (r0.w != 0) {
    r1.xy = -cb0[1406].xy + v1.xy;
    r1.yz = cb0[1406].zz * abs(r1.xy);
    r1.x = cb0[1405].w * r1.y;
    r0.w = dot(r1.xz, r1.xz);
    r0.w = 1 + -r0.w;
    r0.w = max(0, r0.w);
    r0.w = log2(r0.w);
    r0.w = cb0[1406].w * r0.w;
    r0.w = exp2(r0.w);
    r1.xyz = float3(1,1,1) + -cb0[1405].xyz;
    r1.xyz = r0.www * r1.xyz + cb0[1405].xyz;
    r0.xyz = r1.xyz * r0.xyz;
  }
  //r0.xyz = saturate(cb0[1396].www * r0.xyz);
  r0.xyz = (cb0[1396].www * r0.xyz);

  float y = 0.2126*r0.x + 0.7152*r0.y + 0.0722*r0.z;
  float3 untonemapped = r0.xyz;

  r0.w = cmp(0 < cb0[1397].w);
  if (r0.w != 0) {
    r1.xyz = float3(12.9200001,12.9200001,12.9200001) * r0.xyz;
    r2.xyz = log2(r0.xyz);
    r2.xyz = float3(0.416666657,0.416666657,0.416666657) * r2.xyz;
    r2.xyz = exp2(r2.xyz);
    r2.xyz = r2.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
    r3.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.xyz);
    r1.xyz = r3.xyz ? r1.xyz : r2.xyz;
    r2.xyz = cb0[1397].zzz * r1.zxy;
    r0.w = floor(r2.x);
    r2.xw = float2(0.5,0.5) * cb0[1397].xy;
    r2.yz = r2.yz * cb0[1397].xy + r2.xw;
    r2.x = r0.w * cb0[1397].y + r2.y;
    r3.xyzw = t4.SampleLevel(s0_s, r2.xz, 0).xyzw;
    r4.x = cb0[1397].y;
    r4.y = 0;
    r2.xy = r4.xy + r2.xz;
    r2.xyzw = t4.SampleLevel(s0_s, r2.xy, 0).xyzw;
    r0.w = r1.z * cb0[1397].z + -r0.w;
    r2.xyz = r2.xyz + -r3.xyz;
    r2.xyz = r0.www * r2.xyz + r3.xyz;
    r2.xyz = r2.xyz + -r1.xyz;
    r1.xyz = cb0[1397].www * r2.xyz + r1.xyz;
    r2.xyz = float3(0.0773993805,0.0773993805,0.0773993805) * r1.xyz;
    r3.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r1.xyz;
    r3.xyz = float3(0.947867334,0.947867334,0.947867334) * r3.xyz;
    r3.xyz = log2(abs(r3.xyz));
    r3.xyz = float3(2.4000001,2.4000001,2.4000001) * r3.xyz;
    r3.xyz = exp2(r3.xyz);
    r1.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r1.xyz);
    r0.xyz = r1.xyz ? r2.xyz : r3.xyz;
  }
  r0.xyw = cb0[1396].zzz * r0.xyz;

  r0.w = floor(r0.w);
  r1.xy = float2(0.5,0.5) * cb0[1396].xy;
  r1.yz = r0.xy * cb0[1396].xy + r1.xy;
  r1.x = r0.w * cb0[1396].y + r1.y;
  r2.xyzw = t3.SampleLevel(s0_s, r1.xz, 0).xyzw;
  r0.x = cb0[1396].y;
  r0.y = 0;
  r0.xy = r1.xz + r0.xy;
  r1.xyzw = t3.SampleLevel(s0_s, r0.xy, 0).xyzw;
  r0.x = r0.z * cb0[1396].z + -r0.w;
  r0.yzw = r1.xyz + -r2.xyz;
  r0.xyz = r0.xxx * r0.yzw + r2.xyz;

  float y2 = 0.2126*r0.x + 0.7152*r0.y + 0.0722*r0.z;
  if (y > y2 && y2 >= 0.9) {
    // r0.xyz *= y / y2;
  }
  if (RENODX_TONE_MAP_TYPE > 0.f) {
    r0.xyz = ToneMapPassCustom(untonemapped.xyz, r0.xyz, RestoreHighlightSaturation(untonemapped));
  }

  o0.xyz = renodx::color::srgb::EncodeSafe(r0.xyz);
/*
  r1.xyz = float3(12.9200001,12.9200001,12.9200001) * r0.xyz;
  r2.xyz = log2(abs(r0.xyz));
  r2.xyz = float3(0.416666657,0.416666657,0.416666657) * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r2.xyz = r2.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
  r0.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.xyz);
  o0.xyz = r0.xyz ? r1.xyz : r2.xyz;

*/


  o0.w = 1;
  return;
}