// ---- Created with 3Dmigoto v1.4.1 on Fri Jun 27 15:19:40 2025



// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_POSITION0,
  float4 v1 : COLOR0,
  out float4 o0 : SV_Target0)
{
  o0.xyzw = v1.xyzw;
  return;
}