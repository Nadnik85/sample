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
#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <memory.h>
#include <malloc.h>
#include "ringbuffer.h"
#include "packet.h"
#include <stdarg.h>
#include <stdio.h>

class RingBuffer
{
	public:
		RingBuffer();
		RingBuffer(int size);
		~RingBuffer();
		int Write(const void *in, int len); //Returns # bytes written, and adds them to the buffer
		int Writef(const char *format, ...); //Returns # bytes written, and adds them to the buffer (using format like printf)
		int Read(void *out, int len); //Returns # bytes read, and 'removes' them from the buffer
		bool ReadLine(char *out, int &maxlen); //Returns a line up to end of buffer
		bool LineAvailable();
		int ReadUntil(char *out, int maxlen, char terminator); //Returns a line up to end of buffer
		int Peek(void *out, int len); //Same as read above, but doesn't alter buf
		int ReadAvailable();
		int WriteAvailable();
		bool isFull();
		bool isEmpty();

	private:
		int Write(const unsigned char in); //Returns # bytes written, and adds them to the buffer
		int Read(unsigned char &out); //Returns # bytes read, and 'removes' them from the buffer
		int Read(char &out); //Returns # bytes read, and 'removes' them from the buffer
		int Peek(unsigned char &out, int offset); //Same as above, but doesn't alter buf
		unsigned char *buf;
		unsigned int sz;
		bool empty;
		bool full;
		int w_avail;
		int r_avail;
		int end;
		int start;
};

#endif
