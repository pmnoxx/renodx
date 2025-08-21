#pragma once

#include <vector>

namespace renodx::utils::settings2 {
    class Setting;
}

namespace renodx::ui {
    void AddHdrColorspaceSettings(std::vector<renodx::utils::settings2::Setting*>& settings);
}
