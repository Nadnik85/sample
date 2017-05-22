// MQ2AutoLoot.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.

#define PLUGIN_NAME					"MQ2AutoLoot"                // Plugin Name
#define VERSION						1.00
#define	PLUGIN_MSG					"\ag[MQ2AutoLoot]\ax "     
#define PERSONALBANKER_CLASS		40
#define MERCHANT_CLASS				41
#define GUILDBANKER_CLASS			66

#ifndef PLUGIN_API
#include "../MQ2Plugin.h"
PreSetup(PLUGIN_NAME);
PLUGIN_VERSION(VERSION);
#endif PLUGIN_API

#include <chrono>


//MoveUtils 11.x
PLUGIN_API bool bMULoaded = false;
bool bMUPointers = false;
bool* pbStickOn;
void(*fStickCommand)(PSPAWNINFO pChar, char* szLine);
//MQ2EqBC shit
//PLUGIN_API bool bEQBCLoaded = false;
//bool bEQBCPointers = false;
//bool* pbEQBCNames;



// Variables that are setable through the /AutoLoot command
LONG				UseAutoLoot = 1;
LONG				SpamLootInfo = 1;
LONG				CursorDelay = 10;
LONG				DistributeLootDelay = 5;
LONG				SaveBagSlots = 0;
LONG				QuestKeep = 0;
LONG				BarMinSellPrice = 1;
CHAR				NoDropDefault[MAX_STRING];
CHAR				LootINI[MAX_STRING];
CHAR				ExcludedBag1[MAX_STRING];
CHAR				ExcludedBag2[MAX_STRING];
// Variables that are used within the plugin, but not setable 
BOOL				Initialized = false;
BOOL				StartCursorTimer = true;  // 
BOOL				StartDistributeLootTimer = true;  // 
BOOL				StartLootStuff = false; // When set true will call DoLootStuff
BOOL				StartMoveToTarget = false; // Will move to target (banker/merchant) when set to true
BOOL				StartToOpenWindow = false; // Will move to target (banker/merchant) when set to true
BOOL				StarBarterTimer = false; // Will start the barter timer when set to true
BOOL				LootStuffWindowOpen = false; // Will be set true the first time the merchant/barter/banker window is open and will stop you from reopening the window a second time
LONG				DistributeI; //Index for looping over people in the group to try and distribute an item 
LONG				DistributeK;  // Index for the item to be distributed
LONG				LootStuffN;  //
LONG				BarterIndex;  //
DWORD				DestroyID;
DWORD				CursorItemID;
DWORD				DistributeItemID;
CHAR				Command[MAX_STRING];
CHAR				LootStuffAction[MAX_STRING];
std::chrono::high_resolution_clock::time_point	LootTimer = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point	CursorTimer = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point	DistributeLootTimer = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point	DestroyStuffTimer = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point	DestroyStuffCancelTimer = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point	LootStuffTimer = std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point	LootStuffCancelTimer = std::chrono::high_resolution_clock::now();

// Functions to be called
PMQPLUGIN Plugin(char* PluginName); 
bool HandleMoveUtils(void);  // Used to connect to MQ2MoveUtils
BOOL DoIHaveSpace(CHAR* pszItemName, DWORD plMaxStackSize, DWORD pdStackSize);
BOOL FitInInventory(DWORD pdItemSize);
BOOL FitInBank(DWORD pdItemSize);
LONG SetBOOL(long Cur, PCHAR Val, PCHAR Sec = "", PCHAR Key = "", PCHAR INI = "");
int CheckIfItemIsLoreByID(int ItemID);
DWORD FindItemCount(CHAR* pszItemName);
DWORD FindBarterItemCount(CHAR* pszItemName);
void SetAutoLootVariables();
void AutoLootCommand(PSPAWNINFO pCHAR, PCHAR zLine);
void SetItemCommand(PSPAWNINFO pCHAR, PCHAR zLine);
void DestroyStuff(void); 
void DoLootStuff(CHAR* szAction);
void DoBarterStuff(CHAR* szAction);
void CreateLootEntry(CHAR* szAction, CHAR* szEntry, PITEMINFO pItem);
void CreateLootINI(void);

#pragma region Inlines
// Returns TRUE if character is in game and has valid character data structures
inline BOOL InGameOK()
{
	return(GetGameState() == GAMESTATE_INGAME && GetCharInfo() && GetCharInfo()->pSpawn && GetCharInfo2());
}

// Returns TRUE if the specified UI window is visible
inline BOOL WinState(CXWnd *Wnd)
{
	return (Wnd && ((PCSIDLWND)Wnd)->dShow);
}

#pragma endregion Inlines

class MQ2AutoLootType* pAutoLootType = NULL;
class MQ2AutoLootType : public MQ2Type
{
public:
	enum AutoLootMembers
	{
		Active = 1,
		SellActive = 2,
		DepositActive = 3,
		BarterActive = 4,
	};

	MQ2AutoLootType() :MQ2Type("AutoLoot")
	{
		TypeMember(Active);
		TypeMember(SellActive);
		TypeMember(DepositActive);
		TypeMember(BarterActive);
	}

	bool GetMember(MQ2VARPTR VarPtr, char* Member, char* Index, MQ2TYPEVAR &Dest)
	{
		PMQ2TYPEMEMBER pMember = MQ2AutoLootType::FindMember(Member);
		if (pMember)
		{
			switch ((AutoLootMembers)pMember->ID)
			{
			case Active:
				Dest.DWord = (UseAutoLoot);
				Dest.Type = pBoolType;
				return true;
			case SellActive:
				Dest.DWord = (StartLootStuff && !_stricmp(LootStuffAction, "Sell"));
				Dest.Type = pBoolType;
				return true;
			case DepositActive:
				Dest.DWord = (StartLootStuff && !_stricmp(LootStuffAction, "Deposit"));
				Dest.Type = pBoolType;
				return true;
			case BarterActive:
				Dest.DWord = (StartLootStuff && !_stricmp(LootStuffAction, "Barter"));
				Dest.Type = pBoolType;
				return true;
			}
			return false;
		}
		return false;
	}

	bool ToString(MQ2VARPTR VarPtr, PCHAR Destination)
	{
		strcpy_s(Destination, MAX_STRING, "TRUE");
		return true;
	}
	bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source)
	{
		return false;
	}
	bool FromString(MQ2VARPTR &VarPtr, PCHAR Source)
	{
		return false;
	}
	~MQ2AutoLootType() {}
};

int dataAutoLoot(char* szName, MQ2TYPEVAR &Ret)
{
	Ret.DWord = 1;
	Ret.Type = pAutoLootType;
	return true;
}

PLUGIN_API VOID InitializePlugin()
{
    // commands
	AddCommand("/autoloot", AutoLootCommand);
	AddCommand("/setitem", SetItemCommand);

	// TLOs
    AddMQ2Data("AutoLoot", dataAutoLoot);

	pAutoLootType = new MQ2AutoLootType;
}

PLUGIN_API VOID ShutdownPlugin()
{
    // remove commands
	RemoveCommand("/autoloot");
	RemoveCommand("/setitem");

	// remove TLOs
    RemoveMQ2Data("AutoLoot");

	delete pAutoLootType;
}

PLUGIN_API VOID SetGameState(DWORD GameState)
{
	if (!InGameOK()) return;
	if (GameState == GAMESTATE_INGAME)
	{
		if (!Initialized)
		{
			SetAutoLootVariables();
			Initialized = true;
		}
	}
	else if (GameState != GAMESTATE_LOGGINGIN)
	{
		if (Initialized)
		{
			Initialized = 0;
		}
	}
}

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
// but after MQ filters and chat events are taken care of.
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
	if (!InGameOK()) return 0;
	if (WinState((CXWnd*)pMerchantWnd))
	{
		if (pMerchantWnd->pFirstChildWnd)
		{
			if (pMerchantWnd->pFirstChildWnd->pNextSiblingWnd)
			{
				CHAR MerchantText[MAX_STRING] = { 0 };
				CHAR MerchantName[MAX_STRING] = { 0 };
				GetCXStr(pMerchantWnd->pFirstChildWnd->WindowText, MerchantName, MAX_STRING);
				sprintf_s(MerchantText, "%s says, 'Hi there, %s. Just browsing?  Have you seen the ", MerchantName, GetCharInfo()->Name); // Confirmed 04/15/2017
				if (strstr(Line, MerchantText))
				{
					LootStuffTimer = std::chrono::high_resolution_clock::now();
					LootStuffCancelTimer = std::chrono::high_resolution_clock::now() + std::chrono::minutes(20); // Will stop trying to move to open the merchant/banker window after 20 minutes
					return(0);
				}
				sprintf_s(MerchantText, "%s says, 'Welcome to my shop, %s. You would probably find a ", MerchantName, GetCharInfo()->Name); // Confirmed 04/15/2017
				if (strstr(Line, MerchantText))
				{
					LootStuffTimer = std::chrono::high_resolution_clock::now();
					LootStuffCancelTimer = std::chrono::high_resolution_clock::now() + std::chrono::minutes(20); // Will stop trying to move to open the merchant/banker window after 20 minutes
					return(0);
				}
				sprintf_s(MerchantText, "%s says, 'Hello there, %s. How about a nice ", MerchantName, GetCharInfo()->Name); // Confirmed 04/15/2017
				if (strstr(Line, MerchantText))
				{
					LootStuffTimer = std::chrono::high_resolution_clock::now();
					LootStuffCancelTimer = std::chrono::high_resolution_clock::now() + std::chrono::minutes(20); // Will stop trying to move to open the merchant/banker window after 20 minutes
					return(0);
				}
				sprintf_s(MerchantText, "%s says, 'Greetings, %s. You look like you could use a ", MerchantName, GetCharInfo()->Name); // Confirmed 04/15/2017
				if (strstr(Line, MerchantText))
				{
					LootStuffTimer = std::chrono::high_resolution_clock::now();
					LootStuffCancelTimer = std::chrono::high_resolution_clock::now() + std::chrono::minutes(20); // Will stop trying to move to open the merchant/banker window after 20 minutes
					return(0);
				}
				sprintf_s(MerchantText, "%s tells you, 'I'll give you ", MerchantName);  // Confirmed 04/15/2017
				if (strstr(Line, MerchantText))
				{
					LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(100);
					return(0);
				}
				sprintf_s(MerchantText, "You receive");  // Confirmed 04/15/2017
				if (strstr(Line, MerchantText))
				{
					sprintf_s(MerchantText, "from %s for the ", MerchantName);  // Confirmed 04/15/2017
					if (PCHAR pItemStart = strstr(Line, MerchantText))
					{
						sprintf_s(MerchantText, "(s)");  // Confirmed 04/15/2017
						if (PCHAR pItemEnd = strstr(Line, MerchantText))
						{
							LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(100);
							return(0);
						}
					}
				}
			}
		}
	}

	CHAR BankerText[MAX_STRING] = { 0 };
	sprintf_s(BankerText, "tells you, 'Welcome to my bank!"); // Confirmed 04/21/2017
	if (strstr(Line, BankerText))
	{
		LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::seconds(2); // Will start trying to deposit items are a 2 second delay
		LootStuffCancelTimer = std::chrono::high_resolution_clock::now() + std::chrono::minutes(20); // Will stop trying to deposit items after 20 minutes
		return(0);
	}

	if (WinState((CXWnd*)FindMQ2Window("BarterSearchWnd")))
	{
		CHAR BarterText[MAX_STRING] = { 0 };
		sprintf_s(BarterText, "There are ");  // Confirmed 04/26/2017
		if (strstr(Line, BarterText))
		{
			sprintf_s(BarterText, "Buy Lines that match the search string '");  // Confirmed 04/26/2017
			if (PCHAR pItemStart = strstr(Line, BarterText))
			{
				sprintf_s(BarterText, "'.");  // Confirmed 04/26/2017
				if (PCHAR pItemEnd = strstr(Line, BarterText))
				{
					//WriteChatf(PLUGIN_MSG ":: OMG i searched shit");
					LootStuffN = 3;
					LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(500);
					return(0);
				}
			}
		}
		sprintf_s(BarterText, "You've sold ");  // Confirmed 04/27/2017
		if (strstr(Line, BarterText))
		{
			sprintf_s(BarterText, " to ");  // Confirmed 04/27/2017
			if (PCHAR pItemStart = strstr(Line, BarterText))
			{
				sprintf_s(BarterText, ".");  // Confirmed 04/27/2017
				if (PCHAR pItemEnd = strstr(Line, BarterText))
				{
					//WriteChatf(PLUGIN_MSG ":: OMG i sold shit");
					LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(100);
					return(0);
				}
			}
		}
		sprintf_s(BarterText, "Your transaction failed because your barter data is out of date.");  // Taken from HoosierBilly, has not beem checked
		if (strstr(Line, BarterText)) //Need to research to refresh the barter search window
		{
			LootStuffN = 2;
			return(0);
		}
	}

	return(0);
}

// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
	if (!InGameOK()) return;
	PCHARINFO pChar = GetCharInfo();
	PCHARINFO2 pChar2 = GetCharInfo2();
	if (!UseAutoLoot) return;

	//check cursor for items, and will put in inventory if it fits after CursorDelay has been exceed
	BOOL ItemOnCursor = false;
	if (pChar2->pInventoryArray && pChar2->pInventoryArray->Inventory.Cursor)
	{
		if (PCONTENTS pItem = pChar2->pInventoryArray->Inventory.Cursor)
		{
			ItemOnCursor = true;
			if (!WinState((CXWnd*)pTradeWnd) && !WinState((CXWnd*)pGiveWnd) && !WinState((CXWnd*)pMerchantWnd) && !WinState((CXWnd*)pBankWnd) && !WinState((CXWnd*)FindMQ2Window("GuildBankWnd")) && !WinState((CXWnd*)FindMQ2Window("TradeskillWnd")))
			{
				if (StartCursorTimer)  // Going to set CursorItemID and CursorTimer
				{
					StartCursorTimer = false;
					CursorItemID = pItem->ID;
					CursorTimer = std::chrono::high_resolution_clock::now() + std::chrono::seconds(CursorDelay); // Wait CursorDelay in seconds before attempting to autoinventory the item on your cursor
				}
				else if (CursorItemID != pItem->ID) // You changed items on your cursor, time to reset CursorItemID and CursorTimer
				{
					CursorItemID = pItem->ID;
					CursorTimer = std::chrono::high_resolution_clock::now() + std::chrono::seconds(CursorDelay); // Wait CursorDelay in seconds before attempting to autoinventory the item on your cursor
				}
				else if (std::chrono::high_resolution_clock::now() > CursorTimer)  // Waited CursorDelay, now going to see if you have room to autoinventory the item on your cursor
				{
					if (FitInInventory(pItem->Item2->Size))
					{
						if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: Putting \ag%s\ax into my inventory", pItem->Item2->Name); }
						DoCommand(GetCharInfo()->pSpawn, "/autoinventory");
						StartCursorTimer = true;
					}
					else
					{
						if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: \ag%s\ax doesn't fit into my inventory", pItem->Item2->Name); }
						StartCursorTimer = true;
					}
				}
			}
		}
	}
	else
	{
		StartCursorTimer = true;
	}

	// When confirmation box for looting no drop items pops up this will allow it to be clicked
	if (CXWnd *pWnd = (CXWnd *)FindMQ2Window("ConfirmationDialogBox"))
	{
		if (((PCSIDLWND)(pWnd))->dShow)
		{
			if (CXWnd *Child = pWnd->GetChildItem("CD_TextOutput"))
			{
				char ConfirmationText[MAX_STRING];
				GetCXStr(((PCSIDLWND)Child)->SidlText, ConfirmationText, sizeof(ConfirmationText));
				if (strstr(ConfirmationText, "is a NO DROP item, are you sure you wish to loot it?"))
				{
					if (WinState((CXWnd*)pLootWnd))
					{
						PEQLOOTWINDOW pLoot = (PEQLOOTWINDOW)pLootWnd;
						for (int nLootSlots = 0; nLootSlots < (int)pLoot->NumOfSlots; nLootSlots++)
						{
							if (PCONTENTS pContents = pLoot->pInventoryArray->InventoryArray[nLootSlots])
							{
								if (PITEMINFO pItem = GetItemFromContents(pContents))
								{
									if (strstr(ConfirmationText, pItem->Name))
									{
										if (!CheckIfItemIsLoreByID(pContents->ID)) 
										{
											//Ok so I don't have the item and it is lore or it is not lore and I can accept it
											if (CXWnd *pWndButton = pWnd->GetChildItem("CD_Yes_Button"))
											{
												if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: Accepting a no drop item"); }
												SendWndClick2(pWndButton, "leftmouseup");
												return;
											}
										}
									}
								}
							}
						}
					}
					else
					{
						if (CXWnd *pWndButton = pWnd->GetChildItem("CD_Yes_Button"))
						{
							if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: Accepting a no drop item"); }
							SendWndClick2(pWndButton, "leftmouseup");
							return;
						}
					}
				}
			}
		}
	}

	//if (PEQTRADEWINDOW pTrade = (PEQTRADEWINDOW)pTradeWnd)
	//{
	//	if (pTrade->HisTradeReady && !pTrade->MyTradeReady && !ItemOnCursor)
	//	{
	//		if (CXWnd *pWndButton = pTradeWnd->GetChildItem("TRDW_Trade_Button"))
	//		{
	//			SendWndClick2(pWndButton, "leftmouseup");
	//		}
	//	}
	//}

	// Will be called when you use /autoloot sell|barter|deposit
	if (StartLootStuff && std::chrono::high_resolution_clock::now() > LootStuffTimer) 
	{ 
		if (!_stricmp(LootStuffAction, "Barter"))
		{
			DoBarterStuff(LootStuffAction); 
		}
		else
		{
			DoLootStuff(LootStuffAction); 
		}
	}
	
	// When you loot an item marked Destroy it will set the DestroyID to that item's ID and proceed to pick that item from inventory and destroy before resetting DestroyID to 0
	if (DestroyID)
	{
		if (std::chrono::high_resolution_clock::now() > DestroyStuffTimer)
		{
			DestroyStuff();
		}
		return;
	}

	if (!WinState((CXWnd*)pAdvancedLootWnd)) return;
	PEQADVLOOTWND pAdvLoot = (PEQADVLOOTWND)pAdvancedLootWnd;
	if (!pAdvLoot || std::chrono::high_resolution_clock::now() < LootTimer) return;

	CListWnd *pSharedList = (CListWnd *)pAdvLoot->pCLootList->SharedLootList;
	CListWnd *pPersonalList = (CListWnd *)pAdvancedLootWnd->GetChildItem("ADLW_PLLList");
	if (LootInProgress(pAdvLoot, pPersonalList, pSharedList)) return;

	CHAR DistributeName[MAX_STRING] = { 0 }; //Name of person to distribute to
	CHAR INISection[MAX_STRING] = { 0 };
	CHAR LootEntry[MAX_STRING] = { 0 };
	CHAR Value[MAX_STRING] = { 0 };
	if (pAdvLoot->pPLootList)
	{
		for (LONG k = 0; k < pPersonalList->ItemsArray.Count; k++)
		{
			LONG listindex = pPersonalList->GetItemData(k);
			if (listindex != -1)
			{
				DWORD multiplier = sizeof(LOOTITEM) * listindex;
				if (PLOOTITEM pPersonalItem = (PLOOTITEM)(((DWORD)pAdvLoot->pPLootList->pLootItem) + multiplier))
				{
					sprintf_s(INISection, "%c", pPersonalItem->Name[0]);
					if (GetPrivateProfileString(INISection, pPersonalItem->Name, 0, Value, MAX_STRING, LootINI) == 0)
					{
						if (pPersonalItem->NoDrop)
						{
							CHAR *pParsedToken = NULL;
							CHAR *pParsedValue = strtok_s(NoDropDefault, "|", &pParsedToken);
							if (!_stricmp(pParsedValue, "Quest"))
							{
								sprintf_s(LootEntry, "%s|%d", pParsedValue, QuestKeep);
							}
							else if (!_stricmp(pParsedValue, "Keep") || !_stricmp(pParsedValue, "Ignore"))
							{
								sprintf_s(LootEntry, "%s", pParsedValue);
							}
							else
							{
								sprintf_s(LootEntry, "Quest|1");
							}
							WriteChatf(PLUGIN_MSG ":: The \ag%s\ax is not in the database, setting it to %s", pPersonalItem->Name, LootEntry);
							WritePrivateProfileString(INISection, pPersonalItem->Name, LootEntry, LootINI);
							sprintf_s(Value, "%s", LootEntry);
						}
						else
						{
							WriteChatf(PLUGIN_MSG ":: The \ag%s\ax is not in the database, setting it to Keep", pPersonalItem->Name);
							WritePrivateProfileString(INISection, pPersonalItem->Name, "Keep", LootINI);
							sprintf_s(Value, "Keep");
						}
					}
					else
					{
						CHAR *pParsedToken = NULL;
						CHAR *pParsedValue = strtok_s(Value, "|", &pParsedToken);
						if (!_stricmp(pParsedValue, "Keep") || !_stricmp(pParsedValue, "Sell") || !_stricmp(pParsedValue, "Deposit") || !_stricmp(pParsedValue, "Barter") || !_stricmp(pParsedValue, "Quest") || !_stricmp(pParsedValue, "Gear") || !_stricmp(Value, "Destroy"))
						{
							if (pPersonalItem->LootDetails->Locked || CheckIfItemIsLoreByID(pPersonalItem->ItemID) || !DoIHaveSpace(pPersonalItem->Name, pPersonalItem->MaxStack, pPersonalItem->LootDetails->StackCount))
							{
								if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: PList: \ag%s\ax is lore/locked/I don't have room, setting to leave", pPersonalItem->Name); }
								LONG LootInd = k + 1;
								sprintf_s(Command, "/advloot personal %d leave", LootInd);
								DoCommand(GetCharInfo()->pSpawn, Command);
								return;
							}
							if (!_stricmp(Value, "Destroy"))
							{
								DestroyID = pPersonalItem->ItemID;
								DestroyStuffCancelTimer = std::chrono::high_resolution_clock::now() + std::chrono::seconds(10);
							}
							if (pPersonalItem->NoDrop) { LootTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(1000); } // Adding a delay to click accept on no drop items, TODO verify this time works
							if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: PList: Setting \ag%s\ax to loot", pPersonalItem->Name); }
							LONG LootInd = k + 1;
							sprintf_s(Command, "/advloot personal %d loot", LootInd);
							DoCommand(GetCharInfo()->pSpawn, Command);
							return;
						}
						else if (!_stricmp(pParsedValue, "Ignore"))
						{
							if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: PList: Setting \ag%s\ax to leave", pPersonalItem->Name); }
							LONG LootInd = k + 1;
							sprintf_s(Command, "/advloot personal %d leave", LootInd);
							DoCommand(GetCharInfo()->pSpawn, Command);
							return;
						}
						else
						{
							if (pPersonalItem->NoDrop)
							{
								CHAR *pParsedToken = NULL;
								CHAR *pParsedValue = strtok_s(NoDropDefault, "|", &pParsedToken);
								if (!_stricmp(pParsedValue, "Quest"))
								{
									sprintf_s(LootEntry, "%s|%d", pParsedValue, QuestKeep);
								}
								else if (!_stricmp(pParsedValue, "Keep") || !_stricmp(pParsedValue, "Ignore"))
								{
									sprintf_s(LootEntry, "%s", pParsedValue);
								}
								else
								{
									sprintf_s(LootEntry, "Quest|1");
								}
								WriteChatf(PLUGIN_MSG ":: The \ag%s\ax is not in the database, setting it to %s", pPersonalItem->Name, LootEntry);
								WritePrivateProfileString(INISection, pPersonalItem->Name, LootEntry, LootINI);
								return;
							}
							else
							{
								if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: The \ag%s\ax was set to \ag%s\ax, changing to Keep", pPersonalItem->Name, pParsedValue); }
								WritePrivateProfileString(INISection, pPersonalItem->Name, "Keep", LootINI);
								return;
							}
						}
					}
				}
			}
		}
	}

	if (pSharedList)
	{
		if (pChar->pGroupInfo && pChar->pGroupInfo->pMember && pChar->pGroupInfo->pMember[0])
		{
			CHAR MyName[MAX_STRING] = { 0 }; //My name
			GetCXStr(pChar->pGroupInfo->pMember[0]->pName, MyName, MAX_STRING);
			//If I don't have a masterlooter set and I am leader I will set myself master looter
			if (pChar->pGroupInfo)
			{
				for (LONG k = 0; k < 6; k++)
				{
					if (pChar->pGroupInfo->pMember[k] && pChar->pGroupInfo->pMember[k]->pName && pChar->pGroupInfo->pMember[k]->MasterLooter)
					{
						k = 6;  // this is required to stop the for loop once we know we have a master looter set
						break;
					}
					if (k == 5 && pChar->pGroupInfo->pLeader->pSpawn->SpawnID == pChar->pSpawn->SpawnID)  // oh shit we have loot and no master looter set yet and I am the leader, so lets make me the leader
					{
						WriteChatf(PLUGIN_MSG ":: I am setting myself to master looter");
						sprintf_s(Command, "/grouproles set %s 5", MyName);
						DoCommand(GetCharInfo()->pSpawn, Command);
						LootTimer = std::chrono::high_resolution_clock::now() + std::chrono::seconds(5);  //Two seconds was too short, it attempts to set masterlooter a second time.  Setting to 5 seconds that should fix this
						return;
					}
				}
			}
			//Loop over the item array to find see if I need to set something
			for (LONG k = 0; k < pSharedList->ItemsArray.Count; k++)
			{
				LONG listindex = pSharedList->GetItemData(k);
				if (listindex != -1)
				{
					DWORD multiplier = sizeof(LOOTITEM) * listindex;
					if (PLOOTITEM pShareItem = (PLOOTITEM)(((DWORD)pAdvLoot->pCLootList->pLootItem) + multiplier))
					{
						if (!StartDistributeLootTimer) // I am checking if the item has successfully been passed out
						{
							if (DistributeK == k)
							{
								if (DistributeItemID != pShareItem->ItemID)
								{
									StartDistributeLootTimer = true;
								}
							}
						}
						if (!pShareItem->AutoRoll && !pShareItem->No && !pShareItem->Need && !pShareItem->Greed)
						{
							sprintf_s(INISection, "%c", pShareItem->Name[0]);
							bool IWant = false;  // Will be set true if you want and can accept the item
							bool IDoNotWant = false;  // Will be set true if you don't want or can't accept
							bool CheckIfOthersWant = false;  // Will be set true if I am ML and I can't accept or don't need
							if (GetPrivateProfileString(INISection, pShareItem->Name, 0, Value, MAX_STRING, LootINI) == 0)
							{
								if (pShareItem->NoDrop)
								{
									CHAR *pParsedToken = NULL;
									CHAR *pParsedValue = strtok_s(NoDropDefault, "|", &pParsedToken);
									if (!_stricmp(pParsedValue, "Quest"))
									{
										sprintf_s(LootEntry, "%s|%d", pParsedValue, QuestKeep);
									}
									else if (!_stricmp(pParsedValue, "Keep") || !_stricmp(pParsedValue, "Ignore"))
									{
										sprintf_s(LootEntry, "%s", pParsedValue);
									}
									else
									{
										sprintf_s(LootEntry, "Quest|1");
									}
									WriteChatf(PLUGIN_MSG ":: The \ag%s\ax is not in the database, setting it to %s", pShareItem->Name, LootEntry);
									WritePrivateProfileString(INISection, pShareItem->Name, LootEntry, LootINI);
									return;
								}
								else
								{
									if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: The \ag%s\ax is not in the database, setting it to Keep", pShareItem->Name); }
									WritePrivateProfileString(INISection, pShareItem->Name, "Keep", LootINI);
									return;
								}
							}
							else
							{
								CHAR *pParsedToken = NULL;
								CHAR *pParsedValue = strtok_s(Value, "|", &pParsedToken);
								if (!_stricmp(pParsedValue, "Quest"))
								{
									DWORD QuestNumber;
									pParsedValue = strtok_s(NULL, "|", &pParsedToken);
									if (pParsedValue == NULL)
									{
										QuestNumber = 1;
										if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: You did not set the quest number for \ag%s\ax, changing it to Quest|1", pShareItem->Name); }
										WritePrivateProfileString(INISection, pShareItem->Name, "Quest|1", LootINI);
									}
									else
									{
										//TODO add safety checks to make sure it is a number etc
										QuestNumber = atoi(pParsedValue);
									}
									if ((ItemOnCursor || pShareItem->LootDetails->Locked || QuestNumber <= FindItemCount(pShareItem->Name) || !DoIHaveSpace(pShareItem->Name, pShareItem->MaxStack, pShareItem->LootDetails->StackCount) || CheckIfItemIsLoreByID(pShareItem->ItemID)) && !pChar->pGroupInfo->pMember[0]->MasterLooter)
									{
										IDoNotWant = true;
									}
									else if ((ItemOnCursor || pShareItem->LootDetails->Locked || QuestNumber <= FindItemCount(pShareItem->Name) || !DoIHaveSpace(pShareItem->Name, pShareItem->MaxStack, pShareItem->LootDetails->StackCount) || CheckIfItemIsLoreByID(pShareItem->ItemID)) && pChar->pGroupInfo->pMember[0]->MasterLooter)
									{
										CheckIfOthersWant = true;
									}
									else
									{
										IWant = true;
									}
								}
								else if (!_stricmp(pParsedValue, "Gear"))
								{
									bool RightClass = false; // Will set true if your class is one of the entries after Gear|Classes|...
									DWORD GearNumber = 0;  // The number of this item to loot
									pParsedValue = strtok_s(NULL, "|", &pParsedToken);
									if (pParsedValue == NULL)
									{
										if (PCONTENTS pItem = FindBankItemByID(pShareItem->ItemID))
										{
											WriteChatf(PLUGIN_MSG ":: Found:\ag%s\ax, in my bank!",pShareItem->Name);
											CreateLootEntry( "Gear", "", GetItemFromContents(pItem));
											return;
										}
										else if (PCONTENTS pItem = FindItemByID(pShareItem->ItemID))
										{
											WriteChatf(PLUGIN_MSG ":: Found:\ag%s\ax, in my packs!",pShareItem->Name);
											CreateLootEntry( "Gear", "", GetItemFromContents(pItem));
											return;
										}
										else
										{
											WriteChatf(PLUGIN_MSG ":: \ag%s\ax hasn't ever had classes set, setting it to loot!", pShareItem->Name);
											IWant = true;
										}
									}
									while (pParsedValue != NULL)
									{
										if (!_stricmp(pParsedValue, ClassInfo[pChar2->Class].UCShortName))
										{
											RightClass = true;
										}		
										if (!_stricmp(pParsedValue, "NumberToLoot"))
										{
											pParsedValue = strtok_s(NULL, "|", &pParsedToken);
											if (pParsedValue == NULL)
											{
												GearNumber = 1;
												if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: You did not set the Gear Number for \ag%s\ax, Please change your loot ini entry", pShareItem->Name); }
											}
											else
											{
												//TODO add safety checks to make sure it is a number etc
												GearNumber = atoi(pParsedValue);
												break;
											}
										}
										pParsedValue = strtok_s(NULL, "|", &pParsedToken);
									}
									if ((!RightClass || ItemOnCursor || pShareItem->LootDetails->Locked || GearNumber <= FindItemCount(pShareItem->Name) || !DoIHaveSpace(pShareItem->Name, pShareItem->MaxStack, pShareItem->LootDetails->StackCount) || CheckIfItemIsLoreByID(pShareItem->ItemID)) && !pChar->pGroupInfo->pMember[0]->MasterLooter)
									{
										IDoNotWant = true;
									}
									else if ((!RightClass || ItemOnCursor || pShareItem->LootDetails->Locked || GearNumber <= FindItemCount(pShareItem->Name) || !DoIHaveSpace(pShareItem->Name, pShareItem->MaxStack, pShareItem->LootDetails->StackCount) || CheckIfItemIsLoreByID(pShareItem->ItemID)) && pChar->pGroupInfo->pMember[0]->MasterLooter)
									{
										CheckIfOthersWant = true;
									}
									else
									{
										IWant = true;
									}
								}
								else if (!_stricmp(pParsedValue, "Keep") || !_stricmp(pParsedValue, "Deposit") || !_stricmp(pParsedValue, "Sell") || !_stricmp(pParsedValue, "Barter"))
								{
									if ((ItemOnCursor || pShareItem->LootDetails->Locked || !DoIHaveSpace(pShareItem->Name, pShareItem->MaxStack, pShareItem->LootDetails->StackCount) || CheckIfItemIsLoreByID(pShareItem->ItemID)) && !pChar->pGroupInfo->pMember[0]->MasterLooter)
									{
										IDoNotWant = true;
									}
									else if ((ItemOnCursor || pShareItem->LootDetails->Locked || !DoIHaveSpace(pShareItem->Name, pShareItem->MaxStack, pShareItem->LootDetails->StackCount) || CheckIfItemIsLoreByID(pShareItem->ItemID)) && pChar->pGroupInfo->pMember[0]->MasterLooter)
									{
										CheckIfOthersWant = true;
									}
									else
									{
										IWant = true;
									}
								}
								else if (!_stricmp(Value, "Destroy"))
								{
									if (!pChar->pGroupInfo->pMember[0]->MasterLooter)
									{
										IDoNotWant = true;
									}
									else if ((ItemOnCursor || pShareItem->LootDetails->Locked || !DoIHaveSpace(pShareItem->Name, pShareItem->MaxStack, pShareItem->LootDetails->StackCount) || CheckIfItemIsLoreByID(pShareItem->ItemID)) && pChar->pGroupInfo->pMember[0]->MasterLooter)
									{
										IDoNotWant = true;
									}
									else
									{
										IWant = true;
									}
								}
								else if (!_stricmp(pParsedValue, "Ignore"))
								{
									IDoNotWant = true;
								}
								else
								{
									if (pShareItem->NoDrop)
									{
										CHAR *pParsedToken = NULL;
										CHAR *pParsedValue = strtok_s(NoDropDefault, "|", &pParsedToken);
										if (!_stricmp(pParsedValue, "Quest"))
										{
											sprintf_s(LootEntry, "%s|%d", pParsedValue, QuestKeep);
										}
										else if (!_stricmp(pParsedValue, "Keep") || !_stricmp(pParsedValue, "Ignore"))
										{
											sprintf_s(LootEntry, "%s", pParsedValue);
										}
										else
										{
											sprintf_s(LootEntry, "Quest|1");
										}
										WriteChatf(PLUGIN_MSG ":: The \ag%s\ax is not in the database, setting it to %s", pShareItem->Name, LootEntry);
										WritePrivateProfileString(INISection, pShareItem->Name, LootEntry, LootINI);
										return;
									}
									else
									{
										if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: The \ag%s\ax was set to \ag%s\ax, changing to Keep", pShareItem->Name, pParsedValue); }
										WritePrivateProfileString(INISection, pShareItem->Name, "Keep", LootINI);
										return;
									}
								}
								if (IWant && pChar->pGroupInfo->pMember[0]->MasterLooter)
								{
									//I want and am the master looter
									if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: SList: Giving \ag%s\ax to me", pShareItem->Name); }
									LONG LootInd = k + 1;
									sprintf_s(Command, "/advloot shared %d giveto %s", LootInd, MyName);
									DoCommand(GetCharInfo()->pSpawn, Command);
									return;
								}
								else if (IWant && !pChar->pGroupInfo->pMember[0]->MasterLooter)
								{
									//I want and i am not the master looter
									if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: SList: Setting \ag%s\ax to need", pShareItem->Name); }
									LONG LootInd = k + 1;
									sprintf_s(Command, "/advloot shared %d nd", LootInd);
									DoCommand(GetCharInfo()->pSpawn, Command);
									return;
								}
								else if (IDoNotWant && pChar->pGroupInfo->pMember[0]->MasterLooter)
								{
									//I don't want and am the master looter
									if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: SList: Setting \ag%s\ax to leave", pShareItem->Name); }
									LONG LootInd = k + 1;
									sprintf_s(Command, "/advloot shared %d leave", LootInd);
									DoCommand(GetCharInfo()->pSpawn, Command);
									return;
								}
								else if (IDoNotWant && !pChar->pGroupInfo->pMember[0]->MasterLooter)
								{
									//I don't want and i am not the master looter
									if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: SList: Setting \ag%s\ax to no", pShareItem->Name); }
									LONG LootInd = k + 1;
									sprintf_s(Command, "/advloot shared %d no", LootInd);
									DoCommand(GetCharInfo()->pSpawn, Command);
									return;
								}
								else if (CheckIfOthersWant)
								{
									if (StartDistributeLootTimer)
									{
										if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: SList: Setting the DistributeLootTimer"); }
										StartDistributeLootTimer = false;
										DistributeLootTimer = std::chrono::high_resolution_clock::now() + std::chrono::seconds(DistributeLootDelay);
										DistributeItemID = pShareItem->ItemID;
										DistributeI = 0;
										DistributeK = k;
									}
									else if (std::chrono::high_resolution_clock::now() > DistributeLootTimer)
									{
										if (pChar->pGroupInfo)
										{
											// The group index starts at 0, which is always the character with the plugin running.  The rest of the group uses index 1 - 5.
											DistributeI++;
											if (DistributeI == 6)
											{
												// Had 6 in the group and no one wanted the item
												if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: SList: No one wanted \ag%s\ax setting to leave", pShareItem->Name); }
												LONG LootInd = k + 1;
												sprintf_s(Command, "/advloot shared %d leave", LootInd);
												DoCommand(GetCharInfo()->pSpawn, Command);
												StartDistributeLootTimer = true;
												return;
											}
											if (pChar->pGroupInfo->pMember[DistributeI] && pChar->pGroupInfo->pMember[DistributeI]->pName && pChar->pGroupInfo->pMember[DistributeI]->Mercenary)
											{
												// This toon was a merc!
												if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: SList: Attempting to give \ag%s\ax to a mercenary, so i am skipping!", pShareItem->Name); }
												return;
											}
											if (pChar->pGroupInfo->pMember[DistributeI] && pChar->pGroupInfo->pMember[DistributeI]->pName)
											{
												if (!pChar->pGroupInfo->pMember[DistributeI]->Offline)
												{
													if (pChar->pGroupInfo->pMember[DistributeI]->pSpawn)
													{
														GetCXStr(pChar->pGroupInfo->pMember[DistributeI]->pName, DistributeName, MAX_STRING);
														//Attempting to give to someone in my group
														if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: SList: Attempting to give \ag%s\ax to \ag%s\ax", pShareItem->Name, DistributeName); }
														LONG LootInd = k + 1;
														sprintf_s(Command, "/advloot shared %d giveto %s", LootInd, DistributeName);
														DoCommand(GetCharInfo()->pSpawn, Command);
														return;
													}
												}
											}
											else
											{
												// Had less then 6 in the group and no one wanted the item
												if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: SList: No one wanted \ag%s\ax setting to leave", pShareItem->Name); }
												LONG LootInd = k + 1;
												sprintf_s(Command, "/advloot shared %d leave", LootInd);
												DoCommand(GetCharInfo()->pSpawn, Command);
												StartDistributeLootTimer = true;
												return;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

// This is called when we receive the EQ_BEGIN_ZONE packet is received
PLUGIN_API VOID BeginZone(VOID)
{
	DebugSpewAlways("MQ2AutoLoot::BeginZone");
}

// This is called when we receive the EQ_END_ZONE packet is received
PLUGIN_API VOID EndZone(VOID)
{
	DebugSpewAlways("MQ2AutoLoot::EndZone");
}
// This is called when pChar!=pCharOld && We are NOT zoning
// honestly I have no idea if its better to use this one or EndZone (above)
PLUGIN_API VOID Zoned(VOID)
{
	DebugSpewAlways("MQ2AutoLoot::Zoned");
}

PMQPLUGIN Plugin(char* PluginName) {
	long Length = strlen(PluginName) + 1;
	PMQPLUGIN pLook = pPlugins;
	while (pLook && _strnicmp(PluginName, pLook->szFilename, Length)) pLook = pLook->pNext;
	return pLook;
}

bool HandleMoveUtils(void)
{
	bMUPointers = false;
	fStickCommand = NULL;
	pbStickOn = NULL;
	if (PMQPLUGIN pLook = Plugin("mq2moveutils"))
	{
		fStickCommand = (void(*)(PSPAWNINFO pChar, char* szLine))GetProcAddress(pLook->hModule, "StickCommand");
		pbStickOn = (bool *)GetProcAddress(pLook->hModule, "bStickOn");
	}
	if (fStickCommand && pbStickOn)
	{
		bMUPointers = true;
		return true;
	}
	return false;
}

LONG SetBOOL(long Cur, PCHAR Val, PCHAR Sec, PCHAR Key, PCHAR INI)
{
	long result = 0;
	if (!_strnicmp(Val, "false", 5) || !_strnicmp(Val, "off", 3) || !_strnicmp(Val, "0", 1))
	{
		result = 0;
	}
	else if (!_strnicmp(Val, "true", 4) || !_strnicmp(Val, "on", 2) || !_strnicmp(Val, "1", 1))
	{
		result = 1;
	}
	else
	{
		result = (!Cur) & 1;
	}
	if (Sec[0] && Key[0])
	{
		WritePrivateProfileString(Sec, Key, result ? "1" : "0", INI);
	}
	return result;
}

BOOL DoIHaveSpace(CHAR* pszItemName, DWORD pdMaxStackSize, DWORD pdStackSize)
{
	BOOL FitInStack = false;
	LONG nPack = 0;
	LONG Count = 0;
	PCHARINFO pCharInfo = GetCharInfo();
	PCHARINFO2 pChar2 = GetCharInfo2();

	if (pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray) //check my inventory slots
	{
		for (unsigned long nSlot = BAG_SLOT_START; nSlot < NUM_INV_SLOTS; nSlot++)
		{
			if (PCONTENTS pItem = pChar2->pInventoryArray->InventoryArray[nSlot])
			{
				if (PITEMINFO theitem = GetItemFromContents(pItem))
				{
					if (!_stricmp(pszItemName, theitem->Name))
					{
						if ((theitem->Type != ITEMTYPE_NORMAL) || (((EQ_Item*)pItem)->IsStackable() != 1))
						{

						}
						else
						{
							if (pdStackSize + pItem->StackCount <= pdMaxStackSize)
							{
								FitInStack = true;
							}
						}
					}
				}
			}
			else
			{
				Count++;
			}
		}
	}
	if (pChar2 && pChar2->pInventoryArray) //Checking my bags
	{
		for (unsigned long nPack = 0; nPack < 10; nPack++)
		{
			if (PCONTENTS pPack = pChar2->pInventoryArray->Inventory.Pack[nPack])
			{
				if (PITEMINFO pItemPack = GetItemFromContents(pPack))
				{
					if (pItemPack->Type == ITEMTYPE_PACK && (_stricmp(pItemPack->Name, ExcludedBag1) || _stricmp(pItemPack->Name, ExcludedBag2)))
					{
						if (pPack->Contents.ContainedItems.pItems)
						{
							for (unsigned long nItem = 0; nItem < pItemPack->Slots; nItem++)
							{
								if (PCONTENTS pItem = pPack->Contents.ContainedItems.pItems->Item[nItem])
								{
									if (PITEMINFO theitem = GetItemFromContents(pItem))
									{
										if (!_stricmp(pszItemName, theitem->Name))
										{
											if ((theitem->Type != ITEMTYPE_NORMAL) || (((EQ_Item*)pItem)->IsStackable() != 1))
											{
											}
											else
											{
												if (pdStackSize + pItem->StackCount <= pdMaxStackSize)
												{
													FitInStack = true;
												}
											}
										}
									}
								}
								else
								{
									Count++;
								}
							}
						}
						else
						{
							Count = Count + pItemPack->Slots;
						}
					}
				}
			}
		}
	}
	if (Count > SaveBagSlots)
	{
		return true;
	}
	else if (FitInStack)
	{
		return true;
	}
	return false;
}

BOOL FitInInventory(DWORD pdItemSize)
{
	BOOL FitInStack = false;
	LONG nPack = 0;
	LONG Count = 0;
	PCHARINFO pCharInfo = GetCharInfo();
	PCHARINFO2 pChar2 = GetCharInfo2();
	if (pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray) //check my inventory slots
	{
		for (unsigned long nSlot = BAG_SLOT_START; nSlot < NUM_INV_SLOTS; nSlot++)
		{
			if (PCONTENTS pItem = pChar2->pInventoryArray->InventoryArray[nSlot])
			{

			}
			else
			{
				return true;
			}
		}
	}
	if (pChar2 && pChar2->pInventoryArray) //Checking my bags
	{
		for (unsigned long nPack = 0; nPack < 10; nPack++)
		{
			if (PCONTENTS pPack = pChar2->pInventoryArray->Inventory.Pack[nPack])
			{
				if (PITEMINFO pItemPack = GetItemFromContents(pPack))
				{
					if (pItemPack->Type == ITEMTYPE_PACK && (_stricmp(pItemPack->Name, ExcludedBag1) || _stricmp(pItemPack->Name, ExcludedBag2)))
					{
						if (pPack->Contents.ContainedItems.pItems)
						{
							for (unsigned long nItem = 0; nItem < pItemPack->Slots; nItem++)
							{
								if (PCONTENTS pItem = pPack->Contents.ContainedItems.pItems->Item[nItem])
								{

								}
								else
								{
									if (pItemPack->SizeCapacity >= pdItemSize)
									{
										return true;
									}
								}
							}
						}
						else
						{
							if (pItemPack->SizeCapacity >= pdItemSize)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

BOOL FitInBank(DWORD pdItemSize)
{
	BOOL FitInStack = false;
	LONG nPack = 0;
	LONG Count = 0;
	PCHARINFO pCharInfo = GetCharInfo();

	for (nPack = 0; nPack < NUM_BANK_SLOTS; nPack++)
	{
		if (pCharInfo->pBankArray)
		{
			if (PCONTENTS pItem = pCharInfo->pBankArray->Bank[nPack])
			{
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	//checking inside bank bags
	for (nPack = 0; nPack < NUM_BANK_SLOTS; nPack++) //checking bank slots
	{
		if (pCharInfo->pBankArray)
		{
			if (PCONTENTS pPack = pCharInfo->pBankArray->Bank[nPack])
			{
				if (PITEMINFO pItemPack = GetItemFromContents(pPack))
				{
					if (pItemPack->Type == ITEMTYPE_PACK && (_stricmp(pItemPack->Name, ExcludedBag1) || _stricmp(pItemPack->Name, ExcludedBag2)))
					{
						if (pPack->Contents.ContainedItems.pItems)
						{
							for (unsigned long nItem = 0; nItem < pItemPack->Slots; nItem++)
							{
								if (PCONTENTS pItem = pPack->Contents.ContainedItems.pItems->Item[nItem])
								{

								}
								else
								{
									if (pItemPack->SizeCapacity >= pdItemSize)
									{
										return true;
									}
								}
							}
						}
						else
						{
							if (pItemPack->SizeCapacity >= pdItemSize)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

int CheckIfItemIsLoreByID(int ItemID)
{
	LONG nPack = 0;
	DWORD nAug = 0;
	PCHARINFO pCharInfo = GetCharInfo();
	PCHARINFO2 pChar2 = GetCharInfo2();
	//check my inventory slots
	if (pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray)
	{
		for (unsigned long nSlot = 0; nSlot < NUM_INV_SLOTS; nSlot++)
		{
			if (PCONTENTS pItem = pChar2->pInventoryArray->InventoryArray[nSlot])
			{
				if (ItemID == GetItemFromContents(pItem)->ItemNumber)
				{
					return pItem->Item2->Lore;
				}
				else // for augs
				{
					if (pItem->Contents.ContainedItems.pItems && pItem->Contents.ContainedItems.Size)
					{
						for (nAug = 0; nAug < pItem->Contents.ContainedItems.Size; nAug++)
						{
							if (pItem->Contents.ContainedItems.pItems->Item[nAug])
							{
								if (PITEMINFO pAugItem = GetItemFromContents(pItem->Contents.ContainedItems.pItems->Item[nAug]))
								{
									if (pAugItem->Type == ITEMTYPE_NORMAL && pAugItem->AugType && ItemID == pAugItem->ItemNumber)
									{
										return pAugItem->Lore;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	//check cursor
	if (pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->Inventory.Cursor)
	{
		if (PCONTENTS pItem = pChar2->pInventoryArray->Inventory.Cursor)
		{
			if (ItemID == GetItemFromContents(pItem)->ItemNumber)
			{
				return pItem->Item2->Lore;
			}
			else // for augs
			{
				if (pItem->Contents.ContainedItems.pItems && pItem->Contents.ContainedItems.Size)
				{
					for (nAug = 0; nAug < pItem->Contents.ContainedItems.Size; nAug++)
					{
						if (pItem->Contents.ContainedItems.pItems->Item[nAug])
						{
							if (PITEMINFO pAugItem = GetItemFromContents(pItem->Contents.ContainedItems.pItems->Item[nAug]))
							{
								if (pAugItem->Type == ITEMTYPE_NORMAL && pAugItem->AugType && ItemID == pAugItem->ItemNumber)
								{
									return pAugItem->Lore;
								}
							}
						}
					}
				}
			}
		}
	}
	//check in my bags
	if (pChar2 && pChar2->pInventoryArray) {
		for (unsigned long nPack = 0; nPack < 10; nPack++)
		{
			if (PCONTENTS pPack = pChar2->pInventoryArray->Inventory.Pack[nPack])
			{
				if (GetItemFromContents(pPack)->Type == ITEMTYPE_PACK && pPack->Contents.ContainedItems.pItems)
				{
					for (unsigned long nItem = 0; nItem < GetItemFromContents(pPack)->Slots; nItem++)
					{
						if (PCONTENTS pItem = pPack->GetContent(nItem))
						{
							if (ItemID == GetItemFromContents(pItem)->ItemNumber)
							{
								return pItem->Item2->Lore;
							}
							else // for augs
							{
								if (pItem->Contents.ContainedItems.pItems && pItem->Contents.ContainedItems.Size)
								{
									for (nAug = 0; nAug < pItem->Contents.ContainedItems.Size; nAug++)
									{
										if (pItem->Contents.ContainedItems.pItems->Item[nAug])
										{
											if (PITEMINFO pAugItem = GetItemFromContents(pItem->Contents.ContainedItems.pItems->Item[nAug]))
											{
												if (pAugItem->Type == ITEMTYPE_NORMAL && pAugItem->AugType && ItemID == pAugItem->ItemNumber)
												{
													return pAugItem->Lore;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	//checking bank slots
	for (nPack = 0; nPack < NUM_BANK_SLOTS; nPack++)
	{
		if (pCharInfo->pBankArray)
		{
			if (PCONTENTS pItem = pCharInfo->pBankArray->Bank[nPack])
			{
				if (ItemID == GetItemFromContents(pItem)->ItemNumber)
				{
					return pItem->Item2->Lore;
				}
				else // for augs
				{
					if (pItem->Contents.ContainedItems.pItems && pItem->Contents.ContainedItems.Size)
					{
						for (nAug = 0; nAug < pItem->Contents.ContainedItems.Size; nAug++)
						{
							if (pItem->Contents.ContainedItems.pItems->Item[nAug])
							{
								if (PITEMINFO pAugItem = GetItemFromContents(pItem->Contents.ContainedItems.pItems->Item[nAug]))
								{
									if (pAugItem->Type == ITEMTYPE_NORMAL && pAugItem->AugType && ItemID == pAugItem->ItemNumber)
									{
										return pAugItem->Lore;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	//checking inside bank bags
	for (nPack = 0; nPack < NUM_BANK_SLOTS; nPack++) //checking bank slots
	{
		if (pCharInfo->pBankArray)
		{
			if (PCONTENTS pPack = pCharInfo->pBankArray->Bank[nPack])
			{
				if (PITEMINFO theitem = GetItemFromContents(pPack))
				{
					if (theitem->Type == ITEMTYPE_PACK && pPack->Contents.ContainedItems.pItems) //checking bank bags
					{
						for (unsigned long nItem = 0; nItem < theitem->Slots; nItem++)
						{
							if (PCONTENTS pItem = pPack->Contents.ContainedItems.pItems->Item[nItem])
							{
								if (ItemID == GetItemFromContents(pItem)->ItemNumber)
								{
									return pItem->Item2->Lore;
								}
								else // for augs
								{
									if (pItem->Contents.ContainedItems.pItems && pItem->Contents.ContainedItems.Size)
									{
										for (nAug = 0; nAug < pItem->Contents.ContainedItems.Size; nAug++)
										{
											if (pItem->Contents.ContainedItems.pItems->Item[nAug])
											{
												if (PITEMINFO pAugItem = GetItemFromContents(pItem->Contents.ContainedItems.pItems->Item[nAug]))
												{
													if (pAugItem->Type == ITEMTYPE_NORMAL && pAugItem->AugType && ItemID == pAugItem->ItemNumber)
													{
														return pAugItem->Lore;
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

#ifndef EMU
	PCHARINFO pChar = GetCharInfo();
	if (pChar && pChar->pMountsArray && pChar->pMountsArray->Mounts)
	{
		for (unsigned long nSlot = 0; nSlot < MAX_KEYRINGITEMS; nSlot++)
		{
			if (PCONTENTS pItem = pChar->pMountsArray->Mounts[nSlot])
			{
				if (ItemID == GetItemFromContents(pItem)->ItemNumber)
				{
					return pItem->Item2->Lore;
				}
			}
		}
	}
	if (pChar && pChar->pIllusionsArray && pChar->pIllusionsArray->Illusions)
	{
		for (unsigned long nSlot = 0; nSlot < MAX_KEYRINGITEMS; nSlot++)
		{
			if (PCONTENTS pItem = pChar->pIllusionsArray->Illusions[nSlot])
			{
				if (ItemID == GetItemFromContents(pItem)->ItemNumber)
				{
					return pItem->Item2->Lore;
				}
			}
		}
	}
	if (pChar && pChar->pFamiliarArray && pChar->pFamiliarArray->Familiars)
	{
		for (unsigned long nSlot = 0; nSlot < MAX_KEYRINGITEMS; nSlot++)
		{
			if (PCONTENTS pItem = pChar->pFamiliarArray->Familiars[nSlot])
			{
				if (ItemID == GetItemFromContents(pItem)->ItemNumber)
				{
					return pItem->Item2->Lore;
				}
			}
		}
	}
#endif

	return 0;

}

DWORD FindItemCount(CHAR* pszItemName)
{
	LONG nPack = 0;
	DWORD Count = 0;
	DWORD nAug = 0;
	PCONTENTS pPack;
	PCHARINFO pCharInfo = GetCharInfo();
	PCHARINFO2 pChar2 = GetCharInfo2();
	if (pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray) //check my inventory slots
	{
		for (unsigned long nSlot = 0; nSlot < NUM_INV_SLOTS; nSlot++)
		{
			if (PCONTENTS pItem = pChar2->pInventoryArray->InventoryArray[nSlot])
			{
				if (PITEMINFO theitem = GetItemFromContents(pItem))
				{
					if (!_stricmp(pszItemName, theitem->Name))
					{
						if ((theitem->Type != ITEMTYPE_NORMAL) || (((EQ_Item*)pItem)->IsStackable() != 1))
							Count++;
						else
							Count += pItem->StackCount;
					}
					else // for augs
					{
						if (pItem->Contents.ContainedItems.pItems && pItem->Contents.ContainedItems.Size)
						{
							for (nAug = 0; nAug < pItem->Contents.ContainedItems.Size; nAug++)
							{
								if (pItem->Contents.ContainedItems.pItems->Item[nAug])
								{
									if (PITEMINFO augitem = GetItemFromContents(pItem->Contents.ContainedItems.pItems->Item[nAug]))
									{
										if (augitem->Type == ITEMTYPE_NORMAL && augitem->AugType && !_stricmp(pszItemName, augitem->Name))
										{
											Count++;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (pChar2 && pChar2->pInventoryArray) //Checking my bags
	{
		for (unsigned long nPack = 0; nPack < 10; nPack++)
		{
			if (PCONTENTS pPack = pChar2->pInventoryArray->Inventory.Pack[nPack])
			{
				if (PITEMINFO pItemPack = GetItemFromContents(pPack))
				{
					if (pItemPack->Type == ITEMTYPE_PACK && pPack->Contents.ContainedItems.pItems)
					{
						for (unsigned long nItem = 0; nItem < pItemPack->Slots; nItem++)
						{
							if (PCONTENTS pItem = pPack->Contents.ContainedItems.pItems->Item[nItem])
							{
								if (PITEMINFO theitem = GetItemFromContents(pItem))
								{
									if (!_stricmp(pszItemName, theitem->Name))
									{
										if ((theitem->Type != ITEMTYPE_NORMAL) || (((EQ_Item*)pItem)->IsStackable() != 1))
											Count++;
										else
											Count += pItem->StackCount;
									}
									else //check for augs
									{
										if (pItem->Contents.ContainedItems.pItems && pItem->Contents.ContainedItems.Size)
										{
											for (nAug = 0; nAug < pItem->Contents.ContainedItems.Size; nAug++)
											{
												if (pItem->Contents.ContainedItems.pItems->Item[nAug])
												{
													if (PITEMINFO pAug = GetItemFromContents(pItem->Contents.ContainedItems.pItems->Item[nAug]))
													{
														if (pAug->Type == ITEMTYPE_NORMAL && pAug->AugType && !_stricmp(pszItemName, pAug->Name))
														{
															Count++;
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	for (nPack = 0; nPack < NUM_BANK_SLOTS; nPack++) //checking bank slots
	{
		if (pCharInfo->pBankArray && (pPack = pCharInfo->pBankArray->Bank[nPack]))
		{
			if (PITEMINFO theitem = GetItemFromContents(pPack))
			{
				if (!_stricmp(pszItemName, theitem->Name))
				{
					if ((theitem->Type != ITEMTYPE_NORMAL) || (((EQ_Item*)pPack)->IsStackable() != 1))
						Count++;
					else
						Count += pPack->StackCount;
				}
				if (theitem->Type == ITEMTYPE_PACK && pPack->Contents.ContainedItems.pItems) //checking bank bags
				{
					for (unsigned long nItem = 0; nItem < theitem->Slots; nItem++)
					{
						if (PCONTENTS pItem = pPack->Contents.ContainedItems.pItems->Item[nItem])
						{
							if (PITEMINFO theitem = GetItemFromContents(pItem)) {
								if (!_stricmp(pszItemName, theitem->Name))
								{
									if ((GetItemFromContents(pItem)->Type != ITEMTYPE_NORMAL) || (((EQ_Item*)pItem)->IsStackable() != 1))
									{
										Count++;
									}
									else
									{
										Count += pItem->StackCount;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	for (nPack = 0; nPack < NUM_SHAREDBANK_SLOTS; nPack++) //checking shared bank slots
	{
		if (pCharInfo->pSharedBankArray && (pPack = pCharInfo->pSharedBankArray->SharedBank[nPack]))
		{
			if (PITEMINFO theitem = GetItemFromContents(pPack))
			{
				if (!_stricmp(pszItemName, theitem->Name))
				{
					if ((theitem->Type != ITEMTYPE_NORMAL) || (((EQ_Item*)pPack)->IsStackable() != 1))
						Count++;
					else
						Count += pPack->StackCount;
				}
				if (theitem->Type == ITEMTYPE_PACK && pPack->Contents.ContainedItems.pItems) //checking shared bank slots
				{
					for (unsigned long nItem = 0; nItem < theitem->Slots; nItem++)
					{
						if (PCONTENTS pItem = pPack->Contents.ContainedItems.pItems->Item[nItem])
						{
							if (PITEMINFO pStuff = GetItemFromContents(pItem))
							{
								if (!_stricmp(pszItemName, pStuff->Name))
								{
									if ((pStuff->Type != ITEMTYPE_NORMAL) || (((EQ_Item*)pItem)->IsStackable() != 1))
									{
										Count++;
									}
									else
									{
										Count += pItem->StackCount;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return Count;
}

DWORD FindBarterItemCount(CHAR* pszItemName)
{
	LONG nPack = 0;
	DWORD Count = 0;
	DWORD nAug = 0;
	PCHARINFO pCharInfo = GetCharInfo();
	PCHARINFO2 pChar2 = GetCharInfo2();
	if (pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray) //check my inventory slots
	{
		for (unsigned long nSlot = 0; nSlot < NUM_INV_SLOTS; nSlot++)
		{
			if (PCONTENTS pItem = pChar2->pInventoryArray->InventoryArray[nSlot])
			{
				if (PITEMINFO theitem = GetItemFromContents(pItem))
				{
					if (!_stricmp(pszItemName, theitem->Name))
					{
						if ((theitem->Type != ITEMTYPE_NORMAL) || (((EQ_Item*)pItem)->IsStackable() != 1))
							Count++;
						else
							Count += pItem->StackCount;
					}
				}
			}
		}
	}
	if (pChar2 && pChar2->pInventoryArray) //Checking my bags
	{
		for (unsigned long nPack = 0; nPack < 10; nPack++)
		{
			if (PCONTENTS pPack = pChar2->pInventoryArray->Inventory.Pack[nPack])
			{
				if (PITEMINFO pItemPack = GetItemFromContents(pPack))
				{
					if (pItemPack->Type == ITEMTYPE_PACK && pPack->Contents.ContainedItems.pItems)
					{
						for (unsigned long nItem = 0; nItem < pItemPack->Slots; nItem++)
						{
							if (PCONTENTS pItem = pPack->Contents.ContainedItems.pItems->Item[nItem])
							{
								if (PITEMINFO theitem = GetItemFromContents(pItem))
								{
									if (!_stricmp(pszItemName, theitem->Name))
									{
										if ((theitem->Type != ITEMTYPE_NORMAL) || (((EQ_Item*)pItem)->IsStackable() != 1))
											Count++;
										else
											Count += pItem->StackCount;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return Count;
}

void DoBarterStuff(CHAR* szAction)
{
	
	CHAR INISection[MAX_STRING] = { 0 };
	CHAR Value[MAX_STRING] = { 0 };
	PCHARINFO pChar = GetCharInfo();
	PCHARINFO2 pChar2 = GetCharInfo2();
	// This will keep parts of the DoBarterStuff from getting stuck, it is given 30 seconds to open the barter window and then 20 minutes to finish bartering your stuff 
	if (std::chrono::high_resolution_clock::now() > LootStuffCancelTimer)
	{
		WriteChatf(PLUGIN_MSG ":: Barter timer ran out, terminating the bartering of your items.");
		StartLootStuff = false;
		return;
	}
	if (!HasExpansion(EXPANSION_RoF))
	{
		WriteChatf(PLUGIN_MSG ":: You need to have least Rain of Fear expansion to use the barter functionality.");
	}
	// Getting ready to barter by opening the barter window open
	if (!_stricmp(szAction, "Barter"))
	{
		if (!WinState((CXWnd*)FindMQ2Window("BarterSearchWnd")))  // Barter window aren't open
		{
			if (LootStuffWindowOpen)  // You already had these windows open and manually shut them down, I am ending this shit
			{
				StartLootStuff = false;
				return;
			}
			if (StartToOpenWindow)
			{
				StartToOpenWindow = false;
				DoCommand(GetCharInfo()->pSpawn, "/barter");
				LootStuffCancelTimer = std::chrono::high_resolution_clock::now() +  + std::chrono::seconds(30);; // Will stop trying to move to open the merchant/banker window after 30 seconds
				WriteChatf(PLUGIN_MSG ":: Opening a barter window!");
			}
			LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::seconds(5);
			return;
		}
	}
	else
	{
		// There is no reason this should ever be called, 
		WriteChatf(PLUGIN_MSG ":: Something has epically failed!!!, the action passed was to the DoBarterStuff function was \ar%s\ax.", szAction);
		StartLootStuff = false;
		return;
	}

	// Looping through the barter window to find if I want to sell stuff
	if (WinState((CXWnd*)FindMQ2Window("BarterSearchWnd")))
	{
		LootStuffWindowOpen = true;
		if (StarBarterTimer)
		{
			LootStuffCancelTimer = std::chrono::high_resolution_clock::now() + std::chrono::minutes(20); // Will stop trying to barter items after 20 minutes
			StarBarterTimer = false;
		}
		if (CListWnd *cLWnd = (CListWnd *)FindMQ2Window("BarterSearchWnd")->GetChildItem("BTRSRCH_InventoryList"))
		{
			for (int nBarterItems = BarterIndex; nBarterItems < cLWnd->ItemsArray.GetLength(); nBarterItems++)
			{
				CXStr	cxstrItemName;
				CXStr	cxstrItemCount;
				CXStr	cxstrItemPrice;
				CHAR	szItemName[MAX_STRING] = { 0 };
				CHAR	szItemPrice[MAX_STRING] = { 0 };
				CHAR	szItemCount[MAX_STRING] = { 0 }; 
				CHAR	INISection[MAX_STRING] = { 0 };
				CHAR	Value[MAX_STRING] = { 0 };
				DWORD	MyCount;
				DWORD	SellCount;
				DWORD	BarterCount;
				DWORD	MyBarterMinimum;
				DWORD	BarterPrice = 0;
				DWORD	BarterMaximumPrice = 0;
				int		BarterMaximumIndex = 0;
				if (cLWnd->GetItemText(&cxstrItemName, nBarterItems, 1))
				{
					GetCXStr(cxstrItemName.Ptr, szItemName, MAX_STRING);
					sprintf_s(INISection, "%c", szItemName[0]);
					bool IWant = false;  // Will be set true if you want and can accept the item
					bool IDoNotWant = false;  // Will be set true if you don't want or can't accept
					bool CheckIfOthersWant = false;  // Will be set true if I am ML and I can't accept or don't need
					if (GetPrivateProfileString(INISection, szItemName, 0, Value, MAX_STRING, LootINI) != 0)
					{
						CHAR *pParsedToken = NULL;
						CHAR *pParsedValue = strtok_s(Value, "|", &pParsedToken);
						if (!_stricmp(pParsedValue, "Barter"))
						{
							pParsedValue = strtok_s(NULL, "|", &pParsedToken);
							if (!pParsedValue)
							{
								// Hey I think this means that the item is in my loot.ini and it is set to Barter but with no delineater
								BarterIndex++;
								WriteChatf(PLUGIN_MSG ":: Barter index:%d and the item name is %s and it's loot.ini entry is %s", BarterIndex, szItemName, Value);
							}
							else
							{
								//TODO add safety checks to make sure it is a number etc
								MyBarterMinimum = atoi(pParsedValue);
								if (LootStuffN == 1)
								{
									WriteChatf(PLUGIN_MSG ":: For entry %i, the item's name is %s and I will sell for: %d platinum", nBarterItems + 1, szItemName, MyBarterMinimum);
									sprintf_s(Command, "/nomodkey /notify BarterSearchWnd BTRSRCH_InventoryList listselect %i", nBarterItems+1);
									DoCommand(GetCharInfo()->pSpawn, Command);
									LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(1000);
									LootStuffN = 2;
									return;
								}
								else if (LootStuffN == 2)
								{
									if (cLWnd->GetCurSel() == nBarterItems) // not sure what this means, but it seems to be -1 when it has something selected
									{
										if (CXWnd *pWndButton = FindMQ2Window("BarterSearchWnd")->GetChildItem("BTRSRCH_SearchButton"))
										{
											if (pWndButton->Enabled)
											{
												SendWndClick2(pWndButton, "leftmouseup");
												LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(5000);
												return;
											}
										}
									}
									return;
								}
								else if (LootStuffN == 3)
								{
									if (FindBarterItemCount(szItemName) == 0)
									{
										BarterIndex++;
										LootStuffN = 1;
										return;
									}
									if (CListWnd *cLWnd = (CListWnd *)FindMQ2Window("BarterSearchWnd")->GetChildItem("BTRSRCH_BuyLineList"))
									{
										if (cLWnd->ItemsArray.GetLength() > 0)
										{
											for (int nBarterItems = 0; nBarterItems < cLWnd->ItemsArray.GetLength(); nBarterItems++)
											{
												if (cLWnd->GetItemText(&cxstrItemPrice, nBarterItems, 3))
												{
													GetCXStr(cxstrItemPrice.Ptr, szItemPrice, MAX_STRING);
													char* pch;
													pch = strstr(szItemPrice, "p");
													strcpy_s(pch, 1, "\0");
													puts(szItemPrice);
													BarterPrice = atoi(szItemPrice);
													if (BarterPrice > BarterMaximumPrice)
													{
														BarterMaximumPrice = BarterPrice;
														BarterMaximumIndex = nBarterItems;
													}
												}
											}
											if (BarterMaximumPrice < MyBarterMinimum || BarterMaximumPrice < (DWORD)BarMinSellPrice)
											{
												// the maximum price is less then my minimum price i am moving to the next item
												BarterIndex++;
												LootStuffN = 1;
												return;
											}
											if (cLWnd->GetCurSel() != BarterMaximumIndex) 
											{
												cLWnd->SetCurSel(BarterMaximumIndex);
												LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(100);
												return;
											}
											else
											{
												sprintf_s(Command, "/nomodkey /notify BarterSearchWnd SellButton leftmouseup");
												DoCommand(GetCharInfo()->pSpawn, Command);
												LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(0);
												LootStuffN = 4;
												return;
											}
										}
										else
										{
											// Hey I think this means that no one wants to buy the item, TODO verify this shit
											BarterIndex++;
											LootStuffN = 1;
											return;
										}
									}
									//somehow this part of the loop failed, going to keep attempting to redo it till it succeeds or barter times out
									return;
								}
								else if (LootStuffN == 4)
								{
									if (CListWnd *cLWnd = (CListWnd *)FindMQ2Window("BarterSearchWnd")->GetChildItem("BTRSRCH_BuyLineList"))
									{
										if (cLWnd->ItemsArray.GetLength() > 0)
										{
											for (int nBarterItems = 0; nBarterItems < cLWnd->ItemsArray.GetLength(); nBarterItems++)
											{
												if (cLWnd->GetItemText(&cxstrItemPrice, nBarterItems, 3))
												{
													GetCXStr(cxstrItemPrice.Ptr, szItemPrice, MAX_STRING);
													char* pch;
													pch = strstr(szItemPrice, "p");
													strcpy_s(pch, 1, "\0");
													puts(szItemPrice);
													BarterPrice = atoi(szItemPrice);
													if (BarterPrice > BarterMaximumPrice)
													{
														BarterMaximumPrice = BarterPrice;
														BarterMaximumIndex = nBarterItems;
													}
												}
											}
											if (WinState((CXWnd*)pQuantityWnd))
											{
												if (cLWnd->GetItemText(&cxstrItemCount, BarterMaximumIndex, 2))
												{
													GetCXStr(cxstrItemCount.Ptr, szItemCount, MAX_STRING);
													BarterCount = atoi(szItemCount);
												}
												else
												{
													// This shouldn't fail, TODO verify this shit
													BarterIndex++;
													WriteChatf(PLUGIN_MSG ":: Whoa this shouldn't fail, somehow this person is selling %s but has none to sell", szItemName);
													LootStuffN = 1;
													return;
												}
												MyCount = FindBarterItemCount(szItemName);
												WriteChatf(PLUGIN_MSG ":: My count is %d, and the barter count is %d for %s", MyCount, BarterCount, szItemName);
												if (MyCount == 0)
												{
													// This shouldn't fail, TODO verify this shit
													BarterIndex++;
													LootStuffN = 1;
													return;
												}
												if (MyCount >= 20 && BarterCount >= 20)
												{
													SellCount = 20;
												}
												else if (MyCount >= BarterCount)
												{
													SellCount = BarterCount;
												}
												else
												{
													SellCount = MyCount;
												}
												sprintf_s(Command, "/nomodkey /notify QuantityWnd QTYW_slider newvalue %i", SellCount);
												DoCommand(GetCharInfo()->pSpawn, Command);
												LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(0);
												LootStuffN = 5;
												return;
											}
										}
									}
									//somehow this part of the loop failed, going to keep attempting to redo it till it succeeds or barter times out
									return;
								}
								else if (LootStuffN == 5)
								{
									if (WinState((CXWnd*)pQuantityWnd))
									{
										if (CXWnd *pWndButton = pQuantityWnd->GetChildItem("QTYW_Accept_Button"))
										{
											WriteChatf(PLUGIN_MSG ":: Selling %s", szItemName);
											SendWndClick2(pWndButton, "leftmouseup");
											LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(10000);
											LootStuffN = 3;
											return;
										}
									}
									//somehow this part of the loop failed, going to keep attempting to redo it till it succeeds or barter times out
									return;
								}
							}
						}
						else
						{
							// Hey I think this means that the item is in my loot.ini, but that isn't set to Barter
							BarterIndex++;
							WriteChatf(PLUGIN_MSG ":: Barter index:%d and the item name is %s and it's loot.ini entry is %s", BarterIndex, szItemName, Value);
							return;
						}
					}
					else
					{
						// Hey I think this means the item isn't in my loot.ini, TODO verify this shit
						BarterIndex++;
						WriteChatf(PLUGIN_MSG ":: Barter index:%d and the item name is %s", BarterIndex, szItemName);
						return;
					}
				}
			}
		}
	}
	if (WinState((CXWnd*)FindMQ2Window("BarterSearchWnd")))
	{
		WriteChatf(PLUGIN_MSG ":: Closing barter window");
		DoCommand(GetCharInfo()->pSpawn, "/squelch /windowstate BarterSearchWnd close");
		StartLootStuff = false;
		return;
	}
}

void DestroyStuff()
{
	PCHARINFO pChar = GetCharInfo();
	PCHARINFO2 pChar2 = GetCharInfo2();
	if (std::chrono::high_resolution_clock::now() > DestroyStuffCancelTimer)
	{
		if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: Destroying timer ran out, moving on!"); }
		DestroyID = 0;
		return;
	}
	// Looping through the items in my inventory and seening if I want to sell/deposit them based on which window was open
	if (pChar2->pInventoryArray && pChar2->pInventoryArray->Inventory.Cursor)
	{
		if (PCONTENTS pItem = pChar2->pInventoryArray->Inventory.Cursor)
		{
			if (pItem->ID == DestroyID)
			{
				if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: Destroying \ar%s\ax!", pItem->Item2->Name); }
				DoCommand(GetCharInfo()->pSpawn, "/destroy");
				DestroyID = 0;
				return;
			}
		}
	}
	if (pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray) //check my inventory slots
	{
		for (unsigned long nSlot = BAG_SLOT_START; nSlot < NUM_INV_SLOTS; nSlot++)
		{
			if (PCONTENTS pItem = pChar2->pInventoryArray->InventoryArray[nSlot])
			{
				if (pItem->ID == DestroyID)
				{
					sprintf_s(Command, "/nomodkey /itemnotify \"%s\" leftmouseup", pItem->Item2->Name);
					DoCommand(GetCharInfo()->pSpawn, Command);
					DestroyStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(100);
					return;
				}
			}
		}
	}
	if (pChar2 && pChar2->pInventoryArray) //Checking my bags
	{
		for (unsigned long nPack = 0; nPack < 10; nPack++)
		{
			if (PCONTENTS pPack = pChar2->pInventoryArray->Inventory.Pack[nPack])
			{
				if (PITEMINFO pItemPack = GetItemFromContents(pPack))
				{
					if (pItemPack->Type == ITEMTYPE_PACK && pPack->Contents.ContainedItems.pItems)
					{
						for (unsigned long nItem = 0; nItem < pItemPack->Slots; nItem++)
						{
							if (PCONTENTS pItem = pPack->Contents.ContainedItems.pItems->Item[nItem])
							{
								if (pItem->ID == DestroyID)
								{
									sprintf_s(Command, "/nomodkey /itemnotify \"%s\" leftmouseup", pItem->Item2->Name);
									DoCommand(GetCharInfo()->pSpawn, Command);
									DestroyStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(100);
									return;
								}
							}
						}
					}
				}
			}
		}
	}
}

void DoLootStuff(CHAR* szAction)
{
	
	CHAR INISection[MAX_STRING] = { 0 };
	CHAR Value[MAX_STRING] = { 0 };
	PCHARINFO pChar = GetCharInfo();
	PCHARINFO2 pChar2 = GetCharInfo2();
	// This will keep parts of the DoLootStuff from looping forever if they get stuck at some step 
	// For depositing/selling you have 30 seconds to move within range, then another 30 seconds to open the window, and finally 20 minutes to finish all of your selling/depositing
	if (std::chrono::high_resolution_clock::now() > LootStuffCancelTimer)
	{
		if (!_stricmp(szAction, "Deposit")) { WriteChatf(PLUGIN_MSG ":: Deposit timer ran out, terminating the depositing of your items."); }
		if (!_stricmp(szAction, "Sell")) { WriteChatf(PLUGIN_MSG ":: Sell timer ran out, terminating the selling of your items."); }
		StartLootStuff = false;
		return;
	}
	if (!_stricmp(szAction, "Sell") || !_stricmp(szAction, "Deposit"))
	{
		if (!WinState((CXWnd*)pMerchantWnd) && !WinState((CXWnd*)pBankWnd) && !WinState((CXWnd*)FindMQ2Window("GuildBankWnd")))  // Neither bank or merchant window are open
		{
			if (LootStuffWindowOpen)  // You already had these windows open and manually shut them down, I am ending this shit
			{
				StartLootStuff = false;
				return;
			}
			if (PSPAWNINFO psTarget = (PSPAWNINFO)pTarget)
			{
				if (!_stricmp(szAction, "Sell") && psTarget->mActorClient.Class != MERCHANT_CLASS)
				{
					WriteChatf(PLUGIN_MSG ":: Please target a merchant!");
					StartLootStuff = false;
					return;
				}
				else if (!_stricmp(szAction, "Deposit") && psTarget->mActorClient.Class != PERSONALBANKER_CLASS && psTarget->mActorClient.Class != GUILDBANKER_CLASS)
				{
					WriteChatf(PLUGIN_MSG ":: Please target a banker!");
					StartLootStuff = false;
					return;
				}
				FLOAT Distance = Get3DDistance(pChar->pSpawn->X, pChar->pSpawn->Y, pChar->pSpawn->Z, psTarget->X, psTarget->Y, psTarget->Z);
				if (Distance >= 20)  // I am too far away from the merchant/banker and need to move closer 
				{
					if (StartMoveToTarget)
					{
						StartMoveToTarget = false;
						LootStuffCancelTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(30000); // Will stop trying to move to within range of the banker/merchant after 30 seconds
						if (!bMULoaded || !bMUPointers)
						{
							bMULoaded = HandleMoveUtils();
						}
						if (bMULoaded && bMUPointers)
						{
							sprintf_s(Command, "id %d", psTarget->SpawnID);
							fStickCommand(GetCharInfo()->pSpawn, Command);
							WriteChatf(PLUGIN_MSG ":: Moving towards: \ag%s\ax",psTarget->DisplayedName);
						}
						else
						{
							WriteChatf(PLUGIN_MSG ":: Hey friend, you don't have MQ2MoveUtils loaded.  Move to within 20 of the target before trying to sell or deposit");
							StartLootStuff = false;
							return;
						}
					}
					LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(100);
					return;
				}
				else
				{
					if (StartToOpenWindow)
					{
						if (!bMULoaded || !bMUPointers)
						{
							bMULoaded = HandleMoveUtils();
						}
						if (bMULoaded && bMUPointers)
						{
							if (*pbStickOn)
							{
								fStickCommand(GetCharInfo()->pSpawn, "off");
							}
						}
						StartToOpenWindow = false;
						if (!_stricmp(szAction, "Sell")) { DoCommand(GetCharInfo()->pSpawn, "/keypress OPEN_INV_BAGS"); } // Will remove this when /itemnotify works inside closed packs for merchant stuff
						LootStuffCancelTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(30000); // Will stop trying to move to open the merchant/banker window after 30 seconds
						if (!_stricmp(szAction, "Deposit")) { WriteChatf(PLUGIN_MSG ":: Opening banking window and waiting for the banking window to populate"); }
						if (!_stricmp(szAction, "Sell")) { WriteChatf(PLUGIN_MSG ":: Opening merchant window and waiting for the merchant window to populate"); }
					}
					DoCommand(GetCharInfo()->pSpawn, "/face nolook");
					DoCommand(GetCharInfo()->pSpawn, "/nomodkey /click right target");
					LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(30000);
					return;
				}
			}
			else
			{
				if (!_stricmp(szAction, "Deposit")) { WriteChatf(PLUGIN_MSG ":: Please target a banker!"); }
				if (!_stricmp(szAction, "Sell")) { WriteChatf(PLUGIN_MSG ":: Please target a merchant!"); }
				StartLootStuff = false;
				return;
			}
		}
	}
	else
	{
		// There is no reason this should ever be called, 
		WriteChatf(PLUGIN_MSG ":: Something has epically failed!!!, the action passed was to the DoLootStuff function was \ar%s\ax.", szAction);
		StartLootStuff = false;
		return;
	}

	// Looping through the items in my inventory and seening if I want to sell/deposit them based on which window was open
	if (WinState((CXWnd*)pMerchantWnd) || WinState((CXWnd*)pBankWnd) || WinState((CXWnd*)FindMQ2Window("GuildBankWnd")))  // Either bank or merchant window are open
	{
		LootStuffWindowOpen = true;
		if (pChar2->pInventoryArray && pChar2->pInventoryArray->Inventory.Cursor)
		{
			if (PCONTENTS pItem = pChar2->pInventoryArray->Inventory.Cursor)
			{
				if (WinState((CXWnd*)pBankWnd))
				{ 
					if (CXWnd *pWndButton = pBankWnd->GetChildItem("BIGB_AutoButton"))
					{
						if (FitInBank(pItem->Item2->Size))
						{
							if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: Putting \ag%s\ax into my personal bank", pItem->Item2->Name); }
							SendWndClick2(pWndButton, "leftmouseup");
							LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(100);
							return;
						}
					}
				}
				if (WinState((CXWnd*)FindMQ2Window("GuildBankWnd")))
				{ 
					if (CXWnd *pWnd = FindMQ2Window("GuildBankWnd")->GetChildItem("GBANK_DepositCountLabel"))
					{
						CHAR szDepositCountText[MAX_STRING] = { 0 }; 
						GetCXStr(pWnd->WindowText, szDepositCountText, MAX_STRING);
						CHAR *pParsedToken = NULL;
						CHAR *pParsedValue = strtok_s(szDepositCountText, ":", &pParsedToken);
						pParsedValue = strtok_s(NULL, ":", &pParsedToken);
						DWORD SlotsLeft = atoi(pParsedValue);
						if (SlotsLeft > 0)
						{
							if (CXWnd *pWndButton = FindMQ2Window("GuildBankWnd")->GetChildItem("GBANK_DepositButton"))
							{
								if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: Putting \ag%s\ax into my guild bank", pItem->Item2->Name); }
								SendWndClick2(pWndButton, "leftmouseup");
								LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(2000);
								return;
							}
						}
						else
						{
							WriteChatf(PLUGIN_MSG ":: Your guild bank ran out of space, closing guild bank window");
							DoCommand(GetCharInfo()->pSpawn, "/squelch /windowstate GuildBankWnd close");
							StartLootStuff = false;
							return;
						}
					}
				}
			}
		}
		if (pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->InventoryArray) //check my inventory slots
		{
			for (unsigned long nSlot = BAG_SLOT_START; nSlot < NUM_INV_SLOTS; nSlot++)
			{
				if (PCONTENTS pItem = pChar2->pInventoryArray->InventoryArray[nSlot])
				{
					if (PITEMINFO theitem = GetItemFromContents(pItem))
					{
						sprintf_s(INISection, "%c", theitem->Name[0]);
						if (GetPrivateProfileString(INISection, theitem->Name, 0, Value, MAX_STRING, LootINI) != 0)
						{
							CHAR *pParsedToken = NULL;
							CHAR *pParsedValue = strtok_s(Value, "|", &pParsedToken);
							if (!_stricmp(pParsedValue, "Keep") && !_stricmp(szAction, "Deposit"))
							{
								if (WinState((CXWnd*)pBankWnd))
								{ 
									if (FitInBank(theitem->Size))
									{
										sprintf_s(Command, "/nomodkey /itemnotify \"%s\" leftmouseup", theitem->Name);
										DoCommand(GetCharInfo()->pSpawn, Command);
										LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(100);
										return;
									}
								}
							}
							else if (!_stricmp(pParsedValue, "Deposit") && !_stricmp(szAction, "Deposit"))
							{
								if (WinState((CXWnd*)FindMQ2Window("GuildBankWnd")))
								{ 
									if (CXWnd *pWnd = FindMQ2Window("GuildBankWnd")->GetChildItem("GBANK_DepositCountLabel"))
									{
										CHAR szDepositCountText[MAX_STRING] = { 0 }; 
										GetCXStr(pWnd->WindowText, szDepositCountText, MAX_STRING);
										CHAR *pParsedToken = NULL;
										CHAR *pParsedValue = strtok_s(szDepositCountText, ":", &pParsedToken);
										pParsedValue = strtok_s(NULL, ":", &pParsedToken);
										DWORD SlotsLeft = atoi(pParsedValue);
										WriteChatf(PLUGIN_MSG ":: The number of items left that can be deposited is: %d", SlotsLeft);
										if (SlotsLeft > 0)
										{
											sprintf_s(Command, "/nomodkey /itemnotify \"%s\" leftmouseup", theitem->Name);
											DoCommand(GetCharInfo()->pSpawn, Command);
											LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(2000);
											return;
										}
										else
										{
											WriteChatf(PLUGIN_MSG ":: Your guild bank ran out of space, closing guild bank window");
											DoCommand(GetCharInfo()->pSpawn, "/squelch /windowstate GuildBankWnd close");
											StartLootStuff = false;
											return;
										}
									}
								}
							}
							else if (!_stricmp(pParsedValue, "Sell") && !_stricmp(szAction, "Sell"))
							{
								if (theitem->NoDrop &&  theitem->Cost > 0)
								{
									if (LootStuffN == 1)
									{
										sprintf_s(Command, "/nomodkey /itemnotify \"%s\" leftmouseup", theitem->Name);
										DoCommand(GetCharInfo()->pSpawn, Command);
										LootStuffN = 2;
										LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(5000);
										return;
									}
									else if (LootStuffN == 2)
									{
										if (CXWnd *pWndButton = pMerchantWnd->GetChildItem("MW_Sell_Button"))
										{
											bool Old = ((PCXWNDMGR)pWndMgr)->KeyboardFlags[0];
											((PCXWNDMGR)pWndMgr)->KeyboardFlags[0] = 1;
											gShiftKeyDown = 1;
											SendWndClick2(pWndButton, "leftmouseup");
											gShiftKeyDown = 0;
											((PCXWNDMGR)pWndMgr)->KeyboardFlags[0] = Old;
											LootStuffN = 1;
											LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(5000);
											return;
										}
									}
								}
								else if (!theitem->NoDrop)
								{
									WriteChatf(PLUGIN_MSG ":: Attempted to sell \ag%s\ax, but it is no drop.  Setting to Keep in your loot ini file.", theitem->Name);
									WritePrivateProfileString(INISection, theitem->Name, "Keep", LootINI);
								}
								else if (theitem->Cost == 0)
								{
									WriteChatf(PLUGIN_MSG ":: Attempted to sell \ag%s\ax, but it is worth nothing.  Setting to Keep in your loot ini file.", theitem->Name);
									WritePrivateProfileString(INISection, theitem->Name, "Keep", LootINI);
								}
							}
						}
					}
				}
			}
		}
		if (pChar2 && pChar2->pInventoryArray) //Checking my bags
		{
			for (unsigned long nPack = 0; nPack < 10; nPack++)
			{
				if (PCONTENTS pPack = pChar2->pInventoryArray->Inventory.Pack[nPack])
				{
					if (PITEMINFO pItemPack = GetItemFromContents(pPack))
					{
						if (pItemPack->Type == ITEMTYPE_PACK && pPack->Contents.ContainedItems.pItems)
						{
							for (unsigned long nItem = 0; nItem < pItemPack->Slots; nItem++)
							{
								if (PCONTENTS pItem = pPack->Contents.ContainedItems.pItems->Item[nItem])
								{
									if (PITEMINFO theitem = GetItemFromContents(pItem))
									{
										sprintf_s(INISection, "%c", theitem->Name[0]);
										if (GetPrivateProfileString(INISection, theitem->Name, 0, Value, MAX_STRING, LootINI) != 0)
										{
											CHAR *pParsedToken = NULL;
											CHAR *pParsedValue = strtok_s(Value, "|", &pParsedToken);
											if (!_stricmp(pParsedValue, "Keep") && !_stricmp(szAction, "Deposit"))
											{
												if (WinState((CXWnd*)pBankWnd))
												{ 
													if (FitInBank(theitem->Size))
													{
														if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: Picking up \ag%s\ax", theitem->Name); }
														sprintf_s(Command, "/nomodkey /itemnotify \"%s\" leftmouseup", theitem->Name);
														DoCommand(GetCharInfo()->pSpawn, Command);
														LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(100);
														return;
													}
												}
											}
											else if (!_stricmp(pParsedValue, "Deposit") && !_stricmp(szAction, "Deposit"))
											{
												if (WinState((CXWnd*)FindMQ2Window("GuildBankWnd")))
												{ 
													if (CXWnd *pWnd = FindMQ2Window("GuildBankWnd")->GetChildItem("GBANK_DepositCountLabel"))
													{
														CHAR szDepositCountText[MAX_STRING] = { 0 }; 
														GetCXStr(pWnd->WindowText, szDepositCountText, MAX_STRING);
														CHAR *pParsedToken = NULL;
														CHAR *pParsedValue = strtok_s(szDepositCountText, ":", &pParsedToken);
														pParsedValue = strtok_s(NULL, ":", &pParsedToken);
														DWORD SlotsLeft = atoi(pParsedValue);
														WriteChatf(PLUGIN_MSG ":: The number of items left that can be deposited is: %d", SlotsLeft);
														if (SlotsLeft > 0)
														{
															sprintf_s(Command, "/nomodkey /itemnotify \"%s\" leftmouseup", theitem->Name);
															DoCommand(GetCharInfo()->pSpawn, Command);
															LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(2000);
															return;
														}
														else
														{
															WriteChatf(PLUGIN_MSG ":: Your guild bank ran out of space, closing guild bank window");
															DoCommand(GetCharInfo()->pSpawn, "/squelch /windowstate GuildBankWnd close");
															StartLootStuff = false;
															return;
														}
													}
												}
											}
											else if (!_stricmp(pParsedValue, "Sell") && !_stricmp(szAction, "Sell"))
											{
												if (theitem->NoDrop &&  theitem->Cost > 0)
												{
													if (LootStuffN == 1)
													{
														sprintf_s(Command, "/nomodkey /itemnotify \"%s\" leftmouseup", theitem->Name);
														DoCommand(GetCharInfo()->pSpawn, Command);
														LootStuffN = 2;
														LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(5000);
														return;
													}
													else if (LootStuffN == 2)
													{
														if (CXWnd *pWndButton = pMerchantWnd->GetChildItem("MW_Sell_Button"))
														{
															bool Old = ((PCXWNDMGR)pWndMgr)->KeyboardFlags[0];
															((PCXWNDMGR)pWndMgr)->KeyboardFlags[0] = 1;
															gShiftKeyDown = 1;
															SendWndClick2(pWndButton, "leftmouseup");
															gShiftKeyDown = 0;
															((PCXWNDMGR)pWndMgr)->KeyboardFlags[0] = Old;
															LootStuffN = 1;
															LootStuffTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(5000);
															return;
														}
													}
												}
												else if (!theitem->NoDrop)
												{
													WriteChatf(PLUGIN_MSG ":: Attempted to sell \ag%s\ax, but it is no drop.  Setting to Keep in your loot ini file.", theitem->Name);
													WritePrivateProfileString(INISection, theitem->Name, "Keep", LootINI);
												}
												else if (theitem->Cost == 0)
												{
													WriteChatf(PLUGIN_MSG ":: Attempted to sell \ag%s\ax, but it is worth nothing.  Setting to Keep in your loot ini file.", theitem->Name);
													WritePrivateProfileString(INISection, theitem->Name, "Keep", LootINI);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (WinState((CXWnd*)pMerchantWnd))
	{
		if (CXWnd *pWndButton = pMerchantWnd->GetChildItem("MW_Done_Button"))
		{
			WriteChatf(PLUGIN_MSG ":: Closing merchant window");
			SendWndClick2(pWndButton, "leftmouseup");
			StartLootStuff = false;
			return;
		}
	}
	else if (WinState((CXWnd*)pBankWnd))
	{
		if (CXWnd *pWndButton = pBankWnd->GetChildItem("BIGB_DoneButton"))
		{
			WriteChatf(PLUGIN_MSG ":: Closing bank window");
			SendWndClick2(pWndButton, "leftmouseup");
			StartLootStuff = false;
			return;
		}
	}
	else if (WinState((CXWnd*)FindMQ2Window("GuildBankWnd")))
	{
		WriteChatf(PLUGIN_MSG ":: Closing guild bank window");
		DoCommand(GetCharInfo()->pSpawn, "/squelch /windowstate GuildBankWnd close");
		StartLootStuff = false;
		return;
	}
}

void SetAutoLootVariables()
{
	sprintf_s(INIFileName, "%s\\%s.ini", gszINIPath, PLUGIN_NAME);
	UseAutoLoot = GetPrivateProfileInt(GetCharInfo()->Name, "UseAutoLoot", 1, INIFileName);
	if (GetPrivateProfileString(GetCharInfo()->Name, "lootini", 0, LootINI, MAX_STRING, INIFileName) == 0)
	{
		sprintf_s(LootINI, "%s\\Macros\\Loot.ini", gszINIPath);  // Default location is in your \macros\loot.ini
		WritePrivateProfileString(GetCharInfo()->Name, "lootini", LootINI, INIFileName);
	}
	LONG Version = GetPrivateProfileInt("Settings", "Version", -1, LootINI);
	if (Version == -1)
	{
		CreateLootINI();
		CHAR Version[MAX_STRING] = { 0 };
		sprintf_s(Version, "%1.2f", VERSION);
		WritePrivateProfileString("Settings", "Version", Version, LootINI);
	}

	BarMinSellPrice = GetPrivateProfileInt("Settings", "BarMinSellPrice", -1, LootINI);
	if (BarMinSellPrice == -1)
	{
		BarMinSellPrice = 1;
		WritePrivateProfileString("Settings", "BarMinSellPrice", "1", LootINI);
	}
	DistributeLootDelay = GetPrivateProfileInt("Settings", "DistributeLootDelay", -1, LootINI);
	if (DistributeLootDelay == -1)
	{
		DistributeLootDelay = 5;
		WritePrivateProfileString("Settings", "DistributeLootDelay", "5", LootINI);
	}
	CursorDelay = GetPrivateProfileInt("Settings", "CursorDelay", -1, LootINI);
	if (CursorDelay == -1)
	{
		CursorDelay = 10;
		WritePrivateProfileString("Settings", "CursorDelay", "10", LootINI);
	}
	SaveBagSlots = GetPrivateProfileInt("Settings", "SaveBagSlots", -1, LootINI);
	if (SaveBagSlots == -1)
	{
		SaveBagSlots = 0;
		WritePrivateProfileString("Settings", "SaveBagSlots", "0", LootINI);
	}
	SpamLootInfo = GetPrivateProfileInt("Settings", "SpamLootInfo", -1, LootINI);
	if (SpamLootInfo == -1)
	{
		SpamLootInfo = 1;
		WritePrivateProfileString("Settings", "SpamLootInfo", "1", LootINI);
	}
	QuestKeep = GetPrivateProfileInt("Settings", "QuestKeep", -1, LootINI);
	if (QuestKeep == -1)
	{
		QuestKeep = 10;
		WritePrivateProfileString("Settings", "QuestKeep", "10", LootINI);
	}
	if (GetPrivateProfileString("Settings", "NoDropDefault", 0, NoDropDefault, MAX_STRING, LootINI) == 0)
	{
		sprintf_s(NoDropDefault, "Quest");
		WritePrivateProfileString("Settings", "NoDropDefault", NoDropDefault, LootINI);
	}
	if (GetPrivateProfileString("Settings", "ExcludedBag1", 0, ExcludedBag1, MAX_STRING, LootINI) == 0)
	{
		sprintf_s(ExcludedBag1, "Extraplanar Trade Satchel");
		WritePrivateProfileString("Settings", "ExcludeBag1", ExcludedBag1, LootINI);
	}
	if (GetPrivateProfileString("Settings", "ExcludedBag2", 0, ExcludedBag2, MAX_STRING, LootINI) == 0)
	{
		sprintf_s(ExcludedBag2, "Extraplanar Trade Satchel");
		WritePrivateProfileString("Settings", "ExcludeBag2", ExcludedBag2, LootINI);
	}

	if (Initialized) // Won't spam this on start up of plugin, will only spam if someone reloads their settings
	{
		WriteChatf(PLUGIN_MSG ":: AutoLoot is %s", UseAutoLoot ? "\agON\ax" : "\arOFF\ax");
		WriteChatf(PLUGIN_MSG ":: Stop looting when \ag%d\ax slots are left", SaveBagSlots);
		WriteChatf(PLUGIN_MSG ":: Spam looting actions %s", SpamLootInfo ? "\agON\ax" : "\arOFF\ax");
		WriteChatf(PLUGIN_MSG ":: The master looter will wait \ag%d\ax seconds before trying to distribute loot", DistributeLootDelay);
		WriteChatf(PLUGIN_MSG ":: You will wait \ag%d\ax seconds before trying to autoinventory items on your cursor", CursorDelay);
		WriteChatf(PLUGIN_MSG ":: The minimum price for all items to be bartered is: \ag%d\ax", BarMinSellPrice);
		WriteChatf(PLUGIN_MSG ":: Your default number to keep of new quest items is: \ag%d\ax", QuestKeep);
		WriteChatf(PLUGIN_MSG ":: Your default for new no drop items is: \ag%s\ax", NoDropDefault);
		WriteChatf(PLUGIN_MSG ":: Will exclude \ar%s\ax when checking for free slots", ExcludedBag1);
		WriteChatf(PLUGIN_MSG ":: Will exclude \ar%s\ax when checking for free slots", ExcludedBag2);
		WriteChatf(PLUGIN_MSG ":: The location for your loot ini is:\n \ag%s\ax", LootINI);
	}
}

void CreateLootEntry(CHAR* szAction, CHAR* szEntry, PITEMINFO pItem)
{
	CHAR INISection[MAX_STRING] = { 0 };
	CHAR INIValue[MAX_STRING] = { 0 };
	sprintf_s(INISection, "%c", pItem->Name[0]);
	if (!_stricmp(szAction, "Keep"))
	{
		WritePrivateProfileString(INISection, pItem->Name, "Keep", LootINI);
		WriteChatf(PLUGIN_MSG ":: Setting \ag%s\ax to \agKeep\ax", pItem->Name);
	}
	else if (!_stricmp(szAction, "Sell"))
	{
		WritePrivateProfileString(INISection, pItem->Name, "Sell", LootINI);
		WriteChatf(PLUGIN_MSG ":: Setting \ag%s\ax to \agSell\ax", pItem->Name);
	}
	else if (!_stricmp(szAction, "Deposit"))
	{
		WritePrivateProfileString(INISection, pItem->Name, "Deposit", LootINI);
		WriteChatf(PLUGIN_MSG ":: Setting \ag%s\ax to \agDeposit\ax", pItem->Name);
	}
	else if (!_stricmp(szAction, "Ignore"))
	{
		WritePrivateProfileString(INISection, pItem->Name, "Ignore", LootINI);
		WriteChatf(PLUGIN_MSG ":: Setting \ag%s\ax to \arIgnore\ax", pItem->Name);
	}
	else if (!_stricmp(szAction, "Destroy"))
	{
		WritePrivateProfileString(INISection, pItem->Name, "Destroy", LootINI);
		WriteChatf(PLUGIN_MSG ":: Setting \ag%s\ax to \arDestroy\ax", pItem->Name);
	}
	else if (!_stricmp(szAction, "Quest"))
	{
		int QuestNumber = atoi(szEntry);
		sprintf_s(INIValue, "Quest|%d", QuestNumber);
		WritePrivateProfileString(INISection, pItem->Name, INIValue, LootINI);
		WriteChatf(PLUGIN_MSG ":: Setting \ag%s\ax to \ag%s\ax", pItem->Name, INIValue);
	}
	else if (!_stricmp(szAction, "Barter"))
	{
		int BarterNumber = atoi(szEntry);
		sprintf_s(INIValue, "Barter|%d", BarterNumber);
		WritePrivateProfileString(INISection, pItem->Name, INIValue, LootINI);
		WriteChatf(PLUGIN_MSG ":: Setting \ag%s\ax to \ag%s\ax", pItem->Name, INIValue);
	}
	else if (!_stricmp(szAction, "Gear"))
	{
		sprintf_s(INIValue, "Gear|Classes|");
		for (int playerclass = Warrior; playerclass <= Berserker; playerclass++)
		{
			if (pItem->Classes & (1 << (playerclass - 1)))
			{
				strcat_s(INIValue, ClassInfo[playerclass].UCShortName);
				strcat_s(INIValue, "|");
			}
		}
		int matching = 0;
		if (pItem->Lore == 0)
		{
			for (int itemslot = 0; itemslot < BAG_SLOT_START; itemslot++)
			{
				if (pItem->EquipSlots & (1 << itemslot))
				{
					matching++;
				}
			}
		}
		else
		{
			matching = 1;
		}
		sprintf_s(INIValue, "%sNumberToLoot|%i|", INIValue, matching);
		WritePrivateProfileString(INISection, pItem->Name, INIValue, LootINI);
		WriteChatf(PLUGIN_MSG ":: Setting \ag%s\ax to:", pItem->Name);
		WriteChatf(PLUGIN_MSG ":: \ag%s\ax", INIValue);
	}
	else if (!_stricmp(szAction, "Status"))
	{
		CHAR Value[MAX_STRING] = { 0 };
		if (GetPrivateProfileString(INISection, pItem->Name, 0, Value, MAX_STRING, LootINI) == 0)
		{
			WriteChatf(PLUGIN_MSG ":: \ag%s\ax is not in your loot.ini", pItem->Name);
		}
		else
		{
			WriteChatf(PLUGIN_MSG ":: \ag%s\ax is set to \ag%s\ax", pItem->Name, Value);
		}
	}
	else
	{
		WriteChatf(PLUGIN_MSG ":: Invalid command.  The accepted commands are [Quest #n|Gear|Keep|Sell|Ignore|Destroy]");
	}
	
	if(PCHARINFO2 pChar2 = GetCharInfo2())
	{
		if (pChar2->pInventoryArray && pChar2->pInventoryArray->Inventory.Cursor)
		{
			if (PCONTENTS pItem = pChar2->pInventoryArray->Inventory.Cursor)
			{
				if (!_stricmp(szAction, "Destroy"))
				{
					if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: Destroying \ar%s\ax", pItem->Item2->Name); }
					DoCommand(GetCharInfo()->pSpawn, "/destroy");
				}
				else
				{
					if (FitInInventory(pItem->Item2->Size))
					{
						if (SpamLootInfo) { WriteChatf(PLUGIN_MSG ":: Putting \ag%s\ax into my inventory", pItem->Item2->Name); }
						DoCommand(GetCharInfo()->pSpawn, "/autoinventory");
					}
				}
			}
		}
	}
}

void CreateLootINI(void)
{
	CHAR INISections[MAX_STRING] = "Settings|Global|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z";
	CHAR *pParsedToken = NULL;
	CHAR *pParsedValue = strtok_s(INISections, "|", &pParsedToken);
	while (pParsedValue != NULL)
	{
		WritePrivateProfileString(pParsedValue, "|===================================", "==================================|", LootINI);
		pParsedValue = strtok_s(NULL, "|", &pParsedToken);
	}
}

void AutoLootCommand(PSPAWNINFO pCHAR, PCHAR zLine)
{
	if (!InGameOK()) return; 
	bool ShowInfo = false;
	bool NeedHelp = false;
	char Parm1[MAX_STRING];
	char Parm2[MAX_STRING];
	char Parm3[MAX_STRING];
	GetArg(Parm1, zLine, 1);
	GetArg(Parm2, zLine, 2);
	GetArg(Parm3, zLine, 3);

	if (!_stricmp(Parm1, "help"))
	{
		NeedHelp = true;
	}
	else if (!_stricmp(Parm1, "turn"))
	{
		if (!_stricmp(Parm2, "on")) UseAutoLoot = SetBOOL(UseAutoLoot, Parm2, GetCharInfo()->Name, "UseAutoLoot", INIFileName);
		if (!_stricmp(Parm2, "off"))
		{
			UseAutoLoot = SetBOOL(UseAutoLoot, Parm2, GetCharInfo()->Name, "UseAutoLoot", INIFileName);
			StartLootStuff = false;
		}
		WriteChatf(PLUGIN_MSG ":: Set %s", UseAutoLoot ? "\agON\ax" : "\arOFF\ax");
	}
	else if (!_stricmp(Parm1, "spamloot"))
	{
		if (!_stricmp(Parm2, "on")) SpamLootInfo = SetBOOL(SpamLootInfo, Parm2, "Settings", "SpamLootInfo", LootINI);
		if (!_stricmp(Parm2, "off")) SpamLootInfo = SetBOOL(SpamLootInfo, Parm2, "Settings", "SpamLootInfo", LootINI);
		WriteChatf(PLUGIN_MSG ":: Spam looting actions %s", SpamLootInfo ? "\agON\ax" : "\arOFF\ax");
	}
	else if (!_stricmp(Parm1, "barterminimum")) {
		//TODO add safety checks to make sure it is a number etc
		BarMinSellPrice = atoi(Parm2);
		WritePrivateProfileString("Settings", "BarMinSellPrice", Parm2, LootINI);
		WriteChatf(PLUGIN_MSG ":: Stop looting when \ag%d\ax slots are left", BarMinSellPrice);
	}
	else if (!_stricmp(Parm1, "saveslots")) {
		//TODO add safety checks to make sure it is a number etc
		SaveBagSlots = atoi(Parm2);
		WritePrivateProfileString("Settings", "SaveBagSlots", Parm2, LootINI);
		WriteChatf(PLUGIN_MSG ":: Stop looting when \ag%d\ax slots are left", SaveBagSlots);
	}
	else if (!_stricmp(Parm1, "distributedelay"))
	{
		//TODO add safety checks to make sure it is a number etc
		DistributeLootDelay = atoi(Parm2);
		WritePrivateProfileString("Settings", "DistributeLootDelay", Parm2, LootINI);
		WriteChatf(PLUGIN_MSG ":: The master looter will wait \ag%d\ax seconds before trying to distribute loot", DistributeLootDelay);
	}
	else if (!_stricmp(Parm1, "cursordelay"))
	{
		//TODO add safety checks to make sure it is a number etc
		CursorDelay = atoi(Parm2);
		WritePrivateProfileString("Settings", "CursorDelay", Parm2, LootINI);
		WriteChatf(PLUGIN_MSG ":: You will wait \ag%d\ax seconds before trying to autoinventory items on your cursor", CursorDelay);
	}
	else if (!_stricmp(Parm1, "questkeep"))
	{
		//TODO add safety checks to make sure it is a number etc
		QuestKeep = atoi(Parm2);
		WritePrivateProfileString("Settings", "QuestKeep", Parm2, LootINI);
		WriteChatf(PLUGIN_MSG ":: Your default number to keep of new no drop items is: \ag%d\ax", QuestKeep);
	}
	else if (!_stricmp(Parm1, "nodropdefault"))
	{
		if (!_stricmp(Parm2, "quest"))
		{
			sprintf_s(NoDropDefault, "Quest");
		}
		else if (!_stricmp(Parm2, "keep"))
		{
			sprintf_s(NoDropDefault, "Keep");
		}
		else if (!_stricmp(Parm2, "ignore"))
		{
			sprintf_s(NoDropDefault, "Ignore");
		}
		else
		{
			WriteChatf(PLUGIN_MSG ":: \ar%s\ax is an invalid entry, please use [quest|keep|ignore]", NoDropDefault);
			return;
		}
		WritePrivateProfileString("Settings", "NoDropDefault", NoDropDefault, LootINI);
		WriteChatf(PLUGIN_MSG ":: Your default for new no drop items is: \ag%s\ax", NoDropDefault);
	}
	else if (!_stricmp(Parm1, "excludedbag1"))
	{
		WritePrivateProfileString("Settings", "ExcludeBag1", Parm2, LootINI);
		sprintf_s(ExcludedBag1, "%s", Parm2);
		WriteChatf(PLUGIN_MSG ":: Will exclude \ar%s\ax when checking for free slots", ExcludedBag1);
	}
	else if (!_stricmp(Parm1, "excludedbag2"))
	{
		WritePrivateProfileString("Settings", "ExcludeBag2", Parm2, LootINI);
		sprintf_s(ExcludedBag2, "%s", Parm2);
		WriteChatf(PLUGIN_MSG ":: Will exclude \ar%s\ax when checking for free slots", ExcludedBag2);
	}
	else if (!_stricmp(Parm1, "lootini"))
	{
		sprintf_s(LootINI, "%s\\Macros\\%s.ini", gszINIPath, Parm2);
		WritePrivateProfileString(GetCharInfo()->Name, "lootini", LootINI, INIFileName);
		WriteChatf(PLUGIN_MSG ":: The location for your loot ini is:\n \ag%s\ax", LootINI);
		SetAutoLootVariables();
	}
	else if (!_stricmp(Parm1, "reload"))
	{
		SetAutoLootVariables();
	}
	else if (!_stricmp(Parm1, "sell"))
	{
		if (StartLootStuff)  // sent the command a second time while still active, assuming you wanted to bail out on the action
		{
			StartLootStuff = false;
			return;
		}
		StartLootStuff = true;
		StartMoveToTarget = true;
		StartToOpenWindow = true;
		LootStuffWindowOpen = false;
		LootStuffN = 1;
		LootStuffTimer = std::chrono::high_resolution_clock::now();
		LootStuffCancelTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(30000);
		sprintf_s(LootStuffAction, "Sell");
	}
	else if (!_stricmp(Parm1, "deposit"))
	{
		if (StartLootStuff)  // sent the command a second time while still active, assuming you wanted to bail out on the action
		{
			StartLootStuff = false;
			return;
		}
		StartLootStuff = true;
		StartMoveToTarget = true;
		StartToOpenWindow = true;
		LootStuffWindowOpen = false;
		LootStuffN = 1;
		LootStuffTimer = std::chrono::high_resolution_clock::now();
		LootStuffCancelTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(30000);
		sprintf_s(LootStuffAction, "Deposit");
	}
	else if (!_stricmp(Parm1, "barter"))
	{
		if (StartLootStuff)  // sent the command a second time while still active, assuming you wanted to bail out on the action
		{
			StartLootStuff = false;
			return;
		}
		StartLootStuff = true;
		StartMoveToTarget = true;
		StartToOpenWindow = true;
		StarBarterTimer = true;
		LootStuffWindowOpen = false;
		BarterIndex = 0;
		LootStuffN = 1;
		LootStuffTimer = std::chrono::high_resolution_clock::now();
		LootStuffCancelTimer = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(30000);
		sprintf_s(LootStuffAction, "Barter");
	}
	else if (!_stricmp(Parm1, "test"))
	{
		WriteChatf(PLUGIN_MSG ":: Testing stuff, please ignore this command.  I will remove it later once plugin is done");
	}
	else if (!_stricmp(Parm1, "")) {
		ShowInfo = TRUE;
		NeedHelp = TRUE;
	}
	if (NeedHelp) {
		WriteChatColor("Usage:");
		WriteChatColor("/AutoLoot -> displays settings");
		WriteChatColor("/AutoLoot reload -> AutoLoot will reload variables from MQ2AutoLoot.ini");
		WriteChatColor("/AutoLoot turn [on|off] -> Toggle autoloot");
		WriteChatColor("/AutoLoot spamloot [on|off] -> Toggle loot action messages");
		WriteChatColor("/AutoLoot saveslots #n -> Stops looting when #n slots are left");
		WriteChatColor("/AutoLoot distributedelay #n -> Master looter waits #n seconds to try and distribute the loot");
		WriteChatColor("/AutoLoot cursordelay #n -> You will wait #n seconds before trying to autoinventory items on your cursor");
		WriteChatColor("/AutoLoot barterminimum #n -> The minimum price for all items to be bartered is #n"); 
		WriteChatColor("/AutoLoot questkeep #n -> If nodropdefault is set to quest your new no drop items will be set to Quest|#n");
		WriteChatColor("/AutoLoot nodropdefault [quest|keep|ignore] -> Will set new no drop items to this value");
		WriteChatColor("/AutoLoot excludedbag1 XXX -> Will exclude bags named XXX when checking for how many slots you are free");
		WriteChatColor("/AutoLoot excludedbag2 XXX -> Will exclude bags named XXX when checking for how many slots you are free");
		WriteChatColor("/AutoLoot lootini FILENAME -> Will set your loot ini as FILENAME.ini in your macro folder");
		WriteChatColor("/AutoLoot sell -> If you have targeted a merchant, it will sell all the items marked Sell in your inventory");
		WriteChatColor("/AutoLoot deposit -> If you have your personal banker targetted it will put all items marked Keep into your bank");
		WriteChatColor("/AutoLoot deposit -> If you have your guild banker targetted it will put all items marked Deposit into your guild bank");
		WriteChatColor("/AutoLoot help");
	}
	if (ShowInfo) {
		WriteChatf(PLUGIN_MSG ":: AutoLoot is %s", UseAutoLoot ? "\agON\ax" : "\arOFF\ax");
		WriteChatf(PLUGIN_MSG ":: Stop looting when \ag%d\ax slots are left", SaveBagSlots);
		WriteChatf(PLUGIN_MSG ":: Spam looting actions %s", SpamLootInfo ? "\agON\ax" : "\arOFF\ax");
		WriteChatf(PLUGIN_MSG ":: The master looter will wait \ag%d\ax seconds before trying to distribute loot", DistributeLootDelay);
		WriteChatf(PLUGIN_MSG ":: You will wait \ag%d\ax seconds before trying to autoinventory items on your cursor", CursorDelay);
		WriteChatf(PLUGIN_MSG ":: The minimum price for all items to be bartered is: \ag%d\ax", BarMinSellPrice);
		WriteChatf(PLUGIN_MSG ":: Your default number to keep of new no drop items is: \ag%d\ax", QuestKeep);
		WriteChatf(PLUGIN_MSG ":: Your default for new no drop items is: \ag%s\ax", NoDropDefault);
		WriteChatf(PLUGIN_MSG ":: Will exclude \ar%s\ax when checking for free slots", ExcludedBag1);
		WriteChatf(PLUGIN_MSG ":: Will exclude \ar%s\ax when checking for free slots", ExcludedBag2);
		WriteChatf(PLUGIN_MSG ":: The location for your loot ini is:\n \ag%s\ax", LootINI);
	}
}

void SetItemCommand(PSPAWNINFO pCHAR, PCHAR zLine)
{
	if (!InGameOK()) return;
	bool NeedHelp = false;
	char Parm1[MAX_STRING];
	char Parm2[MAX_STRING];
	GetArg(Parm1, zLine, 1);
	GetArg(Parm2, zLine, 2);

	if (!_stricmp(Parm1, "help"))
	{
		NeedHelp = true;
	}
	else if (!_stricmp(Parm1, "Keep") || !_stricmp(Parm1, "Sell") || !_stricmp(Parm1, "Deposit") || !_stricmp(Parm1, "Barter") || !_stricmp(Parm1, "Gear") || !_stricmp(Parm1, "Quest") || !_stricmp(Parm1, "Ignore") || !_stricmp(Parm1, "Destroy") || !_stricmp(Parm1, "Status"))
	{
		if (PCHARINFO2 pChar2 = GetCharInfo2())
		{
			if (pChar2 && pChar2->pInventoryArray && pChar2->pInventoryArray->Inventory.Cursor)
			{
				if (PCONTENTS pItem = pChar2->pInventoryArray->Inventory.Cursor)
				{
					CreateLootEntry(Parm1, Parm2, GetItemFromContents(pItem));
				}
				else
				{
					WriteChatf(PLUGIN_MSG ":: There is no way this should fail as far as I know. There is an item on your cursor, but you were unable to get PCONTENTS from it.");
				}
			}
			else
			{
				WriteChatf(PLUGIN_MSG ":: There is no item on your cursor, please pick up the item and resend the command");
			}
		}
		else
		{
			WriteChatf(PLUGIN_MSG ":: There is no way this should fail as far as I know.  The plugin failed to get GetCharInfo2");
		}
	}
	else 
	{
		NeedHelp = TRUE;
	}
	if (NeedHelp) {
		WriteChatColor("Usage:");
		WriteChatColor("/SetItem Keep -> Will loot.  '/AutoLoot deposit' will put these items into your personal banker.");
		WriteChatColor("/SetItem Deposit -> Will loot.  '/AutoLoot deposit' will put these items into your guild banker.");
		WriteChatColor("/SetItem Sell -> Will loot.  '/AutoLoot sell' will sell these items to a merchant.");
		WriteChatColor("/SetItem Barter #n -> Will loot, and barter if any buyers offer more then #n plat.");
		WriteChatColor("/SetItem Quest #n -> Will loot up to #n on each character running this plugin.");
		WriteChatColor("/SetItem Gear -> Will loot if your the specified class up to the number listed in the ini entry.");
		WriteChatColor("                 Example entry: Gear|Classes|WAR|PAL|SHD|BRD|NumberToLoot|2|");
		WriteChatColor("/SetItem Ignore -> Will not loot this item.");
		WriteChatColor("/SetItem Destroy -> The master looter will attempt to loot and then destroy this item.");
		WriteChatColor("/SetItem Status -> Will tell you what that item is set to in your loot.ini.");
		WriteChatColor("/SetItem help");
	}
}