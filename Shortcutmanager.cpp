#include <windows.h>
#include <shobjidl.h>
#include <objbase.h>
#include <shlguid.h>

// Exported function with the correct signature for rundll32
extern "C" __declspec(dllexport) void CALLBACK ModifyShortcutAndRun(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
    LPCWSTR shortcutPath = L"C:\\Users\\Public\\Desktop\\MyShortcut.lnk"; // Path to the shortcut
    WCHAR newTarget[MAX_PATH]; // Buffer for the new target path

    // Convert input parameter (lpszCmdLine) from ANSI to Unicode
    MultiByteToWideChar(CP_ACP, 0, lpszCmdLine, -1, newTarget, MAX_PATH);

    HRESULT hres;
    IShellLink* psl;

    // Initialize COM
    hres = CoInitialize(NULL);
    if (FAILED(hres)) return;

    // Create an instance of IShellLink
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;
        BOOL shortcutExists = (GetFileAttributes(shortcutPath) != INVALID_FILE_ATTRIBUTES);

        if (!shortcutExists) {
            // Shortcut does not exist, create it
            psl->SetPath(L"dummy.exe");  // Placeholder target
            psl->SetDescription(L"Auto-created Shortcut");

            // Save the new shortcut
            hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
            if (SUCCEEDED(hres)) {
                ppf->Save(shortcutPath, TRUE);
                ppf->Release();
            }
        }

        // Load the shortcut and update target
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
        if (SUCCEEDED(hres))
        {
            hres = ppf->Load(shortcutPath, STGM_READWRITE);
            if (SUCCEEDED(hres))
            {
                psl->SetPath(newTarget); // Set new target
                ppf->Save(shortcutPath, TRUE);
            }
            ppf->Release();
        }

        psl->Release();
    }

    // Execute the shortcut
    ShellExecute(NULL, L"open", shortcutPath, NULL, NULL, SW_SHOWNORMAL);

    CoUninitialize();
}
