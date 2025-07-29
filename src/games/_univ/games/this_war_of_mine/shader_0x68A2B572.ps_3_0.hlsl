
sampler2D InputTexture : register(s0);
sampler2D ToneMapLookup : register(s1);
float4 PerDrawCall = float4(0.25, 0.212599993, 0.715200007, 0.0722000003);

float4 main(float4 texcoord: TEXCOORD, float4 texcoord2: TEXCOORD1) : COLOR {
  // Force exact register usage to match original assembly
  float4 r0, r1, r2;

  // Sample and add in exact order
  r0 = tex2D(InputTexture, texcoord.xy);
  r1 = tex2D(InputTexture, texcoord.zw);
  r0.rgb = r0.rgb + r1.rgb;

  r1 = tex2D(InputTexture, texcoord2.xy);
  r0.rgb = r0.rgb + r1.rgb;

  r1 = tex2D(InputTexture, texcoord2.zw);
  r0.rgb = r0.rgb + r1.rgb;

  // Apply operations in exact order from original assembly
  r1.rgb = r0.rgb * PerDrawCall.x;
  r2.x = PerDrawCall.x;
  r0.rgb = r0.rgb * r2.x - 0.25;
  float3 saturated = saturate(r0.rgb + r0.rgb);

  r0.rgb = r1.rgb * 0.25;
  r0.rgb = r0.rgb * PerDrawCall.x;

  // Sample lookup texture
  r1.x = tex2D(ToneMapLookup, float2(r0.x, 0.0)).x;
  r2.y = tex2D(ToneMapLookup, float2(r0.y, 0.0)).y;
  r0.z = tex2D(ToneMapLookup, float2(r0.z, 0.0)).z;

  r1.z = r0.z;
  r1.y = r2.y;

  float alpha = dot(r1.rgb, PerDrawCall.yzw);

  if (true) {
    return float4(10.f, 0.f, 0.f, 1.f);
  }

  return float4(saturated, alpha);
}


/*sampler2D InputTexture : register(s0);
sampler2D ToneMapLookup : register(s1);
float4 PerDrawCall;

float4 main(float4 texcoord: TEXCOORD, float4 texcoord2: TEXCOORD1) : COLOR {
  float4 o;
  o.r = 10.f;
  o.g = 0.f;
  o.b = 0.f;
  o.a = 1.f;


  return o;
}
*/

/*


// ---- Created with 3Dmigoto v1.4.1 on Mon Jul 28 18:14:54 2025

Texture2D<float4> InputTexture : register(t0);
Texture2D<float4> ToneMapLookup : register(t1);

// Sampler states
SamplerState InputTextureSampler : register(s0);
SamplerState ToneMapLookupSampler : register(s1);

// Constant buffer
cbuffer PerDrawCall : register(b0)
{
    float4 PerDrawCall;
}

// Input/output structures
struct PSInput
{
    float4 texcoord : TEXCOORD0;
    float4 texcoord1 : TEXCOORD1;
};

struct PSOutput
{
    float4 color : SV_Target0;
};

// 3Dmigoto declarations
#define cmp -

PSOutput main(PSInput input)
{
    PSOutput output;
    float4 r0, r1, r2;
    
    // Original code with fixes:
    // r0 = InputTexture.Sample(v0.xyzw);
    r0 = InputTexture.Sample(InputTextureSampler, input.texcoord.xy);
    
    // r1 = InputTexture.Sample(v0.zwzw);
    r1 = InputTexture.Sample(InputTextureSampler, input.texcoord.zw);
    
    // r0.xyz = r1.xyz + r0.xyz;
    r0.xyz = r1.xyz + r0.xyz;
    
    // r1 = InputTexture.Sample(v1.xyzw);
    r1 = InputTexture.Sample(InputTextureSampler, input.texcoord1.xy);
    
    // r0.xyz = r1.xyz + r0.xyz;
    r0.xyz = r1.xyz + r0.xyz;
    
    // r1 = InputTexture.Sample(v1.zwzw);
    r1 = InputTexture.Sample(InputTextureSampler, input.texcoord1.zw);
    
    // r0.xyz = r1.xyz + r0.xyz;
    r0.xyz = r1.xyz + r0.xyz;
    
    // r1.xyz = c0.x 0.250000 * r0.xyz;
    r1.xyz = 0.250000 * r0.xyz;
    
    // r2.x = c0.x 0.250000;
    r2.x = 0.250000;
    
    // r0.xyz = r0.xyz * r2.x + -$PerDrawCall.x;
    r0.xyz = r0.xyz * r2.x + -PerDrawCall.x;
    
    // oC0.xyz = saturate(r0.xyz + r0.xyz);
    output.color.xyz = saturate(r0.xyz + r0.xyz);
    
    // r0.xyz = $PerDrawCall.y * r1.xyz;
    r0.xyz = PerDrawCall.y * r1.xyz;
    
    // r0.xyz = c0.x 0.250000 * r0.xyz;
    r0.xyz = 0.250000 * r0.xyz;
    
    // r1 = ToneMapLookup.Sample(r0.x);
    r1 = ToneMapLookup.Sample(ToneMapLookupSampler, r0.x);
    
    // r2 = ToneMapLookup.Sample(r0.y);
    r2 = ToneMapLookup.Sample(ToneMapLookupSampler, r0.y);
    
    // r0 = ToneMapLookup.Sample(r0.z);
    r0 = ToneMapLookup.Sample(ToneMapLookupSampler, r0.z);
    
    // r1.z = r0.z;
    r1.z = r0.z;
    
    // r1.y = r2.y;
    r1.y = r2.y;
    
    // oC0.w = dot(r1.xyz, c0.yzw 0.212600 0.715200 0.072200);
    output.color.w = dot(r1.xyz, float3(0.212600, 0.715200, 0.072200));

    // test code
    output.color = float4(10.0, 0.0, 0.0, 1.0);
    
    return output;
}
*/