//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Projet: MQ2Cast.cpp    | Set DEBUGGING to true for DEBUGGING msg
// Author: s0rCieR        | 
//         A_Enchanter_00 |
//         htw            |
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Last edited by: devestator 10/30/2010 Updated for HoT & MQ2BagWindow
// Last edited by: Maskoi  4/15/2011 ->item changes
// 10.0 edited by: EqMule  12/14/2013 Removed MQ2BagWindow dependecy - Sponsored by RedGuides.com
// Since a recent patch, I think that there are very few items that require a swap/must equip to be cast.
// Therefore, this version does NOT swap stuff in and out of bags and/or use bandolier anymore.
// If you need that in your macro, use mq2exchange or /itemnotify to move/swap things. (or a version prior to 9.11)
// I also, made this plugin debugable again.
// Please future editors, keep the {} in the if's and while's
// Cause debugging without them is a total pain. (cant set bps if the return is on same line for example)
// 10.02 edited by: three-p-o 1/12/2014 switched over item casting to use EQ's own /cast command.
//    Fixed issue in CastHandle with {} not matching up properly. Resolves issue with cast returning before casting is completed.
// Also added in my changes to return CAST_UNKNOWN if you are trying to cast or memorize a spell that is not in your book.
// 10.03 edited by: three-p-o 3/23/2014  Updated for MQ2-20140322
// 10.04 edited by: trev 3/28/2014  Fixed: spells not getting memmed
// 10.05 edited by: eqmule 2/17/2016  new spell system
// 11.0 - Eqmule 07-22-2016 - Added string safety.
// 11.1 - Eqmule 08-22-2017 - Dont check Twisting if Mq2Twist is NOT loaded and some other tweaks to improve performance.
// 12.0 - dannuic 06-10-2018 - Complete refactor, major version upgrade.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#include <functional>
#include <limits>

#include "MQ2Cast.h"

#include "../Blech/Blech.h"

using namespace MQ2Cast;

#ifdef PLUGIN_API
PreSetup("MQ2Cast");
PLUGIN_VERSION(12.0);
#endif

bool MQ2Cast::DEBUGGING = false;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
bool         Parsed = false;            // BTree List Found Flags
Blech         LIST013('#');            // BTree List for OnChat Message on Color  13
Blech         LIST264('#');            // BTree List for OnChat Message on Color 264
Blech         LIST289('#');            // BTree List for OnChat Message on Color 289
Blech         UNKNOWN('#');            // BTree List for OnChat Message on UNKNOWN Yet Color
Blech         SUCCESS('#');            // BTree List for OnChat Message on SUCCESS Detection

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#pragma region BlechDefinitions

#define aCastEvent(List, Value, Filter) List.AddEvent(Filter, CastEvent, (void*)Value);
void __stdcall CastEvent(unsigned int ID, void *pData, PBLECHVALUE pValues)
{
	Parsed = true;

	// the joys of void pointers...
	CastResult eventResult = (CastResult)(long)(pData);

	if (CastingState::instance().getCurrentResult() < eventResult) {
		CastingState::instance().setCurrentResult(eventResult);
	}

	if (DEBUGGING) {
		WriteChatf("[%I64u] MQ2Cast:[OnChat]: Result=[%d] Called=[%d].", MQGetTickCount64(), GetReturnString(CastingState::instance().getCurrentResult()), (long)pData);
	}
}

// This function removes and re-inserts the Blech filters for the currently casting spell
void MQ2Cast::BlechSpell(PSPELL Cast) {
	SUCCESS.Reset();
	if (DEBUGGING) { WriteChatf("BlechSpell(Cast) = %d", Cast); }

	if (Cast) {
		char szBuffer[MAX_STRING];
		bool Added = false;

		/*CastByMe,CastByOther,CastOnYou,CastOnAnother,WearOff*/
		if (char *str = GetSpellString(Cast->ID, 2)) { // CastOnYou
			sprintf_s(szBuffer, "%s#*#", str);
			aCastEvent(SUCCESS, CastResult::Success, szBuffer);
			Added = true;
		}

		if (char *str = GetSpellString(Cast->ID, 3)) { // CastOnAnother
			sprintf_s(szBuffer, "#*#%s#*#", str);
			aCastEvent(SUCCESS, CastResult::Success, szBuffer);
			Added = true;
		}

		if (!Added) {
			aCastEvent(SUCCESS, CastResult::Success, "You begin casting#*#");
		}
	}
}

const std::string MQ2Cast::GetReturnString(CastResult Result) {
	switch (Result) {
	case CastResult::Success:      return std::string("CAST_SUCCESS");
	case CastResult::Interrupted:  return std::string("CAST_INTERRUPTED");
	case CastResult::Resist:       return std::string("CAST_RESIST");
	case CastResult::Collapse:     return std::string("CAST_COLLAPSE");
	case CastResult::Recover:      return std::string("CAST_RECOVER");
	case CastResult::Fizzle:       return std::string("CAST_FIZZLE");
	case CastResult::Standing:     return std::string("CAST_STANDING");
	case CastResult::Stunned:      return std::string("CAST_STUNNED");
	case CastResult::Invisible:    return std::string("CAST_INVISIBLE");
	case CastResult::NotReady:     return std::string("CAST_NOTREADY");
	case CastResult::OutOfMana:    return std::string("CAST_OUTOFMANA");
	case CastResult::OutOfRange:   return std::string("CAST_OUTOFRANGE");
	case CastResult::NoTarget:     return std::string("CAST_NOTARGET");
	case CastResult::CannotSee:    return std::string("CAST_CANNOTSEE");
	case CastResult::Components:   return std::string("CAST_COMPONENTS");
	case CastResult::Outdoors:     return std::string("CAST_OUTDOORS");
	case CastResult::TakeHold:     return std::string("CAST_TAKEHOLD");
	case CastResult::Immune:       return std::string("CAST_IMMUNE");
	case CastResult::Distracted:   return std::string("CAST_DISTRACTED");
	case CastResult::Cancelled:    return std::string("CAST_CANCELLED");
	case CastResult::Unknown:      return std::string("CAST_UNKNOWN");
	default:					   return std::string("CAST_NEEDFIXTYPE");
	}
}

#pragma endregion

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#pragma region Helper Functions

long MQ2Cast::Evaluate(PCHAR zFormat, ...) {
	char zOutput[MAX_STRING] = { 0 };
	va_list vaList;
	va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);

	if (!zOutput[0]) {
		return 1;
	}

	ParseMacroData(zOutput, sizeof(zOutput));
	return atoi(zOutput);
}

void MQ2Cast::Execute(PCHAR zFormat, ...) {
	char zOutput[MAX_STRING] = { 0 };
	va_list vaList;
	va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);

	if (!zOutput[0]) {
		return;
	}

	DoCommand(GetCharInfo()->pSpawn, zOutput);
}


void MQ2Cast::PluginCmd(PCHAR pluginName, PCHAR callName, PCHAR zFormat, ...) {
	typedef VOID(__cdecl *PluginCALL) (PSPAWNINFO, PCHAR);

	char zOutput[MAX_STRING];
	va_list vaList;
	va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);

	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, pluginName, strlen(pluginName))) {
		pLook = pLook->pNext;
	}

	if (pLook && pLook->fpVersion > 0.9999 && pLook->RemoveSpawn) {
		if (PluginCALL Request = (PluginCALL)GetProcAddress(pLook->hModule, callName)) {
			Request(GetCharInfo()->pSpawn, zOutput);
		}
	}
}

#pragma endregion

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#pragma region TLO

class MQ2CastType *pCastType = 0;
class MQ2CastType : public MQ2Type {
private:
	char Temps[MAX_STRING];

public:
	enum CastMembers {
		Active = 1,
		Effect = 2,
		Stored = 3,
		Result = 4,
		Return = 5,
		Status = 6,
		Timing = 7,
		Taken = 8,
		Ready = 9,
	};

	MQ2CastType() : MQ2Type("Cast") {
		TypeMember(Active);
		TypeMember(Effect);
		TypeMember(Stored);
		TypeMember(Result);
		TypeMember(Return);
		TypeMember(Status);
		TypeMember(Timing);
		TypeMember(Taken);
		TypeMember(Ready);
	}

	bool GetMember(MQ2VARPTR VarPtr, PCHAR Member, PCHAR Index, MQ2TYPEVAR &Dest) {
		PMQ2TYPEMEMBER pMember = MQ2CastType::FindMember(Member);
		if (pMember) {
			switch ((CastMembers)pMember->ID) {
			case Active:
				Dest.DWord = gbInZone;
				Dest.Type = pBoolType;
				return true;
			case Effect:
				Dest.DWord = CastingState::instance().detectSpellCast();
				if (Dest.DWord == NOID && CastingState::instance().getCmdType() != CastType::Ability && CastingState::instance().getCmdType() != CastType::Memorize && CastingState::instance().getCmdType() != CastType::None) {
					Dest.DWord = CastingState::instance().getCurrentID();
				}
				if (Dest.DWord != NOID) {
					Dest.Ptr = GetSpellByID(Dest.DWord);
					Dest.Type = pSpellType;
				}
				return true;
			case Stored:
				if (CastingState::instance().getLastID() != NOID) {
					Dest.Ptr = GetSpellByID(CastingState::instance().getLastID());
					Dest.Type = pSpellType;
				}
				return true;
			case Timing:
				Dest.DWord = CastingState::instance().castTimeRemaining();
				Dest.Type = pIntType;
				return true;
			case Status:
				Temps[0] = '\0';
				if (CastingState::instance().getCurrentID() != NOID || 
					(CastingState::instance().getCmdType() != CastType::Memorize && CastingState::instance().getCmdType() != CastType::None) || 
					(pCastingWnd && ((PCSIDLWND)pCastingWnd)->dShow)) {
					strcat_s(Temps, "C"); // casting
				}
				if (CastingState::instance().isPaused())								strcat_s(Temps, "S");	// stop moving
				if (CastingState::instance().isPaused(PauseCommand::FollowFlag))		strcat_s(Temps, "A");	// stopping adv path inc
				if (CastingState::instance().isPaused(PauseCommand::Stick))				strcat_s(Temps, "F");	// stopping stick
				if (CastingState::instance().isPaused(PauseCommand::FollowPath))		strcat_s(Temps, "P");	// stopping advpath following
				if (CastingState::instance().isPaused(PauseCommand::Path))				strcat_s(Temps, "P");	// stopping advpath pathing
				if (CastingState::instance().isPaused(PauseCommand::Navigation))		strcat_s(Temps, "N");	// stopping nav pathing
				if (CastingState::instance().getCmdType() == CastType::Memorize)		strcat_s(Temps, "M");	// currently memming
				if (CastingState::instance().isDucking())								strcat_s(Temps, "D");	// interrupting
				if (CastingState::instance().getCmdType() == CastType::Item)			strcat_s(Temps, "E");	// item casting
				if (CastingState::instance().getCmdType() == CastType::Discipline)		strcat_s(Temps, "R");	// item casting
				if (CastingState::instance().getCmdType() == CastType::AltAbility)		strcat_s(Temps, "L");	// AA casting
				if (!Temps[0]) {
					strcat_s(Temps, "I"); // idle
				}
				Dest.Ptr = Temps;
				Dest.Type = pStringType;
				return true;
			case Result:
			case Return:
				strcpy_s(Temps, GetReturnString(CastingState::instance().getLastResult()).c_str());
				Dest.Ptr = Temps;
				Dest.Type = pStringType;
				return true;
			case Ready:
				Dest.DWord = gbInZone && !CastingState::instance().hasCastBar() && !CastingState::instance().isWindowOpen() && pSpellBookWnd && !pSpellBookWnd->dShow && !GetCharInfo()->Stunned && CastingState::instance().isCastableReady(Index);
				Dest.Type = pBoolType;
				return true;
			case Taken:
				Dest.DWord = CastingState::instance().getLastResult() == CastResult::TakeHold;
				Dest.Type = pBoolType;
				return true;
			}
		}
		strcpy_s(Temps, "NULL");
		Dest.Type = pStringType;
		Dest.Ptr = Temps;
		return true;
	}

	bool ToString(MQ2VARPTR VarPtr, PCHAR Destination) {
		strcpy_s(Destination, MAX_STRING, "TRUE");
		return true;
	}

	bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source) {
		return false;
	}

	bool FromString(MQ2VARPTR &VarPtr, PCHAR Source) {
		return false;
	}

	~MQ2CastType() {}
};

BOOL dataCast(PCHAR szName, MQ2TYPEVAR &Dest)
{
	Dest.DWord = 1;
	Dest.Type = pCastType;
	return true;
}

#pragma endregion

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#pragma region Commands

PLUGIN_API VOID CastDebug(PSPAWNINFO pChar, PCHAR Cmd) {
	char zParm[MAX_STRING];
	GetArg(zParm, Cmd, 1);

	if (zParm[0] == 0) {
		DEBUGGING = !DEBUGGING;
	} else if (!_strnicmp(zParm, "on", 2)) {
		DEBUGGING = true;
	} else if (!_strnicmp(zParm, "off", 2)) {
		DEBUGGING = false;
	} else {
		DEBUGGING = !DEBUGGING;
	}

	WriteChatf("\arMQ2Cast\ax::\amDEBUGGING is now %s\ax.", DEBUGGING ? "\aoON" : "\agOFF");
}

PLUGIN_API VOID CastingCommand(PSPAWNINFO pChar, PCHAR Cmd) {
	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Command Start %d %s.", MQGetTickCount64(), pChar, Cmd); }
	if (!gbInZone || CastingState::instance().isCasting() || CastingState::instance().hasCastBar() || CastingState::instance().isWindowOpen() || (pSpellBookWnd && ((PCSIDLWND)pSpellBookWnd)->dShow)) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Casting]: Cannot Cast. [%d][%s%s%s%s]", MQGetTickCount64(), GetReturnString(CastingState::instance().getCurrentResult()),
				gbInZone ? " ZONE " : "", 
				CastingState::instance().isCasting() ? " isCasting " : "", 
				CastingState::instance().hasCastBar() ? " hasCastBar " : "", 
				CastingState::instance().isWindowOpen() ? " isWindowOpen " : "", 
				(pSpellBookWnd && ((PCSIDLWND)pSpellBookWnd)->dShow) ? " SHOW " : ""
			);
		}
		return;
	}

	CastingState::instance().Reset();

	CHAR szParam[MAX_STRING] = { 0 };
	CHAR castID[MAX_STRING] = { 0 };
	CHAR castTypeInput[MAX_STRING] = { 0 };

	auto isArg = [&szParam](PCHAR arg) -> bool {
		return !_strnicmp(szParam, arg, strlen(arg));
	};

	std::list<const ImmediateCommand*> preQueue;
	std::list<const ImmediateCommand*> postQueue;

	for (int iParam = 0; GetArg(szParam, Cmd, ++iParam)[0];) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Argument %s at %d.", MQGetTickCount64(), szParam, iParam); }
		if (!szParam[0]) {
			break;
		} else if (isArg("-targetid|")) {
			auto targetIDInput = &szParam[10];
			DWORD targetID = NOID;

			if (IsNumber(targetIDInput)) {
				targetID = (DWORD)atoi(targetIDInput);
			}

			preQueue.push_back(new TargetCommand(targetID));
		} else if (isArg("-kill")) {
			preQueue.push_back(new RecastCommand(CastRecast::Dead, 9999));
			postQueue.push_back(new RecastCommand(CastRecast::NTimes, 1));
		} else if (isArg("-maxtries|") && IsNumber(&szParam[10])) {
			int maxTries = atoi(&szParam[10]);
			preQueue.push_back(new RecastCommand(CastRecast::Land, maxTries));
			postQueue.push_back(new RecastCommand(CastRecast::NTimes, 1));
		} else if (isArg("-recast|") && IsNumber(&szParam[8])) {
			int nTimes = atoi(&szParam[8]);
			preQueue.push_back(new RecastCommand(CastRecast::NTimes, nTimes));
			postQueue.push_back(new RecastCommand(CastRecast::NTimes, 1));
		} else if (isArg("-setin|") || isArg("-bandolier|")) {
			CHAR passedName[MAX_STRING] = { 0 };
			GetArg(passedName, szParam, 2, FALSE, FALSE, FALSE, '|');

			preQueue.push_back(new BandolierCommand("add", "MQ2CastSaved"));
			preQueue.push_back(new BandolierCommand("activate", passedName));
			postQueue.push_back(new BandolierCommand("activate", "MQ2CastSaved"));
			postQueue.push_back(new BandolierCommand("delete", "MQ2CastSaved"));
		} else if (isArg("-invis") && CastingState::instance().getCastWhileInvis()) {
			preQueue.push_back(new InvisCommand(false));
			postQueue.push_back(new InvisCommand(true));
		} else if (szParam[0] != '-' && !castID[0]) {
			GetArg(castID, szParam, 1, FALSE, FALSE, FALSE, '|');
			GetArg(castTypeInput, szParam, 2, FALSE, FALSE, FALSE, '|');
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Spell Detected %s With Type %s.", MQGetTickCount64(), castID, castTypeInput); }
		} else if (szParam[0] != '-' && !castTypeInput[0]) {
			GetArg(castTypeInput, szParam, 1, FALSE, FALSE, FALSE, '|');
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Type Detected %s.", MQGetTickCount64(), castTypeInput); }
		}
	}

	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Name<%s> Type<%s>.", MQGetTickCount64(), castID, castTypeInput); }

	if (!castID[0]) {
		// we have nothing to cast after parsing the args, bail out.
		return;
	}

	CastType castType = CastType::None;
	if (castTypeInput[0]) {
		if (!_strnicmp(castTypeInput, "alt", 3)) {
			castType = CastType::AltAbility;
		} else if (!_strnicmp(castTypeInput, "gem", 3) || IsNumber(castTypeInput)) {
			castType = CastType::Spell;
		} else if (!_strnicmp(castTypeInput, "item", 4)) {
			castType = CastType::Item;
		} else if (!_strnicmp(castTypeInput, "disc", 4)) {
			castType = CastType::Discipline;
		}
	}

	if (castType == CastType::None) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Found No Type From %s.", MQGetTickCount64(), castTypeInput); }
		castType = CastingState::instance().findCastableType(castID);
	}

	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Found Type %d From %s.", MQGetTickCount64(), castType, castTypeInput); }

	CastCommand* castCmd;

	switch (castType) {
	case CastType::Item:
		castCmd = ItemCommand::Create(castID, preQueue, postQueue);
		break;
	case CastType::AltAbility:
		castCmd = AltAbilityCommand::Create(castID, preQueue, postQueue);
		break;
	case CastType::Discipline:
		castCmd = DiscCommand::Create(castID, preQueue, postQueue);
		break;
	case CastType::Ability:
		castCmd = AbilityCommand::Create(castID, preQueue, postQueue);
		break;
	case CastType::Spell:
	{
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Queueing Spell.", MQGetTickCount64()); }

		if (CastingState::instance().isBard() && (CastingState::instance().isTwisting() || CastingState::instance().isCasting())) {
			preQueue.push_back(new StopCastCommand());
		}

		int gemSlot = -1;
		if (castTypeInput[0]) {
			gemSlot = SpellCommand::GetGem(&castTypeInput[0]);
		}

		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Queueing %s From Gem %d.", MQGetTickCount64(), castID, gemSlot); }
		castCmd = SpellCommand::Create(castID, gemSlot, false, preQueue, postQueue);
		break;
	}
	default:
		// wrong cast type -- nothing to do
		return;
		break;
	};

	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Found Command To Push.", MQGetTickCount64()); }

	CastingState::instance().pushCmd(castCmd);
}

PLUGIN_API VOID DuckCommand(PSPAWNINFO pChar, PCHAR Cmd) {
	CastingState::instance().doNext(new StopCastCommand());

	// after we get done interrupting, we want to unpause movement activities
	CastingState::instance().pushImmediate(new RemobilizeCommand());
}

// do not interrupt casting, but do immobilize first.
PLUGIN_API VOID MemoCommand(PSPAWNINFO pChar, PCHAR szLine) {
	if (!gbInZone || CastingState::instance().hasCastBar() || CastingState::instance().isWindowOpen() || !pSpellBookWnd) {
		return;
	}

	CHAR szParam[MAX_STRING] = { 0 };
	CHAR szSubParam[MAX_STRING] = { 0 };
	for (int iParam = 0; GetArg(szParam, szLine, ++iParam)[0];) {
		if (!szParam[0]) {
			break;
		}

		// get the spell name or ID
		GetArg(szSubParam, szParam, 1, FALSE, FALSE, FALSE, '|');

		CHAR spellName[MAX_STRING] = { 0 };
		strcpy_s(spellName, szSubParam);
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Spell Name %s Found From %s.", MQGetTickCount64(), spellName, szSubParam); }

		auto gemSlot = 1; // TODO: what is the default gem? Is this ini-specified?

		// if there is a '|' character, it will be followed by the gem number
		GetArg(szSubParam, szParam, 2, FALSE, FALSE, FALSE, '|');

		// if there is not a '|' character, then we need to grab the next argument and hope its a gem
		if (!szSubParam[0]) {
			GetArg(szSubParam, szLine, iParam + 1);
		}

		// if there was an argument, let's try to parse it
		if (szSubParam[0]) {
			auto testGem = SpellCommand::GetGem(&szSubParam[0]);

			// got a gem, let's set it and increment the counter -- this can fail if the spellID of the next spell is less than the number of spell gems (there are a few cases of this)
			if (testGem > 0 && testGem <= NUM_SPELL_GEMS) {
				gemSlot = testGem;
				++iParam;
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Spell Gem %d Found From %s.", MQGetTickCount64(), gemSlot, szSubParam); }
			}
		}

		CastingState::instance().pushCmd(MemCommand::Create(spellName, gemSlot));
	}
}

#pragma endregion

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#pragma region Plugin Boilerplate/Virtuals

PLUGIN_API VOID InitializePlugin(VOID) {
	aCastEvent(LIST289, CastResult::Collapse, "Your gate is too unstable, and collapses#*#");
	aCastEvent(LIST289, CastResult::CannotSee, "You cannot see your target#*#");
	aCastEvent(LIST289, CastResult::Components, "You are missing some required components#*#");
	aCastEvent(UNKNOWN, CastResult::Components, "Your ability to use this item has been disabled because you do not have at least a gold membership#*#");
	aCastEvent(LIST289, CastResult::Components, "You need to play a#*#instrument for this song#*#");
	aCastEvent(LIST289, CastResult::Distracted, "You are too distracted to cast a spell now#*#");
	aCastEvent(LIST289, CastResult::Distracted, "You can't cast spells while invulnerable#*#");
	aCastEvent(LIST289, CastResult::Distracted, "You *CANNOT* cast spells, you have been silenced#*#");
	aCastEvent(LIST289, CastResult::Immune, "Your target has no mana to affect#*#");
	aCastEvent(LIST013, CastResult::Immune, "Your target is immune to changes in its attack speed#*#");
	aCastEvent(LIST013, CastResult::Immune, "Your target is immune to changes in its run speed#*#");
	aCastEvent(LIST013, CastResult::Immune, "Your target is immune to snare spells#*#");
	aCastEvent(LIST289, CastResult::Immune, "Your target cannot be mesmerized#*#");
	aCastEvent(UNKNOWN, CastResult::Immune, "Your target looks unaffected#*#");
	aCastEvent(LIST264, CastResult::Interrupted, "Your spell is interrupted#*#");
	aCastEvent(UNKNOWN, CastResult::Interrupted, "Your casting has been interrupted#*#");
	aCastEvent(LIST289, CastResult::Fizzle, "Your spell fizzles#*#");
	aCastEvent(LIST289, CastResult::Fizzle, "You miss a note, bringing your song to a close#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "You must first select a target for this spell#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "This spell only works on#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "You must first target a group member#*#");
	aCastEvent(LIST289, CastResult::NotReady, "Spell recast time not yet met#*#");
	aCastEvent(LIST289, CastResult::OutOfMana, "Insufficient Mana to cast this spell#*#");
	aCastEvent(LIST289, CastResult::OutOfRange, "Your target is out of range, get closer#*#");
	aCastEvent(LIST289, CastResult::Outdoors, "This spell does not work here#*#");
	aCastEvent(LIST289, CastResult::Outdoors, "You can only cast this spell in the outdoors#*#");
	aCastEvent(LIST289, CastResult::Outdoors, "You can not summon a mount here#*#");
	aCastEvent(LIST289, CastResult::Outdoors, "You must have both the Horse Models and your current Luclin Character Model enabled to summon a mount#*#");
	aCastEvent(LIST264, CastResult::Recover, "You haven't recovered yet#*#");
	aCastEvent(LIST289, CastResult::Recover, "Spell recovery time not yet met#*#");
	aCastEvent(LIST289, CastResult::Resist, "Your target resisted the#*#spell#*#");
	aCastEvent(LIST289, CastResult::Standing, "You must be standing to cast a spell#*#");
	aCastEvent(LIST289, CastResult::Stunned, "You can't cast spells while stunned#*#");
	aCastEvent(LIST289, CastResult::Success, "You are already on a mount#*#");
	aCastEvent(LIST289, CastResult::TakeHold, "Your spell did not take hold#*#");
	aCastEvent(LIST289, CastResult::TakeHold, "Your spell would not have taken hold#*#");
	aCastEvent(LIST289, CastResult::TakeHold, "Your spell is too powerfull for your intended target#*#");
	aCastEvent(LIST289, CastResult::TakeHold, "You need to be in a more open area to summon a mount#*#");
	aCastEvent(LIST289, CastResult::TakeHold, "You can only summon a mount on dry land#*#");
	aCastEvent(LIST289, CastResult::TakeHold, "This pet may not be made invisible#*#");
	pCastType = new MQ2CastType;
	AddMQ2Data("Cast", dataCast);
	AddCommand("/castdebug", CastDebug);
	AddCommand("/casting", CastingCommand);
	AddCommand("/interrupt", DuckCommand);
	AddCommand("/memorize", MemoCommand);
}

PLUGIN_API VOID SetGameState(unsigned long ulGameState) {
	if (GetGameState() != GAMESTATE_INGAME) {
		CastingState::instance().ReloadUI();
	}
}

PLUGIN_API VOID ShutdownPlugin(VOID) {
	RemoveMQ2Data("Cast");
	delete pCastType;
	RemoveCommand("/castdebug");
	RemoveCommand("/casting");
	RemoveCommand("/interrupt");
	RemoveCommand("/memorize");
}

PLUGIN_API VOID OnEndZone(VOID) {
	CastingState::instance().Reset();
}

PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color) {
	CHAR szLine[MAX_STRING] = { 0 };
	strcpy_s(szLine, Line);
	if (gbInZone) {
		if (CastingState::instance().getCurrentID() != NOID) {
			Parsed = false;

			if (DEBUGGING) {
				WriteChatf("[%I64u] OnIncomingChat:: ChatLine: %s Color: %d", MQGetTickCount64(), szLine, Color);
			}

			if (Color == 264) {
				LIST264.Feed(szLine);
				SUCCESS.Feed(szLine);
			} else if (Color == 289) {
				LIST289.Feed(szLine);
			} else if (Color == 13) {
				LIST013.Feed(szLine);
			}

			if (!Parsed) {
				UNKNOWN.Feed(szLine);
				if (DEBUGGING && Parsed) {
					WriteChatf("\arMQ2Cast::Note for Author[\ay%s\ar]=(\ag%d\ar)\ax", szLine, Color);
				}
			}
		}
	}

	return 0;
}

PLUGIN_API VOID OnPulse(VOID) {
	if (!gbInZone || !GetCharInfo() || !GetCharInfo()->pSpawn) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: No Zone.", MQGetTickCount64()); }
		return;
	}

	if (!CastingState::instance().shouldPulse()) {
		return;
	}

	// execute the front of the immediate queue
	if (!CastingState::instance().doImmediate()) {
		// if it's still executing, no need to worry about anything else
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Immediate Command Running.", MQGetTickCount64()); }
		return;
	}

	CastingState::instance().updateTimeout();

	// are we casting something right now?
	auto detectedID = CastingState::instance().detectSpellCast();
	if (detectedID != NOID) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Detected Spell Casting: %d.", MQGetTickCount64(), detectedID); }
		if (CastingState::instance().getOnTargetID() == NOID) {
			CastingState::instance().setOnTargetID(pTarget ? ((PSPAWNINFO)pTarget)->SpawnID : NOID);
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Setting Target ID %d.", MQGetTickCount64(), CastingState::instance().getOnTargetID()); }
		}

		// was this an unexpected cast?
		if (detectedID != CastingState::instance().getCurrentID()) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Unexpected Cast -> Setting Variables.", MQGetTickCount64()); }
			CastingState::instance().setCurrentID(detectedID);
			CastingState::instance().setCurrentResult(CastResult::Casting);
			CastingState::instance().updateTimeout();
			BlechSpell(GetSpellByID(detectedID));
		}

		CastingState::instance().setCurrentResult(CastResult::Casting);

		return; // we are casting, don't do anything else.
	} else if (CastingState::instance().getCmdType() != CastType::None) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Nothing Casting With Queue Size %d -> Executing Command Type %d.", MQGetTickCount64(), CastingState::instance().getCmdSize(), CastingState::instance().getCmdType()); }
		if (CastingState::instance().getOnTargetID() == NOID) {
			CastingState::instance().setOnTargetID(pTarget ? ((PSPAWNINFO)pTarget)->SpawnID : NOID);
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Setting Target ID %d.", MQGetTickCount64(), CastingState::instance().getOnTargetID()); }
		} else if (pTarget) {
			auto targetID = ((PSPAWNINFO)pTarget)->SpawnID;
			if (targetID != NOID) {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Wrong Target ID %d -> Set Target To ID %d.", MQGetTickCount64(), targetID, CastingState::instance().getOnTargetID()); }
				// we need to target. Push a command and return.
				CastingState::instance().pushImmediate(new TargetCommand(targetID));
				return;
			}
		}

		// these are "universal" checks, so don't include them in the individual command functions
		auto currentID = CastingState::instance().getCurrentID();
		if (currentID != NOID) {
			if (GetCharInfo()->Stunned) {
				// can't do things while stunned!
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Found Stunned.", MQGetTickCount64()); }
				CastingState::instance().setCurrentResult(CastResult::Stunned);
			} else if (!CastingState::instance().getCastWhileInvis() && GetCharInfo()->pSpawn->HideMode) {
				// we specified not to cast while invis
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Found Invis While CastWhileInvis=false.", MQGetTickCount64()); }
				CastingState::instance().setCurrentResult(CastResult::Invisible);
			} else if (PSPELL currentSpell = GetSpellByID(currentID)) {
				// maybe we have the wrong thing targeted?
				switch (currentSpell->TargetType) {
				case 18: // Uber Dragons
				case 17: // Uber Giants
				case 16: // Plant
				case 15: // Corpse
				case 14: // Pet
				case 11: // Summoned
				case 10: // Undead
				case  9: // Animal
				case  5: // Single
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Found No Target When Spell Requires Target.", MQGetTickCount64()); }
					if (!pTarget) {
						CastingState::instance().setCurrentResult(CastResult::NoTarget);
					}
					break;
				}
			}
		}

		// if we aren't done with the command, then execute it. If we are, don't do anything.
		if (CastingState::instance().getCurrentResult() < CastResult::Success && GetCharInfo()) {
			auto pMySpawn = GetCharInfo()->pSpawn;
			// check the timeout first. We know we aren't actually casting anything right now
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Command Executing.", MQGetTickCount64()); }
			if ((pMySpawn && pMySpawn->TimeStamp < CastingState::instance().getTimeout()) || (CastingState::instance().getCurrentID() == NOID && CastingState::instance().getCmdSize() > 0)) {
				// we should be doing something right now, so let's do it.
				if (auto cmd = CastingState::instance().getCmd()) {
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Running Command Function.", MQGetTickCount64()); }
					cmd->execute(); // do the function
				}
			} else if (!pMySpawn || pMySpawn->TimeStamp > CastingState::instance().getTimeout()) {
				// the assumption here is that we had sufficient time to cast the spell, so we probably actually cast it
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Command Timed Out With TimeStamp %d of %d.", MQGetTickCount64(), pMySpawn ? pMySpawn->TimeStamp : -1, CastingState::instance().getTimeout()); }
				CastingState::instance().setCurrentResult(CastResult::Success);
			} else {
				// this clause is mostly for debugging (could be rolled into other clause)
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Failed to find pMySpawn %d.", MQGetTickCount64(), pMySpawn); }
				CastingState::instance().setCurrentResult(CastResult::Success);
			}

			CastingState::instance().updateTimeout();
		}
	}

	// if we are finished, pop the command (will save off the ID and result and get ready for the next one)
	if (CastingState::instance().getCurrentResult() >= CastResult::Success) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Command Finished With Result %d.", MQGetTickCount64(), GetReturnString(CastingState::instance().getCurrentResult())); }
		if (CastingState::instance().decRecast() > 0) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Command Recast? (%d).", MQGetTickCount64(), CastingState::instance().getRecast()); }
			switch (CastingState::instance().getRecast().first) {
			case CastRecast::Dead:
				if (auto target = (PSPAWNINFO)GetSpawnByID(CastingState::instance().getOnTargetID())) {
					if (target->StandState != STANDSTATE_DEAD) {
						if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Not Dead! Recasting.", MQGetTickCount64()); }
						CastingState::instance().setCurrentResult(CastResult::Idle);
					}
				} // if we can't find the target, then assume its dead anyway
				break;
			case CastRecast::Land:
				if (CastingState::instance().getCurrentResult() != CastResult::Success) {
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Not Successful! Recasting.", MQGetTickCount64()); }
					CastingState::instance().setCurrentResult(CastResult::Idle);
				}
				break;
			case CastRecast::NTimes:
				// all we need to do here is reset to idle and do it again
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Not Done! Recasting.", MQGetTickCount64()); }
				CastingState::instance().setCurrentResult(CastResult::Idle);
				break;
			default:
				break;
			};
		}
	}
	
	if (CastingState::instance().getCurrentResult() >= CastResult::Success) {
		CastingState::instance().popCmd();
		CastingState::instance().pushImmediate(new RemobilizeCommand());

		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Casting Complete ID[%d] Result=[%d]", MQGetTickCount64(), CastingState::instance().getLastID(), GetReturnString(CastingState::instance().getLastResult()));
		}
	}
}

PLUGIN_API VOID OnReloadUI() {
	CastingState::instance().ReloadUI();
}

#pragma endregion

#pragma region Detection and State Functions

bool CastingState::isImmobile() {
	ULONGLONG MyTimer = MQGetTickCount64();

	// If we haven't stopped yet, then add 500ms to the timer
	// this has the effect that we will have a 500ms 
	// window in which we have potentially stopped but will still
	// be seen as "moving" per this plugin. This should account
	// for server lag.
	if (fabs(FindSpeed(GetCharInfo()->pSpawn)) > std::numeric_limits<float>::epsilon()) {
		ImmobileTime = MyTimer + 500;
	}

	// gbMoving appears to be set on pulse and has a 100 ms window.
	// that window is likely not long enough for spell cast interrupts.
	Immobile = (!MQ2Globals::gbMoving && (ImmobileTime == 0 || MyTimer > ImmobileTime));
	return Immobile;
}

// TODO: Add in general bard functionality
bool CastingState::isBard() {
	return GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->mActorClient.Class == Bard;
}

bool CastingState::isTwisting() {
	return FindMQ2DataType("Twist") && (bool)Evaluate("${If[${Twist.Twisting},1,0]}");
}

// This function is used to check if there are any windows open that prevent casting
bool CastingState::isWindowOpen() {
	if (CastingState::instance().isBard()) {
		return false;
	}

	if (pLootWnd && ((PCSIDLWND)pLootWnd)->dShow) {
		return true;
	}

	if (pBankWnd && ((PCSIDLWND)pBankWnd)->dShow) {
		return true;
	}

	if (pMerchantWnd && ((PCSIDLWND)pMerchantWnd)->dShow) {
		return true;
	}

	if (pTradeWnd && ((PCSIDLWND)pTradeWnd)->dShow) {
		return true;
	}

	if (pGiveWnd && ((PCSIDLWND)pGiveWnd)->dShow) {
		return true;
	}

	if (!TributeMasterWnd) {
		TributeMasterWnd = FindMQ2Window("TributeMasterWnd");
	}

	if (TributeMasterWnd && TributeMasterWnd->dShow) {
		return true;
	}

	if (!GuildBankWnd) {
		GuildBankWnd = FindMQ2Window("GuildBankWnd");
	}
	
	if (GuildBankWnd && GuildBankWnd->dShow) {
		return true;
	}

	return false;
}

bool CastingState::hasCastBar() {
	if (!isBard() && pCastingWnd && ((PCSIDLWND)pCastingWnd)->dShow) {
		if (DEBUGGING) { WriteChatf("MQ2Cast: pCastingWnd=TRUE"); }
		return true;
	}

	return false;
}

DWORD CastingState::detectSpellCast() {
	if (GetCharInfo() && GetCharInfo()->pSpawn) {
		return GetCharInfo()->pSpawn->CastingData.SpellID;
	}

	return NOID;
}

CastType CastingState::findCastableType(PCHAR ID) {
	if (!ID) return CastType::None;

	if (IsNumber(ID)) {
		// don't check discs or abilities or gems here because they collide with spell ID's
		auto nID = atoi(ID);
		if (FindItemByID(nID)) {
			return CastType::Item;
		} else if (GetAAByIdWrapper(GetAAIndexByID(nID)) != 0) { // don't need level here since it's by ID
			return CastType::AltAbility;
		} else if (GetSpellByID(nID)) {
			return CastType::Spell;
		}
	} else {
		// okay, we've got a name -- since we only care about the type of thing being cast, just lookup actual names
		CHAR szName[MAX_STRING] = { 0 };
		strcpy_s(szName, ID);
		bool exact = StripQuotes(szName);
		if (FindItemByName(szName, exact)) {
			return CastType::Item;
		} else if ((PSPAWNINFO)pLocalPlayer && GetAAByIdWrapper(GetAAIndexByName(ID), ((PSPAWNINFO)pLocalPlayer)->Level)) {
			return CastType::AltAbility;
		} else {
			for (int caIdx = 0; caIdx < NUM_COMBAT_ABILITIES; ++caIdx) {
				if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(caIdx)) {
					if (auto pSpell = GetSpellByID(pPCData->GetCombatAbility(caIdx))) {
						if (!_stricmp(ID, pSpell->Name)) {
							return CastType::Discipline;
						}
					}
				}
			}

			for (DWORD abilityIdx = 0; szSkills[abilityIdx]; ++abilityIdx) {
				if (!_stricmp(ID, szSkills[abilityIdx])) {
					return CastType::Ability;
				}
			}

			if (GetSpellByName(ID)) {
				return CastType::Spell;
			}
		}
	}

	return CastType::None;
}

bool CastingState::isCastableReady(PCHAR ID, CastType cType) {
	switch (cType) {
	case CastType::Item:
		if (auto pCont = ItemCommand::GetContents(ID)) {
			if (auto pItem = GetItemFromContents(pCont)) {
				return (pItem->Clicky.TimerID != -1 && GetItemTimer(pCont) == 0) || pItem->Clicky.SpellID != NOID;
			}
		}
		return false;
	case CastType::AltAbility:
	{
		auto idxPair = AltAbilityCommand::GetAAIndex(ID);
		if (PlayerHasAAAbility(idxPair.first)) {
			if (auto pAbility = GetAAByIdWrapper(idxPair.first, idxPair.second)) {
				return pAltAdvManager && pAltAdvManager->IsAbilityReady(pPCData, pAbility, 0);
			}
		}
		return false;
	}
	case CastType::Discipline:
		if (auto pSpell = GetSpellByID(DiscCommand::FindDiscID(ID))) {
			return DiscCommand::GetDiscTimer(pSpell) < (DWORD)time(NULL);
		}
		return false;
	case CastType::Ability:
		if (auto idx = AbilityCommand::FindAbilityIndex(ID)) {
			if (idx == 105 || idx == 107) {
				return LoH_HT_Ready();
			} else {
				return pCSkillMgr && pCSkillMgr->IsAvailable(idx);
			}
		}
		return false;
	case CastType::Spell:
		if (auto pSpell = GetSpellByName(ID)) {
			int gemIdx = SpellCommand::GetGem(pSpell->ID);
			return gemIdx != -1 && SpellCommand::GetCooldown(gemIdx) <= 0;
		}
		return false;
	case CastType::Memorize:
		if (auto pSpell = GetSpellByName(ID)) {
			if (SpellCommand::GetGem(pSpell->ID) != -1) {
				return false; // can't mem when it's already memmed...
			}

			for (DWORD spellIdx = 0; spellIdx < NUM_BOOK_SLOTS; ++spellIdx) {
				if (GetCharInfo2()->SpellBook[spellIdx] == pSpell->ID) {
					return true;
				}
			}
		}
		return false;
	default:
		return false;
	}
}

bool CastingState::isCastableReady(PCHAR ID) {
	return isCastableReady(ID, findCastableType(ID));
}

#pragma endregion

long CastingState::castTimeRemaining() {
	// no need to nullcheck GetCharInfo()->pSpawn here since it is done through isCasting()
	if (isCasting()) {
		// if isCasting() == true, then we need to return at least 1 here
		return max(GetCharInfo()->pSpawn->CastingData.SpellETA - GetCharInfo()->pSpawn->TimeStamp, 1);
	} else {
		return 0;
	}
}

ULONGLONG CastingState::updateTimeout() {
	if (!GetCharInfo()) {
		return Timeout;
	}

	if (auto pMySpawn = GetCharInfo()->pSpawn) {
		LastTimestamp = pMySpawn->TimeStamp;

		if (pMySpawn->CastingData.SpellETA > 0) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Timeout]: Found Spell ETA %d -> TimeStamp %d.", MQGetTickCount64(), pMySpawn->CastingData.SpellETA, pMySpawn->TimeStamp); }
			// TODO: potentially account for lag here (+ MQGetTickCount64() + 450 + (pConnection->Last) * 4;)
			Timeout = pMySpawn->TimeStamp + pMySpawn->CastingData.SpellETA;
		} else if (getCurrentID() != NOID) {
			if (auto pSpell = GetSpellByID(getCurrentID())) {
				if (getCmdType() == CastType::Memorize) {
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Timeout]: Memming TimeStamp %d.", MQGetTickCount64(), pMySpawn->TimeStamp); }
					// TODO: Can I get a memorization time? -- as long as we don't LD (timestamp stays more frequent than 1s), we will recheck this every pulse
					Timeout = pMySpawn->TimeStamp + 1000;
				} else {
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Timeout]: Casting Time %d TimeStamp %d.", MQGetTickCount64(), pSpell->CastTime, pMySpawn->TimeStamp); }
					Timeout = pMySpawn->TimeStamp + pSpell->CastTime;
				}
			} else {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Timeout]: No Spell Detected TimeStamp %d.", MQGetTickCount64(), pMySpawn->TimeStamp); }
				Timeout = pMySpawn->TimeStamp - 100;
			}
		} else {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Timeout]: No Spell Casting TimeStamp %d.", MQGetTickCount64(), pMySpawn->TimeStamp); }
			// -100 to guarantee its in the past
			Timeout = pMySpawn->TimeStamp - 100;
		}
	}

	return Timeout;
}

bool CastingState::shouldPulse() {
	return GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->TimeStamp > LastTimestamp;
}