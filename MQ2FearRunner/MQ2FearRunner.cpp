// MQ2FearRunner.cpp : RedGuides exclusive plugin to execute commands when you are first feared and when fear wears off, normally to run back to your starting location.
// v1.0 - Sym - 08-19-2017



#include "../MQ2Plugin.h"
PLUGIN_VERSION(1.0);

#pragma warning(disable:4800)
PreSetup("MQ2FearRunner");

#define EFFECT_FEAR            23

#define SKIP_PULSES 40
long SkipPulse = 0;
long dSize;
bool bFearRunnerEnabled = false;
bool bWasFeared = false;
bool bInitDone = false;
char szTemp[MAX_STRING];
char szStartCommand[MAX_STRING];
char szStopCommand[MAX_STRING];

PSPELL dList[45];

bool AmIFeared() {
    for(long buff=0; buff<dSize; buff++) {
        for(int slot=0; slot<GetSpellNumEffects(dList[buff]); slot++) {
            if(GetSpellAttrib(dList[buff],slot)==EFFECT_FEAR)
                return true;
        }
    }
    return false;
}

void ShowHelp (void) {
    WriteChatf("\atMQ2FearRunner :: v%1.2f :: by Sym for RedGuides.com\ax", MQ2Version);
    WriteChatf("/fearrunner :: Lists command syntax");
    WriteChatf("/fearrunner on|off :: Toggle plugin. Default is \agON\ax");
    WriteChatf("/fearrunner status :: Shows the current status and settings. Changes automatically update the ini file.");
    WriteChatf("/fearrunner startcommand \"COMMAND\" :: Sets the command to run when feared. Enclose in \"\" if it contains spaces. Default is \ag/echo feared");
    WriteChatf("/fearrunner stopcommand \"COMMAND\" :: Sets the command to run when no longer feared. Enclose in \"\" if it contains spaces. Default is \ag/echo no longer feared");

}

void ShowStatus() {
    WriteChatf("\atMQ2FearRunner :: v%1.2f :: by Sym for RedGuides.com\ax", MQ2Version);
    WriteChatf("MQ2FearRunner :: %s", bFearRunnerEnabled?"\agENABLED\ax":"\arDISABLED\ax");
    WriteChatf("MQ2FearRunner :: Start Command is \ag%s\ax", szStartCommand);
    WriteChatf("MQ2FearRunner :: Stop Command is \ag%s\ax", szStopCommand);
}

void Update_INIFileName() {
    sprintf_s(INIFileName,"%s\\MQ2FearRunner.ini",gszINIPath);
}

VOID LoadINI(VOID) {
    Update_INIFileName();
    sprintf_s(szTemp,"%s_Settings",GetCharInfo()->Name);
    bFearRunnerEnabled = GetPrivateProfileInt(szTemp, "Enabled", 1, INIFileName) > 0 ? true : false;
    GetPrivateProfileString(szTemp,"StartCommand","/echo feared",szStartCommand,2047,INIFileName);
    GetPrivateProfileString(szTemp,"StopCommand","/echo no longer feared",szStopCommand,2047,INIFileName);
    ShowStatus();
    bInitDone = true;
}
VOID SaveINI(VOID) {

    Update_INIFileName();
    // write on/off settings
    sprintf_s(szTemp,"%s_Settings",GetCharInfo()->Name);
    WritePrivateProfileSection(szTemp, "", INIFileName);
    WritePrivateProfileString(szTemp,"Enabled",bFearRunnerEnabled? "1" : "0",INIFileName);
    WritePrivateProfileString(szTemp,"StartCommand",szStartCommand,INIFileName);
    WritePrivateProfileString(szTemp,"StopCommand",szStopCommand,INIFileName);

    if (bInitDone) WriteChatf("\atMQ2FearRunner :: Settings updated\ax");
}


VOID FearRunnerCommand (PSPAWNINFO pCHAR, PCHAR zLine) {
    GetArg(szTemp,zLine,1);
    if ( !_strnicmp (szTemp, "status", 6) ) {
        ShowStatus();
    } else if(!_strnicmp(szTemp,"on",2)) {
        bFearRunnerEnabled = true;
        WriteChatf("MQ2FearRunner :: \agEnabled\ax");
    } else if(!_strnicmp(szTemp,"off",3)) {
        bFearRunnerEnabled = false;
        WriteChatf("MQ2FearRunner :: \arDisabled\ax");
    } else if(!_strnicmp(szTemp,"startcommand",12)) {
        GetArg(szTemp,zLine,2);
        if(!_strcmpi(szTemp, "")) {
            WriteChatf("Usage: /fearrunner startcommand \"COMMAND TEXT\"");
            return;
        }
        sprintf_s(szStartCommand,"%s",szTemp);
        WriteChatf("MQ2FearRunner :: Start command set to \"\ay%s\"\ax", szStartCommand);
        SaveINI();
    } else if(!_strnicmp(szTemp,"stopcommand",11)) {
        GetArg(szTemp,zLine,2);
        if(!_strcmpi(szTemp, "")) {
            WriteChatf("Usage: /fearrunner stopcommand \"COMMAND TEXT\"");
            return;
        }
        sprintf_s(szStopCommand,"%s",szTemp);
        WriteChatf("MQ2FearRunner :: Stop command set to \"\ay%s\"\ax", szStopCommand);
        SaveINI();
    } else {
        ShowHelp ();
    }
}

PLUGIN_API VOID InitializePlugin(VOID){
    DebugSpewAlways("Initializing MQ2FearRunner");
    AddCommand("/fearrunner",FearRunnerCommand);
}

PLUGIN_API VOID ShutdownPlugin(VOID) {
    DebugSpewAlways("Shutting down MQ2FearRunner");
    RemoveCommand("/fearrunner");

}


PLUGIN_API VOID SetGameState(DWORD GameState) {
    DebugSpewAlways("MQ2FearRunner::SetGameState()");
    if(GameState==GAMESTATE_INGAME) {
        if (!bInitDone) LoadINI();
    } else if(GameState!=GAMESTATE_LOGGINGIN) {
        if (bInitDone) bInitDone=false;
    }

}


PLUGIN_API VOID OnPulse(VOID) {
    if (GetGameState() != GAMESTATE_INGAME)
        return;
    if (!bInitDone)
        return;
    if (!bFearRunnerEnabled)
        return;
    SkipPulse++;

    if ( SkipPulse == SKIP_PULSES ) {
        SkipPulse = 0;
        if (bWasFeared) {
            if (AmIFeared()) return;
            bWasFeared = false;
            WriteChatf("\atMQ2FearRunner :: \agNo longer feared. Executing \ag%s\ax", szStopCommand);
            DoCommand(GetCharInfo()->pSpawn,szStopCommand);
            return;
        }
        if (AmIFeared() && !bWasFeared) {
            bWasFeared = true;
            WriteChatf ("\atMQ2FearRunner :: \arI am feared. Executing \ag%s\ax", szStartCommand);
            DoCommand(GetCharInfo()->pSpawn,szStartCommand);
            return;
        }
    }
}

