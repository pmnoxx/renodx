#include "proxy.hpp"
#include "../addon.hpp"

// Forward declarations for RenoDX includes
#include "../../../utils/settings.hpp"

namespace renodx::proxy {

// ============================================================================
// Settings System Proxy Implementation
// ============================================================================

void SetUsePresets(bool use) {
    renodx::utils::settings::use_presets = use;
}

void SetGlobalName(const std::string& name) {
    renodx::utils::settings::global_name = name;
}

void InitializeSettings(DWORD fdw_reason, void* settings_vector) {
    auto* settings = static_cast<renodx::utils::settings::Settings*>(settings_vector);
    renodx::utils::settings::Use(fdw_reason, settings);
}

} // namespace renodx::proxy
