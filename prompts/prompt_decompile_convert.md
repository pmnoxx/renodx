# Decompiled Shader Code Conversion Instructions

## Overview
This document provides instructions for converting original decompiled shader code into clean, readable HLSL code using modern practices.

## Conversion Guidelines

### 1. Reference Code Preservation
- **If reference code is missing**: Add the original decompiled code in comment brackets `/* */` before the `main()` function
- **Purpose**: Preserve the original implementation for debugging, verification, and understanding
- **Format**: 
```hlsl
/*
// Original decompiled implementation for reference:
[original code here]
*/
```

### 2. Register Variable Replacement
Replace all register-based variables (r0, r1, r2, etc.) with descriptive `const` immutable variables:

#### Before (Decompiled):
```hlsl
r0.xy = -cb0[131].xy * float2(0.5,0.5) + cb0[28].xy;
r0.xy = min(v1.xy, r0.xy);
r0.xyzw = t0.SampleBias(s0_s, r0.xy, cb0[4].x).xyzw;
r0.xyz = renodx::color::srgb::DecodeSafe(r0.xyz);
```

#### After (Converted):
```hlsl
const float2 half_vec = float2(0.5, 0.5);
const float2 uv_offset = -cb0[131].xy * half_vec + cb0[28].xy;
const float2 sample_uv = min(v1.xy, uv_offset);
const float4 sampled_texture = t0.SampleBias(s0_s, sample_uv, cb0[4].x);
const float3 color_v0 = renodx::color::srgb::DecodeSafe(sampled_texture.xyz);
```

### 3. Variable Naming Conventions

#### Data Flow Variables
Use descriptive names that show the pipeline flow:
- `color_v0`, `color_v1`, `color_v2`, etc. - For color data through pipeline stages
- `uv_offset`, `sample_uv` - For texture coordinate calculations
- `vignette_calc`, `lut_result` - For specific effect calculations

#### Const Variables
Use `const` for all intermediate calculations:
- `const float2 half_vec = float2(0.5, 0.5);`
- `const float vignette_enabled = cmp(0 < cb0[145].z);`
- `const float3 final_color = renodx::color::srgb::EncodeSafe(color_v6);`

#### Mutable Variables
Only use mutable variables when needed for conditional logic:
```hlsl
float3 color_v1 = color_v0;  // Mutable for conditional vignette
if (vignette_enabled != 0) {
    // ... vignette calculations ...
    color_v1 = vignette_result * color_v0;
}
```

### 4. Code Organization

#### Section Comments
Add clear section comments to explain processing stages:
```hlsl
// Sample input texture with bias
// Vignette effect
// Apply brightness multiplier
// First LUT application (t2)
// Tone mapping and color space conversion
// Second LUT application (t1)
// Output encoding
```

#### Mathematical Operations
Break down complex calculations into named const variables:
```hlsl
const float vignette_dist_sq = dot(vignette_calc.xz, vignette_calc.xz);
const float vignette_falloff = 1 + -vignette_dist_sq;
const float vignette_clamped = max(0, vignette_falloff);
const float vignette_log = log2(vignette_clamped);
const float vignette_strength = cb0[145].w;
const float vignette_factor = exp2(vignette_strength * vignette_log);
```

### 5. Function Calls
Replace manual implementations with library functions when available:
- Use `renodx::color::srgb::DecodeSafe()` instead of manual sRGB decoding
- Use `renodx::lut::SampleTetrahedral()` instead of manual LUT sampling
- Use `renodx::color::bt2020::from::BT709()` for color space conversions

### 6. Example Conversion

#### Original Decompiled:
```hlsl
r0.xy = -cb0[131].xy * float2(0.5,0.5) + cb0[28].xy;
r0.xy = min(v1.xy, r0.xy);
r0.xyzw = t0.SampleBias(s0_s, r0.xy, cb0[4].x).xyzw;
r0.xyz = renodx::color::srgb::DecodeSafe(r0.xyz);
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
```

#### Converted:
```hlsl
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
    color_v1 = vignette_result * color_v0;
}
```

## Benefits of This Approach

1. **Readability**: Code is self-documenting with descriptive variable names
2. **Maintainability**: Easy to understand and modify individual processing stages
3. **Debugging**: Clear data flow makes it easier to identify issues
4. **Performance**: Compiler can optimize const variables more effectively
5. **Reference**: Original code is preserved for comparison and verification

## Checklist for Conversion

- [ ] Add original decompiled code in comments before main() if missing
- [ ] Replace all register variables (r0, r1, etc.) with descriptive const names
- [ ] Use data flow variables (color_v0, color_v1, etc.) for pipeline stages
- [ ] Add section comments explaining each processing stage
- [ ] Break down complex calculations into named const variables
- [ ] Use library functions instead of manual implementations when available
- [ ] Ensure all mathematical operations are preserved exactly
- [ ] Test that converted code produces identical results 