#pragma once

#include <vector>
#include "../../../utils/settings.hpp"

namespace renodx::ui {

// Function to add Independent Flip failure reasons settings
void AddIndependentFlipFailuresSettings(std::vector<renodx::utils::settings::Setting*>& settings);

} // namespace renodx::ui
