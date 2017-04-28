#pragma once

//#include "../MQ2Main/MQ2Main.h"
//#include "../MQ2Main/MQ2Globals.h"
#ifndef PLUGIN_API
#include "../MQ2Plugin.h"
#endif PLUGIN_API

#include <string>
#include <list>

class Character
{
private:
	static PCHARINFO info;
	static PCHARINFO2 info2;
	enum StandState { StateStanding, StateSitting, StateUnknown };
	int Pct(int cur, int max);
	long Evaluate(PCHAR zFormat, ...);
	PSPELL MemorizedSpells[0x10];
	int MedPct = 98;

public:
	Character();
	~Character();

	bool Standing();
	bool Sitting();
	bool OnMount();
	bool Moving();
	bool FeetWet();
	bool AnyBuff(PCHAR _BuffName, bool _Partial = true);
	bool Buff(PCHAR _BuffName, bool _Partial = true);
	bool ShortBuff(PCHAR _BuffName, bool _Partial = true);
	bool TargetBuff(PCHAR _BuffName, bool _Partial = true);
	bool CombatState(char *&state);
	int XTargetHaters(int *level = NULL, bool Named = false, float Dist=0.0);
	int XTargetNamed(int *level = NULL);
	bool InCombat();
	bool Invis();
	int PctMana();
	int PctHPs();
	long CastingLeft();
	long CastingCoolDown();
	bool CastReady();
	bool GetSpellRankName(PCHAR spell);
	bool IsItemReady(PCHAR Index, float Range = 0);
	bool IsSpellReady(PCHAR Index, float Range = 0);
	bool IsAAReady(PCHAR Index, float Range=0);
	DWORD GetPetID();
	void UpdateMemorizedSpells();
	bool Indoors();
	bool Outdoors();
	int Level();
	bool MedCheck();

	bool ItemTypeReady(string &SpellName, const vector <string>SpellList, float Range = 0);
	bool SpellTypeReady(string &SpellName, const vector <string>SpellList, float Range = 0);
	bool AATypeReady(string &SpellName, vector <string>AAList, float Range = 0);
	PlayerClass Class();
};
