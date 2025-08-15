#include "addon.hpp"

// Include the UI settings header to get the settings vector declaration
#include "ui_settings.hpp"



// Forward declarations for continuous monitoring functions
void StartContinuousMonitoring();
void StopContinuousMonitoring();

// External declarations for settings
extern float s_remove_top_bar;

BOOL APIENTRY DllMain(HMODULE h_module, DWORD fdw_reason, LPVOID lpv_reserved) {
  switch (fdw_reason) {
    case DLL_PROCESS_ATTACH:
      if (!reshade::register_addon(h_module)) return FALSE;
      reshade::register_event<reshade::addon_event::init_swapchain>(OnInitSwapchain);
      
      // Defer NVAPI / Reflex init until after settings are loaded below
      
      // Intercept SetFullscreenState; return true to skip original call
      reshade::register_event<reshade::addon_event::set_fullscreen_state>(
          [](reshade::api::swapchain* swapchain, bool fullscreen, void* hmonitor) -> bool {
            // log the event (reshade::log::message does not support printf-style formatting)
            {
              std::ostringstream oss;
              oss << "[Resolution Override] SetFullscreenState: fullscreen=" << (fullscreen ? 1 : 0)
                  << ", hmonitor=" << hmonitor;
              reshade::log::message(reshade::log::level::info, oss.str().c_str());
            }

            // If fullscreen prevention is enabled, block fullscreen attempts
            if (s_nvapi_fullscreen_prevention >= 0.5f && fullscreen) {
              LogInfo("Blocking fullscreen attempt - NVAPI fullscreen prevention is enabled");
              
              // Ensure NVAPI is initialized and prevention is active
              extern NVAPIFullscreenPrevention g_nvapiFullscreenPrevention;
              if (!g_nvapiFullscreenPrevention.IsAvailable()) {
                if (g_nvapiFullscreenPrevention.Initialize()) {
                  LogInfo("NVAPI initialized on-demand for fullscreen prevention");
                } else {
                  LogWarn("Failed to initialize NVAPI on-demand");
                }
              }
              
              // Apply fullscreen prevention if not already active
              if (g_nvapiFullscreenPrevention.IsAvailable() && !g_nvapiFullscreenPrevention.IsFullscreenPreventionEnabled()) {
                if (g_nvapiFullscreenPrevention.SetFullscreenPrevention(true)) {
                  LogInfo("NVAPI fullscreen prevention applied on-demand");
                } else {
                  LogWarn("Failed to apply NVAPI fullscreen prevention on-demand");
                }
              }
              
              // Return true to prevent the original SetFullscreenState call
              return true;
            }

            return false; // not handled; call original
          });

      renodx::utils::settings::use_presets = false;
      renodx::utils::settings::global_name = "ResolutionOverride";
      g_attach_time = std::chrono::steady_clock::now();
      g_shutdown.store(false);
      std::thread(RunBackgroundAudioMonitor).detach();
      // NVAPI HDR monitor will be started after settings load below if enabled
      // Seed default fps limit snapshot
      g_default_fps_limit.store(renodx::utils::swapchain::fps_limit);
      reshade::register_event<reshade::addon_event::present>(OnPresentUpdate);
      break;
    case DLL_PROCESS_DETACH:
      // Clean up the failure tracking structure
      IndependentFlipFailures* failures = g_if_failures.load();
      if (failures != nullptr) {
        delete failures;
        g_if_failures.store(nullptr);
      }
      

      
      // Clean up continuous monitoring if it's running
      StopContinuousMonitoring();
      

      
      // Clean up Reflex hooks if they're installed
      UninstallReflexHooks();
      
      // Clean up DXGI Device Info Manager
      g_dxgiDeviceInfoManager.reset();
      
      reshade::unregister_event<reshade::addon_event::present>(OnPresentUpdate);
      reshade::unregister_event<reshade::addon_event::set_fullscreen_state>(
          [](reshade::api::swapchain*, bool, void*) { return false; });
      reshade::unregister_event<reshade::addon_event::init_swapchain>(OnInitSwapchain);
      reshade::unregister_addon(h_module);
      g_shutdown.store(true);
      break;
  }

  renodx::utils::settings::Use(fdw_reason, &settings);
  renodx::utils::swapchain::Use(fdw_reason);

  // Initialize hooks if settings are enabled on startup (after settings are loaded)
  if (fdw_reason == DLL_PROCESS_ATTACH) {

    
    // Check if continuous monitoring should be enabled
    if (s_continuous_monitoring_enabled >= 0.5f) {
      StartContinuousMonitoring();
      LogInfo("Continuous monitoring started proactively");
    }
    
    // Initialize NVAPI fullscreen prevention if enabled
    if (s_nvapi_fullscreen_prevention >= 0.5f) {
      extern NVAPIFullscreenPrevention g_nvapiFullscreenPrevention;
      if (g_nvapiFullscreenPrevention.Initialize()) {
        LogInfo("NVAPI initialized proactively for fullscreen prevention");
        if (g_nvapiFullscreenPrevention.SetFullscreenPrevention(true)) {
          LogInfo("NVAPI fullscreen prevention applied proactively");
        } else {
          LogWarn("Failed to apply NVAPI fullscreen prevention proactively");
        }
      } else {
        LogWarn("Failed to initialize NVAPI proactively");
      }
    }

    // Initialize Reflex hooks if enabled
    if (s_reflex_enabled >= 0.5f) {
      if (InstallReflexHooks()) {
        LogInfo("Reflex hooks initialized proactively");
      } else {
        LogWarn("Failed to initialize Reflex hooks proactively");
      }
    }

    // Start NVAPI HDR monitor if enabled
    if (s_nvapi_hdr_logging >= 0.5f) {
      std::thread(RunBackgroundNvapiHdrMonitor).detach();
    }

    // Initialize DXGI Device Info Manager
    g_dxgiDeviceInfoManager = std::make_unique<DXGIDeviceInfoManager>();
    if (g_dxgiDeviceInfoManager->Initialize()) {
      LogInfo("DXGI Device Info Manager initialized successfully");
    } else {
      LogWarn("Failed to initialize DXGI Device Info Manager");
    }
  }

  return TRUE;
}


