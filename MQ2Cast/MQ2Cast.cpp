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

bool DEBUGGING = false;

#ifndef PLUGIN_API
#include "../MQ2Plugin.h"
#include "../Blech/Blech.h"
PreSetup("MQ2Cast");
PLUGIN_VERSION(12.0);
#endif

#include <functional>
#include <limits>

#include "MQ2Cast.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
CastingState cState;

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

	if (cState.getCurrentResult() < eventResult) {
		cState.setCurrentResult(eventResult);
	}

	if (DEBUGGING) {
		WriteChatf("[%I64u] MQ2Cast:[OnChat]: Result=[%d] Called=[%d].", MQGetTickCount64(), cState.getCurrentResult(), (long)pData);
	}
}

// This function removes and re-inserts the Blech filters for the currently casting spell
void BlechSpell(PSPELL Cast) {
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

#pragma endregion

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#pragma region Helper Functions

long Evaluate(PCHAR zFormat, ...) {
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

void Execute(PCHAR zFormat, ...) {
	char zOutput[MAX_STRING] = { 0 };
	va_list vaList;
	va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);

	if (!zOutput[0]) {
		return;
	}

	DoCommand(GetCharInfo()->pSpawn, zOutput);
}


void PluginCmd(PCHAR pluginName, PCHAR callName, PCHAR zFormat, ...) {
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

template<typename... Args>
void Stick(PCHAR zFormat, Args... args) {
	PluginCmd("MQ2MoveUtils", "StickCommand", zFormat, args...);
}

template<typename... Args>
void Nav(PCHAR zFormat, Args... args) {
	PluginCmd("MQ2Nav", "NavigateCommand", zFormat, args...);
}

template<typename... Args>
void FollowPath(PCHAR zFormat, Args... args) {
	PluginCmd("MQ2AdvPath", "MQFollowCommand", zFormat, args...);
}

template<typename... Args>
void Path(PCHAR zFormat, Args... args) {
	PluginCmd("MQ2AdvPath", "MQPlayCommand", zFormat, args...);
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
				Dest.DWord = cState.detectSpellCast();
				if (Dest.DWord == NOID && cState.getCmdType() != CastType::Ability && cState.getCmdType() != CastType::Memorize && cState.getCmdType() != CastType::None) {
					Dest.DWord = cState.getCurrentID();
				}
				if (Dest.DWord != NOID) {
					Dest.Ptr = GetSpellByID(Dest.DWord);
					Dest.Type = pSpellType;
				}
				return true;
			case Stored:
				if (cState.getLastID() != NOID) {
					Dest.Ptr = GetSpellByID(cState.getLastID());
					Dest.Type = pSpellType;
				}
				return true;
			case Timing:
				Dest.DWord = cState.castTimeRemaining();
				Dest.Type = pIntType;
				return true;
			case Status:
				Temps[0] = '\0';
				if (cState.getCurrentID() != NOID || 
					(cState.getCmdType() != CastType::Memorize && cState.getCmdType() != CastType::None) || 
					(pCastingWnd && ((PCSIDLWND)pCastingWnd)->dShow)) {
					strcat_s(Temps, "C"); // casting
				}
				if (cState.isPaused())									strcat_s(Temps, "S");	// stop moving
				if (cState.isPaused(PauseCommand::FollowFlag))			strcat_s(Temps, "A");	// stopping adv path inc
				if (cState.isPaused(PauseCommand::Stick))				strcat_s(Temps, "F");	// stopping stick
				if (cState.isPaused(PauseCommand::FollowPath))			strcat_s(Temps, "P");	// stopping advpath following
				if (cState.isPaused(PauseCommand::Path))				strcat_s(Temps, "P");	// stopping advpath pathing
				if (cState.isPaused(PauseCommand::Navigation))			strcat_s(Temps, "N");	// stopping nav pathing
				if (cState.getCmdType() == CastType::Memorize)			strcat_s(Temps, "M");	// currently memming
				if (cState.isDucking())									strcat_s(Temps, "D");	// interrupting
				if (cState.getCmdType() == CastType::Item)				strcat_s(Temps, "E");	// item casting
				if (cState.getCmdType() == CastType::Discipline)		strcat_s(Temps, "R");	// item casting
				if (cState.getCmdType() == CastType::AltAbility)		strcat_s(Temps, "L");	// AA casting
				if (!Temps[0]) {
					strcat_s(Temps, "I"); // idle
				}
				Dest.Ptr = Temps;
				Dest.Type = pStringType;
				return true;
			case Result:
			case Return:
				switch (cState.getLastResult()) {
				case CastResult::Success:      strcpy_s(Temps, "CAST_SUCCESS");     break;
				case CastResult::Interrupted:  strcpy_s(Temps, "CAST_INTERRUPTED"); break;
				case CastResult::Resist:       strcpy_s(Temps, "CAST_RESIST");      break;
				case CastResult::Collapse:     strcpy_s(Temps, "CAST_COLLAPSE");    break;
				case CastResult::Recover:      strcpy_s(Temps, "CAST_RECOVER");     break;
				case CastResult::Fizzle:       strcpy_s(Temps, "CAST_FIZZLE");      break;
				case CastResult::Standing:     strcpy_s(Temps, "CAST_STANDING");    break;
				case CastResult::Stunned:      strcpy_s(Temps, "CAST_STUNNED");     break;
				case CastResult::Invisible:    strcpy_s(Temps, "CAST_INVISIBLE");   break;
				case CastResult::NotReady:     strcpy_s(Temps, "CAST_NOTREADY");    break;
				case CastResult::OutOfMana:    strcpy_s(Temps, "CAST_OUTOFMANA");   break;
				case CastResult::OutOfRange:   strcpy_s(Temps, "CAST_OUTOFRANGE");  break;
				case CastResult::NoTarget:     strcpy_s(Temps, "CAST_NOTARGET");    break;
				case CastResult::CannotSee:    strcpy_s(Temps, "CAST_CANNOTSEE");   break;
				case CastResult::Components:   strcpy_s(Temps, "CAST_COMPONENTS");  break;
				case CastResult::Outdoors:     strcpy_s(Temps, "CAST_OUTDOORS");    break;
				case CastResult::TakeHold:     strcpy_s(Temps, "CAST_TAKEHOLD");    break;
				case CastResult::Immune:       strcpy_s(Temps, "CAST_IMMUNE");      break;
				case CastResult::Distracted:   strcpy_s(Temps, "CAST_DISTRACTED");  break;
				case CastResult::Cancelled:    strcpy_s(Temps, "CAST_CANCELLED");   break;
				case CastResult::Unknown:      strcpy_s(Temps, "CAST_UNKNOWN");     break;
				default:					   strcpy_s(Temps, "CAST_NEEDFIXTYPE"); break;
				}

				Dest.Ptr = Temps;
				Dest.Type = pStringType;
				return true;
			case Ready:
				Dest.DWord = gbInZone && !cState.hasCastBar() && !cState.isWindowOpen() && pSpellBookWnd && !pSpellBookWnd->dShow && !GetCharInfo()->Stunned && cState.isCastableReady(Index);
				Dest.Type = pBoolType;
				return true;
			case Taken:
				Dest.DWord = cState.getLastResult() == CastResult::TakeHold;
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

PLUGIN_API VOID CastCommand(PSPAWNINFO pChar, PCHAR Cmd) {
	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Command Start %d %s.", MQGetTickCount64(), pChar, Cmd); }
	if (!gbInZone || cState.hasCastBar() || cState.isWindowOpen() || (pSpellBookWnd && ((PCSIDLWND)pSpellBookWnd)->dShow)) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Casting]: Cannot Cast. [%d][%s%s%s%s]", MQGetTickCount64(), cState.getCurrentResult(),
				gbInZone ? " ZONE " : "", cState.hasCastBar() ? " hasCastBar " : "", cState.isWindowOpen() ? " isWindowOpen " : "", (pSpellBookWnd && ((PCSIDLWND)pSpellBookWnd)->dShow) ? " SHOW " : "");
		}
		return;
	}

	cState.Reset();

	CHAR szParam[MAX_STRING] = { 0 };
	CHAR castID[MAX_STRING] = { 0 };
	CHAR castTypeInput[MAX_STRING] = { 0 };

	auto isArg = [&szParam](PCHAR arg) -> bool {
		return !_strnicmp(szParam, arg, strlen(arg));
	};

	std::queue<CastingState::ImmediateCommand> preQueue;
	std::queue<CastingState::ImmediateCommand> postQueue;

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

			auto targetCmd = [targetID]() -> bool {
				if (auto target = (PSPAWNINFO)GetSpawnByID(targetID)) {
					*(PSPAWNINFO*)ppTarget = target;
				}

				cState.setOnTargetID(targetID);

				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Targeting ID %d.", MQGetTickCount64(), targetID); }
				
				return true;
			};

			preQueue.push(targetCmd);
		} else if (isArg("-kill")) {
			preQueue.push([]() { cState.doRecast(CastRecast::Dead, 9999); return true; });
			postQueue.push([]() { cState.doRecast(CastRecast::NTimes, 1); return true; });
		} else if (isArg("-maxtries|")) {
			int maxTries = atoi(&szParam[10]);
			preQueue.push([maxTries]() { cState.doRecast(CastRecast::Land, maxTries); return true; });
			postQueue.push([]() { cState.doRecast(CastRecast::NTimes, 1); return true; });
		} else if (isArg("-recast|")) {
			int nTimes = atoi(&szParam[8]);
			preQueue.push([nTimes]() { cState.doRecast(CastRecast::NTimes, nTimes); return true; });
			postQueue.push([]() { cState.doRecast(CastRecast::NTimes, 1); return true; });
		} else if (isArg("-setin|") || isArg("-bandolier|")) {
			auto bandolierCmd = [](const char *cmd, const char *bName) -> bool {
				Execute("/bandolier %s %s", cmd, bName);

				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Bandolier %s %s.", MQGetTickCount64(), cmd, bName); }

				return true;
			};

			CHAR passedName[MAX_STRING] = { 0 };
			GetArg(passedName, szParam, 2, FALSE, FALSE, FALSE, '|');

			preQueue.push(std::bind(bandolierCmd, "add", "MQ2CastSaved"));
			preQueue.push(std::bind(bandolierCmd, "activate", passedName));
			postQueue.push(std::bind(bandolierCmd, "activate", "MQ2CastSaved"));
			postQueue.push(std::bind(bandolierCmd, "delete", "MQ2CastSaved"));
		} else if (isArg("-invis")) {
			auto invisCmd = [](bool bInvis) -> bool {
				cState.setCastWhileInvis(bInvis);
				return true;
			};

			preQueue.push(std::bind(invisCmd, false));
			postQueue.push(std::bind(invisCmd, true));
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
		castType = cState.findCastableType(castID);
	}

	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Found Type %d From %s.", MQGetTickCount64(), castType, castTypeInput); }

	CastingState::MQ2CastFunc castCmd;

	switch (castType) {
	case CastType::Item:
		castCmd = cState.castAltAbility(castID);
		break;
	case CastType::AltAbility:
		castCmd = cState.castAltAbility(castID);
		break;
	case CastType::Discipline:
		castCmd = cState.castDisc(castID);
		break;
	case CastType::Ability:
		castCmd = cState.castAbility(castID);
		break;
	case CastType::Spell:
	{
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Queueing Spell.", MQGetTickCount64()); }

		if (cState.isBard()) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Is Bard.", MQGetTickCount64()); }
			if (cState.isTwisting()) {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Is Twisting.", MQGetTickCount64()); }
				preQueue.push([]() { Execute("/stoptwist"); return !cState.isTwisting(); });
			}

			if (cState.isCasting()) {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Is Singing.", MQGetTickCount64()); }
				preQueue.push([]() { Execute("/stopsong"); return cState.isCasting(); });
			}
		}

		int gemIdx = -1;
		if (castTypeInput[0]) {
			gemIdx = cState.getGem(&castTypeInput[0]);
		}

		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Queueing %s From Gem %d.", MQGetTickCount64(), castID, gemIdx); }
		castCmd = cState.castSpell(castID, gemIdx);
		break;
	}
	default:
		// wrong cast type -- nothing to do
		return;
		break;
	};

	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Found Command To Push.", MQGetTickCount64()); }

	cState.pushCmd(castType, castCmd, preQueue, postQueue);
	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Command Complete.", MQGetTickCount64()); }
}

PLUGIN_API VOID DuckCommand(PSPAWNINFO pChar, PCHAR Cmd) {
	auto interruptCmd = []() -> bool {
		cState.clearCmds();
		if (cState.isBard()) {
			Execute("/stopsong");
		} else {
			Execute("/stopcast");
		}

		return !gbInZone || cState.setDucking(cState.isCasting());
	};

	cState.doNext(interruptCmd);
	// after we get done interrupting, we want to unpause movement activities
	cState.pushImmediate(std::bind(&CastingState::doRemobilize, &cState));
}

// do not interrupt casting, but do immobilize first.
PLUGIN_API VOID MemoCommand(PSPAWNINFO pChar, PCHAR szLine) {
	if (!gbInZone || cState.hasCastBar() || cState.isWindowOpen() || !pSpellBookWnd) {
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
			auto testGem = cState.getGem(&szSubParam[0]);

			// got a gem, let's set it and increment the counter -- this can fail if the spellID of the next spell is less than the number of spell gems (there are a few cases of this)
			if (testGem > 0 && testGem <= NUM_SPELL_GEMS) {
				gemSlot = testGem;
				++iParam;
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Spell Gem %d Found From %s.", MQGetTickCount64(), gemSlot, szSubParam); }
			}
		}

		cState.pushCmd(CastType::Memorize, cState.memSpell(spellName, gemSlot));
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
	AddCommand("/casting", CastCommand);
	AddCommand("/interrupt", DuckCommand);
	AddCommand("/memorize", MemoCommand);
}

PLUGIN_API VOID SetGameState(unsigned long ulGameState) {
	if (GetGameState() != GAMESTATE_INGAME) {
		cState.ReloadUI();
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
	cState.Reset();
}

PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color) {
	CHAR szLine[MAX_STRING] = { 0 };
	strcpy_s(szLine, Line);
	if (gbInZone) {
		if (cState.getCurrentID() != NOID) {
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
	// get rid of the pulse check. just do stuff every pulse. be efficient.
	if (!gbInZone || !GetCharInfo() || !GetCharInfo()->pSpawn) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: No Zone.", MQGetTickCount64()); }
		return;
	}

	// execute the front of the immediate queue
	if (!cState.doImmediate()) {
		// if it's still executing, no need to worry about anything else
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Immediate Command Running.", MQGetTickCount64()); }
		return;
	}

	cState.updateTimeout();

	// are we casting something right now?
	auto detectedID = cState.detectSpellCast();
	if (detectedID != NOID) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Detected Spell Casting: %d.", MQGetTickCount64(), detectedID); }
		if (cState.getOnTargetID() == NOID) {
			cState.setOnTargetID(pTarget ? ((PSPAWNINFO)pTarget)->SpawnID : NOID);
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Setting Target ID %d.", MQGetTickCount64(), cState.getOnTargetID()); }
		}

		// was this an unexpected cast?
		if (detectedID != cState.getCurrentID()) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Unexpected Cast -> Setting Variables.", MQGetTickCount64()); }
			cState.setCurrentID(detectedID);
			cState.setCurrentResult(CastResult::Casting);
			cState.updateTimeout();
			BlechSpell(GetSpellByID(detectedID));
		}

		cState.setCurrentResult(CastResult::Casting);

		return; // we are casting, don't do anything else.
	} else if (cState.getCmdType() != CastType::None) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Nothing Casting With Queue Size %d -> Executing Command Type %d.", MQGetTickCount64(), cState.getCmdSize(), cState.getCmdType()); }
		if (cState.getOnTargetID() == NOID) {
			cState.setOnTargetID(pTarget ? ((PSPAWNINFO)pTarget)->SpawnID : NOID);
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Setting Target ID %d.", MQGetTickCount64(), cState.getOnTargetID()); }
		} else if (pTarget) {
			auto targetID = ((PSPAWNINFO)pTarget)->SpawnID;
			if (targetID != NOID) {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Wrong Target ID %d -> Set Target To ID %d.", MQGetTickCount64(), targetID, cState.getOnTargetID()); }
				// we need to target. Push a command and return.
				auto targetCmd = [targetID]() -> bool {
					if (auto target = (PSPAWNINFO)GetSpawnByID(targetID)) {
						*(PSPAWNINFO*)ppTarget = target;
						cState.setOnTargetID(targetID);
					}

					// either it set or it failed to find the spawn, we want to continue in either case.
					return true;
				};

				cState.pushImmediate(targetCmd);
				return;
			}
		}

		// these are "universal" checks, so don't include them in the individual command functions
		auto currentID = cState.getCurrentID();
		if (currentID != NOID) {
			if (GetCharInfo()->Stunned) {
				// can't do things while stunned!
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Found Stunned.", MQGetTickCount64()); }
				cState.setCurrentResult(CastResult::Stunned);
			} else if (!cState.getCastWhileInvis() && GetCharInfo()->pSpawn->HideMode) {
				// we specified not to cast while invis
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Found Invis While CastWhileInvis=false.", MQGetTickCount64()); }
				cState.setCurrentResult(CastResult::Invisible);
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
						cState.setCurrentResult(CastResult::NoTarget);
					}
					break;
				}
			}
		}

		// if we aren't done with the command, then execute it. If we are, don't do anything.
		if (cState.getCurrentResult() < CastResult::Success && GetCharInfo()) {
			auto pMySpawn = GetCharInfo()->pSpawn;
			// check the timeout first. We know we aren't actually casting anything right now
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Command Executing.", MQGetTickCount64()); }
			if (!pMySpawn || pMySpawn->TimeStamp > cState.getTimeout()) {
				// the assumption here is that we had sufficient time to cast the spell, so we probably actually cast it
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Command Timed Out With TimeStamp %d.", MQGetTickCount64(), pMySpawn->TimeStamp); }
				cState.setCurrentResult(CastResult::Success);
			} else {
				// we should be doing something right now, so let's do it.
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Running Command Function.", MQGetTickCount64()); }
				auto cmd = cState.getCmd();
				cmd.castFunc(); // do the function
			}

			cState.updateTimeout();
		}
	}

	// if we are finished, pop the command (will save off the ID and result and get ready for the next one)
	if (cState.getCurrentResult() >= CastResult::Success) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Command Finished With Result %d.", MQGetTickCount64(), cState.getCurrentResult()); }
		if (cState.decRecast() > 0) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Command Recast? (%d).", MQGetTickCount64(), cState.getRecast()); }
			switch (cState.getRecast().first) {
			case CastRecast::Dead:
				if (auto target = (PSPAWNINFO)GetSpawnByID(cState.getOnTargetID())) {
					if (target->StandState != STANDSTATE_DEAD) {
						if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Not Dead! Recasting.", MQGetTickCount64()); }
						cState.setCurrentResult(CastResult::Idle);
					}
				} // if we can't find the target, then assume its dead anyway
				break;
			case CastRecast::Land:
				if (cState.getCurrentResult() != CastResult::Success) {
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Not Successful! Recasting.", MQGetTickCount64()); }
					cState.setCurrentResult(CastResult::Idle);
				}
				break;
			case CastRecast::NTimes:
				// all we need to do here is reset to idle and do it again
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Not Done! Recasting.", MQGetTickCount64()); }
				cState.setCurrentResult(CastResult::Idle);
				break;
			default:
				break;
			};
		}
	}
	
	if (cState.getCurrentResult() >= CastResult::Success) {
		cState.popCmd();
		cState.pushImmediate(std::bind(&CastingState::doRemobilize, &cState));

		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:: Casting Complete ID[%d] Result=[%d]", MQGetTickCount64(), cState.getLastID(), cState.getLastResult());
		}
	}
}

PLUGIN_API VOID OnReloadUI() {
	cState.ReloadUI();
}

#pragma endregion

#pragma region Detection and State Functions

PCONTENTS CastingState::getContentsFromID(PCHAR ID) {
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

std::pair<DWORD, int> CastingState::getAAIndexFromID(PCHAR ID) {
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

DWORD CastingState::getSpellIDFromDiscID(PCHAR ID) {
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

ULONG CastingState::getDiscTimer(PSPELL pSpell) {
#ifndef EMU
	return pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex, pSpell->SpellGroup);
#else
	return pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex);
#endif
}

DWORD CastingState::getAbilityIDFromID(PCHAR ID) {
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
	if (cState.isBard()) {
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
		if (auto pCont = getContentsFromID(ID)) {
			if (auto pItem = GetItemFromContents(pCont)) {
				return (pItem->Clicky.TimerID != -1 && GetItemTimer(pCont) == 0) || pItem->Clicky.SpellID != NOID;
			}
		}
		return false;
	case CastType::AltAbility:
	{
		auto idxPair = getAAIndexFromID(ID);
		if (PlayerHasAAAbility(idxPair.first)) {
			if (auto pAbility = GetAAByIdWrapper(idxPair.first, idxPair.second)) {
				return pAltAdvManager && pAltAdvManager->IsAbilityReady(pPCData, pAbility, 0);
			}
		}
		return false;
	}
	case CastType::Discipline:
		if (auto pSpell = GetSpellByID(getSpellIDFromDiscID(ID))) {
			return getDiscTimer(pSpell) < (DWORD)time(NULL);
		}
		return false;
	case CastType::Ability:
		if (auto idx = getAbilityIDFromID(ID)) {
			if (idx == 105 || idx == 107) {
				return LoH_HT_Ready();
			} else {
				return pCSkillMgr && pCSkillMgr->IsAvailable(idx);
			}
		}
		return false;
	case CastType::Spell:
		if (auto pSpell = GetSpellByName(ID)) {
			int gemIdx = getGem(pSpell->ID);
			return gemIdx != -1 && cooldownTimeRemaining(gemIdx) <= 0;
		}
		return false;
	case CastType::Memorize:
		if (auto pSpell = GetSpellByName(ID)) {
			if (getGem(pSpell->ID) != -1) {
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

#pragma region Command Function Generators

std::function<void()> CastingState::castItem(PCHAR ID) {
	if (!ID) return nullptr;

	auto execItem = [this](PCONTENTS pCont) -> void {
		if (!pCont) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Item]: Failed To Lookup Item.", MQGetTickCount64()); }
			setCurrentResult(CastResult::Unknown);
		} else if (PITEMINFO pItem = GetItemFromContents(pCont)) {
			setCurrentID(pItem->Clicky.SpellID);

			CHAR szBuffer[MAX_STRING] = { 0 };
			sprintf_s(szBuffer, "\"%s\"", pItem->Name); // we have the exact name, so use it (UseItemCommand() uses quotes for exact names)
			auto f = std::bind(UseItemCmd, GetCharInfo()->pSpawn, szBuffer);

			if (pItem->Clicky.TimerID != -1) { // there is a timer, so we know it's not instant click
				if (GetItemTimer(pCont) == 0) { // the timer is 0, so it's clickable
					if (DEBUGGING) { WriteChatf("Cast = %s : szBuffer %s -- UseItemCmd() Timer Ready.", GetCharInfo()->pSpawn, szBuffer); }
					setCurrentResult(CastResult::Casting);
					f();
				} else {
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Item]: Item Timer Not Ready.", MQGetTickCount64()); }
					setCurrentResult(CastResult::NotReady);
				}
			} else if (pItem->Clicky.SpellID != NOID) { // there was no timer, but if there's a spellID, it's instant
				if (DEBUGGING) { WriteChatf("Cast = %s : szBuffer %s -- UseItemCmd() Instant Click.", GetCharInfo()->pSpawn, szBuffer); }
				setCurrentResult(CastResult::Casting);
				f();
			}
		}
	};

	return std::bind(execItem, getContentsFromID(ID));
}

std::function<void()> CastingState::castAltAbility(PCHAR ID) {
	if (!ID) return nullptr;

	auto executeAA = [this](DWORD AAIndex, int level) -> void {
		if (!PlayerHasAAAbility(AAIndex)) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/alt act]: Player Does Not Have AA %d.", MQGetTickCount64(), AAIndex); }
			setCurrentResult(CastResult::Unknown);
		} else {
			if (auto pAbility = GetAAByIdWrapper(AAIndex, level)) {
				setCurrentID(pAbility->SpellID);
				if (pAltAdvManager && pAltAdvManager->IsAbilityReady(pPCData, pAbility, 0)) {
					CHAR szBuffer[MAX_STRING] = { 0 };
					sprintf_s(szBuffer, "/alt act %d", pAbility->ID);
					if (DEBUGGING) { WriteChatf("Cast = %s : szBuffer %s -- /alt act", GetCharInfo()->pSpawn, szBuffer); }

					setCurrentResult(CastResult::Casting);
					Execute(szBuffer);
				} else {
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/alt act]: AA Not Ready.", MQGetTickCount64()); }
					setCurrentResult(CastResult::NotReady);
				}
			} else {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/alt act]: Failed AA Lookup %d %d.", MQGetTickCount64(), AAIndex, level); }
				setCurrentResult(CastResult::Unknown);
			}
		}
	};

	auto idxPair = getAAIndexFromID(ID);
	return std::bind(executeAA, idxPair.first, idxPair.second);
}

std::function<void()> CastingState::castDisc(PCHAR ID) {
	if (!ID) return nullptr;

	auto executeDisc = [this](DWORD spellID) -> void {
		if (getCurrentID() == NOID) setCurrentID(spellID);


		DWORD detectedID;

		if (spellID == NOID) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/disc]: Failed Disc Lookup.", MQGetTickCount64()); }
			setCurrentResult(CastResult::Unknown);
		} else if ((detectedID = detectSpellCast()) != NOID) {
			if (detectedID != spellID) {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/disc]: Different Spell Casting.", MQGetTickCount64()); }
				setCurrentResult(CastResult::NotReady);
			} else {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/disc]: Already Casting Disc.", MQGetTickCount64()); }
				setCurrentResult(CastResult::Casting);
				BlechSpell(GetSpellByID(detectedID));
			}
		} else {
			if (auto pSpell = GetSpellByID(spellID)) {
				if (getDiscTimer(pSpell) < (DWORD)time(NULL)) {
					CHAR szBuffer[MAX_STRING] = { 0 };
					sprintf_s(szBuffer, "/disc %d", pSpell->ID);
					if (DEBUGGING) { WriteChatf("Cast = %s : szBuffer %s -- /disc", GetCharInfo()->pSpawn, szBuffer); }

					setCurrentResult(CastResult::Casting);
					BlechSpell(pSpell);

					Execute(szBuffer);
				} else {
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/disc]: Disc Not Ready.", MQGetTickCount64()); }
					setCurrentResult(CastResult::NotReady);
				}
			} else {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[/disc]: Failed Disc Lookup By ID.", MQGetTickCount64()); }
				setCurrentResult(CastResult::Unknown);
			}
		}

	};


	return std::bind(executeDisc, getSpellIDFromDiscID(ID));
}

std::function<void()> CastingState::castAbility(PCHAR ID) {
	if (!ID) return nullptr;

	auto executeSkill = [this](DWORD abilityIdx) -> void {
		if (getCurrentID() == NOID) setCurrentID(abilityIdx);

		if (abilityIdx == NOID) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[DoAbility]: Failed To Find Ability.", MQGetTickCount64()); }
			setCurrentResult(CastResult::Unknown);
		} else if (getCurrentResult() != CastResult::Idle) {
			// we attempted to use the ability, now it is used
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[DoAbility]: Ability Use Detected.", MQGetTickCount64()); }
			setCurrentResult(CastResult::Success);
		} else if (abilityIdx != 105 && abilityIdx != 107 && (!pCSkillMgr || !pCSkillMgr->IsAvailable(abilityIdx))) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[DoAbility]: Ability Not Ready.", MQGetTickCount64()); }
			setCurrentResult(CastResult::NotReady);
		} else if ((abilityIdx == 105 || abilityIdx == 107) && !LoH_HT_Ready()) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[DoAbility]: LoH/HT Not Ready.", MQGetTickCount64()); }
			setCurrentResult(CastResult::NotReady);
		} else {
			setCurrentResult(CastResult::Casting);

			CHAR szBuffer[MAX_STRING] = { 0 };
			sprintf_s(szBuffer, "%d", abilityIdx);
			if (DEBUGGING) { WriteChatf("Cast = %s : szBuffer %s -- DoAbility()", GetCharInfo()->pSpawn, szBuffer); }
			DoAbility(GetCharInfo()->pSpawn, szBuffer);
		}
	};

	return std::bind(executeSkill, getAbilityIDFromID(ID));
}

std::function<void()> CastingState::castSpell(PCHAR ID, int gemSlot) {
	if (!ID) return nullptr;

	// assume we either mean spell ID or spell name. Do not cast by gem here -- ergo,
	// pass in the spellID if the user specifies gem. There are collisions between
	// gem number and spellID.

	// TODO: Handle `loc x y z` for splash casts here -- potentially pass args in CastCommand
	auto executeCast = [this](int gemIdx, DWORD spellID, bool doWait = false) -> void {
		if (getCurrentID() == NOID) setCurrentID(spellID);

		DWORD detectedID;

		if (spellID == NOID) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Failed Spell Lookup.", MQGetTickCount64()); }
			setCurrentResult(CastResult::Unknown);
		} else if (gemIdx == -1) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Failed Spell Mem.", MQGetTickCount64()); }
			setCurrentResult(CastResult::NotReady);
		} else if ((detectedID = detectSpellCast()) != NOID) {
			if (detectedID != spellID) { // TODO: do we re-queue here?
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Different Spell Casting.", MQGetTickCount64()); }
				setCurrentResult(CastResult::NotReady);
			} else {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Already Casting Spell.", MQGetTickCount64()); }
				setCurrentResult(CastResult::Casting);
				BlechSpell(GetSpellByID(detectedID));
			}
		} else {
			if (auto pSpell = GetSpellByID(spellID)) {
				if (!isImmobile() && !isBard()) {
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Moving While Casting.", MQGetTickCount64()); }
					doNext([this]() { doImmobilize(); return isImmobile(); });
					setCurrentResult(CastResult::Idle);
				} else if (doWait && cooldownTimeRemaining(gemIdx) > 0) {
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Spell Memming Refresh.", MQGetTickCount64()); }
					setCurrentResult(CastResult::Memorizing);
				} else if (cooldownTimeRemaining(gemIdx) <= 0) {
					CHAR szBuffer[MAX_STRING] = { 0 };
					sprintf_s(szBuffer, "%d", gemIdx);
					if (DEBUGGING) { WriteChatf("Cast = %s : szBuffer %s -- Cast()", GetCharInfo()->pSpawn, szBuffer); }

					setCurrentResult(CastResult::Casting);
					BlechSpell(pSpell);

					Cast(GetCharInfo()->pSpawn, szBuffer);
				} else {
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Spell Not Ready.", MQGetTickCount64()); }
					setCurrentResult(CastResult::NotReady);
				}
			} else {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Failed Spell Lookup By ID.", MQGetTickCount64()); }
				setCurrentResult(CastResult::Unknown);
			}
		}
	};

	// let's exploit the fact that GetSpellByName() will find the ID or the name
	if (auto pSpell = GetSpellByName(ID)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Found Spell: %s.", MQGetTickCount64(), pSpell->Name); }
		int gemIdx = getGem(pSpell->ID);
		if (gemIdx != -1) {
			// we have the spell memorized
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Found Spell In Slot %d.", MQGetTickCount64(), gemIdx); }
			return std::bind(executeCast, gemIdx, pSpell->ID);
		} else if (gemSlot != -1) {
			// okay, it's not memmed, let's try to mem it first
			// push the mem command (queues are FIFO)
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Memming Spell In Slot %d.", MQGetTickCount64(), gemSlot); }
			pushCmd(CastType::Memorize, memSpell(pSpell->Name, gemSlot));

			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Then Cast Command Generated.", MQGetTickCount64()); }
			// then return the function that will get pushed to the queue after the mem
			return std::bind(executeCast, gemSlot, pSpell->ID, true);
		} else {
			// we don't have it memmed and we don't want to mem it, this will result in a NotReady
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Unmemmed Spell -- Not Casting.", MQGetTickCount64()); }
			return std::bind(executeCast, -1, pSpell->ID);
		}
	}

	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Cast]: Empty Cast Command Generated.", MQGetTickCount64()); }
	// spell lookup failed, this with result in Unknown
	return std::bind(executeCast, -1, NOID);
}

std::function<void()> CastingState::memSpell(PCHAR ID, int gemSlot) {
	if (!ID) return nullptr;

	auto memCmd = [this, ID, gemSlot]() -> void {
		if (getCurrentID() == NOID) setCurrentID(gemSlot);

		if (!pSpellBookWnd) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: No Book Window.", MQGetTickCount64()); }
			setCurrentResult(CastResult::Unknown);
		} else if (auto pSpell = GetSpellByName(ID)) {
			if (getGem(pSpell->ID) != -1) {
				// it's memmed, we're done
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Memmed.", MQGetTickCount64()); }
				setCurrentResult(CastResult::Success);
			} else if (!isImmobile()) {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Moving While Memming.", MQGetTickCount64()); }
				doNext([this]() { doImmobilize(); return isImmobile(); });
				setCurrentResult(CastResult::Idle);
			} else if (getCurrentResult() == CastResult::Idle && gemSlot > 0 && gemSlot <= NUM_SPELL_GEMS && GetCharInfo2()) {
				// we're idle and we have a valide gemslot to mem to, let's start memorizing
				for (DWORD spellIdx = 0; spellIdx < NUM_BOOK_SLOTS; ++spellIdx) {
					if (GetCharInfo2()->SpellBook[spellIdx] == pSpell->ID) {
						setCurrentResult(CastResult::Memorizing);

						CHAR szBuffer[MAX_STRING] = { 0 };
						sprintf_s(szBuffer, "gem%d %s", gemSlot, pSpell->Name);
						if (DEBUGGING) { WriteChatf("Cast = %s : szBuffer %s -- MemSpell()", GetCharInfo()->pSpawn, szBuffer); }
						MemSpell(GetCharInfo()->pSpawn, szBuffer);

						return; // bail here -- we found the spell
					}
				}

				// went through the whole spellbook and didn't find anything
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Spell Not In Book.", MQGetTickCount64()); }
				setCurrentResult(CastResult::Unknown);
			} else if (getCurrentResult() == CastResult::Idle) {
				// still idle, which means we failed the gem validation
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Invalid Gem.", MQGetTickCount64()); }
				setCurrentResult(CastResult::Unknown);
			} // else maintain the current result
		} else {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Spell Lookup Failed.", MQGetTickCount64()); }
			setCurrentResult(CastResult::Unknown);
		}

		// we've finished, close the book if its open.
		if (getCurrentResult() >= CastResult::Success && pSpellBookWnd && ((PCSIDLWND)pSpellBookWnd)->dShow) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Closebook.", MQGetTickCount64()); }
			Execute("/book");
		}
	};

	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Memorize]: Mem Command Generated.", MQGetTickCount64()); }
	return memCmd;
}

#pragma endregion

#pragma region Immediate Functions

bool CastingState::doImmobilize() {
	// TODO: should bound the timeframe for stopping in case some unaccounted for plugin won't stop moving you

	// go through various known plugins that move your character and request that they pause while casting
	if (Evaluate("${If[${Stick.Status.Equal[ON]},1,0]}")) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: Stick Pause Request.", MQGetTickCount64()); }
		addPauseMask(PauseCommand::Stick);
		Stick("pause");
	}

	//looks like AdvPath.inc is loaded... ok then, fine we will check its tlo...
	if (FindMQ2DataVariable("FollowFlag") && Evaluate("${If[${Bool[${FollowFlag}]},1,0]}")) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: AdvPath Pause Request.", MQGetTickCount64()); }
		addPauseMask(PauseCommand::FollowFlag);
		Execute("/varcalc PauseFlag 1");
	}

	if (FindMQ2DataType("Navigation") && Evaluate("${If[${Navigation.Active} && !${Navigation.Paused},1,0]}")) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: MQ2Nav Pause Request.", MQGetTickCount64()); }
		addPauseMask(PauseCommand::Navigation);
		Nav("pause");
	}

	if (FindMQ2DataType("AdvPath")) {
		if (Evaluate("${If[${AdvPath.Following} && !${AdvPath.Paused},1,0]}")) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: MQ2AdvPath Pause Request.", MQGetTickCount64()); }
			addPauseMask(PauseCommand::FollowPath);
			FollowPath("pause");
		}
		if (Evaluate("${If[${AdvPath.Playing} && !${AdvPath.Paused},1,0]}")) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: MQ2AdvPath Pause Request.", MQGetTickCount64()); }
			addPauseMask(PauseCommand::Path);
			Path("pause");
		}
	}

	ExecuteCmd(FindMappableCommand("back"), 1, 0);
	ExecuteCmd(FindMappableCommand("back"), 0, 0);

	return isImmobile();
}

bool CastingState::doRemobilize() {
	// go through various known plugins that move your character and request that they pause while casting
	if (isPaused(PauseCommand::Stick)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: Stick Pause Request.", MQGetTickCount64()); }
		Stick("unpause");
	}

	if (isPaused(PauseCommand::FollowFlag)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: AdvPath Pause Request.", MQGetTickCount64()); }
		Execute("/varcalc PauseFlag 0");
	}

	if (isPaused(PauseCommand::Navigation)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: MQ2Nav Pause Request.", MQGetTickCount64()); }
		Nav("pause");
	}

	if (isPaused(PauseCommand::FollowPath)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: MQ2AdvPath Pause Request.", MQGetTickCount64()); }
		FollowPath("unpause");
	}
	if (isPaused(PauseCommand::Path)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: MQ2AdvPath Pause Request.", MQGetTickCount64()); }
		Path("unpause");
	}

	clearPauseMask();
	return true; // this is an instant action
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

long CastingState::cooldownTimeRemaining(int gemIdx) {
	auto pMyPlayer = (PSPAWNINFO)pLocalPlayer;
	if (!pMyPlayer || gemIdx < 1 || gemIdx >= NUM_SPELL_GEMS)
		return 0xFFFFFFFF;

	return max(pMyPlayer->SpellGemETA[gemIdx - 1], pMyPlayer->SpellCooldownETA) - GetCharInfo()->pSpawn->TimeStamp;
}

int CastingState::getGem(LONG spellID) {
	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Gem]: Searching Gems For %d.", MQGetTickCount64(), spellID); }
	for (int gemIdx = 0; gemIdx < NUM_SPELL_GEMS; ++gemIdx) {

		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Gem]: Testing Gem %d.", MQGetTickCount64(), gemIdx); }

		if (PCHARINFO pCharInfo = GetCharInfo()) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Gem]: Got Char Info %d.", MQGetTickCount64(), pCharInfo); }
			if (pCharInfo->pCharacterBase) {
				if (CharacterBase *cb = (CharacterBase *)&pCharInfo->pCharacterBase) {
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Gem]: Got Char Base %d.", MQGetTickCount64(), cb); }
					auto gemID = cb->GetMemorizedSpell(gemIdx);

					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Gem]: Got Gem ID %d.", MQGetTickCount64(), gemID); }
					if (gemID == spellID) {
						return gemIdx;
					}
				}
			}
		}
		/*
		if (GetMemorizedSpell(gemIdx) == spellID) {
			return gemIdx;
		}*/
	}

	return -1;
}

int CastingState::getGem(PCHAR ID) {
	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Gem]: Has Gem \"%s\".", MQGetTickCount64(), ID); }
	if (strlen(ID) >= 3 && !_strnicmp(ID, "gem", 3)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Gem]: Parsing Gem As A String.", MQGetTickCount64()); }
		return atoi(&ID[3]);
	} else if (IsNumber(ID)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Gem]: Parsing Gem As A Number.", MQGetTickCount64()); }
		return atoi(ID);
	}

	return -1;
}

#pragma region Queue Functions

void CastingState::pushCmd(CastingState::MQ2CastCommand cmd) {
	// last I heard, MSVC didn't do emplace very well, so just use push
	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Queue]: Pushing a Command of Type %d to the Queue.", MQGetTickCount64(), cmd.castType); }
	cmdQueue.push(cmd);
}

void CastingState::pushCmd(CastType type, CastingState::MQ2CastFunc func) {
	pushCmd(MQ2CastCommand(type, func));
}

void CastingState::pushCmd(CastType type, CastingState::MQ2CastFunc func, std::queue<ImmediateCommand> preCmd, std::queue<ImmediateCommand> postCmd) {
	pushCmd(MQ2CastCommand(type, func, preCmd, postCmd));
	updateTimeout();
}

int CastingState::getCmdSize() {
	return cmdQueue.size();
}

CastingState::MQ2CastCommand CastingState::getCmd() {
	if (cmdQueue.empty()) {
		return MQ2CastCommand(CastType::None, nullptr);
	}

	return cmdQueue.front();
}

CastType CastingState::getCmdType() {
	if (cmdQueue.empty()) {
		return CastType::None;
	}

	return cmdQueue.front().castType;
}

void CastingState::clearCmds() {
	while (!cmdQueue.empty()) {
		cmdQueue.pop();
	}
}

void CastingState::popCmd() {
	if (!cmdQueue.empty()) {
		setLastID(getCurrentID());
		setLastResult(getCurrentResult());

		setOnTargetID(NOID);
		setCurrentID(NOID);
		setCurrentResult(CastResult::Idle);

		cmdQueue.pop();
	}
}

void CastingState::doNext(CastingState::ImmediateCommand cmd) {
	immediateQueue.push_front(cmd);
}

void CastingState::pushImmediate(CastingState::ImmediateCommand cmd) {
	immediateQueue.push_back(cmd);
}

// we need to perform the immediate queue front and if it finished (returns true) pop it.
bool CastingState::doImmediate() {
	if (immediateQueue.empty()) {
		return true;
	}

	auto finished = immediateQueue.front()();
	if (finished) {
		immediateQueue.pop_front();
	}

	return finished && immediateQueue.empty();
}

#pragma endregion

ULONGLONG CastingState::updateTimeout() {
	if (!GetCharInfo()) {
		return Timeout;
	}

	if (auto pMySpawn = GetCharInfo()->pSpawn) {
		if (pMySpawn->CastingData.SpellETA > 0) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Timeout]: Found Spell ETA %d -> TimeStamp %d.", MQGetTickCount64(), pMySpawn->CastingData.SpellETA, pMySpawn->TimeStamp); }
			// TODO: potentially account for lag here (+ MQGetTickCount64() + 450 + (pConnection->Last) * 4;)
			Timeout = pMySpawn->TimeStamp + pMySpawn->CastingData.SpellETA;
		} else if (getCurrentID() != NOID) {
			if (auto pSpell = GetSpellByID(getCurrentID())) {
				if (getCmdType() == CastType::Memorize) {
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Timeout]: Memming TimeStamp %d.", MQGetTickCount64(), pMySpawn->TimeStamp); }
					// TODO: Can I get a memorization time?
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