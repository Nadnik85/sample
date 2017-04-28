#include "Druid.h"

CDruid::CDruid()
{
	FastHeals.push_back("Adrenaline Torrent");	// Druid Level 104
	FastHeals.push_back("Adrenaline Rush");		// Druid Level 100
	FastHeals.push_back("Adrenaline Flood");		// Druid Level  95
	FastHeals.push_back("Adrenaline Blast");		// Druid Level  90
	FastHeals.push_back("Adrenaline Burst");		// Druid Level  85
	FastHeals.push_back("Adrenaline Swell");		// Druid Level  80
	FastHeals.push_back("Adrenaline Surge");		// Druid Level  75
	FastHeals.push_back("Ancient Chlorobon");	// Druid Level  70
	FastHeals.push_back("Karana's Renewal");		// Druid Level  64
	FastHeals.push_back("Nature's Touch");		// Druid Level  60

	GroupHeals.push_back("Survival of the Serendipitous");	// Druid Level 104 Fast Group Heal
	GroupHeals.push_back("Lunasoothe");						// Druid Level 105 Slow Group Heal
	GroupHeals.push_back("Survival of the Fortuitous");		// Druid Level  98 Fast Group Heal
	GroupHeals.push_back("Lunassuage");						// Druid Level 100 Slow Group Heal
	GroupHeals.push_back("Survival of the Prosperous");		// Druid Level  93 Fast Group Heal
	GroupHeals.push_back("Lunalleviation");					// Druid Level  95 Slow Group Heal
	GroupHeals.push_back("Survival of the Propitious");		// Druid Level  88 Fast Group Heal
	GroupHeals.push_back("Lunamelioration");					// Druid Level  90 Slow Group Heal
	GroupHeals.push_back("Survival of the Felicitous");		// Druid Level  83 Fast Group Heal
	GroupHeals.push_back("Lunulation");						// Druid Level  85 Slow Group Heal
	GroupHeals.push_back("Survival of the Fittest");			// Druid Level  78 Fast Group Heal
	GroupHeals.push_back("Crescentbloom");					// Druid Level  80 Slow Group Heal
	GroupHeals.push_back("Survival of the Fittest");			// Druid Level  78 Fast Group Heal
	GroupHeals.push_back("Lunarlight");						// Druid Level  75 Slow Group Heal
	GroupHeals.push_back("Moonshadow");						// Druid Level  70 Slow Group Heal

	BigHeals.push_back("Resurgence");		// Druid 105
	BigHeals.push_back("Vivification");		// Druid 100
	BigHeals.push_back("Invigoration");		// Druid  95
	BigHeals.push_back("Rejuvilation");		// Druid  90

	SlowHeals.push_back("Panavida");					// Druid Level 102
	SlowHeals.push_back("Sterivida");				// Druid Level  97
	SlowHeals.push_back("Sanavida");					// Druid Level  92
	SlowHeals.push_back("Benevida");					// Druid Level  87
	SlowHeals.push_back("Granvida");					// Druid Level  82
	SlowHeals.push_back("Puravida");					// Druid Level  77
	SlowHeals.push_back("Pure Life");				// Druid Level  72
	SlowHeals.push_back("Chlorotrope");				// Druid Level  68
	SlowHeals.push_back("Sylvan Infusion");			// Druid Level  65
	SlowHeals.push_back("Nature's Infusion");		// Druid Level  63
	SlowHeals.push_back("Nature's Touch");			// Druid Level  60
	SlowHeals.push_back("Chloroblast");				// Druid Level  55 / Shaman 55 / Ranger & Beastlord 62
	SlowHeals.push_back("Superior Healing");			// CLR/30 PAL/57 DRU/51 SHM/51
	SlowHeals.push_back("Healing Water");			// DRU/44
	SlowHeals.push_back("Greater Healing");			// CLR/20 PAL/36 RNG/57 DRU/29 SHM/29 BST/57
	SlowHeals.push_back("Healing");					// CLR/10 PAL/27 RNG/38 DRU/19 SHM/19 BST/36
	SlowHeals.push_back("Light Healing");			// CLR/4  PAL/12 RNG/21 DRU/9  SHM/9  BST/20
	SlowHeals.push_back("Minor Healing");			// CLR/1  PAL/6  RNG/8  DRU/1  SHM/1  BST/6

	DelayedHeals.push_back("Promised Regeneration");	// Druid Level 102
	DelayedHeals.push_back("Promised Recovery");		// Druid Level  97
	DelayedHeals.push_back("Promised Revitalization");	// Druid Level  92
	DelayedHeals.push_back("Promised Replenishment");	// Druid Level  87
	DelayedHeals.push_back("Promised Reknit");			// Druid Level  82

	DichotomicSpells.push_back("Dichotomic Winds");

	RemoteFireSpells.push_back("Remote Sunflash");
	RemoteFireSpells.push_back("Remote Sunfire");
	RemoteFireSpells.push_back("Remote Sunflare");
	RemoteFireSpells.push_back("Remote Sunburst");
	RemoteFireSpells.push_back("Remote Manaflux");

	StunNukeSpells.push_back("Anabatic Roar");
	StunNukeSpells.push_back("Katabatic Roar");
	StunNukeSpells.push_back("Roar of Kolos");

	FireWrathSpells.push_back("Nature's Fiery Wrath");
	FireWrathSpells.push_back("Nature's Withering Wrath");
	FireWrathSpells.push_back("Nature's Scorching Wrath");
	FireWrathSpells.push_back("Nature's Incinerating Wrath");
	FireWrathSpells.push_back("Nature's Searing Wrath");
	FireWrathSpells.push_back("Nature's Burning Wrath");
	FireWrathSpells.push_back("Nature's Blazing Wrath");

	SkinDebuffSpells.push_back("Skin to Seedlings");
	SkinDebuffSpells.push_back("Skin to Foliage");
	SkinDebuffSpells.push_back("Skin to Leaves");
	SkinDebuffSpells.push_back("Skin to Flora");
	SkinDebuffSpells.push_back("Skin to Mulch");
	SkinDebuffSpells.push_back("Skin to Vines");

	WinterFireSpells.push_back("Winter's Wildflame");
	WinterFireSpells.push_back("Winter's Wildfire");
	WinterFireSpells.push_back("Winter's Sear");
	WinterFireSpells.push_back("Winter's Pyre");
	WinterFireSpells.push_back("Winter's Flare");
	WinterFireSpells.push_back("Winter's Blaze");
	WinterFireSpells.push_back("Winter's Flame");
	WinterFireSpells.push_back("Winter's Frost");

	FreeNukeSpells.push_back("Fire Storm");
	FreeNukeSpells.push_back("Storm Strike");
}

CDruid::~CDruid()
{
}

bool CDruid::DichotomicNukeReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, DichotomicSpells, Distance);
}

bool CDruid::WinterNukeReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, WinterFireSpells, Distance);
}

bool CDruid::StunNukeReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, StunNukeSpells, Distance);
}

bool CDruid::WrathNukeReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, FireWrathSpells, Distance);
}

bool CDruid::RemoteNukeReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, RemoteFireSpells, Distance);
}

bool CDruid::SkinDebuffReady(string &NukeSpell, float Distance)
{
	return SpellTypeReady(NukeSpell, SkinDebuffSpells, Distance);
}

bool CDruid::FreeNukeReady(string &NukeSpell, float Distance)
{
	return AATypeReady(NukeSpell, FreeNukeSpells, Distance);
}
