// MQ2KillTracker.cpp : RedGuides exclusive plugin to track kills over time
// v1.00 :: Sym - 2017-05-03
// v1.01 :: Sym - 2017-05-07 - Fixed a couple display errors and added /killcount command
// v1.02 :: Sym - 2017-05-21 - Fixed reset command and reporting at character select
// v1.03 :: Sym - 2017-05-27 - Fixed reset on every zone


#include "../MQ2Plugin.h"
PLUGIN_VERSION(1.03);


PreSetup("MQ2KillTracker");

#define SKIP_PULSES 25
#define SECOND 1000
#define MINUTE (60 * SECOND)
#define HOUR (60 * MINUTE)
#define DAY (24 * HOUR)

long SkipPulse = 0;
bool bKillTrackerEnabled;
bool bInit = false;
long killCount = 0;
long reportMinutes = 60;
long reportTimer = reportMinutes * MINUTE;
char szTemp[MAX_STRING];


DWORD StartTime = GetTickCount();
DWORD reportLastSent = GetTickCount();



void Update_INIFileName() {
    sprintf_s(INIFileName,"%s\\%s_%s.ini",gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
}

VOID LoadINI (VOID) {

    Update_INIFileName ();
    // get on/off settings
    sprintf_s (szTemp, "%s", "MQ2KillTracker");
    bKillTrackerEnabled = GetPrivateProfileInt (szTemp, "Enabled", 1, INIFileName) > 0?true:false;
    reportMinutes = GetPrivateProfileInt (szTemp, "DelayInMinutes", 60, INIFileName);
    reportTimer = reportMinutes * MINUTE;
}

VOID SaveINI (VOID) {
    char ToStr[16];

    Update_INIFileName ();
    // write on/off settings
    sprintf_s (szTemp, "%s", "MQ2KillTracker");
    WritePrivateProfileSection (szTemp, "", INIFileName);
    WritePrivateProfileString (szTemp, "Enabled", bKillTrackerEnabled?"1":"0", INIFileName);
    sprintf_s (ToStr, "%d", reportMinutes);
    WritePrivateProfileString (szTemp, "DelayInMinutes", ToStr, INIFileName);

    WriteChatf("MQ2KillTracker :: \agSettings updated\ax");
}

void ShowStatus(void) {
    WriteChatf("\atMQ2KillTracker :: v%1.2f :: by Sym for RedGuides.com\ax", MQ2Version);
    WriteChatf("MQ2KillTracker :: %s", bKillTrackerEnabled?"\agENABLED\ax":"\arDISABLED\ax");
    if ( reportMinutes > 0) {
        WriteChatf ("MQ2KillTracker :: Auto reporting every \ag%d\ax minute%s", reportMinutes, reportMinutes==1?"":"s");
    } else {
        WriteChatf ("MQ2KillTracker :: Auto reporting \arDISABLED\ax");
    }

}

void ReportStats(void) {

    if (!killCount) {
        WriteChatf ("MQ2KillTracker :: \ayNo kills to report yet\ax");
        return;
    }
    DWORD RunningTime = GetTickCount() - StartTime;
    FLOAT RunningTimeFloat = (float)RunningTime / HOUR;
    FLOAT perHour = (float)killCount/RunningTimeFloat;

    if (RunningTimeFloat < 1) {
        RunningTimeFloat = (float)RunningTime / MINUTE;
    }

    WriteChatf ("MQ2KillTracker :: You have killed \ag%d\ax mob%s in \ag%.1f\ax %s%s. (\ag%1.2f\ax kills/hour average)",
        killCount,
        killCount == 1?"":"s",
        RunningTimeFloat,
        RunningTime / MINUTE >= 60?"hour":"minute",
        RunningTimeFloat == 1?"":"s",
        perHour
    );


}

void ResetStats(void) {
    StartTime = reportLastSent = GetTickCount();
    killCount = 0;
    WriteChatf("MQ2KillTracker :: \agStats have been reset\ax");
}

void ShowHelp (void) {
    WriteChatf ("\atMQ2KillTracker :: v%1.2f :: by Sym for RedGuides.com\ax", MQ2Version);
    WriteChatf ("/killtracker :: Lists command syntax.");
    WriteChatf ("/killtracker on|off :: Enables kill tracking. Default is \ag*ON*\ax.");
    WriteChatf ("/killtracker status :: Shows current status of plugin.");
    WriteChatf ("/killtracker report :: Shows current tracked kill count.");
    WriteChatf ("/killtracker reset :: Reset tracked kill count to 0.");
    WriteChatf ("/killtracker delay # :: Set time in minutes between kill count auto reports. Set to 0 to disable. Default is \ar60\ax.");
    WriteChatf ("/killcount :: Same as /killtracker report.");
}

VOID KillCountCommand (PSPAWNINFO pChar, PCHAR zLine) {
    ReportStats ();
}

VOID KillTrackerCommand (PSPAWNINFO pChar, PCHAR zLine) {
    char szTemp[MAX_STRING], szBuffer[MAX_STRING];

    GetArg (szTemp, zLine, 1);
    if ( !_strnicmp (szTemp, "on", 2) ) {
        bKillTrackerEnabled = true;
        WriteChatf ("MQ2KillTracker :: \agEnabled\ax");
        SaveINI ();
    } else if ( !_strnicmp (szTemp, "off", 2) ) {
        bKillTrackerEnabled = false;
        WriteChatf ("MQ2KillTracker :: \arDisabled\ax");
        SaveINI ();
    } else if ( !_stricmp (szTemp, "status") ) {
        ShowStatus ();
    } else if ( !_stricmp (szTemp, "report") ) {
        ReportStats ();
    } else if ( !_stricmp (szTemp, "reset") ) {
        ResetStats ();
    } else if ( !_stricmp (szTemp, "delay") ) {
        GetArg(szBuffer, zLine, 2);
        if(strlen(szBuffer)>0) {
            long lTemp = atoi(szBuffer);
            WriteChatf("MQ2KillTracker :: Auto report delay is \ag%d\ax minute%s", lTemp, lTemp == 1?"":"s");
            reportMinutes = lTemp;
            reportTimer = reportMinutes * MINUTE;
            SaveINI ();
        } else {
            ShowHelp ();
        }
    } else {
        ShowHelp ();
    }
}


PLUGIN_API VOID InitializePlugin(VOID) {
    DebugSpewAlways("Initializing MQ2KillTracker");
    AddCommand("/killtracker", KillTrackerCommand);
    AddCommand("/killcount", KillCountCommand);
}

PLUGIN_API VOID ShutdownPlugin(VOID) {
   DebugSpewAlways("Shutting down MQ2KillTracker");
   RemoveCommand("/killtracker");
   RemoveCommand("/killcount");
}

PLUGIN_API VOID OnPulse (VOID) {
    if (GetGameState() != GAMESTATE_INGAME) return;
    if ( SkipPulse == SKIP_PULSES ) {
        SkipPulse = 0;
        if (reportTimer > 0 && GetTickCount() - reportLastSent > (DWORD)reportTimer) {
            reportLastSent = GetTickCount ();
            WriteChatf ("MQ2KillTracker :: \agAuto Reporting\ax");
            ReportStats ();
        }
    }
    SkipPulse++;
}


PLUGIN_API DWORD OnIncomingChat (PCHAR Line, DWORD Color) {
    if ( !bKillTrackerEnabled ) return 0;

    if ( strstr (Line, "has been slain") || strstr (Line, "You have slain") ) {
        killCount++;
    }
    return 0;
}

PLUGIN_API VOID SetGameState(DWORD GameState) {
    DebugSpewAlways("MQ2KillTracker::SetGameState()");
    if (GameState==GAMESTATE_INGAME) {
        if ( !bInit ) {
            bInit = true;
            LoadINI ();
            StartTime = reportLastSent = GetTickCount ();
            ShowStatus ();
        }
    }
}
