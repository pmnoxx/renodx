// ---- Created with 3Dmigoto v1.4.1 on Tue Jul  1 19:47:50 2025
Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s3_s : register(s3);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb2 : register(b2)
{
  float4 cb2[7];
}

cbuffer cb1 : register(b1)
{
  float4 cb1[32];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[168];
}




// 3Dmigoto declarations
#define cmp -


#include "./common.h"

void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : TEXCOORD0,
  float4 v2 : TEXCOORD1,
  float4 v3 : TEXCOORD5,
  float4 v4 : COLOR0,
  float4 v5 : TEXCOORD6,
  float4 v6 : TEXCOORD7,
  float4 v7 : TEXCOORD8,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.x = dot(v5.xyz, v5.xyz);
  r0.x = rsqrt(r0.x);
  r0.xyz = v5.xyz * r0.xxx;
  r1.x = r0.y * r0.y;
  r1.x = r0.x * r0.x + -r1.x;
  r2.xyzw = r0.xyzz * r0.yzzx;
  r3.x = dot(cb1[28].xyzw, r2.xyzw);
  r3.y = dot(cb1[29].xyzw, r2.xyzw);
  r3.z = dot(cb1[30].xyzw, r2.xyzw);
  r1.xyz = cb1[31].xyz * r1.xxx + r3.xyz;
  r0.w = 1;
  r2.x = dot(cb1[25].xyzw, r0.xyzw);
  r2.y = dot(cb1[26].xyzw, r0.xyzw);
  r2.z = dot(cb1[27].xyzw, r0.xyzw);
  r0.x = dot(cb0[6].xyz, r0.xyz);
  r0.x = max(0, r0.x);
  r0.x = r0.x * cb0[160].x + cb0[160].y;
  r0.yzw = r2.xyz + r1.xyz;
  r1.xyz = log2(abs(r0.yzw));
  r1.xyz = float3(0.416666657,0.416666657,0.416666657) * r1.xyz;
  r1.xyz = exp2(r1.xyz);
  r1.xyz = r1.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
  r2.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.yzw);
  r0.yzw = float3(12.9232101,12.9232101,12.9232101) * r0.yzw;
  r0.yzw = r2.xyz ? r0.yzw : r1.xyz;
  r0.yzw = cb0[15].yyy * r0.yzw;
  r0.yzw = max(float3(0,0,0), r0.yzw);
  r1.xy = v3.xy * cb2[0].xy + cb2[0].zw;
  r1.xyzw = t0.Sample(s3_s, r1.xy).xyzw;

  r1.xyz = ApplyReverseReinhard(r1.xyz);

  r1.xyzw = v4.xyzw * r1.xyzw;
  r1.xyzw = cb2[1].xyzw * r1.xyzw;
  r0.yzw = r1.xyz * r0.yzw;
  r2.xyz = cb0[7].xyz * r1.xyz;
  r0.xyz = r2.xyz * r0.xxx + r0.yzw;
  r0.w = cmp(cb2[5].z == 1.000000);
  r2.xyzw = float4(-1,-1,-1,-1) + cb1[14].xyzw;
  r2.xyzw = r0.wwww ? r2.xyzw : 0;
  r0.w = dot(r2.xyzw, cb0[8].xyzw);
  r0.w = 1 + r0.w;
  r0.w = min(1, r0.w);
  r2.xyz = float3(1,1,1) + -cb0[153].xyz;
  r2.xyz = r0.www * r2.xyz + cb0[153].xyz;
  r3.xyz = r2.xyz * r0.xyz;
  r4.xyzw = t1.Sample(s0_s, v7.zw).xyzw;
  r4.xyzw = cb0[163].xyzw * r4.xyzw;
  r4.xyz = r4.xyz * r4.www;
  r4.xyz = cb0[161].yyy * r4.xyz;
  r0.xyz = r0.xyz * r2.xyz + r4.xyz;
  r0.xyz = -r3.xyz * r4.xyz + r0.xyz;
  r2.xyzw = t2.Sample(s1_s, v7.xy).xyzw;
  r2.xyz = cb0[165].xyz * r2.xyz + -r0.xyz;
  r0.w = cb0[165].w * r2.w;
  r0.w = cb0[161].y * r0.w;
  r0.xyz = r0.www * r2.xyz + r0.xyz;
  r2.xyzw = t3.Sample(s2_s, v6.zw).xyzw;
  r3.xyzw = cb0[167].xyzw * r2.xyzw;
  r2.xyz = cb0[167].xyz * r2.xyz + -r0.xyz;
  r0.w = cb0[161].y * r3.w;
  r4.xyz = r3.xyz * r0.www + r0.xyz;
  r3.xyz = r3.xyz * r0.www;
  r2.xyz = r0.www * r2.xyz + r0.xyz;
  r0.xyz = -r0.xyz * r3.xyz + r4.xyz;
  r0.xyz = r0.xyz + -r2.xyz;
  r1.xyz = cb0[166].www * r0.xyz + r2.xyz;
  r0.xyzw = cb0[150].xyzw * cb0[149].xyzw;
  r2.xyzw = cb0[156].xyzw + cb0[155].xyzw;
  r0.xyzw = r1.xyzw * r0.xyzw + r2.xyzw;
  r1.x = -cb2[6].x + r0.w;
  o0.xyzw = r0.xyzw;
  r0.x = cmp(r1.x < 0);
  if (r0.x != 0) discard;
  return;
}