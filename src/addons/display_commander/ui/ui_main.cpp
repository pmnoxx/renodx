#include "ui_common.hpp"
#include "ui_general_settings.hpp"
#include "ui_display_settings.hpp"
#include "ui_display_tab.hpp"
#include "ui_audio_settings.hpp"
#include "ui_window_controls.hpp"
#include "ui_developer_settings.hpp"

#include "ui_hdr_colorspace_settings.hpp"
#include "../nvapi/ui_nvapi_settings.hpp"
// ui_device_info.hpp removed - moved to new UI system
// ui_window_info.hpp removed - moved to new UI system
#include "../reflex/ui_reflex_settings.hpp"
#include "ui_latency_display.hpp"
// ui_swapchain_info.hpp removed - moved to new UI system
// ../dxgi/ui_dxgi_device_info.hpp removed - moved to new UI system
// ../dxgi/ui_dxgi_device_info_detailed.hpp removed - moved to new UI system
// ../dxgi/ui_dxgi_composition_info.hpp removed - moved to new UI system
#include "new_ui/new_ui.hpp"

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
    
    // Add ImGui tabs here, as ::CUSTOM element type
    new_ui::AddNewUISettings(settings);              // New ImGui-based UI system with tabs
    
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
    // Moved to new UI system - no longer using old settings
    
    // ============================================================================
    // WINDOW INFO TAB (3) - Window debugging and state
    // ============================================================================
    // Moved to new UI system - no longer using old settings
    
    // ============================================================================
    // SWAPCHAIN TAB (4) - Swapchain and composition information
    // ============================================================================
    // Moved to new UI system - no longer using old settings
    
    // Mark as initialized
    ui_settings_initialized = true;
}

