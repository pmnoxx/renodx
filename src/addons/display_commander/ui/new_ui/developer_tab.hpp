#pragma once

#include <deps/imgui/imgui.h>
#include <functional>

namespace renodx::ui::new_ui {

// Draw the developer tab content
void DrawDeveloperTab();

// Draw display-related developer settings
void DrawDisplayDeveloperSettings();

// Draw NVAPI settings
void DrawNvapiSettings();

// Draw Reflex settings
void DrawReflexSettings();

// Draw latency display settings
void DrawLatencyDisplaySettings();

// Draw HDR and colorspace settings
void DrawHdrColorspaceSettings();

} // namespace renodx::ui::new_ui
