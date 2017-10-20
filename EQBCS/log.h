/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>

#ifndef WIN32
#include <unistd.h>
#endif

#ifndef uint32_t
#define uint32_t unsigned __int32
#endif

class Log
{
	public:
		Log();
		Log(const char *filename, uint32_t maxsz);
		int Write(const char *data, ... );
		bool Open();
		bool Open(const char *filename, uint32_t maxsz);
		bool Close();
		bool WriteTimestamp();
		~Log();
	private:
		FILE *m_logfile;
		char *m_filename;
		bool m_open;
		uint32_t m_maxsz;
		uint32_t m_fnmax;
		uint32_t m_maxfiles;
};

#endif // LOG_H
