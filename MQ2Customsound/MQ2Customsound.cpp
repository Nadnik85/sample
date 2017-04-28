
/*
*   MQ2CustomSound.cpp v1.2
*   2004.08.28 - kuv
*
*   MQ2CustomSound allows users to define sound effects for any kind of game event that causes
*   a unique text message to be displayed in-game. All events are defined in an INI file, no
*   re-compiling is necessary.
*
*   COMMANDS:
*   /togglesounds - Turns custom sound effects on/off DEFAULT: OFF
*   /npctellsounds - Turns NPC tell filter on/off, when ON - tells that come from NPCs will not
*      fire sound events DEFAULT: ON
*   /sound <sound name> - plays the sound associated with a certain key (from the INI)
*   /sound stop - immediately stops the play-back of any currently playing sound
*
*   Props to Digitalxero for the MQ2PlaySound plug-in which got me thinking about this.
*   The /sound command also comes from that plug-in.
*
*   << SEE THE README FILE FOR INSTRUCTIONS ON INI FILE SETUP! >>
*
*   Known issues:
*    - There's no real good way to make a sound event when using pop-up /tell windows as
*      they do not have any '...tells you,..." kind of message.
*
*/

#include "../MQ2Plugin.h"
#include <mmsystem.h>
#include <vector>

#ifdef WIN32_MEAN_AND_LEAN
#undef WIN32_MEAN_AND_LEAN
#endif

#pragma comment(lib,"winmm.lib")

vector<string> events;
vector<string> matchStrings;
vector<string> sounds;

bool soundsEnabled = true;
bool npcTellFilter = true;

PreSetup("MQ2CustomSound");

// PlaySound - plays sound events when using the /sound command
// Inspired by the MQ2PlaySound plug-in by Digitalxero
void PlaySoundCmd(PSPAWNINFO pChar, PCHAR szLine)
{
   CHAR szSoundName[MAX_STRING] = {0};
   CHAR szSoundFile[MAX_STRING] = {0};
   
   // Get sound file from INI
   GetArg(szSoundName,szLine,1);
   GetPrivateProfileString("Sounds",szSoundName,"default",szSoundFile,MAX_STRING,INIFileName);

   if ( _stricmp( szSoundName, "stop" ) == 0 ) {
      PlaySound(NULL,NULL,SND_ASYNC);
   } else {
      PlaySound(szSoundFile,NULL,SND_FILENAME | SND_ASYNC);      
   }
}

string GetOnOffLabel( bool val )
{
   string s;

   if( val )
      s = "\agON\ax";
   else
      s = "\auOFF\ax";

   return s;
}

// ToggleSounds - toggles custom sounds on/off
void ToggleSoundsCmd(PSPAWNINFO pChar, PCHAR szLine)
{
   soundsEnabled = !soundsEnabled;
   
   char szTemp[MAX_STRING];
   string enabledTxt = GetOnOffLabel(soundsEnabled);
   sprintf_s(szTemp,"Custom sound effects are now: %s", enabledTxt.c_str());
   WriteChatColor(szTemp);
}

// ToggleNpcTellFilter - toggles the NPC tell filter
// TRUE/ON = no tell sound events if source is an NPC
// FALSE/OFF = no special handling for tells - any match will fire sound events
void ToggleNpcTellFilterCmd(PSPAWNINFO pChar, PCHAR szLine)
{
   npcTellFilter = !npcTellFilter;

   char szTemp[MAX_STRING];
   string enabledTxt = GetOnOffLabel(npcTellFilter);
   sprintf_s(szTemp,"NPC tell sound filter is now: %s",enabledTxt.c_str());

   WriteChatColor("NPC tell sound filter suppresses tell events if the source is an NPC.");
   WriteChatColor(szTemp);   
}

// InitEvents - loads custom events and match strings from INI file
void InitEvents()
{
   CHAR szAllKeys[MAX_STRING] = {0};
   GetPrivateProfileString("Events","keys","MQ2CustomSound_Error",szAllKeys,MAX_STRING,INIFileName);

   string strAllKeys( szAllKeys );
   
   string::size_type pos = strAllKeys.find("|",0);
   while (pos != string::npos )
   {
      // Add event name to events vector
      string strTempEvent = strAllKeys.substr(0, pos);
      events.push_back( strTempEvent );

      // Get event match string from INI
      CHAR szMatchString[MAX_STRING] = {0};
      GetPrivateProfileString("MatchStrings",strTempEvent.c_str(),"MQ2CustomSound_Error",szMatchString,MAX_STRING,INIFileName);
      
      string strTempMatch ( szMatchString );
      matchStrings.push_back( strTempMatch );

      // Get sound file from INI
      CHAR szSound[MAX_STRING] = {0};
      GetPrivateProfileString("Sounds",strTempEvent.c_str(),"MQ2CustomSound_Error",szSound,MAX_STRING,INIFileName);

      string strTempSound ( szSound );
      sounds.push_back( strTempSound );

      // Erase command from AllKeys variable (so we process the next one on next loop)
      strAllKeys.erase(0, pos + 1);

      pos = strAllKeys.find("|");
   }
}

// IsNpc - checks for a spawn in the current zone by name and returns TRUE if it is an NPC
bool IsNpc( string strSpawnName )
{
   // Ghetto NPC check: Spaces in name
   if( strSpawnName.find(" ") != string::npos )
      return true;

   // Real NPC check: cycle through spawn list
    PSPAWNINFO pSpawn = (PSPAWNINFO)pSpawnList;

   while (pSpawn)
   {
      string tmpName (pSpawn->Name);
      if ( tmpName.find(strSpawnName) != string::npos )
      {
         // Found a name match - check if it's an NPC
         if ( pSpawn->Type != 0 )
            return true;
      }
      pSpawn = pSpawn->pNext;
   }

   return false;
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
   DebugSpewAlways("Initializing MQ2CustomSound");

   // Add commands, MQ2Data items, hooks, etc.
   // AddCommand("/mycommand",MyCommand);
   // AddXMLFile("MQUI_MyXMLFile.xml");
   // bmMyBenchmark=AddMQ2Benchmark("My Benchmark Name");
   
   AddCommand("/sound",PlaySoundCmd);
   AddCommand("/togglesounds",ToggleSoundsCmd);
   AddCommand("/npctellsounds",ToggleNpcTellFilterCmd);

   InitEvents();
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
   DebugSpewAlways("Shutting down MQ2CustomSound");

   // Remove commands, MQ2Data items, hooks, etc.
   // RemoveMQ2Benchmark(bmMyBenchmark);
   // RemoveCommand("/mycommand");
   // RemoveXMLFile("MQUI_MyXMLFile.xml");

   RemoveCommand("/sound");
   RemoveCommand("/togglesounds");
   RemoveCommand("/npctellsounds");
}

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
// but after MQ filters and chat events are taken care of.
//
// MQ2CustomSound uses this function to start playing custom sounds
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
   DebugSpewAlways("MQ2CustomSound::OnIncomingChat(%s)",Line);

    // Only process sound events if in-game
   if (MQ2Globals::gGameState != GAMESTATE_INGAME)
      return 0;
    // Only process sound events if sounds are enabled
   if( !soundsEnabled )
      return 0;

    // Process all sound events for matches   
   for( unsigned int i = 0; i < events.size(); i++ )
   {
      string strLine ( Line );
      string::size_type pos = strLine.find(matchStrings[i],0);

      if ( pos != string::npos ) {

         // Special handling for 'tells you' match string
         // Should only play sounds if the tell came from a player
         if( (matchStrings[i].find("tells you") != string::npos) && npcTellFilter ) {   
            
            // Only need to check if the source is a player if color is USERCOLOR_TELL
            if(Color == USERCOLOR_TELL) {
               // Get tell source
               string strName = strLine.substr(0, pos - 1);

               // NPC check
               if( IsNpc(strName) )
                  return 0;

               // Special handling for suspend minion: unsuspend
               // Because the pet is not actually in the spawn list when the tell from it
               // is received, IsNpc does not catch it - so just filter based on the
               // unsuspend message here.
               if( strLine.find("tells you, 'I live again...'") != string::npos )
                  return 0;
            } else
               return 0;
         }   
         // END special 'tells you' handling

          PlaySound(sounds[i].c_str(),NULL,SND_FILENAME | SND_ASYNC);             
      }
   }
   return 0;
} 