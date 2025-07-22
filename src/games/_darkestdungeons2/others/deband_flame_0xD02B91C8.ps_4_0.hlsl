#include "../custom.hlsl"

// ---- Created with 3Dmigoto v1.4.1 on Wed Jul  9 15:49:05 2025
Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb1 : register(b1)
{
  float4 cb1[5];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[24];
}



// 3Dmigoto declarations
#define cmp -

float4 deband(Texture2D<float4> tex, SamplerState sam, float2 v1, float strength) {
    float width, height;
    t0.GetDimensions(width, height);
    
    float4 sample_sum = float4(0, 0, 0, 0);
    float weight_sum = 0.0;
    const float num_samples = 1;
    float seed = shader_injection.custom_random;
    
    for (int i = 0; i < num_samples; i++) {
      float2 sample_pos = v1.xy;
      const float random_number_x = frac(renodx::random::Generate(sample_pos.xy + seed + 2*i));
      const float random_number_y = frac(renodx::random::Generate(sample_pos.xy + seed + 2*i + 1));

      const float random_offset_x = 1.f / width * strength * (random_number_x * 2.f - 1.f);
      const float random_offset_y = 1.f / height * strength * (random_number_y * 2.f - 1.f);

      sample_pos.xy += float2(random_offset_x, random_offset_y);
      sample_sum += tex.Sample(sam, sample_pos.xy);
    }
    return sample_sum / num_samples;
}


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  float4 v2 : TEXCOORD3,
  float4 v3 : TEXCOORD4,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyz = v3.xyz / v3.www;
  r1.xyzw = t1.SampleBias(s0_s, r0.xy, cb0[5].x).xyzw;
 // r1.xyzw = deband(t1, s0_s, r0.xy, 1.f);


  r0.x = cb0[23].z * r0.z + cb0[23].w;
  r0.x = 1 / r0.x;
  r0.y = cb0[23].z * r1.x + cb0[23].w;
  r0.y = 1 / r0.y;
  r0.x = r0.y + -r0.x;
  r0.x = r0.x / cb1[4].x;
  r0.x = min(1, abs(r0.x));
  r0.y = -cb0[21].y + v2.w;
  r0.y = -cb1[4].z + r0.y;
  r0.y = saturate(r0.y / cb1[4].y);
  r0.x = r0.x * r0.y;
  r1.xyzw = t0.Sample(s1_s, v2.xy).xyzw;
 // r1.xyzw = deband(t0, s1_s, v2.xy, 8.f);

  r0.y = saturate(r1.x * r1.w + -v2.z);
  r1.xyz = v1.xyz * r1.www;
  o0.xyz = cb1[0].xyz * r1.xyz;
  r0.y = log2(r0.y);
  r0.y = cb1[3].z * r0.y;
  r0.y = exp2(r0.y);
  r0.y = cb1[3].w * r0.y;
  r0.y = v1.w * r0.y;
  o0.w = saturate(r0.y * r0.x);
  return;
}