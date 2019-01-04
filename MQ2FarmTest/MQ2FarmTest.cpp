// MQ2FarmTest.cpp
// Chatwiththisname/PeteSampras - Started MQ2Farm Initial Setup/Preperation
// Chatwiththisname/Renji - 10/07/2018 - Worked SpawnSearch and Nav Code
// Chatwiththisname 10/8/2018 - Working on Algorithms for Farming.
//					|	Adding FarmCommand Functionality.
//					|	Added Health/Endurance/Mana Checks, still requires logic for resting routines.
//					|	Added HaveAggro() function. Returns true if any Autohaters are populated on Xtargetlist. 
// Chatwiththisname 10/16/2018 - Test has been down a while, but added a getFirstAggroed() function to try and
//					|	deal with aggro before trying to find a mob with a shortest path to fix
//					|	ignoring aggrod mobs. 
// Chatwiththisname 11/12/2018 - Working on CastDetrimental() function to cast single target detrimental spells.
// Chatwiththisname 1/2/2019 - Cleaning up the code
//							 - trying to add Discs to combat routines, not having much luck.
//							 - Separated FarmTest into seperate files.
//							 - Tried adding TLO and member support. No luck, but kept the file.
// Chatwiththisname 1/3/2019 - Now generating a list of Discs for toons to use and separating them to sections. 
//							 - Firing Discs during combat based on the previously mentioned list.
//							 - DiscReady function now checks endurance/mana/reagent requirements before reporting true.
//							 - Actually enjoying testing this on my berserker. He's doing pretty good DPS without MQ2Melee. 


#include "../MQ2Plugin.h"
#include "Prototypes.h"
#include "Variables.h"
#include "NavCommands.h"
#include "FarmType.h"
#include <vector>
using namespace std;

bool pulsing = false;
//#define MerchSelect(X) if (pMerchantWnd) *((PEQMERCHWINDOW*)pMerchantWnd)->SelectedSlotID)=X
#define PLUGIN_NAME "MQ2FarmTest"
#define VERSION 0.1
#define PLUGINMSG "\ar[\a-tMQ2Farm\ar]\ao:: "

PreSetup(PLUGIN_NAME);
PLUGIN_VERSION(VERSION);

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	CheckAlias();
	DoINIThings();
	AddCommand("/farm", FarmCommand);
	AddCommand("/ignorethese", IgnoreTheseCommand);
	AddCommand("/ignorethis", IgnoreThisCommand);
	DiscSetup();
	WriteChatf("%s\aw- \agv0.1",PLUGINMSG);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	PluginOff();
	RemoveCommand("/farm");
	RemoveCommand("/ignorethese");
	RemoveCommand("/ignorethis");
}

// Called after entering a new zone
//PLUGIN_API VOID OnZoned(VOID)
//{
//    DebugSpewAlways("MQ2FarmTest::OnZoned()");
//}

// Called once directly after initialization, and then every time the gamestate changes
//PLUGIN_API VOID SetGameState(DWORD GameState)
//{
    //DebugSpewAlways("MQ2FarmTest::SetGameState()");
    //if (GameState==GAMESTATE_INGAME)
    // create custom windows if theyre not set up, etc
//}


// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
	
	if (!InGame() || !activated || !MeshLoaded() || pulsing || ++Pulse < PulseDelay) return;
	pulsing = true;
	Pulse = 0;
	if (!HaveAggro()) {
		if (!AmIReady()) {
			if (NavActive())
				NavEnd(GetCharInfo()->pSpawn);
			if (GetCharInfo()->pSpawn->StandState == STANDSTATE_STAND)
				EzCommand("/sit");
			pulsing = false;
			return;
		}
	}
	PSPAWNINFO Mob = (PSPAWNINFO)GetSpawnByID(MyTargetID);
	if (!Mob || !Mob->SpawnID || Mob->Type == SPAWN_CORPSE)
		MyTargetID = 0;
	if (getFirstAggroed() && !MyTargetID) {
		MyTargetID = getFirstAggroed();
		Mob = (PSPAWNINFO)GetSpawnByID(MyTargetID);
		NavEnd(GetCharInfo()->pSpawn);
	}
	if (!MyTargetID) {
		GemIndex = 0;
		MyTargetID = SearchSpawns(searchString);
		Mob = (PSPAWNINFO)GetSpawnByID(MyTargetID);
	}
	if (Mob && (AmIReady() || HaveAggro()))
		NavigateToID(MyTargetID);
	pulsing = false;
}


// This is called each time a spawn is removed from a zone (removed from EQ's list of spawns).
// It is NOT called for each existing spawn when a plugin shuts down.
PLUGIN_API VOID OnRemoveSpawn(PSPAWNINFO pSpawn)
{
	//I may use this, time will tell. 
    //DebugSpewAlways("MQ2FarmTest::OnRemoveSpawn(%s)",pSpawn->Name);
}

PLUGIN_API VOID Zoned(VOID)
{
	if (!MeshLoaded()) {
		WriteChatf("%s\a-rNo Mesh loaded for this zone. MQ2Farm does't work without it. Create a mesh and type /nav reload",PLUGINMSG);
	}
    //DebugSpewAlways("MQ2FarmTest::Zoned");
}

void CheckAlias()
{
	char aliases[MAX_STRING] = { 0 };
	if (RemoveAlias("/farm"))
		strcat_s(aliases, " /farm ");
	if (RemoveAlias("/ignorethis"))
		strcat_s(aliases, " /ignorethis ");
	if (RemoveAlias("/ignorethese"))
		strcat_s(aliases, " /ignorethese ");
	if (RemoveAlias("/loadignore"))
		strcat_s(aliases, " /loadignore ");
	if (strlen(aliases) > 0)
		WriteChatf("%s\ayWARNING\ao::\awAliases for \ao%s\aw were detected and removed.", PLUGINMSG, aliases);
}

void VerifyINI(char Section[MAX_STRING], char Key[MAX_STRING], char Default[MAX_STRING], char ININame[MAX_STRING])
{
	char temp[MAX_STRING] = { 0 };
	if (GetPrivateProfileString(Section, Key, 0, temp, MAX_STRING, ININame) == 0)
	{
		WritePrivateProfileString(Section, Key, Default, ININame);
	}
}

//convert char array to bool
bool atob(char x[MAX_STRING])
{
	for (int i = 0; i < 4; i++)
		x[i] = tolower(x[i]);
	if (!strcmp(x, "true") || atoi(x) != 0)
		return true;
	return false;
}

inline bool InGame()
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2());
}

void DoINIThings()
{
	char temp[MAX_STRING] = { 0 };
	DebugSpewAlways("Initializing MQ2FarmTest");
	//Check for INI entry for Key Debugging in General section.
	VerifyINI("General", "Debugging", "false", INIFileName);
	GetPrivateProfileString("General", "Debugging", "false", temp, MAX_STRING, INIFileName);
	Debugging = atob(temp);
	if (Debugging) WriteChatf("\ar[\a-tGeneral\ar]");
	if (Debugging) WriteChatf("\ayDebugging: \at%d", Debugging);

	//Check for INI entry for Key useLogOut in General section.
	VerifyINI("General", "useLogOut", "false", INIFileName);
	GetPrivateProfileString("General", "useLogOut", "false", temp, MAX_STRING, INIFileName);
	useLogOut = atob(temp);
	if (Debugging) WriteChatf("\ayuseLogOut: \at%d", useLogOut);

	//Check for INI entry for Key useEQBC in General section.
	VerifyINI("General", "useEQBC", "false", INIFileName);
	GetPrivateProfileString("General", "useEQBC", "false", temp, MAX_STRING, INIFileName);
	useEQBC = atob(temp);
	if (Debugging) WriteChatf("\ayuseEQBC: \at%d", useEQBC);

	//Check for INI entry for Key useMerc in General section.
	VerifyINI("General", "useMerc", "false", INIFileName);
	GetPrivateProfileString("General", "useMerc", "false", temp, MAX_STRING, INIFileName);
	useMerc = atob(temp);
	if (Debugging) WriteChatf("\ayuseMerc: \at%d", useMerc);

	//Check for INI entry for Key CastDetrimental in General section.
	VerifyINI("General", "CastDetrimental", "false", INIFileName);
	GetPrivateProfileString("General", "CastDetrimental", "false", temp, MAX_STRING, INIFileName);
	CastDetrimental = atob(temp);
	if (Debugging) WriteChatf("\ayCastDetrimental: \at%d", CastDetrimental);

	if (Debugging) WriteChatf("\ar[\a-tPull\ar]");
	//Check for INI entry for Key ZRadius in Pull section.
	VerifyINI("Pull", "ZRadius", "25", INIFileName);
	GetPrivateProfileString("Pull", "ZRadius", "25", temp, MAX_STRING, INIFileName);
	ZRadius = atoi(temp);
	if (Debugging) WriteChatf("\ayZRadius: \at%d", ZRadius);

	//Check for INI entry for Key Radius in Pull section.
	VerifyINI("Pull", "Radius", "100", INIFileName);
	GetPrivateProfileString("Pull", "Radius", "100", temp, MAX_STRING, INIFileName);
	Radius = atoi(temp);
	if (Debugging) WriteChatf("\ayRadius: \at%d", Radius);

	//Check for INI entry for Key PullAbility in Pull section.
	VerifyINI("Pull", "PullAbility", "melee", INIFileName);
	GetPrivateProfileString("Pull", "PullAbility", "melee", PullAbility, MAX_STRING, INIFileName);
	if (Debugging) WriteChatf("\ayPullAbility: \at%s", PullAbility);

	if (Debugging) WriteChatf("\ar[\a-tHealth\ar]");
	//Check for INI entry for Key HealAt in Health section.
	VerifyINI("Health", "HealAt", "70", INIFileName);
	GetPrivateProfileString("Health", "HealAt", "70", temp, MAX_STRING, INIFileName);
	HealAt = atoi(temp);
	if (Debugging) WriteChatf("\ayHealAt: \at%u", HealAt);

	//Check for INI entry for Key HealTill in Health section.
	VerifyINI("Health", "HealTill", "100", INIFileName);
	GetPrivateProfileString("Health", "HealTill", "100", temp, MAX_STRING, INIFileName);
	HealTill = atoi(temp);
	if (Debugging) WriteChatf("\ayHealTill: \at%u", HealTill);

	if (Debugging) WriteChatf("\ar[\a-tEndurance\ar]");
	//Check for INI entry for Key MedEndAt in Endurance section.
	VerifyINI("Endurance", "MedEndAt", "10", INIFileName);
	GetPrivateProfileString("Endurance", "MedEndAt", "10", temp, MAX_STRING, INIFileName);
	MedEndAt = atoi(temp);
	if (Debugging) WriteChatf("\ayMedEndAt: \at%u", MedEndAt);

	//Check for INI entry for Key MedEndTill in Endurance section.
	VerifyINI("Endurance", "MedEndTill", "100", INIFileName);
	GetPrivateProfileString("Endurance", "MedEndTill", "100", temp, MAX_STRING, INIFileName);
	MedEndTill = atoi(temp);
	if (Debugging) WriteChatf("\ayMedEndAt: \at%u", MedEndTill);

	if (Debugging) WriteChatf("\ar[\a-tMana\ar]");
	//Check for INI entry for Key MedAt in Mana section.
	VerifyINI("Mana", "MedAt", "30", INIFileName);
	GetPrivateProfileString("Mana", "MedAt", "30", temp, MAX_STRING, INIFileName);
	MedAt = atoi(temp);
	if (Debugging) WriteChatf("\ayMedAt: \at%u", MedAt);

	//Check for INI entry for Key MedTill in Mana section.
	VerifyINI("Mana", "MedTill", "100", INIFileName);
	GetPrivateProfileString("Mana", "MedTill", "100", temp, MAX_STRING, INIFileName);
	MedTill = atoi(temp);
	if (Debugging) WriteChatf("\ayMedTill: \at%u", MedTill);
	UpdateSearchString();
	
}

//Start Farm Commands
void IgnoreTheseCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!strlen(szLine)) {
		if (pTarget) {
			CHAR IgnoreString[MAX_STRING];
			sprintf_s(IgnoreString, MAX_STRING, "add 1 %s", ((PSPAWNINFO)pTarget)->DisplayedName);
			Alert(GetCharInfo()->pSpawn, IgnoreString);
			MyTargetID = 0;
			ClearTarget();
		}
	}
}

void IgnoreThisCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!strlen(szLine)) {
		if (pTarget) {
			CHAR IgnoreString[MAX_STRING];
			sprintf_s(IgnoreString, MAX_STRING, "add 1 %s", ((PSPAWNINFO)pTarget)->Name);
			Alert(GetCharInfo()->pSpawn, IgnoreString);
			MyTargetID = 0;
			ClearTarget();
		}
	}
}

void FarmCommand(PSPAWNINFO pChar, PCHAR Line) {
	if (!InGame())
		return;
	if (!strlen(Line)) {
		ListCommands();
	}

	char buffer[MAX_STRING] = "";
	if (strlen(Line)) {
		CHAR Arg1[MAX_STRING] = { 0 }, Arg2[MAX_STRING] = { 0 };
		GetArg(Arg1, Line, 1);
		if (Debugging) WriteChatf("\ar[\a-tMQ2Farm\ar]\ao::\atArg1 is: \ap%s", Arg1);

		if (!_stricmp(Arg1, "help")) {
			ListCommands();
			return;
		}

		if (!_stricmp(Arg1, "on")) {
			DoINIThings();
			PluginOn();
		}

		if (!_stricmp(Arg1, "off"))
		{
			PluginOff();
		}

		if (!_stricmp(Arg1, "radius"))
		{
			GetArg(Arg2, Line, 2);
			if (IsNumber(Arg2))
			{
				Radius = atoi(Arg2);
				WritePrivateProfileString("Pull", "Radius", Arg2, INIFileName);
				sprintf_s(searchString, MAX_STRING, "npc noalert 1 radius %i zradius %i targetable %s", Radius, ZRadius, FarmMob);
				WriteChatf("%s\atRadius is now: \ap%i", PLUGINMSG, Radius);
			} else {
				WriteChatf("%s\atRadius: \ap%i", PLUGINMSG, Radius);
			}
		}

		if (!_stricmp(Arg1, "zradius"))
		{
			GetArg(Arg2, Line, 2);
			if (IsNumber(Arg2))
			{
				ZRadius = atoi(Arg2);
				WritePrivateProfileString("Pull", "ZRadius", Arg2, INIFileName);
				sprintf_s(searchString, MAX_STRING, "npc noalert 1 radius %i zradius %i targetable %s", Radius, ZRadius, FarmMob);
				WriteChatf("%s\atZRadius is now: \ap%i", PLUGINMSG, ZRadius);
			} else {
				WriteChatf("%s\atZRadius: \ap%i", PLUGINMSG, ZRadius);
			}
		}

		if (!_stricmp(Arg1, "farmmob"))
		{
			GetArg(Arg2, Line, 2);
			if (!strlen(Arg2)) 
			{
				WriteChatf("%sCurrent farmmob is %s", PLUGINMSG, FarmMob);
			}
			if (!IsNumber(Arg2))
			{
				if (!_stricmp(Arg2, "clear"))
				{
					sprintf_s(FarmMob, MAX_STRING, "");
				}
				else {
					sprintf_s(FarmMob, Arg2);
				}
				sprintf_s(searchString, MAX_STRING, "npc noalert 1 radius %i zradius %i targetable %s", Radius, ZRadius, FarmMob);
				WriteChatf("%s\atFarmMob is now: \ap%s", PLUGINMSG, FarmMob);
			}
		}
		if (!_stricmp(Arg1, "castdetrimental"))
		{
			GetArg(Arg2, Line, 2);
			if (!strlen(Arg2))
				if (CastDetrimental) {
					WriteChatf("%sCast Detrimental is On!", PLUGINMSG);
				}
				else {
					WriteChatf("%sCast Detrimental is Off!", PLUGINMSG);
				}
			if (IsNumber(Arg2))
			{
				if (!_stricmp(Arg2, "1"))
				{
					if (!CastDetrimental) {
						WriteChatf("%sTurning Cast Detrimental: On!", PLUGINMSG);
						CastDetrimental = true;
						WritePrivateProfileString("General", "CastDetrimental", "true", INIFileName);
					}
				}
				if (!_stricmp(Arg2, "0")) 
				{
					if (CastDetrimental) {
						CastDetrimental = false;
						WritePrivateProfileString("General", "CastDetrimental", "false", INIFileName);
					}
				}
			}
			else {
				if (!_stricmp(Arg2, "on"))
				{
					if (!CastDetrimental) {
						CastDetrimental = true;
						WritePrivateProfileString("General", "CastDetrimental", "true", INIFileName);
					}
				}
				if (!_stricmp(Arg2, "off"))
				{
					if (CastDetrimental) {
						CastDetrimental = false;
						WritePrivateProfileString("General", "CastDetrimental", "false", INIFileName);
					}
				}
			}
		}

		if (!_stricmp(Arg1, "debug") || !_stricmp(Arg1, "debugging"))
		{
			GetArg(Arg2, Line, 2);
			if (!strlen(Arg2))
				WriteChatf("%sDebugging: %d", PLUGINMSG, Debugging);
			if (!IsNumber(Arg2))
			{
				if (!_stricmp(Arg2, "on"))
				{
					if (!Debugging) {
						Debugging = true;
						WritePrivateProfileString("General", "Debugging", "true", INIFileName);
						WriteChatf("%sDebugging: \agOn", PLUGINMSG);
					}	
				}
				if (!_stricmp(Arg2, "off"))
				{
					if (Debugging) {
						Debugging = false;
						WritePrivateProfileString("General", "Debugging", "false", INIFileName);
						WriteChatf("%sDebugging: \arOff", PLUGINMSG);
					}	
				}
			} else {
				if (atoi(Arg2) == 1)
					if (!Debugging) {
						Debugging = true;
						WritePrivateProfileString("General", "Debugging", "true", INIFileName);
						WriteChatf("%sDebugging: \agOn", PLUGINMSG);
					}	
				if (atoi(Arg2) == 0)
					if (Debugging) {
						Debugging = false;
						WritePrivateProfileString("General", "Debugging", "false", INIFileName);
						WriteChatf("%sDebugging: \arOff", PLUGINMSG);
					}	
			}
		}

		//Mana updates
		if (!_stricmp(Arg1, "MedAt"))
		{
			GetArg(Arg2, Line, 2);
			if (IsNumber(Arg2))
			{
				MedAt = atoi(Arg2);
				WritePrivateProfileString("Mana", "MedAt", Arg2, INIFileName);
				WriteChatf("%s\atMedAt is now: \ap%i", PLUGINMSG, MedAt);
			}
			else {
				WriteChatf("%s\atMedAt: \ap%i", PLUGINMSG, MedAt);
			}
		}

		if (!_stricmp(Arg1, "MedTill"))
		{
			GetArg(Arg2, Line, 2);
			if (IsNumber(Arg2))
			{
				MedTill = atoi(Arg2);
				WritePrivateProfileString("Mana", "MedTill", Arg2, INIFileName);
				WriteChatf("%s\atMedTill is now: \ap%i", PLUGINMSG, MedTill);
			}
			else {
				WriteChatf("%s\atMedTill: \ap%i", PLUGINMSG, MedTill);
			}
		}

		//Health Updates
		if (!_stricmp(Arg1, "HealAt"))
		{
			GetArg(Arg2, Line, 2);
			if (IsNumber(Arg2))
			{
				HealAt = atoi(Arg2);
				WritePrivateProfileString("Health", "HealAt", Arg2, INIFileName);
				WriteChatf("%s\atHealAt is now: \ap%i", PLUGINMSG, HealAt);
			}
			else {
				WriteChatf("%s\atHealAt: \ap%i", PLUGINMSG, HealAt);
			}
		}

		if (!_stricmp(Arg1, "HealTill"))
		{
			GetArg(Arg2, Line, 2);
			if (IsNumber(Arg2))
			{
				HealTill = atoi(Arg2);
				WritePrivateProfileString("Health", "HealTill", Arg2, INIFileName);
				WriteChatf("%s\atHealTill is now: \ap%i", PLUGINMSG, HealTill);
			}
			else {
				WriteChatf("%s\atHealTill: \ap%i", PLUGINMSG, HealTill);
			}
		}
		//Endurance updates
		if (!_stricmp(Arg1, "MedEndAt"))
		{
			GetArg(Arg2, Line, 2);
			if (IsNumber(Arg2))
			{
				MedEndAt = atoi(Arg2);
				WritePrivateProfileString("Endurance", "MedEndAt", Arg2, INIFileName);
				WriteChatf("%s\atMedEndAt is now: \ap%i", PLUGINMSG, MedEndAt);
			}
			else {
				WriteChatf("%s\atMedEndAt: \ap%i", PLUGINMSG, MedEndAt);
			}
		}

		if (!_stricmp(Arg1, "MedEndTill"))
		{
			GetArg(Arg2, Line, 2);
			if (IsNumber(Arg2))
			{
				MedEndTill = atoi(Arg2);
				WritePrivateProfileString("Endurance", "MedEndTill", Arg2, INIFileName);
				WriteChatf("%s\atMedEndTill is now: \ap%i", PLUGINMSG, MedEndTill);
			}
			else {
				WriteChatf("%s\atMedEndTill: \ap%i", PLUGINMSG, MedEndTill);
			}
		}
	}
}

//End Farm Commands

//Example: SearchSpawns("npc noalert 1 targetable radius 100 zradius 50");
DWORD SearchSpawns(char szIndex[MAX_STRING])
{
	double fShortest = 9999.0f;
	PSPAWNINFO sShortest;
	SEARCHSPAWN ssSpawn;
	ClearSearchSpawn(&ssSpawn);
	ssSpawn.FRadius = 999999.0f;
	ParseSearchSpawn(szIndex, &ssSpawn);
	for (unsigned long N = 0; N < gSpawnCount; N++)
	{
		if (EQP_DistArray[N].Value.Float > ssSpawn.FRadius && !ssSpawn.bKnownLocation || N > 50)
			break;
		if (SpawnMatchesSearch(&ssSpawn, (PSPAWNINFO)pCharSpawn, (PSPAWNINFO)EQP_DistArray[N].VarPtr.Ptr))
		{
			PSPAWNINFO pSpawn = (PSPAWNINFO)EQP_DistArray[N].VarPtr.Ptr;
			if (strlen(pSpawn->Lastname)) continue;
			if (PathExists(pSpawn->SpawnID) && (int)PathLength(pSpawn->SpawnID) >= (int)GetDistance3D(GetCharInfo()->pSpawn->X, GetCharInfo()->pSpawn->Y, GetCharInfo()->pSpawn->Z,pSpawn->X, pSpawn->Y, pSpawn->Z)) {
				
				if (fShortest > PathLength(pSpawn->SpawnID)) {
					fShortest = PathLength(pSpawn->SpawnID);
					sShortest = pSpawn;
				}
			}
		}
	}
	if (sShortest) {
		if (GetSpawnByID(sShortest->SpawnID)) {
			WriteChatf("%s\ar[%u]: %s is my Target", PLUGINMSG, sShortest->SpawnID, sShortest->Name);
			return sShortest->SpawnID;
		}
	}
	return false;
}

void ClearTarget() {
	bool bTemp = gFilterMQ;
	gFilterMQ = true;
	if (GetCharInfo()) Target(GetCharInfo()->pSpawn, "clear");
	gFilterMQ = bTemp;
}

void ListCommands()
{
	WriteChatf("%s\atCommands Available", PLUGINMSG);
	WriteChatf("%s\ay/farm \aw--- \atWill output this help menu", PLUGINMSG);
	WriteChatf("%s\ar[\a-tMQ2Farm\ar]\ao::\ay/farm on|off \aw--- \atWill turn on|off farming with INI settings.", PLUGINMSG);
	//WriteChatf("\ar[\a-tMQ2Farm\ar]\ao::\ay/farm status \aw--- \atDisplay if the plugin is On/Off.");
	WriteChatf("%s\ay/farm radius #### \aw--- \atWill set radius to number provided.", PLUGINMSG);
	WriteChatf("%s\ay/farm zradius #### \aw--- \atWill set zradius to number provided.", PLUGINMSG);
	WriteChatf("%s\ar[\a-tMQ2Farm\ar]\ao::\ay/farm farmmob \"Mob Name Here\" \aw--- \atWill specify a farmmob to farm.", PLUGINMSG);
	WriteChatf("%s\ar[\a-tMQ2Farm\ar]\ao::\ay/farm farmmob clear \aw--- \atWill clear the FarmMob and attack anything not on an alertlist.", PLUGINMSG);
	WriteChatf("%s\ar[\a-tMQ2Farm\ar]\ao::\ay/farm castdetrimental 1|On 0|Off \aw--- \atWill turn on and off casting of single target detrimental spells.", PLUGINMSG);
	WriteChatf("%s\ar[\a-tMQ2Farm\ar]\ao::\ay/ignorethis \aw--- \atWill temporarily ignore your current target.", PLUGINMSG);
	WriteChatf("%s\ar[\a-tMQ2Farm\ar]\ao::\ay/ignorethese \aw--- \atWill temporarily ignore all spawns with this targets clean name.", PLUGINMSG);
	//WriteChatf("\ar[\a-tMQ2Farm\ar]\ao::\ay/loadignore \aw--- \atWill loadignores from the Ignore_Mobs.ini in the macro folder.");
}

bool AmIReady()
{	
	//Start Medding Endurance Here.
	if (!GettingEndurance && (float)GetCharInfo()->pSpawn->EnduranceCurrent / (float)GetCharInfo()->pSpawn->EnduranceMax * 100.0f < MedEndAt) {
		if (!GettingEndurance) {
			GettingEndurance = true;
			WriteChatf("%s\arNeed to med endurance.", PLUGINMSG);
		}
		return false;
	}
	
	if (GettingEndurance && (float)GetCharInfo()->pSpawn->EnduranceCurrent / (float)GetCharInfo()->pSpawn->EnduranceMax * 100.0f < MedEndTill) {
		return false;
	} else {
		if (GettingEndurance) {
			WriteChatf("%s\agDone medding Endurance.", PLUGINMSG);
			GettingEndurance = false;
		}
	}
	//GetCharInfo()->pGroupInfo->pMember[1]
		
	//Starting Medding Mana here. 
	if (!GettingMana && PercentMana() < MedAt) {
		if (!GettingMana) {
			WriteChatf("%s\arNeed to med Mana!", PLUGINMSG);
			GettingMana = true;
		}
		return false;
	}

	if (GettingMana && PercentMana() < MedTill) {
		//WriteChatf("%f", PercentMana());
		return false;
	} else {
		if (GettingMana) {
			WriteChatf("%s\agDone medding Mana!", PLUGINMSG);
			GettingMana = false;
		}
	}
	
	//Start medding health here.
	if (!GettingHealth && (float)GetCharInfo()->pSpawn->HPCurrent / (float)GetCharInfo()->pSpawn->HPMax * 100.0f < HealAt) {
		if (!GettingHealth) {
			WriteChatf("%s\arNeed to med Health!", PLUGINMSG);
			GettingHealth = true;
		}
		return false;
	}
	if (GettingHealth && (float)GetCharInfo()->pSpawn->HPCurrent / (float)GetCharInfo()->pSpawn->HPMax * 100.0f < HealTill) {
		return false;
	} else {
		if (GettingHealth) {
			WriteChatf("%s\agDone medding health!", PLUGINMSG);
			GettingHealth = false;
		}
	}
	return true;
}

bool HaveAggro()
{
	if (ExtendedTargetList *xtm = GetCharInfo()->pXTargetMgr) {
		if (pAggroInfo) {
			for (int i = 0; i < xtm->XTargetSlots.Count; i++) {
				XTARGETSLOT xts = xtm->XTargetSlots[i];
				DWORD spID = xts.SpawnID;
				if (spID && xts.xTargetType == XTARGET_AUTO_HATER) {
					if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(spID)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

void CastDetrimentalSpells()
{
	if (!pLocalPlayer) {
		if (Debugging) WriteChatf("No pLocalPlayer, returning");
		return;
	}
	if (GetCharInfo()->pSpawn->CastingData.IsCasting()) {
		if (Debugging) WriteChatf("already casting, returning");
		return;
	}
	if (!SpellsMemorized()) {
		if (Debugging) WriteChatf("No SpellsMemorized, returning");
		return;
	}
	//TODO: Need to verify line of sight!!!! Haven't done that yet.
	if (GemIndex > 13) {
		GemIndex = 0;
	}
	
	SPELL *spell = GetSpellByID(GetCharInfo2()->MemorizedSpells[GemIndex]);
	while (!spell) {
		GemIndex++;
		if (GemIndex > 13) GemIndex = 0;
		spell = GetSpellByID(GetCharInfo2()->MemorizedSpells[GemIndex]);
	}
	
	if (spell && spell->CanCastInCombat) {
		//BYTE    TargetType;         //03=Group v1, 04=PB AE, 05=Single, 06=Self, 08=Targeted AE, 0e=Pet, 28=AE PC v2, 29=Group v2, 2a=Directional
		if (spell->TargetType == 05) {
			//*0x180*/   BYTE    SpellType;          //0=detrimental, 1=Beneficial, 2=Beneficial, Group Only
			if (spell->SpellType == 0) {
				if (GetCharInfo()->pSpawn->SpeedRun > 0.0f) {
					if (Debugging) WriteChatf("Ending Navigation to cast");
					if (NavActive()) NavEnd(GetCharInfo()->pSpawn);
				}
				if (Debugging) WriteChatf("Spell: %s, TargetType: %d, SpellType: %d", spell->Name, spell->TargetType, spell->SpellType);
				char castcommand[MAX_STRING] = "/cast ";
				string s = to_string(GemIndex+1);
				strcat_s(castcommand, MAX_STRING, s.c_str());
				if (GetCharInfo()->pSpawn->ManaCurrent > (int)spell->ManaCost) {
					if (pTarget && GetDistance(GetCharInfo()->pSpawn, (PSPAWNINFO)pTarget) < spell->Range) {
						WriteChatf("%s\arCasting \a-t----> \ap%s \ayfrom Gem %d", PLUGINMSG, spell->Name, GemIndex + 1);
						EzCommand(castcommand);
					} else {
						if (Debugging) WriteChatf("Target not in range, or no target.");
					}
				} else {
					if (Debugging) WriteChatf("Not enough Mana to cast %s", spell->Name);
				}
			}
		}
	}
	GemIndex++;
}

DWORD getFirstAggroed()
{
	if (ExtendedTargetList *xtm = GetCharInfo()->pXTargetMgr) {
		if (pAggroInfo) {
			for (int i = 0; i < xtm->XTargetSlots.Count; i++) {
				XTARGETSLOT xts = xtm->XTargetSlots[i];
				DWORD spID = xts.SpawnID;
				if (spID && xts.xTargetType == XTARGET_AUTO_HATER) {
					if (PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(spID)) {
						//WriteChatf("I have aggro from: %s", pSpawn->Name);
						return spID;
					}
				}
			}
		}
	}
	return 0;
}

void UpdateSearchString()
{
	sprintf_s(searchString, MAX_STRING, "npc noalert 1 radius %i zradius %i targetable %s", Radius, ZRadius, FarmMob);
}

void PluginOn()
{
	if (activated)
		return;
	if (!MeshLoaded()) {
		WriteChatf("%s\a-rNo Mesh loaded for this zone. MQ2Farm does't work without it. Create a mesh and type /nav reload", PLUGINMSG);
		return;
	}
	activated = true;
	CheckAlias();
	DoINIThings();
	WriteChatf("%s\aw- \agv0.1", PLUGINMSG);
	WriteChatf("%s\agActivated", PLUGINMSG);
}

void PluginOff()
{
	if (!activated)
		return;
	MyTargetID = 0;
	activated = false;
	NavEnd(GetCharInfo()->pSpawn);
	WriteChatf("%s\arDeactivated", PLUGINMSG);
}

float AmFacing(DWORD ID)
{
	if (!InGame())
		return false;
	PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(ID);
	float fMyHeading = GetCharInfo()->pSpawn->Heading*0.703125f;
	float fSpawnHeadingTo = (FLOAT)(atan2f(((PSPAWNINFO)pCharSpawn)->Y - pSpawn->Y, pSpawn->X - ((PSPAWNINFO)pCharSpawn)->X) * 180.0f / PI + 90.0f);
	if (fSpawnHeadingTo < 0.0f)
		fSpawnHeadingTo += 360.0f;
	else if (fSpawnHeadingTo >= 360.0f)
		fSpawnHeadingTo -= 360.0f;
	float facing = abs(fSpawnHeadingTo - fMyHeading);
	return facing;
}

bool SpellsMemorized()
{
	for (int i = 0; i < 13; i++) {
		SPELL *Spell = GetSpellByID(GetCharInfo2()->MemorizedSpells[i]);
		if (Spell) return true;
	}
	return false;
}

inline float PercentMana()
{
	return (float)GetCharInfo()->pSpawn->ManaCurrent / (float)GetCharInfo()->pSpawn->ManaMax*100.0f;
}

inline float PercentHealth()
{
	return (float)GetCharInfo()->pSpawn->HPCurrent / (float)GetCharInfo()->pSpawn->HPMax * 100.0f;
}

inline float PercentEndurance()
{
	return (float)GetCharInfo()->pSpawn->EnduranceCurrent / (float)GetCharInfo()->pSpawn->EnduranceMax * 100.0f;
}

void NavigateToID(DWORD ID) {
	PSPAWNINFO Mob = (PSPAWNINFO)GetSpawnByID(ID);
	CHAR szNavInfo[32];
	sprintf_s(szNavInfo, 32, "%u", ID);
	if (Mob && (!LineOfSight(GetCharInfo()->pSpawn, Mob) || GetDistance(GetCharInfo()->pSpawn, Mob) > 20)) {
		if (!NavActive()) {
			if (!GetCharInfo()->pSpawn->CastingData.IsCasting()) NavCommand(GetCharInfo()->pSpawn, szNavInfo);

		}
	}
	if (LineOfSight(GetCharInfo()->pSpawn, Mob) && GetDistance(GetCharInfo()->pSpawn, Mob) < 20) {
		if (NavActive()) NavEnd(GetCharInfo()->pSpawn);
		if (Mob && pTarget && ((PSPAWNINFO)pTarget)->SpawnID != Mob->SpawnID || !pTarget) {
			TargetIt(Mob);
		}
		if (GetCharInfo()->pSpawn->StandState == STANDSTATE_SIT)
			EzCommand("/stand");
		//if (Debugging) WriteChatf("AmFacing() returned: %f", AmFacing(Mob->SpawnID));
		if (AmFacing(Mob->SpawnID) > 30) Face(GetCharInfo()->pSpawn, "");
	}
	if (Mob && pTarget) {
		if (useDiscs) UseDiscs();
		if (CastDetrimental) CastDetrimentalSpells();
		if (*EQADDR_ATTACK) return;
		*EQADDR_ATTACK = 00000001;
		EzCommand("/pet attack");
		EzCommand("/pet swarm");

	}
}

void DiscSetup()
{
	if (!InGame()) return;

	bool replaced = false;
	bool duplicatetimer = false;
	int MaxIndex = 0;

	int CATemp[NUM_COMBAT_ABILITIES];
	for (int i = 0; i < NUM_COMBAT_ABILITIES; i++) {
		CATemp[i] = -1;
	}

	for (int i = 0; i < NUM_COMBAT_ABILITIES; i++) {
		if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(i)) {
			if (PSPELL pSpell = GetSpellByID(pPCData->GetCombatAbility(i))) {
				//need to make sure the disc is a usable disc. IE: Dichotomic Rage has two abilities, one is level 250.
				if (pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] <= 110) {
					if (MaxIndex == 0) {
						//WriteChatf("Adding first item: \ay%s\aw at [i]Index:\ag %i", pSpell->Name, i);
						CATemp[i] = pSpell->ID;
						MaxIndex++;
						//WriteChatf("Adding 1 to MaxIndex: %i", MaxIndex);
					}
					else {
						for (int j = 0; j <= MaxIndex; j++) {
							//WriteChatf("[j] %i of [MaxIndex] %i", j, MaxIndex);
							if (CATemp[j] != -1) {
								if (PSPELL temp = GetSpellByID(CATemp[j])) {
									if (temp->ReuseTimerIndex == pSpell->ReuseTimerIndex) {
										//WriteChatf("Comparing \ap%s\aw Timer: %i to \ay%s \awTimer: %i", temp->Name, temp->ReuseTimerIndex, pSpell->Name, pSpell->ReuseTimerIndex);
										duplicatetimer = true;
										if (temp->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class] < pSpell->ClassLevel[GetCharInfo()->pSpawn->mActorClient.Class]) {
											//WriteChatf("\arReplacing \ay%s\aw with \ay%s", temp->Name, pSpell->Name, i);
											CATemp[j] = pSpell->ID;
											replaced = true;
											break;
										}
										else {
											//WriteChatf("Keeping: \ay%s", temp->Name);
										}
									}
								}
							}
						}
						if (!replaced && !duplicatetimer) {
							MaxIndex++;
							//WriteChatf("\agAdding: \ap%s \aw Timer: \ag%i", pSpell->Name, pSpell->ReuseTimerIndex);
							CATemp[MaxIndex] = pSpell->ID;
						}
						replaced = false;
						duplicatetimer = false;
					}
				}
				
			}
		}
	}
	//BYTE    TargetType;         //03=Group v1, 04=PB AE, 05=Single, 06=Self, 08=Targeted AE, 0e=Pet, 28=AE PC v2, 29=Group v2, 2a=Directional, 46=Target Of Target
	//*0x180*/   BYTE    SpellType;          //0=detrimental, 1=Beneficial, 2=Beneficial, Group Only
	for (int i = 0; i < NUM_COMBAT_ABILITIES; i++) {
		if (CATemp[i] != -1) {
			if (PSPELL pSpell = GetSpellByID(CATemp[i])) {
				if (pSpell->SpellType == 0 && pSpell->TargetType == 5) {
					//Single Target Detrimental
					SingleDetrimental.push_back(pSpell);
					//WriteChatf("\ar%s", pSpell->Name);
				}
				else if (pSpell->SpellType == 1 && pSpell->TargetType == 46) {
					//Beneficial Target of Target
					ToTBeneficial.push_back(pSpell);
					//WriteChatf("\au%s TargetType: %i", pSpell->Name, pSpell->TargetType);
				}
				else if (pSpell->SpellType == 0) {
					//All other Detrimentals AEs?
					AEDetrimental.push_back(pSpell);
				}
				else if (pSpell->SpellType == 1 && pSpell->TargetType == 06) {
					//Self Target Beneficial
					SelfBeneficial.push_back(pSpell);
					//WriteChatf("\ay%s", pSpell->Name);
				}
				else if (pSpell->SpellType == 1 && pSpell->TargetType == 05) {
					//Single Target Beneficial
					SingleBeneficial.push_back(pSpell);
					//WriteChatf("\ag%s", pSpell->Name);
				}
				else if (pSpell->SpellType == 2) {
					//Group Beneficial
					GroupBeneficial.push_back(pSpell);
					//WriteChatf("\ap%s TargetType: %i", pSpell->Name, pSpell->TargetType);
				}
				else {
					//Everything else
					WriteChatf("\aoNoCategory:%s TargetType: %i SpellType: %i", pSpell->Name, pSpell->TargetType, pSpell->SpellType);
				}
			}
		}
	}
	WriteChatf("[Single Detrimental]");
	for (unsigned int i = 0; i < SingleDetrimental.size(); i++) {
		WriteChatf("\ar%s", SingleDetrimental.at(i)->Name);
	}
	WriteChatf("[AE Detrimental]");
	for (unsigned int i = 0; i < AEDetrimental.size(); i++) {
		WriteChatf("\au%s", AEDetrimental.at(i)->Name);
	}
	WriteChatf("[Self Beneficial]");
	for (unsigned int i = 0; i < SelfBeneficial.size(); i++) {
		WriteChatf("\ay%s", SelfBeneficial.at(i)->Name);
	}
	WriteChatf("[Single Beneficial]");
	for (unsigned int i = 0; i < SingleBeneficial.size(); i++) {
		WriteChatf("\ag%s", SingleBeneficial.at(i)->Name);
	}
	WriteChatf("[Group Beneficial]");
	for (unsigned int i = 0; i < GroupBeneficial.size(); i++) {
		WriteChatf("\ap%s", GroupBeneficial.at(i)->Name);
	}
	WriteChatf("[Target of Target Beneficial]");
	for (unsigned int i = 0; i < ToTBeneficial.size(); i++) {
		WriteChatf("\ao%s", ToTBeneficial.at(i)->Name);
	}
}

void UseDiscs()
{
	if (!useDiscs || !InGame()) return;
	if (GetCharInfo()->pSpawn->CastingData.IsCasting()) {
		if (Debugging) WriteChatf("already casting, returning");
		return;
	}
	
	CHAR temp[MAX_STRING] = "";
	for (unsigned int i = 0; i < SingleDetrimental.size(); i++) {
		if (DiscLastTimeUsed < GetTickCount64()) {
			if (DiscReady(SingleDetrimental.at(i))) {
				if (SingleDetrimental.at(i)->CanCastInCombat) {
					WriteChatf("Trying to fire ---> \ap%s", SingleDetrimental.at(i)->Name);
					//DiscLastTimeUsed = (DWORD)time(NULL);
					DiscLastTimeUsed = GetTickCount64();
					pCharData->DoCombatAbility(SingleDetrimental.at(i)->ID);
				}
			}
		}
	}
}

BOOL DiscReady(PSPELL pSpell)
{
	if (!InGame()) return false;
	DWORD timeNow = (DWORD)time(NULL);
	if (pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex, pSpell->SpellGroup) < timeNow) {
		if (GetCharInfo()->pSpawn->ManaCurrent >= (int)pSpell->ManaCost && GetCharInfo()->pSpawn->EnduranceCurrent >= (int)pSpell->EnduranceCost) {
			
			if (pSpell->CasterRequirementID == 518) {
				//Requires under 90% hitpoints
				if (PercentHealth() > 89) return false;
			}
			else if (pSpell->CasterRequirementID == 825) {
				//Requires under 21% Endurance
				if (PercentEndurance() > 21) return false;
			}
			else if (pSpell->CasterRequirementID == 826) {
				//Requires under 25% Endurance
				if (PercentEndurance() > 25) return false;
			}
			else if (pSpell->CasterRequirementID == 827) {
				//Requires under 29% Endurance
				if (PercentEndurance() > 29) return false;
			}
			for (int i = 0; i < 4; i++) {
				if (pSpell->ReagentCount[i] > 0) {
					if (pSpell->ReagentID[i] != -1) {
						DWORD count = FindItemCountByID((int)pSpell->ReagentID[i]);
						if (count < pSpell->ReagentCount[i]) {
							if (PCHAR pitemname = GetItemFromContents(FindItemByID((int)pSpell->ReagentID[i]))->Name) {
								WriteChatf("\ap%s\aw needs Reagent: \ay%s x \ag%i", pSpell->Name, pitemname, pSpell->ReagentCount[i]);
							}
							else {
								WriteChatf("%s needs Item ID: %d x \ag%d", pSpell->Name, pSpell->ReagentID[i], pSpell->ReagentCount[i]);
							}
							return false;
						}
					}
				}
			}
		}
		else {
			return false;
		}
		return true;
	}		
	return false;
}

