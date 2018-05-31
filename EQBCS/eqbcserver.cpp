/***************************************************************************
*   Copyright (C) 2007 by Suede Worthey   *
*   suedeworthey@gmail.com   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#ifndef WIN32
#include <pthread.h>
#include "WSSockets.h"
#define TCHAR char
#else
#include "stdafx.h"
#include <winsock2.h>
#include "eqbcs_win.h"
#endif

#include "server.h"

//#define _USE_YASSL_

//#include "ssl.h"
#include <event.h>
#include <event2/thread.h>

#ifndef _strnicmp
//#define _strnicmp strncasecmp
#endif

#include "log.h"

TCHAR configfile[1024];
bool got_config = false;
Server server;
event_base *base = NULL;
//Log log;

void ProcessArgs(int argc, TCHAR *argv[]);
bool LoadConfig(const char *configpath);
//void SSL_Init(SSL_CTX *&ctx, SSL_METHOD *&meth);
void OnSignal(int fd, short event, void *arg);
bool InitWinSock();
BOOL CtrlHandler(DWORD fdwCtrlType);
bool gDebug = false;

//*
#ifndef WIN32
int main(int argc, char *argv[])
#else
int svc_main(int argc, LPTSTR *argv, bool &stop, int port, LPTSTR ipaddr, bool debug)
#endif
{
	char addr[64] = { 0 };
	gDebug = debug;
	//event ev_signal;
	//event *ev_sstimer;
	event_enable_debug_mode();
	evthread_use_windows_threads();
	evthread_enable_lock_debugging();
//	struct timeval timer;
	memset(configfile, 0, 1024);
	server.stopsvc = &stop;
#ifdef WIN32
	if (!InitWinSock())
	{
		fprintf(stderr, "Failed to initialize Windows Sockets...\n");
		return -1;
	}
#endif
	ProcessArgs(argc, argv);
	if (!got_config) _stprintf_s(configfile, ini_path);
	if (1)//(LoadConfig(configfile))
	{
		event_config *config = event_config_new();
		if (event_config_set_flag(config, EVENT_BASE_FLAG_STARTUP_IOCP) == 0)
		{
			fprintf(stderr, "Successfully set up IOCP...\n");
		}
		else
		{
			fprintf(stderr, "Failed to set up IOCP...\n");
		}
		base = event_base_new_with_config(config);

		server.SetEventBase(base);
		server.SetTCPPort(port);
		if(sizeof(char)!=sizeof(TCHAR))
			_snprintf(addr, 64, "%S", ipaddr);
		server.SetTCPAddr(addr);
		if (server.Start())
		{
			bool done = false;
			int clients = 0;
			SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
			event_base_dispatch(base);
			server.Stop();
		}
	}
	else
	{
		fprintf(stderr, "main() Unable to load configuration file.\r\n");
		return -1;
	}
	milisleep(100); // Allow other threads to exit...
	return 0;
}

#ifdef _DEBUG
void RunTests()
{
	static char cbuf[MAX_BUF] = { 0 };
	RingBuffer buf;
	int i = 0;
	for (int y = 0; y < 100; y++)
	{
		while (!buf.isFull())
		{
			buf.Writef("This is a test line %i...\n", i++);
		}
		for (int x = 0; x < 100; x++)
		{
			int maxlen = MAX_BUF;
			memset(cbuf, 0, MAX_BUF);
			if (buf.LineAvailable())
			{
				if (buf.ReadLine(cbuf, maxlen))
					fprintf(stderr, "%s", cbuf);
			}
		}
	}
	while (!buf.isEmpty())
	{
		int maxlen = MAX_BUF;
		if (buf.LineAvailable())
		{
			if (buf.ReadLine(cbuf, maxlen))
				fprintf(stderr, "%s", cbuf);
		}
		else
			break;
	}
}
#endif

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		fprintf(stderr, "Ctrl+C signal received... Shutting down server.\n");
		server.Stop();
		event_base_loopbreak(base);
		Sleep(100);
		return(TRUE);

		// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		fprintf(stderr, "Ctrl+Close signal received... Shutting down server.\n");
		server.Stop();
		event_base_loopbreak(base);
		Sleep(100);
		return(TRUE);

		// Pass other signals to the next handler. (Not any more!)
	case CTRL_BREAK_EVENT:
		fprintf(stderr, "Ctrl+Break signal received... Shutting down server.\n");
		server.Stop();
		event_base_loopbreak(base);
		Sleep(100);
		return TRUE;

	case CTRL_LOGOFF_EVENT:
		fprintf(stderr, "Ctrl-Logoff signal received... Shutting down server.\n");
		server.Stop();
		event_base_loopbreak(base);
		Sleep(100);
		return TRUE;

	case CTRL_SHUTDOWN_EVENT:
		fprintf(stderr, "Ctrl-Shutdown signal received... Shutting down server.\n");
		server.Stop();
		event_base_loopbreak(base);
		Sleep(100);
		return TRUE;

	default:
		return FALSE;
	}
}

#ifdef WIN32
bool InitWinSock()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
		return false;
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		return false;
	}
	return true;
}
#endif

void OnSignal(int fd, short event, void *arg)
{
	struct timeval when;
	when.tv_sec = 0;
	when.tv_usec = 0;
	fprintf(stderr, "OnSignal() called.\r\n");
	if (arg != NULL)
		event_base_loopexit((event_base *)arg, &when);
	else
		exit(1234);
}

void ProcessArgs(int argc, TCHAR *argv[])
{
	int x = 0;
	int len = 0;
	char *tmp = NULL;
	if (argc>1)
	{
		for (x = 1; x<argc; x++)
		{
			if (_tcsnicmp(argv[x], _T("--config="), 9) == 0)
			{
				TCHAR *tmp = argv[x] + 9;
				len = _tcslen(tmp);
				if (len>1023) len = 1023;
				_tcsncpy_s(configfile, tmp, len);
				got_config = true;
			}
		}
	}
}

bool LoadConfig(const char *configpath)
{
	bool ret = false;
	FILE *f;
	fopen_s(&f, configpath, "rt");
	char buf[4096];
	if (f)
	{
		while (!feof(f))
		{
			memset(buf, 0, 4096);
			fgets((char *)buf, 4095, f);
		}
		ret = true;
		fclose(f);
	}
	else
		return false;
	return ret;
}