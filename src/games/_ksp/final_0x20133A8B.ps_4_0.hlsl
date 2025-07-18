// ---- Created with 3Dmigoto v1.4.1 on Thu Jul 17 13:12:57 2025

SamplerState BlitSampler_s : register(s0);
Texture2D<float4> BlitTexture : register(t0);

#include "./common.hlsl"

// 3Dmigoto declarations
#define cmp -

void main(
    float2 v0: TEXCOORD0,
    float4 v1: SV_POSITION0,
    out float4 o0: SV_Target0)
{
  float4 color = BlitTexture.Sample(BlitSampler_s, v0.xy).xyzw;

  // Convert angle to radians
  float angle_rad = radians(shader_injection.effect_split_angle);

  // Compute the split line normal
  float2 normal = float2(cos(angle_rad), sin(angle_rad));

  // Vector from center to current pixel
  float2 from_center = float2(v1.x - 0.5, v1.y - 0.5);

  // Side of the line: dot > 0 is one side, < 0 is the other
  bool applyEffect = true;
  if (shader_injection.effect_split_mode != 0) {
    applyEffect = dot(from_center, normal) < shader_injection.effect_split_x;
  }

  color.rgb = renodx::draw::InvertIntermediatePass(color.rgb);
  color.rgb *= RENODX_DIFFUSE_WHITE_NITS / RENODX_GRAPHICS_WHITE_NITS;

  if (applyEffect) {
    color.rgb = renodx_ksp_apply_tonemap_and_boost(color.rgb);
  }

  color.rgb = renodx::draw::RenderIntermediatePass(color.rgb);

  o0.xyzw = color;

  return;
}