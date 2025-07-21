#include "../common.hlsl"

// ---- Created with 3Dmigoto v1.4.1 on Wed Jul  2 13:58:13 2025
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

cbuffer cb0 : register(b0)
{
  float4 cb0[43];
}


// 3Dmigoto declarations
#define cmp -

void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  float2 w1 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t1.Sample(s1_s, v1.xy).xyzw;
  r1.xyzw = t0.Sample(s0_s, w1.xy).xyzw;
  r1 = debug_mode(r1, w1);

  r1.w = saturate(r1.w);
  r1.xyz = renodx::color::bt709::clamp::BT2020(r1.xyz);
  r1 = debug_mode(r1, w1);
  r0.yzw = renodx::color::srgb::DecodeSafe(r1.xyz);
  /*
  // srg to linear
  r0.yzw = float3(0.0773993805,0.0773993805,0.0773993805) * r1.xyz;
  r2.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r1.xyz;
  r2.xyz = float3(0.947867334,0.947867334,0.947867334) * r2.xyz;
  r2.xyz = max(float3(1.1920929e-07,1.1920929e-07,1.1920929e-07), abs(r2.xyz));
  r2.xyz = log2(r2.xyz);
  r2.xyz = float3(2.4000001,2.4000001,2.4000001) * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r3.xyz = cmp(float3(0.0404499993, 0.0404499993, 0.0404499993) >= r1.xyz);
  r0.yzw = r3.xyz ? r0.yzw : r2.xyz;
*/


  r1.xyz = r0.yzw * r0.xxx;
  r0.xyzw = float4(1,1,-1,0) * cb0[32].xyxy;
  r2.xyzw = saturate(-r0.xywy * cb0[34].xxxx + v1.xyxy);
  r2.xyzw = cb0[26].xxxx * r2.xyzw;
  r3.xyzw = t2.Sample(s2_s, r2.xy).xyzw;
  r2.xyzw = t2.Sample(s2_s, r2.zw).xyzw;
  r2.xyzw = r2.xyzw * float4(2,2,2,2) + r3.xyzw;
  r3.xy = saturate(-r0.zy * cb0[34].xx + v1.xy);
  r3.xy = cb0[26].xx * r3.xy;
  r3.xyzw = t2.Sample(s2_s, r3.xy).xyzw;
  r2.xyzw = r3.xyzw + r2.xyzw;
  r3.xyzw = saturate(r0.zwxw * cb0[34].xxxx + v1.xyxy);
  r3.xyzw = cb0[26].xxxx * r3.xyzw;
  r4.xyzw = t2.Sample(s2_s, r3.xy).xyzw;
  r2.xyzw = r4.xyzw * float4(2,2,2,2) + r2.xyzw;
  r3.xy = saturate(v1.xy);
  r3.xy = cb0[26].xx * r3.xy;
  r4.xyzw = t2.Sample(s2_s, r3.xy).xyzw;
  r2.xyzw = r4.xyzw * float4(4,4,4,4) + r2.xyzw;
  r3.xyzw = t2.Sample(s2_s, r3.zw).xyzw;
  r2.xyzw = r3.xyzw * float4(2,2,2,2) + r2.xyzw;
  r3.xyzw = saturate(r0.zywy * cb0[34].xxxx + v1.xyxy);
  r3.xyzw = cb0[26].xxxx * r3.xyzw;
  r4.xyzw = t2.Sample(s2_s, r3.xy).xyzw;
  r2.xyzw = r4.xyzw + r2.xyzw;
  r3.xyzw = t2.Sample(s2_s, r3.zw).xyzw;
  r2.xyzw = r3.xyzw * float4(2,2,2,2) + r2.xyzw;
  r0.xy = saturate(r0.xy * cb0[34].xx + v1.xy);
  r0.xy = cb0[26].xx * r0.xy;
  r0.xyzw = t2.Sample(s2_s, r0.xy).xyzw;
  r0.xyzw = r2.xyzw + r0.xyzw;
  r0.xyzw = cb0[34].yyyy * r0.xyzw;
  r2.xy = v1.xy * cb0[33].xy + cb0[33].zw;
  r2.xyzw = t3.Sample(s3_s, r2.xy).xyzw;
  r3.xyzw = float4(0.0625,0.0625,0.0625,0.0625) * r0.xyzw;
  r2.xyz = cb0[34].zzz * r2.xyz;
  r2.w = 0;
  r0.xyzw = float4(0.0625,0.0625,0.0625,1) * r0.xyzw;
  r4.xyz = cb0[35].xyz * r0.xyz;
  r4.w = 0.0625 * r0.w;
  r0.xyzw = r4.xyzw + r1.xyzw;
  r0.xyzw = r2.xyzw * r3.xyzw + r0.xyzw;
  
  r1.x = cmp(cb0[40].y < 0.5);
  if (r1.x != 0) {
    r1.xy = -cb0[38].xy + v1.xy;
    r1.yz = cb0[39].xx * abs(r1.yx);
    r1.w = cb0[22].x / cb0[22].y;
    r1.w = -1 + r1.w;
    r1.w = cb0[39].w * r1.w + 1;
    r1.x = r1.z * r1.w;
    //r1.xy = saturate(r1.xy);
    r1.xy = log2(r1.xy);
    r1.xy = cb0[39].zz * r1.xy;
    r1.xy = exp2(r1.xy);
    r1.x = dot(r1.xy, r1.xy);
    r1.x = 1 + -r1.x;
    r1.x = max(0, r1.x);
    r1.x = log2(r1.x);
    r1.x = cb0[39].y * r1.x;
    r1.x = exp2(r1.x);
    r1.yzw = float3(1,1,1) + -cb0[37].xyz;
    r1.yzw = r1.xxx * r1.yzw + cb0[37].xyz;
    r1.yzw = r1.yzw * r0.xyz;
    r2.x = -1 + r0.w;
    r1.x = r1.x * r2.x + 1;
  } else {
    r2.xyzw = t4.Sample(s4_s, v1.xy).xyzw;
    r2.xyz = float3(1,1,1) + -cb0[37].xyz;
    r2.xyz = r2.www * r2.xyz + cb0[37].xyz;
    r2.xyz = r0.xyz * r2.xyz + -r0.xyz;
    r1.yzw = cb0[40].xxx * r2.xyz + r0.xyz;
    r0.x = -1 + r0.w;
    r1.x = r2.w * r0.x + 1;
  }
  r0.x = cmp(0.5 < cb0[42].x);
  if (r0.x != 0) {
    r0.xyz = saturate(r1.yzw);
    o0.w = dot(r0.xyz, float3(0.212672904,0.715152204,0.0721750036));
  } else {
    o0.w = r1.x;
  }

  o0.xyz = r1.yzw;
  
  if (RENODX_TONE_MAP_TYPE >= 1.f) {
    o0.xyz = ApplyReverseReinhard(o0.xyz, SCENE_TYPE_3D);
    o0.xyz = ToneMapPassWrapper(o0.xyz);
  }

  o0.xyz = renodx::draw::RenderIntermediatePass(o0.xyz);
  //o0 = debug_mode(o0, v1);

  return;
}

// ===== Original Shader Code (from dump/0x699FCD0A.ps_5_0.hlsl) =====
/*
// ---- Created with 3Dmigoto v1.4.1 on Sun Jul 20 13:32:04 2025

cbuffer CBUFFER_PIXEL_SHADER : register(b0)
{
  float4x4 g_PS_InverseViewProjectionMatrix : packoffset(c0);
  float4x4 g_PS_WorldInverseMatrix : packoffset(c4);
  float4x4 g_PS_AlphaLightMatrix : packoffset(c8);
  float4 g_PS_AlphaLightParameter : packoffset(c12);
  float4 g_PS_EyePosition : packoffset(c13);
  float4 g_PS_ScreenParameter : packoffset(c14);
  float4 g_PS_Parameters[2] : packoffset(c15);
  float4 g_PS_ScreenPosition : packoffset(c17);
  float4 g_PS_UvTransform[12] : packoffset(c18);
  float4 g_PS_FalloffParameter[4] : packoffset(c30);
  float4 g_PS_SpecularParameter : packoffset(c34);
  float4 g_PS_SceneAmbientColor : packoffset(c35);
  float4 g_PS_SceneLightDirection : packoffset(c36);
  float4 g_PS_SceneLightColor : packoffset(c37);
  float4 g_PS_VFXLightAmbient : packoffset(c38);
  float4 g_PS_VFXLightDirection : packoffset(c39);
  float4 g_PS_VFXLightColor : packoffset(c40);
  float4 g_PS_FogParameter : packoffset(c41);
  float4 g_PS_ScreenTextureCoordRect : packoffset(c42);
}

SamplerState g_SamplerColor1_s : register(s0);
SamplerState g_SamplerColor2_s : register(s1);
SamplerState g_SamplerDistortion1_s : register(s3);
Texture2D<float4> g_TextureColor1 : register(t0);
Texture2D<float4> g_TextureColor2 : register(t1);
Texture2D<float4> g_TextureDistortion1 : register(t3);

// 3Dmigoto declarations
#define cmp -

void main(
  linear centroid float4 v0 : TEXCOORD0,
  float4 v1 : TEXCOORD1,
  float4 v2 : TEXCOORD2,
  float4 v3 : TEXCOORD3,
  float4 v4 : TEXCOORD4,
  float4 v5 : TEXCOORD5,
  float3 v6 : TEXCOORD7,
  float4 v7 : SV_Position0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xy = v2.zw;
  r0.z = 1;
  r1.x = dot(r0.xyz, g_PS_UvTransform[6].xyw);
  r1.y = dot(r0.xyz, g_PS_UvTransform[7].xyw);
  r0.xy = g_TextureDistortion1.Sample(g_SamplerDistortion1_s, r1.xy).xy;
  r0.xy = float2(-0.498039216,-0.498039216) + r0.xy;
  r1.xy = v1.xy;
  r1.z = 1;
  r2.x = dot(r1.xyz, g_PS_UvTransform[0].xyw);
  r2.y = dot(r1.xyz, g_PS_UvTransform[1].xyw);
  r0.zw = -r0.xy * g_PS_Parameters[0].xx + r2.xy;
  r1.xyzw = g_TextureColor1.Sample(g_SamplerColor1_s, r0.zw).xyzw;
  r2.xy = v1.zw;
  r2.z = 1;
  r3.x = dot(r2.xyz, g_PS_UvTransform[2].xyw);
  r3.y = dot(r2.xyz, g_PS_UvTransform[3].xyw);
  r0.xy = -r0.xy * g_PS_Parameters[0].xx + r3.xy;
  r0.x = g_TextureColor2.Sample(g_SamplerColor2_s, r0.xy).x;
  r0.xyzw = r1.xyzw * r0.xxxx;
  r1.xyzw = v0.xyzw * r0.xyzw;
  r0.w = dot(float3(0.300000012,0.589999974,0.109999999), r1.xyz);
  o0.w = saturate(r1.w);
  r0.xyz = r0.xyz * v0.xyz + -r0.www;
  o0.xyz = g_PS_EyePosition.www * r0.xyz + r0.www;
  return;
}
*/
