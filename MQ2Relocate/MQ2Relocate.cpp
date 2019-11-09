// MQ2Relocate.cpp : Defines the entry point for the DLL application.
/**
MQ2Relocate by Sic & ChatWithThisName

Purpose & Scope: Plugin to allow easier use of relocation items/aas, as well as specific logic to handle "converting" travel items like Wishing Lamp: and Zeuria Slide:

Usage:
	/relocate help will give a list of available options
		/relocate air, fire, stone - Wishing Lamp
		/relocate stonebrunt, dreadlands, nek, nro, skyfire for Zueria Slide
		/relocate pok - uses drunkards stein, brick of knowledge, the binden concerrentia, and the fabled brinden concerrentia
		/relocate gate - uses class gate AA or a translocation potion
		/relocate origin - uses origin AA or Sceptre of Draconic Calling
		/relocate brell - uses Mark of Brell
		/relocate anchor - uses Primary or Secondary Anchor
		/relocate anchor1 - uses specifically Primary Anchor
		/relocate anchor2 - uses specifically Secondary Anchor
		/relocate fellow - uses your fellowship insignia

Changelog:
	10/30/2019 - V1.0 - Submitted Plugin
	10/31/2019 - V1.1 - Updated w/ CWTN Common changes - Added Component Check to Binden POK item -
	11/05/2019 - V1.2 - Updates to add lobby, blood, evac, teleport, and translocate to /relocate
						Updates to add /translocate with current target or name
	
**/
#include "../MQ2Plugin.h"
#include "../CWTNCommons/UtilityFunctions.h"

PreSetup("MQ2Relocate");
PLUGIN_VERSION(1.2f);

void ReloCmd(PSPAWNINFO pChar, PCHAR szLine);
void TransloCmd(PSPAWNINFO pChar, PCHAR szLine);
bool HaveAlias(PCHAR ShortCommand);
bool UseClickyByItemName(PCHAR szItem);
bool IsClickyReadyByItemName(PCHAR szItem);
bool IsTargetPlayer(PCHAR szItem);

char convertoption[MAX_STRING] = { 0 };
char reloClicky[128] = { 0 };
bool bAmConverting = false;
bool needsUsing = false;
bool canGatePotion = false;
bool canGateAA = false;
bool canOriginAA = false;
bool canLobbyAA = false;
bool canHarmonicAA = false;
bool canEvacAA = false;
bool canTranslocate = false;
bool canTeleportAA = false;
bool canGroupEvacAA = false;

void ReloCmd(PSPAWNINFO pChar, PCHAR szLine) {
	if (!InGame())
		return;
	//Get our parameters
	CHAR Arg[MAX_STRING] = { 0 };
	GetArg(Arg, szLine, 1);
	if (strlen(Arg)) {
		char temp[MAX_STRING] = "/useitem ";
		if (!_stricmp(Arg, "help")) { //output available arguments for /relocate
			WriteChatf("Welcome to MQ2Relocate!");
			WriteChatf("By \agSic\aw & \aoChatWithThisName\aw Exclusively for \arRedGuides\aw.");
			WriteChatf("\agValid Relocate options are:\aw");
			WriteChatf("/relocate \agair\aw, \agfire\aw, and \agstone\aw for \ayZephyr's Lamp\aw.");
			WriteChatf("/relocate \agstonebrunt\aw, \agdreadlands\aw, \aggreatdivide\aw, \agnek\aw, \agnro\aw, and \agskyfire\aw for \ayZueria Slide\aw.");
			WriteChatf("/relocate \agpok\aw for \ayPlane of Knowledge\aw.");
			WriteChatf("/relocate \aggate\aw to use your \ayGate AA\aw or \ayTranslocation Potion\aw.");
			WriteChatf("/relocate \agorigin\aw to use your \ayOrigin AA\aw.");
			WriteChatf("/relocate \agbrell\aw to use your \ayMark of Brell\aw.");
			WriteChatf("/relocate \aganchor\aw to use your \ayPrimary \awor \aySecondary Anchor\aw.");
			WriteChatf("/relocate \aganchor1\aw to use your \ayPrimary Anchor\aw.");
			WriteChatf("/relocate \aganchor2\aw to use your \aySecondary Anchor\aw.");
			WriteChatf("/relocate \agfellow\aw or \agfellowship\aw to use your \ayFellowship Insignia\aw.");
			WriteChatf("/relocate \aglobby\aw to use your \ay Throne of Heroes AA\aw.");
			WriteChatf("/relocate \agblood\aw to use your \ay Theatre of Blood AA\aw.");
			WriteChatf("/relocate \agevac\aw to use your \ay Group Evac AA\aw (if you are in a group) or your \ay Personal Evac AA\aw.");
			WriteChatf("/relocate \agteleport\aw to use your \ay AoE Teleport AA\aw.");
			WriteChatf("\agValid Translocate options are:\aw");
			WriteChatf("\ay/translocate\aw to \ayTranslocate\aw your target to their bind.");
			WriteChatf("\ay/translocate\aw \arRedBot\aw to \ayTranslocate\aw \arRedBot\aw to their bind.");
			return;
		}
		if (!_stricmp(Arg, "air") || !_stricmp(Arg, "fire") || !_stricmp(Arg, "stone")) { //use Wishing Lamp:
			char reloClicky[64] = "Wishing Lamp:";
			char air[64] = "Wishing Lamp: Zephyr's Flight";
			char fire[64] = "Wishing Lamp: Palace of Embers";
			char stone[64] = "Wishing Lamp: The Stone Demesne";
			if (!FindItemByName(reloClicky)) {
				WriteChatf("You do not appear to have a Wishing Lamp.");
				return;
			}
			if (!_stricmp(Arg, "air")) {
				if (FindItemByName(air)) {
					strcat_s(temp, air);
					EzCommand(temp);
					WriteChatf("Relocating with: \ag%s ", air);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, air);
				}
			}
			if (!_stricmp(Arg, "fire")) {
				if (FindItemByName(fire)) {
					strcat_s(temp, fire);
					EzCommand(temp);
					WriteChatf("Relocating with: \ag%s ", fire);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, fire);
				}
			}
			if (!_stricmp(Arg, "stone")) {
				//We have a lamp for sure at this point. Lets see if it's already the right conversion of it. 
				if (FindItemByName(stone)) {
					strcat_s(temp, stone);
					EzCommand(temp);
					WriteChatf("Relocating with: \ag%s ", stone);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, stone);
				}
			}
			return;
		}
		if (!_stricmp(Arg, "stonebrunt") || !_stricmp(Arg, "dreadlands") || !_stricmp(Arg, "nek") || !_stricmp(Arg, "greatdivide") || !_stricmp(Arg, "nro") || !_stricmp(Arg, "skyfire")) { // Zueria Slide
			char reloClicky[64] = "Zueria Slide:";
			char stonebrunt[64] = "Zueria Slide: Stonebrunt";
			char dreadlands[64] = "Zueria Slide: Dreadlands";
			char greatdivide[64] = "Zueria Slide: Great Divide";
			char nek[64] = "Zueria Slide: Nektulos";
			char nro[64] = "Zueria Slide: North Ro";
			char skyfire[64] = "Zueria Slide: Skyfire";
			if (!FindItemByName(reloClicky)) {
				WriteChatf("You do not appear to have a Zueria Slide.");
				return;
			}
			if (!_stricmp(Arg, "stonebrunt")) {
				if (FindItemByName(stonebrunt)) {
					strcat_s(temp, stonebrunt);
					EzCommand(temp);
					WriteChatf("Relocating with: \ag%s ", stonebrunt);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, stonebrunt);
				}
			}
			if (!_stricmp(Arg, "dreadlands")) {
				if (FindItemByName(dreadlands)) {
					strcat_s(temp, dreadlands);
					EzCommand(temp);
					WriteChatf("Relocating with: \ag%s ", dreadlands);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, dreadlands);
				}
			}
			if (!_stricmp(Arg, "greatdivide")) {
				if (FindItemByName(greatdivide)) {
					strcat_s(temp, greatdivide);
					EzCommand(temp);
					WriteChatf("Relocating with: \ag%s ", greatdivide);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, greatdivide);
				}
			}
			if (!_stricmp(Arg, "nek")) {
				if (FindItemByName(nek)) {
					strcat_s(temp, nek);
					EzCommand(temp);
					WriteChatf("Relocating with: \ag%s ", nek);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, nek);
				}
			}
			if (!_stricmp(Arg, "nro")) {
				if (FindItemByName(nro)) {
					strcat_s(temp, nro);
					EzCommand(temp);
					WriteChatf("Relocating with: \ag%s ", nro);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, nro);
				}
			}
			if (!_stricmp(Arg, "skyfire")) {
				if (FindItemByName(skyfire)) {
					strcat_s(temp, skyfire);
					EzCommand(temp);
					WriteChatf("Relocating with: \ag%s ", skyfire);
				}
				else {
					bAmConverting = true;
					sprintf_s(convertoption, MAX_STRING, skyfire);
				}
			}
			return;
		}
		if (!_stricmp(Arg, "pok")) { // Plane of Knowledge
			if (FindItemByName("Drunkard's Stein") && IsClickyReadyByItemName("Drunkard's Stein")) {
				sprintf_s(reloClicky, "Drunkard's Stein");
			}
			else if (FindItemByName("Brick of Knowledge") && IsClickyReadyByItemName("Brick of Knowledge")) {
				sprintf_s(reloClicky, "Brick of Knowledge");
			}
			else if (FindItemByName("The Binden Concerrentia") && FindItemCountByName("Quintessence of Knowledge", 1) && IsClickyReadyByItemName("The Binden Concerrentia")) {
				sprintf_s(reloClicky, "The Binden Concerrentia");
			}
			else if (FindItemByName("The Fabled Binden Concerrentia") && FindItemCountByName("Quintessence of Knowledge", 1) && IsClickyReadyByItemName("The Fabled Binden Concerrentia")) {
				sprintf_s(reloClicky, "The Fabled Binden Concerrentia");
			}
			else {
				WriteChatf("\aoDOH!\ar You don't have a \aypok \arclicky that is ready!");
				return;
			}
			if (FindItemByName(reloClicky)) {
				if (UseClickyByItemName(reloClicky)) {
					//WriteChatf("Casting \ag %s \aw ", reloClicky);
				}
			}
			else {
				//This else for error checking and should never happen.
				WriteChatf("I don't have a %s", reloClicky);
			}
			return;
		}
		if (!_stricmp(Arg, "gate")) { // use gate AA if you have it, otherwise try and use a gate potion
			DWORD classID = GetCharInfo2()->Class;
			switch (classID) {
			case EQData::Cleric:
			case EQData::Druid:
			case EQData::Enchanter:
			case EQData::Mage:
			case EQData::Necromancer:
			case EQData::Shaman:
			case EQData::Wizard:
				if (AltAbility("Gate") && AltAbility("Gate")->CurrentRank > 0) {
					canGateAA = true;
					return;
				}
			default:
				break;
			}
			if (!canGateAA) {
				// Check if i have a Philter of Major Translocation - if so, turn canGatePotion to true
				if (FindItemByName("Philter of Major Translocation")) {
					canGatePotion = true;
				}
				else {
					WriteChatf("\arI don't seem to have the ability to gate, nor do you have a gate potion!\aw");
				}
			}
			return;
		}
		if (!_stricmp(Arg, "origin")) { //try and use Origin AA, otherwise try and use Sceptre of Draconic Calling
			if (AltAbility("Origin") && AltAbility("Origin")->CurrentRank > 0 && AltAbilityReady("Origin")) { // added the AltAbilityCheck because we don't want to canOriginAA if it it not ready
				canOriginAA = true;
				return;
			}
			else {
				if (AltAbility("Origin") && AltAbility("Origin")->CurrentRank > 0 && !AltAbilityReady("Origin")) {
					WriteChatf("\ayOrigin \arisn't ready right now\aw.");
				}
				else if ((GetCharInfo()->StartingCity == 394) && FindItemByName("Sceptre of Draconic Calling") && IsClickyReadyByItemName("Sceptre of Draconic Calling")) {
					UseClickyByItemName("Sceptre of Draconic Calling");
					return; // returning here since we don't care about the !canOriginAA check
				}
				else if (!AltAbility("Origin") || AltAbility("Origin")->CurrentRank == 0) {
					WriteChatf("\arI don't have the \ayOrigin \arAA\aw!");
					return;
				}
				if (!FindItemByName("Sceptre of Draconic Calling")) {
					WriteChatf("\arI don't have a \aySceptre of Draconic Calling\aw.");
				}
				else if (FindItemByName("Sceptre of Draconic Calling") && IsClickyReadyByItemName("Sceptre of Draconic Calling")) {
					WriteChatf("\aySceptre of Draconic Calling \arisn't ready right now\aw!");
				}
				WriteChatf("\arWe don't have a way to Origin at the moment\ar.");
			}
			return;
		}
		if (!_stricmp(Arg, "brell")) { // Mark of Brell for Brell's Rest
			if (FindItemByName("Mark of Brell") && IsClickyReadyByItemName("Mark of Brell")) {
				UseClickyByItemName("Mark of Brell");
				return;
			}
			if (!FindItemByName("Mark of Brell")) {
				WriteChatf("\arYou do not have a \ayMark of Brell\aw!");
				return;
			}
			if (FindItemByName("Mark of Brell") && !IsClickyReadyByItemName("Mark of Brell")) {
				WriteChatf("\ayMark of Brell \aris not ready\aw!");
				return;
			}
		}
		if (!_stricmp(Arg, "anchor")) { // Use Primary or Secondary Anchor
			if (FindItemByName("Primary Anchor Transport Device") && IsClickyReadyByItemName("Primary Anchor Transport Device")) {
				sprintf_s(reloClicky, "Primary Anchor Transport Device");
			}
			else if (FindItemByName("Secondary Anchor Transport Device") && IsClickyReadyByItemName("Secondary Anchor Transport Device")) {
				sprintf_s(reloClicky, "Secondary Anchor Transport Device");
			}
			else {
				WriteChatf("\arDOH!\aw I don't have an anchor clicky that is ready");
				return;
			}
			if (FindItemByName(reloClicky)) {
				if (UseClickyByItemName(reloClicky)) {
				}
			}
			else {
				//This else for error checking and should never happen.
				WriteChatf("I don't have a %s", reloClicky);
			}
			return;
		}
		if (!_stricmp(Arg, "anchor1")) { // Use specifically Primary Anchor
			if (FindItemByName("Primary Anchor Transport Device") && IsClickyReadyByItemName("Primary Anchor Transport Device")) {
				sprintf_s(reloClicky, "Primary Anchor Transport Device");
			}
			else {
				WriteChatf("\arDOH!\aw I don't have an anchor clicky that is ready");
				return;
			}
			if (FindItemByName(reloClicky)) {
				if (UseClickyByItemName(reloClicky)) {
					//WriteChatf("Casting \ag %s \aw ", reloClicky);
				}
			}
			else {
				//This else for error checking and should never happen.
				WriteChatf("I don't have a %s", reloClicky);
			}
			return;
		}
		if (!_stricmp(Arg, "anchor2")) { // use specifically Secondary Anchor
			if (FindItemByName("Secondary Anchor Transport Device") && IsClickyReadyByItemName("Secondary Anchor Transport Device")) {
				sprintf_s(reloClicky, "Secondary Anchor Transport Device");
			}
			else {
				WriteChatf("\arDOH!\aw I don't have an anchor clicky that is ready");
				return;
			}
			if (FindItemByName(reloClicky)) {
				if (UseClickyByItemName(reloClicky)) {
					//WriteChatf("Casting \ag %s \aw ", reloClicky);
				}
			}
			else {
				//This else for error checking and should never happen.
				WriteChatf("I don't have a %s", reloClicky);
			}
			return;
		}
		if (!_stricmp(Arg, "fellow") || !_stricmp(Arg, "fellowship")) { // Use fellowship Insignia
			if (FindItemByName("Fellowship Registration Insignia") && IsClickyReadyByItemName("Fellowship Registration Insignia")) {
				if (pLocalPlayer && ((PSPAWNINFO)pLocalPlayer)->Campfire) {
					UseClickyByItemName("Fellowship Registration Insignia");
					return;
				}
				else {
					if (!(pLocalPlayer && ((PSPAWNINFO)pLocalPlayer)->Campfire)) {
						WriteChatf("\arYou do not have a campfire up\aw.");
						return;
					}
				}
			}
			if (!FindItemByName("Fellowship Registration Insignia")) {
				WriteChatf("\arYou do not have a Fellowship Registration Insigna!\aw");
				return;
			}
			if (FindItemByName("Fellowship Registration Insignia") && !IsClickyReadyByItemName("Fellowship Registration Insignia")) {
				WriteChatf("\arFellowship Registration Insignia is not ready!\aw");
				return;
			}
		}
		if (!_stricmp(Arg, "lobby")) {
			if (AltAbility("Throne of Heroes") && AltAbility("Throne of Heroes")->CurrentRank > 0 && AltAbilityReady("Throne of Heroes")) {
				canLobbyAA = true;
				return;
			}
			else {
				if (AltAbility("Throne of Heroes") && AltAbility("Throne of Heroes")->CurrentRank > 0 && !AltAbilityReady("Throne of Heroes")) {
					WriteChatf("\ayThrone of Heroes \arisn't ready right now\aw.");
				}
				else if (!AltAbility("Throne of Heroes") || AltAbility("Throne of Heroes")->CurrentRank == 0) {
					WriteChatf("\arI don't have the \ayThrone of Heroes \arAA\aw!");
				}
			}
			return;
		}
		if (!_stricmp(Arg, "blood")) {
			if (AltAbility("Harmonic Dissonance") && AltAbility("Harmonic Dissonance")->CurrentRank > 0 && AltAbilityReady("Harmonic Dissonance")) {
				canHarmonicAA = true;
				return;
			}
			else {
				if (AltAbility("Harmonic Dissonance") && AltAbility("Harmonic Dissonance")->CurrentRank > 0 && !AltAbilityReady("Harmonic Dissonance")) {
					WriteChatf("\ayHarmonic Dissonance \arisn't ready right now\aw.");
				}
				else if (!AltAbility("Harmonic Dissonance") || AltAbility("Harmonic Dissonance")->CurrentRank == 0) {
					WriteChatf("\arI don't have the \ayHarmonic Dissonance \arAA\aw!");
				}
			}
			return;
		}
		if (!_stricmp(Arg, "evac")) { // use evac AA if you have it
			DWORD classID = GetCharInfo2()->Class;
			switch (classID) {
			case EQData::Druid:
				if (GroupSize() > 1 && AltAbility("Exodus") && AltAbility("Exodus")->CurrentRank > 0 && AltAbilityReady("Exodus")) { // Exodus GROUP evac 43
					canGroupEvacAA = true;
					break;
				}
				if (AltAbility("Egress") && AltAbility("Egress")->CurrentRank > 0 && AltAbilityReady("Egress")) { // Egress SELF evac 8602
					canEvacAA = true;
					break;
				}
			case EQData::Necromancer:
				if (AltAbility("Levant") && AltAbility("Levant")->CurrentRank > 0 && AltAbilityReady("Levant")) { // Self Evac
					canEvacAA = true;
					break;
				}
			case EQData::Rogue:
				if (AltAbility("Stealthy Getaway") && AltAbility("Stealthy Getaway")->CurrentRank > 0 && AltAbilityReady("Stealthy Getaway")) { // Self Evac 789
					canEvacAA = true;
					break;
				}
			case EQData::Wizard:
				if (GroupSize() > 1 && AltAbility("Exodus") && AltAbility("Exodus")->CurrentRank > 0 && AltAbilityReady("Exodus")) { // Exodus GROUP evac 43
					canGroupEvacAA = true;
					break;
				}
				if (AltAbility("Abscond") && AltAbility("Abscond")->CurrentRank > 0 && AltAbilityReady("Abscond")) { // Abscond SELF evac
					canEvacAA = true;
					break;
				}
				WriteChatf("We did not break out of the case");
			default:
				break;
			}
			if (!canEvacAA && !canGroupEvacAA) {
				WriteChatf("\arI don't seem to have the ability to evac!\aw");
			}
			return;	
		}
		if (!_stricmp(Arg, "teleport")) { // use teleportAA
			if (AltAbility("Teleport") && AltAbility("Teleport")->CurrentRank > 0 && AltAbilityReady("Teleport")) {
				canTeleportAA = true;
				return;
			}
			else {
				if (AltAbility("Teleport") && AltAbility("Teleport")->CurrentRank > 0 && !AltAbilityReady("Teleport")) {
					WriteChatf("\ayTeleport \arisn't ready right now\aw.");
				}
				if (!AltAbility("Teleport") || AltAbility("Teleport")->CurrentRank == 0) {
					WriteChatf("\arI don't have the \ayTeleport \arAA\aw!");
				}
				return;			
			}
		}
	}
	WriteChatf("\arYou didn't provide a valid option for /relocate.\aw");
	WriteChatf("\arPlease do \"/relocate help\" for more information.\aw");
	return;
}

void TransloCmd(PSPAWNINFO pChar, PCHAR szLine) {
	if (!InGame())
		return;
	CHAR Arg[MAX_STRING] = { 0 };
	GetArg(Arg, szLine, 1);

	if (!_stricmp(Arg, "help")) {
		WriteChatf("\agValid Translocate options are:\aw");
		WriteChatf("\ay/translocate\aw \arRedBot\aw to Translocate \arRedBot\aw to their bind.");
		return;
	}
	DWORD classID = GetCharInfo2()->Class;
	if (classID == EQData::Wizard) { // If I am a wizard?
		if (!MyTarget()) {
			if (strlen(Arg) == 0) {
				WriteChatf(static_cast<char*>("\arPlease provide a target for translocate\aw."));
				WriteChatf(static_cast<char*>("\arYou can physically target a player, or provide a name like: \ag/translocate RedBot\aw."));
				return;
			}
			else {
				//Let us see if we can find the target that was provided as an argument.
				PSPAWNINFO desiredTarget = (PSPAWNINFO)GetSpawnByName(Arg);
				PSPAWNINFO me = GetCharInfo()->pSpawn;
				PSPELL pSpell = GetSpellByName("Translocate");
				bool haveAA = false;
				if (pSpell && AltAbility(pSpell->Name) && AltAbility(pSpell->Name)->CurrentRank > 0)
					haveAA = true;
				if (!desiredTarget || !haveAA || !me) {
					if (!desiredTarget)
						WriteChatf("\arI can't find a player with the name \ay%s\aw", Arg);
					if (!haveAA)
						WriteChatf("\arI don't have the AA Translocate");
					if (!me)
						WriteChatf("\arI'm not in game, knock that off!");
					return;
				}
				if (desiredTarget && me && GetDistance3D(desiredTarget->X, desiredTarget->Y, desiredTarget->Z, me->X, me->Y, me->Z) > pSpell->Range) {
					WriteChatf("\arIt seems \ay%s\aw is out of range of \ay%s\aw.", Arg, pSpell->Name); // double check this spits out both %s
					return;
				}
				TargetIt(desiredTarget);
				if (!MyTarget()) {
					WriteChatf("\arWe do not have a target.");
					return;
				}
				if (MyTarget() && MyTarget()->Type != SPAWN_PLAYER) {
					WriteChatf("\ay%s\aw \aris not a Player!\aw", MyTarget()->Type);
					return;
				}
				if (MyTargetID() == Me()->SpawnID) {
					WriteChatf("\arYou can't translocate yourself, knock it off!");
					return;
				}
				else {
					canTranslocate = true;
				}
			}
		} else {
			if (strlen(Arg) == 0) {
				PSPAWNINFO desiredTarget = (PSPAWNINFO)GetSpawnByName(MyTarget()->Name);
				PSPAWNINFO me = GetCharInfo()->pSpawn;
				PSPELL pSpell = GetSpellByName("Translocate");
				if (desiredTarget && me && GetDistance3D(desiredTarget->X, desiredTarget->Y, desiredTarget->Z, me->X, me->Y, me->Z) > pSpell->Range) {
					WriteChatf("\arIt seems \ay%s\aw \aris out of range of \ay%s\aw.", MyTarget()->Name, pSpell->Name); // double check this spits out both %s
					return;
				}
				if (MyTarget()->Type != SPAWN_PLAYER) {
					WriteChatf("\ay%s\aw \aris not a Player!\aw", MyTarget()->Name);
					return;
				}
				if (MyTargetID() == Me()->SpawnID) {
					WriteChatf("\arYou can't translocate yourself, knock it off!\aw");
					return;
				}
				canTranslocate = true;
				return;
			}
			else {
				PSPAWNINFO desiredTarget = (PSPAWNINFO)GetSpawnByName(Arg);
				PSPAWNINFO me = GetCharInfo()->pSpawn;
				PSPELL pSpell = GetSpellByName("Translocate");
				if (!desiredTarget) {
					WriteChatf("\arI can't find a player with the name >>> \ay%s\ar <<<\aw.", Arg);
					return;
				}
				if (desiredTarget && desiredTarget->Type != SPAWN_PLAYER) {
					WriteChatf("\ay%s\aw \aris not a player.\aw", desiredTarget);
					return;
				}
				if (desiredTarget->SpawnID == Me()->SpawnID) {
					WriteChatf("\arYou can't translocate yourself, knock it off!\aw");
					return;
				}
				if (desiredTarget && me && GetDistance3D(desiredTarget->X, desiredTarget->Y, desiredTarget->Z, me->X, me->Y, me->Z) > pSpell->Range) {
					WriteChatf("\arIt seems \ay%s\ar is out of range of \ay%s\aw.", Arg, pSpell->Name); // double check this spits out both %s
					return;
				}
				TargetIt(desiredTarget);
				if (!MyTarget()) {
					WriteChatf("We do not have a target.");
					return;
				}
				if (MyTarget()->Type != SPAWN_PLAYER) {
					WriteChatf("\ay%s\aw \aris not a Player!\aw", MyTarget()->Name);
					return;
				}
				canTranslocate = true;
				return;
			}
		}

	}
	WriteChatf("\arYou are not a Wizard! No Translocate for you\aw!");
}

//Check to see if an alias exists with the name of "ShortCommand"
bool HaveAlias(PCHAR ShortCommand) {
	std::string sName = ShortCommand;
	std::transform(sName.begin(), sName.end(), sName.begin(), tolower);
	if (mAliases.find(sName) != mAliases.end()) {
		return true;
	}
	return false;
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	VERSION = 1.0f;
	iPulseDelay = 75;
	if ((HaveAlias("/relo")) || (HaveAlias("/relocate"))) { //check our aliases
		WriteChatf("\ar[\a-tMQ2Relocate\ar]\ao:: \arIt appears you already have an Alias for \ap/relocate\ar  please type \"\ay/alias /relocate delete\ar\" then reload this plugin.");
		EzCommand("/timed 10 /plugin MQ2Relocate Unload");
	}
	if (HaveAlias("/translocate")) { //check our aliases
		WriteChatf("\ar[\a-tMQ2Relocate\ar]\ao:: \arIt appears you already have an Alias for \ap/translocate\ar  please type \"\ay/alias /translocate delete\ar\" then reload this plugin.");
		EzCommand("/timed 10 /plugin MQ2Relocate Unload");
	}
	else {
		AddCommand("/relo", ReloCmd);
		AddCommand("/relocate", ReloCmd);
		AddCommand("/translocate", TransloCmd);
	}
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	RemoveCommand("/relo");
	RemoveCommand("/relocate");
	RemoveCommand("/translocate");
}

// This is called every time MQ pulses (MainLOOP!)
PLUGIN_API VOID OnPulse(VOID)
{
	if (++iPulse < iPulseDelay) return;
	iPulse = 0;
	//Base Cases
	if (!InGame()) {
		return;
	}
	//DON'T FORGET TO CHANGE THE FREAKING BASE CASES SO THAT YOU ACTUALLY CONTINUE INTO THE FUNCTION! (WTB back many minutes of my life)
	if (!strlen(convertoption) && !bAmConverting && !needsUsing && !canGatePotion && !canGateAA && !canOriginAA && !canLobbyAA && !canHarmonicAA && !canEvacAA && !canGroupEvacAA && !canTranslocate && !canTeleportAA) {
		return;
	}
	if (ImDucking() || Casting() || Moving(GetCharInfo()->pSpawn)) return;

	char temp[MAX_STRING] = "";
	if (bAmConverting) {
		if (!FindItemByName(convertoption) && FindItemByName(reloClicky)) {
			sprintf_s(temp, "/convertitem %s", convertoption);
			WriteChatf("Converting Item"); // debug purposes
			EzCommand(temp);
		}
		else {
			//Converting is done at this point and I need to swap to using the item.
			bAmConverting = false;
			needsUsing = true;
		}
	}
	if (needsUsing && FindItemByName(convertoption)) {
		//This is where I trigger the usage of the converted item.
		sprintf_s(temp, "/useitem \"%s\"", convertoption);
		EzCommand(temp);
		WriteChatf("Relocating with: \ag%s\aw", convertoption);
		needsUsing = false;//turn off needsusing, because i've used it.
		sprintf_s(convertoption, "");//clear the convertitem string so that it's blank and ready for modification again.
	}
	if (canGateAA && AltAbilityReady("Gate")) {
		WriteChatf("Relocating with: \ayGate AA\aw.");
		char gateAA[16] = "/alt act 1217";
		EzCommand(gateAA);
		canGateAA = false;
	}
	if (canGatePotion) {
		if (UseClickyByItemName("Philter of Major Translocation")) {
			canGatePotion = false;
		}
	}
	if (canOriginAA && AltAbilityReady("Origin")) {
		WriteChatf("Relocating with: \ayOrigin AA\aw.");
		char originAA[16] = "/alt act 331";
		EzCommand(originAA);
		canOriginAA = false;
	}
	if (canLobbyAA && AltAbilityReady("Throne of Heroes")) {
		WriteChatf("Relocating to \agGuild Lobby\aw with: \ayThrone of Heroes AA\aw.");
		char lobbyAA[16] = "/alt act 511";
		EzCommand(lobbyAA);
		canLobbyAA = false;
	}
	if (canHarmonicAA && AltAbilityReady("Harmonic Dissonance")) {
		WriteChatf("Relocating to \agTheater of Blood\aw with: \ayHarmonic Dissonance AA\aw.");
		char harmonicAA[16] = "/alt act 511";
		EzCommand(harmonicAA);
		canHarmonicAA = false;
	}
	if (canEvacAA) { // check each evac AA
		if (AltAbilityReady("Steathly Getaway")) {
			WriteChatf("Self Evac with: \ayStealthy Getaway AA\aw.");
			char evacAA[16] = "/alt act 789";
			EzCommand(evacAA);
			canEvacAA = false;
		}
		if (AltAbilityReady("Abscond")) {
			WriteChatf("Self Evac with: \ayAbscond AA\aw.");
			char evacAA[16] = "/alt act 490";
			EzCommand(evacAA);
			canEvacAA = false;
		}
		if (AltAbilityReady("Egress")) {
			WriteChatf("Self Evac with: \ayEgress AA\aw.");
			char evacAA[16] = "/alt act 8602";
			EzCommand(evacAA);
			canEvacAA = false;
		}
		if (AltAbilityReady("Levant")) {
			WriteChatf("Self Evac with: \ayLevant AA\aw.");
			char evacAA[16] = "/alt act 2899";
			EzCommand(evacAA);
			canEvacAA = false;
		}
	}
	if (canGroupEvacAA) { // check each evac AA
		if (AltAbilityReady("Exodus")) {
			WriteChatf("Group Evac with: \ayExodus AA\aw.");
			char evacAA[16] = "/alt act 43";
			EzCommand(evacAA);
			canGroupEvacAA = false;
		}
	}
	if (canTranslocate && AltAbilityReady("Translocate")) {
		WriteChatf("Translocating \ay%s\aw with: \ayTranslocate AA\aw.", ((PSPAWNINFO)pTarget)->Name);
		char translocateAA[16] = "/alt act 9703";
		EzCommand(translocateAA);
		canTranslocate = false;
	}
	if (canTeleportAA && AltAbilityReady("Teleport")) {
		WriteChatf("Teleporting surrounding allies with: \ayTeleport AA\aw.");
		char teleportAA[16] = "/alt act 9704";
		EzCommand(teleportAA);
		canTeleportAA = false;
	}
}

bool UseClickyByItemName(PCHAR szItem) {
	if (FindItemCountByName(szItem)) {
		if (PCONTENTS item = FindItemByName(szItem)) {
			if (PITEMINFO pIteminf = GetItemFromContents(item)) {
				if ((PVOID)&GetItemFromContents(item)->Clicky) {
					if (pSpellMgr && ItemReady(pIteminf->Name)) {
						UseItem(pIteminf->Name);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool IsClickyReadyByItemName(PCHAR szItem) {
	if (FindItemCountByName(szItem)) {
		if (PCONTENTS item = FindItemByName(szItem)) {
			if (PITEMINFO pIteminf = GetItemFromContents(item)) {
				if ((PVOID)&GetItemFromContents(item)->Clicky) {
					if (pSpellMgr && ItemReady(pIteminf->Name)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool IsTargetPlayer(PCHAR szItem) {
	if (PSPAWNINFO target = (PSPAWNINFO)pTarget) {
		if (target->Type == SPAWN_PLAYER) {
			return true;
		}
	}
	return false;
}

// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState)
{
	//if (GameState==GAMESTATE_INGAME)
	if (GameState != GAMESTATE_INGAME) {
		if (bAmConverting) bAmConverting = false;
		if (needsUsing) needsUsing = false;
		if (strlen(convertoption)) sprintf_s(convertoption, "");
	}
}

/** THIS IS A BLOCK COMMENT!
// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState)
{
	//if (GameState==GAMESTATE_INGAME)
	//Wasn't sure if this would ever be needed, so holding onto it.
}

// This is called every time WriteChatColor is called by MQ2Main or any plugin,
// IGNORING FILTERS, IF YOU NEED THEM MAKE SURE TO IMPLEMENT THEM. IF YOU DONT
// CALL CEverQuest::dsp_chat MAKE SURE TO IMPLEMENT EVENTS HERE (for chat plugins)
PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter)
{
	//Wasn't sure if this would ever be needed, so hanging onto it.
	return 0;
}

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
// but after MQ filters and chat events are taken care of.
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
	//Wasn't sure if this would ever be needed, so hanging onto it.
	return 0;
}
**/