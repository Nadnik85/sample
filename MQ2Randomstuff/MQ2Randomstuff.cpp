// MQ2RandomStuff.cpp : Sets up an MQ2Data with a random string picked from
//                      an INI file
//
// Commands:
//  /randomstuff [section] - Picks a random key from [section] of the INI file,
//                           to be returned by ${RandomStuff}
//

#include "../MQ2Plugin.h"

typedef struct _INISTRING {
   CHAR Text[MAX_STRING];
   struct _INISTRING *pNext;
} INISTRING, *PINISTRING;

PreSetup("MQ2RandomStuff");

PINISTRING pIniString=0;

char Section[MAX_STRING]="DEFAULT";
char MQ2RandomStuffString[MAX_STRING]="NULL";

int SectionCount=0;

VOID ClearElements()
{
   while(pIniString)
   {
      PINISTRING pNext=pIniString->pNext;
      delete pIniString;
      pIniString=pNext;
   }
   SectionCount=0;
}

VOID AddElement(PCHAR szLine)
{
   PINISTRING pElement=new INISTRING;
   pElement->pNext=pIniString;
   pIniString=pElement;
   strcpy_s(pElement->Text,szLine);
   DebugSpew("New element '%s'",pElement->Text);
}

BOOL LoadElements()
{
   ClearElements();
   CHAR ElementList[MAX_STRING*10] = {0};
    CHAR szBuffer[MAX_STRING] = {0};
   CHAR szBuffer2[MAX_STRING];
   
   if (!GetPrivateProfileString(Section,NULL,"",ElementList,MAX_STRING*10,INIFileName)) {
      sprintf_s(szBuffer,"MQ2RandomStuff::Invalid section '%s' specified.  Creating it.",Section);
        WriteChatColor(szBuffer,CONCOLOR_RED);
      for (int i=1;i<=3;i++){
         sprintf_s(szBuffer,"%s%d",Section,i);
         sprintf_s(szBuffer2,"Random string #%d",i);
         WritePrivateProfileString(Section,szBuffer,szBuffer2,INIFileName);
      }
      return 0;
   }
   PCHAR pElementList = ElementList;
   
    while (pElementList[0]!=0) {
      GetPrivateProfileString(Section,pElementList,"",szBuffer,MAX_STRING,INIFileName);
        if (szBuffer[0]!=0) {
         AddElement(szBuffer);
         SectionCount++;
        }
        pElementList+=strlen(pElementList)+1;
    }
   return 1;
}

VOID RandomStuffCommand(PSPAWNINFO pChar, PCHAR szLine)
{
   int szRandInt;
   int i=0;
   
   strcpy_s(Section,szLine);
   if (!szLine[0]) {
      WriteChatColor("MQ2RandomStuff::No section given.  Please use /randomstuff [section]",CONCOLOR_RED);
        return;
   }
   
   if (!LoadElements()) return;
   PINISTRING pElement=pIniString;
   
   szRandInt = rand()%SectionCount;
   while(i!=szRandInt) {
        pElement=pElement->pNext;
      i++;
   }
   strcpy_s(MQ2RandomStuffString,pElement->Text);
}

BOOL dataRandomStuff(PCHAR szIndex, MQ2TYPEVAR &Ret)
{
   Ret.Ptr=&MQ2RandomStuffString[0];
    Ret.Type=pStringType;
   return true;
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
   DebugSpewAlways("Initializing MQ2RandomStuff");
   
   AddCommand("/randomstuff",RandomStuffCommand);
   AddMQ2Data("RandomStuff",dataRandomStuff);
   srand(time_t(0));
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
   DebugSpewAlways("Shutting down MQ2RandomStuff");
   
   ClearElements();
   RemoveCommand("/randomstuff");
   RemoveMQ2Data("RandomStuff");
   
} 