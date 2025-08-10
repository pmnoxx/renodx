## Resolution Override addon — implemented features and ideas

This document tracks what is implemented today and additional ideas inspired by Special‑K and other tooling. Feasibility varies per API/game.

### Implemented (current)
- Display/window
  - Remove Top Bar (borderless/WS_POPUP); style applied off main thread
  - Aspect resize mode (compute height from width and chosen aspect)
  - Apply and Auto‑Apply (delayed) run on a background thread; restore if maximized
  - Reposition If Out: clamp within target monitor (then virtual desktop)
  - Monitor targeting: choose monitor; position offsets to monitor origin
  - Force Windowed (experimental): one‑shot style apply
  - Prevent exclusive fullscreen / Force borderless (global toggles via swapchain hooks)
- Performance
  - FPS Limit (foreground default)
  - Background FPS Limit; when foreground and default limit is 0, runs unlimited
- Audio (per‑app via WASAPI)
  - Volume slider (0–100%) and Mute toggle
  - Mute in Background (auto‑mute when not foreground)
- Reliability/diagnostics
  - Threaded window ops; safe handling of maximized/hidden windows
  - Rich parameter logging for each operation (size, pos, style, flags)

### Display and window management (candidates)
- Borderless toggle UI preset with per‑game persistence
- Work‑area aware centering (rcWork vs rcMonitor)
- Snap layouts: halves/thirds/ultrawide bands
- Remember last window state: size/pos/style per profile
- Don’t minimize on focus loss (where OS/game allows)
- Cursor confinement to window/monitor; optional hotkey

### Swapchain and rendering (candidates)
- V‑Sync modes: vsync/allow tearing (DXGI present flags)
- Frametime target with smoothing (PI/EMA controller)
- Flip model hints (prefer flip‑discard/sequential) when safe
- Backbuffer format overrides (e.g., r16g16b16a16_float)
- Color space controls: extended sRGB/HDR10/HLG
- Monitor peak/SDR white helpers (query output HDR characteristics)

### Performance and timing (candidates)
- Frame pacing overlay: frametime graph, 1%/0.1% lows, sleep/spin metrics
- Power saver mode: coarser background sleeps, reduced polling

### Audio controls (candidates)
- Duck on background: reduce to X% instead of full mute when unfocused

### Input and quality‑of‑life (candidates)
- Raw input toggle
- Mouse sensitivity scaling (separate X/Y)
- Invert axis hotkey
- Controller vibration scaler (where accessible)
- On‑screen display (HUD): size/FPS/timing/vsync/HDR/audio state

### Automation and presets (candidates)
- Profiles: per‑game (size, style, fps/audio bg behavior)
- Auto‑apply per‑game overrides (timers, generation)
- Hotkey actions: apply, force windowed, toggle mute, move to monitor
- Safe sequencing: coalesce window ops during load screens

### Compatibility and safety (candidates)
- Focus‑loss resilience: re‑assert style after engine re‑parenting/DPI changes
- DPI awareness helpers: scale UI/positions for per‑monitor DPI

### Diagnostics and logging (candidates)
- Issue hints: detect exclusive fullscreen/OS snapping/DPI conflicts and suggest fixes
- Test actions: dry‑run mode to compute and log intended Win32 calls

### Stretch goals (advanced)
- **Dynamic resolution presets**: heuristics to pick next size based on GPU load (requires feedback loop).
- **Shader injection hooks**: simple per‑game shader toggles via ReShade pipeline (many games already supported elsewhere in repo).
- **Screenshot/record helpers**: quiesce overlays and trigger capture, save with metadata.
- **Window transparency/blur**: aesthetics for windowed modes on desktop.

## Proposed UI groupings
- **Display**: size, aspect, position, top bar, borderless/fullscreen policies, monitor targeting.
- **Performance**: FPS limits (foreground/global and background), frame pacing, vsync/tearing hints.
- **Audio**: master volume, mute, mute/duck in background.
- **Advanced**: color space, backbuffer format, device/swapchain diagnostics.
- **Shortcuts**: assignable hotkeys for frequent actions.

## Notes
- Some features depend on API capabilities (DXGI/Vulkan) and may not be universally applicable.
- Exclusive fullscreen prevention/force‑borderless is handled in swapchain hooks (mods layer) and exposed here as UI toggles.
- Window operations must avoid the game’s main thread to prevent crashes; current implementation uses detached threads.


