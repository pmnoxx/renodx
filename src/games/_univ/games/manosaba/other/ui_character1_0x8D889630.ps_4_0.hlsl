// ---- Created with 3Dmigoto v1.4.1 on Fri Jul 18 09:58:19 2025
Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[5];
}

#include "../../../common.hlsl"

// 3Dmigoto declarations
#define cmp -

/*
// Original decompiled implementation for reference:
void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  float2 vv2 : TEXCOORD0,
  float2 w2 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2;
  uint4 bitmask, uiDest;
  float4 fDest;
  float4 v2 = float4(vv2, w2);

  r0.xyzw = cmp(v2.xyzw >= float4(0,0,0,0));
  r0.xyzw = r0.xyzw ? float4(1,1,1,1) : 0;
  r1.xyzw = cmp(float4(1,1,1,1) >= v2.xyzw);
  r1.xyzw = r1.xyzw ? float4(1,1,1,1) : 0;
  r0.xyzw = r0.xyzw * r1.xyzw;
  r0.xy = -r0.xz * r0.yw + float2(1,1);
  r1.xyzw = t1.Sample(s1_s, w2.xy).xyzw;
  r1.xyzw = r0.yyyy * -r1.xyzw + r1.xyzw;
  r2.xyzw = t0.Sample(s0_s, v2.xy).xyzw;
  r0.xyzw = r0.xxxx * -r2.xyzw + r2.xyzw;
  r1.xyzw = r1.xyzw + -r0.xyzw;
  r0.xyzw = cb0[4].xxxx * r1.xyzw + r0.xyzw;
  o0.xyzw = v1.xyzw * r0.xyzw;
}
*/

void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  float2 vv2 : TEXCOORD0,
  float2 w2 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  // Combine texture coordinates
  const float4 v2 = float4(vv2, w2);
  
  // Calculate UV bounds checking
  const float4 uv_greater_equal_zero = cmp(v2.xyzw >= float4(0,0,0,0));
  const float4 uv_less_equal_one = cmp(float4(1,1,1,1) >= v2.xyzw);
  const float4 uv_in_bounds = uv_greater_equal_zero * uv_less_equal_one;
  
  // Calculate blend factors for texture mixing
  const float2 blend_factors = -uv_in_bounds.xz * uv_in_bounds.yw + float2(1,1);
  
  // Sample secondary texture (t1) and apply blend
  const float4 secondary_texture = t1.Sample(s1_s, w2.xy);
  const float4 secondary_blended = blend_factors.yyyy * -secondary_texture + secondary_texture;
  
  // Sample primary texture (t0) and apply blend
  const float4 primary_texture = t0.Sample(s0_s, v2.xy);
  const float4 primary_blended = blend_factors.xxxx * -primary_texture + primary_texture;
  
  // Mix textures based on blend factor
  const float4 texture_diff = secondary_blended + -primary_blended;
  const float4 final_texture = cb0[4].xxxx * texture_diff + primary_blended;
  
  // Apply vertex color
  const float4 color_result = v1.xyzw * final_texture;
  
  // Color space processing and brightness adjustment
  o0.xyzw = color_result;
  o0.xyz = renodx::color::srgb::DecodeSafe(o0.xyz);
  o0.xyz *= RENODX_CUSTOM_CHARACTER_BRIGHTNESS;
  o0.xyz = renodx::color::srgb::EncodeSafe(o0.xyz);
  o0.w = saturate(o0.w);
  
  return;
}