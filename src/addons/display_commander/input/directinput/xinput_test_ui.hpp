#pragma once

#include <string>

namespace renodx::input::direct_input::test {

// Render the XInput test UI panel
void RenderXInputTestPanel();

// Get the current test status for display
std::string GetXInputTestStatusText();

// Check if any XInput test is currently running
bool IsAnyXInputTestRunning();

} // namespace renodx::input::direct_input::test

