// ---- Created with 3Dmigoto v1.4.1 on Tue Jul  1 17:25:45 2025
Texture2D<float4> t9 : register(t9);

Texture2D<float4> t8 : register(t8);

Texture2D<float4> t7 : register(t7);

Texture2D<float4> t6 : register(t6);

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

SamplerComparisonState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb4 : register(b4)
{
  float4 cb4[6];
}

cbuffer cb3 : register(b3)
{
  float4 cb3[1314];
}

cbuffer cb2 : register(b2)
{
  float4 cb2[24];
}

cbuffer cb1 : register(b1)
{
  float4 cb1[1280];
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
  const float4 icb[] = { { 1.000000, 0, 0, 0},
                              { 0, 1.000000, 0, 0},
                              { 0, 0, 1.000000, 0},
                              { 0, 0, 0, 1.000000} };
  float4 r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18,r19,r20;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyz = -cb3[20].xyz + v2.xyz;
  r1.xyz = -cb3[21].xyz + v2.xyz;
  r2.xyz = -cb3[22].xyz + v2.xyz;
  r3.xyz = -cb3[23].xyz + v2.xyz;
  r0.x = dot(r0.xyz, r0.xyz);
  r0.y = dot(r1.xyz, r1.xyz);
  r0.z = dot(r2.xyz, r2.xyz);
  r0.w = dot(r3.xyz, r3.xyz);
  r0.xyzw = cmp(r0.xyzw < cb3[24].xyzw);
  r1.xyzw = r0.xyzw ? float4(1,1,1,1) : 0;
  r0.xyz = r0.xyz ? float3(-1,-1,-1) : float3(-0,-0,-0);
  r0.xyz = r1.yzw + r0.xyz;
  r1.yzw = max(float3(0,0,0), r0.xyz);
  r0.x = dot(r1.xyzw, float4(4,3,2,1));
  r0.x = 4 + -r0.x;
  r0.x = (uint)r0.x;
  r0.x = (uint)r0.x << 2;
  r0.yzw = cb3[r0.x+1].xyz * v2.yyy;
  r0.yzw = cb3[r0.x+0].xyz * v2.xxx + r0.yzw;
  r0.yzw = cb3[r0.x+2].xyz * v2.zzz + r0.yzw;
  r0.xyz = cb3[r0.x+3].xyz + r0.yzw;
  r1.xy = v3.xy * cb4[0].xy + cb4[0].zw;
  r1.xyzw = t0.Sample(s7_s, r1.xy).xyzw;

 // r1.xyz = ApplyReverseReinhard(r1.xyz);

  r2.xyzw = t1.Sample(s8_s, v3.xy).xyzw;
  r1.xyzw = -r2.xyzw + r1.xyzw;
  r1.xyzw = v4.wwww * r1.xyzw + r2.xyzw;
  r1.xyz = v4.xyz * r1.xyz;
  r1.xyzw = cb4[1].xyzw * r1.xyzw;
  r0.w = dot(v5.xyz, v5.xyz);
  r0.w = rsqrt(r0.w);
  r2.xyz = v5.xyz * r0.www;
  r0.w = dot(cb0[6].xyz, r2.xyz);
  r3.xy = v3.zw * cb2[23].xy + cb2[23].zw;
  r4.xyz = t5.SampleBias(s0_s, r3.xy, cb0[5].x).xyz;
  r3.xyzw = t6.SampleBias(s1_s, r3.xy, cb0[5].x).xyzw;
  r2.w = cmp(cb4[5].z == 1.000000);
  r4.w = cmp(0 < cb3[27].y);
  if (r4.w != 0) {
    r4.w = cmp(cb3[27].y == 1.000000);
    if (r4.w != 0) {
      r5.xyzw = cb3[25].xyzw + r0.xyxy;
      r6.x = t7.SampleCmpLevelZero(s2_s, r5.xy, r0.z).x;
      r6.y = t7.SampleCmpLevelZero(s2_s, r5.zw, r0.z).x;
      r5.xyzw = cb3[26].xyzw + r0.xyxy;
      r6.z = t7.SampleCmpLevelZero(s2_s, r5.xy, r0.z).x;
      r6.w = t7.SampleCmpLevelZero(s2_s, r5.zw, r0.z).x;
      r4.w = dot(r6.xyzw, float4(0.25,0.25,0.25,0.25));
    } else {
      r5.x = cmp(cb3[27].y == 2.000000);
      if (r5.x != 0) {
        r5.xy = r0.xy * cb3[28].zw + float2(0.5,0.5);
        r5.xy = floor(r5.xy);
        r5.zw = r0.xy * cb3[28].zw + -r5.xy;
        r6.xyzw = float4(0.5,1,0.5,1) + r5.zzww;
        r7.xyzw = r6.xxzz * r6.xxzz;
        r6.xz = float2(0.0799999982,0.0799999982) * r7.yw;
        r7.xy = r7.xz * float2(0.5,0.5) + -r5.zw;
        r7.zw = float2(1,1) + -r5.zw;
        r8.xy = min(float2(0,0), r5.zw);
        r8.xy = -r8.xy * r8.xy + r7.zw;
        r5.zw = max(float2(0,0), r5.zw);
        r5.zw = -r5.zw * r5.zw + r6.yw;
        r8.xy = float2(1,1) + r8.xy;
        r5.zw = float2(1,1) + r5.zw;
        r9.xy = float2(0.159999996,0.159999996) * r7.xy;
        r7.xy = float2(0.159999996,0.159999996) * r7.zw;
        r8.xy = float2(0.159999996,0.159999996) * r8.xy;
        r10.xy = float2(0.159999996,0.159999996) * r5.zw;
        r5.zw = float2(0.159999996,0.159999996) * r6.yw;
        r9.z = r8.x;
        r9.w = r5.z;
        r7.z = r10.x;
        r7.w = r6.x;
        r11.xyzw = r9.zwxz + r7.zwxz;
        r8.z = r9.y;
        r8.w = r5.w;
        r10.z = r7.y;
        r10.w = r6.z;
        r6.xyz = r10.zyw + r8.zyw;
        r7.xyz = r7.xzw / r11.zwy;
        r7.xyz = float3(-2.5,-0.5,1.5) + r7.xyz;
        r8.xyz = r10.zyw / r6.xyz;
        r8.xyz = float3(-2.5,-0.5,1.5) + r8.xyz;
        r7.xyz = cb3[28].xxx * r7.yxz;
        r8.xyz = cb3[28].yyy * r8.xyz;
        r7.w = r8.x;
        r9.xyzw = r5.xyxy * cb3[28].xyxy + r7.ywxw;
        r5.zw = r5.xy * cb3[28].xy + r7.zw;
        r8.w = r7.y;
        r7.yw = r8.yz;
        r10.xyzw = r5.xyxy * cb3[28].xyxy + r7.xyzy;
        r8.xyzw = r5.xyxy * cb3[28].xyxy + r8.wywz;
        r7.xyzw = r5.xyxy * cb3[28].xyxy + r7.xwzw;
        r12.xyzw = r11.zwyz * r6.xxxy;
        r13.xyzw = r11.xyzw * r6.yyzz;
        r5.x = r11.y * r6.z;
        r5.y = t7.SampleCmpLevelZero(s2_s, r9.xy, r0.z).x;
        r6.x = t7.SampleCmpLevelZero(s2_s, r9.zw, r0.z).x;
        r6.x = r12.y * r6.x;
        r5.y = r12.x * r5.y + r6.x;
        r5.z = t7.SampleCmpLevelZero(s2_s, r5.zw, r0.z).x;
        r5.y = r12.z * r5.z + r5.y;
        r5.z = t7.SampleCmpLevelZero(s2_s, r8.xy, r0.z).x;
        r5.y = r12.w * r5.z + r5.y;
        r5.z = t7.SampleCmpLevelZero(s2_s, r10.xy, r0.z).x;
        r5.y = r13.x * r5.z + r5.y;
        r5.z = t7.SampleCmpLevelZero(s2_s, r10.zw, r0.z).x;
        r5.y = r13.y * r5.z + r5.y;
        r5.z = t7.SampleCmpLevelZero(s2_s, r8.zw, r0.z).x;
        r5.y = r13.z * r5.z + r5.y;
        r5.z = t7.SampleCmpLevelZero(s2_s, r7.xy, r0.z).x;
        r5.y = r13.w * r5.z + r5.y;
        r5.z = t7.SampleCmpLevelZero(s2_s, r7.zw, r0.z).x;
        r4.w = r5.x * r5.z + r5.y;
      } else {
        r5.xy = r0.xy * cb3[28].zw + float2(0.5,0.5);
        r5.xy = floor(r5.xy);
        r5.zw = r0.xy * cb3[28].zw + -r5.xy;
        r6.xyzw = float4(0.5,1,0.5,1) + r5.zzww;
        r7.xyzw = r6.xxzz * r6.xxzz;
        r8.yw = float2(0.0408160016,0.0408160016) * r7.yw;
        r6.xz = r7.xz * float2(0.5,0.5) + -r5.zw;
        r7.xy = float2(1,1) + -r5.zw;
        r7.zw = min(float2(0,0), r5.zw);
        r7.xy = -r7.zw * r7.zw + r7.xy;
        r7.zw = max(float2(0,0), r5.zw);
        r6.yw = -r7.zw * r7.zw + r6.yw;
        r7.xy = float2(2,2) + r7.xy;
        r6.yw = float2(2,2) + r6.yw;
        r9.z = 0.0816320032 * r6.y;
        r10.xyz = float3(0.0816320032,0.0816320032,0.0816320032) * r6.zxw;
        r6.xy = float2(0.0816320032,0.0816320032) * r7.xy;
        r9.x = r10.y;
        r9.yw = r5.zz * float2(-0.0816320032,0.0816320032) + float2(0.163264006,0.0816320032);
        r7.xz = r5.zz * float2(-0.0816320032,0.0816320032) + float2(0.0816320032,0.163264006);
        r7.y = r6.x;
        r7.w = r8.y;
        r9.xyzw = r9.xyzw + r7.xyzw;
        r10.yw = r5.ww * float2(-0.0816320032,0.0816320032) + float2(0.163264006,0.0816320032);
        r8.xz = r5.ww * float2(-0.0816320032,0.0816320032) + float2(0.0816320032,0.163264006);
        r8.y = r6.y;
        r6.xyzw = r10.xyzw + r8.xyzw;
        r7.xyzw = r7.xyzw / r9.xyzw;
        r7.xyzw = float4(-3.5,-1.5,0.5,2.5) + r7.xyzw;
        r8.xyzw = r8.xyzw / r6.xyzw;
        r8.xyzw = float4(-3.5,-1.5,0.5,2.5) + r8.xyzw;
        r7.xyzw = cb3[28].xxxx * r7.wxyz;
        r8.xyzw = cb3[28].yyyy * r8.xwyz;
        r10.xzw = r7.yzw;
        r10.y = r8.x;
        r11.xyzw = r5.xyxy * cb3[28].xyxy + r10.xyzy;
        r5.zw = r5.xy * cb3[28].xy + r10.wy;
        r7.y = r10.y;
        r10.y = r8.z;
        r12.xyzw = r5.xyxy * cb3[28].xyxy + r10.xyzy;
        r13.xy = r5.xy * cb3[28].xy + r10.wy;
        r7.z = r10.y;
        r14.xyzw = r5.xyxy * cb3[28].xyxy + r7.xyxz;
        r10.y = r8.w;
        r15.xyzw = r5.xyxy * cb3[28].xyxy + r10.xyzy;
        r7.yz = r5.xy * cb3[28].xy + r10.wy;
        r7.w = r10.y;
        r13.zw = r5.xy * cb3[28].xy + r7.xw;
        r8.xzw = r10.xzw;
        r10.xyzw = r5.xyxy * cb3[28].xyxy + r8.xyzy;
        r8.zw = r5.xy * cb3[28].xy + r8.wy;
        r8.x = r7.x;
        r5.xy = r5.xy * cb3[28].xy + r8.xy;
        r16.xyzw = r9.xyzw * r6.xxxx;
        r17.xyzw = r9.xyzw * r6.yyyy;
        r18.xyzw = r9.xyzw * r6.zzzz;
        r6.xyzw = r9.xyzw * r6.wwww;
        r7.x = t7.SampleCmpLevelZero(s2_s, r11.xy, r0.z).x;
        r7.w = t7.SampleCmpLevelZero(s2_s, r11.zw, r0.z).x;
        r7.w = r16.y * r7.w;
        r7.x = r16.x * r7.x + r7.w;
        r5.z = t7.SampleCmpLevelZero(s2_s, r5.zw, r0.z).x;
        r5.z = r16.z * r5.z + r7.x;
        r5.w = t7.SampleCmpLevelZero(s2_s, r14.xy, r0.z).x;
        r5.z = r16.w * r5.w + r5.z;
        r5.w = t7.SampleCmpLevelZero(s2_s, r12.xy, r0.z).x;
        r5.z = r17.x * r5.w + r5.z;
        r5.w = t7.SampleCmpLevelZero(s2_s, r12.zw, r0.z).x;
        r5.z = r17.y * r5.w + r5.z;
        r5.w = t7.SampleCmpLevelZero(s2_s, r13.xy, r0.z).x;
        r5.z = r17.z * r5.w + r5.z;
        r5.w = t7.SampleCmpLevelZero(s2_s, r14.zw, r0.z).x;
        r5.z = r17.w * r5.w + r5.z;
        r5.w = t7.SampleCmpLevelZero(s2_s, r15.xy, r0.z).x;
        r5.z = r18.x * r5.w + r5.z;
        r5.w = t7.SampleCmpLevelZero(s2_s, r15.zw, r0.z).x;
        r5.z = r18.y * r5.w + r5.z;
        r5.w = t7.SampleCmpLevelZero(s2_s, r7.yz, r0.z).x;
        r5.z = r18.z * r5.w + r5.z;
        r5.w = t7.SampleCmpLevelZero(s2_s, r13.zw, r0.z).x;
        r5.z = r18.w * r5.w + r5.z;
        r5.w = t7.SampleCmpLevelZero(s2_s, r10.xy, r0.z).x;
        r5.z = r6.x * r5.w + r5.z;
        r5.w = t7.SampleCmpLevelZero(s2_s, r10.zw, r0.z).x;
        r5.z = r6.y * r5.w + r5.z;
        r5.w = t7.SampleCmpLevelZero(s2_s, r8.zw, r0.z).x;
        r5.z = r6.z * r5.w + r5.z;
        r5.x = t7.SampleCmpLevelZero(s2_s, r5.xy, r0.z).x;
        r4.w = r6.w * r5.x + r5.z;
      }
    }
  } else {
    r4.w = t7.SampleCmpLevelZero(s2_s, r0.xy, r0.z).x;
  }
  r0.x = 1 + -cb3[27].x;
  r0.x = r4.w * cb3[27].x + r0.x;
  r0.y = cmp(0 >= r0.z);
  r0.z = cmp(r0.z >= 1);
  r0.y = (int)r0.z | (int)r0.y;
  r0.x = r0.y ? 1 : r0.x;
  r5.xyzw = float4(-1,-1,-1,-1) + r3.xyzw;
  r5.xyzw = r2.wwww ? r5.xyzw : 0;
  r0.y = dot(r5.xyzw, cb0[8].xyzw);
  r0.y = 1 + r0.y;
  r3.yzw = -cb0[22].xyz + v2.xyz;
  r0.z = dot(r3.yzw, r3.yzw);
  r2.w = saturate(r0.z * cb3[27].z + cb3[27].w);
  r3.y = cmp(cb3[29].x >= 1);
  r2.w = r3.y ? 0 : r2.w;
  r3.y = 1 + -r0.x;
  r0.x = r2.w * r3.y + r0.x;
  r0.x = min(r0.x, r0.y);
  r3.yzw = float3(1,1,1) + -cb0[153].xyz;
  r3.yzw = r0.xxx * r3.yzw + cb0[153].xyz;
  r5.xyz = cb0[7].xyz * r1.xyz;
  r0.x = max(0, r0.w);
  r0.x = r0.x * cb0[160].z + cb0[160].w;
  r4.xyz = r4.xyz * r1.xyz;
  r0.xyw = r5.xyz * r0.xxx + r4.xyz;
  r4.xyz = r0.xyw * r3.yzw;
  r5.xyzw = t2.Sample(s4_s, v7.zw).xyzw;
  r5.xyzw = cb0[163].xyzw * r5.xyzw;
  r5.xyz = r5.xyz * r5.www;
  r5.xyz = cb0[161].yyy * r5.xyz;
  r6.xyzw = t3.Sample(s5_s, v7.xy).xyzw;
  r2.w = cb0[165].w * r6.w;
  r0.xyw = r0.xyw * r3.yzw + r5.xyz;
  r0.xyw = -r4.xyz * r5.xyz + r0.xyw;
  r2.w = cb0[161].y * r2.w;
  r3.yzw = cb0[165].xyz * r6.xyz + -r0.xyw;
  r0.xyw = r2.www * r3.yzw + r0.xyw;
  r4.xyzw = t4.Sample(s6_s, v6.zw).xyzw;
  r5.xyzw = cb0[167].xyzw * r4.xyzw;
  r2.w = cb0[161].y * r5.w;
  r3.yzw = cb0[167].xyz * r4.xyz + -r0.xyw;
  r3.yzw = r2.www * r3.yzw + r0.xyw;
  r4.xyz = r5.xyz * r2.www;
  r5.xyz = r5.xyz * r2.www + r0.xyw;
  r0.xyw = -r0.xyw * r4.xyz + r5.xyz;
  r0.xyw = r0.xyw + -r3.yzw;
  r4.xyz = cb0[166].www * r0.xyw + r3.yzw;
  r5.xyzw = cb0[150].xyzw * cb0[149].xyzw;
  r4.w = r1.w;
  r6.xyzw = cb0[156].xyzw + cb0[155].xyzw;
  r5.xyzw = r4.xyzw * r5.xyzw + r6.xyzw;
  r0.x = min(cb2[11].y, cb0[11].x);
  r0.x = (int)r0.x;
  r0.y = -1 + r3.x;
  r0.z = saturate(r0.z * cb3[32].x + cb3[32].y);
  r3.xyz = float3(0,0,0);
  r0.w = 0;
  while (true) {
    r1.w = cmp((uint)r0.w >= (uint)r0.x);
    if (r1.w != 0) break;
    r1.w = (uint)r0.w >> 2;
    r2.w = (int)r0.w & 3;
    r1.w = dot(cb2[r1.w+12].xyzw, icb[r2.w+0].xyzw);
    r1.w = (int)r1.w;
    r4.xyz = -v2.xyz * cb1[r1.w+0].www + cb1[r1.w+0].xyz;
    r2.w = dot(r4.xyz, r4.xyz);
    r2.w = max(6.10351562e-05, r2.w);
    r3.w = rsqrt(r2.w);
    r6.xyz = r4.xyz * r3.www;
    r2.w = cb1[r1.w+512].x * r2.w;
    r2.w = t9.SampleBias(s3_s, r2.ww, cb0[5].x).x;
    r3.w = dot(cb1[r1.w+768].xyz, r6.xyz);
    r3.w = saturate(r3.w * cb1[r1.w+512].z + cb1[r1.w+512].w);
    r3.w = r3.w * r3.w;
    r2.w = r3.w * r2.w;
    r3.w = (int)cb3[r1.w+34].w;
    r6.x = cmp((int)r3.w >= 0);
    if (r6.x != 0) {
      r6.x = cmp(0 != cb3[r1.w+34].z);
      if (r6.x != 0) {
        r6.xyz = cmp(abs(r4.zzy) >= abs(r4.xyx));
        r6.x = r6.y ? r6.x : 0;
        r7.xyz = cmp(-r4.zyx < float3(0,0,0));
        r6.yw = r7.xy ? float2(5,3) : float2(4,2);
        r7.x = r7.z ? 1.000000 : 0;
        r6.z = r6.z ? r6.w : r7.x;
        r6.x = r6.x ? r6.y : r6.z;
        r6.y = trunc(cb3[r1.w+34].w);
        r6.x = r6.y + r6.x;
        r3.w = (int)r6.x;
      }
      r3.w = (uint)r3.w << 2;
      r6.xyzw = cb3[r3.w+291].xyzw * v2.yyyy;
      r6.xyzw = cb3[r3.w+290].xyzw * v2.xxxx + r6.xyzw;
      r6.xyzw = cb3[r3.w+292].xyzw * v2.zzzz + r6.xyzw;
      r6.xyzw = cb3[r3.w+293].xyzw + r6.xyzw;
      r6.xyz = r6.xyz / r6.www;
      r3.w = cmp(0 < cb3[r1.w+34].y);
      if (r3.w != 0) {
        r3.w = cmp(1.000000 == cb3[r1.w+34].y);
        if (r3.w != 0) {
          r7.xyzw = cb3[30].xyzw + r6.xyxy;
          r8.x = t8.SampleCmpLevelZero(s2_s, r7.xy, r6.z).x;
          r8.y = t8.SampleCmpLevelZero(s2_s, r7.zw, r6.z).x;
          r7.xyzw = cb3[31].xyzw + r6.xyxy;
          r8.z = t8.SampleCmpLevelZero(s2_s, r7.xy, r6.z).x;
          r8.w = t8.SampleCmpLevelZero(s2_s, r7.zw, r6.z).x;
          r3.w = dot(r8.xyzw, float4(0.25,0.25,0.25,0.25));
        } else {
          r6.w = cmp(2.000000 == cb3[r1.w+34].y);
          if (r6.w != 0) {
            r7.xy = r6.xy * cb3[33].zw + float2(0.5,0.5);
            r7.xy = floor(r7.xy);
            r7.zw = r6.xy * cb3[33].zw + -r7.xy;
            r8.xyzw = float4(0.5,1,0.5,1) + r7.zzww;
            r9.xyzw = r8.xxzz * r8.xxzz;
            r8.xz = float2(0.0799999982,0.0799999982) * r9.yw;
            r9.xy = r9.xz * float2(0.5,0.5) + -r7.zw;
            r9.zw = float2(1,1) + -r7.zw;
            r10.xy = min(float2(0,0), r7.zw);
            r10.xy = -r10.xy * r10.xy + r9.zw;
            r7.zw = max(float2(0,0), r7.zw);
            r7.zw = -r7.zw * r7.zw + r8.yw;
            r10.xy = float2(1,1) + r10.xy;
            r7.zw = float2(1,1) + r7.zw;
            r11.xy = float2(0.159999996,0.159999996) * r9.xy;
            r9.xy = float2(0.159999996,0.159999996) * r9.zw;
            r10.xy = float2(0.159999996,0.159999996) * r10.xy;
            r12.xy = float2(0.159999996,0.159999996) * r7.zw;
            r7.zw = float2(0.159999996,0.159999996) * r8.yw;
            r11.z = r10.x;
            r11.w = r7.z;
            r9.z = r12.x;
            r9.w = r8.x;
            r13.xyzw = r11.zwxz + r9.zwxz;
            r10.z = r11.y;
            r10.w = r7.w;
            r12.z = r9.y;
            r12.w = r8.z;
            r8.xyz = r12.zyw + r10.zyw;
            r9.xyz = r9.xzw / r13.zwy;
            r9.xyz = float3(-2.5,-0.5,1.5) + r9.xyz;
            r10.xyz = r12.zyw / r8.xyz;
            r10.xyz = float3(-2.5,-0.5,1.5) + r10.xyz;
            r9.xyz = cb3[33].xxx * r9.yxz;
            r10.xyz = cb3[33].yyy * r10.xyz;
            r9.w = r10.x;
            r11.xyzw = r7.xyxy * cb3[33].xyxy + r9.ywxw;
            r7.zw = r7.xy * cb3[33].xy + r9.zw;
            r10.w = r9.y;
            r9.yw = r10.yz;
            r12.xyzw = r7.xyxy * cb3[33].xyxy + r9.xyzy;
            r10.xyzw = r7.xyxy * cb3[33].xyxy + r10.wywz;
            r9.xyzw = r7.xyxy * cb3[33].xyxy + r9.xwzw;
            r14.xyzw = r13.zwyz * r8.xxxy;
            r15.xyzw = r13.xyzw * r8.yyzz;
            r6.w = r13.y * r8.z;
            r7.x = t8.SampleCmpLevelZero(s2_s, r11.xy, r6.z).x;
            r7.y = t8.SampleCmpLevelZero(s2_s, r11.zw, r6.z).x;
            r7.y = r14.y * r7.y;
            r7.x = r14.x * r7.x + r7.y;
            r7.y = t8.SampleCmpLevelZero(s2_s, r7.zw, r6.z).x;
            r7.x = r14.z * r7.y + r7.x;
            r7.y = t8.SampleCmpLevelZero(s2_s, r10.xy, r6.z).x;
            r7.x = r14.w * r7.y + r7.x;
            r7.y = t8.SampleCmpLevelZero(s2_s, r12.xy, r6.z).x;
            r7.x = r15.x * r7.y + r7.x;
            r7.y = t8.SampleCmpLevelZero(s2_s, r12.zw, r6.z).x;
            r7.x = r15.y * r7.y + r7.x;
            r7.y = t8.SampleCmpLevelZero(s2_s, r10.zw, r6.z).x;
            r7.x = r15.z * r7.y + r7.x;
            r7.y = t8.SampleCmpLevelZero(s2_s, r9.xy, r6.z).x;
            r7.x = r15.w * r7.y + r7.x;
            r7.y = t8.SampleCmpLevelZero(s2_s, r9.zw, r6.z).x;
            r3.w = r6.w * r7.y + r7.x;
          } else {
            r7.xy = r6.xy * cb3[33].zw + float2(0.5,0.5);
            r7.xy = floor(r7.xy);
            r7.zw = r6.xy * cb3[33].zw + -r7.xy;
            r8.xyzw = float4(0.5,1,0.5,1) + r7.zzww;
            r9.xyzw = r8.xxzz * r8.xxzz;
            r10.yw = float2(0.0408160016,0.0408160016) * r9.yw;
            r8.xz = r9.xz * float2(0.5,0.5) + -r7.zw;
            r9.xy = float2(1,1) + -r7.zw;
            r9.zw = min(float2(0,0), r7.zw);
            r9.xy = -r9.zw * r9.zw + r9.xy;
            r9.zw = max(float2(0,0), r7.zw);
            r8.yw = -r9.zw * r9.zw + r8.yw;
            r9.xy = float2(2,2) + r9.xy;
            r8.yw = float2(2,2) + r8.yw;
            r11.z = 0.0816320032 * r8.y;
            r12.xyz = float3(0.0816320032,0.0816320032,0.0816320032) * r8.zxw;
            r8.xy = float2(0.0816320032,0.0816320032) * r9.xy;
            r11.x = r12.y;
            r11.yw = r7.zz * float2(-0.0816320032,0.0816320032) + float2(0.163264006,0.0816320032);
            r9.xz = r7.zz * float2(-0.0816320032,0.0816320032) + float2(0.0816320032,0.163264006);
            r9.y = r8.x;
            r9.w = r10.y;
            r11.xyzw = r11.xyzw + r9.xyzw;
            r12.yw = r7.ww * float2(-0.0816320032,0.0816320032) + float2(0.163264006,0.0816320032);
            r10.xz = r7.ww * float2(-0.0816320032,0.0816320032) + float2(0.0816320032,0.163264006);
            r10.y = r8.y;
            r8.xyzw = r12.xyzw + r10.xyzw;
            r9.xyzw = r9.xyzw / r11.xyzw;
            r9.xyzw = float4(-3.5,-1.5,0.5,2.5) + r9.xyzw;
            r10.xyzw = r10.xyzw / r8.xyzw;
            r10.xyzw = float4(-3.5,-1.5,0.5,2.5) + r10.xyzw;
            r9.xyzw = cb3[33].xxxx * r9.wxyz;
            r10.xyzw = cb3[33].yyyy * r10.xwyz;
            r12.xzw = r9.yzw;
            r12.y = r10.x;
            r13.xyzw = r7.xyxy * cb3[33].xyxy + r12.xyzy;
            r7.zw = r7.xy * cb3[33].xy + r12.wy;
            r9.y = r12.y;
            r12.y = r10.z;
            r14.xyzw = r7.xyxy * cb3[33].xyxy + r12.xyzy;
            r15.xy = r7.xy * cb3[33].xy + r12.wy;
            r9.z = r12.y;
            r16.xyzw = r7.xyxy * cb3[33].xyxy + r9.xyxz;
            r12.y = r10.w;
            r17.xyzw = r7.xyxy * cb3[33].xyxy + r12.xyzy;
            r9.yz = r7.xy * cb3[33].xy + r12.wy;
            r9.w = r12.y;
            r15.zw = r7.xy * cb3[33].xy + r9.xw;
            r10.xzw = r12.xzw;
            r12.xyzw = r7.xyxy * cb3[33].xyxy + r10.xyzy;
            r10.zw = r7.xy * cb3[33].xy + r10.wy;
            r10.x = r9.x;
            r7.xy = r7.xy * cb3[33].xy + r10.xy;
            r18.xyzw = r11.xyzw * r8.xxxx;
            r19.xyzw = r11.xyzw * r8.yyyy;
            r20.xyzw = r11.xyzw * r8.zzzz;
            r8.xyzw = r11.xyzw * r8.wwww;
            r6.w = t8.SampleCmpLevelZero(s2_s, r13.xy, r6.z).x;
            r9.x = t8.SampleCmpLevelZero(s2_s, r13.zw, r6.z).x;
            r9.x = r18.y * r9.x;
            r6.w = r18.x * r6.w + r9.x;
            r7.z = t8.SampleCmpLevelZero(s2_s, r7.zw, r6.z).x;
            r6.w = r18.z * r7.z + r6.w;
            r7.z = t8.SampleCmpLevelZero(s2_s, r16.xy, r6.z).x;
            r6.w = r18.w * r7.z + r6.w;
            r7.z = t8.SampleCmpLevelZero(s2_s, r14.xy, r6.z).x;
            r6.w = r19.x * r7.z + r6.w;
            r7.z = t8.SampleCmpLevelZero(s2_s, r14.zw, r6.z).x;
            r6.w = r19.y * r7.z + r6.w;
            r7.z = t8.SampleCmpLevelZero(s2_s, r15.xy, r6.z).x;
            r6.w = r19.z * r7.z + r6.w;
            r7.z = t8.SampleCmpLevelZero(s2_s, r16.zw, r6.z).x;
            r6.w = r19.w * r7.z + r6.w;
            r7.z = t8.SampleCmpLevelZero(s2_s, r17.xy, r6.z).x;
            r6.w = r20.x * r7.z + r6.w;
            r7.z = t8.SampleCmpLevelZero(s2_s, r17.zw, r6.z).x;
            r6.w = r20.y * r7.z + r6.w;
            r7.z = t8.SampleCmpLevelZero(s2_s, r9.yz, r6.z).x;
            r6.w = r20.z * r7.z + r6.w;
            r7.z = t8.SampleCmpLevelZero(s2_s, r15.zw, r6.z).x;
            r6.w = r20.w * r7.z + r6.w;
            r7.z = t8.SampleCmpLevelZero(s2_s, r12.xy, r6.z).x;
            r6.w = r8.x * r7.z + r6.w;
            r7.z = t8.SampleCmpLevelZero(s2_s, r12.zw, r6.z).x;
            r6.w = r8.y * r7.z + r6.w;
            r7.z = t8.SampleCmpLevelZero(s2_s, r10.zw, r6.z).x;
            r6.w = r8.z * r7.z + r6.w;
            r7.x = t8.SampleCmpLevelZero(s2_s, r7.xy, r6.z).x;
            r3.w = r8.w * r7.x + r6.w;
          }
        }
      } else {
        r3.w = t8.SampleCmpLevelZero(s2_s, r6.xy, r6.z).x;
      }
      r6.x = -cb3[r1.w+34].x + 1;
      r3.w = r3.w * cb3[r1.w+34].x + r6.x;
      r6.x = cmp(0 >= r6.z);
      r6.y = cmp(r6.z >= 1);
      r6.x = (int)r6.y | (int)r6.x;
      r3.w = r6.x ? 1 : r3.w;
    } else {
      r3.w = 1;
    }
    r6.x = dot(r0.yyyy, cb1[r1.w+1024].xyzw);
    r6.x = 1 + r6.x;
    r6.y = 1 + -r3.w;
    r3.w = r0.z * r6.y + r3.w;
    r3.w = min(r3.w, r6.x);
    r4.x = saturate(dot(r2.xyz, r4.xyz));
    r4.x = r4.x * cb0[160].z + cb0[160].w;
    r6.xyz = cb1[r1.w+256].xyz * r1.xyz;
    r1.w = r4.x * r2.w;
    r1.w = r1.w * r3.w;
    r3.xyz = r6.xyz * r1.www + r3.xyz;
    r0.w = (int)r0.w + 1;
  }
  r0.xyz = r3.xyz * r4.www;
  r0.w = 0;
  o0.xyzw = r5.xyzw + r0.xyzw;
  return;
}