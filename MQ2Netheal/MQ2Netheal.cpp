//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Projet: MQ2NetHeal.cpp   | Added NetCure TLO
// Author: s0rCieR          | Added NetCure Watch and NetWorst Watch
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#define        PLUGIN_DATE     20160217
#define        PLUGIN_VERS        1.5
#define        NETTICK              400
#define        REFRESH             1600
#define        TIMEOUT             2000
#define        MAXBUFF_WARDER        30
#define        MAXBUFF_MYSELF        25

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#ifndef PLUGIN_API
#include "../MQ2Plugin.h"
PreSetup("MQ2NetHeal");
PLUGIN_VERSION(PLUGIN_VERS);
#include <string>
#include <list>
#include <set>
#include <map>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
using namespace std;
#endif PLUGIN_API

enum {
	st_x = 0x0000, st_a = 0xFFFF, st_p = 0x0001, st_pc = 0x0011,
	st_pw = 0x0005, st_pwc = 0x0015, st_n = 0x0002, st_nc = 0x0022,
	st_nw = 0x000A, st_nwc = 0x002A, st_w = 0x000C, st_wn = 0x0008,
	st_wp = 0x0004, st_c = 0x0030, st_cn = 0x0020, st_cp = 0x0010
};

enum { UNK, WAR, CLR, PAL, RNG, SHD, DRU, MNK, BRD, ROG, SHM, NEC, WIZ, MAG, ENC, BST, BER, LST };

typedef BYTE ClassList[LST];

ClassList     feign = { 0,0,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0 };
ClassList     canni = { 0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0 };

class LifeRec {
public:
	DWORD        SpawnID;                    // Spawn ID?
	__int64         LifePct;                    // Life Pct?
	long         Spawned;                    // Spawn Type?
	float        RangeTo;                    // Distance?
	DWORD        ClassID;                    // Class?
	DWORD        DATimer;                    // DA Timer
	DWORD        HOTimer;                    // HO Timer
	DWORD        Updated;                    // Updated?
	CHAR         Name[0x40];                 // Name?
};

char           Sep[8] = { '"',' ','^','|',':','=','\n',0 };

typedef set<DWORD>         IDListe;
typedef map<DWORD, LifeRec> HPListe;
typedef map<string, DWORD>  NMListe;

HPListe        LifeMap;                    // Liste Life
NMListe        NameMap;                    // Liste Name
IDListe        GroupID;                    // Group ID's

KeyCombo       NewComboE;                  // Combo Key for E
KeyCombo       NewComboW;                  // Combo Key for W

long           IniLoad = 0;                  // Plugin IniLoad?
long           KeyBind = 0;                  // Plugin KeyBind?
long           NetStat = 0;                  // Plugin is On?
long           NetGrab = 0;                  // Grab Information?
long           NetSend = 0;                  // Send Information?
long           NetLast = 0;                  // Last Time Mark Pulse
long           NetTime = 0;                  // Last Time Mark Sent
char           NetPack[MAX_STRING] = { 0 };    // Last Pack Information

long           CureWatch = true;             // Cure Watch
long           CureAudio = true;             // Cure Audio Report?
long           CurePopup = true;             // Cure Popup Report?
long           CureQuiet = false;            // Cure Quiet Report?
char           CureSound[MAX_STRING];      // Cure Sound File?
char           CureSaved[MAX_STRING] = { 0 };  // Cure Saved List?

long           LifeWatch = true;             // Life Watch?
long           LifeAudio = true;             // Life Audio Report?
long           LifePopup = true;             // Life Popup Report?
long           LifeQuiet = false;            // Life Quiet Report?
char           LifeSound[MAX_STRING];      // Life Sound File?
char           LifeSaved[MAX_STRING] = { 0 };  // Life Saved Liste

char           WorstQuery[MAX_STRING];     // Worst Query?
long           WorstAEHit = 0;               // # of Player in Area
long           WorstCount = 0;               // Worst Count in List
char           WorstLists[MAX_STRING] = { 0 }; // Worst ID List
float          WorstTotal = 0.0f;            // Worst HPPCT Avarages


PCHAR s0 = "\aroff\ax";
PCHAR s1 = "\agon\ax";

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

bool EQBCConnected() {
	typedef WORD(__cdecl *fEqbcIsConnected)(VOID);
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(pLook->szFilename, "mq2eqbc", 8)) pLook = pLook->pNext;
	if (pLook)
		if (fEqbcIsConnected checkf = (fEqbcIsConnected)GetProcAddress(pLook->hModule, "isConnected"))
			if (checkf()) return true;
	return false;
}

void EQBCBroadCast(PCHAR Buffer) {
	typedef VOID(__cdecl *fEqbcNetBotSendMsg)(PCHAR);
	if (strlen(Buffer)>9) {
		PMQPLUGIN pLook = pPlugins;
		while (pLook && _strnicmp(pLook->szFilename, "mq2eqbc", 8)) pLook = pLook->pNext;
		if (pLook)
			if (fEqbcNetBotSendMsg requestf = (fEqbcNetBotSendMsg)GetProcAddress(pLook->hModule, "NetBotSendMsg")) {
#if    DEBUGGING>1
				DebugSpewAlways("%s->BroadCasting(%s)", "MQ2NetHeal", Buffer);
#endif DEBUGGING
				requestf(Buffer);
			}
	}
}

LifeRec* LifeFind(DWORD ID) {
	HPListe::iterator f = LifeMap.find(ID);
	return(LifeMap.end() == f) ? NULL : &(*f).second;
}

LifeRec *LifeName(PCHAR Name) {
	NMListe::iterator f = NameMap.find(Name);
	if (NameMap.end() == f) return NULL;
	return LifeFind((*f).second);
}

LifeRec* LifeLoad(DWORD ID) {
	HPListe::iterator f = LifeMap.find(ID);
	if (LifeMap.end() == f) {
		LifeRec RecInfo;
		ZeroMemory(&RecInfo.SpawnID, sizeof(LifeRec));
		RecInfo.SpawnID = ID;
		LifeMap.insert(map<DWORD, LifeRec>::value_type(ID, RecInfo));
		f = LifeMap.find(ID);
	}
	return &(*f).second;
}

bool ListFind(IDListe *MyList, DWORD ID) {
	IDListe::iterator f = MyList->find(ID);
	return (MyList->end() != f);
}

long SpawnMask(PSPAWNINFO x) {
	if (!x)                        return st_x;
	if (x->Mercenary) 							return st_p;
	if (x->Type == SPAWN_PLAYER)     return st_p;
	if (x->Type == SPAWN_CORPSE)     return x->Deity ? st_cp : st_cn;
	if (x->Type != SPAWN_NPC)        return st_x;
	if (strstr(x->Name, "s_Mount")) return st_x;
	if (!x->MasterID)              return st_n;
	PSPAWNINFO m = (PSPAWNINFO)GetSpawnByID(x->MasterID);
	return (!m || m->Type != SPAWN_PLAYER) ? st_wn : st_wp;
}

void UpdateIDLifePCT(DWORD ID, __int64 PCT) {
	PSPAWNINFO Member = (PSPAWNINFO)GetSpawnByID(ID);
	if (long SpType = SpawnMask(Member)&st_pw) {
		LifeRec *CurOne = LifeLoad(ID);
		CurOne->Updated = clock();
		if (!CurOne->Spawned) {
			CurOne->Spawned = SpawnMask(Member);
			CurOne->ClassID = Member->mActorClient.Class;
			strcpy_s(CurOne->Name, Member->DisplayedName);
			NameMap.insert(map<string, DWORD>::value_type(CurOne->Name, ID));
		}
		else {
			__int64 HPDif = PCT - CurOne->LifePct; if (HPDif>4 || HPDif<0) CurOne->DATimer = 0;
			if (CurOne->DATimer && CurOne->Updated>CurOne->DATimer) CurOne->DATimer = 0;
			if (CurOne->HOTimer && CurOne->Updated>CurOne->HOTimer) CurOne->HOTimer = 0;
		}
		CurOne->RangeTo = DistanceToSpawn3D(GetCharInfo()->pSpawn, Member);
		CurOne->LifePct = PCT;
		return;
	}
	HPListe::iterator i = LifeMap.find(ID);
	if (LifeMap.end() != i) {
		NMListe::iterator n = NameMap.find((*i).second.Name);
		if (NameMap.end() != n) NameMap.erase(n);
		LifeMap.erase(i);
	}
}

template <unsigned int _Size>PSTR MakeHeal(CHAR(&Buffer)[_Size], PSPAWNINFO x) {
	if ((SpawnMask(x)&st_pw) == 0)
	{
		Buffer[0] = '\0';
	}
	else {
		#if !defined(ROF2EMU) && !defined(UFEMU)
		sprintf_s(Buffer, "%d=%I64d:", x->SpawnID, x->HPCurrent * 100 / x->HPMax);
		#else
		sprintf_s(Buffer, "%d=%d:", x->SpawnID, x->HPCurrent * 100 / x->HPMax);
		#endif
	}
	return Buffer;
}

template <unsigned int _Size>PSTR GroupInsert(CHAR(&Output)[_Size]) {
	char Buffer[MAX_STRING]; PSPAWNINFO NPC, PET; GroupID.clear(); Output[0] = 0;
	if (NPC = (PSPAWNINFO)GetCharInfo()->pSpawn) {
		UpdateIDLifePCT(NPC->SpawnID, NPC->HPCurrent * 100 / NPC->HPMax);
		GroupID.insert(NPC->SpawnID);
		strcat_s(Output, MakeHeal(Buffer, NPC));
		if (PET = ((PSPAWNINFO)GetSpawnByID(NPC->PetID))) {
			UpdateIDLifePCT(PET->SpawnID, PET->HPCurrent * 100 / PET->HPMax);
			GroupID.insert(PET->SpawnID);
			strcat_s(Output, MakeHeal(Buffer, PET));
		}
	}
	if (GetCharInfo()->pGroupInfo) for (int i = 1; i<6; i++) {
		if (NPC = (PSPAWNINFO)GetGroupMember(i)) {
			UpdateIDLifePCT(NPC->SpawnID, NPC->HPCurrent * 100 / NPC->HPMax);
			GroupID.insert(NPC->SpawnID);
			strcat_s(Output, MakeHeal(Buffer, NPC));
			if (PET = ((PSPAWNINFO)GetSpawnByID(NPC->PetID))) {
				UpdateIDLifePCT(PET->SpawnID, PET->HPCurrent * 100 / PET->HPMax);
				GroupID.insert(PET->SpawnID);
				strcat_s(Output, MakeHeal(Buffer, PET));
			}
		}
	}
	if (pTarget) if (NPC = (PSPAWNINFO)pTarget) {
		UpdateIDLifePCT(NPC->SpawnID, NPC->HPCurrent * 100 / NPC->HPMax);
		strcat_s(Output, MakeHeal(Buffer, NPC));
		if (NPC->mPlayerPhysicsClient.pSpawn && (NPC = (PSPAWNINFO)GetSpawnByID(NPC->mPlayerPhysicsClient.pSpawn->TargetOfTarget))) {
			UpdateIDLifePCT(NPC->SpawnID, NPC->HPCurrent * 100 / NPC->HPMax);
			strcat_s(Output, MakeHeal(Buffer, NPC));
		}
	}
	return Output;
}

void BroadCast() {
	char Buffer[MAX_STRING];
	char Output[MAX_STRING];
	GroupInsert(Buffer);
	if (NetSend && EQBCConnected()) {
		sprintf_s(Output, "[NH]|%d^%d|%s", GetCharInfo()->zoneId, GetCharInfo()->instance, Buffer);
		if ((long)clock()>NetTime || strcmp(NetPack, Output) != 0) {
			NetTime = (long)clock() + REFRESH;
			strcpy_s(NetPack, Output);
			EQBCBroadCast(Output);
		}
	}
}

template <unsigned int _Size>void WorstFind(PCHAR Filter, CHAR(&Lists)[_Size], float *Total, long *Count, long *AEHit) {

	bool pet = 0; bool pc = 0; bool group = 0; bool self = 0; bool fd = 0; bool da = 0; bool hot = 0;
	long dis = 0; long pct = 0; ClassList set; ZeroMemory(set, sizeof(ClassList));
	char tmp[MAX_STRING];
	char *Next_Token = 0;
	strcpy_s(tmp, Filter);
	PCHAR Token = strtok_s(tmp, Sep, &Next_Token);
	IDListe WatchID;

	while (Token) {
		if (!_strnicmp(Token, "radius", 6))     dis = atoi(&Token[6]);
		else if (!_strnicmp(Token, "hp", 2))    pct = atoi(&Token[2]);
		else if (!_strnicmp(Token, "pet", 3))   pet = true;
		else if (!_strnicmp(Token, "pc", 2))    pc = true;
		else if (!_strnicmp(Token, "group", 5)) group = true;
		else if (!_strnicmp(Token, "self", 4))  self = true;
		else if (!_strnicmp(Token, "fd", 2))    fd = true;
		else if (!_strnicmp(Token, "da", 2))    da = true;
		else if (!_strnicmp(Token, "hot", 3))   hot = true;
		else if (!_strnicmp(Token, "war", 3))   set[WAR] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "clr", 3))   set[CLR] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "pal", 3))   set[PAL] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "rng", 3))   set[RNG] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "shd", 3))   set[SHD] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "dru", 3))   set[DRU] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "mnk", 3))   set[MNK] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "brd", 3))   set[BRD] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "rog", 3))   set[ROG] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "shm", 3))   set[SHM] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "nec", 3))   set[NEC] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "wiz", 3))   set[WIZ] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "mag", 3))   set[MAG] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "enc", 3))   set[ENC] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "bst", 3))   set[BST] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "ber", 3))   set[BER] = (IsNumber(&Token[3])) ? atoi(&Token[3]) : true;
		else if (!_strnicmp(Token, "all", 3))   memset(set, IsNumber(&Token[3]) ? atoi(&Token[3]) : true, sizeof(ClassList));
		else if (atoi(Token)>0)              WatchID.insert(atol(Token));
		Token = strtok_s(NULL, Sep, &Next_Token);
	}

	HPListe::iterator l; list<LifeRec> WorstHP; list<LifeRec>::iterator CurList; list<LifeRec>::iterator EndList;
	if (AEHit != NULL) *AEHit = 0;
	GroupInsert(tmp);

	for (l = LifeMap.begin(); l != LifeMap.end(); l++) {
		LifeRec *Rec = &(*l).second;
		if ((long)clock() - Rec->Updated>TIMEOUT) continue;
		PSPAWNINFO Check = (PSPAWNINFO)GetSpawnByID(Rec->SpawnID);
		if ((SpawnMask(Check)&st_pw) == 0) continue;
		if (strcmp(Check->DisplayedName, Rec->Name)) continue;
		if (Check->StandState == STANDSTATE_DEAD) continue;
		if (dis && Rec->RangeTo>dis) continue;
		if (self && Check->SpawnID != GetCharInfo()->pSpawn->SpawnID) continue;
		if (hot && Rec->HOTimer && (DWORD)clock()<Rec->HOTimer) continue;
		if (da && Rec->DATimer && (DWORD)clock()<Rec->DATimer) continue;
		if (da && Check->Linkdead) continue;
		if (AEHit != NULL && (Rec->Spawned&st_p) && (!group || ListFind(&GroupID, Rec->SpawnID))) *AEHit++;
		if (pct && Rec->LifePct >= pct) continue;
		if (fd && feign[Rec->ClassID] && Check->StandState != STANDSTATE_FEIGN) continue;
		if (!ListFind(&WatchID, Rec->SpawnID)) {
			if (group && !ListFind(&GroupID, Rec->SpawnID)) continue;
			if (pc && !pet && Check->MasterID) continue;
			if (pet && !pc && !Check->MasterID) continue;
			if (Check->MasterID && !pet) continue;
			if (!Check->MasterID && !pc) continue;
			if (Rec->Spawned&st_p) {
				if (!set[Rec->ClassID]) continue;
				if (set[Rec->ClassID]>1 && Rec->LifePct>set[Rec->ClassID]) continue;
			}
		}
		CurList = WorstHP.begin();
		EndList = WorstHP.end();
		while (CurList != EndList && CurList->LifePct<Rec->LifePct) CurList++;
		WorstHP.insert(CurList, *Rec);
	}

	long TokCnt = 0; Lists[0] = 0;
	if (Count != NULL) *Count = 0;
	if (Total != NULL) *Total = 0.0f;
	if (WorstHP.size()) {
		CurList = WorstHP.begin();
		EndList = WorstHP.end();
		while (CurList != EndList) {
			if (TokCnt++)
				strcat_s(Lists, " ");
			_itoa_s(CurList->SpawnID, tmp, 10);
			strcat_s(Lists, tmp);
			if (Total != NULL) *Total += CurList->LifePct;
			CurList++;
		}
	}
	if (Total != NULL && TokCnt>1) *Total /= TokCnt;
	if (Count != NULL) *Count = TokCnt;
}

void CheckEffect() {
	char Buffer[MAX_STRING] = "${NetCure[myself]}";
	ParseMacroData(Buffer, sizeof(Buffer));
	if (strcmp(Buffer, CureSaved)) {
		long Parm = 1; char Split[MAX_STRING]; char Liste[MAX_STRING] = { 0 };
		do {
			GetArg(Split, Buffer, Parm++); if (!Split[0]) break;
			if (!strstr(CureSaved, Split)) {
				if (Liste[0]) strcat_s(Liste, " ");
				strcat_s(Liste, Split);
			}
		} while (true);
		if (Liste[0]) {
			if (CurePopup) DisplayOverlayText(Split, CONCOLOR_RED, 100, 500, 500, 3000);
			if (CureAudio && CureSound[0]) PlaySound(CureSound, 0, SND_ASYNC);
			if (!CureQuiet) WriteChatf("[MQ2NetCure]::\ar%s\ax.", Liste);
		}
		strcpy_s(CureSaved, Buffer);
	}
}

void CheckHealth() {
	char Split[MAX_STRING]; long Parm = 1; bool Announce = true; long Count = 0;
	char Liste[MAX_STRING]; WorstFind(WorstQuery, Liste, NULL, &Count, NULL);
	if (Liste[0]) {
		if (!LifeSaved[0]) Announce = true;
		else if (!strcmp(LifeSaved, Liste)) Announce = false;
		else while (Announce) {
			GetArg(Split, LifeSaved, Parm++); if (!Split[0]) break;
			if (strstr(Liste, Split)) Announce = false;
		}
		if (Announce) {
			strcpy_s(LifeSaved, Liste); Liste[0] = 0; long Parm = 1;
			do {
				GetArg(Split, LifeSaved, Parm++); if (!Split[0]) break;
				if (LifeRec *Rec = LifeFind(atol(Split))) {
					if (Liste[0]) strcat_s(Liste, " ");
					strcat_s(Liste, Rec->Name);
				}
			} while (true);
			if (LifePopup) DisplayOverlayText(Liste, CONCOLOR_GREEN, 100, 500, 500, 3000);
			if (LifeAudio && LifeSound[0]) PlaySound(LifeSound, 0, SND_ASYNC);
			if (!LifeQuiet) WriteChatf("[MQ2NetWorst]::\ag%s\ax.", Liste);
		}
	}
	else strcpy_s(LifeSaved, Liste);
}

long SetBOOL(long Cur, PCHAR Val, PCHAR Sec, PCHAR Key) {
	char buffer[128]; long result = 0;
	if (!_strnicmp(Val, "false", 5) || !_strnicmp(Val, "off", 3) || !_strnicmp(Val, "0", 1))    result = 0;
	else if (!_strnicmp(Val, "true", 4) || !_strnicmp(Val, "on", 2) || !_strnicmp(Val, "1", 1)) result = 1;
	else result = (!Cur) & 1;
	if (Sec[0] && Key[0]) WritePrivateProfileString(Sec, Key, result ? "1" : "0", INIFileName);
	sprintf_s(buffer, "%s::%s (%s)", Sec, Key, result ? "\agon\ax" : "\agoff\ax");
	WriteChatColor(buffer);
	return result;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

class MQ2NetWorstType *pNetWorstType = 0;
class MQ2NetWorstType : public MQ2Type {

private:

	enum Info {
		Reserved, Affects, Average, Counter, Members, Request
	};

	char Temps[MAX_STRING];

public:

	void Store(PCHAR Index) {
		strcpy_s(Temps, Index);
	}

	MQ2NetWorstType() :MQ2Type("NetWorst") {
		TypeMember(Average);
		TypeMember(Counter);
		TypeMember(Members);
		TypeMember(Request);
	}

	bool GetMember(MQ2VARPTR VarPtr, PCHAR Member, PCHAR Index, MQ2TYPEVAR &Dest) {
		if (PMQ2TYPEMEMBER pMember = MQ2NetWorstType::FindMember(Member)) {
			switch ((Info)pMember->ID) {
			case Affects:
				Dest.Type = pIntType;
				Dest.Int = WorstAEHit;
				return true;
			case Average:
				Dest.Type = pFloatType;
				Dest.Float = WorstTotal;
				return true;
			case Counter:
				Dest.Type = pIntType;
				Dest.Int = WorstCount;
				return true;
			case Members:
				Dest.Type = pStringType;
				Dest.Ptr = WorstLists;
				return true;
			case Request:
				if (Index[0]) Store(Index);
				WorstFind(Temps, WorstLists, &WorstTotal, &WorstCount, &WorstAEHit);
				Dest.Type = pIntType;
				Dest.Int = WorstCount;
				return true;
			}
		}
		return false;
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

	~MQ2NetWorstType() { }
};

BOOL dataNetWorst(PCHAR Index, MQ2TYPEVAR &Dest) {
	pNetWorstType->Store(Index);
	Dest.Type = pNetWorstType;
	Dest.DWord = 1;
	return true;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

class MQ2NetHealType  *pNetHealType = 0;
class MQ2NetHealType : public MQ2Type {

private:

	enum Info {
		Reserved, Name, ID, Distance, PctHPs, Pet, Class, Feign, Canni, Spawn, da, hot, ttl, Updated,
	};

	LifeRec *CurOne;
	char     Temps[MAX_STRING];

	bool Fetch() {
		if (Temps[0]) {
			if (IsNumber(Temps)) {
				long ID = atol(Temps);
				if (!CurOne || CurOne->SpawnID != ID)
					CurOne = LifeFind(ID);
			}
			else {
				if (!CurOne || _stricmp(CurOne->Name, Temps))
					CurOne = LifeName(Temps);
			}
		}
		else CurOne = NULL;
		return (CurOne != NULL);
	}

	void Worst(PCHAR Index) {
		if (long Value = atoi(Index)) GetArg(Temps, WorstLists, Value);
	}

public:

	void Store(PCHAR Index) {
		strcpy_s(Temps, Index);
	}

	MQ2NetHealType() :MQ2Type("NetHeal") {
		TypeMember(Name);
		TypeMember(ID);
		TypeMember(Distance);
		TypeMember(PctHPs);
		TypeMember(Pet);
		TypeMember(Class);
		TypeMember(Feign);
		TypeMember(Canni);
		TypeMember(Spawn);
		TypeMember(da);
		TypeMember(hot);
		TypeMember(ttl);
		TypeMember(Updated);
	}

	bool GetMember(MQ2VARPTR VarPtr, PCHAR Member, PCHAR Index, MQ2TYPEVAR &Dest) {
		if (PMQ2TYPEMEMBER pMember = MQ2NetHealType::FindMember(Member)) {
			if (Index[0]) Worst(Index);
			if (Fetch()) switch ((Info)pMember->ID) {
			case Name:
				strcpy_s(Temps, CurOne->Name);
				Dest.Ptr = Temps;
				Dest.Type = pStringType;
				return true;
			case ID:
				Dest.Type = pIntType;
				Dest.DWord = CurOne->SpawnID;
				return true;
			case Distance:
				Dest.Type = pFloatType;
				Dest.Float = CurOne->RangeTo;
				return true;
			case PctHPs:
				Dest.Type = pInt64Type;
				Dest.Int64 = CurOne->LifePct;
				return true;
			case Pet:
				Dest.Type = pBoolType;
				Dest.DWord = (CurOne->Spawned&st_wp);
				return true;
			case Class:
				Dest.Type = pClassType;
				Dest.DWord = (CurOne->ClassID>LST) ? 0 : CurOne->ClassID;
				return true;
			case Feign:
				Dest.Type = pBoolType;
				Dest.DWord = ((CurOne->Spawned&st_p) && feign[CurOne->ClassID]);
				return true;
			case Canni:
				Dest.Type = pBoolType;
				Dest.DWord = ((CurOne->Spawned&st_p) && canni[CurOne->ClassID]);
				return true;
			case Spawn:
				Dest.Type = pSpawnType;
				Dest.Ptr = GetSpawnByID(CurOne->SpawnID);
				return true;
			case da:
				Dest.Type = pIntType;
				Dest.Int = (CurOne->DATimer>(DWORD)clock()) ? CurOne->DATimer - clock() : 0;
				return true;
			case hot:
				Dest.Type = pIntType;
				Dest.Int = (CurOne->HOTimer>(DWORD)clock()) ? CurOne->HOTimer - clock() : 0;
				return true;
			case ttl:
				Dest.Type = pIntType;
				Dest.Int = 60000;
				return true;
			case Updated:
				Dest.Type = pIntType;
				Dest.Int = clock() - CurOne->Updated;
				return true;
			}
		}
		return false;
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

	~MQ2NetHealType() { }
};

BOOL dataNetHeal(PCHAR Index, MQ2TYPEVAR &Dest) {
	pNetHealType->Store(Index);
	Dest.Type = pNetHealType;
	Dest.DWord = 1;
	return true;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

class MQ2NetCureType *pNetCureType = 0;
class MQ2NetCureType : public MQ2Type {

private:

	enum Info {
		Reserved, Detrimentals, Counters, Blinded, CastingLevel, Charmed, Cursed, Diseased, EnduDrain,
		Feared, Healing, Invulnerable, LifeDrain, ManaDrain, Mesmerized, Poisoned, Resistance, Rooted,
		Silenced, Slowed, Snared, SpellCost, SpellDamage, SpellSlowed, Trigger, Corruptions, Corrupted, LastOne,
	};

	PSPELL BSP[MAXBUFF_WARDER];
	char   Listed[MAX_STRING];
	int    Values[LastOne];
	int    Totale;

	int SlotCalculate(PSPELL spell, int slot) {
		char Buffer[MAX_STRING] = { 0 };
		SlotValueCalculate(Buffer, spell, slot, 1);
		return atoi(Buffer);
	}

public:

	void CheckBuff(PCHAR Input) {
		char Current[MAX_STRING] = { 0 };
		char Convert[MAX_STRING];
		if (!Input[0] || !_stricmp(Input, "self") || !_stricmp(Input, "myself")) {
			ZeroMemory(&BSP, sizeof(BSP)); Totale = 0;
			for (int b = 0; b<MAXBUFF_MYSELF; b++) {
				if (PSPELL spell = GetSpellByID(GetCharInfo2()->Buff[b].SpellID)) {
					if (Current[0]) strcat_s(Current, " ");
					_itoa_s(spell->ID, Convert, 10);
					strcat_s(Current, Convert);
					if (spell->DurationCap>0) BSP[Totale++] = spell;
				}
			}
		}
		else if (!_stricmp(Input, "pet") || !_stricmp(Input, "warder")) {
			ZeroMemory(&BSP, sizeof(BSP)); Totale = 0;
			if (pPetInfoWnd && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo()->pSpawn->PetID>0) {
				for (int b = 0; b<MAXBUFF_WARDER; b++) {
					if (PSPELL spell = GetSpellByID(((PEQPETINFOWINDOW)pPetInfoWnd)->Buff[b])) {
						if (Current[0]) strcat_s(Current, " ");
						_itoa_s(spell->ID, Convert, 10);
						strcat_s(Current, Convert);
						if (spell->DurationCap>0) BSP[Totale++] = spell;
					}
				}
			}
		}
		else if (strcmp(Input, Listed)) {
			ZeroMemory(&BSP, sizeof(BSP)); Totale = 0;
			for (int b = 0; b<MAXBUFF_WARDER; b++) {
				if (!strlen(GetArg(Convert, Input, b + 1))) break;
				if (PSPELL spell = GetSpellByID(atol(Convert))) {
					if (Current[0]) strcat_s(Current, " ");
					strcat_s(Current, Convert);
					if (spell->DurationCap>0) BSP[Totale++] = spell;
				}
			}
		}
		if (_stricmp(Current, Listed)) {
			strcpy_s(Listed, Current);
			ZeroMemory(&Values, sizeof(Values));
			for (long b = 0; b<Totale; b++) {
				if (BSP[b]->SpellType) {
					for (int s = 0; s<GetSpellNumEffects(BSP[b]); s++) if (GetSpellAttrib(BSP[b],s) == 40) Values[Invulnerable]++;
				}
				else {
					bool d = false;
					bool r = false;
					for (int s = 0; s<GetSpellNumEffects(BSP[b]); s++) switch (GetSpellAttrib(BSP[b],s)) {
					case   0: if (GetSpellBase(BSP[b],s)<0) { Values[LifeDrain] += SlotCalculate(BSP[b], s); d = true; } break;
					case   3: if (GetSpellBase(BSP[b],s)<0) { Values[Snared]++; d = true; } break;
					case  11: if (((GetSpellMax(BSP[b],s)) ? GetSpellMax(BSP[b],s) : GetSpellBase(BSP[b],s)) - 100<0) { Values[Slowed]++; d = true; } break;
					case  15: if (GetSpellBase(BSP[b],s)<0) { Values[ManaDrain] += SlotCalculate(BSP[b], s); d = true; } break;
					case  20: Values[Blinded]++; d = true; break;
					case  22: Values[Charmed]++;  d = true; break;
					case  23: Values[Feared]++; d = true; break;
					case  31: Values[Mesmerized]++; d = true; break;
					case  35: Values[Diseased] += (int)GetSpellBase(BSP[b],s); d = true; break;
					case  36: Values[Poisoned] += (int)GetSpellBase(BSP[b],s); d = true; break;
					case  40: Values[Invulnerable]++; d = true; break;
					case  46: if (GetSpellBase(BSP[b], s)<0) { r = true; d = true; } break;
					case  47: if (GetSpellBase(BSP[b], s)<0) { r = true; d = true; } break;
					case  48: if (GetSpellBase(BSP[b], s)<0) { r = true; d = true; } break;
					case  49: if (GetSpellBase(BSP[b], s)<0) { r = true; d = true; } break;
					case  50: if (GetSpellBase(BSP[b], s)<0) { r = true; d = true; } break;
					case  96: Values[Silenced]++; d = true; break;
					case  99: Values[Rooted]++; d = true; break;
					case 112: if (GetSpellBase(BSP[b], s)<0) { Values[CastingLevel]++; d = true; } break;
					case 116: Values[Cursed] += (int)GetSpellBase(BSP[b], s); d = true; break;
					case 120: if (GetSpellBase(BSP[b], s)<0) { Values[Healing]++; d = true; } break;
					case 124: if (GetSpellBase(BSP[b], s)<0) { Values[SpellDamage]++; d = true; } break;
					case 127: if (GetSpellBase(BSP[b], s)<0) { Values[SpellSlowed]++; d = true; } break;
					case 132: if (GetSpellBase(BSP[b], s)<0) { Values[SpellCost]++; d = true; } break;
					case 189: if (GetSpellBase(BSP[b], s)<0) { Values[EnduDrain] += SlotCalculate(BSP[b], s); d = true; } break;
					case 289: Values[Trigger]++; d = true; break;
					case 369: Values[Corrupted]++; Values[Corruptions]++; d = true; break;
					}
					if (d) Values[Detrimentals]++;
					if (r) Values[Resistance]++;
				}
			}
			Values[Counters] = Values[Cursed] + Values[Diseased] + Values[Poisoned];
		}
	}

	MQ2NetCureType() :MQ2Type("NetCure") {
		TypeMember(Detrimentals);
		TypeMember(Counters);
		TypeMember(Cursed);
		TypeMember(Diseased);
		TypeMember(Poisoned);
		TypeMember(EnduDrain);
		TypeMember(LifeDrain);
		TypeMember(ManaDrain);
		TypeMember(Blinded);
		TypeMember(CastingLevel);
		TypeMember(Charmed);
		TypeMember(Feared);
		TypeMember(Healing);
		TypeMember(Invulnerable);
		TypeMember(Mesmerized);
		TypeMember(Rooted);
		TypeMember(Silenced);
		TypeMember(Slowed);
		TypeMember(Snared);
		TypeMember(SpellCost);
		TypeMember(SpellSlowed);
		TypeMember(SpellDamage);
		TypeMember(Trigger);
		TypeMember(Corruptions);
		TypeMember(Corrupted);
	}

	bool GetMember(MQ2VARPTR VarPtr, PCHAR Member, PCHAR Index, MQ2TYPEVAR &Dest) {
		if (PMQ2TYPEMEMBER pMember = MQ2NetCureType::FindMember(Member)) {
			Dest.Type = pIntType;
			Dest.Int = Values[pMember->ID];
			return true;
		}
		return false;
	}

	bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source) {
		return false;
	}

	bool FromString(MQ2VARPTR &VarPtr, PCHAR Source) {
		return false;
	}

	bool ToString(MQ2VARPTR VarPtr, PCHAR Destination) {
		Destination[0] = 0;
		if (Values[Cursed])       strcat_s(Destination, MAX_STRING, "Cursed ");
		if (Values[Diseased])     strcat_s(Destination, MAX_STRING, "Diseased ");
		if (Values[Poisoned])     strcat_s(Destination, MAX_STRING, "Poisoned ");
		if (Values[EnduDrain])    strcat_s(Destination, MAX_STRING, "EnduDrain ");
		if (Values[LifeDrain])    strcat_s(Destination, MAX_STRING, "LifeDrain ");
		if (Values[ManaDrain])    strcat_s(Destination, MAX_STRING, "ManaDrain ");
		if (Values[Blinded])      strcat_s(Destination, MAX_STRING, "Blinded ");
		if (Values[CastingLevel]) strcat_s(Destination, MAX_STRING, "CastingLevel ");
		if (Values[Charmed])      strcat_s(Destination, MAX_STRING, "Charmed ");
		if (Values[Feared])       strcat_s(Destination, MAX_STRING, "Feared ");
		if (Values[Healing])      strcat_s(Destination, MAX_STRING, "Healing ");
		if (Values[Invulnerable]) strcat_s(Destination, MAX_STRING, "Invulnerable ");
		if (Values[Mesmerized])   strcat_s(Destination, MAX_STRING, "Mesmerized ");
		if (Values[Rooted])       strcat_s(Destination, MAX_STRING, "Rooted ");
		if (Values[Silenced])     strcat_s(Destination, MAX_STRING, "Silenced ");
		if (Values[Slowed])       strcat_s(Destination, MAX_STRING, "Slowed ");
		if (Values[Snared])       strcat_s(Destination, MAX_STRING, "Snared ");
		if (Values[SpellCost])    strcat_s(Destination, MAX_STRING, "SpellCost ");
		if (Values[SpellDamage])  strcat_s(Destination, MAX_STRING, "SpellDamage ");
		if (Values[SpellSlowed])  strcat_s(Destination, MAX_STRING, "SpellSlowed ");
		if (Values[Trigger])      strcat_s(Destination, MAX_STRING, "Trigger ");
		if (Values[Corruptions])  strcat_s(Destination, MAX_STRING, "Corruptions ");
		if (Values[Corrupted])    strcat_s(Destination, MAX_STRING, "Corrupted ");
		if (long len = strlen(Destination)) Destination[--len] = 0;
		return true;
	}

	~MQ2NetCureType() { }
};

BOOL dataNetCure(PCHAR Index, MQ2TYPEVAR &Dest) {
	pNetCureType->CheckBuff(Index);
	Dest.Type = pNetCureType;
	Dest.DWord = 1;
	return true;
}

BOOL dataisNPC(PCHAR Index, MQ2TYPEVAR &Dest) {
	Dest.Type = pBoolType;
	Dest.DWord = (st_n&SpawnMask((PSPAWNINFO)GetSpawnByID(atol(Index))));
	return true;
}

BOOL dataisNPCPET(PCHAR Index, MQ2TYPEVAR &Dest) {
	Dest.Type = pBoolType;
	Dest.DWord = (st_wn&SpawnMask((PSPAWNINFO)GetSpawnByID(atol(Index))));
	return true;
}

BOOL dataisNPCCORPSE(PCHAR Index, MQ2TYPEVAR &Dest) {
	Dest.Type = pBoolType;
	Dest.DWord = (st_cn&SpawnMask((PSPAWNINFO)GetSpawnByID(atol(Index))));
	return true;
}

BOOL dataisPC(PCHAR Index, MQ2TYPEVAR &Dest) {
	Dest.Type = pBoolType;
	Dest.DWord = (st_p&SpawnMask((PSPAWNINFO)GetSpawnByID(atol(Index))));
	return true;
}

BOOL dataisPCPET(PCHAR Index, MQ2TYPEVAR &Dest) {
	Dest.Type = pBoolType;
	Dest.DWord = (st_wp&SpawnMask((PSPAWNINFO)GetSpawnByID(atol(Index))));
	return true;
}

BOOL dataisPCCORPSE(PCHAR Index, MQ2TYPEVAR &Dest) {
	Dest.Type = pBoolType;
	Dest.DWord = (st_cp&SpawnMask((PSPAWNINFO)GetSpawnByID(atol(Index))));
	return true;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

void CureCommand(PSPAWNINFO pChar, PCHAR Cmd) {
	char Tmp[MAX_STRING]; char Var[MAX_STRING]; char Set[MAX_STRING]; int Parm = 1;
	do {
		GetArg(Tmp, Cmd, Parm++);
		GetArg(Var, Tmp, 1, FALSE, FALSE, FALSE, '=');
		GetArg(Set, Tmp, 2, FALSE, FALSE, FALSE, '=');
		if (!_stricmp("auto", Var) || !_stricmp("plugin", Var))       CureWatch = SetBOOL(CureWatch, Set, "MQ2NetCure", "Watch");
		else if (!_stricmp("on", Var) || !_stricmp("true", Var))      CureWatch = SetBOOL(CureWatch, "1", "MQ2NetCure", "Watch");
		else if (!_stricmp("off", Var) || !_stricmp("false", Var))    CureWatch = SetBOOL(CureWatch, "0", "MQ2NetCure", "Watch");
		else if (!_stricmp("silent", Var) || !_stricmp("quiet", Var)) CureQuiet = SetBOOL(CureQuiet, Set, "MQ2NetCure", "Quiet");
		else if (!_stricmp("audio", Var))                           CureAudio = SetBOOL(CureAudio, Set, "MQ2NetCure", "Audio");
		else if (!_stricmp("popup", Var))                           CurePopup = SetBOOL(CurePopup, Set, "MQ2NetCure", "Popup");
	} while (Tmp[0]);
	WriteChatf("%s::(%s) Audio (%s) Popup (%s) Quiet (%s)", "MQ2NetCure", CureWatch ? s1 : s0, CureAudio ? s1 : s0, CurePopup ? s1 : s0, CureQuiet ? s1 : s0);
}

void HealCommand(PSPAWNINFO pChar, PCHAR Cmd) {
	char Tmp[MAX_STRING]; char Var[MAX_STRING]; char Set[MAX_STRING]; int Parm = 1;
	char Name[MAX_STRING] = { 0 }; long Com = 0; long Val = -1; long Stat = 1; long Count = 0;
	do {
		GetArg(Tmp, Cmd, Parm++);
		GetArg(Var, Tmp, 1, FALSE, FALSE, FALSE, '=');
		GetArg(Set, Tmp, 2, FALSE, FALSE, FALSE, '=');
		if (!_stricmp("auto", Var) || !_stricmp("plugin", Var))    NetStat = SetBOOL(NetStat, Set, "MQ2NetHeal", "Stat");
		else if (!_stricmp("on", Var) || !_stricmp("true", Var))   NetStat = SetBOOL(NetStat, "1", "MQ2NetHeal", "Stat");
		else if (!_stricmp("off", Var) || !_stricmp("false", Var)) NetStat = SetBOOL(NetStat, "0", "MQ2NetHeal", "Stat");
		else if (!_stricmp("grab", Var))                         NetGrab = SetBOOL(NetGrab, Set, "MQ2NetHeal", "Grab");
		else if (!_stricmp("send", Var))                         NetSend = SetBOOL(NetSend, Set, "MQ2NetHeal", "Send");
		else if (Var[0]) {
			Stat = 0;
			if (!_stricmp("hot", Var))     Com = 1;
			else if (!_stricmp("da", Var)) Com = 2;
			else if (Com && Val == -1 && IsNumber(Var)) Val = atol(Var);
			else if (Com && Val != -1) {
				if (Count++) strcat_s(Name, " ");
				strcat_s(Name, Var);

			}
		}
	} while (strlen(Tmp));
	if (Stat) WriteChatf("%s::(%s) Grab (%s) Send (%s).", "MQ2NetHeal", NetStat ? s1 : s0, NetGrab ? s1 : s0, NetSend ? s1 : s0);
	else if (Count && Com) {
		for (int i = 0; i<Count; i++) {
			GetArg(Tmp, Name, i + 1);
			LifeRec *CurOne = IsNumber(Tmp) ? LifeFind(atol(Tmp)) : LifeName(Tmp);
			if (CurOne) {
				Parm = (DWORD)clock() + Val;
				if (Com == 1) CurOne->HOTimer = Parm;
				if (Com == 2) CurOne->DATimer = Parm;
			}
		}
	}
}

void WorstCommand(PSPAWNINFO pChar, PCHAR Cmd) {
	char Tmp[MAX_STRING]; char Var[MAX_STRING]; char Set[MAX_STRING]; int Parm = 1;
	do {
		GetArg(Tmp, Cmd, Parm++);
		GetArg(Var, Tmp, 1, FALSE, FALSE, FALSE, '=');
		GetArg(Set, Tmp, 2, FALSE, FALSE, FALSE, '=');
		if (!_stricmp("auto", Var) || !_stricmp("plugin", Var))       LifeWatch = SetBOOL(LifeWatch, Set, "MQ2NetWorst", "Watch");
		else if (!_stricmp("on", Var) || !_stricmp("true", Var))      LifeWatch = SetBOOL(LifeWatch, "1", "MQ2NetWorst", "Watch");
		else if (!_stricmp("off", Var) || !_stricmp("false", Var))    LifeWatch = SetBOOL(LifeWatch, "0", "MQ2NetWorst", "Watch");
		else if (!_stricmp("silent", Var) || !_stricmp("quiet", Var)) LifeQuiet = SetBOOL(LifeQuiet, Set, "MQ2NetWorst", "Quiet");
		else if (!_stricmp("audio", Var))                           LifeAudio = SetBOOL(LifeAudio, Set, "MQ2NetWorst", "Audio");
		else if (!_stricmp("popup", Var))                           LifePopup = SetBOOL(LifePopup, Set, "MQ2NetWorst", "Popup");
	} while (Tmp[0]);
	WriteChatf("%s::(%s) Audio (%s) Popup (%s) Quiet (%s)", "MQ2NetWorst", LifeWatch ? s1 : s0, LifeAudio ? s1 : s0, LifePopup ? s1 : s0, LifeQuiet ? s1 : s0);
}

void WorstCycle(PSPAWNINFO pCHAR, PCHAR Cmd) {
	char Liste[MAX_STRING] = { 0 }; long Count = 0;
	WorstFind(Cmd[0] ? Cmd : WorstQuery, Liste, NULL, &Count, NULL);
	if (Count) {
		char OneID[MAX_STRING]; long Find = 0; PSPAWNINFO x;
		DWORD CurID = (pTarget) ? ((PSPAWNINFO)pTarget)->SpawnID : 0;
		for (int i = 1; i<Count && !Find; i++)
			if (atoi(GetArg(OneID, Liste, i)) == CurID) Find = i;
		do {
			Find++;
			x = (PSPAWNINFO)GetSpawnByID(atol(GetArg(OneID, Liste, (Find>Count) ? 1 : Find)));
		} while (x == NULL && Find <= Count);
		if (x) *(PSPAWNINFO*)ppTarget = x;
	}
}

void WorstTarget(PSPAWNINFO pCHAR, PCHAR Cmd) {
	char Liste[MAX_STRING] = { 0 }; long Count = 0;
	WorstFind(Cmd[0] ? Cmd : WorstQuery, Liste, NULL, &Count, NULL);
	if (Count) {
		char OneID[MAX_STRING]; GetArg(OneID, Liste, 1);
		if (PSPAWNINFO x = (PSPAWNINFO)GetSpawnByID(atol(OneID)))
			*(PSPAWNINFO*)ppTarget = x;
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

void doCycleBind(PCHAR Name, BOOL Down) {
	if (Down && gbInZone)	WorstCycle(NULL, "");
}

void doWorstBind(PCHAR Name, BOOL Down) {
	if (Down && gbInZone) WorstTarget(NULL, "");
}

void Binding(BOOL Bind) {
	if (Bind) {
		if (IniLoad && !KeyBind) {
			AddMQ2KeyBind("WORST_TARGET", doWorstBind);
			AddMQ2KeyBind("WORST_CYCLE", doCycleBind);
			ParseKeyCombo("W", NewComboW);
			ParseKeyCombo("E", NewComboE);
			SetMQ2KeyBind("WORST_TARGET", false, NewComboW);
			SetMQ2KeyBind("WORST_CYCLE", false, NewComboE);
			KeyBind = true;
		}
	}
	else if (KeyBind) {
		RemoveMQ2KeyBind("WORST_TARGET");
		RemoveMQ2KeyBind("WORST_CYCLE");
		KeyBind = false;
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

PLUGIN_API VOID OnEndZone(VOID) {
	LifeMap.clear();
	NameMap.clear();
}

PLUGIN_API VOID OnNetBotMSG(PCHAR Name, PCHAR Msg) {
	if (NetStat && NetGrab && gbInZone && !strncmp(Msg, "[NH]|", 5) &&
		GetCharInfo() && strcmp(GetCharInfo()->Name, Name)) {
		char TK[MAX_STRING];
		strcpy_s(TK, &Msg[5]);
		char *Next_Token = 0;
		if (PCHAR Token = strtok_s(TK, Sep, &Next_Token)) {
			long ZoneID = atol(Token);
			Token = strtok_s(NULL, Sep, &Next_Token);
			if (ZoneID != GetCharInfo()->zoneId || !Token)
				return;
			long InstID = atol(Token);
			Token = strtok_s(NULL, Sep, &Next_Token);
			if (InstID != GetCharInfo()->instance || !Token)
				return;
			while (Token) {
				long SpawnID = atol(Token);
				Token = strtok_s(NULL, Sep, &Next_Token);
				if (!SpawnID || !Token)
					return;
				long SpawnHP = atol(Token);
				Token = strtok_s(NULL, Sep, &Next_Token);
				UpdateIDLifePCT(SpawnID, SpawnHP);
			}
		}
	}
}

PLUGIN_API VOID OnPulse(VOID) {
	if (gbInZone && (long)clock()>NetLast) {
		NetLast = (long)clock() + NETTICK;
		if (GetCharInfo() && GetCharInfo()->pSpawn) {
			if (NetStat) BroadCast();
			if (CureWatch) CheckEffect();
			if (LifeWatch && WorstQuery[0]) CheckHealth();
		}
	}
}

PLUGIN_API VOID SetGameState(DWORD GameState) {
	if (GameState == GAMESTATE_INGAME) {
		if (!IniLoad) {
			sprintf_s(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, GetCharInfo()->Name);
			NetStat = GetPrivateProfileInt("MQ2NetHeal", "Stat", 0, INIFileName);
			NetGrab = GetPrivateProfileInt("MQ2NetHeal", "Grab", 0, INIFileName);
			NetSend = GetPrivateProfileInt("MQ2NetHeal", "Send", 0, INIFileName);
			CureWatch = GetPrivateProfileInt("MQ2NetCure", "Watch", 0, INIFileName);
			CureAudio = GetPrivateProfileInt("MQ2NetCure", "Audio", 0, INIFileName);
			CurePopup = GetPrivateProfileInt("MQ2NetCure", "Popup", 0, INIFileName);
			CureQuiet = GetPrivateProfileInt("MQ2NetCure", "Quiet", 0, INIFileName);
			GetPrivateProfileString("MQ2NetCure", "Sound", "", CureSound, 2047, INIFileName);
			LifeWatch = GetPrivateProfileInt("MQ2NetWorst", "Watch", 0, INIFileName);
			LifeAudio = GetPrivateProfileInt("MQ2NetWorst", "Audio", 0, INIFileName);
			LifePopup = GetPrivateProfileInt("MQ2NetWorst", "Popup", 0, INIFileName);
			LifeQuiet = GetPrivateProfileInt("MQ2NetWorst", "Quiet", 0, INIFileName);
			GetPrivateProfileString("MQ2NetWorst", "Sound", "", LifeSound, 2047, INIFileName);
			GetPrivateProfileString("MQ2NetWorst", "Query", "", WorstQuery, 2047, INIFileName);
			IniLoad = true;
		}
	}
	else if (GameState != GAMESTATE_LOGGINGIN) {
		if (IniLoad) {
			NetStat = false;
			CureWatch = false;
			LifeWatch = false;
			IniLoad = false;
		}
	}
	Binding(GameState == GAMESTATE_INGAME);
}

PLUGIN_API VOID InitializePlugin(VOID) {
	LifeMap.clear();
	NameMap.clear();
	pNetCureType = new MQ2NetCureType;
	pNetHealType = new MQ2NetHealType;
	pNetWorstType = new MQ2NetWorstType;
	AddMQ2Data("NetCure", dataNetCure);
	AddMQ2Data("NetHeal", dataNetHeal);
	AddMQ2Data("NetWorst", dataNetWorst);
	AddMQ2Data("isNPC", dataisNPC);
	AddMQ2Data("isNPCPET", dataisNPCPET);
	AddMQ2Data("isNPCCORPSE", dataisNPCCORPSE);
	AddMQ2Data("isPC", dataisPC);
	AddMQ2Data("isPCPET", dataisPCPET);
	AddMQ2Data("isPCCORPSE", dataisPCCORPSE);
	AddCommand("/netcure", CureCommand);
	AddCommand("/netheal", HealCommand);
	AddCommand("/networst", WorstCommand);
	AddCommand("/worsttarget", WorstTarget);
	AddCommand("/worstcycle", WorstCycle);
}

PLUGIN_API VOID ShutdownPlugin(VOID) {
	SetGameState(GAMESTATE_UNLOADING);
	RemoveCommand("/netcure");
	RemoveCommand("/netheal");
	RemoveCommand("/networst");
	RemoveCommand("/worstcycle");
	RemoveCommand("/worsttarget");
	RemoveMQ2Data("NetCure");
	RemoveMQ2Data("NetHeal");
	RemoveMQ2Data("NetWorst");
	RemoveMQ2Data("isNPC");
	RemoveMQ2Data("isNPCPET");
	RemoveMQ2Data("isNPCCORPSE");
	RemoveMQ2Data("isPC");
	RemoveMQ2Data("isPCPET");
	RemoveMQ2Data("isPCCORPSE");
	delete pNetCureType;
	delete pNetHealType;
	delete pNetWorstType;
}
