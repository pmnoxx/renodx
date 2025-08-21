#pragma once

#include <vector>
#include "../renodx/settings.hpp"

namespace renodx::ui {

// Add DXGI device info settings to the settings vector
void AddDxgiDeviceInfoSettings(std::vector<renodx::utils::settings2::Setting*>& settings);

} // namespace renodx::ui
