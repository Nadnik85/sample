/**
	MQ2TSTrophy by Sic & CWTN; a RedGuides Exclusive

	v 1.0 - 12/10/2019
	v 1.1 - 12/10/2019 - Consolidated functions / clean function checks per Brainiac review
	v 1.2 - 12/12/2019 - Re-Write per Brainiac review
					   - moved variables.h and prototypes.h into mq2tstrophy.cpp, corrected errors per review (inline use, superfluous nesting, updated to const char* where possible, etc)
					   - updated comma delimited list to a vector

	Purpose: Equipping your Tradeskill Trophies when opening an environmental crafting container.
	Actions: This plugin will see what you already have in your ammo slot,
		then swap out to the correct trophy for the environmental container you have open
		and then when you leave the environmental window, it will swap back to what you already had equipped.

	Future: Look for, and add any additional environmental crafting containers
**/
#include "../MQ2Plugin.h"

PreSetup("MQ2TSTrophy");
PLUGIN_VERSION(1.2f);

void UpdateTrophyGroup(PCONTENTS* group, const std::vector<std::string>& itemList, const char* groupname);
void UpdateTrophies();
void UpdateTrophy(PCONTENTS* TrophyVariable, const char* item, const char* Typename);
void WorldContainerCheck();
void SwapSlot(PCONTENTS* Trophy, int slot);
void PluginOn();
bool InGame();
PCONTENTS FindSlotDefault(int slot);
PCONTENTS Cursor();

PCONTENTS BakingTrophy = 0, AlchemistTrophy = 0, BrewingTrophy = 0, FletchingTrophy = 0, JewelerTrophy = 0, TinkeringTrophy = 0, PotteryTrophy = 0, ResearchTrophy = 0, BlacksmithTrophy = 0, TailorTrophy = 0, PoisonTrophy = 0, ammoDefault = 0, FishingTrophy = 0, primaryDefault = 0;

bool  bActivated = false, containerfound = false;

int iStep = 1, iPulse = 0;

char szContainerName[MAX_STRING] = { 0 };

PLUGIN_API VOID InitializePlugin(VOID)
{
	DebugSpewAlways("Initializing MQ2TSTrophy");
	if (gGameState != GAMESTATE_INGAME || !pCharSpawn) return;
}

PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("Shutting down MQ2TSTrophy");
}

PLUGIN_API VOID OnPulse(VOID)
{
	int iPulseDelay = 20;
	if (!InGame()) return;
	if (++iPulse < iPulseDelay) return;
	iPulse = 0;
	if (!bActivated) PluginOn();

	WorldContainerCheck();
	if (iStep == 3 && ItemOnCursor()) {
		EzCommand("/autoinv");
		iStep = 4;
	}

	PCONTENTS ammoItem = FindSlotDefault(22);
	PCONTENTS priItem = FindSlotDefault(13);

	if (containerfound) {

		if (!ammoDefault && ammoItem) {
			ammoDefault = ammoItem;
		}
		if (!primaryDefault && priItem) {
			primaryDefault = priItem;
		}
		if (strstr(szContainerName, "Alchemy Table")) {
			//equip alchemy trophy
			SwapSlot(&AlchemistTrophy, 22);
		}
		else if (strstr(szContainerName, "Mixing Bowl") || strstr(szContainerName, "Oven") || strstr(szContainerName, "Ice Cream")) {
			//equip Baking trophy
			SwapSlot(&BakingTrophy, 22);
		}
		else if (strstr(szContainerName, "Brewing Barrel")) {
			//equip Brewing trophy
			SwapSlot(&BrewingTrophy, 22);
		}
		else if (strstr(szContainerName, "Fletching Table")) {
			//equip Fletching trophy
			SwapSlot(&FletchingTrophy, 22);
		}
		else if (strstr(szContainerName, "Jewelry Making Table")) {
			//equip Jewelry trophy
			SwapSlot(&JewelerTrophy, 22);
		}
		else if (strstr(szContainerName, "Fly Making Bench")) {
			//equip Fishing trophy
			SwapSlot(&FishingTrophy, 13);
		}
		else if (strstr(szContainerName, "Kiln") || strstr(szContainerName, "Pottery Wheel")) {
			//equip Pottery trophy
			SwapSlot(&PotteryTrophy, 22);
		}
		else if (strstr(szContainerName, "Poisoncrafting Table")) {
			//equip Poison making trophy
			SwapSlot(&PoisonTrophy, 22);
		}
		else if (strstr(szContainerName, "Forge")) {
			//equip Smithing trophy
			SwapSlot(&BlacksmithTrophy, 22);
		}
		else if (strstr(szContainerName, "Spell Research Table")) {
			//equip research trophy
			SwapSlot(&ResearchTrophy, 22);
		}
		else if (strstr(szContainerName, "Loom")) {
			//equip Tailoring trophy
			SwapSlot(&TailorTrophy, 22);
		}
		else if (strstr(szContainerName, "Tinkering")) {
			//equip Tinkering trophy
			SwapSlot(&TinkeringTrophy, 22);
		}
		if (iStep == 4) {
			iStep = 1;
		}
	}
	else {
		if ((ammoDefault != NULL && ammoDefault != ammoItem) || (ammoDefault && !ammoItem)) {
			// we need to switch back to primaryDefault
			PITEMINFO itemdefault = GetItemFromContents(ammoDefault);
			PITEMINFO itemtest = GetItemFromContents(ammoItem);
			SwapSlot(&ammoDefault, 22);
			if (iStep == 4) {
				ammoDefault = 0;
				iStep = 1;
			}
			return;
		}
		else if (!Cursor() && ammoDefault && ammoDefault == ammoItem) {
			ammoDefault = 0;
			iStep = 1;
			return;
		}
		if ((primaryDefault && primaryDefault != priItem) || (primaryDefault && !priItem)) {
			// we need to switch back to primaryDefault
			SwapSlot(&primaryDefault, 13);
			if (iStep == 4) {
				primaryDefault = 0;
				iStep = 1;
			}
			return;
		}
		else if (!Cursor() && primaryDefault && primaryDefault == priItem) {
			primaryDefault = 0;
			iStep = 1;
		}
	}
}

void PluginOn() {
	if (!bActivated) bActivated = true;
	UpdateTrophies();
}

void UpdateTrophies() {

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
	UpdateTrophyGroup(&AlchemistTrophy, itemList, "AlchemistTrophy");

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
	UpdateTrophyGroup(&BakingTrophy, itemList, "BakingTrophy");

	////Brewer Trophy
	itemList = {
		"Brewmaster's Mug",
		"Master Brewer Trophy",
		"Expert Brewer Trophy",
		"Journeyman Brewer Trophy",
		"Freshman Brewer Trophy",
		"Apprentice Brewer Trophy",
		"Beginner Brewer Trophy"
	};
	UpdateTrophyGroup(&BrewingTrophy, itemList, "BrewingTrophy");

	////Fletcher Trophy
	itemList = {
		"Fletcher's Arrow",
		"Master Fletcher Trophy",
		"Expert Fletcher Trophy",
		"Journeyman Fletcher Trophy",
		"Freshman Fletcher Trophy",
		"Apprentice Fletcher Trophy",
		"Beginner Fletcher Trophy"
	};
	UpdateTrophyGroup(&FletchingTrophy, itemList, "FletchingTrophy");

	////Jeweler Trophy
	itemList = {
		"Intricate Jewelers Glass",
		"Master Jeweler Trophy",
		"Expert Jeweler Trophy",
		"Journeyman Jeweler Trophy",
		"Freshman Jeweler Trophy",
		"Apprentice Jeweler Trophy",
		"Beginner Jeweler Trophy"
	};
	UpdateTrophyGroup(&JewelerTrophy, itemList, "JewelerTrophy");

	////Tinkering Trophy
	itemList = {
		"Hovering Contraption",
		"Master Mechanist Trophy",
		"Expert Mechanist Trophy",
		"Journeyman Mechanist Trophy",
		"Freshman Mechanist Trophy",
		"Apprentice Mechanist Trophy",
		"Beginner Mechanist Trophy"
	};
	UpdateTrophyGroup(&TinkeringTrophy, itemList, "TinkeringTrophy");

	////Pottery Trophy
	itemList = {
		"Clay Flinger's Loop",
		"Master Potter Trophy",
		"Expert Potter Trophy",
		"Journeyman Potter Trophy",
		"Freshman Potter Trophy",
		"Apprentice Potter Trophy",
		"Beginner Potter Trophy"
	};
	UpdateTrophyGroup(&PotteryTrophy, itemList, "PotteryTrophy");

	////Research Trophy
	itemList = {
		"Ethereal Quill",
		"Master Researcher Trophy",
		"Expert Researcher Trophy",
		"Journeyman Researcher Trophy",
		"Freshman Researcher Trophy",
		"Apprentice Researcher Trophy",
		"Beginner Researcher Trophy"
	};
	UpdateTrophyGroup(&ResearchTrophy, itemList, "ResearchTrophy");

	////Blacksmithing Trophy
	itemList = {
		"Blacksmith's Adamantine Hammer",
		"Master Smith Trophy",
		"Expert Smith Trophy",
		"Journeyman Smith Trophy",
		"Freshman Smith Trophy",
		"Apprentice Smith Trophy",
		"Beginner Smith Trophy"
	};
	UpdateTrophyGroup(&BlacksmithTrophy, itemList, "BlacksmithTrophy");

	////Tailor Trophy
	itemList = {
		"Mystical Bolt",
		"Master Tailor Trophy",
		"Expert Tailor Trophy",
		"Journeyman Tailor Trophy",
		"Freshman Tailor Trophy",
		"Apprentice Tailor Trophy",
		"Beginner Tailor Trophy"
	};
	UpdateTrophyGroup(&TailorTrophy, itemList, "TailorTrophy");

	////Poison Trophy
	itemList = {
		"Peerless Pestle",
		"Master Toxicologist Trophy",
		"Expert Toxicologist Trophy",
		"Journeyman Toxicologist Trophy",
		"Freshman Toxicologist Trophy",
		"Apprentice Toxicologist Trophy",
		"Beginner Toxicologist Trophy"
	};
	UpdateTrophyGroup(&PoisonTrophy, itemList, "PoisonTrophy");

	////Fishing Rod "Trophy"
	itemList = {
		"The Bone Rod"
	};
	UpdateTrophyGroup(&FishingTrophy, itemList, "FishingTrophy");

}

void UpdateTrophyGroup(PCONTENTS* group, const std::vector<std::string>& itemList, const char* groupname)
{
	for (const std::string& name : itemList)
	{
		UpdateTrophy(group, name.c_str(), groupname);
		if (*group) break;
	}
}

void UpdateTrophy(PCONTENTS* TrophyVariable, const char* item, const char* Typename) {
	if (!InGame()) return;

	if (PCONTENTS theTrophy = FindItemByName((char*)item)) {
		if (PITEMINFO thisItem = GetItemFromContents(theTrophy)) {
			*TrophyVariable = theTrophy;
			if (*TrophyVariable) {
				WriteChatf("\ayUpdated - \aw%s: \ap%s", Typename, thisItem->Name);
			}
		}
	}
}

void WorldContainerCheck() {
	PCONTENTS temp = 0;
	if (CContainerMgr* pWnd = pContainerMgr) {
		if (PCONTENTS thiscontaineritem = pWnd->pWorldContainer.pObject) {
			temp = thiscontaineritem;
			if (PITEMINFO worldContainer = GetItemFromContents(thiscontaineritem)) {
				sprintf_s(szContainerName, MAX_STRING, "%s", worldContainer->Name);
				containerfound = true;
			}
		}
	}
	if (containerfound) {
		if (!temp)
			containerfound = false;
	}
}

void SwapSlot(PCONTENTS* Trophy, int slot) { // slot 22 is Ammo; slot 13 is Primary
	if (*Trophy) {
		if (PITEMINFO item = GetItemFromContents(*Trophy)) {
			if (!FindSlotDefault(slot) || FindSlotDefault(slot) != *Trophy) {
				char szBuffer[MAX_STRING] = { 0 };
				if (iStep == 1) {
					//WriteChatf("\ayPicking up: \ap%s", item->Name);
					sprintf_s(szBuffer, MAX_STRING, "/squelch /nomodkey /shiftkey /itemnotify \"%s\" leftmouseup", item->Name);
					EzCommand(szBuffer);
					iStep = 2;
				}
				else if (iStep == 2 && Cursor() == *Trophy) {
					//WriteChatf("\aySwapping \ap%s\aw into slot: \ay%i",item->Name, slot);
					sprintf_s(szBuffer, MAX_STRING, "/squelch /nomodkey /shiftkey /itemnotify %i leftmouseup", slot);
					EzCommand(szBuffer);
					iStep = 3;
				}
			}
		}
	}
}

PCONTENTS Cursor() {
	PCHARINFO2 pChar2 = GetCharInfo2();
	if (pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->Inventory.Cursor) {
		PCONTENTS cursoritem = pChar2->pInventoryArray->Inventory.Cursor;
		return cursoritem;
	}
	return nullptr;
}

PCONTENTS FindSlotDefault(int slot) {
	PCHARINFO2 pChar2 = GetCharInfo2();
	if (pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray) {
		if (PCONTENTS item = pChar2->pInventoryArray->InventoryArray[slot]) {
			return item;
		}
	}
	return nullptr;
}

inline bool InGame()
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2());
}
