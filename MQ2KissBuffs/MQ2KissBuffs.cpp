#include "MQ2KissBuffs.h"

char MyLastID[64] = { 0 };
unsigned __int64 WriteBuffsTimer = 0;
int Pulse = 0;
int PulseDelay = 15;

// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
	if (!InGame()) return;
	if (++Pulse < PulseDelay) return;
	Pulse = 0;
	if (!InGame() || WriteBuffsTimer + 30000 > GetTickCount64()) return;
	WriteBuffs();
}

PLUGIN_API VOID OnZoned(VOID)
{
	if (!InGame()) return;
	WriteBuffs();
}

PLUGIN_API VOID ShutdownPlugin(VOID)
{
	if (!InGame()) return;
	CleanBuffsFile();
}

PLUGIN_API VOID SetGameState(DWORD GameState)
{
	if (GameState != GAMESTATE_INGAME)
		CleanBuffsFile();
}

void WriteBuffs() {
	//Set the INI to use.
	char BuffINI[128] = "";
	sprintf_s(BuffINI, 128, "%s\\Macros\\KissAssist_Buffs.ini", gszINIPath);

	char WriteBuffList[MAX_STRING] = { 0 };
	char myid[64] = { 0 };
	_itoa_s(Me()->SpawnID, myid, 64, 10);//Get my Current SpawnID

	//If my ID and MyLastID don't match, Delete the INI Entry for my previous ID, and set MyLastID to my current ID
	if (_stricmp(myid, MyLastID)) {
		CleanBuffsFile();
	}

	time_t now = time(0);
	struct tm localTime;
	localtime_s(&localTime, &now);
	//Get Current Day
	char day[4] = { 0 };
	_itoa_s(localTime.tm_mday, day, 4, 10);
	//Get current Hour.
	char hour[4] = { 0 };
	_itoa_s(localTime.tm_hour, hour, 4, 10);
	char ZoneID[8] = { 0 };
	//Get ZoneID.
	_itoa_s((((PSPAWNINFO)pLocalPlayer)->GetZoneID() & 0x7FFF), ZoneID, 8, 10);

	//Write the day hour and ZoneID for this buff list to an INI section of myid
	WritePrivateProfileString(myid, "Day", day, BuffINI);
	WritePrivateProfileString(myid, "Hour", hour, BuffINI);
	WritePrivateProfileString(myid, "Zone", ZoneID, BuffINI);

	//Am I running a macro.
	if (gMacroStack && strlen(gszMacroName)) {
		if (strstr(gszMacroName, "kiss")) {
			//Let us get the role we are in from the macro
			if (IsDefined("Role")) {
				char theRole[64] = "${Role}";
				ParseMacroData(theRole, 64);
				WritePrivateProfileString(myid, "MyRole", theRole, BuffINI);
			}
			else if (GetGroupMainAssistTargetID() == atoi(myid)) {
				//if for whatever reason the macro is kiss but ${Role} isn't defined. Lets see if I'm the MA. 
				WritePrivateProfileString(myid, "MyRole", "tank", BuffINI);
			}
			else {
				//if for whatever reason the macro is kiss but ${Role} isn't defined. If I'm not the MA, then I should be assist.
				WritePrivateProfileString(myid, "MyRole", "assist", BuffINI);
			}
			//Lets get AmILooting from the macro
			if (IsDefined("AmILooting")) {
				char AmILooting[64] = "${AmILooting}";
				ParseMacroData(AmILooting, 64);
				WritePrivateProfileString(myid, "AmILooting", AmILooting, BuffINI);
			}
			else {
				//if the macro is Kiss but AmILooting isn't defined, then set it to 0.
				VerifyINI(myid, "AmILooting", "0", BuffINI);
			}

		}
	}
	else {
		//if I'm not running Kissassist then AmILooting is 0
		VerifyINI(myid, "AmILooting", "0", BuffINI);

		//If no macro is running and I am the MainAssist, then my role is tank.
		if (GetGroupMainAssistTargetID() == atoi(myid)) {
			VerifyINI(myid, "MyRole", "tank", BuffINI);
		}
		else {
			VerifyINI(myid, "MyRole", "assist", BuffINI);
		}
	}

	//Get a list of all my Normal buffs and store them in the WriteBuffList with | seperator
	for (int i = 0; i < NUM_LONG_BUFFS; i++) {
		if (GetCharInfo2() && GetCharInfo2()->Buff) {
			if (PSPELL pBuff = GetSpellByID(GetCharInfo2()->Buff[i].SpellID)) {
				if (strstr(pBuff->Name, ":Permanent")) continue;
				strcat_s(WriteBuffList, "|");
				strcat_s(WriteBuffList, pBuff->Name);
			}
		}
	}
	//Write the buffs to the file
	WritePrivateProfileString(myid, "Buffs", WriteBuffList, BuffINI);
	sprintf_s(WriteBuffList, "");//clear the buffs list.

	//lets also write the short buffs.
	for (int i = 0; i < NUM_SHORT_BUFFS; i++) {
		if (GetCharInfo2() && GetCharInfo2()->ShortBuff) {
			if (PSPELL pShortBuff = GetSpellByID(GetCharInfo2()->ShortBuff[i].SpellID)) {
				if (strstr(pShortBuff->Name, ":Permanent")) continue;
				strcat_s(WriteBuffList, "|");
				strcat_s(WriteBuffList, pShortBuff->Name);
			}
		}
	}
	//Write the Short buffs to the file
	WritePrivateProfileString(myid, "ShortBuffs", WriteBuffList, BuffINI);
	sprintf_s(WriteBuffList, "");//clear the buffs list.

	//Now lets write our blocked buffs.
	for (int i = 0; i < 40; i++) {
		if (MyBlockedBuff(i)) {
			strcat_s(WriteBuffList, "|");
			strcat_s(WriteBuffList, MyBlockedBuff(i)->Name);
		}
	}
	//write the blocked buffs to the file.
	WritePrivateProfileString(myid, "BlockedBuffs", WriteBuffList, BuffINI);
	WriteBuffsTimer = GetTickCount64();//set the timer
}

void CleanBuffsFile() {
	char BuffINI[128] = "";
	sprintf_s(BuffINI, 128, "%s\\Macros\\KissAssist_Buffs.ini", gszINIPath);
	WritePrivateProfileString(MyLastID, NULL, NULL, BuffINI);//This deletes the INI entry for our previous ID.
	if (gGameState == GAMESTATE_INGAME) _itoa_s(Me()->SpawnID, MyLastID, 64, 10);
}

PSPELL MyBlockedBuff(int i) {

	if (i < 0 || i > 40)
		return false;
#ifdef NEWCHARINFO
	return false;
#else
	if (PCHARINFONEW pCharinfo = (PCHARINFONEW)GetCharInfo()) {
		if (pCharinfo->BlockedSpell[i]) {
			if (PSPELL BlockedSpell = GetSpellByID(pCharinfo->BlockedSpell[i])) {
				return BlockedSpell;
			}
		}
	}
	return false;
#endif
}

inline bool InGame()
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2());
}

PSPAWNINFO Me()
{
	if (!InGame()) return false;
	if (PSPAWNINFO me = GetCharInfo()->pSpawn)
		return me;
	return false;
}

//I only want to write the things passed through here if it doesn't already have a value.
void VerifyINI(char Section[MAX_STRING], char Key[MAX_STRING], char Default[MAX_STRING], char ININame[MAX_STRING])
{
	char temp[MAX_STRING] = { 0 };
	if (GetPrivateProfileString(Section, Key, 0, temp, MAX_STRING, ININame) == 0)
	{
		WritePrivateProfileString(Section, Key, Default, ININame);
	}
}

//Check to see if a macro variable is defined.
bool IsDefined(PCHAR szLine) {
	return (FindMQ2DataVariable(szLine) != 0);
}