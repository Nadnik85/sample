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
#ifdef WIN32
#include "stdafx.h"
#endif

#include "clients.h"
#include "clientlist.h"
#include "server.h"
#include <time.h>

#ifdef WIN32
#define snprintf _snprintf
#endif

extern bool gDebug;

Client::Client()
{
	Init();
}

Client::Client(int tsock, Server *pserver)
{
	Init();
	ev_read_exists=false;
	if((tsock>0)&&(pserver!=NULL))
	{
		sock=tsock;
		server=pserver;

		SetEventBase(server->m_ev_base);
//		sslsock.PrepareAccept(sock);

		tv_auth_timeout.tv_sec = 10;
		tv_auth_timeout.tv_usec = 0;
		ev_auth_timeout = event_new(m_ev_base, -1, 0, OnAuthenticationTimeout, this);
		ev_auth_timeout_exists = true;

		//tv_ping_timeout.tv_sec=30;
		//tv_ping_timeout.tv_usec=0;
		//ev_ping_timeout=new event;
		//ev_ping_timeout = event_new(m_ev_base, -1, 0, OnPingTimeout, this);
		//ev_ping_timeout_exists = true;

		//timeout_set(ev_auth_timeout, OnAuthenticationTimeout, this);
		//timeout_set(ev_ping_timeout, OnPingTimeout, this);
		//event_base_set(m_ev_base, ev_auth_timeout);
		//event_base_set(m_ev_base, ev_ping_timeout);

		server->clients->Lock();
		server->clients->Add(this);
		server->clients->Unlock();

//		ev_read=new event;
		ev_read = event_new(m_ev_base, sock, EV_READ | EV_PERSIST, OnRead, this);
		ev_read_exists = true;
//		event_set(ev_read, sock, EV_READ|EV_PERSIST, OnRead, this);
//		event_base_set(m_ev_base, ev_read);

		event_add(ev_read, NULL);
		timeout_add(ev_auth_timeout, &tv_auth_timeout);
//		timeout_add(ev_ping_timeout, &tv_ping_timeout);
	}
}

void Client::Accept()
{
//	if(sslsock.Accept())
	{
		disconnected=false;
		accepted=true;
	}
}

void Client::OnAuthenticationTimeout(int fd, short event, void * arg)
{
	if (!arg) return;
	Client *client = (Client *)arg;
	if (!client->authenticated)
	{
		client->Disconnect();
		fprintf(stderr, "Client::OnAuthenticationTimeout() client failed to authenticate...\r\n");
	}
}

void Client::OnPingTimeout(int fd, short event, void * arg)
{
	if (!arg) return;
	Client *client = (Client *)arg;
	client->wbuf->Writef("\tPING\n");
}

void Client::Disconnect()
{
	/*
	if ((ev_read_exists) && (ev_read != NULL))
	{
		event_del_block(ev_read);
//		event_free(ev_read);
		//delete ev_read;
//		ev_read = NULL;
		ev_read_exists = false;
	}
	if (ev_auth_timeout_exists && (ev_auth_timeout != NULL))
	{
		event_del_block(ev_auth_timeout);
//		event_free(ev_auth_timeout);
		//delete ev_auth_timeout;
//		ev_auth_timeout = NULL;
		ev_auth_timeout_exists = false;
	}
	if (ev_ping_timeout_exists && (ev_ping_timeout != NULL))
	{
		event_del_block(ev_ping_timeout);
//		event_free(ev_ping_timeout);
		//delete ev_ping_timeout;
//		ev_ping_timeout = NULL;
		ev_ping_timeout_exists = false;
	}*/
	disconnected=true;
	deleteme = true;
}

void Client::CleanUp()
{
	if ((ev_read_exists) && (ev_read != NULL))
	{
		event_del_block(ev_read);
		event_free(ev_read);
		//delete ev_read;
		ev_read = NULL;
		ev_read_exists = false;
	}
	if (ev_auth_timeout_exists && (ev_auth_timeout != NULL))
	{
		event_del_block(ev_auth_timeout);
		event_free(ev_auth_timeout);
		//delete ev_auth_timeout;
		ev_auth_timeout = NULL;
		ev_auth_timeout_exists = false;
	}
//	if (ev_ping_timeout_exists && (ev_ping_timeout != NULL))
//	{
//		event_del_block(ev_ping_timeout);
//		event_free(ev_ping_timeout);
		//delete ev_ping_timeout;
//		ev_ping_timeout = NULL;
//		ev_ping_timeout_exists = false;
//	}
	closesocket(sock);
	WSASetLastError(0);
}

Client::~Client()
{
//	delete sslsock;
	Disconnect();
	delete rbuf;
	delete wbuf;
}

void Client::Init()
{
	accepted=false;
	authstart = false;
	auth_timeout=true;
	sock=0;
	addrlen=0;
	int x=0;
	wait_status=false;
	ready=false;
	server=NULL;
	memset(name,0,128);
	rbuf=new RingBuffer(MAX_PACKET);
	wbuf=new RingBuffer(MAX_PACKET);
	disconnected=true;
//	memset(packet_buf,0,MAX_PACKET*2);
//	sslsock=new SSLSocket();
	sockerr=0;
	m_ev_base=NULL;
	ev_auth_timeout = NULL;
//	ev_ping_timeout = NULL;
	ev_read = NULL;
	authenticated = false;
	status = STATUS_IDLE;
	deleteme = false;
	command = false;
	localecho = true;
//	ping_timeout = false;
	ev_auth_timeout_exists = false;
//	ev_ping_timeout_exists = false;
	last_ping_time = time(NULL);
	last_ping_response_time = time(NULL);
	ev_read_exists = false;
	channels.clear();
}

bool Client::Connected()
{
	int buf;
	int ret = 0;
#ifndef WIN32
	ret=recv(sock, &buf, 1, MSG_PEEK | MSG_DONTWAIT);
#else
/*	int error_code;
	int error_code_size = sizeof(error_code);
	getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&error_code, &error_code_size);
	return (error_code == 0) ? true : false;*/
	ret = recv(sock, (char *)&buf, 1, MSG_PEEK); // No longer works in winblows!
#endif
	switch(ret)
	{
		case -1: // error
		{
#ifndef WIN32
			if(errno!=EAGAIN)
				return false;
#else
			switch (WSAGetLastError())
			{
				case WSAEWOULDBLOCK: {
					WSASetLastError(0);
					return true;
				}break;
				default: return false;
			}
#endif
		}break;
		case 0: // disconnected
		{
			return false;
		}break;
		default:break;
	}
	return true;
}

int Client::SendPacket(packet_t *packet)
{
	if(!packet)
		return 0;
	if(!Connected())
	{
		fprintf(stderr, "Client::SendPacket() Client is no longer connected to server.\n");
		Disconnect();
		return 0;
	}

	int left = strlen((const char *)packet);
	int total=0;
	int bytes=0;
	int to_read=0;
	int wrote=0;
	do
	{
/*
		if(packet->header.len>MAX_PACKET) packet->header.len=MAX_PACKET; // prevent suicide...
		bytes=packet->header.len;
		packet->header.magic=PACKET_MAGIC;
*/
		//wrote=sslsock.Write((packet)+total, bytes);
		wrote = send(sock, (const char *)(packet+total), bytes, 0);
		if(wrote>0) left-=wrote;
		else if(wrote==0)
		{
			disconnected=true; // Write failed?
			return 0;
		}
		else
		{
			//Write Failed...
			return 0;
		}
	}
	while ((left>0) && (!disconnected));
	return 1;
}

void Client::SetReadyStatus(bool status)
{
	ready=status;
}

bool Client::IsReady()
{
	return ready;
}

bool Client::CheckConnection(char *CallingFunction)
{
	if (!Connected())
	{
		//fprintf(stderr, "%s Client is no longer connected to server.\n", CallingFunction);
		Disconnect();
		return false;
	}
	return true;
}

void Client::ProcessReads()
{
	if (CheckConnection("Client::ProcessReads()"))
	{
		if ((GetAvailableBytes() > 0) && (rbuf->WriteAvailable()))
		{
			if (ev_read)
				event_active(ev_read, EV_TIMEOUT, 0);
		}
		else if (!rbuf->WriteAvailable())
			fprintf(stderr, "Client::ProcessReads() Error: read buffer is full...\n");
	}
	else
	{
		//server->clients->Lock();
	}
}

/*
void Client::DeleteMe()
{
	Server *pserver = server;
	pserver->clients->Lock();
	Disconnect();
	pserver->clients->Remove(this);
	pserver->clients->Unlock();
}
*/
void Client::CheckPing()
{
	const static char *ping = "\tPING\n";
	time_t now = time(NULL);
	if ((last_ping_time + 50) < now)
	{
		wbuf->Write(ping, strlen(ping));
		last_ping_time = now;
	}
}

void Client::ProcessWrites()
{
	if (CheckConnection("Client::ProcessWrites()"))
	{
		CheckPing();
		if (wbuf->ReadAvailable() > 0)
			ProcessWritePacketBuffer();
	}
}

unsigned int Client::GetAvailableBytes()
{
	sockerr=0;
#ifndef WIN32
	unsigned int ret;
	if (ioctl(sock, FIONREAD, &ret) != -1)
#else
	unsigned long ret;
	if (ioctlsocket(sock, FIONREAD, &ret) != -1)
#endif
		return ret;
	else
	{
#ifndef WIN32
		sockerr=errno;
#else
		sockerr=WSAGetLastError();
#endif
		return 0;
	}
}

int Client::ReadPacket()
{
	unsigned char data[MAX_BUF];
	int avail=GetAvailableBytes();
	if(avail)
	do
	{
		int bytes=0;
		int tobuf=0;
		int buffered=0;

		memset(data,0,MAX_BUF);
		//bytes=sslsock.Read(data, MAX_PACKET);
		bytes = recv(sock, (char *)data, MAX_BUF, 0);
		if(bytes>0)
		{
			tobuf=0;
			do
			{
				buffered=rbuf->Write(data+tobuf, bytes);
				tobuf+=buffered;
				//ProcessReadPacketBuffer();
			}while(tobuf<bytes);
		}
		else if(bytes==0)
		{
			if(!Connected()) disconnected=true;
			//if(sslsock.sockerr) disconnected=true;
		}
		//if(sslsock.sockerr) disconnected=true;
		avail=GetAvailableBytes();
	}while((avail>0)&&(!disconnected));
	return 0;
}

void Client::OnRead(int fd, short event, void *arg)
{
	Client *client=(Client *)arg;
	if(client)
	{
		if(!client->accepted)
		{
			client->Accept();
			if(client->accepted)
			{
//				timeout_del(client->ev_ping_timeout);
//				client->ping_timeout=false;
//				client->ev_ping_timeout=NULL;
			}
		}
		else
		{
			client->ReadPacket();
			if(client->Disconnected())
			{
				client->Disconnect();
			}
		}
		//if(client->server->read_handle)
		//	SetEvent(client->server->read_handle);
		//if (client->server->write_handle)
		//	SetEvent(client->server->write_handle);
	}
}

void Client::ProcessWritePacketBuffer()
{
	if (wbuf->ReadAvailable()) // Anything in the buffer?
	{
		int read = 0;
		int wrote = 0;
		int total = 0;
		int avail = wbuf->ReadAvailable();
		avail = (avail > MAX_BUF) ? MAX_BUF : avail;
		packet_t packet = { 0 };
		char * ppacket = (char *)packet;
		read = wbuf->Read(&packet, avail);
		do
		{
			wrote = send(sock, (const char *)ppacket, read, 0);
			if (wrote==SOCKET_ERROR)
			{
				Disconnect();
				break;
			}
			total += wrote;
			ppacket += wrote;
		} while (total < read);
	}
}

void Client::HandleLogin(char *data)
{
	static char message[MAX_BUF] = { 0 };
	/*
	char *pc = data;
	char *pc2 = NULL;
	if (data[0] == '=')
		pc = data + 1;
	if ((pc2 = strchr(pc, ';')) != NULL)
	{
		*pc2 = 0;
		if ((*(server->clients))[pc] != NULL)
		{
			authenticated = false;
			SendPacket((packet_t *)"This user is already connected...\n");
			fprintf(stderr, "The user is already connected...\r\n");
			Disconnect();
			return;
		}
		{
			strncpy(name, pc, strlen(pc));
			authenticated = true;
			_snprintf(message, MAX_BUF, "\tNBJOIN=%s\n", name);
			server->SendToAll(message);
		}
	}//*/
	//*
	if (_strnicmp("LOGIN=", data, 6) == 0)
	{
		char *pc = data + 6;
		char *pc2 = NULL;
		if ((pc2 = strchr(pc, ';')) != NULL)
		{
			*pc2 = 0;
			if ((*(server->clients))[pc] != NULL)
			{
				authenticated = false;
				SendPacket((packet_t *)"This user is already connected...\n");
				fprintf(stderr, "The user is already connected...\r\n");
				Disconnect();
				return;
			}
			{
				static char clientlist[MAX_BUF] = { 0 };
				char *cptr = clientlist;
				memset(clientlist, 0, MAX_BUF);
				strncpy(name, pc, strlen(pc));
				authenticated = true;
				for (int x = 0; x < server->clients->Count(); x++)
				{
					_snprintf(cptr, (MAX_BUF - (cptr-clientlist)), "%s", (*(server->clients))[x]->GetName());
					cptr += strlen(cptr);
					if (x < server->clients->Count() - 1)
					{
						(*cptr) = ' ';
						cptr++;
					}
				}
				_snprintf(message, MAX_BUF, "\tNBJOIN=%s\n\tNBCLIENTLIST=%s\n", name, clientlist);
				//_snprintf(message, MAX_BUF, "\tNBJOIN=%s\n", name);
				server->SendToAll(message);
			}
		}
	}
/*
	else if ((!authstart) && (_strnicmp("LOGIN", data, 5) == 0) && (strlen(data)==5))
	{
		// Login portion broken apart (go "EQBC Interface")
		authstart = true;
	}
	else if (authstart)
	{
		char *pc = NULL;
		if ((pc = strchr(data, ';')) != NULL)
		{
			*pc = 0;
		}
	}
//*/
}

void Client::HandleCommands(char *data)
{
	char *pc = data+1;
	bool cmdtypefound = false;
	for (int x = 0; x < NUM_COMMANDS; x++)
	{
		size_t len = strlen(pc);
		if (len > strlen(Commands[x].Token))
			len = strlen(Commands[x].Token);
		if (_strnicmp(Commands[x].Token, pc, len) == 0)
		{
			commandtype = Commands[x].Type;
			return;
		}
	}
	commandtype = COMMAND_UNK;
}

void Client::HandleDisconnect(char * data)
{
}

void Client::HandleNames(char * data)
{
	fprintf(stderr, "Client::HandleNames() Called for [%s]...\n", name);
	server->SendNames(this, data);
}

void Client::HandleNetbotNames(char * data)
{
	fprintf(stderr, "Client::HandleNetbotNames() Called for [%s]...\n", name);
	server->SendNetbotNames(this, data);
}

void Client::HandleNetbotMessage(char * data)
{
	server->SendNetbotMessage(this, data);
}

void Client::HandleMSGAll(char * data)
{
	fprintf(stderr, "<%s>  [*ALL*] %s", name, data);
	char message[MAX_BUF] = { 0 };
	//snprintf(message, MAX_BUF, "\tMSGALL\n<%s> %s", name, data, true);
	server->SendMSGAll(this, data, true);
	//wbuf->Write(message, strlen(message));
}

void Client::HandleTell(char * data)
{
	//fprintf(stderr, "<%s> %s", name, data); // No Echo for tell?
	char message[MAX_BUF] = { 0 };
	server->SendTell(this, data);
}

void Client::HandleSMSGAll(char * data)
{
	//fprintf(stderr, "<%s>  [*ALL*] %s", name, data); //SILENCE! I KILL YOU!
	char message[MAX_BUF] = { 0 };
	//snprintf(message, MAX_BUF, "\tMSGALL\n<%s> %s", name, data, true);
	server->SendMSGAll(this, data, true, true);
	//wbuf->Write(message, strlen(message));
}

void Client::HandleSTell(char * data)
{
	//fprintf(stderr, "<%s> %s", name, data); // No Echo for tell?
	char message[MAX_BUF] = { 0 };
	server->SendTell(this, data, false, true);
}

void Client::HandlePong(char * data)
{
	// this wasn't used on the old EQBCS...
	last_ping_response_time = time(NULL);
//	timeout_set(ev_ping_timeout, OnPingTimeout, this);
//	event_base_set(m_ev_base, ev_ping_timeout);
//	timeout_add(ev_ping_timeout, &tv_ping_timeout);
//	ping_timeout = false;
}

void Client::HandleLocalEcho(char * data)
{
	int value = 0;
	int ret = 0;
	ret=sscanf(data, "\tLOCALECHO %d\n", &value);
	if (ret > 0)
	{
		localecho = (value > 0) ? true : false;
		wbuf->Writef("-- Local Echo: %s\n", localecho ? "ON" : "OFF");
	}
}

void Client::HandleBCI(char * data)
{
	char message[MAX_BUF];
	memset(message, 0, MAX_BUF);
	server->SendTell(this, data, true);
/*
	char _name[128] = { 0 };
	int count = sscanf(data, "%s %s", _name, message);
	if (count == 2)
	{
		fprintf(stderr, "Got NetBots Data/Request");
		if (strnicmp(name, _name, strlen(name) == 0) && (strlen(name) == strlen(_name)))
		{
			server->SendToAll(data, this);
		}
	}
	else
	{
		count = sscanf(data, "%s %s", _name, message);
		if (count == 2)
		{
			if((strnicmp(message, "REQ", 3)==0)&&(strlen(message)==3))
				server->SendTo(data, _name);
		}
	}
*/
}

void Client::HandleChannels(char * data)
{
	fprintf(stderr, "%s joined channels %s", name, (char *)data);
	char *msg = (char *)data;
	char *token=strtok(msg, " ");
	channels.clear();
	while (token!=NULL)
	{
		int len = strlen(token);
		if (token[len - 1] == '\n')
			token[len - 1] = 0;
		channels.push_back(token);
		token=strtok(NULL, " ");
	}
}

void Client::ProcessCommands(char *data)
{
	switch (commandtype)
	{
		case COMMAND_BCI: HandleBCI(data); break;
		case COMMAND_MSGALL: HandleMSGAll(data); break;
		case COMMAND_TELL: HandleTell(data); break;
		case COMMAND_SMSGALL: HandleSMSGAll(data); break;
		case COMMAND_STELL: HandleSTell(data); break;
		case COMMAND_NBMSG: HandleNetbotMessage(data); break;
		//case COMMAND_PONG: HandlePong(data); break;
		//case COMMAND_DISCONNECT: HandleDisconnect(data); break;
		//case COMMAND_NAMES: HandleNames(data); break;
		//case COMMAND_LOCALECHO: HandleLocalEcho(data); break;
		case COMMAND_CHANNELS: HandleChannels(data); break;
		case COMMAND_UNK:
		default:
		{
			fprintf(stderr, "Unknown Command Data: %s", data);
		}break;
	}
}

void Client::ProcessReadPacketBuffer()
{
	static bool discardpong = false;
	static char *data=NULL;
	data = (char *)malloc(MAX_BUF);
	memset(data, 0, MAX_BUF);
	while ((authenticated && rbuf->LineAvailable()) || ((!authenticated) && (rbuf->ReadAvailable()>0)))
	{
		int ret = 0;
		if (!authenticated)
		{
			ret = rbuf->Peek(data, MAX_BUF);
			if ((ret > 0) && (_strnicmp(data, "LOGIN=", 6) == 0) && (strchr(data, ';')!=NULL))
			{
				ret = rbuf->ReadUntil(data, MAX_BUF - 1, ';');
				if (gDebug) fprintf(stderr, "Debug Login: %s\r\n", data);
				command = false;
				HandleLogin(data);
			}
			else
			{
				if (gDebug) fprintf(stderr, "Debug: partial login, or invalid login: %s\n", data);
			}
			/*
			if (!authstart)
			{
				ret = rbuf->ReadUntil(data, 5, '=');
				if (_strnicmp(data, "LOGIN", 5) == 0)
				{
					authstart = true;
				}
				else
				{
					fprintf(stderr, "Unknown data before login: %s", data);
				}
			}
			else
			{
				ret = rbuf->ReadUntil(data, MAX_BUF - 1, ';');
				if (ret > 0)
				{
					fprintf(stderr, "Debug Login: %s\r\n", data);
					HandleLogin(data);
					command = false;
					//authstart = false;
				}
			}*/
		}
		else if (command)
		{
			ret = MAX_BUF - 1;
			if (rbuf->ReadLine(data, ret))
			{
				if (gDebug) fprintf(stderr, "<%s>:Debug Command Data: %s", name, data);
				ProcessCommands(data);
				command = false;
			}
		}
		else if (discardpong)
		{
			discardpong = false;
			ret = MAX_BUF - 1;
			rbuf->ReadLine(data, ret);
			//fprintf(stderr, "Discarded %i byte(s): %s", ret, data);
		}
		else
		{
			ret = MAX_BUF - 1;
			if (rbuf->ReadLine(data, ret))
			{
				if(gDebug) fprintf(stderr, "<%s>:DEBUG Data: \"%s\"\n", name, data);
				char *ptr = NULL;

				while ((ptr=strchr(data, '\\')) != NULL)
				{
					//data[ptr - data] = 0;
					for (int x = (ptr-data); data[x]!=0; x++)
					{
						data[x] = data[x+1];
					}
				}
				switch (data[0])
				{
					case '\t':
					{
						commandtype = COMMAND_UNK;
						HandleCommands(data);
						switch (commandtype)
						{
						case COMMAND_NAMES:HandleNames(data); break;
						case COMMAND_NBNAMES:HandleNetbotNames(data); break;
						case COMMAND_PONG:{
								HandlePong(data);
								discardpong = true;
								command = false;
							}break;
						case COMMAND_LOCALECHO:HandleLocalEcho(data); break;
						case COMMAND_DISCONNECT:HandleDisconnect(data); break;
						//case COMMAND_NBMSG:HandleNetbotMessage(data); break;
						default:
							command = true;
							break;
						}
					}break;
					case '{':
					{
					}break;
					default:
					{
						if ((data[0]) == '\n')
						{
							if (gDebug) fprintf(stderr, "<%s>:DEBUG: Discarding newline\n", name);
							break;
						}
						fprintf(stderr, "<%s> %s", name, data);
						char message[MAX_BUF] = { 0 };
						snprintf(message, MAX_BUF, "<%s> %s", name, data);
						server->SendMSGAll(this, message);
						wbuf->Write(message, strlen(message));
					}
				}
			}
		}
	}
	free(data);
}

void Client::ProcessPacket(packet_t *packet, int sz)
{
	if (!packet)
		return;
	else
		fprintf(stderr, "Client::ProcessPacket() %s", (const char *)packet);
}

void Client::SetSock(int s)
{
	sock=s;
}

void Client::SendBCMessage(const char * message)
{
	if (message)
		wbuf->Write(message, strlen(message));
}

void Client::GetName(char *& _name, int len)
{
	strncpy_s(_name, len, (const char *)name, strlen(name));
}

void Client::ClearPacket(packet_t &packet)
{
	memset(&packet,0,sizeof(packet_t));
}
