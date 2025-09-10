#include "../common.hlsl"

// ---- Created with 3Dmigoto v1.4.1 on Sun Jul 20 13:49:31 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[29];
}

// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  float4 v2 : COLOR0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t0.Sample(s0_s, v1.xy).xyzw;
  r0.xyzw = r0.xyzw * r0.wwww;
  r1.xyz = r0.xyz;
  r1.w = cb0[28].w;
  r1.xyzw = cb0[28].xyzw * r1.xyzw;
  r0.x = cb0[28].w;
  r0.xyzw = r1.xyzw * r0.xxxw;
  r1.xyz = v2.xyz * v2.www;
  o0.xyz = r1.xyz * r0.xyz;
  o0.w = r0.w;

  // adjust brightness for atmoshpehere effects
 // o0.xyz = renodx::color::srgb::DecodeSafe(o0.xyz);
 // o0.xyz *= RENODX_DIFFUSE_WHITE_NITS / RENODX_GRAPHICS_WHITE_NITS;
//  o0.xyz = renodx::color::srgb::EncodeSafe(o0.xyz);

   // atmosphere scales with UI brightness, some other fix would be needed
  return;
} 
