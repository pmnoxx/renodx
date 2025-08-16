#include "addon.hpp"

// Include the UI settings header to get the settings vector declaration
#include "ui_settings.hpp"
#include "settings.hpp"
#include "ui/ui_main.hpp"
#include "background_tasks/background_task_coordinator.hpp"
#include "reshade_events/fullscreen_prevention.hpp"
#include "renodx/proxy.hpp"



// Forward declarations for continuous monitoring functions
void StartContinuousMonitoring();
void StopContinuousMonitoring();

// Forward declarations for continuous rendering functions
void StartContinuousRendering();
void StopContinuousRendering();

// Forward declarations for ReShade event handlers
void OnInitEffectRuntime(reshade::api::effect_runtime* runtime);
bool OnReShadeOverlayOpen(reshade::api::effect_runtime* runtime, bool open, reshade::api::input_source source);

// External declarations for settings
extern float s_remove_top_bar;

// ReShade effect runtime event handler for input blocking
void OnInitEffectRuntime(reshade::api::effect_runtime* runtime) {
    if (runtime != nullptr) {
        g_reshade_runtime.store(runtime);
        LogInfo("ReShade effect runtime initialized - Input blocking now available");
    }
}

// ReShade overlay event handler for input blocking
bool OnReShadeOverlayOpen(reshade::api::effect_runtime* runtime, bool open, reshade::api::input_source source) {
    if (open) {
        LogInfo("ReShade overlay opened - Input blocking active");
        // When ReShade overlay opens, we can also use its input blocking
        if (runtime != nullptr) {
            g_reshade_runtime.store(runtime);
        }
    } else {
        LogInfo("ReShade overlay closed - Input blocking inactive");
    }
    return false; // Don't prevent ReShade from opening/closing the overlay
}

BOOL APIENTRY DllMain(HMODULE h_module, DWORD fdw_reason, LPVOID lpv_reserved) {
  switch (fdw_reason) {
    case DLL_PROCESS_ATTACH:
      if (!reshade::register_addon(h_module)) return FALSE;
      reshade::register_event<reshade::addon_event::init_swapchain>(OnInitSwapchain);
      
      // Register ReShade effect runtime events for input blocking
      reshade::register_event<reshade::addon_event::init_effect_runtime>(OnInitEffectRuntime);
      reshade::register_event<reshade::addon_event::reshade_open_overlay>(OnReShadeOverlayOpen);
      
      // Defer NVAPI / Reflex init until after settings are loaded below
      
      // Register our fullscreen prevention event handler
      reshade::register_event<reshade::addon_event::set_fullscreen_state>(
          renodx::display_commander::events::OnSetFullscreenState);

      renodx::proxy::SetUsePresets(false);
      renodx::proxy::SetGlobalName("ResolutionOverride");
      g_attach_time = std::chrono::steady_clock::now();
      g_shutdown.store(false);
      std::thread(RunBackgroundAudioMonitor).detach();
      renodx::proxy::StartBackgroundTasks();
      
      // Install window hooks for style management
      renodx::proxy::InstallAllHooks();
      
      // NVAPI HDR monitor will be started after settings load below if enabled
      // Seed default fps limit snapshot
      g_default_fps_limit.store(renodx::proxy::GetFpsLimit());
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
      
      // CONTINUOUS RENDERING CLEANUP REMOVED - Focus spoofing is now handled by Win32 hooks
      
      // Clean up Reflex hooks if they're installed
      UninstallReflexHooks();
      
      // Clean up window hooks
      renodx::proxy::UninstallAllHooks();
      
      // Clean up DXGI Device Info Manager
      g_dxgiDeviceInfoManager.reset();
      
      reshade::unregister_event<reshade::addon_event::present>(OnPresentUpdate);
      reshade::unregister_event<reshade::addon_event::set_fullscreen_state>(
          renodx::display_commander::events::OnSetFullscreenState);
      reshade::unregister_event<reshade::addon_event::init_swapchain>(OnInitSwapchain);
      reshade::unregister_addon(h_module);
      g_shutdown.store(true);
      break;
  }

  // Initialize UI settings before registering the overlay
  InitializeUISettings();
  
  renodx::proxy::InitializeSettings(fdw_reason, &settings);
  renodx::proxy::InitializeSwapchain(fdw_reason);

  // Initialize hooks if settings are enabled on startup (after settings are loaded)
  if (fdw_reason == DLL_PROCESS_ATTACH) {

    
    // Check if continuous monitoring should be enabled
    if (s_continuous_monitoring_enabled >= 0.5f) {
      StartContinuousMonitoring();
      LogInfo("Continuous monitoring started proactively");
    }

    // CONTINUOUS RENDERING INITIALIZATION REMOVED - Focus spoofing is now handled by Win32 hooks
    
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

// CONTINUOUS RENDERING FUNCTIONS REMOVED - Focus spoofing is now handled by Win32 hooks

// CONTINUOUS RENDERING THREAD REMOVED - Focus spoofing is now handled by Win32 hooks
// This provides a much cleaner and more effective solution


