// ---- Created with 3Dmigoto v1.4.1 on Thu Oct  9 08:04:00 2025
Texture2D<float4> t5 : register(t5);

Texture2D<float4> t4 : register(t4);

Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture3D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s4_s : register(s4);

SamplerState s3_s : register(s3);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[146];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t2.SampleBias(s3_s, v1.xy, cb0[5].x).xyzw;
  r0.w = cmp(0 != cb0[139].y);
  if (r0.w != 0) {
    r1.xyzw = t3.SampleBias(s2_s, v1.xy, cb0[5].x).xyzw;
    r2.xyzw = t0.SampleBias(s0_s, v1.xy, cb0[5].x).xyzw;
    r0.w = cmp(18 >= r2.x);
    r1.w = cmp(r2.x >= 10);
    r0.w = r0.w ? r1.w : 0;
    r0.w = r0.w ? 1.000000 : 0;
    r2.xyz = r1.xyz * cb0[139].zzz + -r1.xyz;
    r1.xyz = r0.www * r2.xyz + r1.xyz;
    r0.xyz = r1.xyz + r0.xyz;
  }
  r1.xyzw = t4.SampleBias(s2_s, v1.xy, cb0[5].x).xyzw;
  r1.yzw = float3(1,1,1) + -cb0[137].xyz;
  r1.xyz = r1.xxx * r1.yzw + cb0[137].xyz;
  r0.xyz = r1.xyz * r0.xyz;
  r0.w = cmp(0 < cb0[134].y);
  if (r0.w != 0) {
    r1.xyz = float3(12.9200001,12.9200001,12.9200001) * r0.xyz;
    r2.xyz = log2(abs(r0.xyz));
    r2.xyz = float3(0.416666657,0.416666657,0.416666657) * r2.xyz;
    r2.xyz = exp2(r2.xyz);
    r2.xyz = r2.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
    r3.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.xyz);
    r0.xyz = r3.xyz ? r1.xyz : r2.xyz;
  }
  r1.xy = v1.xy * cb0[143].xy + cb0[143].zw;
  r1.xyzw = t5.SampleBias(s4_s, r1.xy, cb0[5].x).xyzw;
  r0.w = r1.w * 2 + -1;
  r1.x = cmp(r0.w >= 0);
  r1.x = r1.x ? 1 : -1;
  r0.w = 1 + -abs(r0.w);
  r0.w = sqrt(r0.w);
  r0.w = 1 + -r0.w;
  r0.w = r1.x * r0.w;
  r0.w = cb0[144].x * r0.w;
  r1.xyz = float3(12.9200001,12.9200001,12.9200001) * r0.xyz;
  r2.xyz = log2(abs(r0.xyz));
  r2.xyz = float3(0.416666657,0.416666657,0.416666657) * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r2.xyz = r2.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
  r0.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.xyz);
  r0.xyz = r0.xyz ? r1.xyz : r2.xyz;
  r0.xyz = r0.www * float3(0.011764707,0.011764707,0.011764707) + r0.xyz;
  r1.xyz = float3(0.0773993805,0.0773993805,0.0773993805) * r0.xyz;
  r2.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r0.xyz;
  r2.xyz = float3(0.947867334,0.947867334,0.947867334) * r2.xyz;
  r2.xyz = log2(abs(r2.xyz));
  r2.xyz = float3(2.4000001,2.4000001,2.4000001) * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r0.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r0.xyz);
  r0.xyz = r0.xyz ? r1.xyz : r2.xyz;
  r0.xyz = max(float3(0,0,0), r0.xyz);
  r0.xyz = cb0[145].xyz * r0.xyz;
  r1.x = dot(r0.xyz, float3(0.300000012,0.589999974,0.109999999));
  r0.w = 1;
  r1.xyzw = r1.xxxx + -r0.xyzw;
  r0.xyzw = cb0[134].zzzz * r1.xyzw + r0.xyzw;
  r0.xyz = float3(0.00266771927,0.00266771927,0.00266771927) + r0.xyz;
  r0.xyz = log2(r0.xyz);
  r0.xyz = (r0.xyz * float3(0.0714285746, 0.0714285746, 0.0714285746) + float3(0.610726953, 0.610726953, 0.610726953));  // saturate
  r0.xyz = r0.xyz * float3(0.96875,0.96875,0.96875) + float3(0.015625,0.015625,0.015625);
  r1.xyzw = t1.Sample(s1_s, r0.xyz).xyzw;
  o0.xyz = r1.xyz;
  o0.w = r0.w;
  return;
}