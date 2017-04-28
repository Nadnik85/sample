#include "Enchanter.h"

CEnchanter::CEnchanter()
{
	BigNukeSpells.push_back("Mindsunder");
	BigNukeSpells.push_back("Mindcleave");
	BigNukeSpells.push_back("Mindscythe");
	BigNukeSpells.push_back("Mindblade");

	AssaultNukeSpells.push_back("Intellectual Appropriation");
	AssaultNukeSpells.push_back("Polyrefractive Assault");
	AssaultNukeSpells.push_back("Phantasmal Assault");
	AssaultNukeSpells.push_back("Arcane Assault");
	AssaultNukeSpells.push_back("Spectral Assault");
	AssaultNukeSpells.push_back("Polychaotic Assault");
	AssaultNukeSpells.push_back("Multichromatic Assault");
	AssaultNukeSpells.push_back("Polychromatic Assault");

	ManaDrainSpells.push_back("Mind Storm");
	ManaDrainSpells.push_back("Mind Squall");
	ManaDrainSpells.push_back("Mind Spiral");
	ManaDrainSpells.push_back("Mind Helix");
	ManaDrainSpells.push_back("Mind Twist");
	ManaDrainSpells.push_back("Mind Oscillate");
	ManaDrainSpells.push_back("Mind Phobiate");
	ManaDrainSpells.push_back("Mind Shatter");

	BigDoTSpells.push_back("Drown");

	SmallDoTSpells.push_back("Bewildering Constriction");

	SingleMezSpells.push_back("Bewilder");
	SingleMezSpells.push_back("Confound");
	SingleMezSpells.push_back("Mislead");
	SingleMezSpells.push_back("Baffle");
	SingleMezSpells.push_back("Befuddle");
	SingleMezSpells.push_back("Mystify");
	SingleMezSpells.push_back("Bewilderment");
	SingleMezSpells.push_back("Euphoria");
	SingleMezSpells.push_back("Echoing Madness");
	SingleMezSpells.push_back("Felicity");
	SingleMezSpells.push_back("Bliss");
	SingleMezSpells.push_back("Sleep");
	SingleMezSpells.push_back("Apathy");
	SingleMezSpells.push_back("Ancient Eternal Rapture");
	SingleMezSpells.push_back("Rapture");
	SingleMezSpells.push_back("Glamour of Kintaz");
	SingleMezSpells.push_back("Dazzle");
	SingleMezSpells.push_back("Entrance");
	SingleMezSpells.push_back("Enthrall");
	SingleMezSpells.push_back("Mesmerize");

	TAEMezSpells.push_back("Deadening Wave");
	TAEMezSpells.push_back("Slackening Wave");
	TAEMezSpells.push_back("Peaceful Wave");
	TAEMezSpells.push_back("Serene Wave");
	TAEMezSpells.push_back("Enscorcelling Wave");
	TAEMezSpells.push_back("Quelling Wave");
	TAEMezSpells.push_back("Wake of Subdual");
	TAEMezSpells.push_back("Wake of Felicity");
	TAEMezSpells.push_back("Bliss of the Nihil");
	TAEMezSpells.push_back("Fascination");
	TAEMezSpells.push_back("Mesmerization");

	PBAEMezSpells.push_back("Perilous Bafflement");

	TashSpells.push_back("Decree of Tashan");
	TashSpells.push_back("Enunciation of Tashan");
	TashSpells.push_back("Declaration of Tashan");
	TashSpells.push_back("Clamor of Tashan");
	TashSpells.push_back("Bark of Tashan");
	TashSpells.push_back("Din of Tashan");
	TashSpells.push_back("Echo of Tashan");
	TashSpells.push_back("Howl of Tashan");
	TashSpells.push_back("Wind of Tashanian");
	TashSpells.push_back("Tashanian");
	TashSpells.push_back("Wind of Tashani");
	TashSpells.push_back("Tashania");
	TashSpells.push_back("Tashani");
	TashSpells.push_back("Tashina");

	SlowSpells.push_back("Languid Pace");
	SlowSpells.push_back("Languid Pace");
	SlowSpells.push_back("Undermining Helix");
	SlowSpells.push_back("Diminishing Helix");
	SlowSpells.push_back("Attenuating Helix");
	SlowSpells.push_back("Curtailing Helix");
	SlowSpells.push_back("Desolate Deeds");
	SlowSpells.push_back("Dreary Deeds");
	SlowSpells.push_back("Forlorn Deeds");
	SlowSpells.push_back("Shiftless Deeds");
	SlowSpells.push_back("Tepid Deeds");
	SlowSpells.push_back("Languid Pace");

	DichotomicSpells.push_back("Dichotomic Reinforcement");

	CMATarget = NULL;
	LMATarget = NULL;
}

bool CEnchanter::MezNeeded()
{
	if (XTargetHaters() < 2) return false;
	if (!GetCharInfo()->pXTargetMgr) return false;
	for (int x = 0; x < MAX_XTARGETS; x++)
	{

	}
	return false;
}

void CEnchanter::SetMATarget(PSPAWNINFO _Target)
{
	if (!_Target) return;
	LMATarget = CMATarget;
	CMATarget = _Target;
}

bool CEnchanter::MezSpellReady(string &NukeSpell, float Distance)
{
	int haters = XTargetHaters();
	if (haters < 2) return false;
	if (haters > 3)
	{
		if (SpellTypeReady(NukeSpell, TAEMezSpells, Distance))
			return true;
	}
	if (SpellTypeReady(NukeSpell, TAEMezSpells, Distance))
	{
		return true;
	}
	return false;
}

bool CEnchanter::BigNukeReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, BigNukeSpells, Distance);
}

bool CEnchanter::BigDoTReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, BigDoTSpells, Distance);
}

bool CEnchanter::SmallDoTReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, SmallDoTSpells, Distance);
}

bool CEnchanter::AssaultNukeReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, AssaultNukeSpells, Distance);
}

bool CEnchanter::TashSpellReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, TashSpells, Distance);
}

bool CEnchanter::SlowSpellReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, SlowSpells, Distance);
}

bool CEnchanter::DichoSpellReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, DichotomicSpells, Distance);
}

bool CEnchanter::ManaDrainSpellReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, ManaDrainSpells, Distance);
}

bool CEnchanter::TargetHasBigDoT()
{
	if (!pTargetWnd) return false;
	if (!pTarget) return false;
	for (size_t x = 0; x < BigDoTSpells.size(); x++)
	{
		if (TargetBuff((PCHAR)BigDoTSpells[x].c_str(), true))
			return true;
	}
	return false;
}

bool CEnchanter::TargetHasSmallDoT()
{
	if (!pTargetWnd) return false;
	if (!pTarget) return false;
	for (size_t x = 0; x < SmallDoTSpells.size(); x++)
	{
		if (TargetBuff((PCHAR)SmallDoTSpells[x].c_str(), true))
			return true;
	}
	return false;
}

bool CEnchanter::TargetHasManaDrainDebuff()
{
	if (!pTargetWnd) return false;
	if (!pTarget) return false;
	for (size_t x = 0; x < ManaDrainSpells.size(); x++)
	{
		if (TargetBuff((PCHAR)ManaDrainSpells[x].c_str(), true))
			return true;
	}
	return false;
}

bool CEnchanter::TargetHasTashDebuff()
{
	if (!pTargetWnd) return false;
	if (!pTarget) return false;
	for (size_t x = 0; x < TashSpells.size(); x++)
	{
		if (TargetBuff((PCHAR)TashSpells[x].c_str(), true))
			return true;
	}
	return false;
}

bool CEnchanter::TargetHasSlowDebuff()
{
	if (!pTargetWnd) return false;
	if (!pTarget) return false;
	for (size_t x = 0; x < SlowSpells.size(); x++)
	{
		if (TargetBuff((PCHAR)SlowSpells[x].c_str(), true))
			return true;
	}
	return false;
}

/*
VOID MemSpell(PSPAWNINFO pChar, PCHAR szLine)
{
	if (!ppSpellBookWnd) return;
	DWORD Favorite = (DWORD)&MemSpellFavorite;
	CHAR szGem[MAX_STRING] = { 0 };
	DWORD sp;
	WORD Gem = -1;
	CHAR SpellName[MAX_STRING] = { 0 };
	PCHARINFO pCharInfo = NULL;
	if (!pSpellBookWnd) return;
	if (NULL == (pCharInfo = GetCharInfo())) return;

	GetArg(szGem, szLine, 1);
	GetArg(SpellName, szLine, 2);
	Gem = atoi(szGem);
	if (Gem<1 || Gem>NUM_SPELL_GEMS) return;
	Gem--;

	GetCharInfo2()->SpellBook;

	if (!pSpell) return;
	if (pSpell->ClassLevel[pChar->Class]>pChar->Level) return;

	ZeroMemory(&MemSpellFavorite, sizeof(MemSpellFavorite));
	strcpy(MemSpellFavorite.Name, "Mem a Spell");
	MemSpellFavorite.inuse = 1;
	for (sp = 0; sp<NUM_SPELL_GEMS; sp++) MemSpellFavorite.SpellId[sp] = 0xFFFFFFFF;
	MemSpellFavorite.SpellId[Gem] = pSpell->ID;
	pSpellBookWnd->MemorizeSet((int*)Favorite, NUM_SPELL_GEMS);
}
*/

bool CEnchanter::MemorizeSpells()
{
	vector<string> SpellSet;
	PSPELL pSpell = 0;
	DWORD Favorite = (DWORD)&MemSpellFavorite;
	if (!pSpellBookWnd) return false;

	ZeroMemory(&MemSpellFavorite, sizeof(MemSpellFavorite));
	strcpy_s(MemSpellFavorite.Name, "WizardSpells");
	MemSpellFavorite.inuse = 1;
	if ((Level()==105) && (GetSpellByName("Dichotomic Fire") != NULL) && (GetSpellByName("Dichotomic Fire")!=NULL))
	SpellSet.push_back("Dichotomic Fire");
	SpellSet.push_back("Ethereal Fuse");
	SpellSet.push_back("Ethereal Skyblaze");
	SpellSet.push_back("Ethereal Rimeblast");
	SpellSet.push_back("Cloudburst Stormstrike");
	SpellSet.push_back("Shocking Vortex");
	SpellSet.push_back("Claw of the Flameweaver");
	SpellSet.push_back("Synapsefreeze");
	SpellSet.push_back("Wildflash Barrage");
	SpellSet.push_back("Twincast");
	SpellSet.push_back("Bucolic Gambit");
	SpellSet.push_back("Quiescent Harvest");

	for (int gem = 0; gem < NUM_SPELL_GEMS; gem++)
	{
		pSpell = 0;
		MemSpellFavorite.SpellId[gem] = 0xFFFFFFFF;
		for (DWORD N = 0; N < NUM_BOOK_SLOTS; N++)
		{
			if (PSPELL pTempSpell = GetSpellByID(GetCharInfo2()->SpellBook[N]))
			{
				// exact name match only
				if (!_strnicmp(SpellSet[gem].c_str(), pTempSpell->Name, SpellSet[gem].size()))
				{
					pSpell = pTempSpell;
					break;
				}
			}
		}
		if (pSpell) MemSpellFavorite.SpellId[gem] = pSpell->ID;
	}
	pSpellBookWnd->MemorizeSet((int*)Favorite, NUM_SPELL_GEMS);
	return true;
}