// ---- Created with 3Dmigoto v1.4.1 on Tue Jul  1 17:34:29 2025
Texture2D<float4> t7 : register(t7);

Texture2D<float4> t6 : register(t6);

Texture2D<float4> t5 : register(t5);

Texture2D<float4> t4 : register(t4);

Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s7_s : register(s7);

SamplerState s6_s : register(s6);

SamplerState s5_s : register(s5);

SamplerState s4_s : register(s4);

SamplerState s3_s : register(s3);

SamplerComparisonState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb3 : register(b3)
{
  float4 cb3[6];
}

cbuffer cb2 : register(b2)
{
  float4 cb2[30];
}

cbuffer cb1 : register(b1)
{
  float4 cb1[24];
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
  float4 v3 : TEXCOORD5,
  float4 v4 : COLOR0,
  float4 v5 : TEXCOORD6,
  float4 v6 : TEXCOORD7,
  float4 v7 : TEXCOORD8,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyz = -cb2[20].xyz + v2.xyz;
  r1.xyz = -cb2[21].xyz + v2.xyz;
  r2.xyz = -cb2[22].xyz + v2.xyz;
  r3.xyz = -cb2[23].xyz + v2.xyz;
  r0.x = dot(r0.xyz, r0.xyz);
  r0.y = dot(r1.xyz, r1.xyz);
  r0.z = dot(r2.xyz, r2.xyz);
  r0.w = dot(r3.xyz, r3.xyz);
  r0.xyzw = cmp(r0.xyzw < cb2[24].xyzw);
  r1.xyzw = r0.xyzw ? float4(1,1,1,1) : 0;
  r0.xyz = r0.xyz ? float3(-1,-1,-1) : float3(-0,-0,-0);
  r0.xyz = r1.yzw + r0.xyz;
  r1.yzw = max(float3(0,0,0), r0.xyz);
  r0.x = dot(r1.xyzw, float4(4,3,2,1));
  r0.x = 4 + -r0.x;
  r0.x = (uint)r0.x;
  r0.x = (uint)r0.x << 2;
  r0.yzw = cb2[r0.x+1].xyz * v2.yyy;
  r0.yzw = cb2[r0.x+0].xyz * v2.xxx + r0.yzw;
  r0.yzw = cb2[r0.x+2].xyz * v2.zzz + r0.yzw;
  r0.xyz = cb2[r0.x+3].xyz + r0.yzw;
  r1.xy = v3.xy * cb3[0].xy + cb3[0].zw;
  r1.xyzw = t0.Sample(s6_s, r1.xy).xyzw;

  //r1.xyz = ApplyReverseReinhard(r1.xyz);

  r1.xyzw = v4.xyzw * r1.xyzw;
  r1.xyzw = cb3[1].xyzw * r1.xyzw;
  r0.w = dot(v5.xyz, v5.xyz);
  r0.w = rsqrt(r0.w);
  r2.xyz = v5.xyz * r0.www;
  r0.w = dot(cb0[6].xyz, r2.xyz);
  r2.xy = v3.zw * cb1[23].xy + cb1[23].zw;
  r3.xyz = t5.SampleBias(s0_s, r2.xy, cb0[5].x).xyz;
  r2.xyzw = t6.SampleBias(s1_s, r2.xy, cb0[5].x).xyzw;
  r3.w = cmp(cb3[5].z == 1.000000);
  r4.x = cmp(0 < cb2[27].y);
  if (r4.x != 0) {
    r4.x = cmp(cb2[27].y == 1.000000);
    if (r4.x != 0) {
      r4.xyzw = cb2[25].xyzw + r0.xyxy;
      r5.x = t7.SampleCmpLevelZero(s2_s, r4.xy, r0.z).x;
      r5.y = t7.SampleCmpLevelZero(s2_s, r4.zw, r0.z).x;
      r4.xyzw = cb2[26].xyzw + r0.xyxy;
      r5.z = t7.SampleCmpLevelZero(s2_s, r4.xy, r0.z).x;
      r5.w = t7.SampleCmpLevelZero(s2_s, r4.zw, r0.z).x;
      r4.x = dot(r5.xyzw, float4(0.25,0.25,0.25,0.25));
    } else {
      r4.y = cmp(cb2[27].y == 2.000000);
      if (r4.y != 0) {
        r4.yz = r0.xy * cb2[28].zw + float2(0.5,0.5);
        r4.yz = floor(r4.yz);
        r5.xy = r0.xy * cb2[28].zw + -r4.yz;
        r6.xyzw = float4(0.5,1,0.5,1) + r5.xxyy;
        r7.xyzw = r6.xxzz * r6.xxzz;
        r5.zw = float2(0.0799999982,0.0799999982) * r7.yw;
        r6.xz = r7.xz * float2(0.5,0.5) + -r5.xy;
        r7.xy = float2(1,1) + -r5.xy;
        r7.zw = min(float2(0,0), r5.xy);
        r7.zw = -r7.zw * r7.zw + r7.xy;
        r5.xy = max(float2(0,0), r5.xy);
        r5.xy = -r5.xy * r5.xy + r6.yw;
        r7.zw = float2(1,1) + r7.zw;
        r5.xy = float2(1,1) + r5.xy;
        r8.xy = float2(0.159999996,0.159999996) * r6.xz;
        r9.xy = float2(0.159999996,0.159999996) * r7.xy;
        r7.xy = float2(0.159999996,0.159999996) * r7.zw;
        r10.xy = float2(0.159999996,0.159999996) * r5.xy;
        r5.xy = float2(0.159999996,0.159999996) * r6.yw;
        r8.z = r7.x;
        r8.w = r5.x;
        r9.z = r10.x;
        r9.w = r5.z;
        r6.xyzw = r9.zwxz + r8.zwxz;
        r7.z = r8.y;
        r7.w = r5.y;
        r10.z = r9.y;
        r10.w = r5.w;
        r5.xyz = r10.zyw + r7.zyw;
        r7.xyz = r9.xzw / r6.zwy;
        r7.xyz = float3(-2.5,-0.5,1.5) + r7.xyz;
        r8.xyz = r10.zyw / r5.xyz;
        r8.xyz = float3(-2.5,-0.5,1.5) + r8.xyz;
        r7.xyz = cb2[28].xxx * r7.yxz;
        r8.xyz = cb2[28].yyy * r8.xyz;
        r7.w = r8.x;
        r9.xyzw = r4.yzyz * cb2[28].xyxy + r7.ywxw;
        r10.xy = r4.yz * cb2[28].xy + r7.zw;
        r8.w = r7.y;
        r7.yw = r8.yz;
        r11.xyzw = r4.yzyz * cb2[28].xyxy + r7.xyzy;
        r8.xyzw = r4.yzyz * cb2[28].xyxy + r8.wywz;
        r7.xyzw = r4.yzyz * cb2[28].xyxy + r7.xwzw;
        r12.xyzw = r6.zwyz * r5.xxxy;
        r13.xyzw = r6.xyzw * r5.yyzz;
        r4.y = r6.y * r5.z;
        r4.z = t7.SampleCmpLevelZero(s2_s, r9.xy, r0.z).x;
        r4.w = t7.SampleCmpLevelZero(s2_s, r9.zw, r0.z).x;
        r4.w = r12.y * r4.w;
        r4.z = r12.x * r4.z + r4.w;
        r4.w = t7.SampleCmpLevelZero(s2_s, r10.xy, r0.z).x;
        r4.z = r12.z * r4.w + r4.z;
        r4.w = t7.SampleCmpLevelZero(s2_s, r8.xy, r0.z).x;
        r4.z = r12.w * r4.w + r4.z;
        r4.w = t7.SampleCmpLevelZero(s2_s, r11.xy, r0.z).x;
        r4.z = r13.x * r4.w + r4.z;
        r4.w = t7.SampleCmpLevelZero(s2_s, r11.zw, r0.z).x;
        r4.z = r13.y * r4.w + r4.z;
        r4.w = t7.SampleCmpLevelZero(s2_s, r8.zw, r0.z).x;
        r4.z = r13.z * r4.w + r4.z;
        r4.w = t7.SampleCmpLevelZero(s2_s, r7.xy, r0.z).x;
        r4.z = r13.w * r4.w + r4.z;
        r4.w = t7.SampleCmpLevelZero(s2_s, r7.zw, r0.z).x;
        r4.x = r4.y * r4.w + r4.z;
      } else {
        r4.yz = r0.xy * cb2[28].zw + float2(0.5,0.5);
        r4.yz = floor(r4.yz);
        r5.xy = r0.xy * cb2[28].zw + -r4.yz;
        r6.xyzw = float4(0.5,1,0.5,1) + r5.xxyy;
        r7.xyzw = r6.xxzz * r6.xxzz;
        r8.yw = float2(0.0408160016,0.0408160016) * r7.yw;
        r5.zw = r7.xz * float2(0.5,0.5) + -r5.xy;
        r6.xz = float2(1,1) + -r5.xy;
        r7.xy = min(float2(0,0), r5.xy);
        r6.xz = -r7.xy * r7.xy + r6.xz;
        r7.xy = max(float2(0,0), r5.xy);
        r6.yw = -r7.xy * r7.xy + r6.yw;
        r6.xyzw = float4(2,2,2,2) + r6.xyzw;
        r7.z = 0.0816320032 * r6.y;
        r9.xy = float2(0.0816320032,0.0816320032) * r5.wz;
        r5.zw = float2(0.0816320032,0.0816320032) * r6.xz;
        r9.z = 0.0816320032 * r6.w;
        r7.x = r9.y;
        r7.yw = r5.xx * float2(-0.0816320032,0.0816320032) + float2(0.163264006,0.0816320032);
        r6.xz = r5.xx * float2(-0.0816320032,0.0816320032) + float2(0.0816320032,0.163264006);
        r6.y = r5.z;
        r6.w = r8.y;
        r7.xyzw = r7.xyzw + r6.xyzw;
        r9.yw = r5.yy * float2(-0.0816320032,0.0816320032) + float2(0.163264006,0.0816320032);
        r8.xz = r5.yy * float2(-0.0816320032,0.0816320032) + float2(0.0816320032,0.163264006);
        r8.y = r5.w;
        r5.xyzw = r9.xyzw + r8.xyzw;
        r6.xyzw = r6.xyzw / r7.xyzw;
        r6.xyzw = float4(-3.5,-1.5,0.5,2.5) + r6.xyzw;
        r8.xyzw = r8.xyzw / r5.xyzw;
        r8.xyzw = float4(-3.5,-1.5,0.5,2.5) + r8.xyzw;
        r6.xyzw = cb2[28].xxxx * r6.wxyz;
        r8.xyzw = cb2[28].yyyy * r8.xwyz;
        r9.xzw = r6.yzw;
        r9.y = r8.x;
        r10.xyzw = r4.yzyz * cb2[28].xyxy + r9.xyzy;
        r11.xy = r4.yz * cb2[28].xy + r9.wy;
        r6.y = r9.y;
        r9.y = r8.z;
        r12.xyzw = r4.yzyz * cb2[28].xyxy + r9.xyzy;
        r11.zw = r4.yz * cb2[28].xy + r9.wy;
        r6.z = r9.y;
        r13.xyzw = r4.yzyz * cb2[28].xyxy + r6.xyxz;
        r9.y = r8.w;
        r14.xyzw = r4.yzyz * cb2[28].xyxy + r9.xyzy;
        r6.yz = r4.yz * cb2[28].xy + r9.wy;
        r6.w = r9.y;
        r15.xy = r4.yz * cb2[28].xy + r6.xw;
        r8.xzw = r9.xzw;
        r9.xyzw = r4.yzyz * cb2[28].xyxy + r8.xyzy;
        r8.zw = r4.yz * cb2[28].xy + r8.wy;
        r8.x = r6.x;
        r4.yz = r4.yz * cb2[28].xy + r8.xy;
        r16.xyzw = r7.xyzw * r5.xxxx;
        r17.xyzw = r7.xyzw * r5.yyyy;
        r18.xyzw = r7.xyzw * r5.zzzz;
        r5.xyzw = r7.xyzw * r5.wwww;
        r4.w = t7.SampleCmpLevelZero(s2_s, r10.xy, r0.z).x;
        r6.x = t7.SampleCmpLevelZero(s2_s, r10.zw, r0.z).x;
        r6.x = r16.y * r6.x;
        r4.w = r16.x * r4.w + r6.x;
        r6.x = t7.SampleCmpLevelZero(s2_s, r11.xy, r0.z).x;
        r4.w = r16.z * r6.x + r4.w;
        r6.x = t7.SampleCmpLevelZero(s2_s, r13.xy, r0.z).x;
        r4.w = r16.w * r6.x + r4.w;
        r6.x = t7.SampleCmpLevelZero(s2_s, r12.xy, r0.z).x;
        r4.w = r17.x * r6.x + r4.w;
        r6.x = t7.SampleCmpLevelZero(s2_s, r12.zw, r0.z).x;
        r4.w = r17.y * r6.x + r4.w;
        r6.x = t7.SampleCmpLevelZero(s2_s, r11.zw, r0.z).x;
        r4.w = r17.z * r6.x + r4.w;
        r6.x = t7.SampleCmpLevelZero(s2_s, r13.zw, r0.z).x;
        r4.w = r17.w * r6.x + r4.w;
        r6.x = t7.SampleCmpLevelZero(s2_s, r14.xy, r0.z).x;
        r4.w = r18.x * r6.x + r4.w;
        r6.x = t7.SampleCmpLevelZero(s2_s, r14.zw, r0.z).x;
        r4.w = r18.y * r6.x + r4.w;
        r6.x = t7.SampleCmpLevelZero(s2_s, r6.yz, r0.z).x;
        r4.w = r18.z * r6.x + r4.w;
        r6.x = t7.SampleCmpLevelZero(s2_s, r15.xy, r0.z).x;
        r4.w = r18.w * r6.x + r4.w;
        r6.x = t7.SampleCmpLevelZero(s2_s, r9.xy, r0.z).x;
        r4.w = r5.x * r6.x + r4.w;
        r5.x = t7.SampleCmpLevelZero(s2_s, r9.zw, r0.z).x;
        r4.w = r5.y * r5.x + r4.w;
        r5.x = t7.SampleCmpLevelZero(s2_s, r8.zw, r0.z).x;
        r4.w = r5.z * r5.x + r4.w;
        r4.y = t7.SampleCmpLevelZero(s2_s, r4.yz, r0.z).x;
        r4.x = r5.w * r4.y + r4.w;
      }
    }
  } else {
    r4.x = t7.SampleCmpLevelZero(s2_s, r0.xy, r0.z).x;
  }
  r0.x = 1 + -cb2[27].x;
  r0.x = r4.x * cb2[27].x + r0.x;
  r0.y = cmp(0 >= r0.z);
  r0.z = cmp(r0.z >= 1);
  r0.y = (int)r0.z | (int)r0.y;
  r0.x = r0.y ? 1 : r0.x;
  r2.xyzw = float4(-1,-1,-1,-1) + r2.xyzw;
  r2.xyzw = r3.wwww ? r2.xyzw : 0;
  r0.y = dot(r2.xyzw, cb0[8].xyzw);
  r0.y = 1 + r0.y;
  r2.xyz = -cb0[22].xyz + v2.xyz;
  r0.z = dot(r2.xyz, r2.xyz);
  r0.z = saturate(r0.z * cb2[27].z + cb2[27].w);
  r2.x = cmp(cb2[29].x >= 1);
  r0.z = r2.x ? 0 : r0.z;
  r2.x = 1 + -r0.x;
  r0.x = r0.z * r2.x + r0.x;
  r0.x = min(r0.x, r0.y);
  r2.xyz = float3(1,1,1) + -cb0[153].xyz;
  r0.xyz = r0.xxx * r2.xyz + cb0[153].xyz;
  r2.xyz = cb0[7].xyz * r1.xyz;
  r0.w = max(0, r0.w);
  r0.w = r0.w * cb0[160].x + cb0[160].y;
  r3.xyz = r3.xyz * r1.xyz;
  r2.xyz = r2.xyz * r0.www + r3.xyz;
  r3.xyz = t1.Sample(s7_s, v6.xy).xyz;
  r3.xyz = cb3[2].xyz * r3.xyz;
  r3.xyz = cb3[2].www * r3.xyz;
  r0.xyz = r2.xyz * r0.xyz + r3.xyz;
  r2.xyzw = t2.Sample(s3_s, v7.zw).xyzw;
  r2.xyzw = cb0[163].xyzw * r2.xyzw;
  r2.xyz = r2.xyz * r2.www;
  r3.xyz = cb0[161].yyy * r2.xyz;
  r4.xyzw = t3.Sample(s4_s, v7.xy).xyzw;
  r0.w = cb0[165].w * r4.w;
  r2.xyz = r2.xyz * cb0[161].yyy + r0.xyz;
  r0.xyz = -r0.xyz * r3.xyz + r2.xyz;
  r0.w = cb0[161].y * r0.w;
  r2.xyz = cb0[165].xyz * r4.xyz + -r0.xyz;
  r0.xyz = r0.www * r2.xyz + r0.xyz;
  r2.xyzw = t4.Sample(s5_s, v6.zw).xyzw;
  r3.xyzw = cb0[167].xyzw * r2.xyzw;
  r0.w = cb0[161].y * r3.w;
  r2.xyz = cb0[167].xyz * r2.xyz + -r0.xyz;
  r2.xyz = r0.www * r2.xyz + r0.xyz;
  r4.xyz = r3.xyz * r0.www;
  r3.xyz = r3.xyz * r0.www + r0.xyz;
  r0.xyz = -r0.xyz * r4.xyz + r3.xyz;
  r0.xyz = r0.xyz + -r2.xyz;
  r1.xyz = cb0[166].www * r0.xyz + r2.xyz;
  r0.xyzw = cb0[150].xyzw * cb0[149].xyzw;
  r2.xyzw = cb0[156].xyzw + cb0[155].xyzw;
  o0.xyzw = r1.xyzw * r0.xyzw + r2.xyzw;
  return;
}