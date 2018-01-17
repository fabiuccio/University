#include "stdafx.h"
#include <Windows.h>
#include <iostream>


using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
   SC_HANDLE myService, scm;
   BOOL success;
   SERVICE_STATUS status;
   cout << "Removing Service...\n";
   // Open a Service Control Manager connection
   scm = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
   if (!scm)
   {
	   cout<<"OpenSCManager Fails!";
	   cout<<GetLastError()<<endl;
   }
   cout << "Opened Service Control Manager...\n";
   // Get the service's handle
   myService = OpenService(scm, TEXT("VLService"), SERVICE_ALL_ACCESS | DELETE);
   if (!myService)
   {
	  cout<<"OpenService Fails!";
	  cout<<GetLastError()<<endl;
   }
   // Stop the service if necessary
   success = QueryServiceStatus(myService, &status);
   if (!success)
   {
	  cout<<"QueryServiceStatus fails!";
	  cout<<GetLastError()<<endl;
   }
   if (status.dwCurrentState != SERVICE_STOPPED)
   {
      cout << "Service currently active.  Stopping service...\n";
      success = ControlService(myService, SERVICE_CONTROL_STOP, &status);
      if (!success)
      {
		 cout<<"ControlService fails to stop service!";
	     cout<<GetLastError()<<endl;
      }
      Sleep(500);
   }
   // Remove the service
   success = DeleteService(myService);
   if (success)
   {
      cout << "Service successfully removed.\n"<<endl;
   }
   else
   {
	  cout<<"DeleteService Fails!";
	  cout<<GetLastError()<<endl;
   }
   CloseServiceHandle(myService);
   CloseServiceHandle(scm);
	int x;cin>>x;	
   return 0;
}

