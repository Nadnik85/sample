/**
	MQ2TSTrophy by Sic & CWTN; a RedGuides Exclusive

	v 1.0 - 12/10/2019

	Purpose: Equipping your Tradeskill Trophies when opening an environmental crafting container.
	Actions: This plugin will see what you already have in your ammo slot, 
		then swap out to the correct trophy for the environmental container you have open
		and then when you leave the environmental window, it will swap back to what you already had equipped.

	Future: Look for, and add any additional environmental crafting containers

**/
#include "../MQ2Plugin.h"
#include "Prototypes.h"
#include "Variables.h"

PreSetup("MQ2TSTrophy");
PLUGIN_VERSION(1.0f);

PLUGIN_API VOID InitializePlugin(VOID)
{
    DebugSpewAlways("Initializing MQ2TSTrophy");
	if (gGameState != GAMESTATE_INGAME || !pCharSpawn) return;
}

PLUGIN_API VOID ShutdownPlugin(VOID)
{
    DebugSpewAlways("Shutting down MQ2TSTrophy");

    //Remove commands, MQ2Data items, hooks, etc.
    //RemoveMQ2Benchmark(bmMyBenchmark);
    //RemoveCommand("/mycommand");
    //RemoveXMLFile("MQUI_MyXMLFile.xml");
}

PLUGIN_API VOID OnPulse(VOID)
{
	if (!InGame()) return;
	//If the plugin hasn't been activated, do that.
	if (++iPulse < iPulseDelay) return;
	iPulse = 0;
	if (!bActivated) PluginOn();

	WorldContainerCheck();
	if (iStep == 3 && ItemOnCursor()) {
		EzCommand("/autoinv");
		iStep = 4;
	}

	if (containerfound) {
		if (!ammoDefault && FindAmmoDefault()) {
			ammoDefault = FindAmmoDefault();
		}
		if (strstr(szContainerName, "Alchemy Table")) {
			//equip alchemy trophy
			SwapToTrophy(&AlchemistTrophy);
		}
		else if (strstr(szContainerName, "Mixing Bowl") || strstr(szContainerName, "Oven") || strstr(szContainerName, "Ice Cream")) {
			//equip Baking trophy
			SwapToTrophy(&BakingTrophy);
		}
		else if (strstr(szContainerName, "Brewing Barrel")) {
			//equip Brewing trophy
			SwapToTrophy(&BrewingTrophy);
		}
		else if (strstr(szContainerName, "Fletching Table")) {
			//equip Fletching trophy
			SwapToTrophy(&FletchingTrophy);
		}
		else if (strstr(szContainerName, "Jewelry Making Table")) {
			//equip Jewelry trophy
			SwapToTrophy(&JewelerTrophy);
		}
		//else if (strstr(szContainerName, "Fly Making Bench")) {
		//	//equip Fishing trophy
		//	//SwapToTrophy(&FishingTrophy);
		//	return;
		//}
		else if (strstr(szContainerName, "Kiln") || strstr(szContainerName, "Pottery Wheel")) {
			//equip Pottery trophy
			SwapToTrophy(&PotteryTrophy);
		}
		else if (strstr(szContainerName, "Poisoncrafting Table")) {
			//equip Poison making trophy
			SwapToTrophy(&PoisonTrophy);
		}
		else if (strstr(szContainerName, "Forge")) {
			//equip Smithing trophy
			SwapToTrophy(&BlacksmithTrophy);
		}
		else if (strstr(szContainerName, "Spell Research Table")) {
			//equip research trophy
			SwapToTrophy(&ResearchTrophy);
		}
		else if (strstr(szContainerName, "Loom")) {
			//equip Tailoring trophy
			SwapToTrophy(&TailorTrophy);
		}
		else if (strstr(szContainerName, "Tinkering")) {
			//equip Pottery trophy
			SwapToTrophy(&TinkeringTrophy);
		}
		if (iStep == 4) iStep = 1;
	}
	else {
		if ((ammoDefault && FindAmmoDefault() && ammoDefault != FindAmmoDefault()) || (ammoDefault && !FindAmmoDefault())) {
			// we need to switch back to ammoDefault
			SwapToTrophy(&ammoDefault);
			if (iStep == 4) {
				ammoDefault = 0;
				iStep = 1;
			}
		}
		else if (ammoDefault && FindAmmoDefault() && ammoDefault == FindAmmoDefault()) {
			ammoDefault = 0;
			iStep = 1;
		}
	}
}

void PluginOn() {
	if (!bActivated) bActivated = true;
	UpdateTrophies();
}

void UpdateTrophies() {
	if (!InGame()) return;
	char buffer[MAX_STRING] = { 0 };

	//Alchemist Trophy
	strcpy_s(buffer, MAX_STRING, "Mundunugu Medicine Stick,Master Alchemist Trophy,Expert Alchemist Trophy,Journeyman Alchemist Trophy,Freshman Alchemist Trophy,Apprentice Alchemist Trophy,Beginner Alchemist Trophy");
	UpdateTrophyGroup(&AlchemistTrophy, buffer, "AlchemistTrophy");
	if (AlchemistTrophy) {
		bAlchemistTrophy = true;
	}

	//Baking Trophy
	strcpy_s(buffer, MAX_STRING, "Denmother's Rolling Pin,Master Baker Trophy,Expert Baker Trophy,Journeyman Baker Trophy,Freshman Baker Trophy,Apprentice Baker Trophy,Beginner Baker Trophy");
	UpdateTrophyGroup(&BakingTrophy, buffer, "BakingTrophy");
	if (BakingTrophy) {
		bBakingTrophy = true;
	}

	//Brewer Trophy
	strcpy_s(buffer, MAX_STRING, "Brewmaster's Mug,Master Brewer Trophy,Expert Brewer Trophy,Journeyman Brewer Trophy,Freshman Brewer Trophy,Apprentice Brewer Trophy,Beginner Brewer Trophy");
	UpdateTrophyGroup(&BrewingTrophy, buffer, "BrewingTrophy");
	if (BrewingTrophy) {
		bBrewingTrophy = true;
	}

	//Fletcher Trophy
	strcpy_s(buffer, MAX_STRING, "Fletcher's Arrow,Master Fletcher Trophy,Expert Fletcher Trophy,Journeyman Fletcher Trophy,Freshman Fletcher Trophy,Apprentice Fletcher Trophy,Beginner Fletcher Trophy");
	UpdateTrophyGroup(&FletchingTrophy, buffer, "FletchingTrophy");
	if (FletchingTrophy) {
		bFletchingTrophy = true;
	}

	//Jeweler Trophy
	strcpy_s(buffer, MAX_STRING, "Intricate Jewelers Glass,Master Jeweler Trophy,Expert Jeweler Trophy,Journeyman Jeweler Trophy,Freshman Jeweler Trophy,Apprentice Jeweler Trophy,Beginner Jeweler Trophy");
	UpdateTrophyGroup(&JewelerTrophy, buffer, "JewelerTrophy");
	if (JewelerTrophy) {
		bJewelerTrophy = true;
	}

	//Tinkering Trophy
	strcpy_s(buffer, MAX_STRING, "Hovering Contraption,Master Mechanist Trophy,Expert Mechanist Trophy,Journeyman Mechanist Trophy,Freshman Mechanist Trophy,Apprentice Mechanist Trophy,Beginner Mechanist Trophy");
	UpdateTrophyGroup(&TinkeringTrophy, buffer, "TinkeringTrophy");
	if (TinkeringTrophy) {
		bTinkeringTrophy = true;
	}

	//Pottery Trophy
	strcpy_s(buffer, MAX_STRING, "Clay Flinger's Loop,Master Potter Trophy,Expert Potter Trophy,Journeyman Potter Trophy,Freshman Potter Trophy,Apprentice Potter Trophy,Beginner Potter Trophy");
	UpdateTrophyGroup(&PotteryTrophy, buffer, "PotteryTrophy");
	if (PotteryTrophy) {
		bPotteryTrophy = true;
	}

	//Research Trophy
	strcpy_s(buffer, MAX_STRING, "Ethereal Quill,Master Researcher Trophy,Expert Researcher Trophy,Journeyman Researcher Trophy,Freshman Researcher Trophy,Apprentice Researcher Trophy,Beginner Researcher Trophy");
	UpdateTrophyGroup(&ResearchTrophy, buffer, "ResearchTrophy");
	if (ResearchTrophy) {
		bResearchTrophy = true;
	}

	//Blacksmithing Trophy
	strcpy_s(buffer, MAX_STRING, "Blacksmith's Adamantine Hammer,Master Smith Trophy,Expert Smith Trophy,Journeyman Smith Trophy,Freshman Smith Trophy,Apprentice Smith Trophy,Beginner Smith Trophy");
	UpdateTrophyGroup(&BlacksmithTrophy, buffer, "BlacksmithTrophy");
	if (BlacksmithTrophy) {
		bBlacksmithTrophy = true;
	}

	//Tailor Trophy
	strcpy_s(buffer, MAX_STRING, "Mystical Bolt,Master Tailor Trophy,Expert Tailor Trophy,Journeyman Tailor Trophy,Freshman Tailor Trophy,Apprentice Tailor Trophy,Beginner Tailor Trophy");
	UpdateTrophyGroup(&TailorTrophy, buffer, "TailorTrophy");
	if (TailorTrophy) {
		bTailorTrophy = true;
	}

	//Poison Trophy
	strcpy_s(buffer, MAX_STRING, "Peerless Pestle,Master Toxicologist Trophy,Expert Toxicologist Trophy,Journeyman Toxicologist Trophy,Freshman Toxicologist Trophy,Apprentice Toxicologist Trophy,Beginner Toxicologist Trophy");
	UpdateTrophyGroup(&PoisonTrophy, buffer, "PoisonTrophy");
	if (PoisonTrophy) {
		bPoisonTrophy = true;
	}

}

void UpdateTrophyGroup(PCONTENTS* group, char list[MAX_STRING], char groupname[MAX_STRING]) {
	for (int i = 1; i < 30; i++) {
		char Current[MAX_STRING] = "";
		GetArg(Current, list, i, 0, 0, 0, ',', 0);
		if (!strlen(Current)) break;
		UpdateTrophy(group, Current, groupname);
		if (*group) break;
	}
}

void UpdateTrophy(PCONTENTS* TrophyVariable, PCHAR szLine, PCHAR Typename) {
	if (!InGame())
		return;
	PCONTENTS temp = 0;
	if (*TrophyVariable) {
		return;
	}
	if (PCONTENTS theTrophy = FindItemByName(szLine)) {
		if (PITEMINFO thisItem = GetItemFromContents(theTrophy)) {
			/*if (thisItem->SkillModType) {
				WriteChatf("Skill Mod Type: %s", szSkills[thisItem->SkillModType]);
			}*/
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

void SwapToTrophy(PCONTENTS* Trophy) {
	if (*Trophy) {
		if (PITEMINFO trophy = GetItemFromContents(*Trophy)) {
			if (!FindAmmoDefault() || FindAmmoDefault() != *Trophy) {
				char szBuffer[MAX_STRING] = { 0 };
				if (iStep == 1) {
					sprintf_s(szBuffer, MAX_STRING, "/nomodkey /shiftkey /itemnotify \"%s\" leftmouseup", trophy->Name);
					EzCommand(szBuffer);
					iStep = 2;
					return;
				}
				if (iStep == 2 && ItemOnCursor() && Cursor() && Cursor() == *Trophy) {
					sprintf_s(szBuffer, MAX_STRING, "/nomodkey /shiftkey /itemnotify ammo leftmouseup");
					EzCommand(szBuffer);
					iStep = 3;
					return;
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

PCONTENTS FindAmmoDefault() {
	if (PCHARINFO2 pChar2 = GetCharInfo2()) {
		if (pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray) {
			if (PCONTENTS item = pChar2->pInventoryArray->InventoryArray[22]) {
				return item;
			}
		}
	}
	return nullptr;
}

inline bool InGame()
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2());
}
