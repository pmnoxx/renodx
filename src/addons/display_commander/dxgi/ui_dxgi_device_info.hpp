#pragma once

#include <vector>
#include "../../../utils/settings.hpp"

namespace renodx::ui {

// Add DXGI device info settings to the settings vector
void AddDxgiDeviceInfoSettings(std::vector<renodx::utils::settings::Setting*>& settings);

} // namespace renodx::ui
