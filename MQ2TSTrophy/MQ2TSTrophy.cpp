/**
	MQ2TSTrophy by Sic & CWTN; a RedGuides Exclusive

	v 1.0 - 12/10/2019
	v 1.1 - 12/10/2019 - Consolidated functions / clean function checks per Brainiac review
	v 1.2 - 12/12/2019 - Re-Write per Brainiac review
					   - moved variables.h and prototypes.h into mq2tstrophy.cpp, corrected errors per review (inline use, superfluous nesting, updated to const char* where possible, etc)
					   - updated comma delimited list to a vector
	v 1.3 - 12/20/2019 - Additional cleanup

	Purpose: Equipping your Tradeskill Trophies when opening an environmental crafting container.
	Actions: This plugin will see what you already have in your ammo slot,
		then swap out to the correct trophy for the environmental container you have open
		and then when you leave the environmental window, it will swap back to what you already had equipped.

	Future: Look for, and add any additional environmental crafting containers
**/
#include "../MQ2Plugin.h"

PreSetup("MQ2TSTrophy");
PLUGIN_VERSION(2.0);

bool WorldContainerCheck();
bool InGame();
void UpdateTrophyGroup(PCONTENTS* group, const std::vector<std::string>& itemList, const char* groupname);
void UpdateTrophies();
void UpdateTrophy(PCONTENTS* TrophyVariable, const char* item, const char* Typename);
void SwapSlot(PCONTENTS* Trophy, const char* slot);
void PluginOn();

PCONTENTS FindSlotCurrent(const char* slot);
PCONTENTS Cursor();

PCONTENTS BakingTrophy = 0;
PCONTENTS AlchemistTrophy = 0;
PCONTENTS BrewingTrophy = 0;
PCONTENTS FletchingTrophy = 0;
PCONTENTS JewelerTrophy = 0;
PCONTENTS TinkeringTrophy = 0;
PCONTENTS PotteryTrophy = 0;
PCONTENTS ResearchTrophy = 0;
PCONTENTS BlacksmithTrophy = 0;
PCONTENTS TailorTrophy = 0;
PCONTENTS PoisonTrophy = 0;
PCONTENTS ammoDefault = 0;
PCONTENTS FishingTrophy = 0;

bool bActivated = false;
bool containerfound = false;
bool bSilent = false;
char szContainerName[128] = { 0 };
int iStep = 1;
int iPulse = 0;

PLUGIN_API VOID InitializePlugin(VOID)
{
	DebugSpewAlways("Initializing MQ2TSTrophy");
	if (gGameState != GAMESTATE_INGAME || !pCharSpawn) return;
}

PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("Shutting down MQ2TSTrophy");
}

PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color) 
{
	if (!strncmp(Line, "Your ", 5) && strstr(Line, " Trophy has evolved!")) { // update Trophies
		UpdateTrophies();
	}
	return 0;
}

PLUGIN_API VOID OnPulse(VOID)
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

		if (strstr(szContainerName, "Alchemy Table")) {
			//equip alchemy trophy
			SwapSlot(&AlchemistTrophy, "ammo");
		}
		else if (strstr(szContainerName, "Mixing Bowl") || strstr(szContainerName, "Oven") || strstr(szContainerName, "Ice Cream")) {
			//equip Baking trophy
			SwapSlot(&BakingTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Brewing Barrel")) {
			//equip Brewing trophy
			SwapSlot(&BrewingTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Fletching Table")) {
			//equip Fletching trophy
			SwapSlot(&FletchingTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Jewelry Making Table")) {
			//equip Jewelry trophy
			SwapSlot(&JewelerTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Fly Making Bench")) {
			//equip Fishing trophy
			SwapSlot(&FishingTrophy,"mainhand");
		}
		else if (strstr(szContainerName, "Kiln") || strstr(szContainerName, "Pottery Wheel")) {
			//equip Pottery trophy
			SwapSlot(&PotteryTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Poisoncrafting Table")) {
			//equip Poison making trophy
			SwapSlot(&PoisonTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Forge")) {
			//equip Smithing trophy
			SwapSlot(&BlacksmithTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Spell Research Table")) {
			//equip research trophy
			SwapSlot(&ResearchTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Loom")) {
			//equip Tailoring trophy
			SwapSlot(&TailorTrophy,"ammo");
		}
		else if (strstr(szContainerName, "Tinkering")) {
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
	UpdateTrophyGroup(&AlchemistTrophy, itemList, "Alchemist");

	//Baking Trophy
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

	//Brewer Trophy
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

	//Fletcher Trophy
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

	//Jeweler Trophy
	itemList = {
		"Intricate Jewelers Glass",
		"Master Jeweler Trophy",
		"Expert Jeweler Trophy",
		"Journeyman Jeweler Trophy",
		"Freshman Jeweler Trophy",
		"Apprentice Jeweler Trophy",
		"Beginner Jeweler Trophy"
	};
	UpdateTrophyGroup(&JewelerTrophy, itemList, "Jewelery");

	//Tinkering Trophy
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

	//Pottery Trophy
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

	//Research Trophy
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

	//Blacksmithing Trophy
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

	//Tailor Trophy
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

	//Poison Trophy
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

	//Fishing Rod "Trophy"
	itemList = {
		"The Bone Rod"
	};
	UpdateTrophyGroup(&FishingTrophy, itemList, "Fishing");

}

void UpdateTrophyGroup(PCONTENTS* group, const std::vector<std::string>& itemList, const char* groupname) 
{
	for (const std::string& name : itemList) {
		UpdateTrophy(group, name.c_str(), groupname);
		if (*group) break;
	}
}

void UpdateTrophy(PCONTENTS* TrophyVariable, const char* item, const char* Typename) 
{
	if (!InGame()) return;

	if (PCONTENTS theTrophy = FindItemByName((char*)item)) {
		if (PITEMINFO thisItem = GetItemFromContents(theTrophy)) {
			*TrophyVariable = theTrophy;
			if (*TrophyVariable && !bSilent) {
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
				sprintf_s(szContainerName, "%s", worldContainer->Name);
				containerfound = true;
			}
		}
		else {
			containerfound = false;
		}
	}
	return containerfound;
}

void SwapSlot(PCONTENTS* Trophy, const char* slot) 
{ // slot ammo = ammo; slot mainhand = Primary
	if (*Trophy) {
		ITEMINFO* pItem = GetItemFromContents(*Trophy);
		ITEMINFO* pSlotItem = GetItemFromContents(FindSlotCurrent(slot));
		if (!pSlotItem || strcmp(pSlotItem->Name, pItem->Name)) {
			char szBuffer[256] = { 0 };
			if (iStep == 1) {
				WriteChatf("\ar[\atMQ2TSTROPHY\ar]\aw:: \ayPicking up: \ap%s", pItem->Name);
				sprintf_s(szBuffer, "/squelch /nomodkey /shiftkey /itemnotify \"%s\" leftmouseup", pItem->Name);
				EzCommand(szBuffer);
				iStep = 2;
			}
			else if (iStep == 2) {
				if (Cursor() && Cursor()->Item2->Name == pItem->Name) {
					WriteChatf("\ar[\atMQ2TSTROPHY\ar]\aw:: \aySwapping: \ap%s\aw into slot: \ay%s", pItem->Name, slot);
					sprintf_s(szBuffer, "/squelch /nomodkey /shiftkey /itemnotify %s leftmouseup", slot);
					EzCommand(szBuffer);
					iStep = 3;
				}
			}
		}
	}
}

PCONTENTS Cursor() 
{
	PCHARINFO2 pChar2 = GetCharInfo2();
	if (pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->Inventory.Cursor) {
		return pChar2->pInventoryArray->Inventory.Cursor;
	}
	return nullptr;
}

PCONTENTS FindSlotCurrent(const char* slot) 
{
	PCHARINFO2 pChar2 = GetCharInfo2();
	if (strstr(slot, "ammo")) {
		return pChar2->pInventoryArray->Inventory.Ammo;
	}
	else if (strstr(slot, "mainhand")) {
		return pChar2->pInventoryArray->Inventory.Primary;
	}
	return nullptr;
}

inline bool InGame() 
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2());
}
