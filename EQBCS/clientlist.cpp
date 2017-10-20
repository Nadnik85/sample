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

#include "clientlist.h"

ClientList::ClientList()
{
}

 ClientList::~ClientList()
 {
	 Lock();
	 try {
		 for (int x = Count(); x > 0; x--)
		 {
			 Client *client = (Client *)(*this)[x];
			 if (client)
			 {
				 Remove(client);
				 delete client;
			 }
		 }
	 }
	 catch (exception ex)
	 {
		 fprintf(stderr, "ClientList::~ClientList() Exception in cleanup... Cry like a baby, or fix it?\n");
	 }
	 Unlock();
 }

void ClientList::Add(Client *client)
{
	if(client!=NULL)
	{
		clientlist.push_back(client);
	}
}

void ClientList::Remove(Client *client)
{
	clientlist.remove(client);
}

int ClientList::Count()
{
	return clientlist.size();
}

Client *ClientList::operator[](int client)
{
	Client *ret=NULL;
	int count=0;
	int cur=0;
	count=Count();
	if(client>=count)
	{
	}
	else
	{
		locker < list < Client * > >::iterator iter=clientlist.begin();
		for(iter = clientlist.begin(); iter!=clientlist.end(); )
		{
			if(cur==client)
			{
				ret=*iter;
				break;
			}
			iter++;
			cur++;
		}
	}
	return ret;
}

Client * ClientList::operator[](const char * client)
{
	Client *ret = NULL;
	int cur = 0;
	locker < list < Client * > >::iterator iter = clientlist.begin();
	for (iter = clientlist.begin(); iter != clientlist.end(); )
	{
		char name[50] = { 0 };
		char *pname = name;
		if ((*iter)->IsAuthenticated())
		{
			(*iter)->GetName(pname, 50);
			if ((strlen(name) == strlen(client)) && (_strnicmp(client, name, strlen(name)) == 0))
			{
				ret = *iter;
				break;
			}
		}
		iter++;
		cur++;
	}
	return ret;
}

void ClientList::Lock()
{
	clientlist.Lock();
}

void ClientList::Unlock()
{
	clientlist.Unlock();
}