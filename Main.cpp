#include <windows.h>
#include <string>
#include <shlobj.h>
#include <shellapi.h>
#include <atlbase.h>

#pragma comment(lib, "shell32.lib")

// Entry point for rundll32
extern "C" __declspec(dllexport) void RunCommand(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
    if (!lpszCmdLine || strlen(lpszCmdLine) == 0) {
        MessageBoxW(NULL, L"No command provided.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Convert lpszCmdLine (LPSTR) to Unicode (LPWSTR)
    size_t cmdLen = strlen(lpszCmdLine) + 1;
    wchar_t* wideCmdLine = new wchar_t[cmdLen];
    mbstowcs(wideCmdLine, lpszCmdLine, cmdLen);

    // Get the current directory
    wchar_t currentDir[MAX_PATH] = { 0 };
    if (!GetModuleFileNameW(NULL, currentDir, MAX_PATH)) {
        MessageBoxW(NULL, L"Failed to get current directory.", L"Error", MB_OK | MB_ICONERROR);
        delete[] wideCmdLine;
        return;
    }

    std::wstring dllPath = currentDir;
    size_t lastSlash = dllPath.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos) {
        dllPath = dllPath.substr(0, lastSlash);
    }

    // Path to the shortcut
    std::wstring shortcutPath = dllPath + L"\\shortcut.lnk";

    // Modify the shortcut to point to the specified command
    HRESULT hres;
    CComPtr<IShellLinkW> psl; // Use IShellLinkW for Unicode
    hres = CoInitialize(NULL);
    if (SUCCEEDED(hres)) {
        hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&psl);
        if (SUCCEEDED(hres)) {
            CComPtr<IPersistFile> ppf;
            hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
            if (SUCCEEDED(hres)) {
                // Load the shortcut file
                hres = ppf->Load(shortcutPath.c_str(), STGM_READWRITE);
                if (SUCCEEDED(hres)) {
                    // Set the new target path
                    psl->SetPath(wideCmdLine);

                    // Save the changes
                    hres = ppf->Save(shortcutPath.c_str(), TRUE);
                    if (SUCCEEDED(hres)) {
                        // Execute the shortcut
                        ShellExecuteW(NULL, L"open", shortcutPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
                    } else {
                        MessageBoxW(NULL, L"Failed to save the shortcut.", L"Error", MB_OK | MB_ICONERROR);
                    }
                } else {
                    MessageBoxW(NULL, L"Failed to load the shortcut.", L"Error", MB_OK | MB_ICONERROR);
                }
            }
        }
        CoUninitialize();
    }

    delete[] wideCmdLine; // Free the allocated memory
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}
