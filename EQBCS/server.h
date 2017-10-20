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
#ifndef SERVER_H
#define SERVER_H

#ifndef WIN32
#include <sys/select.h>
#include <pthread.h>
#else
//#include <winsock2.h>
#endif

#include <event.h>
#include "clientlist.h"
//#include "persistenttimer.h"
#include "service.h"

#define MAX_FILE_XFER_THREADS 50

//class ClientList;

class Server
{
	private:
		// Client socket info
		int client_sock;					//Server Listening Socket
		struct sockaddr_in client_addr;	//Server Listening Address
		int client_port;				//Server Listening Port
		bool client_port_set;
		bool client_addr_set;

		bool uselog;
		FILE *logfile;
		char ip_address[128];

	public:
		bool *stopsvc;
		event_base *m_ev_base;
		bool main_thread_done;

		event *m_ev_service;
		timeval m_tv_service;

#ifndef WIN32
		pthread_t main_thread;
		static void *MainThread(void *arg);
#else
		//static DWORD __stdcall ReadThread(LPVOID arg);
		static unsigned __stdcall ReadThread(void * arg);
		HANDLE read_thread;
		unsigned int read_thread_id;
		HANDLE read_handle;

		//static DWORD __stdcall ProcessThread(LPVOID arg);
		static unsigned __stdcall ProcessThread(void * arg);
		HANDLE process_thread;
		unsigned int process_thread_id;
		HANDLE process_handle;

		//static DWORD __stdcall WriteThread(LPVOID arg);
		static unsigned __stdcall WriteThread(void * arg);
		HANDLE write_thread;
		unsigned int write_thread_id;
		HANDLE write_handle;
#endif
		ClientList *clients;
		ClientList *cleanup;

		Server();
		~Server();
		bool Stop();
		bool Start();

		bool SetLogFile(const char *filename);
		bool SetTCPPort(int port);
		bool SetTCPAddr(char *addr);

		int GetNumClients();

		//Functions for event-oriented i/o
		event *ev_client_accept;
		event *ev_client_cleanup;
		
		static void OnClientAccept(int fd, short event, void *arg);
		static void OnCleanup(int fd, short event, void *arg);
		void RenewTimeout();
		void UpdateServiceStatus();
		static void OnServiceTimeout(int fd, short event, void *arg);
		bool CreateSocket(int &sock, int port);
		void ProcessPendingReads();
		void ProcessReceiveBuffers();
		void ProcessPendingWrites();
		void SendToAll(const char * msg, Client *source=NULL);
		void SendTo(const char * msg, const char *dest);
		void SetEventBase(event_base *base) { m_ev_base=base; };
		void SendMSGAll(Client * source, const char * msg, bool cmd=false);
		void SendNames(Client *source, const char *msg);
		void SendNetbotMessage(Client * source, const char * msg);
		void SendNetbotNames(Client * source, const char * msg);
		void SendTell(Client *source, const char *msg, bool bci=false);

	public:
		bool BindSendSocket(int &sock, char *int_name);
		static bool SetBroadcast(int &sock);
		static bool SetNonBlock(int &sock);
		static bool SetNonBlocking(int &sock) { return SetNonBlock(sock); }
		static bool SetReuseAddr(int &sock);
		static bool CreateUDPSock(int &sock);
		bool IncreaseMaxFileLimit();
};

#endif
