// MQ2Targets.cpp : Defines the entry point for the DLL application. 
// 

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time 
// are shown below. Remove the ones your plugin does not use.  Always use Initialize 
// and Shutdown for setup and cleanup, do NOT do it in DllMain. 

#define MQ2Targets_Version   "MQ2Targets \ar1.13\ax by \agDrunkDwarf\ax" 
#define MQ2TargetsTitle      "Targets" 

#define MakeUpper(yourstring) transform (yourstring.begin(),yourstring.end(), yourstring.begin(), toupper); 

//some defaults 
#define NUMTARGETS         5   // display closest <x> targets to user 
#define HUDXSTART         165 
#define HUDYSTART         5 
#define HUDYINCREMENT      12 
#define HUDCOLOR         0xFFFFEA08   // yellow 
#define GUILDHUDCOLOR      0xFFFF00FF   // purple 
#define DEADHUDCOLOR      0xFF888888   // gray 
#define TARGETHUDCOLOR      0xFFFF0000   // red 
#define TARGETHUDSTRING      "&clr${Target.CleanName} ${Target.Level}${Target.Class.ShortName} ${Target.Distance}&arr(${Target.HeadingTo})" 
#define NOTIFYHUDSTRING      "${Target.CleanName}(${Target.Level}${Target.Class.ShortName})" 

#define POPUPINTERVAL      2         // interval to show up any popup text in seconds 
#define SECONDINTERVAL      1         // 1 second interval 

#include "../MQ2Plugin.h"
using namespace std;
#include <list> 
#include <map> 

PreSetup("MQ2Targets"); 

struct SEARCHSTRINGS 
{ 
   CHAR szSearchString[MAX_STRING]; 
   bool bNotify; 
   int nIndex; 
}; 

struct NOTIFYSTRUCT 
{ 
   PSPAWNINFO pSpawn; 
   bool bNotified; 
}; 

PCHAR szDirection[] = { 
    "-^-",  //0 
    "<--",   //1 
    "-->"   //2 
}; 

list<SEARCHSTRINGS>   g_SearchStrings;      // list of spawns to watch for 
list<NOTIFYSTRUCT>   g_NotifySpawns;       // list of items to notify user of 

CIndex<PMQRANK>      g_Targets;            // keeps track of targets matching search strings (spawninfo/dist) 

unsigned long g_numWatchedTargets   = NUMTARGETS;   // how many targets to track and display.  All targets 
                                       // are tracked, but only the <numWatchedTargets> closest are 
                                       // shown to the user 
unsigned int g_HUDXStart         = HUDXSTART; 
unsigned int g_HUDYStart         = HUDYSTART; 
unsigned int g_HUDYIncrement      = HUDYINCREMENT; 
unsigned int g_HUDColor            = HUDCOLOR; 
unsigned int g_GuildHUDColor      = GUILDHUDCOLOR; 
unsigned int g_DeadHUDColor         = DEADHUDCOLOR; 
unsigned int g_TargetHUDColor      = TARGETHUDCOLOR; 
CHAR g_HUDString[MAX_STRING]      = {0}; 
CHAR g_NotifyHUDString[MAX_STRING]   = {0}; 

bool g_CheckForTargets            = false;   // during some times (zoning, etc) we don't want to be checking 
                                    // for targets (just a waste).  This global lets us toggle this 
bool g_bReadyToSearch            = false; 

time_t g_timerSeconds; 
time_t g_timerOneSecond; 

CRITICAL_SECTION g_csTargetSection; 

// prototypes 

VOID MQ2TargetsStrReplaceSection(PCHAR szOriginal, size_t BufferSize, DWORD ReplacePos,PCHAR szReplaceString); 

// ini file reading/writing 
VOID LoadPluginSettings(); 
VOID SavePluginSettings(); 
VOID LoadHUDString(); 

// ini file targets 
VOID ReadTargetsFromINI(PCHAR zone); 
VOID DumpTargetsToINI(PCHAR zone); 

// memory 
VOID LoadZoneTargets(); 
VOID ClearTargets(); 
BOOL AddToTargetList(PCHAR targetName, bool bNotify); 
BOOL RemoveFromTargetList(PCHAR targetName, bool bNotify); 

// command handlers 
VOID WatchHandler(PSPAWNINFO pChar, PCHAR szLine); 
VOID WatchList(PCHAR zone); 
VOID WatchAdd(PCHAR zone, PCHAR targetName, bool bNotify = false); 
VOID WatchRemove(PCHAR zone, PCHAR targetName, bool bNotify = false); 
VOID WatchTargets(PCHAR pTargets); 
VOID WatchXStart(PCHAR pXStart); 
VOID WatchYStart(PCHAR pYStart); 
VOID WatchYIncrement(PCHAR pYIncrement); 
VOID WatchColor(PCHAR pColor); 
VOID WatchHUD(PCHAR pLine); 
VOID WatchUsage(); 

// spam to window 
//VOID ToMainChat(DWORD Color, PCHAR szFormat, ...); 
VOID ToMainChat(DWORD Color, PCHAR szText); 
//VOID ToMQ2Chat(DWORD Color, PCHAR szFormat, ...); 
VOID ToMQ2Chat(PCHAR szText, DWORD Color=USERCOLOR_DEFAULT); 

// target display 
VOID StartSearchingForTargets(); 
VOID CheckForTargets(bool bState); 
int CheckTargets(); 
int FindMatchingSpawns(PSEARCHSPAWN pSearchSpawn, PSPAWNINFO pOrigin, bool bNotify = false); 
VOID CleanHUDTargets(); 

// sorting 
static int pMQRankFloatCompareNoDups(const void *A, const void *B); 


// popup text on screen for any targets which need notifying 
int AddToNotify(PSPAWNINFO pSpawn); 
VOID RemoveFromNotify(PSPAWNINFO pSpawn, bool bPopup = false); 
VOID RemoveNotifySpawns(PCHAR searchString); 
VOID PopupNotifyTarget(); 

VOID DisplayHUDTarget(PMQRANK targInfo, DWORD X, DWORD Y, DWORD color); 



// Called once, when the plugin is to initialize 
PLUGIN_API VOID InitializePlugin(VOID) 
{ 
   DebugSpewAlways("Initializing MQ2Targets"); 

   InitializeCriticalSection(&g_csTargetSection); 

   CheckForTargets(false); 

   AddCommand("/watch", WatchHandler); 

   LoadPluginSettings(); 

//   LoadZoneTargets(); 

   // Add commands, MQ2Data items, hooks, etc. 
   // AddCommand("/mycommand",MyCommand); 
   // AddXMLFile("MQUI_MyXMLFile.xml"); 
   // bmMyBenchmark=AddMQ2Benchmark("My Benchmark Name"); 
} 

// Called once, when the plugin is to shutdown 
PLUGIN_API VOID ShutdownPlugin(VOID) 
{ 
   DebugSpewAlways("Shutting down MQ2Targets"); 

   SavePluginSettings(); 

   CleanHUDTargets(); 

   EnterCriticalSection(&g_csTargetSection); 
   DeleteCriticalSection(&g_csTargetSection); 

   RemoveCommand("/watch"); 

   // Remove commands, MQ2Data items, hooks, etc. 
   // RemoveMQ2Benchmark(bmMyBenchmark); 
   // RemoveCommand("/mycommand"); 
   // RemoveXMLFile("MQUI_MyXMLFile.xml"); 
} 

PLUGIN_API VOID OnBeginZone(VOID) 
{ 
   DebugSpewAlways("MQ2Targets::OnBeginZone()"); 

   g_bReadyToSearch = false; 

   // get rid of current items on the HUD 
   CleanHUDTargets(); 

   CheckForTargets(false); 
} 

PLUGIN_API VOID OnEndZone(VOID) 
{ 
   DebugSpewAlways("MQ2Targets::OnEndZone()"); 

   g_bReadyToSearch = true; 
//   StartSearchingForTargets(); 
} 

// Called after entering a new zone 
PLUGIN_API VOID OnZoned(VOID) 
{ 
   DebugSpewAlways("MQ2Targets::OnZoned()"); 
} 

// Called once directly before shutdown of the new ui system, and also 
// every time the game calls CDisplay::CleanGameUI() 
PLUGIN_API VOID OnCleanUI(VOID) 
{ 
   DebugSpewAlways("MQ2Targets::OnCleanUI()"); 
   // destroy custom windows, etc 
} 

// Called once directly after the game ui is reloaded, after issuing /loadskin 
PLUGIN_API VOID OnReloadUI(VOID) 
{ 
   DebugSpewAlways("MQ2Targets::OnReloadUI()"); 
   // recreate custom windows, etc 
} 

// Called every frame that the "HUD" is drawn -- e.g. net status / packet loss bar 
PLUGIN_API VOID OnDrawHUD(VOID) 
{ 
   int i = 0; 

   PSPAWNINFO me = GetCharInfo()->pSpawn; 

   // do this at least once a second, even without spawns adding or removing 
   if(time(NULL) > (g_timerOneSecond + SECONDINTERVAL)) 
   { 
      CleanHUDTargets(); 
      CheckTargets(); 
      g_timerOneSecond = time(NULL); 
   } 

   unsigned long nFound = g_Targets.Count(); 

   // assign distances to each target 
   for(unsigned long N = 0; N < nFound; N++) 
   { 
      PSPAWNINFO theTarget = (PSPAWNINFO)g_Targets[N]->VarPtr.Ptr; 
      if(NULL != theTarget) 
      { 
         g_Targets[N]->Value.Float = GetDistance(me, theTarget); 
      } 
   } 
   // always our list by distance 
   qsort(&g_Targets.List[0], nFound, sizeof(PMQRANK), pMQRankFloatCompareNoDups); 
   // now draw the targets until the desired number is found 

   if(nFound > 0) 
   { 
      CHAR szHUD[MAX_STRING] = {0}; 

      for(unsigned long N = 0; N < min(nFound, g_numWatchedTargets); N++) 
      { 
         PSPAWNINFO theTarget = (PSPAWNINFO)g_Targets[N]->VarPtr.Ptr; 
         if(NULL != theTarget) 
         { 
            DWORD SX=0; 
            DWORD SY=0; 
            if(pScreenX && pScreenY) 
            { 
               SX = ScreenX; 
               SY = ScreenY; 
            }    
            // where on the screen to show the targets 
            DWORD X,Y; 
            X = SX + g_HUDXStart; 
            Y = SX + (g_HUDYStart + (g_HUDYIncrement * i++)); 

            unsigned int color = g_HUDColor; 
            DisplayHUDTarget(g_Targets[N], X, Y, color); 
         } 
      } 
   } 
   // if at least n seconds have elapsed since the last timer, do the next pending popup 
   if(time(NULL) > (g_timerSeconds + POPUPINTERVAL)) 
   { 
      // reset "timer" 
      DebugSpewAlways("MQ2Targets::OnDrawHUD() - Popping up any Notify Targets"); 
      g_timerSeconds = time(NULL); 
      PopupNotifyTarget(); 
   } 

   // DONT leave in this debugspew, even if you leave in all the others 
//   DebugSpewAlways("MQ2Targets::OnDrawHUD()"); 
} 

VOID DisplayHUDTarget(PMQRANK targInfo, DWORD X, DWORD Y, DWORD color) 
{ 
   PSPAWNINFO theTarget = (PSPAWNINFO)targInfo->VarPtr.Ptr; 

   if(NULL != theTarget) 
   { 
      CHAR outText[MAX_STRING] = {0}; 
      strcpy_s(outText, g_HUDString); 

      PSPAWNINFO me = GetCharInfo()->pSpawn; 

      // figure out heading 
      float HeadingTo = (float)(atan2f(me->Y - theTarget->Y, theTarget->X - me->X) * 180.0f / PI + 90.0f); 
      if(HeadingTo < 0.0f) 
         HeadingTo += 360.0f; 
      else if(HeadingTo >= 360.0f) 
         HeadingTo -= 360.0f; 

      PCHAR pSpot; 
      // replace &arr with direction arrow 
      while(pSpot = strstr(outText, "&arr")) 
      { 
         float MyHeading = me->Heading * 0.703125f; 

         int arrow; 
         if(((((int)MyHeading - (int)HeadingTo) + 375) % 360) < 30) 
         { 
            arrow = 0;   // straight ahead (+-30degrees) 
         } 
         else if(((((int)MyHeading - (int)HeadingTo) + 360) % 360) < 180) 
         { 
            arrow = 2;   // to the left 
         } 
         else 
            arrow = 1; 
		 int pSpotSize = (int)(pSpot - outText);
         MQ2TargetsStrReplaceSection(pSpot, pSpotSize, 4, szDirection[arrow]); 
      } 
      // replace &dst with distance 
      while(pSpot = strstr(outText, "&dst")) 
      { 
         CHAR szDist[MAX_STRING] = {0}; 
         if(me->Z - theTarget->Z > 10) 
         { 
            sprintf_s(szDist, "-%.1f", targInfo->Value.Float); 
         } 
         else if(theTarget->Z - me->Z > 10) 
         { 
            sprintf_s(szDist, "+%.1f", targInfo->Value.Float); 
         } 
         else 
            sprintf_s(szDist, "%.1f", targInfo->Value.Float); 
		 int pSpotSize = (int)(pSpot - outText);
         MQ2TargetsStrReplaceSection(pSpot, pSpotSize, 4, szDist); 
      } 
      // replace &clr with target indicators if targeted, also use target colors if &high is found 
      bool bTarget = false; 
      bool bHighLight = false; 
      while(pSpot = strstr(outText, "&clr")) 
      { 
         // color of the HUD display 
         // Yellow 
//         ARGBCOLOR Color; 
//         Color.A = 0xFF; 
//         Color.R = 255; 
//         Color.G = 234; 
//         Color.B = 8; 

         // if we want to highlight/color stuff 
         bHighLight = true; 
         // highlight targeted spawn 
         CHAR szBrackets[MAX_STRING] = {0}; 
         if(ppTarget && pTarget && ((PSPAWNINFO)pTarget)->SpawnID == theTarget->SpawnID) 
         { 
            strcpy_s(szBrackets, ">>"); 
            bTarget = true; 
         } 
		 int pSpotSize = (int)(pSpot - outText);
         MQ2TargetsStrReplaceSection(pSpot, pSpotSize, 4, szBrackets); 
      } 
//      bool bTarget = false; 
//      bool bHighLight = false; 

      if(true == bHighLight) 
      { 
         if(theTarget->GuildID == me->GuildID) 
            color = g_GuildHUDColor; 
         if(true == bTarget) 
         { 
            color = g_TargetHUDColor;   // red 
            X -= 12; 
         } 
         if(theTarget->StandState == STANDSTATE_DEAD) 
            color = g_DeadHUDColor; 
      } 

      // replace "${Target." with "${Spawn[<spawnid>]." 
      CHAR szSpawnID[20] = {0}; 
      sprintf_s(szSpawnID, "${Spawn[%d].", theTarget->SpawnID); 
//      PCHAR pSpot; 
      while(pSpot = strstr(outText, "${Target.")) 
      { 
		  int pSpotSize = (int)(pSpot - outText);
         MQ2TargetsStrReplaceSection(pSpot, pSpotSize, 9, szSpawnID); 
      } 
      // now parse like MQ2HUD ini string 
        ParseMacroParameter(me, outText); 
        if(outText[0] && strcmp(outText, "NULL")) 
        { 
         DrawHUDText(outText, X, Y, color, 2); 
        } 
//      DrawHUDText(outText, X, Y, color); 
   } 
} 

VOID MQ2TargetsStrReplaceSection(PCHAR szOriginal, size_t BufferSize, DWORD ReplacePos,PCHAR szReplaceString) 
{
	if (BufferSize == 0)
		Sleep(0);
	/*NewLength = strlen(szCurrent);
	memmove(&pBrace[NewLength], &pEnd[1], strlen(&pEnd[1]) + 1);
	int addrlen = (int)(pBrace - szOriginal);
	memcpy_s(pBrace, BufferSize-addrlen,szCurrent, NewLength);
	int OrgPos = (int)szOriginal;
	*/
   DWORD NewLength=(DWORD)strlen(szReplaceString); 
   memmove(&szOriginal[NewLength],&szOriginal[ReplacePos],strlen(&szOriginal[ReplacePos])+1);
   //int addrlen = (int)(OrgPos - szOriginal);
   //memcpy_s(pBrace, BufferSize-addrlen,szCurrent, NewLength);
   memcpy_s(szOriginal,BufferSize,szReplaceString,NewLength); 
} 

// Called once directly after initialization, and then every time the gamestate changes 
PLUGIN_API VOID SetGameState(DWORD GameState) 
{ 
   DebugSpewAlways("MQ2Targets::SetGameState (%d)", GameState); 

   if(GameState == GAMESTATE_INGAME && true == g_bReadyToSearch) 
   { 
      StartSearchingForTargets(); 
   } 
} 

// This is called every time MQ pulses 
PLUGIN_API VOID OnPulse(VOID) 
{ 
   // DONT leave in this debugspew, even if you leave in all the others 
//   DebugSpewAlways("MQ2Targets::OnPulse()"); 
} 

// This is called every time WriteChatColor is called by MQ2Main or any plugin, 
// IGNORING FILTERS, IF YOU NEED THEM MAKE SURE TO IMPLEMENT THEM. IF YOU DONT 
// CALL CEverQuest::dsp_chat MAKE SURE TO IMPLEMENT EVENTS HERE (for chat plugins) 
PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter) 
{ 
//   DebugSpewAlways("MQ2Targets::OnWriteChatColor(%s)",Line); 

   if(_stricmp(Line, "plugin 'MQ2targets' Loaded.") == 0) 
   { 
      DebugSpewAlways("MQ2Targets::OnWriteChatColor(%s)", Line); 
      g_bReadyToSearch = true; 
      // use that function to simulate starting the game 
      SetGameState(GAMESTATE_INGAME); 
   } 
   return 0; 
} 

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat, 
// but after MQ filters and chat events are taken care of. 
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color) 
{ 
//   DebugSpewAlways("MQ2Targets::OnIncomingChat(%s)",Line); 
   return 0; 
} 

// This is called each time a spawn is added to a zone (inserted into EQ's list of spawns), 
// or for each existing spawn when a plugin first initializes 
// NOTE: When you zone, these will come BEFORE OnZoned 
PLUGIN_API VOID OnAddSpawn(PSPAWNINFO pNewSpawn) 
{ 
   if(gGameState == GAMESTATE_INGAME && true == g_bReadyToSearch) 
   { 
      DebugSpewAlways("MQ2Targets::OnAddSpawn(%s)", pNewSpawn->DisplayedName); 

      CleanHUDTargets(); 

      CheckTargets(); 
   } 
} 

// This is called each time a spawn is removed from a zone (removed from EQ's list of spawns). 
// It is NOT called for each existing spawn when a plugin shuts down. 
PLUGIN_API VOID OnRemoveSpawn(PSPAWNINFO pSpawn) 
{ 
   if(gGameState == GAMESTATE_INGAME && true == g_bReadyToSearch) 
   { 
      DebugSpewAlways("MQ2Targets::OnRemoveSpawn(%s)", pSpawn->DisplayedName); 

      CleanHUDTargets(); 

      CheckTargets(); 

      // if it's in the notify list, update its text 
      RemoveFromNotify(pSpawn, true); 
   } 
} 

// This is called each time a ground item is added to a zone 
// or for each existing ground item when a plugin first initializes 
// NOTE: When you zone, these will come BEFORE OnZoned 
PLUGIN_API VOID OnAddGroundItem(PGROUNDITEM pNewGroundItem) 
{ 
//   DebugSpewAlways("MQ2Targets::OnAddGroundItem(%d)",pNewGroundItem->DropID); 
} 

// This is called each time a ground item is removed from a zone 
// It is NOT called for each existing ground item when a plugin shuts down. 
PLUGIN_API VOID OnRemoveGroundItem(PGROUNDITEM pGroundItem) 
{ 
//   DebugSpewAlways("MQ2Targets::OnRemoveGroundItem(%d)",pGroundItem->DropID); 
} 

// overall settings for Plugin 
VOID LoadPluginSettings() 
{ 
   DebugSpewAlways("MQ2Targets::LoadPluginSettings()"); 

   g_numWatchedTargets = GetPrivateProfileInt("Settings", "NumDisplayed", NUMTARGETS, INIFileName); 
   g_HUDXStart = GetPrivateProfileInt("Settings", "HUDXStart", HUDXSTART, INIFileName); 
   g_HUDYStart = GetPrivateProfileInt("Settings", "HUDYStart", HUDYSTART, INIFileName); 
   g_HUDYIncrement = GetPrivateProfileInt("Settings", "HUDYIncrement", HUDYINCREMENT, INIFileName); 
   g_HUDColor = GetPrivateProfileInt("Settings", "HUDColor", HUDCOLOR, INIFileName); 
   g_GuildHUDColor = GetPrivateProfileInt("Settings", "GuildHUDColor", GUILDHUDCOLOR, INIFileName); 
   g_DeadHUDColor = GetPrivateProfileInt("Settings", "DeadHUDColor", DEADHUDCOLOR, INIFileName); 
   g_TargetHUDColor = GetPrivateProfileInt("Settings", "TargetHUDColor", TARGETHUDCOLOR, INIFileName); 

   LoadHUDString(); 
    
//   if(!strcmp(spawnCommand, "notfound")) return; 
//      DoCommand((PSPAWNINFO)pCharSpawn, szTemp); 
//[Settings] 
//OnSpawnCommand="/multiline ; /beep ; /popup SPAWN: ${SpawnMaster.LastMatch}!" 
//Enabled=on 
} 

VOID SavePluginSettings() 
{ 
   DebugSpewAlways("MQ2Targets::SavePluginSettings()"); 

   CHAR szTemp[MAX_STRING] = {0}; 

   // save Number of Targets to display 
   sprintf_s(szTemp, "%d", g_numWatchedTargets); 
   WritePrivateProfileString("Settings", "NumDisplayed", szTemp, INIFileName); 

   sprintf_s(szTemp, "%d", g_HUDXStart); 
   WritePrivateProfileString("Settings", "HUDXStart", szTemp, INIFileName); 

   sprintf_s(szTemp, "%d", g_HUDYStart); 
   WritePrivateProfileString("Settings", "HUDYStart", szTemp, INIFileName); 

   sprintf_s(szTemp, "%d", g_HUDYIncrement); 
   WritePrivateProfileString("Settings", "HUDYIncrement", szTemp, INIFileName); 

   sprintf_s(szTemp, "%d", g_HUDColor); 
   WritePrivateProfileString("Settings", "HUDColor", szTemp, INIFileName); 

   sprintf_s(szTemp, "%d", g_GuildHUDColor); 
   WritePrivateProfileString("Settings", "GuildHUDColor", szTemp, INIFileName); 

   sprintf_s(szTemp, "%d", g_DeadHUDColor); 
   WritePrivateProfileString("Settings", "DeadHUDColor", szTemp, INIFileName); 

   sprintf_s(szTemp, "%d", g_TargetHUDColor); 
   WritePrivateProfileString("Settings", "TargetHUDColor", szTemp, INIFileName); 

   sprintf_s(szTemp, "%s", g_HUDString); 
   WritePrivateProfileString("Settings", "HUDString", szTemp, INIFileName); 

   sprintf_s(szTemp, "%s", g_NotifyHUDString); 
   WritePrivateProfileString("Settings", "NotifyHUDString", szTemp, INIFileName); 

//   if(!strcmp(spawnCommand, "notfound")) return; 
//      DoCommand((PSPAWNINFO)pCharSpawn, szTemp); 
//[Settings] 
//OnSpawnCommand="/multiline ; /beep ; /popup SPAWN: ${SpawnMaster.LastMatch}!" 
//Enabled=on 
} 

VOID LoadHUDString() 
{ 
   GetPrivateProfileString("Settings", "HUDString", TARGETHUDSTRING, g_HUDString, MAX_STRING, INIFileName); 

   GetPrivateProfileString("Settings", "NotifyHUDString", NOTIFYHUDSTRING, g_NotifyHUDString, MAX_STRING, INIFileName); 
} 




















// Target-related methods 
VOID LoadZoneTargets() 
{ 
   DebugSpewAlways("MQ2Targets::LoadZoneTargets()"); 

   CHAR zone[MAX_STRING] = ">unknown<"; 

   try 
   { 
      // clean out the old targets to watch for 
      ClearTargets(); 

      PWORLDDATA psWorldData = ((PWORLDDATA)pWorldData); 
      if(NULL != psWorldData && NULL != GetCharInfo()) 
      { 
         // read new targets to watch for in ALL zones 
#ifdef ALL_ZONE 
         ReadTargetsFromINI("all"); 
#endif   //#ifdef ALL_ZONE 

         strcpy_s(zone, psWorldData->ZoneArray[GetCharInfo()->zoneId]->LongName); 
         // read new targets to watch for 
         ReadTargetsFromINI(zone); 
         // show the current watch list 
         WatchList(zone); 
      } 
   } 
   catch(...) 
   { 
      DebugSpewAlways("MQ2Targets::LoadZoneTargets() **Exception**"); 
   } 
} 

// dealing with targets in the INI file 
VOID ReadTargetsFromINI(PCHAR zone) 
{ 
   DebugSpewAlways("MQ2Targets::ReadTargetsFromINI()"); 

   CHAR szTemp[MAX_STRING]; 
   CHAR szBuffer[MAX_STRING]; 
   bool bNotify = false; 

   int i = 0; 
   do 
   { 
      sprintf_s(szTemp, "spawn%d", i); 
      GetPrivateProfileString(zone, szTemp, "notfound", szBuffer, MAX_STRING, INIFileName); 
      if(!strcmp(szBuffer, "notfound")) 
         break; 

      try 
      { 
         char seperator[]   = "|"; 
         char *token; 
		 char *Next_Token;
         // Establish string and get the first token: 
         token = strtok_s(szBuffer, seperator, &Next_Token); 
         while(token!= NULL) 
         { 
            // While there are tokens in "string" 
            if(0 == _stricmp(token, "notify")) 
               bNotify = true; 
            // Get next token: 
            token = strtok_s(NULL, seperator, &Next_Token); 
         } 
      } 
      catch(...) 
      { 
         DebugSpewAlways("MQ2Targets::ReadTargetsFromINI() **strtok Exception**"); 
      } 

      AddToTargetList(szBuffer, bNotify); 
   } while(++i); 
} 

VOID DumpTargetsToINI(PCHAR zone) 
{ 
   DebugSpewAlways("MQ2Targets::DumpTargetsToINI()"); 

   CHAR szTemp[MAX_STRING] = {0}; 

   // clean INI entries for entire zone 
   WritePrivateProfileString(zone, NULL, NULL, INIFileName); 

   // now rewrite INI entries for entire zone 
   int i = 0; 
   char saveString[MAX_STRING]; 
   list<SEARCHSTRINGS>::iterator pSearchStrings = g_SearchStrings.begin(); 
   while(pSearchStrings != g_SearchStrings.end()) 
   { 
      sprintf_s(szTemp, "spawn%d", i++); 
      strcpy_s(saveString, pSearchStrings->szSearchString); 
      if(true == pSearchStrings->bNotify) 
      { 
         strcat_s(saveString, "|notify"); 
      } 
      WritePrivateProfileString(zone, szTemp, saveString, INIFileName); 
      pSearchStrings++; 
   } 
} 

// dealing with Targets in memory 
VOID ClearTargets() 
{ 
   DebugSpewAlways("MQ2Targets::ClearTargets()"); 

   g_SearchStrings.clear(); 
   g_NotifySpawns.clear(); 
} 

BOOL AddToTargetList(PCHAR targetName, bool bNotify) 
{ 
   DebugSpewAlways("MQ2Targets::AddToTargetList()"); 

   BOOL bReturn = FALSE; 

   CHAR szMsg[MAX_STRING] = {0}; 
   list<SEARCHSTRINGS>::iterator pSearchStrings = g_SearchStrings.begin(); 
   while(pSearchStrings != g_SearchStrings.end()) 
   { 
      if(!strcmp(targetName, pSearchStrings->szSearchString)) 
      { 
         sprintf_s(szMsg, "Already watching for \"%s\"", targetName); 
         ToMQ2Chat(szMsg, CONCOLOR_RED); 
         return bReturn; 
      } 
      pSearchStrings++; 
   } 
    
   SEARCHSTRINGS NewString; 
   strcpy_s(NewString.szSearchString, targetName); 
   NewString.bNotify = bNotify; 
   g_SearchStrings.push_back(NewString); 
   sprintf_s(szMsg,"Watching for \"%s\"", targetName); 
   ToMQ2Chat(szMsg, CONCOLOR_YELLOW); 

   bReturn = TRUE; 

   return bReturn; 
} 

BOOL RemoveFromTargetList(PCHAR SpawnName, bool bNotify) 
{ 
   DebugSpewAlways("MQ2Targets::RemoveFromTargetList()"); 

   CHAR szMsg[MAX_STRING] = {0}; 
   list<SEARCHSTRINGS>::iterator pSearchStrings = g_SearchStrings.begin(); 
   while(pSearchStrings != g_SearchStrings.end()) 
   { 
      if(!strcmp(SpawnName, pSearchStrings->szSearchString)) 
      { 
         sprintf_s(szMsg, "No longer watching for \"%s\"", SpawnName); 
         ToMQ2Chat(szMsg, USERCOLOR_CHAT_CHANNEL); 
         g_SearchStrings.erase(pSearchStrings); 
         return TRUE; 
      } 
      pSearchStrings++; 
   } 
   sprintf_s(szMsg, "Cannot delete \"%s\", not found on watch list", SpawnName); 
   ToMQ2Chat(szMsg, CONCOLOR_RED); 

   return FALSE; 
} 
















// Command Handlers 
VOID WatchHandler(PSPAWNINFO pChar, PCHAR szLine) 
{ 
   if(0 == *szLine) 
   { 
      WatchUsage(); 
      return; 
   } 

   CHAR szZone[MAX_STRING] = {0}; 
   CHAR szArg1[MAX_STRING] = {0}; 
   CHAR szArg2[MAX_STRING] = {0}; 

   GetArg(szArg1, szLine, 1); 

   if(strlen(szArg1) == 0) 
   { 
      WatchUsage(); 
   } 
   else 
   { 
      CHAR szArg2[MAX_STRING] = {0}; 
      strcpy_s(szArg2, GetNextArg(szLine)); 

      PWORLDDATA psWorldData = ((PWORLDDATA)pWorldData); 
      if(NULL != psWorldData && NULL != GetCharInfo()) 
      { 
         strcpy_s(szZone, psWorldData->ZoneArray[GetCharInfo()->zoneId]->LongName); 
      } 

      // everything lowercase 
      string lcArg1 = szArg1; 
      MakeLower(lcArg1); 

      string lcArg2 = szArg2; 

      // make Arg2 the target if nothing in second parameter 
      if(lcArg2.length() == 0) 
      { 
         PSPAWNINFO psTarget = NULL; 
         if(ppTarget && pTarget) 
         { 
            psTarget = (PSPAWNINFO)pTarget; 
            lcArg2 = psTarget->DisplayedName; 
         } 
      } 
      MakeLower(lcArg2); 

      if(lcArg1 == "list" || lcArg1 == "l") 
      { 
         // show the current watch list 
         WatchList(szZone); 
      } 
      else if(lcArg1 == "add" && lcArg2.length() > 0) 
      { 
         WatchAdd(szZone, (PCHAR)lcArg2.c_str()); 
      } 
#ifdef ALL_ZONE 
      else if(lcArg1 == "addall" && lcArg2.length() > 0) 
      { 
         WatchAdd("all", (PCHAR)lcArg2.c_str()); 
      } 
#endif   //#ifdef ALL_ZONE 
      else if(lcArg1 == "notify" && lcArg2.length() > 0) 
      { 
         WatchAdd(szZone, (PCHAR)lcArg2.c_str(), true); 
      } 
#ifdef ALL_ZONE 
      else if(lcArg1 == "notifyall" && lcArg2.length() > 0) 

      { 
         WatchAdd("all", (PCHAR)lcArg2.c_str(), true); 
      } 
#endif   //#ifdef ALL_ZONE 
      else if((lcArg1 == "delete" || lcArg1 == "del" || lcArg1 == "remove" || lcArg1 == "rem") && lcArg2.length() > 0) 
      { 
         WatchRemove(szZone, (PCHAR)lcArg2.c_str()); 
      } 
#ifdef ALL_ZONE 
      else if((lcArg1 == "deleteall" || lcArg1 == "delall" || lcArg1 == "removeall" || lcArg1 == "remall") && lcArg2.length() > 0) 
      { 
         WatchRemove("all", (PCHAR)lcArg2.c_str()); 
      } 
#endif   //#ifdef ALL_ZONE 
      else if(lcArg1 == "show" && lcArg2.length() > 0 && IsNumber((PCHAR)lcArg2.c_str())) 
      { 
         WatchTargets((PCHAR)lcArg2.c_str()); 
      } 
      else if(lcArg1 == "x" && lcArg2.length() > 0 && IsNumber((PCHAR)lcArg2.c_str())) 
      { 
         WatchXStart((PCHAR)lcArg2.c_str()); 
      } 
      else if(lcArg1 == "y" && lcArg2.length() > 0 && IsNumber((PCHAR)lcArg2.c_str())) 
      { 
         WatchYStart((PCHAR)lcArg2.c_str()); 
      } 
      else if(lcArg1 == "increment" && lcArg2.length() > 0 && IsNumber((PCHAR)lcArg2.c_str())) 
      { 
         WatchYIncrement((PCHAR)lcArg2.c_str()); 
      } 
      else if(lcArg1 == "color" && NULL != GetNextArg(szLine, 3)) 
      { 
         WatchColor(szLine); 
      } 
      else if(lcArg1 == "hud" && lcArg2.length() > 0) 
      { 
         WatchHUD((PCHAR)lcArg2.c_str()); 
      } 
      else 
      { 
         WatchUsage(); 
      } 
   } 
} 

VOID WatchList(PCHAR zone) 
{ 
   CHAR szMsg[MAX_STRING] = {0}; 

   if(g_SearchStrings.size() == 0) 
   { 
      sprintf_s(szMsg, "Not watching for targets in %s", zone); 
      ToMainChat(CONCOLOR_YELLOW, szMsg); 
   } 
   else 
   { 
      sprintf_s(szMsg, "Loaded %d Target searches for %s", g_SearchStrings.size(), zone); 
      ToMainChat(CONCOLOR_YELLOW, szMsg); 

      int nCount = 0; 
      list<SEARCHSTRINGS>::iterator pSearchStrings = g_SearchStrings.begin(); 
      while(pSearchStrings != g_SearchStrings.end()) 
      { 
         pSearchStrings->nIndex = ++nCount; 
         sprintf_s(szMsg, "%d. \"%s\"%s", pSearchStrings->nIndex, pSearchStrings->szSearchString, pSearchStrings->bNotify?" (Notify)":""); 
         ToMainChat(CONCOLOR_YELLOW, szMsg); 
         pSearchStrings++; 
      } 
   } 
} 

VOID WatchAdd(PCHAR zone, PCHAR targetName, bool bNotify) 
{ 
   if(0 == zone || 0 == targetName) 
   { 
      WatchUsage(); 
      return; 
   } 

   if(TRUE == AddToTargetList(targetName, bNotify)) 
   { 
      // want to do this because we don't want a "hole" in our ini target list 
      DumpTargetsToINI(zone); 
   } 
   // show the current watch list 
   WatchList(zone); 

   CleanHUDTargets(); 

   CheckTargets(); 
} 

VOID WatchRemove(PCHAR zone, PCHAR searchString, bool bNotify) 
{ 
   if(0 == zone || 0 == searchString) 
   { 
      WatchUsage(); 
      return; 
   } 

   CHAR targetName[MAX_STRING] = {0}; 
   strcpy_s(targetName, searchString); 
   if(IsNumber(searchString)) 
   { 
      // now look for each searchstring and build a "closest n" list 
      list<SEARCHSTRINGS>::iterator pSearchStrings = g_SearchStrings.begin(); 

      while(pSearchStrings != g_SearchStrings.end()) 
      { 
         if(pSearchStrings->nIndex == atoi(searchString)) 
         { 
            strcpy_s(targetName, pSearchStrings->szSearchString); 
            break; 
         } 
         pSearchStrings++; 
      } 
   } 

   if(TRUE == RemoveFromTargetList(targetName, bNotify)) 
   { 
      // want to do this because we don't want a "hole" in our ini target list 
      DumpTargetsToINI(zone); 
      // now try to remove any spawns in the notify list that match the targetname 
      RemoveNotifySpawns(targetName); 
   } 
   // show the current watch list 
   WatchList(zone); 

   CleanHUDTargets(); 

   CheckTargets(); 
} 

VOID WatchTargets(PCHAR pNumTargets) 
{ 
   if(NULL != pNumTargets && IsNumber(pNumTargets)) 
   { 
      CHAR szMsg[MAX_STRING] = {0}; 

      g_numWatchedTargets = atoi(pNumTargets); 

      sprintf_s(szMsg, "Listing closest %d targets", g_numWatchedTargets); 
      ToMQ2Chat(szMsg, USERCOLOR_CHAT_CHANNEL); 

      SavePluginSettings(); 
   } 
   else 
      WatchUsage(); 
} 

VOID WatchXStart(PCHAR pXStart) 
{ 
   if(NULL != pXStart && IsNumber(pXStart)) 
   { 
      CHAR szMsg[MAX_STRING] = {0}; 
    
      g_HUDXStart = atoi(pXStart); 

      sprintf_s(szMsg, "X Position set to %d", g_HUDXStart); 
      ToMQ2Chat(szMsg, USERCOLOR_CHAT_CHANNEL); 

      SavePluginSettings(); 
   } 
   else 
      WatchUsage(); 
} 

VOID WatchYStart(PCHAR pYStart) 
{ 
   if(NULL != pYStart && IsNumber(pYStart)) 
   { 
      CHAR szMsg[MAX_STRING] = {0}; 
    
      g_HUDYStart = atoi(pYStart); 

      sprintf_s(szMsg, "Y Position set to %d", g_HUDYStart); 
      ToMQ2Chat(szMsg, USERCOLOR_CHAT_CHANNEL); 

      SavePluginSettings(); 
   } 
   else 
      WatchUsage(); 
} 

VOID WatchYIncrement(PCHAR pYIncrement) 
{ 
   if(NULL != pYIncrement && IsNumber(pYIncrement)) 
   { 
      CHAR szMsg[MAX_STRING] = {0}; 
    
      g_HUDYIncrement = atoi(pYIncrement); 

      sprintf_s(szMsg, "Increment set to %d pixels", g_HUDYIncrement); 
      ToMQ2Chat(szMsg, USERCOLOR_CHAT_CHANNEL); 

      SavePluginSettings(); 
   } 
   else 
      WatchUsage(); 
} 

VOID WatchColor(PCHAR pLine) 
{ 
   if(NULL != pLine) 
   { 
      ToMQ2Chat("Color setting is not implemented at this time", CONCOLOR_RED); 
      ToMQ2Chat(pLine); 
/* 
//      IsNumber(pColor) 
      CHAR szMsg[MAX_STRING] = {0}; 
    
      g_HUDColor = atoi(pColor); 

      sprintf_s(szMsg, "Color set to %d", g_HUDColor); 
      WriteChatColor(szMsg, USERCOLOR_CHAT_CHANNEL); 

      SavePluginSettings(); 
*/ 
   } 
   else 
      WatchUsage(); 
} 

VOID WatchHUD(PCHAR pLine) 
{ 
   if(NULL != pLine) 
   { 
       if(0 == _stricmp("reload", pLine) || 0 == _stricmp("refresh", pLine)) 
      { 
         LoadHUDString(); 
          
         CHAR szMsg[MAX_STRING] = {0}; 
         sprintf_s(szMsg, "HUD String set to %s", g_HUDString); 
         ToMQ2Chat(szMsg, USERCOLOR_CHAT_CHANNEL); 
         sprintf_s(szMsg, "Notify HUD String set to %s", g_NotifyHUDString); 
         ToMQ2Chat(szMsg, USERCOLOR_CHAT_CHANNEL); 
      } 
      else if(0 == _stricmp("show", pLine)) 
      { 
         CHAR szMsg[MAX_STRING] = {0}; 
         sprintf_s(szMsg, "HUDString=%s", g_HUDString); 
         ToMainChat(CONCOLOR_YELLOW, szMsg); 
         sprintf_s(szMsg, "NotifyHUDString=%s", g_NotifyHUDString); 
         ToMainChat(CONCOLOR_YELLOW, szMsg); 
      } 
      else 
      { 
         PCHAR pSpot; 
         // remove all the % chars so it doesn't crash below 
         while(pSpot = strstr(pLine, "%")) 
         { 
			 int pSpotSize = (int)(pSpot - pLine);
            MQ2TargetsStrReplaceSection(pSpot, pSpotSize, 1, NULL); 
         } 
         strcpy_s(g_HUDString, pLine); 

         CHAR szMsg[MAX_STRING] = {0}; 
         sprintf_s(szMsg, "HUD String set to %s", g_HUDString); 
         ToMQ2Chat(szMsg, USERCOLOR_CHAT_CHANNEL); 

         SavePluginSettings(); 
      } 
   } 
   else 
      WatchUsage(); 
} 

VOID WatchUsage() 
{ 
   WriteChatf("--------------------------------------------------------------------------------------------------------"); 
   WriteChatf(MQ2Targets_Version); 
   WriteChatf("\ay/watch [[add|[remove|delete]|notify][all] target] [list] [show n] [x|y n] [increment n]\ax"); 
   WriteChatf("\arNote\ax \"target\" same syntax as /where command\ax"); 
#ifdef ALL_ZONE 
   WriteChatf("\ax \"all\" after [add|delete|remove|notify] tracks in all zones\ax"); 
#endif   //#ifdef ALL_ZONE 
   WriteChatf("\ay/watch [hud [HUDString|&clr|&dst|&arr]]\ax"); 
   WriteChatf("\arExample\ax \ay/watch hud ${Target.CleanName} ${Target.Level}${Target.Class.ShortName} ${Target.Distance}&arr\ax"); 
   WriteChatf("\axdisplays \ayCleric01 75CLR 30.23-->\ax"); 
   WriteChatf("\arHint\ax Use MQ2HUD syntax strings in MQ2Targets.ini, Key HUDString\ax"); 
   WriteChatf("\ax(MQ2Targets replaces \ayTarget.\ax with \aySpawn[<spawnid>].\ax at runtime)"); 
   WriteChatf("--------------------------------------------------------------------------------------------------------"); 
} 

//VOID ToMainChat(DWORD Color, PCHAR szFormat, ...) 
VOID ToMainChat(DWORD Color, PCHAR szText) 
{ 
   CHAR szOutput[MAX_STRING] = {0}; 
/* 
    va_list vaList; 
    va_start(vaList, szFormat); 
    vsprintf_s(szOutput, szFormat, vaList); 
*/ 
   sprintf_s(szOutput, "%s: %s", MQ2TargetsTitle, szText); 
   dsp_chat_no_events (szOutput, Color, false); 
} 

//VOID ToMQ2Chat(DWORD Color, PCHAR szFormat, ...) 
VOID ToMQ2Chat(PCHAR szText, DWORD Color) 
{ 
   CHAR szOutput[MAX_STRING] = {0}; 
/* 
   va_list vaList; 
    va_start(vaList, szFormat); 
    vsprintf_s(szOutput,szFormat, vaList); 
*/ 
   sprintf_s(szOutput, "%s: %s", MQ2TargetsTitle, szText); 
   WriteChatColor(szOutput, Color); 
} 













// starts searching for targets in current zone 
VOID StartSearchingForTargets() 
{ 
   CheckForTargets(true); 

   LoadZoneTargets(); 

   CheckTargets(); 

   // reset "timer" 
   g_timerSeconds = time(NULL); 
   // reset "timer" 
   g_timerOneSecond = time(NULL); 
} 

// target display 
VOID CheckForTargets(bool bState) 
{ 
   g_CheckForTargets = bState; 

   DebugSpewAlways("MQ2Targets::CheckForTargets(%d)", bState); 
} 

// returns number of new targets found 
int CheckTargets() 
{ 
   int nReturn = 0; 

   if(true == g_CheckForTargets) 
   { 
      // don't bother if nothing to search for in this zone 
      if(g_SearchStrings.size() > 0) 
      { 
         CHAR szMsg[MAX_STRING] = {0}; 
         CHAR szArg[MAX_STRING] = {0}; 
         PCHAR szRest = NULL; 

         PSPAWNINFO pOrigin = GetCharInfo()->pSpawn; 

         // now look for each searchstring and build a "closest n" list 
         list<SEARCHSTRINGS>::iterator pSearchStrings = g_SearchStrings.begin(); 

         while(pSearchStrings != g_SearchStrings.end()) 
         { 
            SEARCHSPAWN SearchSpawn; 
            ClearSearchSpawn(&SearchSpawn); 

            strcpy_s(szArg, pSearchStrings->szSearchString); 

            PCHAR szLine = szArg; 

            CHAR szName[MAX_STRING] = {0}; 
            CHAR szLLine[MAX_STRING] = {0}; 
            PCHAR szFilter = szLLine; 
             BOOL bArg = TRUE; 
			 strcpy_s(szLLine, szLine);
             _strlwr_s(szLLine); 

            while(bArg) 
            { 
               GetArg(szArg, szFilter, 1); 
               szFilter = GetNextArg(szFilter,1); 
               if (szArg[0]==0) 
               { 
                  bArg = FALSE; 
               } 
               else 
               { 
                  szFilter = ParseSearchSpawnArgs(szArg, szFilter, &SearchSpawn); 
               } 
            } 

            nReturn += FindMatchingSpawns(&SearchSpawn, pOrigin, pSearchStrings->bNotify); 

            pSearchStrings++; 
         } 
      } 
      else 
      { 
         // clean up targets on HUD 
         CleanHUDTargets(); 
      } 
   } 

   // if we found new matches, set the timer so it'll fire right away 
   if(0 != nReturn) 
      g_timerSeconds -= POPUPINTERVAL; 

   return nReturn; 
} 

int FindMatchingSpawns(PSEARCHSPAWN pSearchSpawn, PSPAWNINFO pOrigin, bool bNotify) 
{ 
   int nReturn = 0; 

   DebugSpewAlways("MQ2Targets::FindMatchingSpawns()"); 

   if(!pSearchSpawn || !pOrigin) 
      return nReturn; 
    
   PSPAWNINFO pSpawn=(PSPAWNINFO)pSpawnList; 

   CHAR szTemp[MAX_STRING] = {0}; 

   while(pSpawn) 
   { 
      if(pSpawn != pOrigin && SpawnMatchesSearch(pSearchSpawn, pOrigin, pSpawn)) 
      { 
         bool bAlreadyInThere = false; 
         // see if it's already in our list.  If so, don't add it again 
         for(unsigned long i = 0; i < g_Targets.Count(); i++) 
         { 
            PSPAWNINFO theTarget = (PSPAWNINFO)g_Targets[i]->VarPtr.Ptr; 
            if(NULL != theTarget && pSpawn->SpawnID == theTarget->SpawnID) 
            { 
               bAlreadyInThere = true; 
            } 
         } 
         // not already in our list, add it 
         if(false == bAlreadyInThere && NULL == strstr(pSpawn->DisplayedName, " Mount")) 
         { 
//            EnterCriticalSection(&g_csTargetSection); 
            // matches search, add to our set 
            PMQRANK pNewRank = new MQRANK; 
            pNewRank->VarPtr.Ptr = pSpawn; 
            pNewRank->Value.Float = GetDistance(pOrigin->X, pOrigin->Y, pSpawn->X, pSpawn->Y); 
            g_Targets += pNewRank; 

            if(true == bNotify) 
            { 
               nReturn += AddToNotify(pSpawn); 
            } 
//            LeaveCriticalSection(&g_csTargetSection); 
         } 
      } 
      pSpawn = pSpawn->pNext; 
   } 
/* 
   if(g_Targets.Count() > 0) 
   { 
      // get our nearest 
      pSpawn = (PSPAWNINFO)g_Targets[0]->VarPtr.Ptr; 
   } 
*/ 
   return nReturn; 
} 

VOID CleanHUDTargets() 
{ 
   DebugSpewAlways("MQ2Targets::CleanHUDTargets()"); 

   g_Targets.Cleanup(); 
} 

static int pMQRankFloatCompareNoDups(const void *A, const void *B) 
{ 
    if ((*(PMQRANK*)A)->Value.Float==(*(PMQRANK*)B)->Value.Float) 
   { 
      // if they're the same distance, sort by name 
       return(_stricmp((PSPAWNINFO((*(PMQRANK*)A)->VarPtr.Ptr))->DisplayedName, (PSPAWNINFO((*(PMQRANK*)B)->VarPtr.Ptr))->DisplayedName) * -1); 
   } 
    if ((*(PMQRANK*)A)->Value.Float<(*(PMQRANK*)B)->Value.Float) 
      return -1; 
   return 1; 
} 

int AddToNotify(PSPAWNINFO pSpawn) 
{ 
   int nReturn = 0; 

   bool bFound = false; 
   // see if it's already in our list.  If so, don't add it again 
   list<NOTIFYSTRUCT>::iterator pItem = g_NotifySpawns.begin(); 
   while(pItem != g_NotifySpawns.end()) 
   { 
      if(pSpawn->SpawnID == pItem->pSpawn->SpawnID) 
      { 
         bFound = true; 
         DebugSpewAlways("MQ2Targets::AddToNotify() %s already in list", pSpawn->DisplayedName); 
         break; 
      } 
      pItem++; 
   } 
   // only add to notify list if it's not already in there 
   if(false == bFound) 
   { 
      DebugSpewAlways("MQ2Targets::AddToNotify() Adding %s to list", pSpawn->DisplayedName); 

      NOTIFYSTRUCT notifyStruct; 
      notifyStruct.bNotified = false; 
      notifyStruct.pSpawn = pSpawn; 
      g_NotifySpawns.push_back(notifyStruct); 
      nReturn = 1; 
   } 

   return 1; 
} 

VOID RemoveFromNotify(PSPAWNINFO pSpawn, bool bPopup) 
{ 
   // see if it's already in our list.  If so, don't add it again 
   list<NOTIFYSTRUCT>::iterator pItem = g_NotifySpawns.begin(); 
   while(pItem != g_NotifySpawns.end()) 
   { 
      if(pItem->pSpawn->SpawnID == pSpawn->SpawnID) 
      { 
         DebugSpewAlways("MQ2Targets::RemoveFromNotify(%s) %d", pSpawn->DisplayedName, bPopup); 

         if(true == bPopup) 
         { 
            char szText[MAX_STRING]; 
            sprintf_s(szText, "%s despawn", pSpawn->DisplayedName); 
            DisplayOverlayText(szText, CONCOLOR_RED, 100, 500, 500, 1000); 
         } 
         // get rid of the one I just showed 
         g_NotifySpawns.erase(pItem); 
         break; 
      } 
      pItem++; 
   } 
} 

// cleans up any spawns in the notify list based on the passed in search string 
// used on "/watch remove ..." 
VOID RemoveNotifySpawns(PCHAR searchString) 
{ 
   CHAR szArg[MAX_STRING] = {0}; 
   SEARCHSPAWN SearchSpawn; 
   ClearSearchSpawn(&SearchSpawn); 

   strcpy_s(szArg, searchString); 

   PCHAR szLine = szArg; 

   CHAR szName[MAX_STRING] = {0}; 
   CHAR szLLine[MAX_STRING] = {0}; 
   PCHAR szFilter = szLLine; 
    BOOL bArg = TRUE; 
	strcpy_s(szLLine, szLine);
    _strlwr_s(szLLine); 

   while(bArg) 
   { 
      GetArg(szArg, szFilter, 1); 
      szFilter = GetNextArg(szFilter,1); 
      if (szArg[0]==0) 
      { 
         bArg = FALSE; 
      } 
      else 
      { 
         szFilter = ParseSearchSpawnArgs(szArg, szFilter, &SearchSpawn); 
      } 
   } 

   PSPAWNINFO pOrigin = GetCharInfo()->pSpawn; 
   PSPAWNINFO pSpawn=(PSPAWNINFO)pSpawnList; 
   CHAR szTemp[MAX_STRING] = {0}; 

   // see if it's already in our list.  If so, don't add it again 
   list<NOTIFYSTRUCT>::iterator pItem = g_NotifySpawns.begin(); 
   while(pItem != g_NotifySpawns.end()) 
   { 
      if(pItem->pSpawn != pOrigin && SpawnMatchesSearch(&SearchSpawn, pOrigin, pItem->pSpawn)) 
      { 
         RemoveFromNotify(pItem->pSpawn); 
         // start at the beginning of the list again (since it's changed) 
         pItem = g_NotifySpawns.begin(); 
      } 
      else 
         pItem++;   // go to the next item 
   } 
} 

VOID PopupNotifyTarget() 
{ 
   if(g_NotifySpawns.size() > 0) 
   { 
      int nShown = 0; 
      char szList[MAX_STRING] = {0}; 
      // grab the first item to show (if it hasn't already been shown) 
      list<NOTIFYSTRUCT>::iterator pItem = g_NotifySpawns.begin(); 
      while(pItem != g_NotifySpawns.end() && nShown < 10) 
      { 
         if(false == pItem->bNotified) 
         { 
            char szText[MAX_STRING]; 
            sprintf_s(szText, "%s spawned\n", pItem->pSpawn->DisplayedName); 
            strcat_s(szList, szText); 
            nShown++; 
         } 
         // mark it as handled 
         pItem->bNotified = true; 
         pItem++; 
      } 
      if(nShown > 0) 
      { 
         DebugSpewAlways("MQ2Targets::PopupNotifyTarget() %s", szList); 
         //EzCommand("/beep");   //PlaySound("SomeSound.wav",0,SND_ASYNC); 
         DisplayOverlayText(szList, CONCOLOR_GREEN, 100, 500, 500, 1900); 
      } 
   } 
}