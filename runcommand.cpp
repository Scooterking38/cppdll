#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <iostream>

#pragma comment(lib, "ole32.lib")  // Required for COM functions

// Function to modify and run the shortcut
void ModifyAndRunShortcut(LPCWSTR newTarget)
{
    // Get the directory of the current DLL
    wchar_t dllPath[MAX_PATH] = {0};
    GetModuleFileNameW(GetModuleHandle(NULL), dllPath, MAX_PATH);

    // Extract the directory (remove the filename)
    wchar_t *lastSlash = wcsrchr(dllPath, L'\\');
    if (lastSlash) *lastSlash = L'\0';

    // Construct shortcut path (shortcut.lnk is in the same directory as the DLL)
    std::wstring shortcutPath = std::wstring(dllPath) + L"\\shortcut.lnk";

    // Initialize COM
    CoInitialize(NULL);

    // Create the ShellLink COM object
    IShellLinkW *pShellLink;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (LPVOID*)&pShellLink);
    if (SUCCEEDED(hr))
    {
        // Get the IPersistFile interface
        IPersistFile *pPersistFile;
        hr = pShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&pPersistFile);
        if (SUCCEEDED(hr))
        {
            // Load the shortcut file
            hr = pPersistFile->Load(shortcutPath.c_str(), STGM_READWRITE);
            if (SUCCEEDED(hr))
            {
                // Set the new target for the shortcut
                pShellLink->SetPath(newTarget);
                
                // Save the modified shortcut
                pPersistFile->Save(shortcutPath.c_str(), TRUE);

                // Execute the updated shortcut
                ShellExecuteW(NULL, L"open", shortcutPath.c_str(), NULL, NULL, SW_SHOWDEFAULT);
            }
            pPersistFile->Release();
        }
        pShellLink->Release();
    }
    CoUninitialize();
}

// DLL Exported Function
extern "C" __declspec(dllexport) void RunCommand(LPCWSTR newTarget)
{
    ModifyAndRunShortcut(newTarget);
}

// DLL Entry Point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}
