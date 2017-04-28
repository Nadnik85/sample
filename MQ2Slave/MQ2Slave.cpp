// MQ2Slave.cpp : Defines the entry point for the DLL application.
//
// Developer: Mr Math
// Date: 9.30.2005
//
// Inspired by Fibby's MQ2MasterMind. MQ2Mastermind was good because
// it let me control my bots, and have them run macros w/o actually
// looking at that screen and manually fiddle with buttons.
//
// I found I would also have situations where I'd want friends to control
// my toons, for situations where one of us would fall asleep etc..
// but MQ2MasterMind only allows 1 controller.
//
// The next problem is each time the bot is run, you need to resend the password
//
// When someone sends a blank password, like many do in eq, it would accept it..
// Not good for a random person checking to see if you've zoned see's
// "I will do you bidding"...
//
// I made MQ2Slave to address all these issues.
//
// To start it, type /plugin mq2slave
// To add someone named "Mrmath" to the master list, type
// /addmaster mrmath
// To remove "Mrmath" from the master list, you need to type
// /remmaster Mrmath
// *Note* removing masters is case sensitive, adding them is not.
//
// That is all there is to it, if any of the masters in the .ini file
// send the slave a tell starting with / then the slave will do it.
//
// The list of masters is saved in the MQ2Slave.ini file which is
// completely customized the first time you run MQ2Slave and is
// edited via eq (you don't have to open the .ini file).
//
// Bugs: None that I am aware of right now, would be good to find out
//
//

#include "../MQ2Plugin.h"

int n_Masters;
char MyMasters[20][MAX_STRING];

void addMaster(PSPAWNINFO pChar, PCHAR szLine);
void remMaster(PSPAWNINFO pChar, PCHAR szLine);
void LoadINI();

PreSetup("MQ2Slave");

void addMaster(PSPAWNINFO pChar, PCHAR szLine) {
    char szTemp[MAX_STRING];
    char szTemp2[MAX_STRING];
    n_Masters++;
    _itoa_s(n_Masters,szTemp,10);
    WritePrivateProfileString("MQ2Slave", "nMasters", szTemp, INIFileName);
    strcpy_s(szTemp2,"Master");
    _itoa_s(n_Masters-1,szTemp,10);
    strcat_s(szTemp2,szTemp);
    strcpy_s(szTemp,szLine);
    szTemp[0] = toupper(szTemp[0]);
    strcpy_s(MyMasters[n_Masters-1],szTemp);
    WritePrivateProfileString("MQ2Slave", szTemp2, szTemp, INIFileName);
    strcpy_s(szTemp,"Added ");
    strcat_s(szTemp,szLine);
    strcat_s(szTemp," as a master.");
    WriteChatColor(szTemp, USERCOLOR_SYSTEM);
}

void removeMaster(PSPAWNINFO pChar, PCHAR szLine) {
    char szTemp[MAX_STRING];
    char szTemp2[MAX_STRING];
    szLine[0] = toupper(szLine[0]);
    for (int i = 0; i < n_Masters; i++) {
        if (strcmp(MyMasters[i],szLine) == 0) {
            for (int j = i; j < n_Masters-1; j++) {
                strcpy_s(MyMasters[j],MyMasters[j+1]);
                _itoa_s(i,szTemp,10);
                strcpy_s(szTemp2,"Master");
                strcat_s(szTemp2,szTemp);
                WritePrivateProfileString("MQ2Slave", szTemp2, MyMasters[j], INIFileName);
            }
            strcpy_s(MyMasters[n_Masters-1],"");
            _itoa_s(n_Masters-1,szTemp,10);
            strcpy_s(szTemp2,"Master");
            strcat_s(szTemp2,szTemp);
            WritePrivateProfileString("MQ2Slave", szTemp2, "", INIFileName);
            n_Masters--;
            _itoa_s(n_Masters,szTemp,10);
            WritePrivateProfileString("MQ2Slave", "nMasters", szTemp , INIFileName);
            strcpy_s(szTemp,szLine);
            strcat_s(szTemp," was removed successfully.");
            WriteChatColor(szTemp, USERCOLOR_SYSTEM);
            return;
        }
    }
    WriteChatColor("That master was not found!", USERCOLOR_SYSTEM);
}

VOID LoadINI() {
    char tempString[MAX_STRING];
    char szTemp[MAX_STRING];
    GetPrivateProfileString("MQ2Slave", "nMasters", "0", tempString, MAX_STRING, INIFileName);
    n_Masters = atoi(tempString);
    strcpy_s(szTemp,tempString);
    strcat_s(szTemp," Masters found.");
    WriteChatColor(szTemp, USERCOLOR_SYSTEM);
    for (int i = 0; i < n_Masters; i++) {
        strcpy_s(szTemp,"Master");
        _itoa_s(i,tempString,10);
        strcat_s(szTemp,tempString);
        GetPrivateProfileString("MQ2Slave", szTemp, "", MyMasters[i], MAX_STRING, INIFileName);
        MyMasters[i][0] = toupper(MyMasters[i][0]);
        strcpy_s(szTemp,MyMasters[i]);
        strcat_s(szTemp," loaded");
        WriteChatColor(szTemp, USERCOLOR_SYSTEM);
    }
    WriteChatColor("Done initializing.", USERCOLOR_SYSTEM);
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID) {
    DebugSpewAlways("Initializing MQ2Slave");
    WriteChatColor("MQ2Slave is initializing...", USERCOLOR_SYSTEM);
    AddCommand("/addmaster", addMaster, 0, 0, 1);
    AddCommand("/remmaster", removeMaster, 0, 0, 1);
    if (MQ2Globals::gGameState==GAMESTATE_INGAME) {
        sprintf_s(INIFileName,"%s\\MQ2Slave.ini",gszINIPath);
        LoadINI();
    }
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID) {
    DebugSpewAlways("Shutting down MQ2Slave");
    RemoveCommand("/addmaster");
    RemoveCommand("/remmaster");
}


// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
// but after MQ filters and chat events are taken care of.
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color) {
    DebugSpewAlways("MQ2MasterMind::OnIncomingChat(%s)",Line);
    char szTemp[MAX_STRING];

    strcpy_s(szTemp,Line);
    strcpy_s(szTemp,strtok_s(szTemp," ",NULL));

    for (int i = 0; i < n_Masters; i++) {
        if (strcmp(szTemp,MyMasters[i]) == 0) {

            // Borrowed from Fibby of MQ2MasterMind
            CHAR TextString[MAX_STRING];
            PSTR Text;
            Text = GetNextArg(Line,1,FALSE,'\'');

            if (Text[0] == '/') {
                strcpy_s(TextString,Text);
                TextString[strlen(Text)-1] = '\0';

                DoCommand((PSPAWNINFO)pCharSpawn,TextString);
            }
        }
    }
    return 0;
}