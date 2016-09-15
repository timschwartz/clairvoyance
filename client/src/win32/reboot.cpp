#include <windows.h>
#include <wbemidl.h>

#include <iostream>
#include "../include/reboot.h"

namespace clairvoyance
{
    bool elevate_privilege(const TCHAR *privilege)
    {
        HANDLE hToken;
        TOKEN_PRIVILEGES tkp;

        // Get a token for this process.

        if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
            return false;

        // Get the LUID for the shutdown privilege.

        LookupPrivilegeValue(NULL, privilege, &tkp.Privileges[0].Luid);    
        
        tkp.PrivilegeCount = 1;  // one privilege to set
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        // Get the shutdown privilege for this process.

        AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

        if (GetLastError() != ERROR_SUCCESS) return false;

        return true;
    } 

    bool reboot()
    {
        bool result = false;
        if(!elevate_privilege(SE_SHUTDOWN_NAME)) return false;

        std::cout << "Rebooting" << std::endl;
        result = ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_MAINTENANCE);
        
        return result;
    }

    bool edit_boot(uint8_t mode)
    {
        HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED); 
        if (FAILED(hr)) 
        { 
            std::cout << "Failed to initialize COM library. Error code = 0x"<< std::hex << hr << std::endl; 
            return hr;
        }

        switch(mode)
        {
            case MODE_NORMAL:
                break;
            case MODE_SAFE_NETWORK:
                break;
        }
    }
}
