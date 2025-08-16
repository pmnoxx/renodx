#include "fullscreen_prevention.hpp"
#include "../addon.hpp"
#include <windows.h>
#include <sstream>

namespace renodx::display_commander::events {

bool OnSetFullscreenState(reshade::api::swapchain* swapchain, bool fullscreen, void* hmonitor) {
    // Only prevent fullscreen attempts, not fullscreen->windowed transitions
    if (!fullscreen) {
        return false;
    }

    // Check if fullscreen prevention is enabled in Display Commander
    if (s_prevent_fullscreen < 0.5f) {
        return false;
    }

    // Log the prevention
    std::ostringstream oss;
    oss << "[Display Commander] Preventing fullscreen attempt - prevention enabled";
    LogInfo(oss.str().c_str());

    // Return true to suppress the fullscreen event
    return true;
}

} // namespace renodx::display_commander::events
