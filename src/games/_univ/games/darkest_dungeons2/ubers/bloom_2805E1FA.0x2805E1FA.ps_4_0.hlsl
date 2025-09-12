// ---- Created with 3Dmigoto v1.4.1 on Wed Jul 23 19:39:57 2025
Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[135];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xy = v1.xy * cb0[133].zw + float2(0.5,0.5);
  r0.zw = frac(r0.xy);
  r0.xy = floor(r0.xy);
  r1.xy = r0.zw * float2(0.5,0.5) + float2(-1,-1);
  r1.zw = r0.zw * r0.zw;
  r1.xy = r1.zw * r1.xy + float2(0.666666687,0.666666687);
  r1.zw = float2(1,1) + -r1.xy;
  r2.xyzw = -r0.zwzw * float4(0.5,0.5,0.166666672,0.166666672) + float4(0.5,0.5,0.5,0.5);
  r2.xyzw = r0.zwzw * r2.xyzw + float4(0.5,0.5,-0.5,-0.5);
  r2.xyzw = r0.zwzw * r2.xyzw + float4(0.166666672,0.166666672,0.166666672,0.166666672);
  r0.zw = -r2.xy + r1.zw;
  r0.zw = r0.zw + -r2.zw;
  r1.zw = r2.xy + r0.zw;
  r2.xy = r2.zw + r1.xy;
  r2.zw = float2(1,1) / r1.zw;
  r3.xy = r0.zw * r2.zw + float2(1,1);
  r0.zw = float2(1,1) / r2.xy;
  r3.zw = r1.xy * r0.zw + float2(-1,-1);
  r4.xyzw = r3.zyxy + r0.xyxy;
  r0.xyzw = r3.zwxw + r0.xyxy;
  r0.xyzw = float4(-0.5,-0.5,-0.5,-0.5) + r0.xyzw;
  r0.xyzw = cb0[133].xyxy * r0.xyzw;
  r0.xyzw = min(float4(1,1,1,1), r0.xyzw);
  r3.xyzw = float4(-0.5,-0.5,-0.5,-0.5) + r4.xyzw;
  r3.xyzw = cb0[133].xyxy * r3.xyzw;
  r3.xyzw = min(float4(1,1,1,1), r3.xyzw);
  r4.xyzw = t1.SampleLevel(s0_s, r3.zw, 0).xyzw;
  r3.xyzw = t1.SampleLevel(s0_s, r3.xy, 0).xyzw;
  r4.xyz = r4.xyz * r1.zzz;
  r3.xyz = r2.xxx * r3.xyz + r4.xyz;
  r1.xyw = r3.xyz * r1.www;
  r3.xyzw = t1.SampleLevel(s0_s, r0.zw, 0).xyzw;
  r0.xyzw = t1.SampleLevel(s0_s, r0.xy, 0).xyzw;
  r3.xyz = r3.xyz * r1.zzz;
  r0.xyz = r2.xxx * r0.xyz + r3.xyz;
  r0.xyz = r2.yyy * r0.xyz + r1.xyw;
  r1.xyzw = t0.SampleBias(s0_s, v1.xy, cb0[5].x).xyzw;
  r0.xyz = -r1.xyz + r0.xyz;
  o0.xyz = cb0[134].xxx * r0.xyz + r1.xyz;
  o0.w = 1;
  return;
}