#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <atlbase.h>
#include <shlobj.h>
#include <winternl.h>
#include <strsafe.h>

using namespace std;

// Function to list all programs in the registry "Run" keys
void ListRegistryStartupPrograms() {
    const char* regKeys[] = {
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        "Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
    };

    HKEY hKey;
    for (const char* regKey : regKeys) {
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, regKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char valueName[255];
            char valueData[255];
            DWORD valueNameSize, valueDataSize;
            DWORD index = 0;

            while (true) {
                valueNameSize = sizeof(valueName);
                valueDataSize = sizeof(valueData);

                if (RegEnumValue(hKey, index, valueName, &valueNameSize, NULL, NULL, (LPBYTE)valueData, &valueDataSize) == ERROR_SUCCESS) {
                    cout << "Registry Startup Program: " << valueName << " = " << valueData << endl;
                    index++;
                } else {
                    break;
                }
            }
            RegCloseKey(hKey);
        }
    }
}

// Function to list all programs in the user's Startup folder
void ListStartupFolderPrograms() {
    // Get the path to the Startup folder
    TCHAR szPath[MAX_PATH];
    if (SHGetFolderPath(NULL, CSIDL_STARTUP, NULL, 0, szPath) == S_OK) {
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile((std::wstring(szPath) + L"\\*").c_str(), &findFileData);

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    continue; // Skip directories
                }
                cout << "Startup Folder Program: " << findFileData.cFileName << endl;
            } while (FindNextFile(hFind, &findFileData) != 0);
            FindClose(hFind);
        }
    }
}

// Function to list all tasks in the Task Scheduler (scheduled to run on startup)
void ListTaskSchedulerPrograms() {
    // Here we would call Task Scheduler API functions to list scheduled tasks
    // For simplicity, let's assume we use a tool like schtasks in this case
    cout << "Task Scheduler Startup Programs (using schtasks):\n";
    system("schtasks /query /fo LIST /v | findstr /i \"At logon\"");
}

// Function to list all automatic services in the system
void ListWindowsServices() {
    // Query the services using OpenSCManager and EnumServicesStatus
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (hSCManager == NULL) {
        cerr << "Failed to open service manager\n";
        return;
    }

    LPENUM_SERVICE_STATUS serviceInfo;
    DWORD bytesNeeded, servicesReturned, resumeHandle = 0;
    if (EnumServicesStatus(hSCManager, SERVICE_WIN32, SERVICE_STATE_ALL, serviceInfo, 0, &bytesNeeded, &servicesReturned, &resumeHandle)) {
        for (DWORD i = 0; i < servicesReturned; ++i) {
            // Check if the service is set to start automatically
            if (serviceInfo[i].ServiceStatus.dwCurrentState == SERVICE_RUNNING) {
                cout << "Running Service: " << serviceInfo[i].lpServiceName << endl;
            }
        }
    } else {
        cerr << "Failed to enumerate services\n";
    }

    CloseServiceHandle(hSCManager);
}

int main() {
    cout << "Analyzing Startup Programs\n" << endl;

    // List Startup Programs in Registry
    ListRegistryStartupPrograms();

    // List Startup Programs in the Startup Folder
    ListStartupFolderPrograms();

    // List Task Scheduler Startup Programs
    ListTaskSchedulerPrograms();

    // List Services that start automatically
    ListWindowsServices();

    return 0;
}
