// ---- Created with 3Dmigoto v1.4.1 on Sat Jul 12 13:24:07 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb1 : register(b1)
{
  float4 cb1[2];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[160];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : TEXCOORD0,
  float4 v2 : TEXCOORD5,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2;
  uint4 bitmask, uiDest;
  float4 fDest;

  if (false) { // debanding
    r0.xy = v2.xy * cb1[0].xy + cb1[0].zw;

    // Debanding implementation
    float width, height;
    t0.GetDimensions(width, height);
    
    float4 sample_sum = float4(0, 0, 0, 0);
    const float num_samples = 16; // Number of samples for debanding
    const float strength = 2.0; // Debanding strength
    
    for (int i = 0; i < num_samples; i++) {
      float2 sample_pos = r0.xy;
      float seed = frac(r0.x * 12.9898 + r0.y * 78.233 + i); // Simple hash for random seed
      
      // Generate pseudo-random offsets
      float random_x = frac(sin(dot(sample_pos + seed + float2(i * 0.1, i * 0.2), float2(12.9898, 78.233))) * 43758.5453);
      float random_y = frac(cos(dot(sample_pos + seed + float2(i * 0.3, i * 0.4), float2(12.9898, 78.233))) * 43758.5453);
      
      // Calculate random offset in pixel space
      float offset_x = (random_x * 2.0 - 1.0) * strength / width;
      float offset_y = (random_y * 2.0 - 1.0) * strength / height;
      
      sample_pos += float2(offset_x, offset_y);
      sample_sum += t0.Sample(s0_s, sample_pos);
    }
    
    r0.xyzw = sample_sum / num_samples;


    r0.xyzw = cb1[1].xyzw * r0.xyzw;
    r1.xyz = cb0[154].xyz * cb0[149].xyz;
    r2.xyz = cb0[159].xyz + cb0[155].xyz;
    o0.xyz = r0.xyz * r1.xyz + r2.xyz;
    o0.w = r0.w;

    return;
  }

  r0.xy = v2.xy * cb1[0].xy + cb1[0].zw;
  r0.xyzw = t0.Sample(s0_s, r0.xy).xyzw;
  r0.xyzw = cb1[1].xyzw * r0.xyzw;
  r1.xyz = cb0[154].xyz * cb0[149].xyz;
  r2.xyz = cb0[159].xyz + cb0[155].xyz;
  o0.xyz = r0.xyz * r1.xyz + r2.xyz;
  o0.w = r0.w;
  return;
}