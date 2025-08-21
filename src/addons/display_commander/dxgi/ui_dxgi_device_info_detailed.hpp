#pragma once

#include <vector>
#include "../renodx/settings.hpp"

namespace renodx::ui {

// Function to add detailed DXGI device information settings
void AddDxgiDeviceInfoDetailedSettings(std::vector<renodx::utils::settings2::Setting*>& settings);

} // namespace renodx::ui
