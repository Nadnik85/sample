/*

MQ2Combat plugin
by Dok

Commands:
/combat -- /auto combat command.  /combat help for more info

To Do:
Remember settings on zoning.  maybe temp settings saved to file on changes?
Auto-accept raid invite.

Changes:
2004.09.12
Revised AssistPlayer to check if AssistName is in range.
Revised AutoGroup to have a random 2-3 sec delay
Added /fight command since /combat was toggling "combat music"
Fixed bug in assist

2004.07.10
Fix CTD when an enrage message goes off same time a mob dies.

2004.05.05
Initial Release
*/

#include "../MQ2Plugin.h"

PreSetup("MQ2Combat");

void DoBack(bool hold);
void LoadINI(void);
void SaveINI(void);
void ResetVars(void);
VOID CombatCommand(PSPAWNINFO pChar, PCHAR szLine);
void AssistPlayer(void);

CHAR MsgEnrage[MAX_STRING] = " has become ENRAGED.";
DWORD EnrageDuration = 14;
DWORD EnragePct = 20;

CHAR MsgInfur[MAX_STRING] = " is infuriated!";
DWORD InfurDuration = 7;

CHAR MsgGroup[MAX_STRING] = "To join the group, click on the 'FOLLOW' option, or 'DISBAND' to cancel.";

BOOL autoGroup = true;
BOOL DoCombat = false;
BOOL MobEnrage = false;
BOOL MobInfur = false;
BOOL sticking = false;
BOOL useStick = true;
CHAR MeleeAbilities[10][MAX_STRING];
CHAR StickParam[MAX_STRING];
FLOAT CombatRange = 100.0;
PCHARINFO pChar;
time_t EnrageStart;
time_t InfurStart;

BOOL DoAssist = false;
CHAR AssistName[MAX_STRING];
FLOAT AssistHPs;

BOOL DoFollow = false;
CHAR FollowName[MAX_STRING];
FLOAT FollowRange;
FLOAT FollowStick = false;

PSPAWNINFO psTarget;
int TimeToInvite = 0;
BOOL GroupInvite = FALSE;

PLUGIN_API VOID InitializePlugin(VOID)
{
   DebugSpewAlways("Initializing MQ2Combat");
   AddCommand("/combat",CombatCommand);
   AddCommand("/fight",CombatCommand);
}
PLUGIN_API VOID ShutdownPlugin(VOID)
{
   DebugSpewAlways("Shutting down MQ2Combat");
   RemoveCommand("/combat");
   RemoveCommand("/fight");
}
PLUGIN_API VOID SetGameState(DWORD GameState)
{
   if (GameState==GAMESTATE_INGAME) {
      pChar = GetCharInfo();
      LoadINI();
      DoAssist = false;
      DoFollow = false;
   }
}
PLUGIN_API VOID OnPulse(VOID)
{         
   if (GroupInvite && TimeToInvite <= clock()){
      pTarget = NULL;
      DoCommand(GetCharInfo()->pSpawn,"/invite");
      GroupInvite = FALSE;
   }
   if ( DoCombat ) {
      CHAR textString[MAX_STRING];

      if ( DoFollow && strlen(FollowName)>0 ) {
         PSPAWNINFO spawn;
         SEARCHSPAWN ssSpawn;
         ClearSearchSpawn(&ssSpawn);
           
         ssSpawn.SpawnType = PC;
         strcpy_s(ssSpawn.szName,FollowName);
         spawn = NthNearestSpawn(&ssSpawn,1,pChar->pSpawn);

         if ( !FollowStick ) {
            if ( (spawn && spawn->SpawnID) ) {
               if ( GetDistance(pChar->pSpawn,spawn) >= FollowRange ) {
                  sprintf_s(textString,"%s is moving.",spawn->Name);
                  WriteChatColor(textString,USERCOLOR_DEFAULT);
                  pTarget = NULL;
                  PSPAWNINFO *sTarget = NULL;
                  sTarget = (PSPAWNINFO*)ppTarget;
                  *sTarget = spawn;
                  FollowStick = true;
                  Beep(0x500,250);
                  if ( useStick )
                            DoCommand(pChar->pSpawn,"/stick");
               } else
                  FollowStick = false;
            } else
               FollowStick = false;
         } else {
            if ( GetDistance(pChar->pSpawn,spawn) <= get_melee_range(pLocalPlayer,(EQPlayer*)spawn) ) {
               FollowStick = false;
               if ( useStick )
                  DoCommand(pChar->pSpawn,"/stick off");
            }
         }
      } else
         FollowStick = false;


      if ( DoAssist && strlen(AssistName)>0 ) {
         if (pTarget && ppTarget) {
            psTarget = (PSPAWNINFO)pTarget;
            if ( (GetSpawnType(psTarget)==NPC || GetSpawnType(psTarget)==PET) && psTarget->StandState) {
               if (GetDistance(pChar->pSpawn,(PSPAWNINFO)pTarget) <= CombatRange && psTarget->HPCurrent <= AssistHPs)
                  DoCommand(pChar->pSpawn,"/attack on");
               else
                  AssistPlayer();
            } else
               AssistPlayer();
         } else
            AssistPlayer();
      }

      if ( pTarget && ppTarget ) {

         psTarget = (PSPAWNINFO)pTarget;
         FLOAT meleeRange = 0;
         time_t curTime;
         time( &curTime );

         if ( (GetSpawnType(psTarget)==NPC || GetSpawnType(psTarget)==PC || GetSpawnType(psTarget)==PET) && psTarget->StandState ) {
            if ( *EQADDR_ATTACK > 0 ) {

               if ( (CombatRange > 0) && (GetDistance(pChar->pSpawn,(PSPAWNINFO)pTarget) > CombatRange) ) {
                  ResetVars();
                  return;
               }

               if ( useStick && !sticking ) {
                  sprintf_s(textString,"/stick %s",StickParam);
                  DoCommand(pChar->pSpawn,textString);
                  sticking = true;
               }

               for ( int counter = 1; counter <= 9; counter++ ) {
                  if ( strcmp(MeleeAbilities[counter],"off") ) {
/*                     if ( !(pChar->Stunned) && !((long)(pChar->pSpawn->pActorInfo->CastingData.SpellID) >= 0) ) */
                     if ( !(pChar->Stunned) && !((long)(pChar->pSpawn->CastingData.SpellID) >= 0) )
                   DoAbility(pChar->pSpawn,MeleeAbilities[counter]);
                  }
               }
            } else
               if (!MobEnrage && !MobInfur)
                  sticking = false;
         
            if (MobEnrage || MobInfur) {
               if ( MobEnrage ) {
                  if ( difftime(curTime, EnrageStart) >= EnrageDuration ) {
                     MobEnrage = false;
                     EnrageStart = 0;
                     if (MobInfur)
                        sprintf_s(textString,"%s no longer enraged, but still infuriated.",psTarget->DisplayedName);
                     else {
                        if (useStick)
                           DoCommand(pChar->pSpawn,"/stick unpause");
                        DoCommand(pChar->pSpawn,"/attack on");
                        sprintf_s(textString,"%s no longer enraged. Attack on.",psTarget->DisplayedName);
                     }
                     WriteChatColor(textString,USERCOLOR_DEFAULT);
                  }
               }
               if ( MobInfur ) {
                  meleeRange = get_melee_range(pLocalPlayer,(EQPlayer*)psTarget);
                  if( GetDistance(pChar->pSpawn,psTarget) < (meleeRange+5) )
                     DoBack(true);
                  else
                     DoBack(false);

                  if ( difftime(curTime, InfurStart) >= InfurDuration ) {
                     MobInfur = false;
                     InfurStart = 0;
                     if (MobEnrage)
                        sprintf_s(textString,"%s no longer infuriated, but still enraged.",psTarget->DisplayedName);
                     else {
                        if (useStick)
                           DoCommand(pChar->pSpawn,"/stick unpause");
                        DoCommand(pChar->pSpawn,"/attack on");
                        sprintf_s(textString,"%s no longer infuriated. Attack on.",psTarget->DisplayedName);
                     }
                     WriteChatColor(textString,USERCOLOR_DEFAULT);
                  }
               }
            } else {
               EnrageStart = 0;
               InfurStart = 0;
            }
         } else
            ResetVars();
      } else
         ResetVars();
   }

}
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
   if (DoCombat) {
      if ( pTarget && ppTarget ) {
         psTarget = (PSPAWNINFO)pTarget;
         CHAR textString[MAX_STRING];

         if ( char *pFound=strstr(Line,MsgEnrage) ) {
            if ( char *pFound=strstr(Line,psTarget->DisplayedName) ) {
				if ((psTarget->HPCurrent) <= (LONG)EnragePct) {
                  if ( *EQADDR_ATTACK > 0 )
                     DoCommand(pChar->pSpawn,"/attack off");
                  if (useStick)
                     DoCommand(pChar->pSpawn,"/stick pause");
                  time( &EnrageStart );
                  MobEnrage = true;
                  sprintf_s(textString,"%s enraged.  Attack off.",psTarget->DisplayedName);
                  WriteChatColor(textString,USERCOLOR_DEFAULT);
               }
            }
         }

         if ( char *pFound=strstr(Line,MsgInfur) ) {
            if ( char *pFound=strstr(Line,psTarget->DisplayedName) ) {
               if ( *EQADDR_ATTACK > 0 )
                  DoCommand(pChar->pSpawn,"/attack off");
               if (useStick)
                  DoCommand(pChar->pSpawn,"/stick pause");
               time( &InfurStart );
               MobInfur = true;
               sprintf_s(textString,"%s infuriated.  Attack off.",psTarget->DisplayedName);
               WriteChatColor(textString,USERCOLOR_DEFAULT);
            }
         }
      }
   }

   if ( autoGroup ) {
      if( !strcmp(Line,MsgGroup) ) {
         int randomnumber = 2000 + (rand()%100);
            TimeToInvite = clock() + randomnumber;
         GroupInvite = TRUE;
      }
   }

   return 0;
}
void DoBack(bool hold)
{
   MQ2Globals::ExecuteCmd(FindMappableCommand("back"),1,0);
   if( !hold )
      MQ2Globals::ExecuteCmd(FindMappableCommand("back"),0,0);
}
VOID LoadINI(VOID)
{
   char szTemp[MAX_STRING];
   GetPrivateProfileString(GetCharInfo()->Name,"DoCombat","on",szTemp,MAX_STRING,INIFileName);
   DoCombat = (strncmp(szTemp,"on",3)==0);

   GetPrivateProfileString(GetCharInfo()->Name,"CombatRange","100.0",szTemp,MAX_STRING,INIFileName);
   CombatRange = (float)atof(szTemp);

   GetPrivateProfileString(GetCharInfo()->Name,"useStick","on",szTemp,MAX_STRING,INIFileName);
   useStick = (strncmp(szTemp,"on",3)==0);

   GetPrivateProfileString(GetCharInfo()->Name,"StickParam","",StickParam,MAX_STRING,INIFileName);

   GetPrivateProfileString(GetCharInfo()->Name,"autoGroup","on",szTemp,MAX_STRING,INIFileName);
   autoGroup = (strncmp(szTemp,"on",3)==0);

   GetPrivateProfileString(GetCharInfo()->Name,"AssistHPs","0.0",szTemp,MAX_STRING,INIFileName);
   AssistHPs = (float)atof(szTemp);

   GetPrivateProfileString(GetCharInfo()->Name,"FollowRange","120.0",szTemp,MAX_STRING,INIFileName);
   FollowRange = (float)atof(szTemp);

   for ( int counter = 1; counter <= 9; counter++ ) {
      sprintf_s(szTemp,"MeleeAbility%d",counter);
      GetPrivateProfileString(GetCharInfo()->Name,szTemp,"off",MeleeAbilities[counter],MAX_STRING,INIFileName);
   }

   SaveINI();
}
VOID SaveINI(VOID)
{
   char szTemp[MAX_STRING];
   sprintf_s(szTemp,"%s",DoCombat?"on":"off");
   WritePrivateProfileString(GetCharInfo()->Name,"DoCombat",szTemp,INIFileName);

   sprintf_s(szTemp,"%.1f",CombatRange);
   WritePrivateProfileString(GetCharInfo()->Name,"CombatRange",szTemp,INIFileName);

   sprintf_s(szTemp,"%s",useStick?"on":"off");
   WritePrivateProfileString(GetCharInfo()->Name,"useStick",szTemp,INIFileName);

   WritePrivateProfileString(GetCharInfo()->Name,"StickParam",StickParam,INIFileName);

   sprintf_s(szTemp,"%s",autoGroup?"on":"off");
   WritePrivateProfileString(GetCharInfo()->Name,"autoGroup",szTemp,INIFileName);

   sprintf_s(szTemp,"%.1f",AssistHPs);
   WritePrivateProfileString(GetCharInfo()->Name,"AssistHPs",szTemp,INIFileName);

   sprintf_s(szTemp,"%.1f",FollowRange);
   WritePrivateProfileString(GetCharInfo()->Name,"FollowRange",szTemp,INIFileName);

   for(int counter = 1; counter <= 9; counter++) {
      sprintf_s(szTemp,"MeleeAbility%d",counter);
      WritePrivateProfileString(GetCharInfo()->Name,szTemp,MeleeAbilities[counter],INIFileName);
   }
}
VOID ResetVars(VOID)
{
   if ( *EQADDR_ATTACK > 0 )
      DoCommand(pChar->pSpawn,"/attack off");
   if ( sticking )
      DoCommand(pChar->pSpawn,"/stick off");
   sticking = false;
   MobEnrage = false;
   MobInfur = false;
   EnrageStart = 0;
   InfurStart = 0;
}
VOID CombatCommand(PSPAWNINFO pChar, PCHAR szLine)
{
   char textString[MAX_STRING];
   char commandArg[MAX_STRING];
   char Arg1[MAX_STRING];
   char Arg2[MAX_STRING];
   char Arg3[MAX_STRING];
   char Arg4[MAX_STRING];
   char Arg5[MAX_STRING];
   DWORD command = 0;

   GetArg(commandArg,szLine,1);
   GetArg(Arg1,szLine,2);
   GetArg(Arg2,szLine,3);
   GetArg(Arg3,szLine,4);
   GetArg(Arg4,szLine,5);
   GetArg(Arg5,szLine,6);

   if (commandArg[0]==0)
        command = 0;
   else if( !strcmp(commandArg,"on") )
      command = 1;
   else if( !strcmp(commandArg,"off") )
      command = 2;
   else if( !strcmp(commandArg,"stick") )
      command = 3;
   else if( !strcmp(commandArg,"dist") )
      command = 4;
   else if( !strcmp(commandArg,"ability") )
      command = 5;
   else if( !strcmp(commandArg,"group") )
      command = 6;
   else if( !strcmp(commandArg,"assist") )
      command = 7;
   else if( !strcmp(commandArg,"reload") )
      command = 8;
   else if( !strcmp(commandArg,"save") )
      command = 9;
   else if( !strcmp(commandArg,"follow") )
      command = 10;
   else
      command = 0;


   switch (command) {
      case 1:  //on
         {
            DoCombat = true;
            WriteChatColor("Combat Enabled",USERCOLOR_DEFAULT);
         }
         break;
      case 2:  //off
         {
                DoCombat = false;
            WriteChatColor("Combat Disabled",USERCOLOR_DEFAULT);
         }
         break;
      case 3:  //stick
         {
            if( !strcmp(Arg1,"on") ) {
               useStick = true;
               WriteChatColor("Combat: Stick Enabled",USERCOLOR_DEFAULT);
            } else if( !strcmp(Arg1,"off") ) {
               useStick = false;
               WriteChatColor("Combat: Stick Disabled",USERCOLOR_DEFAULT);
            } else if( !strcmp(Arg1,"clear") ) {
               sprintf_s(StickParam,"");
               sprintf_s(textString,"Combat: Stick Param (%s)",StickParam);
               WriteChatColor(textString,USERCOLOR_DEFAULT);
            } else if (Arg1[0]!=0) {
               sprintf_s(StickParam,"%s %s %s %s %s",Arg1,Arg2,Arg3,Arg4,Arg5);
               sprintf_s(textString,"Combat: Stick Param (%s)",StickParam);
               WriteChatColor(textString,USERCOLOR_DEFAULT);
            } else {
               if (useStick)
                  WriteChatColor("Combat: Stick Enabled",USERCOLOR_DEFAULT);
               else
                  WriteChatColor("Combat: Stick Disabled",USERCOLOR_DEFAULT);
               sprintf_s(textString,"Combat: Stick Param (%s)",StickParam);
               WriteChatColor(textString,USERCOLOR_DEFAULT);
            }
         }
         break;
      case 4:  //dist
         {
            if ( isdigit(Arg1[0]) ) {
               CombatRange = (float)atof(Arg1);
               sprintf_s(textString,"Combat: Range (%.1f)",CombatRange);
               WriteChatColor(textString,USERCOLOR_DEFAULT);
            } else {
               sprintf_s(textString,"Combat: Range (%.1f)",CombatRange);
               WriteChatColor(textString,USERCOLOR_DEFAULT);
            }
         }
         break;
      case 5:  //ability
         {
            if (Arg2[0]!=0) {
               if (isdigit(Arg1[0])) {
                        sprintf_s(MeleeAbilities[atoi(Arg1)],"%s",Arg2);
                  sprintf_s(textString,"Combat: Ability %d (%s)",atoi(Arg1),MeleeAbilities[atoi(Arg1)]);
                  WriteChatColor(textString,USERCOLOR_DEFAULT);
               }
            } else {
               for(int counter = 1; counter <= 9; counter++) {
                  sprintf_s(textString,"Combat: Ability %d (%s)",counter,MeleeAbilities[counter]);
                  WriteChatColor(textString,USERCOLOR_DEFAULT);
               }
            }
         }
         break;
      case 6:  //group
         {
            if( !strcmp(Arg1,"on") ) {
               autoGroup = true;
               WriteChatColor("Combat: Group Enabled",USERCOLOR_DEFAULT);
            } else if( !strcmp(Arg1,"off") ) {
               autoGroup = false;
               WriteChatColor("Combat: Group Disabled",USERCOLOR_DEFAULT);
            } else {
               if (autoGroup)
                  WriteChatColor("Combat: Group Enabled",USERCOLOR_DEFAULT);
               else
                  WriteChatColor("Combat: Group Disabled",USERCOLOR_DEFAULT);
            }
         }
         break;
      case 7:  //assist
         {
            if( !strcmp(Arg1,"off") ) {
               DoAssist = false;
               WriteChatColor("Combat: Assist Disabled",USERCOLOR_DEFAULT);
            } else if ( isdigit(Arg1[0]) ) {
               AssistHPs = (float)atof(Arg1);
               sprintf_s(textString,"Combat: Assist HPs (%.1f)",AssistHPs);
               WriteChatColor(textString,USERCOLOR_DEFAULT);
            } else if (Arg1[0]!=0) {
               DoAssist = true;
               sprintf_s(AssistName,"%s",Arg1);
               sprintf_s(textString,"Combat: Assist Name (%s)",AssistName);
               WriteChatColor(textString,USERCOLOR_DEFAULT);
            } else {
               if (DoAssist)
                  WriteChatColor("Combat: Assist Enabled",USERCOLOR_DEFAULT);
               else
                  WriteChatColor("Combat: Assist Disabled",USERCOLOR_DEFAULT);
               sprintf_s(textString,"Combat: Assist HPs (%.1f)",AssistHPs);
               WriteChatColor(textString,USERCOLOR_DEFAULT);
               sprintf_s(textString,"Combat: Assist Name (%s)",AssistName);
               WriteChatColor(textString,USERCOLOR_DEFAULT);
            }
         }
         break;
      case 8:  //reload
         {
            LoadINI();
            WriteChatColor("Combat: Loaded INI",USERCOLOR_DEFAULT);
         }
         break;
      case 9:  //save
         {
            SaveINI();
            WriteChatColor("Combat: Saved INI",USERCOLOR_DEFAULT);
         }
         break;
      case 10:  //follow
         {
            if( !strcmp(Arg1,"off") ) {
               DoFollow = false;
               WriteChatColor("Combat: Follow Disabled",USERCOLOR_DEFAULT);
            } else if ( isdigit(Arg1[0]) ) {
               FollowRange = (float)atof(Arg1);
               sprintf_s(textString,"Combat: Follow Range (%.1f)",FollowRange);
               WriteChatColor(textString,USERCOLOR_DEFAULT);
            } else if (Arg1[0]!=0) {
               DoFollow = true;
               sprintf_s(FollowName,"%s",Arg1);
               sprintf_s(textString,"Combat: Follow Name (%s)",FollowName);
               WriteChatColor(textString,USERCOLOR_DEFAULT);
            } else {
               if (DoFollow)
                  WriteChatColor("Combat: Follow Enabled",USERCOLOR_DEFAULT);
               else
                  WriteChatColor("Combat: Follow Disabled",USERCOLOR_DEFAULT);
               sprintf_s(textString,"Combat: Follow Range (%.1f)",FollowRange);
               WriteChatColor(textString,USERCOLOR_DEFAULT);
               sprintf_s(textString,"Combat: Follow Name (%s)",FollowName);
               WriteChatColor(textString,USERCOLOR_DEFAULT);
            }
         }
         break;
      case 0:  //help
         {
            WriteChatColor("Usage: /combat",USERCOLOR_DEFAULT);
            WriteChatColor("ability [#] [ability name or number] - Set ability to use.",USERCOLOR_DEFAULT);
            WriteChatColor("assist [off|<name>|<hp%>] - Set assist.",USERCOLOR_DEFAULT);
            WriteChatColor("dist [#] - Set max distance",USERCOLOR_DEFAULT);
            WriteChatColor("follow [off|<name>|<distance>] - Set follow",USERCOLOR_DEFAULT);
            WriteChatColor("group [on|off] - Enable/disable autogroup on invite.",USERCOLOR_DEFAULT);
            WriteChatColor("on/off - enable/disable combat plugin",USERCOLOR_DEFAULT);
            WriteChatColor("reload - Reload INI settings",USERCOLOR_DEFAULT);
            WriteChatColor("save - Save INI settings",USERCOLOR_DEFAULT);
            WriteChatColor("stick [on|off|<param>|clear] - Set to use /stick command",USERCOLOR_DEFAULT);
         }
         break;
   }
}
VOID AssistPlayer(VOID)
{
   if (DoCombat && DoAssist && strlen(AssistName)>0) {
      PSPAWNINFO spawn;
      SEARCHSPAWN ssSpawn;
      ClearSearchSpawn(&ssSpawn);
           
      ssSpawn.SpawnType = PC;
      strcpy_s(ssSpawn.szName,AssistName);
      spawn = NthNearestSpawn(&ssSpawn,1,pChar->pSpawn);

      if (spawn && GetDistance(pChar->pSpawn,spawn) <= CombatRange) {
         CHAR textString[MAX_STRING];
         sprintf_s(textString,"/assist %s",AssistName);
         DoCommand(pChar->pSpawn,textString);
         if ( *EQADDR_ATTACK > 0 )
            DoCommand(pChar->pSpawn,"/attack off");
      }
   }
} 