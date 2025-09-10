// ---- Created with 3Dmigoto v1.4.1 on Thu Jun 26 05:55:01 2025

// ============================================================================
// Resource Bindings
// ============================================================================
Texture2D<float4> t11 : register(t11);

Texture2D<float4> t10 : register(t10);

Texture2D<float4> t9 : register(t9);

Texture2D<float4> t8 : register(t8);

Texture2D<float4> t7 : register(t7);

Texture2D<float4> t6 : register(t6);

Texture2D<float4> t5 : register(t5);

Texture2D<float4> t4 : register(t4);

Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

TextureCube<float4> t0 : register(t0);

// ============================================================================
// Sampler States
// ============================================================================
SamplerState s8_s : register(s8);

SamplerState s7_s : register(s7);

SamplerComparisonState s6_s : register(s6);

SamplerState s5_s : register(s5);

SamplerState s4_s : register(s4);

SamplerState s3_s : register(s3);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

// ============================================================================
// Constant Buffers
// ============================================================================
cbuffer cb4 : register(b4)
{
  float4 cb4[33];
}

cbuffer cb3 : register(b3)
{
  float4 cb3[19];
}

cbuffer cb2 : register(b2)
{
  float4 cb2[26];
}

cbuffer cb1 : register(b1)
{
  float4 cb1[35];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[175];
}

// ============================================================================
// 3Dmigoto Declarations
// ============================================================================
#define cmp -

// ============================================================================
// Main Pixel Shader Entry Point
// ============================================================================
void main(
  float2 v0 : TEXCOORD0,
  float2 w0 : TEXCOORD1,
  float4 v1 : TEXCOORD2,
  float4 v2 : TEXCOORD3,
  float4 v3 : TEXCOORD4,
  float4 v4 : TEXCOORD5,
  float4 v5 : TEXCOORD6,
  float3 v6 : TEXCOORD8,
  float4 v7 : COLOR0,
  float4 v8 : SV_POSITION0,
  out float4 o0 : SV_Target0,
  out float4 o1 : SV_Target1)
{
  // ============================================================================
  // Variable Declarations
  // ============================================================================
  const float4 icb[] = { { 1.000000, 0, 0, 0},
                              { 0, 1.000000, 0, 0},
                              { 0, 0, 1.000000, 0},
                              { 0, 0, 0, 1.000000} };
  float4 r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17,r18;
  uint4 bitmask, uiDest;
  float4 fDest;

  // ============================================================================
  // Initial Texture Sampling and Setup
  // ============================================================================
  r0.xyzw = t5.Sample(s5_s, v0.xy).xyzw;
  r0.x = 0.5 * cb2[22].y;
  r0.x = r0.y * cb2[22].y + -r0.x;
  r0.y = dot(v6.xyz, v6.xyz);
  r0.y = rsqrt(r0.y);
  r0.zw = v6.xy * r0.yy;
  r0.y = v6.z * r0.y + 0.419999987;
  r0.yz = r0.zw / r0.yy;
  r0.xy = r0.xx * r0.yz;
  r0.xy = r0.xy * cb2[22].zz + v0.xy;
  r1.xyzw = t2.Sample(s2_s, r0.xy).xyzw;
  r2.xyzw = t3.Sample(s3_s, r0.xy).xyzw;
  r0.z = 1 + -cb2[22].w;
  r0.z = r2.z * cb2[22].w + r0.z;
  r0.w = cb2[21].z * r2.w;
  r3.xyz = cb2[18].xyz * r1.xyz;
  r4.xy = r2.xy + r2.xy;
  r2.xy = r2.xy * float2(2,2) + float2(-1,-1);
  r2.x = dot(r2.xy, r2.xy);
  r2.x = min(1, r2.x);
  r2.x = 1 + -r2.x;
  r2.x = sqrt(r2.x);
  r2.x = max(1.00000002e-16, r2.x);
  r5.xyzw = t4.Sample(s4_s, r0.xy).xyzw;
  r5.xyz = r5.www * r5.xyz;
  r5.xyz = cb2[20].xyz * r5.xyz;
  r0.x = cb2[14].x * v8.w;
  r0.x = max(1, r0.x);
  r4.z = r2.x * r0.x;
  r0.x = dot(v4.xyz, v4.xyz);
  r0.x = max(1.17549435e-38, r0.x);
  r0.x = rsqrt(r0.x);
  r2.xyz = v4.xyz * r0.xxx;
  r6.xyz = v3.yzx * v2.zxy;
  r6.xyz = v2.yzx * v3.zxy + -r6.xyz;
  r6.xyz = v3.www * r6.xyz;
  r4.xyz = float3(-1,-1,-1) + r4.xyz;
  r4.xyz = cb2[1].xxx * r4.xyz + float3(0,0,1);
  r6.xyz = r4.yyy * r6.xyz;
  r4.xyw = r4.xxx * v3.xyz + r6.xyz;
  r4.xyz = r4.zzz * v2.xyz + r4.xyw;
  r0.y = dot(r4.xyz, r4.xyz);
  r0.y = rsqrt(r0.y);
  r6.xyz = r4.xyz * r0.yyy;
  r3.w = cmp(0.5 < cb2[24].w);
  if (r3.w != 0) {
    r7.xyzw = t1.Sample(s1_s, w0.xy).xyzw;
  } else {
    r7.xyzw = t1.Sample(s1_s, w0.xy).xyzw;
  }
  r8.xyz = cb2[20].xyz * cb2[20].www + -r5.xyz;
  r5.xyz = cb2[21].xxx * r8.xyz + r5.xyz;
  r3.w = saturate(r2.w * cb2[21].z + cb3[18].y);
  r4.w = -r1.w * 0.959999979 + 0.959999979;
  r5.w = 1 + -r4.w;
  r3.xyz = r4.www * r3.xyz;
  r1.xyz = r1.xyz * cb2[18].xyz + float3(-0.0399999991,-0.0399999991,-0.0399999991);
  r1.xyz = r1.www * r1.xyz + float3(0.0399999991,0.0399999991,0.0399999991);
  r4.w = 1 + -r3.w;
  r6.w = r4.w * r4.w;
  r6.w = max(0.0078125, r6.w);
  r7.w = r6.w * r6.w;
  r3.w = saturate(r5.w + r3.w);
  r8.x = r6.w * 4 + 2;
  r8.yzw = cb3[10].xyz + cb0[5].xyz;
  r9.x = dot(r8.yzw, r8.yzw);
  r9.x = rsqrt(r9.x);
  r9.yzw = r9.xxx * r8.yzw;
  r10.x = 10 / cb4[32].z;
  r10.x = sqrt(r10.x);
  r10.x = r10.x * r10.x + r10.x;
  r10.yzw = -cb0[15].xyz + v1.xyz;
  r10.y = dot(r10.yzw, r10.yzw);
  r10.x = cmp(r10.x >= r10.y);
  if (r10.x != 0) {
    r10.xyz = -cb4[20].xyz + v1.xyz;
    r11.xyz = -cb4[21].xyz + v1.xyz;
    r12.xyz = -cb4[22].xyz + v1.xyz;
    r13.xyz = -cb4[23].xyz + v1.xyz;
    r10.x = dot(r10.xyz, r10.xyz);
    r10.y = dot(r11.xyz, r11.xyz);
    r10.z = dot(r12.xyz, r12.xyz);
    r10.w = dot(r13.xyz, r13.xyz);
    r10.xyzw = cmp(r10.xyzw < cb4[24].xyzw);
    r11.xyzw = r10.xyzw ? float4(1,1,1,1) : 0;
    r10.xyz = r10.xyz ? float3(-1,-1,-1) : float3(-0,-0,-0);
    r10.xyz = r11.yzw + r10.xyz;
    r11.yzw = max(float3(0,0,0), r10.xyz);
    r10.x = dot(r11.xyzw, float4(4,3,2,1));
    r10.x = 4 + -r10.x;
    r10.x = min(3, r10.x);
    r10.x = (uint)r10.x;
    r10.x = (uint)r10.x << 2;
    r10.yzw = cb4[r10.x+1].xyz * v1.yyy;
    r10.yzw = cb4[r10.x+0].xyz * v1.xxx + r10.yzw;
    r10.yzw = cb4[r10.x+2].xyz * v1.zzz + r10.yzw;
    r10.xyz = cb4[r10.x+3].xyz + r10.yzw;
    r11.xy = r10.xy * cb4[25].zw + float2(0.5,0.5);
    r11.xy = floor(r11.xy);
    r10.xy = r10.xy * cb4[25].zw + -r11.xy;
    r12.xyzw = float4(0.5,1,0.5,1) + r10.xxyy;
    r13.xyzw = r12.xxzz * r12.xxzz;
    r11.zw = float2(0.0799999982,0.0799999982) * r13.yw;
    r12.xz = r13.xz * float2(0.5,0.5) + -r10.xy;
    r13.xy = float2(1,1) + -r10.xy;
    r13.zw = min(float2(0,0), r10.xy);
    r13.zw = -r13.zw * r13.zw + r13.xy;
    r10.xy = max(float2(0,0), r10.xy);
    r10.xy = -r10.xy * r10.xy + r12.yw;
    r13.zw = float2(1,1) + r13.zw;
    r10.xy = float2(1,1) + r10.xy;
    r14.xy = float2(0.159999996,0.159999996) * r12.xz;
    r15.xy = float2(0.159999996,0.159999996) * r13.xy;
    r13.xy = float2(0.159999996,0.159999996) * r13.zw;
    r16.xy = float2(0.159999996,0.159999996) * r10.xy;
    r10.xy = float2(0.159999996,0.159999996) * r12.yw;
    r14.z = r13.x;
    r14.w = r10.x;
    r15.z = r16.x;
    r15.w = r11.z;
    r12.xyzw = r15.zwxz + r14.zwxz;
    r13.z = r14.y;
    r13.w = r10.y;
    r16.z = r15.y;
    r16.w = r11.w;
    r10.xyw = r16.zyw + r13.zyw;
    r13.xyz = r15.xzw / r12.zwy;
    r13.xyz = float3(-2.5,-0.5,1.5) + r13.xyz;
    r14.xyz = r16.zyw / r10.xyw;
    r14.xyz = float3(-2.5,-0.5,1.5) + r14.xyz;
    r13.xyz = cb4[25].xxx * r13.yxz;
    r14.xyz = cb4[25].yyy * r14.xyz;
    r13.w = r14.x;
    r15.xyzw = r11.xyxy * cb4[25].xyxy + r13.ywxw;
    r11.zw = r11.xy * cb4[25].xy + r13.zw;
    r14.w = r13.y;
    r13.yw = r14.yz;
    r16.xyzw = r11.xyxy * cb4[25].xyxy + r13.xyzy;
    r14.xyzw = r11.xyxy * cb4[25].xyxy + r14.wywz;
    r13.xyzw = r11.xyxy * cb4[25].xyxy + r13.xwzw;
    r17.xyzw = r12.zwyz * r10.xxxy;
    r18.xyzw = r12.xyzw * r10.yyww;
    r10.x = r12.y * r10.w;
    r10.y = cb2[0].w + r10.z;
    r10.w = t6.SampleCmpLevelZero(s6_s, r15.xy, r10.y).x;
    r11.x = t6.SampleCmpLevelZero(s6_s, r15.zw, r10.y).x;
    r11.x = r17.y * r11.x;
    r10.w = r17.x * r10.w + r11.x;
    r11.x = t6.SampleCmpLevelZero(s6_s, r11.zw, r10.y).x;
    r10.w = r17.z * r11.x + r10.w;
    r11.x = t6.SampleCmpLevelZero(s6_s, r14.xy, r10.y).x;
    r10.w = r17.w * r11.x + r10.w;
    r11.x = t6.SampleCmpLevelZero(s6_s, r16.xy, r10.y).x;
    r10.w = r18.x * r11.x + r10.w;
    r11.x = t6.SampleCmpLevelZero(s6_s, r16.zw, r10.y).x;
    r10.w = r18.y * r11.x + r10.w;
    r11.x = t6.SampleCmpLevelZero(s6_s, r14.zw, r10.y).x;
    r10.w = r18.z * r11.x + r10.w;
    r11.x = t6.SampleCmpLevelZero(s6_s, r13.xy, r10.y).x;
    r10.w = r18.w * r11.x + r10.w;
    r10.y = t6.SampleCmpLevelZero(s6_s, r13.zw, r10.y).x;
    r10.x = r10.x * r10.y + r10.w;
    r10.y = 1 + -cb4[32].x;
    r10.x = r10.x * cb4[32].x + r10.y;
    r10.y = cmp(cb2[25].x == 0.000000);
    r10.w = cmp(0 >= r10.z);
    r10.z = cmp(r10.z >= 1);
    r10.z = (int)r10.z | (int)r10.w;
    r10.y = (int)r10.z | (int)r10.y;
    r10.x = min(1, r10.x);
    r10.x = r10.y ? 1 : r10.x;
  } else {
    r10.x = 1;
  }
  r10.y = dot(-r2.xyz, r6.xyz);
  r10.y = r10.y + r10.y;
  r10.yzw = r6.xyz * -r10.yyy + -r2.xyz;
  r11.x = saturate(dot(r6.xyz, r2.xyz));
  r11.x = 1 + -r11.x;
  r11.x = r11.x * r11.x;
  r11.x = r11.x * r11.x;
  r11.y = cb2[15].y * r11.x;
  r12.xyz = r7.xyz * r0.zzz;
  r11.z = -r4.w * 0.699999988 + 1.70000005;
  r11.z = r11.z * r4.w;
  r11.z = 6 * r11.z;
  r11.w = cmp(0 < cb1[34].w);
  r12.w = cmp(0 != cb2[2].w);
  r13.xyz = cmp(float3(0,0,0) < r10.yzw);
  r13.xyz = r13.xyz ? cb1[32].xyz : cb1[33].xyz;
  r13.xyz = -v1.xyz + r13.xyz;
  r14.xyz = -cb1[34].xyz + r13.xyz;
  r14.xyz = cb2[2].xyz + r14.xyz;
  r14.xyz = r14.xyz / r10.yzw;
  r13.w = min(r14.x, r14.y);
  r13.w = min(r13.w, r14.z);
  r14.xyz = -cb2[2].xyz + v1.xyz;
  r14.xyz = r10.yzw * r13.www + r14.xyz;
  r13.xyz = r13.xyz / r10.yzw;
  r13.x = min(r13.x, r13.y);
  r13.x = min(r13.x, r13.z);
  r13.yzw = -cb1[34].xyz + v1.xyz;
  r13.xyz = r10.yzw * r13.xxx + r13.yzw;
  r13.xyz = r12.www ? r14.xyz : r13.xyz;
  r10.yzw = r11.www ? r13.xyz : r10.yzw;
  r13.xyzw = t0.SampleLevel(s0_s, r10.yzw, r11.z).xyzw;
  r10.y = -1 + r13.w;
  r10.y = cb1[30].w * r10.y + 1;
  r10.y = max(0, r10.y);
  r10.y = log2(r10.y);
  r10.y = cb1[30].y * r10.y;
  r10.y = exp2(r10.y);
  r10.y = cb1[30].x * r10.y;
  r10.yzw = r10.yyy * r13.xyz;
  r10.yzw = r10.yzw * r0.zzz;
  r10.yzw = cb2[13].www * r10.yzw;
  r11.z = cb2[11].z + -cb2[11].y;
  r1.w = r1.w * r11.z + cb2[11].y;
  r11.z = 1 + -r1.w;
  r1.w = r4.w * r11.z + r1.w;
  r4.w = 1 + -cb3[11].w;
  r4.w = r10.x * r4.w + cb3[11].w;
  r1.w = r4.w * r1.w;
  r10.yzw = r10.yzw * r1.www;
  r12.xyz = r12.xyz * r3.xyz;
  r12.xyz = cb2[10].www * r12.xyz;
  r12.xyz = cb3[6].xyz * r12.xyz;
  r10.yzw = cb3[7].xyz * r10.yzw;
  r11.zw = r6.ww * r6.ww + float2(-1,1);
  r1.w = 1 / r11.w;
  r13.xyz = r3.www + -r1.xyz;
  r13.xyz = r11.yyy * r13.xyz + r1.xyz;
  r13.xyz = r13.xyz * r1.www;
  r10.yzw = r10.yzw * r13.xyz + r12.xyz;
  r1.w = cb1[26].z * r10.x;
  r3.w = saturate(dot(r6.xyz, r9.yzw));
  r3.w = saturate(cb3[13].w * r3.w);
  r1.w = r3.w * r1.w;
  r12.yzw = cb0[6].xyz * r1.www;
  r1.w = -1 + r0.z;
  r1.w = cb2[15].x * r1.w + 1;
  r12.yzw = r12.yzw * r1.www;
  r3.xyz = cb2[11].xxx * r3.xyz;
  r8.yzw = r8.yzw * r9.xxx + r2.xyz;
  r1.w = dot(r8.yzw, r8.yzw);
  r1.w = max(1.17549435e-38, r1.w);
  r1.w = rsqrt(r1.w);
  r8.yzw = r8.yzw * r1.www;
  r1.w = saturate(dot(r9.yzw, r8.yzw));
  r3.w = saturate(dot(r6.xyz, r8.yzw));
  r3.w = r3.w * r3.w;
  r3.w = r3.w * r11.z + 1.00399995;
  r1.w = r1.w * r1.w;
  r3.w = r3.w * r3.w;
  r1.w = max(0.100000001, r1.w);
  r1.w = r3.w * r1.w;
  r1.w = r1.w * r8.x;
  r1.w = r7.w / r1.w;
  r8.yzw = cb3[9].xyz * r1.www;
  r8.yzw = cb3[12].www * r8.yzw;
  r8.yzw = r8.yzw * r1.xyz;
  r8.yzw = cb2[13].zzz * r8.yzw;
  r3.xyz = r3.xyz * cb3[8].xyz + r8.yzw;
  r3.xyz = r3.xyz * r12.yzw + r10.yzw;
  if (cb0[174].z != 0) {
    r8.yzw = float3(0.03125,0.03125,0.03125) * cb0[173].xyz;
    r9.xyz = -cb0[172].xyz + v1.xyz;
    r8.yzw = r9.xyz / r8.yzw;
    r8.yw = (int2)r8.yw;
    r1.w = (uint)r8.w << 5;
    r1.w = (int)r8.y + (int)r1.w;
    r1.w = (int)r1.w;
    r9.x = r1.w * 0.0009765625 + 0.00048828125;
    r1.w = trunc(r8.z);
    r9.y = r1.w * 0.03125 + 0.015625;
    r9.xyzw = t8.SampleLevel(s8_s, r9.xy, 0).xyzw;
    r1.w = cb0[174].x * cb0[174].x + -1;
    r3.w = 0.5 * cb0[174].y;
    r10.yw = float2(1,1);
    r8.yzw = float3(0,0,0);
    r4.w = cb0[174].w;
    r6.w = 0;
    while (true) {
      r11.y = cmp((int)r6.w >= 4);
      if (r11.y != 0) break;
      r11.y = cmp((int)r4.w == 1);
      if (r11.y != 0) {
        r11.w = dot(r9.xyzw, icb[r6.w+0].xyzw);
        r12.y = cmp(r11.w != 1.000000);
        if (r12.y != 0) {
          r11.w = r11.w * r1.w + 0.5;
          r11.w = (int)r11.w;
          r11.w = (uint)r11.w;
          r12.z = cb0[174].x * r11.w;
          r12.z = cmp(r12.z >= -r12.z);
          r12.z = r12.z ? cb0[174].x : -cb0[174].x;
          r12.w = 1 / r12.z;
          r12.w = r12.w * r11.w;
          r12.w = frac(r12.w);
          r12.z = r12.z * r12.w;
          r11.w = r11.w / cb0[174].x;
          r12.z = trunc(r12.z);
          r13.x = r12.z * cb0[174].y + r3.w;
          r11.w = trunc(r11.w);
          r13.y = r11.w * cb0[174].y + r3.w;
          r14.xyzw = t7.SampleLevel(s7_s, r13.xy, 0).xyzw;
          r15.xyzw = t9.SampleLevel(s7_s, r13.xy, 0).xyzw;
          r16.xyzw = t10.SampleLevel(s7_s, r13.xy, 0).xyzw;
          r13.xyzw = t11.SampleLevel(s7_s, r13.xy, 0).xyzw;
          r15.xyz = -v1.xyz + r15.xyz;
          r11.w = dot(r15.xyz, r15.xyz);
          r11.w = max(6.10351562e-05, r11.w);
          r12.z = rsqrt(r11.w);
          r15.xyz = r15.xyz * r12.zzz;
          r12.z = 1 / r11.w;
          r11.w = r11.w * r15.w;
          r11.w = -r11.w * r11.w + 1;
          r11.w = max(0, r11.w);
          r11.w = r11.w * r11.w;
          r11.w = r12.z * r11.w;
          r12.z = dot(r16.xyz, r15.xyz);
          r12.z = saturate(r12.z * r13.z + r13.w);
          r12.z = r12.z * r12.z;
          r10.x = r12.z * r11.w;
          r12.zw = r10.xy;
        } else {
          r15.xyz = float3(0,0,0);
          r14.xyz = float3(0,0,0);
          r12.zw = float2(0,0);
        }
      } else {
        r15.xyz = float3(0,0,0);
        r14.xyz = float3(0,0,0);
        r12.zw = float2(0,0);
      }
      if (r11.y == 0) {
        r10.x = dot(r9.xyzw, icb[r6.w+0].xyzw);
        r11.y = cmp(r10.x != 1.000000);
        if (r11.y != 0) {
          r10.x = r10.x * r1.w + 0.5;
          r10.x = (int)r10.x;
          r10.x = (uint)r10.x;
          r11.y = cb0[174].x * r10.x;
          r11.y = cmp(r11.y >= -r11.y);
          r11.y = r11.y ? cb0[174].x : -cb0[174].x;
          r11.w = 1 / r11.y;
          r11.w = r11.w * r10.x;
          r11.w = frac(r11.w);
          r11.y = r11.y * r11.w;
          r10.x = r10.x / cb0[174].x;
          r11.y = trunc(r11.y);
          r13.x = r11.y * cb0[174].y + r3.w;
          r10.x = trunc(r10.x);
          r13.y = r10.x * cb0[174].y + r3.w;
          r14.xyzw = t7.SampleLevel(s7_s, r13.xy, 0).xyzw;
          r13.xyzw = t9.SampleLevel(s7_s, r13.xy, 0).xyzw;
          r13.xyz = -v1.xyz + r13.xyz;
          r10.x = dot(r13.xyz, r13.xyz);
          r10.x = max(6.10351562e-05, r10.x);
          r11.y = rsqrt(r10.x);
          r15.xyz = r13.xyz * r11.yyy;
          r11.y = 1 / r10.x;
          r10.x = r10.x * r13.w;
          r10.x = -r10.x * r10.x + 1;
          r10.x = max(0, r10.x);
          r10.x = r10.x * r10.x;
          r10.z = r11.y * r10.x;
          r12.zw = r10.zw;
          r12.y = -1;
        } else {
          r12.y = 0;
        }
      }
      if (r12.y != 0) {
        r10.x = r12.z * r12.w;
        r10.z = saturate(dot(r6.xyz, r15.xyz));
        r10.z = saturate(cb3[13].w * r10.z);
        r10.x = r10.x * r10.z;
        r12.yzw = r14.xyz * r10.xxx;
        r13.xyz = v4.xyz * r0.xxx + r15.xyz;
        r10.x = dot(r13.xyz, r13.xyz);
        r10.x = max(1.17549435e-38, r10.x);
        r10.x = rsqrt(r10.x);
        r13.xyz = r13.xyz * r10.xxx;
        r10.x = saturate(dot(r15.xyz, r13.xyz));
        r10.z = saturate(dot(r6.xyz, r13.xyz));
        r10.xz = r10.xz * r10.xz;
        r10.z = r10.z * r11.z + 1.00399995;
        r10.z = r10.z * r10.z;
        r10.x = max(0.100000001, r10.x);
        r10.x = r10.z * r10.x;
        r10.x = r10.x * r8.x;
        r10.x = r7.w / r10.x;
        r13.xyz = cb3[9].xyz * r10.xxx;
        r13.xyz = cb3[12].www * r13.xyz;
        r13.xyz = r13.xyz * r1.xyz;
        r13.xyz = cb2[13].zzz * r13.xyz;
        r8.yzw = r13.xyz * r12.yzw + r8.yzw;
      } else {
        break;
      }
      r6.w = (int)r6.w + 1;
    }
    r8.yzw = max(float3(0,0,0), r8.yzw);
    r8.yzw = min(float3(3,3,3), r8.yzw);
  } else {
    r8.yzw = float3(0,0,0);
  }
  r3.xyz = r8.yzw + r3.xyz;
  r4.xyz = r4.xyz * r0.yyy + r2.xyz;
  r4.xyz = float3(0,1,0) + r4.xyz;
  r0.x = dot(r4.xyz, r4.xyz);
  r0.x = max(1.17549435e-38, r0.x);
  r0.x = rsqrt(r0.x);
  r8.yzw = r4.xyz * r0.xxx;
  r2.xyz = r4.xyz * r0.xxx + r2.xyz;
  r0.x = dot(r2.xyz, r2.xyz);
  r0.x = max(1.17549435e-38, r0.x);
  r0.x = rsqrt(r0.x);
  r2.xyz = r2.xyz * r0.xxx;
  r0.x = saturate(dot(r6.xyz, r2.xyz));
  r0.y = saturate(dot(r8.yzw, r2.xyz));
  r1.w = max(cb3[8].w, r11.z);
  r0.xy = r0.xy * r0.xy;
  r0.x = r0.x * r1.w + 1.00001001;
  r0.xz = r0.xw * r0.xz;
  r0.y = max(0.100000001, r0.y);
  r0.x = r0.x * r0.y;
  r0.x = r0.x * r8.x;
  r0.x = r7.w / r0.x;
  r7.xyz = saturate(r7.xyz);
  r2.xyz = log2(r7.xyz);
  r2.xyz = cb3[10].www * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r2.xyz = r2.xyz * r0.xxx;
  r1.yzw = r2.xyz * r1.xyz;
  r0.x = r3.x + r3.y;
  r0.x = saturate(r0.x + r3.z);
  r0.x = 1 + -r0.x;
  r1.yzw = cb3[9].www * r1.yzw;
  r1.yzw = r1.yzw * r0.xxx;
  r1.yzw = r1.yzw * cb2[13].zzz + r3.xyz;
  r1.yzw = r1.yzw + r5.xyz;
  r0.x = 1 + -cb2[18].w;
  r0.x = -cb2[10].z * r0.x + 1;
  r0.y = cb3[7].w + -cb3[6].w;
  r0.y = cb2[12].y * r0.y + cb3[6].w;
  o0.xyz = r1.yzw * r0.yyy;
  r0.y = r5.x + r5.y;
  r0.y = r0.y + r5.z;
  r0.y = cb2[12].z * r0.y;
  o1.x = saturate(0.333330005 * r0.y);
  r0.y = 0.5 * cb2[11].w;
  o1.y = cb3[15].w * r0.y;
  o1.w = saturate(cb2[12].x + r0.x);
  r0.y = -r2.w * cb2[21].z + 1;
  r0.y = r0.y * r0.y;
  r0.y = max(0.0078125, r0.y);
  r1.y = saturate(r2.w * cb2[21].z + r5.w);
  r0.y = r0.y * r0.y + 1;
  r0.y = 1 / r0.y;
  r0.w = r1.y + -r1.x;
  r0.w = r11.x * r0.w + r1.x;
  r0.y = r0.y * r0.w;
  r0.y = cb3[7].x * r0.y + r12.x;
  r0.y = r0.z * r0.y;
  o1.z = cb2[0].x * r0.y;
  o0.w = r0.x;

  return;
}