#pragma once

#include "..\MQ2Heals\Character.h"

class CMagician : public Character
{
public:
	CMagician();
	~CMagician();

	bool ServantSpellReady(string &Spell, float Distance=0);

	bool FickleSpellReady(string &Spell, float Distance = 0);

	bool SpearSpellReady(string &Spell, float Distance = 0);
	bool BoltSpellReady(string &Spell, float Distance=0);

	bool FireRainSpellReady(string &Spell, float Distance=0);
	bool MagicRainSpellReady(string &Spell, float Distance=0);
	bool FreeNukeReady(string &NukeSpell, float Distance);
	bool ThaumatizeSpellReady(string &Spell, float Distance);

	bool NextItemClicky(string &Spell, float Distance);
	bool DichotomicSpellReady(string &Spell, float Distance);
	bool OfManySpellReady(string &Spell, float Distance);

private:
	vector<string> MagicRainSpells;
	vector<string> FireRainSpells;

	vector<string> SpearSpells;
	vector<string> BoltSpells;

	vector<string> FickleSpells;

	vector<string> ServantSpells;

	vector<string> FreeNukeAAs;
	vector<string> ThaumatizeSpells;

	vector<string> ItemClickies;

	vector<string> DichotomicSpells;
	vector<string> OfManySpells;
};
