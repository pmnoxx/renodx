#pragma once

#include <string>

namespace renodx::input::direct_input::test {

// Render the DirectInput test UI panel
void RenderDirectInputTestPanel();

// Get the current test status for display
std::string GetTestStatusText();

// Check if any test is currently running
bool IsAnyTestRunning();

} // namespace renodx::input::direct_input::test
