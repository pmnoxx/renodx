#include "addon.hpp"

BOOL APIENTRY DllMain(HMODULE h_module, DWORD fdw_reason, LPVOID lpv_reserved) {
  switch (fdw_reason) {
    case DLL_PROCESS_ATTACH:
      if (!reshade::register_addon(h_module)) return FALSE;
      reshade::register_event<reshade::addon_event::init_swapchain>(OnInitSwapchain);
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

            return false; // not handled; call original
          });

      renodx::utils::settings::use_presets = false;
      renodx::utils::settings::global_name = "ResolutionOverride";
      g_attach_time = std::chrono::steady_clock::now();
      g_shutdown.store(false);
      std::thread(RunBackgroundAudioMonitor).detach();
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
      
      // Clean up Alt-Tab hook if it's installed
      UninstallAltTabHook();
      
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

  return TRUE;
}
