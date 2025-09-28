// ---- Created with 3Dmigoto v1.4.1 on Sat Sep 27 18:19:17 2025
Texture2D<float4> t1 : register(t1);

Texture2D<float4> t0 : register(t0);

SamplerState s1_s : register(s1);

SamplerState s0_s : register(s0);

cbuffer cb1 : register(b1)
{
  float4 cb1[1];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[10];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  float4 v2 : TEXCOORD0,
  float4 v3 : TEXCOORD1,
  float4 v4 : TEXCOORD2,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xy = cb1[0].yy * cb0[7].zw;
  r0.xy = v2.xy * cb0[7].xy + r0.xy;
  r0.xyzw = t1.Sample(s1_s, r0.xy).xyzw;
  r0.yz = cb1[0].yy * cb0[8].zw;
  r0.yz = v2.xy * cb0[8].xy + r0.yz;
  r1.xyzw = t1.Sample(s1_s, r0.yz).xyzw;
  r0.x = r1.x * r0.x;
  r0.x = cb0[9].x * r0.x;
  r0.yz = v2.xy * cb0[5].xy + cb0[5].zw;
  r1.xyzw = t0.Sample(s0_s, r0.yz).xyzw;
  r0.y = r0.x * r1.w + -0.0199999996;
  r0.x = r1.w * r0.x;
  r0.y = saturate(33.3333321 * r0.y);
  r0.z = r0.y * -2 + 3;
  r0.y = r0.y * r0.y;
  r0.y = r0.z * r0.y;
  r0.x = saturate(cb0[6].z * r0.x + r0.y);
  r0.y = 35.7011528 * r0.x;
  r0.y = min(1, r0.y);
  r1.xyz = float3(0.510235012,0.613895416,0.735849023) * r0.yyy;
  r0.yzw = -r0.yyy * float3(0.510235012,0.613895416,0.735849023) + float3(0.414916307,0.585706592,0.792452812);
  r2.xy = float2(-0.0280003008,-0.500007629) + r0.xx;
  r2.xy = float2(2.11856627,1.99999046) * r2.xy;
  r2.x = saturate(r2.x);
  r2.y = max(0, r2.y);
  r0.yzw = r2.xxx * r0.yzw + r1.xyz;
  r0.yzw = r2.yyy * -r0.yzw + r0.yzw;
  r1.xyz = float3(0.0549999997,0.0549999997,0.0549999997) + r0.yzw;
  r1.xyz = float3(0.947867334,0.947867334,0.947867334) * r1.xyz;
  r1.xyz = log2(r1.xyz);
  r1.xyz = float3(2.4000001,2.4000001,2.4000001) * r1.xyz;
  r1.xyz = exp2(r1.xyz);
  r2.xyz = log2(r0.yzw);
  r2.xyz = float3(2.20000005,2.20000005,2.20000005) * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r3.xyz = cmp(r0.yzw < float3(1,1,1));
  r1.xyz = r3.xyz ? r1.xyz : r2.xyz;
  r2.xyz = cmp(float3(0.0404499993,0.0404499993,0.0404499993) >= r0.yzw);
  r0.yzw = float3(0.0773993805,0.0773993805,0.0773993805) * r0.yzw;
  r0.yzw = r2.xyz ? r0.yzw : r1.xyz;
  r0.yzw = v1.xyz * r0.yzw;
  r1.x = 255 * v1.w;
  r1.x = round(r1.x);
  r1.x = 0.00392156886 * r1.x;
  r0.x = r1.x * r0.x;
  r0.x = cb0[9].y * r0.x;
  r0.x = r0.x * r1.w;
  o0.xyz = r0.yzw * r0.xxx;
  o0.w = r0.x;
  return;
}