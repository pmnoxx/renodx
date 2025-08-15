#pragma once

#include <vector>
#include "../../../utils/settings.hpp"

namespace renodx::ui {

// Function to add DXGI composition and backbuffer information settings
void AddDxgiCompositionInfoSettings(std::vector<renodx::utils::settings::Setting*>& settings);

} // namespace renodx::ui
