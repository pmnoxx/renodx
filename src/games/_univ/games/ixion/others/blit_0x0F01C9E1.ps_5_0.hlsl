
// ---- Created with 3Dmigoto v1.4.1 on Sun Jul 27 16:30:18 2025
Texture2DArray<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[3];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xy = v1.xy;
  r0.z = 0;
  o0.xyzw = t0.SampleLevel(s0_s, r0.xyz, cb0[2].x).xyzw;

  return;
}