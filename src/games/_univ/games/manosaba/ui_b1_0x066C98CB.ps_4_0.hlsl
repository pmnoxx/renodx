// ---- Created with 3Dmigoto v1.4.1 on Fri Jul 18 08:59:24 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[4];
}

// 3Dmigoto declarations
#define cmp -

#include "../../common.hlsl"

/*
// Original decompiled implementation for reference:
void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  float4 v2 : TEXCOORD0,
  float4 v3 : TEXCOORD1,
  float4 v4 : TEXCOORD2,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.x = 255 * v1.w;
  r0.x = round(r0.x);
  r0.w = 0.00392156886 * r0.x;
  r1.xyzw = t0.Sample(s0_s, v2.xy).xyzw;
  r1.xyzw = cb0[3].xyzw + r1.xyzw;
  r0.xyz = v1.xyz;
  r0.xyzw = r1.xyzw * r0.xyzw;
  o0.xyz = r0.xyz * r0.www;
  o0.w = r0.w;
}
*/

void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  float4 v2 : TEXCOORD0,
  float4 v3 : TEXCOORD1,
  float4 v4 : TEXCOORD2,
  out float4 o0 : SV_Target0)
{
  // Alpha channel processing
  const float alpha_scaled = 255 * v1.w;
  const float alpha_rounded = round(alpha_scaled);
  const float alpha_normalized = 0.00392156886 * alpha_rounded; // 1/255
  
  // Sample texture and apply offset
  const float4 sampled_texture = t0.Sample(s0_s, v2.xy);
  const float4 texture_with_offset = cb0[3].xyzw + sampled_texture;
  
  // Color blending
  const float3 input_color = v1.xyz;
  const float4 blended_color = texture_with_offset * float4(input_color, alpha_normalized);
  
  // Final output with alpha premultiplication
  const float3 final_color = blended_color.xyz * blended_color.www;
  const float final_alpha = blended_color.w;
  
  o0.xyz = final_color;
  o0.w = final_alpha;
  
  // Apply UI color adjustment
//  o0.rgba = renodx_adjust_ui_color(o0.rgba);
  
  return;
}

