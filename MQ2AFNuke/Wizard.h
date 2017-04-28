#pragma once

#include "..\MQ2Heals\Character.h"

class CWizard : public Character
{
public:
	CWizard();
	~CWizard();
	bool MagicEtherealReady(string &NukeSpell, float Distance=0);
	bool FireEtherealReady(string &NukeSpell, float Distance=0);
	bool IceEtherealReady(string &NukeSpell, float Distance=0);
	bool EtherealWeaveReady(string &NukeSpell, float Distance=0);

	bool MagicInstantReady(string &NukeSpell, float Distance=0);
	bool FireInstantReady(string &NukeSpell, float Distance=0);
	bool IceInstantReady(string &NukeSpell, float Distance=0);

	bool MagicClawReady(string &NukeSpell, float Distance=0);
	bool FireClawReady(string &NukeSpell, float Distance=0);
	bool IceClawReady(string &NukeSpell, float Distance=0);

	bool FreeNukeReady(string &NukeAA, float Distance=0);

	bool FastNukeReady(string &NukeSpell, float Distance=0);
	bool DichotomicNukeReady(string &NukeSpell, float Distance=0);
	bool AllianceSpellReady(string &NukeSpell, float Distance);
	bool VortexNukeReady(string &NukeSpell, float Distance=0);
	bool LowLevelNukeReady(string &NukeSpell, float Distance = 0);

	bool MemorizeSpells();

private:
	vector<string> EtherealIceSpells;
	vector<string> EtherealFireSpells;
	vector<string> EtherealMagicSpells;
	vector<string> EtherealWeaveSpells;

	vector<string> FreeNukeAAs;

	vector<string> InstantIceSpells;
	vector<string> InstantFireSpells;
	vector<string> InstantMagicSpells;

	vector<string> ClawIceSpells;
	vector<string> ClawFireSpells;
	vector<string> ClawMagicSpells;

	vector<string> FastNukeSpells;

	vector<string> BeamSpells;
	vector<string> SplashSpells;

	vector<string> DichotomicSpells;
	vector<string> AllianceSpells;
	vector<string> VortexSpells;
	vector<string> LowLevelNukes;

	SPELLFAVORITE MemSpellFavorite;
};
