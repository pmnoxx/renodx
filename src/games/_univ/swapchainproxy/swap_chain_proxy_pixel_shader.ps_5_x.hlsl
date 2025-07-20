
#include "../common.hlsl"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);

// Returns whether the effect should be applied based on split parameters
bool ShouldApplyEffect(float2 uv, float split_angle, float split_mode, float split_x) {
  float angle_rad = radians(split_angle);
  float2 normal = float2(cos(angle_rad), sin(angle_rad));
  float2 from_center = float2(uv.x - 0.5, uv.y - 0.5);
  if (split_mode != 0) {
    return dot(from_center, normal) < split_x;
  }
  return true;
}

float4 main(float4 vpos: SV_POSITION, float2 uv: TEXCOORD0)
    : SV_TARGET {
  float4 color = t0.Sample(s0, uv);

  bool applyEffect = ShouldApplyEffect(uv, shader_injection.effect_split_angle, shader_injection.effect_split_mode, shader_injection.effect_split_x);

  color.rgb = renodx::draw::InvertIntermediatePass(color.rgb);
  color.rgb *= RENODX_DIFFUSE_WHITE_NITS / RENODX_GRAPHICS_WHITE_NITS;

  if (applyEffect) {
    color.rgb = renodx_ksp_apply_tonemap_and_boost(color.rgb, uv.xy);
  }

  color.rgb = renodx::draw::RenderIntermediatePass(color.rgb);

  return renodx::draw::SwapChainPass(color);
}
