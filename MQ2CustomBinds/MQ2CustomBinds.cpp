// MQ2CustomBinds.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.



#include "../MQ2Plugin.h"


PreSetup("MQ2CustomBinds");


typedef struct _CustomBind {
    CHAR Name[32];
    CHAR CommandDown[MAX_STRING];
    CHAR CommandUp[MAX_STRING];
} CUSTOMBIND, *PCUSTOMBIND;

CIndex<PCUSTOMBIND> CustomBinds(10);

VOID ExecuteCustomBind(PCHAR Name,BOOL Down);

int FindCustomBind(PCHAR Name)
{
    for (unsigned long N = 0 ; N < CustomBinds.Size ; N++)
        if (PCUSTOMBIND pBind=CustomBinds[N])
        {
            if (!_stricmp(Name,pBind->Name))
                return N;
        }
        return -1;
}

PCUSTOMBIND AddCustomBind(PCHAR Name, PCHAR CommandDown, PCHAR CommandUp)
{
    if (AddMQ2KeyBind(Name,ExecuteCustomBind))
    {
        PCUSTOMBIND pBind = new CUSTOMBIND;
        ZeroMemory(pBind,sizeof(CUSTOMBIND));
        strcpy_s(pBind->Name,Name);
        if (CommandDown)
            strcpy_s(pBind->CommandDown,CommandDown);
        if (CommandUp)
            strcpy_s(pBind->CommandUp,CommandUp);
        CustomBinds+=pBind;
        return pBind;
    }
    return 0;
}

VOID RemoveCustomBind(unsigned long N)
{
    if (N>=CustomBinds.Size)
        return;
    if (PCUSTOMBIND pBind=CustomBinds[N])
    {
        RemoveMQ2KeyBind(pBind->Name);
        delete pBind;
        CustomBinds[N]=0;
    }
}

VOID LoadCustomBinds();
VOID CustomBindCmd(PSPAWNINFO pChar, PCHAR szLine);
VOID SaveCustomBinds();

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
    DebugSpewAlways("Initializing MQ2CustomBinds");

    AddCommand("/custombind",CustomBindCmd,0,0,0);
    // Add commands, macro parameters, hooks, etc.
    // AddCommand("/mycommand",MyCommand);
    // AddParm("$myparm(x)",MyParm);
    // AddXMLFile("MQUI_MyXMLFile.xml");
    // bmMyBenchmark=AddMQ2Benchmark("My Benchmark Name");
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
    DebugSpewAlways("Shutting down MQ2CustomBinds");
    RemoveCommand("/custombind");
    //SaveCustomBinds();

    for (unsigned long N = 0 ; N < CustomBinds.Size ; N++)
        if (PCUSTOMBIND pBind=CustomBinds[N])
        {
            RemoveMQ2KeyBind(pBind->Name);
        }
        CustomBinds.Cleanup();
        // Remove commands, macro parameters, hooks, etc.
        // RemoveMQ2Benchmark(bmMyBenchmark);
        // RemoveParm("$myparm(x)");
        // RemoveCommand("/mycommand");
        // RemoveXMLFile("MQUI_MyXMLFile.xml");
}

PLUGIN_API VOID SetGameState(DWORD GameState)
{
    static bool BindsLoaded=false;
    if (GameState==GAMESTATE_INGAME || GameState==GAMESTATE_CHARSELECT)
    {
        if (!BindsLoaded)
        {
            LoadCustomBinds();
            BindsLoaded=true;
        }
    }
}

/* Config file Data Access Abstraction
 *
 * Abstraction is on Load only ( between INI and TXT config files )
 * provision for the standard INI introduced to read and write configuration.
 * original TXT load amended to write back a note that the file is replaced by INI files.
 *
 */

void WriteRetiredHeaderToTXT(const char* filename)
{
    constexpr char* Asterisks = "* * * * * * * * * *";
    constexpr char* RetiredMsg = "This TXT file is replaced by the INI file.\nYou can safely delete this TXT file.";

    void* data = nullptr;
    struct stat fileStat;
    FILE *file = nullptr;

    errno_t err = stat(filename, &fileStat);
    if (err)
        return;

    err = fopen_s(&file, filename, "rt");
    if (err)
        return;

    data = malloc(fileStat.st_size);
    if (data) {
        int length = fread(data, 1, fileStat.st_size, file);
        freopen_s(&file, filename, "wt", file);
        fprintf(file, "%s\n%s\n%s\n", Asterisks, RetiredMsg, Asterisks);
        fwrite(data, 1, length, file);
        free(data);
    }
    fclose(file);
}

bool LoadCustomBindsTXT()
{
    CHAR filename[MAX_STRING];
    strcpy_s(filename,gszINIPath);
    strcat_s(filename,"\\MQ2CustomBinds.txt");
    FILE *file = nullptr;
    errno_t err = fopen_s(&file,filename, "rt");
    if (err)
        return false;
    CUSTOMBIND NewBind;
    ZeroMemory(&NewBind,sizeof(CUSTOMBIND));
    CHAR szLine[MAX_STRING];

    while(fgets(szLine,2048,file))
    {
        char *Next_Token1 = 0;
        char *Next_Token2 = 0;
        char *Cmd = strtok_s(szLine,"\r\n",&Next_Token1);
        char *Cmd2 = strtok_s(Cmd,"=",&Next_Token2);
        if (!_stricmp(Cmd2,"name"))
        {
            ZeroMemory(&NewBind,sizeof(CUSTOMBIND));
            strcpy_s(NewBind.Name,&szLine[5]);
        }
        else if (!_stricmp(Cmd2,"up"))
        {
            strcpy_s(NewBind.CommandUp,&szLine[3]);
            AddCustomBind(NewBind.Name,NewBind.CommandDown,NewBind.CommandUp);
        }
        else if (!_stricmp(Cmd2,"down"))
        {
            strcpy_s(NewBind.CommandDown,&szLine[5]);
        }
    }

    fclose(file);

    //place txt file retired header into old txt file
    WriteRetiredHeaderToTXT(filename);

    return true;
}

bool LoadCustomBindsINI()
{
    char names[MAX_STRING] = { 0 };

    CUSTOMBIND NewBind;

    //names will get populated with a series of null term strings, and ends with another null.
    GetPrivateProfileSectionNames(names, sizeof(names), INIFileName);
    char* pstr = names;

    if (pstr[0] == 0)
        return false;

    while (pstr[0] != 0) {
        ZeroMemory(&NewBind, sizeof(CUSTOMBIND));
        strcpy_s(NewBind.Name, pstr);

        GetPrivateProfileString(NewBind.Name, "up", "", NewBind.CommandUp, sizeof(NewBind.CommandUp), INIFileName);
        GetPrivateProfileString(NewBind.Name, "down", "", NewBind.CommandDown, sizeof(NewBind.CommandDown), INIFileName);
        AddCustomBind(NewBind.Name, NewBind.CommandDown, NewBind.CommandUp);

        pstr += strlen(pstr) + 1;
    }
    return true;
}

VOID SaveCustomBinds()
{
    //truncate the file
    std::remove(INIFileName);

    for (unsigned long N = 0; N < CustomBinds.Size; N++) {
        if (PCUSTOMBIND pBind = CustomBinds[N])
        {
            WritePrivateProfileString(pBind->Name, "down", pBind->CommandDown, INIFileName);
            WritePrivateProfileString(pBind->Name, "up", pBind->CommandUp, INIFileName);
        }
    }
    WriteChatColor("Custom binds saved to ini");
}

// Load accessor method, to try ini first, fall back to txt.
// if txt is used, save immediate.
VOID LoadCustomBinds()
{
    if (LoadCustomBindsINI()) {
        WriteChatColor("Custom binds loaded from ini");
    }
    else if (LoadCustomBindsTXT()) {
        WriteChatColor("Custom binds loaded from txt");
        // save to ini, for use going forward.
        SaveCustomBinds();
    }
}

/* End of Config file Data Access Abstraction.
 */


VOID ExecuteCustomBind(PCHAR Name,BOOL Down)
{
    int N=FindCustomBind(Name);
    if (N<0)
        return;
    PCHARINFO pCharInfo=GetCharInfo();
    if (!pCharInfo)
        return;
    if (PCUSTOMBIND pBind=CustomBinds[N])
    {
        if (Down)
        {
            if (pBind->CommandDown[0])
                DoCommand(pCharInfo->pSpawn,pBind->CommandDown);
        }
        else if (pBind->CommandUp[0])
            DoCommand(pCharInfo->pSpawn,pBind->CommandUp);
    }
}

VOID CustomBindCmd(PSPAWNINFO pChar, PCHAR szLine)
{
    if (szLine[0]==0)
    {
        SyntaxError("Usage: /custombind <list|add <name>|delete <name>|clear <name><-down|-up>|set <name><-down|-up> <command>>");
        return;
    }
    CHAR szBuffer[MAX_STRING];
    CHAR szArg[MAX_STRING] = {0};
    CHAR szArg2[MAX_STRING] = {0};
    GetArg(szArg,szLine,1);
    GetArg(szArg2,szLine,2);
    PCHAR szRest = GetNextArg(szLine,2);

    if (!_stricmp(szArg,"list"))
    {
        WriteChatColor("Custom binds");
        WriteChatColor("--------------");
        for (unsigned long N = 0 ; N < CustomBinds.Size ; N++)
            if (PCUSTOMBIND pBind=CustomBinds[N])
            {
                sprintf_s(szBuffer,"[\ay%s\ax] [Down:\at%s\ax] [Up:\at%s\ax]",pBind->Name,pBind->CommandDown,pBind->CommandUp);
                WriteChatColor(szBuffer);
            }
            WriteChatColor("--------------");
            WriteChatColor("End custom binds");
            return;
    }
    if (!_stricmp(szArg,"add"))
    {
        if (strchr(szArg2,'-'))
        {
            WriteChatColor("'-' is not allowed in a custom bind name");
        }
        if (PCUSTOMBIND pBind=AddCustomBind(szArg2,0,0))
        {
            WriteChatColor("Custom bind added.  Use /custombind set to set the custom commands.");
        }
        else
        {
            WriteChatColor("Failed to add custom bind (name in use)");
        }
        return;
    }
    if (!_stricmp(szArg,"delete"))
    {
        int N=FindCustomBind(szArg2);
        if (N>=0)
        {
            RemoveCustomBind(N);
            WriteChatColor("Custom bind deleted");
        }
        else
        {
            WriteChatColor("Could not find custom bind with that name to delete");
        }
        return;
    }
    if (!_stricmp(szArg,"set"))
    {
        BOOL Down=true;
        if (PCHAR minus=strchr(szArg2,'-'))
        {
            minus[0]=0;
            if (!_stricmp(&minus[1],"up"))
                Down=false;
        }
        int N=FindCustomBind(szArg2);
        if (N<0)
        {
            sprintf_s(szBuffer,"Could not find custom bind '%s'",szArg2);
            WriteChatColor(szBuffer);
            return;
        }
        PCUSTOMBIND pBind=CustomBinds[N];
        if (Down)
            strcpy_s(pBind->CommandDown,szRest);
        else
            strcpy_s(pBind->CommandUp,szRest);
        sprintf_s(szBuffer,"[\ay%s\ax] [Down:\at%s\ax] [Up:\at%s\ax]",pBind->Name,pBind->CommandDown,pBind->CommandUp);
        WriteChatColor(szBuffer);
        SaveCustomBinds();
        return;
    }
    if (!_stricmp(szArg,"clear"))
    {
        BOOL Down=true;
        if (PCHAR minus=strchr(szArg2,'-'))
        {
            minus[0]=0;
            if (!_stricmp(&minus[1],"up"))
                Down=false;
        }
        int N=FindCustomBind(szArg2);
        if (N<0)
        {
            sprintf_s(szBuffer,"Could not find custom bind '%s'",szArg2);
            WriteChatColor(szBuffer);
            return;
        }
        PCUSTOMBIND pBind=CustomBinds[N];
        if (Down)
            pBind->CommandDown[0]=0;
        else
            pBind->CommandUp[0]=0;
        sprintf_s(szBuffer,"[\ay%s\ax] [Down:\at%s\ax] [Up:\at%s\ax]",pBind->Name,pBind->CommandDown,pBind->CommandUp);
        WriteChatColor(szBuffer);
        SaveCustomBinds();
        return;
    }
}
