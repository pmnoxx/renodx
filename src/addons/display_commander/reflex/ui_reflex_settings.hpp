#pragma once

#include <vector>
#include "../renodx/settings.hpp"

namespace renodx::ui {

// Add Reflex settings to the settings vector
void AddReflexSettings(std::vector<renodx::utils::settings2::Setting*>& settings);

} // namespace renodx::ui
