#pragma once
#include "./swap_chain_proxy_pixel_shader_dx11.h"
#include "./swap_chain_proxy_pixel_shader_dx12.h"
#include "./swap_chain_proxy_vertex_shader_dx11.h"
#include "./swap_chain_proxy_vertex_shader_dx12.h"
#include "./0x45A96F2D.h"
#include "./0x90C6B12E.h"
#define __CUSTOM_SHADER_ENTRIES \
  CustomShaderEntry(0x45A96F2D), \
  CustomShaderEntry(0x90C6B12E)
#define __ALL_CUSTOM_SHADERS \
  __CUSTOM_SHADER_ENTRIES
