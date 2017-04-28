#pragma once
#include "../MQ2Heals/MyTimer.h"
#include "../MQ2Heals/Character.h"



class CBeastlord : public Character
{
public:
	CBeastlord();
	~CBeastlord();

	bool FeralgiaSpellReady(string &Spell, float Distance=0);
	bool MaelstromSpellReady(string &Spell, float Distance=0);
	bool BiteSpellReady(string &Spell, float Distance = 0);
	bool LanceSpellReady(string &Spell, float Distance = 0);
	bool FrozenSpellReady(string &Spell, float Distance = 0);

private:
	vector<string> MaelstromSpells;
	vector<string> FeralgiaSpells;
	vector<string> BiteSpells;
	vector<string> LanceSpells;
	vector<string> FrozenSpells;

	MyTimer FrozenTimer;
};
