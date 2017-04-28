// MQ2NoNagWindows.cpp :: RedGuides exclusive
// Closes annoying nag windows, also keeps browser upgrade window from opening on logout
// v1.0 - Sym - Initial release
// v1.1 - Sym - Added browser blocking code
// v1.1 - Sym - Added autodetect of offset
// v1.11 - Sym - 08-05-2012 - Site name update

#include "../MQ2Plugin.h"

PreSetup("MQ2NoNagWindows");
PLUGIN_VERSION(1.11);
#pragma warning(disable:4800)

#define SKIP_PULSES 40

long SkipPulse = 0;
bool WindowOpen(PCHAR WindowName);
bool CheckWindowValue(PCHAR szToCheck);
char szTemp[MAX_STRING];

DWORD dwAddress=0;
char  mask[]="xxxxxxxxxx";
PBYTE pattern=(PBYTE)"\x51\x56\x8D\x44\x24\x04\x50\x68\x3F\x00";

// credit: radioactiveman/bunny771 ----------------------------------------
bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
    for(;*szMask;++szMask,++pData,++bMask)
        if(*szMask=='x' && *pData!=*bMask )
            return false;
    return (*szMask) == NULL;
}

DWORD dwFindPattern(DWORD dwAddress,DWORD dwLen,BYTE *bMask,char * szMask)
{
	for(DWORD i=0; i < dwLen; i++)
		if( bDataCompare( (BYTE*)( dwAddress+i ),bMask,szMask) )
			return (DWORD)(dwAddress+i);

	return 0;
}
// ------------------------------------------------------------------------

class a
{
public:
	bool b();
	bool c() {
		return false;
	}
};

DETOUR_TRAMPOLINE_EMPTY(bool a::b(void));


// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	DebugSpewAlways("Initializing MQ2NoNagWindows");
	WriteChatf("\atMQ2NoNagWindows :: v%1.2f :: by Sym for RedGuides.com\ax", MQ2Version);
	dwAddress=dwFindPattern(FixOffset(0x400000),0x200000,pattern,mask);
	if(dwAddress) {
		EzDetour(dwAddress,&a::c,&a::b);
	} else {
		WriteChatf("\arMQ2NoNagWindows :: Couldn't find browser offset.\ax");
	}
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	DebugSpewAlways("Shutting down MQ2NoNagWindows");
	if(dwAddress) {
		RemoveDetour(dwAddress);
	}
}


// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
   if (GetGameState() != GAMESTATE_INGAME) return;

   if (SkipPulse == SKIP_PULSES) {
      SkipPulse = 0;
      if(WindowOpen("AlertWnd") && CheckWindowValue("${Window[AlertWnd].Child[ALW_Action_Button].Text.Find[Upgrade]}")) {
          WriteChatf("\agMQ2NoNagWindows :: Closing Upgrade Window\ax");
          DoCommand(GetCharInfo()->pSpawn,"/squelch /nomodkey /notify AlertWnd ALW_Dismiss_Button leftmouseup");          
      }
      if(WindowOpen("HtmlWnd") && CheckWindowValue("${Window[HtmlWnd].Text.Find[Welcome to EverQuest]}")) {
          WriteChatf("\agMQ2NoNagWindows :: Closing HTML Window\ax");
          DoCommand(GetCharInfo()->pSpawn,"/squelch /nomodkey /windowstate HtmlWnd close");       
      }
      if(WindowOpen("LargeDialogWindow") && CheckWindowValue("${Window[LargeDialogWindow].Text.Find[The Hero's Journey]}")) {
          WriteChatf("\agMQ2NoNagWindows :: Closing Hero's Journey Window\ax");
          DoCommand(GetCharInfo()->pSpawn,"/squelch /nomodkey /notify LargeDialogWindow LDW_OkButton leftmouseup");
      }
   }
   SkipPulse++;    
}

bool CheckWindowValue(PCHAR szToCheck) {
    sprintf_s(szTemp,"%s",szToCheck);
	ParseMacroData(szTemp, sizeof(szTemp));
    //DebugSpew("%s :: %s", szToCheck, szTemp);
    if (strcmp(szTemp,"TRUE") == 0) return true;
    else if (atoi(szTemp) > 0 ) return true;
    else return false;
}


bool WindowOpen(PCHAR WindowName) {
  PCSIDLWND pWnd=(PCSIDLWND)FindMQ2Window(WindowName);
  return (!pWnd)?false:(BOOL)pWnd->dShow;
}