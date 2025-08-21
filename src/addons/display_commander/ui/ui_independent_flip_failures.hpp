#pragma once

#include <vector>
#include "../renodx/settings.hpp"

namespace renodx::ui {

// Function to add Independent Flip failure reasons settings
void AddIndependentFlipFailuresSettings(std::vector<renodx::utils::settings2::Setting*>& settings);

} // namespace renodx::ui
