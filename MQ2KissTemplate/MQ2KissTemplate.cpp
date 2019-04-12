// MQ2KissTemplate.cpp - 10/12/2018
// By Chatwiththisname @ Redguides.com



#include "../MQ2Plugin.h"
#include <iostream>
#include <fstream>
using namespace std;

#define PLUGIN_NAME "MQ2KissTemplate"
#define VERSION 0.1

PreSetup(PLUGIN_NAME);
PLUGIN_VERSION(VERSION);

//Variables
bool oldCond = false;
bool useClass = false;
bool useLevel = false;
bool pulling = false;
bool debugging = false;

CHAR filename[MAX_STRING] = "";
CHAR newfilename[MAX_STRING] = "";
CHAR OurClass[MAX_STRING] = "";
CHAR ConditionsFile[MAX_STRING] = "";
CHAR tempClass[MAX_STRING] = "";
CHAR tempLevel[MAX_STRING] = "";

int condNumber = 1;
int OurLevel = 0;
int useConditions = 0;
//End Variables

//Prototypes
void GetINI(char Section[MAX_STRING], char Key[MAX_STRING], char Default[MAX_STRING], char ININame[MAX_STRING]);
void GetINILoop(char Section[MAX_STRING], char Key[MAX_STRING], char Default[MAX_STRING], char ININame[MAX_STRING]);
void ParseArg(CHAR Arg[MAX_STRING]);
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

	CHAR temp[MAX_STRING] = "";
	CHAR Arg1[MAX_STRING] = "";
	CHAR Arg2[MAX_STRING] = "";
	CHAR Arg3[MAX_STRING] = "";
	CHAR Arg4[MAX_STRING] = "";
	GetArg(Arg1, szLine, 1);
	GetArg(Arg2, szLine, 2);
	GetArg(Arg3, szLine, 3);
	GetArg(Arg4, szLine, 4);
	ParseArg(Arg1);
	ParseArg(Arg2);
	ParseArg(Arg3);
	ParseArg(Arg4);
	

	
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
	if (useConditions == 0)
		sprintf_s(ConditionsFile, "");
	if (useConditions == 1)
		sprintf_s(ConditionsFile, "%s\\Macros\\Kissassist_%s_Conditions.ini", gszINIPath, pChar->Name);
	if (useConditions == 2)
		sprintf_s(ConditionsFile, "%s\\Macros\\Kissassist_%s.ini", gszINIPath, pChar->Name);

	//Lemme insert the start of code tags for the user using ios::in - this should overwrite the file for us.
	remove(newfilename);
	WritePrivateProfileString("t", "t", "2", newfilename);
	ofstream myfile;
	myfile.open(newfilename, ios::in);
	char codetag[MAX_STRING] = { 0 };
	sprintf_s(codetag, "[CODE=INI]", OurClass);
	myfile << codetag << endl;
	myfile.close();

	//General Section
	GetINI("General", "KissAssistVer", 0, filename);
	if (oldCond) {
		WritePrivateProfileString("General", "ConditionsOn", "2", newfilename);
		//useConditions = 2;
	}
	if (!_stricmp(OurClass, "BRD")) {
		if (GetPrivateProfileInt("General", "TwistOn", 0, filename) != 0) {
			GetINI("General", "TwistOn", 0, filename);
			GetINI("General", "TwistMed", 0, filename);
			GetINI("General", "TwistWhat", 0, filename);
		}
	}
	//Spellset section
	WritePrivateProfileString("SpellSet", "LoadSpellSet", "2", newfilename);

	//Buff Section
	if (GetPrivateProfileInt("Buffs", "BuffsOn", 0, filename) != 0) {
		GetINI("Buffs", "BuffsOn", 0, filename);
		if (oldCond) 
			if (useConditions) 
				GetINI("Buffs", "BuffsCOn", 0, filename);
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
		if (oldCond) 
			if (useConditions)
				GetINI("DPS", "DPSCOn", 0, filename);
		GetINI("DPS", "DPSSize", 0, filename);
		GetINI("DPS", "DPSSkip", 0, filename);
		GetINI("DPS", "DPSInterval", 0, filename);
		GetINILoop("DPS", "DPS", 0, filename);
		GetINI("DPS", "DebuffAllOn", 0, filename);
	}

	//Aggro Section
	if (GetPrivateProfileInt("Aggro", "AggroOn", 0, filename)) {
		GetINI("Aggro", "AggroOn", 0, filename);
		if (oldCond) 
			if (useConditions)
				GetINI("Aggro", "AggroCOn", 0, filename);
		GetINI("Aggro", "AggroSize", 0, filename);
		GetINILoop("Aggro", "Aggro", 0, filename);
	}

	//Pull Section - PullWith Only??
	if (pulling) {
		if (GetPrivateProfileString("Pull", "PullWith", 0, temp, MAX_STRING, filename) != 0)
			GetINI("Pull", "PullWith", 0, filename);
	}
	

	//Heals Section
	if (GetPrivateProfileInt("Heals", "HealsOn", 0, filename) != 0) {
		GetINI("Heals", "HealsOn", 0, filename);
		if (oldCond) 
			if (useConditions)
				GetINI("Heals", "HealsCOn", 0, filename);
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
	if (oldCond) 
		if (useConditions)
			GetINI("Burn", "BurnCOn", 0, filename);
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

	//Lets close the code tags now. Open newfilename to append to the end of it with ios::app
	myfile.open(newfilename, ios::app);
	myfile << "[/CODE]" << endl;
	myfile.close();

	WriteChatf("\agI'm finished! Template saved to Kissassist_%d_%s", OurLevel, OurClass);
	condNumber -= 1;
	if (condNumber) {
		CHAR temp[MAX_STRING] = "";
		sprintf_s(temp, "%i", condNumber);
		WritePrivateProfileString("KConditions", "ConOn", "1", newfilename);
		WritePrivateProfileString("KConditions", "CondSize", temp, newfilename);
	}
	condNumber = 1;
	//reset my arguement modified variables back to their default settings.  
	sprintf_s(tempClass, "");
	sprintf_s(tempLevel, "");
	useLevel = false;
	useClass = false;
	oldCond = false;
	pulling = false;
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
	CHAR szCondNumber[2] = "";
	for (int i = 0; i < size; i++)
	{
		sprintf_s(KeyNum, "%s%i", Key, i);
		sprintf_s(KeyNumCond, "%sCond%i", Key, i);
		sprintf_s(szCondNumber, "");

		//If there is something stored in our current INI at this section and key, then lets do some copying.
		if (GetPrivateProfileString(Section, KeyNum, 0, temp, MAX_STRING, filename) != 0)
		{
			//If Section, Key is not equal to "NULL" then lets write it to the new file. 
			if (_stricmp(temp, "NULL")) {

				//if |Cond is found in the string for temp, set condfound == true;
				if (strstr(temp, "|Cond") != NULL) {
					//WriteChatf("Condition Found");
					condFound = true;
				}
				//if converting to old conditions, remove |Cond## from the end of the string. 
				//WriteChatf("oldCond: %d, condFound: %d", oldCond, condFound);
				if (oldCond && condFound) {
					char *p = strrchr(temp, '|');
					if (p) p[0] = 0;
					p += 5;
					if (p) sprintf_s(szCondNumber,"%s",p);
					//WriteChatf("CondNumber: %s", szCondNumber);
				}
				WritePrivateProfileString(Section, KeyNum, temp, newfilename);
				//does it already have the KA11 Condition style. 
				
				//If the section isn't KConditions - Lets process conditions.
				if (_stricmp(Section, "KConditions")) {
					//If Section, KeyCond# has a value  and we didn't find a |Cond# in the key in our current condition file, then lets see about copying some things.
					if (GetPrivateProfileString(Section, KeyNumCond, 0, tempCond, MAX_STRING, ConditionsFile) != 0 && !condFound) {
						//If the condition isn't TRUE, isn't NULL,  isn't FALSE, and oldCond isn't true, then lets upgrade it to KA11. 
						if (_stricmp(tempCond, "TRUE") && _stricmp(tempCond, "NULL") && _stricmp(tempCond, "FALSE") && !oldCond) {
							//set CondTemp to our current condNumber and then write the condition to the new file. 
							sprintf_s(condTemp, "Cond%d", condNumber);
							WritePrivateProfileString("KConditions", condTemp, tempCond, newfilename);
							//Just to be sure, if the Section, KeyNum isn't blank, then lets append |Cond# to the end of the current key in the new file.  
							if (GetPrivateProfileString(Section, KeyNum, 0, temp, MAX_STRING, filename) != 0) {
								sprintf_s(condTemp, "|Cond%d", condNumber);
								strcat_s(temp, condTemp);
								WritePrivateProfileString(Section, KeyNum, temp, newfilename);
							}
							//We've used this Cond#, so lets add 1 to it.
							condNumber += 1;
						}
						//If oldCond is true, then we're going to write using the old conditions style. 
						if (oldCond) WritePrivateProfileString(Section, KeyNumCond, tempCond, newfilename);
					}
					else if (condFound) {
						sprintf_s(condTemp, "Cond%s", szCondNumber);
						if (GetPrivateProfileString("KConditions", condTemp, 0, tempCond, MAX_STRING, filename) != 0) {
							WritePrivateProfileString(Section, KeyNumCond, tempCond, newfilename);
						}
						
					}
				}
			}
		}
		condFound = false;
	}
}


void ParseArg(CHAR Arg[MAX_STRING])
{
	if (strlen(Arg)) {
		//WriteChatf("Parsing Arg: %s", Arg);
		if (!IsNumber(Arg)) {
			if (debugging) WriteChatf("Arg is: %s", Arg);
			if (_stricmp(Arg, "BRD") && _stricmp(Arg, "BST") && _stricmp(Arg, "BER") && _stricmp(Arg, "CLR") && _stricmp(Arg, "DRU") && _stricmp(Arg, "ENC") && _stricmp(Arg, "MAG") && _stricmp(Arg, "MNK") && _stricmp(Arg, "NEC") && _stricmp(Arg, "PAL") && _stricmp(Arg, "RNG") && _stricmp(Arg, "ROG") && _stricmp(Arg, "SHD") && _stricmp(Arg, "SHM") && _stricmp(Arg, "WAR") && _stricmp(Arg, "WIZ") && _stricmp(Arg, "old") && _stricmp(Arg, "pull")) {
				WriteChatf("You've provided %s as a class to emulate, but that doesn't appear to be one of the 16 playable classes.", Arg);
				return;
			} else if (_stricmp(Arg, "old") && _stricmp(Arg, "pull")) {
				sprintf_s(tempClass, Arg);
				if (debugging) WriteChatf("\aySetting useClass to true");
				useClass = true;
			} else if (!_stricmp(Arg, "old")) {
				oldCond = true;
			} else if (!_stricmp(Arg, "pull")) {
				if (debugging) WriteChatf("\aySetting pulling to true");
				pulling = true;
			}
		} else {
			if (debugging) WriteChatf("Arg is: %d", atoi(Arg));
			if (atoi(Arg) < 0 || atoi(Arg) > 110) {
				WriteChatf("You've provided %s as a level to emulate, but it is not between 1 and 110", tempLevel);
				return;
			} else {
				sprintf_s(tempLevel, Arg);
				if (debugging) WriteChatf("\aySetting useLevel to true");
				useLevel = true;
			}
		}
	}	
}

inline bool InGame()
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2());
}