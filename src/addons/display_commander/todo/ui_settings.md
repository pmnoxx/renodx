# ui_settings.cpp - TODO and Issues

## Overview
This file defines the UI settings and their properties for the Display Commander addon. It includes settings for Reflex, window management, display settings, and various UI modes.

## Known Issues

### 🔴 Critical Issues
None currently identified.

### 🟠 High Priority Issues

#### 1. Reflex Feature Labels
- **Status**: Partially Resolved
- **Description**: Several Reflex features are labeled as "(NOT FUNCTIONAL)" in the UI
- **Impact**: User confusion about feature availability
- **Date Identified**: 2024-12-19
- **Related**: `s_reflex_low_latency_mode`, `s_reflex_low_latency_boost`, `s_reflex_use_markers`

#### 2. Window Info Mode Implementation
- **Status**: In Progress
- **Description**: New "Window Info" mode has been added but may need refinement
- **Impact**: New functionality may not be fully tested
- **Date Identified**: 2024-12-19

### 🟡 Medium Priority Issues

#### 1. UI Mode Organization
- **Status**: TODO
- **Description**: UI modes could be better organized and documented
- **Impact**: User experience and maintainability
- **Date Identified**: 2024-12-19

#### 2. Setting Validation
- **Status**: TODO
- **Description**: Some settings may not have proper validation
- **Impact**: Potential invalid configurations
- **Date Identified**: 2024-12-19

### 🟢 Low Priority Issues
None currently identified.

## TODO Items

### 🔧 Immediate Tasks

#### 1. Improve Reflex Feature Documentation
- [ ] Update Reflex feature descriptions to explain current limitations
- [ ] Add tooltips explaining what each feature does
- [ ] Consider adding status indicators for feature availability
- [ ] Document roadmap for feature completion

#### 2. Test Window Info Mode
- [ ] Test all Window Info mode functionality
- [ ] Verify window information display accuracy
- [ ] Test refresh and clear functionality
- [ ] Ensure proper error handling

### 📋 Future Improvements

#### 1. UI Organization
- [ ] Reorganize settings into more logical groups
- [ ] Add collapsible sections for better organization
- [ ] Implement search/filter functionality for settings
- [ ] Add setting categories and tags

#### 2. Setting Validation
- [ ] Add input validation for numeric settings
- [ ] Implement range checking for appropriate settings
- [ ] Add validation for file paths and other inputs
- [ ] Implement setting dependency validation

#### 3. User Experience
- [ ] Add setting presets for common configurations
- [ ] Implement setting import/export functionality
- [ ] Add setting change history
- [ ] Implement undo/redo for setting changes

## Technical Debt

### 1. Setting Organization
- **Issue**: Settings are not always logically grouped
- **Impact**: User confusion and maintenance overhead
- **Solution**: Reorganize settings into logical categories

### 2. Setting Validation
- **Issue**: Limited validation for setting values
- **Impact**: Potential invalid configurations
- **Solution**: Implement comprehensive setting validation

### 3. UI Mode Management
- **Issue**: UI modes could be better organized
- **Impact**: Code complexity and user experience
- **Solution**: Implement better mode organization and documentation

## Performance Considerations

### 1. Setting Updates
- **Issue**: Some settings may trigger expensive operations
- **Impact**: UI responsiveness during setting changes
- **Solution**: Implement async setting updates where appropriate

### 2. Window Information Display
- **Issue**: Window information may be expensive to gather
- **Impact**: UI performance during window info display
- **Solution**: Implement caching and lazy loading

### 3. UI Rendering
- **Issue**: Complex UI elements may impact rendering performance
- **Impact**: Reduced frame rates during UI interaction
- **Solution**: Optimize UI rendering and implement virtual scrolling

## Code Quality Issues

### 1. File Size
- **Issue**: File is quite large (1792 lines)
- **Impact**: Harder to maintain and understand
- **Solution**: Consider breaking into smaller, focused modules

### 2. Setting Definition
- **Issue**: Setting definitions are verbose and repetitive
- **Impact**: Maintenance overhead and potential inconsistencies
- **Solution**: Implement setting definition macros or templates

### 3. Error Handling
- **Issue**: Limited error handling for setting operations
- **Impact**: Difficult to debug issues
- **Solution**: Add comprehensive error handling and logging

## Related Files
- `globals.cpp` - Global variables for settings
- `reflex_management.cpp` - Reflex functionality
- `window_management.cpp` - Window management functionality
- `hooks/window_creation_detection.cpp` - Window creation detection

## Notes
- This file is central to the addon's user interface
- Many settings are interconnected and changes may have side effects
- Consider implementing a settings validation system
- Test all UI modes thoroughly to ensure proper functionality
- The file size suggests it may benefit from modularization
