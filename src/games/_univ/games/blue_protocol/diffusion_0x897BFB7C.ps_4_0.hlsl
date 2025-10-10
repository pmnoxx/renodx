// ---- Created with 3Dmigoto v1.4.1 on Thu Oct  9 08:16:47 2025
Texture2D<float4> t5 : register(t5);

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
  float4 cb0[133];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_TARGET0,
  out float oDepth : SV_DEPTH)
{
  float4 r0,r1,r2,r3,r4;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t4.SampleBias(s4_s, v1.xy, cb0[5].x).xyzw;
  float3 color = r0.xyz;
  r1.xyz = log2(r0.xyz);
  r1.xyz = float3(0.416666657,0.416666657,0.416666657) * r1.xyz;
  r1.xyz = exp2(r1.xyz);
  r1.xyz = r1.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
  r2.xyz = float3(12.9200001,12.9200001,12.9200001) * r0.xyz;
  r0.xyz = cmp(r0.xyz >= float3(0.00313066994,0.00313066994,0.00313066994));
  o0.w = r0.w;
  r0.xyz = r0.xyz ? r1.xyz : r2.xyz;

  r1.xyz = -r0.xyz * r0.xyz + float3(1,1,1);
  r2.xyzw = t5.SampleBias(s4_s, v1.xy, cb0[5].x).xyzw;
  r2.xyz = float3(1,1,1) + -r2.xyz;
  r1.xyz = -r1.xyz * r2.xyz + float3(1,1,1);
  r0.xyz = max(r1.xyz, r0.xyz);
  r1.xyz = (r0.xyz);  // saturate
  r1.yzw = r1.xyz * float3(0.9375,0.9375,0.9375) + float3(0.03125,0.03125,0.03125);
  r0.w = r1.w * 16 + -0.5;
  r1.w = floor(r0.w);
  r0.w = -r1.w + r0.w;
  r1.y = r1.y + r1.w;
  r1.x = 0.0625 * r1.y;
  r1.yw = float2(0.0625,0) + r1.xz;
  r2.xyzw = t3.SampleLevel(s3_s, r1.yw, 0).xyzw;
  r3.xyzw = t2.SampleLevel(s2_s, r1.yw, 0).xyzw;
  r4.xyzw = t3.SampleLevel(s3_s, r1.xz, 0).xyzw;
  r1.xyzw = t2.SampleLevel(s2_s, r1.xz, 0).xyzw;
  r2.xyz = -r4.xyz + r2.xyz;
  r2.xyz = r0.www * r2.xyz + r4.xyz;
  r3.xyz = r3.xyz + -r1.xyz;
  r1.xyz = r0.www * r3.xyz + r1.xyz;
  r2.xyz = r2.xyz + -r1.xyz;
  r1.xyz = cb0[132].xxx * r2.xyz + r1.xyz;
  r0.xyz = -r1.xyz + r0.xyz;
  r2.xyzw = t1.SampleBias(s1_s, v1.xy, cb0[5].x).xyzw;
  r0.w = cmp(r2.x == 20.000000);
  r0.w = r0.w ? 1.000000 : 0;
  r0.xyz = r0.www * r0.xyz + r1.xyz;
  r0.xyz = max(float3(6.10351999e-05,6.10351999e-05,6.10351999e-05), r0.xyz);
  r1.xyz = r0.xyz * float3(0.947867274,0.947867274,0.947867274) + float3(0.0521326996,0.0521326996,0.0521326996);
  r1.xyz = log2(r1.xyz);
  r1.xyz = float3(2.4000001,2.4000001,2.4000001) * r1.xyz;
  r1.xyz = exp2(r1.xyz);
  r2.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) < r0.xyz);
  r0.xyz = float3(0.0773993805,0.0773993805,0.0773993805) * r0.xyz;
  o0.xyz = r2.xyz ? r1.xyz : r0.xyz;
  r0.xyzw = t0.SampleBias(s0_s, v1.xy, cb0[5].x).xyzw;
  oDepth = r0.x;


  o0.xyz = color.xyz;
  return;
}