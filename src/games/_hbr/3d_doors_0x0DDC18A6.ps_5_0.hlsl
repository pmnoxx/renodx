// ---- Created with 3Dmigoto v1.4.1 on Tue Jul  1 17:32:21 2025
Texture2D<float4> t5 : register(t5);

Texture2D<float4> t4 : register(t4);

Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s5_s : register(s5);

SamplerState s4_s : register(s4);

SamplerState s3_s : register(s3);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerComparisonState s0_s : register(s0);

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
  float4 cb1[32];
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
  float4 r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17;
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
  r1.xyzw = t0.Sample(s4_s, r1.xy).xyzw;

 // r1.xyz = ApplyReverseReinhard(r1.xyz);

  r1.xyzw = v4.xyzw * r1.xyzw;
  r1.xyzw = cb3[1].xyzw * r1.xyzw;
  r0.w = dot(v5.xyz, v5.xyz);
  r0.w = rsqrt(r0.w);
  r2.xyz = v5.xyz * r0.www;
  r0.w = dot(cb0[6].xyz, r2.xyz);
  r2.w = 1;
  r3.x = dot(cb1[25].xyzw, r2.xyzw);
  r3.y = dot(cb1[26].xyzw, r2.xyzw);
  r3.z = dot(cb1[27].xyzw, r2.xyzw);
  r4.xyzw = r2.xyzz * r2.yzzx;
  r5.x = dot(cb1[28].xyzw, r4.xyzw);
  r5.y = dot(cb1[29].xyzw, r4.xyzw);
  r5.z = dot(cb1[30].xyzw, r4.xyzw);
  r2.y = r2.y * r2.y;
  r2.x = r2.x * r2.x + -r2.y;
  r2.xyz = cb1[31].xyz * r2.xxx + r5.xyz;
  r2.xyz = r3.xyz + r2.xyz;
  r3.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r2.xyz);
  r4.xyz = float3(12.9232101,12.9232101,12.9232101) * r2.xyz;
  r2.xyz = log2(abs(r2.xyz));
  r2.xyz = float3(0.416666657,0.416666657,0.416666657) * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r2.xyz = r2.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
  r2.xyz = r3.xyz ? r4.xyz : r2.xyz;
  r2.xyz = cb0[15].yyy * r2.xyz;
  r2.xyz = max(float3(0,0,0), r2.xyz);
  r2.w = cmp(cb3[5].z == 1.000000);
  r3.x = cmp(0 < cb2[27].y);
  if (r3.x != 0) {
    r3.x = cmp(cb2[27].y == 1.000000);
    if (r3.x != 0) {
      r3.xyzw = cb2[25].xyzw + r0.xyxy;
      r4.x = t5.SampleCmpLevelZero(s0_s, r3.xy, r0.z).x;
      r4.y = t5.SampleCmpLevelZero(s0_s, r3.zw, r0.z).x;
      r3.xyzw = cb2[26].xyzw + r0.xyxy;
      r4.z = t5.SampleCmpLevelZero(s0_s, r3.xy, r0.z).x;
      r4.w = t5.SampleCmpLevelZero(s0_s, r3.zw, r0.z).x;
      r3.x = dot(r4.xyzw, float4(0.25,0.25,0.25,0.25));
    } else {
      r3.y = cmp(cb2[27].y == 2.000000);
      if (r3.y != 0) {
        r3.yz = r0.xy * cb2[28].zw + float2(0.5,0.5);
        r3.yz = floor(r3.yz);
        r4.xy = r0.xy * cb2[28].zw + -r3.yz;
        r5.xyzw = float4(0.5,1,0.5,1) + r4.xxyy;
        r6.xyzw = r5.xxzz * r5.xxzz;
        r4.zw = float2(0.0799999982,0.0799999982) * r6.yw;
        r5.xz = r6.xz * float2(0.5,0.5) + -r4.xy;
        r6.xy = float2(1,1) + -r4.xy;
        r6.zw = min(float2(0,0), r4.xy);
        r6.zw = -r6.zw * r6.zw + r6.xy;
        r4.xy = max(float2(0,0), r4.xy);
        r4.xy = -r4.xy * r4.xy + r5.yw;
        r6.zw = float2(1,1) + r6.zw;
        r4.xy = float2(1,1) + r4.xy;
        r7.xy = float2(0.159999996,0.159999996) * r5.xz;
        r8.xy = float2(0.159999996,0.159999996) * r6.xy;
        r6.xy = float2(0.159999996,0.159999996) * r6.zw;
        r9.xy = float2(0.159999996,0.159999996) * r4.xy;
        r4.xy = float2(0.159999996,0.159999996) * r5.yw;
        r7.z = r6.x;
        r7.w = r4.x;
        r8.z = r9.x;
        r8.w = r4.z;
        r5.xyzw = r8.zwxz + r7.zwxz;
        r6.z = r7.y;
        r6.w = r4.y;
        r9.z = r8.y;
        r9.w = r4.w;
        r4.xyz = r9.zyw + r6.zyw;
        r6.xyz = r8.xzw / r5.zwy;
        r6.xyz = float3(-2.5,-0.5,1.5) + r6.xyz;
        r7.xyz = r9.zyw / r4.xyz;
        r7.xyz = float3(-2.5,-0.5,1.5) + r7.xyz;
        r6.xyz = cb2[28].xxx * r6.yxz;
        r7.xyz = cb2[28].yyy * r7.xyz;
        r6.w = r7.x;
        r8.xyzw = r3.yzyz * cb2[28].xyxy + r6.ywxw;
        r9.xy = r3.yz * cb2[28].xy + r6.zw;
        r7.w = r6.y;
        r6.yw = r7.yz;
        r10.xyzw = r3.yzyz * cb2[28].xyxy + r6.xyzy;
        r7.xyzw = r3.yzyz * cb2[28].xyxy + r7.wywz;
        r6.xyzw = r3.yzyz * cb2[28].xyxy + r6.xwzw;
        r11.xyzw = r5.zwyz * r4.xxxy;
        r12.xyzw = r5.xyzw * r4.yyzz;
        r3.y = r5.y * r4.z;
        r3.z = t5.SampleCmpLevelZero(s0_s, r8.xy, r0.z).x;
        r3.w = t5.SampleCmpLevelZero(s0_s, r8.zw, r0.z).x;
        r3.w = r11.y * r3.w;
        r3.z = r11.x * r3.z + r3.w;
        r3.w = t5.SampleCmpLevelZero(s0_s, r9.xy, r0.z).x;
        r3.z = r11.z * r3.w + r3.z;
        r3.w = t5.SampleCmpLevelZero(s0_s, r7.xy, r0.z).x;
        r3.z = r11.w * r3.w + r3.z;
        r3.w = t5.SampleCmpLevelZero(s0_s, r10.xy, r0.z).x;
        r3.z = r12.x * r3.w + r3.z;
        r3.w = t5.SampleCmpLevelZero(s0_s, r10.zw, r0.z).x;
        r3.z = r12.y * r3.w + r3.z;
        r3.w = t5.SampleCmpLevelZero(s0_s, r7.zw, r0.z).x;
        r3.z = r12.z * r3.w + r3.z;
        r3.w = t5.SampleCmpLevelZero(s0_s, r6.xy, r0.z).x;
        r3.z = r12.w * r3.w + r3.z;
        r3.w = t5.SampleCmpLevelZero(s0_s, r6.zw, r0.z).x;
        r3.x = r3.y * r3.w + r3.z;
      } else {
        r3.yz = r0.xy * cb2[28].zw + float2(0.5,0.5);
        r3.yz = floor(r3.yz);
        r4.xy = r0.xy * cb2[28].zw + -r3.yz;
        r5.xyzw = float4(0.5,1,0.5,1) + r4.xxyy;
        r6.xyzw = r5.xxzz * r5.xxzz;
        r7.yw = float2(0.0408160016,0.0408160016) * r6.yw;
        r4.zw = r6.xz * float2(0.5,0.5) + -r4.xy;
        r5.xz = float2(1,1) + -r4.xy;
        r6.xy = min(float2(0,0), r4.xy);
        r5.xz = -r6.xy * r6.xy + r5.xz;
        r6.xy = max(float2(0,0), r4.xy);
        r5.yw = -r6.xy * r6.xy + r5.yw;
        r5.xyzw = float4(2,2,2,2) + r5.xyzw;
        r6.z = 0.0816320032 * r5.y;
        r8.xy = float2(0.0816320032,0.0816320032) * r4.wz;
        r4.zw = float2(0.0816320032,0.0816320032) * r5.xz;
        r8.z = 0.0816320032 * r5.w;
        r6.x = r8.y;
        r6.yw = r4.xx * float2(-0.0816320032,0.0816320032) + float2(0.163264006,0.0816320032);
        r5.xz = r4.xx * float2(-0.0816320032,0.0816320032) + float2(0.0816320032,0.163264006);
        r5.y = r4.z;
        r5.w = r7.y;
        r6.xyzw = r6.xyzw + r5.xyzw;
        r8.yw = r4.yy * float2(-0.0816320032,0.0816320032) + float2(0.163264006,0.0816320032);
        r7.xz = r4.yy * float2(-0.0816320032,0.0816320032) + float2(0.0816320032,0.163264006);
        r7.y = r4.w;
        r4.xyzw = r8.xyzw + r7.xyzw;
        r5.xyzw = r5.xyzw / r6.xyzw;
        r5.xyzw = float4(-3.5,-1.5,0.5,2.5) + r5.xyzw;
        r7.xyzw = r7.xyzw / r4.xyzw;
        r7.xyzw = float4(-3.5,-1.5,0.5,2.5) + r7.xyzw;
        r5.xyzw = cb2[28].xxxx * r5.wxyz;
        r7.xyzw = cb2[28].yyyy * r7.xwyz;
        r8.xzw = r5.yzw;
        r8.y = r7.x;
        r9.xyzw = r3.yzyz * cb2[28].xyxy + r8.xyzy;
        r10.xy = r3.yz * cb2[28].xy + r8.wy;
        r5.y = r8.y;
        r8.y = r7.z;
        r11.xyzw = r3.yzyz * cb2[28].xyxy + r8.xyzy;
        r10.zw = r3.yz * cb2[28].xy + r8.wy;
        r5.z = r8.y;
        r12.xyzw = r3.yzyz * cb2[28].xyxy + r5.xyxz;
        r8.y = r7.w;
        r13.xyzw = r3.yzyz * cb2[28].xyxy + r8.xyzy;
        r5.yz = r3.yz * cb2[28].xy + r8.wy;
        r5.w = r8.y;
        r14.xy = r3.yz * cb2[28].xy + r5.xw;
        r7.xzw = r8.xzw;
        r8.xyzw = r3.yzyz * cb2[28].xyxy + r7.xyzy;
        r7.zw = r3.yz * cb2[28].xy + r7.wy;
        r7.x = r5.x;
        r3.yz = r3.yz * cb2[28].xy + r7.xy;
        r15.xyzw = r6.xyzw * r4.xxxx;
        r16.xyzw = r6.xyzw * r4.yyyy;
        r17.xyzw = r6.xyzw * r4.zzzz;
        r4.xyzw = r6.xyzw * r4.wwww;
        r3.w = t5.SampleCmpLevelZero(s0_s, r9.xy, r0.z).x;
        r5.x = t5.SampleCmpLevelZero(s0_s, r9.zw, r0.z).x;
        r5.x = r15.y * r5.x;
        r3.w = r15.x * r3.w + r5.x;
        r5.x = t5.SampleCmpLevelZero(s0_s, r10.xy, r0.z).x;
        r3.w = r15.z * r5.x + r3.w;
        r5.x = t5.SampleCmpLevelZero(s0_s, r12.xy, r0.z).x;
        r3.w = r15.w * r5.x + r3.w;
        r5.x = t5.SampleCmpLevelZero(s0_s, r11.xy, r0.z).x;
        r3.w = r16.x * r5.x + r3.w;
        r5.x = t5.SampleCmpLevelZero(s0_s, r11.zw, r0.z).x;
        r3.w = r16.y * r5.x + r3.w;
        r5.x = t5.SampleCmpLevelZero(s0_s, r10.zw, r0.z).x;
        r3.w = r16.z * r5.x + r3.w;
        r5.x = t5.SampleCmpLevelZero(s0_s, r12.zw, r0.z).x;
        r3.w = r16.w * r5.x + r3.w;
        r5.x = t5.SampleCmpLevelZero(s0_s, r13.xy, r0.z).x;
        r3.w = r17.x * r5.x + r3.w;
        r5.x = t5.SampleCmpLevelZero(s0_s, r13.zw, r0.z).x;
        r3.w = r17.y * r5.x + r3.w;
        r5.x = t5.SampleCmpLevelZero(s0_s, r5.yz, r0.z).x;
        r3.w = r17.z * r5.x + r3.w;
        r5.x = t5.SampleCmpLevelZero(s0_s, r14.xy, r0.z).x;
        r3.w = r17.w * r5.x + r3.w;
        r5.x = t5.SampleCmpLevelZero(s0_s, r8.xy, r0.z).x;
        r3.w = r4.x * r5.x + r3.w;
        r4.x = t5.SampleCmpLevelZero(s0_s, r8.zw, r0.z).x;
        r3.w = r4.y * r4.x + r3.w;
        r4.x = t5.SampleCmpLevelZero(s0_s, r7.zw, r0.z).x;
        r3.w = r4.z * r4.x + r3.w;
        r3.y = t5.SampleCmpLevelZero(s0_s, r3.yz, r0.z).x;
        r3.x = r4.w * r3.y + r3.w;
      }
    }
  } else {
    r3.x = t5.SampleCmpLevelZero(s0_s, r0.xy, r0.z).x;
  }
  r0.x = 1 + -cb2[27].x;
  r0.x = r3.x * cb2[27].x + r0.x;
  r0.y = cmp(0 >= r0.z);
  r0.z = cmp(r0.z >= 1);
  r0.y = (int)r0.z | (int)r0.y;
  r0.x = r0.y ? 1 : r0.x;
  r3.xyzw = float4(-1,-1,-1,-1) + cb1[14].xyzw;
  r3.xyzw = r2.wwww ? r3.xyzw : 0;
  r0.y = dot(r3.xyzw, cb0[8].xyzw);
  r0.y = 1 + r0.y;
  r3.xyz = -cb0[22].xyz + v2.xyz;
  r0.z = dot(r3.xyz, r3.xyz);
  r0.z = saturate(r0.z * cb2[27].z + cb2[27].w);
  r2.w = cmp(cb2[29].x >= 1);
  r0.z = r2.w ? 0 : r0.z;
  r2.w = 1 + -r0.x;
  r0.x = r0.z * r2.w + r0.x;
  r0.x = min(r0.x, r0.y);
  r3.xyz = float3(1,1,1) + -cb0[153].xyz;
  r0.xyz = r0.xxx * r3.xyz + cb0[153].xyz;
  r3.xyz = cb0[7].xyz * r1.xyz;
  r0.w = max(0, r0.w);
  r0.w = r0.w * cb0[160].x + cb0[160].y;
  r2.xyz = r2.xyz * r1.xyz;
  r2.xyz = r3.xyz * r0.www + r2.xyz;
  r3.xyz = t1.Sample(s5_s, v6.xy).xyz;
  r3.xyz = cb3[2].xyz * r3.xyz;
  r3.xyz = cb3[2].www * r3.xyz;
  r0.xyz = r2.xyz * r0.xyz + r3.xyz;
  r2.xyzw = t2.Sample(s1_s, v7.zw).xyzw;
  r2.xyzw = cb0[163].xyzw * r2.xyzw;
  r2.xyz = r2.xyz * r2.www;
  r3.xyz = cb0[161].yyy * r2.xyz;
  r4.xyzw = t3.Sample(s2_s, v7.xy).xyzw;
  r0.w = cb0[165].w * r4.w;
  r2.xyz = r2.xyz * cb0[161].yyy + r0.xyz;
  r0.xyz = -r0.xyz * r3.xyz + r2.xyz;
  r0.w = cb0[161].y * r0.w;
  r2.xyz = cb0[165].xyz * r4.xyz + -r0.xyz;
  r0.xyz = r0.www * r2.xyz + r0.xyz;
  r2.xyzw = t4.Sample(s3_s, v6.zw).xyzw;
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