#pragma once

#include <include/reshade.hpp>

namespace renodx::display_commander::events {

// Fullscreen prevention event handler
// Returns true to suppress the event, false to allow it to proceed
bool OnSetFullscreenState(reshade::api::swapchain* swapchain, bool fullscreen, void* hmonitor);

} // namespace renodx::display_commander::events
