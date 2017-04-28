#pragma once
#include "Character.h"
class Healer :
	public Character
{
public:
	Healer();
	~Healer();

	bool BigHealReady(string &HealSpell, float Distance = 0);
	bool HealNukeReady(string &HealSpell, float Distance = 0);
	bool NukeHealReady(string &HealSpell, float Distance = 0);
	bool FastHealReady(string &HealSpell, float Distance = 0);
	bool PatchHealReady(string &HealSpell, float Distance = 0);
	bool SlowHealReady(string &HealSpell, float Distance = 0);
	bool GroupHealReady(string &HealSpell, float Distance = 0);
	bool DelayedHealReady(string &HealSpell, float Distance = 0);
	bool HotSpellReady(string &HealSpell, float Distance = 0);
	bool GroupHotSpellReady(string &HealSpell, float Distance = 0);

	vector <string> WardSpells;
	vector <string> FastHeals;
	vector <string> GroupHeals;
	vector <string> PatchHeals;
	vector <string> SlowHeals;
	vector <string> HealNukes;
	vector <string> NukeHeals;
	vector <string> BigHeals;
	vector <string> DelayedHeals;
	vector <string> YaulpSpells;
	vector <string> HotSpells;
	vector <string> GroupHotSpells;
};

