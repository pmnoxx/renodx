#pragma once

#include <vector>
#include "../renodx/settings.hpp"

namespace renodx::ui {

// Add swapchain info settings to the settings vector
void AddSwapchainInfoSettings(std::vector<renodx::utils::settings2::Setting*>& settings);

} // namespace renodx::ui
