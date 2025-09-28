#include "../custom.hlsl"


// ---- Created with 3Dmigoto v1.4.1 on Wed Jul 23 17:04:00 2025
Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[145];
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
  r0.w = cmp(0 < cb0[142].z);
  if (r0.w != 0) {
    r1.xy = -cb0[142].xy + v1.xy;
    r1.yz = cb0[142].zz * abs(r1.xy);
    r1.x = cb0[141].w * r1.y;
    r0.w = dot(r1.xz, r1.xz);
    r0.w = 1 + -r0.w;
    r0.w = max(0, r0.w);
    r0.w = log2(r0.w);
    r0.w = cb0[142].w * r0.w;
    r0.w = exp2(r0.w);
    r1.xyz = float3(1,1,1) + -cb0[141].xyz;
    r1.xyz = r0.www * r1.xyz + cb0[141].xyz;
    r0.xyz = r1.xyz * r0.xyz;
  }

  if (RENODX_TONE_MAP_TYPE != 0.f) {
    r0.xyz = UnityLookupARIDPQ(r0.xyzw, v1.xy, cb0[132], s0_s, t2);
    r0.xyz = UnityLookupSDRsRGB(r0.xyz, v1.xy, cb0[133].w, t3);
    r0.xyz = debug_mode(r0.xyzw, v1.xy, 0.02f).xyz;
  } else {
    r0.xyz = cb0[132].www * r0.zxy;
    r0.xyz = r0.xyz * float3(5.55555582,5.55555582,5.55555582) + float3(0.0479959995,0.0479959995,0.0479959995);
    r0.xyz = max(float3(0,0,0), r0.xyz);
    r0.xyz = log2(r0.xyz);
    r0.xyz = saturate(r0.xyz * float3(0.0734997839,0.0734997839,0.0734997839) + float3(0.386036009,0.386036009,0.386036009));
    r0.yzw = cb0[132].zzz * r0.xyz;
    r0.y = floor(r0.y);
    r1.xy = float2(0.5,0.5) * cb0[132].xy;
    r1.yz = r0.zw * cb0[132].xy + r1.xy;
    r1.x = r0.y * cb0[132].y + r1.y;
    r2.xyzw = t2.SampleLevel(s0_s, r1.xz, 0).xyzw;
    r3.x = cb0[132].y;
    r3.y = 0;
    r0.zw = r3.xy + r1.xz;
    r1.xyzw = t2.SampleLevel(s0_s, r0.zw, 0).xyzw;
    r0.x = r0.x * cb0[132].z + -r0.y;
    r0.yzw = r1.xyz + -r2.xyz;
    r0.xyz = r0.xxx * r0.yzw + r2.xyz;
    r0.w = cmp(0 < cb0[133].w);
    if (r0.w != 0) {
      r0.xyz = saturate(r0.xyz);
      r1.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.xyz);
      r2.xyz = float3(12.9232101,12.9232101,12.9232101) * r0.xyz;
      r3.xyz = log2(r0.xyz);
      r3.xyz = float3(0.416666657,0.416666657,0.416666657) * r3.xyz;
      r3.xyz = exp2(r3.xyz);
      r3.xyz = r3.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
      r1.xyz = r1.xyz ? r2.xyz : r3.xyz;
      r2.xyz = cb0[133].zzz * r1.zxy;
      r0.w = floor(r2.x);
      r2.xw = float2(0.5,0.5) * cb0[133].xy;
      r2.yz = r2.yz * cb0[133].xy + r2.xw;
      r2.x = r0.w * cb0[133].y + r2.y;
      r3.xyzw = t3.SampleLevel(s0_s, r2.xz, 0).xyzw;
      r4.x = cb0[133].y;
      r4.y = 0;
      r2.xy = r4.xy + r2.xz;
      r2.xyzw = t3.SampleLevel(s0_s, r2.xy, 0).xyzw;
      r0.w = r1.z * cb0[133].z + -r0.w;
      r2.xyz = r2.xyz + -r3.xyz;
      r2.xyz = r0.www * r2.xyz + r3.xyz;
      r2.xyz = r2.xyz + -r1.xyz;
      r1.xyz = cb0[133].www * r2.xyz + r1.xyz;
      r2.xyz = float3(0.0773993805,0.0773993805,0.0773993805) * r1.xyz;
      r3.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r1.xyz;
      r3.xyz = float3(0.947867334,0.947867334,0.947867334) * r3.xyz;
      r3.xyz = log2(abs(r3.xyz));
      r3.xyz = float3(2.4000001,2.4000001,2.4000001) * r3.xyz;
      r3.xyz = exp2(r3.xyz);
      r1.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r1.xyz);
      r0.xyz = r1.xyz ? r2.xyz : r3.xyz;
    }
  }
  r1.xy = v1.xy * cb0[144].xy + cb0[144].zw;
  r1.xyzw = t1.SampleBias(s1_s, r1.xy, cb0[5].x).xyzw;
  r0.w = -0.5 + r1.w;
  r0.w = r0.w + r0.w;
  r1.x = dot(r0.xyz, float3(0.212672904,0.715152204,0.0721750036));
  r1.x = sqrt(r1.x);
  r1.x = cb0[143].y * -r1.x + 1;
  r1.yzw = r0.xyz * r0.www;
  r1.yzw = cb0[143].xxx * r1.yzw;
  o0.xyz = r1.yzw * r1.xxx + r0.xyz;
  o0.w = 1;
  return;
}