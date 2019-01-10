// MQ2DamageParser by EqMule
//
// This plugin parse DPS directly from the hit function.
// No log file reading etc.
//
// 1.0 2017-01-01 Initial Release.
// 1.1 2018-12-12 Fixed a parsing bug due to a change in the client.
// 1.2 2019-01-01 Added TypeMember(Average) and TypeMember(TotalFightingTime)
// 1.3 2019-01-03 Added /dpsreset and /dpsaverage
// 1.4
/////////////////////////////////////////////////////////////////



#include "../MQ2Plugin.h"
#include <memory.h>
#include "resource.h"

using namespace std;
PreSetup("MQ2DamageParser");
PLUGIN_VERSION(1.3);
bool gBUsePerCharSettings = false;
bool gbShowLogWindow = false;
class CDamageParserLogWnd;
CDamageParserLogWnd *pDamageParserLogWnd=0;
RECT LogWindowLoc = { 100,330,710,550 };
void DPSAverage(PSPAWNINFO pLPlayer, char* szLine);
void DPSReset(PSPAWNINFO pLPlayer, char* szLine);
#define WSF_AUTOSTRETCHH	0x00400000
#define WSF_CLIENTMOVABLE	0x00200000
#define WSF_NOHITTEST		0x00008000
#define WSF_USEMYALPHA		0x00000800
#define WSF_TRANSPARENT		0x00000400
#define WSF_SIZABLE			0x00000200
#define WSF_AUTOSTRETCHV	0x00000100
#define WSF_RELATIVERECT	0x00000080
#define WSF_BORDER			0x00000040
#define WSF_MINIMIZEBOX     0x00000020
#define WSF_CLOSEBOX        0x00000008
#define WSF_TITLEBAR		0x00000004

class CDamageParserLogWnd : public CSidlScreenWnd
{
public:
    CDamageParserLogWnd(CXStr *screenpiece):CSidlScreenWnd(0,screenpiece,-1,1,0)
    {
        CreateChildrenFromSidl();
        pXWnd()->Show(gbShowLogWindow,0);
        ReplacevfTable();
        CloseOnESC=1;
    }
    CDamageParserLogWnd(char *screenpiece):CSidlScreenWnd(0,&CXStr(screenpiece),-1,1,0)
    {
        CreateChildrenFromSidl();
		pXWnd()->Show(gbShowLogWindow,0);
        ReplacevfTable();
        CloseOnESC=1;
		SetWndNotification(CDamageParserLogWnd);
		ResetButton = (CButtonWnd*)GetChildItem("DPLW_DPSResetButton");
		UpdateButton = (CButtonWnd*)GetChildItem("DPLW_DPSAvgButton");
		if (LogList = (CListWnd*)GetChildItem("DPLW_LogList"))
		{
			ARGBCOLOR col = { 0 };
			col.A = 255;
			col.R = 138;
			col.G = 163;
			col.B = 255;
			LogList->HeaderText = col.ARGB;
			//CRNormal
			col.R = 255;
			col.G = 255;
			col.B = 255;
			LogList->CRNormal = col.ARGB;
			//Highlight
			col.R = 255;
			col.G = 255;
			col.B = 0;
			LogList->Highlight = col.ARGB;
			//Selected
			col.R = 128;
			col.G = 128;
			col.B = 128;
			LogList->Selected = col.ARGB;;
			LogList->SetColumnsSizable(true);
			LogList->ListWndStyle |= 0x00040000;//SORT is ok...
			LogList->SortCol = 0;
			LogList->bSortAsc = false;
			LogList->SetColumnJustification(1,1);//center column 1
			LogList->SetColumnJustification(2,1);//center column 2
		}
		this->Faded = false;
		//this->ZLayer = 1;
		this->CloseOnESC = false;
		this->Clickable = true;
		this->Alpha = 0xfF;
		this->BGColor = 0xFF000000;
		this->BGType = 1;
		this->SetStyle(WSF_USEMYALPHA | WSF_SIZABLE | WSF_BORDER | WSF_MINIMIZEBOX | WSF_CLOSEBOX | WSF_TITLEBAR); 
    }

    ~CDamageParserLogWnd()
    {
        RemovevfTable();
    }
    int WndNotification(CXWnd *pWnd, unsigned int Message, void *pData)
    {    
		//UpdateButton
		if (Message == XWM_LMOUSEUP)
		{
			if (pWnd == (CXWnd*)UpdateButton)
			{
				DPSAverage((PSPAWNINFO)pLocalPlayer, "");
				return 1;
			}
			else if (pWnd == (CXWnd*)ResetButton)
			{
				DPSReset((PSPAWNINFO)pLocalPlayer, "");
				return 1;
			}
		}
		//check for sortrequest...
		else if (Message == XWM_COLUMNCLICK)
		{
			if (pWnd == (CXWnd*)LogList)
			{
				int colindex = (int)pData;
				if (colindex > 3)//we only have 3 columns...
					colindex = 3;
				int sortcol = LogList->SortCol;
				if (sortcol == colindex)
				{
					LogList->bSortAsc = !LogList->bSortAsc;
				}
				else
				{
					LogList->SortCol = colindex;
					LogList->bSortAsc = true;
				}
				LogList->Sort();
			}
		}
		else if (Message == XWM_SORTREQUEST)
		{
			if (pWnd == (CXWnd*)LogList)
			{
				SListWndSortInfo *SortInf = (SListWndSortInfo *)pData;
				switch(SortInf->SortCol)
				{
				case 0://time
					{
						//add sort code...
						/*SortInf->SortResult = strcmp( SortInf->StrLabel1, SortInf->StrLabel2 );
						if ( SortInf->SortResult == 0 && SortInf->ListWndLine1.Cells.GetLength() > 1 && SortInf->ListWndLine2.Cells.GetLength() > 1 )
						{
							//SortInf->SortResult = strcmp(SortInf->ListWndLine1.Cells[1].Text, SortInf->ListWndLine2.Cells[1].Text);
						}*/

					}
					break;
				case 1://seconds
					{
						//add sort code
					}
					break;
				}
			}
		}
		return CSidlScreenWnd::WndNotification(pWnd,Message,pData);
    }

    void ReplacevfTable()
    {
        OldvfTable=((_CSIDLWND*)this)->pvfTable;
        PCSIDLWNDVFTABLE NewvfTable=new CSIDLWNDVFTABLE;
        memcpy(NewvfTable,OldvfTable,sizeof(CSIDLWNDVFTABLE));
        ((_CSIDLWND*)this)->pvfTable=NewvfTable;
    }

    void RemovevfTable()
    {
        PCSIDLWNDVFTABLE NewvfTable=((_CSIDLWND*)this)->pvfTable;
        ((_CSIDLWND*)this)->pvfTable=OldvfTable;
        delete NewvfTable;
    }

    void SetvfTable(DWORD index, DWORD value)
    {
        DWORD* vtable=(DWORD*)((_CSIDLWND*)this)->pvfTable;
        vtable[index]=value;
    }
	VOID AddLogLine(__time64_t totalfighttime, __int64 dpsaverage, PCHAR szNote, ...)
	{
		if (!LogList)
			return;
		va_list vaList;
		va_start(vaList, szNote);
		int len = _vscprintf(szNote, vaList) + 1;// _vscprintf doesn't count // terminating '\0'  
		if (char *szOutput = (char *)LocalAlloc(LPTR, len + 32)) {
			vsprintf_s(szOutput, len, szNote, vaList);
			char tmpbuf[128] = { 0 };
			struct tm today = { 0 };
			time_t tm = { 0 };
			tm = time(NULL);
			localtime_s(&today, &tm);
			strftime(tmpbuf, 128, "%Y/%m/%d %H:%M:%S", &today);
			int id = LogList->AddString(tmpbuf, 0xFF00FF00, 0, 0);
			sprintf_s(tmpbuf, "%lld Seconds",totalfighttime);
			LogList->SetItemText(id, 1, &CXStr(tmpbuf));
			sprintf_s(tmpbuf, "%lld",dpsaverage);
			LogList->SetItemText(id, 2, &CXStr(tmpbuf));
			LogList->SetItemText(id, 3, &CXStr(szOutput));

			LocalFree(szOutput);
		}
	}
/*0x218*/   CListWnd   *LogList;
/*0x21c*/   CButtonWnd   *UpdateButton;
/*0x220*/   CButtonWnd   *ResetButton;
/*0x224*/	PCSIDLWNDVFTABLE OldvfTable;
};
bool bDisabledDamageParserLogWnd = false;
HANDLE ghLockMap = 0;
typedef struct _MYEQSuccessfulHit 
{
/*0x00*/ CHAR	DamagedName[0x40];
/*0x40*/ CHAR	AttackerName[0x40];
		 int DamagedID;
		 int AttackerID;
/*0x80*/ unsigned char	Skill;//1 HS etc...
/*0x84*/ int	SpellID;
/*0x88*/ int	DamageCaused;
/*0x8c*/ float	Force;
/*0x90*/ float	HitHeading;
/*0x94*/ float	HitPitch;
/*0x98*/ bool	bSecondary;
/*0x9c*/ int	SpecialCase;//origin of damage? need to investigate further -eqmule
/*0xa0*/ //added by me for time of dmg
bool bNPC;
__time64_t TimeStamp;
} MYEQSuccessfulHit, *pMyEQSuccessfulHit;
#pragma pack(push)
#pragma pack(1)
typedef struct _EQSuccessfulHit2
{
/*0x00*/ unsigned short	DamagedID;//Spawn that was hit
/*0x02*/ unsigned short	AttackerID;//spawn who did the hit
/*0x04*/ unsigned char	Skill;//1 HS etc...
/*0x08*/ int	SpellID;
/*0x0c*/ int	DamageCaused;
/*0x10*/ float	Force;
/*0x14*/ float	HitHeading;
/*0x18*/ float	HitPitch;
/*0x1c*/ bool	bSecondary;
/*0x20*/ int	SpecialCase;//origin of damage? need to investigate further -eqmule
/*0x24*/ 
} EQSuccessfulHit2, *pEQSuccessfulHit2;
#pragma pack(pop)
std::map<int, std::list<MYEQSuccessfulHit>>AccumulatedDamage;
void DPSReset(PSPAWNINFO pLPlayer, char* szLine)
{
	lockit lk(ghLockMap,"DPSReset");
	WriteChatf("Cleared the DPS map.");
	AccumulatedDamage.clear();
}
void AddToDamageMap(int ID, pMyEQSuccessfulHit hit)
{
	lockit lk(ghLockMap,"AddToDamageMap");
	AccumulatedDamage[ID].push_back(*hit);
}
bool GetDamageMap(std::map<int, std::list<MYEQSuccessfulHit>>&dmgmap)
{
	lockit lk(ghLockMap,"GetDamageMap");
	if (AccumulatedDamage.size())
	{
		dmgmap = AccumulatedDamage;
		return true;
	}
	return false;
}
size_t GetDamageMapSize()
{
	lockit lk(ghLockMap,"GetDamageMapSize");
	return AccumulatedDamage.size();
}
void ClearDamageMap()
{
	lockit lk(ghLockMap,"ClearDamageMap");
	AccumulatedDamage.clear();
}

class MyCEverQuest
{
public:

	void MyCEverQuest::ReportSuccessfulHit_Tramp(EQSuccessfulHit2 *pHit, unsigned char bOutputText, int ActualHeal);
	void MyCEverQuest::ReportSuccessfulHit_Detour(EQSuccessfulHit2 *pHit, unsigned char bOutputText, int ActualHeal)
	{
		if (PSPAWNINFO pDamaged = (PSPAWNINFO)GetSpawnByID(pHit->DamagedID)) {
			if (PSPAWNINFO pAttacker = (PSPAWNINFO)GetSpawnByID(pHit->AttackerID)) {
				MYEQSuccessfulHit succ;
				succ.bNPC = false;
				if (pDamaged->Type == SPAWN_NPC)
					succ.bNPC = true;
				succ.TimeStamp = MQGetTickCount64();
				strcpy_s(succ.AttackerName, pAttacker->Name);
				strcpy_s(succ.DamagedName, pDamaged->Name);
				succ.DamagedID = pHit->DamagedID;
				succ.AttackerID = pHit->AttackerID;
				succ.bSecondary = pHit->bSecondary;
				if (pHit->DamageCaused < 0 && pAttacker->SpawnID==((PSPAWNINFO)pLocalPlayer)->SpawnID) {
					//we just got healed i think or something, not sure, need to investigate
					//dont want it to impact the parser.
					succ.DamageCaused = 0;
				}
				else {
					succ.DamageCaused = pHit->DamageCaused;
				}
				succ.Force = pHit->Force;
				succ.HitHeading = pHit->HitHeading;
				succ.HitPitch = pHit->HitPitch;
				succ.Skill = pHit->Skill;
				succ.SpecialCase = pHit->SpecialCase;
				succ.SpellID = pHit->SpellID;
				AddToDamageMap(pDamaged->SpawnID,&succ);
				
				//if (!_stricmp(((PSPAWNINFO)pLocalPlayer)->Name, succ.AttackerName)) {
				//	WriteChatf("[1] I did %d damage to %s (%d) skill = %d",succ.DamageCaused,succ.DamagedName, succ.DamagedID,succ.Skill);
				//}
			}
		}
		return ReportSuccessfulHit_Tramp(pHit, bOutputText, ActualHeal);
	}
};
DETOUR_TRAMPOLINE_EMPTY(void MyCEverQuest::ReportSuccessfulHit_Tramp(EQSuccessfulHit2 *pHit, unsigned char bOutputText, int ActualHeal));

int GetTotalDamage(int AttackedID, int AttackerID, PCHAR szAttacker,PCHAR szAttacked)
{
	lockit lk(ghLockMap,"GetTotalDamage");
	int TotalDmg = 0;
	szAttacker[0] = '\0';
	szAttacked[0] = '\0';
	std::map<int, std::list<MYEQSuccessfulHit>>AccDmg;
	if (GetDamageMap(AccDmg))
	{
		if (AccDmg.find(AttackedID) != AccDmg.end()) {
			strcpy_s(szAttacked, MAX_STRING, AccDmg[AttackedID].begin()->DamagedName);
			for (std::list<MYEQSuccessfulHit>::iterator i = AccDmg[AttackedID].begin(); i != AccDmg[AttackedID].end(); i++) {
				if (AttackerID) {
					if (i->AttackerID == AttackerID) {
						if (szAttacker[0] == '\0')
							strcpy_s(szAttacker, MAX_STRING, i->AttackerName);
						TotalDmg += i->DamageCaused;
					}
				}
				else {
					if (szAttacker[0] == '\0')
						strcpy_s(szAttacker, MAX_STRING, "EVERYONE");
					TotalDmg += i->DamageCaused;
				}
			}
		}
	}
	return TotalDmg;
}
void DPSList(PSPAWNINFO pLPlayer, char* szLine)
{
	std::map<int, std::list<MYEQSuccessfulHit>>AccDmg;
	if (GetDamageMap(AccDmg))
	{
		for (std::map<int, std::list<MYEQSuccessfulHit>>::iterator i = AccDmg.begin(); i != AccDmg.end(); i++) {
			WriteChatf("%d (%s)", i->first, i->second.begin()->DamagedName);
		}
	}
}
__int64 GetAverageDPS(char *Name,__time64_t *calctime)
{
	try
	{
		__int64 AllDmg = 0;
		__time64_t fightend = MQGetTickCount64();
		__time64_t fightstart = 0;
		__time64_t fullfighttime = 0;
		std::map<int, std::list<MYEQSuccessfulHit>>AccDmg;

		if (GetDamageMap(AccDmg))
		{
			for (std::map<int, std::list<MYEQSuccessfulHit>>::iterator i = AccDmg.begin(); i != AccDmg.end(); i++) {
				fightstart = 0;
				fightend = 0;
				for (std::list<MYEQSuccessfulHit>::iterator l = i->second.begin(); l != i->second.end(); l++) {
					if (!l->bNPC)
						continue;
					if (!_stricmp(Name, l->AttackerName))
					{
						if (fightstart == 0)
							fightstart = i->second.begin()->TimeStamp;
						if (l->DamageCaused > 0 && l->AttackerID != l->DamagedID)
						{
							AllDmg += l->DamageCaused;
						}
						else {
							Sleep(0);
						}
					}
					fightend = l->TimeStamp;
				}
				if (fightend > fightstart && fightstart != 0)
					fullfighttime += fightend - fightstart;
				else
					Sleep(0);
			}
			if (*calctime = fullfighttime / 10)
			{
				if (__int64 thetime = *calctime / 1000)
				{
					__int64 ret = AllDmg / thetime;
					return ret;
				}
			}
		}
	}
	catch (...)
	{
		Sleep(0);//mf...
	}
	return 0;
}
void DPSAverage(PSPAWNINFO pLPlayer, char* szLine)
{
	CHAR szMe[MAX_STRING] = { 0 };
	strcpy_s(szMe, ((PSPAWNINFO)pLocalPlayer)->Name);
	__time64_t calctime = 0;
	__int64 dps = GetAverageDPS(szMe, &calctime);
	if (pDamageParserLogWnd) {
		if (pDamageParserLogWnd->dShow == true)
		{
			pDamageParserLogWnd->AddLogLine(calctime / 1000, dps, "%s", szMe);
			return;
		}
	}
	WriteChatf("You have been fighting for %lld seconds, Average Damage per second for %s is %I64u right now.", calctime / 1000, szMe, dps);
}
void WriteSetting(const char*Key, const char*value, bool bOutputtext = true)
{
	CHAR szSettingINISection[MAX_STRING] = { 0 };
	if (!pLocalPlayer || EQADDR_SERVERNAME[0] == '\0' || !gBUsePerCharSettings)
	{
		strcpy_s(szSettingINISection, "Default");
	}
	else
	{
		sprintf_s(szSettingINISection, "%s_%s", EQADDR_SERVERNAME, ((PSPAWNINFO)pLocalPlayer)->Name);
	}
	WritePrivateProfileString(szSettingINISection, Key, value, INIFileName);
	if (bOutputtext)
	{
		int ival = atoi(value);
		WriteChatf("\ayMQ2DamageParser\ax::\am%s is now %s\ax.", Key, ival ? "\aoON" : "\agOFF");
	}
}
void DPSLog(PSPAWNINFO pLPlayer, char* szLine)
{
	if (pDamageParserLogWnd)
	{
		if (pDamageParserLogWnd->dShow == false)
		{
			pDamageParserLogWnd->dShow = true;
			gbShowLogWindow = true;
			WriteSetting("ShowLogWindow", "1");
		}
		else
		{
			pDamageParserLogWnd->dShow = false;
			gbShowLogWindow = false;
			WriteSetting("ShowLogWindow", "0");
		}
	}
}

void DPSReport(PSPAWNINFO pLPlayer, char* szLine)
{
	if (szLine && szLine[0] == '\0') {
		WriteChatf("Usage: /dpsreport <AttackedID> optional <AttackerID> if you dont supply a <AttackerID> it will default to yourself.");
		WriteChatf("if you specify <AttackerID> as -1 it will report TOTAL damage on the mob done by EVERYONE.");
		return;
	}
	CHAR szAttackedID[MAX_STRING] = { 0 };
	CHAR szAttackerID[MAX_STRING] = { 0 };
	GetArg(szAttackedID, szLine, 1);
	GetArg(szAttackerID, szLine, 2);
	int AttackedID = atoi(szAttackedID);
	int AttackerID = atoi(szAttackerID);

	if (AttackerID == 0) {
		AttackerID = ((PSPAWNINFO)pLocalPlayer)->SpawnID;
	}
	else if (AttackerID == -1) {
		AttackerID = 0;
	}
	int TotalDmg = GetTotalDamage(AttackedID, AttackerID, szAttackedID, szAttackerID);
	WriteChatf("TotalDamage Caused by %s to %s was %d", szAttackedID,szAttackerID, TotalDmg);
}
#define ISINDEX() (Index[0])
#define ISNUMBER() (IsNumber(Index))
#define GETNUMBER() (atoi(Index))
#define GETFIRST() Index
class MQ2DamageType *pDamageType=0; 

class MQ2DamageType : public MQ2Type 
{ 
    public: 
        enum DamageMembers { 
            Individual=1, 
            Total=2, 
            List=3, 
            Last=4, 
            Average=5,
			TotalFightingTime=6,
        }; 

        MQ2DamageType():MQ2Type("damage") { 
            TypeMember(Individual); 
            TypeMember(Total); 
            TypeMember(List); 
            TypeMember(Last); 
            TypeMember(Average); 
            TypeMember(TotalFightingTime); 
        } 

        ~MQ2DamageType() {}

        bool GetMember(MQ2VARPTR VarPtr, PCHAR Member, PCHAR Index, MQ2TYPEVAR &Dest) { 
            PMQ2TYPEMEMBER pMember=MQ2DamageType::FindMember(Member); 
            if (!pMember) 
                return false; 
            switch((DamageMembers)pMember->ID) {
				case Individual:
				{
					CHAR szAttackerID[MAX_STRING] = { 0 };
					CHAR szSpawnID[MAX_STRING] = { 0 };
					int AttackerID, SpawnID;
					//get the spawnid they want damage for
					if (PCHAR pComma = strchr(GETFIRST(), ',')) {
						*pComma = 0;
						SpawnID = atoi(&pComma[1]);
						AttackerID = atoi(GETFIRST());
					}
					else {
						//default to self
						AttackerID = GETNUMBER();
						SpawnID = ((PSPAWNINFO)pLocalPlayer)->SpawnID;
					}					
					Dest.Int = GetTotalDamage(AttackerID, SpawnID, szAttackerID, szSpawnID);
					Dest.Type = pIntType;
					return true;
				}
                case Total:
					if (ISNUMBER()) {
						int AttackerID = GETNUMBER();
						CHAR szAttackerID[MAX_STRING] = { 0 };
						CHAR szSpawnID[MAX_STRING] = { 0 };
						Dest.Int = GetTotalDamage(AttackerID, 0, szAttackerID, szSpawnID);
						Dest.Type = pIntType;
						return true;
					}
					break;
                case List: 
                    Dest.Ptr=&DataTypeTemp[0]; 
                    Dest.Type=pStringType; 
                    return true;
				case Last:
					Dest.Ptr=&DataTypeTemp[0]; 
                    Dest.Type=pStringType;
					return true;
				case TotalFightingTime:
				{
					__time64_t t64 = 0;
					GetAverageDPS(((PSPAWNINFO)pLocalPlayer)->Name,&t64);
					Dest.UInt64 = t64;
					Dest.Type = pTimeStampType;
					return true;
				}
				case Average:
				{
					__time64_t t64 = 0;
					Dest.UInt64 = GetAverageDPS(((PSPAWNINFO)pLocalPlayer)->Name, &t64);
					Dest.Type = pInt64Type;
					return true;
				}
            } 
            return false; 
        }

        bool ToString(MQ2VARPTR VarPtr, PCHAR Destination)  { 
            strcpy_s(Destination,MAX_STRING,"TRUE"); 
            return true; 
        } 

        bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source) { 
            return false; 
        } 

        bool FromString(MQ2VARPTR &VarPtr, PCHAR Source) { 
            return false; 
        } 
}; 

BOOL dataDamage(PCHAR szName, MQ2TYPEVAR &Dest) 
{ 
    Dest.DWord=1; 
    Dest.Type=pDamageType; 
    return true; 
}
struct TextParamMsg
{
	std::string TargetName;//haven't seen this used...
	bool bSentFromWorld;
	int StringType;
	int StringColor;
	std::string Token[9];
};
#if !defined(ROF2EMU) && !defined(UFEMU)
void msgTokenTextParam_Tramp(char *, UINT);
void msgTokenTextParam_Detour(char *Data, UINT Len)
#else
void msgTokenTextParam_Tramp(char *);
void msgTokenTextParam_Detour(char *Data)
#endif
{
	try {
#if !defined(ROF2EMU) && !defined(UFEMU)
		if (Len > 100000) {
			MessageBox(NULL, "Error in msgTokenTextParam", "You have the offset wrong", MB_SYSTEMMODAL | MB_OK);
			return;
		}
#else 
		int Len = 200;
#endif
		PBYTE Buffer = (PBYTE)LocalAlloc(LPTR, Len);
		PBYTE BufferOrg = Buffer;
		memcpy(Buffer, Data, Len);
#if !defined(ROF2EMU) && !defined(UFEMU)
		Buffer += 4;
#endif
		TextParamMsg Msg;
		Msg.bSentFromWorld = *(bool*)Buffer;
#if !defined(ROF2EMU) && !defined(UFEMU)
		Buffer++;
#else
		Buffer+=4;
#endif
		Msg.StringType = *(int*)Buffer;
		
		if (Msg.StringType == 13327) {// %1 has taken %2 damage from %3 by %4.%5
			Buffer += 4;
			Msg.StringColor = *(int*)Buffer;
			Buffer += 4;
			CHAR szTemp[64] = { 0 };
			#if !defined(ROF2EMU) && !defined(UFEMU)
			int tokenlen = *(int*)Buffer;
			Buffer += 4;
			for (int i = 0; i < 4; ++i)
			{
				memcpy(szTemp, Buffer, tokenlen);
				szTemp[tokenlen] = '\0';
				Msg.Token[i] = szTemp;
				Buffer += Msg.Token[i].size();
				tokenlen = *(int*)Buffer;
				Buffer += 4;
			}
			#else
				Msg.Token[0] = (char*)Buffer;
				Buffer += Msg.Token[0].size() + 1;
				Msg.Token[1] = (char*)Buffer;
				Buffer += Msg.Token[1].size() + 1;
				Msg.Token[2] = (char*)Buffer;
				Buffer += Msg.Token[2].size() + 1;
				Msg.Token[3] = (char*)Buffer;
			#endif
			if (PSPAWNINFO pAttacker = (PSPAWNINFO)GetSpawnByName((char*)Msg.Token[3].c_str())) {
				strcpy_s(szTemp, Msg.Token[0].c_str());
				SEARCHSPAWN SearchSpawn;
				ClearSearchSpawn(&SearchSpawn);
				SearchSpawn.FRadius = 999999.0f;
				strcpy_s(SearchSpawn.szName, szTemp);
				if (IsGroupMember((char*)Msg.Token[3].c_str())) {
					SearchSpawn.bXTarHater = true;
				}
				else {
					//not group member lets try nearest to that guy then
					SearchSpawn.bNearAlert = true;
					SearchSpawn.NearAlertList = 99;
					sprintf_s(szTemp,"add 99 id %d",pAttacker->SpawnID);
					BOOL Temp = gFilterMQ;
					gFilterMQ = true;
					Alert(NULL, szTemp);
					gFilterMQ = Temp;
				}
				if (PSPAWNINFO pDamaged = SearchThroughSpawns(&SearchSpawn, (PSPAWNINFO)pLocalPlayer)) {
					if (PSPELL pSpell = GetSpellByName((char*)Msg.Token[2].c_str())) {
						MYEQSuccessfulHit succ;
						succ.bNPC = false;
						if (pDamaged->Type == SPAWN_NPC)
							succ.bNPC = true;
						succ.TimeStamp = MQGetTickCount64();
						strcpy_s(succ.AttackerName, pAttacker->Name);
						strcpy_s(succ.DamagedName, pDamaged->Name);
						succ.DamagedID = pDamaged->SpawnID;
						succ.AttackerID = pAttacker->SpawnID;
						succ.bSecondary = false;
						succ.DamageCaused = atoi(Msg.Token[1].c_str());
						succ.Force = 0;
						succ.HitHeading = 0;
						succ.HitPitch = 0;
						succ.Skill = 231;//spells
						succ.SpecialCase = 0;
						succ.SpellID = pSpell->ID;
						AddToDamageMap(pDamaged->SpawnID, &succ);
						//if (!_stricmp(((PSPAWNINFO)pLocalPlayer)->Name, succ.AttackerName)) {
						//	WriteChatf("[3] %s did %d damage to %s (%d) skill = %d", succ.AttackerName, succ.DamageCaused, succ.DamagedName, succ.DamagedID, succ.Skill);
						//}
					}
				}
				BOOL Temp = gFilterMQ;
				gFilterMQ = true;
				Alert(NULL, "clear 99");
				gFilterMQ = Temp;
			}
		} else if (Msg.StringType == 9072) {// %1 has taken %2 damage from your %3.%4
			Buffer += 4;
			Msg.StringColor = *(int*)Buffer;
			Buffer += 4;
			CHAR szTemp[64] = { 0 };
			#if !defined(ROF2EMU) && !defined(UFEMU)
				int tokenlen = *(int*)Buffer;
				Buffer += 4;
				for (int i = 0; i < 3; ++i)
				{
					memcpy(szTemp, Buffer, tokenlen);
					szTemp[tokenlen] = '\0';
					Msg.Token[i] = szTemp;
					Buffer += Msg.Token[i].size();
					tokenlen = *(int*)Buffer;
					Buffer += 4;
				}
			#else
				Msg.Token[0] = (char*)Buffer;
				Buffer += Msg.Token[0].size() + 1;
				Msg.Token[1] = (char*)Buffer;
				Buffer += Msg.Token[1].size() + 1;
				Msg.Token[2] = (char*)Buffer;
			#endif
			strcpy_s(szTemp, Msg.Token[0].c_str());
			SEARCHSPAWN SearchSpawn;
			ClearSearchSpawn(&SearchSpawn);
			SearchSpawn.FRadius = 999999.0f;
			strcpy_s(SearchSpawn.szName, szTemp);
			SearchSpawn.bXTarHater = true;
			if(PSPAWNINFO pDamaged = SearchThroughSpawns(&SearchSpawn, (PSPAWNINFO)pLocalPlayer)) {
				if (PSPAWNINFO pAttacker = (PSPAWNINFO)pLocalPlayer) {
					if (PSPELL pSpell = GetSpellByName((char*)Msg.Token[2].c_str())) {
						MYEQSuccessfulHit succ;
						succ.bNPC = false;
						if (pDamaged->Type == SPAWN_NPC)
							succ.bNPC = true;
						succ.TimeStamp = MQGetTickCount64();
						strcpy_s(succ.AttackerName, pAttacker->Name);
						strcpy_s(succ.DamagedName, pDamaged->Name);
						succ.DamagedID = pDamaged->SpawnID;
						succ.AttackerID = pAttacker->SpawnID;
						succ.bSecondary = false;
						succ.DamageCaused = atoi(Msg.Token[1].c_str());
						succ.Force = 0;
						succ.HitHeading = 0;
						succ.HitPitch = 0;
						succ.Skill = 231;//spells
						succ.SpecialCase = 0;
						succ.SpellID = pSpell->ID;
						AddToDamageMap(pDamaged->SpawnID, &succ);
						//if (!_stricmp(((PSPAWNINFO)pLocalPlayer)->Name, succ.AttackerName)) {
						//	WriteChatf("[2] I did %d damage to %s (%d) skill = %d", succ.DamageCaused, succ.DamagedName, succ.DamagedID, succ.Skill);
						//}
					}
				}
			}
		}
		LocalFree(BufferOrg);
		//char*String = pStringTable->getString(Msg.StringType);
		//Sleep(0);
	}
	catch (...) {
		Sleep(0);
	}
	//CUnSerializeBuffer Buffer( Data, Len );
	//Msg.Unserialize( Buffer );
	
	

#if !defined(ROF2EMU) && !defined(UFEMU)
	return msgTokenTextParam_Tramp(Data, Len);
#else
	return msgTokenTextParam_Tramp(Data);
#endif
}
#if !defined(ROF2EMU) && !defined(UFEMU)
DETOUR_TRAMPOLINE_EMPTY(void msgTokenTextParam_Tramp(char *, UINT));
#else
DETOUR_TRAMPOLINE_EMPTY(void msgTokenTextParam_Tramp(char *));
#endif
bool ReadLocSetting(char*Key,int left, int top, int right, int bottom,RECT *RCOut)
{
	CHAR szSettingINISection[MAX_STRING] = { 0 };
	CHAR szUI[MAX_STRING] = { 0 };
	CHAR szLoc[MAX_STRING] = { 0 };
	if (!pLocalPlayer || EQADDR_SERVERNAME[0] == '\0' || !gBUsePerCharSettings)
	{
		strcpy_s(szSettingINISection, "Default");
	}
	else
	{
		sprintf_s(szSettingINISection, "%s_%s", EQADDR_SERVERNAME, ((PSPAWNINFO)pLocalPlayer)->Name);
	}
	sprintf_s(szUI,"%d,%d,%d,%d", left, top, right, bottom);
	int ret = GetPrivateProfileString(szSettingINISection, Key, "",szLoc,MAX_STRING, INIFileName);
	if (szLoc[0] == '\0')
	{
		WritePrivateProfileString(szSettingINISection, Key,szUI, INIFileName);
		(*RCOut).top = top;
		(*RCOut).bottom = bottom;
		(*RCOut).left = left;
		(*RCOut).right = right;
		return false;
	}
	char *token1 = NULL;
	char *next_token1 = NULL;
	CHAR szLocs[4][8];
	token1 = strtok_s(szLoc, ",", &next_token1);
	int j = 0;
	while (token1 != NULL)
	{
		if (token1 != NULL)
		{
			strcpy_s(szLocs[j], token1);
			token1 = strtok_s(NULL, ",", &next_token1);
			j++;
		}
	}
	(*RCOut).left = atoi(szLocs[0]);
	(*RCOut).top = atoi(szLocs[1]);
	(*RCOut).right = atoi(szLocs[2]);
	(*RCOut).bottom = atoi(szLocs[3]);
	return true;
}
VOID DestroyDamageParserLogWnd() 
{ 
    if (pDamageParserLogWnd) 
    { 
		CHAR szLoc[MAX_STRING] = { 0 };
		sprintf_s(szLoc, "%d,%d,%d,%d", pDamageParserLogWnd->Location.left, pDamageParserLogWnd->Location.top, pDamageParserLogWnd->Location.right, pDamageParserLogWnd->Location.bottom);
		WriteSetting("LogWindowLoc", szLoc,false);
		sprintf_s(szLoc, "%d", pDamageParserLogWnd->LogList->GetColumnWidth(0));
		
		WriteSetting("TimeColumnWidth", szLoc,false);
		sprintf_s(szLoc, "%d", pDamageParserLogWnd->LogList->GetColumnWidth(1));
		WriteSetting("TFTColumnWidth", szLoc,false);
		sprintf_s(szLoc, "%d", pDamageParserLogWnd->LogList->GetColumnWidth(2));
		WriteSetting("TDAColumnWidth", szLoc,false);
		sprintf_s(szLoc, "%d", pDamageParserLogWnd->LogList->GetColumnWidth(3));
		WriteSetting("NameColumnWidth", szLoc,false);
		delete pDamageParserLogWnd;
		pDamageParserLogWnd = 0;
    } 
}
template <unsigned int _Size>LPSTR ReadStringSetting(char*Key,char *defaultval,char(&_Out)[_Size])
{
	CHAR szSettingINISection[MAX_STRING] = { 0 };
	if (!pLocalPlayer || EQADDR_SERVERNAME[0] == '\0' || !gBUsePerCharSettings)
	{
		strcpy_s(szSettingINISection, "Default");
	}
	else
	{
		sprintf_s(szSettingINISection, "%s_%s", EQADDR_SERVERNAME, ((PSPAWNINFO)pLocalPlayer)->Name);
	}
	int ret = GetPrivateProfileString(szSettingINISection, Key, "",_Out,_Size, INIFileName);
	if (_Out[0] == '\0')
	{
		WritePrivateProfileString(szSettingINISection, Key,defaultval, INIFileName);
		strcpy_s(_Out, _Size, defaultval);
	}
	return _Out;
}
void CreateDamageParserLogWnd()
{
	try {
		if (pDamageParserLogWnd || bDisabledDamageParserLogWnd) {
			return; 
		}
		
		
		HMODULE hMe = 0;
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCTSTR)CreateDamageParserLogWnd, &hMe);
		void* pMyBinaryData = 0;
		CHAR szEQPath[MAX_PATH] = { "C:\\Users\\Public\\Daybreak Game Company\\Installed Games\\EverQuest\\eqgame.exe" };
		CHAR szMQUI_DamageParserLogWndPath[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, szEQPath, MAX_PATH);
		if (char *pDest = strstr(szEQPath,"eqgame.exe"))
		{
			pDest[0] = '\0';
			strcpy_s(szMQUI_DamageParserLogWndPath, szEQPath);
			strcat_s(szMQUI_DamageParserLogWndPath, "UIFiles\\Default\\MQUI_DamageParserLogWnd.xml");
		}
		WIN32_FIND_DATA FindFile = { 0 };
		HANDLE hSearch = FindFirstFile(szMQUI_DamageParserLogWndPath, &FindFile);
		if (hSearch != INVALID_HANDLE_VALUE) {
			FindClose(hSearch);
			if (pSidlMgr && pSidlMgr->FindScreenPieceTemplate("DamageParserLogWnd")) {
				if (pDamageParserLogWnd = new CDamageParserLogWnd("DamageParserLogWnd")) {
					ReadLocSetting("LogWindowLoc", 100,330,710,550, &LogWindowLoc);
					pDamageParserLogWnd->Location.left = LogWindowLoc.left;
					pDamageParserLogWnd->Location.top = LogWindowLoc.top;
					pDamageParserLogWnd->Location.right = LogWindowLoc.right;
					pDamageParserLogWnd->Location.bottom = LogWindowLoc.bottom;
					CHAR szTemp[MAX_STRING] = { 0 };
					ReadStringSetting("TimeColumnWidth","123",szTemp);
					pDamageParserLogWnd->LogList->SetColumnWidth(0, atoi(szTemp));
					ReadStringSetting("TFTColumnWidth","98",szTemp);
					pDamageParserLogWnd->LogList->SetColumnWidth(1, atoi(szTemp));
					ReadStringSetting("TDAColumnWidth","108",szTemp);
					pDamageParserLogWnd->LogList->SetColumnWidth(2, atoi(szTemp));
					ReadStringSetting("NameColumnWidth","100",szTemp);
					pDamageParserLogWnd->LogList->SetColumnWidth(3, atoi(szTemp));
				}
			}
			else
			{
				bDisabledDamageParserLogWnd = true;
				WriteChatf("Could not find DamageParserLogWnd\nPlease do /loadskin default");
			}
		}
		else {
			bDisabledDamageParserLogWnd = true;
			MessageBox(NULL, "MQUI_DamageParserLogWnd.xml not Found in UIFiles\\default\nI will disable this feature for now.\nYou can retry again by /plugin mq2itemdisplay unload and then /plugin mq2itemdisplay", "MQ2ItemDisplay", MB_OK | MB_SYSTEMMODAL);
		}
	} 
	catch(...) { 
		MessageBox(NULL,"CRAP! in CreateDamageParserLogWnd","An exception occured",MB_OK);
	}
}

PLUGIN_API VOID InitializePlugin(VOID)
{
	if (!ghLockMap)
		ghLockMap = CreateMutex(NULL, FALSE, NULL);
    DebugSpewAlways("Initializing MQ2DamageParser");
	int ret = GetPrivateProfileInt("Default", "UsePerCharSettings", -1, INIFileName);
	gBUsePerCharSettings = (ret == 0 ? FALSE : TRUE);
	if (ret == -1)
	{
		gBUsePerCharSettings = 0;
		WritePrivateProfileString("Default", "UsePerCharSettings", "0", INIFileName);
	}
	CHAR szShowLogWindow[MAX_STRING] = { 0 };
	ReadStringSetting("ShowLogWindow", "0", szShowLogWindow);
	gbShowLogWindow = atoi(szShowLogWindow);
	//dots are unfortunately just text messages from the server
	EzDetourwName(__msgTokenTextParam, msgTokenTextParam_Detour, msgTokenTextParam_Tramp, "mTTP");

	//dd and melee
	EzDetourwName(CEverQuest__ReportSuccessfulHit, &MyCEverQuest::ReportSuccessfulHit_Detour, &MyCEverQuest::ReportSuccessfulHit_Tramp, "RSH");
    //Add commands, MQ2Data items, hooks, etc.
    AddCommand("/dpslog",DPSLog,FALSE,TRUE,TRUE);
    AddCommand("/dpsreset",DPSReset,FALSE,TRUE,TRUE);
    AddCommand("/dpsaverage",DPSAverage,FALSE,TRUE,TRUE);
    AddCommand("/dpsreport",DPSReport,FALSE,TRUE,TRUE);
    AddCommand("/dpslist",DPSList,FALSE,TRUE,TRUE);
	pDamageType = new MQ2DamageType;
	AddMQ2Data("Damage",dataDamage); 
    
	HMODULE hMe = 0;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCTSTR)InitializePlugin, &hMe);
	void* pMyBinaryData = 0;
	CHAR szEQPath[2048] = { "C:\\Users\\Public\\Daybreak Game Company\\Installed Games\\EverQuest\\eqgame.exe" };
	CHAR szMQUI_DamageParserLogWndPath[2048] = { 0 };
	GetModuleFileName(NULL, szEQPath, 2048);
	if (char *pDest = strstr(szEQPath,"eqgame.exe"))
	{
		pDest[0] = '\0';
		strcpy_s(szMQUI_DamageParserLogWndPath, szEQPath);
		strcat_s(szMQUI_DamageParserLogWndPath, "UIFiles\\Default\\MQUI_DamageParserLogWnd.xml");
	}
	WIN32_FIND_DATA FindFile = { 0 };
	HANDLE hSearch = FindFirstFile(szMQUI_DamageParserLogWndPath, &FindFile);
	if (hSearch == INVALID_HANDLE_VALUE) {
		//need to unpack our resource.
		if (HRSRC hRes = FindResource(hMe, MAKEINTRESOURCE(IDR_XML1), "XML")) {
			if (HGLOBAL bin = LoadResource(hMe, hRes)) {
				BOOL bResult = 0;
				if (pMyBinaryData = LockResource(bin)) {
					//save it...
					DWORD ressize = SizeofResource(hMe, hRes);
					FILE *File = 0;
					errno_t err = fopen_s(&File, szMQUI_DamageParserLogWndPath, "wb");
					if (!err) {
						fwrite(pMyBinaryData, ressize, 1, File);
						fclose(File);
					}
					bResult = UnlockResource(hRes);
				}
				bResult = FreeResource(hRes);
			}
		}
	} else {
		FindClose(hSearch);
	}
	AddXMLFile("MQUI_DamageParserLogWnd.xml");;
	if (gGameState == GAMESTATE_INGAME)
	{
		//ReadProfile(GetCharInfo()->Name, FALSE);
		CreateDamageParserLogWnd();
	}
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
    DebugSpewAlways("Shutting down MQ2DamageParser");

    //Remove commands, MQ2Data items, hooks, etc.
    //RemoveMQ2Benchmark(bmMyBenchmark);
	RemoveMQ2Data("Damage"); 
    delete pDamageType;
    RemoveCommand("/dpslog");
    RemoveCommand("/dpsreset");
    RemoveCommand("/dpsaverage");
    RemoveCommand("/dpsreport");
    RemoveCommand("/dpslist");
	RemoveDetour(CEverQuest__ReportSuccessfulHit);
	RemoveDetour(__msgTokenTextParam);
	DestroyDamageParserLogWnd();
	lockit lk(ghLockMap, "leaving");
	if (ghLockMap) {
		ReleaseMutex(ghLockMap);
		CloseHandle(ghLockMap);
		ghLockMap = 0;
	}
}
PLUGIN_API void OnCleanUI()
{
	DestroyDamageParserLogWnd();
}

PLUGIN_API void OnReloadUI()
{
	if (GetGameState() == GAMESTATE_INGAME && pCharSpawn) {
		bDisabledDamageParserLogWnd = false;
		CreateDamageParserLogWnd();
	}
}
int pulsecount = 0;
PLUGIN_API VOID OnPulse(VOID)
{
	pulsecount++;
	if (pulsecount > 100)
	{
		pulsecount = 0;
		if (GetGameState() == GAMESTATE_INGAME) {
			CreateDamageParserLogWnd();
		}
	}
}
PLUGIN_API VOID OnZoned(VOID)
{
	if (GetGameState() == GAMESTATE_INGAME) {
		if (GetDamageMapSize() > 1000) {
			WriteChatf("MQ2DamageParser : Cleared all entries due to zoning and size exceeding 1000 entries.");
			ClearDamageMap();
		}
	}
}