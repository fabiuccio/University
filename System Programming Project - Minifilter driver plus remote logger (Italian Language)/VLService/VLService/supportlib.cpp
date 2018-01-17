#include "supportlib.h"
#include "stdafx.h"
#include <winsock2.h> // L'header corretto per la versione 2.0 della DLL WSAStartup 
#include <ws2tcpip.h> // ip_mreq needed
#include <iostream>
#include <fstream>

using namespace std;
bool dinamicallyLoad(HINSTANCE &hDLL, LPWSTR name){
	/*
		Linking Explicitly
		With explicit linking, applications must make a function call to explicitly load the DLL at run time. 
		To explicitly link to a DLL, an application must: 
			Call LoadLibrary (or a similar function) to load the DLL and obtain a module handle. 
			Call GetProcAddress to obtain a function pointer to each exported function that the application wants to call. Because applications are calling the DLL's functions through a pointer, the compiler does not generate external references, so there is no need to link with an import library. 
			Call FreeLibrary when done with the DLL. 
	*/
	
	/*
		WINDOWS.H defines a macro called TEXT() that will mark string literals as Unicode, depending on whether the UNICODE compile flag is set.
		#ifdef UNICODE 
		#define TEXT(string) L#string 
		#else 
		#define TEXT(string) string 
		#endif // UNICODE 

		please don't use TEXT unless absolutely necessary, 
		use the short version _T("whatever") and for unicode you can use L"whatever"
		just because _T("hi") takes up less space.
		if you don't need to support windows9x or win9x users without MSLU just use L"hi" for unicode, only.
	*/

	hDLL = LoadLibrary(name);
	
	if (hDLL == NULL)
	{
		cerr<<"ERROR (supportlib): Unable to find and/or dynamically link WS2_32.DLL..."<<endl;
		return false;
	}

	#ifdef DEBUG
	cout<<"DEBUG (supportlib): Correctly found and dynamically linked WS2_32.DLL..."<<endl;
	#endif

	return true;
}


bool callConnect(HINSTANCE &hDLL, HANDLE& port){
	
	typedef HANDLE (CALLBACK* LPFNDLLFUNC1)(VOID);
	LPFNDLLFUNC1 lpfnDllFunc1;    // Function pointer
	
	lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(hDLL,  "Connect");
	
	if (!lpfnDllFunc1)
	{
		cerr<<"ERROR!! (supportlib): Function 'Connect' in 'minispy.dll' not found."<<endl;
		return false;
	}

	#ifdef DEBUG
	cout<<"DEBUG (supportlib): Correctly found WSAStartup."<<endl;
	#endif 
 
	port = lpfnDllFunc1();
	if (port==0){
		cerr<<"ERROR (supportlib): Call to 'Connect' returned 0, DLL couldn't connect to minifilter."<<endl;
		return false;
	}else
		#ifdef DEBUG
		cout<<"DEBUG (supportlib): Call to 'Connect' correctly got the port from minifilter ("<<port<<")"<<endl;
		#endif
	return true;
}

BOOL callAttachDevice(HINSTANCE& hDLL, LPCSTR AttachName){
	typedef BOOL (CALLBACK* LPFNDLLCLEANUP)(LPCSTR);
	LPFNDLLCLEANUP lpfnDllCleanup1;   

	lpfnDllCleanup1 = (LPFNDLLCLEANUP)GetProcAddress(hDLL,  "AttachDevice");
	if (!lpfnDllCleanup1)
	{
		cerr<<"ERROR (supportlib): Function 'AttachDevice' in 'minispy.dll' not found."<<endl;
		return FALSE;
	}

	#ifdef DEBUG
	cout<<"DEBUG (supportlib): Correctly found 'AttachDevice' function in 'minispy.dll'..."<<endl;
	#endif
	
			
	return lpfnDllCleanup1(AttachName);
}


BOOL callDetachDevice(HINSTANCE& hDLL, LPCSTR DetachName){
	typedef BOOL (CALLBACK* LPFNDLLCLEANUP)(LPCSTR);
	LPFNDLLCLEANUP lpfnDllCleanup1;   

	lpfnDllCleanup1 = (LPFNDLLCLEANUP)GetProcAddress(hDLL,  "DetachDevice");
	if (!lpfnDllCleanup1)
	{
		cerr<<"ERROR (supportlib): Function 'DetachDevice' in 'minispy.dll' not found!"<<endl;
		return FALSE;
	}

	#ifdef DEBUG
	cout<<"DEBUG (supportlib): Correctly found 'DetachDevice' function in 'minispy.dll'..."<<endl;
	#endif
	return lpfnDllCleanup1(DetachName);
}


void callListDecives(HINSTANCE& hDLL, UCHAR* deviceList){
	typedef void (CALLBACK* LPFNDLLCLEANUP)(UCHAR*);
	LPFNDLLCLEANUP lpfnDllCleanup1;   

	lpfnDllCleanup1 = (LPFNDLLCLEANUP)GetProcAddress(hDLL,  "ListDevices");
	if (!lpfnDllCleanup1)
	{
		cerr<<"ERROR (supportlib): Function 'ListDevices' in 'minispy.dll' not found!"<<endl;
	}

	#ifdef DEBUG
	cout<<"DEBUG (supportlib): Correctly found 'ListDevices' function in 'minispy.dll'..."<<endl;
	#endif
	lpfnDllCleanup1(deviceList);
}


bool callGetPacket(HINSTANCE& hDLL, HANDLE port, UCHAR* buffer){
	typedef VOID (CALLBACK* LPFNDLLCLEANUP)(LPVOID, UCHAR*);
	LPFNDLLCLEANUP lpfnDllCleanup1;   

	lpfnDllCleanup1 = (LPFNDLLCLEANUP)GetProcAddress(hDLL,  "GetPacket");
	if (!lpfnDllCleanup1)
	{
		cerr<<"ERROR (supportlib): Function 'GetPacket' in 'minispy.dll' not found!"<<endl;
		return false;
	}
	#ifdef DEBUG
	cout<<"DEBUG (supportlib): Correctly found 'GetPacket' function in 'minispy.dll'..."<<endl;
	#endif
	
	lpfnDllCleanup1(port, buffer);
	return true;
}

bool writeOnFile(const char* path, const char* message){
	ofstream out(path,ofstream.app); 
	if(!out) { 
		cerr << "Cannot open file: "<<path<<".\n"; 
		return false; 
	} 
 
	out << message << endl; 
	out.close(); 
	return true;
}