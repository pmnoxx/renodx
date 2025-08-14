#include "nvapi_fullscreen_prevention.hpp"
#include "utils.hpp"
#include "../../external/SpecialK/depends/include/nvapi/NvApiDriverSettings.h"
#include <sstream>

NVAPIFullscreenPrevention::NVAPIFullscreenPrevention() {
}

NVAPIFullscreenPrevention::~NVAPIFullscreenPrevention() {
    Cleanup();
}

bool NVAPIFullscreenPrevention::Initialize() {
    if (initialized) {
        return true;
    }
    
    // Initialize NVAPI using static linking (like SpecialK)
    NvAPI_Status status = NvAPI_Initialize();
    if (status != NVAPI_OK) {
        std::ostringstream oss;
        oss << "Failed to initialize NVAPI. Status: " << status;
        
        // Provide basic status information
        if (status == 0x10001) {
            oss << " (API not initialized)";
        } else if (status == 0x10002) {
            oss << " (Library not found)";
        } else if (status == 0x10003) {
            oss << " (No NVIDIA device found)";
        } else if (status == 0x10004) {
            oss << " (General error)";
        }
        
        last_error = oss.str();
        return false;
    }
    
    LogInfo("NVAPI initialized successfully");
    initialized = true;
    return true;
}

void NVAPIFullscreenPrevention::Cleanup() {
    if (hSession) {
        NvAPI_DRS_DestroySession(hSession);
        hSession = {0};
    }
    
    if (initialized) {
        NvAPI_Unload();
        initialized = false;
    }
}

bool NVAPIFullscreenPrevention::IsAvailable() const {
    return initialized;
}

bool NVAPIFullscreenPrevention::SetFullscreenPrevention(bool enable) {
    std::ostringstream oss;
    oss << "SetFullscreenPrevention called with enable=" << (enable ? "true" : "false");
    LogInfo(oss.str().c_str());
    
    if (!initialized) {
        last_error = "NVAPI not initialized";
        LogWarn("SetFullscreenPrevention failed: NVAPI not initialized");
        return false;
    }
    
    // Create DRS session
    LogInfo("Creating DRS session...");
    NvAPI_Status status = NvAPI_DRS_CreateSession(&hSession);
    if (status != NVAPI_OK) {
        last_error = "Failed to create DRS session";
        std::ostringstream oss_err;
        oss_err << "Failed to create DRS session. Status: " << status;
        LogWarn(oss_err.str().c_str());
        return false;
    }
    LogInfo("DRS session created successfully");
    
    // Load settings
    LogInfo("Loading DRS settings...");
    status = NvAPI_DRS_LoadSettings(hSession);
    if (status != NVAPI_OK) {
        last_error = "Failed to load DRS settings";
        std::ostringstream oss_err;
        oss_err << "Failed to load DRS settings. Status: " << status;
        LogWarn(oss_err.str().c_str());
        return false;
    }
    LogInfo("DRS settings loaded successfully");
    
    // Get current executable name
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    char* exeName = strrchr(exePath, '\\');
    if (exeName) exeName++;
    else exeName = exePath;
    
    std::ostringstream oss_exe;
    oss_exe << "Target executable: " << exeName;
    LogInfo(oss_exe.str().c_str());
    
    // Find or create application profile
    NVDRS_APPLICATION app = {0};
    app.version = NVDRS_APPLICATION_VER;
    strcpy((char*)app.appName, exeName);
    
    LogInfo("Searching for existing application profile...");
    status = NvAPI_DRS_FindApplicationByName(hSession, (NvU16*)exeName, &hProfile, &app);
    
    if (status == NVAPI_EXECUTABLE_NOT_FOUND) {
        LogInfo("Application profile not found, creating new one...");
        // Create new profile
        NVDRS_PROFILE profile = {0};
        profile.version = NVDRS_PROFILE_VER;
        profile.isPredefined = FALSE;
        strcpy((char*)profile.profileName, "Fullscreen Prevention Profile");
        
        status = NvAPI_DRS_CreateProfile(hSession, &profile, &hProfile);
        if (status != NVAPI_OK) {
            last_error = "Failed to create DRS profile";
            std::ostringstream oss_err;
            oss_err << "Failed to create DRS profile. Status: " << status;
            LogWarn(oss_err.str().c_str());
            return false;
        }
        LogInfo("DRS profile created successfully");
        
        // Add the application to the profile
        app.version = NVDRS_APPLICATION_VER;
        app.isPredefined = FALSE;
        app.isMetro = FALSE;
        strcpy((char*)app.appName, exeName);
        strcpy((char*)app.userFriendlyName, exeName);
        
        LogInfo("Adding application to profile...");
        status = NvAPI_DRS_CreateApplication(hSession, hProfile, &app);
        if (status != NVAPI_OK) {
            last_error = "Failed to create application in profile";
            std::ostringstream oss_err;
            oss_err << "Failed to create application in profile. Status: " << status;
            LogWarn(oss_err.str().c_str());
            return false;
        }
        LogInfo("Application added to profile successfully");
    } else if (status == NVAPI_OK) {
        LogInfo("Existing application profile found");
    } else {
        last_error = "Failed to find or create application profile";
        std::ostringstream oss_err;
        oss_err << "Failed to find or create application profile. Status: " << status;
        LogWarn(oss_err.str().c_str());
        return false;
    }
    
    if (status != NVAPI_OK) {
        last_error = "Failed to find or create application profile";
        std::ostringstream oss_err;
        oss_err << "Failed to find or create application profile. Status: " << status;
        LogWarn(oss_err.str().c_str());
        return false;
    }
    
    // Set fullscreen prevention setting using the same approach as SpecialK
    // OGL_DX_PRESENT_DEBUG_ID = 0x20324987
    // DISABLE_FULLSCREEN_OPT = 0x00000001
    // ENABLE_DFLIP_ALWAYS = 0x00000004
    // SIGNAL_PRESENT_END_FROM_CPU = 0x00000020
    // ENABLE_DX_SYNC_INTERVAL = 0x00000080
    // FORCE_INTEROP_GPU_SYNC = 0x00000200
    // ENABLE_DXVK = 0x00080000
    
    NVDRS_SETTING setting = {0};
    setting.version = NVDRS_SETTING_VER;
    setting.settingId = 0x20324987; // OGL_DX_PRESENT_DEBUG_ID from SpecialK
    setting.settingType = NVDRS_DWORD_TYPE;
    setting.settingLocation = NVDRS_CURRENT_PROFILE_LOCATION;
    
    if (enable) {
        // Use the same flags as SpecialK for optimal interop
        setting.u32CurrentValue = 0x00000001 | 0x00000004 | 0x00000020 | 0x00000080 | 0x00000200 | 0x00080000;
        // This includes: DISABLE_FULLSCREEN_OPT | ENABLE_DFLIP_ALWAYS | SIGNAL_PRESENT_END_FROM_CPU | ENABLE_DX_SYNC_INTERVAL | FORCE_INTEROP_GPU_SYNC | ENABLE_DXVK
        std::ostringstream oss_flags;
        oss_flags << "Setting fullscreen prevention flags: 0x" << std::hex << setting.u32CurrentValue << std::dec;
        LogInfo(oss_flags.str().c_str());
    } else {
        setting.u32CurrentValue = 0x00000000; // Disable all flags
        LogInfo("Disabling all fullscreen prevention flags");
    }
    
    LogInfo("Applying DRS setting...");
    status = NvAPI_DRS_SetSetting(hSession, hProfile, &setting);
    if (status != NVAPI_OK) {
        last_error = "Failed to set DRS setting";
        std::ostringstream oss_err;
        oss_err << "Failed to set DRS setting. Status: " << status;
        LogWarn(oss_err.str().c_str());
        return false;
    }
    LogInfo("DRS setting applied successfully");
    
    // Save settings
    LogInfo("Saving DRS settings...");
    status = NvAPI_DRS_SaveSettings(hSession);
    if (status != NVAPI_OK) {
        last_error = "Failed to save DRS settings";
        std::ostringstream oss_err;
        oss_err << "Failed to save DRS settings. Status: " << status;
        LogWarn(oss_err.str().c_str());
        return false;
    }
    LogInfo("DRS settings saved successfully");
    
    fullscreen_prevention_enabled = enable;
    std::ostringstream oss_success;
    oss_success << "Fullscreen prevention " << (enable ? "enabled" : "disabled") << " successfully";
    LogInfo(oss_success.str().c_str());
    return true;
}

bool NVAPIFullscreenPrevention::IsFullscreenPreventionEnabled() const {
    return fullscreen_prevention_enabled;
}

std::string NVAPIFullscreenPrevention::GetLastError() const {
    return last_error;
}

std::string NVAPIFullscreenPrevention::GetDriverVersion() const {
    if (!initialized) {
        return "NVAPI not initialized";
    }
    
    NvU32 driverVersion = 0;
    NvAPI_ShortString branchString = {0};
    NvAPI_Status status = NvAPI_SYS_GetDriverAndBranchVersion(&driverVersion, branchString);
    if (status != NVAPI_OK) {
        return "Failed to get driver version";
    }
    
    // Format the driver version similar to SpecialK
    char ver_str[64];
    snprintf(ver_str, sizeof(ver_str), "%03u.%02u", driverVersion / 100, driverVersion % 100);
    return std::string(ver_str);
}

bool NVAPIFullscreenPrevention::HasNVIDIAHardware() const {
    if (!initialized) {
        LogWarn("HasNVIDIAHardware called but NVAPI not initialized");
        return false;
    }
    
    NvU32 gpuCount = 0;
    NvPhysicalGpuHandle gpus[64] = {0};
    NvAPI_Status status = NvAPI_EnumPhysicalGPUs(gpus, &gpuCount);
    
    std::ostringstream oss;
    oss << "NvAPI_EnumPhysicalGPUs returned status: " << status << ", GPU count: " << gpuCount;
    LogInfo(oss.str().c_str());
    
    bool hasHardware = (status == NVAPI_OK && gpuCount > 0);
    std::ostringstream oss_result;
    oss_result << "NVIDIA hardware detection: " << (hasHardware ? "SUCCESS" : "FAILED");
    LogInfo(oss_result.str().c_str());
    
    return hasHardware;
}

std::string NVAPIFullscreenPrevention::GetLibraryPath() const {
    return "Static linking - no DLL path";
}

std::string NVAPIFullscreenPrevention::GetFunctionStatus() const {
    if (!initialized) {
        return "NVAPI not initialized";
    }
    
    std::ostringstream oss;
    oss << "Library: Static linking (nvapi64.lib)\n";
    oss << "Core Functions: ✓ Initialize, ✓ Unload\n";
    oss << "DRS Functions: ✓ CreateSession, ✓ DestroySession, ✓ LoadSettings, ✓ SaveSettings\n";
    oss << "Profile Functions: ✓ FindApp, ✓ CreateProfile, ✓ SetSetting\n";
    oss << "System Functions: ✓ GetDriverVersion, ✓ EnumGPUs";
    
    return oss.str();
}

std::string NVAPIFullscreenPrevention::GetDetailedStatus() const {
    std::ostringstream oss;
    
    oss << "=== NVAPI Detailed Status ===\n";
    oss << "Initialized: " << (initialized ? "Yes" : "No") << "\n";
    oss << "Library: Static linking (nvapi64.lib)\n";
    oss << "Function Status:\n" << GetFunctionStatus() << "\n";
    
    if (initialized) {
        oss << "Session Handle: " << (hSession ? "Valid" : "Invalid") << "\n";
        oss << "Profile Handle: " << (hProfile ? "Valid" : "Invalid") << "\n";
        oss << "Fullscreen Prevention: " << (fullscreen_prevention_enabled ? "Enabled" : "Disabled") << "\n";
        
        if (hSession) {
            oss << "DRS Session: Active\n";
        }
        if (hProfile) {
            oss << "DRS Profile: Active\n";
        }
    }
    
    if (!last_error.empty()) {
        oss << "Last Error: " << last_error << "\n";
    }
    
    return oss.str();
}

std::string NVAPIFullscreenPrevention::GetDllVersionInfo() const {
    if (!initialized) {
        return "NVAPI not initialized";
    }
    
    std::ostringstream oss;
    oss << "Static linking with nvapi64.lib\n";
    oss << "No DLL path - functions resolved at link time\n";
    oss << "Architecture: 64-bit (x64)";
    
    return oss.str();
}

// Global instance
NVAPIFullscreenPrevention g_nvapiFullscreenPrevention;
