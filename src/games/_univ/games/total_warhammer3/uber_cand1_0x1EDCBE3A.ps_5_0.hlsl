#include "../../custom.hlsl"

// ---- Created with 3Dmigoto v1.4.1 on Tue Sep 23 19:19:44 2025

struct Auto_exposure_output
{
    float tone_mapper_brightness;  // Offset:    0
};

cbuffer colorimetry_VS_PS : register(b0)
{
  float g_brightness : packoffset(c0);
  float g_gamma_output : packoffset(c0.y);
  float g_inv_gamma_output : packoffset(c0.z);
}

cbuffer camera : register(b1)
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

cbuffer lighting_VS_PS : register(b2)
{
  bool g_apply_environment_specular : packoffset(c0);
  float3 sun_direction : packoffset(c0.y);
  float3 sun_colour : packoffset(c1);
  float sun_specular : packoffset(c1.w);
  float3 ambient_cube_lr[2] : packoffset(c2);
  float3 ambient_cube_tb[2] : packoffset(c4);
  float3 ambient_cube_fb[2] : packoffset(c6);
  float3 g_deep_water_colour : packoffset(c8);
  float3 g_shallow_water_colour : packoffset(c9);
  float3 g_sea_bed_light_scatter : packoffset(c10);
  float g_refraction_light_scatter : packoffset(c10.w);
  int g_ssr_enabled : packoffset(c11);
  bool g_gi_enabled : packoffset(c11.y);
  bool g_use_spherical_harmonics : packoffset(c11.z);
  bool g_use_spherical_harmonics_array : packoffset(c11.w);
  float2 g_cloud_shadow_direction : packoffset(c12);
  float g_cloud_shadow_speed : packoffset(c12.z);
  float g_cloud_shadow_scale : packoffset(c12.w);
  float g_cloud_shadow_lerp : packoffset(c13);
  float2 g_noise_uv_shift : packoffset(c13.y);
  bool g_skin_enable : packoffset(c13.w);
  float2 g_skin_curvature_scale_bias : packoffset(c14);
  float2 g_skin_translucency_scale_bias : packoffset(c14.z);
  float3 g_skin_blood_colour : packoffset(c15);
  float4 g_world_bounds : packoffset(c16);
  float4 g_playable_area_bounds : packoffset(c17);
  float g_vegetation_wrap_lighting_bias : packoffset(c18);
  float g_spherical_harmonic_terms : packoffset(c18.y);
  float g_spherical_harmonic_fadeout : packoffset(c18.z);
  float g_ambient_fudge_factor : packoffset(c18.w);
  float g_time_of_day_unary : packoffset(c19);
  float terrain_shadow_softening : packoffset(c19.y);
  float terrain_shadow_bias_depth_threshold : packoffset(c19.z);
  float terrain_shadow_full_bias : packoffset(c19.w);
  uint distortion_blendmode : packoffset(c20);
  float g_irradiance_voxel_size[4] : packoffset(c21);
  float3 g_irradiance_world_offsets[4] : packoffset(c25);
  uint g_debug_lighting : packoffset(c28.w);
  float g_environment_specular_shadow_ratio : packoffset(c29);
  float g_vfx_global_emissive_brightness_multiplier : packoffset(c29.y);
  float g_sun_scale : packoffset(c29.z);
  float g_sun_brightness_factor : packoffset(c29.w);
}

cbuffer tone_mapping : register(b3)
{
  float g_tone_mapping_brightness : packoffset(c0);
  float g_tone_mapping_burn : packoffset(c0.y);
  int g_use_auto_exposure : packoffset(c0.z);
}

cbuffer hdr_to_screen_PS : register(b4)
{
  float4x4 colour_matrix : packoffset(c0);
  float lut_weights[3] : packoffset(c4);
  float overscan : packoffset(c6.y);
  int require_distortion_composition : packoffset(c6.z);
  float near_lut_strength : packoffset(c6.w);
  float far_lut_strength : packoffset(c7);
  float near_lut_distance : packoffset(c7.y);
  float far_lut_distance : packoffset(c7.z);
}

SamplerState g_hdr_rgb_texture_sampler_s : register(s0);
SamplerState g_hdr_rgb_bloom_texture_sampler_s : register(s1);
SamplerState distortion_sampler_s : register(s2);
SamplerState lut_sampler_s : register(s3);
Texture2D<float4> gbuffer_channel_3_texture : register(t0);
Texture2D<float4> gbuffer_channel_4_texture : register(t1);
Texture2D<float4> g_hdr_rgb_texture : register(t2);
Texture2D<float4> g_hdr_rgb_bloom_texture : register(t3);
StructuredBuffer<Auto_exposure_output> g_auto_exposure_input_buffer : register(t4);
Texture2D<float4> distortion_texture : register(t5);
Texture3D<float4> lut_texture_1 : register(t6);
Texture3D<float4> lut_texture_2 : register(t7);
Texture3D<float4> lut_texture_3 : register(t8);
Texture3D<float4> lut_secondary_texture_1 : register(t9);
Texture3D<float4> lut_secondary_texture_2 : register(t10);
Texture3D<float4> lut_secondary_texture_3 : register(t11);


// 3Dmigoto declarations
#define cmp -


void main(
  float4 v0 : SV_Position0,
  float2 v1 : TEXCOORD0,
  out float4 o0 : SV_Target0)
{
  float4 r0,r1,r2,r3,r4,r5;
  uint4 bitmask, uiDest;
  float4 fDest;

  r0.xy = g_screen_size.zw * v0.xy;
  r0.zw = overscan * r0.xy;
  r1.xy = overscan * g_screen_size.zw;
  r1.zw = float2(0.5,0.5) * r1.xy;
  r0.xy = r0.xy * overscan + -r1.zw;
  r2.xyzw = distortion_texture.SampleLevel(distortion_sampler_s, r0.xy, 0).xyzw;
  r1.zw = r2.ww * -r2.xy + r2.xy;
  r1.zw = r1.zw * float2(0.0199999996,0.0199999996) + r0.xy;
  r3.x = cmp(distortion_blendmode == 1);
  r3.y = cmp(r2.z < 0.00100000005);
  r2.z = r3.y ? 1 : r2.z;
  r2.xy = r2.xy / r2.zz;
  r2.z = 1 + -r2.w;
  r2.xy = r2.zz * r2.xy;
  r2.xy = r2.ww * -r2.xy + r2.xy;
  r2.xy = r2.xy * float2(0.0199999996,0.0199999996) + r0.xy;
  r0.xy = r3.xx ? r2.xy : r0.xy;
  r0.xy = distortion_blendmode ? r0.xy : r1.zw;
  r0.xy = r1.xy * float2(0.5,0.5) + r0.xy;
  r0.xy = require_distortion_composition ? r0.xy : r0.zw;
  r1.xyz = g_hdr_rgb_texture.SampleLevel(g_hdr_rgb_texture_sampler_s, r0.xy, 0).xyz;
  r2.xyz = g_hdr_rgb_bloom_texture.SampleLevel(g_hdr_rgb_bloom_texture_sampler_s, r0.xy, 0).xyz;
  r1.xyz = r2.xyz + r1.xyz;
  r0.z = g_auto_exposure_input_buffer[0].tone_mapper_brightness;
  r0.z = g_use_auto_exposure ? r0.z : g_tone_mapping_brightness;
  r0.w = dot(r1.xyz, float3(0.212599993,0.715200007,0.0722000003));
  r1.w = cmp(r0.w == 0.000000);
  r0.z = r0.z * r0.z;
  r2.x = r0.z * r0.w;
  r2.y = 1 / g_tone_mapping_burn;
  r2.y = -0.999000013 + r2.y;
  r2.y = r2.x / r2.y;
  r2.y = 1 + r2.y;
  r2.x = r2.x * r2.y;
  r0.z = r0.z * r0.w + 1;
  r0.z = r2.x / r0.z;
  r1.xyz = r1.xyz * r0.zzz;
  r2.xyz = r1.xyz / r0.www;
  r2.w = 1;
  r1.xyzw = r1.wwww ? float4(0,0,0,0) : r2.xyzw;
  gbuffer_channel_4_texture.GetDimensions(0, uiDest.x, uiDest.y, uiDest.z);
  r0.zw = uiDest.xy;
  r2.xy = (uint2)r0.zw;
  r2.xy = r2.xy * r0.xy;
  r2.xy = floor(r2.xy);
  r2.xy = (int2)r2.xy;
  r0.zw = (int2)r0.zw + int2(-1,-1);
  r2.xy = max(int2(0,0), (int2)r2.xy);
  r2.xy = min((int2)r2.xy, (int2)r0.zw);

 /* if (true) {
    o0.xyz = g_brightness * r1.xyz;
    o0.w = 1;
    o0.xyz *= 5.f;
    return;
  }*/

 // r3.xyz = saturate(g_brightness * r1.xyz);
  r3.xyz = max(0.f, g_brightness * r1.xyz); // still in linear space
  float3 untonemapped = r3.xyz;
  if (RENODX_TONE_MAP_TYPE > 0.f) {
    r3.xyz = RestoreHighlightSaturation(untonemapped.xyz);
  }

  r3.xyz = pow(r3.xyz, g_inv_gamma_output); // color space is now gamma encoded

  r3.w = 1;
  r4.x = dot(r3.xyzw, colour_matrix._m00_m10_m20_m30);
  r4.y = dot(r3.xyzw, colour_matrix._m01_m11_m21_m31);
  r4.z = dot(r3.xyzw, colour_matrix._m02_m12_m22_m32);
  r2.zw = float2(0,0);
  r0.z = gbuffer_channel_3_texture.Load(r2.xyw).w;
  r0.z = 255 * r0.z;
  r0.z = round(r0.z);
  r0.z = (uint)r0.z;
  r3.xyz = lut_texture_1.SampleLevel(lut_sampler_s, r4.xyz, 0).xyz;
  r5.xyz = lut_texture_2.SampleLevel(lut_sampler_s, r4.xyz, 0).xyz;
  r5.xyz = lut_weights[1] * r5.xyz;
  r3.xyz = r3.xyz * lut_weights[0] + r5.xyz;
  r5.xyz = lut_texture_3.SampleLevel(lut_sampler_s, r4.xyz, 0).xyz;
  r1.xyz = r5.xyz * lut_weights[2] + r3.xyz;
  r0.z = (int)r0.z & 2;
  if (r0.z != 0) {
    r2.z = gbuffer_channel_4_texture.Load(r2.xyz).x;
    r2.xy = r0.xy * float2(2,-2) + float2(-1,1);
    r2.w = 1;
    r0.x = dot(r2.xyzw, inv_view_projection._m00_m10_m20_m30);
    r0.y = dot(r2.xyzw, inv_view_projection._m01_m11_m21_m31);
    r0.z = dot(r2.xyzw, inv_view_projection._m02_m12_m22_m32);
    r0.w = dot(r2.xyzw, inv_view_projection._m03_m13_m23_m33);
    r0.xyz = r0.xyz / r0.www;
    r0.xyz = -camera_position.xyz + r0.xyz;
    r0.x = dot(r0.xyz, r0.xyz);
    r0.x = sqrt(r0.x);
    r0.y = far_lut_distance + -near_lut_distance;
    r0.x = -near_lut_distance + r0.x;
    r0.y = 1 / r0.y;
    r0.x = saturate(r0.x * r0.y);
    r0.y = r0.x * -2 + 3;
    r0.x = r0.x * r0.x;
    r0.x = r0.y * r0.x;
    r0.y = far_lut_strength + -near_lut_strength;
    r0.x = r0.x * r0.y + near_lut_strength;
    r0.yzw = lut_secondary_texture_1.SampleLevel(lut_sampler_s, r4.xyz, 0).xyz;
    r2.xyz = lut_secondary_texture_2.SampleLevel(lut_sampler_s, r4.xyz, 0).xyz;
    r2.xyz = lut_weights[1] * r2.xyz;
    r0.yzw = r0.yzw * lut_weights[0] + r2.xyz;
    r2.xyz = lut_secondary_texture_3.SampleLevel(lut_sampler_s, r4.xyz, 0).xyz;
    r0.yzw = r2.xyz * lut_weights[2] + r0.yzw;
    r0.yzw = r0.yzw + -r1.xyz;
    r1.xyz = r0.xxx * r0.yzw + r1.xyz;
  }

  // color is still in gamma encoded space

  o0.xyzw = r1.xyzw;
  if (RENODX_TONE_MAP_TYPE > 0.f) {
    o0.xyz = pow(o0.xyz, 1.0f / g_inv_gamma_output); // gamma decode

    float old_y = renodx::color::y::from::BT709(RestoreHighlightSaturation(untonemapped.xyz));
    float new_y = renodx::color::y::from::BT709(o0.xyz);

    o0.xyz *= lerp(old_y, new_y, saturate(old_y)) / max(0.001, new_y);



    o0.xyz = ToneMapPassCustom(untonemapped.xyz, o0.xyz, RestoreHighlightSaturation(untonemapped.xyz));
    o0.xyz = pow(o0.xyz, g_inv_gamma_output); // gamma encode
  }
  o0 = max(o0, 0.f); // remove nans
  return;
}