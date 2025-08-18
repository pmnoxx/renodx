#include "ui_direct_input_status.hpp"
#include "../input/directinput/direct_input.hpp"
#include "../input/directinput/direct_input_test_ui.hpp"
#include "../input/directinput/xinput_test.hpp"
#include "ui_common.hpp"
#include "../addon.hpp"
#include <deps/imgui/imgui.h>

namespace renodx::ui {

void AddDirectInputStatusSettings(std::vector<renodx::utils::settings::Setting*>& settings) {
    // DirectInput Status Overview
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "DirectInputStatus",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .label = "DirectInput Status",
        .section = "DirectInput Status",
        .tooltip = "Comprehensive DirectInput system status and information",
        .on_draw = []() -> bool {
            ImGui::Text("=== DirectInput System Status ===");
            
            if (!renodx::input::direct_input::g_directInputManager) {
                ImGui::Text("DirectInput Manager: Not Available");
                return true;
            }

            ImGui::Text("DirectInput Manager: Available");
            
            // Global enable/disable status
            bool enabled = renodx::input::direct_input::g_directInputManager->IsEnabled();
            ImGui::Text("DirectInput System: %s", enabled ? "ENABLED" : "DISABLED");
            
            // Device counts
            size_t mouse_count = renodx::input::direct_input::g_directInputManager->GetMouseDeviceCount();
            size_t keyboard_count = renodx::input::direct_input::g_directInputManager->GetKeyboardDeviceCount();
            size_t total_count = renodx::input::direct_input::g_directInputManager->GetTotalDeviceCount();
            
            ImGui::Text("Mouse Devices: %zu", mouse_count);
            ImGui::Text("Keyboard Devices: %zu", keyboard_count);
            ImGui::Text("Total Devices: %zu", total_count);
            
            // Input blocking status
            bool mouse_blocked = renodx::input::direct_input::g_directInputManager->IsBlockingMouseInput();
            bool keyboard_blocked = renodx::input::direct_input::g_directInputManager->IsBlockingKeyboardInput();
            
            ImGui::Separator();
            ImGui::Text("Input Blocking Status:");
            ImGui::Text("Mouse Input Blocking: %s", mouse_blocked ? "ENABLED" : "DISABLED");
            ImGui::Text("Keyboard Input Blocking: %s", keyboard_blocked ? "ENABLED" : "DISABLED");
            
            return true;
        },
        .is_visible = []() { return is_direct_input_tab(s_ui_mode); },
    });

    // Global Enable/Disable Control
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "DirectInputGlobalControl",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .label = "Global Control",
        .section = "Global Control",
        .tooltip = "Enable or disable the entire DirectInput system",
        .on_draw = []() -> bool {
            if (!renodx::input::direct_input::g_directInputManager) {
                ImGui::Text("DirectInput Manager not available");
                return true;
            }

            bool enabled = renodx::input::direct_input::g_directInputManager->IsEnabled();
            
            if (ImGui::Checkbox("Enable DirectInput System", &enabled)) {
                renodx::input::direct_input::g_directInputManager->SetEnabled(enabled);
            }
            
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("When disabled, all DirectInput functionality is turned off.\nWhen enabled, individual input blocking can be controlled below.");
            }
            
            return true;
        },
        .is_visible = []() { return is_direct_input_tab(s_ui_mode); },
    });

    // Input Blocking Controls
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "DirectInputBlockingControls",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .label = "Input Blocking Controls",
        .section = "Input Blocking Controls",
        .tooltip = "Control DirectInput input blocking",
        .on_draw = []() -> bool {
            if (!renodx::input::direct_input::g_directInputManager) {
                ImGui::Text("DirectInput Manager not available");
                return true;
            }

            // Check if system is enabled
            bool enabled = renodx::input::direct_input::g_directInputManager->IsEnabled();
            if (!enabled) {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "DirectInput System is DISABLED");
                ImGui::Text("Enable the system above to control input blocking");
                return true;
            }

            bool mouse_blocked = renodx::input::direct_input::g_directInputManager->IsBlockingMouseInput();
            bool keyboard_blocked = renodx::input::direct_input::g_directInputManager->IsBlockingKeyboardInput();

            ImGui::Text("Input Blocking Controls:");
            
            // Mouse blocking toggle
            if (ImGui::Checkbox("Block Mouse Input", &mouse_blocked)) {
                renodx::input::direct_input::g_directInputManager->SetBlockMouseInput(mouse_blocked);
            }
            
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("When enabled, all mouse input from DirectInput devices will be blocked.\nThis prevents mouse movement and clicks from being processed.");
            }

            // Keyboard blocking toggle
            if (ImGui::Checkbox("Block Keyboard Input", &keyboard_blocked)) {
                renodx::input::direct_input::g_directInputManager->SetBlockKeyboardInput(keyboard_blocked);
            }
            
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("When enabled, all keyboard input from DirectInput devices will be blocked.\nThis prevents key presses and releases from being processed.");
            }

            return true;
        },
        .is_visible = []() { return is_direct_input_tab(s_ui_mode); },
    });

    // Information about the implementation
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "DirectInputImplementationNotes",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .label = "Implementation Notes",
        .section = "Implementation Notes",
        .tooltip = "Information about the DirectInput input blocking implementation",
        .on_draw = []() -> bool {
            ImGui::Text("=== Implementation Notes ===");
            ImGui::Text("This DirectInput input blocking system is based on ReShade's approach:");
            ImGui::Text("• Hooks DirectInput creation functions (DirectInput8Create, DirectInputCreateA, etc.)");
            ImGui::Text("• Intercepts GetDeviceState and GetDeviceData calls");
            ImGui::Text("• Blocks input by zeroing device state data when blocking is enabled");
            ImGui::Text("• Supports both DirectInput 7 and DirectInput 8");
            ImGui::Text("• Mouse blocking preserves movement data to prevent camera resetting");
            ImGui::Text("• Keyboard blocking completely zeroes all key states");
            ImGui::Text("• Tracks all DirectInput devices for monitoring and control");
            
            ImGui::Text("");
            ImGui::Text("Note: Hook installation is currently placeholder - will be implemented");
            ImGui::Text("when ReShade integration is fully resolved.");
            
            return true;
        },
        .is_visible = []() { return is_direct_input_tab(s_ui_mode); },
    });

    // DirectInput Testing Panel
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "DirectInputTesting",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .label = "DirectInput Testing",
        .section = "DirectInput Testing",
        .tooltip = "Test DirectInput hooking and input blocking functionality",
        .on_draw = []() -> bool {
            // Render the DirectInput test panel
            renodx::input::direct_input::test::RenderDirectInputTestPanel();
            return true;
        },
        .is_visible = []() { return is_direct_input_tab(s_ui_mode); },
    });

    // XInput Input Blocking Controls
    settings.push_back(new renodx::utils::settings::Setting{
        .key = "XInputBlockingControls",
        .value_type = renodx::utils::settings::SettingValueType::CUSTOM,
        .label = "XInput Input Blocking Controls",
        .section = "XInput Input Blocking Controls",
        .tooltip = "Control XInput input blocking",
        .on_draw = []() -> bool {
            if (!renodx::input::direct_input::test::g_xinputTester) {
                ImGui::Text("XInput Tester not available");
                return true;
            }

            ImGui::Text("=== XInput Input Blocking ===");
            
            bool block_xinput = renodx::input::direct_input::test::g_xinputTester->IsBlockingXInputInput();
            if (ImGui::Checkbox("Block XInput Input", &block_xinput)) {
                renodx::input::direct_input::test::g_xinputTester->SetBlockXInputInput(block_xinput);
            }
            
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("When enabled, all XInput input will be blocked.\nThis prevents controller input from being processed.");
            }
            
            // Show current blocking status
            ImGui::Text("Current Status: %s", block_xinput ? "BLOCKING INPUT" : "ALLOWING INPUT");
            
            // Instructions
            ImGui::Text("=== Instructions ===");
            ImGui::TextWrapped("1. Enable XInput input blocking above to disable controller input");
            ImGui::TextWrapped("2. The blocking affects all XInput functions: GetState, SetState, GetCapabilities, Enable");
            ImGui::TextWrapped("3. When blocking is enabled, controllers will appear disconnected to the game");
            ImGui::TextWrapped("4. Use this to test input blocking functionality or disable controllers temporarily");
            
            return true;
        },
        .is_visible = []() { return is_direct_input_tab(s_ui_mode); },
    });
    }
    
    } // namespace renodx::ui
