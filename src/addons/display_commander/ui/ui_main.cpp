#include "ui_common.hpp"
#include "ui_display_tab.hpp"

#include "new_ui/new_ui.hpp"

#include "../renodx/settings.hpp"
#include "ui_display_tab.hpp"

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
    
    static bool cache_initialized = false;
    if (!cache_initialized) {
        renodx::ui::InitializeDisplayCache();
        cache_initialized = true;
    }

    // Clear any existing settings to prevent duplicates
    settings.clear();
    
    // Add ImGui tabs here, as ::CUSTOM element type
    new_ui::AddNewUISettings(settings);              // New ImGui-based UI system with tabs
    
    
    // Mark as initialized
    ui_settings_initialized = true;
}

