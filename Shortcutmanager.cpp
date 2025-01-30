#include <windows.h>
#include <shobjidl.h>
#include <objbase.h>
#include <shlguid.h>

// Export function for rundll32
extern "C" __declspec(dllexport) void CALLBACK ModifyShortcutAndRun(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
    LPCWSTR shortcutPath = L"C:\\Users\\Public\\Desktop\\MyShortcut.lnk"; // Path to shortcut
    WCHAR newTarget[MAX_PATH]; // Buffer for new target path

    // Convert input parameter (lpszCmdLine) from ANSI to Unicode
    MultiByteToWideChar(CP_ACP, 0, lpszCmdLine, -1, newTarget, MAX_PATH);

    HRESULT hres;
    IShellLinkW* psl;

    // Initialize COM
    hres = CoInitialize(NULL);
    if (FAILED(hres)) return;

    // Create an instance of IShellLinkW (Unicode version)
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (LPVOID*)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;
        BOOL shortcutExists = (GetFileAttributesW(shortcutPath) != INVALID_FILE_ATTRIBUTES);

        if (!shortcutExists) {
            // Create shortcut if it doesn't exist
            psl->SetPath(L"dummy.exe");  // Placeholder target
            psl->SetDescription(L"Auto-created Shortcut");

            // Save new shortcut
            hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
            if (SUCCEEDED(hres)) {
                ppf->Save(shortcutPath, TRUE);
                ppf->Release();
            }
        }

        // Load shortcut and update target
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
    ShellExecuteW(NULL, L"open", shortcutPath, NULL, NULL, SW_SHOWNORMAL);

    CoUninitialize();
}
