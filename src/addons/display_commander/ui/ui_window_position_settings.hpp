#pragma once

#include <vector>

namespace renodx::utils::settings {
    class Setting;
}

namespace renodx::ui {
    void AddWindowPositionSettings(std::vector<renodx::utils::settings::Setting*>& settings);
}
