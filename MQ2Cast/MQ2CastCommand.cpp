#include <functional>

#include "MQ2Cast.h"

using namespace MQ2Cast;

#pragma region MemCommand

MemCommand* MemCommand::Create(const PCHAR ID, const int GemSlot, const std::list<const ImmediateCommand*> PreQueue, const std::list<const ImmediateCommand*> PostQueue) {
	if (!ID) return nullptr;
	return new MemCommand(GemSlot, GetSpellByName(ID), PreQueue, PostQueue);
}

void MemCommand::execute() const {
	if (CastingState::instance().getCurrentID() == NOID) CastingState::instance().setCurrentID(GemSlot);

	if (!pSpellBookWnd) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: No Book Window.", MQGetTickCount64()); }
		CastingState::instance().setCurrentResult(CastResult::Unknown);
	} else if (Spell) {
		if (SpellCommand::GetGem(Spell->ID) != -1) {
			// it's memmed, we're done
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Memmed.", MQGetTickCount64()); }
			CastingState::instance().setCurrentResult(CastResult::Success);
		} else if (!CastingState::instance().isImmobile()) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Moving While Memming.", MQGetTickCount64()); }
			CastingState::instance().doNext(new ImmobilizeCommand());
			CastingState::instance().setCurrentResult(CastResult::Idle);
		} else if (CastingState::instance().getCurrentResult() == CastResult::Idle && GemSlot > 0 && GemSlot <= NUM_SPELL_GEMS && GetCharInfo2()) {
			// we're idle and we have a valide gemslot to mem to, let's start memorizing
			for (DWORD spellIdx = 0; spellIdx < NUM_BOOK_SLOTS; ++spellIdx) {
				if (GetCharInfo2()->SpellBook[spellIdx] == Spell->ID) {
					CastingState::instance().setCurrentResult(CastResult::Memorizing);

					CHAR szBuffer[MAX_STRING] = { 0 };
					sprintf_s(szBuffer, "%d \"%s\"", GemSlot, Spell->Name);
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Cast = %s : szBuffer %s -- MemSpell()", MQGetTickCount64(), GetCharInfo()->pSpawn, szBuffer); }

					// set up a spellset that is all empty but the one we want
					SPELLFAVORITE CastSpellFavorite;
					CastSpellFavorite.inuse = 1;
					for (int i = 0; i < NUM_SPELL_GEMS; ++i) CastSpellFavorite.SpellId[i] = NOID;
					CastSpellFavorite.SpellId[GemSlot - 1] = Spell->ID;

					// and memorize it. all the NOID gems will remain untouched
					pSpellBookWnd->MemorizeSet((int*)&CastSpellFavorite, NUM_SPELL_GEMS);

					return; // bail here -- we found the spell
				}
			}

			// went through the whole spellbook and didn't find anything
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Spell Not In Book.", MQGetTickCount64()); }
			CastingState::instance().setCurrentResult(CastResult::Unknown);
		} else if (CastingState::instance().getCurrentResult() == CastResult::Idle) {
			// still idle, which means we failed the gem validation
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Invalid Gem.", MQGetTickCount64()); }
			CastingState::instance().setCurrentResult(CastResult::Unknown);
		} // else maintain the current result
	} else {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Spell Lookup Failed.", MQGetTickCount64()); }
		CastingState::instance().setCurrentResult(CastResult::Unknown);
	}

	// we've finished, close the book if its open.
	if (CastingState::instance().getCurrentResult() >= CastResult::Success && pSpellBookWnd && ((PCSIDLWND)pSpellBookWnd)->dShow) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Closebook.", MQGetTickCount64()); }
		if (auto cxWnd = (CXWnd*)pSpellBookWnd) {
			cxWnd->Show(0, 0);
		}
	}

}

#pragma endregion

#pragma region SpellCommand

SpellCommand* SpellCommand::Create(const PCHAR ID, const int GemSlot, const bool Wait, const std::list<const ImmediateCommand*> PreQueue, const std::list<const ImmediateCommand*> PostQueue) {
	if (!ID) return nullptr;

	// quick helper function since I can't bind ctors
	auto createCommand = [PreQueue, PostQueue](const int GemSlot, const DWORD SpellID, const bool Wait) {
		return new SpellCommand(GemSlot, SpellID, Wait, PreQueue, PostQueue);
	};

	// let's exploit the fact that GetSpellByName() will find the ID or the name
	if (auto pSpell = GetSpellByName(ID)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Found Spell: %s.", MQGetTickCount64(), pSpell->Name); }
		int gemIdx = GetGem(pSpell->ID);
		if (gemIdx != -1) {
			// we have the spell memorized
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Found Spell In Slot %d.", MQGetTickCount64(), gemIdx); }
			return createCommand(gemIdx + 1, pSpell->ID, Wait);
		} else if (GemSlot != -1) {
			// okay, it's not memmed, let's try to mem it first
			// push the mem command (queues are FIFO)
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Memming Spell In Slot %d.", MQGetTickCount64(), GemSlot); }
			CastingState::instance().pushCmd(MemCommand::Create(pSpell->Name, GemSlot));

			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Then Cast Command Generated.", MQGetTickCount64()); }
			// then return the function that will get pushed to the queue after the mem
			return createCommand(GemSlot, pSpell->ID, true);
		} else {
			// we don't have it memmed and we don't want to mem it, this will result in a NotReady
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Unmemmed Spell -- Not Casting.", MQGetTickCount64()); }
			return createCommand(-1, pSpell->ID, Wait);
		}
	}

	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Empty Cast Command Generated.", MQGetTickCount64()); }
	// spell lookup failed, this with result in Unknown
	return createCommand(-1, NOID, Wait);
}

long SpellCommand::GetCooldown(int GemSlot) {
	auto pMyPlayer = (PSPAWNINFO)pLocalPlayer;
	if (!pMyPlayer || GemSlot < 1 || GemSlot >= NUM_SPELL_GEMS)
		return 0xFFFFFFFF;

	return max(pMyPlayer->SpellGemETA[GemSlot - 1], pMyPlayer->SpellCooldownETA) - GetCharInfo()->pSpawn->TimeStamp;
}

int SpellCommand::GetGem(LONG spellID) {
	for (int gemIdx = 0; gemIdx < NUM_SPELL_GEMS; ++gemIdx) {


		if (PCHARINFO pCharInfo = GetCharInfo()) {
			if (pCharInfo->pCharacterBase) {
				if (CharacterBase *cb = (CharacterBase *)&pCharInfo->pCharacterBase) {
					auto gemID = cb->GetMemorizedSpell(gemIdx);

					if (gemID == spellID) {
						if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Gem]: Got Gem %d with spellID %d.", MQGetTickCount64(), gemID, spellID); }
						return gemIdx;
					}
				}
			}
		}
	}

	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Gem]: No Gem with spellID %d (CharInfo %d, CharBase %d).", MQGetTickCount64(), spellID, GetCharInfo(), GetCharInfo() ? GetCharInfo()->pCharacterBase : 0); }
	return -1;
}

int SpellCommand::GetGem(PCHAR ID) {
	if (strlen(ID) >= 3 && !_strnicmp(ID, "gem", 3)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Gem]: Parsing Gem As A String \"%s\".", MQGetTickCount64(), ID); }
		return atoi(&ID[3]);
	} else if (IsNumber(ID)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Gem]: Parsing Gem As A Number \"%s\".", MQGetTickCount64(), ID); }
		return atoi(ID);
	}

	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Gem]: Could not parse \"%s\".", MQGetTickCount64(), ID); }
	return -1;
}

void SpellCommand::execute() const {
	if (CastingState::instance().getCurrentID() == NOID) CastingState::instance().setCurrentID(SpellID);

	DWORD detectedID;

	if (SpellID == NOID) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Failed Spell Lookup.", MQGetTickCount64()); }
		CastingState::instance().setCurrentResult(CastResult::Unknown);
	} else if (GemSlot <= 0) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Failed Spell Mem.", MQGetTickCount64()); }
		CastingState::instance().setCurrentResult(CastResult::NotReady);
	} else if ((detectedID = CastingState::instance().detectSpellCast()) != NOID) {
		if (detectedID != SpellID) { // TODO: do we re-queue here?
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Different Spell Casting.", MQGetTickCount64()); }
			CastingState::instance().setCurrentResult(CastResult::NotReady);
		} else {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Already Casting Spell.", MQGetTickCount64()); }
			CastingState::instance().setCurrentResult(CastResult::Casting);
			BlechSpell(GetSpellByID(detectedID));
		}
	} else {
		if (auto pSpell = GetSpellByID(SpellID)) {
			if (!CastingState::instance().isImmobile() && !CastingState::instance().isBard()) {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Moving While Casting.", MQGetTickCount64()); }
				CastingState::instance().doNext(new ImmobilizeCommand());
				CastingState::instance().setCurrentResult(CastResult::Idle);
			} else if (Wait && GetCooldown(GemSlot) > 0) {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Spell Refresh Refresh.", MQGetTickCount64()); }
				CastingState::instance().setCurrentResult(CastResult::Memorizing);
			} else if (GetCooldown(GemSlot) <= 0) {
				CHAR szBuffer[MAX_STRING] = { 0 };
				sprintf_s(szBuffer, "%d", GemSlot);
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Cast = %s : szBuffer %s -- Cast()", MQGetTickCount64(), GetCharInfo()->pSpawn, szBuffer); }

				CastingState::instance().setCurrentResult(CastResult::Casting);
				BlechSpell(pSpell);

				// a lot of stuff happens here, let's just re-use all of it
				Cast(GetCharInfo()->pSpawn, szBuffer);
			} else {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Spell Not Ready.", MQGetTickCount64()); }
				CastingState::instance().setCurrentResult(CastResult::NotReady);
			}
		} else {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Failed Spell Lookup By ID.", MQGetTickCount64()); }
			CastingState::instance().setCurrentResult(CastResult::Unknown);
		}
	}
}

#pragma endregion

#pragma region AbilityCommand

AbilityCommand* AbilityCommand::Create(const PCHAR ID, const std::list<const ImmediateCommand*> PreQueue, const std::list<const ImmediateCommand*> PostQueue) {
	if (!ID) return nullptr;
	return new AbilityCommand(FindAbilityIndex(ID), PreQueue, PostQueue);
}

DWORD AbilityCommand::FindAbilityIndex(PCHAR ID) {
	if (IsNumber(ID)) {
		if (UINT abilityIdx = atoi(ID)) {
			if (abilityIdx < 7) {
				abilityIdx += 3;
			} else if (abilityIdx < 11) {
				abilityIdx -= 7;
			} else {
				return NOID;
			}

			if (EQADDR_DOABILITYLIST[abilityIdx] != 0xFFFFFFFF) {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[DoAbility]: Found Ability by Slot.", MQGetTickCount64()); }
				return EQADDR_DOABILITYLIST[abilityIdx];
			}
		}
	} else {
		for (DWORD abilityIdx = 0; szSkills[abilityIdx]; ++abilityIdx) {
			if (!_stricmp(ID, szSkills[abilityIdx])) {
				return abilityIdx;
			}
		}
	}

	return NOID;
}

void AbilityCommand::execute() const {
	if (CastingState::instance().getCurrentID() == NOID) CastingState::instance().setCurrentID(AbilityIndex);

	if (AbilityIndex == NOID) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[DoAbility]: Failed To Find Ability.", MQGetTickCount64()); }
		CastingState::instance().setCurrentResult(CastResult::Unknown);
	} else if (CastingState::instance().getCurrentResult() != CastResult::Idle) {
		// we attempted to use the ability, now it is used
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[DoAbility]: Ability Use Detected.", MQGetTickCount64()); }
		CastingState::instance().setCurrentResult(CastResult::Success);
	} else if (AbilityIndex != 105 && AbilityIndex != 107 && (!pCSkillMgr || !pCSkillMgr->IsAvailable(AbilityIndex))) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[DoAbility]: Ability Not Ready.", MQGetTickCount64()); }
		CastingState::instance().setCurrentResult(CastResult::NotReady);
	} else if ((AbilityIndex == 105 || AbilityIndex == 107) && !LoH_HT_Ready()) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[DoAbility]: LoH/HT Not Ready.", MQGetTickCount64()); }
		CastingState::instance().setCurrentResult(CastResult::NotReady);
	} else {
		CastingState::instance().setCurrentResult(CastResult::Casting);

		CHAR szBuffer[MAX_STRING] = { 0 };
		sprintf_s(szBuffer, "%d", AbilityIndex);
		if (DEBUGGING) { WriteChatf("Cast = %s : szBuffer %s -- DoAbility()", GetCharInfo()->pSpawn, szBuffer); }
		DoAbility(GetCharInfo()->pSpawn, szBuffer); // TODO: can I get rid of this?
	}
}

#pragma endregion

#pragma region DiscCommand

DiscCommand* DiscCommand::Create(const PCHAR ID, const std::list<const ImmediateCommand*> PreQueue, const std::list<const ImmediateCommand*> PostQueue) {
	if (!ID) return nullptr;

	auto returnCmd = new DiscCommand(FindDiscID(ID), PreQueue, PostQueue);
	returnCmd->DiscCmd = FindEQCommand("/disc");

	return returnCmd;
}

DWORD DiscCommand::FindDiscID(PCHAR ID) {
	if (IsNumber(ID)) {
		int caIdx = atoi(ID) - 1;
		if (caIdx >= 0 && caIdx < NUM_COMBAT_ABILITIES && pCombatSkillsSelectWnd->ShouldDisplayThisSkill(caIdx)) {
			return pPCData->GetCombatAbility(caIdx);
		}
	} else {
		for (int caIdx = 0; caIdx < NUM_COMBAT_ABILITIES; ++caIdx) {
			if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(caIdx)) {
				if (auto pSpell = GetSpellByID(pPCData->GetCombatAbility(caIdx))) {
					if (!_stricmp(ID, pSpell->Name)) {
						return pSpell->ID;
					}
				}
			}
		}
	}

	return NOID;
}

ULONG DiscCommand::GetDiscTimer(PSPELL pSpell) {
#ifndef EMU
	return pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex, pSpell->SpellGroup);
#else
	return pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex);
#endif
}

void DiscCommand::execute() const {
	if (CastingState::instance().getCurrentID() == NOID) CastingState::instance().setCurrentID(SpellID);

	DWORD detectedID;

	if (SpellID == NOID) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/disc]: Failed Disc Lookup.", MQGetTickCount64()); }
		CastingState::instance().setCurrentResult(CastResult::Unknown);
	} else if ((detectedID = CastingState::instance().detectSpellCast()) != NOID) {
		if (detectedID != SpellID) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/disc]: Different Spell Casting.", MQGetTickCount64()); }
			CastingState::instance().setCurrentResult(CastResult::NotReady);
		} else {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/disc]: Already Casting Disc.", MQGetTickCount64()); }
			CastingState::instance().setCurrentResult(CastResult::Casting);
			BlechSpell(GetSpellByID(detectedID));
		}
	} else {
		if (auto pSpell = GetSpellByID(SpellID)) {
			if (GetDiscTimer(pSpell) < (DWORD)time(NULL)) {
				if (DEBUGGING) { WriteChatf("Cast = %d : Spell %s -- /disc", GetCharInfo()->pSpawn, pSpell->Name); }

				CastingState::instance().setCurrentResult(CastResult::Casting);
				BlechSpell(pSpell);

				// TODO: Is this by ID or name?
				Execute(DiscCmd, "%d", pSpell->ID);
			} else {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/disc]: Disc Not Ready.", MQGetTickCount64()); }
				CastingState::instance().setCurrentResult(CastResult::NotReady);
			}
		} else {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/disc]: Failed Disc Lookup By ID.", MQGetTickCount64()); }
			CastingState::instance().setCurrentResult(CastResult::Unknown);
		}
	}
}

#pragma endregion

#pragma region AltAbilityCommand

AltAbilityCommand* AltAbilityCommand::Create(const PCHAR ID, const std::list<const ImmediateCommand*> PreQueue, const std::list<const ImmediateCommand*> PostQueue) {
	if (!ID) return nullptr;

	auto idxPair = GetAAIndex(ID);
	auto returnCmd = new AltAbilityCommand(idxPair.first, idxPair.second, PreQueue, PostQueue);

	returnCmd->AltCmd = FindEQCommand("/alt");

	return returnCmd;
}

std::pair<DWORD, int> AltAbilityCommand::GetAAIndex(PCHAR ID) {
	int level = -1;

	DWORD AAIndex;
	if (IsNumber(ID)) {
		AAIndex = GetAAIndexByID(atoi(ID));
	} else {
		AAIndex = GetAAIndexByName(ID);
		if (auto pMe = (PSPAWNINFO)pLocalPlayer) {
			level = pMe->Level;
		}
	}

	return std::make_pair(AAIndex, level);
}

void AltAbilityCommand::execute() const {
	if (!PlayerHasAAAbility(AAIndex)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/alt act]: Player Does Not Have AA %d.", MQGetTickCount64(), AAIndex); }
		CastingState::instance().setCurrentResult(CastResult::Unknown);
	} else {
		if (auto pAbility = GetAAByIdWrapper(AAIndex, Level)) {
			CastingState::instance().setCurrentID(pAbility->SpellID);
			if (pAltAdvManager && pAltAdvManager->IsAbilityReady(pPCData, pAbility, 0)) {
				if (DEBUGGING) { WriteChatf("Cast = %s : id %d -- /alt act", GetCharInfo()->pSpawn, pAbility->ID); }
				CastingState::instance().setCurrentResult(CastResult::Casting);
				Execute(AltCmd, "act %d", pAbility->ID);
			} else {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/alt act]: AA Not Ready.", MQGetTickCount64()); }
				CastingState::instance().setCurrentResult(CastResult::NotReady);
			}
		} else {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/alt act]: Failed AA Lookup %d %d.", MQGetTickCount64(), AAIndex, Level); }
			CastingState::instance().setCurrentResult(CastResult::Unknown);
		}
	}
}

#pragma endregion

#pragma region ItemCommand

ItemCommand* ItemCommand::Create(const PCHAR ID, const std::list<const ImmediateCommand*> PreQueue, const std::list<const ImmediateCommand*> PostQueue) {
	if (!ID) return nullptr;
	return new ItemCommand(GetContents(ID), PreQueue, PostQueue);
}

PCONTENTS ItemCommand::GetContents(PCHAR ID) {
	PCONTENTS pCont = nullptr;
	if (IsNumber(ID)) {
		pCont = FindItemByID(atoi(ID));
	} else {
		CHAR szName[MAX_STRING] = { 0 };
		strcpy_s(szName, ID);
		bool exact = StripQuotes(szName);
		pCont = FindItemByName(szName, exact);
	}

	return pCont;
}

void ItemCommand::execute() const {
	if (!Contents) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Item]: Failed To Lookup Item.", MQGetTickCount64()); }
		CastingState::instance().setCurrentResult(CastResult::Unknown);
	} else if (PITEMINFO pItem = GetItemFromContents(Contents)) {
		auto pMyChar = GetCharInfo();
		if (!pMyChar) return;

		CastingState::instance().setCurrentID(pItem->Clicky.SpellID);

		CHAR szBuffer[MAX_STRING] = { 0 };
		sprintf_s(szBuffer, "\"%s\"", pItem->Name); // we have the exact name, so use it (UseItemCommand() uses quotes for exact names)
		auto f = std::bind(UseItemCmd, pMyChar->pSpawn, szBuffer);

		if (pItem->Clicky.TimerID != -1) { // there is a timer, so we know it's not instant click
			if (GetItemTimer(Contents) == 0) { // the timer is 0, so it's clickable
				if (DEBUGGING) { WriteChatf("Cast = %s : szBuffer %s -- UseItemCmd() Timer Ready.", pMyChar->pSpawn, szBuffer); }
				CastingState::instance().setCurrentResult(CastResult::Casting);
				f();
			} else {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Item]: Item Timer Not Ready.", MQGetTickCount64()); }
				CastingState::instance().setCurrentResult(CastResult::NotReady);
			}
		} else if (pItem->Clicky.SpellID != NOID) { // there was no timer, but if there's a spellID, it's instant
			if (DEBUGGING) { WriteChatf("Cast = %s : szBuffer %s -- UseItemCmd() Instant Click.", pMyChar->pSpawn, szBuffer); }
			CastingState::instance().setCurrentResult(CastResult::Casting);
			f();
		}
	}
}

#pragma endregion
