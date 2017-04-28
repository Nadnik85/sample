#pragma once

#include "..\MQ2Heals\Character.h"

class CEnchanter : public Character
{
public:
	CEnchanter();
	~CEnchanter();

	bool BigNukeReady(string &NukeSpell, float Distance=0);
	bool BigDoTReady(string &NukeSpell, float Distance = 0);
	bool SmallDoTReady(string &NukeSpell, float Distance = 0);
	bool AssaultNukeReady(string &NukeSpell, float Distance = 0);
	bool TashSpellReady(string &NukeSpell, float Distance = 0);
	bool MezSpellReady(string &NukeSpell, float Distance = 0);
	bool ManaDrainSpellReady(string &NukeSpell, float Distance = 0);
	bool DichoSpellReady(string &NukeSpell, float Distance = 0);
	bool SlowSpellReady(string &NukeSpell, float Distance = 0);
	bool TargetHasManaDrainDebuff();
	bool TargetHasTashDebuff();
	bool TargetHasSlowDebuff();
	bool TargetHasBigDoT();
	bool TargetHasSmallDoT();
	bool MezNeeded();

	bool MemorizeSpells();
	void SetMATarget(PSPAWNINFO Target);

private:
	vector<string> BigNukeSpells;
	vector<string> AssaultNukeSpells;
	vector<string> ManaDrainSpells;
	vector<string> DichotomicSpells;
	vector<string> SlowSpells;
	vector<string> CrippleSpells;
	vector<string> BigDoTSpells;
	vector<string> SmallDoTSpells;

	vector<string> SingleMezSpells;
	vector<string> PBAEMezSpells;
	vector<string> TAEMezSpells;

	vector<string> TashSpells;

	SPELLFAVORITE MemSpellFavorite;

	PSPAWNINFO CMATarget;	// Current MA Target
	PSPAWNINFO LMATarget;	// Last MA Target

	vector<PSPAWNINFO> XTargets;
};
