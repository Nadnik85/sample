// MQ2Gmail.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.
// Plugin by Eqmule
// Initial version July 4th 2016
// Version 1.0 - Added xoauth2 support.
// Version 1.1 - Fixed a buffer overflow in error reporting.

#include "../MQ2Plugin.h"


PreSetup("MQ2Gmail");
PLUGIN_VERSION(1.1);
#include <Shellapi.h>
#include <wincrypt.h>
#include <Wininet.h>
#pragma comment(lib, "wininet.lib")
class MQ2GmailType *pGmailType=0;
HANDLE hMailhandle = 0;
HANDLE hEmailthread = 0;
BOOL gbSendStatus = 0;
CHAR szCode[2048] = { 0 };
CHAR bigquerybuffer[100000]={0};
typedef struct _EMAILINFO
{
	CHAR From[MAX_STRING];
	CHAR To[MAX_STRING];
	CHAR Subject[MAX_STRING];
	CHAR Message[MAX_STRING];
} EMAILINFO,*PEMAILINFO;
extern void mailsend(char *fromemailaddress, char *toemailaddress, char*subject, char *message, char*access_token);
void HttpHelper(char *postserver, char *postverb, char *postobject, char *postlogin, char *postpasswd)
{
	DWORD dwError, dwErrorCode;
	HINTERNET hOpenHandle, hConnectHandle, hResourceHandle;


	if (hOpenHandle = InternetOpen("mq2gmail 1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)) {
		if (hConnectHandle = InternetConnect(hOpenHandle, postserver, INTERNET_DEFAULT_HTTPS_PORT, postlogin, postpasswd, INTERNET_SERVICE_HTTP, 0, 0)) {
			if (hResourceHandle = HttpOpenRequest(hConnectHandle, postverb, postobject, NULL, NULL, NULL, INTERNET_FLAG_KEEP_CONNECTION, 0)) {
			resend:
				HttpSendRequest(hResourceHandle, NULL, 0, NULL, 0);
				dwErrorCode = hResourceHandle ? ERROR_SUCCESS : GetLastError();
				dwError = InternetErrorDlg(GetDesktopWindow(), hResourceHandle, dwErrorCode, FLAGS_ERROR_UI_FILTER_FOR_ERRORS | FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS | FLAGS_ERROR_UI_FLAGS_GENERATE_DATA, NULL);
				if (dwError == ERROR_INTERNET_FORCE_RETRY) {
					Sleep(2000);
					goto resend;
				}
				DWORD dwBytesRead;
				CHAR buffer[1024] = { 0 };
				BOOL bRead = InternetReadFile(hResourceHandle, buffer, sizeof(buffer), &dwBytesRead);
				InternetCloseHandle(hResourceHandle);
			}
			InternetCloseHandle(hConnectHandle);
		}
		InternetCloseHandle(hOpenHandle);
	}

}
char *HttpQuery(char *postserver, char *postverb, char *postobject, char *postlogin, char *postpasswd)
{
	DWORD dwError, dwErrorCode;
	HINTERNET hOpenHandle, hConnectHandle, hResourceHandle;

	if (hOpenHandle = InternetOpen("mq2gmail 1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)) {
		if (hConnectHandle = InternetConnect(hOpenHandle, postserver, INTERNET_DEFAULT_HTTPS_PORT, postlogin, postpasswd, INTERNET_SERVICE_HTTP, 0, 0)) {
			if (hResourceHandle = HttpOpenRequest(hConnectHandle, postverb, postobject, NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0)) {
			resend:
				HttpSendRequest(hResourceHandle, NULL, 0, NULL, 0);
				dwErrorCode = hResourceHandle ? ERROR_SUCCESS : GetLastError();
				dwError = InternetErrorDlg(GetDesktopWindow(), hResourceHandle, dwErrorCode, FLAGS_ERROR_UI_FILTER_FOR_ERRORS | FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS | FLAGS_ERROR_UI_FLAGS_GENERATE_DATA, NULL);
				if (dwError == ERROR_INTERNET_FORCE_RETRY) {
					Sleep(2000);
					goto resend;
				}
				DWORD dwBytesRead;
				BOOL bRead = InternetReadFile(hResourceHandle, bigquerybuffer, sizeof(bigquerybuffer), &dwBytesRead);
				InternetCloseHandle(hResourceHandle);
			}
			InternetCloseHandle(hConnectHandle);
		}
		InternetCloseHandle(hOpenHandle);
	}
	return bigquerybuffer;
}
class CAccessTokenWnd *pAccessTokenWindow=0;
class CAccessTokenWnd : public CCustomWnd
{
public:
    CAccessTokenWnd(char *Template):CCustomWnd(Template)
    {
        SetWndNotification(CAccessTokenWnd);
		((CXWnd*)this)->SetWindowTextA(CXStr("XOAUTH2 code needed"));
        InputBox=(CTextEntryWnd*)GetChildItem("TEW_Entry");
        TEW_Prompt=(CLabel*)GetChildItem("TEW_Prompt");
		((CXWnd*)TEW_Prompt)->SetWindowTextA(CXStr("In order to allow the mq2gmail plugin sending mail\nYou need to copy and paste your Gmail Access Code here:"));
        OK_Button=(CButtonWnd*)GetChildItem("TEW_OKButton");
        CANCEL_Button=(CButtonWnd*)GetChildItem("TEW_CancelButton");
		this->dShow = 0;
		this->BGColor = 0x00000000;
    }

    ~CAccessTokenWnd()
    {
    }

    int WndNotification(CXWnd *pWnd, unsigned int Message, void *unknown)
    {    
		if (pWnd == (CXWnd *)OK_Button) {
			if (Message == XWM_LMOUSEUP) {
				if (InputBox && InputBox->WindowText) {
					GetCXStr(InputBox->WindowText, szCode);
					WriteChatf("Your accesscode (%s) has been saved.",szCode);
					pAccessTokenWindow->dShow = 0;
					return 1;
				}
				Sleep(0);
			}
		}
		if (pWnd == (CXWnd *)CANCEL_Button) {
			if (Message == XWM_LMOUSEUP) {
				pAccessTokenWindow->dShow = 0;
				return 1;
			}
		}
        if (pWnd==0)
        {
            if (Message==XWM_CLOSE)
            {
               pAccessTokenWindow->dShow = 0;
                return 1;
            }
        }
        return CSidlScreenWnd::WndNotification(pWnd,Message,unknown);
    };
	
    CTextEntryWnd *InputBox;
	CButtonWnd *OK_Button;
    CButtonWnd *CANCEL_Button;
    CLabel *TEW_Prompt;
};
BOOL WasCodeValid(char *response)
{
	CHAR szOuter[2048] = { 0 };
	strcpy_s(szOuter, response);
	if (char *pDest = strstr(szOuter, "access_token")) {
		return TRUE;
	}
	return FALSE;
}
BOOL GetTokenString(char *response,char*search, char*out,size_t outlen)
{
	CHAR szOuter[2048] = { 0 };
	CHAR thefinalstring[2048] = { 0 };
	CHAR thefinalstring2[2048] = { 0 };
	strcpy_s(szOuter, response);
	if (char *pDest = strstr(szOuter, search)) {
		pDest[0] = '\0';
		pDest += strlen(search)+3;
		strcpy_s(szOuter, pDest);
		if (pDest = strchr(szOuter, '\"')) {
			pDest[0] = '\0';
			strcpy_s(out, outlen, szOuter);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL GetAccessTokenString(char *email, char*accesstoken, char*TokenOut, size_t lenout)
{
	CHAR szinifile[MAX_STRING] = { 0 };
	CHAR szToken[MAX_STRING] = { 0 };
	sprintf_s(szinifile, "%s\\mq2gmail.ini", gszINIPath);
	CHAR szBuffer[2048] = { 0 };
	CHAR thefinalstring[2048] = { 0 };
	CHAR thefinalstring2[2048] = { 0 };
	if (accesstoken && accesstoken[0]) {
		sprintf_s(thefinalstring, "user=%s\x01", email);
		sprintf_s(thefinalstring2, "auth=Bearer %s\x01\x01", accesstoken);
		strcat_s(thefinalstring, thefinalstring2);
		DWORD lul_len = strlen(thefinalstring);
		DWORD out_len = sizeof(szBuffer);
		if (CryptBinaryToStringA((BYTE*)thefinalstring, lul_len, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, szBuffer, &out_len)) {
			strcpy_s(TokenOut, lenout, szBuffer);
			return TRUE;
		}
	}
	return FALSE;
}
BOOL EncryptData(DATA_BLOB *DataIn, DATA_BLOB *DataOut)
{
	return CryptProtectData(DataIn, NULL, NULL, NULL, NULL, 0, DataOut);
}
BOOL DecryptData(DATA_BLOB *DataIn, DATA_BLOB *DataOut)
{
	return CryptUnprotectData(DataIn, NULL, NULL, NULL, NULL, 0, DataOut);
}
int BlobToStr(DATA_BLOB *Blob, LPSTR szOut)
{
	//--------------------------------------------------------------------
	// Parameters passed are:
	//    pv is the array of BYTES to be converted.
	//    cb is the number of BYTEs in the array.
	//    sz is a pointer to the string to be returned.

	BYTE* pb = Blob->pbData;
	DWORD i = 0;
	int b;
	for (; i < Blob->cbData; i++) {
		b = (*pb & 0xF0) >> 4;
		*szOut++ = (CHAR)((b <= 9) ? b + '0' : (b - 10) + 'A');
		b = *pb & 0x0F;
		*szOut++ = (CHAR)((b <= 9) ? b + '0' : (b - 10) + 'A');
		pb++;
	}
	*szOut++ = 0;
	return Blob->cbData;
}
int StrToBlobA(LPCSTR szIn, DATA_BLOB *BlobOut)
{
	size_t len = strlen(szIn);
	BlobOut->pbData = (BYTE *)LocalAlloc(LPTR, (len)+1);
	char *szArg = new char[len + 1];
	strcpy_s(szArg, len + 1, szIn);
	BYTE CurByte = 0;
	DWORD out = 0;
	_strlwr_s(szArg, len + 1);
	for (int count = 0; szArg[count]; count += 2, out++)
	{
		if (((szArg[count]<'0' || szArg[count]>'9') && (szArg[count]<'a' || szArg[count]>'f')) || ((szArg[count + 1]<'0' || szArg[count + 1]>'9') && (szArg[count + 1]<'a' || szArg[count + 1]>'f')))
			break;

		if (szArg[count] >= '0' && szArg[count] <= '9') CurByte = szArg[count] - 0x30;
		else if (szArg[count] >= 'a' && szArg[count] <= 'f') CurByte = szArg[count] - 87;

		CurByte <<= 4;

		if (szArg[count + 1] >= '0' && szArg[count + 1] <= '9') CurByte |= szArg[count + 1] - 0x30;
		else if (szArg[count + 1] >= 'a' && szArg[count + 1] <= 'f') CurByte |= szArg[count + 1] - 87;

		BlobOut->pbData[out] = CurByte;
	}
	BlobOut->cbData = out;
	BlobOut->pbData[out++] = '\0';
	delete[] szArg;
	return BlobOut->cbData;
}
DWORD GetTokenFromIni(_In_opt_ LPCSTR lpAppName, _In_opt_ LPCSTR lpKeyName, _In_opt_ LPCSTR lpDefault, _Out_writes_to_opt_(nSize, return +1) LPSTR lpReturnedString, _In_ DWORD nSize, _In_opt_ LPCSTR lpFileName)
{
	DWORD ret = GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
	if (ret && lpReturnedString && lpReturnedString[0]) {
		DATA_BLOB BlobIn = { 0 };
		DATA_BLOB BlobOut = { 0 };
		if(StrToBlobA(lpReturnedString, &BlobIn)) {
			if (DecryptData(&BlobIn, &BlobOut)) {
				CHAR szOut[2048] = { 0 };
				memcpy_s(lpReturnedString, nSize, (char*)BlobOut.pbData, BlobOut.cbData);
				lpReturnedString[BlobOut.cbData] = '\0';
				ret = strlen(lpReturnedString);
				LocalFree(BlobIn.pbData);//always remember to free this (MSDN)
				LocalFree(BlobOut.pbData);//always remember to free this (MSDN)
				return ret;
			}
		}
		LocalFree(BlobIn.pbData);//always remember to free this (MSDN)
		LocalFree(BlobOut.pbData);//always remember to free this (MSDN)
	}
	return ret;
}
BOOL WriteTokenToIni( _In_opt_ LPCSTR lpAppName, _In_opt_ LPCSTR lpKeyName, _In_opt_ LPCSTR lpString, _In_opt_ LPCSTR lpFileName)
{
	DATA_BLOB BlobIn = { 0 };
	DATA_BLOB BlobOut = { 0 };
	BlobIn.cbData = strlen(lpString);
	BlobIn.pbData = (PBYTE)LocalAlloc(LPTR, BlobIn.cbData + 1);
	strcpy_s((char*)BlobIn.pbData, BlobIn.cbData + 1, lpString);
	if (EncryptData(&BlobIn, &BlobOut)) {
		CHAR szOut[2048] = { 0 };
		if (BlobToStr(&BlobOut, szOut)) {
			LocalFree(BlobIn.pbData);//always remember to free this (MSDN)
			LocalFree(BlobOut.pbData);//always remember to free this (MSDN)
			return WritePrivateProfileString(lpAppName, lpKeyName, szOut, lpFileName);
		}
	}
	LocalFree(BlobIn.pbData);//always remember to free this (MSDN)
	LocalFree(BlobOut.pbData);//always remember to free this (MSDN)
	return FALSE;
}
BOOL GetAccessTokenFromCode(char *email, char*code, char*TokenOut, size_t lenout)
{
	CHAR szinifile[MAX_STRING] = { 0 };
	CHAR szToken[MAX_STRING] = { 0 };
	sprintf_s(szinifile, "%s\\mq2gmail.ini", gszINIPath);
	CHAR szBuffer[2048] = { 0 };
	sprintf_s(szBuffer, "/oauth2/v4/token?code=%s&client_id=898122540026-lojdo6lso717o3h16vajl63n1ei8hnq9.apps.googleusercontent.com&client_secret=RL1hHigh_r_vpmdHfvK6wWqP&redirect_uri=urn:ietf:wg:oauth:2.0:oob&grant_type=authorization_code",code);
	char *outer = HttpQuery("www.googleapis.com", "POST", szBuffer, NULL, NULL);
	CHAR szOut[2048] = { 0 };
	CHAR errorstr[2048] = { 0 };

	if (outer) {
		if(!WasCodeValid(outer)) {
			CHAR accesstokenstr[2048] = { 0 };
			GetTokenString(outer, "error_description\"",errorstr,sizeof(errorstr));
			WriteChatfSafe("Accesstoken Deleted.");
			WritePrivateProfileString(email, "accesstoken", NULL, szinifile);
			if(errorstr[0])
				WriteChatfSafe("%s",errorstr);
			return FALSE;
		}
		CHAR accesstokenstr[2048] = { 0 };
		CHAR refreshtokenstr[2048] = { 0 };
		GetTokenString(outer, "access_token\"",accesstokenstr,sizeof(accesstokenstr));
		GetTokenString(outer, "refresh_token\"",refreshtokenstr,sizeof(refreshtokenstr));
		if (refreshtokenstr[0]) {
			WriteChatfSafe("Refreshtoken Granted.");
			WriteTokenToIni(email, "refreshtoken", refreshtokenstr, szinifile);
		}
		if (accesstokenstr[0]) {
			strcpy_s(TokenOut, lenout, accesstokenstr);
			WriteChatfSafe("Updated the Accesstoken.");
			WriteTokenToIni(email, "accesstoken", accesstokenstr, szinifile);
			return TRUE;
		}
	}
	return FALSE;
}
BOOL GetAccessTokenFromRefreshToken(char *email, char*code, char*TokenOut, size_t lenout)
{
	CHAR szinifile[MAX_STRING] = { 0 };
	CHAR szToken[MAX_STRING] = { 0 };
	sprintf_s(szinifile, "%s\\mq2gmail.ini", gszINIPath);
	CHAR szBuffer[2048] = { 0 };

	sprintf_s(szBuffer, "/oauth2/v4/token?refresh_token=%s&client_id=898122540026-lojdo6lso717o3h16vajl63n1ei8hnq9.apps.googleusercontent.com&client_secret=RL1hHigh_r_vpmdHfvK6wWqP&redirect_uri=urn:ietf:wg:oauth:2.0:oob&grant_type=refresh_token",code);
	char *outer = HttpQuery("www.googleapis.com", "POST", szBuffer, NULL, NULL);
	CHAR szOut[2048] = { 0 };
	CHAR thefinalstring[2048] = { 0 };
	CHAR thefinalstring2[2048] = { 0 };
	CHAR errorstr[2048] = { 0 };

	if (outer) {
		if (!WasCodeValid(outer)) {
			CHAR accesstokenstr[2048] = { 0 };
			GetTokenString(outer, "error_description\"", errorstr, sizeof(errorstr));
			WriteChatfSafe("Accesstoken Deleted.");
			WritePrivateProfileString(email, "accesstoken", NULL, szinifile);
			if (errorstr[0])
				WriteChatfSafe("%s", errorstr);
			return FALSE;
		}
		CHAR accesstokenstr[2048] = { 0 };
		CHAR refreshtokenstr[2048] = { 0 };
		GetTokenString(outer, "access_token\"", accesstokenstr, sizeof(accesstokenstr));
		GetTokenString(outer, "refresh_token\"", refreshtokenstr, sizeof(refreshtokenstr));
		if (refreshtokenstr[0]) {
			WriteChatfSafe("Refreshtoken Updated.");
			WriteTokenToIni(email, "refreshtoken", refreshtokenstr, szinifile);
		}
		if (accesstokenstr[0]) {
			WriteChatfSafe("Accesstoken Updated.");
			strcpy_s(TokenOut, lenout, accesstokenstr);
			WriteTokenToIni(email, "accesstoken", accesstokenstr, szinifile);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL IsAccessTokenValid(char *email, char*accesstoken)
{
	CHAR szinifile[MAX_STRING] = { 0 };
	CHAR szToken[MAX_STRING] = { 0 };
	sprintf_s(szinifile, "%s\\mq2gmail.ini", gszINIPath);
	CHAR szBuffer[2048] = { 0 };
	sprintf_s(szBuffer, "/oauth2/v3/tokeninfo?access_token=%s", accesstoken);
	char *outer = HttpQuery("www.googleapis.com", "POST", szBuffer, NULL, NULL);
	CHAR szOut[2048] = { 0 };
	CHAR thefinalstring[2048] = { 0 };
	CHAR thefinalstring2[2048] = { 0 };
	CHAR errorstr[2048] = { 0 };
	if (outer) {
		//CHAR exp[2048] = { 0 };
		CHAR scope[2048] = { 0 };
		CHAR expires[2048] = { 0 };
		//GetTokenString(outer, "exp\"", exp, sizeof(exp));
		GetTokenString(outer, "scope\"", scope, sizeof(scope));
		GetTokenString(outer, "expires_in\"", expires, sizeof(expires));
		if (expires[0] && !_stricmp(scope, "https://mail.google.com/")) {
			int exptime = atoi(expires);
			if (exptime > 60)
				return TRUE;
		}
	}
	return FALSE;
}
DWORD __stdcall MailThread(PVOID pData)
{
	lockit lk(hMailhandle, "MailThread");
	if (PEMAILINFO pei = (PEMAILINFO)pData) {
		CHAR szFrom[MAX_STRING] = { 0 };
		CHAR szTo[MAX_STRING] = { 0 };
		CHAR szSubject[MAX_STRING] = { 0 };
		CHAR szMessage[MAX_STRING] = { 0 };
		strcpy_s(szFrom, pei->From);
		strcpy_s(szTo, pei->To);
		strcpy_s(szSubject, pei->Subject);
		strcpy_s(szMessage, pei->Message);
		LocalFree(pei);

		CHAR szinifile[MAX_STRING] = { 0 };
		CHAR szAccessToken[MAX_STRING] = { 0 };
		CHAR szRefreshToken[MAX_STRING] = { 0 };
		sprintf_s(szinifile, "%s\\mq2gmail.ini", gszINIPath);
		GetTokenFromIni(szFrom, "refreshtoken", "", szRefreshToken, sizeof(szRefreshToken), szinifile);
		GetTokenFromIni(szFrom, "accesstoken", "", szAccessToken, sizeof(szAccessToken), szinifile);

		if (szAccessToken[0]) {
			if (IsAccessTokenValid(szFrom, szAccessToken)) {
				CHAR szTokenString[2048] = { 0 };
				if (GetAccessTokenString(szFrom, szAccessToken, szTokenString, sizeof(szTokenString))) {
					gbSendStatus = 0;
					mailsend(szFrom, szTo, szSubject, szMessage, szTokenString);
					hEmailthread = 0;
					return 0;
				}
			}
		}
		if (szRefreshToken[0]) {
			if (GetAccessTokenFromRefreshToken(szFrom, szRefreshToken, szAccessToken, sizeof(szAccessToken))) {
				if (IsAccessTokenValid(szFrom, szAccessToken)) {
					CHAR szTokenString[2048] = { 0 };
					if (GetAccessTokenString(szFrom, szAccessToken, szTokenString, sizeof(szTokenString))) {
						gbSendStatus = 0;
						mailsend(szFrom, szTo, szSubject, szMessage, szTokenString);
						hEmailthread = 0;
						return 0;
					}
				}
			}
		}
		else if (szAccessToken[0] == '\0' && szCode[0] == '\0') {
			//we need the oauth2 token from the user now...
			if (pAccessTokenWindow) {
				pAccessTokenWindow->dShow = 1;
			}
			ShellExecute(NULL, "open", "https://accounts.google.com/o/oauth2/auth?scope=https://mail.google.com/&redirect_uri=urn:ietf:wg:oauth:2.0:oob&response_type=code&client_id=898122540026-lojdo6lso717o3h16vajl63n1ei8hnq9.apps.googleusercontent.com", NULL, NULL, SW_SHOWNORMAL);
			hEmailthread = 0;
			return 0;
		}
		else {
			//if we got this far we better have a code...
			if (szCode[0]) {
				GetAccessTokenFromCode(szFrom, szCode, szAccessToken, sizeof(szAccessToken));
				szCode[0] = '\0';
				if (szAccessToken[0]) {
					if (IsAccessTokenValid(szFrom, szAccessToken)) {
						CHAR szTokenString[2048] = { 0 };
						if (GetAccessTokenString(szFrom, szAccessToken, szTokenString, sizeof(szTokenString))) {
							gbSendStatus = 0;
							mailsend(szFrom, szTo, szSubject, szMessage, szTokenString);
							hEmailthread = 0;
							return 0;
						}
					}
				}
			}
		}
	}
	hEmailthread = 0;
	return 0;
}
void GmailCmd(PSPAWNINFO pSpawn, char* szLine)
{
	if (hEmailthread)
		return;
	gbSendStatus = 0;
	if (szLine && szLine[0]) {
		CHAR szinifile[MAX_STRING] = { 0 };
		CHAR szAccessToken[MAX_STRING] = { 0 };
		CHAR szRefreshToken[MAX_STRING] = { 0 };
		sprintf_s(szinifile, "%s\\mq2gmail.ini", gszINIPath);
		CHAR szFrom[MAX_STRING] = { 0 };
		CHAR szTo[MAX_STRING] = { 0 };
		CHAR szSubject[MAX_STRING] = { 0 };
		CHAR szMessage[MAX_STRING] = { 0 };
		GetArg(szFrom, szLine, 1);
		GetArg(szTo, szLine, 2);
		GetArg(szSubject, szLine, 3);
		GetArg(szMessage, szLine, 4);
		if (szFrom[0]!='\0' && szTo[0] == '\0' && szSubject[0] == '\0' && szMessage[0] == '\0') {
			//its a message only mail
			strcpy_s(szMessage, szFrom);
			GetPrivateProfileString("Default", "FromEmail", "", szFrom, sizeof(szFrom), szinifile);
			GetPrivateProfileString("Default", "ToEmail", "", szTo, sizeof(szTo), szinifile);
			GetPrivateProfileString("Default", "Subject", "", szSubject, sizeof(szSubject), szinifile);
		} else if (szFrom[0] != '\0' && szTo[0] != '\0' && szSubject[0] == '\0' && szMessage[0] == '\0') {
			//its a subject and message only mail
			strcpy_s(szSubject, szFrom);
			strcpy_s(szMessage, szTo);
			GetPrivateProfileString("Default", "FromEmail", "", szFrom, sizeof(szFrom), szinifile);
			GetPrivateProfileString("Default", "ToEmail", "", szTo, sizeof(szTo), szinifile);
		} else if (szFrom[0] != '\0' && szTo[0] != '\0' && szSubject[0] != '\0' && szMessage[0] == '\0') {
			//its a To, subject and message only mail
			strcpy_s(szMessage, szSubject);
			strcpy_s(szSubject, szTo);
			strcpy_s(szTo, szFrom);
			GetPrivateProfileString("Default", "FromEmail", "", szFrom, sizeof(szFrom), szinifile);
		}
		if (szFrom[0] && szTo[0] && szSubject[0] && szMessage[0]) {
			PEMAILINFO pei = (PEMAILINFO)LocalAlloc(LPTR, sizeof(EMAILINFO));
			strcpy_s(pei->From, szFrom);
			strcpy_s(pei->To, szTo);
			strcpy_s(pei->Subject, szSubject);
			strcpy_s(pei->Message, szMessage);
			DWORD nThreadID = 0;
			hEmailthread = CreateThread(NULL, 0, MailThread, pei, 0, &nThreadID);
			return;
		}
	}
	WriteChatf("/gmail usage:");
	WriteChatf("/gmail <FromEmailAddress> <ToEmailAddress> <Subject> <Message>");
	WriteChatf("Example: /gmail someone@adomainthathasgmailaccess.com someone@anyemailaddress.com \"Hi There\" \"This is a message\"");
}
VOID DeleteAccessTokenWindow()
{
    if (pAccessTokenWindow)
    {
        delete pAccessTokenWindow;
        pAccessTokenWindow=0;
    }
}
void CreateMyWindow(void)
{
	if ((gGameState == GAMESTATE_INGAME || gGameState == GAMESTATE_CHARSELECT) && pCharSpawn) {
		if (pAccessTokenWindow)
			return;
		//	if (MyWnd) DestroyMyWindow();
		if (pSidlMgr->FindScreenPieceTemplate("TextEntryWnd")) {
			pAccessTokenWindow = new CAccessTokenWnd("TextEntryWnd");
			/*if (pAccessTokenWindow) {
				//ReadWindowINI((PCSIDLWND)MyWnd);
				//WriteWindowINI((PCSIDLWND)MyWnd);
				//MyWnd->SetCheckMarks();
				//MyWnd->HideWin();
			}*/
		}
	}
}
class MQ2GmailType : public MQ2Type
{
public:
	enum GmailMembers {
		Status = 1,
		SendComplete = 2,
	};
	MQ2GmailType():MQ2Type("Gmail") {
		TypeMember(Status);
		TypeMember(SendComplete);
	}
	bool MQ2GmailType::GETMEMBER() {
		PMQ2TYPEMEMBER pMember = MQ2GmailType::FindMember(Member);
		if (pMember) {
			switch ((GmailMembers)pMember->ID) {
				case Status:											// FollowState, 0 = off, 1 = Following, 2 = Playing, 3 = Recording
				{
					switch (gbSendStatus)
					{
						case 1:
						{
							strcpy_s(DataTypeTemp, "ERROR");
							break;
						}
						case 2:
						{
							strcpy_s(DataTypeTemp, "SUCCESS");
							break;
						}
						default:
						{
							strcpy_s(DataTypeTemp, "IDLE");
							break;
						}
					}
					Dest.Ptr = &DataTypeTemp[0];
					Dest.Type = pStringType;
					return true;
				}
				case SendComplete:
				{
					Dest.DWord = false;
					if(gbSendStatus==2)
						Dest.DWord = true;
					Dest.Type = pBoolType;
					return true;
				}
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
	~MQ2GmailType() {

	}
};
BOOL dataGmail(PCHAR szName, MQ2TYPEVAR &Dest) {
	Dest.DWord=1;
	Dest.Type=pGmailType;
	return true;
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	hMailhandle = CreateMutex(NULL, FALSE, NULL);
    DebugSpewAlways("Initializing MQ2Gmail");
	CHAR szinifile[MAX_STRING] = { 0 };
	CHAR szAccessToken[MAX_STRING] = { 0 };
	CHAR szRefreshToken[MAX_STRING] = { 0 };
	sprintf_s(szinifile, "%s\\mq2gmail.ini", gszINIPath);
	CHAR szFrom[MAX_STRING] = { 0 };
	CHAR szTo[MAX_STRING] = { 0 };
	CHAR szSubject[MAX_STRING] = { 0 };
	GetPrivateProfileString("Default", "FromEmail", "", szFrom, sizeof(szFrom), szinifile);
	GetPrivateProfileString("Default", "ToEmail", "", szTo, sizeof(szTo), szinifile);
	GetPrivateProfileString("Default", "Subject", "", szSubject, sizeof(szSubject), szinifile);
	if (szFrom[0] == '\0') {
		WritePrivateProfileString("Default", "FromEmail", "", szinifile);
	}
	if (szTo[0] == '\0') {
		WritePrivateProfileString("Default", "ToEmail", "", szinifile);
	}
	if (szSubject[0] == '\0') {
		WritePrivateProfileString("Default", "Subject", "", szinifile);
	}
	CreateMyWindow();

    //Add commands, MQ2Data items, hooks, etc.
    AddCommand("/gmail",GmailCmd,0,1,0);
	pGmailType = new MQ2GmailType;
	AddMQ2Data("Gmail",dataGmail);
    //AddXMLFile("MQUI_MyXMLFile.xml");
    //bmMyBenchmark=AddMQ2Benchmark("My Benchmark Name");
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
    DebugSpewAlways("Shutting down MQ2Gmail");

    //Remove commands, MQ2Data items, hooks, etc.
    //RemoveMQ2Benchmark(bmMyBenchmark);
    RemoveCommand("/gmail");
	delete pGmailType;
	RemoveMQ2Data("Gmail");
	DeleteAccessTokenWindow();
	if (hMailhandle) {
		ReleaseMutex(hMailhandle);
		CloseHandle(hMailhandle);
		hMailhandle = 0;
	}
	if (hEmailthread) {
		DWORD retwait = WaitForSingleObject(hEmailthread, 20000);
		if (retwait == WAIT_TIMEOUT) {
			TerminateThread(hEmailthread, 0);
		}
	}
    //RemoveXMLFile("MQUI_MyXMLFile.xml");
}
// Called once directly before shutdown of the new ui system, and also
// every time the game calls CDisplay::CleanGameUI()
PLUGIN_API VOID OnCleanUI(VOID)
{
    DebugSpewAlways("MQ2Gmail::OnCleanUI()");
	DeleteAccessTokenWindow();
    // destroy custom windows, etc
}

// Called once directly after the game ui is reloaded, after issuing /loadskin
PLUGIN_API VOID OnReloadUI(VOID)
{
	CreateMyWindow();
    // recreate custom windows, etc
}

// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState)
{
    DebugSpewAlways("MQ2Gmail::SetGameState()");
	CreateMyWindow();
    // create custom windows if theyre not set up, etc
}


// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
    // DONT leave in this debugspew, even if you leave in all the others
    //DebugSpewAlways("MQ2Gmail::OnPulse()");
}

