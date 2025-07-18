// ---- Created with 3Dmigoto v1.4.1 on Sat Jul 12 18:27:56 2025
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

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[43];
}




// 3Dmigoto declarations
#define cmp -


#include "./common.h"


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  float2 w1 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4,r5,r6,r7;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t1.Sample(s1_s, v1.xy).xyzw;
  r0.w = saturate(r0.w);

  r0.yz = v1.xy * float2(2,2) + float2(-1,-1);
  r0.w = dot(r0.yz, r0.yz);
  r0.yz = r0.yz * r0.ww;
  r0.yz = cb0[35].ww * r0.yz;
  r1.xy = cb0[31].zw * -r0.yz;
  r1.xy = float2(0.5,0.5) * r1.xy;
  r0.w = dot(r1.xy, r1.xy);
  r0.w = sqrt(r0.w);
  r0.w = (int)r0.w;
  r0.w = max(3, (int)r0.w);
  r0.w = min(16, (int)r0.w);
  r1.x = (int)r0.w;
  r0.yz = -r0.yz / r1.xx;
  r2.yw = float2(0,0);
  r3.w = 1;
  r4.xyzw = float4(0,0,0,0);
  r5.xyzw = float4(0,0,0,0);
  r1.yz = v1.xy;
  r1.w = 0;
  while (true) {
    r6.x = cmp((int)r1.w >= (int)r0.w);
    if (r6.x != 0) break;
    r6.x = (int)r1.w;
    r6.x = 0.5 + r6.x;
    r2.x = r6.x / r1.x;
    r6.xy = saturate(r1.yz);
    r6.xy = cb0[26].xx * r6.xy;
    r6.xyzw = t0.SampleLevel(s0_s, r6.xy, 0).xyzw;
    r7.xyzw = t4.SampleLevel(s4_s, r2.xy, 0).xyzw;
    r3.xyz = max(float3(0,0,0), r7.xyz);
    r4.xyzw = r6.xyzw * r3.xyzw + r4.xyzw;
    r5.xyzw = r5.xyzw + r3.xyzw;
    r1.yz = r1.yz + r0.yz;
    r1.w = (int)r1.w + 1;
  }
  r1.xyzw = r4.xyzw / r5.xyzw;

  r1 = debug_mode(r1, w1);
  r0.yzw = float3(0.0773993805,0.0773993805,0.0773993805) * r1.xyz;
  r3.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r1.xyz;
  r3.xyz = float3(0.947867334,0.947867334,0.947867334) * r3.xyz;
  r3.xyz = max(float3(1.1920929e-07,1.1920929e-07,1.1920929e-07), abs(r3.xyz));
  r3.xyz = log2(r3.xyz);
  r3.xyz = float3(2.4000001,2.4000001,2.4000001) * r3.xyz;
  r3.xyz = exp2(r3.xyz);
  r4.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r1.xyz);
  r0.yzw = r4.xyz ? r0.yzw : r3.xyz;
  r1.xyz = r0.yzw * r0.xxx;
  r0.xyzw = float4(1,1,-1,0) * cb0[32].xyxy;
  r3.xyzw = saturate(-r0.xywy * cb0[34].xxxx + v1.xyxy);
  r3.xyzw = cb0[26].xxxx * r3.xyzw;
  r4.xyzw = t2.Sample(s2_s, r3.xy).xyzw;
  r3.xyzw = t2.Sample(s2_s, r3.zw).xyzw;
  r3.xyzw = r3.xyzw * float4(2,2,2,2) + r4.xyzw;
  r2.xy = saturate(-r0.zy * cb0[34].xx + v1.xy);
  r2.xy = cb0[26].xx * r2.xy;
  r4.xyzw = t2.Sample(s2_s, r2.xy).xyzw;
  r3.xyzw = r4.xyzw + r3.xyzw;
  r4.xyzw = saturate(r0.zwxw * cb0[34].xxxx + v1.xyxy);
  r4.xyzw = cb0[26].xxxx * r4.xyzw;
  r5.xyzw = t2.Sample(s2_s, r4.xy).xyzw;
  r3.xyzw = r5.xyzw * float4(2,2,2,2) + r3.xyzw;
  r2.xy = saturate(v1.xy);
  r2.xy = cb0[26].xx * r2.xy;
  r5.xyzw = t2.Sample(s2_s, r2.xy).xyzw;
  r3.xyzw = r5.xyzw * float4(4,4,4,4) + r3.xyzw;
  r4.xyzw = t2.Sample(s2_s, r4.zw).xyzw;
  r3.xyzw = r4.xyzw * float4(2,2,2,2) + r3.xyzw;
  r4.xyzw = saturate(r0.zywy * cb0[34].xxxx + v1.xyxy);
  r4.xyzw = cb0[26].xxxx * r4.xyzw;
  r5.xyzw = t2.Sample(s2_s, r4.xy).xyzw;
  r3.xyzw = r5.xyzw + r3.xyzw;
  r4.xyzw = t2.Sample(s2_s, r4.zw).xyzw;
  r3.xyzw = r4.xyzw * float4(2,2,2,2) + r3.xyzw;
  r0.xy = saturate(r0.xy * cb0[34].xx + v1.xy);
  r0.xy = cb0[26].xx * r0.xy;
  r0.xyzw = t2.Sample(s2_s, r0.xy).xyzw;
  r0.xyzw = r3.xyzw + r0.xyzw;
  r0.xyzw = cb0[34].yyyy * r0.xyzw;
  r2.xy = v1.xy * cb0[33].xy + cb0[33].zw;
  r3.xyzw = t3.Sample(s3_s, r2.xy).xyzw;
  r4.xyz = float3(0.0625,0.0625,0.0625) * r0.xyz;
  r3.xyz = cb0[34].zzz * r3.xyz;
  r0.xyzw = float4(0.0625,0.0625,0.0625,1) * r0.xyzw;
  r5.xyz = cb0[35].xyz * r0.xyz;
  r5.w = 0.0625 * r0.w;
  r0.xyzw = r5.xyzw + r1.xyzw;
  r1.xyz = r3.xyz * r4.xyz;
  r1.w = 0;
  r0.xyzw = r1.xyzw + r0.xyzw;
  r0.w = saturate(r0.w); // fix

  if (RENODX_TONE_MAP_TYPE != 0) {
    float3 untonemapped = r0.xyz; // untonemapped here is still in SRGB
    r0.xyz = saturate(untonemapped);

    float3 sdrTonemapped = renodx::tonemap::renodrt::NeutralSDR(untonemapped); // tonemap to SDR you can change this to any SDR tonemapper you want 
    if (RENODX_TONE_MAP_TYPE != 0) {
      float y = renodx::color::y::from::BT709(untonemapped);
      r0.xyz = saturate(lerp(untonemapped, sdrTonemapped, saturate(y)));
    }

    r0.xyz = renodx::color::srgb::EncodeSafe(r0.xyz);
    r0.xyz = renodx::lut::SampleTetrahedral(t5,r0.xyz);
    r0.xyz = renodx::color::srgb::DecodeSafe(r0.xyz);

    if (RENODX_TONE_MAP_TYPE != 0) {
      float3 sdrGraded = r0.xyz;
      float3 color = renodx::tonemap::UpgradeToneMap(untonemapped, sdrTonemapped, sdrGraded, 1.f);
      color = ApplyReverseReinhard(color, SCENE_TYPE_3D);
      r0.rgb = ToneMapPassWrapper(color); // all 3 colors are in LINEAR here
    }
    r0.w = cmp(0.5 < cb0[42].x);
    if (r0.w != 0) {
      r1.xyz = saturate(r0.xyz);
      o0.w = dot(r1.xyz, float3(0.212672904,0.715152204,0.0721750036));
    } else {
      o0.w = r2.w;
    }
    o0.xyz = renodx::draw::RenderIntermediatePass(r0.xyz);
   // o0 = debug_mode(o0, v1);
    return;
  }

  r0.xyzw = saturate(r0.xyzw);
  r1.xyz = float3(12.9200001,12.9200001,12.9200001) * r0.zxy;
  r3.xyz = max(float3(1.1920929e-07,1.1920929e-07,1.1920929e-07), r0.zxy);
  r3.xyz = log2(r3.xyz);
  r3.xyz = float3(0.416666657,0.416666657,0.416666657) * r3.xyz;
  r3.xyz = exp2(r3.xyz);
  r3.xyz = r3.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
  r0.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.zxy);
  r0.xyz = r0.xyz ? r1.xyz : r3.xyz;
  r1.xyz = cb0[36].zzz * r0.xyz;
  r0.y = floor(r1.x);
  r1.xw = float2(0.5,0.5) * cb0[36].xy;
  r1.yz = r1.yz * cb0[36].xy + r1.xw;
  r1.x = r0.y * cb0[36].y + r1.y;
  r3.xyzw = t5.Sample(s5_s, r1.xz).xyzw;
  r2.z = cb0[36].y;
  r1.xy = r1.xz + r2.zw;
  r1.xyzw = t5.Sample(s5_s, r1.xy).xyzw;
  r0.x = r0.x * cb0[36].z + -r0.y;
  r1.xyz = r1.xyz + -r3.xyz;
  r0.xyz = r0.xxx * r1.xyz + r3.xyz;
  r1.xyz = float3(0.0773993805,0.0773993805,0.0773993805) * r0.xyz;
  r2.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r0.xyz;
  r2.xyz = float3(0.947867334,0.947867334,0.947867334) * r2.xyz;
  r2.xyz = max(float3(1.1920929e-07,1.1920929e-07,1.1920929e-07), abs(r2.xyz));
  r2.xyz = log2(r2.xyz);
  r2.xyz = float3(2.4000001,2.4000001,2.4000001) * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r0.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r0.xyz);
  r0.xyz = r0.xyz ? r1.xyz : r2.xyz;
  r1.x = cmp(0.5 < cb0[42].x);
  if (r1.x != 0) {
    r1.xyz = saturate(r0.xyz);
    o0.w = dot(r1.xyz, float3(0.212672904,0.715152204,0.0721750036));
  } else {
    o0.w = r0.w;
  }
  r1.xyz = float3(12.9200001,12.9200001,12.9200001) * r0.xyz;
  r2.xyz = max(float3(1.1920929e-07,1.1920929e-07,1.1920929e-07), abs(r0.xyz));
  r2.xyz = log2(r2.xyz);
  r2.xyz = float3(0.416666657,0.416666657,0.416666657) * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r2.xyz = r2.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
  r0.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.xyz);
  o0.xyz = r0.xyz ? r1.xyz : r2.xyz;
  return;
}