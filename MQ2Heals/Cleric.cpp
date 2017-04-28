#include "Cleric.h"

CCleric::CCleric()
{
	WardSpells.push_back("Ward of Surety");		//Level 102
	WardSpells.push_back("Ward of Certitude");	//Level  97

	FastHeals.push_back("Spiritual Remedy");	// Level 101
	FastHeals.push_back("Graceful Remedy");	// Level 96
	FastHeals.push_back("Faithful Remedy");	// Level 91
	FastHeals.push_back("Earnest Remedy");	// Level 86
	FastHeals.push_back("Devout Remedy");	// Level 81
	FastHeals.push_back("Solemn Remedy");	// Level 76
	FastHeals.push_back("Sacred Remedy");	// Level 71
	FastHeals.push_back("Pious Remedy");		// Level 66
	FastHeals.push_back("Supernal Remedy");	// Level 61
	FastHeals.push_back("Ethereal Remedy");	// Level 59
	FastHeals.push_back("Remedy");			// Level 51

	GroupHeals.push_back("Word of Greater Reformation");	// Level 105
	GroupHeals.push_back("Syllable of Convalescence");	// Level 103
	GroupHeals.push_back("Word of Convalescence");		// Level 101
	GroupHeals.push_back("Word of Reformation");			// Level 100
	GroupHeals.push_back("Syllable of Renewal");			// Level  98
	GroupHeals.push_back("Word of Renewal");				// Level  96
	GroupHeals.push_back("Word of Rehabilitation");		// Level  95
	GroupHeals.push_back("Word of Recuperation");			// Level  91
	GroupHeals.push_back("Word of Resurgence");			// Level  90
	GroupHeals.push_back("Word of Awakening");			// Level  86
	GroupHeals.push_back("Word of Recovery");				// Level  85
	GroupHeals.push_back("Word of Vivacity");				// Level  80
	GroupHeals.push_back("Word of Vivification");			// Level  69
	GroupHeals.push_back("Word of Replenishment");		// Level  64
	GroupHeals.push_back("Word of Redemption");			// Level  60
	GroupHeals.push_back("Word of Restoration");			// Level  57
	GroupHeals.push_back("Word of Vigor");				// Level  52
	GroupHeals.push_back("Word of Healing");				// Level  45
	GroupHeals.push_back("Word of Health");				// Level  30

	PatchHeals.push_back("Undying Life");		// Level  101
	PatchHeals.push_back("Fifteenth Emblem");		// Level  97
	PatchHeals.push_back("Fourteenth Catalyst");	// Level  92
	PatchHeals.push_back("Thirteenth Salve");		// Level  87
	PatchHeals.push_back("Eleventh-Hour");		// Level  77

	YaulpSpells.push_back("Yaulp XIV");	//Level 101
	YaulpSpells.push_back("Yaulp XIII");	//Level  96
	YaulpSpells.push_back("Yaulp XII");	//Level  91
	YaulpSpells.push_back("Yaulp XI");		//Level  86
	YaulpSpells.push_back("Yaulp X");		//Level  81
	YaulpSpells.push_back("Yaulp IX");		//Level  76
	YaulpSpells.push_back("Yaulp VIII");	//Level  71
	YaulpSpells.push_back("Yaulp VII");	//Level  69
	YaulpSpells.push_back("Yaulp VI");		//Level  65
	YaulpSpells.push_back("Yaulp V");		//Level  56
	YaulpSpells.push_back("Yaulp IV");		//Level  53
	YaulpSpells.push_back("Yaulp III");	//Level  41
	YaulpSpells.push_back("Yaulp II");		//Level  16
	YaulpSpells.push_back("Yaulp I");		//Level   1

	BigHeals.push_back("Fervid Renewal");	// Level 105
	BigHeals.push_back("Fraught Renewal");	// Level 100
	BigHeals.push_back("Fervent Renewal");	// Level  95
	BigHeals.push_back("Frenzied Renewal");	// Level  90
	BigHeals.push_back("Frenetic Renewal");	// Level  80
	BigHeals.push_back("Frantic Renewal");	// Level  75
	BigHeals.push_back("Desperate Renewal");	// Level  70

	HealNukes.push_back("Mystical Intervention");	// Level 103
	HealNukes.push_back("Virtuois Intervention");	// Level  98
	HealNukes.push_back("Elysian Intervention");		// Level  93
	HealNukes.push_back("Celestial Intervention");	// Level  88
	HealNukes.push_back("Holy Intervention");			// Level  83

	NukeHeals.push_back("Ardent Contravention");		// Level 105
	NukeHeals.push_back("Virtuois Contravention");	// Level 100
	NukeHeals.push_back("Elysian Contravention");	// Level  95
	NukeHeals.push_back("Celestial Contravention");	// Level  90
	NukeHeals.push_back("Holy Intervention");		// Level  85

	SlowHeals.push_back("Ardent Light");				// Level 103
	SlowHeals.push_back("Reverent Light");			// Level  98
	SlowHeals.push_back("Zealous Light");			// Level  93
	SlowHeals.push_back("Earnest Light");			// Level  88
	SlowHeals.push_back("Devout Light");				// Level  83
	SlowHeals.push_back("Solemn Light");				// Level  78
	SlowHeals.push_back("Sacred Light");				// Level  73
	SlowHeals.push_back("Ancient Hallowed Light");	// Level  70
	SlowHeals.push_back("Pious Light");				// Level  68
	SlowHeals.push_back("Holy Light");				// Level  65
	SlowHeals.push_back("Superior Healing");			// CLR/30 PAL/57 DRU/51 SHM/51
	SlowHeals.push_back("Greater Healing");			// CLR/20 PAL/36 RNG/57 DRU/29 SHM/29 BST/57
	SlowHeals.push_back("Healing");					// CLR/10 PAL/27 RNG/38 DRU/19 SHM/19 BST/36
	SlowHeals.push_back("Light Healing");			// CLR/4  PAL/12 RNG/21 DRU/9  SHM/9  BST/20
	SlowHeals.push_back("Minor Healing");			// CLR/1  PAL/6  RNG/8  DRU/1  SHM/1  BST/6

	DelayedHeals.push_back("Promised Rehabilitation");	// Level 103
	DelayedHeals.push_back("Promised Reformation");		// Level  98
	DelayedHeals.push_back("Promised Restitution");		// Level  93
	DelayedHeals.push_back("Promised Resurgence");		// Level  88
	DelayedHeals.push_back("Promised Recuperation");	// Level  83
	DelayedHeals.push_back("Promised Restoration");		// Level  78
	DelayedHeals.push_back("Promised Renewal");			// Level  73
	DelayedHeals.push_back("Promised Regeneration");	// Druid Level 102
	DelayedHeals.push_back("Promised Recovery");		// Druid Level  97
	DelayedHeals.push_back("Promised Revitalization");	// Druid Level  92
	DelayedHeals.push_back("Promised Replenishment");	// Druid Level  87
	DelayedHeals.push_back("Promised Reknit");			// Druid Level  82
}

bool CCleric::WardSpellReady(string &HealSpell, float Range)
{
	return SpellTypeReady(HealSpell, WardSpells, Range);
}

bool CCleric::YaulpSpellReady(string &HealSpell)
{
	return SpellTypeReady(HealSpell, YaulpSpells);
}
