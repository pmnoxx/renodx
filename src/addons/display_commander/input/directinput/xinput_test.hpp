#pragma once

#include <Windows.h>
#include <Xinput.h>
#include <atomic>
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <include/reshade.hpp>

namespace renodx::input::direct_input::test {

class XInputTester {
public:
    XInputTester();
    ~XInputTester();

    // Initialize the tester and install hooks
    bool Initialize();
    
    // Clean up hooks and resources
    void Shutdown();

    // Test functionality
    void RunXInputTest();
    
    // Status and results
    bool IsTestRunning() const { return m_test_running.load(); }
    const std::string& GetLastTestResult() const { return m_last_test_result; }
    
    // Test configuration
    void SetTestDuration(int milliseconds) { m_test_duration_ms = milliseconds; }
    int GetTestDuration() const { return m_test_duration_ms; }
    
    // Hook status queries
    bool IsXInputGetStateHookActive() const { return m_hooks_installed && m_original_XInputGetState != nullptr; }
    bool IsXInputSetStateHookActive() const { return m_hooks_installed && m_original_XInputSetState != nullptr; }
    bool IsXInputGetCapabilitiesHookActive() const { return m_hooks_installed && m_original_XInputGetCapabilities != nullptr; }
    bool IsXInputEnableHookActive() const { return m_hooks_installed && m_original_XInputEnable != nullptr; }
    
    // Input blocking control
    void SetBlockXInputInput(bool block) { m_block_xinput_input.store(block); }
    bool IsBlockingXInputInput() const { return m_block_xinput_input.load(); }
    
    // Manual retry trigger
    bool RetryLoadModules() { return TryLoadXInputModules(); }
    
    // Thread status
    bool IsRetryThreadRunning() const { return m_retry_thread_running.load(); }
    
    // Debug information
    std::string GetLoadedXInputModulesInfo() const;

private:
    // Hook functions for XInput APIs
    static DWORD WINAPI HookXInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState);
    static DWORD WINAPI HookXInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
    static DWORD WINAPI HookXInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities);
    static void WINAPI HookXInputEnable(BOOL enable);
    
    // Install hooks
    bool InstallHooks();
    void UninstallHooks();
    bool ReplaceXInputFunctions();
    bool ReplaceFunctionInIAT(PIMAGE_THUNK_DATA pThunk, FARPROC newFunction);
    
    // Module loading retry
    bool TryLoadXInputModules();
    
    // Background retry thread management
    void StartRetryThread();
    void StopRetryThread();
    
    // Test helpers
    void LogTestEvent(const std::string& event);
    void SimulateTestInput();
    
    // Original function pointers
    using XInputGetState_t = DWORD(WINAPI*)(DWORD, XINPUT_STATE*);
    using XInputSetState_t = DWORD(WINAPI*)(DWORD, XINPUT_VIBRATION*);
    using XInputGetCapabilities_t = DWORD(WINAPI*)(DWORD, DWORD, XINPUT_CAPABILITIES*);
    using XInputEnable_t = void(WINAPI*)(BOOL);

    XInputGetState_t m_original_XInputGetState = nullptr;
    XInputSetState_t m_original_XInputSetState = nullptr;
    XInputGetCapabilities_t m_original_XInputGetCapabilities = nullptr;
    XInputEnable_t m_original_XInputEnable = nullptr;

    // Test state
    std::atomic<bool> m_test_running{false};
    std::string m_last_test_result;
    int m_test_duration_ms = 5000; // 5 seconds default
    
    // Hook state
    bool m_hooks_installed = false;
    std::atomic<bool> m_retry_thread_running{false};
    std::atomic<bool> m_block_xinput_input{false};
    std::thread m_retry_thread;
    
    // Test statistics
    struct TestStats {
        int controller_states_intercepted = 0;
        int vibration_commands_intercepted = 0;
        int capability_queries_intercepted = 0;
        int enable_calls_intercepted = 0;
        std::vector<std::string> intercepted_events;
    };
    
    TestStats m_current_test_stats;
    
    // Global instance for static access
    static XInputTester* s_instance;
};

// Global instance
extern std::unique_ptr<XInputTester> g_xinputTester;

} // namespace renodx::input::direct_input::test
