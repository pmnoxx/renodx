#include "ui_common.hpp"

namespace renodx::ui {

void AddDebugSettings(std::vector<renodx::utils::settings2::Setting*>& settings) {
    // Simple always-visible debug element to test UI rendering
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "UIRenderTest",
        .binding = nullptr,
        .value_type = renodx::utils::settings2::SettingValueType::TEXT,
        .default_value = 0.f,
        .label = "UI Test - If you see this, UI is working",
        .section = "Debug",
        .tooltip = "This element should always be visible to test UI rendering",
        .is_visible = []() { return true; }, // Always visible
    });
    
    // Simple test of atomic variables (always visible)
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "AtomicTest",
        .binding = nullptr,
        .value_type = renodx::utils::settings2::SettingValueType::CUSTOM, // only custom supports ImGui::Text
        .default_value = 0.f,
        .label = "Atomic Test - Direct Value Display",
        .section = "Debug2",
        .tooltip = "Test direct atomic variable access",
        .on_draw = []() -> bool {
            extern std::atomic<float> g_current_latency_ms;
            extern std::atomic<uint64_t> g_current_frame;
            float latency = ::g_current_latency_ms.load();
            uint64_t frame = ::g_current_frame.load();
            
            // Test different ImGui text methods
            ImGui::Text("Method 1 - Direct: %f ms", latency);
            ImGui::Text("Method 2 - Fixed: %.2f ms", latency);
            
            // Method 3 - Convert to string first
            std::string latency_str = std::to_string(latency);
            ImGui::Text("Method 3 - String: %s ms", latency_str.c_str());
            
            // Method 4 - Use TextUnformatted (no formatting)
            std::ostringstream oss;
            oss << "Method 4 - OStream: " << std::fixed << std::setprecision(2) << latency << " ms";
            ImGui::TextUnformatted(oss.str().c_str());
            
            // Method 5 - Simple concatenation
            ImGui::Text("Method 5 - Simple: %s", ("Value: " + std::to_string(latency) + " ms").c_str());
            
            // Test with integer (frame counter)
            ImGui::Text("Frame Test - Integer: %llu", frame);
            ImGui::Text("Frame Test - String: %s", std::to_string(frame).c_str());
            
            // Show update frequency info
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "Update Info:");
            ImGui::Text("Values update every 10 frames");
            ImGui::Text("Current frame: %llu", frame);
            ImGui::Text("Last update: %llu frames ago", frame % 10);
            
            return false;
        },
        .is_visible = []() { return true; }, // Always visible
    });
}

} // namespace renodx::ui
