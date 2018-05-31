// AR_ScreenSaver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "server.h"
#include "eqbcs_win.h"
#include "service.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char user[128];
bool stop=false;
TCHAR ini_path[2048] = { 0 };

extern Server server;

VOID ServiceStart (DWORD dwArgc, LPTSTR *lpszArgv)
{
	Send_Start_Pending();
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	HRESULT hr=0;
	hr=CoInitializeEx(NULL, COINIT_MULTITHREADED);
	stop=false;
	Sleep(1000);
	Send_Start_Pending();
	Sleep(1000);
	if(!hr)
	{
		int port = 0;
		TCHAR ipaddr[2048] = _T("0.0.0.0");
	 	wVersionRequested = MAKEWORD( 2, 2 );
		err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 )
		{
			ServiceStop();
			return;
		}
		Send_Service_Running();
		Sleep(1000);
		Send_Service_Running();
		Sleep(1000);
		port = GetPrivateProfileInt(_T("EQBCS"), _T("Port"), 2112, ini_path);
		DWORD ret = GetPrivateProfileString(_T("EQBCS"), _T("Address"), _T("0.0.0.0"), (LPWSTR)&ipaddr, 2048, ini_path);
		svc_main((int)dwArgc, lpszArgv, stop, port, ipaddr, false);
		ServiceStop();
		Sleep(1000);
		ServiceStop();
		Sleep(1000);
	}
}

int Send_Start_Pending()
{
	return ReportStatusToSCMgr(SERVICE_START_PENDING, NO_ERROR, 3000);
}

int Send_Service_Running()
{
	return ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 3000);
}

VOID ServiceStop()
{
	ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 3000);
	//stop=true;
	server.Stop();
}

bool GetRegSZValue(HKEY key, TCHAR *value_name, char *data, DWORD &len)
{
	DWORD type=0;
	DWORD size=128;
	int res=0;
	for (int x=0; x<128; x++) data[x]=0;
	res=RegQueryValueEx(key, value_name, 0, &type, (BYTE *)data, &len);
	if(res==ERROR_SUCCESS)
		return true;
	return false;
}

DWORD GetDWORDValue(HKEY key, TCHAR *value_name)
{
	BYTE data[128];
	DWORD type=0;
	DWORD size=128;
	DWORD value=0;
	for (int x=0; x<128; x++) data[x]=0;
	RegQueryValueEx(key, value_name, 0, &type, (BYTE *)data, &size);
	value=((DWORD*)data)[0];
	return value;
}

bool IsUserLoggedIn()
{
	DWORD res=0;
	res=WTSGetActiveConsoleSessionId();
	if (res==0xFFFFFFFF)
	{
		for (int x=0; x<128; x++) user[x]=0;
		return false;
	}
	else return true;
}