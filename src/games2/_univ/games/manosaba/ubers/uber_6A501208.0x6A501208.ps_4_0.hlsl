// ---- Created with 3Dmigoto v1.4.1 on Fri Jul 18 09:24:47 2025
Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[146];
}

#include "../../../custom.hlsl"

// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4;
  uint4 bitmask, uiDest;
  float4 fDest;

  // Sample input texture with bias
  const float2 half_vec = float2(0.5, 0.5);
  const float2 uv_offset = -cb0[131].xy * half_vec + cb0[28].xy;
  const float2 sample_uv = min(v1.xy, uv_offset);
  const float4 sampled_texture = t0.SampleBias(s0_s, sample_uv, cb0[4].x);
  const float3 color_v0 = renodx::color::srgb::DecodeSafe(sampled_texture.xyz);
  
  // Vignette effect
  const float vignette_enabled = cmp(0 < cb0[145].z);
  float3 color_v1 = color_v0;
  if (vignette_enabled != 0) {
    const float2 vignette_center = cb0[145].xy;
    const float2 vignette_offset = -vignette_center + v1.xy;
    const float vignette_scale = cb0[145].z;
    const float vignette_aspect = cb0[144].w;
    
    float4 vignette_calc;
    vignette_calc.xy = vignette_offset;
    vignette_calc.yz = vignette_scale * abs(vignette_calc.xy);
    vignette_calc.x = vignette_aspect * vignette_calc.y;
    
    const float vignette_dist_sq = dot(vignette_calc.xz, vignette_calc.xz);
    const float vignette_falloff = 1 + -vignette_dist_sq;
    const float vignette_clamped = max(0, vignette_falloff);
    const float vignette_log = log2(vignette_clamped);
    const float vignette_strength = cb0[145].w;
    const float vignette_factor = exp2(vignette_strength * vignette_log);
    
    const float3 vignette_color_boost = cb0[144].xyz;
    const float3 vignette_blend = float3(1,1,1) + -vignette_color_boost;
    const float3 vignette_result = vignette_factor * vignette_blend + vignette_color_boost;
    color_v1 = vignette_result * color_v0; // per channel color boost
  }

  // Apply brightness multiplier
  const float brightness_mult = cb0[136].w;
  const float3 color_v2 = brightness_mult * color_v1;
  
  float3 color_v3 = color_v2;
  if (RENODX_TONE_MAP_TYPE != 0.f) {
    color_v3 = saturate(color_v2);
  }
 
  // First LUT application (t2)
  const float lut1_enabled = cmp(0 < cb0[137].w);
  float3 color_v4 = color_v3;
  if (lut1_enabled != 0) {
    const float3 srgb_encoded = renodx::color::srgb::EncodeSafe(color_v3);
    const float3 lut1_sample = renodx::lut::SampleTetrahedral(t2, srgb_encoded);
    const float lut1_blend = cb0[137].w;
    const float3 lut1_result = lerp(srgb_encoded, lut1_sample, lut1_blend);
    color_v4 = renodx::color::srgb::DecodeSafe(lut1_result);
  }

  // Tone mapping and color space conversion
  float3 color_v5 = color_v4;
  if (RENODX_TONE_MAP_TYPE != 0.f) {
    const float3 bt2020_converted = max(0.f, renodx::color::bt2020::from::BT709(color_v4));
    const float pq_nits = 100.f;
    color_v5 = renodx::color::pq::Encode(bt2020_converted, pq_nits);
  }
  
  // Second LUT application (t1)
  const float3 color_v6 = renodx::lut::SampleTetrahedral(t1, color_v5);

  // Output encoding
  const float3 final_color = renodx::color::srgb::EncodeSafe(color_v6);
  o0.xyz = final_color;
  o0.w = 1;
  return;
}

/*
// Original implementation for reference:
// ---- Created with 3Dmigoto v1.4.1 on Sun Jul 20 01:31:30 2025
Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[146];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xy = -cb0[131].xy * float2(0.5,0.5) + cb0[28].xy;
  r0.xy = min(v1.xy, r0.xy);
  r0.xyzw = t0.SampleBias(s0_s, r0.xy, cb0[4].x).xyzw;
  r1.xyz = float3(0.0773993805,0.0773993805,0.0773993805) * r0.xyz;
  r2.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r0.xyz;
  r2.xyz = float3(0.947867334,0.947867334,0.947867334) * r2.xyz;
  r2.xyz = log2(abs(r2.xyz));
  r2.xyz = float3(2.4000001,2.4000001,2.4000001) * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r0.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r0.xyz);
  r0.xyz = r0.xyz ? r1.xyz : r2.xyz;
  r0.w = cmp(0 < cb0[145].z);
  if (r0.w != 0) {
    r1.xy = -cb0[145].xy + v1.xy;
    r1.yz = cb0[145].zz * abs(r1.xy);
    r1.x = cb0[144].w * r1.y;
    r0.w = dot(r1.xz, r1.xz);
    r0.w = 1 + -r0.w;
    r0.w = max(0, r0.w);
    r0.w = log2(r0.w);
    r0.w = cb0[145].w * r0.w;
    r0.w = exp2(r0.w);
    r1.xyz = float3(1,1,1) + -cb0[144].xyz;
    r1.xyz = r0.www * r1.xyz + cb0[144].xyz;
    r0.xyz = r1.xyz * r0.xyz;
  }
  r0.xyz = saturate(cb0[136].www * r0.xyz);
  r0.w = cmp(0 < cb0[137].w);
  if (r0.w != 0) {
    r1.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.xyz);
    r2.xyz = float3(12.9232101,12.9232101,12.9232101) * r0.xyz;
    r3.xyz = log2(r0.xyz);
    r3.xyz = float3(0.416666657,0.416666657,0.416666657) * r3.xyz;
    r3.xyz = exp2(r3.xyz);
    r3.xyz = r3.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
    r1.xyz = r1.xyz ? r2.xyz : r3.xyz;
    r2.xyz = cb0[137].zzz * r1.zxy;
    r0.w = floor(r2.x);
    r2.xw = float2(0.5,0.5) * cb0[137].xy;
    r2.yz = r2.yz * cb0[137].xy + r2.xw;
    r2.x = r0.w * cb0[137].y + r2.y;
    r3.xyzw = t2.SampleLevel(s0_s, r2.xz, 0).xyzw;
    r4.x = cb0[137].y;
    r4.y = 0;
    r2.xy = r4.xy + r2.xz;
    r2.xyzw = t2.SampleLevel(s0_s, r2.xy, 0).xyzw;
    r0.w = r1.z * cb0[137].z + -r0.w;
    r2.xyz = r2.xyz + -r3.xyz;
    r2.xyz = r0.www * r2.xyz + r3.xyz;
    r2.xyz = r2.xyz + -r1.xyz;
    r1.xyz = cb0[137].www * r2.xyz + r1.xyz;
    r2.xyz = float3(0.0773993805,0.0773993805,0.0773993805) * r1.xyz;
    r3.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r1.xyz;
    r3.xyz = float3(0.947867334,0.947867334,0.947867334) * r3.xyz;
    r3.xyz = log2(abs(r3.xyz));
    r3.xyz = float3(2.4000001,2.4000001,2.4000001) * r3.xyz;
    r3.xyz = exp2(r3.xyz);
    r1.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r1.xyz);
    r0.xyz = r1.xyz ? r2.xyz : r3.xyz;
  }
  r0.xyw = cb0[136].zzz * r0.xyz;
  r0.w = floor(r0.w);
  r1.xy = float2(0.5,0.5) * cb0[136].xy;
  r1.yz = r0.xy * cb0[136].xy + r1.xy;
  r1.x = r0.w * cb0[136].y + r1.y;
  r2.xyzw = t1.SampleLevel(s0_s, r1.xz, 0).xyzw;
  r0.x = cb0[136].y;
  r0.y = 0;
  r0.xy = r1.xz + r0.xy;
  r1.xyzw = t1.SampleLevel(s0_s, r0.xy, 0).xyzw;
  r0.x = r0.z * cb0[136].z + -r0.w;
  r0.yzw = r1.xyz + -r2.xyz;
  r0.xyz = r0.xxx * r0.yzw + r2.xyz;
  r1.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.xyz);
  r2.xyz = float3(12.9232101,12.9232101,12.9232101) * r0.xyz;
  r0.xyz = log2(abs(r0.xyz));
  r0.xyz = float3(0.416666657,0.416666657,0.416666657) * r0.xyz;
  r0.xyz = exp2(r0.xyz);
  r0.xyz = r0.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
  o0.xyz = r1.xyz ? r2.xyz : r0.xyz;
  o0.w = 1;
  return;
}
*/