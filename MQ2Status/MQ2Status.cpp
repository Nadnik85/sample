// MQ2Status.cpp : Defines the entry point for the DLL application.
/**
MQ2Status by Chatwiththisname and Sic
Commands: 
    /status

Purpose: 
	to quickly display your "status" as relevant to if:
		a class plugin is running
		a macro is running
			if the macro is kissassist variation
				what your role is
			If the macro is paused
		if you are hidden
Usage:
	using the command /status to output to /bc the information in purpose. 
	can be used with "/bcaa //status" to get all members of eqbc to output their current status (also bcga, bca, bct etc as needed by the user).
**/
#include "../MQ2Plugin.h"
#include "StatusFunctions.h"


PreSetup("MQ2Status");

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	if (HaveAlias("/status")) {
		WriteChatf("\ar[\a-tMQ2Status\ar]\ao:: \arIt appears you already have an Alias for \ap/status\ar please type \"\ay/alias /status delete\ar\" then reload this plugin.");
			EzCommand("/timed 10 /plugin MQ2Status Unload");
	}
	else {
		AddCommand("/status", StatusCmd);
	}
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	RemoveCommand("/status");
}

/* THIS IS A BLOCK COMMENT!
// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState)
{
    //if (GameState==GAMESTATE_INGAME)
	//Wasn't sure if this would ever be needed, so holding onto it.
}


// This is called every time MQ pulses (MainLOOP!)
PLUGIN_API VOID OnPulse(VOID)
{
	//Not sure if this will ever bee needed, so holding onto it. 
}

// This is called every time WriteChatColor is called by MQ2Main or any plugin,
// IGNORING FILTERS, IF YOU NEED THEM MAKE SURE TO IMPLEMENT THEM. IF YOU DONT
// CALL CEverQuest::dsp_chat MAKE SURE TO IMPLEMENT EVENTS HERE (for chat plugins)
PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter)
{
	//Wasn't sure if this would ever be needed, so hanging onto it.
    return 0;
}

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
// but after MQ filters and chat events are taken care of.
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
	//Wasn't sure if this would ever be needed, so hanging onto it.
    return 0;
}
*/