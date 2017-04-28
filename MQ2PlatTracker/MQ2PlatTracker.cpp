// MQ2PlatTracker.cpp :: RedGuides exclusive plugin to track platinum changes
// v1.00 :: Sym - 2012-11-11
// v1.01 :: Sym - 2013-01-03 - Added plat per hour average
// v1.02 :: Sym - 2013-01-27 - Fixed timer reset for plat per hour average

#include "../MQ2Plugin.h"


PLUGIN_VERSION(1.02);
PreSetup("MQ2PlatTracker");


#define SECOND 1000
#define MINUTE (60 * SECOND)
#define HOUR (60 * MINUTE)
#define DAY (24 * HOUR)

//typedef struct _timestamp {
//  SYSTEMTIME systime;
//  DWORD      systicks;
//} TIMESTAMP;


// TIMESTAMP StartTime;
ULONGLONG StartTime = GetTickCount642();
unsigned int sCash = 0;
unsigned int cCash = 0;

bool didInit = false;
char szTemp[MAX_STRING];

void ShowHelp(void) {
	WriteChatf("\atMQ2PlatTracker :: v%1.2f :: by Sym for RedGuides.com\ax", MQ2Version);
	WriteChatf("/plat :: Lists command syntax");
	WriteChatf("/plat reset :: Resets starting amount of platinum to current platinum.");
	WriteChatf("/plat show :: Shows amount of platinum gained or lost since last reset along with the average per hour.");
}

void PlatTrackerCommand(PSPAWNINFO pChar, PCHAR szLine) {
    int b = 0;
    GetArg(szTemp,szLine,1);
    if(!_strnicmp(szTemp,"reset",5)) {
		sCash=GetCharInfo2()->Plat*1000+GetCharInfo2()->Gold*100+GetCharInfo2()->Silver*10+GetCharInfo2()->Copper;
		WriteChatf("MQ2PlatTracker :: \ayStarting amount reset to %.3f platinum\ax", float(sCash)/1000);
		StartTime = GetTickCount642();
        return;
    }
	if(!_strnicmp(szTemp,"show",5)) {
		cCash = GetCharInfo2()->Plat*1000+GetCharInfo2()->Gold*100+GetCharInfo2()->Silver*10+GetCharInfo2()->Copper;
	    ULONGLONG RunningTime = GetTickCount642() - StartTime;
	    FLOAT RunningTimeFloat = (float)RunningTime/HOUR;

		if (cCash > sCash) {
			WriteChatf("MQ2PlatTracker :: \agYou have gained a total of %.3f platinum (%.3f platinum per hour)\ax", float(cCash - sCash) / 1000, float(((cCash - sCash) / 1000)/RunningTimeFloat));
		} else if (cCash < sCash) {
			WriteChatf("MQ2PlatTracker :: \arYou have lost a total of %.3f platinum (%.3f platinum per hour)\ax", float(sCash - cCash) / 1000, float(((sCash - cCash) / 1000)/RunningTimeFloat));
		} else {
			WriteChatf("MQ2PlatTracker :: \ayNo platinum changes detected yet\ax");
		}
		return;
	}
	ShowHelp();
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID) {
    DebugSpewAlways("Initializing MQ2PlatTracker");

    //Add commands, MQ2Data items, hooks, etc.
    AddCommand("/plat",PlatTrackerCommand);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID) {
    DebugSpewAlways("Shutting down MQ2PlatTracker");

    RemoveCommand("/plat");
}



// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState) {
    DebugSpewAlways("MQ2PlatTracker::SetGameState()");
    if (GameState==GAMESTATE_INGAME && !didInit) {
		didInit = true;
		StartTime = GetTickCount642();
		sCash=GetCharInfo2()->Plat*1000+GetCharInfo2()->Gold*100+GetCharInfo2()->Silver*10+GetCharInfo2()->Copper;
		WriteChatf("\atMQ2PlatTracker :: v%1.2f :: by Sym for RedGuides.com\ax", MQ2Version);
	}
}

