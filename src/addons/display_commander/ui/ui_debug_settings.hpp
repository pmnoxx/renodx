#pragma once

#include <vector>
#include "../renodx/settings.hpp"

namespace renodx::ui {

// Add debug settings to the settings vector
void AddDebugSettings(std::vector<renodx::utils::settings2::Setting*>& settings);

} // namespace renodx::ui
