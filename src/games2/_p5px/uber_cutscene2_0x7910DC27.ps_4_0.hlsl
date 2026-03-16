#include "./shared.h"

// ---- Created with 3Dmigoto v1.4.1 on Thu Jun 26 06:24:23 2025

// ============================================================================
// TEXTURE DECLARATIONS
// ============================================================================
// Input textures for various rendering passes
// t0: Main color/albedo texture
// t1: Normal map or secondary color texture  
// t2: Look-up table (LUT) for color grading
// t3: Vignette or mask texture
// t4: Additional color correction texture
Texture2D<float4> t4 : register(t4);
Texture2D<float4> t3 : register(t3);
Texture2D<float4> t2 : register(t2);
Texture2D<float4> t1 : register(t1);
Texture2D<float4> t0 : register(t0);

// ============================================================================
// SAMPLER DECLARATIONS  
// ============================================================================
// Sampler states for texture sampling with different filtering modes
// Similar to Unity's sampler2D with different wrap/filter settings
SamplerState s4_s : register(s4);
SamplerState s3_s : register(s3);
SamplerState s2_s : register(s2);
SamplerState s1_s : register(s1);
SamplerState s0_s : register(s0);

// ============================================================================
// CONSTANT BUFFER
// ============================================================================
// Contains shader parameters like matrices, colors, settings
// Similar to Unity's cbuffer or MaterialPropertyBlock
cbuffer cb0 : register(b0)
{
  float4 cb0[54]; // Array of 54 float4 values for various parameters
}

// ============================================================================
// UTILITY MACROS
// ============================================================================
// 3Dmigoto declarations
#define cmp - // Comparison operator for conditional branching

// ============================================================================
// MAIN PIXEL SHADER FUNCTION
// ============================================================================
// Input parameters:
//   v0: Screen position (SV_POSITION) - pixel coordinates in screen space
//   vv1: Primary UV coordinates (TEXCOORD0) - texture coordinates for main sampling
//   w1: Secondary UV coordinates (TEXCOORD1) - additional texture coordinates
// Output:
//   o0: Final pixel color (SV_Target0) - the color written to the render target
//
// Example input values:
//   v0: float4(1920, 1080, 0, 1) - screen position
//   vv1: float2(0.5, 0.5) - center of texture
//   w1: float2(0.25, 0.75) - secondary UV coordinates
void main(
  float4 v0 : SV_POSITION0,
  float2 vv1 : TEXCOORD0,
  float2 w1 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  // ============================================================================
  // VARIABLE DECLARATIONS
  // ============================================================================
  // Temporary registers for calculations (similar to Unity's float4 variables)
  float4 r0,r1,r2,r3,r4;
  uint4 bitmask, uiDest;
  float4 fDest;

  // Combine UV coordinates into a single vector for easier manipulation
  float4 v1 = float4(vv1.x, vv1.y, w1.x, w1.y);

  // ============================================================================
  // SECTION 1: UV COORDINATE CALCULATION AND TEXTURE SAMPLING
  // ============================================================================
  // Calculate normalized UV coordinates with bounds checking
  // Similar to Unity's saturate() function for clamping values
  r0.xyzw = float4(1,1,1,1) + -cb0[29].zwzw;  // Calculate bounds: 1 - cb0[29].zw
  r0.xyzw = cb0[28].xxxx * r0.xyzw;           // Scale by cb0[28].x (likely viewport scale)
  r0.xyzw = min(v1.xyzw, r0.xyzw);            // Clamp UVs to valid range
  
  // Sample textures using calculated UV coordinates
  // Example: r1 = texture2D(t1, r0.xy) in Unity
  r1.xyzw = t1.Sample(s1_s, r0.xy).xyzw;      // Sample texture t1 (normal/color map)
  r2.xyzw = t0.Sample(s0_s, r0.zw).xyzw;      // Sample texture t0 (main color texture)

  // ============================================================================
  // SECTION 2: COLOR BLENDING AND MIXING
  // ============================================================================
  // Blend colors from different texture samples
  // Similar to Unity's color multiplication: finalColor = baseColor * overlayColor
  r1.xyz = r2.xyz * r1.xxx;  // Multiply main color by overlay color intensity

  // ============================================================================
  // SECTION 3: CONDITIONAL EFFECTS BRANCHING
  // ============================================================================
  // Check if certain effect is enabled (cb0[44].y < 0.5)
  // Similar to Unity's if statements in shaders
  r0.z = cmp(cb0[44].y < 0.5);
  if (r0.z != 0) {
    // ============================================================================
    // BRANCH A: VIGNETTE AND COLOR CORRECTION EFFECTS
    // ============================================================================
    // Calculate normalized screen coordinates
    r0.zw = r0.xy / cb0[28].xx;  // Normalize to 0-1 range
    
    // Calculate vignette effect (darkening at screen edges)
    // Similar to Unity's vignette post-processing effect
    r2.xy = -cb0[42].xy + r0.zw;  // Offset from center
    r2.yz = cb0[43].xx * abs(r2.yx);  // Apply vignette intensity
    
    // Calculate aspect ratio correction
    r1.w = cb0[29].x / cb0[29].y;  // Aspect ratio
    r1.w = -1 + r1.w;              // Normalize
    r1.w = cb0[43].w * r1.w + 1;   // Apply correction factor
    
    // Apply vignette transformation
    r2.x = r2.z * r1.w;
    r2.xy = saturate(r2.xy);       // Clamp to 0-1 range
    r2.xy = log2(r2.xy);           // Logarithmic transformation
    r2.xy = cb0[43].zz * r2.xy;    // Scale by vignette parameters
    r2.xy = exp2(r2.xy);           // Exponential transformation
    
    // Calculate vignette mask
    r1.w = dot(r2.xy, r2.xy);      // Distance from center
    r1.w = 1 + -r1.w;              // Invert for vignette effect
    r1.w = max(0, r1.w);           // Clamp to positive values
    r1.w = log2(r1.w);             // Logarithmic transformation
    r1.w = cb0[43].y * r1.w;       // Apply vignette strength
    r1.w = exp2(r1.w);             // Final vignette mask
    
    // Sample vignette texture
    r0.zw = r0.zw * cb0[41].xy + cb0[41].wz;  // Transform UVs for vignette texture
    r3.xyzw = t3.Sample(s3_s, r0.zw).xyzw;    // Sample vignette texture
    
    // Apply color correction from vignette texture
    // Similar to Unity's color grading with LUT
    r2.xyz = cb0[40].yzw * r3.xyz + float3(-1,-1,-1);  // Color correction
    r2.xyz = r3.www * r2.xyz + float3(1,1,1);          // Blend with white
    r3.xyz = float3(1,1,1) + -r2.xyz;                  // Calculate inverse
    r2.xyz = r1.www * r3.xyz + r2.xyz;                 // Apply vignette mask
    
    // Final color composition
    r4.xyz = r2.xyz * r1.xyz;      // Apply corrected color to base color
    
    // Alpha blending calculations
    r0.z = 1 + -r2.w;              // Inverse alpha
    r0.z = r3.w * r0.z + r2.w;     // Blend alpha values
    r0.w = r2.w + -r0.z;           // Alpha difference
    r4.w = r1.w * r0.w + r0.z;     // Final alpha value
    
  } else {
    // ============================================================================
    // BRANCH B: ALTERNATIVE COLOR PROCESSING
    // ============================================================================
    // Different color processing path when vignette is disabled
    
    // Calculate normalized coordinates
    r0.xy = r0.xy / cb0[28].xx;
    
    // Sample alternative color texture
    r0.xyzw = t4.Sample(s4_s, r0.xy).xyzw;
    
    // Gamma correction calculations
    // Similar to Unity's gamma correction in color space conversion
    r0.y = 0.0549999997 + r0.w;    // Add gamma offset
    r0.xy = float2(0.0773993805,0.947867334) * r0.wy;  // Gamma coefficients
    
    // Logarithmic transformation for gamma correction
    r0.y = max(1.1920929e-07, abs(r0.y));  // Prevent log(0)
    r0.y = log2(r0.y);                     // Logarithmic transformation
    r0.y = 2.4000001 * r0.y;               // Gamma power
    r0.y = exp2(r0.y);                     // Exponential transformation
    
    // Conditional gamma correction
    r0.z = cmp(0.0404499993 >= r0.w);      // Check threshold
    r0.x = r0.z ? r0.x : r0.y;             // Select appropriate gamma value
    
    // Color space transformation
    // Similar to Unity's color space conversion (sRGB to linear)
    r0.yzw = float3(1,1,1) + -cb0[40].yzw;  // Inverse color correction
    r0.yzw = r0.xxx * r0.yzw + cb0[40].yzw; // Apply color correction
    r0.yzw = r1.xyz * r0.yzw + -r1.xyz;     // Blend with original color
    r4.xyz = cb0[44].xxx * r0.yzw + r1.xyz; // Final color blend
    
    // Alpha calculation
    r0.y = -1 + r2.w;              // Alpha adjustment
    r4.w = r0.x * r0.y + 1;        // Final alpha
  }

  // ============================================================================
  // SECTION 4: TONE MAPPING AND COLOR GRADING
  // ============================================================================
  // Store intermediate result and untonemapped color for later use
  r0.xyzw = r4.xyzw;
  float3 untonemapped = r4.xyz;

  // Conditional tone mapping based on RENODX_TONE_MAP_TYPE
  // Similar to Unity's HDR to SDR tone mapping
  if (RENODX_TONE_MAP_TYPE > 0.f) {
    // ============================================================================
    // BRANCH A: PQ-BASED TONE MAPPING WITH LUT
    // ============================================================================
    // Convert to PQ color space for HDR processing
    // Similar to Unity's HDR color space conversion
    float3 lut_input = renodx::color::pq::Encode(untonemapped, 100.f);
    // Alternative: float3 lut_input = renodx::color::pq::Encode(untonemapped, 220.f);
    
    // Sample color grading LUT
    // Similar to Unity's color grading with 3D LUT
    r0.xyz = renodx::lut::Sample(t2, s2_s, saturate(lut_input), cb0[39].yzw);
    
  } else {
    // ============================================================================
    // BRANCH B: ARITHMETIC TONE MAPPING
    // ============================================================================
    // Traditional arithmetic tone mapping without PQ encoding
    
    // Scale color values
    r0.xyzw = cb0[40].xxxx * r4.xyzw;
    
    // Logarithmic color transformation
    // Similar to Unity's logarithmic tone mapping
    r1.xyz = r0.zxy * float3(5.55555582,5.55555582,5.55555582) + float3(0.0479959995,0.0479959995,0.0479959995);
    r1.xyz = log2(r1.xyz);
    r1.xyz = saturate(r1.xyz * float3(0.0734997839,0.0734997839,0.0734997839) + float3(0.386036009,0.386036009,0.386036009));
    
    // LUT sampling with bilinear interpolation
    // Similar to Unity's texture sampling with interpolation
    r1.yzw = cb0[39].www * r1.xyz;  // Scale for LUT coordinates
    r1.y = floor(r1.y);             // Integer part for LUT indexing
    r2.xy = float2(0.5,0.5) * cb0[39].yz;  // LUT center offset
    r2.yz = r1.zw * cb0[39].yz + r2.xy;    // Calculate LUT coordinates
    r2.x = r1.y * cb0[39].z + r2.y;        // Final LUT X coordinate
    
    // Sample LUT with bilinear interpolation
    r3.xyzw = t2.Sample(s2_s, r2.xz).xyzw;  // Sample first LUT point
    r4.x = cb0[39].z;                       // LUT step size
    r4.y = 0;
    r1.zw = r4.xy + r2.xz;                  // Calculate second LUT point
    r2.xyzw = t2.Sample(s2_s, r1.zw).xyzw;  // Sample second LUT point
    
    // Interpolate between LUT samples
    r1.x = r1.x * cb0[39].w + -r1.y;        // Interpolation factor
    r1.yzw = r2.xyz + -r3.xyz;              // Color difference
    r0.xyz = r1.xxx * r1.yzw + r3.xyz;      // Final interpolated color
  }

  // ============================================================================
  // SECTION 5: FINAL OUTPUT PROCESSING
  // ============================================================================
  // Set final output color
  o0.xyzw = r0.xyzw;

  // ============================================================================
  // SECTION 6: RENDER INTERMEDIATE PASS
  // ============================================================================
  // Apply intermediate rendering pass
  // Similar to Unity's intermediate rendering effects
  o0.xyz = renodx::draw::RenderIntermediatePass(o0.xyz);
  
  // ============================================================================
  // SECTION 7: FINAL COLOR PROCESSING AND GRAYSCALE
  // ============================================================================
  // Check if grayscale mode is enabled
  // Similar to Unity's grayscale post-processing effect
  r1.x = cmp(0.5 < cb0[53].y);
  if (r1.x != 0) {
    // Convert to grayscale using luminance weights
    // Similar to Unity's grayscale conversion: Y = 0.2126*R + 0.7152*G + 0.0722*B
    r1.xyz = (r0.xyz);
    r0.w = dot(r1.xyz, float3(0.212672904,0.715152204,0.0721750036));
  }
  
  // Return final pixel color
  return;
}