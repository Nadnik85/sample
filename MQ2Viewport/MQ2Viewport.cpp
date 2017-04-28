// MQ2ViewPort.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.

// MQ2ViewPort - v1.0 - Written by jimbob for RedGuides

#include "../MQ2Plugin.h"

PreSetup("MQ2ViewPort");

bool gbInitDone = false;
bool VPResize = false;
bool Resized = false;
bool Foreground = true;
bool FGState = false;
bool FGChanged = false;
bool ForceResize = false;

HMODULE EQWhMod = 0;
typedef HWND(__stdcall *fEQW_GetDisplayWindow)(VOID);
fEQW_GetDisplayWindow EQW_GetDisplayWindow = 0;

// Stolen from MQ2Rez - jimbob
bool SetBOOL(bool Cur, PCHAR Val, PCHAR Sec, PCHAR Key) {
    char buffer[128]; bool result = false;
    if (!_strnicmp(Val, "false", 5) || !_strnicmp(Val, "off", 3) || !_strnicmp(Val, "0", 1)) 
		result = false;
    else if (!_strnicmp(Val, "true", 4) || !_strnicmp(Val, "on", 2) || !_strnicmp(Val, "1", 1))
		result = true;
    else 
		result = (!Cur) & 1;
    if (Sec[0] && Key[0])
		WritePrivateProfileString(Sec, Key, result ? "1" : "0", INIFileName);
    sprintf_s(buffer, "%s::%s (%s)", Sec, Key, result ? "\agon\ax" : "\agoff\ax");
    WriteChatColor(buffer);
    return result;
}

void VPCommand(PSPAWNINFO pCHAR, PCHAR zLine)
{
    bool help = false;
    char Param1[MAX_STRING];
    char Param2[MAX_STRING];
    GetArg(Param1, zLine, 1);
    GetArg(Param2, zLine, 2);

    if (!Param1[0])
    {
        WriteChatf("Plugin: MQ2ViewPort");
        WriteChatf("/vp on/off -> Enable Viewport Resize when EQ is in background window.");
        WriteChatf("MQ2ViewPort::VPResize (%s)", VPResize ? "\agon\ax" : "\agoff\ax");
    }
    else if (!_stricmp("on", Param1))
    {
        VPResize = SetBOOL(VPResize, Param1, "MQ2ViewPort", "VPResize");
        Resized = true;
        ForceResize = true;
    }
    else if (!_stricmp("off", Param1))
    {
        VPResize = SetBOOL(VPResize, Param1, "MQ2ViewPort", "VPResize");
        EzCommand("/viewport reset");
    }
}
void InitSettings()
{
	//well we cant really read the character name unless we are at least at charselect
	//but this plugin really doesnt need it until we are in game
	//so lets not mess with charselect inireading...
	if(!gbInitDone) {
		if(PCHARINFO pChar = GetCharInfo()) {
			sprintf_s(INIFileName, "%s\\%s_%s.ini", gszINIPath, EQADDR_SERVERNAME, pChar->Name);
			VPResize = GetPrivateProfileInt("MQ2ViewPort", "VPResize", 0, INIFileName) ? true : false;
			gbInitDone = true;
		}
	}

}
// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
    VPResize = false;    // Will get this from .ini later...
    Foreground = true;
    FGState = true;
    FGChanged = false;
    Resized = false;

    DebugSpewAlways("Initializing MQ2ViewPort");
    if (EQWhMod = GetModuleHandle("eqw.dll"))
    {
        EQW_GetDisplayWindow = (fEQW_GetDisplayWindow)GetProcAddress(EQWhMod, "EQW_GetDisplayWindow");
    }
    //Add commands, MQ2Data items, hooks, etc.
    AddCommand("/vp", VPCommand);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
    DebugSpewAlways("Shutting down MQ2ViewPort");
	//its dangerous to call functions while unloading, mq2 might be unloaded as well, and then we crash the user.
	//If he decides to unload this plugin while the viewport is not reset, screw him, he can do a /viewport reset
	//when its unloaded or before he unloads it.
    //EzCommand("/viewport reset");
    RemoveCommand("/vp");
}

// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState)
{
    DebugSpewAlways("MQ2ViewPort::SetGameState()");
    if (GameState == GAMESTATE_INGAME)
    {
		if(!gbInitDone) {
			InitSettings();
		}
    } else {
		if(gbInitDone) {
			gbInitDone = false;
		}
	}
    // create custom windows if theyre not set up, etc
}
// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
    if(GetGameState()==GAMESTATE_INGAME) {
		if(!gbInitDone) {
			InitSettings();
		}
		HWND EQhWnd = *(HWND*)EQADDR_HWND;
		if (EQW_GetDisplayWindow)
			EQhWnd = EQW_GetDisplayWindow();
		if (!VPResize)
			return;

		if (GetForegroundWindow() == EQhWnd)
		{
			Foreground = true;
			if (Foreground == FGState)
				FGChanged = false;
			else
			{
				FGChanged = true;
				FGState = Foreground;
				Resized = false;
			}
		}
		else
		{
			Foreground = false;
			if (Foreground == FGState)
				FGChanged = false;
			else
			{
				FGChanged = true;
				FGState = Foreground;
				Resized = false;
			}
		}
		if ((FGChanged) || (ForceResize))
		{
			if (Foreground)
			{
				if ((!Resized)||ForceResize)
				{
					EzCommand("/viewport reset");
					Resized = true;
				}
			}
			else if (!Foreground)
			{
				if ((!Resized) || ForceResize)
				{
					EzCommand("/viewport 0 0 1 1");
					Resized = true;
				}
			}
			ForceResize = false;
		}
	}
}