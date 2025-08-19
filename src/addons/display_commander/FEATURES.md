# Display Commander Features

## Status Legend
- ✅ **Implemented** - Feature is working and functional
- ❌ **Not Implemented** - Feature is planned but not yet implemented
- 🔧 **In Progress** - Feature is being worked on
- ⚠️ **Broken** - Feature was working but is currently broken

---

## 1. Reflex Support
- ✅ **Basic Reflex Injection** - Works, reflex limits FPS to 95% of max refresh rate
- ❌ **Reflex Low Latency Mode** - Not yet implemented
- ❌ **Reflex Markers Injection** - Not yet implemented  
- ❌ **Reflex Boost** - Not yet implemented

## 2. Input Handling
- ❌ **DirectInput/XInput Support** - Waiting for ReShade addon support to enable suspending mouse/keyboard/gamepad input in background

## 3. Display Management
- ✅ **Fullscreen Borderless** - Fully functional
- ✅ **Display Resolution/Refresh Rate Changing** - Fully functional
- ✅ **NVAPI HDR Fullscreen Borderless** - Fully functional

## 4. Advanced Features
- ⚠️ **DXGI Independent Flip State** - Previously working, currently broken in this version

## 5. Audio Control
- ✅ **Mute** - Fully functional
- ✅ **Volume Control** - Fully functional
- ✅ **Background Mute** - Fully functional

## 6. Continous rendering
- ✅ **FullScreenMode sproofing** - Fully functional
- ❌ **Disable input** - Waiting for DirectInput / Xinput Support.

---

## Implementation Notes
- Most core display and audio features are complete and working
- Reflex support is partially implemented with basic injection working
- Input handling is pending ReShade addon framework support
- DXGI Independent Flip State needs investigation and repair