
// ---- Created with 3Dmigoto v1.4.1 on Fri Jun 27 15:22:15 2025

cbuffer _Globals : register(b0)
{
  row_major float4x4 gProjectionMatrix2D : packoffset(c0);
  float4 gOutputGamma : packoffset(c4);
}

#include "./shared.h"

SamplerState gNoFilterSamplerState_s : register(s0);
Texture2D<float4> gCopyViewportSampler : register(t0);


// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_Position0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = gCopyViewportSampler.Sample(gNoFilterSamplerState_s, v1.xy).xyzw;
  o0.xyzw = r0.xyzw;
 /* r0.xyz = log2(r0.xyz);
  o0.w = r0.w;
  r0.xyz = gOutputGamma.xxx * r0.xyz;
  o0.xyz = exp2(r0.xyz);
*/
  return;
}