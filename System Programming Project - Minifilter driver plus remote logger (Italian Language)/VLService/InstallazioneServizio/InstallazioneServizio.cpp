#include "stdafx.h"
#include <windows.h>
#include <iostream>

using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
   SC_HANDLE myService, scm;
   cout << "Installing Volume Logger Service (VLService)...\n";
   // open a connection to the SCM
   scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
   if (!scm)
   {
      cout << "OpenSCManager fails! ";
	  cout << GetLastError();
   }
   cout << "Opened Service Control Manager...\n";
   // Install the new service
   myService = CreateService(
      scm, TEXT("VLService"), // the internal service name used by the SCM
      TEXT("VLService"),  // the external label seen in the Service Control applet
      SERVICE_ALL_ACCESS,		  // We want full access to control the service
      SERVICE_WIN32_OWN_PROCESS,  // The service is a single app and not a driver
      SERVICE_DEMAND_START,		  // The service will be started by us manually
      SERVICE_ERROR_NORMAL,		  // If error during service start, don't misbehave.
      TEXT("C:\\tesina\\VLService\\debug\\VLService.exe"),
      0, 0, 0, 0, 0);
   if (!myService)
   {
	  cout << "CreateService fails! ";
	  cout << GetLastError();
   }
   else
   {
      cout << "Service successfully installed.\n";
   }
   // clean up
   CloseServiceHandle(myService);
   CloseServiceHandle(scm);

   int x;cin>>x;
}

