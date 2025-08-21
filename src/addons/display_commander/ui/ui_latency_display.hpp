#pragma once

#include <vector>
#include "../renodx/settings.hpp"

namespace renodx::ui {

// Add latency display settings to the settings vector
void AddLatencyDisplaySettings(std::vector<renodx::utils::settings2::Setting*>& settings);

} // namespace renodx::ui
