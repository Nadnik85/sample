// MQ2KissTemplate.cpp - 10/12/2018
// By Chatwiththisname @ Redguides.com



#include "../MQ2Plugin.h"
using namespace std;

#define PLUGIN_NAME "MQ2KissTemplate"
#define VERSION 0.1

PreSetup(PLUGIN_NAME);
PLUGIN_VERSION(VERSION);

//Variables
CHAR filename[MAX_STRING] = "";
CHAR newfilename[MAX_STRING] = "";
CHAR OurClass[MAX_STRING] = "";
CHAR ConditionsFile[MAX_STRING] = "";

int OurLevel = 0;
int useConditions = 0;
int condNumber = 1;
//End Variables

//Prototypes
void GetINI(char Section[MAX_STRING], char Key[MAX_STRING], char Default[MAX_STRING], char ININame[MAX_STRING]);
void GetINILoop(char Section[MAX_STRING], char Key[MAX_STRING], char Default[MAX_STRING], char ININame[MAX_STRING]);
void TemplateCommand(PSPAWNINFO pChar, PCHAR szLine);
inline bool InGame();
//End Prototypes

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	AddCommand("/maketemplate", TemplateCommand);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	RemoveCommand("/maketemplate");
}

PLUGIN_API VOID SetGameState(DWORD GameState)
{
    //if (GameState==GAMESTATE_INGAME)
    // create custom windows if theyre not set up, etc
}


// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{

}

void TemplateCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!InGame()) return;

	bool useClass = false;
	bool useLevel = false;
	CHAR tempClass[MAX_STRING] = "";
	CHAR tempLevel[MAX_STRING] = "";
	CHAR temp[MAX_STRING] = "";
	GetArg(tempClass, szLine, 1);
	GetArg(tempLevel, szLine, 2);
	if (strlen(tempClass))
		if (_stricmp(tempClass, "BRD") && _stricmp(tempClass, "BST") && _stricmp(tempClass, "BER") && _stricmp(tempClass, "CLR") && _stricmp(tempClass, "DRU") && _stricmp(tempClass, "ENC") && _stricmp(tempClass, "MAG") && _stricmp(tempClass, "MNK") && _stricmp(tempClass, "NEC") && _stricmp(tempClass, "PAL") && _stricmp(tempClass, "RNG") && _stricmp(tempClass, "ROG") && _stricmp(tempClass, "SHD") && _stricmp(tempClass, "SHM") && _stricmp(tempClass, "WAR") && _stricmp(tempClass, "WIZ")) {
			WriteChatf("You've provided %s as a class to emulate, but that doesn't appear to be one of the 16 playable classes.", tempClass);
			return;
		}
		else {
			useClass = true;
		}

	if (strlen(tempLevel))
		if (atoi(tempLevel) < 0 || atoi(tempLevel) > 110) {
			WriteChatf("You've provided %s as a level to emulate, but it is not between 1 and 110", tempLevel);
			return;
		}
		else {
			useLevel = true;
		}

	sprintf_s(filename, "%s\\Macros\\Kissassist_%s.ini", gszINIPath, pChar->Name);
	if (useClass)
		sprintf_s(OurClass, "%s", tempClass);
	if (!useClass)
		sprintf_s(OurClass, "%s", pEverQuest->GetClassThreeLetterCode(GetCharInfo2()->Class));
	if (useLevel)
		OurLevel = atoi(tempLevel);
	if (!useLevel)
		OurLevel = GetCharInfo()->pSpawn->Level;
	sprintf_s(newfilename, "%s\\Macros\\Kissassist_%d_%s.ini", gszINIPath, OurLevel, OurClass);
	//WriteChatf("Copying Key parts of %s to a new file %s", filename, newfilename);
	useConditions = GetPrivateProfileInt("General", "ConditionsOn", 0, filename);
	//WriteChatf("useConditions: %d", useConditions);
	if (useConditions == 1)
		sprintf_s(ConditionsFile, "%s\\Macros\\Kissassist_%s_Conditions.ini", gszINIPath, pChar->Name);
	if (useConditions == 2)
		sprintf_s(ConditionsFile, "%s\\Macros\\Kissassist_%s.ini", gszINIPath, pChar->Name);

	//General Section
	GetINI("General", "KissAssVer", 0, filename);
	if (!_stricmp(OurClass, "BRD")) {
		if (GetPrivateProfileInt("General", "TwistOn", 0, filename) != 0) {
			GetINI("General", "TwistOn", 0, filename);
			GetINI("General", "TwistMed", 0, filename);
			GetINI("General", "TwistWhat", 0, filename);
		}
	}

	//Buff Section
	if (GetPrivateProfileInt("Buffs", "BuffsOn", 0, filename) != 0) {
		GetINI("Buffs", "BuffsOn", 0, filename);
		if (useConditions) GetINI("Buffs", "BuffsCOn", 0, filename);
		GetINI("Buffs", "BuffsSize", 0, filename);
		GetINILoop("Buffs", "Buffs", 0, filename);
		GetINI("Buffs", "RebuffOn", 0, filename);
		GetINI("Buffs", "CheckBuffsTimer", 0, filename);
	}

	//Melee Section
	if (!_stricmp(OurClass, "BER")) GetINI("Melee", "BeforeCombat", 0, filename);
	if (!_stricmp(OurClass, "BRD")) {
		GetINI("Melee", "MeleeTwistOn", 0, filename);
		GetINI("Melee", "MeleeTwistWhat", 0, filename);
	}
	//AE Section
	if (GetPrivateProfileInt("AE", "AEOn", 0, filename) != 0) {
		GetINI("AE", "AEOn", 0, filename);
		GetINI("AE", "AESize", 0, filename);
		GetINI("AE", "AERadius", 0, filename);
		GetINILoop("AE", "AE", 0, filename);
	}

	//DPS Section
	if (GetPrivateProfileInt("DPS", "DPSOn", 0, filename) != 0) {
		GetINI("DPS", "DPSOn", 0, filename);
		if (useConditions) GetINI("DPS", "DPSCOn", 0, filename);
		GetINI("DPS", "DPSSize", 0, filename);
		GetINI("DPS", "DPSSkip", 0, filename);
		GetINI("DPS", "DPSInterval", 0, filename);
		GetINILoop("DPS", "DPS", 0, filename);
		GetINI("DPS", "DebuffAllOn", 0, filename);
	}

	//Aggro Section
	if (GetPrivateProfileInt("Aggro", "AggroOn", 0, filename)) {
		GetINI("Aggro", "AggroOn", 0, filename);
		if (useConditions) GetINI("Aggro", "AggroCOn", 0, filename);
		GetINI("Aggro", "AggroSize", 0, filename);
		GetINILoop("Aggro", "Aggro", 0, filename);
	}

	//Pull Section - Not Needed

	//Heals Section
	if (GetPrivateProfileInt("Heals", "HealsOn", 0, filename) != 0) {
		GetINI("Heals", "HealsOn", 0, filename);
		if (useConditions) GetINI("Heals", "HealsCOn", 0, filename);
		GetINI("Heals", "HealsSize", 0, filename);
		GetINILoop("Heals", "Heals", 0, filename);
		if (!_stricmp(OurClass, "CLR") || !_stricmp(OurClass, "NEC") || !_stricmp(OurClass, "SHM") || !_stricmp(OurClass, "DRU") || !_stricmp(OurClass, "PAL")) {
			GetINI("Heals", "AutoRezOn", 0, filename);
			GetINI("Heals", "AutoRezWith", 0, filename);
		}
	}

	//Cures Section
	if (GetPrivateProfileInt("Cures", "CuresOn", 0, filename) != 0) {
		GetINI("Cures", "CuresOn", 0, filename);
		GetINI("Cures", "CuresSize", 0, filename);
		GetINILoop("Cures", "Cures", 0, filename);
	}

	//Pet Section
	if (!_stricmp(OurClass, "DRU") || !_stricmp(OurClass, "SHM") || !_stricmp(OurClass, "BST") || !_stricmp(OurClass, "ENC") || !_stricmp(OurClass, "MAG") || !_stricmp(OurClass, "NEC") || !_stricmp(OurClass, "SHD")) {
		if (GetPrivateProfileInt("Pet", "PetOn", 0, filename) != 0) {
			GetINI("Pet", "PetOn", 0, filename);
			GetINI("Pet", "PetCombatOn", 0, filename);
			GetINI("Pet", "PetSpell", 0, filename);
			GetINI("Pet", "PetBuffsOn", 0, filename);
			GetINI("Pet", "PetBuffsSize", 0, filename);
			GetINILoop("Pet", "PetBuffs", 0, filename);
			if (!_stricmp(OurClass, "MAG")) {
				GetINI("Pet", "PetToysOn", 0, filename);
				GetINI("Pet", "PetToysSize", 0, filename);
				GetINILoop("Pet", "PetToys", 0, filename);
				GetINI("Pet", "PetToysGave", 0, filename);
			}
			GetINI("Pet", "PetRampPullWait", 0, filename);
			GetINI("Pet", "PetSuspend", 0, filename);
			GetINI("Pet", "PetForceHealOnMed", 0, filename);
		}
	}
	//Mez Section
	if (!_stricmp(OurClass, "BRD") || !_stricmp(OurClass, "ENC") || !_stricmp(OurClass, "NEC")) {
		if (GetPrivateProfileInt("Mez", "MezOn", 0, filename) != 0) {
			GetINI("Mez", "MezOn", 0, filename);
			GetINI("Mez", "MezRadius", 0, filename);
			GetINI("Mez", "MezMinLevel", 0, filename);
			GetINI("Mez", "MezMaxLevel", 0, filename);
			GetINI("Mez", "MezStopHPs", 0, filename);
			GetINI("Mez", "MezSpell", 0, filename);
			if (!_stricmp(OurClass, "BRD") || !_stricmp(OurClass, "ENC")) {
				GetINI("Mez", "MezAESpell", 0, filename);
			}
		}
	}
	//Burn Section
	if (useConditions) GetINI("Burn", "BurnCOn", 0, filename);
	GetINI("Burn", "BurnSize", 0, filename);
	GetINI("Burn", "BurnAllNamed", 0, filename);
	GetINILoop("Burn", "Burn", 0, filename);

	//AFKTools - Not Needed

	//GoM Section
	if (_stricmp(OurClass, "BRD") && _stricmp(OurClass, "BER") && _stricmp(OurClass, "MNK") && _stricmp(OurClass, "ROG") && _stricmp(OurClass, "WAR")) {
		GetINI("GoM", "GoMSize", 0, filename);
		GetINILoop("GoM", "GoMSpell", 0, filename);
	}

	//KConditions Section
	GetINI("KConditions", "ConOn", 0, filename);
	GetINI("KConditions", "CondSize", 0, filename);
	GetINILoop("KConditions", "Cond", 0, filename);


	//MySpells - Need to figure out how to handle this.
	GetINILoop("MySpells", "Gem", 0, filename);

	WriteChatf("\agI'm finished! Template saved to Kissassist_%d_%s", OurLevel, OurClass);
	condNumber -= 1;
	if (condNumber) {
		CHAR temp[MAX_STRING] = "";
		sprintf_s(temp, "%i", condNumber);
		WritePrivateProfileString("KConditions", "ConOn", "1", newfilename);
		WritePrivateProfileString("KConditions", "CondSize", temp, newfilename);
	}
	condNumber = 1;
}


void GetINI(char Section[MAX_STRING], char Key[MAX_STRING], char Default[MAX_STRING], char ININame[MAX_STRING])
{
	char temp[MAX_STRING] = { 0 };
	if (GetPrivateProfileString(Section, Key, 0, temp, MAX_STRING, ININame) != 0)
	{
		WritePrivateProfileString(Section, Key, temp, newfilename);
	}
}

void GetINILoop(char Section[MAX_STRING], char Key[MAX_STRING], char Default[MAX_STRING], char ININame[MAX_STRING])
{
	CHAR str[MAX_STRING] = "";
	bool condFound = false;
	int size = GetPrivateProfileInt(Section, Key, 40, filename);
	CHAR KeyNum[MAX_STRING] = "";
	CHAR KeyNumCond[MAX_STRING] = "";
	CHAR temp[MAX_STRING] = "";
	CHAR tempCond[MAX_STRING] = "";
	CHAR condTemp[MAX_STRING] = "Cond";
	for (int i = 0; i < size; i++)
	{
		sprintf_s(KeyNum, "%s%i", Key, i);
		sprintf_s(KeyNumCond, "%sCond%i", Key, i);
		if (GetPrivateProfileString(Section, KeyNum, 0, temp, MAX_STRING, filename) != 0)
		{
			//str = strstr(temp, "Cond");
			//while (str != NULL)
			//{
			//	condFound = true;
			//	str = strstr(str + 1, "Cond");
			//}
			if (strstr(temp, "|Cond") != NULL)
				condFound = true;
			//WriteChatf("Section: %s, Key: %s, Value: %s", Section, KeyNum, temp);
			WritePrivateProfileString(Section, KeyNum, temp, newfilename);
			if (_stricmp(Section, "KConditions")) {
				if (GetPrivateProfileString(Section, KeyNumCond, 0, tempCond, MAX_STRING, ConditionsFile) != 0 && !condFound) {
					if (_stricmp(tempCond, "TRUE") && _stricmp(tempCond, "NULL") && _stricmp(tempCond, "FALSE")) {
						sprintf_s(condTemp, "Cond%d", condNumber);
						WritePrivateProfileString("KConditions", condTemp, tempCond, newfilename);
						
						if (GetPrivateProfileString(Section, KeyNum, 0, temp, MAX_STRING, filename) != 0) {
							sprintf_s(condTemp, "|Cond%d", condNumber);
							strcat_s(temp, condTemp);
							if (_stricmp(Section,"KConditions"))
								WritePrivateProfileString(Section, KeyNum, temp, newfilename);
						}
						condNumber += 1;
						//WritePrivateProfileString(Section, KeyNumCond, tempCond, newfilename);
					}
				}
			}
		}
		condFound = false;
	}
}

inline bool InGame()
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2());
}