// plog bg half blend


// ---- Created with 3Dmigoto v1.4.1 on Wed Jul 16 14:22:33 2025
Texture2D<float4> t3 : register(t3);

Texture2D<float4> t2 : register(t2);

Texture2D<float4> t1 : register(t1); // background

Texture2D<float4> t0 : register(t0); // fog

SamplerState s3_s : register(s3);

SamplerState s2_s : register(s2);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb1 : register(b1)
{
  float4 cb1[7];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[7];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  float2 v2 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xy = v2.xy * float2(2,2) + float2(-1,-1);
  r0.xy = r0.xy * float2(0.400000006,0.400000006) + float2(0.5,0.5);
  r0.z = 0.00300000003 * cb0[6].z;
  r0.zw = v2.xy * float2(0.0500000007,0.0500000007) + r0.zz;
  r1.xyzw = t0.Sample(s3_s, r0.zw).xyzw;
  r0.z = r1.x * 0.150000006 + -0.0219999999;
  r0.xy = r0.zz * float2(1,0.200000003) + r0.xy;
  r0.xy = -v2.xy + r0.xy;
  r0.xy = cb0[6].zz * r0.xy + v2.xy;
  r1.xyzw = t1.Sample(s0_s, r0.xy).xyzw;
  r1.xyzw = saturate(r1.xyzw);  // xxx
  r2.xyzw = t2.Sample(s2_s, r0.xy).xyzw;
  r2.xyzw = saturate(r2.xyzw);  // xxx
  r0.z = cb0[4].z / cb0[5].y;
  r0.w = r0.x * r0.z;
  r0.w = cmp(cb0[5].x >= r0.w);
  r2.w = r0.w ? 1.000000 : 0;
  r3.xy = r0.ww ? float2(0,-0) : float2(1,-1);
  r0.w = r3.y + r2.w;
  r0.w = cb0[5].z * r0.w + r3.x;
  r2.xyz = r2.xyz + -r1.xyz;
  r1.xyz = r0.www * r2.xyz + r1.xyz;
  r0.z = r0.x * r0.z + -cb0[5].x;
  r0.w = 1 / cb0[5].y;
  r0.w = 5 * r0.w;
  r0.z = cmp(r0.w >= abs(r0.z));
  r0.w = -0.5 + cb0[5].x;
  r0.w = cmp(0.49000001 >= abs(r0.w));
  r0.zw = r0.zw ? float2(1,1) : 0;
  r1.xyz = r0.zzz * r0.www + r1.xyz;
  r2.y = cb1[6].y / cb1[6].x;
  r2.x = 1;
  r0.zw = r2.xy * r0.xy;
  r2.xyzw = t0.Sample(s3_s, r0.zw).xyzw;
  r2.xyzw = saturate(r2.xyzw);  // xxx
  r0.z = cmp(r2.x >= cb0[6].y);
  r0.z = r0.z ? 1.000000 : 0;
  r2.xyz = r1.xyz * r0.zzz;
  r0.z = cmp(0 >= cb0[5].w);
  if (r0.z != 0) {
    r3.xyzw = t3.Sample(s1_s, r0.xy).xyzw;
    r3.xyzw = saturate(r3.xyzw);  // xxx
    r0.z = r3.x + -r1.w;
    r0.z = cb0[2].x * r0.z + r1.w;
    r1.xy = float2(-0.5,-0.5) + r0.xy;
    r0.w = min(abs(r1.x), abs(r1.y));
    r1.z = max(abs(r1.x), abs(r1.y));
    r1.z = 1 / r1.z;
    r0.w = r1.z * r0.w;
    r1.z = r0.w * r0.w;
    r1.w = r1.z * 0.0208350997 + -0.0851330012;
    r1.w = r1.z * r1.w + 0.180141002;
    r1.w = r1.z * r1.w + -0.330299497;
    r1.z = r1.z * r1.w + 0.999866009;
    r1.w = r1.z * r0.w;
    r3.x = cmp(abs(r1.y) < abs(r1.x));
    r1.w = r1.w * -2 + 1.57079637;
    r1.w = r3.x ? r1.w : 0;
    r0.w = r0.w * r1.z + r1.w;
    r1.z = cmp(r1.y < -r1.y);
    r1.z = r1.z ? -3.141593 : 0;
    r0.w = r1.z + r0.w;
    r1.z = min(r1.x, r1.y);
    r1.x = max(r1.x, r1.y);
    r1.y = cmp(r1.z < -r1.z);
    r1.x = cmp(r1.x >= -r1.x);
    r1.x = r1.x ? r1.y : 0;
    r0.w = r1.x ? -r0.w : r0.w;
    r0.w = 0.159235656 * r0.w;
    r0.w = frac(r0.w);
    r0.w = cmp(r0.w >= cb0[6].x);
    r0.w = r0.w ? 1.000000 : 0;
    r1.xy = cmp(float2(0,0) < cb0[6].xz);
    r1.z = 1 + -cb0[6].z;
    r0.z = r1.y ? r1.z : r0.z;
    r2.w = r1.x ? r0.w : r0.z;
  } else {
    r0.z = cb0[5].w + cb0[5].w;
    r0.x = 1 + r0.x;
    r0.x = r0.x + -r0.y;
    r0.x = cmp(r0.x >= r0.z);
    r2.w = r0.x ? 1.000000 : 0;
  }
  r0.xyzw = v1.xyzw * r2.xyzw;
  o0.xyzw = float4(2, 2, 2, 1) * r0.xyzw;

  o0.w = 1.f;
  o0.xyzw = saturate(o0.xyzw); // xxfixx
  return;
}