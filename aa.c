#include <stdio.h>
#include <windows.h>

void QueryKey(HKEY hKey) {
    TCHAR    achKey[MAX_PATH];   // buffer for subkey name
    DWORD    cbName;             // size of name string 
    TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys = 0;               // number of subkeys 
    DWORD    cbMaxSubKey;          // longest subkey size 
    DWORD    cchMaxClass;          // longest class string 
    DWORD    cValues;              // number of values for key 
    DWORD    cchMaxValue;          // longest value name 
    DWORD    cbMaxValueData;       // longest value data 
    DWORD    cbSecurityDescriptor; // size of security descriptor 
    FILETIME ftLastWriteTime;      // last write time 

    DWORD i, retCode;

    TCHAR  achValue[MAX_VALUE_NAME];
    DWORD cchValue = MAX_VALUE_NAME;

    // Get the class name and the value count. 
    retCode = RegQueryInfoKey(
        hKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        &cchMaxValue,            // longest value name 
        &cbMaxValueData,         // longest value data 
        &cbSecurityDescriptor,   // security descriptor 
        &ftLastWriteTime);       // last write time 

    // Enumerate the subkeys, until RegEnumKeyEx fails.

    if (cSubKeys) {
        for (i = 0; i < cSubKeys; i++) {
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyEx(hKey, i,
                achKey,
                &cbName,
                NULL,
                NULL,
                NULL,
                &ftLastWriteTime);
            if (retCode == ERROR_SUCCESS) {
                HKEY hSubKey;
                if (RegOpenKeyEx(hKey, achKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
                    TCHAR szDisplayName[MAX_PATH];
                    DWORD dwSize = sizeof(szDisplayName);
                    if (RegQueryValueEx(hSubKey, TEXT("DisplayName"), NULL, NULL, (LPBYTE)szDisplayName, &dwSize) == ERROR_SUCCESS) {
                        TCHAR szDisplayVersion[MAX_PATH];
                        dwSize = sizeof(szDisplayVersion);
                        if (RegQueryValueEx(hSubKey, TEXT("DisplayVersion"), NULL, NULL, (LPBYTE)szDisplayVersion, &dwSize) == ERROR_SUCCESS) {
                            wprintf(L"Application: %s, Version: %s\n", szDisplayName, szDisplayVersion);
                        }
                    }
                    RegCloseKey(hSubKey);
                }
            }
        }
    }
}

int main() {
    HKEY hUninstallKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"), 0, KEY_READ, &hUninstallKey) == ERROR_SUCCESS) {
        QueryKey(hUninstallKey);
        RegCloseKey(hUninstallKey);
    } else {
        printf("Failed to open registry key.\n");
    }

    return 0;
}

Explanation:
Open the Uninstall Registry Key:
The registry path HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall is opened.
Query Key Information:
Information about the subkeys and values is retrieved using RegQueryInfoKey.
Enumerate Subkeys:
Each subkey represents an installed application. The RegEnumKeyEx function is used to enumerate these subkeys.
Query Subkey Values:
For each subkey, the DisplayName and DisplayVersion values are queried using RegQueryValueEx.
Store and Print Results:
The application name and version are printed to the console.
Notes:
64-bit Windows: For 64-bit applications on 64-bit Windows, the registry key might differ. Use KEY_WOW64_64KEY flag for accessing 64-bit registry keys.
Permissions: Ensure the program runs with sufficient permissions to read the registry.
Error Handling: The program includes basic error handling, but additional error checks and logging might be necessary for robustness.
Compile this program with a C compiler, such as gcc, ensuring the Windows headers and libraries are properly linked.
