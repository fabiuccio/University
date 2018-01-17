#include "stdafx.h"
#define _WIN32_WINNT 0x0400
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <list>
#include <iostream> //also for debugging
#include <fstream>
#include "supportlib.h"

using namespace std;

#define SERVICE_NAME "VLService"
#define MULTICAST_ADDRESS "239.255.255.250"
#define MULTICAST_PORT 1600
#define POLLING_TIME 10 //10000?

SERVICE_STATUS_HANDLE serviceStatusHandle;


BOOL serviceRunning = 0;
BOOL servicePaused = 0;
DWORD currentServiceStatus;

HANDLE h[3]={INVALID_HANDLE_VALUE}; 
HANDLE restartEvent=INVALID_HANDLE_VALUE;
HANDLE timer;

SOCKET RecvSocket;
SOCKET SendSocket;
SOCKET ListenSocket;
SOCKET AcceptSocket;

BOOL StartServiceThread();
VOID ServiceCtrlHandler (DWORD controlCode);
VOID ServiceMain(DWORD argc, char *argv[]);
int UpdateSCMStatus (DWORD dwCurrentState, DWORD dwWin32ExitCode,
		     DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint,
		     DWORD dwWaitHint);
void terminateService(int iErr);
unsigned int __stdcall ClientBehaviourThread(LPVOID param );
unsigned int __stdcall userInterfaceThread(LPVOID param );

 
int _tmain(int argc, _TCHAR* argv[])
{
	SERVICE_TABLE_ENTRY serviceTable[] =
	{
	{
	(LPWSTR)SERVICE_NAME,
	(LPSERVICE_MAIN_FUNCTION) ServiceMain}
	,
	{ NULL, NULL }
	};

	BOOL success = StartServiceCtrlDispatcher(serviceTable);

	if (!success)
	{
		return 1;
	}
	return 0;
}



VOID ServiceMain(DWORD argc, char* argv[])
{

   serviceStatusHandle = RegisterServiceCtrlHandler((LPWSTR)SERVICE_NAME,
                           (LPHANDLER_FUNCTION) ServiceCtrlHandler);

   if (!serviceStatusHandle)
   {
	  terminateService(GetLastError());
      return;
   }

   BOOL success = UpdateSCMStatus(SERVICE_START_PENDING, 
				NO_ERROR, 0, 1, 5000);
   if (!success) {
      terminateService(GetLastError());
      return;
   }

   h[2] = CreateEvent (0, TRUE, FALSE, 0);

   if (!h[2]) {
      terminateService(GetLastError());
      return;
   }

   restartEvent=CreateEvent(0,TRUE,FALSE,0);

   if (!restartEvent) {
	 terminateService(GetLastError());
	 return;
   }

   success = UpdateSCMStatus(SERVICE_START_PENDING, 
				NO_ERROR, 0, 2, 1000);
   if (!success) {
      terminateService(GetLastError());
      return;
   }
   
   success = StartServiceThread();
   
   if (!success){
      terminateService(GetLastError());
      return;
   }

   currentServiceStatus = SERVICE_RUNNING;
   success = UpdateSCMStatus(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
   if (!success){
      terminateService(GetLastError());
      return;
   }

   while(1)
   { 

		WaitForMultipleObjects(3,h,TRUE,INFINITE);

		//Se sono nello stato di PAUSA...
		if (currentServiceStatus==SERVICE_PAUSED)
		{
			//... attendo l'evento restartEvent
			WaitForSingleObject(restartEvent, INFINITE);
			ResetEvent(restartEvent);
			
		}
		else
		{
			//... altrimenti termino il servizio
			break;
		}

   }

  closesocket(RecvSocket);
  closesocket(ListenSocket);
  closesocket(SendSocket);
  closesocket(AcceptSocket);

  CloseHandle(timer);
  CloseHandle(h[0]);
  CloseHandle(h[1]);
  CloseHandle(h[2]);
  CloseHandle(restartEvent);

  terminateService(0);
}


unsigned int __stdcall ClientBehaviourThread(LPVOID param )
{
	/* Loading dinamico della DLL per la comunicazione con il 
	   minifilter.*/

//char* path="c:\\documents and settings\\gidan\\desktop\\testing.txt";
//writeOnFile(path,"DEBUGGING Service... ");
	HINSTANCE hDLL;
//writeOnFile(path,"Dynamically loading");
	BOOL returnval = dinamicallyLoad(hDLL, L"minispy.DLL");
	if (!returnval)
		return 0;
//if (returnval) writeOnFile(path,"Loaded CORRECTLY!");
//else writeOnFile(path,	"NOT loaded CORRECTLY!");
	HANDLE port = NULL;
	returnval= callConnect(hDLL, port);
	if (!returnval)
		return 0;
//if (returnval) writeOnFile(path,	"CONNECTED SUCCESFULLY!");
//else writeOnFile(path,	"NOT CONNECTED SUCCESFULLY!");
	returnval = callAttachDevice(hDLL, "c:");
//if (returnval) writeOnFile(path,"ATTACHED c:");
//else writeOnFile(path,"NOT ATTACHED C: :(");
	//BOOL returnval4 = callDetachDevice(hDLL, "c:");
	//BOOL returnval5 = callDetachDevice(hDLL, "c:");

/* */
	WSADATA wsaData;
	sockaddr_in RecvAddr;

	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR){
		WSACleanup();
		return 1;
	}
//writeOnFile(path,"WSAStartup called with no problems!");
	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendSocket==INVALID_SOCKET)
	{
		closesocket(SendSocket);
		WSACleanup();
		return 0;
	}
//writeOnFile(path,"Socket created without any problem!");
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(MULTICAST_PORT);
	RecvAddr.sin_addr.s_addr = inet_addr(MULTICAST_ADDRESS);

	timer=CreateWaitableTimer(NULL,FALSE,NULL);
	LARGE_INTEGER liUTC;
	liUTC.LowPart=0;
	liUTC.HighPart=0;
	if(!SetWaitableTimer(timer, &liUTC, POLLING_TIME, NULL, NULL, FALSE) )
	{
		CloseHandle(timer);
		WSACleanup();
		return 1;
	}
	
//writeOnFile(path,"getting inside service running... service is running?");
	while(serviceRunning)
	{
//writeOnFile(path,"Yes, service is running! Waiting on timer!");
		WaitForSingleObject(timer, INFINITE);
//writeOnFile(path,"Waited on timer...");
		const int BufLen = 1024;
		char SendBuf[BufLen]={0};
//writeOnFile(path,"Calling getPacket...");
		callGetPacket(hDLL, port, (UCHAR *)SendBuf);
//writeOnFile(path,"Calling sendto...");
		sendto(SendSocket, SendBuf, BufLen,0,(SOCKADDR *) &RecvAddr, sizeof(RecvAddr));
//writeOnFile(path,"Sending Packet xD:");
//writeOnFile(path,SendBuf);
	}

	closesocket(SendSocket);
	WSACleanup();
	return 0;
}


unsigned int __stdcall userInterfaceThread(LPVOID param )
{
	/*...*/
	return 0;

}

BOOL StartServiceThread()
{
	DWORD id;
	h[0] = (HANDLE)_beginthreadex( NULL, 0, & ClientBehaviourThread, NULL, 0, (unsigned int *)&id);

  
	DWORD id2;
	h[1] = (HANDLE)_beginthreadex( NULL, 0, &userInterfaceThread, NULL, 0, (unsigned int *)&id2);


   if (h[0] == 0 || h[1] ==0) 
   {
      return FALSE;
   }
   else 
   {
      serviceRunning = TRUE;
      return TRUE;
   }
}

VOID ServiceCtrlHandler (DWORD controlCode)
{
	BOOL success;
   switch(controlCode)
   {
     
      case SERVICE_CONTROL_PAUSE:
         if (serviceRunning && !servicePaused)
         {
            success = UpdateSCMStatus(SERVICE_PAUSE_PENDING, NO_ERROR,
                                      0, 1, 1000);
            servicePaused = TRUE;
			serviceRunning=FALSE;
            currentServiceStatus = SERVICE_PAUSED;

			//arresto timer
			LARGE_INTEGER li;
			li.LowPart = 0;
			li.HighPart = 0;
			SetWaitableTimer(timer,&li,0,NULL,NULL,FALSE);

			closesocket(ListenSocket);
			closesocket(AcceptSocket);
			closesocket(SendSocket);
			closesocket(RecvSocket);
			CloseHandle(h[0]);
			CloseHandle(h[1]);
			CloseHandle(h[2]);
			serviceRunning=TRUE;

         }
         break;
      
      case SERVICE_CONTROL_CONTINUE:
         if (serviceRunning && servicePaused)
         {
            success = UpdateSCMStatus(SERVICE_CONTINUE_PENDING, NO_ERROR, 0,
                                      1, 1000);
            servicePaused=FALSE;
			serviceRunning=TRUE;
            currentServiceStatus = SERVICE_RUNNING;

			success = StartServiceThread();
			if (!success){
				terminateService(GetLastError());
				return ;
			}

			SetEvent(restartEvent);


         }
         break;
      
      case SERVICE_CONTROL_INTERROGATE:
         break;
      
      case SERVICE_CONTROL_SHUTDOWN:
      
      case SERVICE_CONTROL_STOP:
         currentServiceStatus = SERVICE_STOP_PENDING;
         success = UpdateSCMStatus(SERVICE_STOP_PENDING, NO_ERROR, 0, 1, 5000);
         terminateService(0);
         return;
      default:
          break;
   }
   UpdateSCMStatus(currentServiceStatus, NO_ERROR, 0, 0, 0);
}

BOOL UpdateSCMStatus (DWORD dwCurrentState,
                      DWORD dwWin32ExitCode,
                      DWORD dwServiceSpecificExitCode,
                      DWORD dwCheckPoint,
                      DWORD dwWaitHint)
{
   BOOL success;
   SERVICE_STATUS serviceStatus;
   serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
   serviceStatus.dwCurrentState = dwCurrentState;
   
   if (dwCurrentState == SERVICE_START_PENDING)
   {
      serviceStatus.dwControlsAccepted = 0;
   }
   else
   {
      serviceStatus.dwControlsAccepted =
         SERVICE_ACCEPT_STOP |
         SERVICE_ACCEPT_PAUSE_CONTINUE |
         SERVICE_ACCEPT_SHUTDOWN;
   }
  
   if (dwServiceSpecificExitCode == 0)
   {
      serviceStatus.dwWin32ExitCode = dwWin32ExitCode;
   }
   else
   {
      serviceStatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
   }
   serviceStatus.dwServiceSpecificExitCode =   dwServiceSpecificExitCode;
   serviceStatus.dwCheckPoint = dwCheckPoint;
   serviceStatus.dwWaitHint = dwWaitHint;
   
   success = SetServiceStatus (serviceStatusHandle, &serviceStatus);
   if (!success)
   {
      terminateService(0);
   }
   return success;
}



void terminateService(int iErr)
{
	serviceRunning = 0;
	if(h[2]!=INVALID_HANDLE_VALUE) SetEvent(h[2]);
	UpdateSCMStatus(SERVICE_STOPPED,iErr,0,0,0);

}