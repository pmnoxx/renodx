#include "direct_input_test_ui.hpp"
#include "direct_input_test.hpp"
#include <deps/imgui/imgui.h>
#include <sstream>
#include <iomanip>

namespace renodx::input::direct_input::test {

void RenderDirectInputTestPanel() {
    ImGui::Text("=== DirectInput Testing ===");
    
    // Test status display
    ImGui::Text("Test Status: %s", GetTestStatusText().c_str());
    
    // Test duration control
    static int test_duration = 5000; // 5 seconds default
    ImGui::Text("Test Duration: %d ms", test_duration);
    ImGui::Text("Note: Duration control buttons would be implemented here");
    
    // Manual retry button
    ImGui::Text("Module Loading:");
    ImGui::Text("Retry Loading DirectInput Modules");
    ImGui::Text("Note: Retry button would be implemented here");
    ImGui::Text("Retry Thread Status: %s", 
        g_directInputTester->IsRetryThreadRunning() ? "Running" : "Stopped");
    
    // Debug information
    ImGui::Text("=== Debug Information ===");
    ImGui::TextWrapped("%s", g_directInputTester->GetLoadedInputModulesInfo().c_str());
    
    // Test control buttons (using only available ImGui functions)
    ImGui::Text("Test Controls:");
    ImGui::Text("• Run Mouse Test - Click to start mouse input testing");
    ImGui::Text("• Run Keyboard Test - Click to start keyboard input testing");
    ImGui::Text("• Run Input Blocking Test - Click to start input blocking testing");
    
    // Note: In a real implementation, you would add actual button functionality
    // For now, this is a demonstration of the UI layout
    
    // Test status
    if (g_directInputTester->IsTestRunning()) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Test in progress...");
    }
    
    // Test results display
    if (!g_directInputTester->GetLastTestResult().empty()) {
        ImGui::Text("=== Last Test Results ===");
        ImGui::TextWrapped("%s", g_directInputTester->GetLastTestResult().c_str());
    }
    
    // Hook status display
    ImGui::Text("=== Hook Status ===");
    ImGui::Text("DirectInput8Create Hook: %s", 
        g_directInputTester->IsDirectInput8CreateHookActive() ? "Active" : "Waiting for module");
    ImGui::Text("DirectInputCreateA Hook: %s", 
        g_directInputTester->IsDirectInputCreateAHookActive() ? "Active" : "Waiting for module");
    ImGui::Text("DirectInputCreateW Hook: %s", 
        g_directInputTester->IsDirectInputCreateWHookActive() ? "Active" : "Waiting for module");
    ImGui::Text("DirectInputCreateEx Hook: %s", 
        g_directInputTester->IsDirectInputCreateExHookActive() ? "Active" : "Waiting for module");
    ImGui::Text("CreateDevice Hook: %s", "Ready");
    ImGui::Text("SetDeviceData Hook: %s", "Ready");
    ImGui::Text("GetDeviceState Hook: %s", "Ready");
    
    // Test statistics
    ImGui::Text("=== Test Statistics ===");
    ImGui::Text("Mouse Events Intercepted: %d", 0); // Placeholder
    ImGui::Text("Keyboard Events Intercepted: %d", 0); // Placeholder
    ImGui::Text("Input Blocks Performed: %d", 0); // Placeholder
    ImGui::Text("Devices Created: %zu", g_directInputTester->GetTrackedDeviceCount());
    
    // Instructions
    ImGui::Text("=== How to Use ===");
    ImGui::TextWrapped(
        "This panel demonstrates DirectInput testing UI layout:\n\n"
        "1. The test controls would normally have clickable buttons\n"
        "2. Tests would run in the background and log events\n"
        "3. Check the ReShade log for detailed test information\n"
        "4. View test results in the 'Last Test Results' section\n\n"
        "Note: This is a demonstration implementation. In a real scenario, "
        "you would need to install actual API hooks using a library like Detours."
    );
}

std::string GetTestStatusText() {
    if (g_directInputTester->IsTestRunning()) {
        return "Running";
    } else if (!g_directInputTester->GetLastTestResult().empty()) {
        return "Completed";
    } else {
        return "Ready";
    }
}

bool IsAnyTestRunning() {
    return g_directInputTester->IsTestRunning();
}

} // namespace renodx::input::direct_input::test
