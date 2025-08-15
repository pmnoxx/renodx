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





}
