#pragma once

#include <vector>
#include "../renodx/settings.hpp"

namespace renodx::ui {

// Function to add DXGI composition and backbuffer information settings
void AddDxgiCompositionInfoSettings(std::vector<renodx::utils::settings2::Setting*>& settings);

} // namespace renodx::ui
