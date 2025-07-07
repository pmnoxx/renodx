#include "./shared.h"

Texture2D t0 : register(t0);
SamplerState s0 : register(s0);
float4 main(float4 vpos: SV_POSITION, float2 uv: TEXCOORD0)
    : SV_TARGET {

  float4 linearColor = t0.Sample(s0, uv);
  linearColor = pow(linearColor, 2.2f);
  if (RENODX_TONE_MAP_TYPE > 0.f) {


    float3 xyz = renodx::color::XYZ::from::BT709(linearColor.xyz);
    float xyz_grey = 0.18f;
    xyz = max(0.f, xyz);
    float new_grey = 0.18f;
    float y = max(0.f, xyz.y);

    if (y > 0.0000001f)
    {    
        float3 newXYZ = renodx::color::pq::Decode(
            renodx::color::pq::Encode(float3(xyz), 10000.f) * RENODX_PERCEPTUAL_BOOST_PARAM, 
            10000.f);
         new_grey = renodx::color::pq::Decode(
            renodx::color::pq::Encode(new_grey, 10000.f) * RENODX_PERCEPTUAL_BOOST_PARAM, 
            10000.f).r;

          xyz = lerp(
              xyz * (newXYZ.y / y),
              newXYZ.xyz,
              RENODX_PERCEPTUAL_BOOST_COLOR);
    }
    linearColor.xyz = lerp(linearColor, renodx::color::bt709::from::XYZ(xyz) * (0.18f / new_grey), RENODX_PERCEPTUAL_BOOST_STRENGTH);


    //o0.xyz = renodx::tonemap::UpgradeToneMap(untonemapped.rgb, neutral_sdr, o0.rgb, 1.f);
    linearColor.xyz = renodx::draw::ToneMapPass(linearColor.xyz);
  } else {
    linearColor = saturate(linearColor);
  }
      
  linearColor.xyz *= RENODX_DIFFUSE_WHITE_NITS / RENODX_GRAPHICS_WHITE_NITS;
  return renodx::draw::SwapChainPass(linearColor);
}
