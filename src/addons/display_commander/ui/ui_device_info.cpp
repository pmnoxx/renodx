#include "ui_device_info.hpp"
#include "ui_common.hpp"

// External declarations for device info
extern std::atomic<HWND> g_last_swapchain_hwnd;
extern std::atomic<int> g_last_backbuffer_width;
extern std::atomic<int> g_last_backbuffer_height;
extern reshade::api::color_space g_current_colorspace;
extern std::string g_hdr10_override_status;
extern std::string g_hdr10_override_timestamp;
extern std::vector<MonitorInfo> g_monitors;

namespace renodx::ui {

void AddDeviceInfoSettings(std::vector<renodx::utils::settings2::Setting*>& settings) {
    // Device Information Section
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "DeviceInfoHeader",
        .binding = nullptr,
        .value_type = renodx::utils::settings2::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "=== Device Information ===",
        .section = "Device Info",
        .tooltip = "Current device and display information",
        .on_draw = []() {
            // Get current device info
            HWND hwnd = g_last_swapchain_hwnd.load();
            int bb_w = g_last_backbuffer_width.load();
            int bb_h = g_last_backbuffer_height.load();
            reshade::api::color_space colorspace = g_current_colorspace;
            std::string hdr_status = g_hdr10_override_status;
            std::string hdr_timestamp = g_hdr10_override_timestamp;
            
            // Display device information
            ImGui::Text("Current Window: %p", hwnd);
            ImGui::Text("Backbuffer Size: %dx%d", bb_w, bb_h);
            ImGui::Text("Colorspace: %d", static_cast<int>(colorspace));
            ImGui::Text("HDR10 Override: %s", hdr_status.c_str());
            if (hdr_status != "Not applied" && hdr_status != "Never") {
                ImGui::Text("HDR10 Timestamp: %s", hdr_timestamp.c_str());
            }
            
            // Display monitor information
            if (!g_monitors.empty()) {
                ImGui::Separator();
                ImGui::Text("Monitors (%zu):", g_monitors.size());
                for (size_t i = 0; i < g_monitors.size(); ++i) {
                    const auto& monitor = g_monitors[i];
                    ImGui::Text("Monitor %zu: %ldx%ld at (%ld,%ld)", 
                               i + 1, 
                               monitor.info.rcMonitor.right - monitor.info.rcMonitor.left,
                               monitor.info.rcMonitor.bottom - monitor.info.rcMonitor.top,
                               monitor.info.rcMonitor.left, monitor.info.rcMonitor.top);
                }
            }
            
            return true;
        },
        .is_visible = []() { return is_device_tab(s_ui_mode); }
    });

    // Refresh Device Info Button
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "RefreshDeviceInfo",
        .binding = nullptr,
        .value_type = renodx::utils::settings2::SettingValueType::BUTTON,
        .default_value = 0.f,
        .label = "Refresh Device Info",
        .section = "Device Info",
        .tooltip = "Refresh device information display",
        .on_click = []() -> bool {
            // Force refresh of device information
            LogInfo("Device information refreshed");
            return true;
        },
        .is_visible = []() { return is_device_tab(s_ui_mode); }
    });

    // Device Debug Mode
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "DeviceDebugMode",
        .binding = nullptr,
        .value_type = renodx::utils::settings2::SettingValueType::BOOLEAN,
        .default_value = 0.f,
        .label = "Device Debug Mode",
        .section = "Device Info",
        .tooltip = "Enable detailed device debugging information",
        .labels = {"Disabled", "Enabled"},
        .on_change_value = [](float previous, float current) {
            if (current >= 0.5f) {
                LogInfo("Device debug mode enabled");
            } else {
                LogInfo("Device debug mode disabled");
            }
        },
        .is_visible = []() { return is_device_tab(s_ui_mode); }
    });
}

} // namespace renodx::ui
