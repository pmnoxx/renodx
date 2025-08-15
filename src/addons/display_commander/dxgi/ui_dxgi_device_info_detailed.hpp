#pragma once

#include <vector>
#include "../../../utils/settings.hpp"

namespace renodx::ui {

// Function to add detailed DXGI device information settings
void AddDxgiDeviceInfoDetailedSettings(std::vector<renodx::utils::settings::Setting*>& settings);

} // namespace renodx::ui
