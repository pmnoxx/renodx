#include "../custom.hlsl"


// ---- Created with 3Dmigoto v1.4.1 on Wed Jul 16 15:17:41 2025

// ============================================================================
// SECTION 1: TEXTURE AND SAMPLER DECLARATIONS
// ============================================================================
// Unity LutBuilderLDR uses 8 texture samplers for different LUT components
// These textures contain the 3D LUT data that maps input colors to output colors
Texture2D<float4> t7 : register(t7);  // LUT texture 7 - Additional color grading layer
Texture2D<float4> t6 : register(t6);  // LUT texture 6 - Additional color grading layer
Texture2D<float4> t5 : register(t5);  // LUT texture 5 - Additional color grading layer
Texture2D<float4> t4 : register(t4);  // LUT texture 4 - Additional color grading layer
Texture2D<float4> t3 : register(t3);  // LUT texture 3 - Blue channel LUT
Texture2D<float4> t2 : register(t2);  // LUT texture 2 - Green channel LUT
Texture2D<float4> t1 : register(t1);  // LUT texture 1 - Red channel LUT
Texture2D<float4> t0 : register(t0);  // LUT texture 0 - Master LUT (main color grading)

SamplerState s0_s : register(s0);  // Main sampler state for LUT texture sampling

// ============================================================================
// SECTION 2: CONSTANT BUFFER DECLARATION
// ============================================================================
// Unity's constant buffer containing 148 float4 values
// Includes color grading parameters, tone mapping settings, LUT configuration, etc.
cbuffer cb0 : register(b0)
{
  float4 cb0[148];  // Color grading parameters, matrices, curves, etc.
}

// ============================================================================
// SECTION 3: 3DMIGOTO COMPATIBILITY
// ============================================================================
// 3Dmigoto declarations for shader injection compatibility
#define cmp -

// ============================================================================
// SECTION 4: MAIN SHADER FUNCTION
// ============================================================================
// Unity LutBuilderLDR - Builds a 3D Look-Up Table for color grading
// This shader processes input coordinates and generates the final LUT output
void main(
  float4 v0 : SV_POSITION0,    // Screen position (not used in LUT building)
  float2 v1 : TEXCOORD0,       // UV coordinates for LUT sampling
  out float4 o0 : SV_Target0)  // Output color (the LUT entry)
{
  // Register declarations for intermediate calculations
  float4 r0,r1,r2,r3,r4,r5;
  uint4 bitmask, uiDest;
  float4 fDest;

  // ============================================================================
  // SECTION 5: INPUT COORDINATE PROCESSING
  // ============================================================================
  // Convert 2D UV coordinates to 3D LUT coordinates
  // This maps the 2D texture coordinates to 3D color space coordinates
  r0.x = cb0[132].x * v1.x;           // Scale X coordinate by LUT size
  r0.x = floor(r0.x);                 // Get integer part for LUT slice
  r1.x = v1.x * cb0[132].x + -r0.x;  // Get fractional part for interpolation
  r0.x = cb0[132].z * r0.x;           // Apply LUT scaling factor
  r0.z = cb0[132].w * r0.x;           // Calculate Z coordinate (blue channel)
  r1.y = v1.y;                        // Y coordinate (green channel)
  r0.xy = -cb0[132].zz + r1.xy;       // Apply coordinate offset
  r1.x = cb0[132].w;
  r1.z = 2;
  r0.xyz = r1.xxz * r0.xyz;           // Final 3D LUT coordinates (RGB)

  // ============================================================================
  // SECTION 6: RENODX HDR DECODING (MODIFICATION)
  // ============================================================================
  // RenoDX modification: Decode PQ-encoded HDR colors if tone mapping is enabled
  if (RENODX_TONE_MAP_TYPE > 0.f) {
    r0.xyz = renodx::color::pq::Decode(r0.xyz, 100.f);  // Decode from PQ to linear
    r0.xyz = renodx::color::bt709::from::BT2020(r0.xyz);
  }

  // ============================================================================
  // SECTION 7: COLOR SPACE TRANSFORMATION
  // ============================================================================
  // Convert RGB to XYZ color space using BT709 matrix
  // This allows for proper color grading in a perceptually uniform space
  r1.x = dot(float3(0.390404999,0.549941003,0.00892631989), r0.xyz);  // X component
  r1.y = dot(float3(0.070841603,0.963172019,0.00135775004), r0.xyz);  // Y component  
  r1.z = dot(float3(0.0231081992,0.128021002,0.936245024), r0.xyz);   // Z component
  
  // Apply color grading matrix from Unity's constant buffer
  r0.xyz = cb0[133].xyz * r1.xyz;
  
  // Convert XYZ back to RGB using inverse BT709 matrix
  r1.x = dot(float3(2.85846996,-1.62879002,-0.0248910002), r0.xyz);
  r1.y = dot(float3(-0.210181996,1.15820003,0.000324280991), r0.xyz);
  r1.z = dot(float3(-0.0418119989,-0.118169002,1.06867003), r0.xyz);

  // ============================================================================
  // SECTION 8: PQ (PERCEPTUAL QUANTIZER) ENCODING
  // ============================================================================
  // Apply PQ encoding for HDR color space
  // This converts linear RGB to PQ-encoded values for HDR processing
  r0.xyz = r1.xyz * float3(5.55555582,5.55555582,5.55555582) + float3(0.0479959995,0.0479959995,0.0479959995);
  r0.xyz = max(float3(0,0,0), r0.xyz);  // Clamp to positive values
  r0.xyz = log2(r0.xyz);                // Logarithmic transformation
  r0.xyz = r0.xyz * float3(0.0734997839,0.0734997839,0.0734997839) + float3(-0.0275523961,-0.0275523961,-0.0275523961);
  r0.xyz = r0.xyz * cb0[138].zzz + float3(0.0275523961,0.0275523961,0.0275523961);
  r0.xyz = float3(13.6054821,13.6054821,13.6054821) * r0.xyz;
  r0.xyz = exp2(r0.xyz);                // Exponential transformation
  r0.xyz = float3(-0.0479959995,-0.0479959995,-0.0479959995) + r0.xyz;
  r0.xyz = float3(0.179999992,0.179999992,0.179999992) * r0.xyz;

  // ============================================================================
  // SECTION 9: COLOR GRADING APPLICATION
  // ============================================================================
  // Apply Unity's color grading matrix
  r0.xyz = cb0[134].xyz * r0.xyz;
  r0.xyz = max(float3(0,0,0), r0.xyz);
  
  // Apply gamma correction (2.2 gamma)
  r0.xyz = log2(r0.xyz);
  r0.xyz = float3(0.454545468,0.454545468,0.454545468) * r0.xyz;  // 1/2.2
  r0.xyz = exp2(r0.xyz);

  // ============================================================================
  // SECTION 10: CONTRAST AND SATURATION CALCULATIONS
  // ============================================================================
  // Calculate contrast and saturation adjustments
  // These values are used for the color grading curves
  r1.xyz = r0.xyz + r0.xyz;            // 2x for contrast adjustment
  r2.xyz = r0.xyz * r0.xyz;            // Squared for saturation adjustment
  r3.xyz = min(float3(1,1,1), r0.xyz); // Clamped values for highlights
  r0.xyz = sqrt(r0.xyz);               // Square root for shadow adjustment
  
  // Calculate luminance using BT709 coefficients
  r0.w = dot(r3.xyz, float3(0.212672904,0.715152204,0.0721750036));
  r0.w = saturate(cb0[146].w + r0.w);
  r1.w = 1 + -r0.w;

  // ============================================================================
  // SECTION 11: FIRST COLOR GRADING PASS
  // ============================================================================
  // Apply first set of color grading adjustments
  // This implements Unity's color grading curves for shadows, midtones, highlights
  r3.xyz = float3(-0.5,-0.5,-0.5) + cb0[146].xyz;
  r3.xyz = r1.www * r3.xyz + float3(0.5,0.5,0.5);
  r4.xyz = -r3.xyz * float3(2,2,2) + float3(1,1,1);
  r2.xyz = r4.xyz * r2.xyz;
  r2.xyz = r1.xyz * r3.xyz + r2.xyz;
  
  // Conditional color processing based on threshold
  r4.xyz = cmp(r3.xyz >= float3(0.5,0.5,0.5));
  r5.xyz = r4.xyz ? float3(0,0,0) : float3(1,1,1);
  r4.xyz = r4.xyz ? float3(1,1,1) : 0;
  r2.xyz = r5.xyz * r2.xyz;
  r5.xyz = float3(1,1,1) + -r3.xyz;
  r3.xyz = r3.xyz * float3(2,2,2) + float3(-1,-1,-1);
  r1.xyz = r5.xyz * r1.xyz;
  r0.xyz = r0.xyz * r3.xyz + r1.xyz;
  r0.xyz = r0.xyz * r4.xyz + r2.xyz;

  // ============================================================================
  // SECTION 12: SECOND COLOR GRADING PASS
  // ============================================================================
  // Apply second set of color grading adjustments (similar to first pass)
  // This provides additional color grading control
  r1.xyz = r0.xyz + r0.xyz;
  r2.xyz = r0.xyz * r0.xyz;
  r0.xyz = sqrt(r0.xyz);
  r3.xyz = float3(-0.5,-0.5,-0.5) + cb0[147].xyz;
  r3.xyz = r0.www * r3.xyz + float3(0.5,0.5,0.5);
  r4.xyz = -r3.xyz * float3(2,2,2) + float3(1,1,1);
  r2.xyz = r4.xyz * r2.xyz;
  r2.xyz = r1.xyz * r3.xyz + r2.xyz;
  r4.xyz = cmp(r3.xyz >= float3(0.5,0.5,0.5));
  r5.xyz = r4.xyz ? float3(0,0,0) : float3(1,1,1);
  r4.xyz = r4.xyz ? float3(1,1,1) : 0;
  r2.xyz = r5.xyz * r2.xyz;
  r5.xyz = float3(1,1,1) + -r3.xyz;
  r3.xyz = r3.xyz * float3(2,2,2) + float3(-1,-1,-1);
  r1.xyz = r5.xyz * r1.xyz;
  r0.xyz = r0.xyz * r3.xyz + r1.xyz;
  r0.xyz = r0.xyz * r4.xyz + r2.xyz;

  // ============================================================================
  // SECTION 13: FINAL GAMMA CORRECTION
  // ============================================================================
  // Apply final gamma correction (2.2)
  r0.xyz = log2(abs(r0.xyz));
  r0.xyz = float3(2.20000005,2.20000005,2.20000005) * r0.xyz;
  r0.xyz = exp2(r0.xyz);

  // ============================================================================
  // SECTION 14: COLOR MATRIX TRANSFORMATION
  // ============================================================================
  // Apply final color matrix transformation for RGB channels
  r1.x = dot(r0.xyz, cb0[135].xyz);  // Red channel matrix
  r1.y = dot(r0.xyz, cb0[136].xyz);  // Green channel matrix
  r1.z = dot(r0.xyz, cb0[137].xyz);  // Blue channel matrix
  r0.xyz = cb0[143].xyz * r1.xyz;

  // ============================================================================
  // SECTION 15: CURVE ADJUSTMENTS
  // ============================================================================
  // Apply curve adjustments for shadows, midtones, and highlights
  // This implements Unity's color grading curves
  r0.w = dot(r1.xyz, float3(0.212672904,0.715152204,0.0721750036));  // Luminance
  r2.xy = -cb0[145].xz + r0.ww;
  r2.zw = cb0[145].yw + -cb0[145].xz;
  r2.zw = float2(1,1) / r2.zw;
  r2.xy = saturate(r2.xy * r2.zw);
  r2.zw = r2.xy * float2(-2,-2) + float2(3,3);
  r2.xy = r2.xy * r2.xy;
  r0.w = -r2.z * r2.x + 1;
  r1.w = 1 + -r0.w;
  r1.w = -r2.w * r2.y + r1.w;
  r2.x = r2.w * r2.y;
  r0.xyz = r1.www * r0.xyz;
  r2.yzw = cb0[142].xyz * r1.xyz;
  r1.xyz = cb0[144].xyz * r1.xyz;
  r0.xyz = r2.yzw * r0.www + r0.xyz;
  r0.xyz = r1.xyz * r2.xxx + r0.xyz;
  r0.xyz = r0.xyz * cb0[141].xyz + cb0[139].xyz;

  // ============================================================================
  // SECTION 16: SIGN PRESERVATION AND FINAL PROCESSING
  // ============================================================================
  // Preserve sign of colors and apply final adjustments
  r1.xyz = cmp(float3(0,0,0) < r0.xyz);
  r2.xyz = cmp(r0.xyz < float3(0,0,0));
  r0.xyz = log2(abs(r0.xyz));
  r0.xyz = cb0[140].xyz * r0.xyz;
  r0.xyz = exp2(r0.xyz);

 
  r1.xyz = (int3)-r1.xyz + (int3)r2.xyz;
  r1.xyz = (int3)r1.xyz;
  r2.xyz = r1.xyz * r0.xyz;


  // ============================================================================
  // SECTION 18: HSV COLOR SPACE CONVERSION
  // ============================================================================
  // Convert to HSV for hue/saturation adjustments
  // This allows for precise hue and saturation control
  r3.xy = r2.zy;
  r0.xy = r1.yz * r0.yz + -r3.xy;
  r1.x = cmp(r3.y >= r2.z);
  r1.x = r1.x ? 1.000000 : 0;
  r3.zw = float2(-1,0.666666687);
  r0.zw = float2(1,-1);
  r0.xyzw = r1.xxxx * r0.xywz + r3.xywz;
  r1.x = cmp(r2.x >= r0.x);
  r1.x = r1.x ? 1.000000 : 0;
  r3.z = r0.w;
  r0.w = r2.x;
  r2.x = dot(r2.xyz, float3(0.212672904,0.715152204,0.0721750036));
  r3.xyw = r0.wyx;
  r3.xyzw = r3.xyzw + -r0.xyzw;
  r0.xyzw = r1.xxxx * r3.xyzw + r0.xyzw;
  r1.x = min(r0.w, r0.y);
  r1.x = -r1.x + r0.x;
  r1.y = r1.x * 6 + 9.99999975e-05;
  r0.y = r0.w + -r0.y;
  r0.y = r0.y / r1.y;
  r0.y = r0.z + r0.y;
  r3.x = abs(r0.y);
  r2.z = cb0[138].x + r3.x;

  // ============================================================================
  // SECTION 19: LUT SAMPLING
  // ============================================================================
  // Sample from LUT textures for final color grading
  // This is where Unity's 3D LUT is actually applied
  r2.yw = float2(0,0);
  r4.xyzw = t4.SampleBias(s0_s, r2.zw, cb0[5].x).xyzw;  // Sample LUT texture 4
  r5.xyzw = t7.SampleBias(s0_s, r2.xy, cb0[5].x).xyzw;  // Sample LUT texture 7
  r5.x = (r5.x);
  r0.y = r5.x + r5.x;
  r4.x = (r4.x);
  r0.z = -0.5 + r4.x;
  r0.z = r2.z + r0.z;
  r0.w = cmp(1 < r0.z);
  r1.yz = (1,-1) + r0.zz;
  r0.w = r0.w ? r1.z : r0.z;
  r0.z = cmp(r0.z < 0);
  r0.z = r0.z ? r1.y : r0.w;

  // ============================================================================
  // SECTION 20: COLOR INTERPOLATION
  // ============================================================================
  // Interpolate between color values for smooth transitions
  // This ensures smooth color gradients in the LUT
  r1.yzw = float3(1,0.666666687,0.333333343) + r0.zzz;
  r1.yzw = frac(r1.yzw);
  r1.yzw = r1.yzw * float3(6,6,6) + float3(-3,-3,-3);
  r1.yzw = saturate(float3(-1,-1,-1) + abs(r1.yzw));
  r1.yzw = float3(-1,-1,-1) + r1.yzw;
  r0.z = 9.99999975e-05 + r0.x;
  r3.z = r1.x / r0.z;
  r1.xyz = r3.zzz * r1.yzw + float3(1,1,1);
  r2.xyz = r1.xyz * r0.xxx;
  r0.z = dot(r2.xyz, float3(0.212672904,0.715152204,0.0721750036));
  r1.xyz = r0.xxx * r1.xyz + -r0.zzz;

  // ============================================================================
  // SECTION 21: ADDITIONAL LUT SAMPLING
  // ============================================================================
  // Sample additional LUT textures for more color adjustments
  r3.yw = float2(0,0);
  r2.xyzw = t5.SampleBias(s0_s, r3.xy, cb0[5].x).xyzw;  // Sample LUT texture 5
  r3.xyzw = t6.SampleBias(s0_s, r3.zw, cb0[5].x).xyzw;  // Sample LUT texture 6
  r3.x = (r3.x);
  r0.x = r3.x + r3.x;
  r2.x = (r2.x);
  r0.w = r2.x + r2.x;
  r0.x = r0.w * r0.x;
  r0.x = r0.x * r0.y;
  r0.x = cb0[138].y * r0.x;
  r0.xyz = r0.xxx * r1.xyz + r0.zzz;

  // ============================================================================
  // SECTION XX: RENODX TONE MAPPING INTEGRATION (MODIFICATION)
  // ============================================================================
  // RenoDX modification: Apply tone mapping and create SDR version for LUT sampling
  float3 untonemapped = r0.xyz;
  /*float3 sdrTonemapped = renodx::tonemap::renodrt::NeutralSDR(untonemapped);  // tonemap to SDR you can change this to any SDR tonemapper you want
  if (RENODX_TONE_MAP_TYPE != 0) {
    if (RENODX_DEBUG_MODE2 > 0.5f) {
      sdrTonemapped = saturate(untonemapped);
      r0.xyz = sdrTonemapped;
    } else {
      float y = renodx::color::y::from::BT709(untonemapped);
      r0.xyz = saturate(lerp(untonemapped, sdrTonemapped, saturate(y)));
      sdrTonemapped = r0.xyz;
    }
  }
*/

  float3 untonemappedSign = sign(untonemapped);
  untonemapped = abs(untonemapped);
  float3 sdrTonemapped = saturate(untonemapped);
  r0.xyz = sdrTonemapped;

  // ============================================================================
  // SECTION 22: FINAL LUT SAMPLING AND OUTPUT
  // ============================================================================
  // Final LUT sampling and output preparation
  // This samples from the main LUT textures (t0-t3) for the final color
  r0.xyz = float3(0.00390625,0.00390625,0.00390625) + r0.xyz;  // Add small offset for sampling
  r0.w = 0;
  
  // Sample from main LUT textures for final color components
  r1.xyzw = t0.SampleBias(s0_s, r0.xw, cb0[5].x).xyzw;  // Red channel LUT
  r1.x = (r1.x);
  r2.xyzw = t0.SampleBias(s0_s, r0.yw, cb0[5].x).xyzw;  // Green channel LUT
  r0.xyzw = t0.SampleBias(s0_s, r0.zw, cb0[5].x).xyzw;  // Blue channel LUT
  r1.z = (r0.x);
  r1.y = (r2.x);
  
  // Combine color components
  r0.xyz = float3(0.00390625,0.00390625,0.00390625) + r1.xyz;
  r0.w = 0;
  
  // Final sampling from remaining LUT textures
  r1.xyzw = t1.SampleBias(s0_s, r0.xw, cb0[5].x).xyzw;
  o0.x = (r1.x);  // Final red output
  r1.xyzw = t2.SampleBias(s0_s, r0.yw, cb0[5].x).xyzw;
  r0.xyzw = t3.SampleBias(s0_s, r0.zw, cb0[5].x).xyzw;
  o0.z = (r0.x);  // Final blue output
  o0.y = (r1.x);  // Final green output
  o0.w = 1;               // Alpha = 1 (fully opaque)

  // ============================================================================
  // SECTION 23: RENODX FINAL TONE MAPPING (MODIFICATION)
  // ============================================================================
  // RenoDX modification: Apply final tone mapping if enabled
  if (RENODX_TONE_MAP_TYPE != 0) {
    float3 sdrGraded = o0.xyz;
    float3 color;
    color.r = renodx::tonemap::UpgradeToneMap(untonemapped.r, sdrTonemapped.r, sdrGraded.r, 1.f).r;
    color.g = renodx::tonemap::UpgradeToneMap(untonemapped.g, sdrTonemapped.g, sdrGraded.g, 1.f).r;
    color.b = renodx::tonemap::UpgradeToneMap(untonemapped.b, sdrTonemapped.b, sdrGraded.b, 1.f).r;
    o0.rgb = color;
  // o0.rgb = ToneMapPassWrapper(color);  // all 3 colors are in LINEAR here
  }
  o0.rgb *= untonemappedSign;

  return;
}
