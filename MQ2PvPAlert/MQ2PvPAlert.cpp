// MQ2PvPAlert.cpp : Defines the entry point for the DLL application.
// Author: Naes

#include "../MQ2Plugin.h"
PreSetup("MQ2PvPAlert");

int sounds = 0;
int popup = 0;
int message = 0;

bool CheckForPC(PSPAWNINFO pNewSpawn)
{
    // this seems dirty and redundant, but it's necessary to prevent a crash at the char select
    return (GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->Name && 
        GetSpawnType(pNewSpawn) == PC && 
        //strncmp(pNewSpawn->DisplayedName, "LD", 2) != 0 &&   // ignore VZ/TZ MQ crash mob
        strcmp(GetCharInfo()->pSpawn->Name, pNewSpawn->Name) != 0);
}

void EchoNewPlayer(PSPAWNINFO pNewSpawn, bool entered, DWORD color)
{
    char echomsg[MAX_STRING];
    const char* guild = (GetGuildByID(pNewSpawn->GuildID)) ? (GetGuildByID(pNewSpawn->GuildID)) : "";
    const char* came = (entered) ? "entered" : "left";
    sprintf_s(echomsg, "[PvP] %s (%d %s) <%s> %s %s.", pNewSpawn->Name, 
                        pNewSpawn->Level, pEverQuest->GetClassThreeLetterCode(pNewSpawn->mActorClient.Class),
                        guild, came, ((PZONEINFO)pZoneInfo)->ShortName);

    WriteChatColor(echomsg, color);
}

void PopupNewPlayer(PSPAWNINFO pNewSpawn, int color, int seconds)
{
    char buffer[MAX_STRING];
    sprintf_s(buffer, "/popup %s (%d %s) has entered the zone.", pNewSpawn->Name, pNewSpawn->Level, 
                        pEverQuest->GetClassThreeLetterCode(pNewSpawn->mActorClient.Class));
    //sprintf_s(buffer, "/popcustom %d %d %s has entered the zone.", color, seconds, pNewSpawn->Name);
    DoCommand(GetCharInfo()->pSpawn, buffer);
}

PLUGIN_API VOID OnAddSpawn(PSPAWNINFO pNewSpawn)
{
    if (CheckForPC(pNewSpawn))
    {
        DebugSpewAlways("MQ2PvPAlert::OnAddSpawn(%s) - PC DETECTED", pNewSpawn->Name);
        int seconds = 2;
        int color = 15;

        if (message)
            EchoNewPlayer(pNewSpawn, true, CONCOLOR_YELLOW);

        if (popup)
            PopupNewPlayer(pNewSpawn, color, seconds);        

        if (sounds)
            MacroBeep(GetCharInfo()->pSpawn, "AudioTriggers\\default\\alert4.wav");
    }
}

PLUGIN_API VOID OnRemoveSpawn(PSPAWNINFO pSpawn)
{
    if (CheckForPC(pSpawn))
    {
        DebugSpewAlways("MQ2PvPAlert::OnRemoveSpawn(%s) - PC DETECTED", pSpawn->Name);

        if (message)
            EchoNewPlayer(pSpawn, false, CONCOLOR_RED);    
    
        if (sounds)
            MacroBeep(GetCharInfo()->pSpawn, "AudioTriggers\\default\\alert2.wav");
    }
}

void PvPAlertExecuteCmd(PCHAR iniSetting, int &option, int activate)
{
    CHAR szMsg[MAX_STRING]={0};
    CHAR szTemp[MAX_STRING]={0};
    
    sprintf_s(szTemp, "%d", activate);

    option = activate;
    WritePrivateProfileString("MQ2PvPAlert", iniSetting, szTemp, INIFileName);

    sprintf_s(szMsg, "MQ2PvPAlert - %s: %d", iniSetting, activate);
    WriteChatColor(szMsg, USERCOLOR_DEFAULT);
}

void PvPAlertCmd(PSPAWNINFO pChar, PCHAR szLine)
{
    CHAR Arg1[MAX_STRING] = {0}; 
    CHAR Arg2[MAX_STRING] = {0};

    GetArg(Arg1, szLine, 1); 
    GetArg(Arg2, szLine, 2); 

    
    int activate = (!_stricmp(Arg2, "on")) ? 1 : 0;

    //DebugSpewAlways("sZLine: %d, Arg1: %s, Arg2: %s, activate: %d", szLine, Arg1, Arg2, activate);

    if (!_stricmp(Arg1, "sounds"))
    {
        PvPAlertExecuteCmd("sounds", sounds, activate);
    }
    else if(!_stricmp(Arg1, "popup"))
    {
        PvPAlertExecuteCmd("popup", popup, activate);
    }
    else if(!_stricmp(Arg1, "message"))
    {
        PvPAlertExecuteCmd("message", message, activate);
    }
    else
    {
        WriteChatColor("MQ2PvPAlert Usage: /pvpalert [sounds/popup/message] [on/off]", USERCOLOR_DEFAULT);
    }
}

PLUGIN_API VOID InitializePlugin(VOID) 
{
    AddCommand("/pvpalert", PvPAlertCmd); 

    sounds = GetPrivateProfileInt("MQ2PvPAlert", "sounds", 1, INIFileName);
    popup = GetPrivateProfileInt("MQ2PvPAlert", "popup", 1, INIFileName);
    message = GetPrivateProfileInt("MQ2PvPAlert", "message", 1, INIFileName);
}

PLUGIN_API VOID ShutdownPlugin(VOID) 
{ 
    RemoveCommand("/pvpalert"); 
}  