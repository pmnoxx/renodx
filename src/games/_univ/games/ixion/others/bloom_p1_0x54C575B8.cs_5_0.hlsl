// ---- Created with 3Dmigoto v1.4.1 on Thu Jul 24 23:02:36 2025
Texture2DArray<float4> t0 : register(t0);

SamplerState s0_s : register(s0);

cbuffer cb1 : register(b1)
{
  float4 cb1[2];
}

cbuffer cb0 : register(b0)
{
  float4 cb0[51];
}

RWTexture2D<float4> outputUber : register(u0);


// 3Dmigoto declarations
#define cmp -

[numthreads(4, 4, 4)]
void main(uint3 vThreadID: SV_DispatchThreadID) {
// Needs manual fix for instruction:
// unknown dcl_: dcl_uav_typed_texture2darray (float,float,float,float) u0
  float4 r0,r1,r2,r3,r4,r5;
  uint4 bitmask, uiDest;
  float4 fDest;

// Needs manual fix for instruction:
// unknown dcl_: dcl_thread_group 8, 8, 1
  r0.xyz = (uint3)vThreadID.xyz;
  r1.xy = float2(0.5,0.5) + r0.xy;
  r1.xy = cb1[0].zw * r1.xy;
  r1.zw = cb0[47].zw * float2(0.899999976,-0.400000006) + r1.xy;
  r2.xy = -cb0[47].zw * float2(0.5,0.5) + float2(1,1);
  r1.zw = min(r2.xy, r1.zw);
  r3.xy = cb0[50].xy * r1.zw;
  r3.z = r0.z;
  r4.xyz = t0.SampleLevel(s0_s, r3.xyz, 0).xyz;
  r0.w = max(r4.x, r4.y);
  r0.w = max(r0.w, r4.z);
  r1.zw = -cb1[1].yx + r0.ww;
  r0.w = max(9.99999975e-05, r0.w);
  r1.z = max(0, r1.z);
  r1.z = min(cb1[1].z, r1.z);
  r1.z = r1.z * r1.z;
  r1.z = cb1[1].w * r1.z;
  r1.z = max(r1.z, r1.w);
  r0.w = r1.z / r0.w;
  r4.xyzw = r4.xyzx * r0.wwww;
  r0.w = dot(r4.wyz, float3(0.212672904,0.715152204,0.0721750036));
  r0.w = 1 + r0.w;
  r0.w = 1 / r0.w;
  r4.xyzw = r4.xyzw * r0.wwww;
  r1.zw = min(r2.xy, r1.xy);
  r0.xy = cb0[50].xy * r1.zw;
  r0.xyz = t0.SampleLevel(s0_s, r0.xyz, 0).xyz;
  r1.z = max(r0.x, r0.y);
  r1.z = max(r1.z, r0.z);
  r2.zw = -cb1[1].yx + r1.zz;
  r1.z = max(9.99999975e-05, r1.z);
  r1.w = max(0, r2.z);
  r1.w = min(cb1[1].z, r1.w);
  r1.w = r1.w * r1.w;
  r1.w = cb1[1].w * r1.w;
  r1.w = max(r1.w, r2.w);
  r1.z = r1.w / r1.z;
  r5.xyzw = r1.zzzz * r0.xyzx;
  r0.x = dot(r5.wyz, float3(0.212672904,0.715152204,0.0721750036));
  r0.x = 1 + r0.x;
  r0.x = 1 / r0.x;
  r4.xyzw = r5.xyzw * r0.xxxx + r4.xyzw;
  r0.x = r0.x + r0.w;
  r5.xyzw = cb0[47].zwzw * float4(-0.899999976,0.400000006,0.400000006,0.899999976) + r1.xyxy;
  r0.yz = cb0[47].zw * float2(-0.400000006,-0.899999976) + r1.xy;
  r0.yz = min(r0.yz, r2.xy);
  r1.xyzw = min(r5.xyzw, r2.xyxy);
  r1.xyzw = cb0[50].xyxy * r1.zwxy;
  r2.xy = cb0[50].xy * r0.yz;
  r3.xy = r1.zw;
  r0.yzw = t0.SampleLevel(s0_s, r3.xyz, 0).xyz;
  r1.z = r3.z;
  r1.w = max(r0.y, r0.z);
  r1.w = max(r1.w, r0.w);
  r3.xy = -cb1[1].yx + r1.ww;
  r1.w = max(9.99999975e-05, r1.w);
  r2.w = max(0, r3.x);
  r2.w = min(cb1[1].z, r2.w);
  r2.w = r2.w * r2.w;
  r2.w = cb1[1].w * r2.w;
  r2.w = max(r2.w, r3.y);
  r1.w = r2.w / r1.w;
  r3.xyzw = r1.wwww * r0.yzwy;
  r0.y = dot(r3.wyz, float3(0.212672904,0.715152204,0.0721750036));
  r0.y = 1 + r0.y;
  r0.y = 1 / r0.y;
  r3.xyzw = r3.xyzw * r0.yyyy + r4.xyzw;
  r0.x = r0.x + r0.y;
  r0.yzw = t0.SampleLevel(s0_s, r1.xyz, 0).xyz;
  r2.z = r1.z;
  r1.xyz = t0.SampleLevel(s0_s, r2.xyz, 0).xyz;
  r1.w = max(r0.y, r0.z);
  r1.w = max(r1.w, r0.w);
  r2.xy = -cb1[1].yx + r1.ww;
  r1.w = max(9.99999975e-05, r1.w);
  r2.x = max(0, r2.x);
  r2.x = min(cb1[1].z, r2.x);
  r2.x = r2.x * r2.x;
  r2.x = cb1[1].w * r2.x;
  r2.x = max(r2.x, r2.y);
  r1.w = r2.x / r1.w;
  r2.xyzw = r1.wwww * r0.yzwy;
  r0.y = dot(r2.wyz, float3(0.212672904,0.715152204,0.0721750036));
  r0.y = 1 + r0.y;
  r0.y = 1 / r0.y;
  r2.xyzw = r2.xyzw * r0.yyyy + r3.xyzw;
  r0.x = r0.x + r0.y;
  r0.y = max(r1.x, r1.y);
  r0.y = max(r0.y, r1.z);
  r0.zw = -cb1[1].yx + r0.yy;
  r0.yz = max(float2(9.99999975e-05,0), r0.yz);
  r0.z = min(cb1[1].z, r0.z);
  r0.z = r0.z * r0.z;
  r0.z = cb1[1].w * r0.z;
  r0.z = max(r0.z, r0.w);
  r0.y = r0.z / r0.y;
  r1.xyzw = r1.xyzx * r0.yyyy;
  r0.y = dot(r1.wyz, float3(0.212672904,0.715152204,0.0721750036));
  r0.y = 1 + r0.y;
  r0.y = 1 / r0.y;
  r1.xyzw = r1.xyzw * r0.yyyy + r2.xyzw;
  r0.x = r0.x + r0.y;
  r0.xyzw = r1.xyzw / r0.xxxx;
  r1.xy = (uint2)cb1[0].xy;
  r1.xy = cmp((uint2)r1.xy >= (uint2)vThreadID.xy);
  r1.x = r1.y ? r1.x : 0;
  r1.x = r1.x ? 1.000000 : 0;
  r0.xyzw = r1.xxxx * r0.xyzw;

  // No code for instruction (needs manual fix):
  //store_uav_typed u0.xyzw, vThreadID.xyzz, r0.xyzw
  
  // fix for bloom
  r0 = saturate(r0); 
  outputUber[vThreadID.xy] = r0;
  return;
}