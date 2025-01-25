#include <windows.h>
#include <string>
#include <shlobj.h>
#include <shobjidl_core.h>
#include <shellapi.h>
#include <atlbase.h>

#pragma comment(lib, "shell32.lib")

// Entry point for rundll32
extern "C" __declspec(dllexport) void RunCommand(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
    if (!lpszCmdLine || strlen(lpszCmdLine) == 0) {
        MessageBoxA(NULL, "No command provided.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Get the current directory
    char currentDir[MAX_PATH] = { 0 };
    if (!GetModuleFileNameA(NULL, currentDir, MAX_PATH)) {
        MessageBoxA(NULL, "Failed to get current directory.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    std::string dllPath = currentDir;
    size_t lastSlash = dllPath.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        dllPath = dllPath.substr(0, lastSlash);
    }

    // Path to the shortcut
    std::string shortcutPath = dllPath + "\\shortcut.lnk";

    // Modify the shortcut to point to the specified command
    HRESULT hres;
    CComPtr<IShellLink> psl;
    hres = CoInitialize(NULL);
    if (SUCCEEDED(hres)) {
        hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl);
        if (SUCCEEDED(hres)) {
            CComPtr<IPersistFile> ppf;
            hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
            if (SUCCEEDED(hres)) {
                // Load the shortcut file
                wchar_t wszShortcut[MAX_PATH];
                MultiByteToWideChar(CP_ACP, 0, shortcutPath.c_str(), -1, wszShortcut, MAX_PATH);
                hres = ppf->Load(wszShortcut, STGM_READWRITE);
                if (SUCCEEDED(hres)) {
                    // Set the new target path
                    psl->SetPath(lpszCmdLine);

                    // Save the changes
                    hres = ppf->Save(wszShortcut, TRUE);
                    if (SUCCEEDED(hres)) {
                        // Execute the shortcut
                        ShellExecuteA(NULL, "open", shortcutPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
                    } else {
                        MessageBoxA(NULL, "Failed to save the shortcut.", "Error", MB_OK | MB_ICONERROR);
                    }
                } else {
                    MessageBoxA(NULL, "Failed to load the shortcut.", "Error", MB_OK | MB_ICONERROR);
                }
            }
        }
        CoUninitialize();
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}
