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
#include <string>
#include <sstream>

#include "MQ2Cast.h"

#include "../Blech/Blech.h"

using namespace MQ2Cast;

#ifdef PLUGIN_API
PreSetup("MQ2Cast");
PLUGIN_VERSION(12.0);
#endif

#ifndef MAX_STRING
#define MAX_STRING 2048
#endif

bool MQ2Cast::DEBUGGING = false;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
bool         Parsed = false;            // BTree List Found Flags
Blech         LIST013('#');            // BTree List for OnChat Message on Color  13
Blech         LIST015('#');            // BTree List for OnChat Message on Color  15
Blech         LIST264('#');            // BTree List for OnChat Message on Color 264
Blech		  LIST271('#');			   // BTree List for OnChat Message on Color 271
Blech		  LIST273('#');			   // BTree List for OnChat Message on Color 273
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
		WriteChatf("[%I64u] MQ2Cast:[OnChat]: Result=[%s] Called=[%d].", MQGetTickCount64(), GetReturnString(CastingState::instance().getCurrentResult()).c_str(), (long)pData);
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

#pragma endregion

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#pragma region Helper Functions

const std::string MQ2Cast::GetReturnString(const CastResult Result) {
	switch (Result) {
	case CastResult::Idle:		   return std::string("CAST_IDLE");
	case CastResult::Memorizing:   return std::string("CAST_MEMORIZING");
	case CastResult::Casting:	   return std::string("CAST_CASTING");
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

const PMQCOMMAND MQ2Cast::FindMQCommand(const PCHAR Command) {
	auto pCheck = pCommands;
	while (pCheck) {
		if (!_stricmp(pCheck->Command, Command)) {
			return pCheck;
		}

		pCheck = pCheck->pNext;
	}

	return nullptr;
}

const DWORD MQ2Cast::GetDWordMember(MQ2Type* Type, const PCHAR Member) {
// I hate that Type can't be const
if (!Type) return 0;

MQ2TYPEVAR MemberVar;
Type->GetMember(MemberVar.VarPtr, Member, "", MemberVar);

return MemberVar.DWord;
}

void MQ2Cast::SetEQInput(const DWORD Offset, const PCHAR Command, const char Value) {
	auto iCommand = FindMappableCommand(Command);
	if (pKeypressHandler && iCommand >= 0) {
		pKeypressHandler->CommandState[iCommand] = Value;
	}

	auto pulCommand = (unsigned long *)FixOffset(Offset);
	if (pulCommand) {
		*pulCommand = (unsigned long)Value;
	}
}

const fEQCommand MQ2Cast::FindEQCommand(const PCHAR Command) {
	if (auto pCmdList = (PCMDLIST)EQADDR_CMDLIST) {
		for (int iCmd = 0; pCmdList[iCmd].fAddress; ++iCmd) {
			if (!_strnicmp(pCmdList[iCmd].szName, Command, strlen(Command))) {
				return (fEQCommand)pCmdList[iCmd].fAddress;
			}
		}
	}

	return nullptr;
}

void MQ2Cast::Execute(const PCHAR Command, const PCHAR zFormat, ...) {
	auto pMyChar = GetCharInfo();
	if (!pMyChar) return;

	if (auto fCommand = FindEQCommand(Command)) {
		char zArgs[MAX_STRING] = { 0 };
		va_list vaList;
		va_start(vaList, zFormat);
		vsprintf_s(zArgs, zFormat, vaList);

		fCommand(pMyChar->pSpawn, zArgs);
	}
}

void MQ2Cast::Execute(const fEQCommand Command, const PCHAR zFormat, ...) {
	if (!Command) return;

	if (auto pMyChar = GetCharInfo()) {
		char zArgs[MAX_STRING] = { 0 };
		va_list vaList;
		va_start(vaList, zFormat);
		vsprintf_s(zArgs, zFormat, vaList);

		Command(pMyChar->pSpawn, zArgs);
	}
}


bool MQ2Cast::IsValidTarget(const PSPELL pSpell, const PSPAWNINFO pSpellTarget) {
	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[IsValidTarget]: Entry [%d] Target [%d] TargetType: %d.", MQGetTickCount64(), pSpell, pSpellTarget, pSpell->TargetType); }
	// self is not a valid target for cases 5,7-23,27,28,30,34,35,38,39,43,46,47,50
	switch ((CastTarget)pSpell->TargetType) {
	case CastTarget::FreeTargetAE:
		if (pSpellTarget) {
			return IsInRange(pSpell, pSpellTarget);
		} // else check for range of click
		return true;
	/* These are checked in the client before sending to the server, so are probably not really necessary to check again since we can check chat as a sort of return. */
	/* We can go as far as actually checking the body types and stuff, but not really worth it. */
	case CastTarget::Aimed:
	case CastTarget::Bolt:
	case CastTarget::Single:
	case CastTarget::TargetedAE:
	case CastTarget::Animal:
	case CastTarget::Undead:
	case CastTarget::Summoned:
	case CastTarget::Flyer:
	case CastTarget::Lifetap:
	case CastTarget::Corpse:
	case CastTarget::Plant:
	case CastTarget::VeliousGiant:
	case CastTarget::VeliousDragon: // why not Coldain? -- because there is no spell with the Coldain target type
	case CastTarget::GroupLifetap:
	case CastTarget::TargetedAEUndead:
	case CastTarget::TargetedAESummoned:
	case CastTarget::TargetedAEAnimal:
	case CastTarget::Insect:
	case CastTarget::TargetedAEInsect:
	case CastTarget::Object:
	case CastTarget::Muramite:
	case CastTarget::SinglePCOnly:
	case CastTarget::SingleGroupMember:
	case CastTarget::PetsOwner:
	case CastTarget::TargetedAEEnemies:
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[IsValidTarget]: TargetType: %s.", MQGetTickCount64(), GetTargetTypeName((CastTarget)pSpell->TargetType).c_str()); }
		return (pSpellTarget && IsInRange(pSpell, pSpellTarget));
	/* End client redundancy. */
	default:
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[IsValidTarget]: (default)TargetType: %s.", MQGetTickCount64(), GetTargetTypeName((CastTarget)pSpell->TargetType).c_str()); }
		return true;
	}
}

bool MQ2Cast::IsInRange(const PSPELL pSpell, FLOAT Y, FLOAT X, FLOAT Z, BOOL doFocus) {
	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[IsInRange]: Entry [%d] Y: %f X: %f Z: %f doFocus: %d.", MQGetTickCount64(), pSpell, Y, X, Z, doFocus); }
	auto pMyChar = GetCharInfo();
	if (!pMyChar || !pMyChar->pSpawn) {
		return false;
	}

	FLOAT range = pSpell->Range;
	if (doFocus) {
		// we need to grab the focusrangemod
		DWORD n = 0;
		FLOAT rangeMod = (FLOAT)GetFocusRangeModifier((EQ_Spell*)pSpell, (EQ_Equipment**)&n);
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[IsInRange]: RangeModCheck range: %f rangeMod: %f.", MQGetTickCount64(), range, rangeMod); }
		range = min(range + rangeMod, range * 5.0f); // this is the calculation from the client everytime we grab the focus mod
	}

	// the client does a 3D check, so we will too
	FLOAT dist = Distance3DToPoint(pMyChar->pSpawn, X, Y, Z);
	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[IsInRange]: DistCalc -- %f MinRange: %f range: %f.", MQGetTickCount64(), dist, pSpell->MinRange, range); }

	// the client does this in squared distances because it doesn't calculate sqrt for the distance calculation, but this should be the same and I CBA to write a new dist function
	return dist >= pSpell->MinRange && dist <= range;
}

bool MQ2Cast::IsInRange(const PSPELL pSpell, const PSPAWNINFO pSpellTarget) {
	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[IsInRange]: Entry [%d] target [%d].", MQGetTickCount64(), pSpell, pSpellTarget); }
	if (!pSpellTarget) return false;

	return IsInRange(pSpell, pSpellTarget->Y, pSpellTarget->X, pSpellTarget->Z, pSpellTarget->Type == SPAWN_PLAYER || pSpellTarget->Mercenary);
}

const std::string MQ2Cast::GetTargetTypeName(const CastTarget TargetType) {
	switch (TargetType) {
		case CastTarget::Aimed:					return std::string("Aimed");
		case CastTarget::Bolt:					return std::string("Bolt");
		case CastTarget::Area:					return std::string("Area");
		case CastTarget::CasterGroup:			return std::string("Caster Group");
		case CastTarget::PBAE:					return std::string("PB AE");
		case CastTarget::Single:				return std::string("Single");
		case CastTarget::Self:					return std::string("Self");
		case CastTarget::HeldItem:				return std::string("Held Item");
		case CastTarget::TargetedAE:			return std::string("Targeted AE");
		case CastTarget::Animal:				return std::string("Animal");
		case CastTarget::Undead:				return std::string("Undead");
		case CastTarget::Summoned:				return std::string("Summoned");
		case CastTarget::Flyer:					return std::string("Flyer");
		case CastTarget::Lifetap:				return std::string("Lifetap");
		case CastTarget::Pet:					return std::string("Pet");
		case CastTarget::Corpse:				return std::string("Corpse");
		case CastTarget::Plant:					return std::string("Plant");
		case CastTarget::VeliousGiant:			return std::string("Velious Giant");
		case CastTarget::VeliousDragon:			return std::string("Velious Dragon");
		case CastTarget::Coldain:				return std::string("Coldain");
		case CastTarget::GroupLifetap:			return std::string("Group Lifetap");
		case CastTarget::TargetedAEUndead:		return std::string("Targeted AE (Undead)");
		case CastTarget::TargetedAESummoned:	return std::string("Targeted AE (Summoned)");
		case CastTarget::TargetedAEAnimal:		return std::string("Targeted AE (Animal)");
		case CastTarget::PBAEUndead:			return std::string("PB AE (Undead)");
		case CastTarget::PBAESummoned:			return std::string("PB AE (Summoned)");
		case CastTarget::PBAEAnimal:			return std::string("PB AE (Animal)");
		case CastTarget::Insect:				return std::string("Insect");
		case CastTarget::TargetedAEInsect:		return std::string("Targeted AE (Insect)");
		case CastTarget::PBAEInsect:			return std::string("PB AE (Insect)");
		case CastTarget::TargetedAEPlant:		return std::string("Targeted AE (Plant)");
		case CastTarget::PBAEPlant:				return std::string("PB AE (Plant)");
		case CastTarget::HatelistPBAE:			return std::string("Hatelist PB AE");
		case CastTarget::Hatelist:				return std::string("Hatelist");
		case CastTarget::Object:				return std::string("Object");
		case CastTarget::Muramite:				return std::string("Muramite");
		case CastTarget::PBAEPlayersOnly:		return std::string("PB AE (Players Only)");
		case CastTarget::PBAENPCsOnly:			return std::string("PB AE (NPCs Only)");
		case CastTarget::SummonedPet:			return std::string("Summoned Pet");
		case CastTarget::SinglePCOnly:			return std::string("Single (PC Only)");
		case CastTarget::PBAEFriendly:			return std::string("PB AE (Friendly)");
		case CastTarget::TargetGroup: 			return std::string("Target Group");
		case CastTarget::DirectionalAE:			return std::string("Directional AE");
		case CastTarget::SingleGroupMember:		return std::string("Single (Group Member)");
		case CastTarget::Beam:					return std::string("Beam");
		case CastTarget::FreeTargetAE:			return std::string("Free Target AE");
		case CastTarget::SingleTargetsTarget:	return std::string("Single Target's Target");
		case CastTarget::PetsOwner:				return std::string("Pet's Owner");
		case CastTarget::PBAEEnemies:			return std::string("PB AE (Enemies)");
		case CastTarget::RealEstate:			return std::string("Real Estate");
		case CastTarget::TargetedAEEnemies:		return std::string("Targeted AE (Enemies)");
		default:								return std::string("Unknown");
	}

	return std::string("Unknown");
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
		Test = 10,
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
		TypeMember(Test);
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
			case Test:
				Dest.Type = pStringType;
				std::string returnStr;
				PSPAWNINFO pCheckTarget;
				if (pTarget) {
					returnStr += "Target: ";
					pCheckTarget = reinterpret_cast<PSPAWNINFO>(pTarget);
				}
				PSPELL pSpell;
				// not sure what all this checks, but the client calls this in their cast function -- let's get it out of the way early
				pSpellMgr->DoesMeetRequirement((PlayerZoneClient *)pCheckTarget, pSpell->SpellReqAssociationID);
				if (pCharData && pCharData1) {
					//pCharData1->CastSpell();
				}
				if (Index[0]) {

				}
				const char *retPChar = returnStr.c_str();
				Dest.Ptr = &retPChar;
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
	if (!gbInZone || 
		CastingState::instance().isCasting() || 
		CastingState::instance().hasCastBar() || 
		CastingState::instance().isWindowOpen() || 
		CastingState::instance().getCmdType() != CastType::None ||
		(pSpellBookWnd && ((PCSIDLWND)pSpellBookWnd)->dShow)) {
		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[Casting]: Cannot Cast. [%d][%s%s%s%s]", MQGetTickCount64(), GetReturnString(CastingState::instance().getCurrentResult()).c_str(),
				gbInZone ? " ZONE " : "", 
				CastingState::instance().isCasting() ? " isCasting " : "", 
				CastingState::instance().hasCastBar() ? " hasCastBar " : "", 
				CastingState::instance().isWindowOpen() ? " isWindowOpen " : "", 
				CastingState::instance().getCmdType() != CastType::None ? " hasCommandQueued " : "", 
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
		} else if (!_strnicmp(castTypeInput, "abil", 4)) {
			castType = CastType::Ability;
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

PLUGIN_API VOID SpellSetDelete(PSPAWNINFO pChar, PCHAR Cmd) {
	if (!gbInZone) {
		return;
	} else if (!Cmd) {
		MacroError("Usage: /ssd setname");
	} else {
		sprintf_s(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
		WritePrivateProfileString("MQ2Cast(SpellSet)", Cmd, NULL, INIFileName);
	}
}

PLUGIN_API VOID SpellSetList(PSPAWNINFO pChar, PCHAR Cmd) {
	if (!gbInZone)
		return;

	char Keys[MAX_STRING * NUM_SPELL_GEMS] = { 0 };
	char Temp[MAX_STRING];
	PCHAR pKeys = Keys;
	long Disp = 0;

	sprintf_s(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
	WriteChatf("MQ2Cast:: SpellSet [\ay Listing... \ax].", Disp);
	GetPrivateProfileString("MQ2Cast(SpellSet)", NULL, "", Keys, MAX_STRING * NUM_SPELL_GEMS, INIFileName);

	while (pKeys) {
		GetPrivateProfileString("MQ2Cast(SpellSet)", pKeys, "", Temp, MAX_STRING, INIFileName);
		if (Temp[0]) {
			if (!Disp)
				WriteChatf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
			WriteChatf("\ay%s\ax", pKeys);
			Disp++;
		}

		pKeys += strlen(pKeys) + 1;
	}

	if (Disp)
		WriteChatf("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
	WriteChatf("MQ2Cast:: SpellSet [\ay %d Displayed\ax ].", Disp);
}

PLUGIN_API VOID SpellSetMemorize(PSPAWNINFO pChar, PCHAR Cmd) {
	if (!gbInZone) {
		return;

	} else if (!Cmd) {
		MacroError("Usage: /ssm setname");
	} else {
		char List[MAX_STRING];
		sprintf_s(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
		GetPrivateProfileString("MQ2Cast(SpellSet)", Cmd, "", List, MAX_STRING, INIFileName);
		if (List[0]) {
			MemoCommand(GetCharInfo()->pSpawn, List);
		} else {
			MacroError("/ssm Could not find setname %s", Cmd);
		}
	}
}

PLUGIN_API VOID SpellSetSave(PSPAWNINFO pChar, PCHAR Cmd) {
	if (!gbInZone)
		return;

	if (!Cmd) {
		MacroError("Usage: /sss setname [gemlist]");
		return;
	}

	auto pMyChar2 = GetCharInfo2();
	if (!pMyChar2)
		return;

	char zSet[MAX_STRING]; GetArg(zSet, Cmd, 1);
	char zGem[MAX_STRING]; GetArg(zGem, Cmd, 2);

	if (!zSet[0]) {
		MacroError("Usage: /sss setname [gemlist]");
		return;
	}

	// The basic conceit is that we can represent each gem with a single character in base-16. If we ever have more than 15 gem slots, this will break.
	std::string sGemList;
	if (!zGem[0]) {
		std::stringstream ssGem;
		for (int iGem = 1; iGem <= NUM_SPELL_GEMS; ++iGem)
			ssGem << std::hex << iGem; // here's the limitation. If we have more than base-16, someone will have to write a custom stream manipulator, or redo this code.

		sGemList = ssGem.str();
	} else {
		sGemList = std::string(zGem);
	}

	std::string sList;
	for (char& cGemSlot : sGemList) {
		int iGemSlot = strtol(std::string(1, cGemSlot).c_str(), NULL, 16); // this is tied to the limitation above. Luckily, we can just increase the last argument if we need to.

		// if strtol gets a character it can't handle, it will return 0. Luckily for us, 0 is an invalid slot.
		if (iGemSlot <= 0 || iGemSlot > NUM_SPELL_GEMS || (long)pMyChar2->MemorizedSpells[iGemSlot - 1] <= 0)
			continue;

		if (!sList.empty() && sList.back() != ' ') {
			sList += ' ';
		}

		sList += std::to_string(pMyChar2->MemorizedSpells[iGemSlot - 1]);
		sList += '|';
		sList += std::to_string(iGemSlot);
	}

	if (!sList.empty()) {
		sprintf_s(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
		WritePrivateProfileString("MQ2Cast(SpellSet)", Cmd, sList.c_str(), INIFileName);
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
	// from here down: evaluate the return type, I've added a lot of new things here.
	aCastEvent(LIST271, CastResult::Distracted, "You are already using a discipline!#*#");
	aCastEvent(LIST289, CastResult::Distracted, "You are too distracted to use a skill.#*#");
	aCastEvent(LIST289, CastResult::Distracted, "You are too distracted to cast a spell now!#*#");
	aCastEvent(LIST289, CastResult::NotReady, "You are already casting a spell!#*#");
	aCastEvent(LIST289, CastResult::Unknown, "You are not high enough level to cast this spell!#*#");
	aCastEvent(LIST015, CastResult::Distracted, "Warning, you have many items waiting on your cursor.  Please place these items before summoning more.#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "You must hold an item to cast the spell upon.#*#");
	aCastEvent(LIST289, CastResult::OutOfRange, "You are too close to your target. Get farther away.#*#");
	aCastEvent(LIST289, CastResult::OutOfRange, "Your target is out of range, get closer!#*#");
	aCastEvent(LIST289, CastResult::Distracted, "Casting #*# would result in your death!#*#");
	aCastEvent(LIST289, CastResult::Distracted, "You can not cast this spell while in combat.#*#");
	aCastEvent(LIST289, CastResult::Distracted, "You can not use this ability while in combat.#*#");
	aCastEvent(LIST289, CastResult::Distracted, "You can not cast this spell while out of combat.#*#");
	aCastEvent(LIST289, CastResult::Distracted, "You can not use this ability while out of combat.#*#");
	aCastEvent(LIST289, CastResult::Distracted, "You can only cast this spell while in a resting state.#*#");
	aCastEvent(LIST289, CastResult::Distracted, "You can only use this ability while in a resting state.#*#");
	aCastEvent(LIST013, CastResult::Components, "You cannot use this item unless it is equipped.#*#");
	aCastEvent(LIST289, CastResult::Standing, "You need to at least be conscious to sing!#*#");
	aCastEvent(LIST289, CastResult::Standing, "You must stand upright and still in order to cast!#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "This spell only works on other PCs.#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "This spell only works on NPCs.#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "You must first target a living group member whose corpse you wish to summon.#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "You do not have a pet.#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "This effect only works with summoned pets.#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "You must first select a target for this ability!#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "You must first select a target for this spell!#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "Your target does not meet the spell requirements.#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "You can't drain yourself!#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "That spell can not affect this target PC.#*#");
	aCastEvent(LIST289, CastResult::NoTarget, "You must first target a group member.#*#");
	aCastEvent(LIST289, CastResult::OutOfMana, "Insufficient Endurance to activate this ability!#*#");
	aCastEvent(LIST289, CastResult::TakeHold, "You cannot seem to develop an affinity for this place. Try a city.#*#");
	aCastEvent(LIST273, CastResult::Components, "Item is out of charges.#*#");
	aCastEvent(LIST013, CastResult::Components, "You cannot use this item unless it is attuned.#*#");
	aCastEvent(LIST013, CastResult::Components, "You are not sufficient level to use this item.#*#");
	aCastEvent(LIST289, CastResult::NotReady, "You can not use this ability because you have not been hidden for long enough.#*#");
	aCastEvent(LIST289, CastResult::Stunned, "You cannot use a skill while stunned!#*#");
	aCastEvent(LIST289, CastResult::Distracted, "You do not have control of yourself right now.#*#");
	pCastType = new MQ2CastType;
	AddMQ2Data("Cast", dataCast);
	AddCommand("/castdebug", CastDebug);
	AddCommand("/casting", CastingCommand);
	AddCommand("/interrupt", DuckCommand);
	AddCommand("/memorize", MemoCommand);
	AddCommand("/ssd", SpellSetDelete);
	AddCommand("/ssl", SpellSetList);
	AddCommand("/ssm", SpellSetMemorize);
	AddCommand("/sss", SpellSetSave);
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
	RemoveCommand("/ssd");
	RemoveCommand("/ssl");
	RemoveCommand("/ssm");
	RemoveCommand("/sss");
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

			//if (DEBUGGING) { WriteChatf("[%I64u] OnIncomingChat:: ChatLine: %s Color: %d", MQGetTickCount64(), szLine, Color); }

			if (Color == 264) {
				LIST264.Feed(szLine);
				SUCCESS.Feed(szLine);
			}  else if (Color == 271) {
				LIST271.Feed(szLine);
			}  else if (Color == 273) {
				LIST273.Feed(szLine);
			} else if (Color == 289) {
				LIST289.Feed(szLine);
			} else if (Color == 13) {
				LIST013.Feed(szLine);
			} else if (Color == 15) {
				LIST015.Feed(szLine);
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

	// TODO: Decide if we want to be extra fast -- maybe create a static delay instead of using the server-sent time stamps.
	//if (!CastingState::instance().shouldPulse()) {
	//	return;
	//}
	
	// execute the front of the immediate queue
	if (!CastingState::instance().doImmediate()) {
		// if it's still executing, no need to worry about anything else
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Immediate Command Running.", MQGetTickCount64()); }
		return;
	}

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

		if (CastingState::instance().getCurrentResult() != CastResult::Casting) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Casting State Incorrect -> Setting Variables.", MQGetTickCount64()); }
			CastingState::instance().setCurrentResult(CastResult::Casting);
		}

		return; // we are casting, don't do anything else.
	} else if (CastingState::instance().getCmdType() != CastType::None && CastingState::instance().getCurrentResult() < CastResult::Success) {
		// we should be casting, let's see if we actually can and then do it
		if (DEBUGGING) { 
			WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Nothing Casting With Queue Size %d -> Executing Command Type %d.", MQGetTickCount64(), CastingState::instance().getCmdSize(), CastingState::instance().getCmdType());
		}

		// do target checks. If we didn't set the target ID from the command, assume we want to target our current target.
		if (CastingState::instance().getOnTargetID() == NOID) {
			CastingState::instance().setOnTargetID(pTarget ? ((PSPAWNINFO)pTarget)->SpawnID : NOID);
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Setting Target ID %d.", MQGetTickCount64(), CastingState::instance().getOnTargetID()); }
		} else if (pTarget) {
			auto targetID = ((PSPAWNINFO)pTarget)->SpawnID;
			if (targetID != CastingState::instance().getOnTargetID()) {
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Wrong Target ID %d -> Set Target To ID %d.", MQGetTickCount64(), targetID, CastingState::instance().getOnTargetID()); }
				// we need to target. Push a command and return.
				CastingState::instance().pushImmediate(new TargetCommand(CastingState::instance().getOnTargetID()));
				return;
			}
		}

		// these are "universal" checks, so don't include them in the individual command functions -- only check first time (assume Idle implies first time)
		auto currentID = CastingState::instance().getCurrentID();
		if (currentID != NOID && CastingState::instance().getCurrentResult() == CastResult::Idle) {
			auto pMyChar = GetCharInfo();
			if (pMyChar && pMyChar->Stunned) {
				// can't do things while stunned!
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Found Stunned.", MQGetTickCount64()); }
				CastingState::instance().setCurrentResult(CastResult::Stunned);
			} else if (CastingState::instance().getCmdType() != CastType::Memorize && !CastingState::instance().getCastWhileInvis() && pMyChar && pMyChar->pSpawn && pMyChar->pSpawn->HideMode) {
				// we specified not to cast while invis -- but we can mem all we want while invis
				if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Found Invis While CastWhileInvis=false.", MQGetTickCount64()); }
				CastingState::instance().setCurrentResult(CastResult::Invisible);
			} else if (PSPELL currentSpell = GetSpellByID(currentID)) {
				// maybe we have the nothing targeted?
				if (!IsValidTarget(currentSpell, reinterpret_cast<PSPAWNINFO>(pTarget))) {
					if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Found No Target When Spell Requires Target.", MQGetTickCount64()); }
					CastingState::instance().setCurrentResult(CastResult::NoTarget);
				}
			}
		}

		// if we aren't done with the command, then execute it. If we are, don't do anything.
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Check Status %s.", MQGetTickCount64(), GetReturnString(CastingState::instance().getCurrentResult()).c_str()); }
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
		}
	} else if (CastingState::instance().getCurrentID() != NOID) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: No Detected Spell or Command. Need To Clear Variables: %d.", MQGetTickCount64(), CastingState::instance().getCurrentID()); }
		CastingState::instance().popCmd();
	}

	// if we are finished, pop the command (will save off the ID and result and get ready for the next one)
	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Check Result %s.", MQGetTickCount64(), GetReturnString(CastingState::instance().getCurrentResult()).c_str()); }
	if (CastingState::instance().getCurrentResult() >= CastResult::Success) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Command Finished With Result %s.", MQGetTickCount64(), GetReturnString(CastingState::instance().getCurrentResult()).c_str()); }
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
	
	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Pop Command %s.", MQGetTickCount64(), GetReturnString(CastingState::instance().getCurrentResult()).c_str()); }
	if (CastingState::instance().getCurrentResult() >= CastResult::Success) {
		CastingState::instance().popCmd();
		CastingState::instance().pushImmediate(new RemobilizeCommand());

		if (DEBUGGING) {
			WriteChatf("[%I64u] MQ2Cast:[OnPulse]: Casting Complete ID[%d] Result=[%s]", MQGetTickCount64(), CastingState::instance().getLastID(), GetReturnString(CastingState::instance().getLastResult()).c_str());
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
	if (auto pMyChar = GetCharInfo()) {
		return pMyChar->pSpawn && pMyChar->pSpawn->mActorClient.Class == Bard;
	}

	return false;
}

bool CastingState::isTwisting() {
	auto dtTwist = FindMQ2DataType("twist");
	return dtTwist && GetDWordMember(dtTwist, "Twisting");
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

// CEverQuest::IsOkToTransact() ? -- this would be useful to test if any windows are open
//bool sub_5C1F90() {
//	if (((PSPAWNINFO)pLocalPlayer)->Vehicle) {
//		if (((PSPAWNINFO)pLocalPlayer)->Vehicle->pRaceGenderInfo) {
//			// return pRaceGenderInfo->0x2C
//		}
//	} else if (((PSPAWNINFO)pLocalPlayer)->RespawnTimer != 0 || dword_E68B00 != 0 || dword_F075D4 > 0 || dword_F075D0 > 0) {
//		return false;
//	} else if (pContainerMgr && pContainerMgr->0xAC != 0) {
//		return false;
//	} else if (pSpellBookWnd && pSpellBookWnd->bActive) {
//		return false;
//	} else if (pTrainWnd && pTrainWnd->bActive) {
//		return false;
//	} else if (pBazaarWnd && pBazaarWnd->bActive) {
//		return false;
//	} else if (pBarterWnd && pBarterWnd->bActive) {
//		return false;
//	} else if (pTradeWnd && pTradeWnd->bActive) {
//		return false;
//	} else if (pBankWnd && pBankWnd->bActive) {
//		return false;
//	} else if (pMerchantWnd && pMerchantWnd->bActive) {
//		return false;
//	} else if (pLootWnd && pLootWnd->bActive) {
//		return false;
//	} else if (pBarterMerchantWnd && pBarterMerchantWnd->bActive) {
//		return false;
//	} else if (dword_1019E80 && dword_1019E80->bActive) {
//		return false;
//	}
//	return true;
//}

bool CastingState::hasCastBar() {
	if (!isBard() && pCastingWnd && ((PCSIDLWND)pCastingWnd)->dShow) {
		// if (DEBUGGING) { WriteChatf("MQ2Cast: pCastingWnd=TRUE"); }
		return true;
	}

	return false;
}

DWORD CastingState::detectSpellCast() {
	if (auto pMyChar = GetCharInfo()) {
		if (auto pMySpawn = pMyChar->pSpawn) {
			return pMySpawn->CastingData.SpellID;
		}
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
			const PSPELL pFoundSpell = GetSpellByName(ID);
			if (pFoundSpell) {
				for (int caIdx = 0; caIdx < NUM_COMBAT_ABILITIES; ++caIdx) {
					if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(caIdx)) {
						if (auto pSpell = GetSpellByID(pPCData->GetCombatAbility(caIdx))) {
							if (pFoundSpell->SpellGroup == pSpell->SpellGroup && pFoundSpell->SpellSubGroup == pSpell->SpellSubGroup) {
								return CastType::Discipline;
							}
						}
					}
				}
			}

			for (DWORD abilityIdx = 0; szSkills[abilityIdx]; ++abilityIdx) {
				if (!_stricmp(ID, szSkills[abilityIdx])) {
					return CastType::Ability;
				}
			}

			if (pFoundSpell) {
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

const PSPELL CastingState::findMyRank(const PSPELL pSpell, const CastType cType) {
	if (cType == CastType::AltAbility || (cType == CastType::None && GetAAIndexByName(pSpell->Name) > 0)) {
		return pSpell;
	}

	std::vector<PSPELL> spellsInGroup;

	if (cType == CastType::None || cType == CastType::Discipline) {
		for (int caIdx = 0; caIdx < NUM_COMBAT_ABILITIES; ++caIdx) {
			if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(caIdx)) {
				PSPELL pCASpell = GetSpellByID(pPCData->GetCombatAbility(caIdx));
				if (pCASpell && pCASpell->SpellGroup == pSpell->SpellGroup && pCASpell->SpellSubGroup == pSpell->SpellSubGroup) {
					spellsInGroup.push_back(pCASpell);
				}
			}
		}
	}

	if (cType == CastType::None || cType == CastType::Spell) {
		if (auto pMyChar2 = GetCharInfo2()) {
			for (DWORD spellIdx = 0; spellIdx < NUM_BOOK_SLOTS; ++spellIdx) {
				PSPELL pBookSpell = GetSpellByID(pMyChar2->SpellBook[spellIdx]);
				if (pBookSpell && pBookSpell->SpellGroup == pSpell->SpellGroup && pBookSpell->SpellSubGroup == pSpell->SpellSubGroup) {
					spellsInGroup.push_back(pBookSpell);
				}
			}
		}
	}

	if (spellsInGroup.empty()) {
		return nullptr;
	}

	auto pMySpell = std::max_element(spellsInGroup.begin(), spellsInGroup.end(), [](const PSPELL a, const PSPELL b) -> bool {
		return a && b && a->SpellRank < b->SpellRank;
	});


	return *pMySpell;
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
	auto pMyChar = GetCharInfo();
	if (!pMyChar) {
		return Timeout;
	}

	if (auto pMySpawn = pMyChar->pSpawn) {
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
			// if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Timeout]: No Spell Casting TimeStamp %d.", MQGetTickCount64(), pMySpawn->TimeStamp); }
			// -100 to guarantee its in the past
			Timeout = pMySpawn->TimeStamp - 100;
		}
	}

	return Timeout;
}

bool CastingState::shouldPulse() {
	if (auto pMyChar = GetCharInfo()) {
		if (auto pMySpawn = pMyChar->pSpawn) {
			if (pMySpawn->TimeStamp > LastTimestamp) {
				LastTimestamp = pMySpawn->TimeStamp;
				return true;
			} else {
				return false;
			}
		}
	}

	return true;
}