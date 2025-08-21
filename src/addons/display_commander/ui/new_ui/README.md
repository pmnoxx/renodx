# New UI System

This directory contains a modern ImGui-based UI system that replaces the old settings-based UI.

## Overview

The new UI system provides:
- **Tab-based interface** with clean separation of concerns
- **ImGui integration** for modern, responsive UI
- **Easy extensibility** for adding new tabs and content
- **Integration with existing settings** system for gradual migration

## Files

- `new_ui_tabs.hpp/cpp` - Tab management system
- `new_ui_main.hpp/cpp` - Main UI system entry point
- `new_ui_wrapper.hpp/cpp` - Integration with existing settings
- `new_ui.hpp` - Main header that includes everything

## Usage

### Adding a New Tab

```cpp
#include "new_ui/new_ui.hpp"

// In your initialization code:
new_ui::g_tab_manager.AddTab("My Tab", "my_tab", []() {
    ImGui::Text("My Tab Content");
    
    if (ImGui::Button("My Button")) {
        // Handle button click
    }
    
    // Add more UI elements...
});
```

### Adding Content to Existing Tabs

Modify the `InitializeNewUI()` function in `new_ui_tabs.cpp` to add your content to the appropriate tab.

### Integration

The new UI system is automatically integrated into the existing settings system via a single `::CUSTOM` element. This means:

1. It appears as the first setting in the UI
2. It renders the entire tab system
3. It's fully compatible with the existing codebase

### Old UI Integration

The old UI system now only renders when the **Main tab** (index 0) is selected in the new UI:

- **Main Tab (0)**: Shows all old UI settings (legacy system)
- **Other Tabs (1-4)**: Show only new UI content, old settings are hidden

This provides a clean migration path where users can:
- Use the Main tab to access all legacy functionality
- Use other tabs for the new, improved UI experience
- Gradually transition between old and new systems

## Migration Path

1. **Phase 1**: New UI system runs alongside old UI (current state)
2. **Phase 2**: Gradually move individual settings to new UI tabs
3. **Phase 3**: Remove old settings system entirely

## Benefits

- **Better UX**: Cleaner, more intuitive interface
- **Easier maintenance**: Tab-based organization
- **Modern look**: ImGui provides better visual design
- **Performance**: More efficient rendering
- **Extensibility**: Easy to add new features

## Future Enhancements

- Configurable tab visibility
- Tab-specific settings persistence
- Advanced layout options
- Theme support
- Responsive design for different screen sizes
