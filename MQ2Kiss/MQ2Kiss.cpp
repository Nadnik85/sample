// MQ2Kiss.cpp : Single-window MQ Chat 
// 
//#define DEBUG_TRY 1 
#define MAX_CHAT_SIZE 700 
#define LINES_PER_FRAME 3 
#include "../MQ2Plugin.h" 
#include <vector>
PreSetup("MQ2Kiss"); 

struct ChatBuffer 
{ 
    CHAR Text[MAX_STRING]; 
    ChatBuffer *pNext; 
    ChatBuffer *pPrev; 
}; 

ChatBuffer* pPendingChat=0; 
ChatBuffer* pPendingChatTail=0; 

const unsigned int CMD_HIST_MAX=50;
DWORD ulOldVScrollPos=0; 
DWORD bmStripFirstStmlLines=0; 
DWORD PendingChatLines=0; 
CHAR szChatINISection[MAX_STRING]={0}; 
bool bAutoScroll=false; 
bool bNoCharSelect=false; 
bool bSaveByChar=true; 

char Directory[MAX_STRING];
char FileName[MAX_STRING];
char SectionName[MAX_STRING];

class CMQKissWnd; 
CMQKissWnd *MQKissWnd=0; 
class CMQKissWnd : public CCustomWnd 
{ 
public:
    CTextEntryWnd* InputBox;
    CStmlWnd* OutputBox;
    CXWnd* OutWnd;
    struct _CSIDLWND* OutStruct;
private:
    vector<string> sCmdHistory;
    int iCurrentCmd;
public: 
    CMQKissWnd(CXStr *Template):CCustomWnd(Template) 
    { 
        DebugSpew("CMQKissWnd()");
        SetWndNotification(CMQKissWnd); 
        InputBox=(CTextEntryWnd*)GetChildItem("CWChatInput"); 
        InputBox->WindowStyle|=0x800C0; 
        BitOff(WindowStyle,CWS_CLOSE); 
        InputBox->UnknownCW|=0xFFFFFFFF; 
        InputBox->SetMaxChars(512); 
        OutputBox=(CStmlWnd*)GetChildItem("CWChatOutput"); 
        OutStruct=(_CSIDLWND*)GetChildItem("CWChatOutput");
        OutWnd=(CXWnd*)OutputBox;
        OutBoxLines=0; 
        *(DWORD*)&(((PCHAR)OutputBox)[EQ_CHAT_HISTORY_OFFSET])=0x190; 
        OutputBox->Clickable=1; 
        iCurrentCmd=-1;
    } 
    ~CMQKissWnd() 
    { 
    } 
    int WndNotification(CXWnd *pWnd, unsigned int Message, void *kdata) 
    {    
        if (pWnd==(CXWnd*)InputBox) 
        { 
            if (Message==XWM_HITENTER) 
            {  
                char szBuffer[2048]; 
                GetCXStr((PCXSTR)InputBox->InputText,szBuffer,2047); 
                if (szBuffer[0]) 
                { 
                    if (!sCmdHistory.size() || sCmdHistory.front().compare(szBuffer))
                    {
                        if (sCmdHistory.size()>CMD_HIST_MAX)
                        {
                            sCmdHistory.pop_back();
                        }
                        sCmdHistory.insert(sCmdHistory.begin(),string(szBuffer));
                    }
                    iCurrentCmd=-1;

                    SetCXStr(&InputBox->InputText,"");
                    if (szBuffer[0]=='/')
                    {
                        DoCommand(((PCHARINFO)pCharData)->pSpawn,szBuffer); 
                    }
                    else
                    {
                        Echo(((PCHARINFO)pCharData)->pSpawn,szBuffer); 
                    }
                } 
                ((CXWnd*)InputBox)->ClrFocus(); 
            } 
            else if (Message == XWM_HISTORY)
            {
                if (kdata)
                {
                    int* pInt=(int*)kdata;
                    int iKeyPress=pInt[1];
                    if (iKeyPress==0xc8) // KeyUp
                    {
                        if (sCmdHistory.size()>0)
                        {
                            iCurrentCmd++;
                            if (iCurrentCmd<((int)sCmdHistory.size()) && iCurrentCmd>=0)
                            {
                                string s=(string)sCmdHistory.at(iCurrentCmd);
                                ((CXWnd*)InputBox)->SetWindowTextA(CXStr(s.c_str()));
                            }
                            else
                            {
                                iCurrentCmd=((int)sCmdHistory.size())-1;
                            }
                        }
                    }
                    else if (iKeyPress==0xd0) // KeyDown
                    {
                        if (sCmdHistory.size()>0)
                        {
                            iCurrentCmd--;
                            if (iCurrentCmd>=0 && sCmdHistory.size()>0)
                            {
                                string s=(string)sCmdHistory.at(iCurrentCmd);
                                ((CXWnd*)InputBox)->SetWindowTextA(CXStr(s.c_str()));
                            }
                            else if (iCurrentCmd<0)
                            {
                                iCurrentCmd=-1;
                                SetCXStr(&InputBox->InputText,"");
                            }
                        }
                    }
                }
            }
            else 
            { 
                DebugSpew("InputBox message %Xh, value: %Xh",Message,kdata); 
            } 
        } 
        else if (pWnd==0) 
        { 
            if (Message==XWM_CLOSE) 
            { 
                dShow=1; 
                return 1; 
            } 
        } 
        else if (Message==XWM_LINK) 
        { 
            class CChatWindow *p = (class CChatWindow *)this; 
            if (OutputBox!=(CStmlWnd *)pWnd) 
            { 
                CStmlWnd *tmp; 
                int ret; 
                tmp=OutputBox; 
                OutputBox=(CStmlWnd*)pWnd; 
                ret=p->WndNotification(pWnd,Message,kdata); 
                OutputBox=tmp; 
                return ret; 
            } 
            return p->WndNotification(pWnd,Message,kdata); 
        } 
        else 
        { 
            //DebugSpew("MQ2Kiss: 0x%X, Msg: 0x%X, value: %Xh",pWnd,Message,kdata); 
        } 
        return CSidlScreenWnd::WndNotification(pWnd,Message,kdata); 
    }; 

    void SetChatFont(int size) // brainiac 12-12-2007 
    { 
        struct FONTDATA 
        { 
            DWORD NumFonts; 
            PCHAR* Fonts; 
        }; 
        FONTDATA* Fonts;            // font array structure 
        DWORD* SelFont;             // selected font 

        // get fonts structure -- this offset can be found by looking at 
        // SetChatfont which is called from the /chatfontsize function 
        Fonts = (FONTDATA*)&(((char*)pWndMgr)[EQ_CHAT_FONT_OFFSET]); 

        // check font array bounds and pointers 
        if (size<0 || size>=(int)Fonts->NumFonts) 
        { 
            return; 
        } 
        if (!Fonts->Fonts || !MQKissWnd) 
        { 
            return; 
        } 
        //DebugSpew("Setting Size: %i", size); 

        SelFont = (DWORD*)Fonts->Fonts[size]; 

        // Save the text, change the font, then restore the text 
        CXStr str(((CStmlWnd*)MQKissWnd->OutputBox)->GetSTMLText()); 
        ((CXWnd*)MQKissWnd->OutputBox)->SetFont(SelFont); 
        ((CStmlWnd*)MQKissWnd->OutputBox)->SetSTMLText(str,1,0); 
        ((CStmlWnd*)MQKissWnd->OutputBox)->ForceParseNow(); 
        // scroll to bottom of chat window 
        DebugTry(((CXWnd*)MQKissWnd->OutputBox)->SetVScrollPos(MQKissWnd->OutputBox->VScrollMax)); 

        MQKissWnd->FontSize=size; 
    }; 

    DWORD OutBoxLines; 
    DWORD FontSize; 
}; 

VOID LoadChatSettings() 
{ 
    CHAR szTemp[MAX_STRING]={0}; 
    GetPrivateProfileString("Settings","AutoScroll",bAutoScroll?"on":"off",szTemp,MAX_STRING,INIFileName); 
    bAutoScroll=(!strnicmp(szTemp,"on",3)); 
    GetPrivateProfileString("Settings","NoCharSelect",bNoCharSelect?"on":"off",szTemp,MAX_STRING,INIFileName); 
    bNoCharSelect=(!strnicmp(szTemp,"on",3)); 
    GetPrivateProfileString("Settings", "SaveByChar",bSaveByChar?"on":"off",szTemp,MAX_STRING,INIFileName); 
    bSaveByChar=(!strnicmp(szTemp,"on",3)); 
} 

VOID LoadChatFromINI(PCSIDLWND pWindow) 
{ 
    CHAR szTemp[MAX_STRING]={0}; 

    LoadChatSettings(); 

    sprintf(szChatINISection,"%s.%s",EQADDR_SERVERNAME,((PCHARINFO)pCharData)->Name); 
    if (!bSaveByChar) sprintf(szChatINISection,"Default"); 

    pWindow->Location.top      = GetPrivateProfileInt(szChatINISection,"ChatTop",       10,INIFileName); 
    pWindow->Location.bottom   = GetPrivateProfileInt(szChatINISection,"ChatBottom",   210,INIFileName); 
    pWindow->Location.left     = GetPrivateProfileInt(szChatINISection,"ChatLeft",      10,INIFileName); 
    pWindow->Location.right    = GetPrivateProfileInt(szChatINISection,"ChatRight",    410,INIFileName); 
    pWindow->Locked            = GetPrivateProfileInt(szChatINISection,"Locked",         0,INIFileName); 
    pWindow->Fades             = GetPrivateProfileInt(szChatINISection,"Fades",          1,INIFileName); 
    pWindow->TimeMouseOver     = GetPrivateProfileInt(szChatINISection,"Delay",       2000,INIFileName); 
    pWindow->FadeDuration      = GetPrivateProfileInt(szChatINISection,"Duration",     500,INIFileName); 
    pWindow->Alpha             = GetPrivateProfileInt(szChatINISection,"Alpha",        255,INIFileName); 
    pWindow->FadeToAlpha       = GetPrivateProfileInt(szChatINISection,"FadeToAlpha",  255,INIFileName); 
    pWindow->BGType            = GetPrivateProfileInt(szChatINISection,"BGType",         1,INIFileName); 
    pWindow->BGColor.R         = GetPrivateProfileInt(szChatINISection,"BGTint.red",   255,INIFileName); 
    pWindow->BGColor.G         = GetPrivateProfileInt(szChatINISection,"BGTint.green", 255,INIFileName); 
    pWindow->BGColor.B         = GetPrivateProfileInt(szChatINISection,"BGTint.blue",  255,INIFileName); 

    MQKissWnd->SetChatFont(GetPrivateProfileInt(szChatINISection,"FontSize",4,INIFileName)); 
    GetPrivateProfileString(szChatINISection,"WindowTitle","MQ2Kiss Help Window",szTemp,MAX_STRING,INIFileName); 
    SetCXStr(&pWindow->WindowText,szTemp); 
} 

VOID SaveChatToINI(PCSIDLWND pWindow) 
{ 
    CHAR szTemp[MAX_STRING]={0}; 
    WritePrivateProfileString("Settings","AutoScroll",   bAutoScroll?"on":"off",INIFileName); 
    WritePrivateProfileString("Settings","NoCharSelect", bNoCharSelect?"on":"off",INIFileName); 
    WritePrivateProfileString("Settings","SaveByChar",   bSaveByChar?"on":"off",INIFileName); 

    if (pWindow->Minimized) 
    { 
        WritePrivateProfileString(szChatINISection,"ChatTop",    itoa(pWindow->OldLocation.top,    szTemp,10),INIFileName); 
        WritePrivateProfileString(szChatINISection,"ChatBottom", itoa(pWindow->OldLocation.bottom, szTemp,10),INIFileName); 
        WritePrivateProfileString(szChatINISection,"ChatLeft",   itoa(pWindow->OldLocation.left,   szTemp,10),INIFileName); 
        WritePrivateProfileString(szChatINISection,"ChatRight",  itoa(pWindow->OldLocation.right,  szTemp,10),INIFileName); 
    } 
    else 
    { 
        WritePrivateProfileString(szChatINISection,"ChatTop",    itoa(pWindow->Location.top,    szTemp,10),INIFileName); 
        WritePrivateProfileString(szChatINISection,"ChatBottom", itoa(pWindow->Location.bottom, szTemp,10),INIFileName); 
        WritePrivateProfileString(szChatINISection,"ChatLeft",   itoa(pWindow->Location.left,   szTemp,10),INIFileName); 
        WritePrivateProfileString(szChatINISection,"ChatRight",  itoa(pWindow->Location.right,  szTemp,10),INIFileName); 
    } 
    WritePrivateProfileString(szChatINISection,"Locked",       itoa(pWindow->Locked,       szTemp,10),INIFileName); 
    WritePrivateProfileString(szChatINISection,"Fades",        itoa(pWindow->Fades,        szTemp,10),INIFileName); 
    WritePrivateProfileString(szChatINISection,"Delay",        itoa(pWindow->MouseOver,    szTemp,10),INIFileName); 
    WritePrivateProfileString(szChatINISection,"Duration",     itoa(pWindow->FadeDuration, szTemp,10),INIFileName); 
    WritePrivateProfileString(szChatINISection,"Alpha",        itoa(pWindow->Alpha,        szTemp,10),INIFileName); 
    WritePrivateProfileString(szChatINISection,"FadeToAlpha",  itoa(pWindow->FadeToAlpha,  szTemp,10),INIFileName); 
    WritePrivateProfileString(szChatINISection,"BGType",       itoa(pWindow->BGType,       szTemp,10),INIFileName); 
    WritePrivateProfileString(szChatINISection,"BGTint.red",   itoa(pWindow->BGColor.R,    szTemp,10),INIFileName); 
    WritePrivateProfileString(szChatINISection,"BGTint.green", itoa(pWindow->BGColor.G,    szTemp,10),INIFileName); 
    WritePrivateProfileString(szChatINISection,"BGTint.blue",  itoa(pWindow->BGColor.B,    szTemp,10),INIFileName); 
    WritePrivateProfileString(szChatINISection,"FontSize",     itoa(MQKissWnd->FontSize,   szTemp,10),INIFileName); 
    
    GetCXStr(pWindow->WindowText,szTemp); 
    // WritePrivateProfileString(szChatINISection,"WindowTitle",szTemp,INIFileName); 
} 

VOID CreateChatWindow() 
{ 
    DebugSpew("MQ2Kiss::CreateChatWindow()"); 
    class CXStr ChatWnd("ChatWindow"); 
    MQKissWnd = new CMQKissWnd(&ChatWnd); 
    if (!MQKissWnd) 
    { 
        return; 
    } 
    LoadChatFromINI((PCSIDLWND)MQKissWnd); 
    SaveChatToINI((PCSIDLWND)MQKissWnd); // A) we're masochists, B) this creates the file if its not there.. 
} 

VOID DestroyChatWnd() 
{ 
    if (MQKissWnd) 
    { 
        SaveChatToINI((PCSIDLWND)MQKissWnd); 
        delete MQKissWnd; 
        MQKissWnd=0; 
        ulOldVScrollPos=0; 
    } 
} 

PLUGIN_API DWORD KOnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter) 
{ 

    MQKissWnd->dShow=1; 
    PFILTER pFilter=gpFilters; 
    while (pFilter) 
    { 
        if (!pFilter->pEnabled || (*pFilter->pEnabled)) 
        { 
            if (!strnicmp(Line,pFilter->FilterText,pFilter->Length)) 
            { 
                return 0; 
            } 
        } 
        pFilter = pFilter->pNext; 
    } 
    Color=pChatManager->GetRGBAFromIndex(Color); 
    CHAR szProcessed[MAX_STRING]; 
    MQToSTML(Line,szProcessed,MAX_STRING,Color); 
    strcat(szProcessed,"<br>"); 
    CXStr NewText(szProcessed); 
    DebugTry(ConvertItemTags(NewText,FALSE)); 
    ChatBuffer *pNewBuffer = new ChatBuffer; 
    GetCXStr(NewText.Ptr,pNewBuffer->Text,MAX_STRING); 
    pNewBuffer->pPrev=pPendingChatTail; 
    pNewBuffer->pNext=0; 
    if (pPendingChatTail) 
    { 
        pPendingChatTail->pNext=pNewBuffer; 
    } 
    else 
    { 
        pPendingChat=pNewBuffer; 
    } 
    pPendingChatTail=pNewBuffer; 
    PendingChatLines++; 
    return 0; 
} 

void LoadFile()
{
   // Destroy old window
    if (MQKissWnd) 
    { 
        DestroyChatWnd();
    } 
   sprintf(FileName,"%s.%s",SectionName,"txt");
   char s[300]={0};
   char filename[300];
   sprintf(filename,"%s\\%s",gszINIPath,FileName);

   FILE *f;
   f=fopen(filename,"r");
   if (f==NULL)
   {
      sprintf(s,"KissHelp: Help file not found (%s)",filename);
      WriteChatColor(s,CONCOLOR_RED); 
      return;
   }

   //Build Window
    CreateChatWindow();
    sprintf(s,"KissHelp - %s",SectionName);
    SetCXStr(&(((PCSIDLWND)MQKissWnd)->WindowText),s);

   //Load File
   char buff[64000]={0};
   char line[1000]={0};
   while (!feof(f))
   {
      if (fgets(line,999,f)!=NULL)
         strcat(buff,line);
      SetCXStr(&(PCXSTR)MQKissWnd->InputText,line);
   }
   fclose(f);
   // strip tabs
   for (unsigned int i=0;i<strlen(buff);i++) if (buff[i]=='\t') buff[i]=' ';
    KOnWriteChatColor(buff,CONCOLOR_WHITE,0);

}

void KissHelp(PSPAWNINFO pPlayer, char* szLine)
    {
    char szArg[MAX_STRING] = {0};
    char szMsg[MAX_STRING] = {0};

    GetArg(szArg, szLine, 1);
    // open window
    if (!strnicmp(szArg, "show", 5) || !strnicmp(szArg, "open", 5) || !strnicmp(szArg, "on", 3) )
    {
        CreateChatWindow();
        return;
    } 

    // close window
    if (!strnicmp(szArg, "close", 6) || !strnicmp(szArg, "off", 4) && MQKissWnd )
    {
        DestroyChatWnd();  
        return;
    } 

    // load section files
    if (!strnicmp(szArg, "dps", 5))
    {
        sprintf(SectionName,"%s","DPS");
        LoadFile();
    }
    if (!strnicmp(szArg, "general", 8))
    {
        sprintf(SectionName,"%s","General");
        LoadFile();
     }
}

VOID Style(PSPAWNINFO pChar, PCHAR szLine) 
{ 
    if (!szLine || !szLine[0]) 
    { 
        char out[256]; 
        sprintf(out,"Style 0x%X",MQKissWnd->WindowStyle); 
        WriteChatColor(out); 
        return; 
    } 
    if (szLine[0]=='!') 
    { 
        int TurnOff; 
        sscanf(&szLine[1],"%x",&TurnOff); 
        BitOff(MQKissWnd->WindowStyle,TurnOff); 
    } 
    else 
    { 
        int TurnOn; 
        sscanf(&szLine[0],"%x",&TurnOn); 
        BitOn(MQKissWnd->WindowStyle,TurnOn); 
    } 
    char out[256]; 
    sprintf(out,"Style 0x%X",MQKissWnd->WindowStyle); 
    WriteChatColor(out); 
} 

VOID MQKissFont(PSPAWNINFO pChar, PCHAR Line) 
{ 
    if (MQKissWnd) 
    { 
        if (Line[0]) 
        { 
            int size=atoi(Line); 
            if (size<0 || size>10) 
            { 
                WriteChatf("Usage: /mqfont 0-10"); 
                return; 
            } 
            MQKissWnd->SetChatFont(size); 
            SaveChatToINI((PCSIDLWND)MQKissWnd); 
        } 
    } 
} 

VOID MQKissMin(PSPAWNINFO pChar, PCHAR Line) 
{ 
    if (MQKissWnd) 
    { 
        ((CXWnd*)MQKissWnd)->OnMinimizeBox(); 
    } 
} 

VOID MQKissClear(PSPAWNINFO pChar, PCHAR Line) 
{ 
    if(MQKissWnd) 
    { 
        ((CChatWindow*)MQKissWnd)->Clear(); 
        ulOldVScrollPos=0; 
        MQKissWnd->OutBoxLines=0;
    } 
} 

VOID DoMQ2KissBind(PCHAR Name, BOOL Down) 
{ 
    if (!Down) 
    { 
        if (MQKissWnd) 
        { 
            CXRect rect=((CXWnd*)MQKissWnd->InputBox)->GetScreenRect(); 
            CXPoint pt=rect.CenterPoint(); 
            ((CXWnd*)MQKissWnd->InputBox)->SetWindowTextA(CXStr("/")); 
            ((CXWnd*)MQKissWnd->InputBox)->HandleLButtonDown(&pt,0); 
        } 
    } 
} 

PLUGIN_API VOID OnReloadUI() 
{ 
    DebugSpewAlways("MQ2Kiss::OnReloadUI()"); 
    // redraw window when you load/reload UI 
    // DebugTry(CreateChatWindow()); 
} 

PLUGIN_API VOID OnCleanUI() 
{ 
    DebugSpewAlways("MQ2Kiss::OnCleanUI()"); 
    // destroy chatwnd before server select & while reloading UI 
    DestroyChatWnd(); 
} 

PLUGIN_API VOID SetGameState(DWORD GameState) 
{ 
    DebugSpew("MQ2Kiss::SetGameState()"); 
    if (GameState==GAMESTATE_CHARSELECT) 
    { 
        if (bNoCharSelect) 
        { 
            // destroy chatwnd at charselect if NoCharSelect=on 
            DestroyChatWnd(); 
        } 
        else 
        { 
            AddMQ2KeyBind("MQ2CSCHAT",DoMQ2KissBind); 
            KeyCombo Combo; 
            ParseKeyCombo("/",Combo); 
            SetMQ2KeyBind("MQ2CSCHAT",FALSE,Combo); 
        } 
    } 
    else 
    { 
        if (GameState!=GAMESTATE_CHARCREATE) RemoveMQ2KeyBind("MQ2CSCHAT"); 
    } 
} 

PLUGIN_API VOID OnPulse() 
{ 
    if (MQKissWnd) 
    { 
        if(PendingChatLines) 
        { 
            // set 'old' to current 
            ulOldVScrollPos=MQKissWnd->OutputBox->VScrollPos; 

            // scroll down if autoscroll enabled, or current position is the bottom of chatwnd 
            bool bScrollDown=bAutoScroll?true:(MQKissWnd->OutputBox->VScrollPos==MQKissWnd->OutputBox->VScrollMax?true:false); 

            DWORD ThisPulse=PendingChatLines; 
            if (ThisPulse>LINES_PER_FRAME) 
            { 
                ThisPulse=LINES_PER_FRAME; 
            } 
            PendingChatLines-=ThisPulse; 
            MQKissWnd->OutBoxLines+=ThisPulse; 
            if (MQKissWnd->OutBoxLines>MAX_CHAT_SIZE) 
            { 
                DWORD Diff=(MQKissWnd->OutBoxLines-MAX_CHAT_SIZE)+LINES_PER_FRAME; 
                MQKissWnd->OutBoxLines-=Diff; 
                Benchmark(bmStripFirstStmlLines,MQKissWnd->OutputBox->StripFirstSTMLLines(Diff)); 
            } 
            for (DWORD N=0 ; N<ThisPulse ; N++) 
            { 
                DebugTry(MQKissWnd->OutputBox->AppendSTML(pPendingChat->Text)); 
                ChatBuffer *pNext=pPendingChat->pNext; 
                delete pPendingChat; 
                pPendingChat=pNext; 
                //DebugSpew("NEW: max %u - pos: %u",MQKissWnd->OutputBox->VScrollMax,MQKissWnd->OutputBox->VScrollPos); 
            } 
            if (!pPendingChat) 
            { 
                pPendingChatTail=0; 
            } 

            if(bScrollDown) 
            { 
                // set current vscroll position to bottom 
                //DebugTry(((CXWnd*)MQKissWnd->OutputBox)->SetVScrollPos(MQKissWnd->OutputBox->VScrollMax)); 
            } 
            else 
            { 
                // autoscroll is disabled and current vscroll position was not at the bottom, retain position 
                // note: if the window is full (VScrollMax value between 9793 and 9835), this will not adjust with 
                // the flushing of buffer that keeps window a certain max size 
                DebugTry(((CXWnd*)MQKissWnd->OutputBox)->SetVScrollPos(ulOldVScrollPos)); 
            } 
        } 

        if(InHoverState()) 
        { 
            ((CXWnd*)MQKissWnd)->DoAllDrawing(); 
        } 
    } 
} 


PLUGIN_API VOID InitializePlugin() 
{ 
    DebugSpewAlways("Initializing MQ2Kiss"); 
    // Add commands, macro parameters, hooks, etc. 
    AddCommand("/khstyle",Style,0,1,0); 
    AddCommand("/khfont",MQKissFont); 
    AddCommand("/khmin",MQKissMin); 
    AddCommand("/khclear",MQKissClear); 
    AddCommand("/kisshelp",KissHelp);
    AddMQ2KeyBind("MQ2Kiss",DoMQ2KissBind); 
    bmStripFirstStmlLines=AddMQ2Benchmark("StripFirstStmlLines"); 
    LoadChatSettings(); 
} 

PLUGIN_API VOID ShutdownPlugin() 
{ 
    DebugSpewAlways("Shutting down MQ2Kiss"); 
    while(pPendingChat) 
    { 
        ChatBuffer *pNext=pPendingChat->pNext; 
        delete pPendingChat; 
        pPendingChat=pNext; 
    } 
    pPendingChatTail=0; 
    PendingChatLines=0; 
    // Remove commands, macro parameters, hooks, etc. 
    RemoveCommand("/khstyle"); 
    RemoveCommand("/khfont"); 
    RemoveCommand("/khmin"); 
    RemoveCommand("/khclear"); 
    RemoveCommand("/kisshelp");
    RemoveMQ2KeyBind("MQ2Kiss"); 
    RemoveMQ2Benchmark(bmStripFirstStmlLines); 
    DestroyChatWnd(); 
} 
