// MQ2Speaker.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.

#undef CINTERFACE

#include <sapi.h>
#include <Shobjidl.h>
#include <windows.h>
#include <atlstr.h>
#include <regex>

#include "../MQ2Plugin.h"

#undef MakeLower

#define VNAME(v) #v

PLUGIN_VERSION(1.0); 
PreSetup("MQ2Speaker");

static HRESULT coini_result = RPC_E_CHANGED_MODE;
static HRESULT voice_ini = -1;
static ISpVoice* voice = nullptr;
static bool muted = false;
static bool echoing = false;
constexpr char* section_name = "Settings";

static void WritePrivate(const bool value, const char* key_name)
{
	WritePrivateProfileString(section_name, key_name, value ? "1" : "0", INIFileName);
}

static void ReadPrivate(const char* key_name, bool& value)
{
	constexpr auto max_size = 16;
	CHAR buf[max_size] = { 0 };
	GetPrivateProfileString(section_name, key_name, nullptr, buf, max_size, INIFileName);
	if (buf[0])
	{
		CString str(buf);
		str.MakeLower();
		if (str == "0" || str == "false")
			value = false;
		else if (str == "1" || str == "true")
			value = true;
	}
}

static void WriteChat(const char* text) {
	WriteChatColor(const_cast<char*>(text), USERCOLOR_DEFAULT);
}

static void WriteChat(const string& text) {
	WriteChat(text.c_str());
}

enum Concurrency {sync,async};

static void speak(const string& text, const Concurrency concurrency)
{
	if (text.empty()) return;
	if (!muted && SUCCEEDED(voice_ini))
	{
		DWORD flags = SPF_IS_XML;
		if (concurrency == async) flags |= SVSFlagsAsync;
		voice->Speak(CStringW(text.c_str()), flags, nullptr);
	}
	if (echoing) WriteChat(text);
}

static void toggle_or_set_and_save(const string& str, const char* varname, bool& var)
{
	auto old = var;
	if (str.empty())
	{
		var = !var;
	}
	else
	{
		var = str.c_str()[0] == '1';
	}
	if (var != old) WritePrivate(var, varname);
}

static void open_control_panel()
{
	IOpenControlPanel* panel = nullptr;
	auto hr = CoCreateInstance(CLSID_OpenControlPanel, NULL, CLSCTX_ALL, IID_IOpenControlPanel, (void **)&panel);
	if (hr != S_OK) return;
	panel->Open(L"Microsoft.SpeechRecognition", nullptr, nullptr);
	panel->Release();
}

static void display_usage()
{
	WriteChat("MQ2Speaker usage:");
	WriteChat("/speaker speak Hello world -> Say <Hello world>, wait for completion");
	WriteChat("/speaker speaka Hello world -> Say <Hello world>, don't wait for completion");
	WriteChat("/speaker mute -> Toggle muting of the voice");
	WriteChat("/speaker mute 0|1 -> Set muting");
	WriteChat("/speaker echo -> Toggle echoing of the voice into the MQ2 chat window");
	WriteChat("/speaker echo 0|1 -> Set echoing");
	WriteChat("/speaker panel -> Open the Windows speech recognition control panel");
}

static VOID execute_speaker_command(PSPAWNINFO pChar, const PCHAR command)
{
	using namespace std::regex_constants;
	static const regex speak_command(R"(^speak\s+(\S+.*))", icase);
	static const regex speaka_command(R"(^speaka\s+(\S+.*))", icase);
	static const regex mute_command(R"(^mute(?:\s+(0|1))?$)", icase);
	static const regex echo_command(R"(^echo(?:\s+(0|1))?$)", icase);
	static const regex panel_command(R"(^panel\s*)", icase);

	DebugSpewAlways("Processing command in MQ2Speaker");
	if (!command || !command[0]) return;
	cmatch submatches;
	if (regex_match(command, submatches, speak_command))
	{
		speak(submatches.str(1), sync);
	}
	else if (regex_match(command, submatches, speaka_command))
	{
		speak(submatches.str(1), async);
	}
	else if (regex_match(command, submatches, mute_command))
	{
		toggle_or_set_and_save(submatches.str(1), VNAME(muted), muted);
	}
	else if (regex_match(command, submatches, echo_command))
	{
		toggle_or_set_and_save(submatches.str(1), VNAME(echoing), echoing);
	}
	else if (regex_match(command, submatches, panel_command))
	{
		open_control_panel();
	}
	else
	{
		display_usage();
	}
}

static bool speaker_done()
{
	if (SUCCEEDED(voice_ini))
	{
		SPVOICESTATUS vs;
		voice->GetStatus(&vs, nullptr);
		return (vs.dwRunningState == SPRS_DONE);
	}
	return true;
}

static char* tlo_name = "Speaker";

class MQ2SpeakerType : public MQ2Type
{
public:
	enum SpeakerMembers { Muted = 1, Echoing, Done, Exists };

	MQ2SpeakerType() : MQ2Type(tlo_name)
	{
		TypeMember(Muted);
		TypeMember(Echoing);
		TypeMember(Done);
		TypeMember(Exists);
	}

	bool GetMember(MQ2VARPTR VarPtr, char* Member, char* Index, MQ2TYPEVAR &Dest)
	{
		PMQ2TYPEMEMBER pMember = MQ2SpeakerType::FindMember(Member);
		if (pMember)
		{
			switch ((SpeakerMembers)pMember->ID)
			{
			case Muted:
				Dest.DWord = muted;
				Dest.Type = pBoolType;
				return true;
			case Echoing:
				Dest.DWord = echoing;
				Dest.Type = pBoolType;
				return true;
			case Done:
				Dest.DWord = speaker_done();
				Dest.Type = pBoolType;
				return true;
			case Exists:
				Dest.DWord = SUCCEEDED(voice_ini);
				Dest.Type = pBoolType;
				return true;
			}
		}
		return false;
	}

	bool ToString(MQ2VARPTR VarPtr, PCHAR Destination)
	{
		strcpy_s(Destination, MAX_STRING, "TRUE");
		return true;
	}

	bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source)
	{
		return false;
	}

	bool FromString(MQ2VARPTR &VarPtr, PCHAR Source)
	{
		return false;
	}
};

static MQ2SpeakerType speaker_type;

static BOOL return_speaker_tlo(char* szName, MQ2TYPEVAR &Ret)
{
	Ret.DWord = 1;
	Ret.Type = &speaker_type;
	return true;
}

static char* speaker_command = "/speaker";

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
    DebugSpewAlways("Initializing MQ2Speaker");

	coini_result = ::CoInitialize(NULL);
	voice_ini = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&voice);

    //Add commands, MQ2Data items, hooks, etc.
    AddCommand(speaker_command, execute_speaker_command);
	AddMQ2Data(tlo_name, return_speaker_tlo);
    //AddXMLFile("MQUI_MyXMLFile.xml");
    //bmMyBenchmark=AddMQ2Benchmark("My Benchmark Name");
	ReadPrivate(VNAME(muted), muted);
	ReadPrivate(VNAME(echoing), echoing);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
    DebugSpewAlways("Shutting down MQ2Speaker");

	RemoveMQ2Data(tlo_name);
	RemoveCommand(speaker_command);
	if (SUCCEEDED(voice_ini))
	{
		voice->Release();
		voice = nullptr;
		voice_ini = -1;
	}
	if (coini_result != RPC_E_CHANGED_MODE)
	{
		::CoUninitialize();
		coini_result = RPC_E_CHANGED_MODE;
	}
	
		//Remove commands, MQ2Data items, hooks, etc.
    //RemoveMQ2Benchmark(bmMyBenchmark);
    //RemoveXMLFile("MQUI_MyXMLFile.xml");
}

/*
// Called after entering a new zone
PLUGIN_API VOID OnZoned(VOID)
{
    DebugSpewAlways("MQ2Speaker::OnZoned()");
}

// Called once directly before shutdown of the new ui system, and also
// every time the game calls CDisplay::CleanGameUI()
PLUGIN_API VOID OnCleanUI(VOID)
{
    DebugSpewAlways("MQ2Speaker::OnCleanUI()");
    // destroy custom windows, etc
}

// Called once directly after the game ui is reloaded, after issuing /loadskin
PLUGIN_API VOID OnReloadUI(VOID)
{
    DebugSpewAlways("MQ2Speaker::OnReloadUI()");
    // recreate custom windows, etc
}

// Called every frame that the "HUD" is drawn -- e.g. net status / packet loss bar
PLUGIN_API VOID OnDrawHUD(VOID)
{
    // DONT leave in this debugspew, even if you leave in all the others
    //DebugSpewAlways("MQ2Speaker::OnDrawHUD()");
}

// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState)
{
    DebugSpewAlways("MQ2Speaker::SetGameState()");
    //if (GameState==GAMESTATE_INGAME)
    // create custom windows if theyre not set up, etc
}


// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
    // DONT leave in this debugspew, even if you leave in all the others
    //DebugSpewAlways("MQ2Speaker::OnPulse()");
}

// This is called every time WriteChatColor is called by MQ2Main or any plugin,
// IGNORING FILTERS, IF YOU NEED THEM MAKE SURE TO IMPLEMENT THEM. IF YOU DONT
// CALL CEverQuest::dsp_chat MAKE SURE TO IMPLEMENT EVENTS HERE (for chat plugins)
PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter)
{
    DebugSpewAlways("MQ2Speaker::OnWriteChatColor(%s)",Line);
    return 0;
}

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
// but after MQ filters and chat events are taken care of.
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
    DebugSpewAlways("MQ2Speaker::OnIncomingChat(%s)",Line);
    return 0;
}

// This is called each time a spawn is added to a zone (inserted into EQ's list of spawns),
// or for each existing spawn when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API VOID OnAddSpawn(PSPAWNINFO pNewSpawn)
{
    DebugSpewAlways("MQ2Speaker::OnAddSpawn(%s)",pNewSpawn->Name);
}

// This is called each time a spawn is removed from a zone (removed from EQ's list of spawns).
// It is NOT called for each existing spawn when a plugin shuts down.
PLUGIN_API VOID OnRemoveSpawn(PSPAWNINFO pSpawn)
{
    DebugSpewAlways("MQ2Speaker::OnRemoveSpawn(%s)",pSpawn->Name);
}

// This is called each time a ground item is added to a zone
// or for each existing ground item when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API VOID OnAddGroundItem(PGROUNDITEM pNewGroundItem)
{
    DebugSpewAlways("MQ2Speaker::OnAddGroundItem(%d)",pNewGroundItem->DropID);
}

// This is called each time a ground item is removed from a zone
// It is NOT called for each existing ground item when a plugin shuts down.
PLUGIN_API VOID OnRemoveGroundItem(PGROUNDITEM pGroundItem)
{
    DebugSpewAlways("MQ2Speaker::OnRemoveGroundItem(%d)",pGroundItem->DropID);
}

// This is called when we receive the EQ_BEGIN_ZONE packet is received
PLUGIN_API VOID BeginZone(VOID)
{
    DebugSpewAlways("MQ2Speaker::BeginZone");
}

// This is called when we receive the EQ_END_ZONE packet is received
PLUGIN_API VOID EndZone(VOID)
{
    DebugSpewAlways("MQ2Speaker::EndZone");
}

// This is called when pChar!=pCharOld && We are NOT zoning
// honestly I have no idea if its better to use this one or EndZone (above)
PLUGIN_API VOID Zoned(VOID)
{
    DebugSpewAlways("MQ2Speaker::Zoned");
}
*/