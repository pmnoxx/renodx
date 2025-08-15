#pragma once

#include <vector>
#include "../../../utils/settings.hpp"

namespace renodx::ui {

// Add swapchain info settings to the settings vector
void AddSwapchainInfoSettings(std::vector<renodx::utils::settings::Setting*>& settings);

} // namespace renodx::ui
