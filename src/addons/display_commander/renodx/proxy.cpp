#include "proxy.hpp"
#include "../addon.hpp"


namespace renodx::proxy {

// ============================================================================
// Settings System Proxy Implementation
// ============================================================================

void SetUsePresets(bool use) {
    renodx::utils::settings2::use_presets = use;
}

void SetGlobalName(const std::string& name) {
    renodx::utils::settings2::global_name = name;
}

void InitializeSettings(DWORD fdw_reason, void* settings_vector) {
    auto* settings = static_cast<renodx::utils::settings2::Settings*>(settings_vector);
    renodx::utils::settings2::Use(fdw_reason, settings);
}

} // namespace renodx::proxy
