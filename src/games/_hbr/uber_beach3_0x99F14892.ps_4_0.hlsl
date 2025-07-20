#include "./common.h"

// ---- Created with 3Dmigoto v1.4.1 on Tue Jul  8 18:00:29 2025
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
  float4 cb0[37];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  float2 w1 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t0.Sample(s0_s, w1.xy).xyzw;
  r0.w = saturate(r0.w); // fix for bloom
  
  r0 = debug_mode(r0, v1);
  // r1 = debug_mode(r1, v1);

  r0.xyz = renodx::color::bt709::clamp::BT2020(r0.xyz);
  r1.xyz = renodx::color::srgb::DecodeSafe(r0.xyz);
/*
  r1.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r0.xyz;
  r1.xyz = float3(0.947867334,0.947867334,0.947867334) * r1.xyz;
  r1.xyz = max(float3(1.1920929e-07,1.1920929e-07,1.1920929e-07), abs(r1.xyz));
  r1.xyz = log2(r1.xyz);
  r1.xyz = float3(2.4000001,2.4000001,2.4000001) * r1.xyz;
  r1.xyz = exp2(r1.xyz);
  r2.xyz = float3(0.0773993805,0.0773993805,0.0773993805) * r0.xyz;
  r3.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r0.xyz);
  r1.xyz = r3.xyz ? r2.xyz : r1.xyz;
*/
  
  r2.xyzw = t1.Sample(s1_s, v1.xy).xyzw;
  r0.xyz = r2.xxx * r1.xyz;
  r1.xyzw = float4(-1,-1,1,1) * cb0[32].xyxy;
  r2.x = 0.5 * cb0[34].x;
  r3.xyzw = saturate(r1.xyzy * r2.xxxx + v1.xyxy);
  r1.xyzw = saturate(r1.xwzw * r2.xxxx + v1.xyxy);
  r1.xyzw = cb0[26].xxxx * r1.xyzw;
  r2.xyzw = cb0[26].xxxx * r3.xyzw;
  r3.xyzw = t2.Sample(s2_s, r2.xy).xyzw;
  r2.xyzw = t2.Sample(s2_s, r2.zw).xyzw;
  r2.xyzw = r3.xyzw + r2.xyzw;
  r3.xyzw = t2.Sample(s2_s, r1.xy).xyzw;
  r1.xyzw = t2.Sample(s2_s, r1.zw).xyzw;
  r2.xyzw = r3.xyzw + r2.xyzw;
  r1.xyzw = r2.xyzw + r1.xyzw;
  r1.xyzw = cb0[34].yyyy * r1.xyzw;
  r2.xyzw = float4(0.25,0.25,0.25,1) * r1.xyzw;
  r1.xyzw = float4(0.25,0.25,0.25,0.25) * r1.xyzw;
  r3.xyz = cb0[35].xyz * r2.xyz;
  r3.w = 0.25 * r2.w;
  r0.xyzw = r3.xyzw + r0.xyzw;
  r2.xy = v1.xy * cb0[33].xy + cb0[33].zw;
  r2.xyzw = t3.Sample(s3_s, r2.xy).xyzw;
  r2.xyz = cb0[34].zzz * r2.xyz;
  r2.w = 0;
 // r0.xyzw = saturate(r2.xyzw * r1.xyzw + r0.xyzw);
  r0.xyzw = (r2.xyzw * r1.xyzw + r0.xyzw);

  r0.w = saturate(r0.w); // fix

  if (RENODX_TONE_MAP_TYPE != 0) {
    float4 toneMapResult = ToneMapBlock(r0.xyz, r0.w, 0.f, t4, SCENE_TYPE_3D);
    o0 = toneMapResult;
    // o0 = debug_mode(o0, v1);
    return;
  }
  /*
  if (RENODX_TONE_MAP_TYPE != 0) {
    float3 untonemapped = r0.xyz; // untonemapped here is still in SRGB
    r0.xyz = saturate(untonemapped);

    
    float3 sdrTonemapped = renodx::tonemap::renodrt::NeutralSDR(untonemapped); // tonemap to SDR you can change this to any SDR tonemapper you want 
    if (RENODX_TONE_MAP_TYPE != 0) {
      float y = renodx::color::y::from::BT709(untonemapped);
      r0.xyz = saturate(lerp(untonemapped, sdrTonemapped, saturate(y)));
      sdrTonemapped = r0.xyz;
    }

    r0.xyz = renodx::color::srgb::EncodeSafe(r0.xyz);
    r0.xyz = renodx::lut::SampleTetrahedral(t4,r0.xyz);
    r0.xyz = renodx::color::srgb::DecodeSafe(r0.xyz);

    if (RENODX_TONE_MAP_TYPE != 0) {
      float3 sdrGraded = r0.xyz;
      float3 color = renodx::tonemap::UpgradeToneMap(untonemapped, sdrTonemapped, sdrGraded, 1.f);
      color = ApplyReverseReinhard(color, SCENE_TYPE_3D);
      r0.rgb = ToneMapPassWrapper(color); // all 3 colors are in LINEAR here
    }

    o0.xyz = renodx::draw::RenderIntermediatePass(r0.xyz);
    o0.w = saturate(r0.w);
   // o0 = debug_mode(o0, v1);
    return;
  }
    */

  r0.xyzw = saturate(r0.xyzw);

  
  r0.xyz = renodx::color::srgb::EncodeSafe(r0.xyz);
  r0.xyz = renodx::lut::SampleTetrahedral(t4,r0.xyz);
  r0.xyz = renodx::color::srgb::DecodeSafe(r0.xyz);

  o0.xyz = renodx::draw::RenderIntermediatePass(r0.xyz);
  o0.w = r0.w;

  /*
  r1.xyz = max(float3(1.1920929e-07,1.1920929e-07,1.1920929e-07), r0.zxy);
  r1.xyz = log2(r1.xyz);
  r1.xyz = float3(0.416666657,0.416666657,0.416666657) * r1.xyz;
  r1.xyz = exp2(r1.xyz);
  r1.xyz = r1.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
  r2.xyz = float3(12.9200001,12.9200001,12.9200001) * r0.zxy;
  r0.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.zxy);
  o0.w = r0.w;
  r0.xyz = r0.xyz ? r2.xyz : r1.xyz;
  r0.yzw = cb0[36].zzz * r0.xyz;
  r0.y = floor(r0.y);
  r0.x = r0.x * cb0[36].z + -r0.y;
  r1.xy = float2(0.5,0.5) * cb0[36].xy;
  r1.yz = r0.zw * cb0[36].xy + r1.xy;
  r1.x = r0.y * cb0[36].y + r1.y;
  r2.x = cb0[36].y;
  r2.y = 0;
  r0.yz = r2.xy + r1.xz;
  r1.xyzw = t4.Sample(s4_s, r1.xz).xyzw;
  r2.xyzw = t4.Sample(s4_s, r0.yz).xyzw;
  r0.yzw = r2.xyz + -r1.xyz;
  r0.xyz = r0.xxx * r0.yzw + r1.xyz;
  r1.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r0.xyz;
  r1.xyz = float3(0.947867334,0.947867334,0.947867334) * r1.xyz;
  r1.xyz = max(float3(1.1920929e-07,1.1920929e-07,1.1920929e-07), abs(r1.xyz));
  r1.xyz = log2(r1.xyz);
  r1.xyz = float3(2.4000001,2.4000001,2.4000001) * r1.xyz;
  r1.xyz = exp2(r1.xyz);
  r2.xyz = float3(0.0773993805,0.0773993805,0.0773993805) * r0.xyz;
  r0.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r0.xyz);
  r0.xyz = r0.xyz ? r2.xyz : r1.xyz;
  r1.xyz = max(float3(1.1920929e-07,1.1920929e-07,1.1920929e-07), abs(r0.xyz));
  r1.xyz = log2(r1.xyz);
  r1.xyz = float3(0.416666657,0.416666657,0.416666657) * r1.xyz;
  r1.xyz = exp2(r1.xyz);
  r1.xyz = r1.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
  r2.xyz = float3(12.9200001,12.9200001,12.9200001) * r0.xyz;
  r0.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.xyz);
  o0.xyz = r0.xyz ? r2.xyz : r1.xyz;*/
  return;
}