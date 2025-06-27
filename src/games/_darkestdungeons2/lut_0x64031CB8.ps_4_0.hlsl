#include "./shared.h"

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[136];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4,r5;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t0.SampleBias(s0_s, v1.xy, cb0[5].x).xyzw;

  float3 untonemapped = r0.xyz;
  float3 neutral_sdr = renodx::tonemap::renodrt::NeutralSDR(untonemapped);

  float3 originalY = renodx::color::y::from::BT709(untonemapped);

  if (RENODX_TONE_MAP_TYPE == 0) {
    r0.xyz = saturate(r0.xyz);
  } else {
    //r0.xyz = neutral_sdr;
    r0.xyz = lerp(r0.xyz, neutral_sdr, saturate(originalY));
  }

  r1.x = saturate(cb0[135].y * cb0[135].x);
  r1.y = cmp(0 < r1.x);
  if (r1.y != 0) {
    r1.y = -1 + cb0[133].w;

    r0.xyz = saturate(r0.xyz);
    r2.xyz = renodx::color::srgb::EncodeSafe(r0.zxy);
  /*  r0.xyz = saturate(r0.xyz);
    r2.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.zxy);
    r3.xyz = float3(12.9232101,12.9232101,12.9232101) * r0.zxy;
    r4.xyz = log2(r0.zxy);
    r4.xyz = float3(0.416666657,0.416666657,0.416666657) * r4.xyz;
    r4.xyz = exp2(r4.xyz);
    r4.xyz = r4.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
    r2.xyz = r2.xyz ? r3.xyz : r4.xyz;*/

    r2.yzw = r2.xyz * r1.yyy;


    r1.z = floor(r2.y);
    r3.xy = float2(0.5,0.5) * cb0[133].xy;
    r3.yz = r2.zw * cb0[133].xy + r3.xy;
    r3.x = r1.z * cb0[133].y + r3.y;
    r4.xyzw = t1.SampleLevel(s1_s, r3.xz, 0).xyzw;
    r5.x = cb0[133].y;
    r5.y = 0;
    r2.yz = r5.xy + r3.xz;
    r5.xyzw = t1.SampleLevel(s1_s, r2.yz, 0).xyzw;
    r1.y = r2.x * r1.y + -r1.z;
    r5.xyz = r5.xyz + -r4.xyz;
    r4.xyz = r1.yyy * r5.xyz + r4.xyz;
    r3.xyzw = t2.SampleLevel(s2_s, r3.xz, 0).xyzw;
    r2.xyzw = t2.SampleLevel(s2_s, r2.yz, 0).xyzw;
    r2.xyz = r2.xyz + -r3.xyz;
    r1.yzw = r1.yyy * r2.xyz + r3.xyz;
    r2.xyz = r4.xyz + -r1.yzw;
    r1.yzw = cb0[135].zzz * r2.xyz + r1.yzw;

    r0.xyz = renodx::color::srgb::DecodeSafe(r1.yzw);
   /*
    r2.xyz = float3(0.0773993805,0.0773993805,0.0773993805) * r1.yzw;
    r3.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r1.yzw;
    r3.xyz = float3(0.947867334,0.947867334,0.947867334) * r3.xyz;
    r3.xyz = log2(abs(r3.xyz));
    r3.xyz = float3(2.4000001,2.4000001,2.4000001) * r3.xyz;
    r3.xyz = exp2(r3.xyz);
    r1.yzw = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r1.yzw);
    r1.yzw = r1.yzw ? r2.xyz : r3.xyz;
    r1.yzw = r1.yzw + -r0.xyz;
    r0.xyz = r1.xxx * r1.yzw + r0.xyz;*/
  }/*
  if (RENODX_TONE_MAP_TYPE != 0.f) {
    o0.xyz = renodx::tonemap::UpgradeToneMap(
        untonemapped,
        sdr_color,  // Intentional
        r0.xyz, 1.f);
    o0.xyz = renodx::draw::RenderIntermediatePass(o0.xyz);
  } */
  o0.xyzw = r0.xyzw;

 /* if (RENODX_TONE_MAP_TYPE != 0.f) {
   // r0.xyz = sdr_color;
    o0.xyz *= (1 + originalY);
    o0.xyz = renodx::draw::ToneMapPass(o0.xyz);
  } */
  if (RENODX_TONE_MAP_TYPE > 0.f) {
    //o0.xyz = renodx::tonemap::UpgradeToneMap(untonemapped.rgb, neutral_sdr, o0.rgb, 1.f);
    o0.xyz = renodx::draw::ToneMapPass(untonemapped.rgb, o0.xyz);
  }
  /*
  if (RENODX_TONE_MAP_TYPE != 0.f) {
    o0.xyz = renodx::tonemap::UpgradeToneMap(untonemapped, sdr_color, o0.xyz, 1.f);
  }*/

  o0.xyz *= RENODX_DIFFUSE_WHITE_NITS / RENODX_GRAPHICS_WHITE_NITS;
  o0.w = 1.f; // fix leaking blacks outside of the game.

  return;
}