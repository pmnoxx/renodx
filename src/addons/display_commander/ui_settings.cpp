#include "addon.hpp"
#include "nvapi_fullscreen_prevention.hpp"
#include "reflex_management.hpp"
#include "dxgi_device_info.hpp"
#include "utils.hpp"
#include "ui_settings.hpp"
#include <sstream>
#include <iomanip>

// External declarations
extern float s_reflex_debug_output;

// Continuous monitoring function declarations
extern void StartContinuousMonitoring();
extern void StopContinuousMonitoring();

// Helper functions for tab visibility
inline bool is_basic_tab(float ui_mode) { return ui_mode < 0.5f; }
inline bool is_developer_tab(float ui_mode) { return ui_mode >= 0.5f && ui_mode < 2.0f; }
inline bool is_device_tab(float ui_mode) { return ui_mode >= 2.0f; }

// Function to add UI settings to an existing settings vector
void AddUISettings(renodx::utils::settings::Settings& settings) {

















    // DXGI Device Information Tab
    new renodx::utils::settings::Setting{
        .key = "DxgiDeviceInfo",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "",
        .section = "DXGI",
        .tooltip = "Detailed DXGI adapter and output information including HDR capabilities.",
        .on_draw = [](){
                     if (!g_dxgiDeviceInfoManager || !g_dxgiDeviceInfoManager->IsInitialized()) {
             ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "DXGI Device Info Manager not initialized");
             return false;
           }

           // Force device enumeration when this tab is first opened
           static bool first_open = true;
           if (first_open) {
             g_dxgiDeviceInfoManager->RefreshDeviceInfo();
             first_open = false;
           }

           const auto& adapters = g_dxgiDeviceInfoManager->GetAdapters();
           if (adapters.empty()) {
             ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No DXGI adapters found yet. Device enumeration happens automatically during present operations.");
             ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "If you're still not seeing adapters, try refreshing or check if a game/application is running.");
             return false;
           }

                     // Add refresh button
           if (ImGui::Button("Refresh Device Info")) {
             g_dxgiDeviceInfoManager->RefreshDeviceInfo();
           }
           ImGui::SameLine();
           ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Click to refresh device information");
           
                      // Add force re-enumeration button
           ImGui::SameLine();
           if (ImGui::Button("Force Re-enumeration")) {
             g_dxgiDeviceInfoManager->RefreshDeviceInfo();
           }
           ImGui::SameLine();
           ImGui::TextColored(ImVec4(0.8f, 0.8f, 1.0f, 1.0f), "Force re-enumeration of all devices");
           
           // Add HDR metadata reset button
           ImGui::SameLine();
           if (ImGui::Button("Reset HDR Metadata")) {
            // Find first HDR10-capable output and reset its metadata
            const auto& adapters = g_dxgiDeviceInfoManager->GetAdapters();
            for (const auto& adapter : adapters) {
              for (const auto& output : adapter.outputs) {
                if (output.supports_hdr10) {
                                      if (g_dxgiDeviceInfoManager->ResetHDRMetadataOnPresent(output.device_name)) {
                        LogInfo(("HDR metadata reset initiated for: " + output.device_name).c_str());
                    } else {
                        LogWarn(("HDR metadata reset failed for: " + output.device_name).c_str());
                    }
                  break; // Only reset the first HDR10 output found
                }
              }
            }
          }
                     ImGui::SameLine();
           ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Reset HDR metadata for HDR10 displays");

           // Colorspace selector
           ImGui::SameLine();
           if (ImGui::Button("Set Colorspace")) {
             // Colorspace selection will be handled in the dropdown below
           }
           ImGui::SameLine();
           ImGui::TextColored(ImVec4(0.8f, 1.0f, 0.8f, 1.0f), "Set swapchain colorspace");
           
           // Colorspace dropdown
           static int selected_colorspace = 0;
           const char* colorspace_names[] = {
               "sRGB (Non-Linear)",
               "Extended sRGB (Linear)", 
               "HDR10 (ST2084/PQ)",
               "HDR10 (HLG)",
               "scRGB (16-bit Linear)"
           };
           const reshade::api::color_space colorspace_values[] = {
               reshade::api::color_space::srgb_nonlinear,
               reshade::api::color_space::extended_srgb_linear,
               reshade::api::color_space::hdr10_st2084,
               reshade::api::color_space::hdr10_hlg,
               reshade::api::color_space::extended_srgb_linear // Use extended_srgb_linear for scRGB
           };
           
           if (ImGui::Combo("Colorspace", &selected_colorspace, colorspace_names, IM_ARRAYSIZE(colorspace_names))) {
             // Apply the selected colorspace
             if (g_dxgiDeviceInfoManager && g_dxgiDeviceInfoManager->IsInitialized()) {
               bool success = false;
               
               if (selected_colorspace == 4) { // scRGB (16-bit Linear)
                 // Use special scRGB method
                 success = g_dxgiDeviceInfoManager->SetScRGBColorspace();
               } else {
                 // Use standard colorspace method
                 success = g_dxgiDeviceInfoManager->SetColorspace(colorspace_values[selected_colorspace]);
               }
               
               if (success) {
                 LogInfo(("Colorspace changed to: " + std::string(colorspace_names[selected_colorspace])).c_str());
               } else {
                 LogWarn(("Failed to change colorspace to: " + std::string(colorspace_names[selected_colorspace])).c_str());
               }
             }
           }

           ImGui::Separator();

           // Swapchain Information Section
           if (ImGui::TreeNodeEx("Swapchain Information", ImGuiTreeNodeFlags_DefaultOpen)) {
             // Get current swapchain info from ReShade
             extern std::atomic<reshade::api::swapchain*> g_last_swapchain_ptr;
             extern reshade::api::color_space g_current_colorspace;
             
             auto* swapchain = g_last_swapchain_ptr.load();
             if (swapchain) {
               // Colorspace information
               ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Current Colorspace:");
               switch (g_current_colorspace) {
                 case reshade::api::color_space::srgb_nonlinear:
                   ImGui::Text("  sRGB (Non-Linear)");
                   break;
                 case reshade::api::color_space::extended_srgb_linear:
                   ImGui::Text("  Extended sRGB (Linear)");
                   break;
                 case reshade::api::color_space::hdr10_st2084:
                   ImGui::Text("  HDR10 (ST2084/PQ)");
                   break;
                 case reshade::api::color_space::hdr10_hlg:
                   ImGui::Text("  HDR10 (HLG)");
                   break;
                 default:
                   ImGui::Text("  Unknown (%d)", static_cast<int>(g_current_colorspace));
                   break;
               }

               // Backbuffer information
               if (swapchain->get_back_buffer_count() > 0) {
                 auto bb = swapchain->get_back_buffer(0);
                 auto* device = swapchain->get_device();
                 if (device) {
                   auto desc = device->get_resource_desc(bb);
                   ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Backbuffer:");
                   ImGui::Text("  Resolution: %dx%d", desc.texture.width, desc.texture.height);
                   ImGui::Text("  Format: %s", 
                     desc.texture.format == reshade::api::format::r8g8b8a8_unorm ? "R8G8B8A8_UNORM" :
                     desc.texture.format == reshade::api::format::r10g10b10a2_unorm ? "R10G10B10A2_UNORM" :
                     desc.texture.format == reshade::api::format::r16g16b16a16_float ? "R16G16B16A16_FLOAT" :
                     "Other");
                   ImGui::Text("  Backbuffer Count: %d", swapchain->get_back_buffer_count());
                 }
               }

               // Window information
               HWND hwnd = static_cast<HWND>(swapchain->get_hwnd());
               if (hwnd) {
                 ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Window:");
                 RECT rect;
                 if (GetWindowRect(hwnd, &rect)) {
                   ImGui::Text("  Position: (%d, %d) to (%d, %d)", rect.left, rect.top, rect.right, rect.bottom);
                   ImGui::Text("  Size: %dx%d", rect.right - rect.left, rect.bottom - rect.top);
                 }
                 
                 // Window state
                 LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
                 ImGui::Text("  Fullscreen: %s", (style & WS_POPUP) ? "Yes" : "No");
                 ImGui::Text("  Borderless: %s", (style & WS_POPUP) && !(style & WS_CAPTION) ? "Yes" : "No");
               }
             } else {
               ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No active swapchain available");
             }
             ImGui::TreePop();
           }

           ImGui::Separator();

          for (size_t i = 0; i < adapters.size(); ++i) {
            const auto& adapter = adapters[i];
            
            // Adapter header
            std::string adapter_title = adapter.name + " - " + adapter.description;
            if (ImGui::TreeNodeEx(adapter_title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
              
              // Adapter details
              ImGui::Text("Description: %s", adapter.description.c_str());
              ImGui::Text("Dedicated Video Memory: %.1f GB", adapter.dedicated_video_memory / (1024.0 * 1024.0 * 1024.0));
              ImGui::Text("Dedicated System Memory: %.1f GB", adapter.dedicated_system_memory / (1024.0 * 1024.0 * 1024.0));
              ImGui::Text("Shared System Memory: %.1f GB", adapter.shared_system_memory / (1024.0 * 1024.0 * 1024.0));
              ImGui::Text("Software Adapter: %s", adapter.is_software ? "Yes" : "No");
              
              // LUID info
              std::ostringstream luid_oss;
              luid_oss << "Adapter LUID: 0x" << std::hex << adapter.adapter_luid.HighPart << "_" << adapter.adapter_luid.LowPart;
              ImGui::Text("%s", luid_oss.str().c_str());

              // Outputs
              if (!adapter.outputs.empty()) {
                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Outputs (%zu):", adapter.outputs.size());
                
                for (size_t j = 0; j < adapter.outputs.size(); ++j) {
                  const auto& output = adapter.outputs[j];
                  std::string output_title = "Output " + std::to_string(j) + " - " + output.device_name;
                  
                  if (ImGui::TreeNodeEx(output_title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                    
                    // Basic output info
                    ImGui::Text("Device Name: %s", output.device_name.c_str());
                    ImGui::Text("Monitor Name: %s", output.monitor_name.c_str());
                    ImGui::Text("Attached: %s", output.is_attached ? "Yes" : "No");
                    ImGui::Text("Desktop Coordinates: (%d, %d) to (%d, %d)", 
                               output.desktop_coordinates.left, output.desktop_coordinates.top,
                               output.desktop_coordinates.right, output.desktop_coordinates.bottom);
                    
                    // Resolution and refresh rate
                    int width = output.desktop_coordinates.right - output.desktop_coordinates.left;
                    int height = output.desktop_coordinates.bottom - output.desktop_coordinates.top;
                    ImGui::Text("Resolution: %dx%d", width, height);
                    
                    if (output.refresh_rate.Denominator > 0) {
                      float refresh = static_cast<float>(output.refresh_rate.Numerator) / static_cast<float>(output.refresh_rate.Denominator);
                      ImGui::Text("Refresh Rate: %.2f Hz", refresh);
                    }

                    // HDR information
                    if (output.supports_hdr10) {
                      ImGui::Separator();
                      ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "HDR10 Support: ✓ Enabled");
                      ImGui::Text("Max Luminance: %.1f nits", output.max_luminance);
                      ImGui::Text("Min Luminance: %.1f nits", output.min_luminance);
                      ImGui::Text("Max Frame Average Light Level: %.1f nits", output.max_frame_average_light_level);
                      ImGui::Text("Max Content Light Level: %.1f nits", output.max_content_light_level);
                    } else {
                      ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "HDR10 Support: ✗ Not Supported");
                    }

                    // Color space information
                    ImGui::Separator();
                    ImGui::Text("Color Space: %s", 
                               output.color_space == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 ? "HDR10" :
                               output.color_space == DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709 ? "sRGB" :
                               "Other");
                    ImGui::Text("Wide Color Gamut: %s", output.supports_wide_color_gamut ? "Yes" : "No");

                    // Supported modes count
                    if (!output.supported_modes.empty()) {
                      ImGui::Text("Supported Modes: %zu", output.supported_modes.size());
                    }

                    ImGui::TreePop();
                  }
                }
              }

              ImGui::TreePop();
            }
          }

          return false;
        },
                 .is_visible = []() { return is_device_tab(s_ui_mode); }, // Show only in Device Info mode
    },

    // DXGI composition/backbuffer info (text only) — placed at bottom
    new renodx::utils::settings::Setting{
        .key = "DxgiInfo",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "",
        .section = "Performance",
        .tooltip = "DXGI composition mode, current backbuffer size, and fullscreen status.",
        .on_draw = [](){
          const char* mode_str = "Unknown";
          switch (static_cast<int>(s_dxgi_composition_state)) {
            case 1: mode_str = "Composed Flip"; break;
            case 2: mode_str = "Modern Independent Flip"; break;
            case 3: mode_str = "Legacy Independent Flip"; break;
            default: mode_str = "Unknown"; break;
          }
          
          // Check exclusive fullscreen status
          HWND hwnd = g_last_swapchain_hwnd.load();
          if (hwnd == nullptr) hwnd = GetForegroundWindow();
          const bool is_exclusive_fullscreen = GetSpoofedFullscreenState(hwnd);
          
          // Get backbuffer format
          std::string format_str = "Unknown";
          if (g_last_swapchain_ptr.load() != nullptr) {
            auto* device = g_last_swapchain_ptr.load()->get_device();
            if (device != nullptr && g_last_swapchain_ptr.load()->get_back_buffer_count() > 0) {
              auto bb = g_last_swapchain_ptr.load()->get_back_buffer(0);
              auto desc = device->get_resource_desc(bb);
              switch (desc.texture.format) {
                case reshade::api::format::r8g8b8a8_unorm: format_str = "R8G8B8A8_UNORM"; break;
                case reshade::api::format::r8g8b8a8_unorm_srgb: format_str = "R8G8B8A8_UNORM_SRGB"; break;
                case reshade::api::format::r10g10b10a2_unorm: format_str = "R10G10B10A2_UNORM"; break;
                case reshade::api::format::r16g16b16a16_float: format_str = "R16G16B16A16_FLOAT"; break;
                case reshade::api::format::r11g11b10_float: format_str = "R11G11B10_FLOAT"; break;
                case reshade::api::format::b8g8r8a8_unorm: format_str = "B8G8R8A8_UNORM"; break;
                case reshade::api::format::b8g8r8a8_unorm_srgb: format_str = "B8G8R8A8_UNORM_SRGB"; break;
                case reshade::api::format::r8g8b8_unorm: format_str = "R8G8B8_UNORM"; break;
                case reshade::api::format::r8g8b8_unorm_srgb: format_str = "R8G8B8_UNORM_SRGB"; break;
                case reshade::api::format::b5g6r5_unorm: format_str = "B5G6R5_UNORM"; break;
                case reshade::api::format::b5g5r5a1_unorm: format_str = "B5G5R5A1_UNORM"; break;
                case reshade::api::format::b4g4r4a4_unorm: format_str = "B4G4R4A4_UNORM"; break;
                default: format_str = "Format_" + std::to_string(static_cast<int>(desc.texture.format)); break;
              }
            }
          }
          
          // Get colorspace string
          std::string colorspace_str = "Unknown";
          switch (g_current_colorspace) {
            case reshade::api::color_space::unknown: colorspace_str = "Unknown"; break;
            case reshade::api::color_space::srgb_nonlinear: colorspace_str = "sRGB"; break;
            case reshade::api::color_space::extended_srgb_linear: colorspace_str = "Extended sRGB Linear"; break;
            case reshade::api::color_space::hdr10_st2084: colorspace_str = "HDR10 ST2084"; break;
            case reshade::api::color_space::hdr10_hlg: colorspace_str = "HDR10 HLG"; break;
            default: colorspace_str = "ColorSpace_" + std::to_string(static_cast<int>(g_current_colorspace)); break;
          }
          
          ImGui::Separator();
          ImGui::Text("DXGI Composition: %s | Backbuffer: %dx%d | Format: %s | Colorspace: %s",
                      mode_str,
                      g_last_backbuffer_width.load(),
                      g_last_backbuffer_height.load(),
                      format_str.c_str(),
                      colorspace_str.c_str());
          
          // Display HDR10 override status
          ImGui::Text("HDR10 Colorspace Override: %s (Last: %s)", g_hdr10_override_status.c_str(), g_hdr10_override_timestamp.c_str());
          
          // Display current window position and size
          if (hwnd != nullptr) {
            RECT wr{};
            RECT cr{};
            if (GetWindowRect(hwnd, &wr) != FALSE && GetClientRect(hwnd, &cr) != FALSE) {
              int window_width = wr.right - wr.left;
              int window_height = wr.bottom - wr.top;
              int client_width = cr.right - cr.left;
              int client_height = cr.bottom - cr.top;
              
              // Convert client coordinates to screen coordinates
              POINT client_origin = {cr.left, cr.top};
              ClientToScreen(hwnd, &client_origin);
              
              ImGui::Text("Window Frame: %dx%d at (%ld, %ld)", window_width, window_height, wr.left, wr.top);
              ImGui::Text("Client Area: %dx%d at (%ld, %ld)", client_width, client_height, client_origin.x, client_origin.y);
              ImGui::Text("Decorations: %dx%d offset", window_width - client_width, window_height - client_height);
            }
            
            // Display current monitor resolution
            HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            if (hmon != nullptr) {
              MONITORINFOEXW mi{};
              mi.cbSize = sizeof(mi);
              if (GetMonitorInfoW(hmon, &mi) != FALSE) {
                int monitor_width = mi.rcMonitor.right - mi.rcMonitor.left;
                int monitor_height = mi.rcMonitor.bottom - mi.rcMonitor.top;
                ImGui::Text("Monitor: %dx%d at (%ld, %ld)", 
                           monitor_width, monitor_height, mi.rcMonitor.left, mi.rcMonitor.top);
              }
            }
          }
          
          return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    // Independent Flip failure reasons (text only)
    new renodx::utils::settings::Setting{
        .key = "IndependentFlipFailures",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "",
        .section = "Performance",
        .tooltip = "Reasons why Independent Flip is not working.",
        .on_draw = [](){
          IndependentFlipFailures* failures = g_if_failures.load();
          if (failures == nullptr) return false;
          
          // Check if there are any failures
          bool has_failures = failures->swapchain_null.load() ||
                             failures->device_null.load() ||
                             failures->non_dxgi_api.load() ||
                             failures->swapchain_media_failed.load() ||
                             failures->frame_stats_failed.load() ||
                             failures->not_flip_model.load() ||
                             failures->backbuffer_size_mismatch.load() ||
                             failures->window_size_mismatch.load() ||
                             failures->window_not_at_origin.load() ||
                             failures->window_layered.load() ||
                             failures->window_topmost.load() ||
                             failures->window_maximized.load() ||
                             failures->window_minimized.load() ||
                             failures->hwnd_null.load();
          
          if (has_failures) {
            ImGui::Separator();
            ImGui::Text("Independent Flip Debug Info:");
            
            if (failures->swapchain_null.load()) ImGui::BulletText("Swapchain is null");
            if (failures->device_null.load()) ImGui::BulletText("Device is null");
            if (failures->non_dxgi_api.load()) ImGui::BulletText("Non-DXGI API (OpenGL/Vulkan)");
            if (failures->swapchain_media_failed.load()) ImGui::BulletText("Swapchain media interface failed");
            if (failures->frame_stats_failed.load()) ImGui::BulletText("Frame statistics failed");
            if (failures->not_flip_model.load()) ImGui::BulletText("Not a flip-model swapchain");
            if (failures->backbuffer_size_mismatch.load()) ImGui::BulletText("Backbuffer size doesn't match monitor");
            if (failures->window_size_mismatch.load()) ImGui::BulletText("Window size doesn't match monitor");
            if (failures->window_not_at_origin.load()) ImGui::BulletText("Window not at monitor origin (0,0)");
            if (failures->window_layered.load()) ImGui::BulletText("Window has WS_EX_LAYERED style");
            if (failures->window_topmost.load()) ImGui::BulletText("Window has WS_EX_TOPMOST style");
            if (failures->window_maximized.load()) ImGui::BulletText("Window is maximized");
            if (failures->window_minimized.load()) ImGui::BulletText("Window is minimized");
            if (failures->hwnd_null.load()) ImGui::BulletText("Window handle is null");
          }
          
          return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },

    // Window position (X, Y coordinates)
    new renodx::utils::settings::Setting{
        .key = "WindowPosX",
        .binding = &s_windowed_pos_x,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Window Position X",
        .section = "Display",
        .tooltip = "X coordinate for window positioning (relative to target monitor).",
        .min = -10000.f,
        .max = 10000.f,
        .format = "%d px",
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },
    new renodx::utils::settings::Setting{
        .key = "WindowPosY",
        .binding = &s_windowed_pos_y,
        .value_type = renodx::utils::settings::SettingValueType::INTEGER,
        .default_value = 0.f,
        .label = "Window Position Y",
        .section = "Display",
        .tooltip = "Y coordinate for window positioning (relative to target monitor).",
        .min = -10000.f,
        .max = 10000.f,
        .format = "%d px",
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    },






    

    

    
    // NVAPI Tab - NVIDIA-specific features
    new renodx::utils::settings::Setting{
        .key = "NVAPITab",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "",
        .section = "NVAPI",
        .tooltip = "NVIDIA-specific features and HDR management",
        .on_draw = []() -> bool {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "NVIDIA Features");
            ImGui::Separator();
            
            // Fix HDR10 Colorspace option moved here
            extern float s_fix_hdr10_colorspace;
            bool fix_hdr = (s_fix_hdr10_colorspace >= 0.5f);
            if (ImGui::Checkbox("Fix HDR10 Colorspace", &fix_hdr)) {
                s_fix_hdr10_colorspace = fix_hdr ? 1.0f : 0.0f;
            }
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "?");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Automatically fix HDR10 colorspace when swapchain format is RGB10A2 and colorspace is currently sRGB. Only works when the game is using sRGB colorspace.");
            }
            
            // Add more NVIDIA-specific features here in the future
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Additional NVIDIA features coming soon...");
            
            return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Show in Developer mode
    };
}
