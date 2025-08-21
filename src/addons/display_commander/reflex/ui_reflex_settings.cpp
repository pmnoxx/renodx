#include "../ui/ui_common.hpp"
#include "../addon.hpp"

// External declarations for Reflex settings
extern float s_reflex_enabled;
extern float s_reflex_low_latency_mode;
extern float s_reflex_low_latency_boost;
extern float s_reflex_use_markers;
extern float s_reflex_debug_output;

namespace renodx::ui {

void AddReflexSettings(std::vector<renodx::utils::settings2::Setting*>& settings) {
    // Enable NVIDIA Reflex
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "ReflexEnabled",
        .binding = &s_reflex_enabled,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
        .default_value = 1.f, // Enabled by default
        .label = "Enable NVIDIA Reflex",
        .section = "Performance",
        .tooltip = "Enable NVIDIA Reflex for reduced latency. Requires NVIDIA GPU and drivers.",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current) {
            if (current >= 0.5f) {
                InstallReflexHooks();
            } else {
                UninstallReflexHooks();
            }
            // Mark that Reflex settings have changed to force sleep mode update
            extern std::atomic<bool> g_reflex_settings_changed;
            ::g_reflex_settings_changed.store(true);
        },
        .is_visible = []() { return is_basic_tab(); }, // Show in Basic mode
    });

    // Reflex Low Latency Mode
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "ReflexLowLatencyMode",
        .binding = &s_reflex_low_latency_mode,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Reflex Low Latency Mode",
        .section = "Performance",
        .tooltip = "Enable low latency mode for reduced input lag.",
        .labels = {"Disabled", "Enabled"},
        .is_enabled = []() { return false; }, // Disabled
        .on_change_value = [](float previous, float current) {
            // Mark that Reflex settings have changed to force sleep mode update
            extern std::atomic<bool> g_reflex_settings_changed;
            ::g_reflex_settings_changed.store(true);
        },
        .is_visible = []() { return is_basic_tab() && s_reflex_enabled >= 0.5f; }, // Show when Reflex is enabled
    });

    // Reflex Low Latency Boost
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "ReflexLowLatencyBoost",
        .binding = &s_reflex_low_latency_boost,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Reflex Low Latency Boost",
        .section = "Performance",
        .tooltip = "Request maximum GPU clock frequency for lower latency in CPU-limited scenarios.",
        .labels = {"Disabled", "Enabled"},
        .is_enabled = []() { return false; }, // Disabled
        .on_change_value = [](float previous, float current) {
            // Mark that Reflex settings have changed to force sleep mode update
            extern std::atomic<bool> g_reflex_settings_changed;
            ::g_reflex_settings_changed.store(true);
        },
        .is_visible = []() { return is_basic_tab() && s_reflex_enabled >= 0.5f; }, // Show when Reflex is enabled
    });

    // Reflex Use Markers
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "ReflexUseMarkers",
        .binding = &s_reflex_use_markers,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Reflex Use Markers",
        .section = "Performance",
        .tooltip = "Allow latency markers to be used for runtime optimizations.",
        .labels = {"Disabled", "Enabled"},
        .is_enabled = []() { return false; }, // Disabled
        .on_change_value = [](float previous, float current) {
            // Mark that Reflex settings have changed to force sleep mode update
            extern std::atomic<bool> g_reflex_settings_changed;
            ::g_reflex_settings_changed.store(true);
        },
        .is_visible = []() { return is_basic_tab() && s_reflex_enabled >= 0.5f; }, // Show when Reflex is enabled
    });

    // Reflex Debug Output (keep in developer tab)
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "ReflexDebugOutput",
        .binding = &s_reflex_debug_output,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
        .default_value = 1.f,
        .label = "Reflex Debug Output",
        .section = "Performance",
        .tooltip = "Enable or disable Reflex debug messages in the log.",
        .labels = {"Disabled", "Enabled"},
        .is_visible = []() { return is_developer_tab() && s_reflex_enabled >= 0.5f; }, // Keep in Developer mode
    });
}

} // namespace renodx::ui
