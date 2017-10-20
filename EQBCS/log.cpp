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

#include "log.h"

Log::Log()
{
	m_maxsz=0;
	m_filename=NULL;
	m_logfile=NULL;
	m_open=false;
}

Log::Log(const char *filename, uint32_t maxsz)
{
	Log();
}

Log::~Log()
{
	if(m_open)
	{
		fclose(m_logfile);
		if(m_filename!=NULL)
		{
			free(m_filename);
			m_filename=NULL;
		}
		m_open=false;
	}
}

bool Log::Close()
{
	if(!m_open)
		return false; //File was not open, so how can we close it?
	fclose(m_logfile);
	m_logfile=NULL;
	m_open=false;
	return true;
}

bool Log::Open(const char *filename, uint32_t maxsz)
{
	if(m_open)
		Close();
	if(strlen(filename)<m_fnmax)
		strncpy(m_filename, filename, strlen(filename));
	else
	{
		if(m_filename!=NULL)
			free(m_filename);
		m_fnmax=strlen(filename)+1;
		m_filename=(char *)malloc(m_fnmax);
		if(!m_filename)
		{
			m_fnmax=0;
			return false;
		}
		memset(m_filename, 0, m_fnmax);
		strncpy(m_filename, filename, strlen(filename));
	}
	return Open();
}

bool Log::Open()
{
	if(m_filename==NULL)
		return false;
	if(m_maxsz==0)
	{
		m_maxsz=32;
	}
	if(m_open)
		Close();
	m_logfile=fopen(m_filename, "w");
	if(m_logfile!=NULL)
	{
		m_open=true;
		Write("Log::Open() Opened log file successfully...\n");
		return true;
	}
	else
		return false;
	return false;
}

int Log::Write(const char *format, ... )
{
	if(!m_open) return -1;
	va_list args;
	WriteTimestamp();
	va_start(args, format);
	vfprintf(m_logfile, format, args);
	va_end(args);
	fflush(m_logfile);
#ifndef WIN32
	fsync(m_logfile->_fileno);
#endif
	return true;
}

bool Log::WriteTimestamp()
{
/*
	if(!m_open) return false;
#ifdef WIN32
	SYSTEMTIME now;
	fprintf(m_logfile, "%04i/%02i/%02i %02i:%02i:%02i.%03i: ", now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds);
#else
	
	tm tmnow;
	struct tm *localtime(const time_t *timep);
#endif
*/
	return false;
}