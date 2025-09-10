// ---- Created with 3Dmigoto v1.4.1 on Sat Jul  5 21:42:03 2025
#include "./shared.h"

cbuffer _Globals : register(b0)
{
  float nStageNum : packoffset(c0);
  int BlendTexture1BlendModeOfColor : packoffset(c0.y);
  int BlendTexture1BlendModeOfAlpha : packoffset(c0.z);
  int BlendTexture2BlendModeOfColor : packoffset(c0.w);
  int BlendTexture2BlendModeOfAlpha : packoffset(c1);
  float SaturationScale : packoffset(c1.y);
  int ContrastValuesOfTexture[3] : packoffset(c2);
  float vATest : packoffset(c4.y);
}

SamplerState __smpsStage0_s : register(s0);
SamplerState __smpsStage1_s : register(s1);
SamplerState __smpsStage2_s : register(s2);
Texture2D<float4> sStage0 : register(t0);
Texture2D<float4> sStage1 : register(t1);
Texture2D<float4> sStage2 : register(t2);


// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_Position0,
  float4 v1 : TEXCOORD0,
  float4 v2 : TEXCOORD1,
  float4 v3 : TEXCOORD2,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4,r5;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.x = nStageNum;
  r0.y = cmp(0 < (int)r0.x);
  if (r0.y != 0) {
    r1.xyzw = sStage0.Sample(__smpsStage0_s, v2.xy).xyzw;
    r0.y = cmp(0 < ContrastValuesOfTexture[0]);
    r0.z = ContrastValuesOfTexture[0];
    r0.w = 0.00787401572 * r0.z;
    r0.w = log2(abs(r0.w));
    r0.w = 3.030303 * r0.w;
    r0.w = exp2(r0.w);
    r0.w = r0.w * 7 + 1;
    r2.xyz = float3(-0.5,-0.5,-0.5) + r1.xyz;
    r3.xyz = saturate(r0.www * r2.xyz + float3(0.5,0.5,0.5));
    r0.w = cmp(ContrastValuesOfTexture[0] < 0);
    r0.z = -abs(r0.z) * 0.00787401572 + 1;
    r2.xyz = saturate(r0.zzz * r2.xyz + float3(0.5,0.5,0.5));
    r2.xyz = r0.www ? r2.xyz : r1.xyz;
    r0.yzw = r0.yyy ? r3.xyz : r2.xyz;
    r1.xyz = ContrastValuesOfTexture[0] ? r0.yzw : r1.xyz;
    r0.xy = cmp(int2(1,2) < (int2)r0.xx);
    if (r0.x != 0) {
      r2.xyzw = sStage1.Sample(__smpsStage1_s, v2.wz).xyzw;
      r0.x = cmp(0 < ContrastValuesOfTexture[1]);
      r0.z = ContrastValuesOfTexture[1];
      r0.w = 0.00787401572 * r0.z;
      r0.w = log2(abs(r0.w));
      r0.w = 3.030303 * r0.w;
      r0.w = exp2(r0.w);
      r0.w = r0.w * 7 + 1;
      r3.xyz = float3(-0.5,-0.5,-0.5) + r2.xyz;
      r4.xyz = saturate(r0.www * r3.xyz + float3(0.5,0.5,0.5));
      r0.w = cmp(ContrastValuesOfTexture[1] < 0);
      r0.z = -abs(r0.z) * 0.00787401572 + 1;
      r3.xyz = saturate(r0.zzz * r3.xyz + float3(0.5,0.5,0.5));
      r3.xyz = r0.www ? r3.xyz : r2.xyz;
      r0.xzw = r0.xxx ? r4.xyz : r3.xyz;
      r0.xzw = ContrastValuesOfTexture[1] ? r0.xzw : r2.xyz;
      r2.x = cmp(BlendTexture1BlendModeOfColor == 2);
      if (r2.x != 0) {
        r2.xyz = r0.xzw + -r1.xyz;
        r1.xyz = r2.www * r2.xyz + r1.xyz;
      } else {
        r2.x = cmp(BlendTexture1BlendModeOfColor == 3);
        if (r2.x != 0) {
          r1.xyz = r1.xyz * r0.xzw;
        } else {
          r2.x = cmp(BlendTexture1BlendModeOfColor == 1);
          if (r2.x != 0) {
            r2.xyz = r2.www * r0.xzw;
            r1.xyz = saturate(r1.www * r1.xyz + r2.xyz);
          } else {
            r2.x = cmp(BlendTexture1BlendModeOfColor == 4);
            if (r2.x != 0) {
              r1.xyz = r0.xzw;
            } else {
              r2.x = cmp(BlendTexture1BlendModeOfColor == 5);
              if (r2.x != 0) {
                r2.xyz = r2.www * r0.xzw;
                r1.xyz = saturate(r1.www * r1.xyz + -r2.xyz);
              } else {
                r2.x = cmp(BlendTexture1BlendModeOfColor == 6);
                if (r2.x != 0) {
                  r1.x = saturate(dot(r0.xx, r1.xx));
                  r1.y = saturate(dot(r0.zz, r1.yy));
                  r1.z = saturate(dot(r0.ww, r1.zz));
                } else {
                  r2.xyz = float3(1,1,1) + -r0.xzw;
                  r2.xyz = saturate(r1.xyz / r2.xyz);
                  r3.xyz = float3(1,1,1) + -r1.xyz;
                  r3.xyz = r3.xyz / r0.xzw;
                  r3.xyz = saturate(float3(1,1,1) + -r3.xyz);
                  r4.xyz = cmp(BlendTexture1BlendModeOfColor == int3(7,8,9));
                  r5.xyz = r0.xzw + r1.xyz;
                  r0.xzw = r1.xyz * r0.xzw;
                  r0.xzw = saturate(-r0.xzw * float3(2,2,2) + r5.xyz);
                  r0.xzw = r4.zzz ? r0.xzw : r1.xyz;
                  r0.xzw = r4.yyy ? r3.xyz : r0.xzw;
                  r1.xyz = r4.xxx ? r2.xyz : r0.xzw;
                }
              }
            }
          }
        }
      }
      r0.x = max(r2.w, r1.w);
      r0.z = min(r2.w, r1.w);
      r1.w = BlendTexture1BlendModeOfAlpha ? r0.z : r0.x;
    }
    if (r0.y != 0) {
      r0.xyzw = sStage2.Sample(__smpsStage2_s, v3.xy).xyzw;
      r2.x = cmp(0 < ContrastValuesOfTexture[2]);
      r2.y = ContrastValuesOfTexture[2];
      r2.z = 0.00787401572 * r2.y;
      r2.z = log2(abs(r2.z));
      r2.z = 3.030303 * r2.z;
      r2.z = exp2(r2.z);
      r2.z = r2.z * 7 + 1;
      r3.xyz = float3(-0.5,-0.5,-0.5) + r0.xyz;
      r4.xyz = saturate(r2.zzz * r3.xyz + float3(0.5,0.5,0.5));
      r2.z = cmp(ContrastValuesOfTexture[2] < 0);
      r2.y = -abs(r2.y) * 0.00787401572 + 1;
      r3.xyz = saturate(r2.yyy * r3.xyz + float3(0.5,0.5,0.5));
      r2.yzw = r2.zzz ? r3.xyz : r0.xyz;
      r2.xyz = r2.xxx ? r4.xyz : r2.yzw;
      r0.xyz = ContrastValuesOfTexture[2] ? r2.xyz : r0.xyz;
      r2.x = cmp(BlendTexture2BlendModeOfColor == 2);
      if (r2.x != 0) {
        r2.xyz = r0.xyz + -r1.xyz;
        r1.xyz = r0.www * r2.xyz + r1.xyz;
      } else {
        r2.x = cmp(BlendTexture2BlendModeOfColor == 3);
        if (r2.x != 0) {
          r1.xyz = r1.xyz * r0.xyz;
        } else {
          r2.x = cmp(BlendTexture2BlendModeOfColor == 1);
          if (r2.x != 0) {
            r2.xyz = r0.www * r0.xyz;
            r1.xyz = saturate(r1.www * r1.xyz + r2.xyz);
          } else {
            r2.x = cmp(BlendTexture2BlendModeOfColor == 4);
            if (r2.x != 0) {
              r1.xyz = r0.xyz;
            } else {
              r2.x = cmp(BlendTexture2BlendModeOfColor == 5);
              if (r2.x != 0) {
                r2.xyz = r0.www * r0.xyz;
                r1.xyz = saturate(r1.www * r1.xyz + -r2.xyz);
              } else {
                r2.x = cmp(BlendTexture2BlendModeOfColor == 6);
                if (r2.x != 0) {
                  r1.x = saturate(dot(r0.xx, r1.xx));
                  r1.y = saturate(dot(r0.yy, r1.yy));
                  r1.z = saturate(dot(r0.zz, r1.zz));
                } else {
                  r2.xyz = float3(1,1,1) + -r0.xyz;
                  r2.xyz = saturate(r1.xyz / r2.xyz);
                  r3.xyz = float3(1,1,1) + -r1.xyz;
                  r3.xyz = r3.xyz / r0.xyz;
                  r3.xyz = saturate(float3(1,1,1) + -r3.xyz);
                  r4.xyz = cmp(BlendTexture2BlendModeOfColor == int3(7,8,9));
                  r5.xyz = r0.xyz + r1.xyz;
                  r0.xyz = r1.xyz * r0.xyz;
                  r0.xyz = saturate(-r0.xyz * float3(2,2,2) + r5.xyz);
                  r0.xyz = r4.zzz ? r0.xyz : r1.xyz;
                  r0.xyz = r4.yyy ? r3.xyz : r0.xyz;
                  r1.xyz = r4.xxx ? r2.xyz : r0.xyz;
                }
              }
            }
          }
        }
      }
      r0.x = max(r1.w, r0.w);
      r0.y = min(r1.w, r0.w);
      r1.w = BlendTexture2BlendModeOfAlpha ? r0.y : r0.x;
    }
    r0.xyzw = v1.xyzw * r1.xyzw;
  } else {
    r0.xyzw = v1.xyzw;
  }
  r1.x = cmp(r0.w < vATest);
  if (r1.x != 0) discard;
  r1.x = cmp(SaturationScale != 1.000000);
  r1.y = dot(r0.xyz, float3(0.298909992,0.586610019,0.114480004));
  r2.xyz = -r1.yyy + r0.xyz;
  r1.yzw = SaturationScale * r2.xyz + r1.yyy;
  r0.xyz = r1.xxx ? r1.yzw : r0.xyz;
  o0.xyzw = r0.xyzw;
  //o0.xyz *= UI_BRIGHTNESS / 203.f;
  return;
}