// ---- Created with 3Dmigoto v1.4.1 on Fri Jun 27 15:24:53 2025

cbuffer _Globals : register(b0)
{
  row_major float4x4 gRemapMatrix : packoffset(c0);
  float4 gRemapVector : packoffset(c4);
}

SamplerState gPrimitiveTexture_state_s : register(s0);
Texture2D<float4> gPrimitiveTexture : register(t0);


// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : COLOR0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_TARGET0)
{
  float4 r0,r1;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = gPrimitiveTexture.Sample(gPrimitiveTexture_state_s, v1.xy).xyzw;

 // r0.xyzw = max(0.f, r0.xyzw);
 // r0.xyzw = min(1.f, r0.xyzw);
  r0.w = min(1.f, r0.w);

  r1.x = dot(gRemapMatrix._m00_m01_m02_m03, r0.xyzw);
  r1.y = dot(gRemapMatrix._m10_m11_m12_m13, r0.xyzw);
  r1.z = dot(gRemapMatrix._m20_m21_m22_m23, r0.xyzw);
  r1.w = dot(gRemapMatrix._m30_m31_m32_m33, r0.xyzw);
  r0.xyzw = gRemapVector.xyzw + r1.xyzw;
  o0.xyzw = v0.xyzw * r0.xyzw;
  return;
}