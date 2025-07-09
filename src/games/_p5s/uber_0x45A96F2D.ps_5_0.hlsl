#include "./shared.h"

// ---- Created with 3Dmigoto v1.4.1 on Sat Jul  5 20:38:11 2025

cbuffer cbComposite : register(b2)
{
  float4 g_vSceneTexSize : packoffset(c0);
  float4 g_vCompositeInfo : packoffset(c1);
  float4 g_vSun2dInfo : packoffset(c2);
  float4 g_vEtcEffect : packoffset(c3);
  float4 g_vBloomInfo : packoffset(c4);
  float4 g_vLimbDarkenningInfo : packoffset(c5);
  float4 g_vFxaaParams : packoffset(c6);
  float4 g_vGammaCorrection : packoffset(c7);
  float4 g_vRadialBlurCenter : packoffset(c8);
  float4 g_vRadialBlurInfo : packoffset(c9);
  float4 g_vFxaaQualityParams : packoffset(c10);
  float4 g_vCompositeLastViewport : packoffset(c11);
  float4 g_vMaxUV : packoffset(c12);
}

SamplerState sampleLinear_s : register(s7);
Texture2D<float4> g_tSceneMap : register(t0);
Texture2D<float4> g_tLensFlareMap : register(t1);
Texture2D<float4> g_tExposureScaleInfo : register(t2);
Texture3D<float4> g_tHdrLut : register(t3);
Texture3D<float4> g_tLdrLut : register(t4);


// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_Position0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  const float4 icb[] = { { 0, 0, 1.000000, 0},
                              { 0, 1.000000, 0, 0},
                              { 1.000000, 0, 0, 0},
                              { 1.000000, 0, 0, 0},
                              { 0, 1.000000, 0, 0},
                              { 0, 0, 1.000000, 0},
                              { 0, 1.000000, 0, 0},
                              { 1.000000, 0, 0, 0},
                              { 1.000000, 0, 1.000000, 0},
                              { 1.000000, 0, 1.000000, 0},
                              { 1.000000, 0, 0, 0},
                              { 0, 1.000000, 0, 0} };
  float4 r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.x = cmp(0 < g_vSun2dInfo.z);
  r0.y = cmp(0 < g_vLimbDarkenningInfo.w);
  r0.z = cmp(g_vCompositeInfo.z < 0);
  r0.w = g_tExposureScaleInfo.Load(float4(0,0,0,0)).x;
  r1.xy = cmp(float2(0,0) < g_vCompositeInfo.zy);
  r1.x = r1.x ? g_vCompositeInfo.z : 1;
  r0.z = r0.z ? r0.w : r1.x;
  r1.xz = v1.xy * g_vCompositeLastViewport.zw + g_vCompositeLastViewport.xy;
  r2.xyz = g_tSceneMap.SampleLevel(sampleLinear_s, r1.xz, 0).xyz;



  r2.xyz = min(float3(65024,65024,65024), r2.xyz);
  r0.w = cmp(0 < g_vEtcEffect.x);
  if (r0.w != 0) {
    r0.w = (uint)g_vEtcEffect.y;
    r3.xy = r1.xz * float2(2,2) + float2(-1,-1);
    r1.w = dot(r3.xy, r3.xy);
    r3.xy = r3.xy * r1.ww;
    r3.xy = g_vEtcEffect.xx * r3.xy;
    r3.zw = g_vSceneTexSize.xy * -r3.xy;
    r3.zw = float2(0.5,0.5) * r3.zw;
    r1.w = dot(r3.zw, r3.zw);
    r1.w = sqrt(r1.w);
    r1.w = (int)r1.w;
    r1.w = max(3, (int)r1.w);
    r1.w = min(16, (int)r1.w);
    r2.w = (int)r1.w;
    r3.xy = -r3.xy / r2.ww;
    r4.xyz = icb[r0.w+0].xyz * r2.xyz;
    r3.zw = (int2)r0.ww + int2(1,2);
    r5.xyz = icb[r3.z+0].xyz + -icb[r0.w+0].xyz;
    r6.xyz = icb[r3.w+0].xyz + -icb[r3.z+0].xyz;
    r7.xyz = r4.xyz;
    r8.xyz = icb[r0.w+0].xyz;
    r9.xy = r1.xz;
    r3.w = 1;
    while (true) {
      r4.w = cmp((int)r3.w >= (int)r1.w);
      if (r4.w != 0) break;
      r9.xy = r9.xy + r3.xy;
      r10.xyz = g_tSceneMap.SampleLevel(sampleLinear_s, r9.xy, 0).xyz;
      r10.xyz = min(float3(65024,65024,65024), r10.xyz);
      r4.w = (int)r3.w;
      r4.w = r4.w / r2.w;
      r5.w = cmp(r4.w < 0.5);
      if (r5.w != 0) {
        r5.w = r4.w + r4.w;
        r11.xyz = r5.www * r5.xyz + icb[r0.w+0].xyz;
      } else {
        r4.w = r4.w * 2 + -1;
        r11.xyz = r4.www * r6.xyz + icb[r3.z+0].xyz;
      }
      r7.xyz = r10.xyz * r11.xyz + r7.xyz;
      r8.xyz = r11.xyz + r8.xyz;
      r3.w = (int)r3.w + 1;
    }
    r2.xyz = r7.xyz / r8.xyz;
  }
  r2.xyz = r2.xyz * r0.zzz;
  if (r0.x != 0) {
    r3.xyz = g_tSceneMap.SampleLevel(sampleLinear_s, g_vSun2dInfo.xy, 0).xyz;
    r3.xyz = min(float3(65024,65024,65024), r3.xyz);
    r0.xzw = r3.xyz * r0.zzz;
    r3.xyz = g_tLensFlareMap.SampleLevel(sampleLinear_s, r1.xz, 0).xyz;
    r3.xyz = min(float3(65024,65024,65024), r3.xyz);
    r1.w = dot(r0.xzw, float3(0.222014993,0.706655025,0.0713300034));
    r1.w = cmp(g_vEtcEffect.w < r1.w);
    r1.w = r1.w ? g_vEtcEffect.z : 0;
    r0.xzw = r3.xyz * r0.xzw;
    r2.xyz = r0.xzw * r1.www + r2.xyz;
  }
  if (r0.y != 0) {
    r0.yz = float2(-0.5,-0.5) + r1.xz;
    r0.x = g_vCompositeInfo.x * r0.y;
    r0.x = dot(r0.xz, r0.xz);
    r0.y = sqrt(r0.x);
    r0.y = -g_vLimbDarkenningInfo.y + r0.y;
    r0.z = cmp(0 < r0.y);
    r0.y = saturate(-r0.y * g_vLimbDarkenningInfo.z + 1);
    r0.y = r0.z ? r0.y : 1;
    r0.z = cmp(0 < r0.y);
    r0.x = g_vLimbDarkenningInfo.x + r0.x;
    r0.x = g_vLimbDarkenningInfo.x / r0.x;
    r0.x = r0.x * r0.x;
    r0.x = r0.z ? r0.x : 1;
    r0.x = r0.x * r0.y;
    r0.y = 1 + -g_vLimbDarkenningInfo.w;
    r0.x = r0.x * g_vLimbDarkenningInfo.w + r0.y;
    r2.xyz = r2.xyz * r0.xxx;
  }
	

  float3 hdrColor = r2.xyz;
  float3 neutral_sdr_color =  renodx::tonemap::renodrt::NeutralSDR(hdrColor);

  if (RENODX_TONE_MAP_TYPE != 0.f) {
    float color_y = renodx::color::y::from::BT709(hdrColor);
    r2.xyz = lerp(hdrColor, neutral_sdr_color, saturate(color_y));
  }


  r0.xyz = r2.xyz * float3(5.55555582,5.55555582,5.55555582) + float3(0.0479959995,0.0479959995,0.0479959995);
  r0.xyz = log2(r0.xyz) ;
  r0.xyz = (r0.xyz * float3(0.0734997839,0.0734997839,0.0734997839) + float3(0.386036009,0.386036009,0.386036009));

  r0.xyz = g_tHdrLut.SampleLevel(sampleLinear_s, r0.xyz, 0).xyz;

  if (r1.y != 0) {
    r1.xyz = saturate(r0.xyz);
    r1.xyz = log2(r1.xyz);
    r1.xyz = float3(0.454545468,0.454545468,0.454545468) * r1.xyz;
    r1.xyz = exp2(r1.xyz);
    r1.xyz = g_tLdrLut.SampleLevel(sampleLinear_s, r1.xyz, 0).xyz;
    r1.xyz = r1.xyz + -r0.xyz;
    r0.xyz = g_vCompositeInfo.yyy * r1.xyz + r0.xyz;
  }

  if (RENODX_TONE_MAP_TYPE != 0.f) {
    r0.xyz = renodx::draw::ToneMapPass(hdrColor, r0.xyz, neutral_sdr_color);
  }

  r0.w = cmp(g_vGammaCorrection.x != 1.000000);
  r1.xyz = log2(abs(r0.xyz));
  r1.xyz = g_vGammaCorrection.xxx * r1.xyz;
  r1.xyz = exp2(r1.xyz);
  r0.xyz = r0.www ? r1.xyz : r0.xyz;


  o0.xyz = g_vRadialBlurCenter.zzz * r0.xyz;
  o0.xyz = renodx::draw::RenderIntermediatePass(o0.xyz);

  o0.w = 1;
  return;
}