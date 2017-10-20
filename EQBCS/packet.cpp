#include "stdafx.h"
#include "packet.h"
//#include <windows.h>

void milisleep(uint64_t ms)
{
#ifndef WIN32
	nanosleep(ms * 1000000ULL);
#else
	Sleep((DWORD)ms);
#endif
}
