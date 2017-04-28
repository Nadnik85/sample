#include "Character.h"

#define ISINDEX() (Index[0])
#define ISNUMBER() (IsNumber(Index))
#define GETNUMBER() (atoi(Index))
#define GETFIRST() Index
#define pZone ((PZONEINFO)pZoneInfo)

char* stristr(char* str1, const char* str2)
{
	char* p1 = str1;
	const char* p2 = str2;
	char* r = *p2 == 0 ? str1 : 0;

	while (*p1 != 0 && *p2 != 0)
	{
		if (tolower(*p1) == tolower(*p2))
		{
			if (r == 0)
			{
				r = p1;
			}

			p2++;
		}
		else
		{
			p2 = str2;
			if (tolower(*p1) == tolower(*p2))
			{
				r = p1;
				p2++;
			}
			else
			{
				r = 0;
			}
		}

		p1++;
	}

	return *p2 == 0 ? r : 0;
}

Character::Character()
{
	for (int x = 0; x < 16; x++)
		MemorizedSpells[x] = NULL;
}
Character::~Character() {}

bool Character::Standing()
{
	return GetCharInfo()->pSpawn->StandState == STANDSTATE_STAND ? true : false;
}

void Character::UpdateMemorizedSpells()
{
	for (int x = 0; x < 16; x++)
	{
		PSPELL pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[x]);
		if (pSpell)
			MemorizedSpells[x] = pSpell;
		else
			MemorizedSpells[x] = NULL;
	}
}

bool Character::Sitting()
{
	return GetCharInfo()->pSpawn->StandState == STANDSTATE_SIT ? true : false;
}

bool Character::OnMount()
{
	return GetCharInfo()->pSpawn->Mount == NULL ? false : GetCharInfo()->pSpawn->Mount->SpawnID > 0 ? true : false;
}

bool Character::Moving()
{
	//return fabs(info->pSpawn->SpeedRun) > 0.0f ? true : false;
	if (pCharSpawn && GetCharInfo() && GetCharInfo()->pSpawn) {
		return ((((gbMoving) && ((PSPAWNINFO)pCharSpawn)->SpeedRun == 0.0f) && (GetCharInfo()->pSpawn->Mount == NULL)) || (fabs(FindSpeed((PSPAWNINFO)pCharSpawn)) > 0.0f)) ? true : false;
	}
	return false;
}

bool Character::FeetWet()
{
	return (GetCharInfo()->pSpawn->FeetWet == 5) ? true : false;
}

DWORD Character::GetPetID()
{
	if (!GetCharInfo()->pSpawn) return 0;
	return GetCharInfo()->pSpawn->PetID;
}

bool Character::AnyBuff(PCHAR _BuffName, bool _Partial)
{
	if (ShortBuff(_BuffName, _Partial)) return true;
	if (Buff(_BuffName, _Partial)) return true;
	return false;
}

bool Character::Buff(PCHAR _BuffName, bool _Partial)
{
	if (!_BuffName) return false;
	if (!_BuffName[0]) return false;
	if (IsNumber(_BuffName))
	{
		unsigned long nBuff = atoi(_BuffName) - 1;
		if (nBuff >= NUM_LONG_BUFFS)
			return false;
		if (GetCharInfo2()->Buff[nBuff].SpellID <= 0)
			return false;
		//Dest.Ptr = &GetCharInfo2()->Buff[nBuff];
		//Dest.Type = pBuffType;
		return true;
	}
	else
	{
		for (unsigned long nBuff = 0; nBuff < NUM_LONG_BUFFS; nBuff++)
		{
			if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->Buff[nBuff].SpellID))
			{
				if (!_strnicmp(_BuffName, pSpell->Name, strlen(_BuffName)))
				{
					//Dest.Ptr = &GetCharInfo2()->Buff[nBuff];
					//Dest.Type = pBuffType;
					return true;
				}
				else if ((_Partial) && (stristr(pSpell->Name, _BuffName) != NULL))
					return true;
			}
		}
	}
	return false;
}

bool Character::ShortBuff(PCHAR _BuffName, bool _Partial)
{
	if (!_BuffName) return false;
	if (!_BuffName[0]) return false;
	if (IsNumber(_BuffName))
	{
		unsigned long nBuff = atoi(_BuffName) - 1;
		if (nBuff >= NUM_SHORT_BUFFS)
			return false;
		if (GetCharInfo2()->ShortBuff[nBuff].SpellID <= 0)
			return false;
		//Dest.Ptr = &GetCharInfo2()->Buff[nBuff];
		//Dest.Type = pBuffType;
		return true;
	}
	else
	{
		for (unsigned long nBuff = 0; nBuff < NUM_SHORT_BUFFS; nBuff++)
		{
			if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->ShortBuff[nBuff].SpellID))
			{
				if (!_strnicmp(_BuffName, pSpell->Name, strlen(_BuffName)))
				{
					//Dest.Ptr = &GetCharInfo2()->Buff[nBuff];
					//Dest.Type = pBuffType;
					return true;
				}
				else if ((_Partial) && (stristr(pSpell->Name, _BuffName) != NULL))
					return true;
			}
		}
	}
	return false;
}

bool Character::TargetBuff(PCHAR _BuffName, bool _Partial)
{
	if (!_BuffName) return false;
	if (!_BuffName[0]) return false;
	if (!pTarget) return false;
	if (!pTargetWnd) return false;
	if (IsNumber(_BuffName))
	{
		unsigned long nBuff = atoi(_BuffName);
		for (int y = 0; y < NUM_BUFF_SLOTS; y++)
		{
			int buffid = ((PCTARGETWND)pTargetWnd)->BuffSpellID[y];
			if (buffid == nBuff)
				return true;
		}
	}
	else
	{
		for (int y = 0; y < NUM_BUFF_SLOTS; y++)
		{
			int buffid = ((PCTARGETWND)pTargetWnd)->BuffSpellID[y];
			if (buffid > 0)
			{
				if (PSPELL pSpell = GetSpellByID(buffid))
				{
					if (!_strnicmp(_BuffName, pSpell->Name, strlen(_BuffName)) == 0)
						return true;
					else if ((_Partial) && (stristr(pSpell->Name, _BuffName) != NULL))
						return true;
				}
			}
		}
	}
	return false;
}

bool Character::CombatState(char *&state)
{
	if (!pPlayerWnd)
	{
		state = NULL;
		return false;
	}
	switch (((PCPLAYERWND)pPlayerWnd)->CombatState)
	{
	case 0:
		if (((CXWnd*)pPlayerWnd)->GetChildItem("PW_CombatStateAnim"))
		{
			state = "COMBAT";
			break;
		}
		state = "NULL";
		break;
	case 1:
		state = "DEBUFFED";
		break;
	case 2:
		state = "COOLDOWN";
		break;
	case 3:
		state = "ACTIVE";
		break;
	case 4:
		state = "RESTING";
		break;
	default:
		state = "UNKNOWN";
		break;
	}
	return true;
}

int Character::XTargetHaters(int *level, bool Named, float Dist)
{
	int ret = 0;
	int Level = 0;
	PCHARINFO info = GetCharInfo();
	if (ExtendedTargetList *xtl = info->pXTargetMgr) { // XTarget Tank takes priority over group tank...
		for (int x = 0; x < (int)xtl->XTargetSlots.Count; x++) {
			XTARGETSLOT xts = xtl->XTargetSlots[x];
			if ((xts.xTargetType != XTARGET_SPECIFIC_NPC) && (xts.xTargetType != XTARGET_SPECIFIC_PC)) {
				if(EQPlayer *spawn = GetSpawnByID(xts.SpawnID)) {
					if (Named) {
						if (IsNamed(spawn->Data.mPlayerPhysicsClient.pSpawn)) {
							if (spawn->Data.mPlayerPhysicsClient.pSpawn->Level > Level) {
								Level = spawn->Data.mPlayerPhysicsClient.pSpawn->Level;
							}
							ret++;
						}
					} else {
						if (spawn->Data.mPlayerPhysicsClient.pSpawn->Level > Level) {
							Level = spawn->Data.mPlayerPhysicsClient.pSpawn->Level;
						}
						ret++;
					}
				}
			}
		}
	}
	if (level) *level = Level;
	return ret;
}

int Character::XTargetNamed(int *level)
{
	return XTargetHaters(level, true);
}

bool Character::InCombat()
{
	char *state = NULL;
	if (CombatState(state))
		if (_strnicmp(state, "COMBAT", strlen(state)) == 0) return true;
	if (XTargetHaters()) return true;
	return false;
	//return Evaluate("${Me.CombatState.NotEqual[COMBAT]}") ? true : false;
}

bool Character::Invis()
{
	return GetCharInfo()->pSpawn->HideMode ? true : false;
}

int Character::PctMana()
{
	return Pct(GetCharInfo2()->Mana, GetMaxMana());
}

int Character::Pct(int cur, int max)
{
	if (max != 0)
		return cur * 100 / max;
	else
		return 0;
}

int Character::PctHPs()
{
	return Pct(GetCurHPS(), GetMaxHPS());
}

// Stolen from MQ2CastTimer
/*
long Character::CastingLeft() {
	long CL = 0;
	if (pCastingWnd && (PCSIDLWND)pCastingWnd->dShow) {
		CL = GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp;
		if (CL<1) CL = 1;
	}
	return CL;
}
*/
long Character::CastingLeft()
{
	long CL = 0;
	if (pCastingWnd && (PCSIDLWND)pCastingWnd->dShow)
	{
		CL = GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp;
		if (CL<0) CL = 0;
	}
	return CL;
}

long Character::CastingCoolDown()
{
	return ((PCDISPLAY)pDisplay)->TimeStamp <= ((PSPAWNINFO)pLocalPlayer)->SpellCooldownETA ? 1 : 0;
}

bool Character::CastReady()
{
	return Evaluate("${Cast.Ready}") ? true : false;
	//return CastingLeft() == 0 ? true : false;
}

bool Character::GetSpellRankName(PCHAR spell)
{
	PSPELL thespell = GetSpellByName(spell);
	for (DWORD nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++) {
		if (GetCharInfo2()->SpellBook[nSpell] != 0xFFFFFFFF)
		{
			if (PSPELL pFoundSpell = GetSpellByID(GetCharInfo2()->SpellBook[nSpell])) {
				if (pFoundSpell->SpellGroup == thespell->SpellGroup && !_strnicmp(thespell->Name, pFoundSpell->Name, strlen(thespell->Name)))
				{
					spell = pFoundSpell->Name;
					return true;
				}
			}
		}
	}
	return false;
}

bool Character::Indoors()
{
	return (pZone->OutDoor==EOutDoor::IndoorDungeon || pZone->OutDoor == EOutDoor::IndoorCity || pZone->OutDoor == EOutDoor::DungeonCity) ? true : false;
}

bool Character::Outdoors()
{
	return !Indoors();
}

bool Character::IsItemReady(PCHAR Index, float Range)
{
	PCONTENTS pItemContents = NULL;
	PITEMINFO pItem = NULL;
	if (pItemContents = FindItemByName(Index))
	{
		pItem = GetItemFromContents(pItemContents);
		if (pItem)
		{
			if (pItem->Clicky.TimerID != 0xFFFFFFFF)
			{
				if (GetItemTimer(pItemContents) > 0)
					return false; // Item is in cooldown timer...
			}
			if ((pItem->Type != ITEMTYPE_NORMAL) || (pItemContents->Charges > 0))
			{
				PSPELL pSpell = GetSpellByID(GetItemFromContents(pItemContents)->Clicky.SpellID);
				if (pSpell->Range > Range)
					return true;
			}
		}
	}
	return false;
}

bool Character::IsSpellReady(PCHAR Index, float Range)
{
	if (!pCastSpellWnd) return false;
	PEQCASTSPELLWINDOW pSpellWindow = (PEQCASTSPELLWINDOW)pCastSpellWnd;
	if (ISINDEX())
	{
		if (ISNUMBER())
		{
			// numeric 
			unsigned long nGem = GETNUMBER() - 1;
			if (nGem < NUM_SPELL_GEMS)
			{
				if (!pSpellWindow->SpellSlots[nGem])
					return false;
				else
					if (Range == 0)
					{
						PSPELL spell = GetSpellByID(GetCharInfo2()->MemorizedSpells[nGem]);
						if (spell->Mana > GetCharInfo2()->Mana) return false; // Not enough mana to cast it...
						else return (pSpellWindow->SpellSlots[nGem]->spellstate != 1);
/*						else if(((PCDISPLAY)pDisplay)->TimeStamp > ((PSPAWNINFO)pLocalPlayer)->SpellGemETA[nGem] && ((PCDISPLAY)pDisplay)->TimeStamp > ((PSPAWNINFO)pLocalPlayer)->SpellCooldownETA) {
							
							return true;
						}
						else return false;
*/							
					}
					else
					{
						bool ready = (pSpellWindow->SpellSlots[nGem]->spellstate != 1) ? true : false;
/*						bool ready = false;
						if (((PCDISPLAY)pDisplay)->TimeStamp > ((PSPAWNINFO)pLocalPlayer)->SpellGemETA[nGem] && ((PCDISPLAY)pDisplay)->TimeStamp > ((PSPAWNINFO)pLocalPlayer)->SpellCooldownETA) {
							ready = true;
						}*/
						if (!ready) return false;
						PSPELL spell = GetSpellByID(GetCharInfo2()->MemorizedSpells[nGem]);
						if (!spell) return false;
						if (spell->Mana > GetCharInfo2()->Mana) return false; // Not enough mana to cast it...
						return (spell->Range >= Range) ? true :  false;
					}
			}
		}
		else
		{
			for (unsigned long nGem = 0; nGem < NUM_SPELL_GEMS; nGem++)
			{
				if (PSPELL pSpell = GetSpellByID(GetCharInfo2()->MemorizedSpells[nGem]))
				{
					if (!_strnicmp(GETFIRST(), pSpell->Name, strlen(GETFIRST())))
					{
						if (!pSpellWindow->SpellSlots[nGem])
							return false;
						else
						{
							if (pSpell->Mana > GetCharInfo2()->Mana) return false; // Not enough mana to cast it...
							if (Range == 0)
								if (pSpellWindow->SpellSlots[nGem]->spellstate != 1)
								//if (((PCDISPLAY)pDisplay)->TimeStamp > ((PSPAWNINFO)pLocalPlayer)->SpellGemETA[nGem] && ((PCDISPLAY)pDisplay)->TimeStamp > ((PSPAWNINFO)pLocalPlayer)->SpellCooldownETA)
									return true;
								else
									return false;
							else
							{
								bool ready = (pSpellWindow->SpellSlots[nGem]->spellstate != 1) ? true : false;
/*								bool ready = false;
								if (((PCDISPLAY)pDisplay)->TimeStamp > ((PSPAWNINFO)pLocalPlayer)->SpellGemETA[nGem] && ((PCDISPLAY)pDisplay)->TimeStamp > ((PSPAWNINFO)pLocalPlayer)->SpellCooldownETA) {
									ready = true;
								}*/
								if (!ready) return false;
								return (pSpell->Range >= Range) ? true : false;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool Character::IsAAReady(PCHAR Index, float Range)
{
	if (!pAltAdvManager)
		return false;
	if (ISINDEX())
	{
		if (ISNUMBER())
		{
			//numeric
			for (unsigned long nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++) {
#ifndef EMU
				if (PALTABILITY pAbility = pAltAdvManager->GetAAById(pPCData->GetAlternateAbilityId(nAbility), Level())) {
#else
				if (PALTABILITY pAbility = pAltAdvManager->GetAAById(pPCData->GetAlternateAbilityId(nAbility))) {
#endif
					if (pAbility->ID == GETNUMBER()) {
						if (pAltAdvManager->IsAbilityReady(pPCData, pAbility, 0))
							if (pAbility->SpellID)
							{
								PSPELL pSpell = GetSpellByID(pAbility->SpellID);
								return (pSpell->Range >= Range) ? true : false;
							}
							else
								return true;
						else
							return false;
					}
				}
			}
		}
		else {
			// by name
			for (unsigned long nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++) {
#ifndef EMU
				if (PALTABILITY pAbility = pAltAdvManager->GetAAById(pPCData->GetAlternateAbilityId(nAbility), Level())) {
#else
				if (PALTABILITY pAbility = pAltAdvManager->GetAAById(pPCData->GetAlternateAbilityId(nAbility))) {
#endif
					if (PCHAR pName = pCDBStr->GetString(pAbility->nName, 1, NULL)) {
						if (!_strnicmp(GETFIRST(), pName, strlen(GETFIRST()))) {
							if (pAltAdvManager->IsAbilityReady(pPCData, pAbility, 0))
								return true;
							else
								return false;
						}
					}
				}
			}
		}
	}
	return false;
}
long Character::Evaluate(PCHAR zFormat, ...)
{
	char zOutput[MAX_STRING] = { 0 };
	va_list vaList;
	va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);
	if (!zOutput[0]) return -1;	//Error - no input data...
	ParseMacroData(zOutput, MAX_STRING);
	if (IsNumber(zOutput))
		return atoi(zOutput);
	else
	{
		if (_strnicmp(zOutput, "true", 4) == 0)
			return 1;
		else if (_strnicmp(zOutput, "false", 5) == 0)
			return 0;
	}
	return -3;	// Error - unknown output...
}

int Character::Level()
{
	if (GetCharInfo2())
		return GetCharInfo2()->Level;
	if (GetCharInfo())
		return GetCharInfo()->pSpawn->Level;
	return 0;
}

bool Character::MedCheck()
{
	return false;
}



PlayerClass Character::Class()
{
	return (PlayerClass)GetCharInfo2()->Class;
}

bool Character::SpellTypeReady(string &SpellName, vector <string>SpellList, float Range)
{
	for (int x = 0; x < (int)SpellList.size(); x++)
		if (IsSpellReady((PCHAR)SpellList[x].c_str(), Range))
		{
			//WriteChatf("DEBUG: <%s> is ready to cast!", SpellList[x].c_str());
			SpellName = SpellList[x];
			//WriteChatf("DEBUG: <%s> is ready to cast!", SpellName.c_str());
			return true;
		}
	return false;
}

bool Character::AATypeReady(string &SpellName, vector <string>AAList, float Range)
{
	for (int x = 0; x < (int)AAList.size(); x++)
		if (IsAAReady((PCHAR)AAList[x].c_str(), Range))
		{
			//WriteChatf("DEBUG: <%s> is ready to cast!", AAList[x].c_str());
			SpellName = AAList[x];
			//WriteChatf("DEBUG: <%s> is ready to cast!", SpellName.c_str());
			return true;
		}
	return false;
}

bool Character::ItemTypeReady(string &ItemName, vector <string>ItemList, float Range)
{
	for (int x = 0; x < (int)ItemList.size(); x++)
		if (IsItemReady((PCHAR)ItemList[x].c_str(), Range))
		{
			//WriteChatf("DEBUG: <%s> is ready to cast!", AAList[x].c_str());
			ItemName = ItemList[x];
			//WriteChatf("DEBUG: <%s> is ready to cast!", SpellName.c_str());
			return true;
		}
	return false;
}

