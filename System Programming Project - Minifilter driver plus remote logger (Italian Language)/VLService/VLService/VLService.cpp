#include "stdafx.h"
#define _WIN32_WINNT 0x0400
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <list>
#include <psapi.h>
#include <iostream> //also for debugging
#include <fstream>
#include "supportlib.h"

using namespace std;

#define SERVICE_NAME "VLService"
#define MULTICAST_ADDRESS "239.255.255.250"
#define MULTICAST_PORT 1600
#define POLLING_TIME 10 //10000?
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

HINSTANCE hDLL;
HANDLE port = NULL;

BOOL StartServiceThread();
DWORD GetProcessName( DWORD processID,
						   __inout UCHAR *OutputBuffer);
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

	BOOL returnval = dinamicallyLoad(hDLL, L"minispy.DLL");
	if (!returnval)
		return 0;
	
	returnval= callConnect(hDLL, port);
	if (!returnval)
		return 0;

	WSADATA wsaData;
	sockaddr_in RecvAddr;

	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR){
		WSACleanup();
		return 1;
	}

	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendSocket==INVALID_SOCKET)
	{
		closesocket(SendSocket);
		WSACleanup();
		return 0;
	}

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
	
	while(true)
	{
		if((serviceRunning)&&(!servicePaused))
			{
				WaitForSingleObject(timer, INFINITE);
				const int BufLen = 1024;
				DWORD pid,num,i,count=0;
				char SendBuf[BufLen]={0};
				char SendBuf2[BufLen]={0};
				UCHAR SendBuf3[BufLen]={0};
				callGetPacket(hDLL, port, (UCHAR *)SendBuf);
				memcpy(SendBuf2,SendBuf,16);
				
				i=16;

				while(SendBuf[i]!=0)
				{
					SendBuf2[i]=SendBuf[i];
					i++;
				}

				SendBuf2[i]=0;
				i++;

				memcpy(&pid,SendBuf+i,4);
				num=GetProcessName(pid,SendBuf3);
				memcpy(SendBuf2+i,SendBuf3,num);
				memcpy(SendBuf2+i+num,SendBuf+i+4,1024-i-4);
				sendto(SendSocket, SendBuf2, BufLen,0,(SOCKADDR *) &RecvAddr, sizeof(RecvAddr));
			}
		else
			WaitForSingleObject(restartEvent, INFINITE);
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
		currentServiceStatus = SERVICE_PAUSED;

		}
		break;

		case SERVICE_CONTROL_CONTINUE:
			if (serviceRunning && servicePaused)
			{
				success = UpdateSCMStatus(SERVICE_CONTINUE_PENDING, NO_ERROR, 0,
								  1, 1000);
				servicePaused=FALSE;
				currentServiceStatus = SERVICE_RUNNING;

				SetEvent(restartEvent);

			}
		break;

		case SERVICE_CONTROL_INTERROGATE:
		break;

		case ATTACH_A:
			callAttachDevice(hDLL, "a:");
			break;
		case DETACH_A:
			callDetachDevice(hDLL, "a:");
		case ATTACH_B:
			callAttachDevice(hDLL, "b:");
			break;
		case DETACH_B:
			callDetachDevice(hDLL, "b:");
		case ATTACH_C:
			callAttachDevice(hDLL, "c:");
			break;
		case DETACH_C:
			callDetachDevice(hDLL, "c:");
		case ATTACH_D:
			callAttachDevice(hDLL, "d:");
			break;
		case DETACH_D:
			callDetachDevice(hDLL, "d:");
		case ATTACH_E:
			callAttachDevice(hDLL, "e:");
			break;
		case DETACH_E:
			callDetachDevice(hDLL, "e:");
		case ATTACH_F:
			callAttachDevice(hDLL, "f:");
			break;
		case DETACH_F:
			callDetachDevice(hDLL, "f:");
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


DWORD GetProcessName( DWORD processID, __inout UCHAR *Buffer )
{
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	DWORD size = 0;

    // Get a handle to the process.

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, processID );

    // Get the process name.

    if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
             &cbNeeded) )
        {
            GetModuleBaseName( hProcess, hMod, szProcessName, 
                               sizeof(szProcessName)/sizeof(TCHAR) );
        }
    }


	size = WideCharToMultiByte(CP_UTF8, 
									0, 
									szProcessName, 
									-1, 
									(LPSTR)Buffer,
									1024, NULL, NULL);
	return size;

    CloseHandle( hProcess );
}