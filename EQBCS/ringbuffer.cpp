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
#include "ringbuffer.h"

RingBuffer::RingBuffer()
{
	buf=(unsigned char *)malloc(MAX_BUF);
	memset(buf,0,MAX_BUF);
	sz=MAX_BUF;
	full=false;
	empty=true;
	w_avail=sz-1;
	r_avail=0;
	start=0;
	end=1;
}

RingBuffer::RingBuffer(int size)
{
	buf=(unsigned char *)malloc(size);
	memset(buf,0,size);
	sz=size;
	full=false;
	empty=true;
	w_avail=sz-1;
	r_avail=0;
	start=0;
	end=0;
}

RingBuffer::~RingBuffer()
{
	if(buf) free(buf);
}

int RingBuffer::Write(const void *_in, int len)
{
	unsigned char *in=(unsigned char *)_in;
	int to_write=0,x=0;
	if(isFull()) return 0;
	if(w_avail>=len)
		to_write=len;
	else
		to_write=w_avail;
	for(x=0; x<to_write; x++)
	{
		Write(in[x]); // Should never fail, we checked how much we can write above...
	}
	return to_write;
}

int RingBuffer::Read(void *_out, int len)
{
	unsigned char *out=(unsigned char *)_out;
	int to_read=0,x=0;
	if(isEmpty()) return 0;
	if(r_avail>=len) to_read=len;
	else to_read=r_avail;
	for(x=0; x<to_read; x++)
	{
		Read(out[x]);
	}
	return len;
}

bool RingBuffer::ReadLine(char *out, int &maxlen)
{
	int x = 0;
	unsigned char c=0;
	int len = maxlen;
	if (len > r_avail)
		len = r_avail;
	for (x = 0; x < len; x++)
	{
		if (Read(((unsigned char *)out)[x]))
		{
			if (out[x] == '\n')
			{
				maxlen = x + 1;
				out[maxlen] = 0;
				return true;
			}
		}
	}
	maxlen = x;
	return false;
}

bool RingBuffer::LineAvailable()
{
	if (isEmpty()) return false;
	int x = 0;
	unsigned char c = 0;
	for (x = 0; x < r_avail; x++)
	{
		if (buf[(start + x) % sz] == '\n')
			return true;
	}
	return false;
}

int RingBuffer::ReadUntil(char *out, int maxlen, char terminator)
{
	int x = 0;
	unsigned char c = 0;
	while ((c != terminator) && (x < maxlen) && (Read(c)))
		out[x++] = c;
	if (x >= maxlen)
		out[x - 1] = 0;
	else
		out[x] = 0;
	return x;
}

int RingBuffer::Write(const unsigned char in)
{
	int pos=0;
	if(isFull()) return 0;
	buf[end]=in;
	end++;
	end%=sz;
	r_avail++;
	w_avail--;
	return 1;
}

int RingBuffer::Writef(const char * format, ...)
{
	int wrote = 0;
	static char temp[MAX_PACKET] = { 0 };
	memset(temp, 0, MAX_PACKET);
	va_list args;
	va_start(args, format);
	wrote=vsnprintf(temp, MAX_PACKET, format, args);
	if (wrote) Write(temp, wrote);
	va_end(args);
	return wrote;
}

int RingBuffer::Read(unsigned char &in)
{
	if(isEmpty()) return 0;
	in=buf[start];
	start++;
	start%=sz;
	r_avail--;
	w_avail++;
	return 1;
}

int RingBuffer::Read(char &out)
{
	return Read((unsigned char &)out);
}

int RingBuffer::Peek(unsigned char &in, int offset)
{
	if(isEmpty()) return 0;
	in=buf[(start+offset)%sz];
	return 1;
}

int RingBuffer::Peek(void *_out, int len)
{
	unsigned char *out=(unsigned char *)_out;
	int to_read=0,x=0;
	if(isEmpty()) return 0;
	if(r_avail>=len) to_read=len;
	else to_read=r_avail;
	for(x=0; x<to_read; x++)
	{
		Peek(out[x], x);
	}
	return len;
}

int RingBuffer::ReadAvailable()
{
	return r_avail;
}

int RingBuffer::WriteAvailable()
{
	return w_avail;
}

bool RingBuffer::isFull()
{
	if((start==0)&&(end==(sz-1))) full=true;
	else if((start>0)&&(end==(start-1))) full=true;
	else full=false;
	return full;
}

bool RingBuffer::isEmpty()
{
	empty=(start==end);
	return empty;
}
