#include "new_ui_tabs.hpp"
#include "swapchain_tab.hpp"
#include "window_info_tab.hpp"
#include "device_info_tab.hpp"
#include "../../addon.hpp"
#include <sstream>

namespace renodx::ui::new_ui {

// Global tab manager instance
TabManager g_tab_manager;

// TabManager implementation
TabManager::TabManager() : active_tab_(0) {
}

void TabManager::AddTab(const std::string& name, const std::string& id, std::function<void()> on_draw) {
    tabs_.push_back({name, id, on_draw, true});
}

void TabManager::Draw() {
    if (tabs_.empty()) {
        return;
    }
    
    // Draw tab bar
    if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_None)) {
        for (size_t i = 0; i < tabs_.size(); ++i) {
            if (!tabs_[i].is_visible) {
                continue;
            }
            
            if (ImGui::BeginTabItem(tabs_[i].name.c_str())) {
                active_tab_ = static_cast<int>(i);
                
                // Draw tab content
                if (tabs_[i].on_draw) {
                    tabs_[i].on_draw();
                }
                
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
}

// Initialize the new UI system
void InitializeNewUI() {
    // Add tabs with their content
    g_tab_manager.AddTab("Main", "main", []() {
   /*     ImGui::Text("Main Tab - Basic Settings");
        ImGui::Separator();
        
        // Add your main tab content here
        if (ImGui::Button("Test Button")) {
            LogInfo("Test button clicked from Main tab");
        }
        
        ImGui::Text("This is the main tab with basic settings.");
        ImGui::Text("Add your main UI elements here."); */
    });
    
    g_tab_manager.AddTab("Developer", "developer", []() {
      /*  ImGui::Text("Developer Tab - Advanced Features");
        ImGui::Separator();
        
        // Add your developer tab content here
        if (ImGui::Button("Debug Info")) {
            LogInfo("Debug info requested from Developer tab");
        }
        
        ImGui::Text("This is the developer tab with advanced features.");
        ImGui::Text("Add your developer UI elements here."); */
    });
    
    g_tab_manager.AddTab("Device Info", "device_info", []() {
        renodx::ui::new_ui::DrawDeviceInfoTab();
    });
    
    g_tab_manager.AddTab("Window Info", "window_info", []() {
        renodx::ui::new_ui::DrawWindowInfoTab();
    });
    
    g_tab_manager.AddTab("Swapchain", "swapchain", []() {
        renodx::ui::new_ui::DrawSwapchainTab();
    });
    
    LogInfo("New UI system initialized with tabs");
}

// Draw the new UI
void DrawNewUI() {
    g_tab_manager.Draw();
}

} // namespace renodx::ui::new_ui
