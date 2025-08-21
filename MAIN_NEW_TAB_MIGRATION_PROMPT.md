# MainNew Tab Migration Prompt

## Objective
Migrate the Main tab functionality from the old settings-based UI system to the new ImGui-based UI system by creating a "MainNew" tab with identical functionality to the current basic/main tab settings.

## Background
Similar to the DeveloperNew tab migration that was recently completed, the Main tab contains essential user-facing settings that need to be converted from the `renodx::utils::settings2::Setting` system to direct ImGui widgets. The Main tab includes basic display controls, audio settings, and window management tools that users interact with regularly.

## Current Main Tab Settings (from ui_main.cpp lines 45-49)
The main tab currently includes these setting sections:
1. **AddGeneralSettings** - General display settings (some are developer-only)
2. **AddDisplaySettings** - Basic display controls 
3. **AddDisplayTabSettings** - Display tab specific settings
4. **AddAudioSettings** - Audio volume and mute controls
5. **AddWindowControls** - Minimize/Restore buttons

## Detailed Settings to Migrate

### 1. Display Settings Section (from ui_display_settings.cpp)
- **Window Width**: Dropdown with presets (Current Display, 1280, 1366, 1600, 1920, 2560, 3440, 3840)
- **Window Height**: Dropdown with presets (Current Display, 720, 900, 1080, 1200, 1440, 1600, 2160)
- **Resize Mode**: Combo box (Width/Height, Aspect Ratio)
- **Aspect Ratio**: Dropdown (3:2, 4:3, 16:10, 16:9, 19:9, 19.5:9, 21:9, 32:9) - only visible when Resize Mode = Aspect Ratio
- **Target Monitor**: Dropdown with available monitors
- **Remove Top Bar**: Checkbox to remove window title bar
- **Auto Apply**: Checkbox for automatic window changes
- **Apply Changes**: Button to force immediate window update
- **FPS Limit**: Slider (0-240 FPS) with "No Limit" option
- **Background FPS Limit**: Slider (0-240 FPS) with "No Limit" option

### 2. Audio Settings Section (from ui_audio_settings.cpp)
- **Audio Volume (%)**: Slider (0-100%) with immediate volume changes
- **Audio Mute**: Checkbox for manual mute/unmute
- **Mute In Background**: Checkbox to mute when game is not foreground

### 3. Window Controls Section (from ui_window_controls.cpp)
- **Minimize Window**: Button to minimize current window
- **Restore Window**: Button to restore minimized window
- Both buttons should be placed side by side

### 4. Basic Reflex Settings (from reflex/ui_reflex_settings.cpp)
- **Enable NVIDIA Reflex**: Checkbox (this appears in basic tab, not just developer)

## Implementation Requirements

### File Structure
Create these files:
- `src/addons/display_commander/ui/new_ui/main_new_tab.hpp` - Header with function declarations
- `src/addons/display_commander/ui/new_ui/main_new_tab.cpp` - Implementation with ImGui widgets

### External Variables to Include
Based on the settings analysis, these external variables need to be declared:
```cpp
extern float s_windowed_width;
extern float s_windowed_height; 
extern float s_resize_mode;
extern float s_aspect_index;
extern float s_target_monitor_index;
extern float s_remove_top_bar;
extern float s_auto_apply_enabled;
extern float s_fps_limit;
extern float s_fps_limit_background;
extern float s_audio_volume_percent;
extern float s_audio_mute;
extern float s_mute_in_background;
extern float s_reflex_enabled;
```

### Integration Steps
1. Add include for `main_new_tab.hpp` in `new_ui_tabs.cpp`
2. Add MainNew tab registration in `InitializeNewUI()` function
3. Ensure proper includes for external functions like `LogInfo`, audio management, window controls

### Key Implementation Details

#### Display Settings
- Use `ImGui::Combo()` for dropdowns with predefined options
- Implement `ImGui::SliderFloat()` for FPS limits with custom formatting
- Add "No Limit" option for FPS sliders (value 0 = no limit)
- Use proper tooltips with `ImGui::SetTooltip()` on hover
- Implement immediate changes for critical settings

#### Audio Settings  
- Use `ImGui::SliderFloat()` for volume with `"%d%%"` formatting
- Call audio management functions on change (`SetVolumeForCurrentProcess`, `SetMuteForCurrentProcess`)
- Include proper error handling and logging

#### Window Controls
- Use `ImGui::Button()` with proper sizing (48% width each, side by side)
- Execute window operations in background threads to avoid UI blocking
- Include error handling with Windows API error codes

#### Visual Organization
- Use `ImGui::TextColored()` for section headers
- Add `ImGui::Spacing()` and `ImGui::Separator()` between sections
- Group related settings logically
- Use consistent color scheme (0.8f, 0.8f, 0.8f, 1.0f) for headers

### Special Considerations

#### FPS Limiter Integration
- The FPS limiter uses a custom manager (`g_customFpsLimiterManager`)
- Need to handle "No Limit" state properly (value 0)
- Include proper logging for FPS changes

#### Monitor Selection
- Target monitor selection needs to work with monitor enumeration
- Should display monitor names/descriptions in dropdown

#### Auto Apply Functionality
- Auto apply should trigger `ApplyWindowChange()` function
- Include proper delay handling for auto apply

#### Conditional Visibility
- Some settings like Aspect Ratio should only show when Resize Mode = "Aspect Ratio"
- Window Height should be disabled when in Aspect Ratio mode

## Expected Outcome
A fully functional MainNew tab that:
1. Provides identical functionality to the current Main tab
2. Uses modern ImGui widgets for better UX
3. Maintains all existing behavior and callbacks
4. Integrates seamlessly with the new UI tab system
5. Can eventually replace the old Main tab once verified

## Testing Verification
After implementation:
1. Verify all settings work identically to the old Main tab
2. Test FPS limiting functionality
3. Test audio volume/mute controls
4. Test window minimize/restore buttons
5. Test monitor selection and window resizing
6. Ensure proper logging and error handling

## Notes
- Follow the same pattern used in DeveloperNew tab migration
- Maintain exact same variable names and functionality
- Use consistent styling and organization
- Add comprehensive tooltips for user guidance
- Ensure build compiles successfully
- Document any differences or improvements in implementation
