#include "../common.hlsl"
#include "../../../shaders/draw.hlsl"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);
float4 main(float4 vpos: SV_POSITION, float2 uv_org: TEXCOORD0)
    : SV_TARGET {
  float2 uv = uv_org;
  if (RENODX_IS_UPSIDE_DOWN != 0.f) {
    uv.y = 1.f - uv.y;
  }


  float4 o0 = t0.Sample(s0, uv);

  if (RENODX_SIMULATE_RENDER_PASS > 0.f) {
    o0.rgb = renodx::draw::InvertIntermediatePass(o0.rgb);
    o0.rgb *= RENODX_DIFFUSE_WHITE_NITS / RENODX_GRAPHICS_WHITE_NITS;
    o0.rgb = renodx::draw::RenderIntermediatePass(o0.rgb);
  }

  if (RENODX_ENABLE_UI_TONEMAPPASS > 0.f || SCENE_TYPE_DISPLAY_OUTPUT > 0.f) {
    o0.rgb = renodx::draw::DecodeColor(o0.rgb, RENODX_SWAP_CHAIN_DECODING);

    o0.rgb = renodx::color::bt709::clamp::BT2020(o0.rgb);

    if (SCENE_TYPE_DISPLAY_OUTPUT > 0.f) {
      o0.rgb = ApplyReverseReinhard(o0.rgb, SCENE_TYPE_DISPLAY_OUTPUT);
    }
    if (RENODX_ENABLE_UI_TONEMAPPASS > 0.f) {
      o0.rgb = ToneMapPassCustom(o0.rgb, 1.f);
    }

    o0.rgb = renodx::draw::EncodeColor(o0.rgb, RENODX_SWAP_CHAIN_DECODING);
  }

  float4 finalColor = renodx::draw::SwapChainPass(o0);
/*
  if (SCENE_TYPE_DISPLAY_OUTPUT > 0.f) {
    finalColor.rgb = renodx::draw::DecodeColor(finalColor.rgb, RENODX_SWAP_CHAIN_ENCODING);
    finalColor.rgb /= 203.f;
    if (RENODX_ENABLE_UI_TONEMAPPASS > 0.f) {
      finalColor.xyz = ToneMapPassCustom(finalColor.xyz, 1.f);
    }
    finalColor.rgb *= 203.f;
    finalColor.rgb = renodx::draw::EncodeColor(finalColor.rgb, RENODX_SWAP_CHAIN_ENCODING);
  }*/
  return finalColor;
}

/*
  float4 finalColor = renodx::draw::SwapChainPass(o0);

  if (RENODX_ENABLE_UI_TONEMAPPASS > 0.f || SCENE_TYPE_DISPLAY_OUTPUT > 0.f) {
    if (RENODX_SWAP_CHAIN_ENCODING == renodx::draw::ENCODING_PQ) {
      finalColor.rgb = renodx::color::pq::DecodeSafe(finalColor.rgb, RENODX_GRAPHICS_WHITE_NITS);
    } else if (RENODX_SWAP_CHAIN_ENCODING == renodx::draw::ENCODING_SCRGB) {
      finalColor.rgb *= 80.f / RENODX_GRAPHICS_WHITE_NITS;
    } else if (RENODX_SWAP_CHAIN_ENCODING == renodx::draw::ENCODING_SRGB) {
        finalColor.rgb = renodx::color::srgb::DecodeSafe(finalColor.rgb);
    }

    if (SCENE_TYPE_DISPLAY_OUTPUT > 0.f) {
      finalColor.xyz = ApplyReverseReinhard(finalColor.xyz, SCENE_TYPE_DISPLAY_OUTPUT);
    }
    if (RENODX_ENABLE_UI_TONEMAPPASS > 0.f) {
   //  finalColor.xyz = ToneMapPassCustom(finalColor.xyz, 1.f);
    }

    if (RENODX_SWAP_CHAIN_ENCODING == renodx::draw::ENCODING_PQ) {
      finalColor.rgb = renodx::color::pq::EncodeSafe(finalColor.rgb, RENODX_GRAPHICS_WHITE_NITS);
    } else if (RENODX_SWAP_CHAIN_ENCODING == renodx::draw::ENCODING_SCRGB) {
      finalColor.rgb /= 80.f / RENODX_GRAPHICS_WHITE_NITS;
    } else if (RENODX_SWAP_CHAIN_ENCODING == renodx::draw::ENCODING_SRGB) {
      finalColor.rgb = renodx::color::srgb::EncodeSafe(finalColor.rgb);
    }
  }

  return finalColor;
}
*/