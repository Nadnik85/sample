#include "DiscordClient.h"
#include <nlohmann\json.hpp>
#include <fstream>
#include <regex>

using nlohmann::json;

// Sleepy discord library is compiled with default struct align, MQ2 needs 4 byte
// Even with this pragma push/pop, having headers for both included in the same cpp causes stack corruption :(
// So this file contains MQ2 stuff, other file contains discord stuff
#pragma pack(push, 4)
#include "../MQ2Plugin.h"
#pragma pack(pop)

PreSetup("MQ2Discord");

json config;

unsigned int __stdcall MQ2DataVariableLookup(char * VarName, char * Value, size_t ValueLen);
Blech blechAllow('#', '|', MQ2DataVariableLookup);
Blech blechBlock('#', '|', MQ2DataVariableLookup);
std::map<std::string, unsigned int> eventsAllow;
std::map<std::string, unsigned int> eventsBlock;

// MQ2Main isn't nice enough to export this
unsigned int __stdcall MQ2DataVariableLookup(char * VarName, char * Value, size_t ValueLen)
{
	strcpy_s(Value, ValueLen, VarName);
	if (!pLocalPlayer)
		return strlen(Value);
	return strlen(ParseMacroParameter((PSPAWNINFO)pLocalPlayer, Value, ValueLen));
}

void __stdcall BlechEvent(unsigned int ID, void * pData, PBLECHVALUE pValues)
{
}

void Connect(std::string connectMsg)
{
	if (g_pThread)
	{
		WriteChatf("[MQ2Discord] Already connected");
		return;
	}

	std::string token = config["token"].get<std::string>();
	std::string channel = config["channel"].get<std::string>();
	std::string allowedUser = config["user"].get<std::string>();

	if (token == "")
	{
		WriteChatf("[MQ2Discord] No token configured");
		return;
	}
	if (channel == "")
	{
		WriteChatf("[MQ2Discord] No channel configured");
		return;
	}
	if (allowedUser == "")
	{
		WriteChatf("[MQ2Discord] No user configured");
		return;
	}

	// Set run flag to true and start thread
	bRun = true;
	bFinished = false;
	g_pThread = new std::thread(DiscordThread, token, channel, allowedUser, connectMsg);
}

void Disconnect()
{
	if (!g_pThread)
	{
		WriteChatf("[MQ2Discord] Not connected");
		return;
	}

	// Set run flag to false and wait for thread to stop
	bRun = false;
	g_pThread->join();
	g_pThread = nullptr;
	return;
}

void SaveConfig(PCHAR szServer, PCHAR szCharName)
{
	std::ofstream configFile(std::string(gszINIPath) + "\\MQ2Discord_" + szServer + "_" + szCharName + ".json");
	if (configFile.is_open())
		configFile << std::setw(4) << config << std::endl;
}

void LoadConfig(PCHAR szServer, PCHAR szCharName)
{
	std::ifstream configFile(std::string(gszINIPath) + "\\MQ2Discord_" + szServer + "_" + szCharName + ".json");
	if (configFile.is_open())
	{
		configFile >> config;
		configFile.close();
		WriteChatf("[MQ2Discord] Loaded config");
	}
	else
	{
		WriteChatf("[MQ2Discord] Failed to load config");
	}

	// Set some defaults & save
	if (config["token"] == nullptr)
		config["token"] = "";
	if (config["channel"] == nullptr)
		config["channel"] = "";
	if (config["user"] == nullptr)
		config["user"] = "";
	if (config["allow"] == nullptr)
		config["allow"] = { "[MQ2Discord]#*#", "[MQ2]#*#" };
	if (config["block"] == nullptr)
		config["block"] = json::array();
	if (config["autoconnect"] == nullptr)
		config["autoconnect"] = true;

	SaveConfig(szServer, szCharName);
	
	// Load events
	blechAllow.Reset();
	blechBlock.Reset();
	eventsAllow.clear();
	eventsBlock.clear();

	for each (auto jEvent in config["allow"])
		eventsAllow[jEvent.get<std::string>()] = blechAllow.AddEvent(jEvent.get<std::string>().c_str(), BlechEvent);
	for each (auto jEvent in config["block"])
		eventsBlock[jEvent.get<std::string>()] = blechBlock.AddEvent(jEvent.get<std::string>().c_str(), BlechEvent);
}

VOID DiscordCmd(PSPAWNINFO pChar, PCHAR szLine)
{
	// I wonder how many std::strings I create unnecessarily in this function...
	char buffer[MAX_STRING] = { 0 };

	GetArg(buffer, szLine, 1);

	if (!_stricmp(buffer, "connect"))
	{
		Connect(std::string("Connected: ") + pChar->Name);
		return;
	}

	if (!_stricmp(buffer, "disconnect"))
	{
		return Disconnect();
	}

	if (!_stricmp(buffer, "autoconnect"))
	{
		char * setting = GetNextArg(szLine, 1);
		if (setting && *setting && !_stricmp(setting, "on"))
		{
			config["autoconnect"] = true;
			SaveConfig(EQADDR_SERVERNAME, pChar->Name);
		}
		else if (setting && *setting && !_stricmp(setting, "off"))
		{
			config["autoconnect"] = false;
			SaveConfig(EQADDR_SERVERNAME, pChar->Name);
		}
		
		WriteChatf("[MQ2Discord] Autoconnect: %s", config["autoconnect"].get<bool>() ? "true" : "false");

		return;
	}

	if (!_stricmp(buffer, "allow"))
	{
		char * szEvent = GetNextArg(szLine, 1);

		if (szEvent && *szEvent)
		{
			if (!eventsAllow.count(szEvent))
			{
				// Add event if it doesn't exist. Goes in 3 places, the config, blech, and our map of text to blech id (for later removal)
				eventsAllow[szEvent] = blechAllow.AddEvent(szEvent, BlechEvent);
				config["allow"] += szEvent;
				WriteChatf("[MQ2Discord] Added allow event: %s", szEvent);
			}
			else
			{
				// Remove it if it does
				blechAllow.RemoveEvent(eventsAllow[szEvent]);
				eventsAllow.erase(szEvent);
				for (int i = 0; i < config["allow"].size(); i++) // No nice remove method on json array, and I couldn't get std::find working
					if (config["allow"][i] == szEvent)
					{
						config["allow"].erase(i);
						break;
					}

				WriteChatf("[MQ2Discord] Removed allow event: %s", szEvent);
			}

			SaveConfig(EQADDR_SERVERNAME, pChar->Name);
		}
		else
		{
			WriteChatf("[MQ2Discord] Allowing the following events:");
			for each (auto kvp in eventsAllow)
			{
				WriteChatf("[MQ2Discord] %s", kvp.first.c_str());
			}
		}
		return;
	}

	if (!_stricmp(buffer, "block"))
	{
		char * szEvent = GetNextArg(szLine, 1);

		if (szEvent && *szEvent)
		{
			// Add event if it doesn't exist. Goes in 3 places, the config, blech, and our map of text to blech id (for later removal)
			if (!eventsBlock.count(szEvent))
			{
				eventsBlock[szEvent] = blechBlock.AddEvent(szEvent, BlechEvent);
				config["block"] += szEvent;
				SaveConfig(EQADDR_SERVERNAME, pChar->Name);
				WriteChatf("[MQ2Discord] Added block event: %s", szEvent);
			}
			else
			{
				// Remove it if it does
				blechBlock.RemoveEvent(eventsBlock[szEvent]);
				eventsBlock.erase(szEvent);
				for (int i = 0; i < config["allow"].size(); i++)
					if (config["allow"][i] == szEvent)
					{
						config["allow"].erase(i);
						break;
					}
				SaveConfig(EQADDR_SERVERNAME, pChar->Name);
				WriteChatf("[MQ2Discord] Removed block event: %s", szEvent);
			}
		}
		else
		{
			WriteChatf("[MQ2Discord] Blocking the following events:");
			for each (auto kvp in eventsBlock)
			{
				WriteChatf("[MQ2Discord] %s", kvp.first.c_str());
			}
		}
		return;
	}

	if (!_stricmp(buffer, "token"))
	{
		char * szToken = GetNextArg(szLine, 1);

		if (szToken && *szToken)
		{
			config["token"] = szToken;
			SaveConfig(EQADDR_SERVERNAME, pChar->Name);
		}

		WriteChatf("[MQ2Discord] Token: %s", config["token"].get<std::string>().c_str());

		return;
	}

	if (!_stricmp(buffer, "channel"))
	{
		char * szChannel = GetNextArg(szLine, 1);

		if (szChannel && *szChannel)
		{
			config["channel"] = szChannel;
			SaveConfig(EQADDR_SERVERNAME, pChar->Name);
		}

		WriteChatf("[MQ2Discord] Channel: %s", config["channel"].get<std::string>().c_str());

		return;
	}

	if (!_stricmp(buffer, "user"))
	{
		char * szUser = GetNextArg(szLine, 1);

		if (szUser && *szUser)
		{
			config["user"] = szUser;
			SaveConfig(EQADDR_SERVERNAME, pChar->Name);
		}

		WriteChatf("[MQ2Discord] User: %s", config["user"].get<std::string>().c_str());

		return;
	}

	WriteChatf("[MQ2Discord] Usage:");
	WriteChatf("[MQ2Discord] /discord [connect|disconnect] - Connect or disconnect from discord server");
	WriteChatf("[MQ2Discord] /discord autoconnect [on|off] - Set or display autoconnect on load");
	WriteChatf("[MQ2Discord] /discord token [<token>] - Set or display token used to authenticate");
	WriteChatf("[MQ2Discord] /discord channel [<channel id>] - Set or display channel id to output messages to");
	WriteChatf("[MQ2Discord] /discord user [<user id>] - Set or display user id to accept commands from");
	WriteChatf("[MQ2Discord] /discord allow|block [<event>] - Toggle or list events (MQ2 syntax) for output in discord");
}

PLUGIN_API VOID InitializePlugin(VOID)
{
	AddCommand("/discord", DiscordCmd, 0, 0, 1);
}

PLUGIN_API VOID ShutdownPlugin(VOID)
{
	if (g_pThread)
		Disconnect();
	RemoveCommand("/discord");
}

PLUGIN_API VOID OnPulse(VOID)
{
	// Process any received messages
	std::string msg;
	while (fromDiscord.tryDequeue(msg))
	{
		WriteChatf("[MQ2Discord] %s", msg.c_str());

		if (msg[0] == '/')
			DoCommand((PSPAWNINFO)pLocalPlayer, (PCHAR)msg.c_str());
	}

	// Check our thead - if we want to run, and it's not running, it's disconnected for whatever reason. Reconnect it
	if (bRun && bFinished && config["autoconnect"].get<bool>() && GetGameState() == GAMESTATE_INGAME)
	{
		Disconnect();
		Connect(std::string("Reconnected: ") + ((PSPAWNINFO)pLocalPlayer)->Name);
	}
}


void EnqueueIfMatch(const PCHAR Line)
{
	char buffer[MAX_STRING] = { 0 };
	
	// Remove any colours
	StripMQChat(Line, buffer);

	// Remove any item links
	std::regex re("\x12[A-F0-9]{56}([^\x12]*)\x12");
	strcpy_s(buffer, std::regex_replace(buffer, re, "$1").c_str());
	
	if (bRun.load() && blechAllow.Feed(buffer) && !blechBlock.Feed(buffer))
		toDiscord.enqueue(buffer);
}

PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter)
{
	EnqueueIfMatch(Line);
    return 0;
}

PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
	EnqueueIfMatch(Line);
    return 0;
}

PLUGIN_API VOID SetGameState(DWORD GameState)
{
	if (GameState == GAMESTATE_INGAME)
	{
		LoadConfig(EQADDR_SERVERNAME, ((PSPAWNINFO)pLocalPlayer)->Name);

		if (config["autoconnect"].get<bool>())
			Connect(std::string("Connected: ") + ((PSPAWNINFO)pLocalPlayer)->Name);
	}
	else
	{
		Disconnect();
	}
}