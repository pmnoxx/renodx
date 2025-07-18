#include "./shared.h"

Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[139];
}




// 3Dmigoto declarations
#define cmp -

// Decrease luminance of color to fit into sRGB [0,1] before saturate
float3 FitLuminanceToSRGB(float3 color)
{
    // Compute BT.709 luminance
    float lum = renodx::color::y::from::BT709(color);

    // Find the maximum channel (to avoid over-bright colors)
    float max_channel = max(color.r, max(color.g, color.b));

    // Compute the scale needed to bring both luminance and max channel to <= 1.0
    float scale = 1.0;
    if (lum > 1.0)
    {
        scale = min(scale, 1.f / lum);
    }
    if (max_channel > 1.0)
    {
        //scale = min(scale, 1.f / max_channel);
    }

    return saturate(color * scale);
}

void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t0.SampleBias(s0_s, v1.xy, cb0[19].x).xyzw;
  r1.xyzw = t1.SampleBias(s0_s, v1.xy, cb0[19].x).xyzw;


  r0.w = cmp(0 < cb0[131].x);
  r1.xyz = cb0[130].xxx * r1.xyz;
  r0.xyz = r1.xyz * cb0[130].yzw + r0.xyz;


  r0.w = cmp(0 < cb0[138].z);
  if (r0.w != 0) {
    r1.xy = -cb0[138].xy + v1.xy;
    r1.yz = cb0[138].zz * abs(r1.xy);
    r1.x = cb0[137].w * r1.y;
    r0.w = dot(r1.xz, r1.xz);
    r0.w = 1 + -r0.w;
    r0.w = max(0, r0.w);
    r0.w = log2(r0.w);
    r0.w = cb0[138].w * r0.w;
    r0.w = exp2(r0.w);
    r1.xyz = float3(1,1,1) + -cb0[137].xyz;
    r1.xyz = r0.www * r1.xyz + cb0[137].xyz;
    r0.xyz = r1.xyz * r0.xyz;
  }
  r0.xyz = (cb0[128].www * r0.xyz);
  //r0.xyz = saturate(r0.xyz);

    r0.w = cmp(0 < cb0[129].w);
    if (r0.w != 0) {/*
      r1.xyz = float3(12.9200001,12.9200001,12.9200001) * r0.xyz;
      r2.xyz = log2(r0.xyz);
      r2.xyz = float3(0.416666657,0.416666657,0.416666657) * r2.xyz;
      r2.xyz = exp2(r2.xyz);
      r2.xyz = r2.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
      r3.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.xyz);
      r1.xyz = r3.xyz ? r1.xyz : r2.xyz;*/

      r1.xyz = renodx::color::srgb::EncodeSafe(r0.xyz);

      r2.xyz = cb0[129].zzz * r1.zxy; 
      r0.w = floor(r2.x);
      r2.xw = float2(0.5,0.5) * cb0[129].xy;
      r2.yz = r2.yz * cb0[129].xy + r2.xw;
      r2.x = r0.w * cb0[129].y + r2.y;
      r3.xyzw = t3.SampleLevel(s0_s, r2.xz, 0).xyzw;
      r4.x = cb0[129].y;
      r4.y = 0;
      r2.xy = r4.xy + r2.xz;
      r2.xyzw = t3.SampleLevel(s0_s, r2.xy, 0).xyzw;
      r0.w = r1.z * cb0[129].z + -r0.w;
      r2.xyz = r2.xyz + -r3.xyz;
      r2.xyz = r0.www * r2.xyz + r3.xyz;
      r2.xyz = r2.xyz + -r1.xyz;
      r1.xyz = cb0[129].www * r2.xyz + r1.xyz; // saturation or desaturation LUT


      r0.xyz = renodx::color::srgb::DecodeSafe(r1.xyz);
  /*
      r2.xyz = float3(0.0773993805,0.0773993805,0.0773993805) * r1.xyz;
      r3.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r1.xyz;
      r3.xyz = float3(0.947867334,0.947867334,0.947867334) * r3.xyz;
      r3.xyz = log2(abs(r3.xyz));
      r3.xyz = float3(2.4000001,2.4000001,2.4000001) * r3.xyz;
      r3.xyz = exp2(r3.xyz);
      r1.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r1.xyz);
      r0.xyz = r1.xyz ? r2.xyz : r3.xyz;*/
    }

    float slider = 0.f;

    float3 untonemapped = abs(r0.xyz);
    float y_untonemapped = renodx::color::y::from::BT709(abs(untonemapped));

    float3 reinhard_coef = 1. / (1. + y_untonemapped);
    if (v1.x >= slider) {
      r0.xyz = untonemapped * reinhard_coef;
      //r0.xyz = saturate(r0.xyz);
    } else {
      r0.xyz = (r0.xyz);
    }
    
    float3 untonemapped_sign = sign(r0.xyz);
    r0.xyz = saturate(abs(r0.xyz));
    r0.xyw = cb0[128].zzz * r0.xyz;

    r0.w = floor(r0.w);
    r1.xy = float2(0.5,0.5) * cb0[128].xy;
    r1.yz = r0.xy * cb0[128].xy + r1.xy;
    r1.x = r0.w * cb0[128].y + r1.y;
    r2.xyzw = t2.SampleLevel(s0_s, r1.xz, 0).xyzw;
    r0.x = cb0[128].y;
    r0.y = 0;
    r0.xy = r1.xz + r0.xy;
    r1.xyzw = t2.SampleLevel(s0_s, r0.xy, 0).xyzw;
    r0.x = r0.z * cb0[128].z + -r0.w;
    r0.yzw = r1.xyz + -r2.xyz;
    o0.xyz = r0.xxx * r0.yzw + r2.xyz;
    o0.xyz *= untonemapped_sign;

    // reverse reinhard
    if (v1.x > slider) {
      float y_graded = renodx::color::y::from::BT709(abs(o0.xyz));
      float y_graded_scaled = renodx::color::y::from::BT709(abs(o0.xyz)) * 1.f / (1.f - y_graded);

      //
      float3 tmp = renodx::color::ictcp::from::BT709(y_graded_scaled);
      o0.xyz = renodx::color::ictcp::from::BT709(o0.xyz);
      o0.x = tmp.x;
      o0.xyz = renodx::color::bt709::from::ICtCp(o0.xyz);
      //o0.xyz = o0.xyz / (1.f - y_graded);
    }

    if (v1.x < 0.5f) {/*
        float3 graded = o0.xyz;

        float y_graded = renodx::color::y::from::BT709(abs(o0.xyz));

        float3 color_scaled = o0.xyz * y_untonemapped / y_graded;

        float post_process_strength = 1.0;
        o0.xyz  = lerp(untonemapped, color_scaled, post_process_strength);
*/
  } else {  
    //  o0.rgb = renodx::draw::ToneMapPass(o0.rgb);
     // o0.xyz = o0.xyz;
  }
  o0.xyz = renodx::draw::RenderIntermediatePass(o0.xyz);
  o0.w = 1;

  return;
}
