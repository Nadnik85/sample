#include "MQ2KissBuffs.h"
#include "SPA.h"

char MyLastID[64] = { 0 };
char CurrentBuffs[MAX_STRING] = { 0 };
char CurrentShortBuffs[MAX_STRING] = { 0 };
char CurrentBlockedBuffs[MAX_STRING] = { 0 };
unsigned __int64 WriteBuffsTimer = 0;

int iDiseaseCounters = 0;
int iPoisonCounters = 0;
int iCurseCounters = 0;
int iCorruptionCounters = 0;

int Pulse = 0;
int PulseDelay = 15;

// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
	if (!InGame()) return;
	if (++Pulse < PulseDelay) return;
	Pulse = 0;
	WriteBuffs();
}

PLUGIN_API VOID OnZoned(VOID)
{
	if (!InGame()) return;
	strcpy_s(CurrentBuffs, MAX_STRING, "");
	strcpy_s(CurrentShortBuffs, MAX_STRING, "");
	strcpy_s(CurrentBlockedBuffs, MAX_STRING, "");
	iDiseaseCounters = 0;
	iPoisonCounters = 0;
	iCurseCounters = 0;
	iCorruptionCounters = 0;
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
	if (_stricmp(myid, MyLastID))
		CleanBuffsFile();


	//Get a list of all my Normal buffs and store them in the WriteBuffList with | seperator
	for (int i = 0; i < NUM_LONG_BUFFS; i++) {
		if (!GetCharInfo2() || !GetCharInfo2()->Buff)
			continue;
		PSPELL pBuff = GetSpellByID(GetCharInfo2()->Buff[i].SpellID);
		if (!pBuff)
			continue;
		if (strstr(pBuff->Name, ":Permanent")) continue;
		strcat_s(WriteBuffList, "|");
		strcat_s(WriteBuffList, pBuff->Name);
	}
	strcat_s(WriteBuffList, "|");
	//Write the buffs to the file
	if (!CurrentBuffs || _stricmp(CurrentBuffs, WriteBuffList)) {
		//WriteChatf("%s", WriteBuffList);
		WritePrivateProfileString(myid, "Buffs", WriteBuffList, BuffINI);
		sprintf_s(CurrentBuffs, WriteBuffList);
	}

	strcpy_s(WriteBuffList, "");//clear the buffs list.
	//lets also write the short buffs.
	for (int i = 0; i < NUM_SHORT_BUFFS; i++) {
		if (!GetCharInfo2() || !GetCharInfo2()->ShortBuff)
			continue;
		PSPELL pShortBuff = GetSpellByID(GetCharInfo2()->ShortBuff[i].SpellID);
		if (!pShortBuff)
			continue;
		if (strstr(pShortBuff->Name, ":Permanent")) continue;
		strcat_s(WriteBuffList, "|");
		strcat_s(WriteBuffList, pShortBuff->Name);
	}
	strcat_s(WriteBuffList, "|");
	//Write the Short buffs to the file
	if (!CurrentShortBuffs || _stricmp(CurrentShortBuffs, WriteBuffList)) {
		//WriteChatf("Updating ShortBuffs");
		WritePrivateProfileString(myid, "ShortBuffs", WriteBuffList, BuffINI);
		sprintf_s(CurrentShortBuffs, MAX_STRING, WriteBuffList);
	}
	strcpy_s(WriteBuffList, "");//clear the buffs list.

	//Now lets write our blocked buffs.
	for (int i = 0; i < 40; i++) {
		if (MyBlockedBuff(i)) {
			strcat_s(WriteBuffList, "|");
			strcat_s(WriteBuffList, MyBlockedBuff(i)->Name);
		}
	}
	strcat_s(WriteBuffList, "|");
	//Now lets write our blocked buffs to the file.
	if (!CurrentBlockedBuffs || _stricmp(CurrentBlockedBuffs, WriteBuffList)) {
		WritePrivateProfileString(myid, "BlockedBuffs", WriteBuffList, BuffINI);
		sprintf_s(CurrentBlockedBuffs, WriteBuffList);
	}

	{//Write Disease Counters if needed.
		int counters = GetNumDiseaseCounters();
		if (iDiseaseCounters != counters) {
			char szBuffer[8] = { 0 };
			_itoa_s(counters, szBuffer, 10);
			WritePrivateProfileString(myid, "DiseaseCounters", szBuffer, BuffINI);
			iDiseaseCounters = counters;

		}
	}

	{//Write Poison Counters if needed.
		int counters = GetNumPoisonCounters();
		if (iPoisonCounters != counters) {
			char szBuffer[8] = { 0 };
			_itoa_s(counters, szBuffer, 10);
			WritePrivateProfileString(myid, "PoisonCounters", szBuffer, BuffINI);
			iPoisonCounters = counters;
		}
	}

	{//Write Curse Counters if needed.
		int counters = GetNumCurseCounters();
		if (iCurseCounters != counters) {
			char szBuffer[8] = { 0 };
			_itoa_s(counters, szBuffer, 10);
			WritePrivateProfileString(myid, "CurseCounters", szBuffer, BuffINI);
			iCurseCounters = counters;
		}
	}

	{//Write Corruption Counters if needed.
		int counters = GetNumCorruptionCounters();
		if (iCorruptionCounters != counters) {
			char szBuffer[8] = { 0 };
			_itoa_s(counters, szBuffer, 10);
			WritePrivateProfileString(myid, "CorruptionCounters", szBuffer, BuffINI);
			iCorruptionCounters = counters;
		}
	}

	if (!InGame() || WriteBuffsTimer + 30000 > GetTickCount64()) return;


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
	if (gMacroStack && strlen(gszMacroName) && strstr(gszMacroName, "kiss")) {
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
	else {
		//if I'm not running Kissassist then AmILooting is 0
		VerifyINI(myid, "AmILooting", "0", BuffINI);

		//If no macro is running and I am the MainAssist, then my role is tank.
		if (MainAssistID() == atoi(myid)) {
			VerifyINI(myid, "MyRole", "tank", BuffINI);
		}
		else {
			VerifyINI(myid, "MyRole", "assist", BuffINI);
		}
	}
	WriteBuffsTimer = GetTickCount64();//set the timer
}

void CleanBuffsFile() {
	char BuffINI[128] = "";
	sprintf_s(BuffINI, 128, "%s\\Macros\\KissAssist_Buffs.ini", gszINIPath);
	if (MyLastID[0] != 0) {
		WritePrivateProfileString(MyLastID, NULL, NULL, BuffINI);//This deletes the INI entry for our previous ID.
	}
	if (gGameState == GAMESTATE_INGAME) _itoa_s(Me()->SpawnID, MyLastID, 64, 10);
}

PSPELL MyBlockedBuff(int i) {

	if (i < 0 || i > 40)
		return nullptr;
#ifdef NEWCHARINFO
	return nullptr;
#else
	PCHARINFONEW pCharinfo = (PCHARINFONEW)GetCharInfo();
	if (!pCharinfo || !pCharinfo->BlockedSpell[i])
		return nullptr;
	
	PSPELL BlockedSpell = GetSpellByID(pCharinfo->BlockedSpell[i]);
	if (!BlockedSpell)
		return nullptr;
	
	return BlockedSpell;
#endif
}

inline bool InGame()
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2());
}

PSPAWNINFO Me()
{
	if (!InGame()) 
		return nullptr;
	PSPAWNINFO me = GetCharInfo()->pSpawn;
	if (!me)
		return nullptr;
	return me;
}

//I only want to write the things passed through here if it doesn't already have a value.
void VerifyINI(char* Section, char* Key, char* Default, char* ININame)
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

int GetLongBuffCounters(int SPA)
{
	PCHARINFO2 pChar2 = GetCharInfo2();
	if (!pChar2)
		return 0;

	int counters = 0;
	for (unsigned long k = 0; k < NUM_LONG_BUFFS; k++) {
		PSPELL pSpell = GetSpellByID(pChar2->Buff[k].SpellID);
		if (!pSpell || pSpell->SpellType != 0)
			continue;

		long slots = GetSpellNumEffects(pSpell);
		for (long i = 0; i < slots; i++) {
			long attrib = GetSpellAttrib(pSpell, i);
			for (long j = 0; attrib == SPA && j < NUM_SLOTDATA; j++) {
				if (pChar2->Buff[k].SlotData[j].Slot == i) {
					counters += pChar2->Buff[k].SlotData[j].Value;
				}
			}
		}
	}
	return counters;
}

int GetShortBuffCounters(int SPA)
{
	PCHARINFO2 pChar2 = GetCharInfo2();
	if (!pChar2)
		return 0;

	int counters = 0;
	for (unsigned long k = 0; k < NUM_SHORT_BUFFS; k++) {
		PSPELL pSpell = GetSpellByID(pChar2->ShortBuff[k].SpellID);
		if (!pSpell || pSpell->SpellType != 0)
			continue;

		long slots = GetSpellNumEffects(pSpell);
		for (long i = 0; i < slots; i++) {
			long attrib = GetSpellAttrib(pSpell, i);
			for (long j = 0; attrib == SPA && j < NUM_SLOTDATA; j++) {
				if (pChar2->ShortBuff[k].SlotData[j].Slot == i) {
					counters += pChar2->ShortBuff[k].SlotData[j].Value;
				}
			}
		}
	}
	return counters;
}

int GetNumDiseaseCounters() {
	int counters = GetLongBuffCounters(eEQSPA::SPA_DISEASE);
	counters += GetShortBuffCounters(eEQSPA::SPA_DISEASE);
	return counters;
}

int GetNumPoisonCounters() {
	int counters = GetLongBuffCounters(eEQSPA::SPA_POISON);
	counters += GetShortBuffCounters(eEQSPA::SPA_POISON);
	return counters;
}

int GetNumCurseCounters() {
	int counters = GetLongBuffCounters(eEQSPA::SPA_CURSE);
	counters += GetShortBuffCounters(eEQSPA::SPA_POISON);
	return counters;
}

int GetNumCorruptionCounters() {
	int counters = GetLongBuffCounters(eEQSPA::SPA_CORRUPTION);
	counters += GetShortBuffCounters(eEQSPA::SPA_POISON);
	return counters;
}

unsigned long MainAssistID()
{
	PCHARINFO pChar = GetCharInfo();
	if (!pChar || !GroupSize())
		return 0;

	if (pChar->pGroupInfo->pMember[0]->MainAssist)
		return GetCharInfo()->pSpawn->SpawnID;

	for (int i = 1; i < 6; i++)
	{
		if (!pChar->pGroupInfo->pMember[i])
			continue;
		
		if (pChar->pGroupInfo->pMember[i]->MainAssist && pChar->pGroupInfo->pMember[i]->pSpawn)
		{
			return pChar->pGroupInfo->pMember[i]->pSpawn->SpawnID;
		}
		
	}
	return 0;
}

int GroupSize() {
	if (!InGame())
		return 0;
	unsigned long n = 0;
	if (!GetCharInfo()->pGroupInfo)
		return 0;

	for (int i = 1; i < 6; i++) {
		if (GetCharInfo()->pGroupInfo->pMember[i]) n++;
	}
	if (n) n++;
	return n;
}