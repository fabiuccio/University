// InterazioneServizio.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>


#define ATTACH_A 129
#define DETACH_A 130
#define ATTACH_B 131
#define DETACH_B 132
#define ATTACH_C 133
#define DETACH_C 134
#define ATTACH_D 135
#define DETACH_D 136
#define ATTACH_E 137
#define DETACH_E 138
#define ATTACH_F 139
#define DETACH_F 140


#include <iostream>
using namespace std;



int _tmain(int argc, _TCHAR* argv[])
{
   SC_HANDLE myService, scm;
   BOOL success;
   SERVICE_STATUS status;
   // Open a connection to the SCM
   scm = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS | GENERIC_WRITE);
   if (!scm)
   {
	  cout<<"OpenScManager Fails!";
	  cout<< GetLastError()<<endl;
   }
   // Get the service's handle
   myService = OpenService(scm, TEXT("VLService"), SERVICE_ALL_ACCESS);
   if (!myService)
   {
	  cout<<"OpenService Fails!";
	  cout<< GetLastError()<<endl;
   }

   int i=0;

   while(i!=-1)
   {
		cout<<"**************************************************************"<<endl;
		cout<<"***** SELECT THE DEVICE YOU WANT TO ATTACH OR DETACH *********"<<endl;
		cout<<"**************************************************************"<<endl;
		cout<<"  1) Attach the device A:"<<endl;
		cout<<"  2) Detach the device A:"<<endl;
		cout<<"  3) Attach the device B:"<<endl;
		cout<<"  4) Detach the device B:"<<endl;
		cout<<"  5) Attach the device C:"<<endl;
		cout<<"  6) Detach the device C:"<<endl;
		cout<<"  7) Attach the device D:"<<endl;
		cout<<"  8) Detach the device D:"<<endl;
		cout<<"  9) Attach the device E:"<<endl;
		cout<<" 10) Detach the device E:"<<endl;
		cout<<" 11) Attach the device F:"<<endl;
		cout<<" 12) Detach the device F:"<<endl;
		cout<<"Select the number of the funcion you want to accomplish:"<<endl;
		cout<<"-1 to quit."<<endl;
		cin>>i;

		switch(i){
			case 1:
				// Send the service the desired control message.
			   success = ControlService(myService, ATTACH_A, &status);
			   if (!success)
			   {
				  cout<<"ControlService Fails!";
				  cout<< GetLastError()<<endl;
			   }
			   cout << "Service control message successfully sent!" << endl;
				break;
			case 2:
				// Send the service the desired control message.
			   success = ControlService(myService, DETACH_A, &status);
			   if (!success)
			   {
				  cout<<"ControlService Fails!";
				  cout<< GetLastError()<<endl;
			   }
			   cout << "Service control message successfully sent!" << endl;
				break;
			case 3:
				// Send the service the desired control message.
			   success = ControlService(myService, ATTACH_B, &status);
			   if (!success)
			   {
				  cout<<"ControlService Fails!";
				  cout<< GetLastError()<<endl;
			   }
			   cout << "Service control message successfully sent!" << endl;
				break;
			case 4:
				// Send the service the desired control message.
			   success = ControlService(myService, DETACH_B, &status);
			   if (!success)
			   {
				  cout<<"ControlService Fails!";
				  cout<< GetLastError()<<endl;
			   }
			   cout << "Service control message successfully sent!" << endl;
				break;
			case 5:
				// Send the service the desired control message.
			   success = ControlService(myService, ATTACH_C, &status);
			   if (!success)
			   {
				  cout<<"ControlService Fails!";
				  cout<< GetLastError()<<endl;
			   }
			   cout << "Service control message successfully sent!" << endl;
				break;
			case 6:
				// Send the service the desired control message.
			   success = ControlService(myService, DETACH_C, &status);
			   if (!success)
			   {
				  cout<<"ControlService Fails!";
				  cout<< GetLastError()<<endl;
			   }
			   cout << "Service control message successfully sent!" << endl;
				break;
			case 7:
				// Send the service the desired control message.
			   success = ControlService(myService, ATTACH_D, &status);
			   if (!success)
			   {
				  cout<<"ControlService Fails!";
				  cout<< GetLastError()<<endl;
			   }
			   cout << "Service control message successfully sent!" << endl;
				break;
			case 8:
				// Send the service the desired control message.
			   success = ControlService(myService, DETACH_D, &status);
			   if (!success)
			   {
				  cout<<"ControlService Fails!";
				  cout<< GetLastError()<<endl;
			   }
			   cout << "Service control message successfully sent!" << endl;
				break;
			case 9:
				// Send the service the desired control message.
			   success = ControlService(myService, ATTACH_E, &status);
			   if (!success)
			   {
				  cout<<"ControlService Fails!";
				  cout<< GetLastError()<<endl;
			   }
			   cout << "Service control message successfully sent!" << endl;
				break;
			case 10:
				// Send the service the desired control message.
			   success = ControlService(myService, DETACH_E, &status);
			   if (!success)
			   {
				  cout<<"ControlService Fails!";
				  cout<< GetLastError()<<endl;
			   }
			   cout << "Service control message successfully sent!" << endl;
				break;
			case 11:
				// Send the service the desired control message.
			   success = ControlService(myService, ATTACH_F, &status);
			   if (!success)
			   {
				  cout<<"ControlService Fails!";
				  cout<< GetLastError()<<endl;
			   }
			   cout << "Service control message successfully sent!" << endl;
				break;
			case 12:
				// Send the service the desired control message.
			   success = ControlService(myService, DETACH_F, &status);
			   if (!success)
			   {
				  cout<<"ControlService Fails!";
				  cout<< GetLastError()<<endl;
			   }
			   cout << "Service control message successfully sent!" << endl;
				break;
			default:
				break;

			}
		}
   // Clean up
   CloseServiceHandle(myService);
   CloseServiceHandle(scm);
   return 0;
}

