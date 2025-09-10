#include "../../custom.hlsl"

// Generated HLSL code from ASM instructions
//
// #pragma target ps_3_0

// Include helper functions
#include "asm_library.hlsl"

// Constant registers
float4 GlowColorMult: register(c26);
float4 VignetteAndNoiseParams: register(c27);
float4 LocalContrastParams: register(c28);
float4 LookupExcludeColor0: register(c29);
float4 LookupExcludeColor1: register(c30);
float4 LookupExcludeFalloff: register(c31);
float4 SharpenParams: register(c32);
float4 NonGlowColorMult: register(c35);
float4 CustomParams0: register(c36);
float4 CustomParams1: register(c37);
float4 CustomParams2: register(c38);
float4 CustomParams3: register(c39);

// Sampler registers
sampler2D InputTexture: register(s0);
sampler2D GlowTexture: register(s1);
sampler2D ColorLookup: register(s3);
sampler2D ToneMappingLookup: register(s4);
sampler2D CloudTexture: register(s5);
sampler2D MaskTexture: register(s6);

float4 main(
    float4 v0: TEXCOORD,
    float2 v1: TEXCOORD1,
    float3 v2: TEXCOORD2,
    float2 v3: TEXCOORD3,
    float2 v4: TEXCOORD4
) : COLOR
{
    float4 oC0;  // Output color register

    // Constant definitions from def instructions
    float4 c0 = float4(0.25, 0.5, 1, 0);
    float4 c1 = float4(0.212599993, 0.715200007, 0.0722000003, 0);

    // Temporary float registers
    float4 r0 = 0, r1 = 0, r2 = 0, r3 = 0;

    //  0 0x00000340: dcl_texcoord v0
    //  0 dcl_texcoord1 v1.xy
    //  0 dcl_texcoord2_pp v2.xyz
    //  0 dcl_texcoord3 v3.xy
    //  0 dcl_texcoord4 v4.xy
    //  0 dcl_2d s0
    //  0 dcl_2d s1
    //  0 dcl_2d s3
    //  0 dcl_2d s4
    //  0 dcl_2d s5
    //  0 dcl_2d s6
    // instr 58: add r0.xy, c32, v0
    r0.xy = (SharpenParams + v0).xy;

    // instr 59: texld_pp r0, r0, s0
    // sig: float4 = tex2D(sampler2D, float2)
    r0 = tex2D(InputTexture, r0.xy);

    // instr 60: texld_pp r1, v0, s0
    // sig: float4 = tex2D(sampler2D, float2)
    r1 = tex2D(InputTexture, v0.xy);

    // instr 61: lrp_pp r2.xyz, c32.z, r0, r1
    // sig: float4 = __asm_lerp(float1, float4, float4)
    r2.xyz = (__asm_lerp(SharpenParams.z, r0, r1)).xyz;

    // instr 62: mov_pp oC0.w, r1.w
    oC0.w = r1.w;

    // instr 63: mul_pp r0.xyz, r2, c32.w
    r0.xyz = (r2 * SharpenParams.w).xyz;

    // instr 64: mul_pp r0.xyz, r0, c0.x
    r0.xyz = (r0 * c0.x).xyz;


    float3 untonemapped_color = r0.xyz;

    // instr 65: texld_pp r1, r0.x, s4
    // sig: float4 = tex2D(sampler2D, float2)
    r1 = tex2D(ToneMappingLookup, r0.x);

    // instr 66: texld_pp r2, r0.y, s4
    // sig: float4 = tex2D(sampler2D, float2)
    r2 = tex2D(ToneMappingLookup, r0.y);

    // instr 67: texld_pp r0, r0.z, s4
    // sig: float4 = tex2D(sampler2D, float2)
    r0 = tex2D(ToneMappingLookup, r0.z);


    // instr 68: mov_pp r1.z, r0.z
    r1.z = r0.z;

    // instr 69: mov_pp r1.y, r2.y
    r1.y = r2.y;

    if (RENODX_TONE_MAP_TYPE > 0.f) {
        r1.xyz = lerp(r1.xyz, r1.xyz * untonemapped_color, (untonemapped_color > 1.f ? 1.f : 0.f));
    }

    // instr 70: texld_pp r0, v0.zwzw, s1
    // sig: float4 = tex2D(sampler2D, float2)
    r0 = tex2D(GlowTexture, v0.zw);

    // instr 71: max_pp r1.w, r0.w, c28.y
    r1.w = max(r0.w, LocalContrastParams.y);

    // instr 72: mul_pp r0.xyz, r0, c26
    r0.xyz = (r0 * GlowColorMult).xyz;

    // instr 73: mul r0.xyz, r0, c0.y
    r0.xyz = (r0 * c0.y).xyz;

    // instr 74: min_pp r0.w, c28.z, r1.w
    r0.w = min(LocalContrastParams.z, r1.w);

    // instr 75: lrp_pp r2.xyz, c28.x, r1, r0.w
    // sig: float4 = __asm_lerp(float1, float4, float4)
    r2.xyz = (__asm_lerp(LocalContrastParams.x, r1, r0.w)).xyz;

    // instr 76: mad_sat_pp r0.xyz, r2, c35, r0
    // sig: float1 = mad_sat(float1, float1, float1)
    //r0.xyz = (saturate(r2 * NonGlowColorMult + r0)).xyz;
    r0.xyz = ((r2 * NonGlowColorMult + r0)).xyz;
    if (RENODX_TONE_MAP_TYPE == 0.f) {
        r0.xyz = saturate(r0.xyz);
    }

    // instr 77: rsq_pp r0.w, r0.x
    r0.w = rsqrt(r0.x);

    // instr 78: rcp_pp r1.x, r0.w
    r1.x = rcp(r0.w);

    // instr 79: rsq_pp r0.w, r0.y
    r0.w = rsqrt(r0.y);

    // instr 80: rcp_pp r1.y, r0.w
    r1.y = rcp(r0.w);

    // instr 81: rsq_pp r0.w, r0.z
    r0.w = rsqrt(r0.z);

    // instr 82: rcp_pp r1.z, r0.w
    r1.z = rcp(r0.w);

    // instr 83: add_pp r2.xyz, -r1, c29
    r2.xyz = (-r1 + LookupExcludeColor0).xyz;
    // WARNING: add_pp expects 2 operand(s) but got 3

    // instr 84: dp3_pp r2.x, r2, r2
    r2.x = (__asm_dp3_pp(r2, r2));

    // instr 85: add_pp r3.xyz, -r1, c30
    r3.xyz = (-r1 + LookupExcludeColor1).xyz;
    // WARNING: add_pp expects 2 operand(s) but got 3

    // instr 86: dp3_sat_pp r0.w, r1, c1
    // sig: float1 = dp3_sat(float4, float4)
    r0.w = (__asm_dp3_sat_pp(r1, c1));

    // instr 87: texld_pp r1, r0.w, s3
    // sig: float4 = tex2D(sampler2D, float2)
    r1 = tex2D(ColorLookup, r0.w);

    // instr 88: dp3_pp r2.y, r3, r3
    r2.y = (__asm_dp3_pp(r3, r3));

    // instr 89: mad_sat_pp r2.xy, r2, c31, c31.zwzw
    // sig: float1 = mad_sat(float1, float1, float1)
    r2.xy = (saturate(r2 * LookupExcludeFalloff + LookupExcludeFalloff.zwzw)).xy;

    // instr 90: mul_pp r0.w, r1.w, r2.x
    r0.w = (r1.w * r2.x);

    // instr 91: mul_sat_pp r0.w, r2.y, r0.w
    // sig: float1 = mul_sat(float1, float1)
    r0.w = saturate(r2.y * r0.w);

    // instr 92: lrp_pp r2.xyz, r0.w, r1, r0
    // sig: float4 = __asm_lerp(float1, float4, float4)
    r2.xyz = (__asm_lerp(r0.w, r1, r0)).xyz;

    // instr 93: dp3_pp r0.x, r2, c1
    r0.x = (__asm_dp3_pp(r2, c1));

    // instr 94: lrp_pp r1.xyz, c27.z, r2, r0.x
    // sig: float4 = __asm_lerp(float1, float4, float4)
    r1.xyz = (__asm_lerp(VignetteAndNoiseParams.z, r2, r0.x)).xyz;

    // instr 95: add_pp r0.xyz, r1, r1
    r0.xyz = (r1 + r1).xyz;

    // instr 96: log_pp r1.x, r0.x
    r1.x = log(r0.x);

    // instr 97: log_pp r1.y, r0.y
    r1.y = log(r0.y);

    // instr 98: log_pp r1.z, r0.z
    r1.z = log(r0.z);

    // instr 99: mul_pp r0.xyz, r1, c27.w
    r0.xyz = (r1 * VignetteAndNoiseParams.w).xyz;

    // instr 100: exp_pp r1.x, r0.x
    r1.x = exp(r0.x);

    // instr 101: exp_pp r1.y, r0.y
    r1.y = exp(r0.y);

    // instr 102: exp_pp r1.z, r0.z
    r1.z = exp(r0.z);

    // instr 103: mul_sat_pp r0.xyz, r1, c0.y
    // sig: float1 = mul_sat(float1, float1)
    //r0.xyz = (saturate(r1 * c0.y)).xyz;
    r0.xyz = ((r1 * c0.y)).xyz;
    if (RENODX_TONE_MAP_TYPE == 0.f) {
        r0.xyz = saturate(r0.xyz);
    }

    // instr 104: dp3_pp r0.w, r0, c1
    r0.w = (__asm_dp3_pp(r0, c1));

    // instr 105: add r1.x, r0.w, -c37.x
    r1.x = (r0.w + -CustomParams1.x);
    // WARNING: add expects 2 operand(s) but got 3

    // instr 106: add r1.y, -r0.w, c37.z
    r1.y = (-r0.w + CustomParams1.z);
    // WARNING: add expects 2 operand(s) but got 3

    // instr 107: add r0.w, -r0.w, c36.x
    r0.w = (-r0.w + CustomParams0.x);
    // WARNING: add expects 2 operand(s) but got 3

    // instr 108: mul_sat_pp r0.w, r0.w, c36.z
    // sig: float1 = mul_sat(float1, float1)
    r0.w = saturate(r0.w * CustomParams0.z);

    // instr 109: mul_sat_pp r1.xy, r1, c37.ywzw
    // sig: float1 = mul_sat(float1, float1)
    r1.xy = (saturate(r1 * CustomParams1.ywzw)).xy;

    // instr 110: mul_pp r1.x, r1.y, r1.x
    r1.x = (r1.y * r1.x);

    // instr 111: mul_sat r1.x, r1.x, c36.y
    // sig: float1 = mul_sat(float1, float1)
    r1.x = saturate(r1.x * CustomParams0.y);

    // instr 112: texld_pp r2, v0, s6
    // sig: float4 = tex2D(sampler2D, float2)
    r2 = tex2D(MaskTexture, v0.xy);

    // instr 113: add_pp r1.y, -r2.x, c0.z
    r1.y = (-r2.x + c0.z);
    // WARNING: add_pp expects 2 operand(s) but got 3

    // instr 114: mul r1.x, r1.y, r1.x
    r1.x = (r1.y * r1.x);

    // instr 115: texld_pp r2, v3, s5
    // sig: float4 = tex2D(sampler2D, float2)
    r2 = tex2D(CloudTexture, v3.xy);

    // instr 116: texld_pp r3, v4, s5
    // sig: float4 = tex2D(sampler2D, float2)
    r3 = tex2D(CloudTexture, v4.xy);

    // instr 117: lrp_pp r1.yz, c38.x, r2.xxww, r3.xxww
    // sig: float4 = __asm_lerp(float1, float4, float4)
    r1.yz = (__asm_lerp(CustomParams2.x, r2.xxww, r3.xxww)).yz;

    // instr 118: lrp_pp r2.x, r0.w, r1.z, r1.y
    // sig: float4 = __asm_lerp(float1, float4, float4)
    r2.x = (__asm_lerp(r0.w, r1.z, r1.y));

    // instr 119: lrp_pp r3.xyz, r1.x, r2.x, r0
    // sig: float4 = __asm_lerp(float1, float4, float4)
    r3.xyz = (__asm_lerp(r1.x, r2.x, r0)).xyz;

    // instr 120: mul_pp r0.xyz, r3, v2
    r0.xyz = (r3 * v2).xyz;

    // instr 121: dp2add_pp r0.w, v1, v1, c39.x
    // sig: float1 = dp2add(float2, float2, float1)
    r0.w = dot(v1, v1) + CustomParams3.x;

    // instr 122: mad_sat_pp r0.w, r0.w, c27.x, c27.y
    // sig: float1 = mad_sat(float1, float1, float1)
    r0.w = saturate(r0.w * VignetteAndNoiseParams.x + VignetteAndNoiseParams.y);

    // instr 123: mul_pp r0.w, r0.w, r0.w
    r0.w = (r0.w * r0.w);

    // approximately 68 instruction slots used (10 texture, 58 arithmetic)
    // instr 124: mul_pp oC0.xyz, r0.w, r0
    oC0.xyz = (r0.w * r0).xyz;

    return oC0;  // Return the final result
}