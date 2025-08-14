#include "addon.hpp"

// Export addon information
extern "C" __declspec(dllexport) constexpr const char* NAME = "Resolution Override";
extern "C" __declspec(dllexport) constexpr const char* DESCRIPTION =
    "RenoDX Resolution/Window Override (Win32). Note: Applying window operations from the main thread can crash some apps; this addon performs them on a background thread.";
