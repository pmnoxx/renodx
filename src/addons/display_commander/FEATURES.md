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
- ❌ **Mouse support** - features such as disable in background / restrict to window
- ❌ **Keyboard support** - alt-tab restoration
- ❌ **Gamepad support**
- ❌ **SteamAPI support**

## 3. Display Management
- ✅ **Fullscreen Borderless** - Fully functional
- ✅ **Display Resolution/Refresh Rate Changing** - Fully functional
- ✅ **NVAPI HDR Fullscreen Borderless** - Fully functional
- ❌ **VSync**
- ❌ **Audio Device**
- ❌ **Dithering**
- ❌ **Turning off screensaver**
- ❌ **Automatic display resolution restoration after game closes**


## 4. Advanced Features
- ⚠️ **DXGI Independent Flip State** - Used to work, currently broken.

## 5. Audio Control
- ✅ **Mute** - Fully functional
- ✅ **Volume Control** - Fully functional
- ✅ **Background Mute** - Fully functional

## 6. Continous rendering
- ✅ **FullScreenMode spoofing** - Fully functional
- ❌ **Disable input** - Waiting for DirectInput / Xinput Support.

## 7. Window manager
- ✅ **Custom window size support** - Fully functional. Width/Height or Width/Aspect Mode
- ✅ **Screen alignmnet** - Center + corners
- ✅ **Monitor selection** - Fully functional

## 8. Fps limiter
- ✅ **Basic version** - Waits on present - prioritizes fps stability over latency
- ❌ **Latency optimized version** - Not implemented

## 9. Percept boost / FakeHDR
- ❌ **Not implemented**

## 10. Latency monitoring
- ⚠️ **Partially implemented in UI** 00 neweds some work

## 11. Overlays
- ❌ **Latency overlay**
- ❌ **Auto overlay**

---

## Implementation Notes
- Most core display and audio features are complete and working
- Reflex support is partially implemented with basic injection working
- Input handling is pending ReShade addon framework support
- DXGI Independent Flip State needs investigation and repair