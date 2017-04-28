// MQ2Heals.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.
#include "MQ2Heals.h"
#ifndef PLUGIN_API
#include "../MQ2Plugin.h"
PreSetup(PLUGIN_NAME);
PLUGIN_VERSION(PLUGIN_VERS);
/*
#include <string>
#include <list>
#include <set>
#include <map>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
*/
#endif PLUGIN_API
#include "Character.h"
#include "Cleric.h"
#include "Shaman.h"
#include "Druid.h"
#include "MyTimer.h"
#include "..\MQ2AFNuke\Enchanter.h"

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
// This broke some shit... Guess I can't be lazy!
//PCHARINFO Character::info = GetCharInfo();
//PCHARINFO2 Character::info2 = GetCharInfo2();

Healer *Me = NULL;
//Some of these are stolen from FastHeal
bool AutoRez = true;
int HealPoint = 80;
int FastHealPoint = 70;
int GroupHealPoint = 80;
int MinGroupHealCount = 2;
int BigHealPoint = 50;
int PetHealPoint = 0;
int PatchHealPoint = 40;
int ClickHealPoint = 30;
int NukePoint = 40;
int DPSMode = 0;
bool DivineBuff = 0;
int SelfHealPoint = 40;
int SelfDAPoint = 20;
bool DoHealNukes = false;
bool DoNukeHeals = false;
bool ReleaseAndCampfire = false;
bool campfiretimer = false;
MyTimer ReleaseCampfireTimer;
bool CampFireOnZone = false;
bool CampFireResume = false;
bool DoFastHeals = true;
bool DoPromisedHeals = true;
bool DoDynamicHeals = false;
bool DoPatchHeals = true;
bool DoBigHeals = true;
bool DoLightHeals = true;
bool DoClickHeals = true;
bool DoHotHeals = true;
bool DoHotGroupHeals = true;
bool DoGroupHeals = true;
bool DoYaulp = false;
bool DoReviveMerc = true;
bool DoHoTs = false;
bool DoCombatRez = true;
bool DoRez = true;
bool DoMed = true;
bool DoFade = true;
int FadeAggroPct = 95;
bool DoMemSpells = false;
int MedPct = 98;
bool OutDoors = false;
bool DoMount = true;
int BuffGem = 10;
bool DoCombatBuffs = false;
bool DoBuffs = true;
bool *pbStickOn = NULL;
int PullerHealRange = 50;
bool TankHeal = false;
bool XTankHeal = false;
bool GTankHeal = false;
bool Harvest = true;
bool DoDebug = false;
bool PauseOnZone = false;
bool HealPets = true;
int TankHealPoint=90;
bool RaidTankMode=false;

// Borrowed from PeteSampras's MQ2Bot
vector<PSPELL> vHeal;
vector<string> vHealName;
map<string, string> SpellIf;
map<string, string>::iterator SpellIt;

// Cleric Light Spells (slow heals)
const int LightHealCount = 41;
CHAR LightHeals[LightHealCount][MAX_SPELL] = {
	// Shared/Low Level Spells...
	"Chloroblast",				// Druid Level  55 / Shaman 55 / Ranger & Beastlord 62
	"Superior Healing",			// CLR/30 PAL/57 DRU/51 SHM/51
	"Healing Water",			// DRU/44
	"Greater Healing",			// CLR/20 PAL/36 RNG/57 DRU/29 SHM/29 BST/57
	"Healing",					// CLR/10 PAL/27 RNG/38 DRU/19 SHM/19 BST/36
	"Light Healing",			// CLR/4  PAL/12 RNG/21 DRU/9  SHM/9  BST/20
	"Minor Healing",			// CLR/1  PAL/6  RNG/8  DRU/1  SHM/1  BST/6
	"Salve",					//        PAL/1  RNG/1                BST/1
};

typedef struct _BuffType {
	char *Name;			// Name of the spell to cast
	char *GroupName;	// Name of the group version
	char *BuffName;		// Name of the buff to check for
	int MinLevel;		// Minimum level required for single target version
	int MinGroupLevel;	// Minimum level for group version
}BuffType, *PBuffType;

/*
BuffType DIBuffs[] = {
	{"Divine Indemnification",	NULL, NULL, 70},	//Level 102
	{"Divine Interposition",	NULL, NULL, 70},	//Level 97
	{"Divine Invocation",		NULL, NULL, 70},	//Level 92
	{"Divine Intercession",		NULL, NULL, 70},	//Level 87
	{"Divine Intervention",		NULL, NULL, 46}		//Level 60
};

BuffType AegolismBuffs[] = {
	{ "Surety",		"Unified Hand of Surety",		"Surety",		70 },	// Level 102/105
	{ "Certitude",	"Unified Hand of Certitude",	"Certitude",	70 },	// Level 97/100
	{ "Credence",	"Hand of Credence",				"Credence",		70 },
	{ "Reliance",	"Hand of Reliance",				"Reliance",		70 },
	{ "Gallantry",	"Hand of Gallantry",			"Gallantry",	70 },
	{ "Temerity",	"Hand of Temerity",				"Temerity",		70 },
	{ "Tenacity",	"Hand of Tenacity",				"Tenacity",		70 },
	{ "Conviction",	"Hand of Conviction",			"Conviction",	70 },
	{ "Virtue",		"Hand of Virtue",				"Virtue",		70 },
	{ "Aegolism",	"Blessing of Aegolism",			"Aegolism",		70 },
	{ "Temperance",	"Blessing of Temperance",		"Temperance",	70 },
};

BuffType SymbolBuffs[] = {
	{ "Symbol of Nonia",			"Unified Hand of Nonia",	"Symbol of Nonia",				70 },	// Level 101/105
	{ "Symbol of Gezat",			"Unified Hand of Gezat",	"Symbol of Gezat",				70 },	// Level 96/100
	{ "Symbol of the Triumvirate",	"Unity of the Triumvirate",	"Symbol of the Triumvirate",	70 },	// Level 91/95
	{ "Symbol of Ealdun",			"Ealdun's Mark",			"Symbol of Ealdun",				70 },	// Level 86/90
	{ "Symbol of Darianna",			"Darianna's Mark",			"Symbol of Darianna",			70 },	// Level 81/85
	{ "Symbol of Kaerra",			"Kaerra's Mark",			"Symbol of Kaerra",				70 },	// Level 76/80
	{ "Symbol of Elushar",			"Elushar's Mark",			"Symbol of Elushar",			70 },	// Level 71/75
	{ "Symbol of Balikor",			"Balikor's Mark",			"Symbol of Balikor",			70 },	// Level 66/70
	{ "Symbol of Kazad",			"Kazad's Mark",				"Symbol of Kazad",				70 },	// Level 61/63
	{ "Symbol of Marzin",			"Marzin's Mark",			"Symbol of Marzin",				70 },	// Level CLR 54/PAL 63 / CLR 60
	{ "Symbol of Naltron",			"Naltron's Mark",			"Symbol of Naltron",			70 },	// Level CLR 41/PAL 58 / CLR 58
	{ "Symbol of Pinzarn",			NULL,	NULL,	1 },	// Level CLR 41 / PAL 58
	{ "Symbol of Ryltan",			NULL,	NULL,	1 },	// Level 21
	{ "Symbol of Transal",			NULL,	NULL,	1 },	// Level 11
};
*/
/*
// Cleric HP Buffs
const int AegolismBuffCount = 7;
CHAR xAegolismBuffs[AegolismBuffCount][MAX_SPELL] = {
	"Promised Rehabilitation",	// Level 103
	"Promised Reformation",		// Level  98
	"Promised Restitution",		// Level  93
	"Promised Resurgence",		// Level  88
	"Promised Recuperation",	// Level  83
	"Promised Restoration",		// Level  78
	"Promised Renewal",			// Level  73
};

const int GroupAegolismBuffCount = 7;
CHAR GroupAegolismBuffs[GroupAegolismBuffCount][MAX_SPELL] = {
	"Promised Rehabilitation",	// Level 103
	"Promised Reformation",		// Level  98
	"Promised Restitution",		// Level  93
	"Promised Resurgence",		// Level  88
	"Promised Recuperation",	// Level  83
	"Promised Restoration",		// Level  78
	"Promised Renewal",			// Level  73
};
*/
/*
// Druid/Shaman Spells...
const int GrowthSpellCount = 7;
CHAR GrowthSpells[GrowthSpellCount][MAX_SPELL] = {
	"Ferociuos Growth",		// Level SHM 101 / DRU 102
	"Rampant Growth",		// Level SHM 96 / DRU 97
	"Unfettered Growth",	// Level SHM 91 / DRU 92
	"Untamed Growth",		// Level SHM 86 / DRU 87
	"Wild Growth"			// Level SHM 81 / DRU 82
};

// Druid Skin Spells
BuffType SkinBuffs[] = {
	{ "Shieldstone Skin",			"Shieldstone Blessing",			"Shieldstone Skin",			70 },	// Level 102/105
	{ "Granitebark Skin",			"Granitebark Blessing",			"Granitebark Skin",			70 },	// Level 97/100
	{ "Stonebark Skin",				"Stonebark Blessing",			"Stonebark Skin",			70 },	// Level 92/95
	{ "Timbercore Skin",			"Blessing of the Timbercore",	"Timbercore Skin",			70 },	// Level 87/90
	{ "Heartwood Skin",				"Blessing of the Heartwood",	"Heartwood Skin",			70 },	// Level 82/85
	{ "Ironwood Skin",				"Blessing of the Ironwood",		"Ironwood Skin",			70 },	// Level 77/80
	{ "Direwild Skin",				"Blessing of the Direwild",		"Direwild Skin",			70 },	// Level 72/75
	{ "Steeloak Skin",				"Blessing of Steeloak",			"Steeloak Skin",			70 },	// Level 68/70
	{ "Protection of the Nine",		"Blessing of the Nine",			"Protection of the Nine",	70 },	// Level 62/63
	{ "Protection of the Cabbage",	"Protection of the Glades",		"Protection of the Glades",	70 },	// Level CLR 54/PAL 63 / CLR 60
	{ "Symbol of Naltron",			"Naltron's Mark",			"Symbol of Naltron",			70 },	// Level CLR 41/PAL 58 / CLR 58
	{ "Symbol of Pinzarn",			NULL,	NULL,	1 },	// Level CLR 41 / PAL 58
	{ "Symbol of Ryltan",			NULL,	NULL,	1 },	// Level 21
	{ "Symbol of Transal",			NULL,	NULL,	1 },	// Level 11
};
*/
// Mount
CHAR MountItem		[MAX_SPELL] = "Verdant Hedgerow Leaf";

// Cleric Nukes
//CHAR TwinNuke		[MAX_SPELL] = "Glorious Rebuke";
//CHAR DpsNuke		[MAX_SPELL] = "Justice";
//CHAR DpsNuke		[MAX_SPELL] = "Castigation";

// Buffs
CHAR ReverseDS		[MAX_SPELL] = "Mark of the Devoted";
CHAR SelfBuff		[MAX_SPELL] = "Armor of the Reverent";
CHAR GroupBuff		[MAX_SPELL] = "Unified Hand of Certitude";
CHAR GroupBuffCheck	[MAX_SPELL] = "Blessing of Fervor";
CHAR TankBuff		[MAX_SPELL] = "Shining Bastion";
CHAR SymbolBuff		[MAX_SPELL] = "Unified Hand of Gezat";
CHAR AuraBuff1		[MAX_SPELL] = "Aura of Divinity";
CHAR AuraBuff2		[MAX_SPELL] = "Aura of the Reverent";
CHAR DivineAuraBuff	[MAX_SPELL] = "Divine Indemnification";

// Timers
MyTimer PromisedTimer;

bool HealsOn = false;		// True=enabled, false=disabled
bool HealPause = false;		// True=paused, false=running
bool RezOn = true;			// True=rez corpses
int HealOrder = 0;
	// Default: 0
	// 0=Self,XTarget,Group
	// 1=Self,Group,XTarget
	// 2=XTarget,Self,Group
	// 3=XTarget,Group,Self
	// 4=Group,Self,Xtarget
	// 5=Group,XTarget,Self
int HealState = 0;
int HealType=0; // 0 = Single-Any, 1 = Group, 2=Single-Tank, 3=DA, 4=Buff

PSPAWNINFO pHealTarget; // Target for incoming heal
int NumHeals = 0;
bool Initialized = false;
int CastState = 0;
bool UseEqbc = false;

/*
LARGE_INTEGER StartTime;			// For high-resolution timer...
LARGE_INTEGER EndTime;				// For high-resolution timer...
LARGE_INTEGER ElapsedTime;			// For high-resolution timer...
LARGE_INTEGER Frequency;			// For high-resolution timer...

int MinHealDelay = 2000; // minimum wait between heals in ms...
int ReTargetCount=0;
*/

// 100 Cleric Spells
/*
// Level 100
MYSPELL_INFO spell_list_100_nuke[] = {
	{ "Ward of Surety", 1, HEALTYPE_TANKONLY },
	{ "Spiritual Remedy", 2, HEALTYPE_SINGLE },
	{ "Virtuous Intervention", 3, HEALTYPE_SINGLE },
	{ "Elysian Intervention", 4, HEALTYPE_SINGLE },
	{ "Celestial Intervention", 5, HEALTYPE_SINGLE },
	{ "Fraught Renewal", 6, HEALTYPE_SINGLE },
	{ "Fervent Renewal", 7, HEALTYPE_SINGLE },
	{ "Frenzied Renewal", 8, HEALTYPE_SINGLE },
	{ "Syllable of Renewal", 9, HEALTYPE_GROUP },
	{ "Word of Reformation", 10, HEALTYPE_GROUP },
	{ "Shining Bastion", 11, HEALTYPE_BUFF },
	{ "Divine Interposition", 12, HEALTYPE_DI },
	{ NULL, 0 }
};

MYSPELL_INFO spell_list_100_no_nuke[] = {
	{ "Ward of Certitude", 1 },
	{ "Graceful Remedy", 2 },
	{ "Fraught Renewal", 3 },
	{ "Fervent Renewal", 4 },
	{ "Frenzied Renewal", 5 },
	{ "Reverent Light", 6 },
	{ "Virtuous Intervention", 7 },
	{ "Elysian Intervention", 8 },
	{ "Syllable of Renewal", 9 },
	{ "Word of Reformation", 10 },
	{ "Shining Bastion", 11 },
	{ "Divine Interposition", 12 },
	{NULL,0}
};
*/

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

// Borrowed from PeteSampras's MQ2Bot
void CreateHeal();

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

enum TargetType {
	Line_of_Sight = 1,
	Caster_AE = 2,
	Caster_Group = 3,
	Caster_PB = 4,
	Single = 5,
	Self = 6,
	Target_AE = 8,
	Animal = 9,
	Undead = 10,
	Summoned = 11,
	Lifetap = 13,
	Pet = 14,
	Corpse = 15,
	Plant = 16,
	Old_Giants = 17,
	Old_Dragons = 18,
	Target_AE_Lifetap = 20,
	Target_AE_Undead = 24,
	Target_AE_Summoned = 25,
	Hatelist = 32,
	Hatelist2 = 33,
	Chest = 34,
	Special_Muramites = 35, // bane for Ingenuity group trial in MPG
	Caster_PB_Players = 36,
	Caster_PB_NPC = 37,
	Pet2 = 38,
	No_Pets = 39, // single/group/ae ?
	Caster_AE_Players = 40, // bard AE hits all players
	Target_Group = 41,
	Directional_AE = 42,
	Frontal_AE = 44,
	Single_In_Group = 43,
	Target_Ring_AE = 45, // Splash
	Targets_Target = 46,
	Pet_Owner = 47,
	Target_AE_No_Players_Pets = 50 // blanket of forgetfullness. beneficial, AE mem blur, with max targets
};

enum SpellCategory {
	Nuke = 25, // DD-Nuke
	Heal = 42 // Heal
};

enum SubCategory {
	Unk_1 = 14, // Ice Resist
	Unk0 = 38, // Fire Resist
	Unk1 = 58, // Magic Resist
	Unk2 = 137 // Chromatic Resist, Single, Detrimental, Evocation, DD - Magic
};

void CreateHeal()
{
	bool BardClass = (Me->Class() == Bard);
//	if (!InGameOK())
//		return;
	if (BardClass || GetCharInfo()->pSpawn->mActorClient.Class == 11 || GetCharInfo()->pSpawn->mActorClient.Class == 12 || GetCharInfo()->pSpawn->mActorClient.Class == 13 || GetCharInfo()->pSpawn->mActorClient.Class == 14)
		return;
	vHeal.clear();
	vHealName.clear();
	PCHAR szHeal[] = { "Divine Arbitration", "Burst of Life", "Beacon of Life", "Focused Celestial Regeneration", "Celestial Regeneration",
		"Convergence of Spirits", "Union of Spirits", "Focused Paragon of Spirits", "Paragon of Spirit", "Lay on Hands",
		"Hand of Piety", "Ancestral Aid", "Call of the Ancients", "Exquisite Benediction", "Blessing of Tunare", NULL };
	char szBuffer[MAX_STRING];
	int aaIndex;
	_ALTABILITY* aa = nullptr;
	for (int i = 0; szHeal[i]; i++)
	{
		::sprintf_s(szBuffer, MAX_STRING, szHeal[i]);
		aaIndex = GetAAIndexByName(szBuffer);
		if (aaIndex > 0)
		{
		#ifndef EMU
			aa = pAltAdvManager->GetAAById(aaIndex, Me->Level());
		#else
			aa = pAltAdvManager->GetAAById(aaIndex);
		#endif
			if (aa && GetSpellByID(aa->SpellID))
			{
				vHeal.push_back(GetSpellByID(aa->SpellID));
				vHealName.push_back(szBuffer);
			}
		}
	}
	PSPELL pSpell;
	for (int i = 0; i < NUM_SPELL_GEMS; i++)
	{
		pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (pSpell)
		{
			if (pSpell->Category == Heal && pSpell->Subcategory != 19 && pSpell->Subcategory != 82 && (pSpell->TargetType == Target_Ring_AE || pSpell->TargetType == Caster_Group || pSpell->TargetType == Single || pSpell->TargetType == Self || pSpell->TargetType == Target_AE || pSpell->TargetType == Target_Group))
			{
				vHeal.push_back(pSpell);
				vHealName.push_back(pSpell->Name);
			}
		}
	}
	char szTemp[MAX_STRING] = { 0 }, szSpell[MAX_STRING];
	for (int i = 0; i < (int)vHeal.size(); i++)
	{
		::sprintf_s(szSpell, MAX_STRING, "HealIf%d", i);
		WriteChatf("\awHeal%d: \at%s", i, vHealName[i].c_str());
		if (GetPrivateProfileString(PLUGIN_NAME, szSpell, NULL, szTemp, MAX_STRING, INIFileName))
		{
			SpellIf.insert(make_pair<string, string>((string)szSpell, (string)szTemp));
			SpellIt = SpellIf.find(szSpell);
			if (SpellIt != SpellIf.end())
				WriteChatf("HealIf%d: %s", i, SpellIt->second.c_str());
		}
	}
}

void HealCommand(PSPAWNINFO pCHAR, PCHAR zLine)
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
		HealsOn = SetBOOL(HealsOn, Param1, PLUGIN_NAME, "HealsOn");
	else if (!_stricmp("off", Param1))
		HealsOn = SetBOOL(HealsOn, Param1, PLUGIN_NAME, "HealsOn");
	else if (!_stricmp("pause", Param1))
	{
		HealPause = true;
		Notify(PLUGIN_NAME, "Pause", "(\arPaused\ax)");
	}
	else if (!_stricmp("resume", Param1))
	{
		HealPause = false;
		HealsOn = true;
		Notify(PLUGIN_NAME, "Pause", "(\agResumed\ax) and (\agEnabled\ax)");
	}
	else if (!_stricmp("rez", Param1))
		DoRez = SetBOOL(DoRez, Param2, PLUGIN_NAME, "DoRez");
	else if (!_stricmp("combatrez", Param1))
		DoCombatRez = SetBOOL(DoCombatRez, Param2, PLUGIN_NAME, "DoCombatRez");
	else if (!_stricmp("promised", Param1))
		DoPromisedHeals = SetBOOL(DoPromisedHeals, Param2, PLUGIN_NAME, "DoPromisedHeals");
	else if (!_stricmp("patch", Param1))
		DoPatchHeals = SetBOOL(DoPatchHeals, Param2, PLUGIN_NAME, "DoPatchHeals");
	else if (!_stricmp("big", Param1))
		DoBigHeals = SetBOOL(DoBigHeals, Param2, PLUGIN_NAME, "DoBigHeals");
	else if (!_stricmp("click", Param1))
		DoClickHeals = SetBOOL(DoClickHeals, Param2, PLUGIN_NAME, "DoClickHeals");
	else if (!_stricmp("group", Param1))
		DoGroupHeals = SetBOOL(DoGroupHeals, Param2, PLUGIN_NAME, "DoGroupHeals");
	else if (!_stricmp("nukeheal", Param1))
		DoNukeHeals = SetBOOL(DoNukeHeals, Param2, PLUGIN_NAME, "DoNukeHeals");
	else if (!_stricmp("healnuke", Param1))
		DoHealNukes = SetBOOL(DoHealNukes, Param2, PLUGIN_NAME, "DoHealNukes");
	else if (!_stricmp("light", Param1))
		DoLightHeals = SetBOOL(DoLightHeals, Param2, PLUGIN_NAME, "DoLightHeals");
	else if (!_stricmp("revivemerc", Param1))
		DoReviveMerc = SetBOOL(DoReviveMerc, Param2, PLUGIN_NAME, "DoReviveMerc");
	else if (!_stricmp("campfirerez", Param1))
		ReleaseAndCampfire = SetBOOL(ReleaseAndCampfire, Param2, PLUGIN_NAME, "CampFireRez");
	else if (!_stricmp("dynamic", Param1))
		DoDynamicHeals = SetBOOL(DoDynamicHeals, Param2, PLUGIN_NAME, "DoDynamicHeals");
	else if (!_stricmp("debug", Param1))
		DoDebug = SetBOOL(DoDebug, Param2, PLUGIN_NAME, "Debug");
	else if (!_stricmp("raidtankmode", Param1))
		RaidTankMode = SetBOOL(RaidTankMode, Param2, PLUGIN_NAME, "RaidTankMode");
	else if (((!_stricmp("healpoint", Param1)) && IsNumber(Param2)))
		HealPoint = SetINT(HealPoint, Param2, PLUGIN_NAME, "HealPoint");
	else if (((!_stricmp("fasthealpoint", Param1)) && IsNumber(Param2)))
		FastHealPoint = SetINT(FastHealPoint, Param2, PLUGIN_NAME, "FastHealPoint");
	else if (((!_stricmp("tankhealpoint", Param1)) && IsNumber(Param2)))
		TankHealPoint = SetINT(TankHealPoint, Param2, PLUGIN_NAME, "TankHealPoint");
	else if (((!_stricmp("clickhealpoint", Param1)) && IsNumber(Param2)))
		ClickHealPoint = SetINT(ClickHealPoint, Param2, PLUGIN_NAME, "ClickHealPoint");
	else if (((!_stricmp("bighealpoint", Param1)) && IsNumber(Param2)))
		BigHealPoint = SetINT(BigHealPoint, Param2, PLUGIN_NAME, "BigHealPoint");
	else if (((!_stricmp("selfhealpoint", Param1)) && IsNumber(Param2)))
		SelfHealPoint = SetINT(HealPoint, Param2, PLUGIN_NAME, "SelfHealPoint");
	else if (((!_stricmp("grouphealpoint", Param1)) && IsNumber(Param2)))
		GroupHealPoint = SetINT(HealPoint, Param2, PLUGIN_NAME, "GroupHealPoint");
	else if (((!_stricmp("grouphealcount", Param1)) && IsNumber(Param2)))
		MinGroupHealCount = SetINT(MinGroupHealCount, Param2, PLUGIN_NAME, "GroupHealCount");
	else if (((!_stricmp("pullerhealrange", Param1)) && IsNumber(Param2)))
		PullerHealRange = SetINT(MinGroupHealCount, Param2, PLUGIN_NAME, "PullerHealRange");
	else if (((!_stricmp("medpct", Param1)) && IsNumber(Param2)))
		MedPct = SetINT(MedPct, Param2, PLUGIN_NAME, "MedPct");
	else if (!_stricmp("domed", Param1))
		DoMed = SetBOOL(DoMed, Param2, PLUGIN_NAME, "DoMed");
	else if (!_stricmp("dofade", Param1))
		DoMed = SetBOOL(DoFade, Param2, PLUGIN_NAME, "DoFade");
	else if (!_stricmp("domount", Param1))
		DoMount = SetBOOL(DoMount, Param2, PLUGIN_NAME, "DoMount");
	else if ((!_stricmp("mount", Param1)) || (!_stricmp("mountitem", Param1)))
	{
		if (Param2 && Param2[0])
		{
			memset(MountItem, 0, MAX_SPELL);
			strncpy_s(MountItem, Param2, strlen(Param2) < MAX_SPELL ? strlen(Param2) : MAX_SPELL);
			WritePrivateProfileString(PLUGIN_NAME, "MountItem", MountItem, INIFileName);
			Notify(PLUGIN_NAME, "MountItem","(\ag%s\ax)", MountItem);
		}
	}
	else if (!strcmp("settings", Param1))
	{
		ShowSettings();
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
	Notify(NULL, NULL, "/heals -> displays this help");
	Notify(NULL, NULL, "/heals help -> displays this help");
	Notify(NULL, NULL, "/heals settings -> displays settings");
	Notify(NULL, NULL, "/heals on|off -> Enable/Disable plugin");
	Notify(NULL, NULL, "/heals debug on|off -> Enable/Disable debug output (MQ2 chat spam!!!)");
	Notify(NULL, NULL, "/heals pause|resume -> Pause/Resume plugin action");
	Notify(NULL, NULL, "/heals pauseonzone on|off -> Pause/Resume plugin when changing zones");
	Notify(NULL, NULL, "/heals raidtankmode on|off -> Enable special raid tank healing priority");
	Notify(NULL, NULL, "=============Heal Points================");
	Notify(NULL, NULL, "/heals healpoint # -> Start healing at #% HPs");
	Notify(NULL, NULL, "/heals fasthealpoint # -> Use fast heals at #% HPs");
	Notify(NULL, NULL, "/heals clickhealpoint # -> Use clciky/emergency heals at #% HPs");
	Notify(NULL, NULL, "/heals bighealpoint # -> Use big heals at #% HPs");
	Notify(NULL, NULL, "/heals selfhealpoint # -> Heal self first at #% HPs");
	Notify(NULL, NULL, "/heals tankhealpoint # -> Heal tanks first at #% HPs (in RaidTankMode)");
	Notify(NULL, NULL, "/heals grouphealpoint # -> Check group members for group heals at #% HPs");
	Notify(NULL, NULL, "/heals grouphealcount # -> Use group heal when # members get below grouphealpoint HPs");
	Notify(NULL, NULL, "/heals dynamichealpoints on|off -> Dynamically determine heal points");
	Notify(NULL, NULL, "/heals pullerhealrange # -> Heal puller only if he/she is within # feet of healer.");
	Notify(NULL, NULL, "=============Heal Types=================");
	Notify(NULL, NULL, "/heals promised on|off -> Toggle promised heals");
	Notify(NULL, NULL, "/heals patch on|off -> Toggle patch heals");
	Notify(NULL, NULL, "/heals fast on|off -> Toggle fast heals");
	Notify(NULL, NULL, "/heals big on|off -> Toggle big heals");
	Notify(NULL, NULL, "/heals click on|off -> Toggle clicky heals/AA Heals");
	Notify(NULL, NULL, "/heals healnuke on|off -> Toggle heal nukes");
	Notify(NULL, NULL, "/heals nukeheal on|off -> Toggle nuke heals");
	Notify(NULL, NULL, "/heals light on|off -> Toggle light/slow heals");
	Notify(NULL, NULL, "/heals group on|off -> Toggle group heals (not healing group members, but group heal spells)");
	Notify(NULL, NULL, "=============Rez Options================");
	Notify(NULL, NULL, "/heals rez on|off -> Toggle Resurrect Spells/AAs while idle");
	Notify(NULL, NULL, "/heals combatrez on|off -> Toggle Resurrect Spells/AAs while in combat (between heals)");
	Notify(NULL, NULL, "/heals campfirerez on|off -> Toggle release hover, click campfire, and rez");
	Notify(NULL, NULL, "/heals revivemerc on|off -> Toggle revive dead mercenary");
	Notify(NULL, NULL, "=============Rez Options================");
	Notify(NULL, NULL, "/heals domed on|off -> Toggle Meditate on/off");
	Notify(NULL, NULL, "/heals medpct on|off -> Toggle Meditate at # percentage mana");
	Notify(NULL, NULL, "/heals domount on|off -> Toggle Mount on/off");
	Notify(NULL, NULL, "/heals mountitem \"Name of Mount Item\" -> Use this mount item");
	Notify(NULL, NULL, "============Buff Options================");
	Notify(NULL, NULL, "/heals buffs on|off -> Toggle Buff Spells while idle");
	Notify(NULL, NULL, "/heals combatbuff on|off -> Toggle buffs while in combat");
	Notify(NULL, NULL, "/heals buffgem # -> Spell gem # for buffs");

//	Notify(NULL, NULL, "/heals rez filter [filter] -> Rez according to filter (group, raid, xtarget, target, player)");
//	Notify(NULL, NULL, "  Ex: /heals rez group (rez anyone in group)");
//	Notify(NULL, NULL, "  Ex: /heals rez guild group (rez anyone in guild or group)");
//	Notify(NULL, NULL, "  Ex: /heals rez raid guild (rez anyone in raid or guild)");
//	Notify(NULL, NULL, "  Ex: /heals rez zone (rez anyone in the zone)");
}

// Sorta borrowed from MQ2Rez
/*
*/
void ShowSettings()
{
	Notify(NULL, NULL, "=============Plugin Options=============");
	Notify(PLUGIN_NAME, "Plugin","(\ag%s\ax).", (HealsOn ? "Enabled" : "Disabled"));
	Notify(PLUGIN_NAME, "Pause","(\ag%s\ax).", ( HealPause ? "Paused" : "Running"));
	Notify(PLUGIN_NAME, "Debug", "(\ag%s\ax).", (DoDebug ? "on" : "off"));
	Notify(PLUGIN_NAME, "PauseOnZone", "(\ag%s\ax).", (PauseOnZone ? "on" : "off"));
	Notify(PLUGIN_NAME, "RaidTankMode", "(\ag%s\ax).", (RaidTankMode ? "on" : "off"));
	Notify(NULL, NULL, "=============Heal Points================");
	Notify(PLUGIN_NAME, "HealPoint","(\ag%d\ax).", HealPoint);
	Notify(PLUGIN_NAME, "FastHealPoint","(\ag%d\ax).", FastHealPoint);
	Notify(PLUGIN_NAME, "ClickHealPoint","(\ag%d\ax).", ClickHealPoint);
	Notify(PLUGIN_NAME, "BigHealPoint","(\ag%d\ax).", BigHealPoint);
	Notify(PLUGIN_NAME, "SelfHealPoint","(\ag%d\ax).", SelfHealPoint);
	Notify(PLUGIN_NAME, "TankHealPoint","(\ag%d\ax).", TankHealPoint);
	Notify(PLUGIN_NAME, "GroupHealPoint","(\ag%d\ax).", GroupHealPoint);
	Notify(PLUGIN_NAME, "GroupHealCount","(\ag%d\ax).", MinGroupHealCount);
	Notify(PLUGIN_NAME, "DynamicHealPoints","(\ag%s\ax).", (DoDynamicHeals ? "on" : "off"));
	Notify(PLUGIN_NAME, "PullerHealRange", "(\ag%d\ax).", PullerHealRange);
	Notify(NULL, NULL, "=============Heal Types=================");
	Notify(PLUGIN_NAME, "PromisedHeals:","(\ag%s\ax).", (DoPromisedHeals ? "on" : "off"));
	Notify(PLUGIN_NAME, "PatchHeals","(\ag%s\ax).", (DoPatchHeals ? "on" : "off"));
	Notify(PLUGIN_NAME, "FastHeals","(\ag%s\ax).", (DoFastHeals ? "on" : "off"));
	Notify(PLUGIN_NAME, "BigHeals","(\ag%s\ax).", (DoBigHeals ? "on" : "off"));
	Notify(PLUGIN_NAME, "ClickHeals","(\ag%s\ax).", (DoClickHeals ? "on" : "off"));
	Notify(PLUGIN_NAME, "HealNukes","(\ag%s\ax).", (DoHealNukes ? "on" : "off"));
	Notify(PLUGIN_NAME, "NukeHeals","(\ag%s\ax).", (DoNukeHeals ? "on" : "off"));
	Notify(PLUGIN_NAME, "LightHeals","(\ag%s\ax).", (DoLightHeals ? "on" : "off"));
	Notify(PLUGIN_NAME, "HoTHeals", "(\ag%s\ax).", (DoHotHeals ? "on" : "off"));
	Notify(PLUGIN_NAME, "HoTGroupHeals", "(\ag%s\ax).", (DoHotGroupHeals ? "on" : "off"));
	Notify(PLUGIN_NAME, "GroupHeals", "(\ag%s\ax).", (DoGroupHeals ? "on" : "off"));
	//	Notify(NULL, NULL, "==================DPS===================");
	Notify(NULL, NULL, "==================Rez===================");
	Notify(PLUGIN_NAME, "Rez","(\ag%s\ax).", (DoRez ? "on" : "off"));
	Notify(PLUGIN_NAME, "CombatRez","(\ag%s\ax).", (DoCombatRez ? "on" : "off"));
	Notify(PLUGIN_NAME, "CampFireRez","(\ag%s\ax).", (ReleaseAndCampfire ? "on" : "off"));
	Notify(PLUGIN_NAME, "ReviveMerc","(\ag%s\ax).", (DoReviveMerc ? "on" : "off"));
	Notify(NULL, NULL, "==================Med===================");
	Notify(PLUGIN_NAME, "Med","(\ag%s\ax).", (DoMed ? "on" : "off"));
	Notify(PLUGIN_NAME, "MedPct","(\ag%d\ax).", MedPct);
	Notify(PLUGIN_NAME, "MountItem", "(\ag%s\ax).", MountItem);
	/*
	if (HealsOn)
		Notify(NULL, NULL, "Plugin is (\agEnalbed\ax)");
	else
		Notify(NULL, NULL, "Plugin is (\arDisabled\ax)");
	switch (HealOrder)
	{
		case 0:	Notify(PLUGIN_NAME, "Heal Order","(\agSelf, XTarget, Group\ax)"); break;
		case 1:	Notify(PLUGIN_NAME, "Heal Order","(\agSelf, Group, XTarget\ax)"); break;
		case 2:	Notify(PLUGIN_NAME, "Heal Order","(\agXTarget, Self, Group\ax)"); break;
		case 3:	Notify(PLUGIN_NAME, "Heal Order","(\agXTarget, Group, Self\ax)"); break;
		case 4:	Notify(PLUGIN_NAME, "Heal Order","(\agGroup, Self, Xtarget\ax)"); break;
		case 5:	Notify(PLUGIN_NAME, "Heal Order","(\agGroup, XTarget, Self\ax)"); break;
		default:break;
	}
*/
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
    DebugSpewAlways("Initializing MQ2Heals");
	if (EQBCConnected())
		UseEqbc = true;
	else
		UseEqbc = false;

    //Add commands, MQ2Data items, hooks, etc.
    AddCommand("/heals",HealCommand);
	Me = NULL;
    //AddXMLFile("MQUI_MyXMLFile.xml");
    //bmMyBenchmark=AddMQ2Benchmark("My Benchmark Name");
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
    DebugSpewAlways("Shutting down MQ2Heals");

    //Remove commands, MQ2Data items, hooks, etc.
    //RemoveMQ2Benchmark(bmMyBenchmark);
	if (Me)
	{
		delete Me;
		Me = NULL;
	}
    RemoveCommand("/heals");
    //RemoveXMLFile("MQUI_MyXMLFile.xml");
}

// Called after entering a new zone
PLUGIN_API VOID OnZoned(VOID)
{
	DebugSpewAlways("MQ2Heals::OnZoned()");
	if (HealsOn)
	{
		//	WriteChatColor("Zone change detected... Heals paused...");
		//	WriteChatColor("Use: '/heals resume' to resume heals.");
		if (!CampFireResume)	// Resume heal logic if we campfire'd back to zone...
		{
			HealPause = true;
			DNotify(PLUGIN_NAME, "OnZoned", "Zone change detected... Heals paused...");
			DNotify(PLUGIN_NAME, "OnZoned", "Use: '/heals resume' to resume heals.");
		}
		else
		{
			HealPause = false;
			CampFireResume = false;
			DNotify(PLUGIN_NAME, "OnZoned", "Attempt to use campfire clicky and rez...");
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
	HealsOn = GetPrivateProfileInt(PLUGIN_NAME, "Enabled", 0, INIFileName) ? true : false;
	DoRez = GetPrivateProfileInt(PLUGIN_NAME, "Rez", 1, INIFileName) ? true : false;
	DoCombatRez = GetPrivateProfileInt(PLUGIN_NAME, "CombatRez", 1, INIFileName) ? true : false;
	ReleaseAndCampfire = GetPrivateProfileInt(PLUGIN_NAME, "CampFireRez", 1, INIFileName) ? true : false;
	DoDynamicHeals = GetPrivateProfileInt(PLUGIN_NAME, "DynamicHeals", 0, INIFileName) ? true : false;
	DoReviveMerc = GetPrivateProfileInt(PLUGIN_NAME, "DoReviveMerc", 1, INIFileName) ? true : false;
	PauseOnZone = GetPrivateProfileInt(PLUGIN_NAME, "PauseOnZone", 0, INIFileName) ? true : false;
	DoDebug = GetPrivateProfileInt(PLUGIN_NAME, "Debug", 0, INIFileName) ? true : false;

	RaidTankMode = GetPrivateProfileInt(PLUGIN_NAME, "RaidTankMode", 0, INIFileName) ? true : false;
	DoPromisedHeals = GetPrivateProfileInt(PLUGIN_NAME, "DoPromisedHeals", 1, INIFileName) ? true : false;
	DoFastHeals = GetPrivateProfileInt(PLUGIN_NAME, "DoFastHeals", 1, INIFileName) ? true : false;
	DoPatchHeals = GetPrivateProfileInt(PLUGIN_NAME, "DoPatchHeals", 1, INIFileName) ? true : false;
	DoBigHeals = GetPrivateProfileInt(PLUGIN_NAME, "DoBigHeals", 1, INIFileName) ? true : false;
	DoClickHeals = GetPrivateProfileInt(PLUGIN_NAME, "DoClickHeals", 1, INIFileName) ? true : false;
	DoNukeHeals = GetPrivateProfileInt(PLUGIN_NAME, "DoNukeHeals", 1, INIFileName) ? true : false;
	DoHealNukes = GetPrivateProfileInt(PLUGIN_NAME, "DoHealNukes", 1, INIFileName) ? true : false;
	DoLightHeals = GetPrivateProfileInt(PLUGIN_NAME, "DoLightHeals", 1, INIFileName) ? true : false;
	DoGroupHeals = GetPrivateProfileInt(PLUGIN_NAME, "DoGroupHeals", 1, INIFileName) ? true : false;

	HealPoint = GetPrivateProfileInt(PLUGIN_NAME, "HealPoint", 80, INIFileName);
	FastHealPoint = GetPrivateProfileInt(PLUGIN_NAME, "FastHealPoint", 70, INIFileName);
	TankHealPoint = GetPrivateProfileInt(PLUGIN_NAME, "TankHealPoint", 90, INIFileName);
	BigHealPoint = GetPrivateProfileInt(PLUGIN_NAME, "BigHealPoint", 50, INIFileName);
	PatchHealPoint = GetPrivateProfileInt(PLUGIN_NAME, "PatchHealPoint", 40, INIFileName);
	ClickHealPoint = GetPrivateProfileInt(PLUGIN_NAME, "ClickHealPoint", 30, INIFileName);
	GroupHealPoint = GetPrivateProfileInt(PLUGIN_NAME, "GroupHealPoint", 80, INIFileName);
	MinGroupHealCount = GetPrivateProfileInt(PLUGIN_NAME, "GroupHealCount", 2, INIFileName);
	PullerHealRange = GetPrivateProfileInt(PLUGIN_NAME, "PullerHealRange", 50, INIFileName);
	SelfHealPoint = GetPrivateProfileInt(PLUGIN_NAME, "SelfHealPoint", 40, INIFileName);
	SelfDAPoint = GetPrivateProfileInt(PLUGIN_NAME, "SelfDAPoint", 20, INIFileName);

	//DPSMode			=	GetPrivateProfileInt(PLUGIN_NAME, "DPSMode", 0, INIFileName);
	DoYaulp = GetPrivateProfileInt(PLUGIN_NAME, "Yaulp", 1, INIFileName) ? true : false;
	DoMemSpells = GetPrivateProfileInt(PLUGIN_NAME, "MemSpells", 0, INIFileName) ? true : false;
	DoMed = GetPrivateProfileInt(PLUGIN_NAME, "DoMed", 1, INIFileName) ? true : false;
	DoMount = GetPrivateProfileInt(PLUGIN_NAME, "DoMount", 1, INIFileName) ? true : false;
	MedPct = GetPrivateProfileInt(PLUGIN_NAME, "MedPct", 98, INIFileName);
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
	WritePrivateProfileString(PLUGIN_NAME, "Enaled", HealsOn ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "Rez", DoRez ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "CombatRez", DoCombatRez ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "CampFireRez", ReleaseAndCampfire ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DynamicHeals", DoDynamicHeals ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoReviveMerc", DoReviveMerc ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "PauseOnZone", PauseOnZone ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "Debug", DoDebug ? "1" : "0", INIFileName);

	WritePrivateProfileString(PLUGIN_NAME, "DoPromisedHeals", DoPromisedHeals ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoFastHeals", DoFastHeals ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoPatchHeals", DoPatchHeals ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoBigHeals", DoBigHeals ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoClickHeals", DoClickHeals ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoNukeHeals", DoNukeHeals ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoHealNukes", DoHealNukes ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoLightHeals", DoLightHeals ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoGroupHeals", DoGroupHeals ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "RaidTankMode", RaidTankMode ? "1" : "0", INIFileName);

	WritePrivateProfileInt(PLUGIN_NAME, "HealPoint", HealPoint, INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "FastHealPoint", FastHealPoint, INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "TankHealPoint", TankHealPoint, INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "BigHealPoint", BigHealPoint, INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "PatchHealPoint", PatchHealPoint, INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "ClickHealPoint", ClickHealPoint, INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "GroupHealPoint", GroupHealPoint, INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "GroupHealCount", MinGroupHealCount, INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "PullerHealRange", PullerHealRange, INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "SelfHealPoint", SelfHealPoint, INIFileName);
	WritePrivateProfileInt(PLUGIN_NAME, "SelfDAPoint", SelfDAPoint, INIFileName);

	WritePrivateProfileString(PLUGIN_NAME, "Yaulp", DoYaulp ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "MemSpells", DoMemSpells ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoMed", DoMed ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "DoMount", DoMount ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "MedPct", MedPct ? "1" : "0", INIFileName);
	WritePrivateProfileString(PLUGIN_NAME, "MountItem", MountItem, INIFileName);
}

// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState)
{
//    DebugSpewAlways("MQ2Heals::SetGameState()");
	if ((GameState == GAMESTATE_INGAME) && (GetCharInfo()!=NULL) && (GetCharInfo2()!=NULL))
	{
		if (!Initialized)
		{
			LoadIni();
			HealPause = false;
			Initialized = true;
			if (Me)
			{
				delete Me;
				Me = NULL;
			}
			switch (GetCharInfo2()->Class)
			{
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
				default:
					Me = new Healer();
					SpellIf.clear();
					vHeal.clear();
					vHealName.clear();
					Notify(PLUGIN_NAME, "SetGameState", "Unsupported class...");
					Notify(PLUGIN_NAME, "SetGameState", "Heals will be in spellbar order with HealIfs (like MQ2Bot)...");
					CreateHeal();
				break;
			}
		}
	}
	else if (GameState != GAMESTATE_LOGGINGIN)
	{
		if (Initialized)
		{
			HealState = HEALSTATE_IDLE;
			HealPause = true;
			Initialized = false;
			HealsOn = false;
			Me = NULL;
			SpellIf.clear();
			vHeal.clear();
			vHealName.clear();
		}
	}
    // create custom windows if theyre not set up, etc
}

/*
PLUGIN_API VOID SetGameState(DWORD GameState) {
	if (GameState == GAMESTATE_INGAME) {
		if (!IniLoad) {
			sprintf(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
			NetStat = GetPrivateProfileInt("MQ2NetHeal", "Stat", 0, INIFileName);
			NetGrab = GetPrivateProfileInt("MQ2NetHeal", "Grab", 0, INIFileName);
			NetSend = GetPrivateProfileInt("MQ2NetHeal", "Send", 0, INIFileName);
			CureWatch = GetPrivateProfileInt("MQ2NetCure", "Watch", 0, INIFileName);
			CureAudio = GetPrivateProfileInt("MQ2NetCure", "Audio", 0, INIFileName);
			CurePopup = GetPrivateProfileInt("MQ2NetCure", "Popup", 0, INIFileName);
			CureQuiet = GetPrivateProfileInt("MQ2NetCure", "Quiet", 0, INIFileName);
			GetPrivateProfileString("MQ2NetCure", "Sound", "", CureSound, 2047, INIFileName);
			LifeWatch = GetPrivateProfileInt("MQ2NetWorst", "Watch", 0, INIFileName);
			LifeAudio = GetPrivateProfileInt("MQ2NetWorst", "Audio", 0, INIFileName);
			LifePopup = GetPrivateProfileInt("MQ2NetWorst", "Popup", 0, INIFileName);
			LifeQuiet = GetPrivateProfileInt("MQ2NetWorst", "Quiet", 0, INIFileName);
			GetPrivateProfileString("MQ2NetWorst", "Sound", "", LifeSound, 2047, INIFileName);
			GetPrivateProfileString("MQ2NetWorst", "Query", "", WorstQuery, 2047, INIFileName);
			IniLoad = true;
		}
	}
	else if (GameState != GAMESTATE_LOGGINGIN) {
		if (IniLoad) {
			NetStat = false;
			CureWatch = false;
			LifeWatch = false;
			IniLoad = false;
		}
	}
	Binding(GameState == GAMESTATE_INGAME);
}
*/

bool CheckPetHeal(int &_target, int &_worsthp, float &_dist)
{
	PSPAWNINFO pSpawn = NULL;
	int GroupMembersNeedingHeal = 0;
	int worsthp = 100;
	int worst = -1;

	if (!GetCharInfo()->pGroupInfo) return false;	// Are we in a group
	if (!GetCharInfo()->pGroupInfo->pMember) return false;	// If so, does the members pointer exist
	for (int x = 0; x < 6; x++)
	{
		if (!GetCharInfo()->pGroupInfo->pMember[x]) continue;
		if (!(pSpawn = GetCharInfo()->pGroupInfo->pMember[x]->pSpawn)) continue;
		if (!pSpawn->PetID) continue;
		EQPlayer* pet = GetSpawnByID(pSpawn->PetID);
		if (!pet) continue;
		float dist = GetDistance(GetCharInfo()->pSpawn, pet->Data.mPlayerPhysicsClient.pSpawn);
		if ((PlayerPctHPs(pet) < HealPoint) && (dist < 200))
		{
			if (PlayerPctHPs(pet) < worsthp)
			{
				worsthp = PlayerPctHPs(pet);
				worst = x;
				_dist = dist;
			}
		}
	}
	if (worst >= 0)
	{
		_worsthp = worsthp;
		_target = worst;
		return true;	// Does a pet need a heal
	}
	else
		return false;	// No Pets need healed
}

bool CheckGroupHeal(int &_target, int &_worsthp, bool &_groupheal, float &_dist)
{
	PSPAWNINFO pSpawn = NULL;
	int GroupMembersNeedingHeal=0;
	int worsthp = 100;
	int worst = -1;
	int tank = -1;
	int pcthps;
	PCHARINFO info=GetCharInfo();
	float dist = 0;
	float rdist = 0;
	GTankHeal = false;

	if (!info->pGroupInfo) return false;	// Are we in a group
	if (!info->pGroupInfo->pMember) return false;	// If so, does the members pointer exist
	for (int x = 0; x < 6; x++)
	{
		if (!info->pGroupInfo->pMember[x])
			continue;
		if (!(pSpawn=info->pGroupInfo->pMember[x]->pSpawn))
			continue;
		dist = GetDistance(info->pSpawn, pSpawn);
		if ((info->pGroupInfo->pMember[x]->Puller) && (dist > PullerHealRange))
			continue;
		pcthps = SpawnPctHPs(pSpawn);
		if ((pcthps < GroupHealPoint) && (pcthps>0) && (!IsCorpse(pSpawn)) && (!Hovering(pSpawn)) && (dist < 100))
		{
			GroupMembersNeedingHeal++;
			if (pcthps < worsthp)
			{
				worsthp = pcthps;
				worst = x;
				rdist = dist;
			}
		}
		else if ((pcthps < HealPoint) && (pcthps > 0) && (!IsCorpse(pSpawn)) && (!Hovering(pSpawn)) && (dist < 200))
		{
			if (pcthps < worsthp)
			{
				worsthp = pcthps;
				worst = x;
				rdist = dist;
			}
		}
		if (pSpawn->PetID)
		{
			EQPlayer* pet = GetSpawnByID(pSpawn->PetID);
			if (pet)
			{
				pcthps = PlayerPctHPs(pet);
				dist = GetDistance(info->pSpawn, pet->Data.mPlayerPhysicsClient.pSpawn);
				if ((pcthps < GroupHealPoint) && (pcthps>0) && (!IsCorpse(pSpawn)) && (!Hovering(pSpawn)) && (dist < 100))
				{
					GroupMembersNeedingHeal++;
				}
			}
		}
	}
	if (GroupMembersNeedingHeal >= MinGroupHealCount)
		_groupheal = true;
	else _groupheal = false;

	if ((info->pGroupInfo) && (info->pGroupInfo->pMember) && (info->pGroupInfo->pMember[worst]) && (info->pGroupInfo->pMember[worst]->MainTank))
		GTankHeal = true;
	_worsthp = worsthp;
	_target = worst;
	_dist = rdist;

	if (worst >= 0)
		return true;
	else
		return false;
}

bool CheckXTargetHeal(int &_target, int &_worsthp, float &_dist)
{
	int worst = -1;
	int worsthp = 100;
	float worstdist = 0;
	int first = -1;
	XTankHeal = false;
	PSPAWNINFO pSpawn = NULL;
	PCHARINFO info = GetCharInfo();
	if (!info)
		return false;
	if (!info->pXTargetMgr)
		return false;
	if (!info->pXTargetMgr->XTargetSlots.Count)
		return false;
	DWORD XTargetCount = info->pXTargetMgr->XTargetSlots.Count;
	for (int x = 0; x < (int)XTargetCount; x++)
	{
		XTARGETSLOT xts = info->pXTargetMgr->XTargetSlots[x];
		if ((xts.xTargetType == XTARGET_SPECIFIC_NPC) || (xts.xTargetType == XTARGET_SPECIFIC_PC)) {
			if (EQPlayer *spawn = GetSpawnByID(xts.SpawnID)) {
				if (first == -1)
					first = x;
				int hp = PlayerPctHPs(spawn);
				float dist = GetDistance(info->pSpawn, spawn->Data.mPlayerPhysicsClient.pSpawn);
				if (((hp < HealPoint) && (hp < worsthp) && (!IsCorpse(spawn->Data.mPlayerPhysicsClient.pSpawn)) && (!Hovering(spawn->Data.mPlayerPhysicsClient.pSpawn)) && (dist < 200))
					|| (hp < TankHealPoint) && (RaidTankMode))
				{
					worsthp = hp;
					worst = x;
					worstdist = dist;
					if((hp < TankHealPoint) && RaidTankMode)
					{
						XTankHeal=true;
						_target=x;
						_worsthp=hp;
						_dist=dist;
						return true;
					}
				}
			}
		}
	}

	_target = worst;
	_worsthp = worsthp;
	_dist = worstdist;
	if ((_worsthp == first) && (first >= 0))
		XTankHeal = true;

	if (worst >= 0)	// Does an XTarget need a heal?
		return true;
	return false;
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

bool CheckForHeals()
{
	PSPAWNINFO pSpawn = NULL;
	int worsthp = 100;
	int gworst = 100;
	int xworst = 100;
	int pworst = 100;
	float gworstd = 0;
	float xworstd = 0;
	float pworstd = 0;
	int gtarget = -1;
	int xtarget = -1;
	int ptarget = -1;
	bool groupheal = false;
	bool grouphealspell = false;
	bool xheal = false;
	bool petheal = false;
	int worst = -1;
	int heal_target_type = TARGET_NO_HEAL;
	PCHARINFO info=GetCharInfo();
	if (!info)
		return false;
	TankHeal = false;

	pHealTarget = NULL;
	if (Me->PctHPs() < SelfDAPoint)
	{
		pHealTarget = info->pSpawn;
		HealType = HEALTYPE_DI;
		HealState = HEALSTATE_CASTSTART;
		CastState = CASTSTATE_STARTED;
		return true;
	}
	if (Me->PctHPs() <= SelfHealPoint)
	{
		pHealTarget = info->pSpawn;
		HealType = HEALTYPE_SINGLE;
		HealState = HEALSTATE_CASTSTART;
		CastState = CASTSTATE_STARTED;
		return true;
	}
	// If it's not an emergency, add us to the worst if we need a heal...
	if (Me->PctHPs() < HealPoint)
	{
		worsthp = Me->PctHPs();
		heal_target_type = TARGET_SELF_HEAL;
	}

	xheal = CheckXTargetHeal(xtarget, xworst, xworstd);
	groupheal = CheckGroupHeal(gtarget, gworst, grouphealspell, gworstd);
	petheal = CheckPetHeal(ptarget, pworst, pworstd);

	if ((xheal) && (xworst < worsthp) &&
		((xworst<gworst) || (xworst<BigHealPoint)) &&
		((xworst<pworst) || (xworst<BigHealPoint)))
	{
		worsthp = xworst;
		heal_target_type = TARGET_XTAR_HEAL;
		if (XTankHeal) TankHeal = true;
	}
	else if ((xheal) && RaidTankMode)
	{
		if (ExtendedTargetList*xtl = info->pXTargetMgr) {
			if (xtl->XTargetSlots.Count>=xtarget && gtarget>=0) {
				if (xtl->XTargetSlots[xtarget].SpawnID == info->pGroupInfo->pMember[gtarget]->pSpawn->SpawnID)
				{
					string HealSpell;
					if (Me->GroupHealReady(HealSpell, 100))
					{
						heal_target_type = TARGET_GROUP_GROUP_HEAL;
					}
					else
					{
						heal_target_type = TARGET_XTAR_HEAL;
					}
				}
				else {
					heal_target_type = TARGET_XTAR_HEAL;
				}
			}
		}
	}
	else if ((groupheal) && (gworst<worsthp) &&
		((gworst<pworst) || (gworst<BigHealPoint)))
	{
		worsthp = gworst;
		if (GTankHeal) TankHeal = true;
		if (grouphealspell)
			heal_target_type = TARGET_GROUP_GROUP_HEAL;
		else
			heal_target_type = TARGET_GROUP_SINGLE_HEAL;
		if (gworst<BigHealPoint)
			heal_target_type = TARGET_GROUP_SINGLE_HEAL;
	}
	else if ((pworst < worsthp)	&& (xworst > BigHealPoint) && (gworst > BigHealPoint) && (!grouphealspell))
	{
		worsthp = pworst;
		heal_target_type = TARGET_PET_HEAL;
	}
	switch (heal_target_type)
	{
		case TARGET_NO_HEAL:
		{
			HealState = HEALSTATE_IDLE;
			CastState = CASTSTATE_STARTED;
			return false;
		}break;
		case TARGET_SELF_HEAL:
		{
			HealType = HEALTYPE_SINGLE;
			HealState = HEALSTATE_CASTSTART;
			CastState = CASTSTATE_STARTED;
			pHealTarget = GetCharInfo()->pSpawn;
			return true;
		}break;
		case TARGET_GROUP_SINGLE_HEAL:
		{
			// Try to prevent crashes due to bad pointers... In case group explodes...
			if (!info->pGroupInfo) return false;
			if (!info->pGroupInfo->pMember) return false;
			if (!info->pGroupInfo->pMember[gtarget]) return false;
			HealType = HEALTYPE_SINGLE;
			HealState = HEALSTATE_CASTSTART;
			CastState = CASTSTATE_STARTED;
			pHealTarget = info->pGroupInfo->pMember[gtarget]->pSpawn;
			return true;
		}break;
		case TARGET_GROUP_GROUP_HEAL:
		{
			// Try to prevent crashes due to bad pointers... In case group explodes...
			if (!info->pGroupInfo) return false;
			if (!info->pGroupInfo->pMember) return false;
			if (!info->pGroupInfo->pMember[gtarget]) return false;
			HealType = HEALTYPE_GROUP;
			HealState = HEALSTATE_CASTSTART;
			CastState = CASTSTATE_STARTED;
			pHealTarget = info->pGroupInfo->pMember[gtarget]->pSpawn;
			return true;
		}break;
		case TARGET_XTAR_HEAL:
		{
			if (!info)
				return false;
			if (ExtendedTargetList *xtl = info->pXTargetMgr) {
				if (xtl->XTargetSlots.Count>=xtarget) {
					if (EQPlayer *spawn = GetSpawnByID(xtl->XTargetSlots[xtarget].SpawnID)) {
						HealType = HEALTYPE_SINGLE;
						HealState = HEALSTATE_CASTSTART;
						CastState = CASTSTATE_STARTED;
						pHealTarget = spawn->Data.mPlayerPhysicsClient.pSpawn;
						return true;
					}
				}
			}
		}break;
		case TARGET_PET_HEAL:
		{
			if (ptarget < 0) return false;
			if (!info->pGroupInfo) return false;
			if (!info->pGroupInfo->pMember) return false;
			if (!info->pGroupInfo->pMember[ptarget]->pSpawn) return false;
			EQPlayer *spawn = GetSpawnByID(info->pGroupInfo->pMember[ptarget]->pSpawn->PetID);
			if (!spawn) return false;
			HealType = HEALTYPE_SINGLE;
			HealState = HEALSTATE_CASTSTART;
			CastState = CASTSTATE_STARTED;
			pHealTarget = spawn->Data.mPlayerPhysicsClient.pSpawn;
			return true;
		}break;
		default: return false; break;
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

bool CastNextHeal()
{
	char tmp[MAX_STRING] = { 0 };
	//char * HealSpell = NULL;
	string HealSpell;
	HealSpell.clear();

	if (!pHealTarget)
	{
//		if (!UseEqbc)
//			Notify(NULL, NULL, "MQ2Heals::CastNextHeal - No target to heal.");
//		else
//			EzCommand("/bc MQ2Heals::CastNextHeal - No target to heal.");
		DNotify(PLUGIN_NAME, "CastNextHeal", "- No target to heal.");
		HealState = HEALSTATE_IDLE;
		CastState = CASTSTATE_STARTED;
		return false;
	}

	// Remove Targeting logic, use `/casting "Spell" -targetid|TargetID` instead
	if (CastState == CASTSTATE_STARTED) CastState = CASTSTATE_TARGETING;
	switch (CastState)
	{
		case CASTSTATE_STARTED:
		{
//			_snprintf_s(tmp, MAX_STRING, "/target %s", pHealTarget->Name);
//			EzCommand(tmp);
			CastState = CASTSTATE_TARGETING;
//			QueryPerformanceCounter(&StartTime);
		}break;
		case CASTSTATE_TARGETING:
		{
			bool alt = false;
			bool item = false;
			bool targetid = true;
			bool nh = false;
			switch (HealType)
			{
				case HEALTYPE_DI:
				{
					targetid = true;
					HealSpell = DivineAuraBuff;
				}break;
				case HEALTYPE_SINGLE:
				{
					float dist = 0;
					int PctHPs=SpawnPctHPs(pHealTarget);
					dist = GetDistance(GetCharInfo()->pSpawn, pHealTarget);
					if ((Me->Class() == Cleric) && (PctHPs < ClickHealPoint) && (Me->IsAAReady("Burst of Life"))) {
						alt = true;
						HealSpell = "Burst of Life";
					}
					else if ((Me->Class() == Cleric) && (PctHPs < ClickHealPoint) && (Me->IsAAReady("Divine Arbitration"))) {
						alt = true;
						targetid = false;
						HealSpell = "Divine Arbitration";
					}
					//else if ((PctHPs < ClickHealPoint) && (IsItemReady(""))) {
					//}
					else if ((PctHPs < PatchHealPoint) && (Me->PatchHealReady(HealSpell, dist))) {
					}
					else if ((PctHPs < BigHealPoint) && TankHeal && (Me->Class()==Cleric) && (((CCleric *)Me)->WardSpellReady(HealSpell, dist))) {
					}
					else if ((PctHPs < BigHealPoint) && (Me->BigHealReady(HealSpell, dist))) {
					}
					else if ((PctHPs < FastHealPoint) && TankHeal && (Me->HealNukeReady(HealSpell, dist))) {
					}
					else if ((PctHPs < FastHealPoint) && TankHeal && (Me->NukeHealReady(HealSpell, dist))) {
						nh = true;
					}
					else if ((PctHPs < FastHealPoint) && (Me->FastHealReady(HealSpell, dist))) {
					}
					else if ((PctHPs < HealPoint) && (Me->SlowHealReady(HealSpell, dist))) {
					}
					else if ((PctHPs < HealPoint) && TankHeal && (Me->HealNukeReady(HealSpell, dist))) {
					}
					else if ((PctHPs < HealPoint) && TankHeal && (Me->NukeHealReady(HealSpell, dist))) {
						nh = true;
					}
					else if ((PctHPs < HealPoint) && (Me->FastHealReady(HealSpell, dist))) {
					}
					else if ((PctHPs < HealPoint) && (Me->BigHealReady(HealSpell, dist))) {
					}
					else if ((PctHPs < HealPoint) && (Me->PatchHealReady(HealSpell, dist))) {
					}
					else if ((PctHPs < HealPoint) && (Me->HealNukeReady(HealSpell, dist))) {
					}
					else if ((PctHPs < HealPoint) && (Me->NukeHealReady(HealSpell, dist))) {
						nh = true;
					}
					else {
						CastState = CASTSTATE_STARTED;
						HealState = HEALSTATE_IDLE;
						//WriteChatColor("No heal needed?");
						DNotify(PLUGIN_NAME, "CastNextHeal", "No heal available/needed?");
					}
				}break;
				case HEALTYPE_GROUP:
				{
					float dist = 0;
					dist = GetDistance(GetCharInfo()->pSpawn, pHealTarget);
					if (Me->GroupHealReady(HealSpell, dist)) {
					}
					else if (Me->IsAAReady("Divine Arbitration")) {
						alt = true;
						HealSpell = "Divine Arbitration";
					}
				}break;
				default:break;
			}
			if (!HealSpell.empty())
			{
				if (nh)
					_snprintf_s(tmp, MAX_STRING, "/casting \"%s\" alt -targetid|%d", HealSpell.c_str(), pHealTarget->TargetOfTarget);
				else if (alt)
					if (targetid)
						_snprintf_s(tmp, MAX_STRING, "/casting \"%s\" alt -targetid|%d", HealSpell.c_str(), pHealTarget->SpawnID);
					else
						_snprintf_s(tmp, MAX_STRING, "/casting \"%s\" alt", HealSpell.c_str());
				else if (item)
					if (targetid)
						_snprintf_s(tmp, MAX_STRING, "/casting \"%s\" item -targetid|%d", HealSpell.c_str(), pHealTarget->SpawnID);
					else
						_snprintf_s(tmp, MAX_STRING, "/casting \"%s\" item", HealSpell.c_str());
				else
					if (targetid)
						_snprintf_s(tmp, MAX_STRING, "/casting \"${Spell[%s].RankName}\" -targetid|%d", HealSpell.c_str(), pHealTarget->SpawnID);
					else
						_snprintf_s(tmp, MAX_STRING, "/casting \"${Spell[%s].RankName}\"", HealSpell.c_str());
				EzCommand(tmp);
				DNotify(PLUGIN_NAME, "CastNextHeal", "on: \ay<<%s>>\ax with <<\ag%s\ax>>!", pHealTarget->Name, HealSpell.c_str());
				CastState = CASTSTATE_CASTING;
			}
			else
			{
				CastState = CASTSTATE_STARTED;
				HealState = HEALSTATE_IDLE;
				pHealTarget = NULL;
			}
		}break;
		case CASTSTATE_CASTING:
		{
			//ReTargetCount=0;
			if(!Me->CastingLeft())
			{
				HealState = HEALSTATE_CASTING;
				CastState = CASTSTATE_STARTED;
				pHealTarget = NULL;
			}
		}break;
		case CASTSTATE_DONE:
		{
			CastState = CASTSTATE_STARTED;
			HealState = HEALSTATE_IDLE;
		}
		default:break;
	}
	return true;
}

/*
Old Targeting Stuff
//			if (pTarget)
{
//				if (!strcmp(pTarget->Data.Name, pHealTarget->Name))
{
else // Wrong Target? Targeting Failed? -> ReTarget
				{
					QueryPerformanceCounter(&EndTime);
					ElapsedTiMe->QuadPart = EndTiMe->QuadPart - StartTiMe->QuadPart;
					ElapsedTiMe->QuadPart *= 1000;
					ElapsedTiMe->QuadPart /= Frequency.QuadPart;
					if (ElapsedTiMe->QuadPart >= 200)
					{
						CastState=CASTSTATE_TARGETING;
						ReTargetCount++;
						WriteChatf("Re-Targeting %s!", pHealTarget->Name);
					}
					if(ReTargetCount>=5)
					{
						CastState=CASTSTATE_STARTED;
						HealState=HEALSTATE_CASTFAIL;
						WriteChatf("Re-Targetint Failed!");
					}
				}
			}
			else // No Target... Targeting failed? -> ReTarget
			{
				QueryPerformanceCounter(&EndTime);
				ElapsedTiMe->QuadPart = EndTiMe->QuadPart - StartTiMe->QuadPart;
				ElapsedTiMe->QuadPart *= 1000;
				ElapsedTiMe->QuadPart /= Frequency.QuadPart;
				if (ElapsedTiMe->QuadPart >= 200)
				{
					CastState=CASTSTATE_TARGETING;
					ReTargetCount++;
					WriteChatf("Re-Targeting %s!", pHealTarget->Name);
				}
				if(ReTargetCount>=5)
				{
					CastState=CASTSTATE_STARTED;
					HealState=HEALSTATE_CASTFAIL;
					WriteChatf("Re-Targetint Failed!");
				}
			}
*/
bool Hovering(SPAWNINFO *pSpawn)
{
	return pSpawn->RespawnTimer ? true : false;
}

bool IsCorpse(SPAWNINFO *pSpawn)
{
	return (pSpawn->Type==SPAWN_CORPSE) ? true : false;
}

bool AmIHovering()
{
	if(Hovering(GetCharInfo()->pSpawn))	//Are we dead?
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
	PCHARINFO info=GetCharInfo();
	int merc = -1;
	bool foundmerc = false;
	if(!info->pGroupInfo)
		return false;
	if (!info->pGroupInfo->pMember)
		return false;
	for(int x=0; x<6; x++)
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
	int highest = 0, x=0;
	if(pAggroInfo)
	{
		highest = pAggroInfo->aggroData[AD_Player].AggroPct;
/*		for (x = 0; x < 20; x++)
		{
			if (pAggroInfo->aggroData[AD_xTarget1 + x].AggroPct > highest)
				highest = pAggroInfo->aggroData[AD_xTarget1 + x].AggroPct;
		}*/
	}
	return highest;
}

bool CheckSongs(PCHAR songname)
{
	for (unsigned long nBuff=0 ; nBuff < NUM_SHORT_BUFFS ; nBuff++)
	{
		if (PSPELL pSpell=GetSpellByID(GetCharInfo2()->ShortBuff[nBuff].SpellID))
		{
			if (!_strnicmp(songname,pSpell->Name,strlen(songname)))
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
	if(Me->IsAAReady("Divine Peace"))
	{
		EzCommand("/casting \"Divine Peace\" alt");
		return true;
	}
	else if (Me->IsAAReady("Blessing of Sanctuary"))
	{
		EzCommand("/casting \"Blessing of Sanctuary\" alt");
		return true;
	}
	else if (Me->IsAAReady("Sanctuary"))
	{
		EzCommand("/casting \"Sanctuary\" alt");
		return true;
	}
	return false;
}

bool DivineAuraCheck()
{
	if(CheckSongs("Touch of the Divine"))
	{
		EzCommand("/clickoff \"Touch of the Divine\"");
		return true;
	}
	if (PctAggro() > 95)
		return AggroCheck();
	return false;
}

bool CheckPromisedHeals()
{
	bool GroupTank = false;
	bool XTargetTank = false;
	string HealSpell;
	char tmp[MAX_STRING] = { 0 };
	PCHARINFO info = GetCharInfo();
	PSPAWNINFO pPromisedTarget = NULL;
	HealSpell.clear();

	if (!DoPromisedHeals) return false;
	if (PGROUPINFO ginfo = info->pGroupInfo) // Are we in a group
		if (ginfo->pMember) // If so, does the members pointer exist
			for (int x = 0; x < 6; x++)
			{
				if (!ginfo->pMember[x])
					continue;
				if (!(ginfo->pMember[x]->pSpawn))
					continue;
				if (ginfo->pMember[x]->MainTank)
				{
					bool found = false;
					GroupTank = true;
					pPromisedTarget = ginfo->pMember[x]->pSpawn;
					break;
				}
			}
	if (ExtendedTargetList *xtl = info->pXTargetMgr) { // XTarget Tank takes priority over group tank...
		for (int x = 0; x < xtl->XTargetSlots.Count; x++) {
			XTARGETSLOT xts = xtl->XTargetSlots[x];
			if ((xts.xTargetType == XTARGET_SPECIFIC_NPC) || (xts.xTargetType == XTARGET_SPECIFIC_PC)) {
				if(EQPlayer *spawn = GetSpawnByID(xts.SpawnID)) {
					pPromisedTarget = spawn->Data.mPlayerPhysicsClient.pSpawn;
					XTargetTank = true;
					break;
				}
			}
		}
	}
	if ((XTargetTank) || (GroupTank))
	{
		if (Me->DelayedHealReady(HealSpell, GetDistance(GetCharInfo()->pSpawn, pPromisedTarget)))
		{
			if (PromisedTimer.Running())
			{
				if (PromisedTimer.msElapsed() > 23000)
				{
					PromisedTimer.Stop();
					PromisedTimer.Reset();
					_snprintf_s(tmp, MAX_STRING, "/casting \"${Spell[%s].RankName}\" -targetid|%d", HealSpell.c_str(), pPromisedTarget->SpawnID);
					PromisedTimer.Start();
					EzCommand(tmp);
					DNotify(PLUGIN_NAME, "CheckPromisedHeals", "<<\ay%s\ax>> on <<\ag%s\ax>>", HealSpell.c_str(), pPromisedTarget->Name);
					return true;
				}
			}
			else
			{
				PromisedTimer.Start();
				DoCommandf("/casting \"${Spell[%s].RankName}\" -targetid|%d", HealSpell.c_str(), pPromisedTarget->SpawnID);
				DNotify(PLUGIN_NAME, "CheckPromisedHeals", "<<\ay%s\ax>> on <<\ag%s\ax>>", HealSpell.c_str(), pPromisedTarget->Name);
				return true;
			}
		}
	}
	return false;
}

void Notify(PCHAR Plugin, PCHAR Function, PCHAR Format, ...)
{
	char zTemp[MAX_STRING] = { 0 };
	char zOut[MAX_STRING] = { 0 };
	char zOut2[MAX_STRING] = { 0 };
	va_list vaList;
	va_start(vaList, Format);
	vsnprintf_s(zTemp, MAX_STRING, Format, vaList);
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
		if ((zTemp[index] == '\a') && (index+1<MAX_STRING) && (oindex+4<MAX_STRING))
		{ //[+x+]
			zOut[oindex++] = '[';
			zOut[oindex++] = '+';
			zOut[oindex++] = zTemp[index+1];
			zOut[oindex++] = '+';
			zOut[oindex++] = ']';
			index++;
		}
		else
		{
			if (zTemp[index]!='\a')
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
	char zTemp[MAX_STRING+1] = { 0 };
	va_list vaList;
	va_start(vaList, Format);
	vsnprintf_s(zTemp, MAX_STRING, Format, vaList);
	if (!zTemp[0]) return;	//Error - no input data...
	zTemp[MAX_STRING - 1] = 0;	// No buffer overflows please
	EzCommand(zTemp);
}

#define MAX_REZ_TARGETS 10

typedef struct _RezTargetType
{
	MyTimer RezTimer;
	int AttemptCount;
	PSPAWNINFO Target;
	bool GroupTank;
	bool XTarget;
} RezTargetType;

int RezTargetCount = 0;
RezTargetType RezTargets[MAX_REZ_TARGETS];

void InitializeRezTarget(RezTargetType &target)
{
	target.Target = NULL;
	target.GroupTank = false;
	target.XTarget = false;
	target.AttemptCount = 0;
	target.RezTimer.Stop();
	target.RezTimer.Reset();
}

bool AddRezTarget(PSPAWNINFO pSpawn, bool GroupTank = false, bool XTarget = false)
{
	if (!pSpawn) return false;
	bool found = false;
	for (int x = 0; x < MAX_REZ_TARGETS; x++)
	{
		if (RezTargets[x].Target)
			if (RezTargets[x].Target->SpawnID == pSpawn->SpawnID)
			{
				found = true;
				break;
			}
	}
	if (!found)
	{
		int x = 0;
		if (RezTargetCount == MAX_REZ_TARGETS)
			return false;
		for (x = 0; x < MAX_REZ_TARGETS; x++)
		{
			if (RezTargets[x].Target == NULL)
			{
				InitializeRezTarget(RezTargets[x]);
				RezTargets[x].Target = pSpawn;
				RezTargets[x].GroupTank = GroupTank;
				RezTargets[x].XTarget = XTarget;
				if(RezTargets[x].Target!=NULL)
				{
					RezTargetCount++;
					return true;
				}
				else
				{
					InitializeRezTarget(RezTargets[x]);
					return false;
				}
			}
		}
		return false;
	}
	else
		return true;
	return false;
}

void RemoveRezTarget(PSPAWNINFO pSpawn)
{
	if (!pSpawn) return;
	for (int x = 0; x < MAX_REZ_TARGETS; x++)
	{
		if (RezTargets[x].Target)
			if (RezTargets[x].Target->SpawnID == pSpawn->SpawnID)
			{
				InitializeRezTarget(RezTargets[x]);
				RezTargetCount--;
				if (RezTargetCount < 0)
					RezTargetCount = 0;
			}
	}
}

PSPAWNINFO GetNextRezTarget()
{
	int x = 0;
	int y = 0;
	bool GroupTankRez = false;
	bool XTargetRez = false;
	if (RezTargetCount == 0) return NULL;
	for (y = 0; y < 10; y++)
	for (x = 0; x < MAX_REZ_TARGETS; x++)
	{
		if ((RezTargets[x].Target != NULL) && (RezTargets[x].AttemptCount == y ) &&
			((RezTargets[x].RezTimer.Running() && (RezTargets[x].RezTimer.msElapsed() > 20000))
			|| (!RezTargets[x].RezTimer.Running())))
		{
			EQPlayer *player = GetSpawnByID(RezTargets[x].Target->SpawnID);
			if (!player)
			{
				RemoveRezTarget(RezTargets[x].Target);
			}
			else if (player->Data.mPlayerPhysicsClient.pSpawn->Type != CORPSE) 
			{
				RemoveRezTarget(RezTargets[x].Target);
			}
			if (GetDistance(player->Data.X, player->Data.Y)<200)
			{
				RezTargets[x].RezTimer.Stop();
				RezTargets[x].RezTimer.Reset();
				RezTargets[x].RezTimer.Start();
				RezTargets[x].AttemptCount++;
				return RezTargets[x].Target;
			}
		}
	}
	return NULL;
}

bool CheckForRez()
{
	bool GroupTank = false;
	bool XTargetTank = false;
	char * HealSpell = NULL;
	char tmp[MAX_STRING] = { 0 };
	PCHARINFO info = GetCharInfo();
	PSPAWNINFO pRezTarget = NULL;
	PSPAWNINFO pSpawn = NULL;
	bool SpawnAdded = false;
	int x = 0;

	if (!DoRez) return false;
	if (!pSpawnList) return false;
	pSpawn = (PSPAWNINFO)pSpawnList;
	while (pSpawn)
	{
		SpawnAdded = false;
		if ((pSpawn->Type != CORPSE) || (GetDistance(info->pSpawn, pSpawn)>=200))
		{
			pSpawn = pSpawn->pNext;
			continue;
		}
		if (PGROUPINFO ginfo = info->pGroupInfo) 
			if (ginfo->pMember)// Are we in a group
				for (x = 0; x < 6; x++)
				{
					GroupTank = false;
					if (!ginfo->pMember[x])
						continue;
					if (!(ginfo->pMember[x]->pSpawn))
						continue;
					if ((_strnicmp(pSpawn->DisplayedName, ginfo->pMember[x]->pSpawn->DisplayedName, strlen(ginfo->pMember[x]->pSpawn->DisplayedName)) == 0))
					{
						AddRezTarget(pSpawn, GroupTank);
						SpawnAdded = true;
						break;
					}
				}
		if (SpawnAdded)
			continue;
		if (ExtendedTargetList *xtl = info->pXTargetMgr) {
			for (int x = 0; x < xtl->XTargetSlots.Count; x++) {
				XTARGETSLOT xts = xtl->XTargetSlots[x];
				if ((xts.xTargetType == XTARGET_SPECIFIC_NPC) || (xts.xTargetType == XTARGET_SPECIFIC_PC)) {
					if(EQPlayer *spawn = GetSpawnByID(xts.SpawnID)) {
						if ((_strnicmp(pSpawn->DisplayedName, spawn->Data.DisplayedName, strlen(spawn->Data.DisplayedName)) == 0)) {
							AddRezTarget(spawn->Data.mPlayerPhysicsClient.pSpawn, false, true);
						}
					}
				}
			}
		}
		pSpawn = pSpawn->pNext;
	}
	/*
	//Old Broken Method
	if (PGROUPINFO ginfo = info->pGroupInfo) // Are we in a group
		if (ginfo->pMember) // If so, does the members pointer exist
			for (int x = 0; x < 6; x++)
			{
				GroupTank = false;
				if (!ginfo->pMember[x])
					continue;
				if (!(ginfo->pMember[x]->pSpawn))
					continue;
				if (ginfo->pMember[x]->MainTank)
				{
					GroupTank = true;
					//pRezTarget = ginfo->pMember[x]->pSpawn;
				}
				if (Hovering(ginfo->pMember[x]->pSpawn) || IsCorpse(ginfo->pMember[x]->pSpawn) || (SpawnPctHPs(ginfo->pMember[x]->pSpawn) <= 0))
				{
					//pRezTarget = ginfo->pMember[x]->pSpawn;
					AddRezTarget(ginfo->pMember[x]->pSpawn, GroupTank);
				}
				else
				{
					RemoveRezTarget(ginfo->pMember[x]->pSpawn);
				}
			}
	if (info->pXTargetMgr)	// XTarget Tank takes priority over group tank...
		if (info->pXTargetMgr->pXTargetArray)
			if (info->pXTargetMgr->pXTargetArray->pXTargetData)
			{
				DWORD XTargetCount = info->pXTargetMgr->TargetSlots;
				for (int x = 0; x < XTargetCount; x++)
				{
					if ((info->pXTargetMgr->pXTargetArray->pXTargetData[x].xTargetType == XTARGET_SPECIFIC_NPC)
						|| (info->pXTargetMgr->pXTargetArray->pXTargetData[x].xTargetType == XTARGET_SPECIFIC_PC))
					{
						EQPlayer *spawn = GetSpawnByID(info->pXTargetMgr->pXTargetArray->pXTargetData[x].SpawnID);
						if (spawn)
						{
							if((PlayerPctHPs(spawn)<=0)||(Hovering(spawn->Data.mPlayerPhysicsClient.pSpawn))||IsCorpse(spawn->Data.mPlayerPhysicsClient.pSpawn))
								AddRezTarget(spawn->Data.mPlayerPhysicsClient.pSpawn, false, true);
							else
								RemoveRezTarget(spawn->Data.mPlayerPhysicsClient.pSpawn);
						}
					}
				}
			}
*/
	if (Me->IsAAReady("Blessing of Resurrection"))
	{
		PSPAWNINFO pSpawn = GetNextRezTarget();
		if (pSpawn)
		{
			DoCommandf("/casting \"Blessing of Resurrection\" alt -targetid|%d", pSpawn->SpawnID);
			DNotify(PLUGIN_NAME, "CheckForRez", "\ay(Blessing of Resurrection\ax) ==> <<\ag%s\ax>>", pSpawn->DisplayedName);
			return true;
		}
	}
	return false;
}

MyTimer RezTimer;

bool SpellStacks(PCHAR _SpellName)
{
	PSPELL pSpell=GetSpellByName(_SpellName);
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
/*
bool MedCheck()
{
	static MyTimer medtimer;
	string YaulpSpell;
	YaulpSpell.clear();
	if (!MedPct) return false;
	if (Me->PctMana() >= MedPct) return false;
	if ((!medtimer.Running()) || (medtimer.Running() && (medtimer.msElapsed() >= 200)))
	{
		medtimer.Stop();
		medtimer.Reset();
		medtimer.Start();
		if ((Me->PctMana() <= 60) && (Me->IsAAReady("Veturika's Perseverance") && !Me->CastingLeft() && Me->Standing()))
		{
			DoCommandf("/casting \"Veturika's Perseverance\" alt -targetid|${Me->ID}");
			Notify(PLUGIN_NAME, "MedCheck", "Casting (\agVeturika's Perseverance\ax");
			return false;
		}
		if ((Me->PctMana() <= 40) && (Me->IsAAReady("Quiet Miracle") && !Me->CastingLeft()) && Me->Standing())
		{
			DoCommandf("/casting \"Quiet Miracle\" alt -targetid|${Me->ID}");
			Notify(PLUGIN_NAME, "MedCheck", "Casting <<\agQuiet Miracle\ax>> on <<\ay%s\ax>>", GetCharInfo()->Name);
			return false;
		}
		if ((Me->PctMana() <= 98) && (!Me->CastingLeft()))
		{
			// out of combat med time
			if (((!DoMount) || OutDoors) && (!Me->OnMount()) && !IsStickOn() && Evaluate("${Cast.Status.Equal[I]}") && Me->Standing() && !Me->Moving())
			{
				DoCommandf("/sit");
				Notify(PLUGIN_NAME, "MedCheck", "Meditating", GetCharInfo()->Name);
				return true;
			}
			else if (DoMount && !Me->OnMount() && !OutDoors && !Me->XTargetHaters() && !Me->FeetWet() && Me->CastReady())
			{
				if (Me->Sitting())
				{
					DoCommandf("/stand");
					return false;
				}
				DoCommandf("/casting \"%s\" item", MountItem);
				HealState = HEALSTATE_CASTING;
				if (Evaluate("${Cast.Result.Equal[CAST_OUTDOORS]}"))
					OutDoors = true;
				Notify(PLUGIN_NAME, "MedCheck", "Casting Mount <<\ay%s\ax>>", MountItem);
				return false;
			}
			else if (Me->IsAAReady("Yaulp (Lesser)") && Me->XTargetHaters() && !Me->OnMount() && !Me->Buff("Lesser Yaulp"))
			{
				if (Me->Sitting())
				{
					DoCommandf("/stand");
					return true;
				}
				DoCommandf("/casting \"Yaulp (Lesser)\" alt");
				HealState = HEALSTATE_CASTING;
				Notify(PLUGIN_NAME, "MedCheck", "Casting (\ayYaulp AA\ax)");
				return false;
			}
			else if ((Me->Class()==Cleric) && (((CCleric *)Me)->YaulpSpellReady(YaulpSpell)))
			{
				if (YaulpSpell.empty()) return false;
				//if (Evaluate("(((${Me->XTarget[1].ID})&&(!${Me->Mount.ID})&&(${Cast.Ready[%s]})&&(!${Me->Buff[%s].ID})&&(${Spell[%s].Stacks}))", YaulpSpell, YaulpSpell, YaulpSpell))
				if (Me->XTargetHaters() && !Me->OnMount() && Me->IsSpellReady((PCHAR)YaulpSpell.c_str()) && !Me->Buff((PCHAR)YaulpSpell.c_str()) && SpellStacks((PCHAR)YaulpSpell.c_str()))
				{
					if (Me->Sitting()) DoCommandf("/stand");
					{
						DoCommandf("/casting \"%s\"", YaulpSpell.c_str());
					}
					HealState = HEALSTATE_CASTING;
					Notify(PLUGIN_NAME, "MedCheck", "Casting (\ayYaulp\ax%s\ay\ax)", YaulpSpell.c_str());
				}
				return false;
			}
		}
	}
	if (Me->PctHPs() < SelfHealPoint)
		return false;
	if (Me->PctMana() < 8)
		return false;
	return false;
}
*/

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

		if (Harvest && (Me->PctMana() <= 50) && Me->IsAAReady("Veturika's Perseverance") && !Me->CastingLeft())
		{
			if (Me->Sitting()) { DoCommandf("/stand"); return true; }
			DoCommandf("/casting \"Veturika's Perseverance\" alt");
			DNotify(PLUGIN_NAME, "MedCheck", "Casting (\agVeturika's Perseverance\ax)");
			HealState = HEALSTATE_CASTING;
			return false;
		}
		else if (Harvest && (Me->PctMana() <= 50) && Me->IsAAReady("Quiet Miracle") && !Me->CastingLeft())
		{
			if (Me->Sitting()) { DoCommandf("/stand"); return true; }
			DoCommandf("/casting \"Quiet Miracle\" alt -targetid|%d", GetCharInfo()->pSpawn->SpawnID);
			DNotify(PLUGIN_NAME, "MedCheck", "Casting (\agQuiet Miracle\ax)");
			HealState = HEALSTATE_CASTING;
			return false;
		}
		else if (Harvest && (Me->PctMana() < 20))
		{
			//else if ()
			if (Me->IsAAReady("Veturika's Perseverance"))
			{
				if (Me->Sitting()) { DoCommandf("/stand"); return true; }
				DoCommandf("/casting \"Veturika's Perseverance\" alt");
				DNotify(PLUGIN_NAME, "MedCheck", "Casting (\agVeturika's Perseverance\ax)");
				HealState = HEALSTATE_CASTING;
				return false;
			}
			else if (Me->IsAAReady("Quiet Miracle"))
			{
				if (Me->Sitting()) { DoCommandf("/stand"); return true; }
				DoCommandf("/casting \"Quiet Miracle\" alt -targetid|%d", GetCharInfo()->pSpawn->SpawnID);
				DNotify(PLUGIN_NAME, "MedCheck", "Casting (\agQuiet Miracle\ax)");
				HealState = HEALSTATE_CASTING;
				return false;
			}
		}
		else if ((Me->PctMana() <= MedPct) && (!Me->CastingLeft()) && (!Me->InCombat()))
		{
			// out of combat med time
			if ((!DoMount && DoMed) || (Me->Indoors() && DoMed && (!Me->OnMount()) && !IsStickOn() && Evaluate("${Cast.Status.Equal[I]}") && Me->Standing() && !Me->Moving()))
			{
				if (Me->Class() == Cleric)
				{
					CCleric *Clr = (CCleric *)Me;
					if (!(Me->AnyBuff("Yaulp") || Me->Buff("Lesser Yaulp")))
					{
						string YaulpSpell;
						YaulpSpell.clear();
						if (Me->IsAAReady("Yaulp (Lesser)"))
						{
							if (Me->Sitting()) { DoCommandf("/stand"); return true; }
							DoCommandf("/casting \"Yaulp (Lesser)\" alt ");
							DNotify(PLUGIN_NAME, "MedCheck", "Casting (\agYaulp (Lesser)\ax)");
							HealState = HEALSTATE_CASTING;
							return false;
						}
						else if (Clr->YaulpSpellReady(YaulpSpell))
						{
							if (Me->Sitting()) { DoCommandf("/stand"); return true; }
							DoCommandf("/casting \"${Spell[%s].RankName}\"", YaulpSpell);
							DNotify(PLUGIN_NAME, "MedCheck", "Casting (\ag%s\ax)", YaulpSpell.c_str());
							HealState = HEALSTATE_CASTING;
							return false;
						}
					}
				}
				else
					DoCommandf("/sit");
				DNotify(PLUGIN_NAME, "MedCheck", "Meditating", GetCharInfo()->Name);
				return false;
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
				HealState = HEALSTATE_CASTING;
				return false;
			}
		}
	}
	return false;
}

bool CheckAuras()
{

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
/*
	PCHARINFO info = GetCharInfo();
	if (PGROUPINFO ginfo = info->pGroupInfo) // Are we in a group
		if (ginfo->pMember) // If so, does the members pointer exist
			for (int x = 0; x < 6; x++)
			{
				if (!ginfo->pMember[x])
					continue;
				if (!(ginfo->pMember[x]->pSpawn))
					continue;
				if (ginfo->pMember[x]->MainTank)
				{
					bool found = false;
//					GroupTank = true;
//					pPromisedTarget = ginfo->pMember[x]->pSpawn;
					break;
				}
			}
	if (info->pXTargetMgr)	// XTarget Tank takes priority over group tank...
		if (info->pXTargetMgr->pXTargetArray)
			if (info->pXTargetMgr->pXTargetArray->pXTargetData)
			{
				DWORD XTargetCount = info->pXTargetMgr->TargetSlots;
				for (int x = 0; x < XTargetCount; x++)
				{
					if ((info->pXTargetMgr->pXTargetArray->pXTargetData[x].xTargetType == XTARGET_SPECIFIC_NPC)
						|| (info->pXTargetMgr->pXTargetArray->pXTargetData[x].xTargetType == XTARGET_SPECIFIC_PC))
					{
						EQPlayer *spawn = GetSpawnByID(info->pXTargetMgr->pXTargetArray->pXTargetData[x].SpawnID);
						if (spawn)
						{
//							pPromisedTarget = spawn->Data.mPlayerPhysicsClient.pSpawn;
//							XTargetTank = true;
							break;
						}
					}
				}
			}
	return false;
*/
void SetHealPoints()
{
	int nlevel = 0;	// Max named level
	int ncount = 0;	// Named mob count
	int hcount = 0;	// Hater count
	int hlevel = 0;	// Max hater level
	int mylevel = GetCharInfo()->pSpawn->Level;
	string heal;
	heal.clear();

	hcount = Me->XTargetHaters(&hlevel);	// Get # of Haters and max level
	ncount = Me->XTargetNamed(&nlevel);		// Get # of Nameds and max level

	if (((ncount > 0) && (nlevel >= (mylevel - 5)))
		|| ((ncount > 1) && (nlevel >= (mylevel - 10)))
		|| ((hcount > 3) && (hlevel >= (mylevel - 5)))
		|| ((hcount > 5) && (hlevel >= (mylevel - 8)))
		|| ((hcount > 8) && (hlevel >= (mylevel - 10)))
		|| ((hcount > 0) && (hlevel >= mylevel)))
		if ((DoLightHeals) && (Me->SlowHealReady(heal)) && (!heal.empty()))
			HealPoint = 98;
		else
			HealPoint = 90;
	else if ((ncount > 0) && (nlevel >= (mylevel - 10))
		 || ((hcount > 1) && (hlevel >= (mylevel - 5)))
		 || ((hcount > 3) && (hlevel >= (mylevel - 8)))
		 || ((hcount > 5) && (hlevel >= (mylevel - 10))))
		 if ((DoLightHeals) && (Me->SlowHealReady(heal)) && (!heal.empty()))
			 HealPoint = 90;
		 else
			 HealPoint = 85;
	else if ((hcount > 0) && (hlevel >= (mylevel - 5))
		 || ((hcount > 1) && (hlevel >= mylevel))
		 || (ncount>0))
		HealPoint = 80;
	else
		HealPoint = 70;

	if (HealPoint < GroupHealPoint)
		GroupHealPoint = HealPoint;
	if (HealPoint < FastHealPoint)
		FastHealPoint = HealPoint - 30;
	if (FastHealPoint < PatchHealPoint)
		PatchHealPoint = FastHealPoint - 10;
	if (PatchHealPoint < ClickHealPoint)
		ClickHealPoint = PatchHealPoint - 10;
}

// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
	static bool Medding = false;
	static MyTimer standtimer;

	// Update memorized spell set...
	if (Me) Me->UpdateMemorizedSpells();

	// Check if we're connected to eqbc, so we can use Notify()...
	if (EQBCConnected())
		UseEqbc = true;
	else
		UseEqbc = false;

	// If Heals are off, do nothing
	if (!HealsOn) return;

	// We can't heal if we're dead... :)
	if (AmIHovering()) return;

	// If heals are paused, do not heal or rez...
	if (HealPause) return;

	// Check if we have a merc and it's dead...
	if(!Me->InCombat())
		if (CheckReviveMerc()) DNotify(PLUGIN_NAME,"CheckReviveMerc","Tried to revive mercenary...");

	// We can't heal if we're DA'ed, and/or too high on the hate list, if so, click off DA and/or fade...
	if(DivineAuraCheck()) return;

	// Check for combat rez stuff
	if ((DoCombatRez && DoRez && Me->CastingCoolDown()) || ((!Me->InCombat()) && DoRez))
		CheckForRez();	// True if we try to rez someone...

	if (DoDynamicHeals)
		SetHealPoints();
	
	if (!CheckForHeals())
	{
		// Check for Promised Heals...
		if (Me->InCombat())
			if (CheckPromisedHeals()) return;

		// Check for buffs out of combat...
		if (!Me->InCombat())
			if (CheckForBuffs()) return;

		if ((!Me->InCombat()) && (DoMed))
			Medding = MedCheck();
	}

	// Main Heal Loop:
	switch (HealState)
	{
		case HEALSTATE_IDLE:
			if (Me->CastReady())
			{
				if (CheckForHeals())
				{
					if (pHealTarget && (!Me->CastingLeft()) && (!Me->CastingCoolDown()))
					{
						DNotify(PLUGIN_NAME, "CastReady", "<\ag%s\ax> needs a heal!", pHealTarget->Name);
						if (((standtimer.Running()) && (standtimer.msElapsed() > 500)) || (!standtimer.Running()))
						{
							standtimer.Stop();
							standtimer.Reset();
							standtimer.Start();
							EzCommand("/stand");
							Medding = false;
						}
					}
				}
				else
				{
					if (DoMed)
						Medding=MedCheck();
				}
			}
		break;
		case HEALSTATE_CASTSTART:
			if (Me->CastReady() && (!Me->CastingLeft()) && (!Me->CastingCoolDown()))
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
					CastNextHeal();
		break;
		case HEALSTATE_CASTING:
			if (!Me->CastingLeft())
			{
				HealState = HEALSTATE_CASTDONE;
				pHealTarget = NULL;
			}
		break;
		case HEALSTATE_CASTDONE:
			HealState = HEALSTATE_IDLE;
			pHealTarget = NULL;
		break;
		case HEALSTATE_CASTFAIL:
			HealState = HEALSTATE_IDLE;
			pHealTarget = NULL;
		break;
		case HEALSTATE_COOLDOWN:
			if (!Me->CastingCoolDown())
			{
				HealState = HEALSTATE_IDLE;
				pHealTarget = NULL;
			}
		break;
		default:break;
	}
    // DONT leave in this debugspew, even if you leave in all the others
    //DebugSpewAlways("MQ2Heals::OnPulse()");
}
/*
// Called every frame that the "HUD" is drawn -- e.g. net status / packet loss bar
PLUGIN_API VOID OnDrawHUD(VOID)
{
// DONT leave in this debugspew, even if you leave in all the others
//DebugSpewAlways("MQ2Heals::OnDrawHUD()");
}

// Called once directly before shutdown of the new ui system, and also
// every time the game calls CDisplay::CleanGameUI()
PLUGIN_API VOID OnCleanUI(VOID)
{
DebugSpewAlways("MQ2Heals::OnCleanUI()");
// destroy custom windows, etc
}

// Called once directly after the game ui is reloaded, after issuing /loadskin
PLUGIN_API VOID OnReloadUI(VOID)
{
DebugSpewAlways("MQ2Heals::OnReloadUI()");
// recreate custom windows, etc
}

// This is called every time WriteChatColor is called by MQ2Main or any plugin,
// IGNORING FILTERS, IF YOU NEED THEM MAKE SURE TO IMPLEMENT THEM. IF YOU DONT
// CALL CEverQuest::dsp_chat MAKE SURE TO IMPLEMENT EVENTS HERE (for chat plugins)
PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter)
{
    DebugSpewAlways("MQ2Heals::OnWriteChatColor(%s)",Line);
    return 0;
}

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
// but after MQ filters and chat events are taken care of.
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
    DebugSpewAlways("MQ2Heals::OnIncomingChat(%s)",Line);
    return 0;
}

// This is called each time a spawn is added to a zone (inserted into EQ's list of spawns),
// or for each existing spawn when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API VOID OnAddSpawn(PSPAWNINFO pNewSpawn)
{
    DebugSpewAlways("MQ2Heals::OnAddSpawn(%s)",pNewSpawn->Name);
}

// This is called each time a spawn is removed from a zone (removed from EQ's list of spawns).
// It is NOT called for each existing spawn when a plugin shuts down.
PLUGIN_API VOID OnRemoveSpawn(PSPAWNINFO pSpawn)
{
    DebugSpewAlways("MQ2Heals::OnRemoveSpawn(%s)",pSpawn->Name);
}

// This is called each time a ground item is added to a zone
// or for each existing ground item when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API VOID OnAddGroundItem(PGROUNDITEM pNewGroundItem)
{
    DebugSpewAlways("MQ2Heals::OnAddGroundItem(%d)",pNewGroundItem->DropID);
}

// This is called each time a ground item is removed from a zone
// It is NOT called for each existing ground item when a plugin shuts down.
PLUGIN_API VOID OnRemoveGroundItem(PGROUNDITEM pGroundItem)
{
    DebugSpewAlways("MQ2Heals::OnRemoveGroundItem(%d)",pGroundItem->DropID);
}
*/