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
#include <stdio.h>

#ifndef WIN32

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#else
#include "stdafx.h"
#include <winsock2.h>
#endif

#ifndef close
#define close closesocket
#endif

#include "clients.h"
#include "clientlist.h"
#include "server.h"
#include <time.h>
#include <process.h>


#define NEW_MAX (1024*64)

Server::Server()
{
	uselog=false;
	client_port=0;
	client_sock=0;
	memset(&client_addr, 0, sizeof(client_addr));
	clients = new ClientList();
	cleanup = new ClientList();
	main_thread_done=false;
	//fprintf(stderr, "Size of Client: %i. Size of ClientList: %i\r\n", sizeof(Client), sizeof(ClientList));
	IncreaseMaxFileLimit();
	ev_client_accept=NULL;
	ev_client_cleanup = NULL;
	m_ev_base=NULL;
	read_handle = 0;
	write_handle = 0;
	stopsvc = NULL;
	m_ev_service = NULL;
	m_tv_service.tv_sec = 1;
	m_tv_service.tv_usec = 0;
	memset(ip_address, 0, 128);
	//mutexes[0] = CreateMutex(NULL, FALSE, _T("Mutex0"));
	//mutexes[1] = CreateMutex(NULL, FALSE, _T("Mutex1"));
	//mutexes[2] = CreateMutex(NULL, FALSE, _T("Mutex2"));
}

Server::~Server()
{
	if(uselog) if(logfile) fclose(logfile);
	Stop();
	if(ev_client_accept)
	{
		delete ev_client_accept;
		ev_client_accept=NULL;
	}
	if (cleanup) delete cleanup;
	if(clients) delete clients;
}

bool Server::Stop()
{
	main_thread_done=true;
	if (ev_client_accept)
	{
		event_del(ev_client_accept);
		delete ev_client_accept;
		ev_client_accept = NULL;
	}
	milisleep(100);
	if(client_sock>0)
	{
		closesocket(client_sock);
		client_sock=0;
	}
	for (int x = clients->Count(); x >= 0; --x)
	{
		Client *client = (*clients)[x];
		if (client)
		{
			client->Disconnect();
			clients->Remove(client);
			fprintf(stderr, "Server::Stop() Client [%s] Disconnected. %i client(s) remaining.\n", client->GetName(), clients->Count());
			cleanup->Add(client);
		}
	}
	if (ev_client_cleanup)
	{
		event_active(ev_client_cleanup, EV_TIMEOUT, 0);
		milisleep(100); // Allow any threads to exit...
		event_del(ev_client_cleanup);
	}
	return true;
}

bool Server::SetTCPPort(int port)
{
	client_port=port;
	client_port_set=true;
	return true;
}

bool Server::SetTCPAddr(char *addr)
{
	_snprintf(ip_address, 128, "%s", addr);
	ip_address[127] = 0;
	return true;
}

/*
bool Server::AuthClient(ClientList *client)
{
	return true;
}
*/
bool Server::Start()
{
	int ret=0;
	fprintf(stdout, "Server::Start()\n");
	if(client_port_set==true)
	{
		if(CreateSocket(client_sock, client_port))
		{
			ev_client_accept=new event;
			//ev_client_cleanup = new event;
			m_ev_service = new event;
			event_set(ev_client_accept, client_sock, EV_READ|EV_PERSIST, OnClientAccept, this);
			ev_client_cleanup=event_new(m_ev_base, -1, 0, OnCleanup, this);
			event_base_set(m_ev_base, ev_client_accept);
			event_add(ev_client_accept, NULL);
			event_set(m_ev_service, NULL, EV_TIMEOUT, OnServiceTimeout, this);
			event_base_set(m_ev_base, m_ev_service);
			event_add(m_ev_service, &m_tv_service);
			event_add(ev_client_cleanup, NULL);
			main_thread_done=false;
#ifndef WIN32
			pthread_create(&main_thread, NULL, this->MainThread, this);
			pthread_detach(main_thread);
#else
			//read_thread = CreateThread(NULL, 0, ReadThread, this, 0, &read_thread_id);
			//write_thread = CreateThread(NULL, 0, WriteThread, this, 0, &write_thread_id);
			read_thread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)&ReadThread, this, 0, &read_thread_id);
			//process_thread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)&ProcessThread, this, 0, &process_thread_id);
			//write_thread = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)&WriteThread, this, 0, &write_thread_id);
#endif
			fprintf(stdout, "Server::Start() Started Successfully.\n");
			return true;
		}
	}
	fprintf(stdout, "Server::Start() Failed.\n");
	return false;
}

bool Server::CreateSocket(int &sock, int port)
{
	int bytes_received=0;
	struct sockaddr_in tcp_addr;
	unsigned int tcplen=sizeof(tcp_addr);
	int ret=0;

	fprintf(stdout, "Server::CreateSocket Creating socket on %s:%i...\n", ip_address[0] ? ip_address : "0.0.0.0", port);
	memset ((void *)&tcp_addr,0,tcplen);
	sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0)
	{
		fprintf(stderr, "Error opening a socket. ERRNO=0x%08x\r\n",errno);
		return false;
	}
	tcp_addr.sin_family=AF_INET;
	if (!ip_address[0])
		tcp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		tcp_addr.sin_addr.s_addr = inet_addr(ip_address);
	tcp_addr.sin_port=htons(port);
	int option=1;
#ifndef WIN32
	ret=setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&option, sizeof(option));
#else
	ret=setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&option, sizeof(option));
#endif
	if(ret < 0)
	{
		fprintf(stderr, "Error setting socket option SO_REUSEADDR. ERRNO=0x%08x\r\n", errno);
		close(sock);
		return false;
	}
	ret=bind(sock, ( struct sockaddr * ) &tcp_addr, tcplen);
	if ( ret < 0 )
	{
		fprintf(stderr, "Error binding to requested socket. ERRNO=0x%08x\r\n", errno);
		close(sock);
		return false;
	}
	ret=listen(sock, 1000);
	if(ret < 0)
	{
		fprintf(stderr, "Error listening on requested socket. ERRNO=0x%08x\r\n", errno);
		close(sock);
		return false;
	}
	if(!SetNonBlock(sock))
	{
		fprintf(stderr, "Error setting O_NONBLOCK on socket. ERRNO=0x%08x\r\n", errno);
		close(sock);
		return false;
	}
	return true;
}

void Server::ProcessPendingReads()
{
	bool somethingtodo = false;
	bool trigger_cleanup = false;
	//WaitForSingleObject(read_handle, 100);
	if (!clients)
	{
		milisleep(100);
		return;
	}
	//clients->Lock();
	for (int x = clients->Count(); x >= 0; --x)
	{
		Client *client = (*clients)[x];
		if (client)
		{
			if (client->DeleteMe())
			{
				client->Disconnect();
				clients->Remove(client);
				fprintf(stderr, "Server::ProcessPendingReads() Client [%s] Disconnected. %i client(s) remaining.\n", client->GetName(), clients->Count());
				char msg[MAX_BUF] = { 0 };
				snprintf(msg, MAX_BUF, "\tNBQUIT=%s\n", client->GetName());
				//client, "\tNBQUIT=%s\n", false);
				SendToAll(msg, client);
				cleanup->Add(client);
				trigger_cleanup = true;
			}
			else
				client->ProcessReads();
		}
	}
	//clients->Unlock();
	if (trigger_cleanup)
		event_active(ev_client_cleanup, EV_TIMEOUT, 0);
	if(!somethingtodo)milisleep(10);
}

void Server::ProcessReceiveBuffers()
{
	for (list<Client *>::iterator it = clients->clientlist.begin(); it != clients->clientlist.end(); ++it)
	{
		Client *client = *it;
		if(client)
			if (client->rbuf->ReadAvailable() > 0)
			{
				client->ProcessReadPacketBuffer();
			}
	}
}

void Server::ProcessPendingWrites()
{
	bool somethingtodo = false;
	bool trigger_cleanup = false;
	//WaitForSingleObject(write_handle, 100);
	if (!clients)
	{
		milisleep(100);
		return;
	}
	//clients->Lock();
	for (int x = clients->Count(); x >= 0; --x)
	{
		Client *client = (*clients)[x];
		if (client)
		{
			if (client->DeleteMe())
			{
				clients->Remove(client);
				//remove->Add(client);
				client->Disconnect();
				fprintf(stderr, "Client::ProcessPendingWrites() Client [%s] Disconnected. %i client(s) remaining.\n", client->GetName(), clients->Count());
				char msg[MAX_BUF] = { 0 };
				snprintf(msg, MAX_BUF, "\tNBQUIT=%s\n", client->GetName());
				//client, "\tNBQUIT=%s\n", false);
				SendToAll(msg, client);
				cleanup->Add(client);
				trigger_cleanup = true;
			}
			else
				client->ProcessWrites();
		}
	}
	//clients->Unlock();
	if (trigger_cleanup)
		event_active(ev_client_cleanup, EV_TIMEOUT, 0);
	if (!somethingtodo)milisleep(10);
}

void Server::SendToAll(const char *msg, Client *source)
{
	if (!clients) return;
	clients->Lock();
	for (int x = 0; x < clients->Count(); x++)
	{
		Client *client = (*clients)[x];
		if(!((source)&&(client==source)))
			client->SendBCMessage(msg);
	}
	clients->Unlock();
}

void Server::SendTo(const char * msg, const char * dest)
{
	if (!clients) return;
	clients->Lock();
	for (int x = 0; x < clients->Count(); x++)
	{
		Client *client = (*clients)[x];
		if((_strnicmp(client->GetName(), dest, strlen(dest))==0)&&(strlen(dest)==strlen(client->GetName())))
			client->SendBCMessage(msg);
	}
	clients->Unlock();
}

void Server::SendMSGAll(Client * source, const char * msg, bool cmd, bool silent)
{
	if (!clients) return;
	clients->Lock();
	for (int x = 0; x < clients->Count(); x++)
	{
		Client *client = (*clients)[x];
		if ((client) && (client != source))
		{
			if (cmd)
			{
				static char message[MAX_BUF] = { 0 };
				memset(message, 0, MAX_BUF);
				//snprintf(message, MAX_BUF, "\tMSGALL\n<%s> %s %s", source->GetName(), client->GetName(), msg);
				if(silent)
					snprintf(message, MAX_BUF, "<!%s> %s %s", source->GetName(), client->GetName(), msg);
				else
					snprintf(message, MAX_BUF, "<%s> %s %s", source->GetName(), client->GetName(), msg);
				client->SendBCMessage(message);
			}
			else
				client->SendBCMessage(msg);
		}
	}
	clients->Unlock();
}

void Server::SendNames(Client * source, const char * msg)
{
	if (!source->IsAuthenticated()) return;
	static char message[MAX_BUF] = { 0 };
	memset(message, 0, MAX_BUF);
	char *pos = message;
	char *pnamelist = message;
	int space = 0;
	clients->Lock();
	pos += snprintf(message, MAX_BUF, "-- Names:");
	pnamelist = pos;
	//for (int x = 0; x < clients->Count(); x++)
	for (list<Client *>::iterator it = clients->clientlist.begin(); it != clients->clientlist.end(); ++it)
	{
		Client *client = *it;
		if (client->IsAuthenticated())
		{
			int l = strlen(client->GetName()) + 1;
			if (space++) *pos++ = ' ';
			snprintf(pos, MAX_BUF - (pos - message), " %s", client->GetName());
			pos += strlen(pos);
		}
	}
	snprintf(pos, MAX_BUF - (pos - message), ".\n");
	/*
	for (list<Client *>::iterator it = clients->clientlist.begin(); it != clients->clientlist.end(); ++it)
	{
		Client *client = *it;
		if (client->IsAuthenticated())
		{
			client->wbuf->Write(message, strlen(message) + 1);
		}
	}
	clients->Unlock();
	*/
	source->wbuf->Write(message, strlen(message));
	fprintf(stderr, "%s Requested Names:%s", source->GetName(), pnamelist);
}

void Server::SendNetbotMessage(Client *source, const char *msg)
{
	static char message[MAX_BUF] = { 0 };
	memset(message, 0, MAX_BUF);
	_snprintf(message, MAX_BUF, "\tNBPKT:%s:%s", source->GetName(), msg);
	SendToAll(message, source);
}

void Server::SendNetbotNames(Client * source, const char * msg)
{
	if (!source->IsAuthenticated()) return;
	static char message[MAX_BUF] = { 0 };
	memset(message, 0, MAX_BUF);
	char *pos = message;
	char *pnamelist = message;
	clients->Lock();
	pos += snprintf(message, MAX_BUF, "\tNBCLIENTLIST=");
	pnamelist = pos;
	//for (int x = 0; x < clients->Count(); x++)
	int space = 0;
	for (list<Client *>::iterator it = clients->clientlist.begin(); it != clients->clientlist.end(); ++it)
	{
		Client *client = *it;
		if (client->IsAuthenticated())
		{
			int l = strlen(client->GetName()) + 1;
			if(space++) *pos++ = ' ';
			snprintf(pos, MAX_BUF - (pos - message), "%s", client->GetName());
			pos += strlen(pos);
		}
	}
	snprintf(pos, MAX_BUF - (pos - message), "\n");
	/*
	for (list<Client *>::iterator it = clients->clientlist.begin(); it != clients->clientlist.end(); ++it)
	{
	Client *client = *it;
	if (client->IsAuthenticated())
	{
	client->wbuf->Write(message, strlen(message) + 1);
	}
	}
	clients->Unlock();
	*/
	source->wbuf->Write(message, strlen(message));
	fprintf(stderr, "%s Requested Names:%s", source->GetName(), pnamelist);
}

void Server::SendTell(Client * source, const char * msg, bool bci, bool silent)
{
	if (!source->IsAuthenticated()) return;
	static char message[MAX_BUF];
	memset(message, 0, MAX_BUF);
	char name[128] = { 0 };
	int ret=sscanf_s(msg, "%s", name, 128);
	if (ret != 1) return;
	for (list<Client *>::iterator it = clients->clientlist.begin(); it != clients->clientlist.end(); ++it)
	{
		Client *client = *it;
		if (client->IsAuthenticated())
		{
			if ((_strnicmp(name, client->GetName(), strlen(name)) == 0) && (strlen(name) == strlen(client->GetName())))
			{
				if(silent)
					snprintf(message, MAX_BUF, "%c!%s%c %s", bci ? '{' : '[', source->GetName(), bci ? '}' : ']', msg + strlen(name) + 1);
				else
					snprintf(message, MAX_BUF, "%c%s%c %s", bci ? '{' : '[', source->GetName(), bci ? '}' : ']', msg+strlen(name) + 1);
				client->wbuf->Write(message, strlen(message));
				return;
			}
		}
	}
	ret = 0;
	// Handle Channels
	for (list<Client *>::iterator it = clients->clientlist.begin(); it != clients->clientlist.end(); ++it)
	{
		Client *client = *it;
		if (client->IsAuthenticated())
		{
			for (vector<string>::iterator it1 = client->channels.begin(); it1 != client->channels.end(); ++it1)
			{
				if ((_strnicmp(name, (*it1).c_str(), strlen(name)) == 0) && (strlen(name) == strlen((*it1).c_str())))
				{
					if(silent)
						snprintf(message, MAX_BUF, "%c!%s%c %s", bci ? '{' : '[', source->GetName(), bci ? '}' : ']', msg + strlen(name) + 1);
					else
						snprintf(message, MAX_BUF, "%c%s%c %s", bci ? '{' : '[', source->GetName(), bci ? '}' : ']', msg + strlen(name) + 1);
					client->wbuf->Write(message, strlen(message));
					ret = 1;
				}
			}
		}
	}
	if(!ret)
		source->wbuf->Writef("-- %s: No such name.\n", name);
}

bool Server::SetLogFile(const char *filename)
{
	FILE *f = NULL;
	fopen_s(&f, filename, "w");
	if(f)
	{
		uselog=true;
		logfile=f;
		return true;
	}
	else
	{
		uselog=false;
		logfile=NULL;
	}
	return false;
}

int Server::GetNumClients()
{
	return clients->Count();
}

void Server::OnClientAccept(int fd, short event, void *arg)
{
	int the_addrlen=16;
	int the_sock=0;
	int ret=0;
	Server *server=(Server *)arg;
	while (the_sock != INVALID_SOCKET)
	{
		sockaddr_in the_addr;
		the_addrlen = 16;
		the_sock = accept(server->client_sock, (sockaddr *)&the_addr, (socklen_t *)&the_addrlen);
		if (the_sock > 0)
		{
			if (!server->SetNonBlock(the_sock))
			{
				fprintf(stderr, "Error setting O_NONBLOCK on socket. ERRNO=0x%08x\r\n", errno);
				return;
			}
			else
			{
				Client *client = new Client(the_sock, server);
				fprintf(stdout, "Server::OnClientAccept() Client connected. %i clients connected.\r\n", server->clients->Count());
			}
		}
#ifndef WIN32
		else if (errno == EWOULDBLOCK)
#else
		else if (WSAGetLastError() == WSAEWOULDBLOCK)
#endif
		{
			WSASetLastError(0);
			//fprintf(stdout, "Server::OnClientAccept() EWOULDBLOCK Error occurred. This should not happen...\r\n");
		}
		else
		{
			fprintf(stdout, "Server::OnClientAccept() Socket error on accept...\r\n");
		}
	}
/*
	else if((ret==EINTR)||(ret==ENOTCONN))
	{
		event_del(&server->ev_client_accept);
	}
*/
}

void Server::OnCleanup(int fd, short event, void * arg)
{
	Server *server = (Server *)arg;
	if (!server) return;
	for (int x = server->cleanup->Count()-1; x >= 0; x--)
	{
		Client *client = (Client *)((*server->cleanup)[x]);
		client->CleanUp();
		//server->clients->Remove(client);
		server->cleanup->Remove(client);
		delete client;
	}
}

void Server::RenewTimeout()
{
	if (m_ev_service != NULL)
	{
		m_tv_service.tv_sec = 1;
		m_tv_service.tv_usec = 0;
		event_add(m_ev_service, &m_tv_service);
	}
}

void Server::UpdateServiceStatus()
{
	ReportStatusToSCMgr(SERVICE_RUNNING, NO_ERROR, 3000);
}

void Server::OnServiceTimeout(int fd, short event, void * arg)
{
	Server *server = (Server *)arg;
	if (!server)
		return;
	server->UpdateServiceStatus();
	server->RenewTimeout();
}

#ifndef WIN32
void *Server::MainThread(void *arg)
{
	fprintf(stderr, "Server::MainThread() Starting.\r\n");
	if(arg!=NULL)
	{
		Server *server=(Server *)arg;
		while(!server->main_thread_done)
		{
			server->ProcessPendingJobs();
		}
	}
	fprintf(stderr, "Server::MainThread() Ending.\r\n");
}
#else
//DWORD __stdcall Server::ReadThread(LPVOID arg)
unsigned __stdcall Server::ReadThread(void *arg)
{
	fprintf(stderr, "Server::ReadThread() Starting.\r\n");
	if (arg != NULL)
	{
		Server *server = (Server *)arg;
		while (!server->main_thread_done)
		{
			//WaitForSingleObject(server->mutexes[0], INFINITE);
			server->ProcessPendingReads();
			//ReleaseMutex(server->mutexes[0]);
			server->ProcessReceiveBuffers();
			server->ProcessPendingWrites();
			server->OnCleanup(0, 0, arg);
		}
	}
	fprintf(stderr, "Server::ReadThread() Ending.\r\n");
	return 0;
}
unsigned __stdcall Server::ProcessThread(void * arg)
{
	fprintf(stderr, "Server::ProcessThread() Starting.\r\n");
	if (arg != NULL)
	{
		Server *server = (Server *)arg;
		while (!server->main_thread_done)
			//milisleep(1000);
		{
			WaitForSingleObject(server->mutexes[1], INFINITE);
			server->ProcessReceiveBuffers();
			ReleaseMutex(server->mutexes[1]);
		}
	}
	fprintf(stderr, "Server::ProcessThread() Ending.\r\n");
	return 0;
}
#endif

//DWORD __stdcall Server::WriteThread(LPVOID arg)
unsigned __stdcall Server::WriteThread(void *arg)
{
	fprintf(stderr, "Server::WriteThread() Starting.\r\n");
	if (arg != NULL)
	{
		Server *server = (Server *)arg;
		while (!server->main_thread_done)
			//milisleep(1000);
		{
			WaitForSingleObject(server->mutexes[2], INFINITE);
			server->ProcessPendingWrites();
			ReleaseMutex(server->mutexes[2]);
		}
	}
	fprintf(stderr, "Server::WriteThread() Ending.\r\n");
	return 0;
}

unsigned __stdcall Server::CleanupThread(void * arg)
{
	fprintf(stderr, "Server::CleanupThread() Starting.\r\n");
	if (arg != NULL)
	{
		Server *server = (Server *)arg;
		while (!server->main_thread_done)
		{
			milisleep(1000);
			WaitForMultipleObjects(3, server->mutexes, TRUE, INFINITE);
			server->OnCleanup(0, 0, arg);
			ReleaseMutex(server->mutexes[0]);
			ReleaseMutex(server->mutexes[1]);
			ReleaseMutex(server->mutexes[2]);
		}
	}
	fprintf(stderr, "Server::CleanupThread() Ending.\r\n");
	return 0;
}

bool Server::CreateUDPSock(int &sock)
{
	sock=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock!=-1)
	{
		return true;
	}
	return false;
}

bool Server::SetReuseAddr(int &sock)
{
	int tmp = 1;
	if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&tmp, sizeof(tmp)) == -1)
		return false;
	else
		return true;
}

bool Server::SetNonBlock(int &sock)
{
#ifndef WIN32
	if(fcntl(sock, F_SETFL, O_NONBLOCK)!=0)
#else
	unsigned long nonblock=1;
	if(ioctlsocket(sock, FIONBIO, &nonblock)!=0)
#endif
		return false;
	else
		return true;
}

bool Server::SetBroadcast(int &sock)
{
	int tmp = 1;
	if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&tmp, sizeof(tmp)) == -1)
		return false;
	else
		return true;
}

bool Server::BindSendSocket(int &sock, char *int_name)
{
#ifndef WIN32
	if (setsockopt (sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)(int_name), strlen(int_name)) < 0)
	{
		printf("Server::BindSendSocket(): Error setting SO_BINDTODEVICE on interface: %s.\n", int_name);
		return false;
	}
#endif
	return true;
}

bool Server::IncreaseMaxFileLimit()
{
#ifndef WIN32
	int ret=0;
	struct rlimit rl;

	ret = getrlimit(RLIMIT_NOFILE, &rl);
	if(ret != 0)
	{
		fprintf(stderr, "Unable to read open file limit.\r\n(getrlimit(RLIMIT_NOFILE, &rl) failed)\r\n(%d, %s)\r\n", errno, strerror(errno));
		return false;
	}

	fprintf(stderr, "Limit was %d (max %d), setting to %d\r\n", rl.rlim_cur, rl.rlim_max, NEW_MAX);

	rl.rlim_cur = rl.rlim_max = NEW_MAX;
	ret = setrlimit(RLIMIT_NOFILE, &rl);
	if(ret != 0) {
		fprintf(stderr, "Unable to set open file limit.\r\n(setrlimit(RLIMIT_NOFILE, &rl) failed)\r\n(%d, %s)\r\n", errno, strerror(errno));
		return false;
	}

	ret = getrlimit(RLIMIT_NOFILE, &rl);
	if(ret != 0) {
		fprintf(stderr, "Unable to read new open file limit.\r\n(getrlimit(RLIMIT_NOFILE, &rl) failed)\r\n(%d, %s)\r\n", errno, strerror(errno));
		return false;
	}
	if(rl.rlim_cur < NEW_MAX) {
		fprintf(stderr, "Failed to set new open file limit.\r\nLimit is %d, expected %d\r\n", rl.rlim_cur, NEW_MAX);
		return false;
	}
#endif
	return true;
}
