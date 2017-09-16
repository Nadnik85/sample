//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Projet: MQ2Missing.cpp   | Beta
// Author: s0rCieR          | 
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

#define        PLUGIN_NAME "MQ2Missing"
#define        PLUGIN_DATE     20060704
#define        PLUGIN_VERS        1.002

#ifndef PLUGIN_API
  #include "../MQ2Plugin.h"
using namespace std;
  PreSetup(PLUGIN_NAME);
  PLUGIN_VERSION(PLUGIN_VERS);
  #include <string>
  #include <map>
#endif PLUGIN_API

typedef map<string,string> Liste;

void ListInsert(Liste *ThisList, PCHAR ThisName) 
{
	char szName[MAX_STRING] = { 0 };
	strcpy_s(szName, ThisName);
	_strlwr_s(szName);
	ThisList->insert(Liste::value_type(szName,ThisName));
}

void GetExpeMembers(Liste *MyList) 
{
  PDZMEMBER Current=(PDZMEMBER)pDZMember;
  while(Current) 
  {
    ListInsert(MyList,Current->Name);
    Current=Current->pNext;
  }
}

void GetRaidMembers(Liste *MyList) 
{
  if(pRaid && pRaid->RaidMemberCount>0)
    for(int m=0; m<72; m++) 
      if(pRaid->RaidMemberUsed[m]) ListInsert(MyList,pRaid->RaidMember[m].Name);
}

void GetTaskMembers(Liste *MyList) 
{
  PTASKMEMBER Current=(PTASKMEMBER)pTaskMember;
  while(Current)
  {
    ListInsert(MyList,Current->Name);
    Current=Current->pNext;
  }
}

void GetZoneMembers(Liste *MyList, bool InGuild) 
{
	__int64 GuildID = (GetCharInfo() && GetCharInfo()->pSpawn) ? GetCharInfo()->pSpawn->GuildID : -1;
	if(!InGuild || GuildID != -1 || GuildID != 0) 
	{
		PSPAWNINFO Current=(PSPAWNINFO)pSpawnList;
		while(Current)
		{
			if (Current->Type == SPAWN_PLAYER)
				if (!InGuild || GuildID == Current->GuildID)
					ListInsert(MyList, Current->DisplayedName);
			Current=Current->pNext;
		}
	}
}

void Command(PSPAWNINFO pChar, PCHAR Cmd) 
{
  char NameA[MAX_STRING]; char NameB[MAX_STRING];
  bool HaveA=true; bool HaveB=true;
  Liste ListA,ListB; 

  if(strlen(GetArg(NameA,Cmd,1))) {
     ListA.clear();
    if(!_stricmp(NameA,"DZ"))         GetExpeMembers(&ListA);
    else if(!_stricmp(NameA,"TASK"))  GetTaskMembers(&ListA);
    else if(!_stricmp(NameA,"RAID"))  GetRaidMembers(&ListA);
    else if(!_stricmp(NameA,"ZONE"))  GetZoneMembers(&ListA,false);
    else if(!_stricmp(NameA,"GUILD")) GetZoneMembers(&ListA,true);
    else HaveA=false;
  } else HaveA=false;
   
  if(strlen(GetArg(NameB,Cmd,2))) {
     ListB.clear();
    if(!_stricmp(NameB,"DZ"))         GetExpeMembers(&ListB);
    else if(!_stricmp(NameB,"TASK"))  GetTaskMembers(&ListB);
    else if(!_stricmp(NameB,"RAID"))  GetRaidMembers(&ListB);
    else if(!_stricmp(NameB,"ZONE"))  GetZoneMembers(&ListB,false);
    else if(!_stricmp(NameB,"GUILD")) GetZoneMembers(&ListB,true);
    else HaveB=false;
  } else HaveB=false;

  if(!HaveA || !HaveB) {
    WriteChatColor("Usage: /missing [list] [list]");
    WriteChatColor("while list=dz,task,raid,zone,guild");
    WriteChatColor("Example: /missing raid task");
    return;
  }

  Liste::iterator x,f;

  WriteChatf("[MQ2Missing]::[\ag%s\ar>>\ag%s\ax]",NameA,NameB);
  for(x=ListA.begin(); x!=ListA.end(); x++) 
    if(ListB.end()==(f=ListB.find((*x).first))) 
       WriteChatf("\ar>>\ax %s",(*x).second.c_str());

  WriteChatf("[MQ2Missing]::[\ag%s\ar<<\ag%s\ax]",NameA,NameB);
  for(x=ListB.begin(); x!=ListB.end(); x++) 
    if(ListA.end()==(f=ListA.find((*x).first))) 
       WriteChatf("\ar<<\ax %s",(*x).second.c_str());
       
}

PLUGIN_API VOID InitializePlugin() {
  AddCommand("/missing",Command);
}

PLUGIN_API VOID ShutdownPlugin() {
  RemoveCommand("/missing");
}