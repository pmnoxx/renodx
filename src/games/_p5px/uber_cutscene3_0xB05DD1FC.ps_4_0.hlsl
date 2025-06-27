#include "./shared.h"

// ---- Created with 3Dmigoto v1.4.1 on Thu Jun 26 09:35:32 2025
Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s3_s : register(s3);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[54];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 vv1 : TEXCOORD0,
  float2 w1 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4,r5;
  uint4 bitmask, uiDest;
  float4 fDest;

  float4 v1 = float4(vv1.x, vv1.y, w1.x, w1.y);

  r0.xy = float2(1,1) + -cb0[29].zw;
  r0.xy = cb0[28].xx * r0.xy;
  r1.xyzw = min(v1.xyzw, r0.xyxy);
  r2.xyzw = t1.Sample(s1_s, r1.xy).xyzw;
  r3.xyzw = t0.Sample(s0_s, r1.zw).xyzw;
  r4.xyzw = -cb0[38].xyyy * float4(1,1,0,1) + r1.xyxy;
  r4.xyzw = saturate(min(r4.xyzw, r0.xyxy));
  r4.xyzw = cb0[26].xxxx * r4.xyzw;
  r5.xyzw = t2.Sample(s2_s, r4.xy).xyzw;
  r4.xyzw = t2.Sample(s2_s, r4.zw).xyzw;
  r2.yzw = r4.xyz * float3(2,2,2) + r5.xyz;
  r0.zw = -cb0[38].xy * float2(-1,1) + r1.xy;
  r0.zw = saturate(min(r0.zw, r0.xy));
  r0.zw = cb0[26].xx * r0.zw;
  r4.xyzw = t2.Sample(s2_s, r0.zw).xyzw;
  r2.yzw = r4.xyz + r2.yzw;
  r4.xyzw = cb0[38].xyxy * float4(-1,0,1,0) + r1.xyxy;
  r4.xyzw = saturate(min(r4.xyzw, r0.xyxy));
  r4.xyzw = cb0[26].xxxx * r4.xyzw;
  r5.xyzw = t2.Sample(s2_s, r4.xy).xyzw;
  r2.yzw = r5.xyz * float3(2,2,2) + r2.yzw;
  r0.zw = saturate(min(r1.xy, r0.xy));
  r0.zw = cb0[26].xx * r0.zw;
  r5.xyzw = t2.Sample(s2_s, r0.zw).xyzw;
  r2.yzw = r5.xyz * float3(4,4,4) + r2.yzw;
  r4.xyzw = t2.Sample(s2_s, r4.zw).xyzw;
  r2.yzw = r4.xyz * float3(2,2,2) + r2.yzw;
  r4.xyzw = cb0[38].xyyy * float4(-1,1,0,1) + r1.xyxy;
  r4.xyzw = saturate(min(r4.xyzw, r0.xyxy));
  r4.xyzw = cb0[26].xxxx * r4.xyzw;
  r5.xyzw = t2.Sample(s2_s, r4.xy).xyzw;
  r2.yzw = r5.xyz + r2.yzw;
  r4.xyzw = t2.Sample(s2_s, r4.zw).xyzw;
  r2.yzw = r4.xyz * float3(2,2,2) + r2.yzw;
  r0.zw = cb0[38].xy * float2(1,1) + r1.xy;
  r0.xy = saturate(min(r0.zw, r0.xy));
  r0.xy = cb0[26].xx * r0.xy;
  r0.xyzw = t2.Sample(s2_s, r0.xy).xyzw;
  r0.xyz = r2.yzw + r0.xyz;
  r0.xyz = float3(0.0625,0.0625,0.0625) * r0.xyz;
  r3.xyz = r3.xyz * r2.xxx + r0.xyz;


  float3 untonemapped = r3.xyz;

  const float override_arrid_lut = RENODX_TONE_MAP_TYPE > 0.f;

  if (override_arrid_lut) {
    float3 lut_input = renodx::color::pq::Encode(untonemapped, 100.f);
//    float3 lut_input = renodx::color::pq::Encode(untonemapped, 220.f);
    r0.xyz = renodx::lut::Sample(t3, s3_s, saturate(lut_input), cb0[39].yzw);
  } else {

    r0.xyzw = cb0[40].xxxx * r3.xyzw;
    r1.xyz = r0.zxy * float3(5.55555582,5.55555582,5.55555582) + float3(0.0479959995,0.0479959995,0.0479959995);
    r1.xyz = log2(r1.xyz);
    r1.xyz = saturate(r1.xyz * float3(0.0734997839,0.0734997839,0.0734997839) + float3(0.386036009,0.386036009,0.386036009));
    r1.yzw = cb0[39].www * r1.xyz;
    r1.y = floor(r1.y);
    r2.xy = float2(0.5,0.5) * cb0[39].yz;
    r2.yz = r1.zw * cb0[39].yz + r2.xy;
    r2.x = r1.y * cb0[39].z + r2.y;
    r3.xyzw = t3.Sample(s3_s, r2.xz).xyzw;
    r4.x = cb0[39].z;
    r4.y = 0;
    r1.zw = r4.xy + r2.xz;
    r2.xyzw = t3.Sample(s3_s, r1.zw).xyzw;
    r1.x = r1.x * cb0[39].w + -r1.y;
    r1.yzw = r2.xyz + -r3.xyz;
    r0.xyz = r1.xxx * r1.yzw + r3.xyz;
  }
  r1.x = cmp(0.5 < cb0[53].y);
  if (r1.x != 0) {
    r1.xyz = saturate(r0.xyz);
    r0.w = dot(r1.xyz, float3(0.212672904,0.715152204,0.0721750036));
  }
  o0.xyzw = r0.xyzw;
  
  
  o0.xyz = renodx::draw::RenderIntermediatePass(o0.xyz);
  
  DEBUG_MARK(v1);
  return;
}