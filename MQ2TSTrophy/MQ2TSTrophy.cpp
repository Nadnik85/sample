/**
	MQ2TSTrophy by Sic & CWTN; a RedGuides Exclusive
	Purpose: Equipping your Tradeskill Trophies when opening an environmental crafting container.
	Actions: This plugin will see what you already have in your ammo slot,
		then swap out to the correct trophy for the environmental container you have open
		and then when you leave the environmental window, it will swap back to what you already had equipped.
**/
#include "../MQ2Plugin.h"

PreSetup("MQ2TSTrophy");
PLUGIN_VERSION(2.1);

bool WorldContainerCheck();
bool InGame();
void UpdateTrophyGroup(CONTENTS** group, const std::vector<std::string>& itemList, const char* groupname);
void UpdateTrophies();
void UpdateTrophy(CONTENTS** TrophyVariable, const char* item, const char* Typename);
void SwapSlot(CONTENTS** Trophy, const char* slot);
void PluginOn();

CONTENTS* FindSlotCurrent(const char* slot);
CONTENTS* Cursor();

CONTENTS* BakingTrophy = 0;
CONTENTS* AlchemistTrophy = 0;
CONTENTS* BrewingTrophy = 0;
CONTENTS* FletchingTrophy = 0;
CONTENTS* JewelerTrophy = 0;
CONTENTS* TinkeringTrophy = 0;
CONTENTS* PotteryTrophy = 0;
CONTENTS* ResearchTrophy = 0;
CONTENTS* BlacksmithTrophy = 0;
CONTENTS* TailorTrophy = 0;
CONTENTS* PoisonTrophy = 0;
CONTENTS* FishingTrophy = 0;

bool bActivated = false;
bool containerfound = false;
bool bSilent = false;
bool bMatches = false;
char szContainerName[128] = { 0 };
int iStep = 1;
int iPulse = 0;

class MQ2TrophyType : public MQ2Type
{
public:
	enum TrophyMembers
	{
		Version = 1,
		Matches = 2,
		Alchemy = 3,
		Baking = 4,
		Brewing = 5,
		Fletching = 6,
		Jewelry = 7,
		Tinkering = 8,
		Pottery = 9,
		Research = 10,
		Blacksmithing = 11,
		Tailoring = 12,
		Poison = 13,
		Fishing = 14,
		Container = 15,
	};

	MQ2TrophyType() : MQ2Type("Trophy") {
		TypeMember(Version); // 1,Float
		TypeMember(Matches); // 2, bool
		TypeMember(Alchemy); // 3, string
		TypeMember(Baking); // 4, string
		TypeMember(Brewing); // 5, string
		TypeMember(Fletching); // 6, string
		TypeMember(Jewelry); // 7, string
		TypeMember(Tinkering); // 8, string
		TypeMember(Pottery); // 9, string
		TypeMember(Research); // 10, string
		TypeMember(Blacksmithing); // 11, string
		TypeMember(Tailoring); // 12, string
		TypeMember(Poison); // 13, string
		TypeMember(Fishing); // 14, string
		TypeMember(Container); // 15, bool
	};

	~MQ2TrophyType()
	{
	}

	bool GetMember(MQ2VARPTR VarPtr, char* Member, char* Index, MQ2TYPEVAR& Dest)
	{
		PMQ2TYPEMEMBER pMember = MQ2TrophyType::FindMember(Member);
		if (!pMember)
			return false;
		if (!pLocalPlayer)
			return false;
		switch ((TrophyMembers)pMember->ID)
		{
		case Version:
			Dest.Float = MQ2Version;
			Dest.Type = pFloatType;
			return true;
		case Matches: // this will return true/false if our equipped tradeskill trophy matches the world/enviro container we are in
			Dest.DWord = bMatches;
			Dest.Type = pBoolType;
			return true;
		case Alchemy:
			if (AlchemistTrophy) {
				Dest.Ptr = GetItemFromContents(AlchemistTrophy)->Name;
				Dest.Type = pStringType;
				return true;
			}
			return false;
		case Baking:
			if (BakingTrophy) {
				Dest.Ptr = GetItemFromContents(BakingTrophy)->Name;
				Dest.Type = pStringType;
				return true;
			}
			return false;
		case Brewing:
			if (BrewingTrophy) {
				Dest.Ptr = GetItemFromContents(BrewingTrophy)->Name;
				Dest.Type = pStringType;
				return true;
			}
			return false;
		case Fletching:
			if (FletchingTrophy) {
				Dest.Ptr = GetItemFromContents(FletchingTrophy)->Name;
				Dest.Type = pStringType;
				return true;
			}
			return false;
		case Jewelry:
			if (JewelerTrophy) {
				Dest.Ptr = GetItemFromContents(JewelerTrophy)->Name;
				Dest.Type = pStringType;
				return true;
			}
			return false;
		case Tinkering:
			if (TinkeringTrophy) {
				Dest.Ptr = GetItemFromContents(TinkeringTrophy)->Name;
				Dest.Type = pStringType;
				return true;
			}
			return false;
		case Pottery:
			if (PotteryTrophy) {
				Dest.Ptr = GetItemFromContents(PotteryTrophy)->Name;
				Dest.Type = pStringType;
				return true;
			}
			return false;
		case Research:
			if (ResearchTrophy) {
				Dest.Ptr = GetItemFromContents(ResearchTrophy)->Name;
				Dest.Type = pStringType;
				return true;
			}
			return false;
		case Blacksmithing:
			if (BlacksmithTrophy) {
				Dest.Ptr = GetItemFromContents(BlacksmithTrophy)->Name;
				Dest.Type = pStringType;
				return true;
			}
			return false;
		case Tailoring:
			if (TailorTrophy) {
				Dest.Ptr = GetItemFromContents(TailorTrophy)->Name;
				Dest.Type = pStringType;
				return true;
			}
			return false;
		case Poison:
			if (PoisonTrophy) {
				Dest.Ptr = GetItemFromContents(PoisonTrophy)->Name;
				Dest.Type = pStringType;
				return true;
			}
			return false;
		case Fishing:
			if (FishingTrophy) {
				Dest.Ptr = GetItemFromContents(FishingTrophy)->Name;
				Dest.Type = pStringType;
				return true;
			}
			return false;
		case Container: // this returns true/false if we see a world/enviro tradeskill container
			Dest.DWord = containerfound;
			Dest.Type = pBoolType;
			return true;
		default:
			return false;
			break;
		}
	}

	bool ToString(MQ2VARPTR VarPtr, char* Destination)
	{
		return true;
	}

	bool FromData(MQ2VARPTR& VarPtr, MQ2TYPEVAR& Source)
	{
		return false;
	}

	bool FromString(MQ2VARPTR& VarPtr, char* Source)
	{
		return false;
	}
};

MQ2TrophyType* pTrophyType = nullptr;

BOOL TrophyData(char* szIndex, MQ2TYPEVAR& Dest)
{
	Dest.DWord = 1;
	Dest.Type = pTrophyType;
	return true;
}

PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("Initializing MQ2TSTrophy");
	AddMQ2Data("Trophy", TrophyData);
	pTrophyType = new MQ2TrophyType;
}

PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("Shutting down MQ2TSTrophy");
	RemoveMQ2Data("Trophy");
	delete pTrophyType;
}

PLUGIN_API int OnIncomingChat(const char* pLine, unsigned long Color)
{
	if (!strncmp(pLine, "Your ", 5) && strstr(pLine, " Trophy has evolved!")) { // update Trophies
		UpdateTrophies();
	}
	return 0;
}

PLUGIN_API void OnPulse()
{
	int iPulseDelay = 20;
	if (!InGame()) return;
	if (++iPulse < iPulseDelay) return;
	iPulse = 0;
	if (!bActivated) PluginOn();

	WorldContainerCheck();

	if (iStep == 3) {
		if (ItemOnCursor()) {
			EzCommand("/autoinv");
		}
		iStep = 4;
	}

	if (containerfound) {
		// We are updating trophies anytime we open a container, in case someone put their trophy in the bank, or deleted it, or it changed between last time it was used
		// This is understood it might not be the best fix, but it was a good short-term solution
		UpdateTrophies();
		// We are now doing a FindSlotCurrent to check ranged and ammo slots, in case you already have it equipped in another slot
		if (strstr(szContainerName, "Alchemy Table") && (FindSlotCurrent("ammo") != AlchemistTrophy && FindSlotCurrent("range") != AlchemistTrophy)) {
			//equip alchemy trophy
			SwapSlot(&AlchemistTrophy, "ammo");
		}
		else if (strstr(szContainerName, "Mixing Bowl") || strstr(szContainerName, "Oven") || strstr(szContainerName, "Ice Cream") && (FindSlotCurrent("ammo") != BakingTrophy && FindSlotCurrent("range") != BakingTrophy)) {
			//equip Baking trophy
			SwapSlot(&BakingTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Brewing Barrel") && (FindSlotCurrent("ammo") != BrewingTrophy && FindSlotCurrent("range") != BrewingTrophy)) {
			//equip Brewing trophy
			SwapSlot(&BrewingTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Fletching Table") && (FindSlotCurrent("ammo") != FletchingTrophy && FindSlotCurrent("range") != FletchingTrophy)) {
			//equip Fletching trophy
			SwapSlot(&FletchingTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Jewelry Making Table") && (FindSlotCurrent("ammo") != JewelerTrophy && FindSlotCurrent("range") != JewelerTrophy)) {
			//equip Jewelry trophy
			SwapSlot(&JewelerTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Fly Making Bench") && (FindSlotCurrent("mainhand") != FishingTrophy && FindSlotCurrent("ammo") != FishingTrophy && FindSlotCurrent("range") != FishingTrophy)) {
			//equip Fishing trophy
			SwapSlot(&FishingTrophy,"mainhand");
		}
		else if (strstr(szContainerName, "Kiln") || strstr(szContainerName, "Pottery Wheel") && (FindSlotCurrent("ammo") != PotteryTrophy && FindSlotCurrent("range") != PotteryTrophy)) {
			//equip Pottery trophy
			SwapSlot(&PotteryTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Poisoncrafting Table") && (FindSlotCurrent("ammo") != PoisonTrophy && FindSlotCurrent("range") != PoisonTrophy)) {
			//equip Poison making trophy
			SwapSlot(&PoisonTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Forge") && (FindSlotCurrent("ammo") != BlacksmithTrophy && FindSlotCurrent("range") != BlacksmithTrophy)) {
			//equip Smithing trophy
			SwapSlot(&BlacksmithTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Spell Research Table") && (FindSlotCurrent("ammo") != ResearchTrophy && FindSlotCurrent("range") != ResearchTrophy)) {
			//equip research trophy
			SwapSlot(&ResearchTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Loom") && (FindSlotCurrent("ammo") != TailorTrophy && FindSlotCurrent("range") != TailorTrophy)) {
			//equip Tailoring trophy
			SwapSlot(&TailorTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Tinkering") && (FindSlotCurrent("ammo") != TinkeringTrophy && FindSlotCurrent("range") != TinkeringTrophy)) {
			//equip Tinkering trophy
			SwapSlot(&TinkeringTrophy,"ammo");
		}
		else {
			containerfound = false;
		}

		if (iStep == 4) {
			iStep = 1;
		}
	}
}

PLUGIN_API void OnZoned()
{
	if (!InGame()) return;
	UpdateTrophies();
	iStep = 1;
}

void PluginOn()
{
	if (!bActivated) bActivated = true;
	UpdateTrophies();
	bSilent = true;
}

void UpdateTrophies()
{
	BakingTrophy = nullptr;
	AlchemistTrophy = nullptr;
	BrewingTrophy = nullptr;
	FletchingTrophy = nullptr;
	JewelerTrophy = nullptr;
	TinkeringTrophy = nullptr;
	PotteryTrophy = nullptr;
	ResearchTrophy = nullptr;
	BlacksmithTrophy = nullptr;
	TailorTrophy = nullptr;
	PoisonTrophy = nullptr;
	FishingTrophy = nullptr;

	//Alchemist Trophy
	std::vector<std::string> itemList = {
		"Mundunugu Medicine Stick",
		"Master Alchemist Trophy",
		"Expert Alchemist Trophy",
		"Journeyman Alchemist Trophy",
		"Freshman Alchemist Trophy",
		"Apprentice Alchemist Trophy",
		"Beginner Alchemist Trophy"
	};
	UpdateTrophyGroup(&AlchemistTrophy, itemList, "Alchemy");

	//Baking Trophy - will return with ${Trophy.Baking}
	itemList = {
		"Denmother's Rolling Pin",
		"Master Baker Trophy",
		"Expert Baker Trophy",
		"Journeyman Baker Trophy",
		"Freshman Baker Trophy",
		"Apprentice Baker Trophy",
		"Beginner Baker Trophy"
	};
	UpdateTrophyGroup(&BakingTrophy, itemList, "Baking");

	//Brewer Trophy - will return with ${Trophy.Brewing}
	itemList = {
		"Brewmaster's Mug",
		"Master Brewer Trophy",
		"Expert Brewer Trophy",
		"Journeyman Brewer Trophy",
		"Freshman Brewer Trophy",
		"Apprentice Brewer Trophy",
		"Beginner Brewer Trophy"
	};
	UpdateTrophyGroup(&BrewingTrophy, itemList, "Brewing");

	//Fletching Trophy - will return with ${Trophy.Fletching}
	itemList = {
		"Fletcher's Arrow",
		"Master Fletcher Trophy",
		"Expert Fletcher Trophy",
		"Journeyman Fletcher Trophy",
		"Freshman Fletcher Trophy",
		"Apprentice Fletcher Trophy",
		"Beginner Fletcher Trophy"
	};
	UpdateTrophyGroup(&FletchingTrophy, itemList, "Fletching");

	//Jeweler Trophy - will return with ${Trophy.Jewelry}
	itemList = {
		"Intricate Jewelers Glass",
		"Master Jeweler Trophy",
		"Expert Jeweler Trophy",
		"Journeyman Jeweler Trophy",
		"Freshman Jeweler Trophy",
		"Apprentice Jeweler Trophy",
		"Beginner Jeweler Trophy"
	};
	UpdateTrophyGroup(&JewelerTrophy, itemList, "Jewelry");

	//Tinkering Trophy - will return with ${Trophy.Tinkering}
	itemList = {
		"Hovering Contraption",
		"Master Mechanist Trophy",
		"Expert Mechanist Trophy",
		"Journeyman Mechanist Trophy",
		"Freshman Mechanist Trophy",
		"Apprentice Mechanist Trophy",
		"Beginner Mechanist Trophy"
	};
	UpdateTrophyGroup(&TinkeringTrophy, itemList, "Tinkering");

	//Pottery Trophy - will return with ${Trophy.Pottery}
	itemList = {
		"Clay Flinger's Loop",
		"Master Potter Trophy",
		"Expert Potter Trophy",
		"Journeyman Potter Trophy",
		"Freshman Potter Trophy",
		"Apprentice Potter Trophy",
		"Beginner Potter Trophy"
	};
	UpdateTrophyGroup(&PotteryTrophy, itemList, "Pottery");

	//Research Trophy - will return with ${Trophy.Research}
	itemList = {
		"Ethereal Quill",
		"Master Researcher Trophy",
		"Expert Researcher Trophy",
		"Journeyman Researcher Trophy",
		"Freshman Researcher Trophy",
		"Apprentice Researcher Trophy",
		"Beginner Researcher Trophy"
	};
	UpdateTrophyGroup(&ResearchTrophy, itemList, "Research");

	//Blacksmithing Trophy - will return with ${Trophy.Blacksmithing}
	itemList = {
		"Blacksmith's Adamantine Hammer",
		"Master Smith Trophy",
		"Expert Smith Trophy",
		"Journeyman Smith Trophy",
		"Freshman Smith Trophy",
		"Apprentice Smith Trophy",
		"Beginner Smith Trophy"
	};
	UpdateTrophyGroup(&BlacksmithTrophy, itemList, "Blacksmithing");

	//Tailor Trophy - will return with ${Trophy.Tailoring}
	itemList = {
		"Mystical Bolt",
		"Master Tailor Trophy",
		"Expert Tailor Trophy",
		"Journeyman Tailor Trophy",
		"Freshman Tailor Trophy",
		"Apprentice Tailor Trophy",
		"Beginner Tailor Trophy"
	};
	UpdateTrophyGroup(&TailorTrophy, itemList, "Tailoring");

	//Poison Trophy - will return with ${Trophy.Poison}
	itemList = {
		"Peerless Pestle",
		"Master Toxicologist Trophy",
		"Expert Toxicologist Trophy",
		"Journeyman Toxicologist Trophy",
		"Freshman Toxicologist Trophy",
		"Apprentice Toxicologist Trophy",
		"Beginner Toxicologist Trophy"
	};
	UpdateTrophyGroup(&PoisonTrophy, itemList, "Poison");

	//Fishing Rod "Trophy" - will return with ${Trophy.Fishing}
	itemList = {
		"The Bone Rod",
		"Collapsible Fishing Pole"
	};
	UpdateTrophyGroup(&FishingTrophy, itemList, "Fishing");

}

void UpdateTrophyGroup(CONTENTS** ppGroup, const std::vector<std::string>& itemList, const char* groupname)
{
	for (const std::string& name : itemList) {
		UpdateTrophy(ppGroup, name.c_str(), groupname);
		if (*ppGroup) break;
	}
}

void UpdateTrophy(CONTENTS** ppTrophyVariable, const char* item, const char* Typename)
{
	if (!InGame()) return;

	if (PCONTENTS theTrophy = FindItemByName((char*)item)) {
		if (PITEMINFO thisItem = GetItemFromContents(theTrophy)) {
			*ppTrophyVariable = theTrophy;
			if (*ppTrophyVariable && !bSilent) {
				WriteChatf("\ar[\atMQ2TSTROPHY\ar]\aw:: \ao\ayUpdated - \aw%s: \ap%s", Typename, thisItem->Name);
			}
		}
	}
}

bool WorldContainerCheck()
{
	if (CContainerMgr* pWnd = pContainerMgr) {
		PCONTENTS thiscontaineritem = pWnd->pWorldContainer.pObject;
		if (thiscontaineritem && thiscontaineritem->Open == 1) {
			if (PITEMINFO worldContainer = GetItemFromContents(thiscontaineritem)) {
				strcpy_s(szContainerName, worldContainer->Name);
				containerfound = true;
			}
		}
		else {
			containerfound = false;
			if (bMatches) bMatches = false;
		}
	}
	return containerfound;
}

void SwapSlot(CONTENTS** ppTrophy, const char* pSlot)
{ // slot ammo = ammo; slot mainhand = Primary
	if (*ppTrophy) {
		ITEMINFO* pItem = GetItemFromContents(*ppTrophy);
		ITEMINFO* pSlotItem = GetItemFromContents(FindSlotCurrent(pSlot));
		if (!pSlotItem || strcmp(pSlotItem->Name, pItem->Name)) {
			if (bMatches) bMatches = false;
			char szBuffer[256] = { 0 };
			if (iStep == 1) {
				WriteChatf("\ar[\atMQ2TSTROPHY\ar]\aw:: \ayPicking up: \ap%s", pItem->Name);
				sprintf_s(szBuffer, "/squelch /nomodkey /shiftkey /itemnotify \"%s\" leftmouseup", pItem->Name);
				EzCommand(szBuffer);
				iStep = 2;
			}
			else if (iStep == 2) {
				if (Cursor() && Cursor()->Item2->Name == pItem->Name) {
					WriteChatf("\ar[\atMQ2TSTROPHY\ar]\aw:: \aySwapping: \ap%s\aw into slot: \ay%s", pItem->Name, pSlot);
					sprintf_s(szBuffer, "/squelch /nomodkey /shiftkey /itemnotify %s leftmouseup", pSlot);
					EzCommand(szBuffer);
					iStep = 3;
				}
			}
		}
		else if (!strcmp(pSlotItem->Name, pItem->Name) && !bMatches) {
			bMatches = true;
		}
	}
}

CONTENTS* Cursor()
{
	PCHARINFO2 pChar2 = GetCharInfo2();
	if (pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->Inventory.Cursor) {
		return pChar2->pInventoryArray->Inventory.Cursor;
	}
	return nullptr;
}

CONTENTS* FindSlotCurrent(const char* pSlot)
{
	PCHARINFO2 pChar2 = GetCharInfo2();
	if (strstr(pSlot, "ammo")) {
		return pChar2->pInventoryArray->Inventory.Ammo;
	}
	else if (strstr(pSlot, "range")) {
		return pChar2->pInventoryArray->Inventory.Range;
	}
	else if (strstr(pSlot, "mainhand")) {
		return pChar2->pInventoryArray->Inventory.Primary;
	}
	return nullptr;
}

inline bool InGame()
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2());
}