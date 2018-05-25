// MQ2MYButtons.cpp : Defines the entry point for the DLL application.
//
//9/26/2004 Plugin to add a new button bank
// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.
#include "../MQ2Plugin.h"
#include "MQ2MyButtons.h"
PreSetup("MQ2MYButtons");

class CHButWnd : public CCustomWnd 
{ 
public: 
   CHButWnd():CCustomWnd("MQButtonWnd") 
   { 
        
      MyButton1 = (CButtonWnd*)GetChildItem("MQHB_Button1");
      MyButton2 = (CButtonWnd*)GetChildItem("MQHB_Button2");
      MyButton3 = (CButtonWnd*)GetChildItem("MQHB_Button3");
      MyButton4 = (CButtonWnd*)GetChildItem("MQHB_Button4");
      MyButton5 = (CButtonWnd*)GetChildItem("MQHB_Button5");
      MyButton6 = (CButtonWnd*)GetChildItem("MQHB_Button6");
      MyButton7 = (CButtonWnd*)GetChildItem("MQHB_Button7");
      MyButton8 = (CButtonWnd*)GetChildItem("MQHB_Button8");
      MyButton9 = (CButtonWnd*)GetChildItem("MQHB_Button9");
      MyButton10 = (CButtonWnd*)GetChildItem("MQHB_Button10");
      
      SetWndNotification(CHButWnd); 
   } 
    
   ~CHButWnd() 
   { 
   } 

   int WndNotification(CXWnd *pWnd, unsigned int Message, void *unknown) 
   {    
        if (pWnd==(CXWnd*)MyButton1) 
         { 
            if (Message==XWM_LCLICK) 
               DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand1);
               
            else 
               DebugSpew("MyButton1 message %Xh / %d",Message,Message); 
         } 

      if (pWnd==(CXWnd*)MyButton2) 
         { 
            if (Message==XWM_LCLICK) 
               DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand2);
            else 
               DebugSpew("MyButton2 message %Xh / %d",Message,Message); 
         } 


      if (pWnd==(CXWnd*)MyButton3) 
         { 
            if (Message==XWM_LCLICK) 
               DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand3);
            else 
               DebugSpew("MyButton3 message %Xh / %d",Message,Message); 
         } 


      if (pWnd==(CXWnd*)MyButton4) 
         { 
            if (Message==XWM_LCLICK) 
               DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand4);
            else 
               DebugSpew("MyButton4 message %Xh / %d",Message,Message); 
         } 


      if (pWnd==(CXWnd*)MyButton5) 
         { 
            if (Message==XWM_LCLICK) 
               DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand5);
            else 
               DebugSpew("MyButton5 message %Xh / %d",Message,Message); 
         } 


      if (pWnd==(CXWnd*)MyButton6) 
         { 
            if (Message==XWM_LCLICK) 
               DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand6);
            else 
               DebugSpew("MyButton6 message %Xh / %d",Message,Message); 
         } 


      if (pWnd==(CXWnd*)MyButton7) 
         { 
            if (Message==XWM_LCLICK) 
               DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand7);
            else 
               DebugSpew("MyButton7 message %Xh / %d",Message,Message); 
         } 


      if (pWnd==(CXWnd*)MyButton8) 
         { 
            if (Message==XWM_LCLICK) 
               DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand8);
            else 
               DebugSpew("MyButton8 message %Xh / %d",Message,Message); 
         } 


      if (pWnd==(CXWnd*)MyButton9) 
         { 
            if (Message==XWM_LCLICK) 
               DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand9);
            else 
               DebugSpew("MyButton9 message %Xh / %d",Message,Message); 
         } 


      if (pWnd==(CXWnd*)MyButton10) 
         { 
            if (Message==XWM_LCLICK) 
               DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand10);
            else 
               DebugSpew("MyButton10 message %Xh / %d",Message,Message); 
         } 

   return 0; 
   }; 
      CButtonWnd *MyButton1; 
      CButtonWnd *MyButton2; 
      CButtonWnd *MyButton3; 
      CButtonWnd *MyButton4; 
      CButtonWnd *MyButton5; 
      CButtonWnd *MyButton6; 
      CButtonWnd *MyButton7; 
      CButtonWnd *MyButton8; 
      CButtonWnd *MyButton9; 
      CButtonWnd *MyButton10; 
}; 

CHButWnd *MyWnd = 0; 


BOOL dataBntLbl1(PCHAR szIndex, MQ2TYPEVAR &Ret)
{
   char szButoonLbl[MAX_STRING];
   GetPrivateProfileString("MYButtons","MyButtonLabel_1","1",szButoonLbl,MAX_STRING,INIFileName);
   Ret.Ptr=szButoonLbl;
   Ret.Type=pStringType;
   return true;
}

BOOL dataBntLbl2(PCHAR szIndex, MQ2TYPEVAR &Ret)
{
   char szButoonLbl[MAX_STRING];
   GetPrivateProfileString("MYButtons","MyButtonLabel_2","2",szButoonLbl,MAX_STRING,INIFileName);
   Ret.Ptr=szButoonLbl;
   Ret.Type=pStringType;
   return true;
}
BOOL dataBntLbl3(PCHAR szIndex, MQ2TYPEVAR &Ret)
{
   char szButoonLbl[MAX_STRING];
   GetPrivateProfileString("MYButtons","MyButtonLabel_3","3",szButoonLbl,MAX_STRING,INIFileName);
   Ret.Ptr=szButoonLbl;
   Ret.Type=pStringType;
   return true;
}

BOOL dataBntLbl4(PCHAR szIndex, MQ2TYPEVAR &Ret)
{
   char szButoonLbl[MAX_STRING];
   GetPrivateProfileString("MYButtons","MyButtonLabel_4","4",szButoonLbl,MAX_STRING,INIFileName);
   Ret.Ptr=szButoonLbl;
   Ret.Type=pStringType;
   return true;
}
BOOL dataBntLbl5(PCHAR szIndex, MQ2TYPEVAR &Ret)
{
   char szButoonLbl[MAX_STRING];
   GetPrivateProfileString("MYButtons","MyButtonLabel_5","5",szButoonLbl,MAX_STRING,INIFileName);
   Ret.Ptr=szButoonLbl;
   Ret.Type=pStringType;
   return true;
}

BOOL dataBntLbl6(PCHAR szIndex, MQ2TYPEVAR &Ret)
{
   char szButoonLbl[MAX_STRING];
   GetPrivateProfileString("MYButtons","MyButtonLabel_6","6",szButoonLbl,MAX_STRING,INIFileName);
   Ret.Ptr=szButoonLbl;
   Ret.Type=pStringType;
   return true;
}

BOOL dataBntLbl7(PCHAR szIndex, MQ2TYPEVAR &Ret)
{
   char szButoonLbl[MAX_STRING];
   GetPrivateProfileString("MYButtons","MyButtonLabel_7","7",szButoonLbl,MAX_STRING,INIFileName);
   Ret.Ptr=szButoonLbl;
   Ret.Type=pStringType;
   return true;
}
BOOL dataBntLbl8(PCHAR szIndex, MQ2TYPEVAR &Ret)
{
   char szButoonLbl[MAX_STRING];
   GetPrivateProfileString("MYButtons","MyButtonLabel_8","8",szButoonLbl,MAX_STRING,INIFileName);
   Ret.Ptr=szButoonLbl;
   Ret.Type=pStringType;
   return true;
}
BOOL dataBntLbl9(PCHAR szIndex, MQ2TYPEVAR &Ret)
{
   char szButoonLbl[MAX_STRING];
   GetPrivateProfileString("MYButtons","MyButtonLabel_9","9",szButoonLbl,MAX_STRING,INIFileName);
   Ret.Ptr=szButoonLbl;
   Ret.Type=pStringType;
   return true;
}
BOOL dataBntLbl10(PCHAR szIndex, MQ2TYPEVAR &Ret)
{
   char szButoonLbl[MAX_STRING];
   GetPrivateProfileString("MYButtons","MyButtonLabel_10","10",szButoonLbl,MAX_STRING,INIFileName);
   Ret.Ptr=szButoonLbl;
   Ret.Type=pStringType;
   return true;
}
PLUGIN_API VOID OnCleanUI(VOID) 
{ 
   DebugSpewAlways("MQ2HBut::OnCleanUI()"); 
   DebugSpewAlways("MQ2WndTest::OnCleanUI()"); 
    
   if (MyWnd) 
   { 
      delete MyWnd; 
      MyWnd=0; 
   } 

} 

PLUGIN_API VOID SetGameState(DWORD GameState) 
{ 
//   DebugSpewAlways("MQ2WndTest::SetGameState()"); 
   if (GameState==GAMESTATE_INGAME && !MyWnd) 
   { 
      if (pSidlMgr->FindScreenPieceTemplate("MQButtonWnd")) 
         CreateButtonWindow(); 
   } 
} 

PLUGIN_API VOID OnReloadUI() 
{ 
//   DebugSpewAlways("MQ2WndTest::OnReloadUI()"); 
   CreateButtonWindow();
} 

PLUGIN_API VOID OnPulse(VOID) 
{ 
	if (gGameState==GAMESTATE_INGAME && winActive && ( !MyWnd || (MyWnd && !(MyWnd->dShow)))) 
	{    
		CreateButtonWindow(); 
		((CXWnd*)MyWnd)->Show(1,1); 
	} 
       
	if ( gGameState==GAMESTATE_INGAME && !winActive ) 
	{ 
		CreateButtonWindow(); 
		((CXWnd*)MyWnd)->Show(0,0); 
	} 
} 


PLUGIN_API VOID InitializePlugin(VOID) 
{ 
   DebugSpewAlways("Initializing MQ2WndTest"); 
   WriteChatColor("Loading MyMQButtons.05 ...");
   AddXMLFile("MQUI_ButtonsWnd.xml");
   GetPrivateProfileString("MYButtons","MyButtonCommand_1",   NULL,szMyCommand1,MAX_STRING,INIFileName);
   GetPrivateProfileString("MYButtons","MyButtonCommand_2",   NULL,szMyCommand2,MAX_STRING,INIFileName);
   GetPrivateProfileString("MYButtons","MyButtonCommand_3",   NULL,szMyCommand3,MAX_STRING,INIFileName);
   GetPrivateProfileString("MYButtons","MyButtonCommand_4",   NULL,szMyCommand4,MAX_STRING,INIFileName);
   GetPrivateProfileString("MYButtons","MyButtonCommand_5",   NULL,szMyCommand5,MAX_STRING,INIFileName);
   GetPrivateProfileString("MYButtons","MyButtonCommand_6",   NULL,szMyCommand6,MAX_STRING,INIFileName);
   GetPrivateProfileString("MYButtons","MyButtonCommand_7",   NULL,szMyCommand7,MAX_STRING,INIFileName);
   GetPrivateProfileString("MYButtons","MyButtonCommand_8",   NULL,szMyCommand8,MAX_STRING,INIFileName);
   GetPrivateProfileString("MYButtons","MyButtonCommand_9",   NULL,szMyCommand9,MAX_STRING,INIFileName);
   GetPrivateProfileString("MYButtons","MyButtonCommand_10",   NULL,szMyCommand10,MAX_STRING,INIFileName);
    
   AddCommand("/mybntshow",MyButtonWindowShow); 
   AddCommand("/mybntclk1",CmdMyButtonClick1);
   AddCommand("/mybntclk2",CmdMyButtonClick2);
   AddCommand("/mybntclk3",CmdMyButtonClick3);
   AddCommand("/mybntclk4",CmdMyButtonClick4);
   AddCommand("/mybntclk5",CmdMyButtonClick5);
   AddCommand("/mybntclk6",CmdMyButtonClick6);
   AddCommand("/mybntclk7",CmdMyButtonClick7);
   AddCommand("/mybntclk8",CmdMyButtonClick8);
   AddCommand("/mybntclk9",CmdMyButtonClick9);
   AddCommand("/mybntclk10",CmdMyButtonClick10);

   AddMQ2Data("MyBntLbl1",dataBntLbl1);
   AddMQ2Data("MyBntLbl2",dataBntLbl2);
   AddMQ2Data("MyBntLbl3",dataBntLbl3);
   AddMQ2Data("MyBntLbl4",dataBntLbl4);
   AddMQ2Data("MyBntLbl5",dataBntLbl5);
   AddMQ2Data("MyBntLbl6",dataBntLbl6);
   AddMQ2Data("MyBntLbl7",dataBntLbl7);
   AddMQ2Data("MyBntLbl8",dataBntLbl8);
   AddMQ2Data("MyBntLbl9",dataBntLbl9);
   AddMQ2Data("MyBntLbl10",dataBntLbl10);


} 

// Called once, when the plugin is to shutdown 
PLUGIN_API VOID ShutdownPlugin(VOID) 
{ 
   DebugSpewAlways("Shutting down MQ2WndTest"); 

   // Remove commands, macro parameters, hooks, etc. 
   // RemoveParm("$myparm(x)"); 
   // // RemoveCommand("/mycommand"); 
   DestroyButtonWindow();
   RemoveCommand("/mybntshow");
   RemoveCommand("/mybntclk1");
   RemoveCommand("/mybntclk2");
   RemoveCommand("/mybntclk3");
   RemoveCommand("/mybntclk4");
   RemoveCommand("/mybntclk5");
   RemoveCommand("/mybntclk6");
   RemoveCommand("/mybntclk7");
   RemoveCommand("/mybntclk8");
   RemoveCommand("/mybntclk9");
   RemoveCommand("/mybntclk10");

} 



void ReadWindowINI(PCSIDLWND pWindow) 
{ 
   CHAR Buffer[MAX_STRING] = {0}; 
   pWindow->Locked            = GetPrivateProfileInt("Settings","Locked",         0,INIFileName); 
   pWindow->Fades             = GetPrivateProfileInt("Settings","Fades",         0,INIFileName); 
   pWindow->FadeDelay		  = GetPrivateProfileInt("Settings","Delay",         2000,INIFileName); 
   pWindow->FadeDuration      = GetPrivateProfileInt("Settings","Duration",      500,INIFileName); 
   pWindow->Alpha             = GetPrivateProfileInt("Settings","Alpha",         255,INIFileName); 
   pWindow->FadeToAlpha       = GetPrivateProfileInt("Settings","FadeToAlpha",   255,INIFileName); 
   pWindow->BGType            = GetPrivateProfileInt("Settings","BGType",         1,INIFileName); 
   ARGBCOLOR argb;
   argb.A					  = GetPrivateProfileInt("Settings","BGTint.alpha",      255,INIFileName); 
   argb.R					  = GetPrivateProfileInt("Settings","BGTint.red",      255,INIFileName); 
   argb.G					  = GetPrivateProfileInt("Settings","BGTint.green",      255,INIFileName); 
   argb.B					  = GetPrivateProfileInt("Settings","BGTint.blue",      255,INIFileName); 
   pWindow->BGColor			  = argb.ARGB;
 
   GetPrivateProfileString("Settings","WindowTitle","MQ Button Window",Buffer,MAX_STRING,INIFileName); 
   winActive               = 0x00000001 & GetPrivateProfileInt("Settings","winActive",   1,INIFileName); 
   SetCXStr(&pWindow->WindowText,Buffer); 
} 
template <unsigned int _Size>LPSTR SafeItoa(int _Value,char(&_Buffer)[_Size], int _Radix)
{
	errno_t err = _itoa_s(_Value, _Buffer, _Radix);
	if (!err) {
		return _Buffer;
	}
	return "";
}
void WriteWindowINI(PCSIDLWND pWindow) 
{ 
   CHAR szTemp[MAX_STRING] = {0}; 
   WritePrivateProfileString("Settings","Locked",      SafeItoa(pWindow->Locked,         szTemp,10),INIFileName); 

   GetCXStr(pWindow->WindowText,szTemp); 
   WritePrivateProfileString("Settings","WindowTitle",szTemp,INIFileName); 

   WritePrivateProfileString("Settings","Fades",      SafeItoa(pWindow->Fades,        szTemp,10),INIFileName); 
   WritePrivateProfileString("Settings","Delay",      SafeItoa(pWindow->MouseOver,    szTemp,10),INIFileName); 
   WritePrivateProfileString("Settings","Duration",      SafeItoa(pWindow->FadeDuration, szTemp,10),INIFileName); 
   WritePrivateProfileString("Settings","Alpha",      SafeItoa(pWindow->Alpha,        szTemp,10),INIFileName); 
   WritePrivateProfileString("Settings","FadeToAlpha",  SafeItoa(pWindow->FadeToAlpha,  szTemp,10),INIFileName); 
   WritePrivateProfileString("Settings","BGType",      SafeItoa(pWindow->BGType,       szTemp,10),INIFileName); 
   ARGBCOLOR argb;
   argb.ARGB = pWindow->BGColor;
   WritePrivateProfileString("Settings","BGTint.alpha",   SafeItoa(argb.A,    szTemp,10),INIFileName); 
   WritePrivateProfileString("Settings","BGTint.red",   SafeItoa(argb.R,    szTemp,10),INIFileName); 
   WritePrivateProfileString("Settings","BGTint.green", SafeItoa(argb.G,    szTemp,10),INIFileName); 
   WritePrivateProfileString("Settings","BGTint.blue",  SafeItoa(argb.B,    szTemp,10),INIFileName); 

   WritePrivateProfileString("Settings","winActive",   SafeItoa((int)winActive,         szTemp,10),INIFileName); 

} 



VOID MyButtonWindowShow(PSPAWNINFO pChar, PCHAR szLine) 
{ 
   
      if ( (MyWnd && MyWnd->dShow) || strstr(szLine, "off") ) { 
         winActive = false; 
         CreateButtonWindow(); 
         ((CXWnd*)MyWnd)->Show(0,0); 
      } else { 
         winActive = true; 
         CreateButtonWindow(); 
         ((CXWnd*)MyWnd)->Show(1,1); 
      } 

} 

void CreateButtonWindow() 
{ 
   DebugSpewAlways("MQ2Timer::CreateTimerWindow()"); 
   if (MyWnd)  return; 

   if (pSidlMgr->FindScreenPieceTemplate("MQButtonWnd")) { 
      MyWnd = new CHButWnd; 
     ReadWindowINI((PCSIDLWND)MyWnd); 
      WriteWindowINI((PCSIDLWND)MyWnd); 
      
   } 
} 



void DestroyButtonWindow() 
{ 
   DebugSpewAlways("MQ2Timer::DestroyButtonWindow()"); 
   if (MyWnd) 
   { 
   
      WriteWindowINI((PCSIDLWND)MyWnd); 
      delete MyWnd; 
      MyWnd=0; 
   } 
} 


void CmdMyButtonClick1(PSPAWNINFO pChar, PCHAR szLine)
{
   if(szMyCommand1!=NULL){DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand1);}
}
void CmdMyButtonClick2(PSPAWNINFO pChar, PCHAR szLine)
{
   if(szMyCommand2!=NULL){DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand2);}
}
void CmdMyButtonClick3(PSPAWNINFO pChar, PCHAR szLine)
{
   if(szMyCommand3!=NULL){DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand3);}
}
void CmdMyButtonClick4(PSPAWNINFO pChar, PCHAR szLine)
{
   if(szMyCommand4!=NULL){DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand4);}
}
void CmdMyButtonClick5(PSPAWNINFO pChar, PCHAR szLine)
{
   if(szMyCommand5!=NULL){DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand5);}
}
void CmdMyButtonClick6(PSPAWNINFO pChar, PCHAR szLine)
{
   if(szMyCommand6!=NULL){DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand6);}
}
void CmdMyButtonClick7(PSPAWNINFO pChar, PCHAR szLine)
{
   if(szMyCommand7!=NULL){DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand7);}
}
void CmdMyButtonClick8(PSPAWNINFO pChar, PCHAR szLine)
{
   if(szMyCommand8!=NULL){DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand8);}
}
void CmdMyButtonClick9(PSPAWNINFO pChar, PCHAR szLine)
{
   if(szMyCommand9!=NULL){DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand9);}
}
void CmdMyButtonClick10(PSPAWNINFO pChar, PCHAR szLine)
{
   if(szMyCommand10!=NULL){DoCommand((PSPAWNINFO)pCharSpawn,szMyCommand10);}
}
 