// ---- Created with 3Dmigoto v1.4.1 on Sun Jul 27 09:42:27 2025
Texture2D<float4> t6 : register(t6);

Texture2D<float4> t5 : register(t5);

Texture3D<float4> t4 : register(t4);

Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s5_s : register(s5);

SamplerState s4_s : register(s4);

SamplerState s3_s : register(s3);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb3 : register(b3)
{
  float4 cb3[18];
}

cbuffer cb2 : register(b2)
{
  float4 cb2[27];
}

cbuffer cb1 : register(b1)
{
  float4 cb1[368];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[5];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_Position0,
  float4 v1 : TEXCOORD0,
  float4 v2 : TEXCOORD1,
  float4 v3 : TEXCOORD2,
  float4 v4 : TEXCOORD3,
  float4 v5 : TEXCOORD4,
  float4 v6 : TEXCOORD5,
  uint v7 : SV_IsFrontFace0,
  out float4 o0 : SV_Target0,
  out float4 o1 : SV_Target1,
  out float4 o2 : SV_Target2,
  out float4 o3 : SV_Target3,
  out float4 o4 : SV_Target4)
{
  float4 r0,r1,r2,r3,r4,r5,r6,r7,r8;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.x = dot(v2.xyz, v2.xyz);
  r0.x = sqrt(r0.x);
  r0.x = max(1.17549435e-38, r0.x);
  r0.x = 1 / r0.x;
  r0.y = cmp(0 < v3.w);
  r0.z = cmp(cb2[9].w >= 0);
  r0.yz = r0.yz ? float2(1,1) : float2(-1,-1);
  r0.y = r0.y * r0.z;
  r1.xyz = v3.yzx * v2.zxy;
  r1.xyz = v2.yzx * v3.zxy + -r1.xyz;
  r0.yzw = r1.xyz * r0.yyy;
  r1.xyz = v3.xyz * r0.xxx;
  r0.yzw = r0.yzw * r0.xxx;
  r2.xyz = v2.xyz * r0.xxx;
  r0.x = cmp(cb1[54].w == 0.000000);
  r3.x = r0.x ? -v1.x : cb1[0].z;
  r3.y = r0.x ? -v1.y : cb1[1].z;
  r3.z = r0.x ? -v1.z : cb1[2].z;
  r0.x = dot(r3.xyz, r3.xyz);
  r0.x = rsqrt(r0.x);
  r3.xyz = r3.xyz * r0.xxx;
  r4.xy = v6.xy * cb3[1].xy + cb3[1].zw;
  r4.xyz = t0.Sample(s3_s, r4.xy).xyz;
  r5.xyz = cb1[44].xyz + v1.xyz;
  r6.xyz = cb0[2].xyz * r5.yyy;
  r5.xyw = cb0[1].xyz * r5.xxx + r6.xyz;
  r5.xyz = cb0[3].xyz * r5.zzz + r5.xyw;
  r5.xyz = cb0[4].xyz + r5.xyz;
  r0.x = dot(r5.xy, r5.xy);
  r0.x = rsqrt(r0.x);
  r5.xy = r5.yx * r0.xx;
  r0.x = min(abs(r5.x), abs(r5.y));
  r1.w = max(abs(r5.x), abs(r5.y));
  r1.w = 1 / r1.w;
  r0.x = r1.w * r0.x;
  r1.w = r0.x * r0.x;
  r2.w = r1.w * 0.0208350997 + -0.0851330012;
  r2.w = r1.w * r2.w + 0.180141002;
  r2.w = r1.w * r2.w + -0.330299497;
  r1.w = r1.w * r2.w + 0.999866009;
  r2.w = r1.w * r0.x;
  r3.w = cmp(abs(r5.y) < abs(r5.x));
  r2.w = r2.w * -2 + 1.57079637;
  r2.w = r3.w ? r2.w : 0;
  r0.x = r0.x * r1.w + r2.w;
  r1.w = cmp(r5.y < -r5.y);
  r1.w = r1.w ? -3.141593 : 0;
  r0.x = r1.w + r0.x;
  r1.w = min(r5.x, r5.y);
  r2.w = max(r5.x, r5.y);
  r1.w = cmp(r1.w < -r1.w);
  r2.w = cmp(r2.w >= -r2.w);
  r1.w = r1.w ? r2.w : 0;
  r0.x = r1.w ? -r0.x : r0.x;
  r0.x = -r0.x * 57.2957802 + cb3[5].x;
  r0.x = 180 + r0.x;
  r1.w = 360 * r0.x;
  r1.w = cmp(r1.w >= -r1.w);
  r5.xy = r1.ww ? float2(360,0.00277777785) : float2(-360,-0.00277777785);
  r0.x = r5.y * r0.x;
  r0.x = frac(r0.x);
  r0.x = r5.x * r0.x;
  r0.x = cb3[4].z * r0.x;
  r1.w = 45 + r5.z;
  r1.w = -r1.w * 0.0111111114 + 1;
  r5.x = 0.00277777785 * r0.x;
  r5.y = cb3[4].w * r1.w;
  r5.xy = cb0[0].xx * cb3[4].xy + r5.xy;
  r0.x = t1.Sample(s2_s, r5.xy).x;
  r5.xy = v6.xy * cb3[2].xy + cb3[2].zw;
  r5.xyz = t2.Sample(s4_s, r5.xy).xyw;
  r5.x = r5.z * r5.x;
  r6.xy = r5.xy + r5.xy;
  r5.xy = r5.xy * float2(2,2) + float2(-1,-1);
  r1.w = dot(r5.xy, r5.xy);
  r1.w = min(1, r1.w);
  r1.w = 1 + -r1.w;
  r1.w = sqrt(r1.w);
  r6.z = max(1.00000002e-16, r1.w);
  r5.xyz = float3(-1,-1,-1) + r6.xyz;
  r5.xyz = cb3[5].www * r5.xyz + float3(0,0,1);
  r6.xy = v6.xy * cb3[3].xy + cb3[3].zw;
  r6.xyzw = t3.Sample(s5_s, r6.xy).xyzw;
  r7.xyz = cb3[7].xyz * r6.zzz;
  r7.xyz = cb3[6].xxx * r7.xyz;
  r8.xyz = max(float3(0,0,0), r7.xyz);
  r7.xyz = min(r8.xyz, r7.xyz);
  r8.x = dot(float2(2,-0.5), r0.xx);
  r8.y = dot(float2(2,-0.5), r0.xx);
  r8.z = dot(float2(2,-0.5), r0.xx);
  r8.xyz = cb3[5].zzz * r8.xyz;
  r4.xyz = cb3[0].xyz * r4.xyz + r8.xyz;
  r8.xy = v7.xx ? float2(1,1) : cb3[17].zx;
  r2.xyz = r8.xxx * r2.xyz;
  r5.xy = r8.yy * r5.xy;
  r0.xyz = r5.yyy * r0.yzw;
  r0.xyz = r5.xxx * r1.xyz + r0.xyz;
  r0.xyz = r5.zzz * r2.xyz + r0.xyz;
  r0.w = dot(r0.xyz, r0.xyz);
  r0.w = max(1.17549435e-38, r0.w);
  r0.w = rsqrt(r0.w);
  r0.xyz = r0.xyz * r0.www;
  r1.x = asint(cb2[10].x) & 255;
  r1.x = cb1[287].w ? r1.x : 255;
  if (cb1[367].x == 0) {
    r1.y = cmp(cb2[20].x == 0.000000);
    if (r1.y != 0) {
      r0.w = 1;
      r2.x = dot(cb2[13].xyzw, r0.xyzw);
      r2.y = dot(cb2[14].xyzw, r0.xyzw);
      r2.z = dot(cb2[15].xyzw, r0.xyzw);
      r5.xyzw = r0.xyzz * r0.yzzx;
      r8.x = dot(cb2[16].xyzw, r5.xyzw);
      r8.y = dot(cb2[17].xyzw, r5.xyzw);
      r8.z = dot(cb2[18].xyzw, r5.xyzw);
      r1.y = r0.y * r0.y;
      r1.y = r0.x * r0.x + -r1.y;
      r1.yzw = cb2[19].xyz * r1.yyy + r8.xyz;
      r1.yzw = r2.xyz + r1.yzw;
    } else {
      r2.xyz = cb2[22].xyz * cb1[44].yyy;
      r2.xyz = cb2[21].xyz * cb1[44].xxx + r2.xyz;
      r2.xyz = cb2[23].xyz * cb1[44].zzz + r2.xyz;
      r2.xyz = cb2[24].xyz + r2.xyz;
      r2.w = cmp(cb2[20].y == 1.000000);
      r5.xyz = cb2[22].xyz * v1.yyy;
      r5.xyz = cb2[21].xyz * v1.xxx + r5.xyz;
      r5.xyz = cb2[23].xyz * v1.zzz + r5.xyz;
      r2.xyz = r5.xyz + r2.xyz;
      r2.xyz = r2.www ? r2.xyz : v1.xyz;
      r2.xyz = -cb2[26].xyz + r2.xyz;
      r2.yzw = cb2[25].xyz * r2.xyz;
      r2.y = 0.25 * r2.y;
      r3.w = 0.5 * cb2[20].z;
      r4.w = -cb2[20].z * 0.5 + 0.25;
      r2.y = max(r3.w, r2.y);
      r2.x = min(r2.y, r4.w);
      r5.xyzw = t4.SampleLevel(s1_s, r2.xzw, 0).xyzw;
      r8.xyz = float3(0.25,0,0) + r2.xzw;
      r8.xyzw = t4.SampleLevel(s1_s, r8.xyz, 0).xyzw;
      r2.xyz = float3(0.5,0,0) + r2.xzw;
      r2.xyzw = t4.SampleLevel(s1_s, r2.xyz, 0).xyzw;
      r0.w = 1;
      r1.y = dot(r5.xyzw, r0.xyzw);
      r1.z = dot(r8.xyzw, r0.xyzw);
      r1.w = dot(r2.xyzw, r0.xyzw);
    }
  } else {
    r1.yzw = float3(0,0,0);
  }
  r2.y = -r6.w * cb3[6].y + 1;
  r0.w = 1 + -r6.x;
  r5.xyz = r4.xyz * r0.www;
  r8.xyz = float3(-0.0399999991,-0.0399999991,-0.0399999991) + r4.xyz;
  r8.xyz = r6.xxx * r8.xyz + float3(0.0399999991,0.0399999991,0.0399999991);
  r2.z = dot(r0.xyz, r3.xyz);
  r2.z = max(9.99999975e-05, r2.z);
  r2.x = sqrt(r2.z);
  r2.xz = r2.xy * float2(0.984375,0.984375) + float2(0.0078125,0.0078125);
  r2.x = t6.SampleLevel(s0_s, r2.xz, 0).z;
  r2.z = max(r8.x, r8.y);
  r2.z = max(r2.z, r8.z);
  r2.xz = float2(0.5,-0.200000003) + r2.xz;
  r2.z = saturate(4 * r2.z);
  r2.z = cb1[289].w * r2.z;
  r3.xyz = -r4.xyz * r0.www + r8.xyz;
  r3.xyz = r2.zzz * r3.xyz + r5.xyz;
  r2.xzw = r3.xyz * r2.xxx;
  r1.yzw = r2.xzw * r1.yzw;
  r0.w = (int)r1.x & asint(cb1[291].y);
  r0.w = r0.w ? cb1[291].x : 1;
  r1.yzw = r1.yzw * r0.www;
  r0.w = max(0.0009765625, abs(r0.z));
  r2.x = cmp(r0.z >= 0);
  r0.z = r2.x ? r0.w : -r0.w;
  r0.w = dot(abs(r0.xyz), float3(1,1,1));
  r0.w = max(9.99999997e-07, r0.w);
  r0.w = rcp(r0.w);
  r2.xzw = r0.zxy * r0.www;
  r2.x = saturate(-r2.x);
  r2.zw = cmp(r2.zw >= float2(0,0));
  r2.xz = r2.zw ? r2.xx : -r2.xx;
  r0.xy = r0.xy * r0.ww + r2.xz;
  r0.xy = saturate(r0.xy * float2(0.5,0.5) + float2(0.5,0.5));
  r0.xy = float2(4095.5,4095.5) * r0.xy;
  r0.xy = (uint2)r0.xy;
  r0.zw = (uint2)r0.xy >> int2(8,8);
  r0.xy = (int2)r0.xy & int2(255,255);
  r0.z = mad((int)r0.w, 16, (int)r0.z);
  r3.xyz = (uint3)r0.xyz;
  o1.xyz = float3(0.00392156886,0.00392156886,0.00392156886) * r3.xyz;
  r0.xyz = log2(abs(r8.xyz));
  r0.xyz = float3(0.416666657,0.416666657,0.416666657) * r0.xyz;
  r0.xyz = exp2(r0.xyz);
  o2.xyz = saturate(r0.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997));
  r0.xyz = r1.yzw * r6.yyy + r7.xyz;
  r0.w = t5.Load(float4(0,0,0,0)).x;
  r0.w = cb1[289].y * r0.w;
  r0.xyz = r0.xyz * r0.www;
  r1.yzw = cmp(r0.xyz == float3(0,0,0));
  r0.w = r1.z ? r1.y : 0;
  r0.w = r1.w ? r0.w : 0;
  r6.xz = float2(2048,1024);
  o3.xyz = r0.www ? r6.xyz : r0.xyz;
  r0.x = (uint)r1.x;
  o4.w = 0.00392156886 * r0.x;
  o0.xyz = r5.xyz;
  o0.w = 1;
  o1.w = r2.y;
  o2.w = 0;
  o3.w = 0;
  o4.xyz = float3(0,0,0);

 // o0 = (1.f, 0.f, 0.f, 1.f);
 // o1 = 0.f;
 //.. o3 = (1.f, 0.f, 0.f, 1.f);
  //.o4 = (1.f, 0.f, 0.f, 1.f);
  return;
}