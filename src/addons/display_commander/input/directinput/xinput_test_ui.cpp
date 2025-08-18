#include "xinput_test_ui.hpp"
#include "xinput_test.hpp"
#include <deps/imgui/imgui.h>
#include <sstream>
#include <iomanip>

namespace renodx::input::direct_input::test {

void RenderXInputTestPanel() {
    ImGui::Text("=== XInput Testing ===");
    
    // Test status display
    ImGui::Text("Test Status: %s", GetXInputTestStatusText().c_str());
    
    // Test duration control
    static int test_duration = 5000; // 5 seconds default
    ImGui::Text("Test Duration: %d ms", test_duration);
    ImGui::Text("Note: Duration control buttons would be implemented here");
    
    // Manual retry button
    ImGui::Text("Module Loading:");
    ImGui::Text("Retry Loading XInput Modules");
    ImGui::Text("Note: Retry button would be implemented here");
    ImGui::Text("Retry Thread Status: %s", 
        g_xinputTester->IsRetryThreadRunning() ? "Running" : "Stopped");
    
    // Debug information
    ImGui::Text("=== Debug Information ===");
    ImGui::TextWrapped("%s", g_xinputTester->GetLoadedXInputModulesInfo().c_str());
    
    // Test control buttons (using only available ImGui functions)
    ImGui::Text("Test Controls:");
    ImGui::Text("• Run XInput Test - Click to start XInput testing");
    
    // Note: In a real implementation, you would add actual button functionality
    // For now, this is a demonstration of the UI layout
    
    // Test status
    if (g_xinputTester->IsTestRunning()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Test in progress...");
    }
    
    // Test results display
    if (!g_xinputTester->GetLastTestResult().empty()) {
        ImGui::Text("=== Last Test Results ===");
        ImGui::TextWrapped("%s", g_xinputTester->GetLastTestResult().c_str());
    }
    
    // Hook status display
    ImGui::Text("=== Hook Status ===");
    ImGui::Text("XInputGetState Hook: %s", 
        g_xinputTester->IsXInputGetStateHookActive() ? "Active" : "Inactive");
    ImGui::Text("XInputSetState Hook: %s", 
        g_xinputTester->IsXInputSetStateHookActive() ? "Active" : "Inactive");
    ImGui::Text("XInputGetCapabilities Hook: %s", 
        g_xinputTester->IsXInputGetCapabilitiesHookActive() ? "Active" : "Inactive");
    ImGui::Text("XInputEnable Hook: %s", 
        g_xinputTester->IsXInputEnableHookActive() ? "Active" : "Inactive");
    
    // Test statistics
    ImGui::Text("=== Test Statistics ===");
    ImGui::Text("Controller States Intercepted: %d", 0); // Placeholder
    ImGui::Text("Vibration Commands Intercepted: %d", 0); // Placeholder
    ImGui::Text("Capability Queries Intercepted: %d", 0); // Placeholder
    ImGui::Text("Enable Calls Intercepted: %d", 0); // Placeholder
    
    // Instructions
    ImGui::Text("=== How to Use ===");
    ImGui::TextWrapped(
        "This panel demonstrates XInput testing UI layout:\n\n"
        "1. The test controls would normally have clickable buttons\n"
        "2. Tests would run in the background and log events\n"
        "3. Check the ReShade log for detailed test information\n"
        "4. View test results in the 'Last Test Results' section\n\n"
        "Note: This is a demonstration implementation. In a real scenario, "
        "you would need to install actual API hooks using a library like Detours."
    );
}

std::string GetXInputTestStatusText() {
    if (g_xinputTester->IsTestRunning()) {
        return "Running";
    } else if (!g_xinputTester->GetLastTestResult().empty()) {
        return "Completed";
    } else {
        return "Ready";
    }
}

bool IsAnyXInputTestRunning() {
    return g_xinputTester->IsTestRunning();
}

} // namespace renodx::input::direct_input::test
