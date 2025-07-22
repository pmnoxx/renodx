#include "../custom.hlsl"

// ---- Created with 3Dmigoto v1.4.1 on Wed Jul  9 14:34:45 2025
TextureCube<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[5];
}




// 3Dmigoto declarations
#define cmp -

void main(
  float4 v0 : SV_POSITION0,
  float3 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0;
  uint4 bitmask, uiDest;
  float4 fDest;

  
  if (RENODX_REMOVE_BANDING) {
    // 8x sampling to eliminate banding with position-based weights
    float3 sample_offsets[4] = {
      float3(-1, -1, 0),
      float3(1, -1, 0),
      float3(-1, 1, 0),
      float3(1, 1, 0),
    };
    float width, height;
    t0.GetDimensions(width, height);
    
    float4 sample_sum = float4(0, 0, 0, 0);
    float weight_sum = 0.0;
    
    for (int i = 0; i < 4; i++) {
      float3 offset = sample_offsets[i] / float3(width, height, 1.);
      
      float3 sample_pos = v1.xyz + offset * 16;
      
      // Compute weight based on offset magnitude in texture space
      float offset_magnitude = length(sample_offsets[i]);
      float weight = 1.0 / (1.0 + offset_magnitude * 0.5); // Weight based on offset distance

      sample_pos = renodx::effects::ApplyFilmGrain(
        sample_pos,
          sample_pos,
          shader_injection.custom_random,
          1.f / 1024.f * 16.f,
          1.f
          );
     
      sample_sum += t0.Sample(s0_s, sample_pos) * weight;
      weight_sum += weight;
    }
    r0.xyzw = sample_sum / weight_sum;

  } else {
    
    r0.xyzw = t0.Sample(s0_s, v1.xyz).xyzw;
  }
  
  r0.w = -1 + r0.w;
  r0.w = cb0[2].w * r0.w + 1;
  r0.w = log2(r0.w);
  r0.w = cb0[2].y * r0.w;
  r0.w = exp2(r0.w);
  r0.w = cb0[2].x * r0.w;
  r0.xyz = r0.www * r0.xyz;
  r0.xyz = cb0[3].xyz * r0.xyz;
  r0.xyz = cb0[4].xxx * r0.xyz;
  o0.xyz = float3(4.5947938,4.5947938,4.5947938) * r0.xyz;
  o0.w = 1;
  return;
}