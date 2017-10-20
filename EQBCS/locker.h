/***************************************************************************
 *   Copyright (C) 2009 by Suede Worthey   *
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
#ifndef __LOCKER_H__
#define __LOCKER_H__

#ifndef WIN32
#include <pthread.h>
#else
//#include <windows.h>
#include "stdafx.h"
#endif

#include <vector>
#include <map>

#ifndef WIN32
template <typename _T>
class locker: public _T
{
	pthread_mutex_t mutex[1];
	public:
		inline locker()
		{
			pthread_mutexattr_t attr={PTHREAD_MUTEX_ERRORCHECK_NP};
			pthread_mutex_init(mutex, &attr);
		}
		inline ~locker()
		{
			if (pthread_mutex_destroy(mutex)==EBUSY)
			{
				Unlock();
				pthread_mutex_destroy(mutex);
			}
		}
		inline void Lock()
		{
			pthread_mutex_lock(mutex);
		}
		inline void Unlock()
		{
			pthread_mutex_unlock(mutex);
		}
};
#else
template <typename _T>
class locker: public _T
{
	private:
		//pthread_mutex_t mutex[1];
		HANDLE hmutex;
		bool locked;
	public:
		inline locker()
		{
			//pthread_mutexattr_t attr={PTHREAD_MUTEX_ERRORCHECK_NP};
			//pthread_mutex_init(mutex, &attr);
			//hmutex = CreateMutex(NULL, false, NULL);
			locked=false;
		}
		inline ~locker()
		{
			if (locked)
			{
				Unlock();
			}
			//if(hmutex!=NULL)
				//CloseHandle(hmutex);
		}
		inline void Lock()
		{
			//if (hmutex != NULL)
				//WaitForSingleObject(hmutex, INFINITE);
			locked = false;
		}
		inline void Unlock()
		{
			//if (hmutex != NULL)
				//ReleaseMutex(hmutex);
			locked = false;
		}
		//this will return false and lock the mutex if it is unlocked. if it is locked already, it will return true;
		inline bool Locked() 
		{
			return locked;
		}
};
#endif
#endif