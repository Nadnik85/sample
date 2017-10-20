#ifndef _SWCLIENT_WIN_H_
#define _SWCLIENT_WIN_H_
#include "stdafx.h"
//#include <Windows.h>
#include "stdio.h"
#include "service.h"
#include "WtsApi32.h"
#include "winbase.h"
#include "shlwapi.h"

extern TCHAR ini_path[2048];
extern char user[128];
extern bool stop;

struct ScreenSaverSettings_T
{
	int Enabled;
	int PasswordProtected;
	int Timeout;
	char Executable[1024];
};

int Send_Start_Pending();
int Send_Service_Running();
bool IsUserLoggedIn();
extern int svc_main(int argc, LPTSTR *argv, bool &stop, int port, TCHAR *addr);
#endif