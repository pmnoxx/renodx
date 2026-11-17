#include "../../common.hlsl"
// ---- Created with 3Dmigoto v1.4.1 on Tue Nov 17 09:33:35 2026
Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb1 : register(b1)
{
  float4 cb1[1];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[27];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  float4 v2 : TEXCOORD0,
  float4 v3 : TEXCOORD1,
  float4 v4 : TEXCOORD2,
  float4 v5 : TEXCOORD3,
  float4 v6 : TEXCOORD4,
  float4 v7 : COLOR1,
  float4 v8 : TEXCOORD5,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xy = cb0[4].zw * cb1[0].yy + v8.zw;
  r0.xyzw = t2.Sample(s1_s, r0.xy).xyzw;
  r0.xyzw = cb0[5].xyzw * r0.xyzw;
  r0.xyz = r0.xyz * r0.www;
  r1.xyz = cb0[3].xyz * v1.xyz;
  r2.xy = cb0[2].xy * cb1[0].yy + v8.xy;
  r2.xyzw = t1.Sample(s0_s, r2.xy).xyzw;
  r1.xyz = r2.xyz * r1.xyz;
  r2.w = cb0[3].w * r2.w;
  r2.xyz = r2.www * r1.xyz;
  r0.xyzw = -r2.xyzw + r0.xyzw;
  r1.x = cb0[22].w * cb0[6].x;
  r1.x = v3.y * r1.x;
  r1.y = min(1, r1.x);
  r1.x = 0.5 * r1.x;
  r1.y = sqrt(r1.y);
  r3.xyzw = t0.Sample(s2_s, v2.xy).xyzw;
  r1.z = v3.z + -r3.w;
  r1.w = saturate(r1.z * v3.y + r1.x);
  r1.x = r1.z * v3.y + -r1.x;
  r1.y = r1.w * r1.y;
  r0.xyzw = r1.yyyy * r0.xyzw + r2.xyzw;
  r1.y = cb0[22].w * cb0[4].y;
  r1.zw = v3.yy * r1.zy;
  r1.y = r1.y * v3.y + 1;
  r1.x = r1.w * 0.5 + r1.x;
  r1.x = saturate(r1.x / r1.y);
  r1.x = 1 + -r1.x;
  r2.xyzw = r1.xxxx * r0.xyzw;
  r0.x = -r0.w * r1.x + 1;
  r3.xyzw = t0.Sample(s2_s, v6.xy).xyzw;
  r0.y = saturate(r3.w * v6.z + -v6.w);
  r3.xyzw = v7.xyzw * r0.yyyy;
  r0.xyzw = r3.xyzw * r0.xxxx + r2.xyzw;
  r1.xy = cb0[26].zw + -cb0[26].xy;
  r1.xy = -abs(v4.xy) + r1.xy;
  r1.xy = saturate(v4.zw * r1.xy);
  r1.x = r1.x * r1.y;
  r1.y = r0.w * r1.x + -0.00100000005;
  r1.y = cmp(r1.y < 0);
  if (r1.y != 0) discard;
  r1.y = cb0[23].x * cb0[21].x;
  r1.y = 0.5 * r1.y;
  r1.y = -r1.y * v3.y + r1.z;
  r1.z = cmp(r1.y >= 0);
  r1.z = r1.z ? 1.000000 : 0;
  r1.w = cb0[21].y * cb0[23].x + -cb0[21].z;
  r1.z = r1.z * r1.w + cb0[21].z;
  r1.z = 0.5 * r1.z;
  r1.w = r1.z * v3.y + 1;
  r1.z = v3.y * r1.z;
  r1.z = min(1, r1.z);
  r1.z = sqrt(r1.z);
  r1.y = r1.y / r1.w;
  r1.y = min(1, abs(r1.y));
  r1.y = log2(r1.y);
  r1.y = cb0[21].w * r1.y;
  r1.y = exp2(r1.y);
  r1.y = 1 + -r1.y;
  r1.y = r1.y * r1.z;
  r1.y = saturate(dot(cb0[20].ww, r1.yy));
  r0.xyz = cb0[20].xyz * r1.yyy + r0.xyz;
  r0.xyzw = r1.xxxx * r0.xyzw;
  o0.xyzw = v1.wwww * r0.xyzw;
  {
  o0.xyz *= CUSTOM_TEXT_BRIGHTNESS_COEF;
    return;
  }
}