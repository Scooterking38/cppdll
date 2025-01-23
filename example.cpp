#include <windows.h>
#include <iostream>

extern "C" __declspec(dllexport) void addMagnifierToStartup() {
    // Path to the Magnifier executable (Magnifier is a built-in app in Windows)
    const char* magnifierPath = "C:\\Windows\\System32\\magnify.exe";

    // Registry key to add to the current user's startup
    HKEY hKey;
    if (RegCreateKeyExA(
        HKEY_CURRENT_USER, 
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
        0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) 
    {
        // Add a new registry entry under the "Run" key for Magnifier
        RegSetValueExA(hKey, "Magnifier", 0, REG_SZ, (const BYTE*)magnifierPath, (DWORD)(strlen(magnifierPath) + 1));
        RegCloseKey(hKey);

        std::cout << "Magnifier added to startup!" << std::endl;
    } else {
        std::cerr << "Failed to create registry key!" << std::endl;
    }
}
