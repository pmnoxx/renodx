// ---- Created with 3Dmigoto v1.4.1 on Sat Jul 12 12:36:33 2025

/*
// Original source code (reconstructed):
Texture2D<float4> bloomTexture : register(t0);
Texture2D<float4> bloomMask : register(t1);

SamplerState linearSampler : register(s0);
SamplerState pointSampler : register(s1);

cbuffer BloomConstants : register(b0)
{
    float4 screenSize;           // cb0[26].x
    float4 blurOffset;           // cb0[28].xy
    float4 bloomThreshold;       // cb0[31]
    float4 bloomIntensity;       // cb0[32].x
}

void main(
    float4 position : SV_POSITION0,
    float2 texCoord : TEXCOORD0,
    float2 screenPos : TEXCOORD1,
    out float4 output : SV_Target0)
{
    // 9-tap Gaussian blur with weights: center=0.125, corners=0.03125, edges=0.03125
    float4 center = bloomTexture.Sample(linearSampler, texCoord);
    
    // Sample 8 surrounding pixels
    float4 topLeft = bloomTexture.Sample(linearSampler, texCoord + blurOffset * float2(-0.5, -0.5));
    float4 topRight = bloomTexture.Sample(linearSampler, texCoord + blurOffset * float2(0.5, -0.5));
    float4 bottomLeft = bloomTexture.Sample(linearSampler, texCoord + blurOffset * float2(-0.5, 0.5));
    float4 bottomRight = bloomTexture.Sample(linearSampler, texCoord + blurOffset * float2(0.5, 0.5));
    
    float4 top = bloomTexture.Sample(linearSampler, texCoord + blurOffset * float2(0, -1));
    float4 bottom = bloomTexture.Sample(linearSampler, texCoord + blurOffset * float2(0, 1));
    float4 left = bloomTexture.Sample(linearSampler, texCoord + blurOffset * float2(-1, 0));
    float4 right = bloomTexture.Sample(linearSampler, texCoord + blurOffset * float2(1, 0));
    
    // Apply Gaussian weights
    float4 blurred = center * 0.125f;
    blurred += (topLeft + topRight + bottomLeft + bottomRight) * 0.03125f;
    blurred += (top + bottom + left + right) * 0.03125f;
    
    // Clamp to prevent HDR overflow
    blurred = min(blurred, 65504.0f);
    
    // Apply bloom mask
    float mask = bloomMask.Sample(pointSampler, texCoord).r;
    blurred *= mask;
    
    // Clamp to maximum bloom intensity
    blurred = min(blurred, bloomIntensity.x);
    
    // Apply bloom threshold and curve
    float luminance = max(max(blurred.r, blurred.g), blurred.b);
    float threshold = bloomThreshold.x;
    float knee = bloomThreshold.y;
    float curve = bloomThreshold.z;
    float intensity = bloomThreshold.w;
    
    float kneeOffset = max(0.00001f, luminance - threshold);
    float kneeOffsetClamped = min(curve, kneeOffset);
    float kneeWeight = intensity * kneeOffsetClamped;
    float kneeWeightClamped = max(kneeWeight, kneeOffset);
    float bloomFactor = kneeWeightClamped / luminance;
    
    output = bloomFactor * blurred;
}
*/

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[33];
}




// 3Dmigoto declarations
#define cmp -

#include "./common.h"

float4 saturate_bloom_sdr(float4 color) {
  color.xyzw = saturate(color.xyzw);
  if (RENODX_TONE_MAP_TYPE >= 1.f) {
    color.xyzw *= RENODX_3D_BLOOM;
  } else {
    color = saturate(color);
  }
  return color;
}


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  float2 w1 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4,r5;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = saturate(cb0[28].xyxy * float4(-0.5,-0.5,0.5,-0.5) + v1.xyxy);
  r0.xyzw = cb0[26].xxxx * r0.xyzw;
  r1.xyzw = t0.Sample(s0_s, r0.xy).xyzw;
  r1.xyzw = saturate(r1.xyzw);  

  r0.xyzw = t0.Sample(s0_s, r0.zw).xyzw;
  r0.xyzw = saturate_bloom_sdr(r0.xyzw);  


  r0.xyzw = r1.xyzw + r0.xyzw;
  r1.xyzw = saturate(cb0[28].xyxy * float4(-0.5,0.5,0.5,0.5) + v1.xyxy);
  r1.xyzw = cb0[26].xxxx * r1.xyzw;
  r2.xyzw = t0.Sample(s0_s, r1.xy).xyzw;
  r2.xyzw = saturate_bloom_sdr(r2.xyzw);  

  r1.xyzw = t0.Sample(s0_s, r1.zw).xyzw;
  r1.xyzw = saturate_bloom_sdr(r1.xyzw);  

  r0.xyzw = r2.xyzw + r0.xyzw;
  r0.xyzw = r0.xyzw + r1.xyzw;
  r1.xy = saturate(-cb0[28].xy + v1.xy);
  r1.xy = cb0[26].xx * r1.xy;
  r1.xyzw = t0.Sample(s0_s, r1.xy).xyzw;
  r1.xyzw = saturate_bloom_sdr(r1.xyzw);  


  r2.xyzw = saturate(cb0[28].xyxy * float4(0,-1,1,-1) + v1.xyxy);
  r2.xyzw = cb0[26].xxxx * r2.xyzw;
  r3.xyzw = t0.Sample(s0_s, r2.xy).xyzw;
  r3.xyzw = saturate_bloom_sdr(r3.xyzw);  

  r2.xyzw = t0.Sample(s0_s, r2.zw).xyzw;
  r2.xyzw = saturate_bloom_sdr(r2.xyzw);  



  r2.xyzw = r3.xyzw + r2.xyzw;
  r1.xyzw = r3.xyzw + r1.xyzw;
  r3.xy = saturate(v1.xy);
  r3.xy = cb0[26].xx * r3.xy;
  r3.xyzw = t0.Sample(s0_s, r3.xy).xyzw;
  r3.xyzw = saturate_bloom_sdr(r3.xyzw);  

  r1.xyzw = r3.xyzw + r1.xyzw;
  r4.xyzw = saturate(cb0[28].xyxy * float4(-1,0,1,0) + v1.xyxy);
  r4.xyzw = cb0[26].xxxx * r4.xyzw;

  r5.xyzw = t0.Sample(s0_s, r4.xy).xyzw;
  r5.xyzw = saturate_bloom_sdr(r5.xyzw);  

  r4.xyzw = t0.Sample(s0_s, r4.zw).xyzw;
  r4.xyzw = saturate_bloom_sdr(r4.xyzw);  

  r1.xyzw = r5.xyzw + r1.xyzw;
  r5.xyzw = r5.xyzw + r3.xyzw;
  r1.xyzw = float4(0.03125,0.03125,0.03125,0.03125) * r1.xyzw;
  r0.xyzw = r0.xyzw * float4(0.125,0.125,0.125,0.125) + r1.xyzw;
  r1.xyzw = r4.xyzw + r2.xyzw;
  r2.xyzw = r4.xyzw + r3.xyzw;
  r1.xyzw = r1.xyzw + r3.xyzw;
  r0.xyzw = r1.xyzw * float4(0.03125,0.03125,0.03125,0.03125) + r0.xyzw;
  r1.xyzw = saturate(cb0[28].xyxy * float4(-1,1,0,1) + v1.xyxy);
  r1.xyzw = cb0[26].xxxx * r1.xyzw;
  r3.xyzw = t0.Sample(s0_s, r1.zw).xyzw;
  r3.xyzw = saturate_bloom_sdr(r3.xyzw);  

  r1.xyzw = t0.Sample(s0_s, r1.xy).xyzw;
  r1.xyzw = saturate_bloom_sdr(r1.xyzw);  

  r4.xyzw = r5.xyzw + r3.xyzw;
  r1.xyzw = r4.xyzw + r1.xyzw;
  r0.xyzw = r1.xyzw * float4(0.03125,0.03125,0.03125,0.03125) + r0.xyzw;
  r1.xy = saturate(cb0[28].xy + v1.xy);
  r1.xy = cb0[26].xx * r1.xy;
  r1.xyzw = t0.Sample(s0_s, r1.xy).xyzw;

  r1.xyzw = saturate_bloom_sdr(r1.xyzw);  



  r1.xyzw = r2.xyzw + r1.xyzw;
  r1.xyzw = r1.xyzw + r3.xyzw;
  r0.xyzw = r1.xyzw * float4(0.03125,0.03125,0.03125,0.03125) + r0.xyzw;
  r0.xyzw = min(float4(65504,65504,65504,65504), r0.xyzw);
  r1.xyzw = t1.Sample(s1_s, v1.xy).xyzw;



  r0.xyzw = r1.xxxx * r0.xyzw;
  r0.xyzw = min(cb0[32].xxxx, r0.xyzw);
  r1.x = max(r0.x, r0.y);
  r1.x = max(r1.x, r0.z);
  r1.yz = -cb0[31].yx + r1.xx;
  r1.xy = max(float2(9.99999975e-05,0), r1.xy);
  r1.y = min(cb0[31].z, r1.y);
  r1.w = cb0[31].w * r1.y;
  r1.y = r1.w * r1.y;
  r1.y = max(r1.y, r1.z);
  r1.x = r1.y / r1.x;
  o0.xyzw = r1.xxxx * r0.xyzw;
  return;
}