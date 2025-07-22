#include "../custom.hlsl"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);
float4 main(float4 vpos: SV_POSITION, float2 uv: TEXCOORD0)
    : SV_TARGET {


  float4 color = t0.Sample(s0, uv);
 // color.xyz = renodx::draw::ToneMapPass(color.xyz);
  color.xyz = ApplyPerceptualBoostAndToneMap(color.xyz, SCENE_TYPE_2D_BACKGROND, uv);
  color.xyz = renodx::draw::RenderIntermediatePass(color.xyz);

  return renodx::draw::SwapChainPass(color);
}
