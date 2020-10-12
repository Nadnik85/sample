// MQ2Say.cpp : Say Detection and Alerting
//

constexpr int MAX_CHAT_SIZE = 700;
constexpr int LINES_PER_FRAME = 3;

#include "../MQ2Plugin.h"
#include <time.h>
#include <ctime>
#include <iostream>
#include <chrono>

PreSetup("MQ2Say");

std::list<std::string> sPendingSay;
std::map<std::string, std::chrono::steady_clock::time_point> mAlertTimers;

const unsigned int CMD_HIST_MAX = 50;
int ulOldVScrollPos = 0;
int  bmStripFirstStmlLines = 0;
int intIgnoreDelay = 300;
char szSayINISection[MAX_STRING] = { 0 };
char szWinTitle[MAX_STRING] = { 0 };
char strSayAlertCommand[MAX_STRING] = { 0 };
bool bSayStatus = true;
bool bSayDebug = false;
bool bAutoScroll = true;
bool bSaveByChar = true;
bool bSayAlerts = true;
bool bSayTimestamps = true;
class CMQSayWnd;
CMQSayWnd* MQSayWnd = nullptr;

class CMQSayWnd : public CCustomWnd
{
public:
	CMQSayWnd(char* Template) :CCustomWnd(Template)
	{
		DebugSpew("CMQSayWnd()");
		SetWndNotification(CMQSayWnd);
		SetWindowStyle(CWS_CLIENTMOVABLE | CWS_USEMYALPHA | CWS_RESIZEALL | CWS_BORDER | CWS_MINIMIZE | CWS_TITLE);
		//SetWindowStyle(CWS_AUTOVSCROLL | CWS_AUTOHSCROLL | CWS_TITLE | CWS_MINIMIZE | CWS_RELATIVERECT | CWS_BORDER | CWS_RESIZEALL);
		RemoveStyle(CWS_TRANSPARENT | CWS_CLOSE);
		SetBGColor(0xFF000000);//black background
		InputBox = (CTextEntryWnd*)GetChildItem("CW_ChatInput");
		InputBox->AddStyle(CWS_AUTOVSCROLL | CWS_RELATIVERECT | CWS_BORDER);// 0x800C0;
		this->SetFaded(false);
		this->SetEscapable(false);
		this->SetClickable(true);
		this->SetAlpha(0xFF);
		this->SetBGType(1);
		this->ContextMenuID = 3;
		InputBox->SetCRNormal(0xFFFFFFFF);//we want a white cursor
		InputBox->SetMaxChars(512);
		OutputBox = (CStmlWnd*)GetChildItem("CW_ChatOutput");
		OutputBox->SetParentWindow((_CSIDLWND*)this);
		InputBox->SetParentWindow((_CSIDLWND*)this);
		OutBoxLines = 0;
		OutputBox->MaxLines = 0x190;
		OutputBox->SetClickable(true);
		OutputBox->AddStyle(CWS_CLIENTMOVABLE);
		iCurrentCmd = -1;
		SetZLayer(1); // Make this the topmost window (we will leave it as such for charselect, and allow it to move to background ingame)
	}
	~CMQSayWnd()
	{
	}

	int WndNotification(CXWnd* pWnd, unsigned int Message, void* data)
	{
		if (pWnd == 0)
		{
			if (Message == XWM_CLOSE)
			{
				SetVisible(1);
				return 1;
			}
		}
		else if (pWnd == (CXWnd*)InputBox)
		{
			if (Message == XWM_HITENTER)
			{
				char szBuffer[MAX_STRING] = { 0 };
				GetCXStr((PCXSTR)InputBox->InputText, szBuffer, MAX_STRING);
				if (szBuffer[0] != 0)
				{
					if (!sCmdHistory.size() || sCmdHistory.front().compare(szBuffer))
					{
						if (sCmdHistory.size() > CMD_HIST_MAX)
						{
							sCmdHistory.pop_back();
						}
						sCmdHistory.insert(sCmdHistory.begin(), std::string(szBuffer));
					}
					iCurrentCmd = -1;
					SetCXStr(&InputBox->InputText, "");
					if (szBuffer[0] == '/')
					{
						DoCommand((PSPAWNINFO)pLocalPlayer, szBuffer);
					}
					else
					{
						Echo((PSPAWNINFO)pLocalPlayer, szBuffer);
					}
				}
				((CXWnd*)InputBox)->ClrFocus();
			}
			else if (Message == XWM_HISTORY)
			{
				if (data)
				{
					int* pInt = (int*)data;
					int iKeyPress = pInt[1];
					if (iKeyPress == 0xc8) // KeyUp
					{
						if (sCmdHistory.size() > 0)
						{
							iCurrentCmd++;
							if (iCurrentCmd < (static_cast<int>(sCmdHistory.size())) && iCurrentCmd >= 0)
							{
								std::string s = (std::string)sCmdHistory.at(iCurrentCmd);
								((CXWnd*)InputBox)->SetWindowTextA(CXStr(s.c_str()));
							}
							else
							{
								iCurrentCmd = (static_cast<int>(sCmdHistory.size())) - 1;
							}
						}
					}
					else if (iKeyPress == 0xd0) // KeyDown
					{
						if (sCmdHistory.size() > 0)
						{
							iCurrentCmd--;
							if (iCurrentCmd >= 0 && sCmdHistory.size() > 0)
							{
								std::string s = (std::string)sCmdHistory.at(iCurrentCmd);
								((CXWnd*)InputBox)->SetWindowTextA(CXStr(s.c_str()));
							}
							else if (iCurrentCmd < 0)
							{
								iCurrentCmd = -1;
								SetCXStr(&InputBox->InputText, "");
							}
						}
					}
				}
			}
			else
			{
				DebugSpew("InputBox message %Xh, value: %Xh", Message, data);
			}
		}
		else if (Message == XWM_LINK)
		{
			class CChatWindow* p = (class CChatWindow*)this;
			if (OutputBox != (CStmlWnd*)pWnd)
			{
				CStmlWnd* tmp;
				int ret;
				//DebugSpew("MQ2Say: 0x%X, Msg: 0x%X, value: %Xh",pWnd,Message,data);
				//DebugSpew("MQ2Say: pWnd 0x%x != OutputBox 0x%x\n",pWnd,OutputBox);
				tmp = OutputBox;
				OutputBox = (CStmlWnd*)pWnd;
				ret = p->WndNotification(pWnd, Message, data);
				OutputBox = tmp;
				return ret;
			}
			return p->WndNotification(pWnd, Message, data);
		}
		else
		{
			//DebugSpew("MQ2Say: 0x%X, Msg: 0x%X, value: %Xh",pWnd,Message,data);
		}
		return CSidlScreenWnd::WndNotification(pWnd, Message, data);
	};

	void SetSayFont(int size) // brainiac 12-12-2007
	{
		struct FONTDATA
		{
			DWORD NumFonts;
			PCHAR* Fonts;
		};
		FONTDATA* Fonts;            // font array structure
		DWORD* SelFont;             // selected font
		// get fonts structure -- this offset can be found by looking at
		// SetSayFont which is called from the /chatfontsize function
		Fonts = (FONTDATA*)&(((char*)pWndMgr)[EQ_CHAT_FONT_OFFSET]);
		// check font array bounds and pointers
		if (size < 0 || size >= (static_cast<int>(Fonts->NumFonts)))
		{
			return;
		}
		if (!Fonts->Fonts || !MQSayWnd)
		{
			return;
		}
		//DebugSpew("Setting Size: %i", size);
		SelFont = (DWORD*)Fonts->Fonts[size];
		// Save the text, change the font, then restore the text
		CXStr str(((CStmlWnd*)MQSayWnd->OutputBox)->GetSTMLText());
		((CXWnd*)MQSayWnd->OutputBox)->SetFont(SelFont);
		((CStmlWnd*)MQSayWnd->OutputBox)->SetSTMLText(str, 1, 0);
		((CStmlWnd*)MQSayWnd->OutputBox)->ForceParseNow();
		// scroll to bottom of chat window
		((CXWnd*)MQSayWnd->OutputBox)->SetVScrollPos(MQSayWnd->OutputBox->GetVScrollMax());
		MQSayWnd->FontSize = size;
	};
	CTextEntryWnd* InputBox;
	CStmlWnd* OutputBox;
	//CXWnd* OutWnd;
	//struct _CSIDLWND* OutStruct;
	DWORD OutBoxLines;
	DWORD FontSize = 0;
private:
	std::vector<std::string> sCmdHistory;
	int iCurrentCmd;
};

std::string SayCheckGM(std::string strCheckName)
{
	PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByName(&strCheckName[0]);
	if (pSpawn)
	{
		if (pSpawn->GM) {
			return " \ar(GM)\ax ";
		}
		if (pSpawn->Type == PC)
		{
			return "";
		}
	}
	else
	{
		return " \ar(GM)\ax ";
	}
	return "";
}

std::vector<std::string> SplitSay(const std::string& strLine) {
	std::vector<std::string> vReturn;
	// Create a string and assign it the value of strLine, if we end up not formatting, then the original string is returned.
	std::string strOut = strLine;
	// Find the first space
	size_t Pos = strLine.find(' ');
	// If the first space is found (ie, it's not npos)
	if (Pos != std::string::npos) {
		// Set strName to the substring starting from 2 and going to Pos - 3 (check the math on this, I'm trying to exclude the space)
		vReturn.push_back(strLine.substr(2, Pos - 3));
		// Find the first quote
		Pos = strLine.find('\'');
		// Find the last quote
		size_t Pos2 = strLine.rfind('\'');
		// Make sure we found the first quote and it's not the same quote as the last quote
		if (Pos != std::string::npos && Pos != Pos2) {
			//  Set the say text to the substring starting at 1 after the position (excluding the quote itself, and ending at one before the other quote -- again check the math)
			vReturn.push_back('"' + strLine.substr(Pos + 1, Pos2 - Pos - 1) + '"');
		}
	}
	// End your split code
	return vReturn;
}

std::string FormatSay(const std::string& strSenderIn, const std::string& strTextIn)
{
	std::string strTimestamp = " ";
	// Concatenate and color here...example:
	std::string strOut = "\at" + strSenderIn + SayCheckGM(strSenderIn) + "\ax \a-wsays, '\ax\aw" + strTextIn + "\ax\a-w'\ax";
	if (bSayTimestamps) {
		char* tmpbuf = new char[128];
		struct tm today = { 0 };
		time_t tm = { 0 };
		tm = time(nullptr);
		localtime_s(&today, &tm);
		strftime(tmpbuf, 128, "%m/%d %H:%M:%S", &today);
		strTimestamp += " \ar[\ax\ag";
		strTimestamp += tmpbuf;
		strTimestamp += "\ax\ar]\ax ";
		return strTimestamp + strOut;
	}
	else {
		return strOut;
	}
}

void WriteSay(const std::string& SaySender, const std::string& SayText)
{
	if (MQSayWnd == nullptr || !bSayStatus)
	{
		return;
	}
	std::string SayGMFlag = SayCheckGM(SaySender);
	std::string Line = FormatSay(SaySender, SayText);
	MQSayWnd->SetVisible(true);
	char* szProcessed = new char[MAX_STRING];
	int pos = MQToSTML(&Line[0], szProcessed, MAX_STRING - 4);
	strcat_s(szProcessed, MAX_STRING, "<br>");//we left room for this above...
	CXStr NewText(szProcessed);
	ConvertItemTags(NewText, FALSE);
	sPendingSay.push_back(NewText.Ptr->Text);
	delete[] szProcessed;
	return;
}

void DoAlerts(std::string Line)
{
	if (bSayStatus)
	{
		std::vector<std::string> strSaySplit = SplitSay(Line);
		if (strSaySplit.size() == 2) {
			WriteSay(strSaySplit[0], strSaySplit[1]);
			if (bSayAlerts)
			{
				if (strSayAlertCommand[0] == '\0')
				{
					WriteChatf("[\arMQ2Say\ax] \arError\ax: \a-wSay Detected, Alerts are turned on, but AlertCommand is not set in the INI\ax");
				}
				else if (strSayAlertCommand[0] == '/')
				{
					if (mAlertTimers.find(strSaySplit[0]) == mAlertTimers.end())
					{
						EzCommand(strSayAlertCommand);
						//mAlertTimers.insert(strSaySplit[0], std::chrono::steady_clock::now());
						mAlertTimers[strSaySplit[0]] = std::chrono::steady_clock::now();
					}
				}
				else
				{
					WriteChatf("\arMQ2Say Error\ax: AlertCommand in INI must begin with a / and be a valid EQ or MQ2 Command.");
				}
			}
		}
	}
}

void LoadSaySettings()
{
	CHAR szTemp[MAX_STRING] = { 0 };
	GetPrivateProfileString("Settings", "SayStatus", bSayStatus ? "on" : "off", szTemp, MAX_STRING, INIFileName);
	bSayStatus = (!_strnicmp(szTemp, "on", 3));
	GetPrivateProfileString("Settings", "SayDebug", bSayDebug ? "on" : "off", szTemp, MAX_STRING, INIFileName);
	bSayDebug = (!_strnicmp(szTemp, "on", 3));
	GetPrivateProfileString("Settings", "AutoScroll", bAutoScroll ? "on" : "off", szTemp, MAX_STRING, INIFileName);
	bAutoScroll = (!_strnicmp(szTemp, "on", 3));
	GetPrivateProfileString("Settings", "SaveByChar", bSaveByChar ? "on" : "off", szTemp, MAX_STRING, INIFileName);
	bSaveByChar = (!_strnicmp(szTemp, "on", 3));
	intIgnoreDelay = GetPrivateProfileInt("Settings", "IgnoreDelay", intIgnoreDelay, INIFileName);
	return;
}

void LoadSayFromINI(PCSIDLWND pWindow)
{
	CHAR szTemp[MAX_STRING] = { 0 };
	LoadSaySettings();
	sprintf_s(szSayINISection, "%s.%s", EQADDR_SERVERNAME, ((PSPAWNINFO)pLocalPlayer)->Name);
	if (!bSaveByChar)
		sprintf_s(szSayINISection, "Default");
	//left top right bottom
	int left = GetPrivateProfileInt(szSayINISection, "SayLeft", 10, INIFileName);
	if (left == 2000)
	{
		left = 10;
	}
	pWindow->SetLocation({ (LONG)left,
		(LONG)GetPrivateProfileInt(szSayINISection,"SayTop",       10,INIFileName),
		(LONG)GetPrivateProfileInt(szSayINISection,"SayRight",    410,INIFileName),
		(LONG)GetPrivateProfileInt(szSayINISection,"SayBottom",   210,INIFileName) });
	pWindow->SetLocked((GetPrivateProfileInt(szSayINISection, "Locked", 0, INIFileName) ? true : false));
	pWindow->SetFades((GetPrivateProfileInt(szSayINISection, "Fades", 0, INIFileName) ? true : false));
	pWindow->SetFadeDelay(GetPrivateProfileInt(szSayINISection, "Delay", 2000, INIFileName));
	pWindow->SetFadeDuration(GetPrivateProfileInt(szSayINISection, "Duration", 500, INIFileName));
	pWindow->SetAlpha((BYTE)GetPrivateProfileInt(szSayINISection, "Alpha", 255, INIFileName));
	pWindow->SetFadeToAlpha((BYTE)GetPrivateProfileInt(szSayINISection, "FadeToAlpha", 255, INIFileName));
	pWindow->SetBGType(GetPrivateProfileInt(szSayINISection, "BGType", 1, INIFileName));
	ARGBCOLOR col = { 0 };
	col.ARGB = pWindow->GetBGColor();
	col.A = GetPrivateProfileInt(szSayINISection, "BGTint.alpha", 255, INIFileName);
	col.R = GetPrivateProfileInt(szSayINISection, "BGTint.red", 0, INIFileName);
	col.G = GetPrivateProfileInt(szSayINISection, "BGTint.green", 0, INIFileName);
	col.B = GetPrivateProfileInt(szSayINISection, "BGTint.blue", 0, INIFileName);
	pWindow->SetBGColor(col.ARGB);
	MQSayWnd->SetSayFont(GetPrivateProfileInt(szSayINISection, "FontSize", 4, INIFileName));
	GetPrivateProfileString(szSayINISection, "WindowTitle", "Say Detection", szTemp, MAX_STRING, INIFileName);
	pWindow->CSetWindowText(szTemp);
	GetPrivateProfileString(szSayINISection, "Alerts", bSayAlerts ? "on" : "off", szTemp, MAX_STRING, INIFileName);
	int i = GetPrivateProfileString(szSayINISection, "AlertCommand", "/multiline ; /beep ; /timed 1 /beep ; /timed 2 /beep", strSayAlertCommand, MAX_STRING, INIFileName);
	if (i == 0) {
		strcpy_s(strSayAlertCommand, "/multiline ; /beep ; /timed 1 /beep ; /timed 2 /beep");
	}
	GetPrivateProfileString(szSayINISection, "Timestamps", bSayTimestamps ? "on" : "off", szTemp, MAX_STRING, INIFileName);
	//SetCXStr(&pWindow->WindowText,szTemp);
}

template <unsigned int _Size>LPSTR SafeItoa(int _Value, char(&_Buffer)[_Size], int _Radix)
{
	errno_t err = _itoa_s(_Value, _Buffer, _Radix);
	if (!err) {
		return _Buffer;
	}
	return "";
}

void SaveSayToINI(PCSIDLWND pWindow)
{
	CHAR szTemp[MAX_STRING] = { 0 };
	WritePrivateProfileString("Settings", "SayStatus", bSayStatus ? "on" : "off", INIFileName);
	WritePrivateProfileString("Settings", "SayDebug", bSayDebug ? "on" : "off", INIFileName);
	WritePrivateProfileString("Settings", "AutoScroll", bAutoScroll ? "on" : "off", INIFileName);
	WritePrivateProfileString("Settings", "SaveByChar", bSaveByChar ? "on" : "off", INIFileName);
	WritePrivateProfileString("Settings", "IgnoreDelay", SafeItoa(intIgnoreDelay, szTemp, 10), INIFileName);
	if (pWindow->IsMinimized())
	{
		WritePrivateProfileString(szSayINISection, "SayTop", SafeItoa(pWindow->GetOldLocation().top, szTemp, 10), INIFileName);
		WritePrivateProfileString(szSayINISection, "SayBottom", SafeItoa(pWindow->GetOldLocation().bottom, szTemp, 10), INIFileName);
		WritePrivateProfileString(szSayINISection, "SayLeft", SafeItoa(pWindow->GetOldLocation().left, szTemp, 10), INIFileName);
		WritePrivateProfileString(szSayINISection, "SayRight", SafeItoa(pWindow->GetOldLocation().right, szTemp, 10), INIFileName);
	}
	else
	{
		WritePrivateProfileString(szSayINISection, "SayTop", SafeItoa(pWindow->GetLocation().top, szTemp, 10), INIFileName);
		WritePrivateProfileString(szSayINISection, "SayBottom", SafeItoa(pWindow->GetLocation().bottom, szTemp, 10), INIFileName);
		WritePrivateProfileString(szSayINISection, "SayLeft", SafeItoa(pWindow->GetLocation().left, szTemp, 10), INIFileName);
		WritePrivateProfileString(szSayINISection, "SayRight", SafeItoa(pWindow->GetLocation().right, szTemp, 10), INIFileName);
	}
	WritePrivateProfileString(szSayINISection, "Locked", SafeItoa(pWindow->IsLocked(), szTemp, 10), INIFileName);
	WritePrivateProfileString(szSayINISection, "Fades", SafeItoa(pWindow->GetFades(), szTemp, 10), INIFileName);
	WritePrivateProfileString(szSayINISection, "Delay", SafeItoa(pWindow->GetFadeDelay(), szTemp, 10), INIFileName);
	WritePrivateProfileString(szSayINISection, "Duration", SafeItoa(pWindow->GetFadeDuration(), szTemp, 10), INIFileName);
	WritePrivateProfileString(szSayINISection, "Alpha", SafeItoa(pWindow->GetAlpha(), szTemp, 10), INIFileName);
	WritePrivateProfileString(szSayINISection, "FadeToAlpha", SafeItoa(pWindow->GetFadeToAlpha(), szTemp, 10), INIFileName);
	ARGBCOLOR col = { 0 };
	col.ARGB = pWindow->GetBGColor();
	WritePrivateProfileString(szSayINISection, "BGType", SafeItoa(pWindow->GetBGType(), szTemp, 10), INIFileName);
	WritePrivateProfileString(szSayINISection, "BGTint.alpha", SafeItoa(col.A, szTemp, 10), INIFileName);
	WritePrivateProfileString(szSayINISection, "BGTint.red", SafeItoa(col.R, szTemp, 10), INIFileName);
	WritePrivateProfileString(szSayINISection, "BGTint.green", SafeItoa(col.G, szTemp, 10), INIFileName);
	WritePrivateProfileString(szSayINISection, "BGTint.blue", SafeItoa(col.B, szTemp, 10), INIFileName);
	WritePrivateProfileString(szSayINISection, "FontSize", SafeItoa(MQSayWnd->FontSize, szTemp, 10), INIFileName);
	GetCXStr(pWindow->CGetWindowText(), szTemp, MAX_STRING);
	//GetCXStr(pWindow->WindowText,szTemp, MAX_STRING);
	WritePrivateProfileString(szSayINISection, "WindowTitle", szTemp, INIFileName);
	WritePrivateProfileString(szSayINISection, "Alerts", bSayAlerts ? "on" : "off", INIFileName);
	WritePrivateProfileString(szSayINISection, "AlertCommand", strSayAlertCommand, INIFileName);
	WritePrivateProfileString(szSayINISection, "Timestamps", bSayTimestamps ? "on" : "off", INIFileName);
}

void CreateSayWnd()
{
	DebugSpew("MQ2Say::CreateSayWnd()");
	if (MQSayWnd)
	{
		return;
	}
	else
	{
		MQSayWnd = new CMQSayWnd("ChatWindow");
		if (!MQSayWnd)
		{
			return;
		}
	}
	LoadSayFromINI((PCSIDLWND)MQSayWnd);
	SaveSayToINI((PCSIDLWND)MQSayWnd); // A) we're masochists, B) this creates the file if its not there..
}

void DestroySayWnd()
{
	if (MQSayWnd)
	{
		sPendingSay.clear();
		SaveSayToINI((PCSIDLWND)MQSayWnd);
		delete MQSayWnd;
		MQSayWnd = nullptr;
		ulOldVScrollPos = 0;
	}
}

void MQSayFont(PSPAWNINFO pChar, PCHAR Line)
{
	if (MQSayWnd)
	{
		if (Line[0])
		{
			int size = atoi(Line);
			if (size < 0 || size>10)
			{
				WriteChatf("Usage: /sayfont 0-10");
				return;
			}
			MQSayWnd->SetSayFont(size);
			SaveSayToINI((PCSIDLWND)MQSayWnd);
		}
	}
}
// VOID MQSayClear(PSPAWNINFO pChar, PCHAR Line)

void MQSayClear()
{
	//int sizeofCXWnd = sizeof(CXWnd);
	//int sizeofCSidlScreenWnd = sizeof(CSidlScreenWnd);
	if (MQSayWnd)
	{
		//PEQCHATWINDOW pChatw = (PEQCHATWINDOW)MQSayWnd;
		CChatWindow* saywnd = (CChatWindow*)MQSayWnd;
		saywnd->Clear();
		ulOldVScrollPos = 0;
		MQSayWnd->OutBoxLines = 0;
	}
}

void SetSayTitle(PSPAWNINFO pChar, PCHAR Line)
{
	if (MQSayWnd)
	{
		MQSayWnd->CSetWindowText(Line);
		//SetCXStr(&MQSayWnd->WindowText,Line);
	}
}

void MQSay(PSPAWNINFO pChar, PCHAR Line)
{
	PCSIDLWND pWindow = (PCSIDLWND)MQSayWnd;
	CHAR szTemp[MAX_STRING] = { 0 };
	int i = 2;
	if (MQSayWnd)
	{
		if (!_stricmp(Line, "reset"))
		{
			MQSayWnd->SetLocked(false);
			CXRect rc = { 300, 10, 600, 210 };
			((CXWnd*)MQSayWnd)->Move(rc, false);
			SaveSayToINI((PCSIDLWND)MQSayWnd);
			return;
		}
		else if (!_stricmp(Line, "clear"))
		{
			MQSayClear();
			return;
		}
	}
	char Arg[MAX_STRING] = { 0 };
	GetArg(Arg, Line, 1);
	//Display Command Syntax
	if (Arg[0] == '\0') {
		WriteChatf("[\arMQ2Say\ax] \awPlugin is:\ax %s", bSayStatus ? "\agOn\ax" : "\arOff\ax");
		WriteChatf("Usage: /mqsay <on/off>");
		WriteChatf("/mqsay [Option Name] <value/on/off>");
		WriteChatf("Valid options are Reset, Clear, Alerts, Autoscroll, IgnoreDelay, Reload, Timestamps, Title");
		return;
	}
	//user wants to adjust Say Status
	if (!_stricmp(Arg, "on")) {
		CreateSayWnd();
		bSayStatus = true;
		WriteChatf("Say Status is now: \agOn.");
		WritePrivateProfileString("Settings", "SayStatus", bSayStatus ? "on" : "off", INIFileName);
		return;
	}
	if (!_stricmp(Arg, "off")) {
		DestroySayWnd();
		bSayStatus = false;
		WriteChatf("Say Status is now: \arOff.");
		WritePrivateProfileString("Settings", "SayStatus", bSayDebug ? "on" : "off", INIFileName);
		return;
	}
	if (!_stricmp(Arg, "reload")) {
		LoadSayFromINI((PCSIDLWND)MQSayWnd);
		WriteChatf("[\arMQ2Say\ax] \a-wPlugin Settings Reloaded.\ax");
		return;
	}
	//user wants to adjust debug
	if (!_stricmp(Arg, "debug")) {
		GetArg(Arg, Line, 2);
		if (Arg[0] == '\0') {
			WriteChatf("Debug is currently: %s", (bSayDebug ? "\agOn" : "\arOff"));
			return;
		}
		//turn it on.
		if (!_stricmp(Arg, "on")) {
			bSayDebug = true;
			WriteChatf("Debug is now: \agOn.");
		}
		//turn it off.
		else if (!_stricmp(Arg, "off")) {
			bSayDebug = false;
			WriteChatf("Debug is now: \arOff.");
		}
		else {
			WriteChatf("Usage: /mqsay debug [on | off]\n IE: /mqsay debug on");
		}
		WritePrivateProfileString("Settings", "SayDebug", bSayDebug ? "on" : "off", INIFileName);
		return;
	}
	//user wants to change the window title
	if (!_stricmp(Arg, "title")) {
		GetArg(Arg, Line, 2);
		if (Arg[0] == '\0') {
			if (pWindow != nullptr) {
				GetCXStr(pWindow->CGetWindowText(), szTemp, MAX_STRING);
				WriteChatf("The Window title is currently: \ar%s\ax", szTemp);
			}
		}
		else {
			const char* szPos = GetNextArg(Line, 1);
			if (szPos[0] != '\"') {
				strcpy_s(Arg, szPos);
			}
			MQSayWnd->CSetWindowText(Arg);
			WritePrivateProfileString(szSayINISection, "WindowTitle", Arg, INIFileName);
		}
		return;
	}
	//user wants to adjust the font size
	if (!_stricmp(Arg, "font")) {
		GetArg(Arg, Line, 2);
		if (Arg[0] == '\0') {
			WriteChatf("The font size is currently: %s", MQSayWnd->FontSize);
			return;
		}
		else if (atoi(Arg) > 0) {
			MQSayWnd->SetSayFont(atoi(Arg));
		}
		else {
			WriteChatf("Usage: /mqsay font <font size>\n IE: /mqsay font 5");
		}
		WritePrivateProfileString(szSayINISection, "FontSize", SafeItoa(MQSayWnd->FontSize, szTemp, 10), INIFileName);
		return;
	}
	//user wants to adjust the Alert Ignore Timer
	if (!_stricmp(Arg, "IgnoreDelay")) {
		GetArg(Arg, Line, 2);
		if (Arg[0] == '\0') {
			WriteChatf("The Ignore Delay is currently: %i", intIgnoreDelay);
			return;
		}
		else if (atoi(Arg) >= 0) {
			intIgnoreDelay = (atoi(Arg));
			WriteChatf("The Ignore Delay is now set to: %i", intIgnoreDelay);
		}
		else {
			WriteChatf("Usage: /mqsay ignoredelay <Time In Seconds>\n IE: /mqsay ignoredelay 300");
		}
		WritePrivateProfileString("Settings", "IgnoreDelay", SafeItoa(intIgnoreDelay, szTemp, 10), INIFileName);
		return;
	}
	//user wants to audible alerts
	if (!_stricmp(Arg, "alerts")) {
		GetArg(Arg, Line, 2);
		if (Arg[0] == '\0') {
			WriteChatf("Alerts are currently: %s", (bSayAlerts ? "\agOn" : "\arOff"));
			return;
		}
		//turn it on.
		if (!_stricmp(Arg, "on")) {
			bSayAlerts = true;
			WriteChatf("Alerts are now: \agOn.");
		}
		//turn it off.
		else if (!_stricmp(Arg, "off")) {
			bSayAlerts = false;
			WriteChatf("Alerts are now: \arOff.");
		}
		else {
			WriteChatf("Usage: /mqsay alerts [on | off]\n IE: /mqsay alerts on");
		}
		WritePrivateProfileString(szSayINISection, "Alerts", bSayAlerts ? "on" : "off", INIFileName);
		return;
	}
	//user wants to display timestamps
	if (!_stricmp(Arg, "timestamps")) {
		GetArg(Arg, Line, 2);
		if (Arg[0] == '\0') {
			WriteChatf("Timestamps are currently: %s", (bSayTimestamps ? "\agOn" : "\arOff"));
			return;
		}
		//turn it on.
		if (!_stricmp(Arg, "on")) {
			bSayTimestamps = true;
			WriteChatf("Timestamps are now: \agOn.");
		}
		//turn it off.
		else if (!_stricmp(Arg, "off")) {
			bSayTimestamps = false;
			WriteChatf("Timestamps are now: \arOff.");
		}
		else {
			WriteChatf("Usage: /mqsay timestamps [on | off]\n IE: /mqsay timestamps on");
		}
		WritePrivateProfileString(szSayINISection, "Timestamps", bSayTimestamps ? "on" : "off", INIFileName);
		return;
	}
	//user wants to adjust autoscroll
	if (!_stricmp(Arg, "autoscroll")) {
		GetArg(Arg, Line, 2);
		if (Arg[0] == '\0') {
			WriteChatf("Autoscroll is currently: %s", (bAutoScroll ? "\agOn" : "\arOff"));
			return;
		}
		//turn it on.
		if (!_stricmp(Arg, "on")) {
			bAutoScroll = true;
			WriteChatf("Autoscroll is now: \agOn.");
		}
		//turn it off.
		else if (!_stricmp(Arg, "off")) {
			bAutoScroll = false;
			WriteChatf("Autoscroll is now: \arOff.");
		}
		else {
			WriteChatf("Usage: /mqsay autoscroll [on | off]\n IE: /mqsay autoscroll on");
		}
		WritePrivateProfileString("Settings", "AutoScroll", bAutoScroll ? "on" : "off", INIFileName);
		return;
	}
	//user wants to adjust SaveByChar
	if (!_stricmp(Arg, "SaveByChar")) {
		GetArg(Arg, Line, 2);
		if (Arg[0] == '\0') {
			WriteChatf("SaveByChar is currently: %s", (bSaveByChar ? "\agOn" : "\arOff"));
			return;
		}
		//turn it on.
		if (!_stricmp(Arg, "on")) {
			bSaveByChar = true;
			WriteChatf("SaveByChar is now: \agOn.");
		}
		//turn it off.
		else if (!_stricmp(Arg, "off")) {
			bSaveByChar = false;
			WriteChatf("SaveByChar is now: \arOff.");
		}
		else {
			WriteChatf("Usage: /mqsay SaveByChar [on | off]\n IE: /mqsay SaveByChar on");
		}
		WritePrivateProfileString("Settings", "SaveByChar", bSaveByChar ? "on" : "off", INIFileName);
		return;
	}
	WriteChatf("%s was not a valid option. Valid options are Reset, Clear, Alerts, Autoscroll, IgnoreDelay, Reload, Timestamps, Title", Arg);
}

PLUGIN_API VOID OnReloadUI()
{
	DebugSpewAlways("MQ2Say::OnReloadUI()");
	// redraw window when you load/reload UI
	CreateSayWnd();
}

PLUGIN_API VOID OnCleanUI()
{
	DebugSpewAlways("MQ2Say::OnCleanUI()");
	// destroy SayWnd before server select & while reloading UI
	DestroySayWnd();
}

PLUGIN_API VOID SetGameState(DWORD GameState)
{
	DebugSpew("MQ2Say::SetGameState()");
	if (GameState != GAMESTATE_CHARCREATE) RemoveMQ2KeyBind("MQ2CSCHAT");
	if (GameState == GAMESTATE_INGAME && !MQSayWnd)
	{
		// we entered the game, set up shop
		if (bSayStatus) {
			CreateSayWnd();
		}
	}
}

PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
	if (gGameState != GAMESTATE_INGAME) return 0;
	if (!bSayStatus) return 0;
	if (bSayDebug) {
		char szMsg[MAX_STRING] = { 0 };
		strcpy_s(szMsg, Line);
		if (strchr(szMsg, '\x12'))
		{
			// Message includes link (item tags/spell), must clean first
			const int len = strlen(szMsg);
			auto pszCleanOrg = std::make_unique<char[]>(len + 64);
			char* szClean = pszCleanOrg.get();
			strcpy_s(szClean, len + 64, szMsg);
			CXStr out;
#if defined(ROF2EMU) || defined(UFEMU)
			if (CXStr* str = CleanItemTags(&out, szClean)) {
#else
			if (CXStr* str = CleanItemTags(&out, szClean, false)) {
#endif
				GetCXStr(str->Ptr, szClean, len + 64);
			}
			strncpy_s(szMsg, szClean, MAX_STRING - 1);
			}
		WriteChatf("[\ar%i\ax]\a-t: \ap%s", Color, szMsg);
		}
	switch (Color) {
	case 0://Color: 0 - Task Updates
		break;
	case 2: //Color: 2 - Consider Message - Green
		break;
	case 4: //Color: 4 - Consider messages - Dark Blue
		break;
	case 6://Color: 6 - Consider message - Grey
		break;
	case 10://Color: 10 - Merchant says "Hello there, Soandso. How about a nice Iron Ration?"
		break;
	case 13://Color: 13 - Attack is on/off - Other invites to raid.
		break;
	case 15://Color: 15 - Help messages, Consider Message - Yellow (right click on the NPC to consider it, Targeted (NPC): a slatescale serpent, You have gained an ability point)
		break;
	case 18://Color: 18 - Consider message - Light blue
		break;
	case 20://Color: 20 - You regain some experience from resurrection.
		break;
	case 256://Color: 256 - Other player /say messages
		DoAlerts(Line);
		break;
	case 257://Color: 257 - Other /tell's you
		break;
	case 258://Color: 258 - Other tells /group
		break;
	case 259://Color: 259 - Guild Chat - Incoming
		break;
	case 260://Color: 260 - Other /ooc
		break;
	case 261://Color: 261 - Other /auction
		break;
	case 262://Color: 262 - Other /shout
		break;
	case 263://Color: 263 - other Emote commands/eat food messages (/cry, Chomp, chomp, chomp... Soandso takes a bite...etc)
		break;
	case 264://Color: 264 - You begin casting
		break;
	case 265://Color: 265 - You hit other (melee/melee crit)
		break;
	case 266://Color: 266 - Other hits you
		break;
	case 267://Color: 267 - You miss other
		break;
	case 268://Color: 268 - Riposte/Dodge/Other Misses You
		break;
	case 269://Color: 269 - Announcement (GM Events etc)
		break;
	case 270://Color: 270 - You have become better at [Skill]! (Number)
		break;
	case 271://Color: 271 - Song wear off message?? (Ritual Scarification wear off message)
		break;
	case 272://Color: 272 - Pet thanks for stuff you give them?
		break;
	case 273://Color: 273 - Charged mercenary upkeep/Group Invites/Joins/Banker tells you welcome to bank.
		break;
	case 274://Color: 274 - Faction hits
		break;
	case 275://Color: 275 - [Merchant] tells you, that'll be [price] for the [item]
		break;
	case 276://Color: 276 - You sold [QTY] [Item Name] to [Player] for [Amount]/You Purchased [Item Name] from [NPC] for [Amount]
		break;
	case 277://Color: 277 - Your death message.
		break;
	case 278://Color: 278 - Other PC death message.
		break;
	case 279://Color: 279 - Others Hits Other
		break;
	case 280://Color: 280 - Other Misses Other
		break;
	case 281://Color: 281 - /who
		break;
	case 282://Color: 282 - Other /yell
		break;
	case 283://Color: 283 - Your Non-Melee spell dmg.
		break;
	case 284://Color: 284 - Spell wear off messages
		break;
	case 285://Color: 285 - Coin collection/Split
		break;
	case 286://Color: 286 - Loot Related (Master looter actions, you loot)
		break;
	case 287://Color: 287 - Dice Rolls
		break;
	case 288://Color: 288 - Other Starts Casting Messages
		break;
	case 289://Color: 289 - Target is out of range, get closer/Interupt/Fizzle/other spell failure
		break;
	case 290://Color: 290 - Channel Lists after logged in
		break;
	case 291://Color: 291 - Chat Channel 1 - Incoming
		break;
	case 292://Color: 292 - Chat Channel 2 - Incoming
		break;
	case 293://Color: 293 - Chat Channel 3 - Incoming
		break;
	case 294://Color: 294 - Chat Channel 4 - Incoming
		break;
	case 295://Color: 295 - Chat Channel 5 - Incoming
		break;
	case 296://Color: 296 - Chat Channel 6 - Incoming
		break;
	case 297://Color: 297 - Chat Channel 7 - Incoming
		break;
	case 298://Color: 298 - Chat Channel 8 - Incoming
		break;
	case 299://Color: 299 - Chat Channel 9 - Incoming
		break;
	case 300://Color: 300 - Chat Channel 10 - Incoming
		break;
	case 303://Color: 303 - Errors (You must first click on the being you wish to attack, Can't hit them from here)
		break;
	case 306://Color: 306 - Enrage (Showed for pet)
		break;
	case 307://Color: 307 - Your /say messages, mob advances messages.
		break;
	case 308://Color: 308 - You tell other.
		break;
	case 309://Color: 309 - Group conversation
		break;
	case 310://Color: 310 - Guild conversation - Outgoing
		break;
	case 311://Color: 311 - you /ooc
		break;
	case 312://Color: 312 - you /auction
		break;
	case 313://Color: 313 - you shout.
		break;
	case 314://Color: 314 - /emote messages
		break;
	case 315://Color: 315 - Chat Channel 1 - Outgoing
		break;
	case 316://Color: 316 - Chat Channel 2 - Outgoing
		break;
	case 317://Color: 317 - Chat Channel 3 - Outgoing
		break;
	case 318://Color: 318 - Chat Channel 4 - Outgoing
		break;
	case 319://Color: 319 - Chat Channel 5 - Outgoing
		break;
	case 320://Color: 320 - Chat Channel 6 - Outgoing
		break;
	case 321://Color: 321 - Chat Channel 7 - Outgoing
		break;
	case 322://Color: 322 - Chat Channel 8 - Outgoing
		break;
	case 323://Color: 323 - Chat Channel 9 - Outgoing
		break;
	case 324://Color: 324 - Chat Channel 10 - Outgoing
		break;
	case 327://Color: 327 - Any /rsay
		break;
	case 328://Color: 328 - your pet misses
		break;
	case 329://Color: 329 - Damage Shield hits you.
		break;
	case 330://Color: 330 - Raid Role messages.
		break;
	case 333://Color: 333 - Item Focus messages
		break;
	case 334://Color: 334 - You gain Experience messages
		break;
	case 335://Color: 335 - You have already finished collecting [Item].
		break;
	case 336://Color: 336 - Pet Non-Melee/Pet beings to cast
		break;
	case 337://Color: 337 - Pet messages (YourPet says, "Following you master.")
		break;
	case 339://Color: 339 - Strike thru (yours and others)
		break;
	case 340://Color: 340 - You are stunned/unstunned messages.
		break;
	case 342://Color: 342 - fellowship messages
		break;
	case 343://Color: 343 - corpse emote
		break;
	case 345://Color: 345 - Guild plants banner
		break;
	case 346://Color: 346 - Mercenary tells group
		break;
	case 348://Color: 348 - Achievement - you and other.
		break;
	case 349://Color: 349 - Achievement - Guildmate
		break;
	case 358://Color: 358 - NPC Death message
		break;
	case 360://Color: 360 - Other Rolls Dice.
		break;
	case 361://Color: 361 - Injured by falling (self)
		break;
	case 362://Color: 362 - Injured by falling (other)
		break;
	case 363://Color: 363 - Your damage shield
		break;
	case 364://Color: 364 - Other's damage shield
		break;
	case 366://Color: 366 - Your [Spell Name] has been overwritten - Detrimental
		break;
	case 367://Color: 367 - Your [Spell Name] has been overwritten - Beneficial
		break;
	case 368://Color: 368 - You cannot use that command right now (clicking disc too fast)
		break;
	case 369://Color: 369 - You can use [Ability Name] again in [Time till you can use it again]
		break;
	case 370://Color: 370 - AA Reuse Timer failed
		break;
	case 371://Color: 371 - Destroy item message
		break;
	case 373://Color: 373 - Heal over time on other?
		break;
	case 374://Color: 374 - You heal other
		break;
	case 375://Color: 375 - Other buffs other/Other Heal Other
		break;
	case 376://Color: 376 - Your DoT's
		break;
	case 377://Color: 377 - Other's DoT's
		break;
	case 378://Color: 378 - Song messages - Soandso begins to sing a song. <Selo's Sonata I>
		break;
	case 379://Color: 379 - Others Non-Melee
		break;
	case 380://Color: 380 - Your spell messages
		break;
	default:
		//Don't Know these, lets see what it is.
		if (bSayDebug) WriteChatf("[\ar%i\ax]\a-t: \ap%s", Color, Line);
		break;
	}
	return 0;
	}

// This is called every time WriteChatColor is called by MQ2Main or any plugin,
// IGNORING FILTERS, IF YOU NEED THEM MAKE SURE TO IMPLEMENT THEM. IF YOU DONT
// CALL CEverQuest::dsp_chat MAKE SURE TO IMPLEMENT EVENTS HERE

PLUGIN_API VOID OnPulse()
{
	if (MQSayWnd)
	{
		if (gGameState == GAMESTATE_INGAME && MQSayWnd->GetZLayer() != 0) {
			MQSayWnd->SetZLayer(0);
		}
		if (!sPendingSay.empty())
		{
			// set 'old' to current
			ulOldVScrollPos = MQSayWnd->OutputBox->GetVScrollPos();
			// scroll down if autoscroll enabled, or current position is the bottom of SayWnd
			bool bScrollDown = bAutoScroll ? true : (MQSayWnd->OutputBox->GetVScrollPos() == MQSayWnd->OutputBox->GetVScrollMax() ? true : false);
			DWORD ThisPulse = sPendingSay.size();
			if (ThisPulse > LINES_PER_FRAME)
			{
				ThisPulse = LINES_PER_FRAME;
			}
			MQSayWnd->OutBoxLines += ThisPulse;
			if (MQSayWnd->OutBoxLines > MAX_CHAT_SIZE)
			{
				DWORD Diff = (MQSayWnd->OutBoxLines - MAX_CHAT_SIZE) + LINES_PER_FRAME;
				MQSayWnd->OutBoxLines -= Diff;
				Benchmark(bmStripFirstStmlLines, MQSayWnd->OutputBox->StripFirstSTMLLines(Diff));
			}
			for (DWORD N = 0; N < ThisPulse; N++)
			{
				if (!sPendingSay.empty())
				{
					MQSayWnd->OutputBox->AppendSTML(sPendingSay.begin()->c_str());
					sPendingSay.pop_front();
				}
				//DebugSpew("NEW: max %u - pos: %u",MQSayWnd->OutputBox->VScrollMax,MQSayWnd->OutputBox->VScrollPos);
			}
			if (bScrollDown)
			{
				// set current vscroll position to bottom
				((CXWnd*)MQSayWnd->OutputBox)->SetVScrollPos(MQSayWnd->OutputBox->GetVScrollMax());
			}
			else
			{
				// autoscroll is disabled and current vscroll position was not at the bottom, retain position
				// note: if the window is full (VScrollMax value between 9793 and 9835), this will not adjust with
				// the flushing of buffer that keeps window a certain max size
				((CXWnd*)MQSayWnd->OutputBox)->SetVScrollPos(ulOldVScrollPos);
			}
		}
		if (InHoverState())
		{
			((CXWnd*)MQSayWnd)->DoAllDrawing();
		}
		static std::chrono::steady_clock::time_point PulseTimer = std::chrono::steady_clock::now();
		if (!mAlertTimers.empty() && std::chrono::steady_clock::now() >= PulseTimer + std::chrono::seconds(1)) {
			for (auto it = mAlertTimers.cbegin(); it != mAlertTimers.cend(); /* Increment in the function */) {
				if (it->second + std::chrono::seconds(intIgnoreDelay) <= std::chrono::steady_clock::now()) {
					it = mAlertTimers.erase(it);
				}
				else {
					++it;
				}
			}
		}
	}
}

class MQ2SayType* pSayWndType = nullptr;

class MQ2SayType : public MQ2Type
{
public:
	enum SayWndMembers {
		Title = 1,
	};
	MQ2SayType() :MQ2Type("saywnd") {
		TypeMember(Title);
	}
	~MQ2SayType() {}
	bool GetMember(MQ2VARPTR VarPtr, PCHAR Member, PCHAR Index, MQ2TYPEVAR& Dest) {
		PMQ2TYPEMEMBER pMember = MQ2SayType::FindMember(Member);
		if (!pMember)
			return false;
		switch ((SayWndMembers)pMember->ID) {
		case Title:
		{
			if (MQSayWnd)
			{
				GetCXStr(MQSayWnd->CGetWindowText(), DataTypeTemp);
				//GetCXStr(MQSayWnd->WindowText, DataTypeTemp);
				Dest.Ptr = &DataTypeTemp[0];
				Dest.Type = pStringType;
				return true;
			}
			break;
		}
		default:
			break;
		}
		return false;
	}

	bool ToString(MQ2VARPTR VarPtr, PCHAR Destination) {
		if (MQSayWnd)
		{
			GetCXStr(MQSayWnd->CGetWindowText(), Destination);
		}
		else {
			strcpy_s(Destination, MAX_STRING, "");
		}
		return true;
	}
	bool FromData(MQ2VARPTR& VarPtr, MQ2TYPEVAR& Source) {
		return false;
	}
	bool FromString(MQ2VARPTR& VarPtr, PCHAR Source) {
		return false;
	}
};

BOOL dataSayWnd(PCHAR szName, MQ2TYPEVAR & Dest)
{
	Dest.DWord = 1;
	Dest.Type = pSayWndType;
	return true;
}

// Returns a string, takes a reference to a string as input (so no copy)
PLUGIN_API VOID InitializePlugin()
{
	DebugSpewAlways("Initializing MQ2Say");
	// Add commands, macro parameters, hooks, etc.
	AddMQ2Data("SayWnd", dataSayWnd);
	pSayWndType = new MQ2SayType;
	AddCommand("/mqsay", MQSay);
	bmStripFirstStmlLines = AddMQ2Benchmark("StripFirstStmlLines");
	LoadSaySettings();
	if (!bSayStatus) {
		WriteChatf("[\arMQ2Say\ax] \a-wPlugin Loaded sucessfully. Window not displayed due to the plugin being turned off in the ini. Type /mqsay on to turn it back on.\ax");
	}
	else {
		WriteChatf("[\arMQ2Say\ax] \a-wPlugin Loaded sucessfully.\ax");
	}
}

PLUGIN_API VOID ShutdownPlugin()
{
	DebugSpewAlways("Shutting down MQ2Say");
	sPendingSay.clear();
	// Remove commands, macro parameters, hooks, etc.
	RemoveCommand("/mqsay");
	RemoveMQ2Data("SayWnd");
	delete pSayWndType;
	RemoveMQ2Benchmark(bmStripFirstStmlLines);
	bmStripFirstStmlLines = 0;
	DestroySayWnd();
}
