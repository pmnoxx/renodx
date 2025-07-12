#include "./shared.h"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);
float4 main(float4 vpos: SV_POSITION, float2 uv: TEXCOORD0)
    : SV_TARGET {

  float4 color = t0.Sample(s0, uv);

  // Convert angle to radians
  float angle_rad = radians(shader_injection.effect_split_angle);

  // Compute the split line normal
  float2 normal = float2(cos(angle_rad), sin(angle_rad));

  // Vector from center to current pixel
  float2 from_center = float2(uv.x - 0.5, uv.y - 0.5);

  // Side of the line: dot > 0 is one side, < 0 is the other
  bool applyEffect = true;
  if (shader_injection.effect_split_mode != 0) {
      applyEffect = dot(from_center, normal) < shader_injection.effect_split_x;
  }

  color.rgb = renodx::draw::InvertIntermediatePass(color.rgb);

  if (applyEffect) {
    color.rgb = renodx_ksp_apply_tonemap_and_boost(color.rgb);
  }
//  linearColor.xyz *= RENODX_DIFFUSE_WHITE_NITS / RENODX_GRAPHICS_WHITE_NITS;

  color.rgb = renodx::draw::RenderIntermediatePass(color.rgb);

  return renodx::draw::SwapChainPass(color);
}
