#include "./common.h"

// ---- Created with 3Dmigoto v1.4.1 on Wed Jul 16 10:48:11 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[133];
}




// 3Dmigoto declarations
#define cmp -

float3 saturate_or_not(float3 input) {
  if (RENODX_TONE_MAP_TYPE == 0.f) {
    input = saturate(input);
  }
  return input;
}

void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = cb0[132].xyxy * v1.xyxy;
  r0.xyzw = (int4)r0.xyzw;
  r1.xyzw = (int4)r0.zwzw + int4(-1,-1,1,-1);
  r0.xyzw = (int4)r0.xyzw + int4(-1,1,1,1);
  r0.xyzw = (int4)r0.xyzw;
  r0.xyzw = max(float4(0,0,0,0), r0.xyzw);
  r1.xyzw = (int4)r1.xyzw;
  r1.xyzw = max(float4(0,0,0,0), r1.xyzw);
  r2.xyzw = float4(-1,-1,-1,-1) + cb0[132].xyxy;
  r1.xyzw = min(r2.xyzw, r1.xyzw);
  r0.xyzw = min(r2.xyzw, r0.xyzw);
  r0.xyzw = (int4)r0.zwxy;
  r1.xyzw = (int4)r1.zwxy;
  r2.xy = r1.zw;
  r2.zw = float2(0,0);
  r2.xyz = t0.Load(r2.xyz).xyz;
  r2.xyz = saturate_or_not(r2.xyz);
  r2.x = dot(r2.xyz, float3(0.212672904,0.715152204,0.0721750036));
  r3.xy = r0.zw;
  r3.zw = float2(0,0);
  r2.yzw = t0.Load(r3.xyz).xyz;
  r2.yzw = saturate_or_not(r2.yzw);
  r2.y = dot(r2.yzw, float3(0.212672904,0.715152204,0.0721750036));
  r2.z = r2.x + r2.y;
  r1.zw = float2(0,0);
  r1.xyz = t0.Load(r1.xyz).xyz;
  r1.xyz = saturate_or_not(r1.xyz);
  r1.x = dot(r1.xyz, float3(0.212672904,0.715152204,0.0721750036));
  r0.zw = float2(0,0);
  r0.xyz = t0.Load(r0.xyz).xyz;
  r0.xyz = saturate_or_not(r0.xyz);
  r0.x = dot(r0.xyz, float3(0.212672904,0.715152204,0.0721750036));
  r0.y = r1.x + r0.x;
  r3.yw = r2.zz + -r0.yy;
  r0.y = r2.x + r1.x;
  r0.z = r2.y + r0.x;
  r0.z = r0.y + -r0.z;
  r0.y = r0.y + r2.y;
  r0.y = r0.y + r0.x;
  r0.y = 0.03125 * r0.y;
  r0.y = max(0.0078125, r0.y);
  r0.w = min(abs(r0.z), abs(r3.w));
  r3.xz = -r0.zz;
  r0.y = r0.w + r0.y;
  r0.y = rcp(r0.y);
  r3.xyzw = r3.xyzw * r0.yyyy;
  r3.xyzw = max(float4(-8,-8,-8,-8), r3.xyzw);
  r3.xyzw = min(float4(8,8,8,8), r3.xyzw);
  r3.xyzw = cb0[132].zwzw * r3.xyzw;
  r4.xyzw = r3.zwzw * float4(-0.5,-0.5,-0.166666672,-0.166666672) + v1.xyxy;
  r3.xyzw = r3.xyzw * float4(0.166666672,0.166666672,0.5,0.5) + v1.xyxy;
  r0.yzw = t0.SampleBias(s0_s, r4.xy, cb0[5].x).xyz;
  r1.yzw = t0.SampleBias(s0_s, r4.zw, cb0[5].x).xyz;
  r1.yzw = saturate_or_not(r1.yzw);
  r0.yzw = saturate_or_not(r0.yzw);
  r4.xyz = t0.SampleBias(s0_s, r3.zw, cb0[5].x).xyz;
  r3.xyz = t0.SampleBias(s0_s, r3.xy, cb0[5].x).xyz;
  r3.xyz = saturate_or_not(r3.xyz);
  r1.yzw = r3.xyz + r1.yzw;
  r4.xyz = saturate_or_not(r4.xyz);
  r0.yzw = r4.xyz + r0.yzw;
  r0.yzw = float3(0.25,0.25,0.25) * r0.yzw;
  r0.yzw = r1.yzw * float3(0.25,0.25,0.25) + r0.yzw;
  r1.yzw = float3(0.5,0.5,0.5) * r1.yzw;
  r2.z = dot(r0.yzw, float3(0.212672904,0.715152204,0.0721750036));
  r2.w = min(r1.x, r2.y);
  r1.x = max(r1.x, r2.y);
  r1.x = max(r1.x, r0.x);
  r0.x = min(r2.w, r0.x);
  r3.xyz = t0.SampleBias(s0_s, v1.xy, cb0[5].x).xyz;
  r3.xyz = saturate_or_not(r3.xyz);
  r2.y = dot(r3.xyz, float3(0.212672904,0.715152204,0.0721750036));
  r2.w = min(r2.y, r2.x);
  r2.x = max(r2.y, r2.x);
  r1.x = max(r2.x, r1.x);
  r1.x = cmp(r1.x < r2.z);
  r0.x = min(r2.w, r0.x);
  r0.x = cmp(r2.z < r0.x);
  r0.x = (int)r1.x | (int)r0.x;
  o0.xyz = r0.xxx ? r1.yzw : r0.yzw;
  o0.w = 1;
  return;
}