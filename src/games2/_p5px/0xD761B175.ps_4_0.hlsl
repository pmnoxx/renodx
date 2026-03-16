// ---- Created with 3Dmigoto v1.4.1 on Thu Jun 26 03:44:30 2025
Texture2D<float4> t11 : register(t11);

Texture2D<float4> t10 : register(t10);

Texture2D<float4> t9 : register(t9);

Texture2D<float4> t8 : register(t8);

Texture2D<float4> t7 : register(t7);

Texture3D<float4> t6 : register(t6);

Texture2D<float4> t5 : register(t5);

Texture2D<float4> t4 : register(t4);

Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

TextureCube<float4> t0 : register(t0);

SamplerState s8_s : register(s8);

SamplerState s7_s : register(s7);

SamplerState s6_s : register(s6);

SamplerComparisonState s5_s : register(s5);

SamplerState s4_s : register(s4);

SamplerState s3_s : register(s3);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

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
  float4 cb1[47];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[175];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : TEXCOORD0,
  float4 v1 : TEXCOORD1,
  float4 v2 : TEXCOORD2,
  float4 v3 : TEXCOORD3,
  float4 v4 : TEXCOORD4,
  float4 v5 : TEXCOORD5,
  float4 v6 : TEXCOORD6,
  float3 v7 : TEXCOORD8,
  float4 v8 : COLOR0,
  float4 v9 : SV_POSITION0,
  out float4 o0 : SV_Target0,
  out float4 o1 : SV_Target1)
{
  const float4 icb[] = { { 1.000000, 0, 0, 0},
                              { 0, 1.000000, 0, 0},
                              { 0, 0, 1.000000, 0},
                              { 0, 0, 0, 1.000000} };
  float4 r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,r14,r15,r16,r17;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = t4.Sample(s4_s, v0.xy).xyzw;
  r0.x = 0.5 * cb2[22].y;
  r0.x = r0.y * cb2[22].y + -r0.x;
  r0.y = dot(v7.xyz, v7.xyz);
  r0.y = rsqrt(r0.y);
  r0.zw = v7.xy * r0.yy;
  r0.y = v7.z * r0.y + 0.419999987;
  r0.yz = r0.zw / r0.yy;
  r0.xy = r0.xx * r0.yz;
  r0.xy = r0.xy * cb2[22].zz + v0.xy;
  r1.xyzw = t1.Sample(s1_s, r0.xy).xyzw;
  r2.xyzw = t2.Sample(s2_s, r0.xy).xyzw;
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
  r5.xyzw = t3.Sample(s3_s, r0.xy).xyzw;
  r5.xyz = r5.www * r5.xyz;
  r5.xyz = cb2[20].xyz * r5.xyz;
  r0.x = cb2[14].x * v9.w;
  r0.x = max(1, r0.x);
  r4.z = r2.x * r0.x;
  r0.x = dot(v5.xyz, v5.xyz);
  r0.x = max(1.17549435e-38, r0.x);
  r0.x = rsqrt(r0.x);
  r2.xyz = v5.xyz * r0.xxx;
  r6.xyz = v4.yzx * v3.zxy;
  r6.xyz = v3.yzx * v4.zxy + -r6.xyz;
  r6.xyz = v4.www * r6.xyz;
  r4.xyz = float3(-1,-1,-1) + r4.xyz;
  r4.xyz = cb2[1].xxx * r4.xyz + float3(0,0,1);
  r6.xyz = r4.yyy * r6.xyz;
  r4.xyw = r4.xxx * v4.xyz + r6.xyz;
  r4.xyz = r4.zzz * v3.xyz + r4.xyw;
  r0.y = dot(r4.xyz, r4.xyz);
  r0.y = rsqrt(r0.y);
  r4.xyz = r4.xyz * r0.yyy;
  r0.y = cmp(0.5 < cb2[24].w);
  if (r0.y != 0) {
    r6.xyz = -cb2[23].xyz + v2.xyz;
    r0.y = dot(r6.xyz, r6.xyz);
    r0.y = rsqrt(r0.y);
    r6.xyz = r6.xyz * r0.yyy;
    r7.xyz = cb2[24].xyz * r6.xyz;
    r6.xyz = -r6.xyz * cb2[24].xyz + v3.xyz;
    r6.xyz = cb2[23].www * r6.xyz + r7.xyz;
    r0.y = dot(r6.xyz, r6.xyz);
    r0.y = rsqrt(r0.y);
    r6.xyz = r6.xyz * r0.yyy;
    r6.w = 1;
    r7.x = dot(cb1[40].xyzw, r6.xyzw);
    r7.y = dot(cb1[41].xyzw, r6.xyzw);
    r7.z = dot(cb1[42].xyzw, r6.xyzw);
    r8.xyzw = r6.xyzz * r6.yzzx;
    r9.x = dot(cb1[43].xyzw, r8.xyzw);
    r9.y = dot(cb1[44].xyzw, r8.xyzw);
    r9.z = dot(cb1[45].xyzw, r8.xyzw);
    r0.y = r6.y * r6.y;
    r0.y = r6.x * r6.x + -r0.y;
    r6.xyz = cb1[46].xyz * r0.yyy + r9.xyz;
    r6.xyz = r7.xyz + r6.xyz;
    r6.xyz = max(float3(0,0,0), r6.xyz);
  } else {
    r4.w = 1;
    r7.x = dot(cb1[40].xyzw, r4.xyzw);
    r7.y = dot(cb1[41].xyzw, r4.xyzw);
    r7.z = dot(cb1[42].xyzw, r4.xyzw);
    r8.xyzw = r4.xyzz * r4.yzzx;
    r9.x = dot(cb1[43].xyzw, r8.xyzw);
    r9.y = dot(cb1[44].xyzw, r8.xyzw);
    r9.z = dot(cb1[45].xyzw, r8.xyzw);
    r0.y = r4.y * r4.y;
    r0.y = r4.x * r4.x + -r0.y;
    r8.xyz = cb1[46].xyz * r0.yyy + r9.xyz;
    r7.xyz = r8.xyz + r7.xyz;
    r7.xyz = max(float3(0,0,0), r7.xyz);
    r8.xy = cmp(cb1[9].xy == float2(0,1));
    r9.xyz = cb1[11].xyz * v2.yyy;
    r9.xyz = cb1[10].xyz * v2.xxx + r9.xyz;
    r9.xyz = cb1[12].xyz * v2.zzz + r9.xyz;
    r9.xyz = cb1[13].xyz + r9.xyz;
    r8.yzw = r8.yyy ? r9.xyz : v2.xyz;
    r8.yzw = -cb1[15].xyz + r8.yzw;
    r9.yzw = cb1[14].xyz * r8.yzw;
    r0.y = 0.25 * r9.y;
    r3.w = 0.5 * cb1[9].z;
    r4.w = -cb1[9].z * 0.5 + 0.25;
    r0.y = max(r3.w, r0.y);
    r9.x = min(r0.y, r4.w);
    r10.xyzw = t6.SampleLevel(s6_s, r9.xzw, 0).xyzw;
    r8.yzw = float3(0.25,0,0) + r9.xzw;
    r11.xyzw = t6.SampleLevel(s6_s, r8.yzw, 0).xyzw;
    r8.yzw = float3(0.5,0,0) + r9.xzw;
    r9.xyzw = t6.SampleLevel(s6_s, r8.yzw, 0).xyzw;
    r12.xyz = v3.xyz;
    r12.w = 1;
    r10.x = dot(r10.xyzw, r12.xyzw);
    r10.y = dot(r11.xyzw, r12.xyzw);
    r10.z = dot(r9.xyzw, r12.xyzw);
    r6.xyz = r8.xxx ? r7.xyz : r10.xyz;
  }
  r7.xyz = cb2[20].xyz * cb2[20].www + -r5.xyz;
  r5.xyz = cb2[21].xxx * r7.xyz + r5.xyz;
  r0.y = saturate(r2.w * cb2[21].z + cb3[18].y);
  r3.w = -r1.w * 0.959999979 + 0.959999979;
  r4.w = 1 + -r3.w;
  r3.xyz = r3.xyz * r3.www;
  r1.xyz = r1.xyz * cb2[18].xyz + float3(-0.0399999991,-0.0399999991,-0.0399999991);
  r1.xyz = r1.www * r1.xyz + float3(0.0399999991,0.0399999991,0.0399999991);
  r3.w = 1 + -r0.y;
  r5.w = r3.w * r3.w;
  r5.w = max(0.0078125, r5.w);
  r6.w = r5.w * r5.w;
  r0.y = saturate(r4.w + r0.y);
  r7.x = r5.w * 4 + 2;
  r7.yzw = cb3[10].xyz + cb0[5].xyz;
  r8.x = dot(r7.yzw, r7.yzw);
  r8.x = rsqrt(r8.x);
  r8.yzw = r8.xxx * r7.yzw;
  r9.x = 10 / cb4[32].z;
  r9.x = sqrt(r9.x);
  r9.x = r9.x * r9.x + r9.x;
  r9.yzw = -cb0[15].xyz + v2.xyz;
  r9.y = dot(r9.yzw, r9.yzw);
  r9.x = cmp(r9.x >= r9.y);
  if (r9.x != 0) {
    r9.xyz = -cb4[20].xyz + v2.xyz;
    r10.xyz = -cb4[21].xyz + v2.xyz;
    r11.xyz = -cb4[22].xyz + v2.xyz;
    r12.xyz = -cb4[23].xyz + v2.xyz;
    r9.x = dot(r9.xyz, r9.xyz);
    r9.y = dot(r10.xyz, r10.xyz);
    r9.z = dot(r11.xyz, r11.xyz);
    r9.w = dot(r12.xyz, r12.xyz);
    r9.xyzw = cmp(r9.xyzw < cb4[24].xyzw);
    r10.xyzw = r9.xyzw ? float4(1,1,1,1) : 0;
    r9.xyz = r9.xyz ? float3(-1,-1,-1) : float3(-0,-0,-0);
    r9.xyz = r10.yzw + r9.xyz;
    r10.yzw = max(float3(0,0,0), r9.xyz);
    r9.x = dot(r10.xyzw, float4(4,3,2,1));
    r9.x = 4 + -r9.x;
    r9.x = min(3, r9.x);
    r9.x = (uint)r9.x;
    r9.x = (uint)r9.x << 2;
    r9.yzw = cb4[r9.x+1].xyz * v2.yyy;
    r9.yzw = cb4[r9.x+0].xyz * v2.xxx + r9.yzw;
    r9.yzw = cb4[r9.x+2].xyz * v2.zzz + r9.yzw;
    r9.xyz = cb4[r9.x+3].xyz + r9.yzw;
    r10.xy = r9.xy * cb4[25].zw + float2(0.5,0.5);
    r10.xy = floor(r10.xy);
    r9.xy = r9.xy * cb4[25].zw + -r10.xy;
    r11.xyzw = float4(0.5,1,0.5,1) + r9.xxyy;
    r12.xyzw = r11.xxzz * r11.xxzz;
    r10.zw = float2(0.0799999982,0.0799999982) * r12.yw;
    r11.xz = r12.xz * float2(0.5,0.5) + -r9.xy;
    r12.xy = float2(1,1) + -r9.xy;
    r12.zw = min(float2(0,0), r9.xy);
    r12.zw = -r12.zw * r12.zw + r12.xy;
    r9.xy = max(float2(0,0), r9.xy);
    r9.xy = -r9.xy * r9.xy + r11.yw;
    r12.zw = float2(1,1) + r12.zw;
    r9.xy = float2(1,1) + r9.xy;
    r13.xy = float2(0.159999996,0.159999996) * r11.xz;
    r14.xy = float2(0.159999996,0.159999996) * r12.xy;
    r12.xy = float2(0.159999996,0.159999996) * r12.zw;
    r15.xy = float2(0.159999996,0.159999996) * r9.xy;
    r9.xy = float2(0.159999996,0.159999996) * r11.yw;
    r13.z = r12.x;
    r13.w = r9.x;
    r14.z = r15.x;
    r14.w = r10.z;
    r11.xyzw = r14.zwxz + r13.zwxz;
    r12.z = r13.y;
    r12.w = r9.y;
    r15.z = r14.y;
    r15.w = r10.w;
    r9.xyw = r15.zyw + r12.zyw;
    r12.xyz = r14.xzw / r11.zwy;
    r12.xyz = float3(-2.5,-0.5,1.5) + r12.xyz;
    r13.xyz = r15.zyw / r9.xyw;
    r13.xyz = float3(-2.5,-0.5,1.5) + r13.xyz;
    r12.xyz = cb4[25].xxx * r12.yxz;
    r13.xyz = cb4[25].yyy * r13.xyz;
    r12.w = r13.x;
    r14.xyzw = r10.xyxy * cb4[25].xyxy + r12.ywxw;
    r10.zw = r10.xy * cb4[25].xy + r12.zw;
    r13.w = r12.y;
    r12.yw = r13.yz;
    r15.xyzw = r10.xyxy * cb4[25].xyxy + r12.xyzy;
    r13.xyzw = r10.xyxy * cb4[25].xyxy + r13.wywz;
    r12.xyzw = r10.xyxy * cb4[25].xyxy + r12.xwzw;
    r16.xyzw = r11.zwyz * r9.xxxy;
    r17.xyzw = r11.xyzw * r9.yyww;
    r9.x = r11.y * r9.w;
    r9.y = cb2[0].w + r9.z;
    r9.w = t5.SampleCmpLevelZero(s5_s, r14.xy, r9.y).x;
    r10.x = t5.SampleCmpLevelZero(s5_s, r14.zw, r9.y).x;
    r10.x = r16.y * r10.x;
    r9.w = r16.x * r9.w + r10.x;
    r10.x = t5.SampleCmpLevelZero(s5_s, r10.zw, r9.y).x;
    r9.w = r16.z * r10.x + r9.w;
    r10.x = t5.SampleCmpLevelZero(s5_s, r13.xy, r9.y).x;
    r9.w = r16.w * r10.x + r9.w;
    r10.x = t5.SampleCmpLevelZero(s5_s, r15.xy, r9.y).x;
    r9.w = r17.x * r10.x + r9.w;
    r10.x = t5.SampleCmpLevelZero(s5_s, r15.zw, r9.y).x;
    r9.w = r17.y * r10.x + r9.w;
    r10.x = t5.SampleCmpLevelZero(s5_s, r13.zw, r9.y).x;
    r9.w = r17.z * r10.x + r9.w;
    r10.x = t5.SampleCmpLevelZero(s5_s, r12.xy, r9.y).x;
    r9.w = r17.w * r10.x + r9.w;
    r9.y = t5.SampleCmpLevelZero(s5_s, r12.zw, r9.y).x;
    r9.x = r9.x * r9.y + r9.w;
    r9.y = 1 + -cb4[32].x;
    r9.x = r9.x * cb4[32].x + r9.y;
    r9.y = cmp(cb2[25].x == 0.000000);
    r9.w = cmp(0 >= r9.z);
    r9.z = cmp(r9.z >= 1);
    r9.z = (int)r9.z | (int)r9.w;
    r9.y = (int)r9.z | (int)r9.y;
    r9.x = min(1, r9.x);
    r9.x = r9.y ? 1 : r9.x;
  } else {
    r9.x = 1;
  }
  r9.y = dot(-r2.xyz, r4.xyz);
  r9.y = r9.y + r9.y;
  r9.yzw = r4.xyz * -r9.yyy + -r2.xyz;
  r10.x = saturate(dot(r4.xyz, r2.xyz));
  r10.x = 1 + -r10.x;
  r10.x = r10.x * r10.x;
  r10.x = r10.x * r10.x;
  r10.y = cb2[15].y * r10.x;
  r6.xyz = r6.xyz * r0.zzz;
  r10.z = -r3.w * 0.699999988 + 1.70000005;
  r10.z = r10.z * r3.w;
  r10.z = 6 * r10.z;
  r10.w = cmp(0 < cb1[34].w);
  r11.x = cmp(0 != cb2[2].w);
  r11.yzw = cmp(float3(0,0,0) < r9.yzw);
  r11.yzw = r11.yzw ? cb1[32].xyz : cb1[33].xyz;
  r11.yzw = -v2.xyz + r11.yzw;
  r12.xyz = -cb1[34].xyz + r11.yzw;
  r12.xyz = cb2[2].xyz + r12.xyz;
  r12.xyz = r12.xyz / r9.yzw;
  r12.x = min(r12.x, r12.y);
  r12.x = min(r12.x, r12.z);
  r12.yzw = -cb2[2].xyz + v2.xyz;
  r12.xyz = r9.yzw * r12.xxx + r12.yzw;
  r11.yzw = r11.yzw / r9.yzw;
  r11.y = min(r11.y, r11.z);
  r11.y = min(r11.y, r11.w);
  r13.xyz = -cb1[34].xyz + v2.xyz;
  r11.yzw = r9.yzw * r11.yyy + r13.xyz;
  r11.xyz = r11.xxx ? r12.xyz : r11.yzw;
  r9.yzw = r10.www ? r11.xyz : r9.yzw;
  r11.xyzw = t0.SampleLevel(s0_s, r9.yzw, r10.z).xyzw;
  r9.y = -1 + r11.w;
  r9.y = cb1[30].w * r9.y + 1;
  r9.y = max(0, r9.y);
  r9.y = log2(r9.y);
  r9.y = cb1[30].y * r9.y;
  r9.y = exp2(r9.y);
  r9.y = cb1[30].x * r9.y;
  r9.yzw = r9.yyy * r11.xyz;
  r9.yzw = r9.yzw * r0.zzz;
  r9.yzw = cb2[13].www * r9.yzw;
  r10.z = cb2[11].z + -cb2[11].y;
  r1.w = r1.w * r10.z + cb2[11].y;
  r10.z = 1 + -r1.w;
  r1.w = r3.w * r10.z + r1.w;
  r3.w = 1 + -cb3[11].w;
  r3.w = r9.x * r3.w + cb3[11].w;
  r1.w = r3.w * r1.w;
  r9.yzw = r9.yzw * r1.www;
  r6.xyz = r6.xyz * r3.xyz;
  r6.xyz = cb2[10].www * r6.xyz;
  r6.xyz = cb3[6].xyz * r6.xyz;
  r9.yzw = cb3[7].xyz * r9.yzw;
  r10.zw = r5.ww * r5.ww + float2(-1,1);
  r1.w = 1 / r10.w;
  r11.xyz = r0.yyy + -r1.xyz;
  r11.xyz = r10.yyy * r11.xyz + r1.xyz;
  r11.xyz = r11.xyz * r1.www;
  r9.yzw = r9.yzw * r11.xyz + r6.xyz;
  r0.y = cb1[26].z * r9.x;
  r1.w = saturate(dot(r4.xyz, r8.yzw));
  r1.w = saturate(cb3[13].w * r1.w);
  r0.y = r1.w * r0.y;
  r11.xyz = cb0[6].xyz * r0.yyy;
  r0.y = -1 + r0.z;
  r0.y = cb2[15].x * r0.y + 1;
  r11.xyz = r11.xyz * r0.yyy;
  r3.xyz = cb2[11].xxx * r3.xyz;
  r2.xyz = r7.yzw * r8.xxx + r2.xyz;
  r0.y = dot(r2.xyz, r2.xyz);
  r0.y = max(1.17549435e-38, r0.y);
  r0.y = rsqrt(r0.y);
  r2.xyz = r2.xyz * r0.yyy;
  r0.y = saturate(dot(r8.yzw, r2.xyz));
  r1.w = saturate(dot(r4.xyz, r2.xyz));
  r1.w = r1.w * r1.w;
  r1.w = r1.w * r10.z + 1.00399995;
  r0.y = r0.y * r0.y;
  r1.w = r1.w * r1.w;
  r0.y = max(0.100000001, r0.y);
  r0.y = r1.w * r0.y;
  r0.y = r0.y * r7.x;
  r0.y = r6.w / r0.y;
  r2.xyz = cb3[9].xyz * r0.yyy;
  r2.xyz = cb3[12].www * r2.xyz;
  r2.xyz = r2.xyz * r1.xyz;
  r2.xyz = cb2[13].zzz * r2.xyz;
  r2.xyz = r3.xyz * cb3[8].xyz + r2.xyz;
  r2.xyz = r2.xyz * r11.xyz + r9.yzw;
  if (cb0[174].z != 0) {
    r3.xyz = float3(0.03125,0.03125,0.03125) * cb0[173].xyz;
    r7.yzw = -cb0[172].xyz + v2.xyz;
    r3.xyz = r7.yzw / r3.xyz;
    r3.xz = (int2)r3.xz;
    r0.y = (uint)r3.z << 5;
    r0.y = (int)r3.x + (int)r0.y;
    r0.y = (int)r0.y;
    r8.x = r0.y * 0.0009765625 + 0.00048828125;
    r0.y = trunc(r3.y);
    r8.y = r0.y * 0.03125 + 0.015625;
    r3.xyzw = t8.SampleLevel(s8_s, r8.xy, 0).xyzw;
    r0.y = cb0[174].x * cb0[174].x + -1;
    r1.w = 0.5 * cb0[174].y;
    r8.yw = float2(1,1);
    r7.yzw = float3(0,0,0);
    r5.w = cb0[174].w;
    r6.y = 0;
    while (true) {
      r6.z = cmp((int)r6.y >= 4);
      if (r6.z != 0) break;
      r6.z = cmp((int)r5.w == 1);
      if (r6.z != 0) {
        r9.x = dot(r3.xyzw, icb[r6.y+0].xyzw);
        r9.y = cmp(r9.x != 1.000000);
        if (r9.y != 0) {
          r9.x = r9.x * r0.y + 0.5;
          r9.x = (int)r9.x;
          r9.x = (uint)r9.x;
          r9.z = cb0[174].x * r9.x;
          r9.z = cmp(r9.z >= -r9.z);
          r9.z = r9.z ? cb0[174].x : -cb0[174].x;
          r9.w = 1 / r9.z;
          r9.w = r9.x * r9.w;
          r9.w = frac(r9.w);
          r9.z = r9.z * r9.w;
          r9.x = r9.x / cb0[174].x;
          r9.xz = trunc(r9.xz);
          r11.x = r9.z * cb0[174].y + r1.w;
          r11.y = r9.x * cb0[174].y + r1.w;
          r12.xyzw = t7.SampleLevel(s7_s, r11.xy, 0).xyzw;
          r13.xyzw = t9.SampleLevel(s7_s, r11.xy, 0).xyzw;
          r14.xyzw = t10.SampleLevel(s7_s, r11.xy, 0).xyzw;
          r11.xyzw = t11.SampleLevel(s7_s, r11.xy, 0).xyzw;
          r9.xzw = -v2.xyz + r13.xyz;
          r10.y = dot(r9.xzw, r9.xzw);
          r10.y = max(6.10351562e-05, r10.y);
          r10.w = rsqrt(r10.y);
          r9.xzw = r10.www * r9.xzw;
          r10.w = 1 / r10.y;
          r10.y = r10.y * r13.w;
          r10.y = -r10.y * r10.y + 1;
          r10.y = max(0, r10.y);
          r10.y = r10.y * r10.y;
          r10.y = r10.w * r10.y;
          r10.w = dot(r14.xyz, r9.xzw);
          r10.w = saturate(r10.w * r11.z + r11.w);
          r10.w = r10.w * r10.w;
          r8.x = r10.y * r10.w;
          r10.yw = r8.xy;
        } else {
          r9.xzw = float3(0,0,0);
          r12.xyz = float3(0,0,0);
          r10.yw = float2(0,0);
        }
      } else {
        r9.xzw = float3(0,0,0);
        r12.xyz = float3(0,0,0);
        r10.yw = float2(0,0);
      }
      if (r6.z == 0) {
        r6.z = dot(r3.xyzw, icb[r6.y+0].xyzw);
        r8.x = cmp(r6.z != 1.000000);
        if (r8.x != 0) {
          r6.z = r6.z * r0.y + 0.5;
          r6.z = (int)r6.z;
          r6.z = (uint)r6.z;
          r8.x = cb0[174].x * r6.z;
          r8.x = cmp(r8.x >= -r8.x);
          r8.x = r8.x ? cb0[174].x : -cb0[174].x;
          r11.x = 1 / r8.x;
          r11.x = r11.x * r6.z;
          r11.x = frac(r11.x);
          r8.x = r11.x * r8.x;
          r6.z = r6.z / cb0[174].x;
          r8.x = trunc(r8.x);
          r11.x = r8.x * cb0[174].y + r1.w;
          r6.z = trunc(r6.z);
          r11.y = r6.z * cb0[174].y + r1.w;
          r12.xyzw = t7.SampleLevel(s7_s, r11.xy, 0).xyzw;
          r11.xyzw = t9.SampleLevel(s7_s, r11.xy, 0).xyzw;
          r11.xyz = -v2.xyz + r11.xyz;
          r6.z = dot(r11.xyz, r11.xyz);
          r6.z = max(6.10351562e-05, r6.z);
          r8.x = rsqrt(r6.z);
          r9.xzw = r11.xyz * r8.xxx;
          r8.x = 1 / r6.z;
          r6.z = r6.z * r11.w;
          r6.z = -r6.z * r6.z + 1;
          r6.z = max(0, r6.z);
          r6.z = r6.z * r6.z;
          r8.z = r8.x * r6.z;
          r10.yw = r8.zw;
          r9.y = -1;
        } else {
          r9.y = 0;
        }
      }
      if (r9.y != 0) {
        r6.z = r10.y * r10.w;
        r8.x = saturate(dot(r4.xyz, r9.xzw));
        r8.x = saturate(cb3[13].w * r8.x);
        r6.z = r8.x * r6.z;
        r11.xyz = r12.xyz * r6.zzz;
        r12.xyz = v5.xyz * r0.xxx + r9.xzw;
        r6.z = dot(r12.xyz, r12.xyz);
        r6.z = max(1.17549435e-38, r6.z);
        r6.z = rsqrt(r6.z);
        r12.xyz = r12.xyz * r6.zzz;
        r6.z = saturate(dot(r9.xzw, r12.xyz));
        r8.x = saturate(dot(r4.xyz, r12.xyz));
        r8.x = r8.x * r8.x;
        r8.x = r8.x * r10.z + 1.00399995;
        r6.z = r6.z * r6.z;
        r8.x = r8.x * r8.x;
        r6.z = max(0.100000001, r6.z);
        r6.z = r8.x * r6.z;
        r6.z = r6.z * r7.x;
        r6.z = r6.w / r6.z;
        r9.xyz = cb3[9].xyz * r6.zzz;
        r9.xyz = cb3[12].www * r9.xyz;
        r9.xyz = r9.xyz * r1.xyz;
        r9.xyz = cb2[13].zzz * r9.xyz;
        r7.yzw = r9.xyz * r11.xyz + r7.yzw;
      } else {
        break;
      }
      r6.y = (int)r6.y + 1;
    }
    r1.yzw = max(float3(0,0,0), r7.yzw);
    r1.yzw = min(float3(3,3,3), r1.yzw);
  } else {
    r1.yzw = float3(0,0,0);
  }
  r1.yzw = r2.xyz + r1.yzw;
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
  r0.y = cb3[15].w * cb2[11].w;
  o1.y = 0.5 * r0.y;
  o1.w = saturate(cb2[12].x + r0.x);
  r0.y = -r2.w * cb2[21].z + 1;
  r0.yz = r0.yw * r0.yz;
  r0.y = max(0.0078125, r0.y);
  r1.y = saturate(r2.w * cb2[21].z + r4.w);
  r0.y = r0.y * r0.y + 1;
  r0.y = 1 / r0.y;
  r0.w = r1.y + -r1.x;
  r0.w = r10.x * r0.w + r1.x;
  r0.y = r0.y * r0.w;
  r0.y = cb3[7].x * r0.y + r6.x;
  r0.y = r0.z * r0.y;
  o1.z = cb2[0].x * r0.y;
  o0.w = r0.x;
  return;
}