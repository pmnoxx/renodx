/*
 * Copyright (C) 2024 Carlos Lopez
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <atomic>
#include <cstdint>
#include <regex>
#include <mutex>
#include <span>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <crc32_hash.hpp>
#include <include/reshade.hpp>

#include "./device.hpp"
#include "./format.hpp"
#include "./path.hpp"
#include "./shader.hpp"
#include "./shader_compiler_directx.hpp"

namespace renodx::utils::shader::dump {

static std::atomic_bool use_auto_dump = true;
static std::atomic_size_t pending_dump_count = 0;

namespace internal {
struct ShaderInfo {
  std::vector<uint8_t> data;
  reshade::api::pipeline_subobject_type type;
};

static std::unordered_set<uint32_t> shaders_dumped;
static std::unordered_set<uint32_t> shaders_seen;
static std::unordered_map<uint32_t, ShaderInfo> shaders_pending;
static std::shared_mutex mutex;
static reshade::api::device_api device_api = reshade::api::device_api::d3d9;
static std::string file_extension = ".cso";
static bool scanned_disk = false;

static void CheckShadersOnDisk() {
  scanned_disk = true;
  auto dump_path = renodx::utils::path::GetOutputPath();

  if (!std::filesystem::exists(dump_path)) return;
  dump_path /= "dump";
  if (!std::filesystem::exists(dump_path)) return;
  std::unordered_set<uint32_t> shader_hashes_found = {};

  for (const auto& entry : std::filesystem::directory_iterator(dump_path)) {
    if (!entry.is_regular_file()) continue;
    const auto& entry_path = entry.path();
    if (entry_path.extension() != internal::file_extension) continue;
    const auto& basename = entry_path.stem().string();
    if (basename.length() < 2 + 8) continue;
    if (!basename.starts_with("0x")) continue;
    auto hash_string = basename.substr(2, 8);
    uint32_t shader_hash;
    try {
      shader_hash = std::stoul(hash_string, nullptr, 16);
    } catch (const std::exception& e) {
      std::stringstream s;
      s << "utils::shader::dump(Invalid shader string: ";
      s << hash_string;
      s << " at ";
      s << entry_path;
      s << ")";
      reshade::log::message(reshade::log::level::warning, s.str().c_str());
      continue;
    }
    auto [iterator, new_insert] = shader_hashes_found.insert(shader_hash);
    if (!new_insert) {
      std::stringstream s;
      s << "utils::shader::dump(Found duplicate shader: ";
      s << PRINT_CRC32(shader_hash);
      s << " at ";
      s << entry_path;
      s << ")";
      reshade::log::message(reshade::log::level::warning, s.str().c_str());
    } else {
#ifdef DEBUG_LEVEL_1
      std::stringstream s;
      s << "utils::shader::dump(Found shader: ";
      s << PRINT_CRC32(shader_hash);
      s << " at ";
      s << entry_path;
      s << ")";
      reshade::log::message(reshade::log::level::debug, s.str().c_str());
#endif
    }
  }

  shaders_dumped.insert(shader_hashes_found.begin(), shader_hashes_found.end());
}

static void OnInitDevice(reshade::api::device* device) {
  std::unique_lock lock(internal::mutex);
  internal::device_api = device->get_api();
  if (internal::device_api == reshade::api::device_api::vulkan) {
    internal::file_extension = ".spv";
  } else if (internal::device_api == reshade::api::device_api::opengl) {
    internal::file_extension = ".glsl";
  }
  if (!internal::scanned_disk) {
    internal::CheckShadersOnDisk();
  }
}

// After CreatePipelineState
static void OnInitPipeline(
    reshade::api::device* device,
    reshade::api::pipeline_layout layout,
    uint32_t subobject_count,
    const reshade::api::pipeline_subobject* subobjects,
    reshade::api::pipeline pipeline) {
  if (pipeline.handle == 0u) return;

  auto* details = renodx::utils::shader::GetPipelineShaderDetails(pipeline);
  if (details == nullptr) return;

  std::unique_lock lock(mutex);
  for (const auto& info : details->subobject_shaders) {
    // Store immediately in case pipeline destroyed before present
    const auto& shader_hash = info.shader_hash;
    if (shaders_seen.contains(shader_hash)) continue;
    shaders_seen.emplace(shader_hash);

    if (shaders_dumped.contains(shader_hash)) continue;
    if (shaders_pending.contains(shader_hash)) continue;
    const auto& subobject = subobjects[info.index];
    const reshade::api::shader_desc desc = *static_cast<const reshade::api::shader_desc*>(subobject.data);
    if (desc.code_size == 0) continue;
    auto shader_data = std::vector<uint8_t>({
        static_cast<const uint8_t*>(desc.code),
        static_cast<const uint8_t*>(desc.code) + desc.code_size,
    });

    if (device::IsDirectX(device)) {
      try {
        auto shader_version = renodx::utils::shader::compiler::directx::DecodeShaderVersion(shader_data);
        if (shader_version.GetMajor() == 0) {
          // No shader information found
          continue;
        }
      } catch (std::exception& e) {
        std::stringstream s;
        s << "utils::shader::dump(Failed to decode shader data: ";
        s << PRINT_CRC32(shader_hash);
        s << ")";
        reshade::log::message(reshade::log::level::warning, s.str().c_str());
        continue;
      }
    } else {
      // Assume Vulkan
    }
    std::stringstream s;
    s << "utils::shader::dump(storing: ";
    s << PRINT_CRC32(shader_hash);
    s << ", size: " << shader_data.size();
    s << ")";
    reshade::log::message(reshade::log::level::debug, s.str().c_str());

    shaders_pending[shader_hash] = {
        .data = shader_data,
        .type = details->subobjects[info.index].type,
    };
  }
}

static void OnPresent(
    reshade::api::command_queue* queue,
    reshade::api::swapchain* swapchain,
    const reshade::api::rect* source_rect,
    const reshade::api::rect* dest_rect,
    uint32_t dirty_rect_count,
    const reshade::api::rect* dirty_rects) {
  std::shared_lock lock(mutex);
  pending_dump_count = shaders_pending.size();
}

static bool attached = false;

}  // namespace internal

static std::string default_dump_folder = "dump";

static std::filesystem::path GetShaderDumpPath(
    uint32_t shader_hash,
    std::span<uint8_t> shader_data,
    reshade::api::pipeline_subobject_type shader_type = reshade::api::pipeline_subobject_type::unknown,
    const std::string& prefix = "",
    const std::string& entry_point = "") {
  auto dump_path = renodx::utils::path::GetOutputPath();

  if (!std::filesystem::exists(dump_path)) {
    std::filesystem::create_directory(dump_path);
  }
  dump_path /= default_dump_folder;
  if (!std::filesystem::exists(dump_path)) {
    std::filesystem::create_directory(dump_path);
  }

  wchar_t hash_string[11];
  swprintf_s(hash_string, L"0x%08X", shader_hash);

  if (prefix.empty()) {
    dump_path /= hash_string;
  } else {
    dump_path /= prefix;
    dump_path += hash_string;
  }
  
  // Add entry point name to filename if provided
  if (!entry_point.empty()) {
    dump_path += L"_";
    dump_path += entry_point;
  }

  switch (internal::device_api) {
    case reshade::api::device_api::d3d9:
    case reshade::api::device_api::d3d10:
    case reshade::api::device_api::d3d11:
    case reshade::api::device_api::d3d12: {
      renodx::utils::shader::compiler::directx::DxilProgramVersion shader_version;
      try {
#ifdef DEBUG_LEVEL_1
        std::stringstream s;
        s << "utils::shader::dump(Decoding shader_data: ";
        s << PRINT_CRC32(shader_hash);
        s << ": " << shader_data.size();
        s << ")";
        reshade::log::message(reshade::log::level::debug, s.str().c_str());
#endif
        shader_version = renodx::utils::shader::compiler::directx::DecodeShaderVersion(shader_data);
      } catch (std::exception& e) {
        std::stringstream s;
        s << "utils::shader::dump(Failed to parse ";
        s << PRINT_CRC32(shader_hash);
        s << ": " << e.what();
        s << ")";
        reshade::log::message(reshade::log::level::error, s.str().c_str());
      }
      std::string kind = shader_version.GetKindAbbr();
      if (kind != "??") {
        dump_path += L".";
        dump_path += kind;
        dump_path += L"_";
        dump_path += std::to_string(shader_version.GetMajor());
        dump_path += L"_";
        dump_path += std::to_string(shader_version.GetMinor());
      } else {
        std::stringstream s;
        s << "utils::shader::dump(Unknown shader type: ";
        s << PRINT_CRC32(shader_hash);
        s << ", type: " << shader_version.GetKind();
        s << ")";
        reshade::log::message(reshade::log::level::warning, s.str().c_str());
      }
      break;
    }
    case reshade::api::device_api::opengl:
    case reshade::api::device_api::vulkan:
      // Vulkan
      switch (shader_type) {
        case reshade::api::pipeline_subobject_type::pixel_shader:
          dump_path += L".frag";
          break;
        case reshade::api::pipeline_subobject_type::vertex_shader:
          dump_path += L".vert";
          break;
        case reshade::api::pipeline_subobject_type::compute_shader:
          dump_path += L".comp";
          break;
        default:
          break;
      }
    default:
      break;
  }

  dump_path += internal::file_extension;
  return dump_path;
}

static bool DumpShader(
    uint32_t shader_hash,
    std::span<uint8_t> shader_data,
    reshade::api::pipeline_subobject_type shader_type = reshade::api::pipeline_subobject_type::unknown,
    const std::string& prefix = "") {
  auto dump_path = GetShaderDumpPath(shader_hash, shader_data, shader_type, prefix);
  auto is_binary = internal::device_api != reshade::api::device_api::opengl;

  if (is_binary) {
    renodx::utils::path::WriteBinaryFile(dump_path, shader_data);
  } else {
    // trim null
    renodx::utils::path::WriteBinaryFile(dump_path, shader_data.subspan(0, shader_data.size() - 1));
  }
  
  // Mark shader as dumped to avoid re-dumping
  internal::shaders_dumped.emplace(shader_hash);
  
  return true;
}

static bool DumpShader(
    uint32_t shader_hash,
    std::span<uint8_t> shader_data,
    reshade::api::pipeline_stage shader_stage = static_cast<reshade::api::pipeline_stage>(0u),
    const std::string& prefix = "") {
  auto shader_type =
      reshade::api::pipeline_subobject_type::unknown;
  switch (shader_stage) {
    case reshade::api::pipeline_stage::pixel_shader:
      shader_type = reshade::api::pipeline_subobject_type::pixel_shader;
      break;
    case reshade::api::pipeline_stage::vertex_shader:
      shader_type = reshade::api::pipeline_subobject_type::vertex_shader;
      break;
    case reshade::api::pipeline_stage::compute_shader:
      shader_type = reshade::api::pipeline_subobject_type::compute_shader;
      break;
    default:
      break;
  }
  return DumpShader(shader_hash, shader_data, shader_type, prefix);
}

static std::filesystem::path GetShaderDumpPath(
    uint32_t shader_hash,
    std::span<uint8_t> shader_data,
    reshade::api::pipeline_stage shader_stage = static_cast<reshade::api::pipeline_stage>(0u),
    const std::string& prefix = "") {
  auto shader_type =
      reshade::api::pipeline_subobject_type::unknown;
  switch (shader_stage) {
    case reshade::api::pipeline_stage::pixel_shader:
      shader_type = reshade::api::pipeline_subobject_type::pixel_shader;
      break;
    case reshade::api::pipeline_stage::vertex_shader:
      shader_type = reshade::api::pipeline_subobject_type::vertex_shader;
      break;
    case reshade::api::pipeline_stage::compute_shader:
      shader_type = reshade::api::pipeline_subobject_type::compute_shader;
      break;
    default:
      break;
  }

  return GetShaderDumpPath(shader_hash, shader_data, shader_type, prefix);
}

static void DumpAllPending() {
  std::unique_lock lock(internal::mutex);
  for (auto& [shader_hash, shader_info] : internal::shaders_pending) {
    // Skip shaders that have already been dumped
    if (internal::shaders_dumped.contains(shader_hash)) {
      std::stringstream s;
      s << "utils::shader::dump(Skipping already dumped shader: ";
      s << PRINT_CRC32(shader_hash);
      s << ")";
      reshade::log::message(reshade::log::level::debug, s.str().c_str());
      continue;
    }

    std::stringstream s;
    s << "utils::shader::dump(Starting dump: ";
    s << PRINT_CRC32(shader_hash);
    s << ")";
    reshade::log::message(reshade::log::level::debug, s.str().c_str());

    DumpShader(shader_hash, shader_info.data, shader_info.type);
  }
  internal::shaders_pending.clear();
}

template<typename CustomShadersType>
static void DumpAllPending(const CustomShadersType& custom_shaders) {
  std::unique_lock lock(internal::mutex);
  for (auto& [shader_hash, shader_info] : internal::shaders_pending) {
    // Skip shaders that have already been dumped
    if (internal::shaders_dumped.contains(shader_hash)) {
      std::stringstream s;
      s << "utils::shader::dump(Skipping already dumped shader: ";
      s << PRINT_CRC32(shader_hash);
      s << ")";
      reshade::log::message(reshade::log::level::debug, s.str().c_str());
      continue;
    }

    // Skip custom shaders
    if (custom_shaders.contains(shader_hash)) {
      std::stringstream s;
      s << "utils::shader::dump(Skipping custom shader: ";
      s << PRINT_CRC32(shader_hash);
      s << ")";
      reshade::log::message(reshade::log::level::debug, s.str().c_str());
      continue;
    }

    std::stringstream s;
    s << "utils::shader::dump(Starting dump: ";
    s << PRINT_CRC32(shader_hash);
    s << ")";
    reshade::log::message(reshade::log::level::debug, s.str().c_str());

    DumpShader(shader_hash, shader_info.data, shader_info.type);
  }
  internal::shaders_pending.clear();
}

template<typename CustomShadersType, typename DumpedShadersType, typename LutBuilderCheckFunc>
static void DumpAllPending(const CustomShadersType& custom_shaders, const DumpedShadersType& external_dumped_shaders, LutBuilderCheckFunc contains_lutbuilder_float, float g_dump_shaders, float g_autodump_lutbuilders) {
  std::unique_lock lock(internal::mutex);
  for (auto& [shader_hash, shader_info] : internal::shaders_pending) {
    // Skip shaders that have already been dumped internally
    if (internal::shaders_dumped.contains(shader_hash)) {
      std::stringstream s;
      s << "utils::shader::dump(Skipping already dumped shader: ";
      s << PRINT_CRC32(shader_hash);
      s << ")";
      reshade::log::message(reshade::log::level::debug, s.str().c_str());
      continue;
    }

    
    // Skip shaders that have already been dumped externally
    if (external_dumped_shaders.contains(shader_hash)) {
      std::stringstream s;
      s << "utils::shader::dump(Skipping externally dumped shader: ";
      s << PRINT_CRC32(shader_hash);
      s << ")";
      reshade::log::message(reshade::log::level::debug, s.str().c_str());
      continue;
    }
    
    // Skip custom shaders
    if (custom_shaders.contains(shader_hash)) {
      std::stringstream s;
      s << "utils::shader::dump(Skipping custom shader: ";
      s << PRINT_CRC32(shader_hash);
      s << ")";
      reshade::log::message(reshade::log::level::debug, s.str().c_str());
      continue;
    }
    
    // Check if shader contains lutbuilder float and set prefix accordingly
    std::span<const uint8_t> shader_span(shader_info.data.data(), shader_info.data.size());
    bool is_lutbuilder = contains_lutbuilder_float(shader_span);
    
    // Determine if we should dump this shader
    bool should_dump = false;
    if (g_dump_shaders != 0) {
      // Dump all shaders if general dumping is enabled
      should_dump = true;
    } else if (g_autodump_lutbuilders != 0 && is_lutbuilder) {
      // Only dump lutbuilders if only lutbuilder dumping is enabled
      should_dump = true;
    }
    
    if (!should_dump) continue;

    std::stringstream s;
    s << "utils::shader::dump(Starting dump: ";
    s << PRINT_CRC32(shader_hash);
    s << ")";
    reshade::log::message(reshade::log::level::debug, s.str().c_str());

    std::string prefix = is_lutbuilder ? "lutbuilder_" : "shader_";
    
    DumpShader(shader_hash, shader_info.data, shader_info.type, prefix);
  }
  internal::shaders_pending.clear();
}

// New overload that uses DumpShaderPrefix approach
template<typename CustomShadersType, typename DumpedShadersType, typename DumpShaderPrefixFunc>
static void DumpAllPending(const CustomShadersType& custom_shaders, const DumpedShadersType& external_dumped_shaders, DumpShaderPrefixFunc dump_shader_prefix_func) {
  std::unique_lock lock(internal::mutex);
  for (auto& [shader_hash, shader_info] : internal::shaders_pending) {
    // Skip shaders that have already been dumped internally
    if (internal::shaders_dumped.contains(shader_hash)) {
      std::stringstream s;
      s << "utils::shader::dump(Skipping already dumped shader: ";
      s << PRINT_CRC32(shader_hash);
      s << ")";
      reshade::log::message(reshade::log::level::debug, s.str().c_str());
      continue;
    }

    // Skip shaders that have already been dumped externally
    if (external_dumped_shaders.contains(shader_hash)) {
      std::stringstream s;
      s << "utils::shader::dump(Skipping externally dumped shader: ";
      s << PRINT_CRC32(shader_hash);
      s << ")";
      reshade::log::message(reshade::log::level::debug, s.str().c_str());
      continue;
    }
    
    // Skip custom shaders
    if (custom_shaders.contains(shader_hash)) {
      std::stringstream s;
      s << "utils::shader::dump(Skipping custom shader: ";
      s << PRINT_CRC32(shader_hash);
      s << ")";
      reshade::log::message(reshade::log::level::debug, s.str().c_str());
      continue;
    }
    
    // Use the DumpShaderPrefix function to determine if we should dump and get the prefix
    std::span<const uint8_t> shader_span(shader_info.data.data(), shader_info.data.size());
    auto dump_prefix = dump_shader_prefix_func(shader_span);
    
    if (!dump_prefix.has_value()) continue;

    std::stringstream s;
    s << "utils::shader::dump(Starting dump: ";
    s << PRINT_CRC32(shader_hash);
    s << ")";
    reshade::log::message(reshade::log::level::debug, s.str().c_str());

    DumpShader(shader_hash, shader_info.data, shader_info.type, dump_prefix.value());
  }
  internal::shaders_pending.clear();
}

static void Use(DWORD fdw_reason) {
  renodx::utils::shader::Use(fdw_reason);
  switch (fdw_reason) {
    case DLL_PROCESS_ATTACH:
      if (internal::attached) return;
      internal::attached = true;
      reshade::log::message(reshade::log::level::info, "ShaderDump attached.");
      reshade::register_event<reshade::addon_event::init_device>(internal::OnInitDevice);
      reshade::register_event<reshade::addon_event::init_pipeline>(internal::OnInitPipeline);
      reshade::register_event<reshade::addon_event::present>(internal::OnPresent);

      break;
    case DLL_PROCESS_DETACH:
      reshade::unregister_event<reshade::addon_event::init_device>(internal::OnInitDevice);
      reshade::unregister_event<reshade::addon_event::init_pipeline>(internal::OnInitPipeline);
      reshade::unregister_event<reshade::addon_event::present>(internal::OnPresent);
      break;
  }
}

}  // namespace renodx::utils::shader::dump