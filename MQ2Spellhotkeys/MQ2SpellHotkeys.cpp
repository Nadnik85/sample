//    MQ2SpellHotkeys.cpp : Defines the entry point for the DLL application. 
//    Author: Relkin, Caladine, Quixote398 
//    Version: 1.4.4 
//    Please see readme_mq2spellhotkeys.txt for more information 
//    Usage:    /autohotkey < on | off | list <...> | reload | addchat <chatname> | remchat <chatname> | setpopups <...> | save  > 
//              /autohotkey list < all | state | chats | popups | hotkeys > 
//              /autohotkey setpopus < all | NameOfPopUp > < TRUE | FALSE > 

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time 
// are shown below. Remove the ones your plugin does not use.  Always use Initialize 
// and Shutdown for setup and cleanup, do NOT do it in DllMain. 


#include "../MQ2Plugin.h"
using namespace std;
#include <list> 
#include <iterator> 
#include <ctype.h> 
#define BUFF_SIZE            100    // Max length of buffer for text strings (such as hotkeys...) 
#define CHAT_BUFF_SIZE       20     // Length of specified chat name where to send text 
#define SHOW_INI_TEXT        0      // 1 = Show debug text from loading ini 
                                    // 0 = Don't 
PreSetup("MQ2SpellHotkeys"); 

// My own spell structure for storage 
struct SpellStruct { 
    CHAR    Name[BUFF_SIZE]; 
    BOOL    DoPopUps; 
    list    <CHAR *> OnCast, OnHit, OnResist;  // list of Hotkeys for 3 conditions of spells 
}; 

// Enum for pop up fields 
enum popupflags {OnHit, OnResist, OnInterrupt, OnOutOfRange, OnWillNotHold, OnWornOff}; 
#define NUM_POPUP_FLAGS     6       // Keep set to number of flags in previous line. 

// Function Declarations 
VOID AutoHotkey(PSPAWNINFO pChar, CHAR *szLine); 
VOID List(CHAR *szLine); 
VOID AddHotkeyChat(CHAR *szLine); 
VOID RemHotkeyChat(CHAR *szLine); 
VOID ReloadHotkeys(); 
VOID SaveHotkeySettings(); 
VOID ConvertChat(PCHAR dst,size_t dstSize, CHAR *src); 
VOID QueueMessages(list <CHAR *> *dstq, list <CHAR *> *srcq); 
VOID ListChats(); 
VOID ListPopUps(); 
VOID ListHotkeys(); 
VOID SetPopUps(CHAR *szLine); 
template <unsigned int _Size>CHAR *BoolToStr(BOOL b, CHAR(&dstp)[_Size]);

// Global Variables 
BOOL auto_on = FALSE; 
BOOL am_casting = FALSE; 
BOOL missed = FALSE; 
BOOL popups[NUM_POPUP_FLAGS] = {FALSE}; 
CHAR spellname[BUFF_SIZE] = {0}; 
list <SpellStruct *> spells; 
list <SpellStruct *>::iterator spitr; 
list <CHAR *> chat; 
list <CHAR *> output_queue; 
list <CHAR *> hit_queue; 

// Called once, when the plugin is to initialize 
PLUGIN_API VOID InitializePlugin(VOID) 
{ 
    // Add commands, MQ2Data items, hooks, etc. 
    AddCommand("/autohotkey",AutoHotkey); 
    AddAlias("/ahk","/autohotkey"); 
} 

// Called once, when the plugin is to shutdown 
PLUGIN_API VOID ShutdownPlugin(VOID) 
{ 
    RemoveCommand("/autohotkey"); 
    RemoveAlias("/ahk"); 
    // Not totally sure if list will automatically free up all that stuff I allocated for it. 
    for (spitr = spells.begin(); spitr != spells.end() ; spitr++) { 
        (*spitr)->OnHit.clear(); 
        (*spitr)->OnCast.clear(); 
        (*spitr)->OnResist.clear(); 
    } 
    spells.clear(); 
    chat.clear(); 
    output_queue.clear(); 
} //end ShutdownPlugin 

// AutoHotkey - handles parsing commands and simple on/off 
VOID AutoHotkey(PSPAWNINFO pChar, CHAR *szLine) 
{ 
    CHAR text[MAX_STRING] = {0}; 

    // Get first arg and check command 
    GetArg(text, szLine, 1); 
    if (_stricmp(text, "on") == 0) { 
        auto_on = TRUE; 
        WriteChatColor("AutoHotkey is now ON", CONCOLOR_YELLOW); 
    } else if (_stricmp(text, "off") == 0) { 
        auto_on = FALSE; 
        WriteChatColor("AutoHotkey is now OFF", CONCOLOR_YELLOW); 
    } else if (_stricmp(text, "list") == 0) { 
        List(szLine); 
    } else if (_stricmp(text, "reload") == 0) { 
        ReloadHotkeys(); 
        WriteChatColor("Finished loading.", CONCOLOR_YELLOW); 
    } else if (_strnicmp(text, "addchat", 7) == 0) { 
        AddHotkeyChat(szLine); 
    } else if (_strnicmp(text, "remchat", 7) == 0) { 
        RemHotkeyChat(szLine); 
    } else if (_stricmp(text, "save") == 0) { 
        SaveHotkeySettings(); 
    } else if (_stricmp(text, "setpopups") == 0) { 
        SetPopUps(szLine); 
    } else { // No valid command so report usage 
        WriteChatColor("Usage: /autohotkey < on | off | list <...> | reload | addchat <chatname> | remchat <chatname> | setpopups <...> | save  >", CONCOLOR_YELLOW); 
    } 
} //end AutoHotkey 

// Called once directly after initialization, and then every time the gamestate changes 
PLUGIN_API VOID SetGameState(DWORD GameState) 
{ 
    if (GameState==GAMESTATE_INGAME) { 
        if (GetCharInfo()->pSpawn != NULL) { 
            ReloadHotkeys(); 
        } 
    } 
} //end SetGameState 

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat, 
// but after MQ filters and chat events are taken care of. 
PLUGIN_API DWORD OnIncomingChat(CHAR *Line, DWORD Color) 
{ 
    CHAR text[MAX_STRING] = "UNKNOWN"; 
    PSPELL cspell; 

    // Is auto_on = True and am I not NULL 
    if (auto_on && (GetCharInfo()->pSpawn != NULL)) { 
        if (strstr(Line, "You begin casting") != NULL) { 
            am_casting = TRUE; 
            output_queue.clear(); 
            // For some reason, GetSpellByID can come back null even though we just started casting. 
            // So lets check for a valid ID first. 
            if (GetCharInfo()->pSpawn->CastingData.SpellID != -1) { 
                //Find what spell is being cast. 
                cspell = GetSpellByID(GetCharInfo()->pSpawn->CastingData.SpellID); 
                strcpy_s(spellname, cspell->Name); 
                // Now for searching all the spells 
                for (spitr = spells.begin(); spitr != spells.end() ; spitr++) { 
                    if (_stricmp((*spitr)->Name, spellname) == 0) { 
                        // Found a match, queue messages for OnCast and OnHit 
                        QueueMessages(&output_queue, &((*spitr)->OnCast)); 
                        QueueMessages(&hit_queue, &((*spitr)->OnHit)); 
                    } 
                } 
                missed = FALSE; 
            } else { 
                //WriteChatColor("Unable to determine spell being cast in OnIncomingChat.", CONCOLOR_RED); 
                missed = TRUE; 
            } 
        } 
        // Now , check for various ending spellcasting messages 
        if (strstr(Line, "Your target resisted the") != NULL) { 
            // Was a resist 
            output_queue.clear(); 
            for (spitr = spells.begin(); spitr != spells.end() ; spitr++) { 
                if (_stricmp((*spitr)->Name, spellname) == 0) { 
                    // Found a match, queue messages for OnResist 
                    QueueMessages(&output_queue, &(*spitr)->OnResist); 
                    if (((*spitr)->DoPopUps) && popups[OnResist]) { 
                        sprintf_s(text, "%s has been resisted!", spellname); 
                        DisplayOverlayText(text, CONCOLOR_RED, 100, 50, 500, 3000); 
                    } 
                } 
            } 
            // Clean up 
            strcpy_s(spellname, ""); // Clear spellname 
            hit_queue.clear(); // remember to clear hit queue as well 
            am_casting = FALSE; // no longer casting 
            missed = FALSE; 
        } else if ((strstr(Line, "Your ") != NULL && strstr(Line, " spell is interrupted.") != NULL) ||
                (strstr(Line, "Your casting has been interrupted!") != NULL)) { 
            // Not casting anymore 
            // Possible customization here. 
            // User could maybe send messages to him self to notify of an interrupt? 
            if (popups[OnInterrupt]) { 
                for (spitr = spells.begin(); spitr != spells.end() ; spitr++) { 
                    if (_stricmp((*spitr)->Name, spellname) == 0) { 
                        if ((*spitr)->DoPopUps) { 
                            sprintf_s(text, "%s has been interrupted", spellname); 
                            DisplayOverlayText(text, CONCOLOR_RED, 100, 50, 500, 3000); 
                        } 
                    } 
                } 
            } 
            // Clean up 
            strcpy_s(spellname, ""); // Clear spellname 
            output_queue.clear(); 
            hit_queue.clear(); // remember to clear hit queue as well 
            am_casting = FALSE; // no longer casting 
            missed = FALSE; 
        } else if (strstr(Line, "Your spell did not take hold.") != NULL) { 
            // Didn't hold? dont' want to report we hit then... 
            if (popups[OnWillNotHold]) { 
                for (spitr = spells.begin(); spitr != spells.end() ; spitr++) { 
                    if (_stricmp((*spitr)->Name, spellname) == 0) { 
                        if ((*spitr)->DoPopUps) { 
                            sprintf_s(text, "%s will not hold", spellname); 
                            DisplayOverlayText(text, CONCOLOR_RED, 100, 50, 500, 3000); 
                        } 
                    } 
                } 
            } 
            // Clean up 
            strcpy_s(spellname, ""); // Clear spellname 
            output_queue.clear(); 
            hit_queue.clear(); // remember to clear hit queue as well 
            am_casting = FALSE; // no longer casting 
            missed = FALSE; 
        } else if (strstr(Line, "Your target is out of range, get closer!") != NULL) { 
            // Out of range? dont' want to report we hit then... 
            if (popups[OnOutOfRange]) { 
                for (spitr = spells.begin(); spitr != spells.end() ; spitr++) { 
                    if (_stricmp((*spitr)->Name, spellname) == 0) { 
                        if ((*spitr)->DoPopUps) { 
                            DisplayOverlayText("Target has moved out of range!", CONCOLOR_RED, 100, 50, 500, 3000); 
                        } 
                    } 
                } 
            } 
            // Clean up 
            strcpy_s(spellname, ""); // Clear spellname 
            output_queue.clear(); 
            hit_queue.clear(); // remember to clear hit queue as well 
            am_casting = FALSE; // no longer casting 
            missed = FALSE; 
        } else if (strstr(Line, "spell has worn off") != NULL) { 
            // This is well after done casting, need to find name all over again. 
			CHAR temp[BUFF_SIZE] = { 0 };
            INT x = 5; // Start off pointing after "Your " 

            while ( _strnicmp(&Line[x], "spell", 5) != 0 ) x++; 
            // Index 5 through x-6 should be the spell name. 
            strncpy_s(temp, &Line[5], (x-6)); 
            if (popups[OnWornOff]) { 
                for (spitr = spells.begin(); spitr != spells.end() ; spitr++) { 
                    if (_stricmp((*spitr)->Name, temp) == 0) { 
                        if ((*spitr)->DoPopUps) { 
                            sprintf_s(text, "%s has worn off.", temp); 
                            DisplayOverlayText(text, CONCOLOR_YELLOW, 100, 50, 500, 3000); 
                        } 
                    } 
                } 
            } 
        } 
    } 

    return 0; 
} //end OnIncomingChat 

// This is called every time MQ pulses
CHAR text[MAX_STRING] = {0}; 
PLUGIN_API VOID OnPulse(VOID) 
{ 
	if (GetGameState() == GAMESTATE_INGAME) {
		if (PCHARINFO pChar = GetCharInfo()) {
			if (missed) {
				// For some reason, GetSpellByID can come back null even though we just started casting. 
				if (pChar->pSpawn && pChar->pSpawn->CastingData.SpellID != -1) {
					//Find what spell is being cast. 
					if (PSPELL cspell = GetSpellByID(pChar->pSpawn->CastingData.SpellID)) {
						strcpy_s(spellname, cspell->Name);
						// Now for searching all the spells 
						for (spitr = spells.begin(); spitr != spells.end(); spitr++) {
							if (_stricmp((*spitr)->Name, spellname) == 0) {
								// Found a match, queue messages for OnCast and OnHit 
								QueueMessages(&output_queue, &((*spitr)->OnCast));
								QueueMessages(&hit_queue, &((*spitr)->OnHit));
							}
						}
					}
					missed = FALSE;
				}
			}
			// Now for outputing messages, one per pulse if we have any. 
			if (!output_queue.empty()) {
				// Something there to send. Formating should be in already. 
				DoCommand(pChar->pSpawn, output_queue.front());
				output_queue.pop_front();
			}
			else if (!hit_queue.empty()) {
				if (pChar->pSpawn->CastingData.SpellETA == 0) {
					// Spell should have landed and haven't noticed any interrupts. 
					am_casting = FALSE;
					DoCommand(pChar->pSpawn, hit_queue.front());
					hit_queue.pop_front();
					if ((popups[OnHit]) && hit_queue.empty()) {
						for (spitr = spells.begin(); spitr != spells.end(); spitr++) {
							if (_stricmp((*spitr)->Name, spellname) == 0) {
								if ((*spitr)->DoPopUps) {
									sprintf_s(text, "%s was successful.", spellname);
									DisplayOverlayText(text, COLOR_PURPLE, 100, 50, 500, 3000);
								}
							}
						}
					}
					if (hit_queue.empty()) strcpy_s(spellname, "");
				}
			}
		}
	}
} //end OnPulse 

// Lists requested info 
VOID List(CHAR *szLine) 
{ 
    CHAR text[MAX_STRING] = {0}; 

    GetArg(text, szLine, 2); 
    if (_stricmp(text, "state") == 0) { 
        // print out the auto_on state 
        if (auto_on) strcpy_s(text, "Auto Hotkey = ON"); 
        else strcpy_s(text, "Auto Hotkey = OFF"); 
        WriteChatColor(text, CONCOLOR_YELLOW); 
    } else if (_stricmp(text, "chats") == 0) { 
        ListChats(); 
        WriteChatColor("End of chats list.", CONCOLOR_YELLOW); 
    } else if (_stricmp(text, "popups") == 0) { 
        ListPopUps(); 
        WriteChatColor("End of popups list.", CONCOLOR_YELLOW); 
    } else if (_stricmp(text, "hotkeys") == 0) { 
        ListHotkeys(); 
        WriteChatColor("End of hotkeys list.", CONCOLOR_YELLOW); 
    } else if (_stricmp(text, "all") == 0) { 
        if (auto_on) strcpy_s(text, "Auto Hotkey = ON"); 
        else strcpy_s(text, "Auto Hotkey = OFF"); 
        WriteChatColor(text, CONCOLOR_YELLOW); 
        ListChats(); 
        ListPopUps(); 
        ListHotkeys(); 
        WriteChatColor("End of list all.", CONCOLOR_YELLOW); 
    } else { 
        WriteChatColor("Usage: /autohotkey list < all | state | chats | popups | hotkeys >", CONCOLOR_YELLOW); 
    } 
} //end ListHotkeys 

// List chats 
VOID ListChats() 
{ 
    CHAR text[MAX_STRING] = {0}; 
    INT x = 0; 
    list <CHAR *>::iterator hkitr; 

    for (hkitr = chat.begin(), x = 0; hkitr != chat.end(); hkitr++, x++) { 
        sprintf_s(text, "Chat %d: %s", x, (*hkitr)); 
        WriteChatColor(text, CONCOLOR_YELLOW); 
    } 

} //end ListChats 

VOID ListPopUps() 
{ 
    if ( popups[OnHit] || popups[OnResist] || popups[OnInterrupt] || popups[OnOutOfRange] || 
         popups[OnWillNotHold] || popups[OnWornOff]  ) { 
        // do stuff 
        if ( popups[OnHit] )    WriteChatColor("PopUps OnHit enabled.", CONCOLOR_YELLOW); 
        else                    WriteChatColor("PopUps OnHit disabled.", CONCOLOR_YELLOW); 
        if ( popups[OnResist] ) WriteChatColor("PopUps OnResist enabled.", CONCOLOR_YELLOW); 
        else                    WriteChatColor("PopUps OnResist disabled.", CONCOLOR_YELLOW); 
        if ( popups[OnInterrupt] )  WriteChatColor("PopUps OnInterrupt enabled.", CONCOLOR_YELLOW); 
        else                        WriteChatColor("PopUps OnInterrupt disabled.", CONCOLOR_YELLOW); 
        if ( popups[OnOutOfRange] ) WriteChatColor("PopUps OnOutOfRange enabled.", CONCOLOR_YELLOW); 
        else                        WriteChatColor("PopUps OnOutOfRange disabled.", CONCOLOR_YELLOW); 
        if ( popups[OnWillNotHold] )    WriteChatColor("PopUps OnWillNotHold enabled.", CONCOLOR_YELLOW); 
        else                            WriteChatColor("PopUps OnWillNotHold disabled.", CONCOLOR_YELLOW); 
        if ( popups[OnWornOff] )    WriteChatColor("PopUps OnWornOff enabled.", CONCOLOR_YELLOW); 
        else                        WriteChatColor("PopUps OnWornOff disabled.", CONCOLOR_YELLOW); 
    } else { 
        WriteChatColor("All popups disabled.", CONCOLOR_YELLOW); 
    } 
} //end ListPopUps 

VOID ListHotkeys() 
{ 
    CHAR text[MAX_STRING] = {0}; 
    INT x = 0; 
    list <CHAR *>::iterator hkitr; 

    for (spitr = spells.begin(); spitr != spells.end() ; spitr++) { 
        if ((*spitr)->DoPopUps) sprintf_s(text, "Spell: %s - PopUps Enabled - Hotkeys:", (*spitr)->Name); 
        else sprintf_s(text, "Spell: %s - PopUps Disabled - Hotkeys:", (*spitr)->Name); 
        WriteChatColor(text, CONCOLOR_YELLOW); 
        // Now for all the hotkeys associated with this spell 
        for (hkitr = (*spitr)->OnCast.begin(), x = 0; hkitr != (*spitr)->OnCast.end(); hkitr++, x++) { 
            sprintf_s(text, "On Cast %d: %s", x, (*hkitr)); 
            WriteChatColor(text, CONCOLOR_GREEN); 
        } 
        for (hkitr = (*spitr)->OnResist.begin(), x = 0; hkitr != (*spitr)->OnResist.end(); hkitr++, x++) { 
            sprintf_s(text, "On Resist %d: %s", x, (*hkitr)); 
            WriteChatColor(text, CONCOLOR_GREEN); 
        } 
        for (hkitr = (*spitr)->OnHit.begin(), x = 0; hkitr != (*spitr)->OnHit.end(); hkitr++, x++) { 
            sprintf_s(text, "On Hit %d: %s", x, (*hkitr)); 
            WriteChatColor(text, CONCOLOR_GREEN); 
        } 
    } 

} //end ListHotkeys 

// Reloads the ini file 
VOID ReloadHotkeys() 
{ 
    CHAR reply[BUFF_SIZE] = {0}; 
    CHAR text[MAX_STRING] = {0}, *temp; 
    CHAR names[MAX_STRING] = {0}, keys[MAX_STRING] = {0}, chatlist[MAX_STRING] = {0}; 
    CHAR *ptrsec, *ptrkey, *ptrchat; 
    CHAR filename[MAX_STRING] = {0}; 
    INT x = 0; 
    SpellStruct *a_spell; 

    // Set up filename 
    sprintf_s(filename, "%s%chotkeys_%s_%s.ini", gszINIPath, '\\', GetCharInfo()->Name, EQADDR_SERVERNAME); 

    // Clear current lists! 
    for (spitr = spells.begin(); spitr != spells.end() ; spitr++) { 
        (*spitr)->OnHit.clear(); 
        (*spitr)->OnCast.clear(); 
        (*spitr)->OnResist.clear(); 
    } 
    spells.clear(); 
    chat.clear(); 
    // Now, get all the key names from the default section, should be 1 for auto_on and unknown chat(s) channels 
    GetPrivateProfileString("DEFAULT", NULL, "UNDEFINED", chatlist, sizeof(chatlist), filename); 
    ptrchat = chatlist; 
    while (ptrchat[0] != NULL) { 
        // Get the channel name to use from .ini. 
        GetPrivateProfileString("DEFAULT", ptrchat, "UNDEFINED", reply, sizeof(reply), filename); 
        if (_stricmp(ptrchat, "auto_on") == 0) { 
            // Auto_on setting 
            if (_stricmp(reply, "true") == 0)            auto_on = TRUE; 
            else                                        auto_on = FALSE; 
#if SHOW_INI_TEXT 
            strcpy_s(text, "Autohotkey is "); 
            if (auto_on)        strcat(text, "ON"); 
            else                strcat(text, "OFF"); 
            WriteChatColor(text, CONCOLOR_YELLOW); 
#endif 
        } else if (_strnicmp(ptrchat, "chat", 4) == 0) { 
            // Convert chat into an actual command 
            temp = new CHAR[CHAT_BUFF_SIZE]; 
            ConvertChat(temp,CHAT_BUFF_SIZE, reply); 
            chat.push_back(temp); 
#if SHOW_INI_TEXT 
            sprintf_s(text, "Chat: %s", temp); 
            WriteChatColor(text, CONCOLOR_YELLOW); 
#endif 
        } 
        // Skip over this one now 
        ptrchat += strlen(ptrchat) + 1; 
    } 
    // Now to read in PopUps flags 
    GetPrivateProfileString("PopUps", "OnHit", "UNDEFINED", reply, sizeof(reply), filename); 
    if (_stricmp(reply, "TRUE") == 0)    popups[OnHit] = TRUE; 
    else                                popups[OnHit] = FALSE; 
    GetPrivateProfileString("PopUps", "OnResist", "UNDEFINED", reply, sizeof(reply), filename); 
    if (_stricmp(reply, "TRUE") == 0)    popups[OnResist] = TRUE; 
    else                                popups[OnResist] = FALSE; 
    GetPrivateProfileString("PopUps", "OnInterrupt", "UNDEFINED", reply, sizeof(reply), filename); 
    if (_stricmp(reply, "TRUE") == 0)    popups[OnInterrupt] = TRUE; 
    else                                popups[OnInterrupt] = FALSE; 
    GetPrivateProfileString("PopUps", "OnOutOfRange", "UNDEFINED", reply, sizeof(reply), filename); 
    if (_stricmp(reply, "TRUE") == 0)    popups[OnOutOfRange] = TRUE; 
    else                                popups[OnOutOfRange] = FALSE; 
    GetPrivateProfileString("PopUps", "OnWillNotHold", "UNDEFINED", reply, sizeof(reply), filename); 
    if (_stricmp(reply, "TRUE") == 0)    popups[OnWillNotHold] = TRUE; 
    else                                popups[OnWillNotHold] = FALSE; 
    GetPrivateProfileString("PopUps", "OnWornOff", "UNDEFINED", reply, sizeof(reply), filename); 
    if (_stricmp(reply, "TRUE") == 0)    popups[OnWornOff] = TRUE; 
    else                                popups[OnWornOff] = FALSE; 

    // ok so first, get the section headings.  Ignoring default, read in the rest. 
    // Need a dynamicly declared SpellStruct for each new section and push onto the list 
    GetPrivateProfileSectionNames(names, sizeof(names), filename); 
    ptrsec = names; 
    while (ptrsec[0] != NULL) { 
        // Skip if this is the Default or PopUps sections 
        if ((_stricmp(ptrsec, "DEFAULT") == 0) || (_stricmp(ptrsec, "POPUPS") == 0)) { 
            // Skip over this one now 
            ptrsec += strlen(ptrsec) + 1; 
            continue; 
        } 
        a_spell = new SpellStruct; 
        strcpy_s(a_spell->Name, ptrsec); 
#if SHOW_INI_TEXT 
        sprintf_s(text, "Spell: %s", ptrsec); 
        WriteChatColor(text, CONCOLOR_YELLOW); 
#endif 
        // Got the next name, time to find the hotkeys 
        GetPrivateProfileString(a_spell->Name, NULL, "UNDEFINED", keys, sizeof(keys), filename); 
        ptrkey = keys; 
        while (ptrkey[0] != NULL) { 
            temp = new CHAR[BUFF_SIZE]; 
            GetPrivateProfileString(ptrsec, ptrkey, "UNDEFINED", temp, BUFF_SIZE, filename); 
#if SHOW_INI_TEXT 
            sprintf_s(text, "Hotkey: %s : %s", ptrkey, temp); 
            WriteChatColor(text, CONCOLOR_GREEN); 
#endif 
            // Find category to put it in 
            if (_strnicmp(ptrkey, "OnHit", 5) ==0 )            a_spell->OnHit.push_back(temp); 
            else if (_strnicmp(ptrkey, "OnCast", 6) ==0 )      a_spell->OnCast.push_back(temp); 
            else if (_strnicmp(ptrkey, "OnResist", 8) ==0 )    a_spell->OnResist.push_back(temp); 
            else { 
                if (_stricmp(ptrkey, "DoPopUps") == 0) { 
                    if (_stricmp(temp, "TRUE") == 0) a_spell->DoPopUps = TRUE; 
                    else a_spell->DoPopUps = FALSE; 
                } 
                // If we miss all categories need to delete the memory ourselves. 
                delete temp; 
            }    
            // Now skip over this key and go to next 
            ptrkey += strlen(ptrkey) + 1; 
        } 
        spells.push_back(a_spell); 
        // Skip over this one now 
        ptrsec += strlen(ptrsec) + 1; 
    } 
} //end ReloadHotkeys 

// Adds the hotkey chat to list 
VOID AddHotkeyChat(CHAR *szLine) 
{ 
    CHAR text[MAX_STRING] = {0}, *temp; 

    GetArg(text, szLine, 2); 
    if ( strcmp(text, "") != 0 ) { 
        temp = new CHAR[CHAT_BUFF_SIZE]; 
        ConvertChat(temp, CHAT_BUFF_SIZE, text); 
        chat.push_back(temp); 
        sprintf_s(text, "Chat channel added: %s", temp); 
        WriteChatColor(text, CONCOLOR_YELLOW); 
    } else { 
        // Report usage 
        WriteChatColor("Usage: /autohotkey addchat <chatname>", CONCOLOR_YELLOW); 
    } 
} //end AddHotkeyChat 


// Remove the hotkey chat from the list 
VOID RemHotkeyChat(CHAR *szLine) 
{ 
    CHAR text[MAX_STRING] = {0}, temp[MAX_STRING] = {0}; 
    list <CHAR *>::iterator chitr; 

    GetArg(temp, szLine, 2); 
    if ( strcmp(text, "") != 0 ) { 
        ConvertChat(text, MAX_STRING, temp); 
        for (chitr = chat.begin(); chitr != chat.end(); chitr++) { 
            // I tried just chat.remove(text) but that didn't work for some reason. 
            if (strcmp(text, (*chitr)) == 0) { 
                chitr = chat.erase(chitr); 
                chitr--; 
                sprintf_s(temp, "Chat channel removed: %s", text); 
                WriteChatColor(temp, CONCOLOR_YELLOW); 
                break; 
            } 
        } 
    } else { 
        WriteChatColor("Usage: /autohotkey remchat <chatname>", CONCOLOR_YELLOW); 
    } 
} // end RemHotkeyChat 


// Save auto_on and all chat settings to the .ini 
VOID SaveHotkeySettings() 
{ 
    CHAR text[MAX_STRING] = {0}; 
    CHAR filename[MAX_STRING] = {0}; 
    list <CHAR *>::iterator chitr; 
    INT x = 0; 
    
    // Set up filename 
    sprintf_s(filename, "%s%chotkeys_%s_%s.ini", gszINIPath, '\\', GetCharInfo()->Name, EQADDR_SERVERNAME); 
    // First for the auto_on setting 
    if (WritePrivateProfileString("DEFAULT", "AUTO_ON", BoolToStr(auto_on, text), filename) == 0) { 
        // Something went wrong... 
        strcpy_s(text, "Failed to write to:"); 
        WriteChatColor(text, CONCOLOR_RED); 
        WriteChatColor(filename, CONCOLOR_RED); 
    } else { 
        // Now for chat(s) 
        for (chitr = chat.begin(), x = 0; chitr != chat.end(); chitr++, x++) { 
            sprintf_s(text, "CHAT%d", x); 
            WritePrivateProfileString("DEFAULT", text, (*chitr), filename); 
        } 
        strcpy_s(text, "Write successful to:"); 
        WriteChatColor(text, CONCOLOR_YELLOW); 
        WriteChatColor(filename, CONCOLOR_GREEN); 
        // Now for PopUp states 

        WritePrivateProfileString("PopUps", "OnHit", BoolToStr(popups[OnHit], text), filename); 
        WritePrivateProfileString("PopUps", "OnResist", BoolToStr(popups[OnResist], text), filename); 
        WritePrivateProfileString("PopUps", "OnInterrupt", BoolToStr(popups[OnInterrupt], text), filename); 
        WritePrivateProfileString("PopUps", "OnOutOfRange", BoolToStr(popups[OnOutOfRange], text), filename); 
        WritePrivateProfileString("PopUps", "OnWillNotHold", BoolToStr(popups[OnWillNotHold], text), filename); 
        WritePrivateProfileString("PopUps", "OnWornOff", BoolToStr(popups[OnWornOff], text), filename); 
    } 
} //end SaveHotkeySettings 

// I was using these lines at 3 different places.  It converts if needed to a real chat command. 
VOID ConvertChat(PCHAR dst,size_t dstSize, CHAR *src)
{ 
    if ( (src[0] == '/') || (src[0] == ';') ) { 
        // ok they put in their own command structure.  Let's assume they know what they are doing. 
        // Dangerous I know =) 
        if ((_stricmp(src, "group") == 0) || (_stricmp(src, "groupsay") == 0)) { 
            // This has to be just /g for some reason.  /group and /groupsay don't work 
            strcpy_s(dst, dstSize, "/g"); 
        } else { 
            strcpy_s(dst, dstSize, src); 
        } 
    } else if (((strlen(src) == 1) && isdigit(src[0])) || 
               ((strlen(src) == 2) && (strcmp(src, "10") == 0))){ 
        // Number, must be a /<chatnumber> type format 
        sprintf_s(dst, dstSize, "/%s", src); 
    } else if (_stricmp(src, "say") && _stricmp(src, "group") && _stricmp(src, "guild") && 
            _stricmp(src, "ooc") && _stricmp(src, "shout") && _stricmp(src, "auction") && 
            _stricmp(src, "rs") && _stricmp(src, "rsay")) { 
        // if in here, it wasn't a / command (stricmp returns 0 if the same, case independant) 
        // so let's put in for a chat channel which is ;#<channel> 
        sprintf_s(dst, dstSize, ";#%s", src); 
    } else { 
        // Must have been a / command 
        if ((_stricmp(src, "group") == 0) || (_stricmp(src, "groupsay") == 0)) { 
            // This has to be just /g for some reason.  /group and /groupsay don't work 
            strcpy_s(dst, dstSize, "/g"); 
        } else { 
            sprintf_s(dst, dstSize, "/%s", src); 
        } 
    } 
} // end ConvertChat 

// Queue's up messages to a specified queue from a specified queue 
VOID QueueMessages(list <CHAR *> *dstq, list <CHAR *> *srcq) 
{ 
    list <CHAR *>::iterator hkitr, chitr; 
    CHAR *temp; 

    // Traverse all items in source queue 
    for (hkitr = srcq->begin(); hkitr != srcq->end(); hkitr++) { 
        if ((*hkitr)[0]== '/' || (*hkitr)[0] == ';') { 
            // Hotkey is formated already, put in queue 
            dstq->push_back(*hkitr); 
        } else if (!chat.empty()) { 
            // Need to traverse list of default chats and 
            // queue up the hotkey with each chat in front of it. 
            for (chitr = chat.begin(); chitr != chat.end(); chitr++) { 
                temp = new CHAR[BUFF_SIZE]; 
                sprintf_s(temp, BUFF_SIZE, "%s %s", (*chitr), (*hkitr)); 
                dstq->push_back(temp); 
            } 
        } 
    } 
} //end QueueMessages 

// Set a popup TRUE/FALSE 
VOID SetPopUps(CHAR *szLine) 
{ 
    CHAR text[MAX_STRING] = {0}; 
    INT x = 0; 
    BOOL setting = FALSE; 

    GetArg(text, szLine, 3); 
    if (_stricmp(text, "TRUE") == 0)  setting = TRUE; 
    else                        setting = FALSE; 
    GetArg(text, szLine, 2); 
    if (_stricmp(text, "all") == 0) { 
        for (x = 0; x <= NUM_POPUP_FLAGS; x++ ) popups[x] = setting; 
        WriteChatColor("All popups set.",CONCOLOR_YELLOW); 
    } else if (_stricmp(text, "OnHit") == 0) { 
        popups[OnHit] = setting; 
        WriteChatColor("OnHit popups set.",CONCOLOR_YELLOW); 
    } else if (_stricmp(text, "OnResist") == 0) { 
        popups[OnResist] = setting; 
        WriteChatColor("OnResist popups set.",CONCOLOR_YELLOW); 
    } else if (_stricmp(text, "OnInterrupt") == 0) { 
        popups[OnInterrupt] = setting; 
        WriteChatColor("OnInterrupt popups set.",CONCOLOR_YELLOW); 
    } else if (_stricmp(text, "OnOutOfRange") == 0) { 
        popups[OnOutOfRange] = setting; 
        WriteChatColor("OnOutOfRange popups set.",CONCOLOR_YELLOW); 
    } else if (_stricmp(text, "OnWillNotHold") == 0) { 
        popups[OnWillNotHold] = setting; 
        WriteChatColor("OnWillNotHold popups set.",CONCOLOR_YELLOW); 
    } else if (_stricmp(text, "OnWornOff") == 0) { 
        popups[OnWornOff] = setting; 
        WriteChatColor("OnWornOff popups set.",CONCOLOR_YELLOW); 
    } else { 
        WriteChatColor("Usage: /autohotkey setpopus < all | NameOfPopUp > < TRUE | FALSE >", CONCOLOR_YELLOW); 
    } 
} //end SetPopUps 

// Little fast routine for converting BOOL to STR for writing/reading 
template <unsigned int _Size>CHAR *BoolToStr(BOOL b, CHAR(&dstp)[_Size])
{ 
    if (b)
		strcpy_s(dstp, "TRUE"); 
    else 
		strcpy_s(dstp, "FALSE"); 
    return dstp; 
} //end BoolToStr 
