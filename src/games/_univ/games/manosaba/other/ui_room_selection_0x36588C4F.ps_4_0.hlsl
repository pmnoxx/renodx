// ---- Created with 3Dmigoto v1.4.1 on Fri Jul 18 12:31:40 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[3];
}

#include "../../../common.hlsl"


// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  float4 v2 : TEXCOORD0,
  float4 v3 : TEXCOORD1,
  float4 v4 : TEXCOORD2,
  out float4 o0 : SV_Target0)
{
  float4 r0;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t0.Sample(s0_s, v2.xy).xyzw;
  r0.xyzw = cb0[2].xyzw + r0.xyzw;
  o0.xyzw = v1.xyzw * r0.xyzw;
  o0 = saturate(o0);

  o0.xyz = renodx::color::srgb::DecodeSafe(o0.xyz);
  o0.xyzw *= RENODX_CUSTOM_CHARACTER_BRIGHTNESS; // room selection brightness
  o0.xyz = renodx::color::srgb::EncodeSafe(o0.xyz);

  o0.rgb = 0.5f;
  o0.w = saturate(o0.w);
  return;
}