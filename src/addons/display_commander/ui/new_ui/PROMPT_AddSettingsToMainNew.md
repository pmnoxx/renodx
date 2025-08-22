# PROMPT: Adding Settings to MainNew Page

## Overview

This prompt explains how to add settings to the MainNew page using the settings wrapper system that automatically handles ReShade configuration integration.

## What You Need to Do

### 1. **Create MainNew Settings Class**

Create a new file `main_new_tab_settings.hpp` and `main_new_tab_settings.cpp` similar to the Developer tab:

```cpp
// main_new_tab_settings.hpp
#pragma once

#include "settings_wrapper.hpp"
#include <vector>

namespace renodx::ui::new_ui {

class MainNewTabSettings {
public:
    MainNewTabSettings();
    ~MainNewTabSettings() = default;
    
    // Load all settings from ReShade config
    void LoadAll();
    
    // Save all settings to ReShade config
    void SaveAll();
    
    // Add your settings here
    renodx::ui::new_ui::BoolSetting your_setting_name;
    renodx::ui::new_ui::FloatSetting your_float_setting;
    renodx::ui::new_ui::IntSetting your_int_setting;
    
    // Get all settings for bulk operations
    std::vector<renodx::ui::new_ui::SettingBase*> GetAllSettings();
};

// Global instance
extern MainNewTabSettings g_mainNewTabSettings;

} // namespace renodx::ui::new_ui
```

### 2. **Implement the Settings Class**

```cpp
// main_new_tab_settings.cpp
#include "main_new_tab_settings.hpp"

namespace renodx::ui::new_ui {

MainNewTabSettings::MainNewTabSettings()
    : your_setting_name("YourSettingName", false, "DisplayCommanderNew")           // false = default value
    , your_float_setting("YourFloatSetting", 5.0f, 1.0f, 100.0f, "DisplayCommanderNew")  // 5.0f = default, 1.0f-100.0f = range
    , your_int_setting("YourIntSetting", 2, 0, 5, "DisplayCommanderNew")        // 2 = default, 0-5 = range
{
}

void MainNewTabSettings::LoadAll() {
    your_setting_name.Load();
    your_float_setting.Load();
    your_int_setting.Load();
}

void MainNewTabSettings::SaveAll() {
    your_setting_name.Save();
    your_float_setting.Save();
    your_int_setting.Save();
}

std::vector<renodx::ui::new_ui::SettingBase*> MainNewTabSettings::GetAllSettings() {
    return {
        &your_setting_name,
        &your_float_setting,
        &your_int_setting
    };
}

// Global instance
MainNewTabSettings g_mainNewTabSettings;

} // namespace renodx::ui::new_ui
```

### 3. **Update MainNew Tab UI**

In your existing MainNew tab file, add the settings wrapper integration:

```cpp
// In your MainNew tab render function
void DrawMainNewTab() {
    // Ensure settings are loaded
    static bool settings_loaded = false;
    if (!settings_loaded) {
        g_mainNewTabSettings.LoadAll();
        settings_loaded = true;
    }
    
    if (ImGui::BeginTabItem("MainNew")) {
        // Your existing UI code here...
        
        // Add settings with automatic save:
        if (CheckboxSetting(g_mainNewTabSettings.your_setting_name, "Your Setting Label")) {
            // Setting was changed and auto-saved
            // Add any additional logic here if needed
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Your tooltip text");
        }
        
        // Float slider with automatic save:
        if (SliderFloatSetting(g_mainNewTabSettings.your_float_setting, "Float Setting", "%.1f")) {
            // Setting was changed and auto-saved
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Your tooltip text");
        }
        
        // Integer slider with automatic save:
        if (SliderIntSetting(g_mainNewTabSettings.your_int_setting, "Int Setting", "%d")) {
            // Setting was changed and auto-saved
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Your tooltip text");
        }
        
        ImGui::EndTabItem();
    }
}
```

## Available Setting Types

### **BoolSetting** (Checkbox)
```cpp
renodx::ui::new_ui::BoolSetting setting_name("ConfigKey", default_value, "DisplayCommanderNew");
// Usage: CheckboxSetting(setting_name, "Label")
```

### **FloatSetting** (Slider)
```cpp
renodx::ui::new_ui::FloatSetting setting_name("ConfigKey", default_value, min_value, max_value, "DisplayCommanderNew");
// Usage: SliderFloatSetting(setting_name, "Label", "%.1f")
```

### **IntSetting** (Integer Slider)
```cpp
renodx::ui::new_ui::IntSetting setting_name("ConfigKey", default_value, min_value, max_value, "DisplayCommanderNew");
// Usage: SliderIntSetting(setting_name, "Label", "%d")
```

### **ComboSetting** (Dropdown)
```cpp
std::vector<const char*> labels = {"Option 1", "Option 2", "Option 3"};
renodx::ui::new_ui::ComboSetting setting_name("ConfigKey", default_index, labels, "DisplayCommanderNew");
// Usage: ComboSettingWrapper(setting_name, "Label")
```

## Key Benefits

1. **Automatic Persistence**: Settings save to ReShade config immediately when changed
2. **Consistent Section**: All settings use "DisplayCommanderNew" section
3. **Type Safety**: Each setting type handles validation and conversion
4. **Clean UI Code**: UI logic separated from configuration logic
5. **Automatic Validation**: Values are automatically clamped to valid ranges

## Configuration File Structure

Your settings will be saved to the ReShade config file like this:

```ini
[DisplayCommanderNew]
YourSettingName=1
YourFloatSetting=5.0
YourIntSetting=2
```

## Integration Steps Summary

1. **Create** `main_new_tab_settings.hpp/cpp` files
2. **Define** your settings with appropriate types and defaults
3. **Add** `LoadAll()` and `SaveAll()` methods
4. **Update** your MainNew tab UI to use the wrapper functions
5. **Test** that settings persist across application restarts

## Example Complete Implementation

See `developer_new_tab_settings.hpp/cpp` and `developer_new_tab.cpp` for a complete working example of how to implement this system.

## Important Notes

- **Settings are loaded once** when the tab is first opened (static bool prevents reloading)
- **Settings auto-save** when changed through the UI wrapper functions
- **Global variables** can be updated in the change handlers for backward compatibility
- **All settings use "DisplayCommanderNew" section** for consistency
- **Defaults are stored in the constructor** and used if ReShade config is missing

This system provides a clean, maintainable way to add persistent settings to your MainNew page with automatic ReShade integration.
