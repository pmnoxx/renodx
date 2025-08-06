// ---- Created with 3Dmigoto v1.4.1 on Mon Aug  4 23:08:38 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[129];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  float2 w1 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = cb0[124].xyxy * float4(-1,-1,1,-1) + v1.xyxy;
  r1.xyzw = t0.SampleBias(s0_s, r0.xy, cb0[19].x).xyzw;
  r0.xyzw = t0.SampleBias(s0_s, r0.zw, cb0[19].x).xyzw;
  r0 = saturate(r0);
  r1 = saturate(r1);
  r0.xyzw = r1.xyzw + r0.xyzw;
  r1.xyzw = cb0[124].xyxy * float4(-1,1,1,1) + v1.xyxy;
  r2.xyzw = t0.SampleBias(s0_s, r1.xy, cb0[19].x).xyzw;
  r1.xyzw = t0.SampleBias(s0_s, r1.zw, cb0[19].x).xyzw;
  r1 = saturate(r1);
  r2 = saturate(r2);
  r0.xyzw = r2.xyzw + r0.xyzw;
  r0.xyzw = r0.xyzw + r1.xyzw;
  r0.xyzw = float4(0.25,0.25,0.25,0.25) * r0.xyzw;
  r0.xyzw = min(float4(65504,65504,65504,65504), r0.xyzw);
  r0.xyzw = min(cb0[128].xxxx, r0.xyzw);
  r1.x = max(r0.x, r0.y);
  r1.x = max(r1.x, r0.z);
  r1.yz = -cb0[127].yx + r1.xx;
  r1.xy = max(float2(9.99999975e-05,0), r1.xy);
  r1.y = min(cb0[127].z, r1.y);
  r1.w = cb0[127].w * r1.y;
  r1.y = r1.w * r1.y;
  r1.y = max(r1.y, r1.z);
  r1.x = r1.y / r1.x;
  o0.xyzw = r1.xxxx * r0.xyzw;

  o0 = 0;
  o0 = saturate(o0);
  return;
}