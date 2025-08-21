#include "ui_independent_flip_failures.hpp"
#include "ui_common.hpp"
#include "../renodx/settings.hpp"
#include "../utils.hpp"

namespace renodx::ui {

void AddIndependentFlipFailuresSettings(std::vector<renodx::utils::settings2::Setting*>& settings) {
    // Independent Flip failure reasons (text only)
    settings.push_back(new renodx::utils::settings2::Setting{
        .key = "IndependentFlipFailures",
        .value_type = renodx::utils::settings2::SettingValueType::CUSTOM,
        .default_value = 0.f,
        .label = "",
        .section = "Performance",
        .tooltip = "Reasons why Independent Flip is not working.",
        .on_draw = [](){
            IndependentFlipFailures* failures = g_if_failures.load();
            if (failures == nullptr) return false;
            
            // Check if there are any failures
            bool has_failures = failures->swapchain_null.load() ||
                               failures->device_null.load() ||
                               failures->non_dxgi_api.load() ||
                               failures->swapchain_media_failed.load() ||
                               failures->frame_stats_failed.load() ||
                               failures->not_flip_model.load() ||
                               failures->backbuffer_size_mismatch.load() ||
                               failures->window_size_mismatch.load() ||
                               failures->window_not_at_origin.load() ||
                               failures->window_layered.load() ||
                               failures->window_topmost.load() ||
                               failures->window_maximized.load() ||
                               failures->window_minimized.load() ||
                               failures->hwnd_null.load();
            
            if (has_failures) {
                ImGui::Separator();
                ImGui::Text("Independent Flip Debug Info:");
                
                if (failures->swapchain_null.load()) ImGui::BulletText("Swapchain is null");
                if (failures->device_null.load()) ImGui::BulletText("Device is null");
                if (failures->non_dxgi_api.load()) ImGui::BulletText("Non-DXGI API (OpenGL/Vulkan)");
                if (failures->swapchain_media_failed.load()) ImGui::BulletText("Swapchain media interface failed");
                if (failures->frame_stats_failed.load()) ImGui::BulletText("Frame statistics failed");
                if (failures->not_flip_model.load()) ImGui::BulletText("Not a flip-model swapchain");
                if (failures->backbuffer_size_mismatch.load()) ImGui::BulletText("Backbuffer size doesn't match monitor");
                if (failures->window_size_mismatch.load()) ImGui::BulletText("Window size doesn't match monitor");
                if (failures->window_not_at_origin.load()) ImGui::BulletText("Window not at monitor origin (0,0)");
                if (failures->window_layered.load()) ImGui::BulletText("Window has WS_EX_LAYERED style");
                if (failures->window_topmost.load()) ImGui::BulletText("Window has WS_EX_TOPMOST style");
                if (failures->window_maximized.load()) ImGui::BulletText("Window is maximized");
                if (failures->window_minimized.load()) ImGui::BulletText("Window is minimized");
                if (failures->hwnd_null.load()) ImGui::BulletText("Window handle is null");
            }
            
            return false;
        },
        .is_visible = []() { return is_developer_tab(s_ui_mode); } // Only show in Developer mode
    });
}

} // namespace renodx::ui
