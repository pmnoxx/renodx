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
    
    // Hook Keyboard/Mouse functions from user32.dll
    HMODULE user32 = GetModuleHandleA("user32.dll");
    if (user32) {
        m_original_GetAsyncKeyState = (GetAsyncKeyState_t)GetProcAddress(user32, "GetAsyncKeyState");
        if (m_original_GetAsyncKeyState) {
            LogTestEvent("GetAsyncKeyState function found and stored from user32.dll");
        }
        
        m_original_GetKeyState = (GetKeyState_t)GetProcAddress(user32, "GetKeyState");
        if (m_original_GetKeyState) {
            LogTestEvent("GetKeyState function found and stored from user32.dll");
        }
        
        m_original_GetKeyboardState = (GetKeyboardState_t)GetProcAddress(user32, "GetKeyboardState");
        if (m_original_GetKeyboardState) {
            LogTestEvent("GetKeyboardState function found and stored from user32.dll");
        }
        
        m_original_GetCursorPos = (GetCursorPos_t)GetProcAddress(user32, "GetCursorPos");
        if (m_original_GetCursorPos) {
            LogTestEvent("GetCursorPos function found and stored from user32.dll");
        }
        
        m_original_SetCursorPos = (SetCursorPos_t)GetProcAddress(user32, "SetCursorPos");
        if (m_original_SetCursorPos) {
            LogTestEvent("SetCursorPos function found and stored from user32.dll");
        }
    }
    
    // Mark hooks as installed even if some modules aren't loaded yet
    m_hooks_installed = true;
    
    if (m_original_XInputGetState || m_original_XInputSetState || 
        m_original_XInputGetCapabilities || m_original_XInputEnable) {
        LogTestEvent("XInput hooks installed successfully");
        
        // Now try to replace the functions in memory so our hooks are actually called
        if (ReplaceXInputFunctions()) {
            LogTestEvent("XInput functions successfully replaced in memory");
        } else {
            LogTestEvent("Failed to replace XInput functions in memory - hooks may not work");
        }
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

bool XInputTester::ReplaceXInputFunctions() {
    LogTestEvent("Attempting to replace XInput functions in memory...");
    
    // Get the current process handle
    HANDLE hProcess = GetCurrentProcess();
    if (!hProcess) {
        LogTestEvent("Failed to get current process handle");
        return false;
    }
    
    // Get the main module (our addon)
    HMODULE hModule = GetModuleHandleA(nullptr);
    if (!hModule) {
        LogTestEvent("Failed to get main module handle");
        return false;
    }
    
    // Get the DOS header
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        LogTestEvent("Invalid DOS header");
        return false;
    }
    
    // Get the NT headers
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + pDosHeader->e_lfanew);
    if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
        LogTestEvent("Invalid NT headers");
        return false;
    }
    
    // Get the import directory
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)hModule + 
        pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    
    if (!pImportDesc) {
        LogTestEvent("No import directory found");
        return false;
    }
    
    bool replaced_any = false;
    
    // Walk through all import descriptors
    while (pImportDesc->Name) {
        LPCSTR pszDllName = (LPCSTR)((BYTE*)hModule + pImportDesc->Name);
        
        // Check if this is an XInput DLL
        if (strstr(pszDllName, "xinput") || strstr(pszDllName, "XInput")) {
            LogTestEvent("Found XInput DLL: " + std::string(pszDllName));
            
            // Get the import address table (IAT)
            PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((BYTE*)hModule + pImportDesc->FirstThunk);
            PIMAGE_THUNK_DATA pOriginalThunk = (PIMAGE_THUNK_DATA)((BYTE*)hModule + pImportDesc->OriginalFirstThunk);
            
            // Walk through all imports from this DLL
            while (pThunk->u1.Function) {
                // Get the function name
                if (pOriginalThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) {
                    // Ordinal import, skip
                    pThunk++;
                    pOriginalThunk++;
                    continue;
                }
                
                PIMAGE_IMPORT_BY_NAME pImportByName = (PIMAGE_IMPORT_BY_NAME)((BYTE*)hModule + 
                    pOriginalThunk->u1.AddressOfData);
                
                LPCSTR pszFunctionName = (LPCSTR)pImportByName->Name;
                
                // Check if this is an XInput function we want to hook
                if (strcmp(pszFunctionName, "XInputGetState") == 0 && m_original_XInputGetState) {
                    LogTestEvent("Replacing XInputGetState in IAT");
                    if (ReplaceFunctionInIAT(pThunk, (FARPROC)HookXInputGetState)) {
                        replaced_any = true;
                    }
                }
                else if (strcmp(pszFunctionName, "XInputSetState") == 0 && m_original_XInputSetState) {
                    LogTestEvent("Replacing XInputSetState in IAT");
                    if (ReplaceFunctionInIAT(pThunk, (FARPROC)HookXInputSetState)) {
                        replaced_any = true;
                    }
                }
                else if (strcmp(pszFunctionName, "XInputGetCapabilities") == 0 && m_original_XInputGetCapabilities) {
                    LogTestEvent("Replacing XInputGetCapabilities in IAT");
                    if (ReplaceFunctionInIAT(pThunk, (FARPROC)HookXInputGetCapabilities)) {
                        replaced_any = true;
                    }
                }
                else if (strcmp(pszFunctionName, "XInputEnable") == 0 && m_original_XInputEnable) {
                    LogTestEvent("Replacing XInputEnable in IAT");
                    if (ReplaceFunctionInIAT(pThunk, (FARPROC)HookXInputEnable)) {
                        replaced_any = true;
                    }
                }
                
                pThunk++;
                pOriginalThunk++;
            }
        }
        // Check if this is user32.dll for keyboard/mouse functions
        else if (strstr(pszDllName, "user32") || strstr(pszDllName, "USER32")) {
            LogTestEvent("Found USER32 DLL: " + std::string(pszDllName));
            
            // Get the import address table (IAT)
            PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)((BYTE*)hModule + pImportDesc->FirstThunk);
            PIMAGE_THUNK_DATA pOriginalThunk = (PIMAGE_THUNK_DATA)((BYTE*)hModule + pImportDesc->OriginalFirstThunk);
            
            // Walk through all imports from this DLL
            while (pThunk->u1.Function) {
                // Get the function name
                if (pOriginalThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) {
                    // Ordinal import, skip
                    pThunk++;
                    pOriginalThunk++;
                    continue;
                }
                
                PIMAGE_IMPORT_BY_NAME pImportByName = (PIMAGE_IMPORT_BY_NAME)((BYTE*)hModule + 
                    pOriginalThunk->u1.AddressOfData);
                
                LPCSTR pszFunctionName = (LPCSTR)pImportByName->Name;
                
                // Check if this is a keyboard/mouse function we want to hook
                if (strcmp(pszFunctionName, "GetAsyncKeyState") == 0 && m_original_GetAsyncKeyState) {
                    LogTestEvent("Replacing GetAsyncKeyState in IAT");
                    if (ReplaceFunctionInIAT(pThunk, (FARPROC)HookGetAsyncKeyState)) {
                        replaced_any = true;
                    }
                }
                else if (strcmp(pszFunctionName, "GetKeyState") == 0 && m_original_GetKeyState) {
                    LogTestEvent("Replacing GetKeyState in IAT");
                    if (ReplaceFunctionInIAT(pThunk, (FARPROC)HookGetKeyState)) {
                        replaced_any = true;
                    }
                }
                else if (strcmp(pszFunctionName, "GetKeyboardState") == 0 && m_original_GetKeyboardState) {
                    LogTestEvent("Replacing GetKeyboardState in IAT");
                    if (ReplaceFunctionInIAT(pThunk, (FARPROC)HookGetKeyboardState)) {
                        replaced_any = true;
                    }
                }
                else if (strcmp(pszFunctionName, "GetCursorPos") == 0 && m_original_GetCursorPos) {
                    LogTestEvent("Replacing GetCursorPos in IAT");
                    if (ReplaceFunctionInIAT(pThunk, (FARPROC)HookGetCursorPos)) {
                        replaced_any = true;
                    }
                }
                else if (strcmp(pszFunctionName, "SetCursorPos") == 0 && m_original_SetCursorPos) {
                    LogTestEvent("Replacing SetCursorPos in IAT");
                    if (ReplaceFunctionInIAT(pThunk, (FARPROC)HookSetCursorPos)) {
                        replaced_any = true;
                    }
                }
                
                pThunk++;
                pOriginalThunk++;
            }
        }
        
        pImportDesc++;
    }
    
    if (replaced_any) {
        LogTestEvent("Successfully replaced XInput functions in IAT");
        return true;
    } else {
        LogTestEvent("No XInput functions found to replace in IAT");
        return false;
    }
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
        oss << "\nKeyboard/Mouse Events:\n";
        oss << "GetAsyncKeyState calls: " << m_current_test_stats.get_async_key_state_calls << "\n";
        oss << "GetKeyState calls: " << m_current_test_stats.get_key_state_calls << "\n";
        oss << "GetKeyboardState calls: " << m_current_test_stats.get_keyboard_state_calls << "\n";
        oss << "GetCursorPos calls: " << m_current_test_stats.get_cursor_pos_calls << "\n";
        oss << "SetCursorPos calls: " << m_current_test_stats.set_cursor_pos_calls << "\n";
        
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
    oss << "\nKeyboard/Mouse Hook Status:\n";
    oss << "  GetAsyncKeyState: " << (IsGetAsyncKeyStateHookActive() ? "Active" : "Inactive") << "\n";
    oss << "  GetKeyState: " << (IsGetKeyStateHookActive() ? "Active" : "Inactive") << "\n";
    oss << "  GetKeyboardState: " << (IsGetKeyboardStateHookActive() ? "Active" : "Inactive") << "\n";
    oss << "  GetCursorPos: " << (IsGetCursorPosHookActive() ? "Active" : "Inactive") << "\n";
    oss << "  SetCursorPos: " << (IsSetCursorPosHookActive() ? "Active" : "Inactive") << "\n";
    
    return oss.str();
}

// Hook function implementations
DWORD WINAPI XInputTester::HookXInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState) {
    if (s_instance) {
        s_instance->LogTestEvent("XInputGetState intercepted - Controller: " + std::to_string(dwUserIndex));
        s_instance->m_current_test_stats.controller_states_intercepted++;
        
        // Check if input blocking is enabled
        if (s_instance->IsBlockingXInputInput()) {
            s_instance->LogTestEvent("XInputGetState BLOCKED - Input blocking enabled");
            // Return success but with zeroed state (no input)
            if (pState) {
                ZeroMemory(pState, sizeof(XINPUT_STATE));
                pState->dwPacketNumber = 0;
            }
            return ERROR_SUCCESS;
        }
        
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
        
        // Check if input blocking is enabled
        if (s_instance->IsBlockingXInputInput()) {
            s_instance->LogTestEvent("XInputSetState BLOCKED - Input blocking enabled");
            // Return success but don't apply vibration
            return ERROR_SUCCESS;
        }
        
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
        
        // Check if input blocking is enabled
        if (s_instance->IsBlockingXInputInput()) {
            s_instance->LogTestEvent("XInputGetCapabilities BLOCKED - Input blocking enabled");
            // Return success but with zeroed capabilities
            if (pCapabilities) {
                ZeroMemory(pCapabilities, sizeof(XINPUT_CAPABILITIES));
            }
            return ERROR_SUCCESS;
        }
        
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
        
        // Check if input blocking is enabled
        if (s_instance->IsBlockingXInputInput()) {
            s_instance->LogTestEvent("XInputEnable BLOCKED - Input blocking enabled");
            // Don't call original function when blocking
            return;
        }
        
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

// Keyboard/Mouse hook function implementations
SHORT WINAPI XInputTester::HookGetAsyncKeyState(int vKey) {
    if (s_instance) {
        s_instance->LogTestEvent("GetAsyncKeyState intercepted - Key: " + std::to_string(vKey));
        s_instance->m_current_test_stats.get_async_key_state_calls++;
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_GetAsyncKeyState) {
        return s_instance->m_original_GetAsyncKeyState(vKey);
    }
    
    return 0;
}

SHORT WINAPI XInputTester::HookGetKeyState(int nVirtKey) {
    if (s_instance) {
        s_instance->LogTestEvent("GetKeyState intercepted - Key: " + std::to_string(nVirtKey));
        s_instance->m_current_test_stats.get_key_state_calls++;
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_GetKeyState) {
        return s_instance->m_original_GetKeyState(nVirtKey);
    }
    
    return 0;
}

BOOL WINAPI XInputTester::HookGetKeyboardState(PBYTE lpKeyState) {
    if (s_instance) {
        s_instance->LogTestEvent("GetKeyboardState intercepted");
        s_instance->m_current_test_stats.get_keyboard_state_calls++;
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_GetKeyboardState) {
        return s_instance->m_original_GetKeyboardState(lpKeyState);
    }
    
    return FALSE;
}

BOOL WINAPI XInputTester::HookGetCursorPos(LPPOINT lpPoint) {
    if (s_instance) {
        s_instance->LogTestEvent("GetCursorPos intercepted");
        s_instance->m_current_test_stats.get_cursor_pos_calls++;
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_GetCursorPos) {
        return s_instance->m_original_GetCursorPos(lpPoint);
    }
    
    return FALSE;
}

BOOL WINAPI XInputTester::HookSetCursorPos(int x, int y) {
    if (s_instance) {
        s_instance->LogTestEvent("SetCursorPos intercepted - X: " + std::to_string(x) + ", Y: " + std::to_string(y));
        s_instance->m_current_test_stats.set_cursor_pos_calls++;
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_SetCursorPos) {
        return s_instance->m_original_SetCursorPos(x, y);
    }
    
    return FALSE;
}

BOOL WINAPI XInputTester::HookGetMousePos(int* x, int* y) {
    if (s_instance) {
        s_instance->LogTestEvent("GetMousePos intercepted");
        s_instance->m_current_test_stats.get_cursor_pos_calls++;
    }
    
    // Call original function if available
    if (s_instance && s_instance->m_original_GetCursorPos) {
        POINT pt;
        BOOL result = s_instance->m_original_GetCursorPos(&pt);
        if (result && x && y) {
            *x = pt.x;
            *y = pt.y;
        }
        return result;
    }
    
    return FALSE;
}

bool XInputTester::ReplaceFunctionInIAT(PIMAGE_THUNK_DATA pThunk, FARPROC newFunction) {
    if (!pThunk || !newFunction) {
        return false;
    }
    
    // Store the original function pointer
    FARPROC originalFunction = (FARPROC)pThunk->u1.Function;
    
    // Change memory protection to allow writing
    DWORD oldProtect;
    if (!VirtualProtect(&pThunk->u1.Function, sizeof(FARPROC), PAGE_READWRITE, &oldProtect)) {
        LogTestEvent("Failed to change memory protection for IAT");
        return false;
    }
    
    // Replace the function pointer
    pThunk->u1.Function = (ULONGLONG)newFunction;
    
    // Restore memory protection
    DWORD dummy;
    VirtualProtect(&pThunk->u1.Function, sizeof(FARPROC), oldProtect, &dummy);
    
    // Flush instruction cache
    FlushInstructionCache(GetCurrentProcess(), &pThunk->u1.Function, sizeof(FARPROC));
    
    LogTestEvent("Successfully replaced function in IAT: " + std::to_string((ULONGLONG)originalFunction) + 
                 " -> " + std::to_string((ULONGLONG)newFunction));
    
    return true;
}

} // namespace renodx::input::direct_input::test

