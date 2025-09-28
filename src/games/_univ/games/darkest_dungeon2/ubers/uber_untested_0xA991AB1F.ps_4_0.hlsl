#include "../custom.hlsl"

// ---- Created with 3Dmigoto v1.4.1 on Sat Sep 27 18:19:13 2025
Texture2D<float4> t4 : register(t4);

Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[146];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4,r5,r6,r7;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xy = float2(-0.5,-0.5) + v1.xy;
  r0.zw = r0.xy * cb0[139].zz + float2(0.5,0.5);
  r0.xy = r0.xy * cb0[139].zz + -cb0[138].xy;
  r0.xy = cb0[138].zw * r0.xy;
  r1.x = dot(r0.xy, r0.xy);
  r1.x = sqrt(r1.x);
  r1.y = cmp(0 < cb0[139].w);
  if (r1.y != 0) {
    r1.zw = cb0[139].xy * r1.xx;
    sincos(r1.z, r2.x, r3.x);
    r1.z = r2.x / r3.x;
    r1.w = 1 / r1.w;
    r1.z = r1.z * r1.w + -1;
    r1.zw = r0.xy * r1.zz + r0.zw;
  } else {
    r2.x = 1 / r1.x;
    r2.x = cb0[139].x * r2.x;
    r1.x = cb0[139].y * r1.x;
    r2.y = min(1, abs(r1.x));
    r2.z = max(1, abs(r1.x));
    r2.z = 1 / r2.z;
    r2.y = r2.y * r2.z;
    r2.z = r2.y * r2.y;
    r2.w = r2.z * 0.0208350997 + -0.0851330012;
    r2.w = r2.z * r2.w + 0.180141002;
    r2.w = r2.z * r2.w + -0.330299497;
    r2.z = r2.z * r2.w + 0.999866009;
    r2.w = r2.y * r2.z;
    r3.x = cmp(1 < abs(r1.x));
    r2.w = r2.w * -2 + 1.57079637;
    r2.w = r3.x ? r2.w : 0;
    r2.y = r2.y * r2.z + r2.w;
    r1.x = min(1, r1.x);
    r1.x = cmp(r1.x < -r1.x);
    r1.x = r1.x ? -r2.y : r2.y;
    r1.x = r2.x * r1.x + -1;
    r1.zw = r0.xy * r1.xx + r0.zw;
  }
  r0.xyzw = v1.xyxy * float4(2,2,2,2) + float4(-1,-1,-1,-1);
  r1.x = dot(r0.zw, r0.zw);
  r0.xyzw = r1.xxxx * r0.xyzw;
  r0.xyzw = cb0[140].xxxx * r0.xyzw;
  r2.xyzw = t0.SampleBias(s0_s, r1.zw, cb0[5].x).xyzw;
  r0.xyzw = r0.xyzw * float4(-0.333333343,-0.333333343,-0.666666687,-0.666666687) + v1.xyxy;
  r0.xyzw = float4(-0.5,-0.5,-0.5,-0.5) + r0.xyzw;
  r3.xyzw = r0.xyzw * cb0[139].zzzz + float4(0.5,0.5,0.5,0.5);
  r0.xyzw = r0.xyzw * cb0[139].zzzz + -cb0[138].xyxy;
  r0.xyzw = cb0[138].zwzw * r0.xyzw;
  r1.x = dot(r0.xy, r0.xy);
  r1.x = sqrt(r1.x);
  if (r1.y != 0) {
    r2.yz = cb0[139].xy * r1.xx;
    sincos(r2.y, r4.x, r5.x);
    r2.y = r4.x / r5.x;
    r2.z = 1 / r2.z;
    r2.y = r2.y * r2.z + -1;
    r2.yz = r0.xy * r2.yy + r3.xy;
  } else {
    r2.w = 1 / r1.x;
    r2.w = cb0[139].x * r2.w;
    r1.x = cb0[139].y * r1.x;
    r4.x = min(1, abs(r1.x));
    r4.y = max(1, abs(r1.x));
    r4.y = 1 / r4.y;
    r4.x = r4.x * r4.y;
    r4.y = r4.x * r4.x;
    r4.z = r4.y * 0.0208350997 + -0.0851330012;
    r4.z = r4.y * r4.z + 0.180141002;
    r4.z = r4.y * r4.z + -0.330299497;
    r4.y = r4.y * r4.z + 0.999866009;
    r4.z = r4.x * r4.y;
    r4.w = cmp(1 < abs(r1.x));
    r4.z = r4.z * -2 + 1.57079637;
    r4.z = r4.w ? r4.z : 0;
    r4.x = r4.x * r4.y + r4.z;
    r1.x = min(1, r1.x);
    r1.x = cmp(r1.x < -r1.x);
    r1.x = r1.x ? -r4.x : r4.x;
    r1.x = r2.w * r1.x + -1;
    r2.yz = r0.xy * r1.xx + r3.xy;
  }
  r4.xyzw = t0.SampleBias(s0_s, r2.yz, cb0[5].x).xyzw;
  r0.x = dot(r0.zw, r0.zw);
  r0.x = sqrt(r0.x);
  if (r1.y != 0) {
    r1.xy = cb0[139].xy * r0.xx;
    sincos(r1.x, r1.x, r3.x);
    r0.y = r1.x / r3.x;
    r1.x = 1 / r1.y;
    r0.y = r0.y * r1.x + -1;
    r1.xy = r0.zw * r0.yy + r3.zw;
  } else {
    r0.y = 1 / r0.x;
    r0.y = cb0[139].x * r0.y;
    r0.x = cb0[139].y * r0.x;
    r2.y = min(1, abs(r0.x));
    r2.z = max(1, abs(r0.x));
    r2.z = 1 / r2.z;
    r2.y = r2.y * r2.z;
    r2.z = r2.y * r2.y;
    r2.w = r2.z * 0.0208350997 + -0.0851330012;
    r2.w = r2.z * r2.w + 0.180141002;
    r2.w = r2.z * r2.w + -0.330299497;
    r2.z = r2.z * r2.w + 0.999866009;
    r2.w = r2.y * r2.z;
    r3.x = cmp(1 < abs(r0.x));
    r2.w = r2.w * -2 + 1.57079637;
    r2.w = r3.x ? r2.w : 0;
    r2.y = r2.y * r2.z + r2.w;
    r0.x = min(1, r0.x);
    r0.x = cmp(r0.x < -r0.x);
    r0.x = r0.x ? -r2.y : r2.y;
    r0.x = r0.y * r0.x + -1;
    r1.xy = r0.zw * r0.xx + r3.zw;
  }
  r0.xyzw = t0.SampleBias(s0_s, r1.xy, cb0[5].x).xyzw;
  r1.xy = r1.zw * cb0[145].zw + float2(0.5,0.5);
  r2.yz = floor(r1.xy);
  r1.xy = frac(r1.xy);
  r3.xyzw = -r1.xyxy * float4(0.5,0.5,0.166666672,0.166666672) + float4(0.5,0.5,0.5,0.5);
  r3.xyzw = r1.xyxy * r3.xyzw + float4(0.5,0.5,-0.5,-0.5);
  r4.xz = r1.xy * float2(0.5,0.5) + float2(-1,-1);
  r5.xy = r1.xy * r1.xy;
  r4.xz = r5.xy * r4.xz + float2(0.666666687,0.666666687);
  r3.xyzw = r1.xyxy * r3.xyzw + float4(0.166666672,0.166666672,0.166666672,0.166666672);
  r1.xy = float2(1,1) + -r4.xz;
  r1.xy = r1.xy + -r3.xy;
  r1.xy = r1.xy + -r3.zw;
  r3.zw = r3.zw + r4.xz;
  r3.xy = r3.xy + r1.xy;
  r5.xy = float2(1,1) / r3.zw;
  r5.zw = r4.xz * r5.xy + float2(-1,-1);
  r4.xz = float2(1,1) / r3.xy;
  r5.xy = r1.xy * r4.xz + float2(1,1);
  r6.xyzw = r5.zwxw + r2.yzyz;
  r6.xyzw = float4(-0.5,-0.5,-0.5,-0.5) + r6.xyzw;
  r6.xyzw = cb0[145].xyxy * r6.xyzw;
  r6.xyzw = min(float4(1,1,1,1), r6.xyzw);
  r7.xyzw = t1.SampleLevel(s0_s, r6.xy, 0).xyzw;
  r6.xyzw = t1.SampleLevel(s0_s, r6.zw, 0).xyzw;
  r6.xyzw = r6.xyzw * r3.xxxx;
  r6.xyzw = r3.zzzz * r7.xyzw + r6.xyzw;
  r5.xyzw = r5.zyxy + r2.yzyz;
  r5.xyzw = float4(-0.5,-0.5,-0.5,-0.5) + r5.xyzw;
  r5.xyzw = cb0[145].xyxy * r5.xyzw;
  r5.xyzw = min(float4(1,1,1,1), r5.xyzw);
  r7.xyzw = t1.SampleLevel(s0_s, r5.xy, 0).xyzw;
  r5.xyzw = t1.SampleLevel(s0_s, r5.zw, 0).xyzw;
  r5.xyzw = r5.xyzw * r3.xxxx;
  r5.xyzw = r3.zzzz * r7.xyzw + r5.xyzw;
  r5.xyzw = r5.xyzw * r3.yyyy;
  r3.xyzw = r3.wwww * r6.xyzw + r5.xyzw;
  r0.w = cmp(0 < cb0[135].x);
  if (r0.w != 0) {
    r2.yzw = r3.xyz * r3.www;
    r3.xyz = float3(8,8,8) * r2.yzw;
  }
  r2.yzw = cb0[134].xxx * r3.xyz;
  r0.x = r2.x;
  r0.y = r4.y;
  r0.xyz = r2.yzw * cb0[134].yzw + r0.xyz;
  r0.w = cmp(0 < cb0[142].z);
  if (r0.w != 0) {
    r1.xy = -cb0[142].xy + r1.zw;
    r1.yz = cb0[142].zz * abs(r1.xy);
    r1.x = cb0[141].w * r1.y;
    r0.w = dot(r1.xz, r1.xz);
    r0.w = 1 + -r0.w;
    r0.w = max(0, r0.w);
    r0.w = log2(r0.w);
    r0.w = cb0[142].w * r0.w;
    r0.w = exp2(r0.w);
    r1.xyz = float3(1,1,1) + -cb0[141].xyz;
    r1.xyz = r0.www * r1.xyz + cb0[141].xyz;
    r0.xyz = r1.xyz * r0.xyz;
  }
  if (RENODX_TONE_MAP_TYPE != 0.f) {
      // before tone mapping patched
      r0.xyz = UnityLookupARIDPQ(r0.xyzw, v1.xy, cb0[132], s0_s, t3); // t3 from first SampleLevel
      r0.xyz = UnityLookupSDRsRGB(r0.xyz, v1.xy, cb0[133].w, t4); // t4 from first SampleLevel
      r0.xyz = debug_mode(r0.xyzw, v1.xy, 0.02f).xyz;

      // after tone mapping patched
      r1.xy = v1.xy * cb0[144].xy + cb0[144].zw;
      r1.xyzw = t1.SampleBias(s1_s, r1.xy, cb0[5].x).xyzw;
      r0.w = -0.5 + r1.w;
      r0.w = r0.w + r0.w;
      r1.x = dot(r0.xyz, float3(0.212672904, 0.715152204, 0.0721750036));
      r1.x = sqrt(r1.x);
      r1.x = cb0[143].y * -r1.x + 1;
      r1.yzw = r0.xyz * r0.www;
      r1.yzw = cb0[143].xxx * r1.yzw;
      // return result patched
      o0.xyz = r1.yzw * r1.xxx + r0.xyz;
      o0.w = 1;
      return;
  }

  // before tone mapping unpatched
  r0.xyz = cb0[132].www * r0.zxy;
  r0.xyz = r0.xyz * float3(5.55555582,5.55555582,5.55555582) + float3(0.0479959995,0.0479959995,0.0479959995);
  r0.xyz = max(float3(0,0,0), r0.xyz);
  r0.xyz = log2(r0.xyz);
  r0.xyz = saturate(r0.xyz * float3(0.0734997839,0.0734997839,0.0734997839) + float3(0.386036009,0.386036009,0.386036009));
  r0.yzw = cb0[132].zzz * r0.xyz;
  r0.y = floor(r0.y);
  r1.xy = float2(0.5,0.5) * cb0[132].xy;
  r1.yz = r0.zw * cb0[132].xy + r1.xy;
  r1.x = r0.y * cb0[132].y + r1.y;
  // t3 from here
  r2.xyzw = t3.SampleLevel(s0_s, r1.xz, 0).xyzw;
  r3.x = cb0[132].y;
  r3.y = 0;
  r0.zw = r3.xy + r1.xz;
  r1.xyzw = t3.SampleLevel(s0_s, r0.zw, 0).xyzw;
  r0.x = r0.x * cb0[132].z + -r0.y;
  r0.yzw = r1.xyz + -r2.xyz;
  r0.xyz = r0.xxx * r0.yzw + r2.xyz;
  r0.w = cmp(0 < cb0[133].w);
  if (r0.w != 0) {
    r0.xyz = saturate(r0.xyz);
    r1.xyz = cmp(float3(0.00313080009,0.00313080009,0.00313080009) >= r0.xyz);
    r2.xyz = float3(12.9232101,12.9232101,12.9232101) * r0.xyz;
    r3.xyz = log2(r0.xyz);
    r3.xyz = float3(0.416666657,0.416666657,0.416666657) * r3.xyz;
    r3.xyz = exp2(r3.xyz);
    r3.xyz = r3.xyz * float3(1.05499995,1.05499995,1.05499995) + float3(-0.0549999997,-0.0549999997,-0.0549999997);
    r1.xyz = r1.xyz ? r2.xyz : r3.xyz;
    r2.xyz = cb0[133].zzz * r1.zxy;
    r0.w = floor(r2.x);
    r2.xw = float2(0.5,0.5) * cb0[133].xy;
    r2.yz = r2.yz * cb0[133].xy + r2.xw;
    r2.x = r0.w * cb0[133].y + r2.y;
    // t4 from here
    r3.xyzw = t4.SampleLevel(s0_s, r2.xz, 0).xyzw;
    r4.x = cb0[133].y;
    r4.y = 0;
    r2.xy = r4.xy + r2.xz;
    r2.xyzw = t4.SampleLevel(s0_s, r2.xy, 0).xyzw;
    r0.w = r1.z * cb0[133].z + -r0.w;
    r2.xyz = r2.xyz + -r3.xyz;
    r2.xyz = r0.www * r2.xyz + r3.xyz;
    r2.xyz = r2.xyz + -r1.xyz;
    r1.xyz = cb0[133].www * r2.xyz + r1.xyz;
    r2.xyz = float3(0.0773993805,0.0773993805,0.0773993805) * r1.xyz;
    r3.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r1.xyz;
    r3.xyz = float3(0.947867334,0.947867334,0.947867334) * r3.xyz;
    r3.xyz = log2(abs(r3.xyz));
    r3.xyz = float3(2.4000001,2.4000001,2.4000001) * r3.xyz;
    r3.xyz = exp2(r3.xyz);
    r1.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r1.xyz);
    r0.xyz = r1.xyz ? r2.xyz : r3.xyz;
  }
  // after tone mapping unpatched
  r1.xy = v1.xy * cb0[144].xy + cb0[144].zw;
  r1.xyzw = t1.SampleBias(s1_s, r1.xy, cb0[5].x).xyzw;
  r0.w = -0.5 + r1.w;
  r0.w = r0.w + r0.w;
  r1.x = dot(r0.xyz, float3(0.212672904,0.715152204,0.0721750036));
  r1.x = sqrt(r1.x);
  r1.x = cb0[143].y * -r1.x + 1;
  r1.yzw = r0.xyz * r0.www;
  r1.yzw = cb0[143].xxx * r1.yzw;
  // return result unpatched
  o0.xyz = r1.yzw * r1.xxx + r0.xyz;
  o0.w = 1;
  return;
}