#ifndef _MY_SUPPORT_LIB_ 
#define _MY_SUPPORT_LIB_

#define _WINSOCKAPI_  // Prevent winsock.h #include's, dal momento che uso winsock2.h

#include "stdafx.h"
#include <winsock2.h> // L'header corretto per la versione 2.0 della DLL WSAStartup 
#include <ws2tcpip.h> // ip_mreq needed

bool dinamicallyLoad(HINSTANCE &hDLL, LPWSTR name);
bool callConnect(HINSTANCE&, HANDLE&);
BOOL callAttachDevice(HINSTANCE&, LPCSTR);
void callListDecives(HINSTANCE&, UCHAR*);
BOOL callDetachDevice(HINSTANCE&, LPCSTR);
bool callGetPacket(HINSTANCE&, HANDLE, UCHAR*);
bool writeOnFile(const char*, const char*);
#endif