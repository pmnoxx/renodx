# New UI Settings System

This document describes the new settings wrapper system that provides automatic loading and saving of settings to Reshade's config system.

## Overview

The new settings system replaces the old external variables approach with a clean, object-oriented system that:
- Automatically loads settings from Reshade config on first access
- Automatically saves settings when values change
- Provides type-safe wrappers for ImGui controls
- Eliminates the need for manual `SaveSetting()` calls

## Architecture

### Core Classes

- **`SettingBase`** - Abstract base class for all settings
- **`FloatSetting`** - For float values with min/max constraints
- **`IntSetting`** - For integer values with min/max constraints  
- **`BoolSetting`** - For boolean values
- **`ComboSetting`** - For dropdown/combo selections with predefined labels

### Wrapper Functions

- **`SliderFloatSetting()`** - Wraps `ImGui::SliderFloat` with automatic saving
- **`SliderIntSetting()`** - Wraps `ImGui::SliderInt` with automatic saving
- **`CheckboxSetting()`** - Wraps `ImGui::Checkbox` with automatic saving
- **`ComboSettingWrapper()`** - Wraps `ImGui::Combo` with automatic saving
- **`ButtonSetting()`** - Wraps `ImGui::Button` (no value to save)
- **`TextSetting()`** - Wraps `ImGui::Text`
- **`SeparatorSetting()`** - Wraps `ImGui::Separator`
- **`SpacingSetting()`** - Wraps `ImGui::Spacing`

## Usage Example

### 1. Define Settings in Your Tab

```cpp
// In your tab's header file
class MyTabSettings {
public:
    MyTabSettings();
    void LoadSettings();
    
    FloatSetting volume;
    BoolSetting enabled;
    ComboSetting quality;
    
private:
    std::vector<SettingBase*> all_settings_;
};

// Global instance
extern MyTabSettings g_my_tab_settings;
```

### 2. Initialize Settings

```cpp
// In your tab's implementation file
MyTabSettings::MyTabSettings()
    : volume("volume", 50.0f, 0.0f, 100.0f, "renodx_my_tab"),
      enabled("enabled", true, "renodx_my_tab"),
      quality("quality", 1, {"Low", "Medium", "High"}, "renodx_my_tab") {
    
    all_settings_ = {&volume, &enabled, &quality};
}

void MyTabSettings::LoadSettings() {
    LoadTabSettings(all_settings_);
}
```

### 3. Use in UI Code

```cpp
void DrawMyTab() {
    // Load settings on first access
    static bool settings_loaded = false;
    if (!settings_loaded) {
        g_my_tab_settings.LoadSettings();
        settings_loaded = true;
    }
    
    // Use wrapper functions - no manual saving needed!
    if (SliderFloatSetting(g_my_tab_settings.volume, "Volume", "%.0f%%")) {
        LogInfo("Volume changed");
    }
    
    if (CheckboxSetting(g_my_tab_settings.enabled, "Enable Feature")) {
        LogInfo("Feature enabled/disabled");
    }
    
    if (ComboSettingWrapper(g_my_tab_settings.quality, "Quality")) {
        LogInfo("Quality setting changed");
    }
}
```

## Key Benefits

1. **Automatic Persistence** - Settings are saved immediately when changed
2. **Type Safety** - Compile-time checking of setting types
3. **Clean Code** - No more manual `SaveSetting()` calls scattered throughout UI code
4. **Consistent Behavior** - All settings follow the same loading/saving pattern
5. **Easy Migration** - Simple to convert existing external variables to the new system

## Migration from Old System

### Before (Old System)
```cpp
// External variables
extern float s_volume;
extern bool s_enabled;

// Manual saving required
if (ImGui::SliderFloat("Volume", &s_volume, 0.0f, 100.0f)) {
    SaveSetting("renodx", "volume", s_volume);
}
```

### After (New System)
```cpp
// Settings object
FloatSetting volume("volume", 50.0f, 0.0f, 100.0f, "renodx");

// Automatic saving - no manual calls needed
if (SliderFloatSetting(volume, "Volume", "%.0f")) {
    // Setting is automatically saved!
}
```

## Configuration Sections

Settings are organized into sections in the Reshade config file:
- `renodx_main_tab` - Main tab settings
- `renodx_my_tab` - Custom tab settings (replace with your tab name)

## Best Practices

1. **Load Once** - Use static flag to load settings only once per tab
2. **Meaningful Keys** - Use descriptive setting keys for easy debugging
3. **Default Values** - Always provide sensible default values
4. **Validation** - Use min/max constraints to prevent invalid values
5. **Section Names** - Use consistent section naming convention

## Error Handling

The system automatically handles:
- Missing config values (uses defaults)
- Value clamping to valid ranges
- Config file I/O errors (graceful fallback)

## Performance

- Settings are loaded only once per tab on first access
- Saving happens only when values actually change
- Minimal overhead compared to manual saving approach
