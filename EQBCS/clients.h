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
#ifndef CLIENTS_H
#define CLIENTS_H

#ifndef WIN32
#include <pthread.h>
#else
#include <winsock2.h>
#endif

#include <event.h>
#include "ringbuffer.h"
#include "packet.h"
#include <vector>
#include <string>

class Server;

//#define CMD_DISCONNECT "\tDISCONNECT\n"
//#define CMD_NAMES "\tNAMES\n"
//#define CMD_PONG "\tPONG\n"
//#define CMD_MSGALL "\tMSGALL\n"
//#define CMD_TELL "\tTELL\n"
//#define CMD_CHANNELS "\tCHANNELS\n"
//#define CMD_LOCALECHO "\tLOCALECHO "
//#define CMD_BCI "\tBCI\n"
using namespace std;

class Client
{
	public:
		enum StatusCode { STATUS_IDLE = 0, STATUS_LOGIN, STATUS_MESSAGE, STATUS_EXPLODE, STATUS_NAPTIME };
		const static int NUM_COMMANDS = 10;

		enum CommandType { COMMAND_UNK = 0, COMMAND_NAMES = 1, COMMAND_MSGALL, COMMAND_TELL, COMMAND_DISCONNECT, COMMAND_PONG, COMMAND_LOCALECHO, COMMAND_BCI, COMMAND_CHANNELS, COMMAND_NBMSG, COMMAND_NBNAMES };
		struct CommandStruct
		{
			const char *Token;
			CommandType Type;
		};

		CommandStruct Commands[NUM_COMMANDS] = {
			{ "DISCONNECT",	COMMAND_DISCONNECT },
			{ "NAMES",		COMMAND_NAMES },
			{ "MSGALL",		COMMAND_MSGALL },
			{ "TELL",		COMMAND_TELL },
			{ "PONG",		COMMAND_PONG },
			{ "CHANNELS",	COMMAND_CHANNELS },
			{ "LOCALECHO",	COMMAND_LOCALECHO },
			{ "BCI",		COMMAND_BCI },
			{ "NBMSG",		COMMAND_NBMSG },
			{ "NBNAMES",	COMMAND_NBNAMES }
		};

	private:
		int sock;
		int addrlen;
		struct sockaddr_in addr;
		bool wait_status;
		char name[128]; 
		bool ready;
		int sockerr;
		void ProcessWritePacketBuffer();
		void HandleLogin(char * data);
		void HandleCommands(char * data);
		void HandleDisconnect(char *data);
		void HandleNames(char *data);
		void HandleNetbotNames(char * data);
		void HandleNetbotMessage(char * data);
		void HandleMSGAll(char *data);
		void HandleTell(char *data);
		void HandlePong(char *data);
		void HandleLocalEcho(char *data);
		void HandleBCI(char *data);
		void HandleChannels(char *data);
		void ProcessCommands(char * data);
		StatusCode status;
		bool command;
		CommandType commandtype;

	public:
		Client();
		Client(int sock, Server *pserver);
		~Client();
		void Init();
		vector <string> channels;
		event_base *m_ev_base;
		event *ev_auth_timeout;
		timeval tv_auth_timeout;
		bool auth_timeout;
		bool ev_auth_timeout_exists;

		RingBuffer *rbuf;
		RingBuffer *wbuf;

//		event *ev_ping_timeout;
//		timeval tv_ping_timeout;
//		bool ping_timeout;
//		bool ev_ping_timeout_exists;
		time_t last_ping_time;
		time_t last_ping_response_time;


		bool localecho;

		event *ev_read;
		bool ev_read_exists;
		Server *server;
		bool disconnected; //Server is disconnecting client...
		bool accepted;
		bool authenticated;
		bool deleteme;

		/* Socket Stuff/Client Info Stuff */
		int GetSock() { return sock; };
		void SetSock(int s);
		void SendBCMessage(const char *message);
		void GetName(char *&name, int len);
		const char *GetName() { return name; }

		int SendPacket(packet_t *packet);

		/* Client is ready to start processing jobs. */
		void SetReadyStatus(bool status);
		bool IsReady();
		bool IsAuthenticated() { return authenticated; };

		bool CheckConnection(char * CallingFunction);
		void ProcessReadPacketBuffer();

		/* Database and Job Processing */
		void ProcessReads();
		bool DeleteMe() { return deleteme; };
		void CheckPing();
		void ProcessWrites();
		int ReadPacket();
		void ClearPacket(packet_t &packet);

		/* Event Functions */
		static void OnRead(int fd, short event, void *arg);
//		void ProcessPacket(packet_t *packet);
		void ProcessPacket(packet_t *packet, int sz);
		void SetEventBase(event_base *base) { m_ev_base=base; };
		//void SetPingTimeout(bool val) { ping_timeout = val; };
		
		/* Socket Stuff */
		unsigned int GetAvailableBytes();
		bool Disconnected() { return !Connected(); };
		void Disconnect();
		void CleanUp();
		bool Connected();
		void Accept();
		static void OnAuthenticationTimeout(int fd, short event, void *arg);
		static void OnPingTimeout(int fd, short event, void *arg);
};

#endif
