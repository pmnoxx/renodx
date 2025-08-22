#include "ui_common.hpp"
#include "ui_display_settings.hpp"
#include "ui_display_tab.hpp"
#include "ui_audio_settings.hpp"
#include "ui_window_controls.hpp"

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
   // renodx::ui::AddGeneralSettings(settings);        // UI Tab selector (always visible)
    renodx::ui::AddDisplaySettings(settings);        // Basic display controls
    renodx::ui::AddDisplayTabSettings(settings);     // Display tab specific settings
    renodx::ui::AddAudioSettings(settings);          // Audio volume and mute controls
    renodx::ui::AddWindowControls(settings);         // Minimize/Restore buttons
    
    
    // Mark as initialized
    ui_settings_initialized = true;
}

