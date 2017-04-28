#include "Wizard.h"

CWizard::CWizard()
{
	EtherealWeaveSpells.push_back("Ethereal Fuse");
	EtherealWeaveSpells.push_back("Ethereal Weave");

	EtherealFireSpells.push_back("Ethereal Skyblaze");
	EtherealFireSpells.push_back("Ethereal Incandescence");
	EtherealFireSpells.push_back("Ethereal Blaze");
	EtherealFireSpells.push_back("Ethereal Inferno");
	EtherealFireSpells.push_back("Ethereal Combustion");
	EtherealFireSpells.push_back("Ethereal Incineration");
	EtherealFireSpells.push_back("Ethereal Conflagration");
	EtherealFireSpells.push_back("Ether Flame");

	EtherealIceSpells.push_back("Ethereal Rimeblast");
	EtherealIceSpells.push_back("Ethereal Hoarfrost");
	EtherealIceSpells.push_back("Ethereal Frost");
	EtherealIceSpells.push_back("Ethereal Glaciation");
	EtherealIceSpells.push_back("Ethereal Iceblight");
	EtherealIceSpells.push_back("Ethereal Rime");

	EtherealMagicSpells.push_back("Ethereal Flash");
	EtherealMagicSpells.push_back("Ethereal Salvo");
	EtherealMagicSpells.push_back("Ethereal Barrage");

	InstantMagicSpells.push_back("Cloudburst Stormstrike");
	InstantMagicSpells.push_back("Cloudburst Thunderbolt");
	InstantMagicSpells.push_back("Cloudburst Tempest");
	InstantMagicSpells.push_back("Cloudburst Storm");
	InstantMagicSpells.push_back("Cloudburst Levin");
	InstantMagicSpells.push_back("Cloudburst Bolts");
	InstantMagicSpells.push_back("Cloudburst Strike");

	InstantFireSpells.push_back("Flashchar");
	InstantFireSpells.push_back("Flashsear");
	InstantFireSpells.push_back("Flashflares");
	InstantFireSpells.push_back("Flashpyre");
	InstantFireSpells.push_back("Flashblaze");
	InstantFireSpells.push_back("Flashflames");
	InstantFireSpells.push_back("Flashfires");

	InstantIceSpells.push_back("Flashrime");
	InstantIceSpells.push_back("Flashfreeze");
	InstantIceSpells.push_back("Frost Snap");
	InstantIceSpells.push_back("Freezing Snap");
	InstantIceSpells.push_back("Gelid Snap");
	InstantIceSpells.push_back("Rime Snap");
	InstantIceSpells.push_back("Cold Snap");

	ClawMagicSpells.push_back("Claw of the Indagatori");
	ClawMagicSpells.push_back("Claw of the Ashwing");

	ClawFireSpells.push_back("Claw of the Flameweaver");
	ClawFireSpells.push_back("Claw of the Flamewing");

	ClawIceSpells.push_back("Claw of the Oceanlord");
	ClawIceSpells.push_back("Claw of the Icewing");

	SplashSpells.push_back("Splash of Pyroxene");
	SplashSpells.push_back("Splash of Phenocrysts");
	SplashSpells.push_back("Splash of Rhyolite");
	SplashSpells.push_back("Splash of Brimstone");

	FreeNukeAAs.push_back("Force of Flame");
	FreeNukeAAs.push_back("Force of Ice");
	FreeNukeAAs.push_back("Force of Will");
	//FreeNukeAAs.push_back("Banestrike");
	FreeNukeAAs.push_back("Concussive Intuition");

	DichotomicSpells.push_back("Dichotomic Fire");
	AllianceSpells.push_back("Frostbound Alliance");

	FastNukeSpells.push_back("Chaos Scintillation");
	FastNukeSpells.push_back("Wildflash Barrage");
	FastNukeSpells.push_back("Leap of Stormbolts");
	FastNukeSpells.push_back("Chaos Incandescence");
	FastNukeSpells.push_back("Wildether Barrage");
	FastNukeSpells.push_back("Chaos Blaze");
	FastNukeSpells.push_back("Wildspark Barrage");
	FastNukeSpells.push_back("Chaos Char");
	FastNukeSpells.push_back("Wildmana Barrage");
	FastNukeSpells.push_back("Chaos Combustion");
	FastNukeSpells.push_back("Wildmagic Blast");
	FastNukeSpells.push_back("Chaos Conflagration");
	FastNukeSpells.push_back("Wildmagic Burst");
	FastNukeSpells.push_back("Chaos Immolation");
	FastNukeSpells.push_back("Wildmagic Strike");
	FastNukeSpells.push_back("Chaos Flame");

	VortexSpells.push_back("Shocking Vortex");
	VortexSpells.push_back("Hoarfrost Vortex");
	VortexSpells.push_back("Ether Vortex");
	VortexSpells.push_back("Incandescent Vortex");
	VortexSpells.push_back("Frost Vortex");
	VortexSpells.push_back("Power Vortex");
	VortexSpells.push_back("Flame Vortex");
	VortexSpells.push_back("Ice Vortex");
	VortexSpells.push_back("Mana Vortex");
	VortexSpells.push_back("Fire Vortex");

	LowLevelNukes.push_back("Chaos Flame");		// Level 70 -- Chaos line of spells - very mana efficient - overlaps with high-level spell stuff
	LowLevelNukes.push_back("Spark of Ice");	// Level 68

	LowLevelNukes.push_back("Mana Weave");		// Level 69 -- Another Big Nuke
	LowLevelNukes.push_back("Gelidin Comet");	// Level 69 -- Another Ice Comet Upgrade
	LowLevelNukes.push_back("Claw of Vox");		// Level 69
	LowLevelNukes.push_back("Spark of Ice");	// Level 60

	LowLevelNukes.push_back("Thundaka");	// Level 68 -- Big Nuke category

	LowLevelNukes.push_back("Clinging Frost");	// Level 68 -- Snare component
	LowLevelNukes.push_back("Spark of Lightning");	// Level 68 - No Stun
	LowLevelNukes.push_back("Spark of Thunder");	// Level 68 - Stun
	LowLevelNukes.push_back("Spark of Fire");		// Level 66

													// Will probably move the SoS type spells to a "Big Nukes Category"
	LowLevelNukes.push_back("Ancient Strike of Chaos");	// Level 65 - Really Big Nuke
	LowLevelNukes.push_back("White Fire");	// Level 65 - Bigger Nuke
	LowLevelNukes.push_back("Strike of Solusek");	// Level 65 - Big Nuke

	LowLevelNukes.push_back("Shock of Magic");	// Level 65 -- First real "Chaos spell"

	LowLevelNukes.push_back("Ice Meteor");	// Level 64 -- Actual Ice Comet upgrade!
	LowLevelNukes.push_back("Draught of E'ci");	// Level 64

	LowLevelNukes.push_back("Draught of Lightning");// Level 63
	LowLevelNukes.push_back("Draught of Thunder");	// Level 63
	LowLevelNukes.push_back("Agnarr's Thunder");	// Level 63
	LowLevelNukes.push_back("Draught of Ro");	// Level 62
	LowLevelNukes.push_back("Claw of Frost");	// Level 61

	LowLevelNukes.push_back("Ancient Destruction of Ice");	// Level 60
	LowLevelNukes.push_back("Ice Spear of Solist");	// Level 60 -- Ice Comet upgrade-ish...
	LowLevelNukes.push_back("Ice Comet");		// Level 49 -- Best mana/damage nuke until higher levels 60+...

	LowLevelNukes.push_back("Sunstrike");	// Level 60
	LowLevelNukes.push_back("Inferno Shock");	// Level 26
	LowLevelNukes.push_back("Inferno Shock");	// Level 26
												//	LowLevelNukes.push_back("Porlo's Fury");	// Level 60 - Dragon Bane
												//	LowLevelNukes.push_back("Hsagra's Wrath");	// Level 60 - Giant Bane
	LowLevelNukes.push_back("Lure of Lightning");	// Level 58 -- Not sure if I want Lures here...
	LowLevelNukes.push_back("Draught of Ice");	// Level 57
	LowLevelNukes.push_back("Draught of Jiva");	// Level 55
	LowLevelNukes.push_back("Voltaic Draught");	// Level 54
	LowLevelNukes.push_back("Frozen Harpoon");	// Level 52
	LowLevelNukes.push_back("Draught of Fire");	// Level 51

	LowLevelNukes.push_back("Rend");			// Level 47
	LowLevelNukes.push_back("Enticement of Flame");	// Level 44
	LowLevelNukes.push_back("Conflagration");	// Level 43
	LowLevelNukes.push_back("Force Strike");	// Level 41
	LowLevelNukes.push_back("Lightning Shock");	// Level 37
	LowLevelNukes.push_back("Ice Shock");		// Level 34
	LowLevelNukes.push_back("Inferno Shock");	// Level 26
	LowLevelNukes.push_back("Thunder Strike");	// Level 28
	LowLevelNukes.push_back("Inferno Shock");	// Level 26
	LowLevelNukes.push_back("Frost Shock");		// Level 24
	LowLevelNukes.push_back("Garrison's Mighty Mana Shock"); // Level 18
	LowLevelNukes.push_back("Force Snap");		// Level 15
	LowLevelNukes.push_back("Lightning Bolt");	// Level 16
	LowLevelNukes.push_back("Flame Shock");		// Level 15
	LowLevelNukes.push_back("Shock of Lightning");	// Level 10
	LowLevelNukes.push_back("Shock of Ice");	// Level 8
	LowLevelNukes.push_back("Fire Bolt");		// Level 5
	LowLevelNukes.push_back("Shock of Fire");	// Level 4
	LowLevelNukes.push_back("Frost Bolt");		// Level 1
	LowLevelNukes.push_back("Blast of Cold");	// Level 1
}

CWizard::~CWizard()
{
}

bool CWizard::LowLevelNukeReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, LowLevelNukes, Distance);
}

bool CWizard::EtherealWeaveReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, EtherealWeaveSpells, Distance);
}

bool CWizard::FireEtherealReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, EtherealFireSpells, Distance);
}

bool CWizard::IceEtherealReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, EtherealIceSpells, Distance);
}

bool CWizard::MagicEtherealReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, EtherealMagicSpells, Distance);
}

bool CWizard::FreeNukeReady(string &NukeSpell, float Distance)
{
	return AATypeReady(NukeSpell, FreeNukeAAs);
}

bool CWizard::IceClawReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, ClawIceSpells, Distance);
}

bool CWizard::FireClawReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, ClawFireSpells, Distance);
}

bool CWizard::MagicClawReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, ClawMagicSpells, Distance);
}

bool CWizard::IceInstantReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, InstantIceSpells, Distance);
}

bool CWizard::FireInstantReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, InstantFireSpells, Distance);
}

bool CWizard::MagicInstantReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, InstantMagicSpells, Distance);
}

bool CWizard::FastNukeReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, FastNukeSpells, Distance);
}

bool CWizard::DichotomicNukeReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, DichotomicSpells, Distance);
}

bool CWizard::AllianceSpellReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, AllianceSpells, Distance);
}

bool CWizard::VortexNukeReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, VortexSpells, Distance);
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

bool CWizard::MemorizeSpells()
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
	SpellSet.push_back("Hoarfrost Vortex");
	SpellSet.push_back("Frostbound Alliance");
	SpellSet.push_back("Twincast");
	SpellSet.push_back("Quiescent Gambit");
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