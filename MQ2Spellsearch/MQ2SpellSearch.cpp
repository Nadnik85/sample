/* MQ2SpellSearch.cpp 
Version 0.3.0 Oct 01 2006
by htw 
original code by koad

Features: 
- Adds /spellsearch command that you can use to lookup a spell 
by its name, or its casted emote text. Search is done on spell 
data in active memory. This is an interpreter not a dictionary. 
- Spells are searched for by an invoked thread, which on most 
machines helps avoid the 'freeze until search complete' syndrome 
- The dedicated window mode uses the default chatbox, no MQUI files needed 

Setup: 
- You need to setup your Settings section of your MQ2SpellSearch.ini 
or the plugin will not give you more than 1 match ever. Ex: 
[Settings] 
UseWnd=1 
MaxResults=15 
- The window is NOT shown by default at all. When you use a 
/spellsearch, the window will be visible. 
(Press ESC to hide it, output is not lost) 
- Window settings are saved per server per char name in the ini, 
only when the window class is destroyed. ie: game process does 
a OnCleanUI() or plugin is unloaded. 

Changes: 
0.3.0 - Changed the way the window was being handled, to prevent CTDs.
0.2.4 - Dumped my spellslotinfo function for the one that was adapted into MQ2Main 
and is since maintained by the community 
0.2.3 - Cleaned up search thread, it was looking past the end of the valid array 
0.2.2 - Fixed struct usage that was causing GUI crashes. 
0.2.1 - Added Sharp of Fairlight's diff list of spell effect types 
- Added stacking handling display for blocking and overwriting 
the function this uses may need fleshed out further. 
The GetEffectName and GetTargetType functions were split off 
to improve readability and decrease size. 

*/ 

#include "../MQ2Plugin.h" 

PreSetup("MQ2SpellSearch"); 

VOID SpellSearchCommand(PSPAWNINFO pChar, PCHAR argSpell); 
DWORD WINAPI FindSpellThread(LPVOID lpParam); 
VOID ShowSpellInfo(PSPELL pSpell); 
PCHAR GetEffectName(DWORD EffectID, PCHAR szBuffer); 
PCHAR GetTargetType(DWORD TargetID, PCHAR szBuffer); 
VOID SlotValueCalc(PCHAR szBuff, PSPELL pSpell, int i, double mp); 
VOID CreateSpellWindow(); 
VOID DestroySpellWindow(); 

int iMaxResults=10; 
CHAR szSearchString[MAX_STRING]={0}; 
BOOL bSearchFlag=false; 
BOOL bUseWnd=true; 

class CSpellInfoWnd : public CCustomWnd 
{ 
public: 
	CStmlWnd *OutputBox; 
	CSpellInfoWnd():CCustomWnd("ChatWindow") 
	{ 
		OutputBox=(CStmlWnd*)GetChildItem("CWChatOutput"); 
		SetWndNotification(CSpellInfoWnd); 
		dShow = 0; 
	} 

	~CSpellInfoWnd() 
	{ 
	} 

	int WndNotification(CXWnd *pWnd, unsigned int Message, void *unknown) 
	{ 
		CHAR szTemp[MAX_STRING]={0}, szBuffer[MAX_STRING]={0}; 

		if (pWnd==0)
		{
			if (Message==XWM_CLOSE)
			{
				CreateSpellWindow(); 
				((CXWnd*)this)->Show(1,1);
				dShow=1;
				return 1;
			}
		}
		return CSidlScreenWnd::WndNotification(pWnd,Message,unknown); 
	}; 
}; 
CSpellInfoWnd *MyWnd=0; 

// *************************************************************************** 
// Function:    spellout 
// Description: format and pipe MQ formatted text to the corrent places 
// Usage:       pass MQ formatted text (WITHOUT endline!) and it will be put 
//              to the display 
// *************************************************************************** 
void spellout(char *szMsg) { 
	try {
		if(bUseWnd) { 
			if (!(PCSIDLWND)MyWnd->dShow) {
				CreateSpellWindow();
				((CXWnd*)MyWnd)->Show(1,1);
			}
			if (MyWnd) {
				CHAR szProcessed[MAX_STRING]; 
				MQToSTML(szMsg,szProcessed,MAX_STRING,0xFFFFFFF); 
				strcat_s(szProcessed,"<br>"); 
				CXStr NewText(szProcessed); 
				DebugTry( MyWnd->OutputBox->AppendSTML(NewText) ); 
				DebugTry( ((CXWnd*)MyWnd->OutputBox)->SetVScrollPos(MyWnd->OutputBox->VScrollMax) ); 
			}
			else
				WriteChatColor("[MQ2SpellSearch::spellout] Window problem, please submit report.", USERCOLOR_DEFAULT);
		}
		else { 
			WriteChatColor(szMsg,USERCOLOR_DEFAULT); 
		} 
	}
	catch(...) {
#ifndef SUPPRESS_ERRORS
		WriteChatColor("[MQ2SpellSearch::spellout] Caught Terminal Error: attempting to continue as normal.", USERCOLOR_DEFAULT);
#endif
	}
	return; 
} 


// *************************************************************************** 
// Function:    SpellSearchCommand 
// Description: Our '/spellsearch' command 
//              Starts a FindSpellThread to look for matches to supplied args 
// Usage:       /spellsearch spell name      quotations not needed 
//              /spellsearch castonyou or castonother string to be matched 
// *************************************************************************** 
VOID SpellSearchCommand(PSPAWNINFO pChar, PCHAR szLine) 
{ 
	DebugSpewAlways("MQ2SpellSearch::SpellSearchCommand()"); 
	//no command args specified 
	if (szLine[0]==0) { 
		spellout("/spellsearch: use /spellsearch nameofspell"); 
		return; 
	} 

	//check to see if a thread is running (multiple are possbile, but own 
	//your pulse updates) 
	if ( !bSearchFlag ) { 
		DWORD ThreadId; 
		strcpy_s(szSearchString,szLine); 
		DebugTry( CreateThread(NULL,0,&FindSpellThread,0,0,&ThreadId) ); 
	} else { 
		spellout("/spellsearch: ERROR search still in progress"); 
	} 
} 


// *************************************************************************** 
// Function:    FindSpellThread 
// Description: Our spell finding function, FindSpellThread will try to match 
//              args to spell names, castonyou and cast on other string pointers 
// Usage:       set szSearchString to string to search for 
//              and invoke a CreateThread see: SpellSearchCommand() example 
// *************************************************************************** 
DWORD WINAPI FindSpellThread(LPVOID lpParam) 
{ 
	//although most systems can handle flooding the window with TONS of matches, i advise against it: 
	DebugSpewAlways("MQ2SpellSearch::FindSpellThread - Start (%s)",szSearchString); 
	if ( strlen(szSearchString) < 3 ) { 
		spellout("/spellsearch: search text must be at least 3 characters long"); 
		return 0; 
	} 

	CHAR szTemp[MAX_STRING]={0}; 
	PSPELL pSpell; 
	CHAR szSpellName[MAX_STRING]={0}; 
	CHAR szSpellCastOnYou[MAX_STRING]={0}; 
	CHAR szSpellCastOnOther[MAX_STRING]={0}; 
	PCHARINFO pCharInfo = NULL; 

	//make sure we are in a gamestate where we can access the spell array... 
	if (NULL == (pCharInfo = GetCharInfo())) return 0; 
	int FoundCount=0; 

	// find match 
	_strlwr_s(szSearchString); 
	DebugSpewAlways("MQ2SpellSearch::FindSpellThread(): Searching for spell match.."); 

	//make sure the plugin /spellsearch cmd knows we are running this intensive thread.. 
	bSearchFlag=true; 
	for (DWORD i=0; i < TOTAL_SPELL_COUNT; i++) { 
		pSpell = &(*((PSPELLMGR)pSpellMgr)->Spells[i]); 
		if ((pSpell->ID > 0) && (pSpell->ID < TOTAL_SPELL_COUNT)) 
			if (pSpell->Name != NULL) 
			{ 
				strcpy_s(szSpellName, pSpell->Name); 
				_strlwr_s(szSpellName);
				strcpy_s(szSpellCastOnYou, GetSpellString(pSpell->ID,2)); 
				_strlwr_s(szSpellCastOnYou); 
				strcpy_s(szSpellCastOnOther, GetSpellString(pSpell->ID,3)); 
				_strlwr_s(szSpellCastOnOther); 
				if ( strstr(szSpellName, szSearchString) || 
					strstr(szSpellCastOnYou, szSearchString) || 
					strstr(szSpellCastOnOther, szSearchString) ) 
				{ 
					FoundCount++; 
					ShowSpellInfo(pSpell); 
				} 
			} 
			if (FoundCount >= iMaxResults) { 
				sprintf_s(szTemp,"/spellsearch: Your search yielded more than %d results, stopping here.", iMaxResults); 
				spellout(szTemp); 
				break; 
			} 
	} 
	DebugSpewAlways("MQ2SpellSearch::FindSpellThread(): Search end"); 
	bSearchFlag=false; 

	// report if never found anything 
	if ( FoundCount==0 ) { 
		sprintf_s(szTemp,"/spellsearch: no match for: %s",szSearchString); 
		spellout(szTemp); 
	} 
	return 0; 
} 


// *************************************************************************** 
// Function:    ShowSpellInfo 
// Description: Go through spell data and show reference information to the user 
// Usage:       call with a valid spell pointer and the spell's information will be piped 
//              to this custom window or normal MQ2Chat output depending on settings 
// *************************************************************************** 
VOID ShowSpellInfo(PSPELL pSpell) 
{ 
	DebugSpewAlways("MQ2SpellSearch::ShowSpellInfo(): Start"); 
	if (pSpell == NULL || pSpell->Name == NULL) { 
		spellout("ShowSpellInfo(): pSpell - Bad pointer"); 
		return; 
	} 
	CHAR szTemp[MAX_STRING]={0}; 
	CHAR szBuff[MAX_STRING]={0}; 

	sprintf_s(szBuff,"-----Spell:\at%s\ax Range:\ay%.1f\ax CastTime:\ao%.2f\ax Mana:\au%d\ax", 
		pSpell->Name, pSpell->Range, ((float)pSpell->CastTime/1000), pSpell->ManaCost); 
	//show recast if its longer than normal cast refresh 
	float fRecast = ((float)pSpell->RecastTime)/1000; 
	if ( fRecast > (((float)pSpell->CastTime/1000) + 2.5) ) 
	{ 
		sprintf_s(szTemp, " \awRecast:\ao%.2f", fRecast); 
		strcat_s(szBuff, szTemp); 
	} 

	spellout(szBuff); 

	//call slot interpreter to explain each slot effect 
	szBuff[0] = 0; 

	ShowSpellSlotInfo(pSpell, szBuff,MAX_STRING); 

	//break down each slot line for cchatwnd since <br>'s arent 
	//gonna get processed in the chatwnd 
	PCHAR p = &szBuff[0]; 
	int slen = strlen(szBuff); 
	for(int i=0; i<slen-4; i++) 
	{ 
		if ( !_strnicmp( &szBuff[i], "<br>", 4) ) { 
			szBuff[i] = 0; 
			spellout(p); 
			p = &szBuff[i+4]; 
		} 
	} 

	//show usable classes 
	sprintf_s(szBuff,"Classes:\at"); 
	bool bFoundClass=false; 
	for (int j=0; j<=16; j++) 
	{ 
		if (pSpell->ClassLevel[j]>0 && pSpell->ClassLevel[j]<=65) 
		{ 
			sprintf_s(szTemp,"%s\ax/\at%d ", pEverQuest->GetClassDesc(j+1), pSpell->ClassLevel[j]); 
			strcat_s(szBuff, szTemp); 
			bFoundClass=true; 
		} 
	} 
	if ( !bFoundClass )  strcat_s(szBuff, "None"); 
	//add pushback on this line if its nonzero 
	if ( pSpell->PushBack > 0 ) 
	{ 
		sprintf_s(szTemp, "    \axPushBack: \ay%.1f", pSpell->PushBack); 
		strcat_s(szBuff, szTemp); 
	} 
	//add duration if its defined 
	if ( pSpell->DurationCap > 0 ) 
	{ 
		sprintf_s(szTemp, "       \axDuration: \ay%.1fmins", (float)pSpell->DurationCap/10); 
		strcat_s(szBuff, szTemp); 
	} 
	spellout(szBuff); 

	szBuff[0]=0;  //this out line has several optional elements, lets make it blank first since all are options 
	//resist check 
	//only report resist info if its a detrimental spell 
	if ( pSpell->SpellType == 0 ) { 
		strcat_s(szBuff,"Resist Check:\at"); 
		switch(pSpell->Resist) 
		{ 
		case 0: 
			strcat_s(szBuff, "Unresistable"); 
			break; 
		case 1: 
			strcat_s(szBuff, "MR"); 
			break; 
		case 2: 
			strcat_s(szBuff, "FR"); 
			break; 
		case 3: 
			strcat_s(szBuff, "CR"); 
			break; 
		case 4: 
			strcat_s(szBuff, "PR"); 
			break; 
		case 5: 
			strcat_s(szBuff, "DR"); 
			break; 
		case 6: 
			strcat_s(szBuff, "Chromatic"); 
			break; 
		case 7: 
			strcat_s(szBuff, "Prismatic"); 
			break; 
		default: 
			sprintf_s(szTemp, "Unknown%03d", pSpell->Resist); 
			strcat_s(szBuff, szTemp); 
			break; 
		} 
		strcat_s(szBuff, "  "); 
		//show resist adjust if it is defined 
		if ( pSpell->ResistAdj ) 
		{ 
			sprintf_s(szTemp, "\ay%dadj  ", pSpell->ResistAdj); 
			strcat_s(szBuff, szTemp); 
		} 
	} 

	// targettype 
	strcat_s(szBuff, "\awTargetType:\at"); 
	GetTargetType(pSpell->TargetType, szTemp);   strcat_s(szBuff, szTemp); 
	//add aerange on line if its nonzero 
	if ( pSpell->AERange > 0 ) 
	{ 
		sprintf_s(szTemp, "  \axAERange: \ay%.1f", pSpell->AERange); 
		strcat_s(szBuff, szTemp); 
	} 
	//spelltype 
	strcat_s(szBuff,"  \awSpellType:\at"); 
	switch(pSpell->SpellType) 
	{ 
	case 0: 
		strcat_s(szBuff, "Detrimental"); 
		break; 
	case 1: 
		strcat_s(szBuff, "Beneficial"); 
		break; 
	case 2: 
		strcat_s(szBuff, "Beneficial - Group Only"); 
		break; 
	default: 
		sprintf_s(szTemp, "Unknown%03d", pSpell->SpellType); 
		strcat_s(szBuff, szTemp); 
		break; 
	} 
	//show uninteruptable flag if enabled 
	if ( pSpell->Uninterruptable ) { 
		strcat_s(szBuff, "  \ayUNINTERRUPTABLE"); 
	} 
	spellout(szBuff); 

	//show cast messages if they are defined for the spell 
	if (char*str = GetSpellString(pSpell->ID,2))
	{
		/*CastByMe,CastByOther,CastOnYou,CastOnAnother,WearOff*/
		if (str[0]) {
			sprintf_s(szBuff, "CastOnYou:\at%s", str);
			spellout(szBuff);
		}
	} 
	if (char*str = GetSpellString(pSpell->ID,3)) 
	{ 
		if (str[0]) {
			sprintf_s(szBuff, "CastOnOther:\atAradune%s", str);
			spellout(szBuff);
		}
	} 

	//show autocast if it is specified 
	if ( pSpell->Autocast ) 
	{ 
		sprintf_s(szBuff, "\awAutocast:\au%s", GetSpellByID(pSpell->Autocast)->Name); 
		spellout(szBuff); 
	} 

	//zone location limitations  01=Outdoors, 02=dungeons, ff=Any 
	if ( pSpell->ZoneType!=0xff ) 
	{ 
		sprintf_s(szBuff, "\awLocation Type:\au"); 
		switch(pSpell->ZoneType) 
		{ 
		case 1: 
			sprintf_s(szTemp, "Outdoor Only"); 
			break; 
		case 2: 
			sprintf_s(szTemp, "Dungeon Only"); 
			break; 
		default: 
			sprintf_s(szTemp, "Unknown%03d (not any) location", pSpell->ZoneType); 
			break; 
		} 
		strcat_s(szBuff, szTemp); 
		spellout(szBuff); 
	} 

	//time of day limitations 
	if ( pSpell->TimeOfDay ) 
	{ 
		sprintf_s(szBuff, "\awTime Of Day:\au"); 
		switch(pSpell->TimeOfDay) 
		{ 
		case 1: 
			break; 
			sprintf_s(szTemp, "Day Only"); 
		case 2: 
			sprintf_s(szTemp, "Night Only"); 
			break; 
		default: 
			sprintf_s(szTemp, "Unknown%03d (not any) timeofday", pSpell->TimeOfDay); 
			break; 
		} 
		strcat_s(szBuff, szTemp); 
		spellout(szBuff); 
	} 
} 





// *************************************************************************** 
// Function:    GetEffectName 
// Description: Return spell effect string 
// Usage:       Used by ShowSpellSlot to list limited effect types 
// *************************************************************************** 
PCHAR GetTargetType(DWORD TargetID, PCHAR szBuffer) 
{ 
	CHAR szBuff[MAX_STRING]={0}; 
	CHAR szTemp[MAX_STRING]={0}; 
	switch(abs((long)TargetID)) 
	{ 
	case 0: 
		strcat_s(szBuff, "Rag`Zhezum Special"); 
		break; 
	case 1: 
		strcat_s(szBuff, "Line of Sight"); 
		break; 
	case 2: 
		strcat_s(szBuff, "AE PC v1"); 
		break; 
	case 3: 
		strcat_s(szBuff, "Group v1"); 
		break; 
	case 4: 
		strcat_s(szBuff, "PBAE"); 
		break; 
	case 5: 
		strcat_s(szBuff, "Single"); 
		break; 
	case 6: 
		strcat_s(szBuff, "Self"); 
		break; 
	case 8: 
		strcat_s(szBuff, "Targeted AE"); 
		break; 
	case 9: 
		strcat_s(szBuff, "Animal"); 
		break; 
	case 10: 
		strcat_s(szBuff, "Undead"); 
		break; 
	case 11: 
		strcat_s(szBuff, "Summoned"); 
		break; 
	case 13: 
		strcat_s(szBuff, "Lifetap"); 
		break; 
	case 14: 
		strcat_s(szBuff, "Pet"); 
		break; 
	case 15: 
		strcat_s(szBuff, "Corpse"); 
		break; 
	case 16: 
		strcat_s(szBuff, "Plant"); 
		break; 
	case 17: 
		strcat_s(szBuff, "Uber Giants"); 
		break; 
	case 18: 
		strcat_s(szBuff, "Uber Dragons"); 
		break; 
	case 20: 
		strcat_s(szBuff, "Targeted AE, Tap"); 
		break; 
	case 24: 
		strcat_s(szBuff, "AE Undead"); 
		break; 
	case 25: 
		strcat_s(szBuff, "AE Summoned"); 
		break; 
	case 40: 
		strcat_s(szBuff, "AE PC v2"); 
		break; 
	case 41: 
		strcat_s(szBuff, "Group v2"); 
		break; 
	default: 
		sprintf_s(szTemp, "UndefinedTarget%03d", abs((long)TargetID)); 
		strcat_s(szBuff, szTemp); 
		break; 
	} 
	strcpy_s(szBuffer,MAX_STRING, szBuff); 
	return szBuffer; 
} 


// *************************************************************************** 
// Function:    LoadChatFromINI, Save ChatToINI 
// Description: Plugin uses these routines (from Lax's MQ2ChatWnd) to save chatbox settings. 
// Usage:       Used by plugin constructors after defining the window class 
//              to save/load user settings by passing the window element pointer to the functons 
// *************************************************************************** 
CHAR szChatINISection[MAX_STRING] = {0}; 
void LoadChatFromINI(PCSIDLWND pWindow) 
{ 
	DebugSpewAlways("MQ2SpellSearch::LoadChatFromINI()"); 
	CHAR Buffer[MAX_STRING] = {0}; 
	sprintf_s(szChatINISection,"%s.%s",EQADDR_SERVERNAME,((PCHARINFO)pCharData)->Name); 
	pWindow->Location.top      = GetPrivateProfileInt(szChatINISection,"ChatTop",    10,INIFileName); 
	pWindow->Location.bottom   = GetPrivateProfileInt(szChatINISection,"ChatBottom",210,INIFileName); 
	pWindow->Location.left      = GetPrivateProfileInt(szChatINISection,"ChatLeft",   10,INIFileName); 
	pWindow->Location.right    = GetPrivateProfileInt(szChatINISection,"ChatRight", 410,INIFileName); 
	pWindow->Locked             = (GetPrivateProfileInt(szChatINISection,"Locked",      0,INIFileName) ? true:false); 
	pWindow->Fades             = (GetPrivateProfileInt(szChatINISection,"Fades",      1,INIFileName) ? true:false); 
	pWindow->FadeDelay         = GetPrivateProfileInt(szChatINISection,"Delay",    2000,INIFileName); 
	pWindow->FadeDuration       = GetPrivateProfileInt(szChatINISection,"Duration",      500,INIFileName); 
	pWindow->Alpha            = GetPrivateProfileInt(szChatINISection,"Alpha",      255,INIFileName); 
	pWindow->FadeToAlpha      = GetPrivateProfileInt(szChatINISection,"FadeToAlpha",      255,INIFileName); 
	pWindow->BGType            = GetPrivateProfileInt(szChatINISection,"BGType",      1,INIFileName); 
	ARGBCOLOR col = { 0 };
	col.ARGB = pWindow->BGColor;
	col.A = GetPrivateProfileInt(szChatINISection, "BGTint.alpha", 255, INIFileName);
	col.R = GetPrivateProfileInt(szChatINISection, "BGTint.red", 0, INIFileName);
	col.G =  GetPrivateProfileInt(szChatINISection, "BGTint.green", 0, INIFileName);
	col.B = GetPrivateProfileInt(szChatINISection, "BGTint.blue", 0, INIFileName); 
	GetPrivateProfileString(szChatINISection,"WindowTitle","SpellSearch",Buffer,MAX_STRING,INIFileName); 
	SetCXStr(&pWindow->WindowText,Buffer); 
} 
template <unsigned int _Size>LPSTR SafeItoa(int _Value, char(&_Buffer)[_Size], int _Radix)
{
	errno_t err = _itoa_s(_Value, _Buffer, _Radix);
	if (!err) {
		return _Buffer;
	}
	return "";
}
void SaveChatToINI(PCSIDLWND pWindow) 
{ 
	DebugSpewAlways("MQ2SpellSearch::SaveChatToINI()"); 
	CHAR szTemp[MAX_STRING] = {0}; 
	if (pWindow->Minimized) 
	{ 
		WritePrivateProfileString(szChatINISection,"ChatTop",   SafeItoa(pWindow->OldLocation.top,      szTemp,10),INIFileName); 
		WritePrivateProfileString(szChatINISection,"ChatBottom",SafeItoa(pWindow->OldLocation.bottom,   szTemp,10),INIFileName); 
		WritePrivateProfileString(szChatINISection,"ChatLeft",   SafeItoa(pWindow->OldLocation.left,      szTemp,10),INIFileName); 
		WritePrivateProfileString(szChatINISection,"ChatRight",   SafeItoa(pWindow->OldLocation.right,   szTemp,10),INIFileName); 
	} 
	else 
	{ 
		WritePrivateProfileString(szChatINISection,"ChatTop",   SafeItoa(pWindow->Location.top,      szTemp,10),INIFileName); 
		WritePrivateProfileString(szChatINISection,"ChatBottom",SafeItoa(pWindow->Location.bottom,   szTemp,10),INIFileName); 
		WritePrivateProfileString(szChatINISection,"ChatLeft",   SafeItoa(pWindow->Location.left,   szTemp,10),INIFileName); 
		WritePrivateProfileString(szChatINISection,"ChatRight",   SafeItoa(pWindow->Location.right,   szTemp,10),INIFileName); 
	} 
	WritePrivateProfileString(szChatINISection,"Locked",   SafeItoa(pWindow->Locked,         szTemp,10),INIFileName); 

	GetCXStr(pWindow->WindowText,szTemp); 
	WritePrivateProfileString(szChatINISection,"WindowTitle",   szTemp              ,INIFileName); 

	WritePrivateProfileString(szChatINISection,"Fades",   SafeItoa(pWindow->Fades,         szTemp,10),INIFileName); 
	WritePrivateProfileString(szChatINISection,"Delay",   SafeItoa(pWindow->FadeDelay,         szTemp,10),INIFileName); 
	WritePrivateProfileString(szChatINISection,"Duration",   SafeItoa(pWindow->FadeDuration,         szTemp,10),INIFileName); 
	WritePrivateProfileString(szChatINISection,"Alpha",   SafeItoa(pWindow->Alpha,         szTemp,10),INIFileName); 
	WritePrivateProfileString(szChatINISection,"FadeToAlpha",   SafeItoa(pWindow->FadeToAlpha,         szTemp,10),INIFileName); 
	WritePrivateProfileString(szChatINISection,"BGType",   SafeItoa(pWindow->BGType,         szTemp,10),INIFileName); 
	ARGBCOLOR col = { 0 };
	col.ARGB = pWindow->BGColor;
	WritePrivateProfileString(szChatINISection, "BGTint.alpha", SafeItoa(col.A, szTemp, 10), INIFileName);
	WritePrivateProfileString(szChatINISection, "BGTint.red", SafeItoa(col.R, szTemp, 10), INIFileName);
	WritePrivateProfileString(szChatINISection, "BGTint.green", SafeItoa(col.G, szTemp, 10), INIFileName);
	WritePrivateProfileString(szChatINISection, "BGTint.blue", SafeItoa(col.B, szTemp, 10), INIFileName);
} 

// ********************************************** 
// Plugin interaction with main module 
// ********************************************** 

// Called once, when the plugin is to initialize 
PLUGIN_API VOID InitializePlugin(VOID) 
{ 
	DebugSpewAlways("Initializing MQ2SpellSearch"); 
	CHAR szTemp[MAX_STRING]={0}; 

	GetPrivateProfileString("Settings","UseWnd","1",szTemp,MAX_STRING,INIFileName); bUseWnd = (BOOL)strtol(szTemp,NULL,10); 
	GetPrivateProfileString("Settings","MaxResults","15",szTemp,MAX_STRING,INIFileName); iMaxResults = (int)strtol(szTemp,NULL,10); 

	AddCommand("/spellsearch",SpellSearchCommand,0,1); 
	if (gGameState==GAMESTATE_INGAME && !MyWnd) 
	{ 
		if(bUseWnd) { 
			CreateSpellWindow(); 
		} 
	} 
} 

// Called once, when the plugin is to shutdown 
PLUGIN_API VOID ShutdownPlugin(VOID) 
{ 
	DebugSpewAlways("Shutting down MQ2SpellSearch"); 
	RemoveCommand("/spellsearch"); 
} 

PLUGIN_API VOID OnCleanUI(VOID) 
{ 
	DebugSpewAlways("MQ2SpellSearch::OnCleanUI()"); 
	DestroySpellWindow(); 
} 

PLUGIN_API VOID SetGameState(DWORD GameState) 
{ 
	DebugSpewAlways("MQ2SpellSearch::SetGameState()"); 
	if (GameState==GAMESTATE_INGAME && !MyWnd) 
	{ 
		if(bUseWnd) { 
			CreateSpellWindow(); 
		} 
	} 
} 

PLUGIN_API VOID OnReloadUI() 
{ 
	DebugSpewAlways("MQ2SpellSearch::OnReloadUI()"); 
	CreateSpellWindow(); 
} 

void CreateSpellWindow() 
{ 
	DebugSpewAlways("MQ2SpellSearch::CreateSpellWindow()"); 
	if (MyWnd)  return; 

	MyWnd = new CSpellInfoWnd(); 

	LoadChatFromINI((PCSIDLWND)MyWnd); 
	SaveChatToINI((PCSIDLWND)MyWnd); 
} 

void DestroySpellWindow() 
{ 
	DebugSpewAlways("MQ2SpellSearch::DestroySpellWindow()"); 
	if (MyWnd) 
	{ 
		SaveChatToINI((PCSIDLWND)MyWnd); 
		delete MyWnd; 
		MyWnd=0; 
	} 
}
