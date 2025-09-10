#include "../../../custom.hlsl"
#include "../../../shared.h"

// ---- Created with 3Dmigoto v1.4.1 on Sun Jul 27 09:13:35 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb2 : register(b2)
{
  float4 cb2[2];
}

cbuffer cb1 : register(b1)
{
  float4 cb1[37];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[47];
}




// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_Position0,
  float4 v1 : TEXCOORD0,
  float4 v2 : TEXCOORD1,
  float4 v3 : TEXCOORD2,
  float4 v4 : TEXCOORD3,
  uint v5 : SV_IsFrontFace0,
  out float4 o0 : SV_Target0)
{
  float4 r0;
  uint4 bitmask, uiDest;
  float4 fDest;

  if (CUSTOM_GAME_COMPATIBLITY_MODE) {
    // BUG: disabed folliage rendering
    discard;
  }
  

  r0.xy = v4.xy * cb2[1].xy + cb2[1].zw;
  r0.x = t0.Sample(s0_s, r0.xy).w;
  r0.x = -0.5 + r0.x;
  r0.x = cmp(r0.x < 0);
  if (r0.x != 0) discard;
  r0.xy = v2.xy / v2.zz;
  r0.zw = v3.xy / v3.zz;
  r0.xy = r0.xy + -r0.zw;
  r0.zw = float2(0.00999999978,0.00999999978) * cb0[46].zw;
  r0.zw = cmp(abs(r0.xy) < r0.zw);
  r0.xy = r0.zw ? float2(0,0) : r0.xy;
  r0.zw = cb0[46].zw * float2(0.00999999978,0.00999999978) + float2(-1,-1);
  r0.xy = max(r0.xy, r0.zw);
  r0.zw = -cb0[46].zw * float2(0.00999999978,0.00999999978) + float2(1,1);
  r0.xy = min(r0.xy, r0.zw);
  r0.xy = float2(0.5,-0.5) * r0.xy;
  r0.z = cmp(cb1[36].y == 0.000000);
  o0.xy = r0.zz ? float2(2,0) : r0.xy;
  o0.zw = float2(0,0);
  return;
}