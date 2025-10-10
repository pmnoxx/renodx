#include "../../common.hlsl"

// ---- Created with 3Dmigoto v1.3.2 on Thu Dec 26 16:58:24 2024
cbuffer cb0 : register(b0) {
  float4 cb0[62];
}

// 3Dmigoto declarations
#define cmp -
Texture1D<float4> IniParams : register(t120);
Texture2D<float4> StereoParams : register(t125);

void main(
    linear noperspective float2 v0: TEXCOORD0,
    float4 v1: SV_POSITION0,
    uint v2: SV_RenderTargetArrayIndex0,
    out float4 o0: SV_Target0) {
  const float4 icb[] = { { -4.000000, -0.718548, -2.301030, 0.801995 },
                         { -4.000000, 2.081031, -2.301030, 1.198005 },
                         { -3.157377, 3.668124, -1.931200, 1.594300 },
                         { -0.485250, 4.000000, -1.520500, 1.997300 },
                         { 1.847732, 4.000000, -1.057800, 2.378300 },
                         { 1.847732, 4.000000, -0.466800, 2.768400 },
                         { 0, 0, 0.119380, 3.051500 },
                         { 0, 0, 0.708813, 3.274629 },
                         { 0, 0, 1.291187, 3.327431 },
                         { 0, 0, 1.291187, 3.327431 } };
  float4 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xy = float2(-0.015625, -0.015625) + v0.xy;
  r0.xy = float2(1.03225803, 1.03225803) * r0.xy;
  r0.z = (uint)v2.x;
  r1.z = 0.0322580636 * r0.z;
  r0.z = cmp(asuint(cb0[61].z) >= 3);
  r2.xy = log2(r0.xy);
  r2.z = log2(r1.z);
  r0.xyw = float3(0.0126833133, 0.0126833133, 0.0126833133) * r2.xyz;
  r0.xyw = exp2(r0.xyw);
  r2.xyz = float3(-0.8359375, -0.8359375, -0.8359375) + r0.xyw;
  r2.xyz = max(float3(0, 0, 0), r2.xyz);
  r0.xyw = -r0.xyw * float3(18.6875, 18.6875, 18.6875) + float3(18.8515625, 18.8515625, 18.8515625);
  r0.xyw = r2.xyz / r0.xyw;
  r0.xyw = log2(r0.xyw);
  r0.xyw = float3(6.27739477, 6.27739477, 6.27739477) * r0.xyw;
  r0.xyw = exp2(r0.xyw);
  r0.xyw = float3(100, 100, 100) * r0.xyw;
  r1.xy = v0.xy * float2(1.03225803, 1.03225803) + float2(-0.0161290318, -0.0161290318);
  r2.xyz = float3(-0.434017599, -0.434017599, -0.434017599) + r1.xyz;
  r2.xyz = float3(14, 14, 14) * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r2.xyz = r2.xyz * float3(0.180000007, 0.180000007, 0.180000007) + float3(-0.00266771927, -0.00266771927, -0.00266771927);
  r0.xyz = r0.zzz ? r0.xyw : r2.xyz;
  r0.w = 1.00055635 * cb0[40].x;
  r1.w = cmp(6996.10791 >= cb0[40].x);
  r2.xy = float2(4.60700006e+009, 2.0064e+009) / r0.ww;
  r2.xy = float2(2967800, 1901800) + -r2.xy;
  r2.xy = r2.xy / r0.ww;
  r2.xy = float2(99.1100006, 247.479996) + r2.xy;
  r2.xy = r2.xy / r0.ww;
  r2.xy = float2(0.244063005, 0.237039998) + r2.xy;
  r2.x = r1.w ? r2.x : r2.y;
  r0.w = r2.x * r2.x;
  r1.w = 2.86999989 * r2.x;
  r0.w = r0.w * -3 + r1.w;
  r2.y = -0.275000006 + r0.w;
  r3.xyz = cb0[40].xxx * float3(0.000154118257, 0.00084242021, 4.22806261e-005) + float3(0.860117733, 1, 0.317398727);
  r0.w = cb0[40].x * cb0[40].x;
  r3.xyz = r0.www * float3(1.28641219e-007, 7.08145137e-007, 4.20481676e-008) + r3.xyz;
  r3.x = r3.x / r3.y;
  r1.w = -cb0[40].x * 2.8974182e-005 + 1;
  r0.w = r0.w * 1.61456057e-007 + r1.w;
  r3.y = r3.z / r0.w;
  r2.zw = r3.xy + r3.xy;
  r0.w = 3 * r3.x;
  r1.w = -r3.y * 8 + r2.z;
  r1.w = 4 + r1.w;
  r4.x = r0.w / r1.w;
  r4.y = r2.w / r1.w;
  r0.w = cmp(cb0[40].x < 4000);
  r2.xy = r0.ww ? r4.xy : r2.xy;
  r0.w = dot(r3.xy, r3.xy);
  r0.w = rsqrt(r0.w);
  r2.zw = r3.xy * r0.ww;
  r0.w = cb0[40].y * -r2.w;
  r0.w = r0.w * 0.0500000007 + r3.x;
  r1.w = cb0[40].y * r2.z;
  r1.w = r1.w * 0.0500000007 + r3.y;
  r2.z = 3 * r0.w;
  r0.w = r0.w + r0.w;
  r0.w = -r1.w * 8 + r0.w;
  r0.w = 4 + r0.w;
  r3.x = r2.z / r0.w;
  r1.w = r1.w + r1.w;
  r3.y = r1.w / r0.w;
  r2.zw = r3.xy + -r4.xy;
  r2.xy = r2.xy + r2.zw;
  r0.w = max(1.00000001e-010, r2.y);
  r3.x = r2.x / r0.w;
  r1.w = 1 + -r2.x;
  r1.w = r1.w + -r2.y;
  r3.z = r1.w / r0.w;
  r3.y = 1;
  r0.w = dot(float3(0.895099998, 0.266400009, -0.161400005), r3.xyz);
  r1.w = dot(float3(-0.750199974, 1.71350002, 0.0366999991), r3.xyz);
  r2.x = dot(float3(0.0388999991, -0.0684999973, 1.02960002), r3.xyz);
  r0.w = 0.941379249 / r0.w;
  r1.w = 1.04043639 / r1.w;
  r2.x = 1.0897665 / r2.x;
  r2.yzw = float3(0.895099998, 0.266400009, -0.161400005) * r0.www;
  r3.xyz = float3(-0.750199974, 1.71350002, 0.0366999991) * r1.www;
  r4.xyz = float3(0.0388999991, -0.0684999973, 1.02960002) * r2.xxx;
  r5.x = r2.y;
  r5.y = r3.x;
  r5.z = r4.x;
  r6.x = dot(float3(0.986992896, -0.1470543, 0.159962699), r5.xyz);
  r2.x = r2.z;
  r2.y = r3.y;
  r2.z = r4.y;
  r6.y = dot(float3(0.986992896, -0.1470543, 0.159962699), r2.xyz);
  r4.x = r2.w;
  r4.y = r3.z;
  r6.z = dot(float3(0.986992896, -0.1470543, 0.159962699), r4.xyz);
  r3.x = dot(float3(0.432305306, 0.518360317, 0.0492912009), r5.xyz);
  r3.y = dot(float3(0.432305306, 0.518360317, 0.0492912009), r2.xyz);
  r3.z = dot(float3(0.432305306, 0.518360317, 0.0492912009), r4.xyz);
  r5.x = dot(float3(-0.0085287001, 0.040042799, 0.968486726), r5.xyz);
  r5.y = dot(float3(-0.0085287001, 0.040042799, 0.968486726), r2.xyz);
  r5.z = dot(float3(-0.0085287001, 0.040042799, 0.968486726), r4.xyz);
  r2.x = dot(r6.xyz, float3(0.412456393, 0.212672904, 0.0193339009));
  r4.x = dot(r6.xyz, float3(0.357576102, 0.715152204, 0.119191997));
  r6.x = dot(r6.xyz, float3(0.180437505, 0.0721750036, 0.950304091));
  r2.y = dot(r3.xyz, float3(0.412456393, 0.212672904, 0.0193339009));
  r4.y = dot(r3.xyz, float3(0.357576102, 0.715152204, 0.119191997));
  r6.y = dot(r3.xyz, float3(0.180437505, 0.0721750036, 0.950304091));
  r2.z = dot(r5.xyz, float3(0.412456393, 0.212672904, 0.0193339009));
  r4.z = dot(r5.xyz, float3(0.357576102, 0.715152204, 0.119191997));
  r6.z = dot(r5.xyz, float3(0.180437505, 0.0721750036, 0.950304091));
  r3.x = dot(float3(3.2409699, -1.5373832, -0.498610765), r2.xyz);
  r3.y = dot(float3(3.2409699, -1.5373832, -0.498610765), r4.xyz);
  r3.z = dot(float3(3.2409699, -1.5373832, -0.498610765), r6.xyz);
  r5.x = dot(float3(-0.969243646, 1.8759675, 0.0415550582), r2.xyz);
  r5.y = dot(float3(-0.969243646, 1.8759675, 0.0415550582), r4.xyz);
  r5.z = dot(float3(-0.969243646, 1.8759675, 0.0415550582), r6.xyz);
  r2.x = dot(float3(0.0556300804, -0.203976959, 1.05697155), r2.xyz);
  r2.y = dot(float3(0.0556300804, -0.203976959, 1.05697155), r4.xyz);
  r2.z = dot(float3(0.0556300804, -0.203976959, 1.05697155), r6.xyz);
  r3.x = dot(r3.xyz, r0.xyz);
  r3.y = dot(r5.xyz, r0.xyz);
  r3.z = dot(r2.xyz, r0.xyz);
  r0.x = dot(float3(0.613191485, 0.33951208, 0.0473663323), r3.xyz);
  r0.y = dot(float3(0.0702069029, 0.916335821, 0.0134500116), r3.xyz);
  r0.z = dot(float3(0.0206188709, 0.109567292, 0.869606733), r3.xyz);
  r0.w = dot(r0.xyz, float3(0.272228718, 0.674081743, 0.0536895171));

  SetUngradedAP1(r0.xyz);

  r2.xyzw = cb0[46].xyzw * cb0[41].xyzw;
  r3.xyzw = cb0[47].xyzw * cb0[42].xyzw;
  r4.xyzw = cb0[48].xyzw * cb0[43].xyzw;
  r5.xyzw = cb0[49].xyzw * cb0[44].xyzw;
  r6.xyzw = cb0[50].xyzw + cb0[45].xyzw;
  r2.xyz = r2.xyz * r2.www;
  r0.xyz = r0.xyz + -r0.www;
  r2.xyz = r2.xyz * r0.xyz + r0.www;
  r2.xyz = max(float3(0, 0, 0), r2.xyz);
  r2.xyz = float3(5.55555534, 5.55555534, 5.55555534) * r2.xyz;
  r3.xyz = r3.xyz * r3.www;
  r2.xyz = log2(r2.xyz);
  r2.xyz = r3.xyz * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r2.xyz = float3(0.180000007, 0.180000007, 0.180000007) * r2.xyz;
  r3.xyz = r4.xyz * r4.www;
  r3.xyz = float3(1, 1, 1) / r3.xyz;
  r2.xyz = log2(r2.xyz);
  r2.xyz = r3.xyz * r2.xyz;
  r2.xyz = exp2(r2.xyz);
  r3.xyz = r5.xyz * r5.www;
  r4.xyz = r6.xyz + r6.www;
  r2.xyz = r2.xyz * r3.xyz + r4.xyz;
  r1.w = 1 / cb0[61].x;
  r1.w = saturate(r1.w * r0.w);
  r2.w = r1.w * -2 + 3;
  r1.w = r1.w * r1.w;
  r1.w = -r2.w * r1.w + 1;
  r3.xyzw = cb0[56].xyzw * cb0[41].xyzw;
  r4.xyzw = cb0[57].xyzw * cb0[42].xyzw;
  r5.xyzw = cb0[58].xyzw * cb0[43].xyzw;
  r6.xyzw = cb0[59].xyzw * cb0[44].xyzw;
  r7.xyzw = cb0[60].xyzw + cb0[45].xyzw;
  r3.xyz = r3.xyz * r3.www;
  r3.xyz = r3.xyz * r0.xyz + r0.www;
  r3.xyz = max(float3(0, 0, 0), r3.xyz);
  r3.xyz = float3(5.55555534, 5.55555534, 5.55555534) * r3.xyz;
  r4.xyz = r4.xyz * r4.www;
  r3.xyz = log2(r3.xyz);
  r3.xyz = r4.xyz * r3.xyz;
  r3.xyz = exp2(r3.xyz);
  r3.xyz = float3(0.180000007, 0.180000007, 0.180000007) * r3.xyz;
  r4.xyz = r5.xyz * r5.www;
  r4.xyz = float3(1, 1, 1) / r4.xyz;
  r3.xyz = log2(r3.xyz);
  r3.xyz = r4.xyz * r3.xyz;
  r3.xyz = exp2(r3.xyz);
  r4.xyz = r6.xyz * r6.www;
  r5.xyz = r7.xyz + r7.www;
  r3.xyz = r3.xyz * r4.xyz + r5.xyz;
  r2.w = 1 + -cb0[61].y;
  r3.w = -cb0[61].y + r0.w;
  r2.w = 1 / r2.w;
  r2.w = saturate(r3.w * r2.w);
  r3.w = r2.w * -2 + 3;
  r2.w = r2.w * r2.w;
  r4.x = r3.w * r2.w;
  r5.xyzw = cb0[51].xyzw * cb0[41].xyzw;
  r6.xyzw = cb0[52].xyzw * cb0[42].xyzw;
  r7.xyzw = cb0[53].xyzw * cb0[43].xyzw;
  r8.xyzw = cb0[54].xyzw * cb0[44].xyzw;
  r9.xyzw = cb0[55].xyzw + cb0[45].xyzw;
  r4.yzw = r5.xyz * r5.www;
  r0.xyz = r4.yzw * r0.xyz + r0.www;
  r0.xyz = max(float3(0, 0, 0), r0.xyz);
  r0.xyz = float3(5.55555534, 5.55555534, 5.55555534) * r0.xyz;
  r4.yzw = r6.xyz * r6.www;
  r0.xyz = log2(r0.xyz);
  r0.xyz = r4.yzw * r0.xyz;
  r0.xyz = exp2(r0.xyz);
  r0.xyz = float3(0.180000007, 0.180000007, 0.180000007) * r0.xyz;
  r4.yzw = r7.xyz * r7.www;
  r4.yzw = float3(1, 1, 1) / r4.yzw;
  r0.xyz = log2(r0.xyz);
  r0.xyz = r4.yzw * r0.xyz;
  r0.xyz = exp2(r0.xyz);
  r4.yzw = r8.xyz * r8.www;
  r5.xyz = r9.xyz + r9.www;
  r0.xyz = r0.xyz * r4.yzw + r5.xyz;
  r0.w = 1 + -r1.w;
  r0.w = -r3.w * r2.w + r0.w;
  r0.xyz = r0.xyz * r0.www;
  r0.xyz = r2.xyz * r1.www + r0.xyz;
  r0.xyz = r3.xyz * r4.xxx + r0.xyz;

  SetUntonemappedAP1(r0.xyz);

  // AP1 => BT709
  r2.x = dot(float3(1.70505154, -0.621790707, -0.0832583979), r0.xyz);
  r2.y = dot(float3(-0.130257145, 1.14080286, -0.0105485283), r0.xyz);
  r2.z = dot(float3(-0.0240032747, -0.128968775, 1.15297174), r0.xyz);
  r0.x = cmp(cb0[26].w == 0.000000);
  if (r0.x != 0) {
    r0.x = dot(r2.xyz, cb0[19].xyz);
    r0.y = dot(r2.xyz, cb0[20].xyz);
    r0.z = dot(r2.xyz, cb0[21].xyz);
    r0.w = dot(r2.xyz, cb0[24].xyz);
    r0.w = 1 + r0.w;
    r0.w = rcp(r0.w);
    r3.xyz = cb0[26].xyz * r0.www + cb0[25].xyz;
    r0.xyz = r3.xyz * r0.xyz;
    r0.xyz = max(float3(0, 0, 0), r0.xyz);
    r3.xyz = cb0[22].xxx + -r0.xyz;
    r3.xyz = max(float3(0, 0, 0), r3.xyz);
    r4.xyz = max(cb0[22].zzz, r0.xyz);
    r0.xyz = max(cb0[22].xxx, r0.xyz);
    r0.xyz = min(cb0[22].zzz, r0.xyz);
    r5.xyz = r4.xyz * cb0[23].xxx + cb0[23].yyy;
    r4.xyz = cb0[22].www + r4.xyz;
    r4.xyz = rcp(r4.xyz);
    r6.xyz = cb0[19].www * r3.xyz;
    r3.xyz = cb0[22].yyy + r3.xyz;
    r3.xyz = rcp(r3.xyz);
    r3.xyz = r6.xyz * r3.xyz + cb0[20].www;
    r0.xyz = r0.xyz * cb0[21].www + r3.xyz;
    r0.xyz = r5.xyz * r4.xyz + r0.xyz;
    r0.xyz = float3(-0.00200000009, -0.00200000009, -0.00200000009) + r0.xyz;

    SetTonemappedBT709(r0.xyz);

  } else {
    r3.x = dot(float3(0.613191485, 0.33951208, 0.0473663323), r2.xyz);
    r3.y = dot(float3(0.0702069029, 0.916335821, 0.0134500116), r2.xyz);
    r3.z = dot(float3(0.0206188709, 0.109567292, 0.869606733), r2.xyz);
    r3.xyz = max(float3(0, 0, 0), r3.xyz);
    r0.w = dot(r3.xyz, float3(0.272228718, 0.674081743, 0.0536895171));
    r3.xyz = r3.xyz + -r0.www;
    r3.xyz = r3.xyz * float3(0.959999979, 0.959999979, 0.959999979) + r0.www;
    r4.xy = float2(1, 0.180000007) + cb0[27].ww;
    r0.w = -cb0[27].y + r4.x;
    r1.w = 1 + cb0[28].x;
    r2.w = -cb0[27].z + r1.w;
    r3.w = cmp(0.800000012 < cb0[27].y);
    r4.xz = float2(0.819999993, 1) + -cb0[27].yy;
    r4.xz = r4.xz / cb0[27].xx;
    r4.y = r4.y / r0.w;
    r4.xw = float2(-0.744727492, -1) + r4.xy;
    r4.w = 1 + -r4.w;
    r4.y = r4.y / r4.w;
    r4.y = log2(r4.y);
    r4.y = 0.346573591 * r4.y;
    r4.w = r0.w / cb0[27].x;
    r4.y = -r4.y * r4.w + -0.744727492;
    r3.w = r3.w ? r4.x : r4.y;
    r4.x = r4.z + -r3.w;
    r4.y = cb0[27].z / cb0[27].x;
    r4.y = r4.y + -r4.x;
    r3.xyz = log2(r3.xyz);
    r5.xyz = float3(0.30103001, 0.30103001, 0.30103001) * r3.xyz;
    r4.xzw = r3.xyz * float3(0.30103001, 0.30103001, 0.30103001) + r4.xxx;
    r4.xzw = cb0[27].xxx * r4.xzw;
    r5.w = r0.w + r0.w;
    r6.x = -2 * cb0[27].x;
    r0.w = r6.x / r0.w;
    r6.xyz = r3.xyz * float3(0.30103001, 0.30103001, 0.30103001) + -r3.www;
    r7.xyz = r6.xyz * r0.www;
    r7.xyz = float3(1.44269502, 1.44269502, 1.44269502) * r7.xyz;
    r7.xyz = exp2(r7.xyz);
    r7.xyz = float3(1, 1, 1) + r7.xyz;
    r7.xyz = r5.www / r7.xyz;
    r7.xyz = -cb0[27].www + r7.xyz;
    r0.w = r2.w + r2.w;
    r5.w = cb0[27].x + cb0[27].x;
    r2.w = r5.w / r2.w;
    r3.xyz = r3.xyz * float3(0.30103001, 0.30103001, 0.30103001) + -r4.yyy;
    r3.xyz = r3.xyz * r2.www;
    r3.xyz = float3(1.44269502, 1.44269502, 1.44269502) * r3.xyz;
    r3.xyz = exp2(r3.xyz);
    r3.xyz = float3(1, 1, 1) + r3.xyz;
    r3.xyz = r0.www / r3.xyz;
    r3.xyz = -r3.xyz + r1.www;
    r8.xyz = cmp(r5.xyz < r3.www);
    r7.xyz = r8.xyz ? r7.xyz : r4.xzw;
    r5.xyz = cmp(r4.yyy < r5.xyz);
    r3.xyz = r5.xyz ? r3.xyz : r4.xzw;
    r0.w = r4.y + -r3.w;
    r4.xzw = saturate(r6.xyz / r0.www);
    r0.w = cmp(r4.y < r3.w);
    r5.xyz = float3(1, 1, 1) + -r4.xzw;
    r4.xyz = r0.www ? r5.xyz : r4.xzw;
    r5.xyz = -r4.xyz * float3(2, 2, 2) + float3(3, 3, 3);
    r4.xyz = r4.xyz * r4.xyz;
    r4.xyz = r4.xyz * r5.xyz;
    r3.xyz = r3.xyz + -r7.xyz;
    r3.xyz = r4.xyz * r3.xyz + r7.xyz;
    r0.w = dot(r3.xyz, float3(0.272228718, 0.674081743, 0.0536895171));
    r3.xyz = r3.xyz + -r0.www;
    r3.xyz = r3.xyz * float3(0.930000007, 0.930000007, 0.930000007) + r0.www;
    r3.xyz = max(float3(0, 0, 0), r3.xyz);
    r4.x = dot(float3(1.70505154, -0.621790707, -0.0832583979), r3.xyz);
    r4.y = dot(float3(-0.130257145, 1.14080286, -0.0105485283), r3.xyz);
    r4.z = dot(float3(-0.0240032747, -0.128968775, 1.15297174), r3.xyz);
    r0.xyz = max(float3(0, 0, 0), r4.xyz);
  }
  r3.xy = cmp(asint(cb0[61].zz) == int2(3, 5));
  r0.w = (int)r3.y | (int)r3.x;
  if (r0.w != 0) {
    r1.w = cmp(cb0[39].x == 1.000000);
    if (r1.w != 0) {
      r3.xyz = cb0[33].yzw * r2.xyz;
      r4.xyz = -r2.xyz * cb0[33].yzw + cb0[34].xyz;
      r3.xyz = cb0[34].www * r4.xyz + r3.xyz;
      r3.xyz = float3(0.00999999978, 0.00999999978, 0.00999999978) * r3.xyz;
      r3.xyz = log2(r3.xyz);
      r3.xyz = float3(0.159301758, 0.159301758, 0.159301758) * r3.xyz;
      r3.xyz = exp2(r3.xyz);
      r4.xyz = r3.xyz * float3(18.8515625, 18.8515625, 18.8515625) + float3(0.8359375, 0.8359375, 0.8359375);
      r3.xyz = r3.xyz * float3(18.6875, 18.6875, 18.6875) + float3(1, 1, 1);
      r3.xyz = rcp(r3.xyz);
      r3.xyz = r4.xyz * r3.xyz;
      r3.xyz = log2(r3.xyz);
      r3.xyz = float3(78.84375, 78.84375, 78.84375) * r3.xyz;
      r3.xyz = exp2(r3.xyz);
      r3.xyz = cb0[37].www * -cb0[37].xyz + r3.xyz;
      r3.xyz = float3(0.952381015, 0.952381015, 0.952381015) * r3.xyz;
    } else {
      r1.w = cmp(cb0[39].x == 2.000000);
      r4.xyz = cb0[33].yzw * r2.xyz;
      r5.xyz = -r2.xyz * cb0[33].yzw + cb0[34].xyz;
      r4.xyz = cb0[34].www * r5.xyz + r4.xyz;
      r4.xyz = float3(0.00999999978, 0.00999999978, 0.00999999978) * r4.xyz;
      r4.xyz = log2(r4.xyz);
      r4.xyz = float3(0.159301758, 0.159301758, 0.159301758) * r4.xyz;
      r4.xyz = exp2(r4.xyz);
      r5.xyz = r4.xyz * float3(18.8515625, 18.8515625, 18.8515625) + float3(0.8359375, 0.8359375, 0.8359375);
      r4.xyz = r4.xyz * float3(18.6875, 18.6875, 18.6875) + float3(1, 1, 1);
      r4.xyz = rcp(r4.xyz);
      r4.xyz = r5.xyz * r4.xyz;
      r4.xyz = log2(r4.xyz);
      r4.xyz = float3(78.84375, 78.84375, 78.84375) * r4.xyz;
      r4.xyz = exp2(r4.xyz);
      r4.xyz = float3(0.952381015, 0.952381015, 0.952381015) * r4.xyz;
      r3.xyz = r1.www ? r4.xyz : r1.xyz;
    }
    o0.xyz = r3.xyz;
    o0.w = 0;
  }
  if (r0.w == 0) {
    r1.xyz = r0.xyz * r0.xyz;
    r0.xyz = cb0[17].yyy * r0.xyz;
    r0.xyz = cb0[17].xxx * r1.xyz + r0.xyz;
    r0.xyz = cb0[17].zzz + r0.xyz;
    r1.xyz = cb0[33].yzw * r0.xyz;
    r0.xyz = -r0.xyz * cb0[33].yzw + cb0[34].xyz;
    r0.xyz = cb0[34].www * r0.xyz + r1.xyz;
    r0.xyz = max(float3(0, 0, 0), r0.xyz);
    r0.xyz = log2(r0.xyz);
    r0.xyz = cb0[18].yyy * r0.xyz;
    r1.xyz = exp2(r0.xyz);

    if (RENODX_TONE_MAP_TYPE != 0) {
      float3 color = GenerateToneMap(r1.xyz);
      color = renodx::draw::RenderIntermediatePass(color);
      color = cb0[37].www * -cb0[37].xyz + color;  // lerp to 0
      color *= 1.f / 1.05f;
      color = renodx::color::bt709::clamp::BT2020(color);
      o0.rgb = color;
      o0.w = 0;
      return;
    }

    [branch]
    if (asuint(cb0[61].z) == 0) {
      r3.xyz = float3(12.9200001, 12.9200001, 12.9200001) * r1.xyz;
      r4.xyz = cmp(r1.xyz >= float3(0.00313066994, 0.00313066994, 0.00313066994));
      r0.xyz = float3(0.416666657, 0.416666657, 0.416666657) * r0.xyz;
      r0.xyz = exp2(r0.xyz);
      r0.xyz = r0.xyz * float3(1.05499995, 1.05499995, 1.05499995) + float3(-0.0549999997, -0.0549999997, -0.0549999997);
      r0.xyz = r4.xyz ? r0.xyz : r3.xyz;
    } else {
      r3.xyzw = cmp(asint(cb0[61].wwww) == int4(1, 2, 3, 4));
      r4.xyz = r3.www ? float3(1, 0, 0) : float3(1.70505154, -0.621790707, -0.0832583979);
      r5.xyz = r3.www ? float3(0, 1, 0) : float3(-0.130257145, 1.14080286, -0.0105485283);
      r6.xyz = r3.www ? float3(0, 0, 1) : float3(-0.0240032747, -0.128968775, 1.15297174);
      r4.xyz = r3.zzz ? float3(0.695452213, 0.140678704, 0.163869068) : r4.xyz;
      r5.xyz = r3.zzz ? float3(0.0447945632, 0.859671116, 0.0955343172) : r5.xyz;
      r6.xyz = r3.zzz ? float3(-0.00552588282, 0.00402521016, 1.00150073) : r6.xyz;
      r4.xyz = r3.yyy ? float3(1.02579927, -0.0200525094, -0.00577136781) : r4.xyz;
      r5.xyz = r3.yyy ? float3(-0.00223502493, 1.00458264, -0.00235231337) : r5.xyz;
      r3.yzw = r3.yyy ? float3(-0.00501400325, -0.0252933875, 1.03044021) : r6.xyz;
      r4.xyz = r3.xxx ? float3(1.37915885, -0.308850735, -0.0703467429) : r4.xyz;
      r5.xyz = r3.xxx ? float3(-0.0693352968, 1.08229232, -0.0129620517) : r5.xyz;
      r3.xyz = r3.xxx ? float3(-0.00215925858, -0.0454653986, 1.04775953) : r3.yzw;
      r0.w = cmp(asint(cb0[61].z) == 1);
      if (r0.w != 0) {
        r6.x = dot(float3(0.613191485, 0.33951208, 0.0473663323), r1.xyz);
        r6.y = dot(float3(0.0702069029, 0.916335821, 0.0134500116), r1.xyz);
        r6.z = dot(float3(0.0206188709, 0.109567292, 0.869606733), r1.xyz);
        r7.x = dot(r4.xyz, r6.xyz);
        r7.y = dot(r5.xyz, r6.xyz);
        r7.z = dot(r3.xyz, r6.xyz);
        r6.xyz = max(float3(6.10351999e-005, 6.10351999e-005, 6.10351999e-005), r7.xyz);
        r7.xyz = float3(4.5, 4.5, 4.5) * r6.xyz;
        r6.xyz = max(float3(0.0179999992, 0.0179999992, 0.0179999992), r6.xyz);
        r6.xyz = log2(r6.xyz);
        r6.xyz = float3(0.449999988, 0.449999988, 0.449999988) * r6.xyz;
        r6.xyz = exp2(r6.xyz);
        r6.xyz = r6.xyz * float3(1.09899998, 1.09899998, 1.09899998) + float3(-0.0989999995, -0.0989999995, -0.0989999995);
        r0.xyz = min(r7.xyz, r6.xyz);
      } else {
        r6.xy = cmp(asint(cb0[61].zz) == int2(4, 6));
        r0.w = (int)r6.y | (int)r6.x;
        if (r0.w != 0) {
          r2.xyz = float3(1.5, 1.5, 1.5) * r2.xyz;
          r6.y = dot(float3(0.439700812, 0.382978052, 0.1773348), r2.xyz);
          r6.z = dot(float3(0.0897923037, 0.813423157, 0.096761629), r2.xyz);
          r6.w = dot(float3(0.0175439864, 0.111544058, 0.870704114), r2.xyz);
          r0.w = min(r6.y, r6.z);
          r0.w = min(r0.w, r6.w);
          r1.w = max(r6.y, r6.z);
          r1.w = max(r1.w, r6.w);
          r2.xy = max(float2(1.00000001e-010, 0.00999999978), r1.ww);
          r0.w = max(1.00000001e-010, r0.w);
          r0.w = r2.x + -r0.w;
          r0.w = r0.w / r2.y;
          r2.xyz = r6.wzy + -r6.zyw;
          r2.xy = r6.wz * r2.xy;
          r1.w = r2.x + r2.y;
          r1.w = r6.y * r2.z + r1.w;
          r1.w = sqrt(r1.w);
          r2.x = r6.w + r6.z;
          r2.x = r2.x + r6.y;
          r1.w = r1.w * 1.75 + r2.x;
          r2.x = 0.333333343 * r1.w;
          r2.y = -0.400000006 + r0.w;
          r2.z = 2.5 * r2.y;
          r2.z = 1 + -abs(r2.z);
          r2.z = max(0, r2.z);
          r2.w = cmp(0 < r2.y);
          r2.y = cmp(r2.y < 0);
          r2.y = (int)-r2.w + (int)r2.y;
          r2.y = (int)r2.y;
          r2.z = -r2.z * r2.z + 1;
          r2.y = r2.y * r2.z + 1;
          r2.y = 0.0250000004 * r2.y;
          r2.z = cmp(0.159999996 >= r1.w);
          r1.w = cmp(r1.w >= 0.479999989);
          r2.x = 0.0799999982 / r2.x;
          r2.x = -0.5 + r2.x;
          r2.x = r2.y * r2.x;
          r1.w = r1.w ? 0 : r2.x;
          r1.w = r2.z ? r2.y : r1.w;
          r1.w = 1 + r1.w;
          r2.yzw = r6.yzw * r1.www;
          r7.xy = cmp(r2.zw == r2.yz);
          r3.w = r7.y ? r7.x : 0;
          r4.w = r6.z * r1.w + -r2.w;
          r4.w = 1.73205078 * r4.w;
          r5.w = r2.y * 2 + -r2.z;
          r5.w = -r6.w * r1.w + r5.w;
          r6.x = min(abs(r5.w), abs(r4.w));
          r6.z = max(abs(r5.w), abs(r4.w));
          r6.z = 1 / r6.z;
          r6.x = r6.x * r6.z;
          r6.z = r6.x * r6.x;
          r6.w = r6.z * 0.0208350997 + -0.0851330012;
          r6.w = r6.z * r6.w + 0.180141002;
          r6.w = r6.z * r6.w + -0.330299497;
          r6.z = r6.z * r6.w + 0.999866009;
          r6.w = r6.x * r6.z;
          r7.x = cmp(abs(r5.w) < abs(r4.w));
          r6.w = r6.w * -2 + 1.57079637;
          r6.w = r7.x ? r6.w : 0;
          r6.x = r6.x * r6.z + r6.w;
          r6.z = cmp(r5.w < -r5.w);
          r6.z = r6.z ? -3.141593 : 0;
          r6.x = r6.x + r6.z;
          r6.z = min(r5.w, r4.w);
          r4.w = max(r5.w, r4.w);
          r5.w = cmp(r6.z < -r6.z);
          r4.w = cmp(r4.w >= -r4.w);
          r4.w = r4.w ? r5.w : 0;
          r4.w = r4.w ? -r6.x : r6.x;
          r4.w = 57.2957802 * r4.w;
          r3.w = r3.w ? 0 : r4.w;
          r4.w = cmp(r3.w < 0);
          r5.w = 360 + r3.w;
          r3.w = r4.w ? r5.w : r3.w;
          r3.w = max(0, r3.w);
          r3.w = min(360, r3.w);
          r4.w = cmp(180 < r3.w);
          r5.w = -360 + r3.w;
          r3.w = r4.w ? r5.w : r3.w;
          r4.w = cmp(-67.5 < r3.w);
          r5.w = cmp(r3.w < 67.5);
          r4.w = r4.w ? r5.w : 0;
          if (r4.w != 0) {
            r3.w = 67.5 + r3.w;
            r4.w = 0.0296296291 * r3.w;
            r5.w = (int)r4.w;
            r4.w = trunc(r4.w);
            r3.w = r3.w * 0.0296296291 + -r4.w;
            r4.w = r3.w * r3.w;
            r6.x = r4.w * r3.w;
            r7.xyz = float3(-0.166666672, -0.5, 0.166666672) * r6.xxx;
            r6.zw = r4.ww * float2(0.5, 0.5) + r7.xy;
            r6.zw = r3.ww * float2(-0.5, 0.5) + r6.zw;
            r3.w = r6.x * 0.5 + -r4.w;
            r3.w = 0.666666687 + r3.w;
            r7.xyw = cmp((int3)r5.www == int3(3, 2, 1));
            r6.xz = float2(0.166666672, 0.166666672) + r6.zw;
            r4.w = r5.w ? 0 : r7.z;
            r4.w = r7.w ? r6.z : r4.w;
            r3.w = r7.y ? r3.w : r4.w;
            r3.w = r7.x ? r6.x : r3.w;
          } else {
            r3.w = 0;
          }
          r0.w = r3.w * r0.w;
          r0.w = 1.5 * r0.w;
          r1.w = -r6.y * r1.w + 0.0299999993;
          r0.w = r1.w * r0.w;
          r2.x = r0.w * 0.180000007 + r2.y;
          r2.xyz = max(float3(0, 0, 0), r2.xzw);
          r2.xyz = min(float3(65535, 65535, 65535), r2.xyz);
          r6.x = dot(float3(1.45143926, -0.236510754, -0.214928567), r2.xyz);
          r6.y = dot(float3(-0.0765537769, 1.17622972, -0.0996759236), r2.xyz);
          r6.z = dot(float3(0.00831614807, -0.00603244966, 0.997716308), r2.xyz);
          r2.xyz = max(float3(0, 0, 0), r6.xyz);
          r2.xyz = min(float3(65535, 65535, 65535), r2.xyz);
          r0.w = dot(r2.xyz, float3(0.272228718, 0.674081743, 0.0536895171));
          r2.xyz = r2.xyz + -r0.www;
          r2.xyz = r2.xyz * float3(0.959999979, 0.959999979, 0.959999979) + r0.www;
          r6.xyz = cmp(float3(0, 0, 0) >= r2.xyz);
          r2.xyz = log2(r2.xyz);
          r2.xyz = r6.xyz ? float3(-14, -14, -14) : r2.xyz;
          r6.xyz = cmp(float3(-17.4739323, -17.4739323, -17.4739323) >= r2.xyz);
          if (r6.x != 0) {
            r0.w = -4;
          } else {
            r1.w = cmp(-17.4739323 < r2.x);
            r2.w = cmp(r2.x < -2.47393107);
            r1.w = r1.w ? r2.w : 0;
            if (r1.w != 0) {
              r1.w = r2.x * 0.30103001 + 5.26017761;
              r2.w = 0.664385557 * r1.w;
              r3.w = (int)r2.w;
              r2.w = trunc(r2.w);
              r7.y = r1.w * 0.664385557 + -r2.w;
              r6.xw = (int2)r3.ww + int2(1, 2);
              r7.x = r7.y * r7.y;
              r8.x = icb[r3.w + 0].x;
              r8.y = icb[r6.x + 0].x;
              r8.z = icb[r6.w + 0].x;
              r9.x = dot(r8.xzy, float3(0.5, 0.5, -1));
              r9.y = dot(r8.xy, float2(-1, 1));
              r9.z = dot(r8.xy, float2(0.5, 0.5));
              r7.z = 1;
              r0.w = dot(r7.xyz, r9.xyz);
            } else {
              r1.w = cmp(r2.x >= -2.47393107);
              r2.w = cmp(r2.x < 15.5260687);
              r1.w = r1.w ? r2.w : 0;
              if (r1.w != 0) {
                r1.w = r2.x * 0.30103001 + 0.744727492;
                r2.x = 0.553654671 * r1.w;
                r2.w = (int)r2.x;
                r2.x = trunc(r2.x);
                r7.y = r1.w * 0.553654671 + -r2.x;
                r6.xw = (int2)r2.ww + int2(1, 2);
                r7.x = r7.y * r7.y;
                r8.x = icb[r2.w + 0].y;
                r8.y = icb[r6.x + 0].y;
                r8.z = icb[r6.w + 0].y;
                r9.x = dot(r8.xzy, float3(0.5, 0.5, -1));
                r9.y = dot(r8.xy, float2(-1, 1));
                r9.z = dot(r8.xy, float2(0.5, 0.5));
                r7.z = 1;
                r0.w = dot(r7.xyz, r9.xyz);
              } else {
                r0.w = 4;
              }
            }
          }
          r0.w = 3.32192802 * r0.w;
          r7.x = exp2(r0.w);
          if (r6.y != 0) {
            r0.w = -4;
          } else {
            r1.w = cmp(-17.4739323 < r2.y);
            r2.x = cmp(r2.y < -2.47393107);
            r1.w = r1.w ? r2.x : 0;
            if (r1.w != 0) {
              r1.w = r2.y * 0.30103001 + 5.26017761;
              r2.x = 0.664385557 * r1.w;
              r2.w = (int)r2.x;
              r2.x = trunc(r2.x);
              r8.y = r1.w * 0.664385557 + -r2.x;
              r6.xy = (int2)r2.ww + int2(1, 2);
              r8.x = r8.y * r8.y;
              r9.x = icb[r2.w + 0].x;
              r9.y = icb[r6.x + 0].x;
              r9.z = icb[r6.y + 0].x;
              r10.x = dot(r9.xzy, float3(0.5, 0.5, -1));
              r10.y = dot(r9.xy, float2(-1, 1));
              r10.z = dot(r9.xy, float2(0.5, 0.5));
              r8.z = 1;
              r0.w = dot(r8.xyz, r10.xyz);
            } else {
              r1.w = cmp(r2.y >= -2.47393107);
              r2.x = cmp(r2.y < 15.5260687);
              r1.w = r1.w ? r2.x : 0;
              if (r1.w != 0) {
                r1.w = r2.y * 0.30103001 + 0.744727492;
                r2.x = 0.553654671 * r1.w;
                r2.y = (int)r2.x;
                r2.x = trunc(r2.x);
                r8.y = r1.w * 0.553654671 + -r2.x;
                r2.xw = (int2)r2.yy + int2(1, 2);
                r8.x = r8.y * r8.y;
                r9.x = icb[r2.y + 0].y;
                r9.y = icb[r2.x + 0].y;
                r9.z = icb[r2.w + 0].y;
                r10.x = dot(r9.xzy, float3(0.5, 0.5, -1));
                r10.y = dot(r9.xy, float2(-1, 1));
                r10.z = dot(r9.xy, float2(0.5, 0.5));
                r8.z = 1;
                r0.w = dot(r8.xyz, r10.xyz);
              } else {
                r0.w = 4;
              }
            }
          }
          r0.w = 3.32192802 * r0.w;
          r7.y = exp2(r0.w);
          if (r6.z != 0) {
            r0.w = -4;
          } else {
            r1.w = cmp(-17.4739323 < r2.z);
            r2.x = cmp(r2.z < -2.47393107);
            r1.w = r1.w ? r2.x : 0;
            if (r1.w != 0) {
              r1.w = r2.z * 0.30103001 + 5.26017761;
              r2.x = 0.664385557 * r1.w;
              r2.y = (int)r2.x;
              r2.x = trunc(r2.x);
              r6.y = r1.w * 0.664385557 + -r2.x;
              r2.xw = (int2)r2.yy + int2(1, 2);
              r6.x = r6.y * r6.y;
              r8.x = icb[r2.y + 0].x;
              r8.y = icb[r2.x + 0].x;
              r8.z = icb[r2.w + 0].x;
              r9.x = dot(r8.xzy, float3(0.5, 0.5, -1));
              r9.y = dot(r8.xy, float2(-1, 1));
              r9.z = dot(r8.xy, float2(0.5, 0.5));
              r6.z = 1;
              r0.w = dot(r6.xyz, r9.xyz);
            } else {
              r1.w = cmp(r2.z >= -2.47393107);
              r2.x = cmp(r2.z < 15.5260687);
              r1.w = r1.w ? r2.x : 0;
              if (r1.w != 0) {
                r1.w = r2.z * 0.30103001 + 0.744727492;
                r2.x = 0.553654671 * r1.w;
                r2.y = (int)r2.x;
                r2.x = trunc(r2.x);
                r6.y = r1.w * 0.553654671 + -r2.x;
                r2.xz = (int2)r2.yy + int2(1, 2);
                r6.x = r6.y * r6.y;
                r8.x = icb[r2.y + 0].y;
                r8.y = icb[r2.x + 0].y;
                r8.z = icb[r2.z + 0].y;
                r2.x = dot(r8.xzy, float3(0.5, 0.5, -1));
                r2.y = dot(r8.xy, float2(-1, 1));
                r2.z = dot(r8.xy, float2(0.5, 0.5));
                r6.z = 1;
                r0.w = dot(r6.xyz, r2.xyz);
              } else {
                r0.w = 4;
              }
            }
          }
          r0.w = 3.32192802 * r0.w;
          r7.z = exp2(r0.w);
          r2.x = dot(float3(0.695452213, 0.140678704, 0.163869068), r7.xyz);
          r2.y = dot(float3(0.0447945632, 0.859671116, 0.0955343172), r7.xyz);
          r2.z = dot(float3(-0.00552588282, 0.00402521016, 1.00150073), r7.xyz);
          r0.w = dot(float3(1.45143926, -0.236510754, -0.214928567), r2.xyz);
          r1.w = dot(float3(-0.0765537769, 1.17622972, -0.0996759236), r2.xyz);
          r2.x = dot(float3(0.00831614807, -0.00603244966, 0.997716308), r2.xyz);
          r2.y = cmp(0 >= r0.w);
          r0.w = log2(r0.w);
          r0.w = r2.y ? -13.2877121 : r0.w;
          r2.y = cmp(-12.7838678 >= r0.w);
          if (r2.y != 0) {
            r2.y = -2.30102992;
          } else {
            r2.z = cmp(-12.7838678 < r0.w);
            r2.w = cmp(r0.w < 2.26303458);
            r2.z = r2.w ? r2.z : 0;
            if (r2.z != 0) {
              r2.z = r0.w * 0.30103001 + 3.84832764;
              r2.w = 1.54540098 * r2.z;
              r3.w = (int)r2.w;
              r2.w = trunc(r2.w);
              r6.y = r2.z * 1.54540098 + -r2.w;
              r2.zw = (int2)r3.ww + int2(1, 2);
              r6.x = r6.y * r6.y;
              r7.x = icb[r3.w + 0].z;
              r7.y = icb[r2.z + 0].z;
              r7.z = icb[r2.w + 0].z;
              r8.x = dot(r7.xzy, float3(0.5, 0.5, -1));
              r8.y = dot(r7.xy, float2(-1, 1));
              r8.z = dot(r7.xy, float2(0.5, 0.5));
              r6.z = 1;
              r2.y = dot(r6.xyz, r8.xyz);
            } else {
              r2.z = cmp(r0.w >= 2.26303458);
              r2.w = cmp(r0.w < 12.4948215);
              r2.z = r2.w ? r2.z : 0;
              if (r2.z != 0) {
                r2.z = r0.w * 0.30103001 + -0.681241274;
                r2.w = 2.27267218 * r2.z;
                r3.w = (int)r2.w;
                r2.w = trunc(r2.w);
                r6.y = r2.z * 2.27267218 + -r2.w;
                r2.zw = (int2)r3.ww + int2(1, 2);
                r6.x = r6.y * r6.y;
                r7.x = icb[r3.w + 0].w;
                r7.y = icb[r2.z + 0].w;
                r7.z = icb[r2.w + 0].w;
                r8.x = dot(r7.xzy, float3(0.5, 0.5, -1));
                r8.y = dot(r7.xy, float2(-1, 1));
                r8.z = dot(r7.xy, float2(0.5, 0.5));
                r6.z = 1;
                r2.y = dot(r6.xyz, r8.xyz);
              } else {
                r2.y = r0.w * 0.0361235999 + 2.84967208;
              }
            }
          }
          r0.w = 3.32192802 * r2.y;
          r6.x = exp2(r0.w);
          r0.w = cmp(0 >= r1.w);
          r1.w = log2(r1.w);
          r0.w = r0.w ? -13.2877121 : r1.w;
          r1.w = cmp(-12.7838678 >= r0.w);
          if (r1.w != 0) {
            r1.w = -2.30102992;
          } else {
            r2.y = cmp(-12.7838678 < r0.w);
            r2.z = cmp(r0.w < 2.26303458);
            r2.y = r2.z ? r2.y : 0;
            if (r2.y != 0) {
              r2.y = r0.w * 0.30103001 + 3.84832764;
              r2.z = 1.54540098 * r2.y;
              r2.w = (int)r2.z;
              r2.z = trunc(r2.z);
              r7.y = r2.y * 1.54540098 + -r2.z;
              r2.yz = (int2)r2.ww + int2(1, 2);
              r7.x = r7.y * r7.y;
              r8.x = icb[r2.w + 0].z;
              r8.y = icb[r2.y + 0].z;
              r8.z = icb[r2.z + 0].z;
              r9.x = dot(r8.xzy, float3(0.5, 0.5, -1));
              r9.y = dot(r8.xy, float2(-1, 1));
              r9.z = dot(r8.xy, float2(0.5, 0.5));
              r7.z = 1;
              r1.w = dot(r7.xyz, r9.xyz);
            } else {
              r2.y = cmp(r0.w >= 2.26303458);
              r2.z = cmp(r0.w < 12.4948215);
              r2.y = r2.z ? r2.y : 0;
              if (r2.y != 0) {
                r2.y = r0.w * 0.30103001 + -0.681241274;
                r2.z = 2.27267218 * r2.y;
                r2.w = (int)r2.z;
                r2.z = trunc(r2.z);
                r7.y = r2.y * 2.27267218 + -r2.z;
                r2.yz = (int2)r2.ww + int2(1, 2);
                r7.x = r7.y * r7.y;
                r8.x = icb[r2.w + 0].w;
                r8.y = icb[r2.y + 0].w;
                r8.z = icb[r2.z + 0].w;
                r9.x = dot(r8.xzy, float3(0.5, 0.5, -1));
                r9.y = dot(r8.xy, float2(-1, 1));
                r9.z = dot(r8.xy, float2(0.5, 0.5));
                r7.z = 1;
                r1.w = dot(r7.xyz, r9.xyz);
              } else {
                r1.w = r0.w * 0.0361235999 + 2.84967208;
              }
            }
          }
          r0.w = 3.32192802 * r1.w;
          r6.y = exp2(r0.w);
          r0.w = cmp(0 >= r2.x);
          r1.w = log2(r2.x);
          r0.w = r0.w ? -13.2877121 : r1.w;
          r1.w = cmp(-12.7838678 >= r0.w);
          if (r1.w != 0) {
            r1.w = -2.30102992;
          } else {
            r2.x = cmp(-12.7838678 < r0.w);
            r2.y = cmp(r0.w < 2.26303458);
            r2.x = r2.y ? r2.x : 0;
            if (r2.x != 0) {
              r2.x = r0.w * 0.30103001 + 3.84832764;
              r2.y = 1.54540098 * r2.x;
              r2.z = (int)r2.y;
              r2.y = trunc(r2.y);
              r7.y = r2.x * 1.54540098 + -r2.y;
              r2.xy = (int2)r2.zz + int2(1, 2);
              r7.x = r7.y * r7.y;
              r8.x = icb[r2.z + 0].z;
              r8.y = icb[r2.x + 0].z;
              r8.z = icb[r2.y + 0].z;
              r2.x = dot(r8.xzy, float3(0.5, 0.5, -1));
              r2.y = dot(r8.xy, float2(-1, 1));
              r2.z = dot(r8.xy, float2(0.5, 0.5));
              r7.z = 1;
              r1.w = dot(r7.xyz, r2.xyz);
            } else {
              r2.x = cmp(r0.w >= 2.26303458);
              r2.y = cmp(r0.w < 12.4948215);
              r2.x = r2.y ? r2.x : 0;
              if (r2.x != 0) {
                r2.x = r0.w * 0.30103001 + -0.681241274;
                r2.y = 2.27267218 * r2.x;
                r2.z = (int)r2.y;
                r2.y = trunc(r2.y);
                r7.y = r2.x * 2.27267218 + -r2.y;
                r2.xy = (int2)r2.zz + int2(1, 2);
                r7.x = r7.y * r7.y;
                r8.x = icb[r2.z + 0].w;
                r8.y = icb[r2.x + 0].w;
                r8.z = icb[r2.y + 0].w;
                r2.x = dot(r8.xzy, float3(0.5, 0.5, -1));
                r2.y = dot(r8.xy, float2(-1, 1));
                r2.z = dot(r8.xy, float2(0.5, 0.5));
                r7.z = 1;
                r1.w = dot(r7.xyz, r2.xyz);
              } else {
                r1.w = r0.w * 0.0361235999 + 2.84967208;
              }
            }
          }
          r0.w = 3.32192802 * r1.w;
          r6.z = exp2(r0.w);
          r2.x = dot(r4.xyz, r6.xyz);
          r2.y = dot(r5.xyz, r6.xyz);
          r2.z = dot(r3.xyz, r6.xyz);
          r2.xyz = float3(9.99999975e-005, 9.99999975e-005, 9.99999975e-005) * r2.xyz;
          r2.xyz = log2(r2.xyz);
          r2.xyz = float3(0.159301758, 0.159301758, 0.159301758) * r2.xyz;
          r2.xyz = exp2(r2.xyz);
          r6.xyz = r2.xyz * float3(18.8515625, 18.8515625, 18.8515625) + float3(0.8359375, 0.8359375, 0.8359375);
          r2.xyz = r2.xyz * float3(18.6875, 18.6875, 18.6875) + float3(1, 1, 1);
          r2.xyz = rcp(r2.xyz);
          r2.xyz = r6.xyz * r2.xyz;
          r2.xyz = log2(r2.xyz);
          r2.xyz = float3(78.84375, 78.84375, 78.84375) * r2.xyz;
          r0.xyz = exp2(r2.xyz);
        } else {
          r2.x = dot(float3(0.613191485, 0.33951208, 0.0473663323), r1.xyz);
          r2.y = dot(float3(0.0702069029, 0.916335821, 0.0134500116), r1.xyz);
          r2.z = dot(float3(0.0206188709, 0.109567292, 0.869606733), r1.xyz);
          r0.w = dot(r4.xyz, r2.xyz);
          r1.x = dot(r5.xyz, r2.xyz);
          r1.y = dot(r3.xyz, r2.xyz);
          r2.x = log2(r0.w);
          r2.y = log2(r1.x);
          r2.z = log2(r1.y);
          r1.xyz = cb0[18].zzz * r2.xyz;
          r0.xyz = exp2(r1.xyz);
        }
      }
    }
    r0.xyz = cb0[37].www * -cb0[37].xyz + r0.xyz;
    o0.xyz = float3(0.952381015, 0.952381015, 0.952381015) * r0.xyz;
    o0.w = 0;
  }

  o0 = saturate(o0);

  return;
}
