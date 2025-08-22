# Settings Wrapper System for DisplayCommanderNew

This document explains how to use the new settings wrapper system that automatically handles ReShade configuration integration.

## Overview

The settings wrapper system provides automatic loading and saving of settings to/from ReShade's configuration system. When a setting value changes, it's automatically saved using `reshade::set_config_value()` with the section "DisplayCommanderNew".

## Key Benefits

- **Automatic persistence**: No manual save calls needed
- **Consistent section naming**: All settings use "DisplayCommanderNew" section
- **Clean separation**: UI logic separate from config logic
- **Type safety**: Each setting type handles its own data correctly
- **Immediate saving**: Settings are saved as soon as they change

## Setting Types Available

### BoolSetting
For boolean/checkbox settings:
```cpp
renodx::ui::new_ui::BoolSetting prevent_fullscreen("PreventFullscreen", false, "DisplayCommanderNew");
```

### FloatSetting
For float slider settings:
```cpp
renodx::ui::new_ui::FloatSetting hdr_interval("HDRInterval", 5.0f, 1.0f, 60.0f, "DisplayCommanderNew");
```

### IntSetting
For integer slider settings:
```cpp
renodx::ui::new_ui::IntSetting spoof_mode("SpoofMode", 0, 0, 2, "DisplayCommanderNew");
```

### ComboSetting
For dropdown/combo box settings:
```cpp
std::vector<const char*> labels = {"Off", "On", "Auto"};
renodx::ui::new_ui::ComboSetting reflex_mode("ReflexMode", 1, labels, "DisplayCommanderNew");
```

## Usage in UI

### 1. Initialize Settings
```cpp
// In your settings class constructor
DeveloperTabSettings::DeveloperTabSettings()
    : prevent_fullscreen("PreventFullscreen", false, "DisplayCommanderNew")
    , hdr_interval("HDRInterval", 5.0f, 1.0f, 60.0f, "DisplayCommanderNew")
{
}
```

### 2. Load Settings on Startup
```cpp
void DeveloperTabSettings::LoadAll() {
    prevent_fullscreen.Load();
    hdr_interval.Load();
    // ... load other settings
}
```

### 3. Render in UI with Auto-Save
```cpp
// Checkbox automatically saves when changed
if (CheckboxSetting(g_developerTabSettings.prevent_fullscreen, "Prevent Fullscreen")) {
    // Setting was changed and auto-saved
}

// Slider automatically saves when changed
if (SliderFloatSetting(g_developerTabSettings.hdr_interval, "HDR Interval", "%.1f")) {
    // Setting was changed and auto-saved
}
```

## UI Wrapper Functions

The system provides wrapper functions that automatically handle the ImGui controls and settings integration:

- `CheckboxSetting(BoolSetting&, const char* label)` - Checkbox with auto-save
- `SliderFloatSetting(FloatSetting&, const char* label, const char* format)` - Float slider with auto-save
- `SliderIntSetting(IntSetting&, const char* label, const char* format)` - Integer slider with auto-save
- `ComboSettingWrapper(ComboSetting&, const char* label)` - Combo box with auto-save

## Manual Save/Load

You can also manually control when settings are saved or loaded:

```cpp
// Manual save
g_developerTabSettings.prevent_fullscreen.Save();

// Manual load
g_developerTabSettings.prevent_fullscreen.Load();

// Save all settings
g_developerTabSettings.SaveAll();

// Load all settings
g_developerTabSettings.LoadAll();
```

## Configuration File Structure

Settings are saved to ReShade's configuration file with this structure:

```ini
[DisplayCommanderNew]
PreventFullscreen=1
HDRInterval=5.0
ReflexMode=1
```

## Migration from Old System

To migrate from the old manual save system:

1. Replace `NewUISetting` with appropriate wrapper types
2. Remove manual `reshade::set_config_value()` calls
3. Use the wrapper UI functions instead of direct ImGui calls
4. Initialize settings in constructor with proper keys and default values
5. Call `LoadAll()` on startup

## Example Implementation

See `developer_new_tab_settings.hpp/cpp` and `developer_new_tab_ui.cpp` for a complete example of how to implement the settings wrapper system.

## Benefits Over Old System

- **No more manual save calls**: Settings auto-save when changed
- **Consistent section naming**: All settings use "DisplayCommanderNew"
- **Type safety**: Each setting type handles validation and conversion
- **Cleaner code**: UI logic separated from configuration logic
- **Automatic validation**: Values are automatically clamped to valid ranges
- **Easier maintenance**: Adding new settings requires minimal code changes
