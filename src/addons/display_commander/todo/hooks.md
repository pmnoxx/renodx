# hooks/ - TODO and Issues

## Overview
The hooks subdirectory contains modular hook implementations for various Windows API functions. This includes Alt-Tab suppression, window creation detection, window subclassing, and borderless style enforcement.

## Known Issues

### 🔴 Critical Issues
None currently identified.

### 🟠 High Priority Issues

#### 1. Delayed Win32 Calls Implementation
- **Status**: In Progress
- **Description**: All hooks now use delayed Win32 calls from separate threads to prevent locks and crashes
- **Impact**: Improved stability but may introduce timing issues
- **Date Identified**: 2024-12-19

#### 2. Hook Installation Failures
- **Status**: In Progress
- **Description**: Some hooks may fail to install during initialization
- **Impact**: Reduced functionality if hooks are not properly installed
- **Date Identified**: 2024-12-19

### 🟡 Medium Priority Issues

#### 1. Window Creation Detection Hooks
- **Status**: In Progress
- **Description**: Window creation detection hooks are "prepared" but require a hooking library for full implementation
- **Impact**: Limited window creation detection functionality
- **Date Identified**: 2024-12-19

#### 2. Hook Dependencies
- **Status**: TODO
- **Description**: Some hooks may have dependencies that affect installation order
- **Impact**: Potential initialization failures
- **Date Identified**: 2024-12-19

### 🟢 Low Priority Issues
None currently identified.

## TODO Items

### 🔧 Immediate Tasks

#### 1. Complete Hook Implementation
- [ ] Implement proper function hooking for window creation detection
- [ ] Test all hook functionality thoroughly
- [ ] Verify hook installation success rates
- [ ] Add hook health monitoring

#### 2. Test Delayed Win32 Calls
- [ ] Test that delayed Win32 calls prevent crashes
- [ ] Verify timing is appropriate for different operations
- [ ] Test with various games and applications
- [ ] Monitor for any side effects

### 📋 Future Improvements

#### 1. Hook Management
- [ ] Implement centralized hook management system
- [ ] Add hook dependency management
- [ ] Implement hook health monitoring
- [ ] Add automatic hook recovery

#### 2. Performance Optimization
- [ ] Optimize hook installation and removal
- [ ] Implement efficient hook execution
- [ ] Add hook performance monitoring
- [ ] Consider hook batching where appropriate

#### 3. Error Handling
- [ ] Improve error handling for hook failures
- [ ] Add fallback mechanisms for failed hooks
- [ ] Implement automatic retry for failed hooks
- [ ] Add user notification for hook issues

## Technical Debt

### 1. Hook Implementation Completeness
- **Issue**: Some hooks are not fully implemented
- **Impact**: Reduced functionality
- **Solution**: Complete implementation or clearly document limitations

### 2. Hook Dependencies
- **Issue**: Hook dependencies may not be properly managed
- **Impact**: Potential initialization failures
- **Solution**: Implement proper dependency management

### 3. Error Recovery
- **Issue**: Limited error recovery for hook failures
- **Impact**: Reduced functionality when hooks fail
- **Solution**: Implement comprehensive error recovery

## Performance Considerations

### 1. Hook Installation
- **Issue**: Hook installation may be expensive
- **Impact**: Startup delay
- **Solution**: Implement efficient hook installation and consider async installation

### 2. Hook Execution
- **Issue**: Hook execution may impact performance
- **Impact**: Reduced application performance
- **Solution**: Optimize hook execution and implement efficient filtering

### 3. Delayed Operations
- **Issue**: Delayed Win32 calls may introduce timing issues
- **Impact**: Potential race conditions
- **Solution**: Implement proper synchronization and timing controls

## Code Quality Issues

### 1. Hook Organization
- **Issue**: Hooks are now well-organized but may need further refinement
- **Impact**: Maintenance overhead
- **Solution**: Continue organizing and documenting hook functionality

### 2. Error Handling
- **Issue**: Error handling could be improved in some hooks
- **Impact**: Difficult to debug issues
- **Solution**: Add comprehensive error handling and logging

### 3. Hook Documentation
- **Issue**: Some hooks may not be fully documented
- **Impact**: Maintenance overhead
- **Solution**: Complete hook documentation and add usage examples

## Related Files
- `keyboard_hook.cpp` - Legacy hook functionality (being migrated)
- `main_entry.cpp` - Hook initialization and cleanup
- `ui_settings.cpp` - Hook-related UI controls

## Notes
- The hooks subdirectory represents a major refactoring improvement
- All hooks now use delayed Win32 calls to prevent crashes
- Hook functionality is now properly modularized
- Consider implementing hook health monitoring
- Test all hooks thoroughly to ensure proper functionality
- The modular approach makes hooks easier to maintain and debug
