#include "ui_common.hpp"
#include "ui_general_settings.hpp"
#include "ui_display_settings.hpp"
#include "ui_audio_settings.hpp"
#include "ui_window_controls.hpp"
#include "ui_developer_settings.hpp"

#include "ui_hdr_colorspace_settings.hpp"
#include "../nvapi/ui_nvapi_settings.hpp"
#include "ui_device_info.hpp"
#include "ui_window_info.hpp"
#include "../reflex/ui_reflex_settings.hpp"
#include "ui_latency_display.hpp"
#include "ui_debug_settings.hpp"
#include "ui_swapchain_info.hpp"
#include "../dxgi/ui_dxgi_device_info.hpp"
#include "../dxgi/ui_dxgi_device_info_detailed.hpp"
#include "../dxgi/ui_dxgi_composition_info.hpp"
#include "ui_independent_flip_failures.hpp"
#include "ui_direct_input_status.hpp"
#include "../settings.hpp"

// UI/settings - main vector that includes all sections
renodx::utils::settings::Settings settings = {};

// Flag to prevent multiple initializations
static bool ui_settings_initialized = false;

// Initialize all settings sections
void InitializeUISettings() {
    // Prevent multiple initializations
    if (ui_settings_initialized) {
        return;
    }
    
    // Clear any existing settings to prevent duplicates
    settings.clear();
    
    // Add all settings sections
    renodx::ui::AddGeneralSettings(settings);
    renodx::ui::AddDisplaySettings(settings);
    renodx::ui::AddAudioSettings(settings);
    renodx::ui::AddWindowControls(settings);
    renodx::ui::AddDeveloperSettings(settings);

    renodx::ui::AddHdrColorspaceSettings(settings);
    renodx::ui::AddNvapiSettings(settings);
    renodx::ui::AddDeviceInfoSettings(settings);
    renodx::ui::AddWindowInfoSettings(settings);
    renodx::ui::AddReflexSettings(settings);
    renodx::ui::AddLatencyDisplaySettings(settings);
    renodx::ui::AddDebugSettings(settings);
    renodx::ui::AddSwapchainInfoSettings(settings);
    
    // Add DXGI device info settings
    renodx::ui::AddDxgiDeviceInfoSettings(settings);
    
    // Add detailed DXGI device info settings
    renodx::ui::AddDxgiDeviceInfoDetailedSettings(settings);
    
    // Add DXGI composition info settings
    renodx::ui::AddDxgiCompositionInfoSettings(settings);
    
    // Add Independent Flip failures settings
    renodx::ui::AddIndependentFlipFailuresSettings(settings);
    
    // Add DirectInput status settings
    renodx::ui::AddDirectInputStatusSettings(settings);
    
    // Mark as initialized
    ui_settings_initialized = true;
}

