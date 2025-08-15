// Window creation detection functions
WindowCreationInfo DetectWindowCreationInfo(HWND hwnd);
std::string GetDetailedWindowInfo(HWND hwnd);
std::string GetWindowCreationSummary(HWND hwnd);

// Window style change tracking
void TrackWindowStyleChange(HWND hwnd, LONG_PTR old_style, LONG_PTR new_style, LONG_PTR old_ex_style, LONG_PTR new_ex_style, const std::string& reason);

// Hook installation functions
bool InstallWindowCreationDetectionHooks();
void UninstallWindowCreationDetectionHooks();
