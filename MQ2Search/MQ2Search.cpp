// MQ2Search.cpp : Defines the entry point for the DLL application.
#include "../MQ2Plugin.h"

PreSetup("MQ2Search");

// GLOBAL VARIABLES


// FUNCTION PROTOTYPES
void cmdSearchAlla (PSPAWNINFO, PCHAR);
void WriteMessage( const char* );

// FUNCTIONS
void cmdSearchAlla (PSPAWNINFO pChar, PCHAR szLine) {
	char szOption[MAX_STRING];
	//char szArg[MAX_STRING];
	char szURL[MAX_STRING];

	GetArg(szOption, szLine, 1);
	sprintf_s(szURL,"");

	if (!strcmp (szOption, "item")) {
		strcpy_s(szURL, "/www everquest.allakhazam.com/search.html?q=");
		if ( szLine[5] == 0x12) {
			string strLine(szLine + 61);
			const char* cnstCharLine = strLine.c_str();
			strcat_s(szURL, cnstCharLine);
		} else {
			string strLine(szLine + 5);
			const char* cnstCharLine = strLine.c_str();
			strcat_s(szURL, cnstCharLine);
		}
	}

	if (!strcmp (szOption, "npc")) {
		strcpy_s(szURL, "/www eqbeastiary.allakhazam.com/search.shtml?name=");
		string strLine(szLine + 4);
		const char* cnstCharLine = strLine.c_str();
		strcat_s(szURL, cnstCharLine);
	}

	
	if (strlen(szURL)) {
		DoCommand(pChar, szURL);
	} else {
		SyntaxError("MQ2Search - Opens a web browser to search Allakhazam.com for items and npc's.");
		SyntaxError("Usage: /alla [item|npc] [string|item link]");
		SyntaxError("Note: NPC search does not support ${Target.Name}");
	}
		
}

void cmdSearchEQTraders (PSPAWNINFO pChar, PCHAR szLine) {
	char szOption[MAX_STRING];
	char szURL[MAX_STRING];
	
	sprintf_s(szURL,"");

	GetArg(szOption, szLine, 1);

	if (!strcmp (szOption, "i")) {
		strcpy_s(szURL, "/www eqtraders.com/i?");
		string strLine(szLine + 2);
		const char* cnstCharLine = strLine.c_str();
		strcat_s(szURL, cnstCharLine);
	}

	if (!strcmp (szOption, "r")) {
		strcpy_s(szURL, "/www eqtraders.com/r?");
		string strLine(szLine + 2);
		const char* cnstCharLine = strLine.c_str();
		strcat_s(szURL, cnstCharLine);
	}
	
	if (strlen(szURL)) {
		DoCommand(pChar, szURL);
	} else {
		SyntaxError("MQ2Search - Opens a web browser to search eqtrader.com for items and recipes.");
		SyntaxError("Usage: /eqtrader [i|r] [item link]");
	}
}

void WriteMessage( const char* cMsg ) {
	char szMsg[MAX_STRING];
	strcpy_s( szMsg, cMsg);
	WriteChatColor(szMsg, CONCOLOR_GREEN);	
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID) {
	WriteMessage("MQ2Search v1.01 Loaded");
	AddCommand("/alla",cmdSearchAlla);
	AddCommand("/eqtrader", cmdSearchEQTraders);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID) {
	RemoveCommand("/alla");
	RemoveCommand("/eqtrader");

}