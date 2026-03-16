// ---- Created with 3Dmigoto v1.4.1 on Thu Jun 26 03:58:59 2025
Texture2D<float4> t4 : register(t4);

Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s4_s : register(s4);

SamplerState s3_s : register(s3);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[44];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  float2 w1 : TEXCOORD1,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4,r5;
  uint4 bitmask, uiDest;
  float4 fDest;

  if (true) {
    
    o0.xyzw = t2.Sample(s1_s, v1.xy).xyzw;
    return;
  }

  r0.xyzw = t4.Sample(s3_s, w1.xy).xyzw;
  r0.y = 1 + -cb0[20].w;
  r0.z = cb0[21].x * r0.x;
  r0.w = -cb0[20].w * r0.z + 1;
  r0.y = r0.y * r0.z + cb0[21].y;
  r0.y = r0.w / r0.y;
  r0.y = cb0[17].z * r0.y;
  r0.z = r0.y + r0.y;
  r0.z = r0.z / cb0[1].y;
  r0.w = cb0[22].x / cb0[22].y;
  r0.w = r0.w * r0.z;
  r1.x = 0.5 * r0.w;
  r0.w = r0.w * v1.x + -r1.x;
  r1.x = 0.5 * r0.z;
  r0.z = r0.z * v1.y + -r1.x;
  r1.xyz = cb0[41].xyz * r0.zzz;
  r1.xyz = cb0[40].xyz * r0.www + r1.xyz;
  r0.yzw = cb0[42].xyz * r0.yyy + r1.xyz;
  r0.yzw = cb0[43].xyz + r0.yzw;
  r1.xyzw = cb0[29].xyzw * r0.zzzz;
  r1.xyzw = cb0[28].xyzw * r0.yyyy + r1.xyzw;
  r1.xyzw = cb0[30].xyzw * r0.wwww + r1.xyzw;
  r1.xyzw = cb0[31].xyzw + r1.xyzw;
  r1.xyz = r1.xyz / r1.www;
  r2.xyzw = t1.Sample(s0_s, r1.xy).xyzw;
  r0.w = r2.x + -r1.z;
  r0.x = r0.x * cb0[21].z + cb0[21].w;
  r0.x = 1 / r0.x;
  r1.x = 1 + -cb0[32].x;
  r0.w = cmp(0.00999999978 >= r0.w);
  r0.w = r0.w ? 1.000000 : 0;
  r0.w = max(r1.x, r0.w);
  r0.w = cb0[35].w * r0.w;
  r1.x = cmp(0 < cb0[33].x);
  r1.y = r0.x / cb0[33].x;
  r1.y = min(1, r1.y);
  r2.xyzw = t2.Sample(s1_s, v1.xy).xyzw;
  r3.xyzw = t3.Sample(s2_s, v1.xy).xyzw;
  r0.w = saturate(r3.z * r0.w + r3.z);
  r0.yz = sin(r0.yz);
  r1.z = dot(r0.yz, float2(15.6370001,76.2429962));
  r1.z = sin(r1.z);
  r1.z = 14375.5967 * r1.z;
  r3.x = frac(r1.z);
  r0.y = dot(r0.yz, float2(45.3660011,23.1679993));
  r0.y = sin(r0.y);
  r0.y = 14684.6035 * r0.y;
  r3.y = frac(r0.y);
  r0.yz = r3.xy * float2(2,2) + float2(-1,-1);
  r1.z = log2(r0.w);
  r1.z = -cb0[33].z * r1.z;
  r1.z = exp2(r1.z);
  r0.yz = r1.zz * r0.yz;
  r1.zw = float2(-1,-1) + cb0[22].zw;
  r0.yz = r0.yz * r1.zw + v1.xy;
  r0.yz = r0.yz / cb0[39].yy;
  r1.z = min(cb0[34].x, r0.x);
  r1.z = r1.z / cb0[34].x;
  r1.z = log2(abs(r1.z));
  r1.z = cb0[34].y * r1.z;
  r1.z = exp2(r1.z);
  r0.x = log2(r0.x);
  r0.x = cb0[33].y * r0.x;
  r0.x = exp2(r0.x);
  r1.z = 1 + -r1.z;
  r1.z = cb0[34].z * r1.z;
  r0.x = r1.z * r0.x + r0.w;
  r0.x = saturate(r0.w * r0.x);
  r0.x = 1 + -r0.x;
  r1.z = -r0.x * 0.699999988 + 1.70000005;
  r0.x = r1.z * r0.x;
  r1.z = 8 * r0.x;
  r3.xyzw = t0.SampleLevel(s4_s, r0.yz, r1.z).xyzw;
  r1.y = log2(r1.y);
  r1.y = cb0[33].y * r1.y;
  r1.y = exp2(r1.y);
  r1.x = r1.x ? r1.y : 1;
  r1.y = r3.x + r3.y;
  r1.y = r1.y + r3.z;
  r1.z = cmp(r1.y < cb0[38].x);
  if (r1.z != 0) {
    r0.x = r0.x * 8 + cb0[38].y;
    r0.x = min(7, r0.x);
    r0.x = max(0, r0.x);
    r4.xyzw = t0.SampleLevel(s4_s, r0.yz, r0.x).xyzw;
    r0.x = cb0[38].x + -r1.y;
    r0.x = r0.x / cb0[38].x;
    r4.xyzw = r4.xyzw + -r3.xyzw;
    r3.xyzw = r0.xxxx * r4.xyzw + r3.xyzw;
  }
  r0.x = cmp(r2.y >= r2.z);
  r0.x = r0.x ? 1.000000 : 0;
  r4.xy = r2.zy;
  r4.zw = float2(-1,0.666666687);
  r5.xy = -r4.xy + r2.yz;
  r5.zw = float2(1,-1);
  r4.xyzw = r0.xxxx * r5.xyzw + r4.xyzw;
  r0.x = cmp(r2.x >= r4.x);
  r0.x = r0.x ? 1.000000 : 0;
  r5.xyz = r4.xyw;
  r5.w = r2.x;
  r4.xyw = r5.wyx;
  r4.xyzw = r4.xyzw + -r5.xyzw;
  r4.xyzw = r0.xxxx * r4.xyzw + r5.xyzw;
  r0.x = min(r4.w, r4.y);
  r0.x = r4.x + -r0.x;
  r0.y = r4.w + -r4.y;
  r0.z = r0.x * 6 + 9.99999975e-05;
  r0.y = r0.y / r0.z;
  r0.y = r4.z + r0.y;
  r0.z = 9.99999975e-05 + r4.x;
  r0.x = r0.x / r0.z;
  r1.yz = cmp(cb0[38].wz != float2(0,0));
  r0.z = 1 + -cb0[38].w;
  r0.z = r0.x * r0.z;
  r0.x = r1.y ? r0.z : r0.x;
  r0.z = r1.z ? cb0[38].z : r4.x;
  r1.yzw = float3(1,0.666666687,0.333333343) + abs(r0.yyy);
  r1.yzw = frac(r1.yzw);
  r1.yzw = r1.yzw * float3(6,6,6) + float3(-3,-3,-3);
  r1.yzw = saturate(float3(-1,-1,-1) + abs(r1.yzw));
  r1.yzw = float3(-1,-1,-1) + r1.yzw;
  r1.yzw = r0.xxx * r1.yzw + float3(1,1,1);
  r4.xyz = r1.yzw * r0.zzz;
  r4.w = 1;
  r3.xyzw = r4.xyzw * r3.xyzw;
  r3.xyzw = cb0[34].wwww * r3.xyzw;
  r1.xyzw = r3.xyzw * r1.xxxx;
  r1.xyzw = cb0[35].yyyy * r1.xyzw;
  r1.xyzw = r1.xyzw * r0.wwww;
  r0.x = 1 + -r0.w;
  r0.x = log2(r0.x);
  r0.x = cb0[35].z * r0.x;
  r0.x = exp2(r0.x);
  r0.xyzw = r1.xyzw * r0.xxxx;
  o0.xyzw = r2.xyzw * cb0[35].xxxx + r0.xyzw;
  return;
}