#include "../common.hlsl"
#include "../../../shaders/draw.hlsl"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);
float4 main(float4 vpos: SV_POSITION, float2 uv: TEXCOORD0)
    : SV_TARGET {
  float4 o0 = t0.Sample(s0, uv);

  float4 finalColor = renodx::draw::SwapChainPass(o0);

  if (RENODX_ENABLE_UI_TONEMAPPASS > 0.f || SCENE_TYPE_DISPLAY_OUTPUT > 0.f) {
    if (RENODX_SWAP_CHAIN_ENCODING == renodx::draw::ENCODING_PQ) {
      finalColor.rgb = renodx::color::pq::DecodeSafe(finalColor.rgb, RENODX_DIFFUSE_WHITE_NITS);
    } else if (RENODX_SWAP_CHAIN_ENCODING == renodx::draw::ENCODING_SCRGB) {
      finalColor.rgb *= 80.f / RENODX_DIFFUSE_WHITE_NITS;
    } else if (RENODX_SWAP_CHAIN_ENCODING == renodx::draw::ENCODING_SRGB) {
        finalColor.rgb = renodx::color::srgb::DecodeSafe(finalColor.rgb);
    }

    finalColor.xyz = ApplyReverseReinhard(finalColor.xyz, SCENE_TYPE_DISPLAY_OUTPUT);
    finalColor.xyz = ToneMapPassCustom2(finalColor.xyz);

    if (RENODX_SWAP_CHAIN_ENCODING == renodx::draw::ENCODING_PQ) {
      finalColor.rgb = renodx::color::pq::EncodeSafe(finalColor.rgb, RENODX_DIFFUSE_WHITE_NITS);
    } else if (RENODX_SWAP_CHAIN_ENCODING == renodx::draw::ENCODING_SCRGB) {
      finalColor.rgb /= 80.f / RENODX_DIFFUSE_WHITE_NITS;
    } else if (RENODX_SWAP_CHAIN_ENCODING == renodx::draw::ENCODING_SRGB) {
      finalColor.rgb = renodx::color::srgb::EncodeSafe(finalColor.rgb);
    }
  }

  return finalColor;
}
