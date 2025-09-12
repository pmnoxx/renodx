// ---- Created with 3Dmigoto v1.4.1 on Thu Sep 11 23:28:42 2025
Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb1 : register(b1)
{
  float4 cb1[6];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[126];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : INTERP0,
  float3 v2 : INTERP1,
  float3 v3 : INTERP2,
  float3 v4 : INTERP3,
  out float4 o0 : SV_TARGET0)
{
  float4 r0,r1,r2,r3;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t2.SampleBias(s2_s, v1.xy, cb0[19].x).xyzw;
  r0 = saturate(r0);
  r1.x = cmp(r0.w < 0);
  if (r1.x != 0) discard;
  r1.xyzw = float4(-0.300000012,-0.150000006,-0.200000003,0) * cb0[125].xxxx;
  r1.xyzw = v1.xyxy * cb1[5].xyxy + r1.xyzw;
  r2.xyz = t0.SampleBias(s0_s, r1.xy, cb0[19].x).xyz;
  r2 = saturate(r2);
  r1.xyz = t0.SampleBias(s0_s, r1.zw, cb0[19].x).xyz;
  r1 = saturate(r1);
  r1.xyz = cb1[3].xyz * r1.xyz;
  r2.xyz = cb1[3].xyz * r2.xyz;
  r3.xyz = t1.SampleBias(s1_s, v1.xy, cb0[19].x).xyz;
  r3 = saturate(r3);
  r1.xyz = r3.xyz * r1.xyz;
  r1.xyz = r3.xyz * r2.xyz + r1.xyz;
  r1.xyz = r1.xyz / cb1[5].zzz;
  r1.xyz = cb1[2].yyy * r1.xyz;
  r1.w = 1 + -v1.x;
  r1.w = 0.899999976 * r1.w;
  r1.w = log2(r1.w);
  r1.w = cb1[2].x * r1.w;
  r1.w = exp2(r1.w);
  o0.xyz = r1.www * r1.xyz + r0.xyz;
  o0.w = r0.w;
  o0 = saturate(o0);
  return;
}