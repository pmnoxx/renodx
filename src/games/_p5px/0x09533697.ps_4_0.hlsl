// ---- Created with 3Dmigoto v1.4.1 on Thu Jun 26 22:25:31 2025

//=============================================================================
// TEXTURE DECLARATIONS
//=============================================================================
Texture2D<float4> t6 : register(t6);  // Shadow map
Texture2D<float4> t5 : register(t5);  // Light cookie/mask
Texture2D<float4> t4 : register(t4);  // Normal map
Texture2D<float4> t3 : register(t3);  // Metallic/roughness map
Texture2D<float4> t2 : register(t2);  // Emission map
Texture2D<float4> t1 : register(t1);  // Albedo/diffuse map
TextureCube<float4> t0 : register(t0); // Environment reflection map

//=============================================================================
// SAMPLER DECLARATIONS
//=============================================================================
SamplerComparisonState s6_s : register(s6); // Shadow comparison sampler
SamplerState s5_s : register(s5);           // Light cookie sampler
SamplerState s4_s : register(s4);           // Normal map sampler
SamplerState s3_s : register(s3);           // Metallic/roughness sampler
SamplerState s2_s : register(s2);           // Emission sampler
SamplerState s1_s : register(s1);           // Albedo sampler
SamplerState s0_s : register(s0);           // Environment map sampler

//=============================================================================
// CONSTANT BUFFER DECLARATIONS
//=============================================================================
cbuffer cb4 : register(b4)  // Lighting and shadow data
{
  float4 cb4[33];
}

cbuffer cb3 : register(b3)  // Material and rendering parameters
{
  float4 cb3[20];
}

cbuffer cb2 : register(b2)  // Material properties and lighting
{
  float4 cb2[32];
}

cbuffer cb1 : register(b1)  // Material parameters
{
  float4 cb1[43];
}

cbuffer cb0 : register(b0)  // View and projection matrices, global parameters
{
  float4 cb0[156];
}

//=============================================================================
// 3DMIGOTO DECLARATIONS
//=============================================================================
#define cmp -

//=============================================================================
// MAIN FRAGMENT SHADER
//=============================================================================
void main(
  float4 v0 : TEXCOORD0,    // UV coordinates
  float4 v1 : TEXCOORD1,    // Tangent space vectors
  float4 v2 : TEXCOORD2,    // World position
  float4 v3 : TEXCOORD3,    // Normal vector
  float4 v4 : TEXCOORD5,    // Tangent vector
  float4 v5 : TEXCOORD6,    // Bitangent vector
  float4 v6 : COLOR0,       // Vertex color
  float4 v7 : SV_POSITION0, // Screen position
  out float4 o0 : SV_Target0, // Main color output
  out float4 o1 : SV_Target1)  // Additional output (normal, depth, etc.)
{
  //=============================================================================
  // CONSTANTS AND VARIABLE DECLARATIONS
  //=============================================================================
  const float4 icb[] = { { 1.000000, 0, 0, 0},
                              { 0, 1.000000, 0, 0},
                              { 0, 0, 1.000000, 0},
                              { 0, 0, 0, 1.000000} };
  float4 r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16;
  uint4 bitmask, uiDest;
  float4 fDest;

  //=============================================================================
  // DEPTH FADE AND DITHERING
  //=============================================================================
  r0.xy = -cb2[1].xz + cb0[15].xz;        // cb2[1].xz = Light position or camera offset
  r0.x = dot(r0.xy, r0.xy);
  r0.x = sqrt(r0.x);
  r0.x = -cb0[155].y + r0.x;
  r0.y = cb0[155].z + -cb0[155].y;
  r0.y = max(0.00999999978, r0.y);
  r0.x = r0.x / r0.y;
  r0.x = max(0, r0.x);
  r0.x = log2(r0.x);
  r0.x = cb0[155].w * r0.x;
  r0.x = exp2(r0.x);
  r0.y = 1 + -cb2[27].y;                  // cb2[27].y = Shadow fade distance
  r0.x = max(cb0[155].x, r0.x);
  r0.x = max(r0.y, r0.x);
  r0.x = min(cb2[27].x, r0.x);            // cb2[27].x = Shadow fade start distance
  
  // Dithering pattern calculation
  r0.yz = (uint2)v7.yx;
  r0.yz = (int2)r0.yz & int2(3,3);
  r1.x = dot(float4(0.0588235296,0.764705896,0.235294119,0.941176474), icb[r0.z+0].xyzw);
  r1.y = dot(float4(0.529411793,0.294117659,0.70588237,0.470588237), icb[r0.z+0].xyzw);
  r1.z = dot(float4(0.176470593,0.882352948,0.117647059,0.823529422), icb[r0.z+0].xyzw);
  r1.w = dot(float4(0.647058845,0.411764711,0.588235319,0.352941185), icb[r0.z+0].xyzw);
  r0.y = dot(r1.xyzw, icb[r0.y+0].xyzw);
  r0.x = r0.x + -r0.y;
  r0.x = cb0[154].w * -r0.x + r0.x;
  r0.x = cmp(r0.x < 0);
  if (r0.x != 0) discard;

  //=============================================================================
  // TEXTURE SAMPLING AND MATERIAL SETUP
  //=============================================================================
  r0.xyzw = t1.Sample(s1_s, v0.xy).xyzw;  // Sample albedo/diffuse texture
  r1.xyz = cb2[16].xyz * r0.xyz;          // cb2[16].xyz = Material color tint, cb2[16].w = Alpha cutoff
  r2.xyzw = t2.Sample(s2_s, v0.xy).xyzw;  // Sample emission texture
  r2.xyz = r2.www * r2.xyz;               // Apply emission alpha

  //=============================================================================
  // NORMAL VECTOR CALCULATION
  //=============================================================================
  r1.w = dot(v4.xyz, v4.xyz);             // Tangent vector normalization
  r1.w = max(1.17549435e-38, r1.w);
  r1.w = rsqrt(r1.w);
  r3.xyz = v4.xyz * r1.www;
  
  r1.w = dot(v3.xyz, v3.xyz);             // Normal vector normalization
  r1.w = rsqrt(r1.w);
  r4.xyz = v3.xyz * r1.www;

  //=============================================================================
  // MATERIAL PROPERTY EXTRACTION
  //=============================================================================
  r5.x = saturate(cb1[40].w);             // Metallic value
  r5.y = saturate(cb1[41].w);             // Roughness value
  r5.z = saturate(cb1[42].w);             // Additional material property

  //=============================================================================
  // LIGHTING CALCULATIONS
  //=============================================================================
  r1.w = -cb2[25].x * 0.959999979 + 0.959999979;  // cb2[25].x = Metallic workflow blend factor
  r2.w = 1 + -r1.w;
  r6.xyz = r1.xyz * r1.www;               // Diffuse lighting contribution

  //=============================================================================
  // METALLIC WORKFLOW CONVERSION
  //=============================================================================
  r0.xyz = r0.xyz * cb2[16].xyz + float3(-0.0399999991,-0.0399999991,-0.0399999991);
  r0.xyz = cb2[25].xxx * r0.xyz + float3(0.0399999991,0.0399999991,0.0399999991);

  //=============================================================================
  // ROUGHNESS AND FRESNEL CALCULATIONS
  //=============================================================================
  r3.w = 1 + -cb2[24].w;                  // cb2[24].w = Roughness value, cb2[24].y = Specular intensity
  r4.w = r3.w * r3.w;
  r4.w = max(0.0078125, r4.w);
  r5.w = r4.w * r4.w;
  r2.w = saturate(cb2[24].w + r2.w);
  r6.w = r4.w * 4 + 2;

  //=============================================================================
  // AMBIENT LIGHTING SETUP
  //=============================================================================
  r7.xyz = float3(-1,-1,-1) + cb0[6].xyz;
  r7.xyz = cb3[18].www * r7.xyz + float3(1,1,1);

  //=============================================================================
  // SHADOW MAPPING CALCULATIONS
  //=============================================================================
  r7.w = 10 / cb4[32].z;
  r7.w = sqrt(r7.w);
  r7.w = r7.w * r7.w + r7.w;
  r8.xyz = -cb0[15].xyz + v2.xyz;
  r8.x = dot(r8.xyz, r8.xyz);
  r7.w = cmp(r7.w >= r8.x);
  if (r7.w != 0) {
    // Shadow cascade selection and PCF filtering
    r8.xyz = -cb4[20].xyz + v2.xyz;
    r9.xyz = -cb4[21].xyz + v2.xyz;
    r10.xyz = -cb4[22].xyz + v2.xyz;
    r11.xyz = -cb4[23].xyz + v2.xyz;
    r8.x = dot(r8.xyz, r8.xyz);
    r8.y = dot(r9.xyz, r9.xyz);
    r8.z = dot(r10.xyz, r10.xyz);
    r8.w = dot(r11.xyz, r11.xyz);
    r8.xyzw = cmp(r8.xyzw < cb4[24].xyzw);
    r9.xyzw = r8.xyzw ? float4(1,1,1,1) : 0;
    r8.xyz = r8.xyz ? float3(-1,-1,-1) : float3(-0,-0,-0);
    r8.xyz = r9.yzw + r8.xyz;
    r9.yzw = max(float3(0,0,0), r8.xyz);
    r7.w = dot(r9.xyzw, float4(4,3,2,1));
    r7.w = 4 + -r7.w;
    r7.w = min(3, r7.w);
    r7.w = (uint)r7.w;
    r7.w = (uint)r7.w << 2;
    r8.xyz = cb4[r7.w+1].xyz * v2.yyy;
    r8.xyz = cb4[r7.w+0].xyz * v2.xxx + r8.xyz;
    r8.xyz = cb4[r7.w+2].xyz * v2.zzz + r8.xyz;
    r8.xyz = cb4[r7.w+3].xyz + r8.xyz;
    r9.xy = r8.xy * cb4[25].zw + float2(0.5,0.5);
    r9.xy = floor(r9.xy);
    r8.xy = r8.xy * cb4[25].zw + -r9.xy;
    r10.xyzw = float4(0.5,1,0.5,1) + r8.xxyy;
    r11.xyzw = r10.xxzz * r10.xxzz;
    r9.zw = float2(0.0799999982,0.0799999982) * r11.yw;
    r10.xz = r11.xz * float2(0.5,0.5) + -r8.xy;
    r11.xy = float2(1,1) + -r8.xy;
    r11.zw = min(float2(0,0), r8.xy);
    r11.zw = -r11.zw * r11.zw + r11.xy;
    r8.xy = max(float2(0,0), r8.xy);
    r8.xy = -r8.xy * r8.xy + r10.yw;
    r11.zw = float2(1,1) + r11.zw;
    r8.xy = float2(1,1) + r8.xy;
    r12.xy = float2(0.159999996,0.159999996) * r10.xz;
    r13.xy = float2(0.159999996,0.159999996) * r11.xy;
    r11.xy = float2(0.159999996,0.159999996) * r11.zw;
    r14.xy = float2(0.159999996,0.159999996) * r8.xy;
    r8.xy = float2(0.159999996,0.159999996) * r10.yw;
    r12.z = r11.x;
    r12.w = r8.x;
    r13.z = r14.x;
    r13.w = r9.z;
    r10.xyzw = r13.zwxz + r12.zwxz;
    r11.z = r12.y;
    r11.w = r8.y;
    r14.z = r13.y;
    r14.w = r9.w;
    r8.xyw = r14.zyw + r11.zyw;
    r11.xyz = r13.xzw / r10.zwy;
    r11.xyz = float3(-2.5,-0.5,1.5) + r11.xyz;
    r12.xyz = r14.zyw / r8.xyw;
    r12.xyz = float3(-2.5,-0.5,1.5) + r12.xyz;
    r11.xyz = cb4[25].xxx * r11.yxz;
    r12.xyz = cb4[25].yyy * r12.xyz;
    r11.w = r12.x;
    r13.xyzw = r9.xyxy * cb4[25].xyxy + r11.ywxw;
    r9.zw = r9.xy * cb4[25].xy + r11.zw;
    r12.w = r11.y;
    r11.yw = r12.yz;
    r14.xyzw = r9.xyxy * cb4[25].xyxy + r11.xyzy;
    r12.xyzw = r9.xyxy * cb4[25].xyxy + r12.wywz;
    r11.xyzw = r9.xyxy * cb4[25].xyxy + r11.xwzw;
    r15.xyzw = r10.zwyz * r8.xxxy;
    r16.xyzw = r10.xyzw * r8.yyww;
    r7.w = r10.y * r8.w;
    r8.x = cb2[13].z + r8.z;              // cb2[13].z = Shadow bias
    r8.y = t6.SampleCmpLevelZero(s6_s, r13.xy, r8.x).x;
    r8.w = t6.SampleCmpLevelZero(s6_s, r13.zw, r8.x).x;
    r8.w = r15.y * r8.w;
    r8.y = r15.x * r8.y + r8.w;
    r8.w = t6.SampleCmpLevelZero(s6_s, r9.zw, r8.x).x;
    r8.y = r15.z * r8.w + r8.y;
    r8.w = t6.SampleCmpLevelZero(s6_s, r12.xy, r8.x).x;
    r8.y = r15.w * r8.w + r8.y;
    r8.w = t6.SampleCmpLevelZero(s6_s, r14.xy, r8.x).x;
    r8.y = r16.x * r8.w + r8.y;
    r8.w = t6.SampleCmpLevelZero(s6_s, r14.zw, r8.x).x;
    r8.y = r16.y * r8.w + r8.y;
    r8.w = t6.SampleCmpLevelZero(s6_s, r12.zw, r8.x).x;
    r8.y = r16.z * r8.w + r8.y;
    r8.w = t6.SampleCmpLevelZero(s6_s, r11.xy, r8.x).x;
    r8.y = r16.w * r8.w + r8.y;
    r8.x = t6.SampleCmpLevelZero(s6_s, r11.zw, r8.x).x;
    r7.w = r7.w * r8.x + r8.y;
    r8.x = 1 + -cb4[32].x;
    r7.w = r7.w * cb4[32].x + r8.x;
    r8.x = cmp(0 >= r8.z);
    r8.y = cmp(r8.z >= 1);
    r8.x = (int)r8.y | (int)r8.x;
    r7.w = min(1, r7.w);
    r7.w = r8.x ? 1 : r7.w;
  } else {
    r7.w = 1;
  }

  //=============================================================================
  // LIGHT DIRECTION AND INTENSITY CALCULATIONS
  //=============================================================================
  r8.xyz = cb3[2].xyz * cb0[5].yyy;
  r8.xyz = cb3[1].xyz * cb0[5].xxx + r8.xyz;
  r8.xyz = cb3[3].xyz * cb0[5].zzz + r8.xyz;
  r8.w = dot(r8.xyz, r8.xyz);
  r9.x = rsqrt(r8.w);
  r9.yzw = r9.xxx * r8.xyz;
  r7.w = -1 + r7.w;
  r7.w = cb3[18].z * r7.w + 1;

  //=============================================================================
  // DIFFUSE LIGHTING CALCULATION
  //=============================================================================
  r10.x = dot(r3.xyz, r9.yzw);
  r10.y = cb2[11].z * abs(r10.x);         // cb2[11].z = Light attenuation factor
  r10.z = cmp(0 < r10.x);
  r10.w = cmp(r10.x < 0);
  r10.z = (int)-r10.z + (int)r10.w;
  r10.z = (int)r10.z;
  r10.z = saturate(r10.z);
  r10.x = abs(r10.x) * cb2[11].y + -r10.y; // cb2[11].y = Light falloff factor
  r10.x = r10.z * r10.x + r10.y;
  r10.x = cb2[11].x + r10.x;              // cb2[11].x = Ambient light intensity
  r10.y = dot(v3.xyz, r9.yzw);

  //=============================================================================
  // SPECULAR LIGHTING CALCULATION
  //=============================================================================
  r11.xyz = cb2[2].yyy * cb1[17].xyz;     // cb2[2].xyz = Light direction, cb2[2].w = Light intensity
  r11.xyz = cb1[16].xyz * cb2[2].xxx + r11.xyz;
  r11.xyz = cb1[18].xyz * cb2[2].zzz + r11.xyz;
  r11.xyz = cb1[19].xyz + r11.xyz;
  r11.xyz = v2.xyz + -r11.xyz;
  r10.z = dot(r11.xyz, r11.xyz);
  r10.z = rsqrt(r10.z);
  r11.xyz = r11.xyz * r10.zzz;
  r9.y = dot(r11.xyz, r9.yzw);
  r9.y = r9.y + -r10.y;
  r9.y = cb2[28].x * r9.y + r10.y;        // cb2[28].x = Specular reflection strength

  //=============================================================================
  // NORMAL MAPPING AND SPECULAR CALCULATIONS
  //=============================================================================
  r11.xyzw = t4.Sample(s4_s, v0.xy).xyzw;
  r9.y = 1 + r9.y;
  r9.y = saturate(r9.y * 0.5 + -r10.x);
  r12.x = saturate(cb2[11].w * r9.y);     // cb2[11].w = Specular intensity multiplier
  r12.y = 1;
  r12.xyzw = t3.Sample(s3_s, r12.xy).xyzw;
  r9.yzw = r12.xyz * r11.xxx;

  //=============================================================================
  // REFLECTION VECTOR CALCULATION
  //=============================================================================
  r10.x = dot(-r3.xyz, r4.xyz);
  r10.x = r10.x + r10.x;
  r10.xzw = r4.xyz * -r10.xxx + -r3.xyz;
  r11.x = saturate(dot(r4.xyz, r3.xyz));
  r11.x = 1 + -r11.x;
  r11.x = r11.x * r11.x;
  r11.x = r11.x * r11.x;

  //=============================================================================
  // ENVIRONMENT REFLECTION SAMPLING
  //=============================================================================
  r11.z = dot(r5.xyz, float3(0.298999995,0.587000012,0.114));
  r5.xyz = -r11.zzz + r5.xyz;
  r5.xyz = cb3[19].zzz * r5.xyz + r11.zzz;
  r11.z = -r3.w * 0.699999988 + 1.70000005;
  r11.z = r11.z * r3.w;
  r11.z = 6 * r11.z;
  r12.xyzw = t0.SampleLevel(s0_s, r10.xzw, r11.z).xyzw;

  //=============================================================================
  // ENVIRONMENT REFLECTION PROCESSING
  //=============================================================================
  r10.x = -1 + r12.w;
  r10.x = cb1[30].w * r10.x + 1;
  r10.x = max(0, r10.x);
  r10.x = log2(r10.x);
  r10.x = cb1[30].y * r10.x;
  r10.x = exp2(r10.x);
  r10.x = cb1[30].x * r10.x;
  r13.xyz = r10.xxx * r12.xyz + -cb0[0].xyz;
  r13.xyz = cb2[25].xxx * r13.xyz + cb0[0].xyz;  // cb2[25].x = Environment reflection intensity
  r10.xzw = r10.xxx * r12.xyz + -r13.xyz;
  r10.xzw = cb2[27].zzz * r10.xzw + r13.xyz;     // cb2[27].z = Environment reflection blend factor

  //=============================================================================
  // FINAL LIGHTING COMBINATION
  //=============================================================================
  r11.z = cb2[30].z + -cb2[30].y;         // cb2[30].yz = Fresnel power range
  r11.z = cb2[25].x * r11.z + cb2[30].y;
  r11.w = 1 + -r11.z;
  r3.w = r3.w * r11.w + r11.z;
  r11.z = 1 + -cb3[11].w;
  r11.z = r7.w * r11.z + cb3[11].w;
  r3.w = r11.z * r3.w;
  r10.xzw = r10.xzw * r3.www;
  r5.xyz = r5.xyz * r6.xyz;
  r5.xyz = cb2[29].www * r5.xyz;          // cb2[29].w = Emission intensity
  r5.xyz = cb3[6].xyz * r5.xyz;
  r6.xyz = cb3[7].xyz * r10.xzw;
  r6.xyz = cb3[12].xyz * r6.xyz;

  //=============================================================================
  // FRESNEL AND SPECULAR CALCULATIONS
  //=============================================================================
  r10.xz = r4.ww * r4.ww + float2(-1,1);
  r3.w = 1 / r10.z;
  r12.xyz = r2.www + -r0.xyz;
  r11.xzw = r11.xxx * r12.xyz + r0.xyz;
  r11.xzw = r11.xzw * r3.www;
  r6.xyz = r11.xzw * r6.xyz;
  r5.xyz = r5.xyz * cb3[11].xyz + r6.xyz;

  //=============================================================================
  // EMISSION AND ADDITIONAL LIGHTING
  //=============================================================================
  r2.w = cb1[26].z * r7.w;
  r3.w = saturate(r2.w);
  r6.xyz = r2.www * r9.yzw;
  r4.w = max(1.17549435e-38, r8.w);
  r4.w = rsqrt(r4.w);
  r9.yzw = r8.xyz * r4.www;
  r4.w = saturate(dot(r4.xyz, r9.yzw));
  r4.w = r4.w * r4.w;
  r4.w = r4.w * r10.x + 1.00399995;
  r4.w = r4.w * r4.w;
  r4.w = r4.w * r6.w;
  r4.w = r5.w / r4.w;
  r9.yzw = cb3[9].xyz * r4.www;
  r9.yzw = cb3[14].xyz * r9.yzw;
  r9.yzw = cb2[24].yyy * r9.yzw;          // cb2[24].y = Specular reflection intensity
  r0.xyz = r9.yzw * r0.xyz;

  //=============================================================================
  // AMBIENT OCCLUSION AND COLOR CORRECTION
  //=============================================================================
  r9.yzw = saturate(r7.xyz);
  r4.w = dot(r9.yzw, float3(0.298999995,0.587000012,0.114));
  r9.yzw = r9.yzw + -r4.www;
  r9.yzw = cb3[19].www * r9.yzw + r4.www;
  r9.yzw = cb3[15].xyz * r9.yzw;
  r10.xzw = cb2[23].xyz * r1.xyz;         // cb2[23].xyz = Ambient occlusion color, cb2[23].w = AO intensity
  r10.xzw = r10.xzw * cb3[17].xyz + -r1.xyz;
  r10.xzw = cb2[23].www * r10.xzw + r1.xyz;
  r11.xzw = r9.yzw * r6.xyz;
  r1.xyz = r1.xyz * r1.www + -r10.xzw;
  r1.xyz = r11.xzw * r1.xyz + r10.xzw;

  //=============================================================================
  // FINAL COLOR BLENDING AND OUTPUT
  //=============================================================================
  r10.xzw = float3(-1,-1,-1) + cb3[8].xyz;
  r10.xzw = cb3[19].xxx * r10.xzw + float3(1,1,1);
  r10.xzw = cb3[13].xyz * r10.xzw;
  r1.w = -r7.w * cb1[26].z + 1;
  r1.w = saturate(r1.w * 0.850000024 + r2.w);
  r1.xyz = r1.xyz * r10.xzw + -r5.xyz;
  r1.xyz = r1.www * r1.xyz + r5.xyz;
  r0.xyz = r0.xyz * r7.xyz;
  r0.xyz = r0.xyz * r3.www;
  r0.xyz = r1.xyz * float3(0.699999988,0.699999988,0.699999988) + r0.xyz;
  r0.xyz = r5.xyz + r0.xyz;


  //=============================================================================
  // LIGHT COOKIE/MASK PROCESSING
  //=============================================================================
  r1.xyzw = t5.Sample(s5_s, v0.xy).xyzw;
  r1.yzw = r8.xyz * r9.xxx + r3.xyz;
  r2.w = dot(r1.yzw, r1.yzw);
  r2.w = rsqrt(r2.w);
  r1.yzw = r2.www * r1.yzw;
  r1.y = saturate(dot(r4.xyz, r1.yzw));
  r1.y = r1.y * r1.y;
  r1.y = r1.y * r1.y;
  r1.x = r1.x * r1.y;
  r1.x = cmp(r1.x >= cb2[26].z);          // cb2[26].z = Light cookie threshold
  r1.x = r1.x ? 1.000000 : 0;
  r1.xyz = r9.yzw * r1.xxx;
  r4.xyz = cb2[21].xyz * r1.xyz;          // cb2[21].xyz = Light cookie color
  r4.xyz = cb2[26].www * r4.xyz;          // cb2[26].w = Light cookie intensity
  r1.xyz = r1.xyz * cb2[21].xyz + -r4.xyz;
  r1.xyz = r6.xyz * r1.xyz + r4.xyz;
  if (false) {
    r0.xyz = saturate(r1.xyz + r0.xyz); // brightness capping happens here
  } else {
    r0.xyz = (r1.xyz + r0.xyz); // brightness capping happens here
  }
  r0.xyz = r2.xyz * cb2[18].xyz + r0.xyz; // cb2[18].xyz = Emission color

  //=============================================================================
  // FOG AND ATMOSPHERIC EFFECTS
  //=============================================================================
  r1.x = dot(v3.xyz, r3.xyz);
  r1.y = saturate(1 + -r7.w);
  r1.y = cb1[26].z * r1.y;
  r7.w = saturate(r7.w);
  r1.zw = cb0[15].xz + -v2.xz;
  r1.z = dot(r1.zw, r1.zw);
  r1.z = sqrt(r1.z);
  r1.w = 1 + -cb2[29].y;                  // cb2[29].y = Fog start distance
  r1.z = -r1.z * 0.00999999978 + 1;
  r2.x = 1 + -r1.w;
  r1.w = r1.z + -r1.w;
  r2.x = 1 / r2.x;
  r1.w = saturate(r2.x * r1.w);
  r2.x = r1.w * -2 + 3;
  r1.w = r1.w * r1.w;
  r1.w = r2.x * r1.w;
  r2.x = -0.100000001 + cb2[29].x;        // cb2[29].x = Fog density
  r2.y = -0.100000001 + cb2[8].y;
  r2.x = r2.x + -r2.y;
  r1.w = r1.w * r2.x + r2.y;
  r1.xw = float2(1,1) + -r1.xw;
  r1.x = r1.x + -r1.w;
  r1.w = 1 + -abs(r10.y);
  r1.x = -r1.w * cb2[9].x + r1.x;         // cb2[9].x = Rim lighting strength
  r1.w = saturate(-r10.y);
  r1.x = -r1.w * cb2[8].z + r1.x;         // cb2[8].z = Rim lighting falloff
  r1.x = saturate(cb2[8].w * r1.x);       // cb2[8].w = Rim lighting intensity
  r2.xyz = cb2[19].xyz * r1.xxx;          // cb2[19].xyz = Rim lighting color
  r3.xyz = cb2[20].xyz * r1.xxx;          // cb2[20].xyz = Rim lighting secondary color
  r3.xyz = cb2[7].www * -r3.xyz + r3.xyz; // cb2[7].w = Rim lighting blend factor
  r1.w = cb2[9].y + -r10.y;               // cb2[9].y = Rim lighting angle
  r1.w = saturate(-0.5 + r1.w);
  r1.w = r1.w + r1.w;
  r1.w = min(1, r1.w);
  r2.w = r1.w * -2 + 3;
  r1.w = r1.w * r1.w;
  r1.w = r2.w * r1.w;
  r3.xyz = -r1.xxx * cb2[19].xyz + r3.xyz;
  r2.xyz = r1.www * r3.xyz + r2.xyz;
  r1.x = r1.x * r1.w;
  r3.xyz = r1.xxx * cb2[20].xyz + float3(-1,-1,-1);
  r3.xyz = r3.xyz * r1.xxx;
  r2.xyz = cb2[8].xxx * r2.xyz;           // cb2[8].x = Rim lighting power
  r2.xyz = r2.xyz * r7.xyz;
  r2.xyz = cb3[16].xyz * r2.xyz;
  r1.x = 1 + -cb2[28].z;                  // cb2[28].z = Fog blend factor
  r1.w = 1 + -r1.x;
  r1.x = r1.z + -r1.x;
  r1.z = 1 / r1.w;
  r1.x = saturate(r1.x * r1.z);
  r1.z = r1.x * -2 + 3;
  r1.x = r1.x * r1.x;
  r1.x = -r1.z * r1.x + 1;
  r1.y = cb2[9].z * r1.y;                 // cb2[9].z = Rim lighting intensity multiplier
  r1.y = r7.w * cb1[26].z + r1.y;
  r1.yzw = r2.xyz * r1.yyy;
  r1.yzw = r1.xxx * -r1.yzw + r1.yzw;
  r1.yzw = r1.yzw + r0.xyz;
  r2.xyz = cb2[7].www * r3.xyz + float3(1,1,1); // cb2[7].w = Rim lighting blend factor
  r3.xyz = float3(1,1,1) + -r2.xyz;
  r2.xyz = r1.xxx * r3.xyz + r2.xyz;
  r1.xyz = r1.yzw * r2.xyz + -r0.xyz;
  r1.xyz = cb2[2].www * r1.xyz;           // cb2[2].w = Final lighting blend factor
  r1.w = 1 + -r11.y;
  r1.w = cb2[29].z * r1.w + r11.y;        // cb2[29].z = Fog color blend factor
  r1.w = cb2[2].w * r1.w;                 // cb2[2].w = Final lighting blend factor
  r0.xyz = r1.www * r1.xyz + r0.xyz;

  //=============================================================================
  // ALPHA AND FINAL OUTPUT PROCESSING
  //=============================================================================
  r0.w = -r0.w * cb2[16].w + 1;           // cb2[16].w = Alpha cutoff value
  o0.w = -cb2[25].w * r0.w + 1;           // cb2[25].w = Alpha blend factor
  r0.w = cb3[7].w + -cb3[6].w;
  r0.w = cb2[31].y * r0.w + cb3[6].w;     // cb2[31].y = Final color blend factor
  r1.xyz = r0.xyz * r0.www;
  r0.xyz = -r0.xyz * r0.www + r0.xyz;
  r0.xyz = cb2[31].zzz * r0.xyz + r1.xyz; // cb2[31].z = Final color blend factor
  r1.xyz = float3(1,1,1) + -r0.xyz;
  o0.xyz = cb2[26].yyy * r1.xyz + r0.xyz; // cb2[26].y = Final color correction factor

  //=============================================================================
  // ADDITIONAL OUTPUT (NORMAL, DEPTH, ETC.)
  //=============================================================================
  r0.x = 0.5 * cb2[30].w;                 // cb2[30].w = Additional output scale factor
  o1.y = cb3[16].w * r0.x;
  o1.xzw = float3(0,0,1);
  return;
}