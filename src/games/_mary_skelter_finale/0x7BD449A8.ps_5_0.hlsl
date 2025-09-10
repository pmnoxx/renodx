// ---- Created with 3Dmigoto v1.4.1 on Fri Jun 27 15:11:07 2025

cbuffer _Globals : register(b0)
{
  row_major float4x4 Mk_ViewProjection : packoffset(c0);
  float4 ParamUVAdd : packoffset(c4);
  float4 ParamColorAll : packoffset(c5);
}

SamplerState Mk_TexSamp_Tex1_state_s : register(s0);
Texture2D<float4> Mk_TexSamp_Tex1 : register(t0);


// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xyzw = Mk_TexSamp_Tex1.Sample(Mk_TexSamp_Tex1_state_s, v1.xy).xyzw;
  o0.xyzw = ParamColorAll.xyzw * r0.xyzw;
  return;
}