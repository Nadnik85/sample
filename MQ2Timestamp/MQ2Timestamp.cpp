#include "../MQ2Plugin.h"
PreSetup("MQ2TimeStamp");

typedef struct _TIMESTAMP {
   CHAR Server[0x20];
   CHAR Character[0x40];
   BOOL Location;
   CHAR Format[MAX_STRING];
   struct _TIMESTAMP *pNext;
} TIMESTAMP, *PTIMESTAMP;

bool      bStamp;
struct tm   *currentTime;
time_t   long_time;
CHAR      strTimeBuffer[MAX_STRING] = {0}, szTemp[MAX_STRING] = {0}, strDefault[MAX_STRING] = {0};
INT         maxLength;
PTIMESTAMP   pTimestamp=0, pCurrentTimestamp=0;

VOID ClearTimestamps()
{
   while(pTimestamp)
   {
      PTIMESTAMP pNext = pTimestamp->pNext;
      delete pTimestamp;
      pTimestamp = pNext;
   }
}

VOID LoadTimestamps(PCHARINFO pCharInfo)
{
   ClearTimestamps();
   BOOL Location;
   CHAR Servers[MAX_STRING] = {0}, Characters[MAX_STRING];
   PCHAR pServer, pCharacter, pComma, temp;
   PTIMESTAMP tmpTimestamp;

   GetPrivateProfileSectionNames(Servers, sizeof(Servers), INIFileName);
    pServer = Servers;
   while (pServer[0] != NULL) {
      // Config section is handled differently
      if (stricmp(pServer, "Config") == 0) {
         GetPrivateProfileString("Config", "Default", "[%H:%M:%S]", strDefault, sizeof(strDefault), INIFileName);
         maxLength = GetPrivateProfileInt("Config", "UseTimestamps", 1, INIFileName);
         bStamp = (maxLength > 0 ? true: false);
         maxLength = GetPrivateProfileInt("Config", "MaxLength", 20, INIFileName);
         pServer += strlen(pServer)+1;
         continue;
      }
      // Got the server, parse characters
      GetPrivateProfileString(pServer, NULL, "UNDEFINED", Characters, sizeof(Characters), INIFileName);
      pCharacter = Characters;
      while (pCharacter[0] != NULL) {
         temp = szTemp;
         GetPrivateProfileString(pServer, pCharacter, strDefault, temp, sizeof(szTemp), INIFileName);
         //Location
         pComma = strchr(temp,',');
         if (!pComma)
            continue;
         *pComma = 0;
         Location = atoi(temp);
         temp = &pComma[1];

         //Format
         if (!temp[0])
            continue;
         tmpTimestamp = new TIMESTAMP;
         tmpTimestamp->pNext = pTimestamp;
         pTimestamp = tmpTimestamp;
         pTimestamp->Location = Location;
         strcpy(pTimestamp->Server, pServer);
         strcpy(pTimestamp->Character, pCharacter);
         strcpy(pTimestamp->Format, temp);

         // Check if this is the info for the current character
         if((stricmp(pServer, EQADDR_SERVERNAME) == 0) && (stricmp(pCharacter, pCharInfo->Name) == 0))
            pCurrentTimestamp = pTimestamp;
         // Next Character
         pCharacter += strlen(pCharacter)+1;
      }
      // Next Server
      pServer += strlen(pServer)+1;
   }
}

VOID SaveConfig() {
   WritePrivateProfileString("Config", "Default", strDefault, INIFileName);
   sprintf(szTemp, "%i", maxLength);
   WritePrivateProfileString("Config", "MaxLength", szTemp, INIFileName);
   sprintf(szTemp, "%i", (int)bStamp);
   WritePrivateProfileString("Config", "UseTimestamps", szTemp, INIFileName);
}

VOID SaveTimestamp(PCHARINFO pCharInfo, PCHAR pFormat, BOOL bLocation) {
   PTIMESTAMP tmpTimestamp;

   SaveConfig();

   // Sanity Checks!
   if(!pFormat || !*pFormat || pFormat==NULL)
      pFormat = strDefault;
   bLocation = (bLocation > 0 ? 1: 0);

   if(pCurrentTimestamp != NULL) {
   // Timestamp structure already exists for this character, update it, and dump to file.
      pCurrentTimestamp->Location = bLocation;
      strcpy(pCurrentTimestamp->Format, pFormat);

      sprintf(szTemp, "%i,%s", (int)pCurrentTimestamp->Location, pCurrentTimestamp->Format);
      WritePrivateProfileString(EQADDR_SERVERNAME, pCharInfo->Name, szTemp, INIFileName);
   } else {
   // No Timestamp structure for this character, create one, and dump to file.
      tmpTimestamp = new TIMESTAMP;
      tmpTimestamp->pNext = pTimestamp;
      pTimestamp = tmpTimestamp;
      pTimestamp->Location = bLocation;
      strcpy(pTimestamp->Server, EQADDR_SERVERNAME);
      strcpy(pTimestamp->Character, pCharInfo->Name);
      strcpy(pTimestamp->Format ,pFormat);

      pCurrentTimestamp=pTimestamp;

      sprintf(szTemp, "%i,%s", (int)pCurrentTimestamp->Location, pCurrentTimestamp->Format);
      WritePrivateProfileString(EQADDR_SERVERNAME, pCharInfo->Name, szTemp, INIFileName);
   }
}

VOID TimeStampCmd(PSPAWNINFO pChar, PCHAR szLine)
{
   // Get first arg and check command
   GetArg(szTemp, szLine, 1);
   if (stricmp(szTemp, "on") == 0) {
      bStamp = true;
      SaveConfig();
      WriteChatColor("Timestamp on.");
   } else if (stricmp(szTemp, "off") == 0) {
      bStamp = false;
      SaveConfig();
      WriteChatColor("Timestamp off.");
   } else if (stricmp(szTemp, "reload") == 0) {
      LoadTimestamps(GetCharInfo());
      WriteChatColor("Reloaded timestamp formats.");
   } else if (stricmp(szTemp, "default") == 0) {
      if(pCurrentTimestamp != NULL)
         SaveTimestamp(GetCharInfo(), strDefault, pCurrentTimestamp->Location);
      else
         SaveTimestamp(GetCharInfo(), strDefault, 1);
      WriteChatColor("Timestamp format reset to default.");
   } else if (stricmp(szTemp, "format") == 0) {
      GetArg(szTemp, szLine, 2);
      if(pCurrentTimestamp != NULL)
         SaveTimestamp(GetCharInfo(), szTemp, pCurrentTimestamp->Location);
      else
         SaveTimestamp(GetCharInfo(), szTemp, 1);
      WriteChatColor("Timestamp format updated.");
   } else if (stricmp(szTemp, "maxlen") == 0) {
      GetArg(szTemp, szLine, 2);
      if(IsNumber(szTemp)) {
         maxLength = atoi(szTemp);
         maxLength = (maxLength < 15 ? 15: maxLength);
         SaveConfig();
         WriteChatColor("Timestamp truncation length updated.");
      } else
         WriteChatColor("Maximum length must be a number.");
   } else if (stricmp(szTemp, "loc") == 0) {
      GetArg(szTemp, szLine, 2);
      if(IsNumber(szTemp)) {
         if(pCurrentTimestamp != NULL)
            SaveTimestamp(GetCharInfo(), pCurrentTimestamp->Format, atoi(szTemp));
         else
            SaveTimestamp(GetCharInfo(), strDefault, atoi(szTemp));
         WriteChatColor("Timestamp location updated.");
      } else
         WriteChatColor("Location must be 0 or 1.");
   } else if (stricmp(szTemp, "help") == 0) {
      WriteChatColor("Usage: /timestamp [ help| on | off | reload | default | format <format> | loc <0|1> | maxlen <length> ]", CONCOLOR_YELLOW);
   } else {
      bStamp = !bStamp;
      SaveConfig();
      sprintf(szTemp, "Timestamp %s", (bStamp ? "on": "off"));
      WriteChatColor(szTemp, CONCOLOR_LIGHTBLUE);
      WriteChatColor("Type /timestamp help for usage.", CONCOLOR_YELLOW);
   }
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
   bStamp = true;
   maxLength = 20;
   strcpy(strDefault, "[%H:%M:%S]");

   AddCommand("/timestamp", TimeStampCmd);

   if(GetCharInfo() != NULL)
      LoadTimestamps(GetCharInfo());
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
   RemoveCommand("/timestamp");
}

// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState)
{
   if (GameState==GAMESTATE_INGAME)
      LoadTimestamps(GetCharInfo());
} //end SetGameState

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
// but after MQ filters and chat events are taken care of.
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
   if (bStamp)
   {
      time(&long_time);
      currentTime = localtime(&long_time);

      if(pCurrentTimestamp != NULL) {
         strftime(strTimeBuffer, maxLength, pCurrentTimestamp->Format, currentTime);
         sprintf(szTemp, "%s %s", (pCurrentTimestamp->Location ? strTimeBuffer: Line), (pCurrentTimestamp->Location ? Line: strTimeBuffer));
         strcpy(strTimeBuffer, szTemp);
      } else {
         strftime(strTimeBuffer, maxLength, strDefault, currentTime);
         strcat(strTimeBuffer, Line);
      }

      dsp_chat_no_events(strTimeBuffer, Color, 1);
   }

   return bStamp;
}