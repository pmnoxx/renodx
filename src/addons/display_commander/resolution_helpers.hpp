#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <sstream>

namespace renodx::resolution {

// Helper function to get available resolutions for a monitor
std::vector<std::string> GetResolutionLabels(int monitor_index);

// Helper function to get available refresh rates for a monitor and resolution
std::vector<std::string> GetRefreshRateLabels(int monitor_index, int width, int height);

// Helper function to get selected resolution
bool GetSelectedResolution(int monitor_index, int resolution_index, int& out_width, int& out_height);

// Helper function to get selected refresh rate
bool GetSelectedRefreshRate(int monitor_index, int width, int height, int refresh_rate_index, float& out_refresh_rate);

} // namespace renodx::resolution
