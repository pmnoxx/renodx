#include "../../custom.hlsl"

// ---- Created with 3Dmigoto v1.4.1 on Sat Jul 26 06:16:05 2025
Texture2D<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb0 : register(b0)
{
  float4 cb0[10];
}

RWTexture3D<float4> outputLUT : register(u0);

// 3Dmigoto declarations
#define cmp -

[numthreads(4, 4, 4)]
void main(uint3 vThreadID: SV_DispatchThreadID) {
// Needs manual fix for instruction:
// unknown dcl_: dcl_uav_typed_texture3d (float,float,float,float) u0
  float4 r0,r1,r2,r3;
  uint4 bitmask, uiDest;
  float4 fDest;
  float inf = 1e30;

// Needs manual fix for instruction:
// unknown dcl_: dcl_thread_group 4, 4, 4
  r0.xyz = (uint3)vThreadID.xyz;
  r1.xyz = cmp(r0.xyz < cb0[0].xxx);
  r0.w = r1.y ? r1.x : 0;
  r0.w = r1.z ? r0.w : 0;
  if (r0.w != 0) {
    r0.xyz = r0.xyz * cb0[0].yyy + float3(-0.386036009,-0.386036009,-0.386036009);
    r0.xyz = float3(13.6054821,13.6054821,13.6054821) * r0.xyz;
    r0.xyz = exp2(r0.xyz);
    r0.xyz = float3(-0.0479959995,-0.0479959995,-0.0479959995) + r0.xyz;
    r0.xyz = float3(0.179999992,0.179999992,0.179999992) * r0.xyz;

    r1.x = dot(float3(0.439700991,0.382977992,0.177334994), r0.xyz);
    r1.y = dot(float3(0.0897922963,0.813422978,0.0967615992), r0.xyz);
    r1.z = dot(float3(0.0175439995,0.111543998,0.870703995), r0.xyz);
    r0.xyz = max(float3(0,0,0), r1.xyz);
    r0.xyz = min(float3(65504,65504,65504), r0.xyz);
    r1.xyz = cmp(r0.xyz < float3(3.05175708e-05,3.05175708e-05,3.05175708e-05));
    r2.xyz = r0.xyz * float3(0.5,0.5,0.5) + float3(1.525878e-05,1.525878e-05,1.525878e-05);
    r2.xyz = log2(r2.xyz);
    r2.xyz = float3(9.72000027,9.72000027,9.72000027) + r2.xyz;
    r2.xyz = float3(0.0570776239,0.0570776239,0.0570776239) * r2.xyz;
    r0.xyz = log2(r0.xyz);
    r0.xyz = float3(9.72000027,9.72000027,9.72000027) + r0.xyz;
    r0.xyz = float3(0.0570776239,0.0570776239,0.0570776239) * r0.xyz;
    r0.xyz = r1.xyz ? r2.xyz : r0.xyz;
    r0.xyz = float3(-0.413588405,-0.413588405,-0.413588405) + r0.xyz;
    r0.xyz = r0.xyz * cb0[3].zzz + float3(0.413588405,0.413588405,0.413588405);
    r1.xyz = r0.xyz * float3(17.5200005,17.5200005,17.5200005) + float3(-9.72000027,-9.72000027,-9.72000027);
    r1.xyz = exp2(r1.xyz);
    r2.xyz = float3(-1.52587891e-05,-1.52587891e-05,-1.52587891e-05) + r1.xyz;
    r2.xyz = r2.xyz + r2.xyz;
    r3.xyzw = cmp(r0.xxyy < float4(-0.301369876,1.46799636,-0.301369876,1.46799636));
    r0.xy = r3.yw ? r1.xy : float2(65504,65504);
    r3.xy = r3.xz ? r2.xy : r0.xy;
    r0.xy = cmp(r0.zz < float2(-0.301369876,1.46799636));
    r0.y = r0.y ? r1.z : 65504;
    r3.z = r0.x ? r2.z : r0.y;
    r0.x = dot(float3(1.45143926,-0.236510754,-0.214928567), r3.xyz);
    r0.y = dot(float3(-0.0765537769,1.17622972,-0.0996759236), r3.xyz);
    r0.z = dot(float3(0.00831614807,-0.00603244966,0.997716308), r3.xyz);
    r1.x = dot(float3(0.390404999,0.549941003,0.00892631989), r0.xyz);
    r1.y = dot(float3(0.070841603,0.963172019,0.00135775004), r0.xyz);
    r1.z = dot(float3(0.0231081992,0.128021002,0.936245024), r0.xyz);
    r0.xyz = cb0[1].xyz * r1.xyz;
    r1.x = dot(float3(2.85846996,-1.62879002,-0.0248910002), r0.xyz);
    r1.y = dot(float3(-0.210181996,1.15820003,0.000324280991), r0.xyz);
    r1.z = dot(float3(-0.0418119989,-0.118169002,1.06867003), r0.xyz);
    r0.xyz = cb0[2].xyz * r1.xyz;
    r1.x = dot(r0.xyz, cb0[4].xyz);
    r1.y = dot(r0.xyz, cb0[5].xyz);
    r1.z = dot(r0.xyz, cb0[6].xyz);
    r0.xyz = r1.xyz * cb0[9].xyz + cb0[7].xyz;
    r1.xyz = saturate(r0.xyz * float3(12345678,12345678,12345678) + float3(0.5,0.5,0.5));
    r1.xyz = r1.xyz * float3(2,2,2) + float3(-1,-1,-1);
    r0.xyz = log2(abs(r0.xyz));
    r0.xyz = cb0[8].xyz * r0.xyz;
    r0.xyz = exp2(r0.xyz);
    r0.xyz = r1.xyz * r0.xyz;
    r0.xyz = max(float3(0,0,0), r0.xyz);
    r0.w = cmp(r0.y >= r0.z);
    r0.w = r0.w ? 1.000000 : 0;
    r1.xy = r0.zy;
    r1.zw = float2(-1,0.666666687);
    r2.xy = -r1.xy + r0.yz;
    r2.zw = float2(1,-1);
    r1.xyzw = r0.wwww * r2.xyzw + r1.xyzw;
    r0.w = cmp(r0.x >= r1.x);
    r0.w = r0.w ? 1.000000 : 0;
    r2.xyz = r1.xyw;
    r2.w = r0.x;
    r1.xyw = r2.wyx;
    r1.xyzw = r1.xyzw + -r2.xyzw;
    r1.xyzw = r0.wwww * r1.xyzw + r2.xyzw;
    r0.w = min(r1.w, r1.y);
    r0.w = r1.x + -r0.w;
    r1.y = r1.w + -r1.y;
    r1.w = r0.w * 6 + 9.99999975e-05;
    r1.y = r1.y / r1.w;
    r1.y = r1.z + r1.y;
    r2.x = abs(r1.y);
    r1.y = 9.99999975e-05 + r1.x;
    r2.z = r0.w / r1.y;
    r2.yw = float2(0.25,0.25);
    r0.w = t0.SampleLevel(s0_s, r2.xy, 0).y;
    r0.w = saturate(r0.w);
    r0.w = r0.w + r0.w;
    r1.y = t0.SampleLevel(s0_s, r2.zw, 0).z;
    r1.y = saturate(r1.y);
    r0.w = dot(r1.yy, r0.ww);
    r3.x = dot(r0.xyz, float3(0.212672904,0.715152204,0.0721750036));
    r3.yw = float2(0.25,0.25);
    r0.x = t0.SampleLevel(s0_s, r3.xy, 0).w;
    r0.x = saturate(r0.x);
    r0.x = r0.x * r0.w;
    r3.z = cb0[3].x + r2.x;
    r0.y = t0.SampleLevel(s0_s, r3.zw, 0).x;
    r0.y = saturate(r0.y);
    r0.y = -0.5 + r0.y;
    r0.y = r3.z + r0.y;
    r0.z = cmp(r0.y < 0);
    r0.w = cmp(1 < r0.y);
    r1.yz = float2(1,-1) + r0.yy;
    r0.y = r0.w ? r1.z : r0.y;
    r0.y = r0.z ? r1.y : r0.y;
    r0.yzw = float3(1,0.666666687,0.333333343) + r0.yyy;
    r0.yzw = frac(r0.yzw);
    r0.yzw = r0.yzw * float3(6,6,6) + float3(-3,-3,-3);
    r0.yzw = saturate(float3(-1,-1,-1) + abs(r0.yzw));
    r0.yzw = float3(-1,-1,-1) + r0.yzw;
    r0.yzw = r2.zzz * r0.yzw + float3(1,1,1);
    r1.yzw = r1.xxx * r0.yzw;
    r0.x = dot(cb0[3].yy, r0.xx);
    r1.y = dot(r1.yzw, float3(0.212672904,0.715152204,0.0721750036));
    r0.yzw = r1.xxx * r0.yzw + -r1.yyy;
    r0.xyz = r0.xxx * r0.yzw + r1.yyy;

    float3 untonemapped = r0.xyz;
    if (RENODX_TONE_MAP_TYPE == 2.f) {  // only apply Aces once
      r0.xyz = renodx::draw::ToneMapPass(r0.xyz);  //, sdrTonemapped);
      r0.xyz = renodx::draw::RenderIntermediatePass(r0.xyz);
      r0.xyz = max(float3(0.f, 0.f, 0.f), r0.xyz);
      r0.w = 1.f;
      outputLUT[vThreadID.xyz] = r0;
      return;
    }


    r1.y = dot(float3(0.695452213,0.140678704,0.163869068), r0.xyz);
    r1.z = dot(float3(0.0447945632,0.859671116,0.0955343172), r0.xyz);
    r1.w = dot(float3(-0.00552588282,0.00402521016,1.00150073), r0.xyz);
    r0.x = min(r1.y, r1.z);
    r0.x = min(r0.x, r1.w);
    r0.y = max(r1.y, r1.z);
    r0.y = max(r0.y, r1.w);
    r0.xyz = max(float3(9.99999975e-05,9.99999975e-05,0.00999999978), r0.xyy);
    r0.x = r0.y + -r0.x;
    r0.x = r0.x / r0.z;
    r0.yzw = r1.wzy + -r1.zyw;
    r0.yz = r1.wz * r0.yz;
    r0.y = r0.y + r0.z;
    r0.y = r1.y * r0.w + r0.y;
    r0.y = sqrt(r0.y);
    r0.z = r1.w + r1.z;
    r0.z = r0.z + r1.y;
    r0.y = r0.y * 1.75 + r0.z;
    r0.w = -0.400000006 + r0.x;
    r1.x = 2.5 * r0.w;
    r1.x = 1 + -abs(r1.x);
    r1.x = max(0, r1.x);
    r0.w = saturate(r0.w * inf + 0.5);
    r0.w = r0.w * 2 + -1;
    r1.x = -r1.x * r1.x + 1;
    r0.w = r0.w * r1.x + 1;
    r0.zw = float2(0.333333343,0.0250000004) * r0.yw;
    r1.x = cmp(0.159999996 >= r0.y);
    r0.y = cmp(r0.y >= 0.479999989);
    r0.z = 0.0799999982 / r0.z;
    r0.z = -0.5 + r0.z;
    r0.z = r0.w * r0.z;
    r0.y = r0.y ? 0 : r0.z;
    r0.y = r1.x ? r0.w : r0.y;
    r0.y = 1 + r0.y;
    r2.yzw = r1.yzw * r0.yyy;
    r0.zw = cmp(r2.zw == r2.yz);
    r0.z = r0.w ? r0.z : 0;
    r0.w = r1.z * r0.y + -r2.w;
    r0.w = 1.73205078 * r0.w;
    r1.x = r2.y * 2 + -r2.z;
    r1.x = -r1.w * r0.y + r1.x;
    r1.z = min(abs(r1.x), abs(r0.w));
    r1.w = max(abs(r1.x), abs(r0.w));
    r1.w = 1 / r1.w;
    r1.z = r1.z * r1.w;
    r1.w = r1.z * r1.z;
    r3.x = r1.w * 0.0208350997 + -0.0851330012;
    r3.x = r1.w * r3.x + 0.180141002;
    r3.x = r1.w * r3.x + -0.330299497;
    r1.w = r1.w * r3.x + 0.999866009;
    r3.x = r1.z * r1.w;
    r3.y = cmp(abs(r1.x) < abs(r0.w));
    r3.x = r3.x * -2 + 1.57079637;
    r3.x = r3.y ? r3.x : 0;
    r1.z = r1.z * r1.w + r3.x;
    r1.w = cmp(r1.x < -r1.x);
    r1.w = r1.w ? -3.141593 : 0;
    r1.z = r1.z + r1.w;
    r1.w = min(r1.x, r0.w);
    r0.w = max(r1.x, r0.w);
    r1.x = cmp(r1.w < -r1.w);
    r0.w = cmp(r0.w >= -r0.w);
    r0.w = r0.w ? r1.x : 0;
    r0.w = r0.w ? -r1.z : r1.z;
    r0.w = 57.2957802 * r0.w;
    r0.z = r0.z ? 0 : r0.w;
    r0.w = cmp(r0.z < 0);
    r1.x = 360 + r0.z;
    r0.z = r0.w ? r1.x : r0.z;
    r0.w = cmp(r0.z < -180);
    r1.x = cmp(180 < r0.z);
    r1.zw = float2(360,-360) + r0.zz;
    r0.z = r1.x ? r1.w : r0.z;
    r0.z = r0.w ? r1.z : r0.z;
    r0.z = 0.0148148146 * r0.z;
    r0.z = 1 + -abs(r0.z);
    r0.z = max(0, r0.z);
    r0.w = r0.z * -2 + 3;
    r0.z = r0.z * r0.z;
    r0.z = r0.w * r0.z;
    r0.z = r0.z * r0.z;
    r0.x = r0.z * r0.x;
    r0.y = -r1.y * r0.y + 0.0299999993;
    r0.x = r0.x * r0.y;
    r2.x = r0.x * 0.180000007 + r2.y;
    r0.x = dot(float3(1.45143926,-0.236510754,-0.214928567), r2.xzw);
    r0.y = dot(float3(-0.0765537769,1.17622972,-0.0996759236), r2.xzw);
    r0.z = dot(float3(0.00831614807,-0.00603244966,0.997716308), r2.xzw);
    r0.xyz = max(float3(0,0,0), r0.xyz);
    r0.w = dot(r0.xyz, float3(0.272228986,0.674081981,0.0536894985));
    r0.xyz = r0.xyz + -r0.www;
    r0.xyz = r0.xyz * float3(0.959999979,0.959999979,0.959999979) + r0.www;
    r1.xyz = r0.xyz * float3(278.508514,278.508514,278.508514) + float3(10.7771997,10.7771997,10.7771997);
    r1.xyz = r1.xyz * r0.xyz;
    r2.xyz = r0.xyz * float3(293.604492,293.604492,293.604492) + float3(88.7121964,88.7121964,88.7121964);
    r0.xyz = r0.xyz * r2.xyz + float3(80.6889038,80.6889038,80.6889038);
    r0.xyz = r1.xyz / r0.xyz;
    r1.x = dot(float3(0.662454188,0.134004205,0.156187683), r0.xyz);
    r1.y = dot(float3(0.272228718,0.674081743,0.0536895171), r0.xyz);
    r1.z = dot(float3(-0.00557464967,0.0040607336,1.01033914), r0.xyz);
    r0.x = dot(r1.xyz, float3(1,1,1));
    r0.x = max(9.99999975e-05, r0.x);
    r0.xy = r1.xy / r0.xx;
    r0.w = max(0, r1.y);
    r0.w = min(65504, r0.w);
    r0.w = log2(r0.w);
    r0.w = 0.981100023 * r0.w;
    r1.y = exp2(r0.w);
    r0.w = max(9.99999975e-05, r0.y);
    r0.w = r1.y / r0.w;
    r1.w = 1 + -r0.x;
    r0.z = r1.w + -r0.y;
    r1.xz = r0.xz * r0.ww;
    r0.x = dot(float3(1.6410234,-0.324803293,-0.236424699), r1.xyz);
    r0.y = dot(float3(-0.663662851,1.61533165,0.0167563483), r1.xyz);
    r0.z = dot(float3(0.0117218941,-0.00828444213,0.988394856), r1.xyz);
    r0.w = dot(r0.xyz, float3(0.272228986,0.674081981,0.0536894985));
    r0.xyz = r0.xyz + -r0.www;
    r0.xyz = r0.xyz * float3(0.930000007,0.930000007,0.930000007) + r0.www;
    r1.x = dot(float3(0.662454188,0.134004205,0.156187683), r0.xyz);
    r1.y = dot(float3(0.272228718,0.674081743,0.0536895171), r0.xyz);
    r1.z = dot(float3(-0.00557464967,0.0040607336,1.01033914), r0.xyz);
    r0.x = dot(float3(0.987223983,-0.00611326983,0.0159533005), r1.xyz);
    r0.y = dot(float3(-0.00759836007,1.00186002,0.00533019984), r1.xyz);
    r0.z = dot(float3(0.00307257008,-0.00509594986,1.08168006), r1.xyz);
    r1.x = dot(float3(3.2409699,-1.5373832,-0.498610765), r0.xyz);
    r1.y = dot(float3(-0.969243646,1.8759675,0.0415550582), r0.xyz);
    r1.z = dot(float3(0.0556300804,-0.203976959,1.05697155), r0.xyz);
    r0.xyz = max(float3(0, 0, 0), r1.xyz);
    r0.w = 1;
    if (RENODX_TONE_MAP_TYPE > 0.f) {
      r0.xyz = renodx::draw::ToneMapPass(untonemapped.xyz, r0.xyz);  //, sdrTonemapped);
      r0.xyz = max(float3(0.f, 0.f, 0.f), r0.xyz);
    }
  }
  outputLUT[vThreadID.xyz] = r0;
  return;
}