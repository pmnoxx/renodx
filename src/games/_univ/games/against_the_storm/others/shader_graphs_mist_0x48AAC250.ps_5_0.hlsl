// ---- Created with 3Dmigoto v1.4.1 on Thu Jul 10 17:09:07 2025
Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb1 : register(b1)
{
  float4 cb1[2];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[81];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float3 v1 : INTERP0,
  float3 v2 : INTERP1,
  float3 v3 : INTERP2,
  out float4 o0 : SV_TARGET0)
{
  float4 r0,r1,r2;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyz = cb0[74].xyw * v1.yyy;
  r0.xyz = cb0[73].xyw * v1.xxx + r0.xyz;
  r0.xyz = cb0[75].xyw * v1.zzz + r0.xyz;
  r0.xyz = cb0[76].xyw + r0.xyz;
  r1.xz = float2(0.5,0.5) * r0.xz;
  r0.x = cb0[17].x * r0.y;
  r1.w = 0.5 * r0.x;
  r0.xy = r1.xw + r1.zz;
  r0.xy = r0.xy / r0.zz;
  r0.w = cmp(cb0[21].w == 1.000000);
  if (r0.w != 0) {
    r0.w = t0.SampleBias(s0_s, r0.xy, cb0[19].x).x;
    r1.xy = r0.xy * float2(2,2) + float2(-1,-1);
    r2.xyzw = cb0[78].xyzw * -r1.yyyy;
    r1.xyzw = cb0[77].xyzw * r1.xxxx + r2.xyzw;
    r1.xyzw = cb0[79].xyzw * r0.wwww + r1.xyzw;
    r1.xyzw = cb0[80].xyzw + r1.xyzw;
    r1.xyz = r1.xyz / r1.www;
    r0.w = cb0[62].z * r1.y;
    r0.w = cb0[61].z * r1.x + r0.w;
    r0.w = cb0[63].z * r1.z + r0.w;
    r0.w = cb0[64].z + r0.w;
    r0.w = abs(r0.w);
  } else {
    r1.x = t0.SampleBias(s0_s, r0.xy, cb0[19].x).x;
    r1.x = cb0[20].z * r1.x + cb0[20].w;
    r0.w = 1 / r1.x;
  }
  r0.z = r0.w + -r0.z;
  r0.z = saturate(cb1[1].x * r0.z);
  o0.xyz = cb1[0].xyz * r0.zzz;
  r0.x = t1.SampleBias(s1_s, r0.xy, cb0[19].x).x;
  r0.x = 0.119999997 + r0.x;
  r0.x = log2(r0.x);
  r0.x = cb1[1].y * r0.x;
  r0.x = exp2(r0.x);
  r0.x = -0.189999998 + r0.x;
  r0.x = cb1[1].z * r0.x;
  o0.w = r0.z * r0.x;\

  // HOTFIX
  o0.w = saturate(o0.w);
  return;
}