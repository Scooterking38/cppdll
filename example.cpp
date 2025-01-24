#include <windows.h>
#include <commdlg.h>  // For file dialogs
#include <iostream>

#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "shell32.lib")  // Ensure that shell32.lib is linked for ShellExecute

// Function prototype for the DLL entry point
extern "C" __declspec(dllexport) void __stdcall RunFileLauncher(LPSTR lpParam);

// This function will create a file dialog and execute the selected file
void LaunchFileDialogAndRun()
{
    OPENFILENAME ofn;       // Common dialog box structure
    char szFile[260];       // Buffer for file name

    // Initialize OPENFILENAME structure
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "All Files\0*.*\0Executable Files\0*.exe\0DLL Files\0*.dll\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Select a file to run";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

    // Display the file dialog
    if (GetOpenFileNameA(&ofn) == TRUE) {
        // Use ShellExecute to run the selected file
        ShellExecuteA(NULL, "open", ofn.lpstrFile, NULL, NULL, SW_SHOWNORMAL);
    }
}

// This function is the entry point that rundll32 will call
extern "C" __declspec(dllexport) void __stdcall RunFileLauncher(LPSTR lpParam)
{
    // Show the file dialog and run the selected file
    LaunchFileDialogAndRun();
}

// DLL entry point (necessary for DLLs in Windows)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize any necessary resources or perform setup here
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        // Clean up resources here if necessary
        break;
    }
    return TRUE;
}
