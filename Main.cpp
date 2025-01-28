#include <windows.h>
#include <shobjidl.h> // For IShellLink
#include <shlguid.h>  // For CLSID_ShellLink
#include <shellapi.h> // For ShellExecute
#include <iostream>

extern "C" __declspec(dllexport) int CALLBACK ChangeShortcutTarget(HINSTANCE hinst, LPSTR lpCmdLine, int nCmdShow) {
    // Parse lpCmdLine to get the shortcut path and new target path
    wchar_t shortcutPath[MAX_PATH];
    wchar_t newTargetPath[MAX_PATH];

    // Example command line: "C:\\path\\to\\your\\shortcut.lnk C:\\new\\target\\path.exe"
    swscanf_s(lpCmdLine, L"%ls %ls", shortcutPath, MAX_PATH, newTargetPath, MAX_PATH);

    HRESULT hres;
    IShellLink* psl;

    // Initialize COM library
    CoInitialize(NULL);

    // Create an instance of IShellLink
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres)) {
        IPersistFile* ppf;

        // Load the shortcut
        hres = psl->Load(shortcutPath, STGM_READ);
        if (SUCCEEDED(hres)) {
            // Set the new target path
            psl->SetPath(newTargetPath);

            // Save the changes
            hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
            if (SUCCEEDED(hres)) {
                hres = ppf->Save(shortcutPath, TRUE);
                ppf->Release();
            }
        }
        psl->Release();
    }

    // Uninitialize COM library
    CoUninitialize();

    // Execute the shortcut
    ShellExecute(NULL, L"open", shortcutPath, NULL, NULL, SW_SHOWNORMAL);

    return SUCCEEDED(hres) ? 0 : 1; // Return 0 on success, 1 on failure
}
