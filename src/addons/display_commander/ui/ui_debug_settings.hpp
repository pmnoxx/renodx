#pragma once

#include <vector>
#include "../../../utils/settings.hpp"

namespace renodx::ui {

// Add debug settings to the settings vector
void AddDebugSettings(std::vector<renodx::utils::settings::Setting*>& settings);

} // namespace renodx::ui
