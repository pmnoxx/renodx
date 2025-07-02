// ---- Created with 3Dmigoto v1.4.1 on Tue Jul  1 17:30:38 2025
Texture2D<float4> t8 : register(t8);

Texture2D<float4> t7 : register(t7);

TextureCube<float4> t6 : register(t6);

Texture2D<float4> t5 : register(t5);

Texture2D<float4> t4 : register(t4);

Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s8_s : register(s8);

SamplerState s7_s : register(s7);

SamplerState s6_s : register(s6);

SamplerState s5_s : register(s5);

SamplerState s4_s : register(s4);

SamplerState s3_s : register(s3);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb3 : register(b3)
{
  float4 cb3[1];
}

cbuffer cb2 : register(b2)
{
  float4 cb2[34];
}

cbuffer cb1 : register(b1)
{
  float4 cb1[1536];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[168];
}


#include "./common.h"


// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : TEXCOORD0,
  float4 v2 : TEXCOORD1,
  float4 v3 : TEXCOORD2,
  float4 v4 : TEXCOORD3,
  float4 v5 : TEXCOORD4,
  float4 v6 : TEXCOORD5,
  float4 v7 : TEXCOORD8,
  float4 v8 : TEXCOORD9,
  float4 v9 : TEXCOORD10,
  float4 v10 : TEXCOORD11,
  out float4 o0 : SV_Target0)
{
  const float4 icb[] = { { 1.000000, 0, 0, 0},
                              { 0, 1.000000, 0, 0},
                              { 0, 0, 1.000000, 0},
                              { 0, 0, 0, 1.000000} };
  float4 r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.x = v4.w;
  r0.y = v5.w;
  r0.z = v6.w;
  r1.xyz = cb0[22].xyz + -r0.xyz;
  r0.w = dot(r1.xyz, r1.xyz);
  r0.w = max(1.17549435e-38, r0.w);
  r0.w = rsqrt(r0.w);
  r1.xyz = r1.xyz * r0.www;
  r2.xyz = t0.Sample(s6_s, v9.xy).xyz;

  //r2.xyz = ApplyReverseReinhard(r2.xyz);

  r3.xyz = t1.Sample(s7_s, v9.zw).xyz;
  r4.xyz = t2.Sample(s8_s, v10.xy).xyz;
  r2.xyz = r3.xyz * r4.xyz + r2.xyz;
  r2.xyz = saturate(cb3[0].xyz * r2.xyz);
  r3.xyz = cb0[150].xyz * cb0[149].xyz;
  r4.xyz = cb0[156].xyz + cb0[155].xyz;
  r2.xyz = r2.xyz * r3.xyz + r4.xyz;
  r3.xyz = v4.xyz;
  r3.w = 1;
  r4.x = dot(cb2[25].xyzw, r3.xyzw);
  r4.y = dot(cb2[26].xyzw, r3.xyzw);
  r4.z = dot(cb2[27].xyzw, r3.xyzw);
  r3.xyzw = v4.xyzz * v4.yzzx;
  r5.x = dot(cb2[28].xyzw, r3.xyzw);
  r5.y = dot(cb2[29].xyzw, r3.xyzw);
  r5.z = dot(cb2[30].xyzw, r3.xyzw);
  r0.w = v4.y * v4.y;
  r0.w = v4.x * v4.x + -r0.w;
  r3.xyz = cb2[31].xyz * r0.www + r5.xyz;
  r3.xyz = r4.xyz + r3.xyz;
  r4.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r3.xyz);
  r5.xyz = float3(12.9232101,12.9232101,12.9232101) * r3.xyz;
  r3.xyz = log2(abs(r3.xyz));
  r3.xyz = float3(0.416666657,0.416666657,0.416666657) * r3.xyz;
  r3.xyz = exp2(r3.xyz);
  r3.xyz = r3.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
  r3.xyz = r4.xyz ? r5.xyz : r3.xyz;
  r3.xyz = cb0[15].yyy * r3.xyz;
  r3.xyz = max(float3(0,0,0), r3.xyz);
  r2.xyz = float3(0.779083729,0.779083729,0.779083729) * r2.xyz;
  r4.xyzw = float4(-1,-1,-1,-1) + cb2[14].xyzw;
  r0.w = dot(v4.xyz, r1.xyz);
  r1.w = dot(-r1.xyz, v4.xyz);
  r1.w = r1.w + r1.w;
  r5.xyz = v4.xyz * -r1.www + -r1.xyz;
  r1.w = saturate(r0.w);
  r1.w = 1 + -r1.w;
  r1.w = r1.w * r1.w;
  r1.w = r1.w * r1.w;
  r6.xyz = min(cb2[32].xyz, cb2[18].xyz);
  r7.xyz = max(cb2[33].xyz, cb2[17].xyz);
  r2.w = cmp(0 < cb2[19].w);
  r8.xyz = cmp(float3(0,0,0) < r5.xyz);
  r6.xyz = r8.xyz ? r7.xyz : r6.xyz;
  r6.xyz = r6.xyz + -r0.xyz;
  r6.xyz = r6.xyz / r5.xyz;
  r3.w = min(r6.x, r6.y);
  r3.w = min(r3.w, r6.z);
  r6.xyz = -cb2[19].xyz + r0.xyz;
  r6.xyz = r5.xyz * r3.www + r6.xyz;
  r5.xyz = r2.www ? r6.xyz : r5.xyz;
  r5.xyzw = t6.SampleLevel(s0_s, r5.xyz, 6).xyzw;
  r2.w = -1 + r5.w;
  r2.w = cb2[15].w * r2.w + 1;
  r2.w = max(0, r2.w);
  r2.w = log2(r2.w);
  r2.w = cb2[15].y * r2.w;
  r2.w = exp2(r2.w);
  r2.w = cb2[15].x * r2.w;
  r5.xyz = r2.www * r5.xyz;
  r1.w = r1.w * -2.98023224e-08 + 0.220916301;
  r1.w = 0.720000029 * r1.w;
  r5.xyz = r5.xyz * r1.www;
  r3.xyz = r3.xyz * r2.xyz + r5.xyz;
  r1.w = asint(cb0[9].x) & asint(cb2[10].x);
  if (r1.w != 0) {
    r1.w = dot(r4.xyzw, cb0[8].xyzw);
    r1.w = 1 + r1.w;
    r1.w = min(1, r1.w);
    r5.xy = cb0[17].yy * cb0[137].zw + r0.xz;
    r5.xy = cb0[137].xy * r5.xy;
    r2.w = t7.SampleBias(s1_s, r5.xy, cb0[5].x).x;
    r3.w = 1 + -cb0[138].x;
    r2.w = r2.w * cb0[138].x + r3.w;
    r3.w = r2.w * r1.w;
    r5.x = saturate(dot(v4.xyz, cb0[6].xyz));
    r5.y = cb2[11].z * r3.w;
    r5.y = r5.y * r5.x;
    r5.yzw = cb0[7].xyz * r5.yyy;
    r3.w = -r3.w * r5.x + 1;
    r3.w = cb2[11].z * r3.w;
    r6.xyz = cb0[16].xxx * r2.xyz;
    r6.xyz = cb0[135].xyz * r6.xyz;
    r6.xyz = r6.xyz * r3.www;
    r1.w = -r1.w * r2.w + 1;
    r1.w = 4 * r1.w;
    r1.w = cb0[136].w * r1.w;
    r1.w = min(1, r1.w);
    r7.xyz = float3(-1,-1,-1) + cb0[136].xyz;
    r7.xyz = r1.www * r7.xyz + float3(1,1,1);
    r7.xyz = r7.xyz * r2.xyz;
    r1.w = saturate(dot(cb0[6].xyz, r1.xyz));
    r2.w = 1 + -r5.x;
    r3.w = r2.w * r2.w;
    r3.w = r3.w * r3.w;
    r2.w = r3.w * r2.w;
    r1.w = 0.5 + r1.w;
    r2.w = r1.w * r2.w + 1;
    r3.w = 1 + -abs(r0.w);
    r6.w = r3.w * r3.w;
    r6.w = r6.w * r6.w;
    r3.w = r6.w * r3.w;
    r1.w = r1.w * r3.w + 1;
    r1.w = r2.w * r1.w;
    r1.w = 0.965521276 * r1.w;
    r2.w = r5.x + abs(r0.w);
    r2.w = max(1.17549435e-38, r2.w);
    r2.w = 0.159154937 / r2.w;
    r2.w = 3.14159274 * r2.w;
    r2.w = max(0.0078125, r2.w);
    r2.w = sqrt(r2.w);
    r2.w = 0.220916301 * r2.w;
    r7.xyz = r7.xyz * r1.www + r2.www;
    r5.xyz = r7.xyz * r5.yzw + r6.xyz;
  } else {
    r5.xyz = float3(0,0,0);
  }
  r1.w = min(cb2[11].y, cb0[11].x);
  r1.w = (int)r1.w;
  r6.xyz = cb0[16].xxx * r2.xyz;
  r6.xyz = cb0[135].xyz * r6.xyz;
  r7.xyz = float3(-1,-1,-1) + cb0[136].xyz;
  r2.w = 1 + -abs(r0.w);
  r3.w = r2.w * r2.w;
  r3.w = r3.w * r3.w;
  r2.w = r3.w * r2.w;
  r8.xyzw = float4(0,0,0,0);
  while (true) {
    r3.w = cmp((uint)r8.w >= (uint)r1.w);
    if (r3.w != 0) break;
    r3.w = (uint)r8.w >> 2;
    r5.w = (int)r8.w & 3;
    r3.w = dot(cb2[r3.w+12].xyzw, icb[r5.w+0].xyzw);
    r3.w = (int)r3.w;
    r5.w = asint(cb2[10].x) & asint(cb1[r3.w+1280].x);
    if (r5.w != 0) {
      r9.xyz = -r0.xyz * cb1[r3.w+0].www + cb1[r3.w+0].xyz;
      r5.w = dot(r9.xyz, r9.xyz);
      r5.w = max(6.10351562e-05, r5.w);
      r6.w = rsqrt(r5.w);
      r9.xyz = r9.xyz * r6.www;
      r5.w = cb1[r3.w+512].x * r5.w;
      r5.w = t8.SampleBias(s2_s, r5.ww, cb0[5].x).x;
      r6.w = dot(cb1[r3.w+768].xyz, r9.xyz);
      r6.w = saturate(r6.w * cb1[r3.w+512].z + cb1[r3.w+512].w);
      r6.w = r6.w * r6.w;
      r5.w = r6.w * r5.w;
      r6.w = dot(r4.xyzw, cb1[r3.w+1024].xyzw);
      r6.w = 1 + r6.w;
      r6.w = min(1, r6.w);
      r7.w = saturate(dot(v4.xyz, r9.xyz));
      r9.w = r6.w * r5.w;
      r9.w = r9.w * r7.w;
      r10.xyz = cb1[r3.w+256].xyz * r9.www;
      r3.w = -r6.w * r7.w + 1;
      r3.w = r3.w * r5.w;
      r11.xyz = r6.xyz * r3.www;
      r3.w = 1 + -r6.w;
      r3.w = 4 * r3.w;
      r3.w = cb0[136].w * r3.w;
      r3.w = min(1, r3.w);
      r12.xyz = r3.www * r7.xyz + float3(1,1,1);
      r12.xyz = r12.xyz * r2.xyz;
      r3.w = saturate(dot(r9.xyz, r1.xyz));
      r5.w = 1 + -r7.w;
      r6.w = r5.w * r5.w;
      r6.w = r6.w * r6.w;
      r5.w = r6.w * r5.w;
      r3.w = 0.5 + r3.w;
      r5.w = r3.w * r5.w + 1;
      r3.w = r3.w * r2.w + 1;
      r3.w = r5.w * r3.w;
      r3.w = 0.965521276 * r3.w;
      r5.w = r7.w + abs(r0.w);
      r5.w = max(1.17549435e-38, r5.w);
      r5.w = 0.159154937 / r5.w;
      r5.w = 3.14159274 * r5.w;
      r5.w = max(0.0078125, r5.w);
      r5.w = sqrt(r5.w);
      r5.w = 0.220916301 * r5.w;
      r9.xyz = r12.xyz * r3.www + r5.www;
      r9.xyz = r9.xyz * r10.xyz + r11.xyz;
      r8.xyz = r9.xyz + r8.xyz;
    }
    r8.w = (int)r8.w + 1;
  }
  r0.xyz = r5.xyz + r3.xyz;
  r0.xyz = r0.xyz + r8.xyz;
  r1.xyzw = t3.Sample(s3_s, v7.zw).xyzw;
  r1.xyzw = cb0[163].xyzw * r1.xyzw;
  r1.xyz = r1.xyz * r1.www;
  r2.xyz = cb0[161].yyy * r1.xyz;
  r1.xyz = r1.xyz * cb0[161].yyy + r0.xyz;
  r0.xyz = -r0.xyz * r2.xyz + r1.xyz;
  r1.xyzw = t4.Sample(s4_s, v7.xy).xyzw;
  r0.w = cb0[165].w * r1.w;
  r0.w = cb0[161].y * r0.w;
  r1.xyz = r1.xyz * cb0[165].xyz + -r0.xyz;
  r0.xyz = r0.www * r1.xyz + r0.xyz;
  r1.xyzw = t5.Sample(s5_s, v8.xy).xyzw;
  r2.xyzw = cb0[167].xyzw * r1.xyzw;
  r2.xyz = r2.xyz * r2.www;
  r3.xyz = cb0[161].yyy * r2.xyz;
  r2.xyz = r2.xyz * cb0[161].yyy + r0.xyz;
  r2.xyz = -r0.xyz * r3.xyz + r2.xyz;
  r0.w = cb0[161].y * r2.w;
  r1.xyz = r1.xyz * cb0[167].xyz + -r0.xyz;
  r0.xyz = r0.www * r1.xyz + r0.xyz;
  r1.xyz = r2.xyz + -r0.xyz;
  o0.xyz = cb0[166].www * r1.xyz + r0.xyz;
  o0.w = 1;
  return;
}