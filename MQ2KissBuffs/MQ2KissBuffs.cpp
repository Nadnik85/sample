#include "MQ2KissBuffs.h"

unsigned __int64 WriteBuffsTimer = 0;
unsigned __int64 WriteBlockedBuffsTimer = 0;
int Pulse = 0;
int PulseDelay = 15;

// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
#ifdef BugsUser
	if (!Authorized) return;
#endif
	if (!InGame()) return;
	if (++Pulse < PulseDelay) return;
	Pulse = 0;
	WriteBuffs();
}

void WriteBuffs() {
	if (!InGame() || WriteBuffsTimer + 30000 > GetTickCount64()) return;

	char BuffINI[MAX_STRING] = "";
	sprintf_s(BuffINI, MAX_STRING, "%s\\Macros\\KissAssist_Buffs.ini", gszINIPath);
	char WriteBuffList[MAX_STRING] = { 0 };
	char myid[MAX_STRING] = { 0 };
	_itoa_s(Me()->SpawnID, myid, MAX_STRING, 10);

	CleanBuffsFile();

	time_t now = time(0);
	struct tm localTime;
	localtime_s(&localTime, &now);
	//Get Current Day
	char day[MAX_STRING] = { 0 };
	_itoa_s(localTime.tm_mday, day, MAX_STRING, 10);
	//Get current Hour.
	char hour[MAX_STRING] = { 0 };
	_itoa_s(localTime.tm_hour, hour, MAX_STRING, 10);
	char ZoneID[MAX_STRING] = { 0 };
	//Get ZoneID.
	_itoa_s((((PSPAWNINFO)pLocalPlayer)->GetZoneID() & 0x7FFF), ZoneID, MAX_STRING, 10);

	WritePrivateProfileString(myid, "Day", day, BuffINI);
	WritePrivateProfileString(myid, "Hour", hour, BuffINI);
	WritePrivateProfileString(myid, "Zone", ZoneID, BuffINI);
	VerifyINI(myid, "Buffs", "", BuffINI);
	VerifyINI(myid, "Blockedbuffs", "", BuffINI);
	VerifyINI(myid, "AmILooting", "0", BuffINI);
	VerifyINI(myid, "MyRole", "Assist", BuffINI);
	//Write all current buffs to Kissassist_buffs.ini file
	for (int i = 0; i < NUM_LONG_BUFFS; i++) {
		if (GetCharInfo2() && GetCharInfo2()->Buff) {
			if (PSPELL pBuff = GetSpellByID(GetCharInfo2()->Buff[i].SpellID)) {
				if (strstr(pBuff->Name, ":Permanent")) continue;
				strcat_s(WriteBuffList, pBuff->Name);
				strcat_s(WriteBuffList, "|");
			}
		}
	}
	//Write the buffs to the file
	WritePrivateProfileString(myid, "Buffs", WriteBuffList, BuffINI);
	WriteBuffsTimer = GetTickCount64();


	if (WriteBlockedBuffsTimer + 120000 > GetTickCount64()) return;
	char BlockedBuffList[MAX_STRING] = { 0 };
	//Now lets write our blocked buffs.
	for (int i = 0; i < 40; i++) {
		if (MyBlockedBuff(i)) {
			strcat_s(BlockedBuffList, MyBlockedBuff(i)->Name);
			strcat_s(BlockedBuffList, "|");
		}
	}
	WritePrivateProfileString(myid, "BlockedBuffs", BlockedBuffList, BuffINI);
	WriteBlockedBuffsTimer = GetTickCount64();
}

void CleanBuffsFile() {

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
void VerifyINI(char Section[MAX_STRING], char Key[MAX_STRING], char Default[MAX_STRING], char ININame[MAX_STRING])
{
	char temp[MAX_STRING] = { 0 };
	if (GetPrivateProfileString(Section, Key, 0, temp, MAX_STRING, ININame) == 0)
	{
		WritePrivateProfileString(Section, Key, Default, ININame);
	}
}