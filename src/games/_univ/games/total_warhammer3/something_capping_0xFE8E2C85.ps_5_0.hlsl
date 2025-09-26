// ---- Created with 3Dmigoto v1.4.1 on Tue Sep 23 19:31:06 2025

cbuffer camera : register(b0)
{
  float3 camera_position : packoffset(c0);
  float3 prev_camera_position : packoffset(c1);
  float4x4 view : packoffset(c2);
  float4x4 projection : packoffset(c6);
  float4x4 view_projection : packoffset(c10);
  float4x4 prev_view_projection : packoffset(c14);
  float4x4 inv_view : packoffset(c18);
  float4x4 prev_inv_view : packoffset(c22);
  float4x4 inv_projection : packoffset(c26);
  float4x4 inv_view_projection : packoffset(c30);
  float4 camera_near_far : packoffset(c34);
  float time_in_sec : packoffset(c35);
  float prev_time_in_sec : packoffset(c35.y);
  float real_time_in_sec : packoffset(c35.z);
  float update_time_in_sec : packoffset(c35.w);
  float2 g_inverse_focal_length : packoffset(c36);
  float g_vertical_fov : packoffset(c36.z);
  float g_aspect_ratio : packoffset(c36.w);
  float4 g_screen_size : packoffset(c37);
  float g_vpos_texel_offset : packoffset(c38);
  float4 g_viewport_dimensions : packoffset(c39);
  float2 g_viewport_origin : packoffset(c40);
  float4 g_render_target_dimensions : packoffset(c41);
  float4 g_camera_temp0 : packoffset(c42);
  float4 g_camera_temp1 : packoffset(c43);
  float4 g_camera_temp2 : packoffset(c44);
  float4 g_clip_rect : packoffset(c45);
  float3 g_vr_head_rotation : packoffset(c46);
  int g_num_of_samples : packoffset(c46.w);
  float g_supersampling : packoffset(c47);
  float4 g_mouse_position : packoffset(c48);
  float3 g_frustum_points[8] : packoffset(c49);
  float4 g_frustum_planes[6] : packoffset(c57);
  float g_orthographic : packoffset(c63);
  float g_overlay_lerp : packoffset(c63.y);
  float g_overlay_parchment_lerp : packoffset(c63.z);
  float g_overlay_show_details : packoffset(c63.w);
  float g_amount_shadow_in_far_distance : packoffset(c64);
  float2 g_camera_jitter : packoffset(c64.y);
  float2 g_prev_camera_jitter : packoffset(c65);
  uint g_debug_visualization_mode : packoffset(c65.z);
  bool g_taa_is_enabled : packoffset(c65.w);
}

cbuffer SHARPEN_CONSTANTS : register(b1)
{
  float g_sharpening_strength : packoffset(c0);
}

Texture2D<float3> t_frame : register(t0);


// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_Position0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.zw = float2(0,0);
  r1.xyzw = float4(-1,-1,-1,-1) + g_render_target_dimensions.xyxy;
  r1.xyzw = (int4)r1.xyzw;
  r2.xy = (int2)v0.xy;
  r3.xyzw = (int4)r2.xyxy + int4(0,-1,-1,0);
  r3.xyzw = max(int4(0,0,0,0), (int4)r3.xyzw);
  r3.xyzw = min((int4)r3.zwxy, (int4)r1.zwxy);
  r0.xy = r3.zw;
  r0.xyz = t_frame.Load(r0.xyz).xyz;
  r3.zw = float2(0,0);
  r3.xyz = t_frame.Load(r3.xyz).xyz;
  r0.xyz = -r3.xyz + -r0.xyz;
  r2.zw = float2(0,0);
  r3.xyz = t_frame.Load(r2.xyz).xyz;
  r2.xyzw = (int4)r2.xyxy + int4(1,0,0,1);
  r2.xyzw = max(int4(0,0,0,0), (int4)r2.xyzw);
  r1.xyzw = min((int4)r2.zwxy, (int4)r1.zwxy);
  r0.xyz = r3.xyz * float3(5,5,5) + r0.xyz;
  r2.xy = r1.zw;
  r2.zw = float2(0,0);
  r2.xyz = t_frame.Load(r2.xyz).xyz;
  r0.xyz = -r2.xyz + r0.xyz;
  r1.zw = float2(0,0);
  r1.xyz = t_frame.Load(r1.xyz).xyz;
  r0.xyz = -r1.xyz + r0.xyz;
  r0.xyz = r0.xyz + -r3.xyz;
  //o0.xyz = saturate(g_sharpening_strength * r0.xyz + r3.xyz);
  o0.xyz = (g_sharpening_strength * r0.xyz + r3.xyz);
  o0.w = 1;
  return;
}