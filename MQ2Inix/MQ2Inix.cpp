//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Project: MQ2IniX.cpp
// Author: htw (based on his code / works!)
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// v1.0 - 02-01-2019 - Initial Release

#define PLUGIN_NAME "MQ2IniX"

#ifndef PLUGIN_API
	#include "../MQ2Plugin.h"
	PreSetup(PLUGIN_NAME);
	PLUGIN_VERSION(1.0);
#endif PLUGIN_API

VOID IniX(PSPAWNINFO pChar, PCHAR szLine)
{

	CHAR szArg1[MAX_STRING]   = { 0 };  //Filename
	CHAR szArg2[MAX_STRING]   = { 0 };  //Section
	CHAR szArg3[MAX_STRING]   = { 0 };  //Key
	CHAR szArg4[MAX_STRING]   = { 0 };  //Data to write
    CHAR szOutput[MAX_STRING] = { 0 };  //Success / Error Output

    GetArg(szArg1, szLine, 1);
    GetArg(szArg2, szLine, 2);
    GetArg(szArg3, szLine, 3);
    GetArg(szArg4, szLine, 4);
	
    int NumArgs = 4;
    if (strlen(szArg4) < 1) NumArgs = 3;
    if (strlen(szArg3) < 1) NumArgs = 2;
    if (strlen(szArg2) < 1) NumArgs = 1;
    if (strlen(szArg1) < 1) NumArgs = 0;

	DebugSpew("/inix input -- %s %s %s %s", szArg1, szArg2, szArg3, szArg4);

	PCHAR pTemp = szArg1;
    while (pTemp[0])
    {
		if (pTemp[0] == '/') pTemp[0] = '\\';
        pTemp++;

    }
	
    pTemp = szArg4;
    while (pTemp[0])
    {
		if (pTemp[0] == '^') pTemp[0] = '$';
        pTemp++;
    }
	
    if (szArg1[0] != '\\' && !strchr(szArg1, ':'))
    {
		sprintf_s(szOutput, "%s\\%s", gszMacroPath, szArg1);
        strcpy_s(szArg1, szOutput);
    }

	if (!strstr(szArg1, ".")) strcat_s(szArg1, ".ini");
	
    ZeroMemory(szOutput, MAX_STRING);

    BOOL Result = false; 					// default to fail for fall-through errors

	switch(NumArgs)
	{
		case 0: // empty parameters
		case 1: // no section - don't allow removal of entire ini file via this method
			WriteChatf("\arUsage: /inix FileName SectionName KeyName Data");
			return;
		case 2:	// empty key = delete section
			Result = WritePrivateProfileString(szArg2, NULL, NULL, szArg1);
			break;
		case 3:	// empty data = delete key
			Result = WritePrivateProfileString(szArg2, szArg3, NULL, szArg1);
			break;
		case 4: // Write normal data
            Result = WritePrivateProfileString(szArg2, szArg3, szArg4, szArg1);
	}

	sprintf_s(szOutput,(Result)?"/Inix Output Write Successful!: %s":"/inix Output ERROR -- during WritePrivateProfileString: %s",szLine);
	DebugSpew(szOutput);
}

PLUGIN_API VOID InitializePlugin()
{
	AddCommand("/inix", IniX);
}

PLUGIN_API VOID ShutdownPlugin()
{
	RemoveCommand("/inix");
}
