#pragma once

#include <vector>

namespace renodx::utils::settings {
    class Setting;
}

namespace renodx::ui {
    void AddDeveloperSettings(std::vector<renodx::utils::settings::Setting*>& settings);
}
