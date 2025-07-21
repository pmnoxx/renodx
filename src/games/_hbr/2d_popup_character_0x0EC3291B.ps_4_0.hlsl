#include "./common.hlsl"


// ---- Created with 3Dmigoto v1.4.1 on Tue Jul  1 20:00:11 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb1 : register(b1)
{
  float4 cb1[1];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[8];
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
  float4 r0,r1,r2;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.x = 1 / cb0[7].x;
  r0.y = v2.y / r0.x;
  r0.y = trunc(r0.y);
  r0.y = r0.y * r0.x;
  r0.z = 4 * r0.y;
  r1.xyz = float3(15,5,50) * cb1[0].yyy;
  r1.xyz = sin(r1.xyz);
  r0.z = r0.z * r1.x + r1.y;
  r0.w = 10 * r0.z;
  r0.z = -1 + r0.z;
  r0.z = -r0.z * r0.z + 1;
  r0.w = sin(r0.w);
  r0.z = r0.w * r0.z + 0.5;
  r0.w = cb0[6].y * r1.z;
  r1.x = 0.100000001 * r1.x;
  r0.z = r0.w * r0.z;
  r0.w = 1 / cb0[6].w;
  r1.y = v2.x / r0.w;
  r1.y = trunc(r1.y);
  r0.x = r1.y * r0.w + r0.z;
  r0.z = sin(cb0[6].z);
  r1.xy = r1.xx * r0.zz + r0.xy;
  r0.zw = -r0.zz * float2(0.100000001,0.100000001) + r0.xy;
  r2.xyzw = t0.Sample(s0_s, r0.xy).xyzw;
  r2.w = saturate(r2.w);


  if (RENODX_TONE_MAP_TYPE >= 1.f) {
    r0.xyz = ApplyPerceptualBoostAndToneMap(r0.xyz, SCENE_TYPE_2D_CHARACTER);
  }

  r0.xyzw = t0.Sample(s0_s, r0.zw).xyzw;
  r0.w = saturate(r2.w);

  r2.z = r0.z;
  r0.xyzw = t0.Sample(s0_s, r1.xy).xyzw;
  r0.w = saturate(r2.w);

  r2.x = r0.x;
  r0.x = dot(v1.xyz, float3(0.300000012,0.589999974,0.109999999));
  r0.x = 1 + -r0.x;
  r0.xyz = -r2.xyz * v1.xyz + r0.xxx;
  r1.w = v1.w * r2.w;
  r1.xyz = abs(r0.xyz);
  r0.xyzw = t0.Sample(s0_s, v2.xy).xyzw;
  r0.w = saturate(r2.w);
  r0.xyzw = cb0[3].xyzw + r0.xyzw;
  r0.xyzw = v1.xyzw * r0.xyzw;
  r2.x = asint(cb0[6].x);
  r2.y = cmp(0 >= r2.x);
  r2.x = cmp(r2.x >= 1);
  r2.xy = r2.xy ? float2(1,1) : 0;
  r0.xyzw = r2.yyyy * r0.xyzw;
  o0.xyzw = r1.xyzw * r2.xxxx + r0.xyzw;

  // saturate fix
  o0.w = saturate(o0.w);
  return;
}
