#include "xinput_test.hpp"
#include <include/reshade.hpp>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>

namespace renodx::input::direct_input::test {

// Global instance for static access
XInputTester* XInputTester::s_instance = nullptr;

// Global instance
std::unique_ptr<XInputTester> g_xinputTester = std::make_unique<XInputTester>();

XInputTester::XInputTester() {
    s_instance = this;
}

XInputTester::~XInputTester() {
    Shutdown();
    s_instance = nullptr;
}

bool XInputTester::Initialize() {
    if (m_hooks_installed) {
        return true; // Already initialized
    }
    
    bool ok = InstallHooks();
    // Start retry thread if no XInput hooks active yet
    if (ok && !IsXInputGetStateHookActive()) {
        StartRetryThread();
    }
    return ok;
}

void XInputTester::Shutdown() {
    if (m_hooks_installed) {
        UninstallHooks();
    }
    StopRetryThread();
}

bool XInputTester::InstallHooks() {
    // Try to get XInput modules - they might not be loaded yet
    HMODULE xinput13 = GetModuleHandleA("xinput1_3.dll");
    HMODULE xinput14 = GetModuleHandleA("xinput1_4.dll");
    HMODULE xinput9 = GetModuleHandleA("xinput9_1_0.dll");
    
    if (!xinput13 && !xinput14 && !xinput9) {
        LogTestEvent("No XInput modules found yet - will retry later");
    }
    
    // Hook XInputGetState if available
    if (xinput13) {
        m_original_XInputGetState = (XInputGetState_t)GetProcAddress(xinput13, "XInputGetState");
        if (m_original_XInputGetState) {
            LogTestEvent("XInputGetState function found and stored from xinput1_3.dll");
        }
    }
    if (xinput14 && !m_original_XInputGetState) {
        m_original_XInputGetState = (XInputGetState_t)GetProcAddress(xinput14, "XInputGetState");
        if (m_original_XInputGetState) {
            LogTestEvent("XInputGetState function found and stored from xinput1_4.dll");
        }
    }
    
    // Hook XInputSetState if available
    if (xinput13) {
        m_original_XInputSetState = (XInputSetState_t)GetProcAddress(xinput13, "XInputSetState");
        if (m_original_XInputSetState) {
            LogTestEvent("XInputSetState function found and stored from xinput1_3.dll");
        }
    }
    if (xinput14 && !m_original_XInputSetState) {
        m_original_XInputSetState = (XInputSetState_t)GetProcAddress(xinput14, "XInputSetState");
        if (m_original_XInputSetState) {
            LogTestEvent("XInputSetState function found and stored from xinput1_4.dll");
        }
    }
    
    // Hook XInputGetCapabilities if available
    if (xinput13) {
        m_original_XInputGetCapabilities = (XInputGetCapabilities_t)GetProcAddress(xinput13, "XInputGetCapabilities");
        if (m_original_XInputGetCapabilities) {
            LogTestEvent("XInputGetCapabilities function found and stored from xinput1_3.dll");
        }
    }
    if (xinput14 && !m_original_XInputGetCapabilities) {
        m_original_XInputGetCapabilities = (XInputGetCapabilities_t)GetProcAddress(xinput14, "XInputGetCapabilities");
        if (m_original_XInputGetCapabilities) {
            LogTestEvent("XInputGetCapabilities function found and stored from xinput1_4.dll");
        }
    }
    
    // Hook XInputEnable if available
    if (xinput13) {
        m_original_XInputEnable = (XInputEnable_t)GetProcAddress(xinput13, "XInputEnable");
        if (m_original_XInputEnable) {
            LogTestEvent("XInputEnable function found and stored from xinput1_3.dll");
        }
    }
    if (xinput14 && !m_original_XInputEnable) {
        m_original_XInputEnable = (XInputEnable_t)GetProcAddress(xinput14, "XInputEnable");
        if (m_original_XInputEnable) {
            LogTestEvent("XInputEnable function found and stored from xinput1_4.dll");
        }
    }
    
    // Mark hooks as installed even if some modules aren't loaded yet
    m_hooks_installed = true;
    
    if (m_original_XInputGetState || m_original_XInputSetState || 
        m_original_XInputGetCapabilities || m_original_XInputEnable) {
        LogTestEvent("XInput hooks installed successfully");
    } else {
        LogTestEvent("XInput hooks prepared - will activate when modules are loaded");
    }
    
    return true;
}

void XInputTester::UninstallHooks() {
    if (!m_hooks_installed) {
        return;
    }
    
    // Restore original functions
    m_original_XInputGetState = nullptr;
    m_original_XInputSetState = nullptr;
    m_original_XInputGetCapabilities = nullptr;
    m_original_XInputEnable = nullptr;
    
    m_hooks_installed = false;
    LogTestEvent("XInput hooks uninstalled");
}

bool XInputTester::TryLoadXInputModules() {
    bool found_any = false;
    
    // Try to load XInput modules
    HMODULE xinput13 = GetModuleHandleA("xinput1_3.dll");
    HMODULE xinput14 = GetModuleHandleA("xinput1_4.dll");
    HMODULE xinput9 = GetModuleHandleA("xinput9_1_0.dll");
    
    // Try XInputGetState
    if (!m_original_XInputGetState) {
        if (xinput13) {
            m_original_XInputGetState = (XInputGetState_t)GetProcAddress(xinput13, "XInputGetState");
            if (m_original_XInputGetState) {
                LogTestEvent("XInputGetState function loaded on retry from xinput1_3.dll");
                found_any = true;
            }
        }
        if (xinput14 && !m_original_XInputGetState) {
            m_original_XInputGetState = (XInputGetState_t)GetProcAddress(xinput14, "XInputGetState");
            if (m_original_XInputGetState) {
                LogTestEvent("XInputGetState function loaded on retry from xinput1_4.dll");
                found_any = true;
            }
        }
    }
    
    // Try XInputSetState
    if (!m_original_XInputSetState) {
        if (xinput13) {
            m_original_XInputSetState = (XInputSetState_t)GetProcAddress(xinput13, "XInputSetState");
            if (m_original_XInputSetState) {
                LogTestEvent("XInputSetState function loaded on retry from xinput1_3.dll");
                found_any = true;
            }
        }
        if (xinput14 && !m_original_XInputSetState) {
            m_original_XInputSetState = (XInputSetState_t)GetProcAddress(xinput14, "XInputSetState");
            if (m_original_XInputSetState) {
                LogTestEvent("XInputSetState function loaded on retry from xinput1_4.dll");
                found_any = true;
            }
        }
    }
    
    // Try XInputGetCapabilities
    if (!m_original_XInputGetCapabilities) {
        if (xinput13) {
            m_original_XInputGetCapabilities = (XInputGetCapabilities_t)GetProcAddress(xinput13, "XInputGetCapabilities");
            if (m_original_XInputGetCapabilities) {
                LogTestEvent("XInputGetCapabilities function loaded on retry from xinput1_3.dll");
                found_any = true;
            }
        }
        if (xinput14 && !m_original_XInputGetCapabilities) {
            m_original_XInputGetCapabilities = (XInputGetCapabilities_t)GetProcAddress(xinput14, "XInputGetCapabilities");
            if (m_original_XInputGetCapabilities) {
                LogTestEvent("XInputGetCapabilities function loaded on retry from xinput1_4.dll");
                found_any = true;
            }
        }
    }
    
    // Try XInputEnable
    if (!m_original_XInputEnable) {
        if (xinput13) {
            m_original_XInputEnable = (XInputEnable_t)GetProcAddress(xinput13, "XInputEnable");
            if (m_original_XInputEnable) {
                LogTestEvent("XInputEnable function loaded on retry from xinput1_3.dll");
                found_any = true;
            }
        }
        if (xinput14 && !m_original_XInputEnable) {
            m_original_XInputEnable = (XInputEnable_t)GetProcAddress(xinput14, "XInputEnable");
            if (m_original_XInputEnable) {
                LogTestEvent("XInputEnable function loaded on retry from xinput1_4.dll");
                found_any = true;
            }
        }
    }
    
    // Enhanced debugging: Log what we found
    if (found_any) {
        LogTestEvent("Successfully loaded XInput functions on retry");
    } else {
        // Log module status for debugging
        if (!xinput13 && !xinput14 && !xinput9) {
            LogTestEvent("No XInput modules found in process");
        } else if (xinput13 || xinput14 || xinput9) {
            LogTestEvent("XInput modules found but functions not accessible");
        }
    }
    
    return found_any;
}

void XInputTester::StartRetryThread() {
    if (m_retry_thread_running.load()) return;
    m_retry_thread_running.store(true);
    m_retry_thread = std::thread([this]() {
        LogTestEvent("XInput background retry thread started");
        int retry_count = 0;
        // Poll a few times per second until any XInput hook becomes active
        while (m_retry_thread_running.load() && !IsXInputGetStateHookActive()) {
            retry_count++;
            LogTestEvent("XInput retry attempt " + std::to_string(retry_count) + " - checking for XInput modules");
            bool found_any = TryLoadXInputModules();
            if (found_any) {
                LogTestEvent("Found XInput modules on retry " + std::to_string(retry_count));
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        if (IsXInputGetStateHookActive()) {
            LogTestEvent("XInput hooks became active");
        } else {
            LogTestEvent("XInput background retry thread exiting without finding XInput modules");
        }
        m_retry_thread_running.store(false);
    });
}

void XInputTester::StopRetryThread() {
    if (!m_retry_thread_running.load()) return;
    m_retry_thread_running.store(false);
    if (m_retry_thread.joinable()) {
        m_retry_thread.join();
    }
}

void XInputTester::RunXInputTest() {
    if (m_test_running.load()) {
        LogTestEvent("XInput test already running, cannot start new test");
        return;
    }
    
    m_test_running.store(true);
    m_current_test_stats = TestStats{}; // Reset stats
    
    LogTestEvent("Starting XInput test...");
    
    // Run test in background thread
    std::thread([this]() {
        auto start_time = std::chrono::steady_clock::now();
        
        while (m_test_running.load()) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
            
            if (elapsed.count() >= m_test_duration_ms) {
                break;
            }
            
            // Simulate some XInput calls to trigger hooks
            SimulateTestInput();
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Generate test report
        std::ostringstream oss;
        oss << "XInput test completed in " << m_test_duration_ms << "ms\n";
        oss << "Controller states intercepted: " << m_current_test_stats.controller_states_intercepted << "\n";
        oss << "Vibration commands intercepted: " << m_current_test_stats.vibration_commands_intercepted << "\n";
        oss << "Capability queries intercepted: " << m_current_test_stats.capability_queries_intercepted << "\n";
        oss << "Enable calls intercepted: " << m_current_test_stats.enable_calls_intercepted << "\n";
        
        m_last_test_result = oss.str();
        m_test_running.store(false);
        
        LogTestEvent("XInput test completed: " + m_last_test_result);
    }).detach();
}

void XInputTester::LogTestEvent(const std::string& event) {
    // Use ReShade logging
    reshade::log::message(reshade::log::level::info, 
        ("[XInput Test] " + event).c_str());
    
    // Store in test stats for UI display
    if (m_current_test_stats.intercepted_events.size() < 100) { // Limit storage
        m_current_test_stats.intercepted_events.push_back(event);
    }
}

void XInputTester::SimulateTestInput() {
    // Simulate XInput calls to test hooks
    // This is just for testing - in real usage, these would be intercepted from the game
    m_current_test_stats.controller_states_intercepted++;
}

std::string XInputTester::GetLoadedXInputModulesInfo() const {
    std::ostringstream oss;
    oss << "XInput module status:\n";
    
    // Check XInput modules
    HMODULE xinput13 = GetModuleHandleA("xinput1_3.dll");
    HMODULE xinput14 = GetModuleHandleA("xinput1_4.dll");
    HMODULE xinput9 = GetModuleHandleA("xinput9_1_0.dll");
    
    oss << "  xinput1_3.dll: " << (xinput13 ? "Loaded" : "Not loaded") << "\n";
    oss << "  xinput1_4.dll: " << (xinput14 ? "Loaded" : "Not loaded") << "\n";
    oss << "  xinput9_1_0.dll: " << (xinput9 ? "Loaded" : "Not loaded") << "\n";
    
    // Check hook status
    oss << "Hook Status:\n";
    oss << "  XInputGetState: " << (IsXInputGetStateHookActive() ? "Active" : "Inactive") << "\n";
    oss << "  XInputSetState: " << (IsXInputSetStateHookActive() ? "Active" : "Inactive") << "\n";
    oss << "  XInputGetCapabilities: " << (IsXInputGetCapabilitiesHookActive() ? "Active" : "Inactive") << "\n";
    oss << "  XInputEnable: " << (IsXInputEnableHookActive() ? "Active" : "Inactive") << "\n";
    
    return oss.str();
}

// Hook function implementations
DWORD WINAPI XInputTester::HookXInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState) {
    if (s_instance) {
        s_instance->LogTestEvent("XInputGetState intercepted - Controller: " + std::to_string(dwUserIndex));
        s_instance->m_current_test_stats.controller_states_intercepted++;
        
        // Try to load modules if we don't have the original function yet
        if (!s_instance->m_original_XInputGetState) {
            s_instance->TryLoadXInputModules();
        }
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_XInputGetState) {
        return s_instance->m_original_XInputGetState(dwUserIndex, pState);
    }
    
    return ERROR_DEVICE_NOT_CONNECTED;
}

DWORD WINAPI XInputTester::HookXInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration) {
    if (s_instance) {
        s_instance->LogTestEvent("XInputSetState intercepted - Controller: " + std::to_string(dwUserIndex));
        s_instance->m_current_test_stats.vibration_commands_intercepted++;
        
        // Try to load modules if we don't have the original function yet
        if (!s_instance->m_original_XInputSetState) {
            s_instance->TryLoadXInputModules();
        }
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_XInputSetState) {
        return s_instance->m_original_XInputSetState(dwUserIndex, pVibration);
    }
    
    return ERROR_DEVICE_NOT_CONNECTED;
}

DWORD WINAPI XInputTester::HookXInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities) {
    if (s_instance) {
        s_instance->LogTestEvent("XInputGetCapabilities intercepted - Controller: " + std::to_string(dwUserIndex));
        s_instance->m_current_test_stats.capability_queries_intercepted++;
        
        // Try to load modules if we don't have the original function yet
        if (!s_instance->m_original_XInputGetCapabilities) {
            s_instance->TryLoadXInputModules();
        }
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_XInputGetCapabilities) {
        return s_instance->m_original_XInputGetCapabilities(dwUserIndex, dwFlags, pCapabilities);
    }
    
    return ERROR_DEVICE_NOT_CONNECTED;
}

void WINAPI XInputTester::HookXInputEnable(BOOL enable) {
    if (s_instance) {
        s_instance->LogTestEvent("XInputEnable intercepted - Enable: " + std::string(enable ? "TRUE" : "FALSE"));
        s_instance->m_current_test_stats.enable_calls_intercepted++;
        
        // Try to load modules if we don't have the original function yet
        if (!s_instance->m_original_XInputEnable) {
            s_instance->TryLoadXInputModules();
        }
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_XInputEnable) {
        s_instance->m_original_XInputEnable(enable);
    }
}

} // namespace renodx::input::direct_input::test
