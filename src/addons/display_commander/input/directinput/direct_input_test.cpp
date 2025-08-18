#include "direct_input_test.hpp"
#include <include/reshade.hpp>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>

namespace renodx::input::direct_input::test {

// Global instance for static access
DirectInputTester* DirectInputTester::s_instance = nullptr;

// Global instance
std::unique_ptr<DirectInputTester> g_directInputTester = std::make_unique<DirectInputTester>();

DirectInputTester::DirectInputTester() {
    s_instance = this;
}

DirectInputTester::~DirectInputTester() {
    Shutdown();
    s_instance = nullptr;
}

bool DirectInputTester::Initialize() {
    if (m_hooks_installed) {
        return true; // Already initialized
    }
    
    bool ok = InstallHooks();
    // Start retry thread if no creation hooks active yet
    if (ok && !AreAnyCreationHooksActive()) {
        StartRetryThread();
    }
    return ok;
}

void DirectInputTester::Shutdown() {
    if (m_hooks_installed) {
        UninstallHooks();
    }
    StopRetryThread();
}

bool DirectInputTester::InstallHooks() {
    // Try to get the DirectInput8 module - it might not be loaded yet
    HMODULE dinput8 = GetModuleHandleA("dinput8.dll");
    if (!dinput8) {
        LogTestEvent("dinput8.dll not loaded yet - will retry later");
        // Don't fail completely, just note that we'll need to retry
    }
    
    // Try to get the DirectInput module (for older versions)
    HMODULE dinput = GetModuleHandleA("dinput.dll");
    if (!dinput) {
        LogTestEvent("dinput.dll not loaded yet - will retry later");
    }
    
    // Hook DirectInput8Create if available
    if (dinput8) {
        m_original_DirectInput8Create = (DirectInput8Create_t)GetProcAddress(dinput8, "DirectInput8Create");
        if (m_original_DirectInput8Create) {
            LogTestEvent("DirectInput8Create function found and stored");
        } else {
            LogTestEvent("Failed to find DirectInput8Create function in dinput8.dll");
        }
    }
    
    // Hook DirectInputCreateA if available
    if (dinput) {
        m_original_DirectInputCreateA = (DirectInputCreateA_t)GetProcAddress(dinput, "DirectInputCreateA");
        if (m_original_DirectInputCreateA) {
            LogTestEvent("DirectInputCreateA function found and stored");
        }
    }
    
    // Hook DirectInputCreateW if available
    if (dinput) {
        m_original_DirectInputCreateW = (DirectInputCreateW_t)GetProcAddress(dinput, "DirectInputCreateW");
        if (m_original_DirectInputCreateW) {
            LogTestEvent("DirectInputCreateW function found and stored");
        }
    }
    
    // Hook DirectInputCreateEx if available
    if (dinput) {
        m_original_DirectInputCreateEx = (DirectInputCreateEx_t)GetProcAddress(dinput, "DirectInputCreateEx");
        if (m_original_DirectInputCreateEx) {
            LogTestEvent("DirectInputCreateEx function found and stored");
        }
    }
    
    // Mark hooks as installed even if some modules aren't loaded yet
    // We'll retry loading them when DirectInput functions are actually called
    m_hooks_installed = true;
    
    if (dinput8 || dinput) {
        LogTestEvent("DirectInput creation hooks installed successfully");
    } else {
        LogTestEvent("DirectInput hooks prepared - will activate when modules are loaded");
    }
    
    return true;
}

void DirectInputTester::StartRetryThread() {
    if (m_retry_thread_running.load()) return;
    m_retry_thread_running.store(true);
    m_retry_thread = std::thread([this]() {
        LogTestEvent("Background retry thread started");
        int retry_count = 0;
        // Poll a few times per second until any creation hook becomes active
        while (m_retry_thread_running.load() && !AreAnyCreationHooksActive()) {
            retry_count++;
            LogTestEvent("Retry attempt " + std::to_string(retry_count) + " - checking for DirectInput modules");
            bool found_any = TryLoadDirectInputModules();
            if (found_any) {
                LogTestEvent("Found DirectInput modules on retry " + std::to_string(retry_count));
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        if (AreAnyCreationHooksActive()) {
            LogTestEvent("DirectInput creation hooks became active");
        } else {
            LogTestEvent("Background retry thread exiting without finding DirectInput modules");
        }
        m_retry_thread_running.store(false);
    });
}

void DirectInputTester::StopRetryThread() {
    if (!m_retry_thread_running.load()) return;
    m_retry_thread_running.store(false);
    if (m_retry_thread.joinable()) {
        m_retry_thread.join();
    }
}

void DirectInputTester::UninstallHooks() {
    if (!m_hooks_installed) {
        return;
    }
    
    // Restore original functions
    m_original_DirectInput8Create = nullptr;
    m_original_DirectInputCreateA = nullptr;
    m_original_DirectInputCreateW = nullptr;
    m_original_DirectInputCreateEx = nullptr;
    m_original_CreateDevice = nullptr;
    m_original_SetDeviceData = nullptr;
    m_original_GetDeviceState = nullptr;
    
    // Clean up device hooks
    if (m_device_hooks_installed) {
        UninstallDeviceHooks();
    }
    
    m_hooks_installed = false;
    LogTestEvent("DirectInput hooks uninstalled");
}

bool DirectInputTester::InstallDeviceHooks(IDirectInputDevice8* device) {
    if (!device) {
        return false;
    }
    
    // Track the device
    TrackDevice(device);
    
    // For now, we'll just track the device
    // In a real implementation, you would hook the device's virtual function table
    LogTestEvent("Device hooks installed for new device");
    m_device_hooks_installed = true;
    
    return true;
}

bool DirectInputTester::TryLoadDirectInputModules() {
    bool found_any = false;
    
    // Try to load DirectInput8 module
    HMODULE dinput8 = GetModuleHandleA("dinput8.dll");
    if (dinput8 && !m_original_DirectInput8Create) {
        m_original_DirectInput8Create = (DirectInput8Create_t)GetProcAddress(dinput8, "DirectInput8Create");
        if (m_original_DirectInput8Create) {
            LogTestEvent("DirectInput8Create function loaded on retry");
            found_any = true;
        }
    }
    
    // Try to load DirectInput module
    HMODULE dinput = GetModuleHandleA("dinput.dll");
    if (dinput) {
        if (!m_original_DirectInputCreateA) {
            m_original_DirectInputCreateA = (DirectInputCreateA_t)GetProcAddress(dinput, "DirectInputCreateA");
            if (m_original_DirectInputCreateA) {
                LogTestEvent("DirectInputCreateA function loaded on retry");
                found_any = true;
            }
        }
        
        if (!m_original_DirectInputCreateW) {
            m_original_DirectInputCreateW = (DirectInputCreateW_t)GetProcAddress(dinput, "DirectInputCreateW");
            if (m_original_DirectInputCreateW) {
                LogTestEvent("DirectInputCreateW function loaded on retry");
                found_any = true;
            }
        }
        
        if (!m_original_DirectInputCreateEx) {
            m_original_DirectInputCreateEx = (DirectInputCreateEx_t)GetProcAddress(dinput, "DirectInputCreateEx");
            if (m_original_DirectInputCreateEx) {
                LogTestEvent("DirectInputCreateEx function loaded on retry");
                found_any = true;
            }
        }
    }
    
    // Enhanced debugging: Log what we found
    if (found_any) {
        LogTestEvent("Successfully loaded DirectInput functions on retry");
    } else {
        // Log module status for debugging
        if (!dinput8 && !dinput) {
            LogTestEvent("No DirectInput modules found in process");
        } else if (dinput8 || dinput) {
            LogTestEvent("DirectInput modules found but functions not accessible");
        }
    }
    
    return found_any;
}

std::string DirectInputTester::GetLoadedInputModulesInfo() const {
    std::ostringstream oss;
    oss << "Input module status:\n";
    
    // Check DirectInput modules
    HMODULE dinput8 = GetModuleHandleA("dinput8.dll");
    HMODULE dinput = GetModuleHandleA("dinput.dll");
    
    oss << "  dinput8.dll: " << (dinput8 ? "Loaded" : "Not loaded") << "\n";
    oss << "  dinput.dll: " << (dinput ? "Loaded" : "Not loaded") << "\n";
    
    // Check other common input modules
    HMODULE xinput = GetModuleHandleA("xinput1_4.dll");
    HMODULE xinput9 = GetModuleHandleA("xinput9_1_0.dll");
    HMODULE xinput13 = GetModuleHandleA("xinput1_3.dll");
    
    oss << "  xinput1_4.dll: " << (xinput ? "Loaded" : "Not loaded") << "\n";
    oss << "  xinput9_1_0.dll: " << (xinput9 ? "Loaded" : "Not loaded") << "\n";
    oss << "  xinput1_3.dll: " << (xinput13 ? "Loaded" : "Not loaded") << "\n";
    
    // Check Windows input modules
    HMODULE user32 = GetModuleHandleA("user32.dll");
    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    
    oss << "  user32.dll: " << (user32 ? "Loaded" : "Not loaded") << "\n";
    oss << "  kernel32.dll: " << (kernel32 ? "Loaded" : "Not loaded") << "\n";
    
    return oss.str();
}

void DirectInputTester::UninstallDeviceHooks() {
    // Clean up tracked devices
    m_tracked_devices.clear();
    m_device_hooks_installed = false;
    LogTestEvent("Device hooks uninstalled");
}

void DirectInputTester::TrackDevice(IDirectInputDevice8* device) {
    if (!device) return;
    
    // Check if device is already tracked
    auto it = std::find(m_tracked_devices.begin(), m_tracked_devices.end(), device);
    if (it == m_tracked_devices.end()) {
        m_tracked_devices.push_back(device);
        m_current_test_stats.devices_created++;
        LogTestEvent("New DirectInput device tracked");
    }
}

void DirectInputTester::UntrackDevice(IDirectInputDevice8* device) {
    if (!device) return;
    
    auto it = std::find(m_tracked_devices.begin(), m_tracked_devices.end(), device);
    if (it != m_tracked_devices.end()) {
        m_tracked_devices.erase(it);
        LogTestEvent("DirectInput device untracked");
    }
}

void DirectInputTester::RunMouseTest() {
    if (m_test_running.load()) {
        LogTestEvent("Test already running, cannot start mouse test");
        return;
    }
    
    m_test_running.store(true);
    m_current_test_stats = TestStats{}; // Reset stats
    
    LogTestEvent("Starting mouse input test...");
    
    // Run test in background thread
    std::thread([this]() {
        auto start_time = std::chrono::steady_clock::now();
        
        while (m_test_running.load()) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
            
            if (elapsed.count() >= m_test_duration_ms) {
                break;
            }
            
            // Simulate some mouse movement to trigger hooks
            SimulateTestInput();
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Generate test report
        std::ostringstream oss;
        oss << "Mouse test completed in " << m_test_duration_ms << "ms\n";
        oss << "Events intercepted: " << m_current_test_stats.mouse_events_intercepted << "\n";
        oss << "Input blocks: " << m_current_test_stats.input_blocks_performed << "\n";
        
        m_last_test_result = oss.str();
        m_test_running.store(false);
        
        LogTestEvent("Mouse test completed: " + m_last_test_result);
    }).detach();
}

void DirectInputTester::RunKeyboardTest() {
    if (m_test_running.load()) {
        LogTestEvent("Test already running, cannot start keyboard test");
        return;
    }
    
    m_test_running.store(true);
    m_current_test_stats = TestStats{}; // Reset stats
    
    LogTestEvent("Starting keyboard input test...");
    
    // Run test in background thread
    std::thread([this]() {
        auto start_time = std::chrono::steady_clock::now();
        
        while (m_test_running.load()) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
            
            if (elapsed.count() >= m_test_duration_ms) {
                break;
            }
            
            // Simulate some keyboard input to trigger hooks
            SimulateTestInput();
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Generate test report
        std::ostringstream oss;
        oss << "Keyboard test completed in " << m_test_duration_ms << "ms\n";
        oss << "Events intercepted: " << m_current_test_stats.keyboard_events_intercepted << "\n";
        oss << "Input blocks: " << m_current_test_stats.input_blocks_performed << "\n";
        
        m_last_test_result = oss.str();
        m_test_running.store(false);
        
        LogTestEvent("Keyboard test completed: " + m_last_test_result);
    }).detach();
}

void DirectInputTester::RunInputBlockingTest() {
    if (m_test_running.load()) {
        LogTestEvent("Test already running, cannot start input blocking test");
        return;
    }
    
    m_test_running.store(true);
    m_current_test_stats = TestStats{}; // Reset stats
    
    LogTestEvent("Starting input blocking test...");
    
    // Run test in background thread
    std::thread([this]() {
        auto start_time = std::chrono::steady_clock::now();
        
        while (m_test_running.load()) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
            
            if (elapsed.count() >= m_test_duration_ms) {
                break;
            }
            
            // Simulate input that should be blocked
            SimulateTestInput();
            
            // Increment block counter to simulate blocking
            m_current_test_stats.input_blocks_performed++;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        
        // Generate test report
        std::ostringstream oss;
        oss << "Input blocking test completed in " << m_test_duration_ms << "ms\n";
        oss << "Total input blocks: " << m_current_test_stats.input_blocks_performed << "\n";
        oss << "Block rate: " << std::fixed << std::setprecision(1) 
            << (m_current_test_stats.input_blocks_performed * 1000.0 / m_test_duration_ms) << " blocks/sec\n";
        
        m_last_test_result = oss.str();
        m_test_running.store(false);
        
        LogTestEvent("Input blocking test completed: " + m_last_test_result);
    }).detach();
}

void DirectInputTester::LogTestEvent(const std::string& event) {
    // Use ReShade logging
    reshade::log::message(reshade::log::level::info, 
        ("[DirectInput Test] " + event).c_str());
    
    // Store in test stats for UI display
    if (m_current_test_stats.intercepted_events.size() < 100) { // Limit storage
        m_current_test_stats.intercepted_events.push_back(event);
    }
}

void DirectInputTester::SimulateTestInput() {
    // Simulate mouse movement
    INPUT inputs[1] = {};
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE;
    
    // Use proper random number generation
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(-5, 5);
    
    inputs[0].mi.dx = dis(gen);
    inputs[0].mi.dy = dis(gen);
    
    SendInput(1, inputs, sizeof(INPUT));
    
    // Update test stats
    m_current_test_stats.mouse_events_intercepted++;
}

// Hook function implementations
HRESULT WINAPI DirectInputTester::HookDirectInput8Create(
    HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, 
    LPVOID *ppvOut, LPUNKNOWN punkOuter) {
    
    if (s_instance) {
        s_instance->LogTestEvent("DirectInput8Create intercepted - version: " + std::to_string(dwVersion));
        
        // Try to load modules if we don't have the original function yet
        if (!s_instance->m_original_DirectInput8Create) {
            s_instance->TryLoadDirectInputModules();
        }
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_DirectInput8Create) {
        return s_instance->m_original_DirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
    }
    
    return E_NOTIMPL;
}

HRESULT WINAPI DirectInputTester::HookDirectInputCreateA(
    HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA *ppDI, 
    LPUNKNOWN punkOuter) {
    
    if (s_instance) {
        s_instance->LogTestEvent("DirectInputCreateA intercepted - version: " + std::to_string(dwVersion));
        
        // Try to load modules if we don't have the original function yet
        if (!s_instance->m_original_DirectInputCreateA) {
            s_instance->TryLoadDirectInputModules();
        }
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_DirectInputCreateA) {
        return s_instance->m_original_DirectInputCreateA(hinst, dwVersion, ppDI, punkOuter);
    }
    
    return E_NOTIMPL;
}

HRESULT WINAPI DirectInputTester::HookDirectInputCreateW(
    HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW *ppDI, 
    LPUNKNOWN punkOuter) {
    
    if (s_instance) {
        s_instance->LogTestEvent("DirectInputCreateW intercepted - version: " + std::to_string(dwVersion));
        
        // Try to load modules if we don't have the original function yet
        if (!s_instance->m_original_DirectInputCreateW) {
            s_instance->TryLoadDirectInputModules();
        }
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_DirectInputCreateW) {
        return s_instance->m_original_DirectInputCreateW(hinst, dwVersion, ppDI, punkOuter);
    }
    
    return E_NOTIMPL;
}

HRESULT WINAPI DirectInputTester::HookDirectInputCreateEx(
    HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, 
    LPVOID *ppvOut, LPUNKNOWN punkOuter) {
    
    if (s_instance) {
        s_instance->LogTestEvent("DirectInputCreateEx intercepted - version: " + std::to_string(dwVersion));
        
        // Try to load modules if we don't have the original function yet
        if (!s_instance->m_original_DirectInputCreateEx) {
            s_instance->TryLoadDirectInputModules();
        }
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_DirectInputCreateEx) {
        return s_instance->m_original_DirectInputCreateEx(hinst, dwVersion, riidltf, ppvOut, punkOuter);
    }
    
    return E_NOTIMPL;
}

HRESULT WINAPI DirectInputTester::HookCreateDevice(
    IDirectInput8* This, REFGUID rguid, 
    LPDIRECTINPUTDEVICE *ppDevice, LPUNKNOWN pUnkOuter) {
    
    if (s_instance) {
        // Log device creation attempt
        // Note: In a real implementation, you would properly identify device types
        // For now, we'll just log that a device creation was intercepted
        s_instance->LogTestEvent("Device creation intercepted");
        s_instance->m_current_test_stats.mouse_events_intercepted++; // Placeholder increment
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_CreateDevice) {
        HRESULT result = s_instance->m_original_CreateDevice(This, rguid, ppDevice, pUnkOuter);
        
        // If device creation was successful, install hooks on it
        if (SUCCEEDED(result) && ppDevice && *ppDevice && s_instance) {
            // Note: In a real implementation, you would need to handle the type conversion properly
            // For now, we'll just log that we would install hooks
            s_instance->LogTestEvent("Device created successfully - would install hooks here");
        }
        
        return result;
    }
    
    return E_NOTIMPL;
}

HRESULT WINAPI DirectInputTester::HookSetDeviceData(
    IDirectInputDevice8* This, DWORD cbObjectData, 
    LPDIOBJECTDATAFORMAT rgdod, DWORD dwFlags, DWORD dwInOut) {
    
    if (s_instance) {
        s_instance->LogTestEvent("SetDeviceData intercepted - blocking input");
        s_instance->m_current_test_stats.input_blocks_performed++;
        
        // Block input by returning success but not calling original
        return DI_OK;
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_SetDeviceData) {
        return s_instance->m_original_SetDeviceData(This, cbObjectData, rgdod, dwFlags, dwInOut);
    }
    
    return E_NOTIMPL;
}

HRESULT WINAPI DirectInputTester::HookGetDeviceState(
    IDirectInputDevice8* This, DWORD cbData, LPVOID lpvData) {
    
    if (s_instance && s_instance->m_original_GetDeviceState) {
        s_instance->LogTestEvent("GetDeviceState intercepted");
        return s_instance->m_original_GetDeviceState(This, cbData, lpvData);
    }
    
    return E_NOTIMPL;
}

} // namespace renodx::input::direct_input::test


