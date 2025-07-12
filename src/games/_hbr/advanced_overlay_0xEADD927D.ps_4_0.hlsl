//#define NEW_CODE


#ifdef NEW_CODE
// Advanced Overlay Shader
// This shader applies an overlay effect to UI elements or character portraits

Texture2D<float4> overlayTexture : register(t1);  // The overlay texture (t1)
Texture2D<float4> baseTexture : register(t0);     // The base texture (t0)

SamplerState overlaySampler : register(s1);
SamplerState baseSampler : register(s0);

cbuffer Constants : register(b0)
{
    // Various shader constants including:
    // cb0[134].xy - UV offset for overlay
    // cb0[134].zw - UV scale for overlay  
    // cb0[135] - Overlay color/tint
    // cb0[5].x - LOD bias for texture sampling
    float4 cb0[136];
}

void main(
    float4 position : SV_POSITION0,
    float2 texCoord : TEXCOORD0,
    out float4 output : SV_Target0)
{
    // Calculate UV coordinates for overlay texture
    float2 overlayUV = texCoord * cb0[134].zw + cb0[134].xy;
    
    // Sample overlay texture
    float4 overlay = overlayTexture.SampleBias(overlaySampler, overlayUV, cb0[5].x);
    overlay.a = saturate(overlay.a); // Clamp alpha to prevent artifacts
    
    // Apply overlay color/tint
    float4 tintedOverlay = cb0[135] * overlay;
    
    // Calculate overlay blend factor
    float3 overlayBlend = 1.0 - overlay.rgb * cb0[135].rgb;
    
    // Sample base texture
    float4 base = baseTexture.SampleBias(baseSampler, texCoord, cb0[5].x);
    base.a = saturate(base.a); // Clamp alpha to prevent artifacts
    
    // Calculate blend factors for overlay effect
    float3 baseInvert = 1.0 - base.rgb;
    baseInvert = baseInvert * 2.0; // Double the inverted base
    
    // Apply overlay blend
    overlayBlend = 1.0 - baseInvert * overlayBlend;
    
    // Calculate final overlay contribution
    float3 overlayContribution = base.rgb * 2.0;
    tintedOverlay.rgb = overlayContribution * tintedOverlay.rgb - overlayBlend;
    
    // Calculate final alpha
    float finalAlpha = base.a * tintedOverlay.a;
    
    // Determine which pixels to apply overlay to (based on brightness)
    float3 brightnessMask = (base.rgb >= 0.5) ? 1.0 : 0.0;
    
    // Blend overlay with base
    float3 finalColor = brightnessMask * tintedOverlay.rgb + overlayBlend;
    finalColor = finalColor - base.rgb;
    
    // Apply final alpha blending
    output.rgb = finalAlpha * finalColor + base.rgb;
    output.a = 1.0;
}

#else

// ---- Created with 3Dmigoto v1.4.1 on Fri Jul 11 17:50:46 2025
Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[136];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xy = v1.xy * cb0[134].zw + cb0[134].xy;
  r0.xyzw = t1.SampleBias(s1_s, r0.xy, cb0[5].x).xyzw;
  r0.w = saturate(r0.w);

  r1.xyzw = cb0[135].xyzw * r0.xyzw;
  r0.xyz = -r0.xyz * cb0[135].xyz + float3(1,1,1);
  r2.xyzw = t0.SampleBias(s0_s, v1.xy, cb0[5].x).xyzw;
  r2.w = saturate(r2.w);

  r3.xyz = float3(1,1,1) + -r2.xyz;
  r3.xyz = r3.xyz + r3.xyz;
  r0.xyz = -r3.xyz * r0.xyz + float3(1,1,1);
  r3.xyz = r2.xyz + r2.xyz;
  r1.xyz = r3.xyz * r1.xyz + -r0.xyz;
  r0.w = r2.w * r1.w;
  r3.xyz = cmp(float3(0.5,0.5,0.5) >= r2.xyz);
  r3.xyz = r3.xyz ? float3(1,1,1) : 0;
  r0.xyz = r3.xyz * r1.xyz + r0.xyz;
  r0.xyz = r0.xyz + -r2.xyz;
  o0.xyz = r0.www * r0.xyz + r2.xyz;
  o0.w = 1;
  return;
}
#endif