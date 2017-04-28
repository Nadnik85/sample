#include "../MQ2Plugin.h" 
#define MODULE_NAME "MQ2Paranoid" 
PreSetup(MODULE_NAME); 

time_t Seconds; 
unsigned long ulDelay = 7; 
bool bZoning = false; 
bool bParanoid = true; 
bool bEnter = true; 
bool bExit = true; 
bool bChatOutput = true; 
bool bPopOutput = true; 
bool bLoaded = false; 

void ParanoidState() 
{ 
    char szOn[10] = "\agON\ax"; 
    char szOff[10] = "\arOFF\ax"; 
    WriteChatf("\ay%s\aw:: %s - Enter(%s) Exit(%s) Chat(%s) Pop(%s) Delay(\ag%u\ax)", MODULE_NAME, bParanoid ? szOn : szOff, bEnter ? szOn : szOff, bExit ? szOn : szOff, bChatOutput ? szOn : szOff, bPopOutput ? szOn : szOff, ulDelay); 
} 
template <unsigned int _Size>LPSTR SafeItoa(int _Value, char(&_Buffer)[_Size], int _Radix)
{
	errno_t err = _itoa_s(_Value, _Buffer, _Radix);
	if (!err) {
		return _Buffer;
	}
	return "";
}
void RetainParanoia(bool bSave) 
{ 
    if (bSave) 
    { 
        char szSave[MAX_STRING] = {0}; 
        WritePrivateProfileString("Settings", "Enabled", bParanoid ? "on" : "off", INIFileName); 
        WritePrivateProfileString("Settings", "Enter", bEnter ? "on" : "off", INIFileName); 
        WritePrivateProfileString("Settings", "Exit", bExit ? "on" : "off", INIFileName); 
        WritePrivateProfileString("Settings", "Chat", bChatOutput ? "on" : "off", INIFileName); 
        WritePrivateProfileString("Settings", "Popup", bPopOutput ? "on" : "off", INIFileName); 
        WritePrivateProfileString("Settings", "Delay", SafeItoa((int)ulDelay, szSave, 10), INIFileName);
    } 
    else 
    { 
        char szLoad[MAX_STRING] = {0}; 
        GetPrivateProfileString("Settings", "Enabled", "on", szLoad, MAX_STRING, INIFileName); 
        bParanoid = (!_strnicmp(szLoad, "on", 3)); 
        GetPrivateProfileString("Settings", "Enter", "on", szLoad, MAX_STRING, INIFileName); 
        bEnter = (!_strnicmp(szLoad, "on", 3)); 
        GetPrivateProfileString("Settings", "Exit", "on", szLoad, MAX_STRING, INIFileName); 
        bExit = (!_strnicmp(szLoad, "on", 3)); 
        GetPrivateProfileString("Settings", "Chat", "on", szLoad, MAX_STRING, INIFileName); 
        bChatOutput = (!_strnicmp(szLoad, "on", 3)); 
        GetPrivateProfileString("Settings", "Popup", "on", szLoad, MAX_STRING, INIFileName); 
        bPopOutput = (!_strnicmp(szLoad, "on", 3)); 
        int iValid = GetPrivateProfileInt("Settings", "Delay", (int)ulDelay, INIFileName); 
        if (iValid > 0) ulDelay = iValid; 
    } 
    if (bLoaded) WriteChatf("\ay%s\aw:: Configuration file %s.", MODULE_NAME, bSave ? "saved" : "loaded"); 
} 

void ParanoiaLevel(PSPAWNINFO pChar, char* szLine) 
{ 
    char szCurArg[MAX_STRING] = {0}; 

    GetArg(szCurArg, szLine, 1); 
    if (!*szCurArg) 
    { 
        bParanoid = !bParanoid; 
    } 
    else if (!_strnicmp(szCurArg, "on", 3)) 
    { 
        bParanoid = true; 
    } 
    else if (!_strnicmp(szCurArg, "off", 4)) 
    { 
        bParanoid = false; 
    } 
    else if (!_strnicmp(szCurArg, "enter", 6)) 
    { 
        bEnter = !bEnter; 
    } 
    else if (!_strnicmp(szCurArg, "exit", 5)) 
    { 
        bExit = !bExit; 
    } 
    else if (!_strnicmp(szCurArg, "chat", 5)) 
    { 
        bChatOutput = !bChatOutput; 
    } 
    else if (!_strnicmp(szCurArg, "pop", 4)) 
    { 
        bPopOutput = !bPopOutput; 
    } 
    else if (!_strnicmp(szCurArg, "status", 7)) 
    { 
        // valid command 
    } 
    else if (!_strnicmp(szCurArg, "delay", 6)) 
    { 
        char* pNotNum = NULL; 
        GetArg(szCurArg, szLine, 2); 
        int iValid = (int)strtoul(szCurArg, &pNotNum, 10); 
        if (iValid < 1 || *pNotNum) 
        { 
            WriteChatf("\ay%s\aw:: \arError\ax - Delay must be a positive numerical value.", MODULE_NAME); 
            return; 
        } 
        ulDelay = iValid; 
    } 
    else if (!_strnicmp(szCurArg, "load", 5)) 
    { 
        RetainParanoia(false); 
        return; 
    } 
    else if (!_strnicmp(szCurArg, "save", 5)) 
    { 
        RetainParanoia(true); 
        return; 
    } 
    else if (!_strnicmp(szCurArg, "help", 5)) 
    { 
        WriteChatf("\ay%s\aw:: \ag/para\ax [ \agon\ax | \agoff\ax | \agenter\ax | \agexit\ax | \agchat\ax | \agpop\ax ]", MODULE_NAME); 
        WriteChatf("\ay%s\aw:: \ag/para\ax [ \agsave\ax | \agload\ax | \agstatus\ax | \aghelp\ax ]", MODULE_NAME); 
        WriteChatf("\ay%s\aw:: \ag/para\ax [ \agdelay #\ax ] - Sets zone time delay.", MODULE_NAME); 
        return; 
    } 
    else 
    { 
        WriteChatf("\ay%s\aw:: Invalid parameter. Use \ag/para help\ax for valid options.", MODULE_NAME); 
        return; 
    } 
    ParanoidState(); 
} 

PLUGIN_API void OnBeginZone() 
{ 
    bZoning = true; 
} 

PLUGIN_API void OnEndZone() 
{ 
    Seconds = time(NULL); 
    bZoning = false; 
} 

PLUGIN_API void OnAddSpawn(PSPAWNINFO pNewSpawn) 
{ 
    if (GetGameState() == GAMESTATE_INGAME && !bZoning && bParanoid && time(NULL) > Seconds + ulDelay && pNewSpawn->SpawnID && pNewSpawn->Type == SPAWN_PLAYER) 
    { 
        if (bPopOutput) 
        { 
            char szMsg[MAX_STRING] = {0}; 
            sprintf_s(szMsg, "%s %d %s %s has entered the zone.", pNewSpawn->DisplayedName, pNewSpawn->Level, pEverQuest->GetRaceDesc(pNewSpawn->mActorClient.Race), GetClassDesc(pNewSpawn->mActorClient.Class)); 
            DisplayOverlayText(szMsg, CONCOLOR_RED, 100, 500, 500, 2000); 
        } 
        if (bChatOutput) 
        { 
            char szMsg[MAX_STRING] = {0}; 
            sprintf_s(szMsg, "\ay%s\aw:: \ar%s %d %s %s has entered the zone.", MODULE_NAME, pNewSpawn->DisplayedName, pNewSpawn->Level, pEverQuest->GetRaceDesc(pNewSpawn->mActorClient.Race), GetClassDesc(pNewSpawn->mActorClient.Class)); 
            WriteChatf(szMsg); 
        } 
    } 
} 

PLUGIN_API void OnRemoveSpawn(PSPAWNINFO pOldSpawn) 
{ 
    if (GetGameState() == GAMESTATE_INGAME && !bZoning && bParanoid && time(NULL) > Seconds + ulDelay && pOldSpawn->SpawnID && pOldSpawn->Type == SPAWN_PLAYER) 
    { 
        if (bPopOutput) 
        { 
            char szMsg[MAX_STRING] = {0}; 
            sprintf_s(szMsg, "%s %d %s %s has left the zone.", pOldSpawn->DisplayedName, pOldSpawn->Level, pEverQuest->GetRaceDesc(pOldSpawn->mActorClient.Race), GetClassDesc(pOldSpawn->mActorClient.Class)); 
            DisplayOverlayText(szMsg, CONCOLOR_GREEN, 100, 500, 500, 2000); 
        } 
        if (bChatOutput) 
        { 
            char szMsg[MAX_STRING] = {0}; 
            sprintf_s(szMsg, "\ay%s\aw:: \ag%s %d %s %s has left the zone.", MODULE_NAME, pOldSpawn->DisplayedName, pOldSpawn->Level, pEverQuest->GetRaceDesc(pOldSpawn->mActorClient.Race), GetClassDesc(pOldSpawn->mActorClient.Class)); 
            WriteChatf(szMsg); 
        } 
    } 
} 

PLUGIN_API void InitializePlugin() 
{ 
    AddCommand("/para", ParanoiaLevel); 
    Seconds = time(NULL); 
    RetainParanoia(false); 
    bLoaded = true; 
} 

PLUGIN_API void ShutdownPlugin() 
{ 
    RemoveCommand("/para"); 
}