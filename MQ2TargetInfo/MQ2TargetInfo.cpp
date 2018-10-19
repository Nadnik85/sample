// MQ2TargetInfo.cpp : Defines the entry point for the DLL application.
// by EqMule 2018

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.



#include "../MQ2Plugin.h"
#include "resource.h"

PreSetup("MQ2TargetInfo");
CHAR szTargetInfo[128] = { "Target Info" };
CHAR szTargetDistance[128] = { "Target Distance" };
CHAR szGroupDistance[128] = { "Member Distance" };
CHAR szCanSeeTarget[128] = { "Can See Target" };
CHAR szPHToolTip[128] = { "Target is a Place Holder" };
CHAR szPH[128] = { "PH" };
CHAR szNavToolTip[128] = { "ALL will Nav to Me" };
CHAR szNav[128] = { "Come to Me" };
CHAR szFollowMeToolTip[128] = { "Follow Me around." };
CHAR szFollowMe[128] = { "Follow Me" };

HANDLE hLockphmap = 0;
CLabelWnd*InfoLabel = 0;
CLabelWnd*DistanceLabel = 0;
CLabelWnd*CanSeeLabel = 0;
CButtonWnd*PHButton = 0;
CButtonWnd*NavButton = 0;
CButtonWnd*FollowMeButton = 0;
bool gbFollowme = false;
CSidlScreenWnd*Target_BuffWindow = 0;
CLabelWnd*Target_AggroPctPlayerLabel = 0;
CLabelWnd*Target_AggroNameSecondaryLabel = 0;
CLabelWnd*Target_AggroPctSecondaryLabel = 0;

typedef struct _phinfo
{
	std::string Expansion;
	std::string Zone;
	std::string Named;
	std::string Link;
}phinfo,*pphinfo;

std::map<std::string, phinfo> phmap;
bool GetPhMap(PSPAWNINFO pSpawn, phinfo *pinf);
class MyCTargetWnd
{
public:
	//we can safely use this as its always called when a user leftclicks the target window.
	//also the upside here is that we dont collide with the trade with target detour in mq2windows...
	void MyCTargetWnd::HandleBuffRemoveRequest_Tramp(CXWnd *);
	void MyCTargetWnd::HandleBuffRemoveRequest_Detour(CXWnd *pWnd)
	{
		if(PHButton && pWnd==PHButton)
		{
			if (pTarget) {
				phinfo pinf;
				if (GetPhMap((PSPAWNINFO)pTarget, &pinf)) {
					std::string url = "https://webproxy.to/browse.php?b=4&u=";
					url.append(pinf.Link);// https://eqresource.com&b=4";
					//std::string url = "https://www.google.com/search?q=";
					//std::string url = "http://everquest.allakhazam.com/search.html?q=";
					//url.append(pinf.Named);
					if (CHtmlWnd *ItemHtmlwnd = pCWebManager->CreateHtmlWnd(url.c_str(), pinf.Named.c_str(), NULL, true, pinf.Named.c_str()))
					{
						//Beep(1000, 100);
					}
				}
			}
		}
		HandleBuffRemoveRequest_Tramp(pWnd);
	}
};
DETOUR_TRAMPOLINE_EMPTY(void MyCTargetWnd::HandleBuffRemoveRequest_Tramp(CXWnd*));
void LoadPHs(char*szMyName) {
	//well we have it, lets fill in the map...
	//Chief Librarian Lars^a shissar arbiter, a shissar defiler^tds^kattacastrumdeluge^https://tds.eqresource.com/chieflibrarianlars.php
	phinfo phinf;
	std::string phs;
	int commapos = 0;
	CHAR szBuffer[2048] = { 0 };
	FILE *fp = 0;
	errno_t err = fopen_s(&fp, szMyName, "rb");
	if (!err) {
		while (fgets(szBuffer, 2048, fp) != 0) {
			if (char *pDest = strchr(szBuffer, '^')) {
				pDest[0] = '\0';
				phinf.Named = szBuffer;
				pDest++;
				if (char *pDest2 = strchr(pDest, '^')) {
					pDest2[0] = '\0';
					phs = pDest;
					*pDest2++;
					if (pDest = strchr(pDest2, '^')) {
						pDest[0] = '\0';
						phinf.Expansion = pDest2;
						pDest++;
						if (pDest2 = strchr(pDest, '^')) {
							pDest2[0] = '\0';
							phinf.Zone = pDest;
							pDest2++;
							if (pDest = strchr(pDest2, '\r')) {
								pDest[0] = '\0';
							}
							phinf.Link = pDest2;
						}
					}
				}
			}
			if (phs.find(",") != phs.npos && phs.find("Yikkarvi,") == phs.npos &&
				phs.find("Furg,") == phs.npos && phs.find("Tykronar,") == phs.npos &&
				phs.find("Ejarld,") == phs.npos && phs.find("Grald,") == phs.npos &&
				phs.find("Graluk,") == phs.npos) {
				while ((commapos = phs.find_last_of(",")) != phs.npos) {
					//more than one...
					std::string temp = phs.substr(commapos + 2, -1);
					phs.erase(commapos, -1);
					phmap[temp] = phinf;
				}
				phmap[phs] = phinf;
			}
			else {
				phmap[phs] = phinf;
			}
		}
		fclose(fp);
	}
}
/*#define Target_BuffWindow_TopOffset 62;
#define dTopOffset 46;
#define dBottomOffset 60;
#define InfoTopOffset 33;
#define dLeftOffset 50;
#define InfoBottomOffset 47;
*/
#define Target_BuffWindow_TopOffset 62+14;
#define dTopOffset 46+14;
#define dBottomOffset 60+14;
#define InfoTopOffset 33+14;
#define dLeftOffset 50;
#define InfoBottomOffset 47+14;
CGaugeWnd *GW_Gauge1 = 0;
CGaugeWnd *GW_Gauge2 = 0;
CGaugeWnd *GW_Gauge3 = 0;
CGaugeWnd *GW_Gauge4 = 0;
CGaugeWnd *GW_Gauge5 = 0;
CLabelWnd *GroupDistLabel1 = 0;
CLabelWnd *GroupDistLabel2 = 0;
CLabelWnd *GroupDistLabel3 = 0;
CLabelWnd *GroupDistLabel4 = 0;
CLabelWnd *GroupDistLabel5 = 0;
bool CreateDistLabel(CGroupWnd*pGwnd,CControlTemplate *DistLabelTemplate,CLabelWnd **labelwnd,char*label,int top, int bottom, int right)
{
	SetCXStr(&DistLabelTemplate->Name, label);
	SetCXStr(&DistLabelTemplate->ScreenID, label);
	if (*labelwnd = (CLabelWnd *)pSidlMgr->CreateXWndFromTemplate((CXWnd*)pGwnd, DistLabelTemplate)) {
		(*labelwnd)->dShow = true;
		(*labelwnd)->TopOffset = top;
		(*labelwnd)->BottomOffset = bottom;
		(*labelwnd)->LeftOffset = 50;
		(*labelwnd)->RightOffset = right;
		(*labelwnd)->CRNormal = 0xFF00FF00;//green
		(*labelwnd)->BGColor = 0xFFFFFFFF;
		SetCXStr(&(*labelwnd)->Tooltip, szGroupDistance);
		return true;
	}
	return false;
}
int navmenuid = 0;
int separatorid = 0;

void CreateAButton(CGroupWnd*pGwnd,CControlTemplate *Template,CButtonWnd **button,char*label,char*labelscreen, int fontsize, int top, int bottom, int left, int right, COLORREF color, COLORREF bgcolor, char*tooltip, char*text)
{
	Template->Font = 1;
	SetCXStr(&Template->Name, label);
	SetCXStr(&Template->ScreenID, labelscreen);
	if (*button = (CButtonWnd *)pSidlMgr->CreateXWndFromTemplate((CXWnd*)pGwnd, Template)) {

		(*button)->dShow = true;
		(*button)->TopOffset = top;
		(*button)->BottomOffset = bottom;
		(*button)->LeftOffset = left;
		(*button)->RightOffset = right;
		(*button)->CRNormal = color;
		(*button)->BGColor = bgcolor;
		SetCXStr(&(*button)->WindowText, text);
		SetCXStr(&(*button)->Tooltip, tooltip);
	}
}
void Initialize()
{
	if (!DistanceLabel && GetGameState() == GAMESTATE_INGAME)
	{
		//setup the group info
		if (CGroupWnd*pGwnd = (CGroupWnd*)pGroupWnd) {
			if (pGwnd->GroupContextMenu)
			{
				//separatorid = pGwnd->GroupContextMenu->AddSeparator();
				navmenuid = pGwnd->GroupContextMenu->AddMenuItem("Nav to Me", 200);
				((CListWnd*)pGwnd->GroupContextMenu)->CalculateVSBRange();
			}
			GW_Gauge1 = (CGaugeWnd*)((CXWnd*)pGwnd)->GetChildItem("GW_Gauge1");
			GW_Gauge2 = (CGaugeWnd*)((CXWnd*)pGwnd)->GetChildItem("GW_Gauge2");
			GW_Gauge3 = (CGaugeWnd*)((CXWnd*)pGwnd)->GetChildItem("GW_Gauge3");
			GW_Gauge4 = (CGaugeWnd*)((CXWnd*)pGwnd)->GetChildItem("GW_Gauge4");
			GW_Gauge5 = (CGaugeWnd*)((CXWnd*)pGwnd)->GetChildItem("GW_Gauge5");
			//
			CControlTemplate *DistLabelTemplate = (CControlTemplate*)pSidlMgr->FindScreenPieceTemplate("Target_AggroPctSecondaryLabel");
			CControlTemplate *NavButtonTemplate = (CControlTemplate*)pSidlMgr->FindScreenPieceTemplate("GW_InviteButton");//borrowing this...
			//
			if (GW_Gauge1 && DistLabelTemplate) {

				SetCXStr(&DistLabelTemplate->Controller, "0");
				//create the distance label 1
				
				bool ther = CreateDistLabel(pGwnd, DistLabelTemplate,&GroupDistLabel1,"Group_DistLabel1", GW_Gauge1->TopOffset, GW_Gauge1->BottomOffset, GW_Gauge1->RightOffset);
				ther = CreateDistLabel(pGwnd, DistLabelTemplate,&GroupDistLabel2,"Group_DistLabel2", GW_Gauge2->TopOffset, GW_Gauge2->BottomOffset, GW_Gauge2->RightOffset);
				ther = CreateDistLabel(pGwnd, DistLabelTemplate,&GroupDistLabel3,"Group_DistLabel3", GW_Gauge3->TopOffset, GW_Gauge3->BottomOffset, GW_Gauge3->RightOffset);
				ther = CreateDistLabel(pGwnd, DistLabelTemplate,&GroupDistLabel4,"Group_DistLabel4", GW_Gauge4->TopOffset, GW_Gauge4->BottomOffset, GW_Gauge4->RightOffset);
				ther = CreateDistLabel(pGwnd, DistLabelTemplate,&GroupDistLabel5,"Group_DistLabel5", GW_Gauge5->TopOffset, GW_Gauge5->BottomOffset, GW_Gauge5->RightOffset);
				
				//create Nav All to Me Button
				if (NavButtonTemplate)
				{
					CButtonWnd*Butt = (CButtonWnd*)((CXWnd*)pGwnd)->GetChildItem("GW_InviteButton");
					CreateAButton(pGwnd, NavButtonTemplate,&NavButton,"GW_NavButton","NavButton", 1, Butt->TopOffset + 40, Butt->BottomOffset + 25, 6, 46, 0xFF00FFFF, 0xFFFFFFFF, szNavToolTip, szNav);
					CreateAButton(pGwnd, NavButtonTemplate,&FollowMeButton,"GW_FollowMeButton","FollowMeButton", 1, Butt->TopOffset + 40, Butt->BottomOffset + 25, 50, 90, 0xFF00FFFF, 0xFFFFFFFF, szFollowMeToolTip, szFollowMe);
					
					//
					//now set the template values back
					SetCXStr(&NavButtonTemplate->Name, "GW_InviteButton");
					SetCXStr(&NavButtonTemplate->ScreenID, "InviteButton");
					NavButtonTemplate->Font = 2;
				}
				//
				//now set the template values back
				SetCXStr(&DistLabelTemplate->Name, "Target_AggroPctSecondaryLabel");
				SetCXStr(&DistLabelTemplate->ScreenID, "Target_AggroPctSecondaryLabel");
				SetCXStr(&DistLabelTemplate->Controller, "308");
			}
		}
		//setup the targetinfo
		if (PCTARGETWND pTwnd = (PCTARGETWND)pTargetWnd) {
			//
			Target_AggroPctPlayerLabel = (CLabelWnd*)((CXWnd*)pTwnd)->GetChildItem("Target_AggroPctPlayerLabel");
			Target_AggroNameSecondaryLabel = (CLabelWnd*)((CXWnd*)pTwnd)->GetChildItem("Target_AggroNameSecondaryLabel");
			Target_AggroPctSecondaryLabel = (CLabelWnd*)((CXWnd*)pTwnd)->GetChildItem("Target_AggroPctSecondaryLabel");
			Target_BuffWindow = (CSidlScreenWnd*)((CXWnd*)pTwnd)->GetChildItem("Target_BuffWindow");

			CControlTemplate *DistLabelTemplate = (CControlTemplate*)pSidlMgr->FindScreenPieceTemplate("Target_AggroPctSecondaryLabel");
			CControlTemplate *CanSeeLabelTemplate = (CControlTemplate*)pSidlMgr->FindScreenPieceTemplate("Target_AggroNameSecondaryLabel");
			CControlTemplate *PHButtonTemplate = (CControlTemplate*)pSidlMgr->FindScreenPieceTemplate("IDW_ModButton");//borrowing this...
			if (PHButtonTemplate && Target_BuffWindow && CanSeeLabelTemplate && Target_AggroNameSecondaryLabel && Target_AggroPctSecondaryLabel && Target_AggroPctPlayerLabel && DistLabelTemplate) {
				Target_BuffWindow->BGColor = 0xFF000000;
				Target_BuffWindow->TopOffset = Target_BuffWindow_TopOffset;
				Target_AggroPctPlayerLabel->BGColor = 0xFF00000;
				Target_AggroNameSecondaryLabel->BGColor = 0xFF00000;
				Target_AggroPctSecondaryLabel->BGColor = 0xFF00000;
				Target_AggroPctPlayerLabel->TopOffset = dTopOffset;
				Target_AggroPctPlayerLabel->BottomOffset = dBottomOffset;
				Target_AggroNameSecondaryLabel->TopOffset = dTopOffset;
				Target_AggroNameSecondaryLabel->BottomOffset = dBottomOffset;
				Target_AggroPctSecondaryLabel->TopOffset = dTopOffset;
				Target_AggroPctSecondaryLabel->BottomOffset = dBottomOffset;
				//CHAR szTemp[16];
				//sprintf_s(szTemp, "\xE2\x8C\x96");// , 0xE2, 0x8C, 0x96);

				SetCXStr(&DistLabelTemplate->Controller, "0");
				SetCXStr(&CanSeeLabelTemplate->Controller, "0");
				//create the info label
				SetCXStr(&DistLabelTemplate->Name, "Target_InfoLabel");
				SetCXStr(&DistLabelTemplate->ScreenID, "Target_InfoLabel");
				if (InfoLabel = (CLabelWnd *)pSidlMgr->CreateXWndFromTemplate((CXWnd*)pTwnd, DistLabelTemplate)) {
					InfoLabel->dShow = true;
					InfoLabel->bAlignCenter = false;
					InfoLabel->bAlignRight = false;
					InfoLabel->bLeftAnchoredToLeft = true;
					InfoLabel->TopOffset = 33;
					InfoLabel->BottomOffset = 47;
					InfoLabel->LeftOffset = 2;
					InfoLabel->RightOffset = 60;
					InfoLabel->CRNormal = 0xFF00FF00;//green
					InfoLabel->BGColor = 0xFFFFFFFF;
					SetCXStr(&InfoLabel->Tooltip, szTargetInfo);
				}
				//create the distance label
				SetCXStr(&DistLabelTemplate->Name, "Target_DistLabel");
				SetCXStr(&DistLabelTemplate->ScreenID, "Target_DistLabel");
				if (DistanceLabel = (CLabelWnd *)pSidlMgr->CreateXWndFromTemplate((CXWnd*)pTwnd, DistLabelTemplate)) {
					DistanceLabel->dShow = true;
					DistanceLabel->TopOffset = InfoTopOffset;
					DistanceLabel->BottomOffset = InfoBottomOffset;
					DistanceLabel->LeftOffset = dLeftOffset;
					DistanceLabel->RightOffset = 2;
					DistanceLabel->CRNormal = 0xFF00FF00;//green
					DistanceLabel->BGColor = 0xFFFFFFFF;
					SetCXStr(&DistanceLabel->Tooltip, szTargetDistance);
				}
				//create can see label
				SetCXStr(&CanSeeLabelTemplate->Name, "Target_CanSeeLabel");
				SetCXStr(&CanSeeLabelTemplate->ScreenID, "Target_CanSeeLabel");
				if (CanSeeLabel = (CLabelWnd *)pSidlMgr->CreateXWndFromTemplate((CXWnd*)pTwnd, CanSeeLabelTemplate)) {
					CanSeeLabel->dShow = true;
					CanSeeLabel->TopOffset = InfoTopOffset;
					CanSeeLabel->BottomOffset = InfoBottomOffset;
					CanSeeLabel->LeftOffset = dLeftOffset;
					CanSeeLabel->RightOffset = dLeftOffset;
					CanSeeLabel->CRNormal = 0xFF00FF00;//green
					CanSeeLabel->BGColor = 0xFFFFFFFF;
					SetCXStr(&CanSeeLabel->Tooltip, szCanSeeTarget);
				}
				//create PHButton
				PHButtonTemplate->Font = 0;
				if (PHButton = (CButtonWnd *)pSidlMgr->CreateXWndFromTemplate((CXWnd*)pTwnd, PHButtonTemplate)) {
					PHButton->dShow = true;
					PHButton->bBottomAnchoredToTop = Target_AggroPctPlayerLabel->bBottomAnchoredToTop;
					PHButton->bLeftAnchoredToLeft = Target_AggroPctPlayerLabel->bLeftAnchoredToLeft;
					PHButton->bRightAnchoredToLeft = Target_AggroPctPlayerLabel->bRightAnchoredToLeft;
					PHButton->bTopAnchoredToTop = Target_AggroPctPlayerLabel->bTopAnchoredToTop;
					PHButton->TopOffset = InfoTopOffset + 1;
					PHButton->BottomOffset = dTopOffset - 1;
					PHButton->LeftOffset = Target_AggroPctPlayerLabel->LeftOffset;
					PHButton->RightOffset = Target_AggroPctPlayerLabel->RightOffset;
					PHButton->Location.top = InfoTopOffset + 1;
					PHButton->Location.bottom = PHButton->BottomOffset;
					PHButton->Location.left = 2;
					PHButton->Location.right = 20;
					PHButton->CRNormal = 0xFF00FFFF;//cyan
					PHButton->BGColor = 0xFFFFFFFF;
					SetCXStr(&PHButton->Tooltip, szPHToolTip);
					SetCXStr(&PHButton->WindowText, szPH);
				}
				//
				//now set the template values back
				PHButtonTemplate->Font = 2;
				SetCXStr(&DistLabelTemplate->Name, "Target_AggroPctSecondaryLabel");
				SetCXStr(&DistLabelTemplate->ScreenID, "Target_AggroPctSecondaryLabel");
				SetCXStr(&DistLabelTemplate->Controller, "308");
				SetCXStr(&CanSeeLabelTemplate->Name, "Target_AggroNameSecondaryLabel");
				SetCXStr(&CanSeeLabelTemplate->ScreenID, "Target_AggroNameSecondaryLabel");
				SetCXStr(&CanSeeLabelTemplate->Controller, "304");
			}
		}
	}
}
bool CXWnd::IsType(EWndRuntimeType eType) const
{
	int Types = RuntimeTypes.GetLength();
	for (int i = 0; i < Types; i++)
	{
		if (RuntimeTypes[i] == (UINT)eType)
		{
			return true;
		}
	}
	return false;
}
int rightclickindex = -1;
void StopMovement()
{
	FollowMeButton->Checked = false;
	gbFollowme = false;
	CHAR szMe[MAX_STRING] = { 0 };
	if (GetModuleHandle("mq2advpath"))
	{
		sprintf_s(szMe, "/squelch /bcg //squelch /afollow off");
		DoCommand((PSPAWNINFO)pLocalPlayer, szMe);
	}
	if (GetModuleHandle("mq2moveutils"))
	{
		sprintf_s(szMe, "/squelch /bcg //squelch /stick off");
		DoCommand((PSPAWNINFO)pLocalPlayer, szMe);
	}
	if (GetModuleHandle("mq2nav"))
	{
		sprintf_s(szMe, "/squelch /bcg //squelch /nav stop");
		DoCommand((PSPAWNINFO)pLocalPlayer, szMe);
	}
	WriteChatf("\ayMQ2TargetInfo\ax : Stopped Following.");
}
class CGroupWnd2
{
public:
	void UpdateDisplay_Tramp(int, PSPAWNINFO, COLORREF, UINT);
	void UpdateDisplay_Detour(int Index, PSPAWNINFO groupmember, COLORREF NameColor, UINT RoleBits)
	{
		if (PCHARINFO pChar = GetCharInfo())
		{
			if (CGroupClient *group = (CGroupClient *)pChar->pGroupInfo)
			{
				int ind = rightclickindex;// group->GroupSelectID;
				if (Index == ind)
				{
					//NameColor = 0xFF0040FF;
				}
			}
		}
		/*if (NameColor == 0xFF00FF00)
		{
			NameColor = 0xFF0000FF;
		}*/
		return UpdateDisplay_Tramp(Index, groupmember, NameColor, RoleBits);
	}
	int CGroupWnd2::GetSelectedGroupIndex(CXWnd* pWnd)
	{
		CLabel* lab = (CLabel*)pWnd;
		int index = -1;
		if (PCHARINFO pChar = GetCharInfo())
		{
			if (CGroupClient *group = (CGroupClient *)pChar->pGroupInfo)
			{
				for (int i = 0; i < 6; i++)
				{
					if (   pWnd == ((CGroupWnd*)this)->HPGauge[i]
						|| pWnd == ((CGroupWnd*)this)->PetGauge[i]
						|| pWnd == ((CGroupWnd*)this)->ManaGauge[i]
						|| pWnd == ((CGroupWnd*)this)->EnduranceGauge[i]
						|| lab  == ((CGroupWnd*)this)->HPLabel[i]
						|| lab  == ((CGroupWnd*)this)->HPPercLabel[i]
						|| lab  == ((CGroupWnd*)this)->ManaLabel[i]
						|| lab  == ((CGroupWnd*)this)->ManaPercLabel[i]
						|| lab  == ((CGroupWnd*)this)->EnduranceLabel[i]
						|| lab  == ((CGroupWnd*)this)->EndurancePercLabel[i]
						|| lab  == ((CGroupWnd*)this)->EnduranceLabel[i]
						|| pWnd == ((CGroupWnd*)this)->GroupTankButton[i]
						|| pWnd == ((CGroupWnd*)this)->GroupAssistButton[i]
						|| pWnd == ((CGroupWnd*)this)->GroupPullerButton[i]
						|| pWnd == ((CGroupWnd*)this)->GroupMarkNPCButton[i]
						|| lab  == ((CGroupWnd*)this)->AggroPercLabel[i]
						)
					{
						index = i;
						break;
					}
				}
			}
		}
		return index;
	}
	
	int WndNotification_Trampoline(CXWnd*, unsigned __int32, void*);
	int WndNotification_Detour(CXWnd* pWnd, unsigned __int32 Message, void* pData)
	{
		if (Message == XWM_RCLICK || Message == XWM_RSELITEM_DOWN)
		{
			rightclickindex = this->GetSelectedGroupIndex(pWnd);
			//if (rightclickindex != -1)
			//{
			//	WriteChatf("User Rightclicked group member number %d", rightclickindex);
			//}
			//return 1;
		}
		else if (Message == XWM_LCLICK)
		{
			if (pWnd == NavButton)
			{
				StopMovement();
				CHAR szMe[MAX_STRING] = { 0 };
				sprintf_s(szMe, "/bcg //nav id %d", ((PSPAWNINFO)pLocalPlayer)->SpawnID);
				DoCommand((PSPAWNINFO)pLocalPlayer, szMe);
				return 1;
			} else if (pWnd == FollowMeButton)
			{
				gbFollowme ^= true;
				FollowMeButton->Checked = gbFollowme;
				CHAR szMe[MAX_STRING] = { 0 };
				if (gbFollowme)
				{
					if (GetModuleHandle("mq2advpath"))
					{
						sprintf_s(szMe, "/bcg //afollow spawn %d", ((PSPAWNINFO)pLocalPlayer)->SpawnID);
					}
					else if (GetModuleHandle("mq2moveutils"))
					{
						sprintf_s(szMe, "/bcg //stick id %d 5 uw", ((PSPAWNINFO)pLocalPlayer)->SpawnID);
					}
					else
					{
						WriteChatf("Please do /plugin mq2advpath or /plugin mq2moveutils.");
						StopMovement();
						return 1;
					}
				}
				else
				{
					StopMovement();
					return 1;
				}
				DoCommand((PSPAWNINFO)pLocalPlayer, szMe);
				return 1;
			}
		}
		else if (Message == XWM_MENUSELECT)
		{
			if ((int)pData == 200)//our nav menu id
			{
				CHAR szMe[MAX_STRING] = { 0 };
				//Beep(1000, 100);
				PSPAWNINFO pSpawn = 0;
				if (PCHARINFO pChar = GetCharInfo())
				{
					if (CGroupClient *group = (CGroupClient *)pChar->pGroupInfo)
					{
						int ind = rightclickindex;// group->GroupSelectID;
						if (ind!=-1 && ind < 6)
						{
							if (group->pMembers[ind] && group->pMembers[ind]->pSpawn && group->pMembers[ind]->Type!=1)
							{
								pSpawn = group->pMembers[ind]->pSpawn;
							}
						}
					}
				}
				if (pSpawn)
				{
					if (gbFollowme)
					{
						FollowMeButton->Checked = false;
						gbFollowme = false;
					}
					StopMovement();
					sprintf_s(szMe, "/bct %s //nav id %d",pSpawn->Name, ((PSPAWNINFO)pLocalPlayer)->SpawnID);
					DoCommand((PSPAWNINFO)pLocalPlayer, szMe);
				}
				return 1;//we dont need to call the tramp, its our message...
			}
		}
		return WndNotification_Trampoline(pWnd, Message, pData);
	}
};
DETOUR_TRAMPOLINE_EMPTY(int CGroupWnd2::WndNotification_Trampoline(CXWnd*, unsigned __int32, void*));
DETOUR_TRAMPOLINE_EMPTY(void CGroupWnd2::UpdateDisplay_Tramp(int, PSPAWNINFO, COLORREF, UINT));
// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	if (!hLockphmap)
		hLockphmap = CreateMutex(NULL, FALSE, NULL);
	EzDetourwName(CGroupWnd__UpdateDisplay, &CGroupWnd2::UpdateDisplay_Detour, &CGroupWnd2::UpdateDisplay_Tramp, "GUD");
	EzDetourwName(CGroupWnd__WndNotification, &CGroupWnd2::WndNotification_Detour, &CGroupWnd2::WndNotification_Trampoline, "GWW");
	HMODULE hMe = 0;
	CHAR szMyName[2048] = { 0 };
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCTSTR)InitializePlugin, &hMe);
	void* pMyBinaryData = 0;
	GetModuleFileName(hMe, szMyName, 2048);
	if (char *pDest = strrchr(szMyName, '.')) {
		pDest[0] = '\0';
		strcat_s(szMyName, ".txt");
	}
	WIN32_FIND_DATA FindFile = { 0 };
	HANDLE hSearch = FindFirstFile(szMyName, &FindFile);
	if (hSearch == INVALID_HANDLE_VALUE) {
		//need to unpack our resource.
		
		if (HRSRC hRes = FindResource(hMe, MAKEINTRESOURCE(IDR_DB1), "DB")) {
			if (HGLOBAL bin = LoadResource(hMe, hRes)) {
				BOOL bResult = 0;
				if (pMyBinaryData = LockResource(bin)) {
					//save it...
					DWORD ressize = SizeofResource(hMe, hRes);
					FILE *File = 0;
					errno_t err = fopen_s(&File, szMyName, "wb");
					if (!err) {
						fwrite(pMyBinaryData, ressize, 1, File);
						fclose(File);
					}
					bResult = UnlockResource(hRes);
				}
				bResult = FreeResource(hRes);
			}
		}
		LoadPHs(szMyName);
	}
	else {
		FindClose(hSearch);
		LoadPHs(szMyName);
	}
	EzDetourwName(CTargetWnd__HandleBuffRemoveRequest, &MyCTargetWnd::HandleBuffRemoveRequest_Detour, &MyCTargetWnd::HandleBuffRemoveRequest_Tramp, "CTargetWnd__HandleBuffRemoveRequest");
	DebugSpewAlways("Initializing MQ2TargetInfo");
	Initialize();
}
void CleanUp(bool bUnload)
{
	if (CGroupWnd*pGwnd = (CGroupWnd*)pGroupWnd) {
		if (pGwnd->GroupContextMenu && navmenuid)
		{
			//pGwnd->GroupContextMenu->RemoveMenuItem(separatorid);
			pGwnd->GroupContextMenu->RemoveMenuItem(navmenuid);//54
			navmenuid = 0;
			Sleep(0);
		}
	}
	if (GroupDistLabel1) {
		((CButtonWnd*)GroupDistLabel1)->Destroy();
		GroupDistLabel1 = 0;
	}
	if (GroupDistLabel2) {
		((CButtonWnd*)GroupDistLabel2)->Destroy();
		GroupDistLabel2 = 0;
	}
	if (GroupDistLabel3) {
		((CButtonWnd*)GroupDistLabel3)->Destroy();
		GroupDistLabel3 = 0;
	}
	if (GroupDistLabel4) {
		((CButtonWnd*)GroupDistLabel4)->Destroy();
		GroupDistLabel4 = 0;
	}
	if (GroupDistLabel5) {
		((CButtonWnd*)GroupDistLabel5)->Destroy();
		GroupDistLabel5 = 0;
	}
	if (InfoLabel) {
		((CButtonWnd*)InfoLabel)->Destroy();
		InfoLabel = 0;
	}
	if (DistanceLabel) {
		((CButtonWnd*)DistanceLabel)->Destroy();
		DistanceLabel = 0;
	}
	if (CanSeeLabel) {
		((CButtonWnd*)CanSeeLabel)->Destroy();
		CanSeeLabel = 0;
	}
	if (PHButton) {
		((CButtonWnd*)PHButton)->Destroy();
		PHButton = 0;
	}
	if (NavButton) {
		((CButtonWnd*)NavButton)->Destroy();
		NavButton = 0;
	}
	if (FollowMeButton) {
		((CButtonWnd*)FollowMeButton)->Destroy();
		FollowMeButton = 0;
	}
	
	if (GetGameState() == GAMESTATE_INGAME) {
		if (bUnload) {
			if (!IsBadReadPtr(Target_BuffWindow, 4)) {
				Target_BuffWindow->TopOffset = 50;
			}

			if (!IsBadReadPtr(Target_AggroPctPlayerLabel, 4)) {
				Target_AggroPctPlayerLabel->TopOffset = 33;
				Target_AggroPctPlayerLabel->BottomOffset = 47;
			}
			if (!IsBadReadPtr(Target_AggroNameSecondaryLabel, 4)) {
				Target_AggroNameSecondaryLabel->TopOffset = 33;
				Target_AggroNameSecondaryLabel->BottomOffset = 47;
			}
			if (!IsBadReadPtr(Target_AggroPctSecondaryLabel, 4)) {
				Target_AggroPctSecondaryLabel->TopOffset = 33;
				Target_AggroPctSecondaryLabel->BottomOffset = 47;
			}
		}
	}
}
// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("Shutting down MQ2TargetInfo");
	if(CTargetWnd__HandleBuffRemoveRequest)
		RemoveDetour(CTargetWnd__HandleBuffRemoveRequest);
	CleanUp(true);
	if (hLockphmap) {
		ReleaseMutex(hLockphmap);
		CloseHandle(hLockphmap);
		hLockphmap = 0;
	}
	RemoveDetour(CGroupWnd__WndNotification);
	RemoveDetour(CGroupWnd__UpdateDisplay);
}

// Called after entering a new zone
PLUGIN_API VOID OnZoned(VOID)
{
	DebugSpewAlways("MQ2TargetInfo::OnZoned()");
}

// Called once directly before shutdown of the new ui system, and also
// every time the game calls CDisplay::CleanGameUI()
PLUGIN_API VOID OnCleanUI(VOID)
{
	DebugSpewAlways("MQ2TargetInfo::OnCleanUI()");
	// destroy custom windows, etc
	CleanUp(false);
}

bool IsPlaceHolder(PSPAWNINFO pSpawn)
{
	lockit lk(hLockphmap,"IsPlaceHolder");
	if (pSpawn && phmap.find(pSpawn->DisplayedName) != phmap.end()) {
		return true;
	}
	return false;
}
bool GetPhMap(PSPAWNINFO pSpawn, phinfo *pinf)
{
	lockit lk(hLockphmap,"IsPlaceHolder");
	if (pSpawn && phmap.find(pSpawn->DisplayedName) != phmap.end()) {
		*pinf = phmap[pSpawn->DisplayedName];
		return true;
	}
	return false;
}
// This is called every time MQ pulses
CHAR szTargetDist[64] = { 0 };
int looper = 0;
PSPAWNINFO oldspawn = 0;
void UpdateGroupDist(PCHARINFO pChar, int index)
{
	CLabelWnd *pWnd = 0;
	switch (index)
	{
	case 1:
		pWnd = GroupDistLabel1;
		break;
	case 2:
		pWnd = GroupDistLabel2;
		break;
	case 3:
		pWnd = GroupDistLabel3;
		break;
	case 4:
		pWnd = GroupDistLabel4;
		break;
	case 5:
		pWnd = GroupDistLabel5;
		break;
	};
	if (pWnd)
	{
		if (pChar->pGroupInfo->pMember[index] && pChar->pGroupInfo->pMember[index]->pSpawn)
		{
			float dist = Distance3DToSpawn(pLocalPlayer, pChar->pGroupInfo->pMember[index]->pSpawn);
			sprintf_s(szTargetDist, "Dist: %.2f", dist);
			if (dist < 250) {
				pWnd->CRNormal = 0xFF00FF00;//green
			}
			else {
				pWnd->CRNormal = 0xFFFF0000;//red
			}
			SetCXStr(&pWnd->WindowText, szTargetDist);
			pWnd->dShow = true;
		}
		else
		{
			pWnd->dShow = false;
		}
	}
}
PLUGIN_API VOID OnPulse(VOID)
{
	// DONT leave in this debugspew, even if you leave in all the others
	//DebugSpewAlways("MQ2TargetInfo::OnPulse()");
	looper++;
	if (looper > 20) {
		looper = 0;
		if (GetGameState() == GAMESTATE_INGAME) {
			Initialize();
			//
			if (CGroupWnd *pGwnd = (CGroupWnd*)pGroupWnd) {
				if (GroupDistLabel1 && GroupDistLabel2 && GroupDistLabel3 && GroupDistLabel4 && GroupDistLabel5)
				{
					if (PCHARINFO pChar = GetCharInfo())
					{
						if (pChar->pGroupInfo)
						{
							UpdateGroupDist(pChar,1);
							UpdateGroupDist(pChar,2);
							UpdateGroupDist(pChar,3);
							UpdateGroupDist(pChar,4);
							UpdateGroupDist(pChar,5);
						}
						else {
							GroupDistLabel1->dShow = false;
							GroupDistLabel2->dShow = false;
							GroupDistLabel3->dShow = false;
							GroupDistLabel4->dShow = false;
							GroupDistLabel5->dShow = false;
						}
					}
				}
			}
			if (PCTARGETWND pTwnd = (PCTARGETWND)pTargetWnd) {
				if (InfoLabel && DistanceLabel && CanSeeLabel && PHButton) {
					if (pTarget && pCharSpawn) {
						if (oldspawn != (PSPAWNINFO)pTarget) {
							oldspawn = (PSPAWNINFO)pTarget;
							phinfo pinf;
							if (GetPhMap((PSPAWNINFO)pTarget, &pinf)) {
								SetCXStr(&PHButton->Tooltip, (char*)pinf.Named.c_str());
								PHButton->dShow = true;
							}
							else {
								PHButton->dShow = false;
							}
						}
						//set info
						PSPAWNINFO pInfo = (PSPAWNINFO)pTarget;
						switch (pInfo->Anon)
						{
						case 1:
							sprintf_s(szTargetDist, "Anonymous");
							break;
						case 2:
							sprintf_s(szTargetDist, "Roleplaying");
							break;
						default:
							sprintf_s(szTargetDist, "Lvl: %d %s %s", pInfo->Level, pEverQuest->GetRaceDesc(pInfo->mActorClient.Race), GetClassDesc(pInfo->mActorClient.Class));
							break;
						}
						SetCXStr(&InfoLabel->WindowText, szTargetDist);
						//then distance
						float dist = Distance3DToSpawn(pLocalPlayer, pTarget);
						sprintf_s(szTargetDist, "Dist: %.2f", dist);
						if (dist < 250) {
							DistanceLabel->CRNormal = 0xFF00FF00;//green
						}
						else {
							DistanceLabel->CRNormal = 0xFFFF0000;//red
						}
						SetCXStr(&DistanceLabel->WindowText, szTargetDist);
						//now do can see
						bool cansee = pCharSpawn->CanSee((EQPlayer*)pTarget);
						sprintf_s(szTargetDist, "%s", cansee ? "O" : "X");
						if (cansee) {
							CanSeeLabel->CRNormal = 0xFF00FF00;//green
						}
						else {
							CanSeeLabel->CRNormal = 0xFFFF0000;//red
						}
						SetCXStr(&CanSeeLabel->WindowText, szTargetDist);
					}
					else {
						SetCXStr(&InfoLabel->WindowText, "");
						SetCXStr(&DistanceLabel->WindowText, "");
						SetCXStr(&CanSeeLabel->WindowText, "");
						PHButton->dShow = false;
					}
				}
			}
		}
	}
}