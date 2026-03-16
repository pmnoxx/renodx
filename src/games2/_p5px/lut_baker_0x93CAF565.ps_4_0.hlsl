#include "./shared.h"

// ---- Created with 3Dmigoto v1.3.16 on Thu Jun 26 13:28:05 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[41];
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

  r0.yz = -cb0[30].yz + v1.xy;
  r1.x = cb0[30].x * r0.y;
  r0.x = frac(r1.x);
  r1.x = r0.x / cb0[30].x;
  r0.w = -r1.x + r0.y;

  if (RENODX_TONE_MAP_TYPE > 0.f) {
    r0.xzw = renodx::color::pq::Decode(r0.xzw, 100.f);

    // copied linear2logc from output shader, but without the saturate that caps input
    r0.xzw = r0.xzw * float3(5.55555582, 5.55555582, 5.55555582) + float3(0.0479959995, 0.0479959995, 0.0479959995);
    r0.xzw = log2(r0.xzw);
    r0.xzw = (r0.xzw * float3(0.0734997839,0.0734997839,0.0734997839) + float3(0.386036009,0.386036009,0.386036009));
}

  r0.xyz = r0.xzw * cb0[30].www + float3(-0.413588405,-0.413588405,-0.413588405);
  r0.xyz = r0.xyz * cb0[34].zzz + float3(0.0275523961,0.0275523961,0.0275523961);
  r0.xyz = float3(13.6054821,13.6054821,13.6054821) * r0.xyz;
  r0.xyz = exp2(r0.xyz);
  r0.xyz = float3(-0.0479959995,-0.0479959995,-0.0479959995) + r0.xyz;
  r0.xyz = float3(0.179999992,0.179999992,0.179999992) * r0.xyz;



  r1.x = dot(float3(0.390404999,0.549941003,0.00892631989), r0.xyz);
  r1.y = dot(float3(0.070841603,0.963172019,0.00135775004), r0.xyz);
  r1.z = dot(float3(0.0231081992,0.128021002,0.936245024), r0.xyz);
  r0.xyz = cb0[32].xyz * r1.xyz;
  r1.x = dot(float3(2.85846996,-1.62879002,-0.0248910002), r0.xyz);
  r1.y = dot(float3(-0.210181996,1.15820003,0.000324280991), r0.xyz);
  r1.z = dot(float3(-0.0418119989,-0.118169002,1.06867003), r0.xyz);
  r0.xyz = cb0[33].xyz * r1.xyz;
  r1.x = dot(r0.xyz, cb0[35].xyz);
  r1.y = dot(r0.xyz, cb0[36].xyz);
  r1.z = dot(r0.xyz, cb0[37].xyz);
  r0.xyz = r1.xyz * cb0[40].xyz + cb0[38].xyz;
  r1.xyz = log2(abs(r0.xyz));
  r0.xyz = (r0.xyz > 0.0) ? 1.0 : ((r0.xyz < 0.0) ? 0.0 : 0.5);
  r0.xyz = r0.xyz * float3(2,2,2) + float3(-1,-1,-1);
  r1.xyz = cb0[39].xyz * r1.xyz;
  r1.xyz = exp2(r1.xyz);
  r0.xyz = r1.xyz * r0.xyz;
  r0.xyz = max(float3(0,0,0), r0.xyz);
  r0.w = cmp(r0.y >= r0.z);
  r0.w = r0.w ? 1.000000 : 0;
  r1.xy = r0.zy;
  r2.xy = -r1.xy + r0.yz;
  r1.zw = float2(-1,0.666666687);
  r2.zw = float2(1,-1);
  r1.xyzw = r0.wwww * r2.xywz + r1.xywz;
  r0.w = cmp(r0.x >= r1.x);
  r0.w = r0.w ? 1.000000 : 0;
  r2.z = r1.w;
  r1.w = r0.x;
  r3.x = dot(r0.xyz, float3(0.212672904,0.715152204,0.0721750036));
  r2.xyw = r1.wyx;
  r2.xyzw = r2.xyzw + -r1.xyzw;
  r0.xyzw = r0.wwww * r2.xyzw + r1.xyzw;
  r1.x = min(r0.w, r0.y);
  r1.x = -r1.x + r0.x;
  r1.y = r1.x * 6 + 9.99999975e-005;
  r0.y = r0.w + -r0.y;
  r0.y = r0.y / r1.y;
  r0.y = r0.z + r0.y;
  r2.x = abs(r0.y);
  r3.z = cb0[34].x + r2.x;
  r3.yw = float2(0.25,0.25);
  r4.xyzw = t0.SampleLevel(s0_s, r3.zw, 0).xyzw;
  r5.xyzw = t0.SampleLevel(s0_s, r3.xy, 0).wxyz;
  r5.x = saturate(r5.x);
  r4.x = saturate(r4.x);
  r0.y = r4.x + r3.z;
  r0.yzw = float3(-0.5,0.5,-1.5) + r0.yyy;
  r1.y = cmp(1 < r0.y);
  r0.w = r1.y ? r0.w : r0.y;
  r0.y = cmp(r0.y < 0);
  r0.y = r0.y ? r0.z : r0.w;
  r0.yzw = float3(1,0.666666687,0.333333343) + r0.yyy;
  r0.yzw = frac(r0.yzw);
  r0.yzw = r0.yzw * float3(6,6,6) + float3(-3,-3,-3);
  r0.yzw = saturate(float3(-1,-1,-1) + abs(r0.yzw));
  r0.yzw = float3(-1,-1,-1) + r0.yzw;
  r1.y = 9.99999975e-005 + r0.x;
  r2.z = r1.x / r1.y;
  r0.yzw = r2.zzz * r0.yzw + float3(1,1,1);
  r1.xyz = r0.xxx * r0.yzw;
  r1.x = dot(r1.xyz, float3(0.212672904,0.715152204,0.0721750036));
  r0.xyz = r0.xxx * r0.yzw + -r1.xxx;
  r2.yw = float2(0.25,0.25);
  r3.xyzw = t0.SampleLevel(s0_s, r2.xy, 0).yxzw;
  r2.xyzw = t0.SampleLevel(s0_s, r2.zw, 0).zxyw;
  r2.x = saturate(r2.x);
  r3.x = saturate(r3.x);
  r0.w = r3.x + r3.x;
  r0.w = dot(r2.xx, r0.ww);
  r0.w = r5.x * r0.w;
  r0.w = dot(cb0[34].yy, r0.ww);
  r0.xyz = r0.www * r0.xyz + r1.xxx;
  r0.xyz = max(float3(0,0,0), r0.xyz);

  if (RENODX_TONE_MAP_TYPE == 0.f) {
    r1.xyz = r0.xyz * float3(0.262677222,0.262677222,0.262677222) + float3(0.0695999935,0.0695999935,0.0695999935);
    r2.xyz = float3(1.31338608,1.31338608,1.31338608) * r0.xyz;
    r0.xyz = r0.xyz * float3(0.262677222,0.262677222,0.262677222) + float3(0.289999992,0.289999992,0.289999992);
    r0.xyz = r2.xyz * r0.xyz + float3(0.0816000104,0.0816000104,0.0816000104);
    r1.xyz = r2.xyz * r1.xyz + float3(0.00543999998,0.00543999998,0.00543999998);
    r0.xyz = r1.xyz / r0.xyz;
    r0.xyz = float3(-0.0666666627,-0.0666666627,-0.0666666627) + r0.xyz;
    r0.xyz = saturate(float3(1.31338608,1.31338608,1.31338608) * r0.xyz);

    r0.xyz = float3(0.00390625,0.00390625,0.00390625) + r0.xyz;
    r0.w = 0.75;
    r1.xyzw = t0.Sample(s0_s, r0.xw).wxyz;
    r1.x = saturate(r1.x);
    r2.xyzw = t0.Sample(s0_s, r0.yw).xyzw;
    r0.xyzw = t0.Sample(s0_s, r0.zw).xyzw;
    r1.z = saturate(r0.w);
    r1.y = saturate(r2.w);
    r0.xyz = float3(0.00390625,0.00390625,0.00390625) + r1.xyz;
    r0.w = 0.75;
    r1.xyzw = t0.Sample(s0_s, r0.xw).xyzw;
    o0.x = (r1.x);
    r1.xyzw = t0.Sample(s0_s, r0.yw).xyzw;
    r0.xyzw = t0.Sample(s0_s, r0.zw).xyzw;
    o0.z = (r0.z);
    o0.y = saturate(r1.y);
  } else {
    o0.xyz = r0.xyz;
  }

  if (RENODX_TONE_MAP_TYPE > 0.f) {
    //o0.xyz = renodx::tonemap::UpgradeToneMap(untonemapped.rgb, neutral_sdr, o0.rgb, 1.f);
    // o0.xyz = renodx::draw::ToneMapPass(untonemapped.rgb, o0.xyz);
    o0.xyz = renodx::draw::ToneMapPass(o0.xyz);
  }
  
  o0.w = 1;
  return;
}