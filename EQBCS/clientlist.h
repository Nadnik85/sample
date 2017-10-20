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
#ifndef __CLIENTLIST_H__
#define __CLIENTLIST_H__

#ifndef WIN32
using namespace std;
#endif

#include <vector>
#include <map>
#include <list>
#include "clients.h"
#include "locker.h"

using namespace std;

class ClientList
{
	public:
		ClientList();
		~ClientList();
		void Add(Client *client);
		void Remove(Client *client);
		void Lock();
		void Unlock();
		int Count();
		Client *operator[](int client);
		Client *operator[](const char *client);
		locker <list <Client *>> clientlist;
};

#endif