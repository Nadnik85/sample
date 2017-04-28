
#include <windows.h>
#define PRIVATEEMU
#ifndef PRIVATEEMU
#include <Wininet.h>
#pragma comment(lib, "wininet.lib")
#include <wincrypt.h>
#include <Uxtheme.h>
#include <Ole2.h>
#pragma comment(lib, "UxTheme.lib")
#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Comctl32.lib")

CHAR bigquerybuffer[1024] = { 0 };
TRACKMOUSEEVENT tme = { 0 };
BOOL _bSelected = 0;
BOOL _bHoover = 0;
BOOL _bTM = 0;
HRGN hRgn = 0;
//HINTERNET 
void HttpHelper(char *postserver, char *postverb, char *postobject, char *postlogin, char *postpasswd)
{
	DWORD dwError, dwErrorCode;
	HINTERNET hOpenHandle, hConnectHandle, hResourceHandle;


	if (hOpenHandle = InternetOpen("EqPoster 1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)) {
		if (hConnectHandle = InternetConnect(hOpenHandle, postserver, INTERNET_DEFAULT_HTTP_PORT, postlogin, postpasswd, INTERNET_SERVICE_HTTP, 0, 0)) {
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

	if (hOpenHandle = InternetOpen("EQPoster 1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0)) {
		if (hConnectHandle = InternetConnect(hOpenHandle, postserver, INTERNET_DEFAULT_HTTP_PORT, postlogin, postpasswd, INTERNET_SERVICE_HTTP, 0, 0)) {
			if (hResourceHandle = HttpOpenRequest(hConnectHandle, postverb, postobject, NULL, NULL, NULL, INTERNET_FLAG_KEEP_CONNECTION, 0)) {
			resend:
				HttpSendRequest(hResourceHandle, NULL, 0, NULL, 0);
				dwErrorCode = hResourceHandle ? ERROR_SUCCESS : GetLastError();
				dwError = InternetErrorDlg(GetDesktopWindow(), hResourceHandle, dwErrorCode, FLAGS_ERROR_UI_FILTER_FOR_ERRORS | FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS | FLAGS_ERROR_UI_FLAGS_GENERATE_DATA, NULL);
				if (dwError == ERROR_INTERNET_FORCE_RETRY) {
					Sleep(2000);
					goto resend;
				}
				ZeroMemory(bigquerybuffer, sizeof(bigquerybuffer));
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
char* md5(char* data, DWORD *result)
{
	DWORD dwStatus = 0;
	DWORD cbHash = 16;
	int i = 0;
	HCRYPTPROV cryptProv;
	HCRYPTHASH cryptHash;
	BYTE hash[16];
	char *hex = "0123456789abcdef";
	char *strHash;
	strHash = (char*)malloc(500);
	memset(strHash, '\0', 500);
	if (!CryptAcquireContext(&cryptProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		dwStatus = GetLastError();
		printf("CryptAcquireContext failed: %d\n", dwStatus);
		*result = dwStatus;
		return NULL;
	}
	if (!CryptCreateHash(cryptProv, CALG_MD5, 0, 0, &cryptHash))
	{
		dwStatus = GetLastError();
		printf("CryptCreateHash failed: %d\n", dwStatus);
		CryptReleaseContext(cryptProv, 0);
		*result = dwStatus;
		return NULL;
	}
	if (!CryptHashData(cryptHash, (BYTE*)data, strlen(data), 0))
	{
		dwStatus = GetLastError();
		printf("CryptHashData failed: %d\n", dwStatus);
		CryptReleaseContext(cryptProv, 0);
		CryptDestroyHash(cryptHash);
		*result = dwStatus;
		return NULL;
	}
	if (!CryptGetHashParam(cryptHash, HP_HASHVAL, hash, &cbHash, 0))
	{
		dwStatus = GetLastError();
		printf("CryptGetHashParam failed: %d\n", dwStatus);
		CryptReleaseContext(cryptProv, 0);
		CryptDestroyHash(cryptHash);
		*result = dwStatus;
		return NULL;
	}
	for (i = 0; i < (int)cbHash; i++)
	{
		strHash[i * 2] = hex[hash[i] >> 4];
		strHash[(i * 2) + 1] = hex[hash[i] & 0xF];
	}
	CryptReleaseContext(cryptProv, 0);
	CryptDestroyHash(cryptHash);
	return strHash;
}
HWND hMainWindow = 0;
HWND hLoginStatic = 0;
HWND hLoginEdit = 0;
HWND hPassStatic = 0;
HWND hPassEdit = 0;
HWND hAuthButton = 0;
HWND hInfoStatic = 0;
#define IDC_STATIC_LOGIN 108
#define IDC_EDIT_LOGIN 109
#define IDC_STATIC_PASS 110
#define IDC_EDIT_PASS 111
#define IDC_BUTTON_AUTH 112
#define IDC_STATIC_INFO 113
HBITMAP h_bitmapDefault = 0, h_bitmapTemp=0;
HBITMAP g_hBitmap = 0, g_hBitmapDefault = 0;
HBITMAP g_hButtonBitmapNormal = 0, g_hButtonBitmapNormalDefault = 0;
HBITMAP g_hButtonBitmapHover = 0, g_hButtonBitmapHoverDefault = 0;
HBITMAP g_hButtonBitmapClicked = 0, g_hButtonBitmapClickedDefault = 0;
int mainwidth = 0;
int mainheight = 0;
int buttonwidth = 0;
int buttonheight = 0;
BOOL PaintStuff(HWND hwnd)
{
	if (g_hBitmap) {
		PAINTSTRUCT ps = { 0 };
		HDC hdc = BeginPaint(hwnd, &ps);
		HDC hMemDC = CreateCompatibleDC(hdc);
		HGDIOBJ old = SelectObject(hMemDC, g_hBitmapDefault);
		BOOL ret1 = StretchBlt(hdc, 0, 0, mainwidth, mainheight, hMemDC, 0, 0, mainwidth, mainheight, SRCCOPY);
		SelectObject(hMemDC, old);
		DeleteDC(hMemDC);
		EndPaint(hwnd, &ps);
		return 1;
	}
	return 0;
}
BOOL ApplySkin(LPDRAWITEMSTRUCT di)
{
	HBITMAP hBitmap = 0;
	if (di->itemAction == ODA_DRAWENTIRE) {
		if (di->CtlType == ODT_BUTTON) {
			if (di->itemState & ODS_SELECTED) {
				hBitmap = g_hButtonBitmapClicked;
			}
			else {
				hBitmap = g_hButtonBitmapNormal;
			}
		}
	}
	else if (di->itemAction == ODA_SELECT) {
		if (di->itemState & ODS_SELECTED) {
			hBitmap = g_hButtonBitmapClicked;
		}
		else {
			hBitmap = g_hButtonBitmapNormal;
		}
	}
	else if (di->itemAction == ODA_FOCUS) {
		hBitmap = g_hButtonBitmapHover;
	}
	else {
		//handle error
	}
	if (_bSelected == 1) {
		hBitmap = g_hButtonBitmapClicked;
	}
	if (_bHoover && (di->itemState & ODS_SELECTED) == FALSE) {
		hBitmap = g_hButtonBitmapHover;
	}
	BITMAP bmp = { 0 };
	if (hBitmap) {
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		HDC hdcMem = CreateCompatibleDC(di->hDC);
		HGDIOBJ hOld = SelectObject(hdcMem, hBitmap);
		int x = di->rcItem.left;
		int y = di->rcItem.top;
		int width = di->rcItem.right - di->rcItem.left;
		int height = di->rcItem.bottom - di->rcItem.top;
		int oldstrm = SetStretchBltMode(di->hDC, HALFTONE);
		TransparentBlt(di->hDC, x, y, width, height, hdcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(0,0,255));
		hBitmap = (HBITMAP)SelectObject(hdcMem, hOld);
		SetStretchBltMode(di->hDC, oldstrm);
		DeleteDC(hdcMem);
	}
	return TRUE;
}
static HBRUSH hBrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH); //CreateSolidBrush(RGB(230, 230, 230));
LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
	{
		if (hWnd && hWnd==hMainWindow && PaintStuff(hWnd)) {
			return 0;
		}
		break;
	}
	case WM_DRAWITEM:
	{
		HWND hbutton = ((LPDRAWITEMSTRUCT)lParam)->hwndItem;
		if (hbutton && hbutton==hAuthButton)
		{
			if (ApplySkin((LPDRAWITEMSTRUCT)lParam)) {
				return TRUE;
			}
		}
		break;
	}
	case WM_NCHITTEST: {
		LRESULT hit = DefWindowProc(hWnd, uMsg, wParam, lParam);
		if (hit == HTCLIENT) hit = HTCAPTION;
		return hit;
	}
	case WM_COMMAND:
	{
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
		{
			switch (LOWORD(wParam))
			{
			case IDC_BUTTON_AUTH:
			{
				EnableWindow((HWND)lParam,FALSE);
				//Beep(1000, 100);// ShellExecute(NULL, L"open", L"http://redguides.com", NULL, NULL, SW_SHOWNORMAL);
				//need to ask server to encrypt their pass
				//https://www.redguides.com/maskoi/apitest3.php?ptpass=password
				CHAR szBuffer[MAX_STRING] = { 0 };
				CHAR szPass[MAX_STRING] = { 0 };
				GetDlgItemText(hMainWindow, IDC_EDIT_PASS, szPass, MAX_STRING - 1);
				sprintf_s(szBuffer, "/maskoi/passgen.php?ptpass=%s", szPass);
				char *passhash = HttpQuery("www.redguides.com", "POST", szBuffer, NULL, NULL);
				SetDlgItemText(hMainWindow, IDC_EDIT_PASS, passhash);
				sprintf_s(szBuffer, "%s\\emucreds.ini", gszINIPath);
				WritePrivateProfileStringA("Creds", "Pass",passhash, szBuffer);
				GetDlgItemText(hMainWindow, IDC_EDIT_LOGIN, szPass, MAX_STRING - 1);
				WritePrivateProfileStringA("Creds", "Login", szPass, szBuffer);
				ShowWindow(hWnd, SW_HIDE);
				
				PostQuitMessage(0);
				return 0;
			}
			}
			break;
		}
		break;
		}
		break;
	}
	case WM_CTLCOLORBTN:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORSTATIC:
	{
		DWORD CtrlID = GetDlgCtrlID((HWND)lParam); //Window Control ID
		if (CtrlID == IDC_BUTTON_AUTH || CtrlID == IDC_STATIC_INFO || CtrlID == IDC_STATIC_LOGIN || CtrlID == IDC_STATIC_PASS || CtrlID == IDC_EDIT_LOGIN || CtrlID == IDC_EDIT_PASS) //If desired control
		{
			HDC hdcStatic = (HDC)wParam;//0BB5FF
			SetTextColor(hdcStatic, RGB(0xb, 0xb5, 0xff));
			SetBkColor(hdcStatic, TRANSPARENT);
			return (INT_PTR)hBrush;
		}
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
WNDPROC OrgMouseTrackWndProc;
LRESULT MouseTrackWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSELEAVE:
	{
		//eHwnd->SetHoover(0);
		_bHoover = 0;
		//eHwnd->SetMT(0);
		_bTM = 0;
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;//If an application processes this message, it should return zero. (so thats what we do...)
	}
	case WM_MOUSEMOVE:
	{
		if (!_bTM) {
			_bHoover = 1;
			TrackMouseEvent(&tme);
			_bTM = 1;
			InvalidateRect(hWnd, NULL, TRUE);
			return 0;//If an application processes this message, it should return zero. (so thats what we do...)
		}
		break;
	}
	}
	return CallWindowProc((WNDPROC)OrgMouseTrackWndProc, hWnd, message, wParam, lParam);
}

#define IDB_CLICKED                     101
#define IDB_HOVER                       102
#define IDB_NORMAL                      103
#define IDB_MAIN                        104
HINSTANCE g_hinst = 0;
BYTE* Get24BitPixels(HBITMAP pBitmap, WORD *pwWidth, WORD *pwHeight)
{
	// a bitmap object just to get bitmap width and height
	BITMAP bmpBmp;

	// pointer to original bitmap info
	LPBITMAPINFO pbmiInfo;

	// bitmap info will hold the new 24bit bitmap info
	BITMAPINFO bmiInfo;

	// width and height of the bitmap
	WORD wBmpWidth, wBmpHeight;

	// ---------------------------------------------------------
	// get some info from the bitmap
	// ---------------------------------------------------------
	GetObject(pBitmap, sizeof(bmpBmp), &bmpBmp);
	pbmiInfo = (LPBITMAPINFO)&bmpBmp;

	// get width and height
	wBmpWidth = (WORD)pbmiInfo->bmiHeader.biWidth;
	wBmpWidth -= (wBmpWidth % 4);                       // width is 4 byte boundary aligned.
	wBmpHeight = (WORD)pbmiInfo->bmiHeader.biHeight;

	// copy to caller width and height parms
	*pwWidth = wBmpWidth;
	*pwHeight = wBmpHeight;
	// ---------------------------------------------------------

	// allocate width * height * 24bits pixels
	BYTE *pPixels = new BYTE[wBmpWidth*wBmpHeight * 3];
	if (!pPixels) return NULL;

	// get user desktop device context to get pixels from
	HDC hDC = GetWindowDC(NULL);

	// fill desired structure
	bmiInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiInfo.bmiHeader.biWidth = wBmpWidth;
	bmiInfo.bmiHeader.biHeight = -wBmpHeight;
	bmiInfo.bmiHeader.biPlanes = 1;
	bmiInfo.bmiHeader.biBitCount = 24;
	bmiInfo.bmiHeader.biCompression = BI_RGB;
	bmiInfo.bmiHeader.biSizeImage = wBmpWidth*wBmpHeight * 3;
	bmiInfo.bmiHeader.biXPelsPerMeter = 0;
	bmiInfo.bmiHeader.biYPelsPerMeter = 0;
	bmiInfo.bmiHeader.biClrUsed = 0;
	bmiInfo.bmiHeader.biClrImportant = 0;

	// get pixels from the original bitmap converted to 24bits
	int iRes = GetDIBits(hDC, pBitmap, 0, wBmpHeight, (LPVOID)pPixels, &bmiInfo, DIB_RGB_COLORS);

	// release the device context
	ReleaseDC(NULL, hDC);

	// if failed, cancel the operation.
	if (!iRes)
	{
		delete[] pPixels;
		return NULL;
	};

	// return the pixel array
	return pPixels;
}
HRGN ScanRegion(HBITMAP pBitmap, BYTE jTranspR, BYTE jTranspG, BYTE jTranspB)
{
	// bitmap width and height
	WORD wBmpWidth, wBmpHeight;  // the final region and a temporary region
	HRGN hRgn, hTmpRgn;  // 24bit pixels from the bitmap
	BYTE *pPixels = Get24BitPixels(pBitmap, &wBmpWidth, &wBmpHeight);
	if (!pPixels) return NULL;  // create our working region
	hRgn = CreateRectRgn(0, 0, wBmpWidth, wBmpHeight);
	if (!hRgn) { delete pPixels; return NULL; }  // ---------------------------------------------------------
												 // scan the bitmap
												 // ---------------------------------------------------------
	DWORD p = 0;
	for (WORD y = 0; y<wBmpHeight; y++)
	{
		for (WORD x = 0; x<wBmpWidth; x++)
		{
			BYTE jRed = pPixels[p + 2];
			BYTE jGreen = pPixels[p + 1];
			BYTE jBlue = pPixels[p + 0];
			if (jRed == jTranspR && jGreen == jTranspG && jBlue == jTranspB)
			{
				// remove transparent color from region
				hTmpRgn = CreateRectRgn(x, y, x + 1, y + 1);
				CombineRgn(hRgn, hRgn, hTmpRgn, RGN_XOR);
				DeleteObject(hTmpRgn);
			}      // next pixel
			p += 3;
		}
	}  // release pixels
	delete pPixels;  // return the region
	return hRgn;
}
bool ShouldWeInject()
{
	CHAR szBuffer[MAX_STRING] = { 0 };
	CHAR szLogin[256] = { 0 };
	CHAR szPass[256] = { 0 };
	CHAR szHash[256] = { 0 };
	sprintf_s(szBuffer, "%s\\emucreds.ini", gszINIPath);
	//DeleteFile(szBuffer);
getcreds:
	GetPrivateProfileStringA("Creds", "Login", "", szLogin, 255, szBuffer);
	GetPrivateProfileStringA("Creds", "Pass", "", szPass, 255, szBuffer);
	if (szPass[0] == '\0') {
		INITCOMMONCONTROLSEX icc;
		icc.dwSize = sizeof(icc);
		icc.dwICC = ICC_WIN95_CLASSES;

		if (InitCommonControlsEx(&icc) != TRUE)
		{
			//handle error
		}
		OleInitialize(0);
		WNDCLASSEX wcx = { sizeof(wcx) };
		wcx.style = CS_HREDRAW | CS_VREDRAW;// redraw if size changes 
		wcx.lpfnWndProc = StaticWndProc;     // points to window procedure 
		wcx.cbClsExtra = 0;                // no extra class memory 
		wcx.cbWndExtra = 0;                // no extra window memory 
		wcx.hInstance = g_hinst;				// handle to instance 
		wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);// predefined app. icon 
		wcx.hCursor = LoadCursor(NULL, IDC_ARROW);// predefined arrow 
		wcx.hbrBackground = (HBRUSH)BLACK_BRUSH;// black background brush 
		wcx.lpszClassName = "EmuAuthWindowClass";  // name of window class 
		wcx.lpszMenuName = NULL;
		wcx.hIconSm = NULL;
		RegisterClassEx(&wcx);
		int x = 200;
		int y = 200;
		CHAR szOut[256] = { 0 };
		GetModuleFileNameA(g_hinst, szOut, 255);
		if (szOut[0]) {
			if (char *pDest = strrchr(szOut, '\\')) {
				pDest[0] = '\0';
				strcat_s(szOut, "\\Emuauth.dll");
			}
		}
		HMODULE hResdll = LoadLibraryA(szOut);
		if (hResdll) {
			g_hButtonBitmapHover = (HBITMAP)LoadImage(hResdll, MAKEINTRESOURCE(IDB_HOVER), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_VGACOLOR);
			g_hButtonBitmapClicked = (HBITMAP)LoadImage(hResdll, MAKEINTRESOURCE(IDB_CLICKED), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_VGACOLOR);
			g_hButtonBitmapNormal = (HBITMAP)LoadImage(hResdll, MAKEINTRESOURCE(IDB_NORMAL), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_VGACOLOR);
			g_hBitmap = (HBITMAP)LoadImage(hResdll, MAKEINTRESOURCE(IDB_MAIN), IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT);
		}
		else {
			 MessageBoxA(NULL, "No, just no, you need to have the emuauth.dll in the same folder as mq2main.dll", "Im going to terminate this crap", MB_SYSTEMMODAL | MB_OK);
			UnregisterClass("EmuAuthWindowClass", g_hinst);
			return false;
		}
		BITMAP bmp = { 0 };
		GetObject(g_hBitmap, sizeof(BITMAP), &bmp);
		mainwidth = bmp.bmWidth;
		mainheight = bmp.bmHeight;
		GetObject(g_hButtonBitmapNormal, sizeof(BITMAP), &bmp);
		buttonwidth = bmp.bmWidth;
		buttonheight = bmp.bmHeight;
		hRgn = ScanRegion(g_hButtonBitmapNormal, 0, 0, 255);
		
		hMainWindow = CreateWindowEx(WS_EX_LAYERED|WS_EX_TOPMOST, "EmuAuthWindowClass", "Emu Authenticator 1.0",WS_CLIPCHILDREN|WS_CLIPSIBLINGS| WS_POPUP , x, y, mainwidth, mainheight, 0, (HMENU)NULL, g_hinst, (LPVOID)NULL);
		if (hMainWindow) {
			SetWindowTheme(hMainWindow, L" ", L" ");
			HBRUSH brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
			SetClassLongPtr(hMainWindow, GCLP_HBRBACKGROUND, (LONG)brush);
			hLoginStatic = CreateWindowEx(0, "Static", "Login:", WS_VISIBLE | SS_LEFT | WS_CHILD,
				60, 60, 100, 20, hMainWindow, (HMENU)IDC_STATIC_LOGIN, g_hinst, 0);
			hPassStatic = CreateWindowEx(0, "Static", "Password:", WS_VISIBLE | SS_LEFT | WS_CHILD,
				60, 80, 240, 20, hMainWindow, (HMENU)IDC_STATIC_PASS, g_hinst, 0);
			hLoginEdit = CreateWindowEx(0, "EDIT", "", WS_TABSTOP|WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NOHIDESEL | ES_WANTRETURN | WS_CHILD,
				165, 60, 100, 20, hMainWindow, (HMENU)IDC_EDIT_LOGIN, g_hinst, 0);
			hPassEdit = CreateWindowEx(0, "EDIT", "", WS_TABSTOP|WS_VISIBLE | WS_BORDER | ES_NOHIDESEL | ES_AUTOHSCROLL | ES_WANTRETURN | WS_CHILD,
				165, 80, 100, 20, hMainWindow, (HMENU)IDC_EDIT_PASS, g_hinst, 0);
			//hAuthButton = CreateWindowEx(0, "BUTTON", "Authenticate", WS_VISIBLE | WS_CHILD,
			hAuthButton = CreateWindowEx(0, "BUTTON", "", WS_TABSTOP|WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
				265, 270, buttonwidth, buttonheight, hMainWindow, (HMENU)IDC_BUTTON_AUTH, g_hinst, 0);
			SetWindowRgn(hAuthButton, hRgn, TRUE);
			//hInfoStatic = CreateWindowEx(0, "Static", "This information will only be asked of you once.\nYour login credentials are being stored using one way encryption.\nIf you lose or forget it, you will have to create a new one, because there is no feasable way for us to recover it.", WS_VISIBLE | SS_LEFT | WS_CHILD,
			//	40, 140, 330, 120, hMainWindow, (HMENU)IDC_STATIC_INFO, hinst, 0);

			SendMessage(hLoginStatic, (UINT)WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
			SendMessage(hPassStatic, (UINT)WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
			SendMessage(hLoginEdit, (UINT)WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
			SendMessage(hPassEdit, (UINT)WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
			SendMessage(hAuthButton, (UINT)WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
			//CreateScalableFontResource()
			//HFONT hf = CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Tarragon ICG");
			//SendMessage(hLoginStatic, (UINT)WM_SETFONT, (WPARAM)hf, (LPARAM)MAKELPARAM(FALSE, 0));
			//SendMessage(hPassStatic, (UINT)WM_SETFONT, (WPARAM)hf, (LPARAM)MAKELPARAM(FALSE, 0));

			//SendMessage(hInfoStatic, (UINT)WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), (LPARAM)MAKELPARAM(FALSE, 0));
			ShowWindow(hMainWindow, SW_SHOWNORMAL);
			SetLayeredWindowAttributes(hMainWindow, RGB(0x00, 0x00, 0xff), 0, LWA_COLORKEY | LWA_COLORKEY);
			//main
			HDC hdc = GetDC(hMainWindow);
			HDC hMemDC = CreateCompatibleDC(hdc);
			HDC hMemDC1 = CreateCompatibleDC(hdc);
			g_hBitmapDefault = CreateCompatibleBitmap(hdc, mainwidth, mainheight);
			HGDIOBJ hOld1 = SelectObject(hMemDC, g_hBitmap);
			HGDIOBJ old = SelectObject(hMemDC1, g_hBitmapDefault);
			BOOL ret1 = StretchBlt(hMemDC1, 0, 0, mainwidth, mainheight, hMemDC, 0, 0, mainwidth, mainheight, SRCCOPY);
			g_hBitmapDefault = (HBITMAP)SelectObject(hMemDC1, old);
			SelectObject(hMemDC, hOld1);
			DeleteDC(hMemDC1);
			DeleteDC(hMemDC);
			ReleaseDC(hMainWindow, hdc);
			//default button
			hdc = GetDC(hAuthButton);
			hMemDC = CreateCompatibleDC(hdc);
			hMemDC1 = CreateCompatibleDC(hdc);
			g_hButtonBitmapNormalDefault = CreateCompatibleBitmap(hdc, buttonwidth, buttonheight);
			hOld1 = SelectObject(hMemDC, g_hButtonBitmapNormal);
			old = SelectObject(hMemDC1, g_hButtonBitmapNormalDefault);
			ret1 = StretchBlt(hMemDC1, 0, 0, buttonwidth, buttonheight, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			g_hButtonBitmapNormalDefault = (HBITMAP)SelectObject(hMemDC1, old);
			SelectObject(hMemDC, hOld1);
			DeleteDC(hMemDC1);
			DeleteDC(hMemDC);
			ReleaseDC(hAuthButton, hdc);
			//hoover button
			hdc = GetDC(hAuthButton);
			hMemDC = CreateCompatibleDC(hdc);
			hMemDC1 = CreateCompatibleDC(hdc);
			g_hButtonBitmapHoverDefault = CreateCompatibleBitmap(hdc, buttonwidth, buttonheight);
			hOld1 = SelectObject(hMemDC, g_hButtonBitmapHover);
			old = SelectObject(hMemDC1, g_hButtonBitmapHoverDefault);
			ret1 = StretchBlt(hMemDC1, 0, 0, buttonwidth, buttonheight, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			g_hButtonBitmapHoverDefault = (HBITMAP)SelectObject(hMemDC1, old);
			SelectObject(hMemDC, hOld1);
			DeleteDC(hMemDC1);
			DeleteDC(hMemDC);
			ReleaseDC(hAuthButton, hdc);
			//clicked button
			hdc = GetDC(hAuthButton);
			hMemDC = CreateCompatibleDC(hdc);
			hMemDC1 = CreateCompatibleDC(hdc);
			g_hButtonBitmapClickedDefault = CreateCompatibleBitmap(hdc, buttonwidth, buttonheight);
			hOld1 = SelectObject(hMemDC, g_hButtonBitmapClicked);
			old = SelectObject(hMemDC1, g_hButtonBitmapClickedDefault);
			ret1 = StretchBlt(hMemDC1, 0, 0, buttonwidth, buttonheight, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
			g_hButtonBitmapClickedDefault = (HBITMAP)SelectObject(hMemDC1, old);
			SelectObject(hMemDC, hOld1);
			DeleteDC(hMemDC1);
			DeleteDC(hMemDC);
			ReleaseDC(hAuthButton, hdc);
			

			OrgMouseTrackWndProc = (WNDPROC)SetWindowLongPtr(hAuthButton, GWL_WNDPROC, (LONG)MouseTrackWndProc);
			
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.hwndTrack = hAuthButton;
			tme.dwHoverTime = 1500;
			tme.dwFlags = TME_HOVER | TME_LEAVE;

			TrackMouseEvent(&tme);
			FreeLibrary(hResdll);

		}
		MSG msg;
		BOOL bRet;

		while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
		{
			if (!IsDialogMessage(hMainWindow, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		UnregisterClass("EmuAuthWindowClass", g_hinst);
		goto getcreds;
	}
	sprintf_s(szBuffer, "/maskoi/apitest3.php?login=%s&saltme=1", szLogin);
	//https://www.redguides.com/maskoi/apitest3.php?login=neoweo&saltme=1
	char *salt = HttpQuery("www.redguides.com", "POST", szBuffer, NULL, NULL);

	//Login is md5(username + salt) pass is encrypted pass word from step 1
	//https://www.redguides.com/maskoi/apitest3.php?login=c8bd2e84126b8d3a4835260a1e0576ca&pass=EaA4cXV00kG4a9qqd2/0pYuLN6TVuitYNyqlzFT6ym8=
	sprintf_s(szBuffer, "%s%s", szLogin, salt);

	DWORD out = 0;
	char *secret = md5(szBuffer, &out);

	sprintf_s(szBuffer, "/maskoi/apitest3.php?login=%s&pass=%s", secret, szPass);
	bool login = false;
	char *ok = HttpQuery("www.redguides.com", "POST", szBuffer, NULL, NULL);
	int serverreturn = atoi(ok);
	if (serverreturn == 0) {
		//Reign of Fear requires a valid subscription at Redguides.com
		 MessageBoxA(NULL, "Reign of Fear requires a valid subscription at Redguides.com", "", MB_SYSTEMMODAL | MB_OK);
	} else if (serverreturn == 1) {
		// Login in successful enjoy Reign of Fear
		 MessageBoxA(NULL, "Login in successful enjoy Reign of Fear", "", MB_SYSTEMMODAL | MB_OK);
		login = true;
	}
	else if (serverreturn == 2) {
		// ERROR: Incorrect username or password.
		 MessageBoxA(NULL, "ERROR: Incorrect username or password.", "", MB_SYSTEMMODAL | MB_OK);
		sprintf_s(szBuffer, "%s\\emucreds.ini", gszINIPath);
		WritePrivateProfileStringA("Creds", "Pass", "", szBuffer);
		//DeleteFile(szBuffer);
	}
	//return false;
	return login;
}
#endif
inline void WINAPI MQ2StartEmu()
{
#ifndef PRIVATEEMU
	MEMORY_BASIC_INFORMATION mbi;
	static int dummy;
	VirtualQuery(&dummy, &mbi, sizeof(mbi));
	g_hinst = (HMODULE)mbi.AllocationBase;
	if (!ShouldWeInject()) {
		if (g_hinst) {
			FreeLibraryAndExitThread(g_hinst, 0);
		}
		exit(0);
	}
#endif
}