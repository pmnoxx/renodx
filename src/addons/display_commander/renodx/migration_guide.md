# RenoDX Migration Guide

This guide outlines the steps to replace RenoDX dependencies with alternative implementations, enabling Display Commander to work independently.

## Phase 1: Proxy Implementation

### 1.1 Create Proxy Interface
- [x] Create `renodx/` subfolder structure
- [x] Document all dependencies
- [x] Implement proxy interface in `proxy.hpp`
- [x] Implement proxy functions in `proxy.cpp`

### 1.2 Fullscreen Prevention Implementation ✅
- [x] **RESOLVED**: Display Commander uses its own event handler, avoids setting RenoDX variable
- [x] No conflicts between systems - both work independently
- [x] No redundant code - clean separation of concerns
- [ ] Test that fullscreen prevention works correctly with current approach

### 1.2 Update Display Commander Code
- [ ] Replace direct RenoDX calls with proxy calls
- [ ] Update `main_entry.cpp` to use proxy
- [ ] Update `ui_developer_settings.cpp` to use proxy
- [ ] Update all other files to use proxy

### 1.3 Test Proxy Layer
- [ ] Verify all functionality works through proxy
- [ ] Test with RenoDX backend
- [ ] Ensure no performance regression

## Phase 2: Alternative Implementation

### 2.1 Settings System Replacement
**Current**: `renodx::utils::settings`
**Alternative**: Custom configuration system

```cpp
// Implementation options:
// 1. INI file parser (simple, fast)
// 2. JSON parser (flexible, modern)
// 3. Registry-based (Windows native)
// 4. SQLite (persistent, queryable)

class ConfigManager {
public:
    bool LoadConfig(const std::string& filename);
    bool SaveConfig(const std::string& filename);
    void SetValue(const std::string& key, const std::string& value);
    std::string GetValue(const std::string& key, const std::string& default = "");
    
private:
    std::unordered_map<std::string, std::string> settings;
};
```

### 2.2 Background Task System Replacement
**Current**: `renodx::background`
**Alternative**: Custom thread pool

```cpp
class TaskManager {
public:
    void Start();
    void Stop();
    void AddTask(std::function<void()> task);
    void SetThreadCount(size_t count);
    
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop_requested = false;
};
```

### 2.3 Hooks System Replacement
**Current**: `renodx::hooks`
**Alternative**: Direct Windows API

```cpp
class WindowHookManager {
public:
    bool InstallHooks();
    void UninstallHooks();
    void SetWindowSubclass(HWND hwnd, WNDPROC proc);
    void RemoveWindowSubclass(HWND hwnd);
    
private:
    HHOOK mouse_hook = nullptr;
    HHOOK keyboard_hook = nullptr;
    std::unordered_map<HWND, WNDPROC> subclasses;
};
```

### 2.4 Swapchain Mods Replacement
**Current**: `renodx::mods::swapchain`
**Alternative**: Direct DXGI/D3D API calls

```cpp
class SwapchainManager {
public:
    void SetFullscreenPrevention(bool enabled);
    void SetForceBorderless(bool enabled);
    void SetForceScreenTearing(bool enabled);
    bool ModifySwapchain(IDXGISwapChain* swapchain);
    
private:
    bool prevent_fullscreen = false;
    bool force_borderless = false;
    bool force_tearing = false;
};
```

## Phase 3: Testing and Validation

### 3.1 Unit Tests
- [ ] Test each proxy function independently
- [ ] Verify error handling and edge cases
- [ ] Performance benchmarking

### 3.2 Integration Tests
- [ ] Test with real applications
- [ ] Verify all Display Commander features work
- [ ] Performance comparison with RenoDX

### 3.3 Compatibility Tests
- [ ] Test with different DirectX versions
- [ ] Test with various game engines
- [ ] Test on different Windows versions

## Implementation Timeline

### Week 1-2: Proxy Layer
- Implement proxy interface
- Update Display Commander code
- Basic testing

### Week 3-4: Settings System
- Implement custom config manager
- Replace RenoDX settings
- Test configuration persistence

### Week 5-6: Background Tasks
- Implement custom task manager
- Replace RenoDX background system
- Test FPS limiting and monitoring

### Week 7-8: Hooks System
- Implement Windows API hooks
- Replace RenoDX hooks
- Test window management

### Week 9-10: Swapchain Management
- Implement direct DXGI calls
- Replace RenoDX swapchain mods
- Test display features

### Week 11-12: Final Testing
- Comprehensive testing
- Performance optimization
- Documentation updates

## Risk Assessment

### High Risk
- **Hooks System**: Complex Windows API integration
- **Background Tasks**: Threading and synchronization
- **Performance**: Ensuring no regression

### Medium Risk
- **Settings System**: Data migration and compatibility
- **Swapchain Mods**: DirectX API complexity

### Low Risk
- **Documentation**: Code organization and clarity
- **Testing**: Validation and verification

## Rollback Plan

If issues arise during migration:
1. Keep RenoDX as fallback backend
2. Implement feature flags to switch between implementations
3. Gradual migration with dual support
4. Maintain RenoDX compatibility until stable

## Success Criteria

- [ ] All Display Commander features work without RenoDX
- [ ] No performance regression (>95% of original performance)
- [ ] No compatibility issues with supported applications
- [ ] Clean, maintainable codebase
- [ ] Comprehensive documentation and tests
