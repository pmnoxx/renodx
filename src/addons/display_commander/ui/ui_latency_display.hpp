#pragma once

#include <vector>
#include "../../../utils/settings.hpp"

namespace renodx::ui {

// Add latency display settings to the settings vector
void AddLatencyDisplaySettings(std::vector<renodx::utils::settings::Setting*>& settings);

} // namespace renodx::ui
