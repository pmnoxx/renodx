#include "ui_common.hpp"
#include "ui_general_settings.hpp"
#include "ui_display_settings.hpp"
#include "ui_display_tab.hpp"
#include "ui_audio_settings.hpp"
#include "ui_window_controls.hpp"
#include "ui_developer_settings.hpp"

#include "ui_hdr_colorspace_settings.hpp"
#include "../nvapi/ui_nvapi_settings.hpp"
#include "ui_device_info.hpp"
#include "ui_window_info.hpp"
#include "../reflex/ui_reflex_settings.hpp"
#include "ui_latency_display.hpp"
#include "ui_swapchain_info.hpp"
#include "../dxgi/ui_dxgi_device_info.hpp"
#include "../dxgi/ui_dxgi_device_info_detailed.hpp"
#include "../dxgi/ui_dxgi_composition_info.hpp"

#include "../renodx/settings.hpp"

// UI/settings - main vector that includes all sections
renodx::utils::settings2::Settings settings = {};

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
    
    // ============================================================================
    // SIMPLE TAB (0) - Basic settings for everyday use
    // ============================================================================
    renodx::ui::AddGeneralSettings(settings);        // UI Tab selector (always visible)
    renodx::ui::AddDisplaySettings(settings);        // Basic display controls
    renodx::ui::AddDisplayTabSettings(settings);     // Display tab specific settings
    renodx::ui::AddAudioSettings(settings);          // Audio volume and mute controls
    renodx::ui::AddWindowControls(settings);         // Minimize/Restore buttons
    
    // ============================================================================
    // DEVELOPER TAB (1) - Advanced features and debugging
    // ============================================================================
    renodx::ui::AddDeveloperSettings(settings);      // Developer-specific controls
    renodx::ui::AddHdrColorspaceSettings(settings);  // HDR and colorspace settings
    renodx::ui::AddNvapiSettings(settings);          // NVAPI features and HDR logging
    renodx::ui::AddReflexSettings(settings);         // NVIDIA Reflex settings
    renodx::ui::AddLatencyDisplaySettings(settings); // Latency monitoring display
    
    // ============================================================================
    // DEVICE INFO TAB (2) - Graphics device information
    // ============================================================================
    renodx::ui::AddDeviceInfoSettings(settings);     // Basic device information
    renodx::ui::AddDxgiDeviceInfoSettings(settings); // DXGI device details
    renodx::ui::AddDxgiDeviceInfoDetailedSettings(settings); // Detailed DXGI information
    
    // ============================================================================
    // WINDOW INFO TAB (3) - Window debugging and state
    // ============================================================================
    renodx::ui::AddWindowInfoSettings(settings);     // Window state and debugging info
    
    // ============================================================================
    // SWAPCHAIN TAB (4) - Swapchain and composition information
    // ============================================================================
    renodx::ui::AddSwapchainInfoSettings(settings);  // Swapchain and adapter info
    renodx::ui::AddDxgiCompositionInfoSettings(settings); // DXGI composition details
    
    // Mark as initialized
    ui_settings_initialized = true;
}

