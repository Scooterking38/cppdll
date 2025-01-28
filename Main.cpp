#include <windows.h>
#include <string>
#include <shlobj.h>
#include <shellapi.h>

#pragma comment(lib, "shell32.lib")

extern "C" __declspec(dllexport) void RunCommand(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
    if (!lpszCmdLine || strlen(lpszCmdLine) == 0) {
        MessageBoxW(NULL, L"No command provided.", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Convert command-line argument to Unicode
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

    std::wstring shortcutPath = dllPath + L"\\shortcut.lnk";

    // Initialize COM
    HRESULT hres = CoInitialize(NULL);
    if (FAILED(hres)) {
        MessageBoxW(NULL, L"Failed to initialize COM.", L"Error", MB_OK | MB_ICONERROR);
        delete[] wideCmdLine;
        return;
    }

    IShellLinkW* psl = nullptr;
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&psl);
    if (SUCCEEDED(hres) && psl) {
        IPersistFile* ppf = nullptr;
        hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
        if (SUCCEEDED(hres) && ppf) {
            hres = ppf->Load(shortcutPath.c_str(), STGM_READWRITE);
            if (SUCCEEDED(hres)) {
                psl->SetPath(wideCmdLine);
                hres = ppf->Save(shortcutPath.c_str(), TRUE);
                if (SUCCEEDED(hres)) {
                    ShellExecuteW(NULL, L"open", shortcutPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
                } else {
                    MessageBoxW(NULL, L"Failed to save the shortcut.", L"Error", MB_OK | MB_ICONERROR);
                }
            } else {
                MessageBoxW(NULL, L"Failed to load the shortcut.", L"Error", MB_OK | MB_ICONERROR);
            }
            ppf->Release();
        }
        psl->Release();
    }

    CoUninitialize();
    delete[] wideCmdLine;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}
