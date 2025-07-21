// ---- Created with 3Dmigoto v1.4.1 on Thu Jul 17 23:38:57 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);


#include "../common.hlsl"

// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  float2 w1 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  o0.xyzw = t0.Sample(s0_s, w1.xy).xyzw;

 // o0.xyz = renodx::color::bt2020::from::BT709(o0.xyz);
  // .. o0.xyz = renodx::color::bt709::from::BT2020(o0.xyz);
  // o0 = saturate(o0);

//  o0 = saturate(o0);

  return;
}
