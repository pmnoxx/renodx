#include "./shared.h"


// ---- Created with 3Dmigoto v1.4.1 on Thu Jun 26 22:45:03 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[41];
}




// 3Dmigoto declarations
#define cmp -

// ============================================================================
// SECTION 0: COLOR SPACE CONVERSION UTILITIES
// ============================================================================
// ACES color space conversion matrices for Unity/ACEScg interoperability
static const half3x3 sRGB_2_AP1 = {
    0.61319, 0.33951, 0.04737,
    0.07021, 0.91634, 0.01345,
    0.02062, 0.10957, 0.86961
};

static const half3x3 AP1_2_sRGB = {
    1.70505, -0.62179, -0.08326,
    -0.13026, 1.14080, -0.01055,
    -0.02400, -0.12897, 1.15297,
};

half3 unity_to_ACEScg(half3 x)
{
    x = mul(sRGB_2_AP1, x);
    return x;
}

half3 ACEScg_to_unity(half3 x)
{
    x = mul(AP1_2_sRGB, x);
    return x;
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

  // ============================================================================
  // SECTION 1: INPUT PROCESSING AND COORDINATE CALCULATION
  // ============================================================================
  // Calculate UV coordinates with offset and scaling
  r0.yz = -cb0[30].yz + v1.xy;
  r1.x = cb0[30].x * r0.y;
  r0.x = frac(r1.x);
  r1.x = r0.x / cb0[30].x;
  r0.w = -r1.x + r0.y;

  // ============================================================================
  // SECTION 2: EARLY EXIT FOR DEBUGGING/DEVELOPMENT
  // ============================================================================
  // Quick debug output - return PQ decoded color directly
 /* if (true) {
    r0.xyz = renodx::color::pq::Decode(r0.xzw, 100.f);
    o0.xyz = r0.xyz;
    o0.w = 1.f;
    return;
  }*/

  // ============================================================================
  // SECTION 3: COLOR SPACE DECODING
  // ============================================================================
  // Decode from PQ (Perceptual Quantizer) or apply alternative decoding
  if (RENODX_TONE_MAP_TYPE > 0.f && true) {
    // Use PQ decoding for HDR content
    r0.xyz = renodx::color::pq::Decode(r0.xzw, 100.f);
  } else {
    // Alternative decoding path for non-PQ content
    // Apply offset and scaling
    r0.xyz = r0.xzw * cb0[30].www + float3(-0.386036009,-0.386036009,-0.386036009);
    r0.xyz = float3(13.6054821,13.6054821,13.6054821) * r0.xyz;
    r0.xyz = exp2(r0.xyz);
    r0.xyz = float3(-0.0479959995,-0.0479959995,-0.0479959995) + r0.xyz;
    r0.xyz = float3(0.179999992,0.179999992,0.179999992) * r0.xyz;
  }

  // ============================================================================
  // SECTION 4: COLOR SPACE TRANSFORMATION (RGB TO XYZ)
  // ============================================================================
  // Convert from RGB to CIE XYZ color space using standard matrix
  r1.x = dot(float3(0.439700991,0.382977992,0.177334994), r0.xyz);
  r1.y = dot(float3(0.0897922963,0.813422978,0.0967615992), r0.xyz);
  r1.z = dot(float3(0.0175439995,0.111543998,0.870703995), r0.xyz);
  r0.xyz = max(float3(0,0,0), r1.xyz);
  r0.xyz = min(float3(65504,65504,65504), r0.xyz);

  // ============================================================================
  // SECTION 5: LOGARITHMIC TRANSFORMATION
  // ============================================================================
  // Apply logarithmic transformation for tone mapping
  r1.xyz = r0.xyz * float3(0.5,0.5,0.5) + float3(1.525878e-05,1.525878e-05,1.525878e-05);
  r1.xyz = log2(r1.xyz);
  r1.xyz = float3(9.72000027,9.72000027,9.72000027) + r1.xyz;
  r1.xyz = float3(0.0570776239,0.0570776239,0.0570776239) * r1.xyz;
  r2.xyz = log2(r0.xyz);
  r0.xyz = cmp(r0.xyz < float3(3.05175708e-05,3.05175708e-05,3.05175708e-05));
  r2.xyz = float3(9.72000027,9.72000027,9.72000027) + r2.xyz;
  r2.xyz = float3(0.0570776239,0.0570776239,0.0570776239) * r2.xyz;
  r0.xyz = r0.xyz ? r1.xyz : r2.xyz;
  r0.xyz = float3(-0.413588405,-0.413588405,-0.413588405) + r0.xyz;
  r0.xyz = r0.xyz * cb0[34].zzz + float3(0.413588405,0.413588405,0.413588405);

  // ============================================================================
  // SECTION 6: EXPONENTIAL TRANSFORMATION AND CLAMPING
  // ============================================================================
  // Apply exponential transformation and clamp values
  r1.xyzw = cmp(r0.xxyy < float4(-0.301369876,1.46799636,-0.301369876,1.46799636));
  r0.xyw = r0.xyz * float3(17.5200005,17.5200005,17.5200005) + float3(-9.72000027,-9.72000027,-9.72000027);
  r2.xy = cmp(r0.zz < float2(-0.301369876,1.46799636));
  r0.xyz = exp2(r0.xyw);
  r1.yw = r1.yw ? r0.xy : float2(65504,65504);
  r0.xyw = float3(-1.52587891e-05,-1.52587891e-05,-1.52587891e-05) + r0.xyz;
  r0.z = r2.y ? r0.z : 65504;
  r0.xyw = r0.xyw + r0.xyw;
  r1.xy = r1.xz ? r0.xy : r1.yw;
  r1.z = r2.x ? r0.w : r0.z;

  // ============================================================================
  // SECTION 7: COLOR SPACE TRANSFORMATION (XYZ TO RGB)
  // ============================================================================
  // Convert back from XYZ to RGB color space
  r0.x = dot(float3(1.45143926,-0.236510754,-0.214928567), r1.xyz);
  r0.y = dot(float3(-0.0765537769,1.17622972,-0.0996759236), r1.xyz);
  r0.z = dot(float3(0.00831614807,-0.00603244966,0.997716308), r1.xyz);

  
  r1.x = dot(float3(0.390404999,0.549941003,0.00892631989), r0.xyz);
  r1.y = dot(float3(0.070841603,0.963172019,0.00135775004), r0.xyz);
  r1.z = dot(float3(0.0231081992,0.128021002,0.936245024), r0.xyz);

  // ============================================================================
  // SECTION 8: COLOR GRADING AND CORRECTION
  // ============================================================================
  // Apply color grading matrices and corrections
  r0.xyz = cb0[32].xyz * r1.xyz;
  r1.x = dot(float3(2.85846996,-1.62879002,-0.0248910002), r0.xyz);
  r1.y = dot(float3(-0.210181996,1.15820003,0.000324280991), r0.xyz);
  r1.z = dot(float3(-0.0418119989,-0.118169002,1.06867003), r0.xyz);
  r0.xyz = cb0[33].xyz * r1.xyz;
  r1.x = dot(r0.xyz, cb0[35].xyz);
  r1.y = dot(r0.xyz, cb0[36].xyz);
  r1.z = dot(r0.xyz, cb0[37].xyz);
  r0.xyz = r1.xyz * cb0[40].xyz + cb0[38].xyz;

  // ============================================================================
  // SECTION 9: TONE MAPPING AND COMPRESSION
  // ============================================================================
  // Apply tone mapping compression and saturation adjustment
  r1.xyz = log2(abs(r0.xyz));
  r0.xyz = saturate(r0.xyz * float3(3.40282347e+38,3.40282347e+38,3.40282347e+38) + float3(0.5,0.5,0.5));
  r0.xyz = r0.xyz * float3(2,2,2) + float3(-1,-1,-1);
  r1.xyz = cb0[39].xyz * r1.xyz;
  r1.xyz = exp2(r1.xyz);
  r0.xyz = r1.xyz * r0.xyz;
  r0.xyz = max(float3(0,0,0), r0.xyz);

  // ============================================================================
  // SECTION 10: HSV COLOR SPACE CONVERSION
  // ============================================================================
  // Convert RGB to HSV for saturation adjustment
  r0.w = cmp(r0.y >= r0.z);
  r0.w = r0.w ? 1.000000 : 0;
  r1.xy = r0.zy;
  r2.xy = -r1.xy + r0.yz;
  r1.zw = float2(-1,0.666666687);
  r2.zw = float2(1,-1);
  r1.xyzw = r0.wwww * r2.xywz + r1.xywz;
  r0.w = cmp(r0.x >= r1.x);
  r0.w = r0.w ? 1.000000 : 0;
  r2.z = r1.w;
  r1.w = r0.x;
  r3.x = dot(r0.xyz, float3(0.212672904,0.715152204,0.0721750036));
  r2.xyw = r1.wyx;
  r2.xyzw = r2.xyzw + -r1.xyzw;
  r0.xyzw = r0.wwww * r2.xyzw + r1.xyzw;
  r1.x = min(r0.w, r0.y);
  r1.x = -r1.x + r0.x;
  r1.y = r1.x * 6 + 9.99999975e-05;
  r0.y = r0.w + -r0.y;
  r0.y = r0.y / r1.y;
  r0.y = r0.z + r0.y;
  r2.x = abs(r0.y);
  r3.z = cb0[34].x + r2.x;
  r3.yw = float2(0.25,0.25);

  // ============================================================================
  // SECTION 11: LUT SAMPLING FOR COLOR CORRECTION
  // ============================================================================
  // Sample from 3D LUT texture for color correction
  r4.xyzw = t0.SampleLevel(s0_s, r3.zw, 0).xyzw;
  r5.xyzw = t0.SampleLevel(s0_s, r3.xy, 0).wxyz;
  r5.x = saturate(r5.x);
  r0.y = r5.x + r5.x;
  r4.x = saturate(r4.x);
  r0.z = r4.x + r3.z;
  r1.yzw = float3(-0.5,0.5,-1.5) + r0.zzz;
  r0.z = cmp(1 < r1.y);
  r0.z = r0.z ? r1.w : r1.y;
  r0.w = cmp(r1.y < 0);
  r0.z = r0.w ? r1.z : r0.z;
  r1.yzw = float3(1,0.666666687,0.333333343) + r0.zzz;
  r1.yzw = frac(r1.yzw);
  r1.yzw = r1.yzw * float3(6,6,6) + float3(-3,-3,-3);
  r1.yzw = saturate(float3(-1,-1,-1) + abs(r1.yzw));
  r1.yzw = float3(-1,-1,-1) + r1.yzw;
  r0.z = 9.99999975e-05 + r0.x;
  r2.z = r1.x / r0.z;
  r1.xyz = r2.zzz * r1.yzw + float3(1,1,1);
  r3.xyz = r1.xyz * r0.xxx;
  r0.z = dot(r3.xyz, float3(0.212672904,0.715152204,0.0721750036));
  r1.xyz = r0.xxx * r1.xyz + -r0.zzz;
  r2.yw = float2(0.25,0.25);
  r3.xyzw = t0.SampleLevel(s0_s, r2.xy, 0).yxzw;
  r2.xyzw = t0.SampleLevel(s0_s, r2.zw, 0).zxyw;
  r2.x = saturate(r2.x);
  r0.x = r2.x + r2.x;
  r3.x = saturate(r3.x);
  r0.w = r3.x + r3.x;
  r0.x = r0.w * r0.x;
  r0.x = r0.x * r0.y;
  r0.x = cb0[34].y * r0.x;
  r0.xyz = r0.xxx * r1.xyz + r0.zzz;

  // ============================================================================
  // SECTION 12: ACES TONE MAPPING ALGORITHM
  // ============================================================================
  // Apply ACES (Academy Color Encoding System) tone mapping
  // This is the core ACES implementation for HDR to SDR conversion
  
  float3 untonemapped = r0.xyz;

  if (RENODX_TONE_MAP_TYPE == 0.f) { // disabled unit it can be tested
    // ACES Input Transform Matrix (RGB to ACEScc)
    // r1.y = dot(float3(0.695452213,0.140678704,0.163869068), r0.xyz);  // ACEScc R coefficient
    // r1.z = dot(float3(0.0447945632,0.859671116,0.0955343172), r0.xyz); // ACEScc G coefficient  
    // r1.w = dot(float3(-0.00552588282,0.00402521016,1.00150073), r0.xyz); // ACEScc B coefficient
    r1.y = dot(float3(0.695452213,0.140678704,0.163869068), r0.xyz);
    r1.z = dot(float3(0.0447945632,0.859671116,0.0955343172), r0.xyz);
    r1.w = dot(float3(-0.00552588282,0.00402521016,1.00150073), r0.xyz);
    
    // ACES Tone Mapping Curve Parameters
    // r0.xyz = r1.wzy + -r1.zyw;  // Calculate chroma components for tone mapping
    r0.xyz = r1.wzy + -r1.zyw;
    r0.xy = r1.wz * r0.xy;        // Cross-multiply for chroma calculation
    r0.x = r0.x + r0.y;           // Sum chroma components
    r0.x = r1.y * r0.z + r0.x;    // Add luminance contribution
    r0.x = sqrt(r0.x);            // Calculate chroma magnitude
    
    // ACES Tone Mapping Curve Constants
    // r0.y = r1.w + r1.z + r1.y;  // Sum of all color components
    r0.y = r1.w + r1.z;
    r0.y = r0.y + r1.y;
    r0.x = r0.x * 1.75 + r0.y;    // ACES curve parameter: 1.75 (shoulder strength)
    
    // ACES Tone Mapping Thresholds and Parameters
    r0.y = 0.333333343 * r0.x;    // 1/3 for ACES curve calculation
    r0.y = 0.0799999982 / r0.y;   // ACES parameter: 0.08 (toe strength)
    
    // ACES Contrast and Saturation Parameters
    r0.z = min(r1.y, r1.z);       // Find minimum component
    r0.z = min(r0.z, r1.w);
    r0.z = max(9.99999975e-05, r0.z); // Prevent division by zero
    r0.w = max(r1.y, r1.z);       // Find maximum component
    r0.w = max(r0.w, r1.w);
    r2.xy = max(float2(9.99999975e-05,0.00999999978), r0.ww); // ACES contrast limits
    r0.z = r2.x + -r0.z;          // Calculate contrast range
    r0.z = r0.z / r2.y;           // Normalize contrast
    
    // ACES Tone Mapping Curve Offsets
    r0.yw = float2(-0.5,-0.400000006) + r0.yz; // ACES curve offsets
    r1.x = 2.5 * r0.w;            // ACES parameter: 2.5 (slope)
    
    // ACES Tone Mapping Curve Smoothing
    const float inf = 3.40282347e+38;
    r0.w = saturate(r0.w * inf + 0.5); // Smooth step function
    r0.w = r0.w * 2 + -1;         // Convert to [-1,1] range
    r1.x = 1 + -abs(r1.x);        // Calculate smooth falloff
    r1.x = max(0, r1.x);
    r1.x = -r1.x * r1.x + 1;      // Smooth curve
    r0.w = r0.w * r1.x + 1;       // Apply smoothing
    r0.w = 0.0250000004 * r0.w;   // ACES parameter: 0.025 (smoothing strength)
    r0.y = r0.w * r0.y;           // Apply tone mapping
    
    // ACES Tone Mapping Thresholds
    r1.x = cmp(r0.x >= 0.479999989); // ACES threshold: 0.48 (shoulder start)
    r0.x = cmp(0.159999996 >= r0.x); // ACES threshold: 0.16 (toe end)
    r0.y = r1.x ? 0 : r0.y;       // Apply shoulder curve
    r0.x = r0.x ? r0.w : r0.y;    // Apply toe curve
    r0.x = 1 + r0.x;              // Final tone mapping multiplier
    
    // Apply ACES tone mapping to color components
    r2.yzw = r1.yzw * r0.xxx;     // Scale colors by tone mapping result
    
    // ACES Output Transform Matrix (ACEScc to display RGB)
    // r0.y = -r1.y * r0.x + 0.0299999993;  // Output transform offset
    r0.y = -r1.y * r0.x + 0.0299999993;
    r0.w = r1.z * r0.x + -r2.w;   // Calculate intermediate values
    r0.w = 1.73205078 * r0.w;     // sqrt(3) for color space conversion
    r1.x = r2.y * 2 + -r2.z;      // Calculate chroma component
    r0.x = -r1.w * r0.x + r1.x;   // Final chroma calculation
    
    // ACES Color Space Conversion (Hue calculation)
    r1.x = max(abs(r0.w), abs(r0.x)); // Find maximum component for atan2
    r1.x = 1 / r1.x;              // Reciprocal for atan2 calculation
    r1.y = min(abs(r0.w), abs(r0.x)); // Find minimum component
    r1.x = r1.y * r1.x;           // Normalize for atan2
    r1.y = r1.x * r1.x;           // Square for polynomial approximation
    
    // ACES Atan2 Polynomial Approximation (optimized for GPU)
    r1.z = r1.y * 0.0208350997 + -0.0851330012; // Polynomial coefficient 1
    r1.z = r1.y * r1.z + 0.180141002;           // Polynomial coefficient 2
    r1.z = r1.y * r1.z + -0.330299497;          // Polynomial coefficient 3
    r1.y = r1.y * r1.z + 0.999866009;           // Polynomial coefficient 4
    r1.z = r1.x * r1.y;          // Apply polynomial
    r1.z = r1.z * -2 + 1.57079637; // Adjust for atan2 quadrant
    r1.w = cmp(abs(r0.x) < abs(r0.w)); // Determine atan2 quadrant
    r1.z = r1.w ? r1.z : 0;       // Select correct quadrant
    r1.x = r1.x * r1.y + r1.z;    // Final atan2 result
    
    // ACES Hue Angle Calculation and Normalization
    r1.y = cmp(r0.x < -r0.x);     // Check sign for angle correction
    r1.y = r1.y ? -3.141593 : 0;  // Add pi for negative angles
    r1.x = r1.x + r1.y;           // Apply angle correction
    r1.y = min(r0.w, r0.x);       // Find minimum for quadrant detection
    r0.x = max(r0.w, r0.x);       // Find maximum for quadrant detection
    r0.x = cmp(r0.x >= -r0.x);    // Check quadrant
    r0.w = cmp(r1.y < -r1.y);     // Check sign
    r0.x = r0.x ? r0.w : 0;       // Determine final sign
    r0.x = r0.x ? -r1.x : r1.x;   // Apply final sign
    r0.x = 57.2957802 * r0.x;     // Convert radians to degrees (180/pi)
    
    // ACES Hue Normalization to [0, 360] range
    r1.xy = cmp(r2.zw == r2.yz);  // Check for achromatic colors
    r0.w = r1.y ? r1.x : 0;       // Set hue to 0 for achromatic
    r0.x = r0.w ? 0 : r0.x;       // Apply achromatic correction
    r0.w = cmp(r0.x < 0);         // Check for negative angles
    r1.x = 360 + r0.x;            // Add 360 for negative angles
    r0.x = r0.w ? r1.x : r0.x;    // Normalize to [0, 360]
    r0.w = cmp(180 < r0.x);       // Check for angles > 180
    r1.xy = float2(360,-360) + r0.xx; // Calculate normalization values
    r0.w = r0.w ? r1.y : r0.x;    // Normalize to [-180, 180]
    r0.x = cmp(r0.x < -180);      // Check for angles < -180
    r0.x = r0.x ? r1.x : r0.w;    // Final normalization
    
    // ACES Saturation Adjustment Based on Hue
    r0.x = 0.0148148146 * r0.x;   // Scale hue for saturation calculation (1/67.5)
    r0.x = 1 + -abs(r0.x);        // Calculate saturation falloff
    r0.x = max(0, r0.x);          // Clamp to [0, 1]
    r0.w = r0.x * -2 + 3;         // Calculate smooth curve parameter
    r0.x = r0.x * r0.x;           // Square for smooth curve
    r0.x = r0.w * r0.x;           // Apply smooth curve
    r0.x = r0.x * r0.x;           // Fourth power for final curve
    r0.x = r0.x * r0.z;           // Apply contrast adjustment
    r0.x = r0.x * r0.y;           // Apply tone mapping adjustment
    r2.x = r0.x * 0.180000007 + r2.y; // ACES parameter: 0.18 (saturation scale)

    // ============================================================================
    // SECTION 13: COLOR SPACE TRANSFORMATION BACK TO RGB
    // ============================================================================
    // Transform back from ACES color space to RGB
    r0.x = dot(float3(1.45143926,-0.236510754,-0.214928567), r2.xzw);
    r0.y = dot(float3(-0.0765537769,1.17622972,-0.0996759236), r2.xzw);
    r0.z = dot(float3(0.00831614807,-0.00603244966,0.997716308), r2.xzw);
    r0.xyz = max(float3(0,0,0), r0.xyz);
    r0.w = dot(r0.xyz, float3(0.272228986,0.674081981,0.0536894985));
    r0.xyz = r0.xyz + -r0.www;
    r0.xyz = r0.xyz * float3(0.959999979,0.959999979,0.959999979) + r0.www;

  } else { 
    // WARNING UNTESTED  pmnox
    // TODO adjust parameters if scene is encountered
    // ============================================================================
    // SECTION 12B: RENODX ACES HDR CONFIGURATION
    // ============================================================================
    // Create ACES configuration for HDR upgrade using RenoDX's ApplyACES function
    
    // Initialize ACES configuration with HDR-optimized parameters
    renodx::tonemap::Config config = renodx::tonemap::config::Create();
    
    // Set ACES tone mapping type (2 = ACES)
    config.type = renodx::tonemap::config::type::ACES;
   
    // HDR Display Parameters
    config.peak_nits = 10000.f;        // Target peak brightness for HDR displays (10,000 nits)
    config.game_nits = 203.f;         // Game's native peak brightness (1,000 nits typical for HDR games)
    
    // Gamma Correction (0 = none, 1 = 2.2, 2 = 2.4)
    config.gamma_correction = 0.f;     // No gamma correction for HDR output
    
    // Color Grading Parameters
    config.exposure = 1.0f;            // Exposure multiplier (1.0 = no change)
    config.highlights = 1.0f;          // Highlight compression (1.0 = no compression)
    config.shadows = 1.0f;             // Shadow lift (1.0 = no lift)
    config.contrast = 1.0f;            // Contrast adjustment (1.0 = no change)
    config.saturation = 1.0f;          // Saturation multiplier (1.0 = no change)
    
    // Mid-gray calibration for ACES
    config.mid_gray_value = 0.18f;     // Standard 18% gray reference
    config.mid_gray_nits = 18.f;       // Mid-gray in nits (18 nits = 18% of 100 nits)
    
    // RenoDRT Parameters (not used for ACES but required by config)
    config.reno_drt_highlights = 1.0f;
    config.reno_drt_shadows = 1.0f;
    config.reno_drt_contrast = 1.0f;
    config.reno_drt_saturation = 1.0f;
    config.reno_drt_dechroma = 0.5f;
    config.reno_drt_flare = 0.0f;
    
    // Hue Correction Parameters
    config.hue_correction_type = renodx::tonemap::config::hue_correction_type::INPUT;
    config.hue_correction_strength = 1.0f;
    config.hue_correction_color = float3(0.0f, 0.0f, 0.0f);
    
    // RenoDRT Method Parameters
    config.reno_drt_hue_correction_method = 0u;  // OKLAB
    config.reno_drt_tone_map_method = 0u;        // DANIELE
    config.reno_drt_working_color_space = 0u;    // BT709
    config.reno_drt_per_channel = false;
    config.reno_drt_blowout = 0.0f;
    config.reno_drt_clamp_color_space = 2.0f;    // BT2020
    config.reno_drt_clamp_peak = 1.0f;
    config.reno_drt_white_clip = 100.0f;
    
    // Apply ACES tone mapping with HDR configuration
    r0.xyz = unity_to_ACEScg(renodx::tonemap::config::ApplyACES(ACEScg_to_unity(r0.xyz), config));
   //  r2.xyz = r0.xyz;
  }
  
  // ============================================================================
  // SECTION 14: FINAL COLOR TRANSFORMATION AND GAMMA CORRECTION
  // ============================================================================
  // Apply final color transformations and gamma correction
  r1.xyz = r0.xyz * float3(278.508514,278.508514,278.508514) + float3(10.7771997,10.7771997,10.7771997);
  r1.xyz = r1.xyz * r0.xyz;
  r2.xyz = r0.xyz * float3(293.604492,293.604492,293.604492) + float3(88.7121964,88.7121964,88.7121964);
  r0.xyz = r0.xyz * r2.xyz + float3(80.6889038,80.6889038,80.6889038);
  r0.xyz = r1.xyz / r0.xyz;
  r1.z = dot(float3(-0.00557464967,0.0040607336,1.01033914), r0.xyz);
  r1.x = dot(float3(0.662454188,0.134004205,0.156187683), r0.xyz);
  r1.y = dot(float3(0.272228718,0.674081743,0.0536895171), r0.xyz);
  r0.x = dot(r1.xyz, float3(1,1,1));
  r0.x = max(9.99999975e-05, r0.x);
  r0.xy = r1.xy / r0.xx;
  r0.w = max(0, r1.y);
  r0.w = min(65504, r0.w);
  r0.w = log2(r0.w);
  r0.w = 0.981100023 * r0.w;
  r1.y = exp2(r0.w);
  r0.w = 1 + -r0.x;
  r0.z = r0.w + -r0.y;
  r0.y = max(9.99999975e-05, r0.y);
  r0.y = r1.y / r0.y;
  r1.xz = r0.xz * r0.yy;
  
  // ============================================================================
  // SECTION 15: FINAL COLOR SPACE CONVERSION TO DISPLAY RGB
  // ============================================================================
  // Convert to final display RGB color space
  r0.x = dot(float3(1.6410234,-0.324803293,-0.236424699), r1.xyz);
  r0.y = dot(float3(-0.663662851,1.61533165,0.0167563483), r1.xyz);
  r0.z = dot(float3(0.0117218941,-0.00828444213,0.988394856), r1.xyz);
  r0.w = dot(r0.xyz, float3(0.272228986,0.674081981,0.0536894985));
  r0.xyz = r0.xyz + -r0.www;
  r0.xyz = r0.xyz * float3(0.930000007,0.930000007,0.930000007) + r0.www;
  r1.x = dot(float3(0.662454188,0.134004205,0.156187683), r0.xyz);
  r1.y = dot(float3(0.272228718,0.674081743,0.0536895171), r0.xyz);
  r1.z = dot(float3(-0.00557464967,0.0040607336,1.01033914), r0.xyz);
  r0.x = dot(float3(0.987223983,-0.00611326983,0.0159533005), r1.xyz);
  r0.y = dot(float3(-0.00759836007,1.00186002,0.00533019984), r1.xyz);
  r0.z = dot(float3(0.00307257008,-0.00509594986,1.08168006), r1.xyz);
  
  // ============================================================================
  // SECTION 16: FINAL RGB TO sRGB CONVERSION
  // ============================================================================
  // Convert to sRGB color space with conditional saturation
  if (RENODX_TONE_MAP_TYPE == 0.f) {
    r1.x = saturate(dot(float3(3.2409699,-1.5373832,-0.498610765), r0.xyz));
    r1.y = saturate(dot(float3(-0.969243646,1.8759675,0.0415550582), r0.xyz));
    r1.z = saturate(dot(float3(0.0556300804,-0.203976959,1.05697155), r0.xyz));
  } else {
    r1.x = (dot(float3(3.2409699,-1.5373832,-0.498610765), r0.xyz));
    r1.y = (dot(float3(-0.969243646,1.8759675,0.0415550582), r0.xyz));
    r1.z = (dot(float3(0.0556300804,-0.203976959,1.05697155), r0.xyz));
  }

  // ============================================================================
  // SECTION 17: FINAL LUT SAMPLING AND OUTPUT
  // ============================================================================
  // Apply final LUT sampling for color grading and output
  if (RENODX_TONE_MAP_TYPE == 0.f) {
    r0.xyz = float3(0.00390625,0.00390625,0.00390625) + r1.xyz;
    r0.w = 0.75;
    r1.xyzw = t0.Sample(s0_s, r0.xw).wxyz;
    r1.x = saturate(r1.x);
    r2.xyzw = t0.Sample(s0_s, r0.yw).xyzw;
    r0.xyzw = t0.Sample(s0_s, r0.zw).xyzw;
    r1.z = saturate(r0.w);
    r1.y = saturate(r2.w);
    r0.xyz = float3(0.00390625,0.00390625,0.00390625) + r1.xyz;
    r0.w = 0.75;
    r1.xyzw = t0.Sample(s0_s, r0.xw).xyzw;
    o0.x = saturate(r1.x);
    r1.xyzw = t0.Sample(s0_s, r0.yw).xyzw;
    r0.xyzw = t0.Sample(s0_s, r0.zw).xyzw;
    o0.z = saturate(r0.z);
    o0.y = saturate(r1.y);
  } else {
    o0.xyz = r1.xyz;
  }
  
  // ============================================================================
  // SECTION 18: RENODX TONE MAPPING PASS
  // ============================================================================
  // Apply RenoDX tone mapping pass for HDR content
  if (RENODX_TONE_MAP_TYPE > 0.f) {
    //o0.xyz = renodx::tonemap::UpgradeToneMap(untonemapped.rgb, neutral_sdr, o0.rgb, 1.f);
    // o0.xyz = renodx::draw::ToneMapPass(untonemapped.rgb, o0.xyz);
    o0.xyz = renodx::draw::ToneMapPass(o0.xyz);
  }
  

  // ============================================================================
  // SECTION 19: FINAL OUTPUT
  // ============================================================================
  // Set final alpha and return
  o0.w = 1;
  return;
}