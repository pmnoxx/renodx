#include "../custom.hlsl"

// ---- Created with 3Dmigoto v1.4.1 on Wed Jul  9 12:54:51 2025
Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

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
  float4 r0,r1,r2,r3,r4;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t0.SampleBias(s0_s, v1.xy, cb0[5].x).xyzw;
  r1.x = cmp(0 < cb0[135].x);

  if (r1.x != 0 && RENODX_TONE_MAP_TYPE != 0.f) {
    r0.xyz = ProcessLUTWithUntonemappedGrading(r0.xyz, t1, cb0[135].x);
    
    o0.xyz = r0.xyz;
    o0.w = 1.f;
    return;
  }


  if (r1.x != 0) {
    float3 untonemapped = r0.xyz; // untonemapped here is still in SRGB

    r0.xyz = saturate(untonemapped);

    r1.x = -1 + cb0[133].w;
    r0.xyz = saturate(r0.xyz);
    r1.yzw = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.zxy);
    r2.xyz = float3(12.9232101,12.9232101,12.9232101) * r0.zxy;
    r3.xyz = log2(r0.zxy);
    r3.xyz = float3(0.416666657,0.416666657,0.416666657) * r3.xyz;
    r3.xyz = exp2(r3.xyz);
    r3.xyz = r3.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
    r1.yzw = r1.yzw ? r2.xyz : r3.xyz;
    
    r2.xyz = r1.yzw * r1.xxx; // (1- strength)
    r1.z = floor(r2.x);
    r2.xw = float2(0.5,0.5) * cb0[133].xy;
    r2.yz = r2.yz * cb0[133].xy + r2.xw;
    r2.x = r1.z * cb0[133].y + r2.y;
    r3.xyzw = t1.SampleLevel(s1_s, r2.xz, 0).xyzw;
    r4.x = cb0[133].y;
    r4.y = 0;
    r2.xy = r4.xy + r2.xz;
    r2.xyzw = t1.SampleLevel(s1_s, r2.xy, 0).xyzw;
    r1.x = r1.y * r1.x + -r1.z;
    r1.yzw = r2.xyz + -r3.xyz;
    r1.xyz = r1.xxx * r1.yzw + r3.xyz;
    
    r2.xyz = float3(0.0773993805,0.0773993805,0.0773993805) * r1.xyz;
    r3.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r1.xyz;
    r3.xyz = float3(0.947867334,0.947867334,0.947867334) * r3.xyz;
    r3.xyz = log2(abs(r3.xyz));
    r3.xyz = float3(2.4000001,2.4000001,2.4000001) * r3.xyz;
    r3.xyz = exp2(r3.xyz);
    r1.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r1.xyz);
    r1.xyz = r1.xyz ? r2.xyz : r3.xyz;

    r1.xyz = r1.xyz + -r0.xyz;
    r0.xyz = cb0[135].xxx * r1.xyz + r0.xyz;
  }

  r0.xyz = renodx::draw::ToneMapPass(r0.xyz);

  o0.xyzw = r0.xyzw;
  
  return;
}