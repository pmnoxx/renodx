#include "./shared.h"

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
  r0.yzw = float3(0.0773993805,0.0773993805,0.0773993805) * r1.xyz;
  r2.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r1.xyz;
  r2.xyz = float3(0.947867334,0.947867334,0.947867334) * r2.xyz;
  r2.xyz = max(float3(1.1920929e-07,1.1920929e-07,1.1920929e-07), abs(r2.xyz));
  r2.xyz = log2(r2.xyz);
  r2.xyz = float3(2.4000001,2.4000001,2.4000001) * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r3.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r1.xyz);
  r0.yzw = r3.xyz ? r0.yzw : r2.xyz;
  r1.xyz = r0.yzw * r0.xxx;


  
  r0.xyzw = float4(-1,-1,1,1) * cb0[32].xyxy;
  r2.x = 0.5 * cb0[34].x;
  r3.xyzw = saturate(r0.xyzy * r2.xxxx + v1.xyxy);
  r3.xyzw = cb0[26].xxxx * r3.xyzw;
  r4.xyzw = t2.Sample(s2_s, r3.xy).xyzw;
  r3.xyzw = t2.Sample(s2_s, r3.zw).xyzw;
  r3.xyzw = r4.xyzw + r3.xyzw;
  r0.xyzw = saturate(r0.xwzw * r2.xxxx + v1.xyxy);
  r0.xyzw = cb0[26].xxxx * r0.xyzw;
  r2.xyzw = t2.Sample(s2_s, r0.xy).xyzw;
  r2.xyzw = r3.xyzw + r2.xyzw;
  r0.xyzw = t2.Sample(s2_s, r0.zw).xyzw;
  r0.xyzw = r2.xyzw + r0.xyzw;
  r0.xyzw = cb0[34].yyyy * r0.xyzw;
  r2.xy = v1.xy * cb0[33].xy + cb0[33].zw;
  r2.xyzw = t3.Sample(s3_s, r2.xy).xyzw;
  r3.xyzw = float4(0.25,0.25,0.25,0.25) * r0.xyzw;
  r2.xyz = cb0[34].zzz * r2.xyz;
  r2.w = 0;
  r0.xyzw = float4(0.25,0.25,0.25,1) * r0.xyzw;
  r4.xyz = cb0[35].xyz * r0.xyz;
  r4.w = 0.25 * r0.w;
  r0.xyzw = r4.xyzw + r1.xyzw;
  r0.xyzw = (r2.xyzw * r3.xyzw + r0.xyzw);
  
  r0.w = saturate(r0.w);

  if (RENODX_TONE_MAP_TYPE != 0) {
    float3 untonemapped = r0.xyz; // untonemapped here is still in SRGB
    r0.xyz = saturate(untonemapped);

    
    if (RENODX_TONE_MAP_TYPE != 0) {
      float3 sdrTonemapped = renodx::tonemap::renodrt::NeutralSDR(untonemapped); // tonemap to SDR you can change this to any SDR tonemapper you want 
      float y = renodx::color::y::from::BT709(untonemapped);
      r0.xyz = lerp(untonemapped, sdrTonemapped, saturate(y));
    }

    r0.xyz = renodx::color::srgb::EncodeSafe(r0.xyz);
    r0.xyz = renodx::lut::SampleTetrahedral(t4,r0.xyz);
    r0.xyz = renodx::color::srgb::DecodeSafe(r0.xyz);

    if (RENODX_TONE_MAP_TYPE != 0) {
      float3 sdrGraded = r0.xyz;
      r0.rgb = renodx::draw::ToneMapPass(untonemapped, sdrGraded); // all 3 colors are in LINEAR here
    }

    
    r0.w = cmp(0.5 < cb0[42].x);
    if (r0.w != 0) {
      r1.xyz = saturate(r0.xyz);
      o0.w = dot(r1.xyz, float3(0.212672904,0.715152204,0.0721750036));
    } else {
      o0.w = r2.w;
    }
    r0.xyz = renodx::draw::RenderIntermediatePass(r0.xyz);
    return;
  }

  if (RENODX_TONE_MAP_TYPE == 0.f) {
    r0.xyzw = saturate(r0.xyzw);
  }

  
  r0.xyz = renodx::color::srgb::EncodeSafe(r0.xyz);
  r0.xyz = renodx::lut::SampleTetrahedral(t4,r0.xyz);
  r0.xyz = renodx::color::srgb::DecodeSafe(r0.xyz);
 //// r0.xyz = renodx::color::pq::EncodeSafe(r0.xyz, 100.f);
 // r0.xyz = renodx::lut::Sample(t4, s4_s, r0.xyz, cb0[36].xyz);
  
  r0.w = cmp(0.5 < cb0[42].x);
  if (r0.w != 0) {
    r1.xyz = saturate(r0.xyz);
    o0.w = dot(r1.xyz, float3(0.212672904,0.715152204,0.0721750036));
  } else {
    o0.w = r2.w;
  }

  o0.xyz = renodx::draw::RenderIntermediatePass(r0.xyz);

  return;
}