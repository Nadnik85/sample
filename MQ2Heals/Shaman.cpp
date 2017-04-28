#include "Shaman.h"

CShaman::CShaman()
{
	GroupHeals.push_back("Historian's Intervention");		// Shaman Level 103 - Fast Group Heal
	GroupHeals.push_back("Antecessor's Intervention");	// Shaman Level  98 - Fast Group Heal
	GroupHeals.push_back("Progenitor's Intervention");	// Shaman Level  93 - Fast Group Heal
	GroupHeals.push_back("Ascendant's Intervention");		// Shaman Level  88 - Fast Group Heal
	GroupHeals.push_back("Antecedent's Intervention");	// Shaman Level  83 - Fast Group Heal
	GroupHeals.push_back("Ancestral's Intervention");		// Shaman Level  78 - Fast Group Heal
	GroupHeals.push_back("Krasir's Recourse");			// Shaman Level 102 - 1s Group Heal
	GroupHeals.push_back("Blezon's Recourse");			// Shaman Level  97 - 1s Group Heal
	GroupHeals.push_back("Gotikan's Recourse");			// Shaman Level  92 - 1s Group Heal
	GroupHeals.push_back("Qirik's Recourse");				// Shaman Level  87 - 1s Group Heal

	BigHeals.push_back("Reckless Regeneration");	// Shaman 105
	BigHeals.push_back("Reckless Restoration");		// Shaman 100
	BigHeals.push_back("Reckless Remedy");			// Shaman  95
	BigHeals.push_back("Reckless Mending");			// Shaman  90

	SlowHeals.push_back("Krasir's Mending");			// Shaman Level 103
	SlowHeals.push_back("Blezon's Mending");			// Shaman Level  98
	SlowHeals.push_back("Gotikan's Mending");		// Shaman Level  93
	SlowHeals.push_back("Qirik's Mending");			// Shaman Level  88
	SlowHeals.push_back("Dannal's Mending");			// Shaman Level  83
	SlowHeals.push_back("Gemmi's Mending");			// Shaman Level  78
	SlowHeals.push_back("Ahnkaul's Mending");		// Shaman Level  73
	SlowHeals.push_back("Ancient Wilslik's Mending");// Shaman Level  70
	SlowHeals.push_back("Yoppa's Mending");			// Shaman Level  68
	SlowHeals.push_back("Daluda's Mending");			// Shaman Level  65
	SlowHeals.push_back("Tnarg's Mending");			// Shaman Level  62
	SlowHeals.push_back("Kragg's Mending");			// Shaman Level  58	// 10s - CH Type

	// Shared/Low Level Spells...
	SlowHeals.push_back("Chloroblast");				// Druid Level  55 / Shaman 55 / Ranger & Beastlord 62
	SlowHeals.push_back("Superior Healing");		// CLR/30 PAL/57 DRU/51 SHM/51
	SlowHeals.push_back("Greater Healing");			// CLR/20 PAL/36 RNG/57 DRU/29 SHM/29 BST/57
	SlowHeals.push_back("Healing");					// CLR/10 PAL/27 RNG/38 DRU/19 SHM/19 BST/36
	SlowHeals.push_back("Light Healing");			// CLR/4  PAL/12 RNG/21 DRU/9  SHM/9  BST/20
	SlowHeals.push_back("Minor Healing");			// CLR/1  PAL/6  RNG/8  DRU/1  SHM/1  BST/6

//	"Ferociuos Growth");		// Level SHM 101 / DRU 102
//	"Rampant Growth");		// Level SHM 96 / DRU 97
//	"Unfettered Growth");	// Level SHM 91 / DRU 92
//	"Untamed Growth");		// Level SHM 86 / DRU 87
//	"Wild Growth");			// Level SHM 81 / DRU 82
}

CShaman::~CShaman()
{
}
