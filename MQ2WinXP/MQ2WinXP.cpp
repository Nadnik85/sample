// MQ2WinXP.cpp <<Previously MQ2Shutdown.cpp>>
// Author: Alatyami, Redguides.com, 2007
//=================== MODIFICATION NOTES ===================== 
// 18 Sep 2007 by Alatyami
// - Added in INI functionality
// - Changed the name to MQ2WinXP v2.0
//
// 17 Sep 2007 by Alatyami
// - Added the ability to stop/stop services (v1.6)
// - Added the ability to stop/start explorer.exe (v1.5)
//
// 25 Jan 2007 by Alatyami
// - Implimented %SystemRoot% notation (v1.2)
//
// 24 Jan 2007 by Alatyami
// - Released MQ2Shutdown (v1.0)
//============================================================
// Description: This plugin is designed to allow you to manage your 
// MS Windows services while you are in game.  Additionally, with 
// this plugin, you are able to reboot, shutdown, or logout of your 
// computer. This can come in handy for  farming. Once you get your 
// fill, just shutdown your computer.
//============================================================
// Usage:
//		/WinSession [shutdown|reboot|logout]
//			[shutdown]	Shutsdown your computer and powers it off
//			[reboot]	Reboots your computer
//			[logout]	Logs you out of your computer
//      /WinExplorer [kill|start]
//			[kill]		Kills the explorer.exe process
//			[start]		Starts the explorer.exe process
//      /WinServices [stop|start]
//			[stop]		Attempts to stop every service listed in the INI
//			[start]		Attempts to start every service listed in the INI
//============================================================
//

#include <string>
#include <windows.h>
#include "../MQ2Plugin.h"

PreSetup("MQ2WinXP"); PLUGIN_VERSION(2.0);

SC_HANDLE ConnectToSCM();
void DisconnectFromSCM(SC_HANDLE);
void StopCmd(SC_HANDLE, string);
void StartCmd(SC_HANDLE, string);
void cmdWSession(PSPAWNINFO pChar, PCHAR szLine);
void cmdExplorer(PSPAWNINFO pChar, PCHAR szLine);
void cmdServices(PSPAWNINFO pChar, PCHAR szLine);

void cmdWSession(PSPAWNINFO pChar, PCHAR szLine) {
	if (!stricmp(szLine,"shutdown")) {
		system("%SystemRoot%\\System32\\shutdown.exe -s -f -t 00");
	} else if (!stricmp(szLine,"reboot")) {	
		system("%SystemRoot%\\System32\\shutdown.exe -r -f -t 00");
	} else if (!stricmp(szLine,"logout")) {
		system("%SystemRoot%\\System32\\shutdown.exe -l -f -t 00");
	}
}

void cmdExplorer(PSPAWNINFO pChar, PCHAR szLine) {
   if (!stricmp(szLine,"kill")) {
	   WriteChatColor("Killing Windows Explorer ...");
	   system("%SystemRoot%\\System32\\taskkill.exe /F /IM explorer.exe");
	   WriteChatColor("Done.");
   } else if (!stricmp(szLine,"start")) {
	   WriteChatColor("Starting Explorer.exe");
	   system("%SystemRoot%\\explorer.exe");
	   WriteChatColor("Done.");
   } else {
		DebugSpewAlways("MQ2Shutdown::Syntax Error (/optimize)\nUsage: /explorer [kill|start]\n");
   }
}
void cmdServices(PSPAWNINFO pChar, PCHAR szLine) {
   if (!stricmp(szLine,"start")) {
		WriteChatColor("Starting services ...", CONCOLOR_GREEN);
		
		int i=0;  
		do {  
			CHAR szTemp[MAX_STRING]={0};
			CHAR szBuffer[MAX_STRING]={0};
			CHAR strFeedback[MAX_STRING]={0};
			
			sprintf(szTemp,"Service%d",i);  
			GetPrivateProfileString("Services",szTemp,"notfound",szBuffer,MAX_STRING,INIFileName);  
			if (!strcmp(szBuffer,"notfound"))   break; 	
			sprintf(strFeedback, "~ Starting %s ...", szBuffer);
			WriteChatColor(strFeedback, CONCOLOR_GREEN);
			
			SC_HANDLE hscm;
			hscm = ConnectToSCM();
			StartCmd(hscm, szBuffer);
			DisconnectFromSCM(hscm);			 
			 
		} while (++i);  
		
		WriteChatColor("Done.", CONCOLOR_GREEN);


   } else if (!stricmp(szLine,"stop")) {
		WriteChatColor("Stopping services ...", CONCOLOR_BLUE);
		
		int i=0;  
		do {  
			CHAR szTemp[MAX_STRING]={0};
			CHAR szBuffer[MAX_STRING]={0};
			CHAR strFeedback[MAX_STRING]={0};
			
			sprintf(szTemp,"Service%d",i);  
			GetPrivateProfileString("Services",szTemp,"notfound",szBuffer,MAX_STRING,INIFileName);  
			if (!strcmp(szBuffer,"notfound"))   break; 	
			sprintf(strFeedback, "~ Stopping %s ...", szBuffer);
			WriteChatColor(strFeedback, CONCOLOR_BLUE);
			
			SC_HANDLE hscm;
			hscm = ConnectToSCM();
			StopCmd(hscm, szBuffer);
			DisconnectFromSCM(hscm);			 
			 
		} while (++i);  
		
		WriteChatColor("Done.", CONCOLOR_BLUE);

   } else {
		DebugSpewAlways("MQ2Shutdown::Syntax Error (/optimize)\nUsage: /explorer [kill|start]\n");
   }
}

PLUGIN_API VOID InitializePlugin(VOID) {
	DebugSpewAlways("Initializing MQ2WinXP");
	AddCommand("/WinSession",cmdWSession);
	AddCommand("/WinExplorer",cmdExplorer);
	AddCommand("/WinServices",cmdServices);
}

PLUGIN_API VOID ShutdownPlugin(VOID) {
	DebugSpewAlways("Shutting down MQ2Shutdown");
	RemoveCommand("/WinSession");
	RemoveCommand("/WinExplorer");
	RemoveCommand("/WinServices");
}

void serviceMgr(string name, string cmd) {

	wstring attribute, value1, value2, msg;
	SC_HANDLE hscm;
	// Connect to the service control manager 
	hscm = ConnectToSCM();
	// Handle the command
	StartCmd(hscm, name);
	// Disconnect from the SCM and exit
	DisconnectFromSCM(hscm);

}

SC_HANDLE ConnectToSCM() {
	// Connect to the Service Control Manager.
	SC_HANDLE hscm;
	hscm = OpenSCManager(0, 0, SC_MANAGER_CONNECT | SC_MANAGER_LOCK);
	return hscm;
}

void DisconnectFromSCM(SC_HANDLE hscm) {
	// Disconnect from the service control manager.
	CloseServiceHandle(hscm);
	return;
}

void StartCmd(SC_HANDLE hscm, string name) {
	// Start a given service.
	SC_HANDLE hsvc;
	hsvc = OpenService(hscm, name.c_str(), SERVICE_START);
	if ( !StartService(hsvc, 0, 0) ) WriteChatColor("Could not start services.", CONCOLOR_RED);
	CloseServiceHandle(hsvc);
}

void StopCmd(SC_HANDLE hscm, string name) {
	// Stop a given service.
	SC_HANDLE hsvc;
	SERVICE_STATUS ss;
	hsvc = OpenService(hscm, name.c_str(), SERVICE_STOP);
	if ( !ControlService(hsvc, SERVICE_CONTROL_STOP, &ss) ) WriteChatColor("Can not stop the service.");
	CloseServiceHandle(hsvc);
}

