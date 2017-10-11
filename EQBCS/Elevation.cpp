/****************************** Module Header ******************************\
Module Name:  CppUACSelfElevation.cpp
Project:      CppUACSelfElevation
Copyright (c) Microsoft Corporation.

User Account Control (UAC) is a new security component in Windows Vista and 
newer operating systems. With UAC fully enabled, interactive administrators 
normally run with least user privileges. This example demonstrates how to 
check the privilege level of the current process, and how to self-elevate 
the process by giving explicit consent with the Consent UI. 

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/en-us/openness/resources/licenses.aspx#MPL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#pragma region Includes and Manifest Dependencies
#include "stdafx.h"
#include "Elevation.h"
#include "service.h"
#include <VersionHelpers.h>
#include "guicon.h"
#include "eqbcs_win.h"
#include <commctrl.h>
#include <windowsx.h>

// Some Global variables because I'm f'in lazy!
bool service = false;
bool debug = false;
bool standalone = false;
DWORD port = 2112;
TCHAR ipaddr[2048] = { 0 };

// Enable Visual Style
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#pragma endregion


#pragma region "Helper Functions for Admin Privileges and Elevation Status"

//
//   FUNCTION: IsUserInAdminGroup()
//
//   PURPOSE: The function checks whether the primary access token of the 
//   process belongs to user account that is a member of the local 
//   Administrators group, even if it currently is not elevated.
//
//   RETURN VALUE: Returns TRUE if the primary access token of the process 
//   belongs to user account that is a member of the local Administrators 
//   group. Returns FALSE if the token does not.
//
//   EXCEPTION: If this function fails, it throws a C++ DWORD exception which 
//   contains the Win32 error code of the failure.
//
//   EXAMPLE CALL:
//     try 
//     {
//         if (IsUserInAdminGroup())
//             wprintf (L"User is a member of the Administrators group\n");
//         else
//             wprintf (L"User is not a member of the Administrators group\n");
//     }
//     catch (DWORD dwError)
//     {
//         wprintf(L"IsUserInAdminGroup failed w/err %lu\n", dwError);
//     }
//
BOOL IsUserInAdminGroup()
{
    BOOL fInAdminGroup = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    HANDLE hToken = NULL;
    HANDLE hTokenToCheck = NULL;
    DWORD cbSize = 0;
    OSVERSIONINFO osver = { sizeof(osver) };

    // Open the primary access token of the process for query and duplicate.
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE, 
        &hToken))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Determine whether system is running Windows Vista or later operating 
    // systems (major version >= 6) because they support linked tokens, but 
    // previous versions (major version < 6) do not.
    //if (!GetVersionEx(&osver))
    //{
    //    dwError = GetLastError();
    //    goto Cleanup;
    //}

    if (IsWindowsVistaOrGreater())
    {
        // Running Windows Vista or later (major version >= 6). 
        // Determine token type: limited, elevated, or default. 
        TOKEN_ELEVATION_TYPE elevType;
        if (!GetTokenInformation(hToken, TokenElevationType, &elevType, 
            sizeof(elevType), &cbSize))
        {
            dwError = GetLastError();
            goto Cleanup;
        }

        // If limited, get the linked elevated token for further check.
        if (TokenElevationTypeLimited == elevType)
        {
            if (!GetTokenInformation(hToken, TokenLinkedToken, &hTokenToCheck, 
                sizeof(hTokenToCheck), &cbSize))
            {
                dwError = GetLastError();
                goto Cleanup;
            }
        }
    }
    
    // CheckTokenMembership requires an impersonation token. If we just got a 
    // linked token, it already is an impersonation token.  If we did not get 
    // a linked token, duplicate the original into an impersonation token for 
    // CheckTokenMembership.
    if (!hTokenToCheck)
    {
        if (!DuplicateToken(hToken, SecurityIdentification, &hTokenToCheck))
        {
            dwError = GetLastError();
            goto Cleanup;
        }
    }

    // Create the SID corresponding to the Administrators group.
    BYTE adminSID[SECURITY_MAX_SID_SIZE];
    cbSize = sizeof(adminSID);
    if (!CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSID,  
        &cbSize))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Check if the token to be checked contains admin SID.
    // http://msdn.microsoft.com/en-us/library/aa379596(VS.85).aspx:
    // To determine whether a SID is enabled in a token, that is, whether it 
    // has the SE_GROUP_ENABLED attribute, call CheckTokenMembership.
    if (!CheckTokenMembership(hTokenToCheck, &adminSID, &fInAdminGroup)) 
    {
        dwError = GetLastError();
        goto Cleanup;
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (hToken)
    {
        CloseHandle(hToken);
        hToken = NULL;
    }
    if (hTokenToCheck)
    {
        CloseHandle(hTokenToCheck);
        hTokenToCheck = NULL;
    }

    // Throw the error if something failed in the function.
    if (ERROR_SUCCESS != dwError)
    {
        throw dwError;
    }

    return fInAdminGroup;
}


// 
//   FUNCTION: IsRunAsAdmin()
//
//   PURPOSE: The function checks whether the current process is run as 
//   administrator. In other words, it dictates whether the primary access 
//   token of the process belongs to user account that is a member of the 
//   local Administrators group and it is elevated.
//
//   RETURN VALUE: Returns TRUE if the primary access token of the process 
//   belongs to user account that is a member of the local Administrators 
//   group and it is elevated. Returns FALSE if the token does not.
//
//   EXCEPTION: If this function fails, it throws a C++ DWORD exception which 
//   contains the Win32 error code of the failure.
//
//   EXAMPLE CALL:
//     try 
//     {
//         if (IsRunAsAdmin())
//             wprintf (L"Process is run as administrator\n");
//         else
//             wprintf (L"Process is not run as administrator\n");
//     }
//     catch (DWORD dwError)
//     {
//         wprintf(L"IsRunAsAdmin failed w/err %lu\n", dwError);
//     }
//
BOOL IsRunAsAdmin()
{
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    // Allocate and initialize a SID of the administrators group.
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &NtAuthority, 
        2, 
        SECURITY_BUILTIN_DOMAIN_RID, 
        DOMAIN_ALIAS_RID_ADMINS, 
        0, 0, 0, 0, 0, 0, 
        &pAdministratorsGroup))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Determine whether the SID of administrators group is enabled in 
    // the primary access token of the process.
    if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (pAdministratorsGroup)
    {
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = NULL;
    }

    // Throw the error if something failed in the function.
    if (ERROR_SUCCESS != dwError)
    {
        throw dwError;
    }

    return fIsRunAsAdmin;
}


//
//   FUNCTION: IsProcessElevated()
//
//   PURPOSE: The function gets the elevation information of the current 
//   process. It dictates whether the process is elevated or not. Token 
//   elevation is only available on Windows Vista and newer operating 
//   systems, thus IsProcessElevated throws a C++ exception if it is called 
//   on systems prior to Windows Vista. It is not appropriate to use this 
//   function to determine whether a process is run as administartor.
//
//   RETURN VALUE: Returns TRUE if the process is elevated. Returns FALSE if 
//   it is not.
//
//   EXCEPTION: If this function fails, it throws a C++ DWORD exception 
//   which contains the Win32 error code of the failure. For example, if 
//   IsProcessElevated is called on systems prior to Windows Vista, the error 
//   code will be ERROR_INVALID_PARAMETER.
//
//   NOTE: TOKEN_INFORMATION_CLASS provides TokenElevationType to check the 
//   elevation type (TokenElevationTypeDefault / TokenElevationTypeLimited /
//   TokenElevationTypeFull) of the process. It is different from 
//   TokenElevation in that, when UAC is turned off, elevation type always 
//   returns TokenElevationTypeDefault even though the process is elevated 
//   (Integrity Level == High). In other words, it is not safe to say if the 
//   process is elevated based on elevation type. Instead, we should use 
//   TokenElevation.
//
//   EXAMPLE CALL:
//     try 
//     {
//         if (IsProcessElevated())
//             wprintf (L"Process is elevated\n");
//         else
//             wprintf (L"Process is not elevated\n");
//     }
//     catch (DWORD dwError)
//     {
//         wprintf(L"IsProcessElevated failed w/err %lu\n", dwError);
//     }
//
BOOL IsProcessElevated()
{
    BOOL fIsElevated = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    HANDLE hToken = NULL;

    // Open the primary access token of the process with TOKEN_QUERY.
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Retrieve token elevation information.
    TOKEN_ELEVATION elevation;
    DWORD dwSize;
    if (!GetTokenInformation(hToken, TokenElevation, &elevation, 
        sizeof(elevation), &dwSize))
    {
        // When the process is run on operating systems prior to Windows 
        // Vista, GetTokenInformation returns FALSE with the 
        // ERROR_INVALID_PARAMETER error code because TokenElevation is 
        // not supported on those operating systems.
        dwError = GetLastError();
        goto Cleanup;
    }

    fIsElevated = elevation.TokenIsElevated;

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (hToken)
    {
        CloseHandle(hToken);
        hToken = NULL;
    }

    // Throw the error if something failed in the function.
    if (ERROR_SUCCESS != dwError)
    {
        throw dwError;
    }

    return fIsElevated;
}


//
//   FUNCTION: GetProcessIntegrityLevel()
//
//   PURPOSE: The function gets the integrity level of the current process. 
//   Integrity level is only available on Windows Vista and newer operating 
//   systems, thus GetProcessIntegrityLevel throws a C++ exception if it is 
//   called on systems prior to Windows Vista.
//
//   RETURN VALUE: Returns the integrity level of the current process. It is 
//   usually one of these values:
//
//     SECURITY_MANDATORY_UNTRUSTED_RID (SID: S-1-16-0x0)
//     Means untrusted level. It is used by processes started by the 
//     Anonymous group. Blocks most write access. 
//
//     SECURITY_MANDATORY_LOW_RID (SID: S-1-16-0x1000)
//     Means low integrity level. It is used by Protected Mode Internet 
//     Explorer. Blocks write acess to most objects (such as files and 
//     registry keys) on the system. 
//
//     SECURITY_MANDATORY_MEDIUM_RID (SID: S-1-16-0x2000)
//     Means medium integrity level. It is used by normal applications 
//     being launched while UAC is enabled. 
//
//     SECURITY_MANDATORY_HIGH_RID (SID: S-1-16-0x3000)
//     Means high integrity level. It is used by administrative applications 
//     launched through elevation when UAC is enabled, or normal 
//     applications if UAC is disabled and the user is an administrator. 
//
//     SECURITY_MANDATORY_SYSTEM_RID (SID: S-1-16-0x4000)
//     Means system integrity level. It is used by services and other 
//     system-level applications (such as Wininit, Winlogon, Smss, etc.)  
//
//   EXCEPTION: If this function fails, it throws a C++ DWORD exception 
//   which contains the Win32 error code of the failure. For example, if 
//   GetProcessIntegrityLevel is called on systems prior to Windows Vista, 
//   the error code will be ERROR_INVALID_PARAMETER.
//
//   EXAMPLE CALL:
//     try 
//     {
//         DWORD dwIntegrityLevel = GetProcessIntegrityLevel();
//     }
//     catch (DWORD dwError)
//     {
//         wprintf(L"GetProcessIntegrityLevel failed w/err %lu\n", dwError);
//     }
//
DWORD GetProcessIntegrityLevel()
{
    DWORD dwIntegrityLevel = 0;
    DWORD dwError = ERROR_SUCCESS;
    HANDLE hToken = NULL;
    DWORD cbTokenIL = 0;
    PTOKEN_MANDATORY_LABEL pTokenIL = NULL;

    // Open the primary access token of the process with TOKEN_QUERY.
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Query the size of the token integrity level information. Note that 
    // we expect a FALSE result and the last error ERROR_INSUFFICIENT_BUFFER
    // from GetTokenInformation because we have given it a NULL buffer. On 
    // exit cbTokenIL will tell the size of the integrity level information.
    if (!GetTokenInformation(hToken, TokenIntegrityLevel, NULL, 0, &cbTokenIL))
    {
        if (ERROR_INSUFFICIENT_BUFFER != GetLastError())
        {
            // When the process is run on operating systems prior to Windows 
            // Vista, GetTokenInformation returns FALSE with the 
            // ERROR_INVALID_PARAMETER error code because TokenElevation 
            // is not supported on those operating systems.
            dwError = GetLastError();
            goto Cleanup;
        }
    }

    // Now we allocate a buffer for the integrity level information.
    pTokenIL = (TOKEN_MANDATORY_LABEL *)LocalAlloc(LPTR, cbTokenIL);
    if (pTokenIL == NULL)
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Retrieve token integrity level information.
    if (!GetTokenInformation(hToken, TokenIntegrityLevel, pTokenIL, 
        cbTokenIL, &cbTokenIL))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Integrity Level SIDs are in the form of S-1-16-0xXXXX. (e.g. 
    // S-1-16-0x1000 stands for low integrity level SID). There is one and 
    // only one subauthority.
    dwIntegrityLevel = *GetSidSubAuthority(pTokenIL->Label.Sid, 0);

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (hToken)
    {
        CloseHandle(hToken);
        hToken = NULL;
    }
    if (pTokenIL)
    {
        LocalFree(pTokenIL);
        pTokenIL = NULL;
        cbTokenIL = 0;
    }

    // Throw the error if something failed in the function.
    if (ERROR_SUCCESS != dwError)
    {
        throw dwError;
    }

    return dwIntegrityLevel;
}

#pragma endregion


//
//   FUNCTION: ReportError(LPWSTR, DWORD)
//
//   PURPOSE: Display an error dialog for the failure of a certain function.
//
//   PARAMETERS:
//   * pszFunction - the name of the function that failed.
//   * dwError - the Win32 error code. Its default value is the calling 
//   thread's last-error code value.
//
//   NOTE: The failing function must be immediately followed by the call of 
//   ReportError if you do not explicitly specify the dwError parameter of 
//   ReportError. This is to ensure that the calling thread's last-error code 
//   value is not overwritten by any calls of API between the failing 
//   function and ReportError.
//
void ReportError(LPCWSTR pszFunction, DWORD dwError = GetLastError())
{
    wchar_t szMessage[200];
    if (SUCCEEDED(StringCchPrintfW(szMessage, ARRAYSIZE(szMessage), 
        L"%s failed w/err 0x%08lx", pszFunction, dwError)))
    {
        MessageBoxW(NULL, szMessage, L"Error", MB_ICONERROR);
    }
}


// 
//   FUNCTION: OnInitDialog(HWND, HWND, LPARAM)
//
//   PURPOSE: Process the WM_INITDIALOG message. Check and display the 
//   "run as administrator" status, the elevation information, and the 
//   integrity level of the current process.
//
BOOL OnInitDialog(HWND hWnd, HWND hwndFocus, LPARAM lParam)
{
	try
	{
		// Get and display the process elevation information. 
		BOOL const fIsElevated = IsProcessElevated();
		//SetWindowText(hIsElevatedLabel, fIsElevated ? L"True" : L"False");

		HWND hServiceStartBtn = GetDlgItem(hWnd, IDC_SERVICE_START);
		HWND hServiceStopBtn = GetDlgItem(hWnd, IDC_SERVICE_STOP);
		HWND hServiceInstallBtn = GetDlgItem(hWnd, IDC_SERVICE_INSTALL);
		HWND hServiceRemoveBtn = GetDlgItem(hWnd, IDC_SERVICE_REMOVE);
		Button_SetElevationRequiredState(hServiceStartBtn, !fIsElevated);
		Button_SetElevationRequiredState(hServiceStopBtn, !fIsElevated);
		Button_SetElevationRequiredState(hServiceInstallBtn, !fIsElevated);
		Button_SetElevationRequiredState(hServiceRemoveBtn, !fIsElevated);
	}
	catch (DWORD dwError)
	{
		//SetWindowText(hIsElevatedLabel, L"N/A");
		ReportError(L"IsProcessElevated", dwError);
	}
	port=GetPrivateProfileInt(_T("EQBCS"), _T("Port"), 2112, ini_path);
	DWORD ret=GetPrivateProfileString(_T("EQBCS"), _T("Address"), _T("0.0.0.0"), (LPWSTR)ipaddr, 2048, ini_path);
	SetDlgItemText(hWnd, IDC_IPADDRESS, ipaddr);
	SetDlgItemInt(hWnd, IDC_PORT, port, FALSE);

    // Get and display whether the primary access token of the process 
    // belongs to user account that is a member of the local Administrators 
    // group even if it currently is not elevated (IsUserInAdminGroup).
/*    HWND hInAdminGroupLabel = GetDlgItem(hWnd, IDC_STATIC_INADMINGROUP);
    try
    {
        BOOL const fInAdminGroup = IsUserInAdminGroup();
        SetWindowText(hInAdminGroupLabel, fInAdminGroup ? L"True" : L"False");
    }
    catch (DWORD dwError)
    {
        SetWindowText(hInAdminGroupLabel, L"N/A");
        ReportError(L"IsUserInAdminGroup", dwError);
    }

    // Get and display whether the process is run as administrator or not 
    // (IsRunAsAdmin).
    HWND hIsRunAsAdminLabel = GetDlgItem(hWnd, IDC_STATIC_ISRUNASADMIN);
    try
    {
        BOOL const fIsRunAsAdmin = IsRunAsAdmin();
        SetWindowText(hIsRunAsAdminLabel, fIsRunAsAdmin ? L"True" : L"False");
    }
    catch (DWORD dwError)
    {
        SetWindowText(hIsRunAsAdminLabel, L"N/A");
        ReportError(L"IsRunAsAdmin", dwError);
    }
    
    // Get and display the process elevation information (IsProcessElevated) 
    // and integrity level (GetProcessIntegrityLevel). The information is not 
    // available on operating systems prior to Windows Vista.

    HWND hIsElevatedLabel = GetDlgItem(hWnd, IDC_STATIC_ISELEVATED);
    HWND hILLabel = GetDlgItem(hWnd, IDC_STATIC_IL);

    OSVERSIONINFO osver = { sizeof(osver) };
    if (GetVersionEx(&osver) && osver.dwMajorVersion >= 6)
    {
        // Running Windows Vista or later (major version >= 6).

        try
        {
            // Get and display the process elevation information.
            BOOL const fIsElevated = IsProcessElevated();
            SetWindowText(hIsElevatedLabel, fIsElevated ? L"True" : L"False");

            // Update the Self-elevate button to show the UAC shield icon on 
            // the UI if the process is not elevated. The 
            // Button_SetElevationRequiredState macro (declared in Commctrl.h) 
            // is used to show or hide the shield icon in a button. You can 
            // also get the shield directly as an icon by calling 
            // SHGetStockIconInfo with SIID_SHIELD as the parameter.
            HWND hElevateBtn = GetDlgItem(hWnd, IDC_BUTTON_ELEVATE);
            Button_SetElevationRequiredState(hElevateBtn, !fIsElevated);
        }
        catch (DWORD dwError)
        {
            SetWindowText(hIsElevatedLabel, L"N/A");
            ReportError(L"IsProcessElevated", dwError);
        }

        try
        {
            // Get and display the process integrity level.
            DWORD const dwIntegrityLevel = GetProcessIntegrityLevel();
            switch (dwIntegrityLevel)
            {
            case SECURITY_MANDATORY_UNTRUSTED_RID: SetWindowText(hILLabel, L"Untrusted"); break;
            case SECURITY_MANDATORY_LOW_RID: SetWindowText(hILLabel, L"Low"); break;
            case SECURITY_MANDATORY_MEDIUM_RID: SetWindowText(hILLabel, L"Medium"); break;
            case SECURITY_MANDATORY_HIGH_RID: SetWindowText(hILLabel, L"High"); break;
            case SECURITY_MANDATORY_SYSTEM_RID: SetWindowText(hILLabel, L"System"); break;
            default: SetWindowText(hILLabel, L"Unknown"); break;
            }
        }
        catch (DWORD dwError)
        {
            SetWindowText(hILLabel, L"N/A");
            ReportError(L"GetProcessIntegrityLevel", dwError);
        }
    }
    else
    {
        SetWindowText(hIsElevatedLabel, L"N/A");
        SetWindowText(hILLabel, L"N/A");
    }*/

    return TRUE;
}

struct handle_data {
	unsigned long process_id;
	HWND best_handle;
};

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam);
BOOL is_main_window(HWND handle);

HWND find_main_window(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.best_handle = 0;
	EnumWindows(enum_windows_callback, (LPARAM)&data);
	return data.best_handle;
}

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !is_main_window(handle)) {
		return TRUE;
	}
	data.best_handle = handle;
	return FALSE;
}

BOOL is_main_window(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

void ErrorMessage(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

//
//   FUNCTION: OnCommand(HWND, int, HWND, UINT)
//
//   PURPOSE: Process the WM_COMMAND message
//
void OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
	TCHAR parameters[64] = { 0 };
	bool gotipaddr = false;
	port = GetDlgItemInt(hWnd, IDC_PORT, NULL, FALSE);

	DWORD ret = GetDlgItemText(hWnd, IDC_IPADDRESS, ipaddr, 2048);
	if (ret) gotipaddr = true;
	if (!((port > 0) && (port < 65536)))
		port = 2112;
	_sntprintf(parameters, 2048, _T("%i"), port);
	WritePrivateProfileString(_T("EQBCS"), _T("Port"), parameters, ini_path);
	if (gotipaddr)
		WritePrivateProfileString(_T("EQBCS"), _T("Address"), ipaddr, ini_path);
	else
		WritePrivateProfileString(_T("EQBCS"), _T("Address"), _T("0.0.0.0"), ini_path);

	switch (id)
	{
	case IDC_SERVICE_STOP:
	case IDC_SERVICE_START:
	case IDC_SERVICE_INSTALL:
	case IDC_SERVICE_REMOVE:
	{
		TCHAR szPath[MAX_PATH];
		if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
		{
			// Launch itself as administrator.
			switch (id)
			{
				case IDC_SERVICE_STOP: _sntprintf(parameters, 64, _T("-stop")); break;
				case IDC_SERVICE_START: _sntprintf(parameters, 64, _T("-start")); break;
				case IDC_SERVICE_INSTALL: _sntprintf(parameters, 64, _T("-install")); break;
				case IDC_SERVICE_REMOVE: _sntprintf(parameters, 64, _T("-remove")); break;
			}

			SHELLEXECUTEINFO sei = { sizeof(sei) };
			sei.lpVerb = _T("runas");
			sei.lpFile = szPath;
			sei.lpParameters = parameters;
			sei.hwnd = hWnd;
			sei.nShow = SW_NORMAL;

			if (!ShellExecuteEx(&sei))
			{
				DWORD dwError = GetLastError();
				if (dwError != ERROR_CANCELLED)
					ErrorMessage(_T("ShellExecuteEx"));
			}
		}
	}break;
	case IDC_RUN_DEBUG:
		debug = true;
		EndDialog(hWnd, TRUE);  // Quit itself
	break;
	case IDC_RUN_STANDALONE:
		standalone = true;
		EndDialog(hWnd, TRUE);  // Quit itself
	break;
    case IDOK:
    case IDCANCEL:
		standalone = false;
		debug = false;
		EndDialog(hWnd, 0);
        break;
    }
}


//
//   FUNCTION: OnClose(HWND)
//
//   PURPOSE: Process the WM_CLOSE message
//
void OnClose(HWND hWnd)
{
    EndDialog(hWnd, 0);
}


//
//  FUNCTION: DialogProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main dialog.
//
INT_PTR CALLBACK DialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // Handle the WM_INITDIALOG message in OnInitDialog
        HANDLE_MSG (hWnd, WM_INITDIALOG, OnInitDialog);

        // Handle the WM_COMMAND message in OnCommand
        HANDLE_MSG (hWnd, WM_COMMAND, OnCommand);

        // Handle the WM_CLOSE message in OnClose
        HANDLE_MSG (hWnd, WM_CLOSE, OnClose);

    default:
        return FALSE;
    }
    return 0;
}


//
//  FUNCTION: wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
//
//  PURPOSE:  The entry point of the application.
//
int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR    lpCmdLine,
	int       nCmdShow)
{
	INT ret = 0;
	int NumArgs = 0;

	//TCHAR ini_path[2048] = { 0 };
	memset(ini_path, 0, 2048*sizeof(TCHAR));
	if (!GetModuleFileName(NULL, ini_path, 2048)) return -1;
	TCHAR *ext = _tcsrchr(ini_path, _T('\\'));
	ext++;
	_sntprintf(ext, 2048 - _tcslen(ini_path), _T("%s"), _T("eqbcs.ini"));

	LPWSTR *lpArgs=CommandLineToArgvW(lpCmdLine, &NumArgs);
	for (int i = 0; i < NumArgs; i++)
	{
		int l = _tcslen(lpArgs[i]);
		if ((l == 8) && (_tcsnicmp(_T("-install"), lpArgs[i], 8) == 0))
		{
			CmdInstallService();
			return 0;
		}
		else if ((l == 7) && (_tcsnicmp(_T("-remove"), lpArgs[i], 7) == 0))
		{
			CmdRemoveService();
			return 0;
		}
		else if ((l == 5) && (_tcsnicmp(_T("-stop"), lpArgs[i], 5) == 0))
		{
			CmdStopService();
			return 0;
		}
		else if ((l == 6) && (_tcsnicmp(_T("-start"), lpArgs[i], 6) == 0))
		{
			CmdStartService();
			return 0;
		}
		else if ((l == 6) && (_tcsnicmp(_T("-debug"), lpArgs[i], 6) == 0))
		{
			debug = true;
		}
		else if ((l == 11) && (_tcsnicmp(_T("-standalone"), lpArgs[i], 11) == 0))
		{
			standalone = true;
		}
		else if ((l == 2) && (_tcsnicmp(_T("-p"), lpArgs[i], 2) == 0))
		{
			if (NumArgs > (i + 2))
			{
				port = _ttoi(lpArgs[++i]);
				if ((port <= 0) || (port >= 65536))
					port = 2112;
			}
		}
		else if ((l == 2) && (_tcsnicmp(_T("-i"), lpArgs[i], 2) == 0))
		{
			if (NumArgs > (i + 2))
			{
				_sntprintf(ipaddr, 2048, _T("%s"), lpArgs[++i]);
			}
			else
				_sntprintf(ipaddr, 2048, _T("0.0.0.0"));
		}
	}
	if (debug)
	{
		RedirectIOToConsole();
		svc_main(0, NULL, stop, port, ipaddr);
	}
	else if (standalone)
	{
		RedirectIOToConsole();
		svc_main(0, NULL, stop, port, ipaddr);
	}
	else
	{
		SERVICE_TABLE_ENTRY dispatchTable[] =
		{
			{ TEXT(SZSERVICENAME), (LPSERVICE_MAIN_FUNCTION)service_main },
			{ NULL, NULL }
		};
		if (!StartServiceCtrlDispatcher(dispatchTable))
		{
			if (GetLastError() == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
			{
				ret = DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, DialogProc);
			}
			else
			{
				return 0;
			}
		}
		else
			return 0;
	}
	if (debug || standalone)
	{
		RedirectIOToConsole();
		svc_main(0, NULL, stop, port, ipaddr);
	}
	return 0;
}