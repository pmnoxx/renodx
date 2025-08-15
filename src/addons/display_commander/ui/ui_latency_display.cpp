#include "ui_common.hpp"

namespace renodx::ui {

void AddLatencyDisplaySettings(std::vector<renodx::utils::settings::Setting*>& settings) {
    // Latency Display Section
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "LatencyDisplaySection",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::TEXT,
        .default_value = 0.f,
        .label = "=== Latency Information ===",
        .section = "Performance",
        .tooltip = "Real-time latency and Reflex status information",
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });
    
    // Current Latency Display
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "CurrentLatency",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "Current Latency: ",
        .section = "Performance",
        .tooltip = "Current frame latency in milliseconds",
        .on_draw = []() -> bool {
            extern std::atomic<float> g_current_latency_ms;
            float latency = ::g_current_latency_ms.load();
            
            // Use TextUnformatted for most reliable display
            std::ostringstream oss;
            oss << "Current Latency: " << std::fixed << std::setprecision(2) << latency << " ms";
            ImGui::TextUnformatted(oss.str().c_str());
            
            return false; // No value change
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });
    
    // PCL AV Latency Display (Most Important)
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "PCLLatency",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "PCL AV Latency: ",
        .section = "Performance",
        .tooltip = "PCL Average Latency (30-frame average) - This is what the NVIDIA overlay should show",
        .on_draw = []() -> bool {
            extern std::atomic<float> g_pcl_av_latency_ms;
            float pcl_latency = ::g_pcl_av_latency_ms.load();
            
            // Use TextUnformatted for most reliable display
            std::ostringstream oss;
            oss << "PCL AV Latency: " << std::fixed << std::setprecision(2) << pcl_latency << " ms";
            ImGui::TextUnformatted(oss.str().c_str());
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "(30-frame avg)");
            
            return false; // No value change
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });
    
    // Reflex Status Display
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "ReflexStatus",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "Reflex Status: ",
        .section = "Performance",
        .tooltip = "Current Reflex status and configuration",
        .on_draw = []() -> bool {
            extern std::atomic<bool> g_reflex_active;
            extern std::atomic<uint64_t> g_current_frame;
            bool is_active = ::g_reflex_active.load();
            uint64_t frame = ::g_current_frame.load();
            
            // Use TextUnformatted for most reliable display
            std::ostringstream oss;
            if (is_active) {
                oss << "Reflex Status: Active (Frame " << frame << ")";
            } else {
                oss << "Reflex Status: Inactive";
            }
            ImGui::TextUnformatted(oss.str().c_str());
            
            return false; // No value change
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });
    
    // Debug Button to show atomic variable values
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "DebugLatency",
        .binding = nullptr,
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "Debug: Show All Latency Values",
        .section = "Performance",
        .tooltip = "Click to log all current latency values for debugging",
        .on_draw = []() -> bool {
            if (ImGui::Button("Log All Latency Values")) {
                extern std::atomic<float> g_current_latency_ms;
                extern std::atomic<float> g_pcl_av_latency_ms;
                extern std::atomic<float> g_average_latency_ms;
                extern std::atomic<float> g_min_latency_ms;
                extern std::atomic<float> g_max_latency_ms;
                extern std::atomic<uint64_t> g_current_frame;
                extern std::atomic<bool> g_reflex_active;
                
                std::ostringstream oss;
                oss << "=== LATENCY DEBUG INFO ===" << std::endl;
                oss << "Current Latency: " << std::fixed << std::setprecision(2) << ::g_current_latency_ms.load() << " ms" << std::endl;
                oss << "PCL AV Latency: " << std::fixed << std::setprecision(2) << ::g_pcl_av_latency_ms.load() << " ms" << std::endl;
                oss << "Average Latency: " << std::fixed << std::setprecision(2) << ::g_average_latency_ms.load() << " ms" << std::endl;
                oss << "Min Latency: " << std::fixed << std::setprecision(2) << ::g_min_latency_ms.load() << " ms" << std::endl;
                oss << "Max Latency: " << std::fixed << std::setprecision(2) << ::g_max_latency_ms.load() << " ms" << std::endl;
                oss << "Current Frame: " << ::g_current_frame.load() << std::endl;
                oss << "Reflex Active: " << (::g_reflex_active.load() ? "Yes" : "No") << std::endl;
                oss << "========================";
                
                extern void LogInfo(const char* message);
                ::LogInfo(oss.str().c_str());
            }
            
            return false; // No value change
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); }, // Only show in Developer mode
    });
}

} // namespace renodx::ui
