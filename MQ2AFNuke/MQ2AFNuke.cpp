// MQ2AFNuke.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.
#include "MQ2AFNuke.h"
#ifndef PLUGIN_API
#include "../MQ2Plugin.h"
#endif PLUGIN_API
/*
#include <string>
#include <list>
#include <set>
#include <map>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
*/

PreSetup(PLUGIN_NAME);
PLUGIN_VERSION(PLUGIN_VERS);

#include "..\MQ2Heals\MyTimer.h"
#include "..\MQ2Heals\Character.h"

#include "Wizard.h"
#include "Magician.h"
#include "Enchanter.h"
#include "Beastlord.h"
#include "..\MQ2Heals\Druid.h"


#ifndef DNotify
#define DNotify if (DoDebug) Notify
#endif

// My Structures
typedef struct _MYSPELL_INFO
{
	CHAR Name[MAX_SPELL];
	INT Gem;
	INT Type;
}MYSPELL_INFO, *PMYSPELL_INFO;

// My Classes
// Global Variables
Character* Me = NULL;
//Some of these are stolen from FastHeal
bool AutoRez = true;
bool ReleaseAndCampfire = false;
bool campfiretimer = false;
MyTimer ReleaseCampfireTimer;
bool CampFireOnZone = false;
bool CampFireResume = false;
bool DoReviveMerc = true;
bool DoMed = true;
bool DoMemSpells = false;
int MedPct = 98;
bool OutDoors = false;
bool DoMount = false;
int BuffGem = 10;
bool DoCombatBuffs = false;
bool DoBuffs = true;
bool *pbStickOn = NULL;
bool CombatDummy = false;
bool PauseOnZone = false;
bool ManageAggro = true;
bool MakeVisible = false;
bool Dummy = false;
bool Harvest = true;
int HarvestAt = 75;
int AssistAt = 99;
int StopAt = 0;
bool DoDebug = false;
bool DoMez = false;
bool DoBurn = true;
bool Megaburn = false;
int MezThreshold = 2;
bool ManualAssist=false;
bool StartNuking=false;
float CampRadius=50.0;
int MobBurnThreshold=2;
bool BurnNamed=true;
bool DoAANukes = true;
bool BreakInvis = false;

// These will not be saved in .ini... Just for temporarily disabling spells (Think Velishan's Revenge, Enslaver of Souls, etc...)
bool DoFire = true; 
bool DoIce = true;
bool DoMagic = true;

// Mount
CHAR MountItem[MAX_SPELL] = "Verdant Hedgerow Leaf";

// Buffs
// Timers
bool NukesOn = false;		// True=enabled, false=disabled
bool NukePause = false;		// True=paused, false=running
bool RezOn = true;			// True=rez corpses
int NukeState = 0;
PSPAWNINFO pNukeTarget; // Target for incoming nuke

bool Initialized = false;
int CastState = 0;
bool UseEqbc = false;

// Function prototypes
void ShowHelp();
void ShowSettings();
int PlayerPctHPs(EQPlayer *player);
int SpawnPctHPs(PSPAWNINFO spawn);
void Notify(PCHAR Plugin, PCHAR Function, PCHAR Format, ...);
void DoCommandf(PCHAR Format, ...);	// Formatted DoCommand/EzCommand
long Evaluate(PCHAR zFormat, ...);
bool Hovering(SPAWNINFO *pSpawn);
bool IsCorpse(SPAWNINFO *pSpawn);
bool SetBOOL(bool Cur, PCHAR Val, PCHAR Sec, PCHAR Key);
int SetINT(int Cur, PCHAR Val, PCHAR Sec, PCHAR Key);
bool IsStickOn();
void LoadIni();
void SaveIni();
int SelectResurrect(bool bind = false);

// Functions
bool IsStickOn()
{
	if (!pbStickOn)
	{
		PMQPLUGIN pLook = pPlugins;
		while (pLook && _strnicmp(pLook->szFilename, "mq2moveutils", 12)) pLook = pLook->pNext;
		if (pLook)
			pbStickOn = (bool *)GetProcAddress(pLook->hModule, "bStickOn");
	}
	if (pbStickOn) return *pbStickOn;
	return false;
}

bool EQBCConnected()
{
	typedef WORD(__cdecl *fEqbcIsConnected)(VOID);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "mq2eqbc", 8)) pLook = pLook->pNext;
	if (pLook)
		if (fEqbcIsConnected checkf = (fEqbcIsConnected)GetProcAddress(pLook->hModule, "isConnected"))
			if (checkf()) return true;
	return false;
}

void NukeCommand(PSPAWNINFO pCHAR, PCHAR zLine)
{
	bool help = false;
	char Param1[MAX_STRING];
	char Param2[MAX_STRING];
	GetArg(Param1, zLine, 1);
	GetArg(Param2, zLine, 2);

	if (!_stricmp("help", Param1)) help = true;
	else if (!_stricmp("loadini", Param1))
		LoadIni();
	else if (!_stricmp("saveini", Param1))
		SaveIni();
	else if (!_stricmp("on", Param1))
		NukesOn = SetBOOL(NukesOn, Param1, PLUGIN_NAME, "NukesOn");
	else if (!_stricmp("off", Param1))
		NukesOn = SetBOOL(NukesOn, Param1, PLUGIN_NAME, "NukesOn");
	else if (!_stricmp("pauseonzone", Param1))
		PauseOnZone = SetBOOL(PauseOnZone, Param2, PLUGIN_NAME, "PauseOnZone");
	else if (!_stricmp("pause", Param1))
	{
		NukePause = true;
		Notify(PLUGIN_NAME, "Pause", "(\arPaused\ax)");
	}
	else if (!_stricmp("resume", Param1))
	{
		NukePause = false;
		NukesOn = true;
		Notify(PLUGIN_NAME, "Pause", "(\agResumed\ax) and (\agEnabled\ax)");
	}
	else if (!_stricmp("campfirerez", Param1))
		ReleaseAndCampfire = SetBOOL(ReleaseAndCampfire, Param2, PLUGIN_NAME, "CampFireRez");
	else if (((!_stricmp("medpct", Param1)) && IsNumber(Param2)))
		MedPct = SetINT(MedPct, Param2, PLUGIN_NAME, "MedPct");
	else if (((!_stricmp("assistat", Param1)) && IsNumber(Param2)))
		AssistAt = SetINT(AssistAt, Param2, PLUGIN_NAME, "AssistAt");
	else if (((!_stricmp("harvestat", Param1)) && IsNumber(Param2)))
		HarvestAt = SetINT(HarvestAt, Param2, PLUGIN_NAME, "HarvestAt");
	else if (((!_stricmp("harvest", Param1)) && IsNumber(Param2)))
		Harvest = SetBOOL(Harvest, Param2, PLUGIN_NAME, "Harvest");
	else if (((!_stricmp("stopat", Param1)) && IsNumber(Param2)))
		StopAt = SetINT(StopAt, Param2, PLUGIN_NAME, "StopAt");
	else if (((!_stricmp("mezthreshold", Param1)) && IsNumber(Param2)))
		MezThreshold = SetINT(StopAt, Param2, PLUGIN_NAME, "MezThreshold");
	else if (!_stricmp("domed", Param1))
		DoMed = SetBOOL(DoMed, Param2, PLUGIN_NAME, "DoMed");
	else if (!_stricmp("domez", Param1))
		DoMez = SetBOOL(DoMez, Param2, PLUGIN_NAME, "DoMez");
	else if (!_stricmp("doaanukes", Param1))
		DoAANukes = SetBOOL(DoAANukes, Param2, PLUGIN_NAME, "DoAANukes");
	else if (!_stricmp("domount", Param1))
		DoMount = SetBOOL(DoMount, Param2, PLUGIN_NAME, "DoMount");
	else if (!_stricmp("doburn", Param1))
		DoBurn = SetBOOL(DoMount, Param2, PLUGIN_NAME, "DoBurn");
	else if (!_stricmp("debug", Param1))
		DoDebug = SetBOOL(DoDebug, Param2, PLUGIN_NAME, "Debug");
	else if (!_stricmp("manualassist", Param1))
		ManualAssist = SetBOOL(ManualAssist, Param2, PLUGIN_NAME, "ManualAssist");
	else if (!_stricmp("dummy", Param1))
		Dummy = SetBOOL(Dummy, Param2, PLUGIN_NAME, "Dummy");
	else if (!_stricmp("breakinvis", Param1))
		BreakInvis = SetBOOL(BreakInvis, Param2, PLUGIN_NAME, "BreakInvis");
	else if (!_stricmp("fire", Param1))
		DoFire = SetBOOL(DoFire, Param2, PLUGIN_NAME, "Fire");
	else if (!_stricmp("cold", Param1))
		DoIce = SetBOOL(DoIce , Param2, PLUGIN_NAME, "Cold");
	else if (!_stricmp("magic", Param1))
		DoMagic = SetBOOL(DoMagic, Param2, PLUGIN_NAME, "Magic");
	else if (!_stricmp("start", Param1))
	{
		if (ManualAssist)
		{
			StartNuking=true;
		}
	}
	else if (!_stricmp("stop", Param1))
	{
		StartNuking=false;
	}
	else if ((!_stricmp("mount", Param1)) || (!_stricmp("mountitem", Param1)))
	{
		if (Param2 && Param2[0])
		{
			memset(MountItem, 0, MAX_SPELL);
			strncpy_s(MountItem, Param2, strlen(Param2) < MAX_SPELL ? strlen(Param2) : MAX_SPELL);
			WritePrivateProfileString(PLUGIN_NAME, "MountItem", MountItem, INIFileName);
			Notify(PLUGIN_NAME, "MountItem", "(\ag%s\ax)", MountItem);
		}
	}
	else if (!strcmp("settings", Param1))
	{
		ShowSettings();
	}
	else if (!strcmp("memorize", Param1))
	{
		if (Me->Class() == Wizard)
		{
			CWizard *wiz = (CWizard *)(Me);
			wiz->MemorizeSpells();
		}
	}
	if ((!_stricmp("", Param1)) || help)
	{
		ShowHelp();
	}
}

// Sorta borrowed from MQ2Rez
void ShowHelp()
{
	Notify(NULL, NULL, "Usage:");
	Notify(NULL, NULL, "==========Plugin Options/Help==========");
	Notify(NULL, NULL, "/nuke -> displays this help");
	Notify(NULL, NULL, "/nuke help -> displays this help");
	Notify(NULL, NULL, "/nuke settings -> displays settings");
	Notify(NULL, NULL, "/nuke on|off -> Enable/Disable plugin");
	Notify(NULL, NULL, "/nuke debug on|off -> Enable/Disable debug output (MQ2 chat spam!!!)");
	Notify(NULL, NULL, "/nuke pause|resume -> Pause/Resume plugin action");
	Notify(NULL, NULL, "/nuke pauseonzone on|off -> Pause/Resume plugin when changing zones");
	Notify(NULL, NULL, "/nuke campfirerez on|off -> Toggle release hover, click campfire, and wait for rez");
	Notify(NULL, NULL, "/nuke manualassist on|off -> Manually target mobs, and use /nuke start to kill.");
	Notify(NULL, NULL, "/nuke start -> Start nuking mob on manual targeting.");
	Notify(NULL, NULL, "/nuke stop -> Stop nuking mob on manual targeting.");
	Notify(NULL, NULL, "/nuke breakinvis on|off -> Break invis to start nuking (not Harvest).");
	Notify(NULL, NULL, "=============DPS Options================");
	Notify(NULL, NULL, "/nuke assistat # -> Assist the MA/MT at #% HPs (default: 99)");
	Notify(NULL, NULL, "/nuke stopat # -> Stop assisting the MA/MT at #% HPs (default: 0)");
	Notify(NULL, NULL, "/nuke burn on|off -> Turn automatic burns on/off.");
	Notify(NULL, NULL, "/nuke doaanukes on|off -> Toggle AA Nukes on/off");
	Notify(NULL, NULL, "/nuke fire on|off -> Toggle Fire Nukes on/off");
	Notify(NULL, NULL, "/nuke magic on|off -> Toggle Magic Nukes on/off");
	Notify(NULL, NULL, "/nuke cold on|off -> Toggle Cold Nukes on/off");
	Notify(NULL, NULL, "/nuke reset -> Toggle Fire/Cold/Magic Nukes on (default)");
	Notify(NULL, NULL, "=============Med Options================");
	Notify(NULL, NULL, "/nuke domed on|off -> Toggle Meditate on/off");
	Notify(NULL, NULL, "/nuke harvest on|off -> Toggle Harvest abilities/items on/off");
	Notify(NULL, NULL, "/nuke harvestat # -> Use Harvest spells/abilities at #% mana");
	Notify(NULL, NULL, "/nuke medpct # -> Toggle Meditate at # percentage mana");
	Notify(NULL, NULL, "/nuke domount on|off -> Toggle Mount on/off");
	Notify(NULL, NULL, "/nuke mountitem \"Name of Mount Item\" -> Use this mount item");
	Notify(NULL, NULL, "============Buff Options================");
	Notify(NULL, NULL, "/nuke buffs on|off -> Toggle Buff Spells while idle");
	Notify(NULL, NULL, "/nuke combatbuff on|off -> Toggle buffs while in combat");
	Notify(NULL, NULL, "/nuke buffgem # -> Spell gem # for buffs");
	Notify(NULL, NULL, "===========Debuff Options===============");
	Notify(NULL, NULL, "/nuke domez on|off -> Toggle Mezmerize on/off");
	Notify(NULL, NULL, "/nuke mezthreshold # -> # of XTargets required to mez");
	Notify(NULL, NULL, "============Test Options================");
	Notify(NULL, NULL, "/nuke dummy on|off -> Test spell rotation on combat dummy.");
	Notify(NULL, NULL, "/nuke memorize -> Memorize spells for caster.");


	//	Notify(NULL, NULL, "/nuke rez filter [filter] -> Rez according to filter (group, raid, xtarget, target, player)");
	//	Notify(NULL, NULL, "  Ex: /nuke rez group (rez anyone in group)");
	//	Notify(NULL, NULL, "  Ex: /nuke rez guild group (rez anyone in guild or group)");
	//	Notify(NULL, NULL, "  Ex: /nuke rez raid guild (rez anyone in raid or guild)");
	//	Notify(NULL, NULL, "  Ex: /nuke rez zone (rez anyone in the zone)");
}

// Sorta borrowed from MQ2Rez
/*
*/
void ShowSettings()
{
	Notify(NULL, NULL, "=============Plugin Options=============");
	Notify(PLUGIN_NAME, "Plugin", "(\ag%s\ax).", (NukesOn ? "Enabled" : "Disabled"));
	Notify(PLUGIN_NAME, "Pause", "(\ag%s\ax).", (NukePause ? "Paused" : "Running"));
	Notify(PLUGIN_NAME, "Debug", "(\ag%s\ax).", (DoDebug ? "on" : "off"));
	Notify(PLUGIN_NAME, "PauseOnZone", "(\ag%s\ax).", (PauseOnZone ? "on" : "off"));
	Notify(PLUGIN_NAME, "ManualAssist", "(\ag%s\ax).", (ManualAssist ? "on" : "off"));
	Notify(PLUGIN_NAME, "BreakInvis", "(\ag%s\ax).", (BreakInvis ? "on" : "off"));
	Notify(NULL, NULL, "==============DPS Options===============");
	Notify(PLUGIN_NAME, "AssistAt", "(\ag%i\ax).", AssistAt);
	Notify(PLUGIN_NAME, "StopAt", "(\ag%i\ax).", StopAt);
	Notify(PLUGIN_NAME, "Burn", "(\ag%s\ax).", (DoBurn ? "on" : "off"));
	Notify(PLUGIN_NAME, "DoAANukes", "(\ag%s\ax).", (DoAANukes ? "on" : "off"));
	Notify(PLUGIN_NAME, "Fire", "(\ag%s\ax).", (DoFire ? "on" : "off"));
	Notify(PLUGIN_NAME, "Ice", "(\ag%s\ax).", (DoIce ? "on" : "off"));
	Notify(PLUGIN_NAME, "Magic", "(\ag%s\ax).", (DoMagic ? "on" : "off"));
	Notify(NULL, NULL, "==================Rez===================");
	Notify(PLUGIN_NAME, "CampFireRez", "(\ag%s\ax).", (ReleaseAndCampfire ? "on" : "off"));
	Notify(PLUGIN_NAME, "ReviveMerc", "(\ag%s\ax).", (DoReviveMerc ? "on" : "off"));
	Notify(NULL, NULL, "==================Mez===================");
	Notify(PLUGIN_NAME, "Mez", "(\ag%s\ax).", (DoMez ? "on" : "off"));
	Notify(PLUGIN_NAME, "MezThreshold", "(\ag%d\ax).", MezThreshold);
	Notify(NULL, NULL, "==================Med===================");
	Notify(PLUGIN_NAME, "Med", "(\ag%s\ax).", (DoMed ? "on" : "off"));
	Notify(PLUGIN_NAME, "MedPct", "(\ag%d\ax).", MedPct);
	Notify(PLUGIN_NAME, "Harvest", "(\ag%s\ax).", (Harvest ? "on" : "off"));
	Notify(PLUGIN_NAME, "HarvestAt", "(\ag%d\ax).", HarvestAt);
	Notify(PLUGIN_NAME, "MountItem", "(\ag%s\ax).", MountItem);
	Notify(PLUGIN_NAME, "DoMount", "(\ag%s\ax).", (DoMount ? "on" : "off"));
	Notify(NULL, NULL, "============Test Options================");
	Notify(PLUGIN_NAME, "Dummy", "(\ag%s\ax).", (Dummy ? "on" : "off"));
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	DebugSpewAlways("Initializing MQ2AFNuke");
	if (EQBCConnected())
		UseEqbc = true;
	else
		UseEqbc = false;

	//Add commands, MQ2Data items, hooks, etc.
	AddCommand("/nuke", NukeCommand);
	Me = NULL;
	//AddXMLFile("MQUI_MyXMLFile.xml");
	//bmMyBenchmark=AddMQ2Benchmark("My Benchmark Name");
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("Shutting down MQ2AFNuke");

	//Remove commands, MQ2Data items, hooks, etc.
	//RemoveMQ2Benchmark(bmMyBenchmark);
	if (Me)
	{
		delete Me;
		Me = NULL;
	}
	RemoveCommand("/nuke");
	//RemoveXMLFile("MQUI_MyXMLFile.xml");
}

// Called after entering a new zone
PLUGIN_API VOID OnZoned(VOID)
{
	DebugSpewAlways("MQ2AFNuke::OnZoned()");
	if (!PauseOnZone) return;
	if (NukesOn)
	{
		//	WriteChatColor("Zone change detected... Heals paused...");
		//	WriteChatColor("Use: '/heals resume' to resume heals.");
		if (!CampFireResume)	// Resume heal logic if we campfire'd back to zone...
		{
			NukePause = true;
			Notify(PLUGIN_NAME, "OnZoned", "Zone change detected... Nukes paused...");
			Notify(PLUGIN_NAME, "OnZoned", "Use: '/nuke resume' to resume Nukes.");
		}
		else
		{
			NukePause = false;
			CampFireResume = false;
			Notify(PLUGIN_NAME, "OnZoned", "Attempt to use campfire clicky and just stand there waiting for a rez...");
		}
	}
}

void GetBool(bool &ResultVar, char *IniVarName, bool default = false)
{
	if (!IniVarName) return;
	ResultVar = GetPrivateProfileInt(PLUGIN_NAME, IniVarName, default ? 1 : 0, INIFileName) ? true : false;
}

void SetBool(bool Var, char *IniVarName)
{
	if (!IniVarName) return;
	WritePrivateProfileString(PLUGIN_NAME, IniVarName, Var ? "1" : "0", INIFileName);
}

// Stolen from MQ2Rez - jimbob
bool SetBOOL(bool Cur, PCHAR Val, PCHAR Sec, PCHAR Key) {
	//char buffer[128];
	bool result = false;
	if (!_strnicmp(Val, "false", 5) || !_strnicmp(Val, "off", 3) || !_strnicmp(Val, "0", 1))    result = false;
	else if (!_strnicmp(Val, "true", 4) || !_strnicmp(Val, "on", 2) || !_strnicmp(Val, "1", 1)) result = true;
	else result = (!Cur) & 1;
	if (Sec[0] && Key[0]) WritePrivateProfileString(Sec, Key, result ? "1" : "0", INIFileName);
	Notify(PLUGIN_NAME, Key, "(%s)", result ? "\agon\ax" : "\agoff\ax");
	//sprintf(buffer, "%s::%s (%s)", Sec, Key, result ? "\agon\ax" : "\agoff\ax");
	//WriteChatColor(buffer);
	return result;
}

int SetINT(int Cur, PCHAR Val, PCHAR Sec, PCHAR Key)
{
	int result = Cur;
	if (IsNumber(Val))
	{
		if (Sec[0] && Key[0]) WritePrivateProfileString(Sec, Key, Val, INIFileName);
		result = atoi(Val);
	}
	Notify(PLUGIN_NAME, Key, "(\ag%s\ax)", Val);
	return result;
}

void LoadIni()
{
	PCHAR NewMountItem[MAX_SPELL] = { 0 };
	DWORD ret = 0;
	_snprintf_s(INIFileName, 260, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
	NukesOn = GetPrivateProfileInt(PLUGIN_NAME, "Enabled", 0, INIFileName) ? true : false;
	ReleaseAndCampfire = GetPrivateProfileInt(PLUGIN_NAME, "CampFireRez", 0, INIFileName) ? true : false;
	DoReviveMerc = GetPrivateProfileInt(PLUGIN_NAME, "DoReviveMerc", 1, INIFileName) ? true : false;
	DoDebug = GetPrivateProfileInt(PLUGIN_NAME, "Debug", 0, INIFileName) ? true : false;
	Dummy = GetPrivateProfileInt(PLUGIN_NAME, "Dummy", 0, INIFileName) ? true : false;
	ManualAssist = GetPrivateProfileInt(PLUGIN_NAME, "ManualAssist", 0, INIFileName) ? true : false;
	BreakInvis = GetPrivateProfileInt(PLUGIN_NAME, "BreakInvis", 0, INIFileName) ? true : false;

	//DPSMode			=	GetPrivateProfileInt(PLUGIN_NAME, "DPSMode", 0, INIFileName);
	DoMemSpells = GetPrivateProfileInt(PLUGIN_NAME, "MemSpells", 0, INIFileName) ? true : false;
	DoMed = GetPrivateProfileInt(PLUGIN_NAME, "DoMed", 1, INIFileName) ? true : false;
	Harvest = GetPrivateProfileInt(PLUGIN_NAME, "Harvest", 1, INIFileName) ? true : false;
	DoMount = GetPrivateProfileInt(PLUGIN_NAME, "DoMount", 1, INIFileName) ? true : false;
	DoBurn = GetPrivateProfileInt(PLUGIN_NAME, "DoBurn", 1, INIFileName) ? true : false;
	DoAANukes = GetPrivateProfileInt(PLUGIN_NAME, "DoAANukes", 1, INIFileName) ? true : false;
	MedPct = GetPrivateProfileInt(PLUGIN_NAME, "MedPct", 98, INIFileName);
	AssistAt = GetPrivateProfileInt(PLUGIN_NAME, "AssistAt", 99, INIFileName);
	HarvestAt = GetPrivateProfileInt(PLUGIN_NAME, "HarvestAt", 75, INIFileName);
	StopAt = GetPrivateProfileInt(PLUGIN_NAME, "StopAt", 0, INIFileName);
	DoMez = GetPrivateProfileInt(PLUGIN_NAME, "DoMez", 0, INIFileName) ? true : false;
	MezThreshold = GetPrivateProfileInt(PLUGIN_NAME, "MezThreshold", 2, INIFileName);
	PauseOnZone = GetPrivateProfileInt(PLUGIN_NAME, "PauseOnZone", 0, INIFileName) ? true : false;
	ret = GetPrivateProfileString(PLUGIN_NAME, "MountItem", MountItem, (LPSTR)NewMountItem, MAX_SPELL, INIFileName);
	if (ret) strncpy_s(MountItem, (const char *)NewMountItem, MAX_SPELL<(ret) ? MAX_SPELL : ret);
	Initialized = true;
}

BOOL WINAPI WritePrivateProfileInt(LPCSTR AppName, LPCSTR KeyName, INT Value, LPCSTR Filename)
{
	CHAR Temp[1024] = { 0 };
	_snprintf_s(Temp, 1023, "%i", Value);
	return WritePrivateProfileString(AppName, KeyName, Temp, Filename);
}

void SaveIni()
{
	_snprintf_s(INIFileName, 260, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
	WritePrivateProfileString(PLUGIN_NAME, "Enaled", NukesOn ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "CampFireRez", ReleaseAndCampfire ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoReviveMerc", DoReviveMerc ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "PauseOnZone", PauseOnZone ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "MemSpells", DoMemSpells ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoMed", DoMed ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoMez", DoMez ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoBurn", DoBurn ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoAANukes", DoAANukes ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "Harvest", Harvest ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "ManualAssist", ManualAssist ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "BreakInvis", BreakInvis ? "1" : "0", INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "AssistAt", AssistAt, INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "HarvestAt", HarvestAt, INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoMount", DoMount ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "Dummy", Dummy ? "1" : "0", INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "MezThreshold", MezThreshold, INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "MedPct", MedPct, INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "StopAt", StopAt, INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "MountItem", MountItem, INIFileName);
}

// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState)
{
	//    DebugSpewAlways("MQ2AFNuke::SetGameState()");
	if (GameState == GAMESTATE_INGAME)
	{
		if ((!Initialized) || (!Me))
		{
			LoadIni();
			if (Me)
			{
				delete Me;
				Me = NULL;
			}
			switch (GetCharInfo2()->Class)
			{
				case Wizard:
					Me = (Character *) new CWizard;
					Notify(PLUGIN_NAME, "SetGameState", "Configured for Wizard Nuking!");
				break;
				case Mage:
					Me = (Character *) new CMagician;
					Notify(PLUGIN_NAME, "SetGameState", "Configured for Mage Nuking!");
				break;
				case Enchanter:
					Me = (Character *) new CEnchanter;
					Notify(PLUGIN_NAME, "SetGameState", "Configured for Enchanter Nuking!");
				break;
				case Beastlord:
					Me = (Character *) new CBeastlord;
					Notify(PLUGIN_NAME, "SetGameState", "Configured for Beastlord Nuking!");
				break;
				case Druid:
					Me = (Character *) new CDruid;
					Notify(PLUGIN_NAME, "SetGameState", "Configured for Druid Nuking!");
				break;
/*
			case Cleric:
				Me = (Healer *) new CCleric;
				Notify(PLUGIN_NAME, "SetGameState", "Configured for Cleric healing!");
				break;
			case Druid:
				Me = (Healer *) new CDruid;
				Notify(PLUGIN_NAME, "SetGameState", "Configured for Druid healing!");
				break;
			case Shaman:
				Me = (Healer *) new CShaman;
				Notify(PLUGIN_NAME, "SetGameState", "Configured for Shaman healing!");
				break;
*/
				default:
					Notify(PLUGIN_NAME, "SetGameState", "Unsupported class...");
				break;
			}

			NukePause = false;
			Initialized = true;
		}
	}
	else if (GameState != GAMESTATE_LOGGINGIN)
	{
		if (Initialized)
		{
			NukeState = NUKESTATE_IDLE;
			NukePause = true;
			Initialized = false;
			NukesOn = false;
			Me = NULL;
		}
	}
	// create custom windows if theyre not set up, etc
}

int Pct(int cur, int max)
{
	if (max != 0)
		return cur * 100 / max;
	else
		return 0;
}

int PlayerPctHPs(EQPlayer *player)
{
	if (!player) return 0;
	return Pct(player->Data.HPCurrent, player->Data.HPMax);
}

int SpawnPctHPs(PSPAWNINFO spawn)
{
	if (!spawn) return 0;
	return Pct(spawn->HPCurrent, spawn->HPMax);
}

PSPAWNINFO GetAssistNPC()
{
	DWORD nAssist;
	PCHARINFO info=GetCharInfo();
	PSPAWNINFO pMA=NULL;
	if(!info) return NULL;
	if (info->pSpawn->GroupAssistNPC[0] != 0)
	{
		EQPlayer *spawn;
		spawn = GetSpawnByID(info->pSpawn->GroupAssistNPC[0]);
		if (spawn) return spawn->Data.mPlayerPhysicsClient.pSpawn;
	}
	for (nAssist = 0; nAssist < 3; nAssist++)
	{
		if (GetCharInfo()->pSpawn->RaidAssistNPC[nAssist] != 0)
		{
			EQPlayer *spawn;
			spawn = GetSpawnByID(info->pSpawn->RaidAssistNPC[nAssist]);
			if (spawn) return spawn->Data.mPlayerPhysicsClient.pSpawn;
		}
	}
	if(!info->pGroupInfo) return NULL;
	if(!info->pGroupInfo->pMember) return NULL;
	for(int x=0; x<6; x++)
	{
		if(!info->pGroupInfo->pMember[x]) continue;
		if(info->pGroupInfo->pMember[x]->MainAssist)
			return info->pGroupInfo->pMember[x]->pSpawn;
		else if(info->pGroupInfo->pMember[x]->MainTank)
			pMA=info->pGroupInfo->pMember[x]->pSpawn;
	}
	return pMA;
}

bool CheckForNukes()
{
	PCHARINFO info = GetCharInfo();
	if (Me->Invis()&&!BreakInvis) return false;
	if (Me->Moving()) return false;
	if (!Me->InCombat() && !Dummy && !ManualAssist) return false;

	if (Dummy)
	{
		EQPlayer* lpTarget = pTarget;
		if (lpTarget!=NULL)
		{
			if (strstr(pTarget->Data.mPlayerPhysicsClient.pSpawn->Name, "Combat_Dummy") != NULL)
			{
				pNukeTarget = pTarget->Data.mPlayerPhysicsClient.pSpawn;
				NukeState = NUKESTATE_CASTSTART;
				return true;
			}
		}
		return false;
	}
	if (ManualAssist)
	{
		if (!StartNuking) return false;
		EQPlayer* lpTarget = pTarget;
		if (lpTarget!=NULL)
		{
			if(GetSpawnType(lpTarget->Data.mPlayerPhysicsClient.pSpawn)==NPC)
			{
				pNukeTarget = pTarget->Data.mPlayerPhysicsClient.pSpawn;
				NukeState = NUKESTATE_CASTSTART;
				return true;
			}
			else
				StartNuking=false;
		}
		else
			StartNuking=false;
		return false;
	}
	PSPAWNINFO pSpawn = GetAssistNPC();
	//if (Me->Sitting()) return false;
	if (pSpawn)
	{
		if ((GetSpawnType(pSpawn) == NPC)
			&& (SpawnPctHPs(pSpawn) <= AssistAt)
			&& (SpawnPctHPs(pSpawn) >= StopAt)
			&& LineOfSight(info->pSpawn, pSpawn))
		{
			pNukeTarget = pSpawn;
			NukeState = NUKESTATE_CASTSTART;
			return true;
		}
	}
	else
	{
		pNukeTarget = NULL;
		NukeState = NUKESTATE_IDLE;
	}
	return false;
}

bool DismissFamiliar()
{
	if (Me->GetPetID() && (Me->Class()==Wizard))
	{
		EQPlayer *player = GetSpawnByID(Me->GetPetID());
		if (player)
		{
			EzCommand("/pet get lost");
			return true;
		}
	}
	return false;
}

// Stolen from MQ2Cast, and modifried by jimbob
long Evaluate(PCHAR zFormat, ...)
{
	char zOutput[MAX_STRING] = { 0 };
	va_list vaList;
	va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);
	if (!zOutput[0]) return -1;	//Error - no input data...
	ParseMacroData(zOutput, MAX_STRING);
	if (IsNumber(zOutput))
		return atoi(zOutput);
	else
	{
		if (_strnicmp(zOutput, "true", 4) == 0)
			return 1;
		else if (_strnicmp(zOutput, "false", 5) == 0)
			return 0;
	}
	return -3;	// Error - unknown output...
}

bool CastNextFreeNuke()
{
	string NukeSpell;
	NukeSpell.clear();
	if (!DoAANukes) return false;

	if (!pNukeTarget)
	{
		DNotify(PLUGIN_NAME, "CastFreeNuke", "No target to nuke.");
		NukeState = NUKESTATE_IDLE;
		CastState = CASTSTATE_STARTED;
		return false;
	}
	if (Me->Class() == Wizard)
	{
		float dist=GetDistance(GetCharInfo()->pSpawn, pNukeTarget);
		CWizard *wizard = (CWizard *)Me;
		if (DoFire && wizard->FreeFireNukeReady(NukeSpell, dist)) {}
		else if(DoIce && wizard->FreeIceNukeReady(NukeSpell, dist)) {}
		else if (DoMagic && wizard->FreeMagicNukeReady(NukeSpell, dist)) {}
		else if (wizard->FreeMiscNukeReady(NukeSpell, dist)) {}
		if(NukeSpell.length()>0)
		{
			DoCommandf("/casting \"%s\" alt -targetid|%d", NukeSpell.c_str(), pNukeTarget->SpawnID);
			DNotify(PLUGIN_NAME, "CastFreeNuke", "Casting AA Nuke <\ar%s\ax> on <\ay%s\ax>", NukeSpell.c_str(), pNukeTarget->Name);
			return true;
		}
	}
	if (Me->Class() == Mage)
	{
		float dist = GetDistance(GetCharInfo()->pSpawn, pNukeTarget);
		CMagician *mage = (CMagician *)Me;
		if (mage->FreeNukeReady(NukeSpell, dist))
		{
			DoCommandf("/casting \"%s\" alt -targetid|%d", NukeSpell.c_str(), pNukeTarget->SpawnID);
			DNotify(PLUGIN_NAME, "CastFreeNuke", "Casting AA Nuke <\ar%s\ax> on <\ay%s\ax>", NukeSpell.c_str(), pNukeTarget->Name);
			return true;
		}
		else if (mage->NextItemClicky(NukeSpell, dist))
		{
			DoCommandf("/casting \"%s\" item -targetid|%d", NukeSpell.c_str(), pNukeTarget->SpawnID);
			DNotify(PLUGIN_NAME, "CastFreeNuke", "Casting Item Clicky Nuke <\ar%s\ax> on <\ay%s\ax>", NukeSpell.c_str(), pNukeTarget->Name);
			return true;
		}
	}
	if (Me->Class() == Druid)
	{
		float dist = GetDistance(GetCharInfo()->pSpawn, pNukeTarget);
		CDruid *druid = (CDruid *)Me;
		if (druid->FreeNukeReady(NukeSpell, dist))
		{
			DoCommandf("/casting \"%s\" alt -targetid|%d", NukeSpell.c_str(), pNukeTarget->SpawnID);
			DNotify(PLUGIN_NAME, "CastFreeNuke", "Casting AA Nuke <\ar%s\ax> on <\ay%s\ax>", NukeSpell.c_str(), pNukeTarget->Name);
			return true;
		}
	}
	return false;
}

bool CastNextNuke()
{
	char tmp[MAX_STRING] = { 0 };
	//char * NukeSpell = NULL;
	int HealType = HEALTYPE_SINGLE;
	string NukeSpell;
	NukeSpell.clear();

	if (!pNukeTarget)
	{
		DNotify(PLUGIN_NAME, "CastNextNuke", "No target to nuke.");
		NukeState = NUKESTATE_IDLE;
		CastState = CASTSTATE_STARTED;
		return false;
	}

	// Remove Targeting logic, use `/casting "Spell" -targetid|TargetID` instead
	if (CastState == CASTSTATE_STARTED) CastState = CASTSTATE_TARGETING;
	switch (CastState)
	{
	case CASTSTATE_STARTED:
	case CASTSTATE_TARGETING:
	{
		bool alt = false;
		bool item = false;
		bool targetid = true;
		bool new_target = false;
		static PSPAWNINFO last_target=NULL;

		int PctHPs = SpawnPctHPs(pNukeTarget);
		if (last_target == pNukeTarget)
		{
			new_target = true;
			last_target = pNukeTarget;
		}
		float dist = GetDistance(GetCharInfo()->pSpawn, pNukeTarget);
		if (((PctHPs >= StopAt) && (PctHPs <= AssistAt)) || Dummy || ManualAssist)
		{
			switch (Me->Class())
			{
				case Wizard:
				{
					bool EncSynergy = Me->ShortBuff("Beguiler's Synergy");
					bool Haze = Me->ShortBuff("Chromatic Haze", true) | Me->AnyBuff("Gift of Hazy Thoughts", true) | EncSynergy;
					bool ITC = Me->Buff("Improved Twincast", true) | Me->Buff("Twincast");
					bool LowMana = (Me->PctMana() < 20) ? true : false;
					bool GoM = Me->ShortBuff("Gift of Mana", true);
					bool Gambit = Me->ShortBuff("Gambit", true);
					bool Burn = Me->ShortBuff("Devastation", true) | Me->ShortBuff("Destruction",true);
					bool Spire2 = Me->Buff("Second Spire of Arcanum", true);
					int MobsInCamp=Me->XTargetHaters(0, false, CampRadius);
					int NamedMobInCamp=Me->XTargetHaters(0, true, CampRadius);
					CWizard *Wiz = (CWizard *)Me;
					if(DoBurn && ((MobsInCamp>MobBurnThreshold) || (BurnNamed&&(NamedMobInCamp>0))))
					{
						string BurnSpell;
						bool alt=true;
						do
						{
							if (!Burn || Megaburn)
							{
								if (Me->IsAAReady("Frenzied Devastation"))
								{
									BurnSpell = "Frenzied Devastation";
									break;
								}
								else if (Me->IsAAReady("Sustained Devastation"))
								{
									BurnSpell = "Sustained Devastation";
									break;
								}
								else if (Me->IsAAReady("Arcane Destruction"))
								{
									BurnSpell = "Arcane Destruction";
									break;
								}
							}
							if (((!Me->Buff("Fury of Ro")) && (!Me->Buff("Fury of Kerafyrm")))||Megaburn)
							{
								if (Me->IsAAReady("Fury of Ro"))
								{
									BurnSpell = "Fury of Ro";
									break;
								}
								else if (Me->IsAAReady("Fury of Kerafyrm"))
								{
									BurnSpell = "Fury of Kerafyrm";
									break;
								}
							}
							if (!Me->Buff("Arcane Fury") && Me->IsAAReady("Arcane Fury"))
							{
								BurnSpell = "Arcane Fury";
								break;
							}
							else if (!Me->AnyBuff("Focus of Arcanum") && Me->IsAAReady("Focus of Arcanum"))
							{
								BurnSpell = "Focus of Arcanum";
							}
							else if (!Me->Buff("Second Spire of Arcanum") && Me->IsAAReady("Fundament: Second Spire of Arcanum"))
							{
								BurnSpell = "Fundament: Second Spire of Arcanum";
								break;
							}
							else if (!ITC && Me->IsSpellReady("Twincast"))
							{
								BurnSpell = "Twincast";
								alt = false;
								break;
							}
							else if (!ITC && Me->IsAAReady("Improved Twincast"))
							{
								BurnSpell = "Improved Twincast";
								break;
							}
							break;
						} while (1);
						if(BurnSpell.size()>0)
						{
							if(alt)
								_snprintf_s(tmp, MAX_STRING, "/casting \"%s\" alt", BurnSpell.c_str());
							else
								_snprintf_s(tmp, MAX_STRING, "/casting \"${Spell[%s].RankName}\"", BurnSpell.c_str());
							EzCommand(tmp);
							DNotify(PLUGIN_NAME, "CastNextNuke", "Burning: with <<\ag%s\ax>>!", BurnSpell.c_str());
							CastState = CASTSTATE_CASTING;
							NukeState = NUKESTATE_CASTING;
							return true;
						}
					}
					if (DoFire && !Haze && !ITC && !GoM && Wiz->FireClawReady(NukeSpell, dist)){}
					else if (DoIce && !Haze && !ITC && !GoM && Wiz->IceClawReady(NukeSpell, dist)) {}
					else if (DoMagic && !Haze && !ITC && !GoM && Wiz->MagicClawReady(NukeSpell, dist)) {}
					else if (DoMagic && !Haze && !ITC && !GoM && Wiz->VortexNukeReady(NukeSpell, dist)) {}
					else if (DoMagic && !Haze && !ITC && Wiz->MagicInstantReady(NukeSpell, dist)) {}
					else if (DoFire && !Haze && !ITC && Wiz->FireInstantReady(NukeSpell, dist)) {}
					else if (DoIce && !Haze && !ITC && Wiz->IceInstantReady(NukeSpell, dist)) {}
					else if (DoIce && DoFire && DoMagic && !Haze && !Burn && Gambit && !GoM && Wiz->FastNukeReady(NukeSpell, dist)) {}
					else if ((!LowMana && Haze || GoM) && DoFire && (!ITC) && Wiz->DichotomicNukeReady(NukeSpell, dist)) {}
					else if ((!LowMana || Haze || GoM) && DoFire && DoIce && Wiz->EtherealWeaveReady(NukeSpell, dist)) {}
					else if ((!LowMana || Haze || GoM) && DoFire && Wiz->FireEtherealReady(NukeSpell, dist)) {}
					else if ((!LowMana || Haze || GoM) && DoIce && Wiz->IceEtherealReady(NukeSpell, dist)) {}
					else if ((!LowMana || Haze || GoM) && DoMagic && Wiz->MagicEtherealReady(NukeSpell, dist)) {}
					else if (LowMana && !GoM && DoFire && DoIce && DoMagic && Wiz->FastNukeReady(NukeSpell, dist)) {}
					else if (DoMagic && Wiz->MagicInstantReady(NukeSpell, dist)) {}
					else if (DoFire && Wiz->FireInstantReady(NukeSpell, dist)) {}
					else if (DoIce && Wiz->IceInstantReady(NukeSpell, dist)) {}
					else if (DoFire && Wiz->FireClawReady(NukeSpell, dist)){}
					else if (DoIce && Wiz->IceClawReady(NukeSpell, dist)) {}
					else if (DoMagic && Wiz->MagicClawReady(NukeSpell, dist)) {}
					else if (DoFire && DoIce && DoMagic && Wiz->FastNukeReady(NukeSpell, dist)) {}
					else if (DoFire && DoIce && DoMagic && Wiz->LowLevelNukeReady(NukeSpell, dist)) {}
					else {
						CastState = CASTSTATE_STARTED;
						NukeState = NUKESTATE_IDLE;
						DNotify(PLUGIN_NAME, "CastNextNuke", "No Nuke available/needed?");
					}
				}break;
				case Mage:
				{
					bool Haze = Me->AnyBuff("Chromatic Haze", true) | Me->AnyBuff("Gift of Hazy Thoughts", true);
					bool ITC = Me->Buff("Improved Twincast", true);
					bool LowMana = (Me->PctMana() < 20) ? true : false;
					bool GoM = Me->ShortBuff("Gift of Mana", true);
					int tHPs = ((pNukeTarget->HPCurrent*100)/pNukeTarget->HPMax);
					CMagician *Mage = (CMagician *)Me;
					if ((tHPs < 81) && Mage->ThaumatizeSpellReady(NukeSpell, dist)) {}
					else if (GoM && Mage->IsSpellReady("Dichotomic Companion") && Mage->GetPetID()) { NukeSpell = "Dichotomic Companion"; }
					else if (Mage->ServantSpellReady(NukeSpell, dist)){}
					else if (Mage->OfManySpellReady(NukeSpell, dist)) {}
					else if (Mage->SpearSpellReady(NukeSpell, dist)) {}
					else if (Mage->BoltSpellReady(NukeSpell, dist)) {}
					else if (Mage->FickleSpellReady(NukeSpell, dist)) {}
					else if (Mage->NextItemClicky(NukeSpell, dist)) {}
					else {
						CastState = CASTSTATE_STARTED;
						NukeState = NUKESTATE_IDLE;
						DNotify(PLUGIN_NAME, "CastNextNuke", "No Nuke available/needed?");
					}
				}break;
				case Enchanter:
				{
					PSPAWNINFO pSpawnClosest = NULL;
					PSPAWNINFO *psTarget = NULL;
					int XTarCount = Me->XTargetHaters();
					bool Haze = Me->AnyBuff("Chromatic Haze", true) | Me->AnyBuff("Gift of Hazy Thoughts", true);
					bool ITC = Me->Buff("Improved Twincast", true);
					bool LowMana = (Me->PctMana() < 40) ? true : false;
					bool GoM = Me->ShortBuff("Gift of Mana", true);
					bool AlreadyTargeted = false;
					bool Burn = Me->AnyBuff("Calculated Insanity", true);
					int MobsInCamp = Me->XTargetHaters(0, false, CampRadius);
					int NamedMobInCamp = Me->XTargetHaters(0, true, CampRadius);
					if (pTarget && pTargetWnd && (pTarget->Data.SpawnID == pNukeTarget->SpawnID))
						AlreadyTargeted = true;
					else
					{
						if (ppTarget) {
							psTarget = (PSPAWNINFO*)ppTarget;
							*psTarget = pNukeTarget;
							DNotify(PLUGIN_NAME, "CastNextNuke", "Target - %s selected", pNukeTarget->Name);
						}
						return false;
					}
					CEnchanter *Enc = (CEnchanter *)Me;
					if ((XTarCount > MezThreshold) && DoMez)
					{
						Enc->SetMATarget(pNukeTarget);
						if (Enc->MezNeeded())
						{
							CastState = CASTSTATE_CASTING;
							NukeState = NUKESTATE_CASTING;
							return true;
						}
					}
					if (DoBurn && ((MobsInCamp>MobBurnThreshold) || (BurnNamed && (NamedMobInCamp>0))))
					{
						string BurnSpell;
						bool alt = true;
						if (!Haze && Me->IsAAReady("Chromatic Haze"))
						{
							BurnSpell = "Chromatic Haze";
						}
						else if (!Me->AnyBuff("Illusions of Grandeur") && Me->IsAAReady("Illusions of Grandeur"))
						{
							BurnSpell = "Illusions of Grandeur";
						}
						else if (!Burn && (!Me->AnyBuff("Calcualted Insanity"), true) && (Me->IsAAReady("Calcualted Insanity")))
						{
							BurnSpell = "Calculated Insanity";
						}
						else if (!Me->AnyBuff("Fundament: Third Spire of Enchantment") && Me->IsAAReady("Fundament: Third Spire of Enchantment"))
						{
							BurnSpell = "Fundament: Third Spire of Enchantment";
						}
						else if (!ITC && Me->IsAAReady("Improved Twincast"))
						{
							BurnSpell = "Improved Twincast";
						}
						if (BurnSpell.size()>0)
						{
							if (alt)
								_snprintf_s(tmp, MAX_STRING, "/casting \"%s\" alt", BurnSpell.c_str());
							else
								_snprintf_s(tmp, MAX_STRING, "/casting \"%s\"", BurnSpell.c_str());
							EzCommand(tmp);
							DNotify(PLUGIN_NAME, "CastNextNuke", "Burning: with <<\ag%s\ax>>!", BurnSpell.c_str());
							CastState = CASTSTATE_CASTING;
							NukeState = NUKESTATE_CASTING;
							return true;
						}
					}
					if (new_target && Enc->MezSpellReady(NukeSpell, dist)) {}
					else if (!Haze && !GoM && !ITC && !Enc->TargetHasTashDebuff() && Enc->TashSpellReady(NukeSpell, dist)) {}
					else if (!Haze && !GoM && !ITC && !Enc->TargetHasSlowDebuff() && Enc->SlowSpellReady(NukeSpell, dist)) {}
					else if (!Haze && !Enc->TargetHasManaDrainDebuff() && Enc->ManaDrainSpellReady(NukeSpell, dist)) {}
					else if (!Haze && !Enc->TargetHasBigDoT() && Enc->BigDoTReady(NukeSpell, dist)) {}
					else if (!Haze && !Enc->TargetHasSmallDoT() && Enc->SmallDoTReady(NukeSpell, dist)) {}
					else if (!Haze && !ITC && Enc->DichoSpellReady(NukeSpell, dist)) {}
					else if (Enc->BigNukeReady(NukeSpell, dist)){}
					else if (Enc->AssaultNukeReady(NukeSpell, dist)) {}
					else {
						CastState = CASTSTATE_STARTED;
						NukeState = NUKESTATE_IDLE;
						DNotify(PLUGIN_NAME, "CastNextNuke", "No Nuke available/needed?");
					}
				}break;
				case Beastlord:
				{
					static PSPAWNINFO pLastTarget = NULL;
					static bool skipfrozen = false;
					CBeastlord *Bst = (CBeastlord *)Me;
					if (pNukeTarget == pLastTarget)
					{
						skipfrozen = false;
					}
					if ((!skipfrozen) && Bst->FrozenSpellReady(NukeSpell, dist)) { skipfrozen = true; pLastTarget = pNukeTarget; }
					else if (Bst->MaelstromSpellReady(NukeSpell, dist)) { skipfrozen = false; }
					else if (Bst->FeralgiaSpellReady(NukeSpell, dist)) { skipfrozen = false; }
					else if (Bst->BiteSpellReady(NukeSpell, dist)) { skipfrozen = false; }
					else if (Bst->LanceSpellReady(NukeSpell, dist)) { skipfrozen = false; }
					else {
						CastState = CASTSTATE_STARTED;
						NukeState = NUKESTATE_IDLE;
						DNotify(PLUGIN_NAME, "CastNextNuke", "No Nuke available/needed?");
					}
				}break;
				case Druid:
				{
					static PSPAWNINFO pLastTarget = NULL;
					static bool skipseed = false, skipwrath=false;
					if (pNukeTarget == pLastTarget)	{ }
					else { skipseed=false; skipwrath=false; };
					bool Haze = Me->AnyBuff("Chromatic Haze", true) | Me->AnyBuff("Gift of Hazy Thoughts", true);
					bool ITC = Me->Buff("Improved Twincast", true);
					bool LowMana = (Me->PctMana() < 20) ? true : false;
					bool GoM = Me->ShortBuff("Gift of Mana", true);
					int tHPs = ((pNukeTarget->HPCurrent * 100) / pNukeTarget->HPMax);
					CDruid *Druid = (CDruid *)Me;
					pLastTarget = pNukeTarget;
					if ((!skipseed) && Druid->SkinDebuffReady(NukeSpell, dist)) { skipseed = true; }
					else if (Druid->DichotomicNukeReady(NukeSpell, dist)) {}
					else if ((!skipwrath) && Druid->WrathNukeReady(NukeSpell, dist)) { skipwrath=true; }
					else if (Druid->WinterNukeReady(NukeSpell, dist)){}
					else if (Druid->RemoteNukeReady(NukeSpell, dist)){}
					else if (Druid->StunNukeReady(NukeSpell, dist)) {}
					else {
						CastState = CASTSTATE_STARTED;
						NukeState = NUKESTATE_IDLE;
						DNotify(PLUGIN_NAME, "CastNextNuke", "No Nuke available/needed?");
					}
				}break;
				default:break;
			}
		}
		if (!NukeSpell.empty())
		{
			if (alt)
				if (targetid)
					_snprintf_s(tmp, MAX_STRING, "/casting \"%s\" alt -targetid|%d", NukeSpell.c_str(), pNukeTarget->SpawnID);
				else
					_snprintf_s(tmp, MAX_STRING, "/casting \"%s\" alt", NukeSpell.c_str());
			else if (item)
				if (targetid)
					_snprintf_s(tmp, MAX_STRING, "/casting \"%s\" item -targetid|%d", NukeSpell.c_str(), pNukeTarget->SpawnID);
				else
					_snprintf_s(tmp, MAX_STRING, "/casting \"%s\" item", NukeSpell.c_str());
			else
				if (targetid)
					_snprintf_s(tmp, MAX_STRING, "/casting \"${Spell[%s].RankName}\" -targetid|%d", NukeSpell.c_str(), pNukeTarget->SpawnID);
				else
					_snprintf_s(tmp, MAX_STRING, "/casting \"${Spell[%s].RankName}\"", NukeSpell.c_str());
			EzCommand(tmp);
			DNotify(PLUGIN_NAME, "CastNextNuke", "on: \ay<<%s>>\ax with <<\ag%s\ax>>!", pNukeTarget->Name, NukeSpell.c_str());
			CastState = CASTSTATE_CASTING;
			NukeState = NUKESTATE_CASTING;
		}
		else
		{
			CastState = CASTSTATE_STARTED;
			NukeState = NUKESTATE_IDLE;
			pNukeTarget = NULL;
		}
	}break;
	case CASTSTATE_CASTING:
	{
		//ReTargetCount=0;
		if (!Me->CastingLeft())
		{
			NukeState = NUKESTATE_CASTING;
			CastState = CASTSTATE_STARTED;
			pNukeTarget = NULL;
		}
	}break;
	case CASTSTATE_DONE:
	{
		CastState = CASTSTATE_STARTED;
		NukeState = NUKESTATE_IDLE;
	}
	default:break;
	}
	return true;
}

bool Hovering(SPAWNINFO *pSpawn)
{
	return pSpawn->RespawnTimer ? true : false;
}

bool IsCorpse(SPAWNINFO *pSpawn)
{
	return (pSpawn->Type == SPAWN_CORPSE) ? true : false;
}

bool AmIHovering()
{
	if (Hovering(GetCharInfo()->pSpawn))	//Are we dead?
	{
		if (ReleaseAndCampfire)
		{
			CampFireOnZone = true;
			CampFireResume = true;
			if (!campfiretimer)
			{
				ReleaseCampfireTimer.Reset();
				ReleaseCampfireTimer.Start();
				campfiretimer = true;
				SelectResurrect(true);
			}
			else
			{
				int elapsed = ReleaseCampfireTimer.msElapsed();
				if (elapsed > 200) // wait 200 ms to click... (.2 seconds)
				{
					CampFireOnZone = true;
					campfiretimer = false;
					EzCommand("/squelch /notify RespawnWnd RW_SelectButton leftmouseup");
				}
			}
		}
		return true;
	}
	else // Not dead... Are we supposed to Click Camp Fire?
	{
		if ((ReleaseAndCampfire) && (CampFireOnZone)) // Did we zone, and need to click camp fire now?
		{
			CampFireOnZone = false;
			CampFireResume = true;
			campfiretimer = false;
			EzCommand("/casting \"Fellowship Registration Insignia\" item");
			return true; // not really hovering, but don't need to process any more heal logic until we zone back..
		}
		return false;
	}
}

int SelectResurrect(bool bind)
{
	CListWnd *Child;
	CXWnd *pWnd;
	CXStr Text;
	char buf[2048];
	int x = 0;

	pWnd = (CXWnd *)FindMQ2Window("RespawnWnd");
	if (!pWnd) return -1;
	if (((PCSIDLWND)(pWnd))->dShow == 0) return -1;
	Child = (CListWnd *)pWnd->GetChildItem("rw_optionslist");
	if (!Child) return -1;
	for (x = 0; x<Child->ItemsArray.Count; x++)
	{
		memset(buf, 0, 2048);
		Child->GetItemText(&Text, x, 1);
		GetCXStr(Text.Ptr, buf, 2048);

		DebugSpew("Item %i: %s", x, buf);
		if (((!bind) && (_strnicmp(buf, "Resurrect", 9) == 0)) || (bind && (_strnicmp(buf, "Resurrect", 9) != 0)))
		{
			SendListSelect("RespawnWnd", "RW_OptionsList", x);
			DebugSpew("Selected %i: %s", x, buf);
		}
	}
	return 0;
	//GetCXStr(((PCSIDLWND)Child)->SidlText,InputCXStr,sizeof(InputCXStr));
	//Resurrect
}

void ReleaseToBind()
{
	SelectResurrect();
}

bool CheckReviveMerc()
{
	static MyTimer merctimer;
	PCHARINFO info = GetCharInfo();
	int merc = -1;
	bool foundmerc = false;
	if (!info->pGroupInfo)
		return false;
	if (!info->pGroupInfo->pMember)
		return false;
	for (int x = 0; x<6; x++)
	{
		if (info->pGroupInfo->pMember[x])
			if (info->pGroupInfo->pMember[x]->Mercenary)
				if (info->pGroupInfo->pMember[x]->pOwner)
					if (strlen(info->Name) == info->pGroupInfo->pMember[x]->pOwner->Length)
						if (!_strnicmp(info->pGroupInfo->pMember[x]->pOwner->Text, info->Name, strlen(info->Name)))
							merc = x;
	}
	if (merc)
		if (Evaluate("${Window[MMGW_ManageWnd].Child[MMGW_SuspendButton].Text.Equal[Revive]}") && (Evaluate("${Mercenary.State.Equal[DEAD]}")))
		{
			if (!merctimer.Running())
			{
				EzCommand("/notify MMGW_ManageWnd MMGW_SuspendButton LeftMouseUp");
				merctimer.Start();
				//WriteChatColor("Reviving Mercenary...");
				return true;
			}
			else // Revive delay -- Don't try to revive again for at least 5 seconds...
			{
				if (merctimer.msElapsed() > 5000)
				{
					merctimer.Stop();
					merctimer.Reset();
					return false;
				}
				else
					return false;
			}
		}
		else // Mercenary is not dead, or can't be revived
			return false;
	else // No mercenary
		return false;
}

int PctAggro()
{
	if (pAggroInfo)
	{
		return pAggroInfo->aggroData[AD_Player].AggroPct;
	}
	return 0;
}

bool CheckSongs(PCHAR songname)
{
	for (unsigned long nBuff = 0; nBuff < NUM_SHORT_BUFFS; nBuff++)
	{
		if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->ShortBuff[nBuff].SpellID))
		{
			if (!_strnicmp(songname, pSpell->Name, strlen(songname)))
			{
				//Dest.Ptr=&GetCharInfo2()->ShortBuff[nBuff];
				//Dest.Type=pBuffType;
				return true;
			}
		}
	}
	return false;
}

bool AggroCheck()
{
	if (PctAggro() < 95) return false;
	if (Me->IsAAReady("Mind Crash"))
	{
		EzCommand("/casting \"Mind Crash\" alt");
		return true;
	}
	else if (Me->IsAAReady("Arcane Whisper"))
	{
		EzCommand("/casting \"Arcane Whisper\" alt");
		return true;
	}
	else if (Me->IsAAReady("Concussive Intuition"))
	{
		EzCommand("/casting \"Concussive Intuition\" alt");
		return true;
	}
	else if (Me->IsAAReady("A Hole In Space"))
	{
		EzCommand("/casting \"A Hole In Space\" alt");
		MakeVisible = true;
		return true;
	}
	return false;
}

bool DivineAuraCheck()
{
	if (CheckSongs("Touch of the Divine"))
	{
		EzCommand("/clickoff \"Touch of the Divine\"");
		return true;
	}
	if (MakeVisible)
	{
		MakeMeVisible(NULL, NULL);
		MakeVisible = false;
	}
	//return AggroCheck();
	return false;
}

void Notify(PCHAR Plugin, PCHAR Function, PCHAR Format, ...)
{
	char zTemp[MAX_STRING] = { 0 };
	char zOut[MAX_STRING] = { 0 };
	char zOut2[MAX_STRING] = { 0 };
	va_list vaList;
	va_start(vaList, Format);
	vsnprintf(zTemp, MAX_STRING, Format, vaList);
	if (!zTemp[0]) return;	//Error - no input data...
	zTemp[MAX_STRING - 1] = 0;
	int index = 0;
	int oindex = 0;
	if (!UseEqbc)
	{
		if (Plugin && Function)
			WriteChatf("%s::%s %s", Plugin, Function, zTemp);
		else
			WriteChatf("%s", zTemp);
		return;
	}
	while ((zTemp[index]) && (index<MAX_STRING))
	{
		if ((zTemp[index] == '\a') && (index + 1<MAX_STRING) && (oindex + 4<MAX_STRING))
		{ //[+x+]
			zOut[oindex++] = '[';
			zOut[oindex++] = '+';
			zOut[oindex++] = zTemp[index + 1];
			zOut[oindex++] = '+';
			zOut[oindex++] = ']';
			index++;
		}
		else
		{
			if (zTemp[index] != '\a')
				zOut[oindex++] = zTemp[index];
		}
		index++;
	}

	if (Plugin && Function)
		_snprintf_s(zOut2, MAX_STRING, "/bc %s::%s %s", Plugin, Function, zOut);
	else
		_snprintf_s(zOut2, MAX_STRING, "/bc %s", zOut);
	EzCommand(zOut2);
}

void DoCommandf(PCHAR Format, ...)
{
	char zTemp[MAX_STRING + 1] = { 0 };
	va_list vaList;
	va_start(vaList, Format);
	vsnprintf(zTemp, MAX_STRING, Format, vaList);
	if (!zTemp[0]) return;	//Error - no input data...
	zTemp[MAX_STRING - 1] = 0;	// No buffer overflows please
	EzCommand(zTemp);
}

bool SpellStacks(PCHAR _SpellName)
{
	PSPELL pSpell = GetSpellByName(_SpellName);
	if (!pSpell) return false;
	unsigned long buffduration;
	unsigned long duration = 99999;
	if (IsNumber(_SpellName))
		duration = atoi(_SpellName);
	unsigned long nBuff;
	PCHARINFO2 pChar = GetCharInfo2();
	//	Dest.DWord = true;
	//	Dest.Type = pBoolType;
	// Check Buffs
	for (nBuff = 0; nBuff<NUM_LONG_BUFFS; nBuff++) {
		if (pChar->Buff[nBuff].SpellID > 0) {
			if (PSPELL buffSpell = GetSpellByID(pChar->Buff[nBuff].SpellID)) {
				buffduration = pChar->Buff[nBuff].Duration;
				LONG neff=GetSpellNumEffects(pSpell);
				for (int nSlot = 0; nSlot < neff; nSlot++) {
					if (TriggeringEffectSpell(pSpell, nSlot)) {		// Check the triggered effect against the current buff for stacking
						if (PSPELL triggeredSpell = GetSpellByID(GetSpellBase2(pSpell,nSlot))) {
							if (GetSpellDuration(triggeredSpell, (PSPAWNINFO)pLocalPlayer) >= 0xFFFFFFFE) {
								buffduration = 99999 + 1;
							}
							if (!BuffStackTest(triggeredSpell, buffSpell) || ((pSpell == triggeredSpell) && (buffduration>duration))) {
								//Dest.DWord = false;
								//return true;
								return false;
							}
						}
					}
				}
				if (GetSpellDuration(buffSpell, (PSPAWNINFO)pLocalPlayer) >= 0xFFFFFFFE) {
					buffduration = 99999 + 1;
				}
				if (!BuffStackTest(pSpell, buffSpell, TRUE) || ((buffSpell == pSpell) && (buffduration>duration))) {
					//Dest.DWord = false;
					//return true;
					return false;
				}
			}
		}
	}
	// Check Songs
	for (nBuff = 0; nBuff<NUM_SHORT_BUFFS; nBuff++) {
		if (pChar->ShortBuff[nBuff].SpellID>0) {
			if (PSPELL buffSpell = GetSpellByID(pChar->Buff[nBuff].SpellID)) {
				buffduration = pChar->ShortBuff[nBuff].Duration;
				if (!IsBardSong(buffSpell) && !((IsSPAEffect(pSpell, SPA_ILLUSION) && !pSpell->DurationWindow))) {		// Don't check against bard songs or buff window illusions
					LONG neff=GetSpellNumEffects(pSpell);
					for (int nSlot = 0; nSlot < neff; nSlot++) {
						if (TriggeringEffectSpell(pSpell, nSlot)) {		// Check the triggered effect against the current buff for stacking
							if (PSPELL triggeredSpell = GetSpellByID(GetSpellBase2(pSpell,nSlot))) {
								if (GetSpellDuration(triggeredSpell, (PSPAWNINFO)pLocalPlayer) >= 0xFFFFFFFE) {
									buffduration = 99999 + 1;
								}
								if (!BuffStackTest(triggeredSpell, buffSpell) || ((pSpell == triggeredSpell) && (buffduration>duration))) {
									//Dest.DWord = false;
									//return true;
									return false;
								}
							}
						}
					}
					if (GetSpellDuration(buffSpell, (PSPAWNINFO)pLocalPlayer) >= 0xFFFFFFFE) {
						buffduration = 99999 + 1;
					}
					if (!BuffStackTest(pSpell, buffSpell, TRUE) || ((buffSpell == pSpell) && (buffduration>duration))) {
						//Dest.DWord = false;
						//return true;
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool MedCheck()
{
	static MyTimer medtimer;
	string HarvestSpell;
	HarvestSpell.clear();
	if (!MedPct) return false;
	if ((!medtimer.Running()) || (medtimer.Running() && (medtimer.msElapsed() >= 200)))
	{
		medtimer.Stop();
		medtimer.Reset();
		medtimer.Start();

		if (Harvest && !Me->Invis() && (Me->PctMana() <= HarvestAt) && Me->IsAAReady("Harvest of Druzzil") && !Me->CastingLeft())
		{
			if (Me->Sitting()) { DoCommandf("/stand"); return true; }
			DoCommandf("/casting \"Harvest of Druzzil\" alt");
			DNotify(PLUGIN_NAME, "MedCheck", "Casting (\agHarvest of Druzzil\ax)");
			NukeState = NUKESTATE_CASTING;
			return false;
		}
		else if (Harvest && !Me->Invis() && (Me->PctMana() <= HarvestAt) && !Me->InCombat() && Me->IsSpellReady("Quiescent Harvest") && !Me->CastingLeft())
		{
			if (Me->Sitting()) { DoCommandf("/stand"); return true; }
			DoCommandf("/casting \"${Spell[Quiescent Harvest].RankName}\"");
			DNotify(PLUGIN_NAME, "MedCheck", "Casting (\agQuiescent Harvest\ax)", GetCharInfo()->Name);
			NukeState = NUKESTATE_CASTING;
			return false;
		}
		else if (Harvest && !Me->Invis() && (Me->PctMana() <= HarvestAt) && Me->IsSpellReady("Quiescent Gambit") && !Me->CastingLeft())
		{
			if (Me->Sitting()) { DoCommandf("/stand"); return true; }
			DoCommandf("/casting \"${Spell[Quiescent Gambit].RankName}\"");
			DNotify(PLUGIN_NAME, "MedCheck", "Casting (\agQuiescent Gambit\ax)", GetCharInfo()->Name);
			NukeState = NUKESTATE_CASTING;
			return false;
		}
		else if (Harvest && !Me->Invis() && (Me->PctMana() <= HarvestAt) && Me->IsSpellReady("Bucolic Gambit") && !Me->CastingLeft())
		{
			if (Me->Sitting()) { DoCommandf("/stand"); return true; }
			DoCommandf("/casting \"${Spell[Bucolic Gambit].RankName}\"");
			DNotify(PLUGIN_NAME, "MedCheck", "Casting (\agBucolic Gambit\ax)", GetCharInfo()->Name);
			NukeState = NUKESTATE_CASTING;
			return false;
		}
		else if ((Me->PctMana() <= MedPct) && (!Me->CastingLeft()) && (!Me->InCombat()))
		{
			// out of combat med time
			if ((!DoMount && DoMed) || (Me->Indoors() && DoMed && (!Me->OnMount()) && !IsStickOn() && Evaluate("${Cast.Status.Equal[I]}") && Me->Standing() && !Me->Moving()))
			{
				DoCommandf("/sit");
				DNotify(PLUGIN_NAME, "MedCheck", "Meditating", GetCharInfo()->Name);
				return true;
			}
			else if (DoMount && !Me->OnMount() && Me->Outdoors() && !Me->XTargetHaters() && !Me->FeetWet() && Me->CastReady())
			{
				if (Me->Sitting())
				{
					DoCommandf("/stand");
					return false;
				}
				DoCommandf("/casting \"%s\" item", MountItem);
				DNotify(PLUGIN_NAME, "MedCheck", "Casting Mount <<\ay%s\ax>>", MountItem);
				NukeState = NUKESTATE_CASTING;
				return false;
			}
		}
		else if (Harvest && !Me->Invis() && (Me->PctMana() < 20))
		{
			//else if ()
			if (Me->IsAAReady("Harvest of Druzzil"))
			{
				if (Me->Sitting()) { DoCommandf("/stand"); return true; }
				DoCommandf("/casting \"Harvest of Druzzil\" alt");
				DNotify(PLUGIN_NAME, "MedCheck", "Casting (\agHarvest of Druzzil\ax)");
				NukeState = NUKESTATE_CASTING;
				return false;
			}
			else if (Me->IsSpellReady("Quiescent Harvest"))
			{
				if (Me->Sitting()) { DoCommandf("/stand"); return true; }
				DoCommandf("/casting \"${Spell[Quiescent Harvest].RankName}\"");
				DNotify(PLUGIN_NAME, "MedCheck", "Casting (\agQuiescent Harvest\ax)", GetCharInfo()->Name);
				NukeState = NUKESTATE_CASTING;
				return false;
			}
			else if (Me->IsSpellReady("Quiescent Gambit"))
			{
				if (Me->Sitting()) { DoCommandf("/stand"); return true; }
				DoCommandf("/casting \"${Spell[Quiescent Gambit].RankName}\"");
				DNotify(PLUGIN_NAME, "MedCheck", "Casting (\agQuiescent Gambit\ax)", GetCharInfo()->Name);
				NukeState = NUKESTATE_CASTING;
				return false;
			}
			else if (Me->IsSpellReady("Bucolic Gambit"))
			{
				if (Me->Sitting()) { DoCommandf("/stand"); return true; }
				DoCommandf("/casting \"${Spell[Bucolic Gambit].RankName}\"");
				DNotify(PLUGIN_NAME, "MedCheck", "Casting (\agBucolic Gambit\ax)", GetCharInfo()->Name);
				NukeState = NUKESTATE_CASTING;
				return false;
			}
		}
	}
	return false;
}

#define BUFF_STATUS_IDLE 0
#define BUFF_STATUS_TARGETING 1
#define BUFF_STATUS_CHECKING 2
#define BUFF_STATUS_CASTING 3
#define BUFF_STATUS_NEXT 4

#define BUFF_FLAG_SINGLE	0x01
#define BUFF_FLAG_GROUP		0x02
#define BUFF_FLAG_MT		0x04
#define BUFF_FLAG_PULLER	0x08

bool CheckForBuffs()
{
	static int BuffStatus = BUFF_STATUS_IDLE;
	static MyTimer BuffTimer;
	static int CurrentGroupMember = 0;

	if (Me->InCombat())
	{
		BuffStatus = BUFF_STATUS_IDLE;
		return false;
	}

	switch (BuffStatus)
	{
	case BUFF_STATUS_IDLE:
	{

	}break;
	case BUFF_STATUS_TARGETING:
	{

	}break;
	}
	return false;
}
// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
	static bool Medding = false;
	static MyTimer standtimer;
	// Check if we're connected to eqbc, so we can use Notify()...
	if (EQBCConnected())
		UseEqbc = true;
	else
		UseEqbc = false;

	// If Heals are off, do nothing
	if (!NukesOn) return;

	// We can't heal if we're dead... :)
	if (AmIHovering()) return;

	// If heals are paused, do not heal or rez...
	if (NukePause) return;

	// Check if we have a merc and it's dead...
	if (!Me->InCombat())
		if (CheckReviveMerc()) DNotify(PLUGIN_NAME, "CheckReviveMerc", "Tried to revive mercenary...");

	// We can't DPS if we're DA'ed, and/or too high on the hate list, if so, click off DA and/or fade...
	if (DivineAuraCheck()) return;

	if (Me->InCombat() && ManageAggro)
		if(AggroCheck()) return;

	// Hopefully, this will work now...
	if (DismissFamiliar()) return;

	// Check for combat rez stuff -- Wizards Don't Rez!!! :)
//	if ((DoCombatRez && DoRez && Me->CastingCoolDown()) || ((!Me->InCombat()) && DoRez))
//		CheckForRez();	// True if we try to rez someone...

	// Check for buffs out of combat...
	if (!Me->InCombat())
		if (CheckForBuffs()) return;

	if (!Me->InCombat())
		Medding = MedCheck();

	// Main Heal Loop:
	switch (NukeState)
	{
		case NUKESTATE_IDLE:
		if (Me->CastReady() && !Me->CastingCoolDown())
		{
			if (CheckForNukes())
			{
				if (pNukeTarget)
				{
					if (Me->Sitting())
					{
						EzCommand("/stand");
						Medding = false;
						return;
					}
				}
			}
			else
			{
				Medding = MedCheck();
			}
		}
		else if (Me->CastReady() && Me->CastingCoolDown())
		{
			if (CheckForNukes())
			{
				if (pNukeTarget)
				{
					if (Me->Sitting())
					{
						EzCommand("/stand");
						Medding = false;
						return;
					}
					else
					{
					}
				}
			}
		}
		break;
		case NUKESTATE_CASTSTART:
		if (Me->CastReady() && (!Me->CastingLeft()) && (!Me->CastingCoolDown()))
		{
			if ((!Me->Standing()) || Medding)
			{
				if (((standtimer.Running()) && (standtimer.msElapsed() > 500)) || (!standtimer.Running()))
				{
					standtimer.Stop();
					standtimer.Reset();
					standtimer.Start();
					EzCommand("/stand");
					Medding = false;
				}
			}
			else
				CastNextNuke();
		}
		else if (Me->CastReady() && (!Me->CastingLeft()) && Me->CastingCoolDown())
		{
			if ((!Me->Standing()) || Medding)
			{
				if (((standtimer.Running()) && (standtimer.msElapsed() > 500)) || (!standtimer.Running()))
				{
					standtimer.Stop();
					standtimer.Reset();
					standtimer.Start();
					EzCommand("/stand");
					Medding = false;
				}
			}
			else
				if(DoAANukes)
					CastNextFreeNuke();
		}
		break;
		case NUKESTATE_CASTING:
		if (!Me->CastingLeft())
		{
			NukeState = NUKESTATE_CASTDONE;
			pNukeTarget = NULL;
		}
		break;
		case NUKESTATE_CASTDONE:
			NukeState = NUKESTATE_IDLE;
			pNukeTarget = NULL;
		break;
		case NUKESTATE_CASTFAIL:
			NukeState = NUKESTATE_IDLE;
			pNukeTarget = NULL;
		break;
		case NUKESTATE_COOLDOWN:
			if (!Me->CastingCoolDown())
			{
				NukeState = NUKESTATE_IDLE;
				pNukeTarget = NULL;
			}
			else
			{
				if (CheckForNukes() && !Me->CastingLeft() && pNukeTarget &&
				(((SpawnPctHPs(pNukeTarget) < AssistAt) && (SpawnPctHPs(pNukeTarget)>=StopAt)) || Dummy || ManualAssist))
				{
					if(DoAANukes)
						CastNextFreeNuke();
					//string NukeSpell;
					//NukeSpell.clear();
					//switch (Me->Class())
					//{
					//	case Wizard:
					//	{
					//		float dist = GetDistance(GetCharInfo()->pSpawn, pNukeTarget);
					//		CWizard *wizard = (CWizard *)Me;
					//		if (wizard->FreeNukeReady(NukeSpell, dist))
					//			DoCommandf("/casting \"%s\" alt -targetid|%d", NukeSpell.c_str(), pNukeTarget->SpawnID);
					//	}break;
					//	case Mage:
					//	{
					//		float dist = GetDistance(GetCharInfo()->pSpawn, pNukeTarget);
					//		CMagician *mage = (CMagician *)Me;
					//		if (mage->FreeNukeReady(NukeSpell, dist))
					//			DoCommandf("/casting \"%s\" alt -targetid|%d", NukeSpell.c_str(), pNukeTarget->SpawnID);
					//	}break;
					//	case Druid:
					//	{
					//		float dist = GetDistance(GetCharInfo()->pSpawn, pNukeTarget);
					//		CDruid *druid = (CDruid *)Me;
					//		if (druid->FreeNukeReady(NukeSpell, dist))
					//			DoCommandf("/casting \"%s\" alt -targetid|%d", NukeSpell.c_str(), pNukeTarget->SpawnID);

					//	}break;
					//	default:break;
					//}
				}
			}
			break;
		default:break;
	}
	// DONT leave in this debugspew, even if you leave in all the others
	//DebugSpewAlways("MQ2AFNuke::OnPulse()");
}
/*
// Called every frame that the "HUD" is drawn -- e.g. net status / packet loss bar
PLUGIN_API VOID OnDrawHUD(VOID)
{
// DONT leave in this debugspew, even if you leave in all the others
//DebugSpewAlways("MQ2AFNuke::OnDrawHUD()");
}

// Called once directly before shutdown of the new ui system, and also
// every time the game calls CDisplay::CleanGameUI()
PLUGIN_API VOID OnCleanUI(VOID)
{
DebugSpewAlways("MQ2AFNuke::OnCleanUI()");
// destroy custom windows, etc
}

// Called once directly after the game ui is reloaded, after issuing /loadskin
PLUGIN_API VOID OnReloadUI(VOID)
{
DebugSpewAlways("MQ2AFNuke::OnReloadUI()");
// recreate custom windows, etc
}

// This is called every time WriteChatColor is called by MQ2Main or any plugin,
// IGNORING FILTERS, IF YOU NEED THEM MAKE SURE TO IMPLEMENT THEM. IF YOU DONT
// CALL CEverQuest::dsp_chat MAKE SURE TO IMPLEMENT EVENTS HERE (for chat plugins)
PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter)
{
DebugSpewAlways("MQ2AFNuke::OnWriteChatColor(%s)",Line);
return 0;
}

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
// but after MQ filters and chat events are taken care of.
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
DebugSpewAlways("MQ2AFNuke::OnIncomingChat(%s)",Line);
return 0;
}

// This is called each time a spawn is added to a zone (inserted into EQ's list of spawns),
// or for each existing spawn when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API VOID OnAddSpawn(PSPAWNINFO pNewSpawn)
{
DebugSpewAlways("MQ2AFNuke::OnAddSpawn(%s)",pNewSpawn->Name);
}

// This is called each time a spawn is removed from a zone (removed from EQ's list of spawns).
// It is NOT called for each existing spawn when a plugin shuts down.
PLUGIN_API VOID OnRemoveSpawn(PSPAWNINFO pSpawn)
{
DebugSpewAlways("MQ2AFNuke::OnRemoveSpawn(%s)",pSpawn->Name);
}

// This is called each time a ground item is added to a zone
// or for each existing ground item when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API VOID OnAddGroundItem(PGROUNDITEM pNewGroundItem)
{
DebugSpewAlways("MQ2AFNuke::OnAddGroundItem(%d)",pNewGroundItem->DropID);
}

// This is called each time a ground item is removed from a zone
// It is NOT called for each existing ground item when a plugin shuts down.
PLUGIN_API VOID OnRemoveGroundItem(PGROUNDITEM pGroundItem)
{
DebugSpewAlways("MQ2AFNuke::OnRemoveGroundItem(%d)",pGroundItem->DropID);
}
*/