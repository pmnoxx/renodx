// HLSL Library for ASM instruction implementations

// Lerp function with parameter reordering (ASM lrp_pp)
float __asm_lerp(float X, float A, float B) { return lerp(B, A, X);}
float2 __asm_lerp(float2 X, float2 A, float2 B) { return lerp(B, A, X);}
float3 __asm_lerp(float3 X, float3 A, float3 B) { return lerp(B, A, X);}
float4 __asm_lerp(float4 X, float4 A, float4 B) { return lerp(B, A, X);}

// Basic arithmetic operations (most of these are just aliases for standard HLSL)
float __asm_mul_pp(float A, float B) { return A * B; }
float2 __asm_mul_pp(float2 A, float2 B) { return A * B; }
float3 __asm_mul_pp(float3 A, float3 B) { return A * B; }
float4 __asm_mul_pp(float4 A, float4 B) { return A * B; }

float __asm_mul(float A, float B) { return A * B; }
float2 __asm_mul(float2 A, float2 B) { return A * B; }
float3 __asm_mul(float3 A, float3 B) { return A * B; }
float4 __asm_mul(float4 A, float4 B) { return A * B; }

float __asm_add_pp(float A, float B) { return A + B; }
float2 __asm_add_pp(float2 A, float2 B) { return A + B; }
float3 __asm_add_pp(float3 A, float3 B) { return A + B; }
float4 __asm_add_pp(float4 A, float4 B) { return A + B; }

float __asm_add_sat_pp(float A, float B) { return saturate(A + B); }
float2 __asm_add_sat_pp(float2 A, float2 B) { return saturate(A + B); }
float3 __asm_add_sat_pp(float3 A, float3 B) { return saturate(A + B); }
float4 __asm_add_sat_pp(float4 A, float4 B) { return saturate(A + B); }

float __asm_mov_pp(float A) { return A; }
float2 __asm_mov_pp(float2 A) { return A; }
float3 __asm_mov_pp(float3 A) { return A; }
float4 __asm_mov_pp(float4 A) { return A; }

float __asm_max_pp(float A, float B) { return max(A, B); }
float2 __asm_max_pp(float2 A, float2 B) { return max(A, B); }
float3 __asm_max_pp(float3 A, float3 B) { return max(A, B); }
float4 __asm_max_pp(float4 A, float4 B) { return max(A, B); }

float __asm_min_pp(float A, float B) { return min(A, B); }
float2 __asm_min_pp(float2 A, float2 B) { return min(A, B); }
float3 __asm_min_pp(float3 A, float3 B) { return min(A, B); }
float4 __asm_min_pp(float4 A, float4 B) { return min(A, B); }

// Mathematical functions
float __asm_rsq_pp(float A) { return rsqrt(A); }
float2 __asm_rsq_pp(float2 A) { return rsqrt(A); }
float3 __asm_rsq_pp(float3 A) { return rsqrt(A); }
float4 __asm_rsq_pp(float4 A) { return rsqrt(A); }

float __asm_rcp_pp(float A) { return rcp(A); }
float2 __asm_rcp_pp(float2 A) { return rcp(A); }
float3 __asm_rcp_pp(float3 A) { return rcp(A); }
float4 __asm_rcp_pp(float4 A) { return rcp(A); }

float __asm_log_pp(float A) { return log(A); }
float2 __asm_log_pp(float2 A) { return log(A); }
float3 __asm_log_pp(float3 A) { return log(A); }
float4 __asm_log_pp(float4 A) { return log(A); }

float __asm_exp_pp(float A) { return exp(A); }
float2 __asm_exp_pp(float2 A) { return exp(A); }
float3 __asm_exp_pp(float3 A) { return exp(A); }
float4 __asm_exp_pp(float4 A) { return exp(A); }

// Multiply-add operations
float __asm_mad(float A, float B, float C) { return A * B + C; }
float2 __asm_mad(float2 A, float2 B, float2 C) { return A * B + C; }
float3 __asm_mad(float3 A, float3 B, float3 C) { return A * B + C; }
float4 __asm_mad(float4 A, float4 B, float4 C) { return A * B + C; }

float __asm_mad_sat_pp(float A, float B, float C) { return saturate(A * B + C); }
float2 __asm_mad_sat_pp(float2 A, float2 B, float2 C) { return saturate(A * B + C); }
float3 __asm_mad_sat_pp(float3 A, float3 B, float3 C) { return saturate(A * B + C); }
float4 __asm_mad_sat_pp(float4 A, float4 B, float4 C) { return saturate(A * B + C); }

// Saturate multiply operations
float __asm_mul_sat(float A, float B) { return saturate(A * B); }
float2 __asm_mul_sat(float2 A, float2 B) { return saturate(A * B); }
float3 __asm_mul_sat(float3 A, float3 B) { return saturate(A * B); }
float4 __asm_mul_sat(float4 A, float4 B) { return saturate(A * B); }

float __asm_mul_sat_pp(float A, float B) { return saturate(A * B); }
float2 __asm_mul_sat_pp(float2 A, float2 B) { return saturate(A * B); }
float3 __asm_mul_sat_pp(float3 A, float3 B) { return saturate(A * B); }
float4 __asm_mul_sat_pp(float4 A, float4 B) { return saturate(A * B); }

// Dot product operations (dp3 operates on first 3 components)
float __asm_dp3(float3 A, float3 B) { return dot(A, B); }
float __asm_dp3(float4 A, float4 B) { return dot(A.xyz, B.xyz); }
float __asm_dp3_pp(float3 A, float3 B) { return dot(A, B); }
float __asm_dp3_pp(float4 A, float4 B) { return dot(A.xyz, B.xyz); }
float __asm_dp3_sat_pp(float3 A, float3 B) { return saturate(dot(A, B)); }
float __asm_dp3_sat_pp(float4 A, float4 B) { return saturate(dot(A.xyz, B.xyz)); }

// dp2add operation (dot product of first 2 components + third component)
float __asm_dp2add_pp(float2 A, float2 B, float C) { return dot(A, B) + C; }
