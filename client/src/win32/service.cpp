#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <iostream>

#define SERVICE_NAME             _T("ClairvoyanceService")

#define SERVICE_DISPLAY_NAME     _T("Clairvoyance Client Service")

#define SERVICE_START_TYPE       SERVICE_AUTO_START

#define SERVICE_DEPENDENCIES     _T("")

#define SERVICE_ACCOUNT          _T("NT AUTHORITY\\LocalSystem")

#define SERVICE_PASSWORD         NULL

SERVICE_STATUS          svc_status; 
SERVICE_STATUS_HANDLE   svc_status_handle; 
HANDLE                  svc_stop_event = NULL;

void service_install()
{
    SC_HANDLE scm_handle;
    SC_HANDLE service_handle;
    TCHAR exe_path[MAX_PATH];

    scm_handle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if(NULL == scm_handle) 
    {
        std::cout << "OpenSCManager failed: " << GetLastError() << std::endl;
        return;
    }

    service_handle = CreateService(scm_handle, SERVICE_NAME, SERVICE_DISPLAY_NAME, 
                                   SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
                                   SERVICE_START_TYPE, SERVICE_ERROR_NORMAL, exe_path,
                                   NULL, NULL, SERVICE_DEPENDENCIES, SERVICE_ACCOUNT,
                                   SERVICE_PASSWORD);

    if(NULL == service_handle) 
    {
        std::cout << "CreateService failed: " << GetLastError() << std::endl;
        CloseServiceHandle(scm_handle);
        return;
    }
    else std::cout << "Service installed successfully" << std::endl;

    CloseServiceHandle(service_handle); 
    CloseServiceHandle(scm_handle);
}
