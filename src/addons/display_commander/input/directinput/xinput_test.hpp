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
    
    // Keyboard/Mouse hook status queries
    bool IsGetAsyncKeyStateHookActive() const { return m_hooks_installed && m_original_GetAsyncKeyState != nullptr; }
    bool IsGetKeyStateHookActive() const { return m_hooks_installed && m_original_GetKeyState != nullptr; }
    bool IsGetKeyboardStateHookActive() const { return m_hooks_installed && m_original_GetKeyboardState != nullptr; }
    bool IsGetCursorPosHookActive() const { return m_hooks_installed && m_original_GetCursorPos != nullptr; }
    bool IsSetCursorPosHookActive() const { return m_hooks_installed && m_original_SetCursorPos != nullptr; }
    
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
    
    // Hook functions for Keyboard/Mouse APIs (for testing)
    static SHORT WINAPI HookGetAsyncKeyState(int vKey);
    static SHORT WINAPI HookGetKeyState(int nVirtKey);
    static BOOL WINAPI HookGetKeyboardState(PBYTE lpKeyState);
    static BOOL WINAPI HookGetCursorPos(LPPOINT lpPoint);
    static BOOL WINAPI HookSetCursorPos(int x, int y);
    static BOOL WINAPI HookGetMousePos(int* x, int* y);
    
    // Install hooks
    bool InstallHooks();
    void UninstallHooks();
    
    // Runtime hooking for dynamically loaded functions
    bool InstallRuntimeHooks();
    void UninstallRuntimeHooks();
    
    // Function replacement in loaded DLLs
    bool ReplaceXInputFunctionsInDLL();
    bool ReplaceFunctionInDLL(FARPROC originalFunction, FARPROC newFunction);
    
    // Proper detour-style hooking
    bool InstallDetourHooks();
    bool InstallDetourHook(FARPROC originalFunction, FARPROC newFunction);
    
    // Safer API hooking approach
    bool InstallAPIHooks();
    
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

    // Original function pointers for Keyboard/Mouse APIs
    using GetAsyncKeyState_t = SHORT(WINAPI*)(int);
    using GetKeyState_t = SHORT(WINAPI*)(int);
    using GetKeyboardState_t = BOOL(WINAPI*)(PBYTE);
    using GetCursorPos_t = BOOL(WINAPI*)(LPPOINT);
    using SetCursorPos_t = BOOL(WINAPI*)(int, int);

    GetAsyncKeyState_t m_original_GetAsyncKeyState = nullptr;
    GetKeyState_t m_original_GetKeyState = nullptr;
    GetKeyboardState_t m_original_GetKeyboardState = nullptr;
    GetCursorPos_t m_original_GetCursorPos = nullptr;
    SetCursorPos_t m_original_SetCursorPos = nullptr;

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
        // XInput stats
        int controller_states_intercepted = 0;
        int vibration_commands_intercepted = 0;
        int capability_queries_intercepted = 0;
        int enable_calls_intercepted = 0;
        
        // Keyboard/Mouse stats
        int get_async_key_state_calls = 0;
        int get_key_state_calls = 0;
        int get_keyboard_state_calls = 0;
        int get_cursor_pos_calls = 0;
        int set_cursor_pos_calls = 0;
        
        // General
        std::vector<std::string> intercepted_events;
    };
    
    TestStats m_current_test_stats;
    
    // Global instance for static access
    static XInputTester* s_instance;
};

// Global instance
extern std::unique_ptr<XInputTester> g_xinputTester;

} // namespace renodx::input::direct_input::test
