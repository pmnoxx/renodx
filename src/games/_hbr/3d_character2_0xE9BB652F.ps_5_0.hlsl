// ---- Created with 3Dmigoto v1.4.1 on Tue Jul  1 17:52:09 2025
Texture2D<float4> t4 : register(t4);

Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s4_s : register(s4);

SamplerState s3_s : register(s3);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb2 : register(b2)
{
  float4 cb2[12];
}

cbuffer cb1 : register(b1)
{
  float4 cb1[32];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[171];
}




// 3Dmigoto declarations
#define cmp -

#include "./common.h"

void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : TEXCOORD0,
  float4 v2 : TEXCOORD1,
  float4 v3 : TEXCOORD5,
  float4 v4 : TEXCOORD6,
  float4 v5 : TEXCOORD7,
  float4 v6 : TEXCOORD8,
  float4 v7 : COLOR0,
  float4 v8 : TEXCOORD9,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.x = dot(v4.xyz, v4.xyz);
  r0.x = rsqrt(r0.x);
  r0.xyz = v4.xyz * r0.xxx;
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
  r0.xyz = r2.xyz + r1.xyz;
  r1.xyz = log2(abs(r0.xyz));
  r1.xyz = float3(0.416666657,0.416666657,0.416666657) * r1.xyz;
  r1.xyz = exp2(r1.xyz);
  r1.xyz = r1.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
  r2.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.xyz);
  r0.xyz = float3(12.9232101,12.9232101,12.9232101) * r0.xyz;
  r0.xyz = r2.xyz ? r0.xyz : r1.xyz;
  r0.xyz = cb0[15].yyy * r0.xyz;
  r0.xyz = max(float3(0,0,0), r0.xyz);
  r1.xy = v3.xy * cb2[10].xy + cb2[10].zw;
  r1.xyzw = t0.Sample(s4_s, r1.xy).xyzw;

  //r1.xyz = ApplyReverseReinhard(r1.xyz);  

  r2.xyz = cb2[2].xyz * r1.xyz;
  r0.xyz = r2.xyz * r0.xyz;
  r2.xyz = -cb2[6].xyz + cb0[168].xyz;
  r2.xyz = cb0[168].www * r2.xyz + cb2[6].xyz;
  r0.xyz = r2.xyz * r0.xyz;
  r0.xyz = cb2[4].xxx * r0.xyz;
  r0.xyz = r0.xyz * cb0[151].xyz + cb0[157].xyz;
  r0.w = 1 + -r1.w;
  r1.xyz = r1.xyz * r0.www;
  r0.xyz = r1.xyz * cb2[1].xyz + r0.xyz;
  r0.w = dot(v5.xyz, v5.xyz);
  r0.w = rsqrt(r0.w);
  r1.xy = v5.xy * r0.ww;
  r1.xy = r1.xy * v6.zz + float2(1,1);
  r1.xy = r1.xy * float2(0.5,0.5) + v6.xy;
  r1.xyz = t1.Sample(s3_s, r1.xy).xyz;
  r1.xyz = cb2[11].xyz * r1.xyz;
  r1.xyz = v7.xxx * r1.xyz;
  r2.xyz = -cb2[5].xyz + cb0[170].xyz;
  r2.xyz = cb0[170].www * r2.xyz + cb2[5].xyz;
  r1.xyz = r2.xyz * r1.xyz;
  r1.xyz = cb2[4].yyy * r1.xyz;
  r0.xyz = r1.xyz * cb0[152].xyz + r0.xyz;
  r1.xyzw = t2.Sample(s0_s, v8.zw).xyzw;
  r1.xyzw = cb0[163].xyzw * r1.xyzw;
  r1.xyz = r1.xyz * r1.www;
  r2.xyz = r1.xyz * cb0[161].xxx + r0.xyz;
  r1.xyz = cb0[161].xxx * r1.xyz;
  r0.xyz = -r0.xyz * r1.xyz + r2.xyz;
  r1.xyzw = t3.Sample(s1_s, v8.xy).xyzw;
  r1.xyz = cb0[165].xyz * r1.xyz + -r0.xyz;
  r0.w = cb0[165].w * r1.w;
  r0.w = cb0[161].x * r0.w;
  r0.xyz = r0.www * r1.xyz + r0.xyz;
  r1.xyzw = t4.Sample(s2_s, v3.zw).xyzw;
  r2.xyzw = cb0[167].xyzw * r1.xyzw;
  r1.xyz = cb0[167].xyz * r1.xyz + -r0.xyz;
  r0.w = cb0[161].x * r2.w;
  r3.xyz = r2.xyz * r0.www + r0.xyz;
  r2.xyz = r2.xyz * r0.www;
  r1.xyz = r0.www * r1.xyz + r0.xyz;
  r0.xyz = -r0.xyz * r2.xyz + r3.xyz;
  r0.xyz = r0.xyz + -r1.xyz;
  r0.xyz = cb0[166].www * r0.xyz + r1.xyz;
  r0.xyz = cb0[149].xyz * r0.xyz;
  r0.w = cb2[2].w * cb0[149].w;
  r0.xyzw = cb0[155].xyzw + r0.xyzw;
  o0.xyzw = r0.xyzw * cb2[7].xyzw + cb2[8].xyzw;
  return;
}