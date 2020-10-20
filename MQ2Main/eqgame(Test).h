/******************************************************************************
MQ2Main.dll: MacroQuest2's extension DLL for EverQuest
Copyright (C) 2002-2003 Plazmic, 2003-2005 Lax

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
******************************************************************************/
#ifdef PRIVATE
#if defined(TEST)
#include "eqgame-private(test).h"
#endif
#endif

#define __ClientName                                              "eqgame"
#define __ExpectedVersionDate                                     "Oct 14 2020"
#define __ExpectedVersionTime                                     "10:54:59"
#define __ActualVersionDate_x                                      0xAF22C8
#define __ActualVersionTime_x                                      0xAF22BC
#define __ActualVersionBuild_x                                     0xAE02D4

#define __ClientOverride                                           0
#define __MacroQuestWinClassName                                  "__MacroQuest2Tray(Test)"
#define __MacroQuestWinName                                       "MacroQuest2(Test)"

// Memory Protection
#define __MemChecker0_x                                            0x640FA0
#define __MemChecker1_x                                            0x8F3D60
#define __MemChecker2_x                                            0x6B1400
#define __MemChecker3_x                                            0x6B1350
#define __MemChecker4_x                                            0x84A980
#define __EncryptPad0_x                                            0xC27500
#define __EncryptPad1_x                                            0xC84D38
#define __EncryptPad2_x                                            0xC378C8
#define __EncryptPad3_x                                            0xC374C8
#define __EncryptPad4_x                                            0xC752D8
#define __EncryptPad5_x                                            0xF470B8
#define __AC1_x                                                    0x8092C6
#define __AC2_x                                                    0x5FA66F
#define __AC3_x                                                    0x601CC0
#define __AC4_x                                                    0x605C70
#define __AC5_x                                                    0x60BF5F
#define __AC6_x                                                    0x610416
#define __AC7_x                                                    0x5FA0E0
#define __AC1_Data                                                 0x11111111
#define __EP1_Data_x                                               0x191CC4

// Direct Input
#define DI8__Main_x                                                0xF470D8
#define DI8__Keyboard_x                                            0xF470DC
#define DI8__Mouse_x                                               0xF470F4
#define DI8__Mouse_Copy_x                                          0xEA045C
#define DI8__Mouse_Check_x                                         0xF44C0C
#define __AutoSkillArray_x                                         0xEA1378
#define __Attack_x                                                 0xF3F19B
#define __Autofire_x                                               0xF3F19C
#define __BindList_x                                               0xC15F38
#define g_eqCommandStates_x                                        0xC16CB8
#define __Clicks_x                                                 0xEA0514
#define __CommandList_x                                            0xC17878
#define __CurrentMapLabel_x                                        0xF57AAC
#define __CurrentSocial_x                                          0xBFF7C8
#define __DoAbilityList_x                                          0xED6CBC
#define __do_loot_x                                                0x5C6160
#define __DrawHandler_x                                            0x15D6AA0
#define __GroupCount_x                                             0xE9B532
#define __Guilds_x                                                 0xE9F398
#define __gWorld_x                                                 0xE9B3AC
#define __HWnd_x                                                   0xF470F8
#define __heqmain_x                                                0xF470A0
#define __InChatMode_x                                             0xEA0444
#define __LastTell_x                                               0xEA23B8
#define __LMouseHeldTime_x                                         0xEA0580
#define __Mouse_x                                                  0xF470C4
#define __MouseLook_x                                              0xEA04DA
#define __MouseEventTime_x                                         0xF3FC7C
#define __gpbCommandEvent_x                                        0xE98E30
#define __NetStatusToggle_x                                        0xEA04DD
#define __PCNames_x                                                0xEA1968
#define __RangeAttackReady_x                                       0xEA165C
#define __RMouseHeldTime_x                                         0xEA057C
#define __RunWalkState_x                                           0xEA0448
#define __ScreenMode_x                                             0xDE67C8
#define __ScreenX_x                                                0xEA03FC
#define __ScreenY_x                                                0xEA03F8
#define __ScreenXMax_x                                             0xEA0400
#define __ScreenYMax_x                                             0xEA0404
#define __ServerHost_x                                             0xE98FFB
#define __ServerName_x                                             0xED6C7C
#define __ShiftKeyDown_x                                           0xEA0AD8
#define __ShowNames_x                                              0xEA1818
#define EverQuestInfo__bSocialChanged_x                            0xED6D04
#define __Socials_x                                                0xED6D7C
#define __SubscriptionType_x                                       0xF9BD70
#define __TargetAggroHolder_x                                      0xF5A460
#define __ZoneType_x                                               0xEA08D8
#define __GroupAggro_x                                             0xF5A4A0
#define __LoginName_x                                              0xF47834
#define __Inviter_x                                                0xF3F118
#define __AttackOnAssist_x                                         0xEA17D4
#define __UseTellWindows_x                                         0xEA1B00
#define __gfMaxZoomCameraDistance_x                                0xAE9ED0
#define __gfMaxCameraDistance_x                                    0xB12B50
#define __pulAutoRun_x                                             0xEA04F8
#define __pulForward_x                                             0xEA1B38
#define __pulBackward_x                                            0xEA1B3C
#define __pulTurnRight_x                                           0xEA1B40
#define __pulTurnLeft_x                                            0xEA1B44
#define __pulStrafeLeft_x                                          0xEA1B48
#define __pulStrafeRight_x                                         0xEA1B4C

//// 
//Section 1: Vital Offsets 
//// 
#define instCRaid_x                                                0xE9B730
#define instEQZoneInfo_x                                           0xEA06D0
#define pinstActiveBanker_x                                        0xE9931C
#define pinstActiveCorpse_x                                        0xE99314
#define pinstActiveGMaster_x                                       0xE99318
#define pinstActiveMerchant_x                                      0xE99310
#define pinstAltAdvManager_x                                       0xDE7960
#define pinstBandageTarget_x                                       0xE9932C
#define pinstCamActor_x                                            0xDE67B8
#define pinstCDBStr_x                                              0xDE667C
#define pinstCDisplay_x                                            0xE9B384
#define pinstCEverQuest_x                                          0xF470F0
#define pinstEverQuestInfo_x                                       0xEA03F0
#define pinstCharData_x                                            0xE9B720
#define pinstCharSpawn_x                                           0xE99364
#define pinstControlledMissile_x                                   0xE99374
#define pinstControlledPlayer_x                                    0xE99364
#define pinstCResolutionHandler_x                                  0x15D6CD0
#define pinstCSidlManager_x                                        0x15D5C68
#define pinstCXWndManager_x                                        0x15D5C64
#define instDynamicZone_x                                          0xE9F270
#define pinstDZMember_x                                            0xE9F380
#define pinstDZTimerInfo_x                                         0xE9F384
#define pinstEqLogin_x                                             0xF47180
#define instEQMisc_x                                               0xDE65C0
#define pinstEQSoundManager_x                                      0xDE8930
#define pinstEQSpellStrings_x                                      0xCA8F78
#define instExpeditionLeader_x                                     0xE9F2BA
#define instExpeditionName_x                                       0xE9F2FA
#define pinstGroup_x                                               0xE9B52E
#define pinstImeManager_x                                          0x15D5C60
#define pinstLocalPlayer_x                                         0xE9930C
#define pinstMercenaryData_x                                       0xF41768
#define pinstMercenaryStats_x                                      0xF5A52C
#define pinstModelPlayer_x                                         0xE99324
#define pinstPCData_x                                              0xE9B720
#define pinstSkillMgr_x                                            0xF438D0
#define pinstSpawnManager_x                                        0xF42378
#define pinstSpellManager_x                                        0xF43B10
#define pinstSpellSets_x                                           0xF37DC4
#define pinstStringTable_x                                         0xE9B728
#define pinstSwitchManager_x                                       0xE98CD0
#define pinstTarget_x                                              0xE99360
#define pinstTargetObject_x                                        0xE9936C
#define pinstTargetSwitch_x                                        0xE9937C
#define pinstTaskMember_x                                          0xDE6450
#define pinstTrackTarget_x                                         0xE99368
#define pinstTradeTarget_x                                         0xE99320
#define instTributeActive_x                                        0xDE65E1
#define pinstViewActor_x                                           0xDE67B4
#define pinstWorldData_x                                           0xE98E20
#define pinstZoneAddr_x                                            0xEA0970
#define pinstPlayerPath_x                                          0xF42410
#define pinstTargetIndicator_x                                     0xF43D78
#define EQObject_Top_x                                             0xE99104
 
//// 
//Section 2:  UI Related Offsets 
//// 
#define pinstCAAWnd_x                                              0xDE6AFC
#define pinstCAchievementsWnd_x                                    0xDE6754
#define pinstCActionsWnd_x                                         0xDE6F90
#define pinstCAdvancedDisplayOptionsWnd_x                          0xDE6814
#define pinstCAdvancedLootWnd_x                                    0xDE67AC
#define pinstCAdventureLeaderboardWnd_x                            0xF51280
#define pinstCAdventureRequestWnd_x                                0xF51330
#define pinstCAdventureStatsWnd_x                                  0xF513E0
#define pinstCAggroMeterWnd_x                                      0xDE67FC
#define pinstCAlarmWnd_x                                           0xDE677C
#define pinstCAlertHistoryWnd_x                                    0xDE688C
#define pinstCAlertStackWnd_x                                      0xDE67E4
#define pinstCAlertWnd_x                                           0xDE67F8
#define pinstCAltStorageWnd_x                                      0xF51740
#define pinstCAudioTriggersWindow_x                                0xC9B2A8
#define pinstCAuraWnd_x                                            0xDE67B0
#define pinstCAvaZoneWnd_x                                         0xDE6860
#define pinstCBandolierWnd_x                                       0xDE6810
#define pinstCBankWnd_x                                            0xDE6844
#define pinstCBarterMerchantWnd_x                                  0xF52980
#define pinstCBarterSearchWnd_x                                    0xF52A30
#define pinstCBarterWnd_x                                          0xF52AE0
#define pinstCBazaarConfirmationWnd_x                              0xDE6790
#define pinstCBazaarSearchWnd_x                                    0xDE6B00
#define pinstCBazaarWnd_x                                          0xDE6760
#define pinstCBlockedBuffWnd_x                                     0xDE67D8
#define pinstCBlockedPetBuffWnd_x                                  0xDE6800
#define pinstCBodyTintWnd_x                                        0xDE6FA4
#define pinstCBookWnd_x                                            0xDE683C
#define pinstCBreathWnd_x                                          0xDE6B84
#define pinstCBuffWindowNORMAL_x                                   0xDE67A4
#define pinstCBuffWindowSHORT_x                                    0xDE67A8
#define pinstCBugReportWnd_x                                       0xDE6840
#define pinstCButtonWnd_x                                          0x15D5ED0
#define pinstCCastingWnd_x                                         0xDE6830
#define pinstCCastSpellWnd_x                                       0xDE6B0C
//#define pinstCCharacterCreationWnd_x                             0xDA8EBC
#define pinstCCharacterListWnd_x                                   0xDE6FB0
#define pinstCChatWindowManager_x                                  0xF535A0
#define pinstCClaimWnd_x                                           0xF536F8
#define pinstCColorPickerWnd_x                                     0xDE6AA8
#define pinstCCombatAbilityWnd_x                                   0xDE6770
#define pinstCCombatSkillsSelectWnd_x                              0xDE6728
#define pinstCCompassWnd_x                                         0xDE6F94
#define pinstCConfirmationDialog_x                                 0xF585F8
#define pinstCContainerMgr_x                                       0xDE6FA0
#define pinstCContextMenuManager_x                                 0x15D5C20
#define pinstCCursorAttachment_x                                   0xDE6824
#define pinstCDynamicZoneWnd_x                                     0xF53CC0
#define pinstCEditLabelWnd_x                                       0xDE67A0
#define pinstCEQMainWnd_x                                          0xF53F08
#define pinstCEventCalendarWnd_x                                   0xDE6B04
#define pinstCExtendedTargetWnd_x                                  0xDE67E0
#define pinstCFacePick_x                                           0xDE672C
#define pinstCFactionWnd_x                                         0xDE674C
#define pinstCFellowshipWnd_x                                      0xF54108
#define pinstCFileSelectionWnd_x                                   0xDE6AA4
#define pinstCFindItemWnd_x                                        0xDE673C
#define pinstCFindLocationWnd_x                                    0xF54260
#define pinstCFriendsWnd_x                                         0xDE6740
#define pinstCGemsGameWnd_x                                        0xDE6748
#define pinstCGiveWnd_x                                            0xDE6780
#define pinstCGroupSearchFiltersWnd_x                              0xDE6818
#define pinstCGroupSearchWnd_x                                     0xF54658
#define pinstCGroupWnd_x                                           0xF54708
#define pinstCGuildBankWnd_x                                       0xEA17B8
#define pinstCGuildCreationWnd_x                                   0xF54868
#define pinstCGuildMgmtWnd_x                                       0xF54918
#define pinstCharacterCreation_x                                   0xDE6734
#define pinstCHelpWnd_x                                            0xDE6784
#define pinstCHeritageSelectionWnd_x                               0xDE6738
#define pinstCHotButtonWnd_x                                       0xF56A70
#define pinstCHotButtonWnd1_x                                      0xF56A70
#define pinstCHotButtonWnd2_x                                      0xF56A74
#define pinstCHotButtonWnd3_x                                      0xF56A78
#define pinstCHotButtonWnd4_x                                      0xF56A7C
#define pinstCIconSelectionWnd_x                                   0xDE6820
#define pinstCInspectWnd_x                                         0xDE6804
#define pinstCInventoryWnd_x                                       0xDE67BC
#define pinstCInvSlotMgr_x                                         0xDE6AEC
#define pinstCItemDisplayManager_x                                 0xF57000
#define pinstCItemExpTransferWnd_x                                 0xF57130
#define pinstCItemOverflowWnd_x                                    0xDE6B08
#define pinstCJournalCatWnd_x                                      0xDE6ABC
//#define pinstCJournalNPCWnd_x                                    0xDA93B0
#define pinstCJournalTextWnd_x                                     0xDE6878
#define pinstCKeyRingWnd_x                                         0xDE682C
#define pinstCLargeDialogWnd_x                                     0xF59278
#define pinstCLayoutCopyWnd_x                                      0xF57480
#define pinstCLFGuildWnd_x                                         0xF57530
#define pinstCLoadskinWnd_x                                        0xDE6764
#define pinstCLootFiltersCopyWnd_x                                 0xCB7958
#define pinstCLootFiltersWnd_x                                     0xDE67E8
#define pinstCLootSettingsWnd_x                                    0xDE680C
#define pinstCLootWnd_x                                            0xDE6AC0
#define pinstCMailAddressBookWnd_x                                 0xDE6AF4
#define pinstCMailCompositionWnd_x                                 0xDE6AB0
#define pinstCMailIgnoreListWnd_x                                  0xDE6AF8
#define pinstCMailWnd_x                                            0xDE6A9C
#define pinstCManageLootWnd_x                                      0xDE7E10
#define pinstCMapToolbarWnd_x                                      0xDE678C
#define pinstCMapViewWnd_x                                         0xDE6758
#define pinstCMarketplaceWnd_x                                     0xDE67EC
#define pinstCMerchantWnd_x                                        0xDE6ACC
#define pinstCMIZoneSelectWnd_x                                    0xF57D68
#define pinstCMusicPlayerWnd_x                                     0xDE6A98
#define pinstCNameChangeMercWnd_x                                  0xDE6788
#define pinstCNameChangePetWnd_x                                   0xDE675C
#define pinstCNameChangeWnd_x                                      0xDE679C
#define pinstCNoteWnd_x                                            0xDE6768
#define pinstCObjectPreviewWnd_x                                   0xDE6808
#define pinstCOptionsWnd_x                                         0xDE6774
#define pinstCPetInfoWnd_x                                         0xDE6894
#define pinstCPetitionQWnd_x                                       0xDE6724
//#define pinstCPlayerCustomizationWnd_x                           0xDA8E24
#define pinstCPlayerNotesWnd_x                                     0xDE6FA8
#define pinstCPlayerWnd_x                                          0xDE6888
#define pinstCPopupWndManager_x                                    0xF585F8
#define pinstCProgressionSelectionWnd_x                            0xF586A8
#define pinstCPurchaseGroupWnd_x                                   0xDE685C
#define pinstCPurchaseWnd_x                                        0xDE6834
#define pinstCPvPLeaderboardWnd_x                                  0xF58758
#define pinstCPvPStatsWnd_x                                        0xF58808
#define pinstCQuantityWnd_x                                        0xDE6874
#define pinstCRaceChangeWnd_x                                      0xDE67D0
#define pinstCRaidOptionsWnd_x                                     0xDE681C
#define pinstCRaidWnd_x                                            0xDE6AC8
#define pinstCRealEstateItemsWnd_x                                 0xDE6750
#define pinstCRealEstateLayoutDetailsWnd_x                         0xDE6F98
#define pinstCRealEstateManageWnd_x                                0xDE6864
#define pinstCRealEstateNeighborhoodWnd_x                          0xDE6880
#define pinstCRealEstatePlotSearchWnd_x                            0xDE6AA0
#define pinstCRealEstatePurchaseWnd_x                              0xDE6AB8
#define pinstCRespawnWnd_x                                         0xDE67F4
//#define pinstCRewardSelectionWnd_x                               0x10205B0
#define pinstCSelectorWnd_x                                        0xDE6720
#define pinstCSendMoneyWnd_x                                       0xDE6794
#define pinstCServerListWnd_x                                      0xDE6AC4
#define pinstCSkillsSelectWnd_x                                    0xDE671C
#define pinstCSkillsWnd_x                                          0xDE6F9C
#define pinstCSocialEditWnd_x                                      0xDE6828
#define pinstCSocialWnd_x                                          0xDE6778
#define pinstCSpellBookWnd_x                                       0xDE676C
#define pinstCStoryWnd_x                                           0xDE6F8C
#define pinstCTargetOfTargetWnd_x                                  0xF5A5B0
#define pinstCTargetWnd_x                                          0xDE67F0
#define pinstCTaskOverlayWnd_x                                     0xDE6854
#define pinstCTaskSelectWnd_x                                      0xF5A708
#define pinstCTaskManager_x                                        0xCB8298
#define pinstCTaskTemplateSelectWnd_x                              0xF5A7B8
#define pinstCTaskWnd_x                                            0xF5A868
#define pinstCTextEntryWnd_x                                       0xDE6838
#define pinstCTimeLeftWnd_x                                        0xDE6718
#define pinstCTipWndCONTEXT_x                                      0xF5AA6C
#define pinstCTipWndOFDAY_x                                        0xF5AA68
#define pinstCTitleWnd_x                                           0xF5AB18
#define pinstCTrackingWnd_x                                        0xDE67DC
#define pinstCTradeskillWnd_x                                      0xF5AC80
#define pinstCTradeWnd_x                                           0xDE6FAC
#define pinstCTrainWnd_x                                           0xDE6AB4
#define pinstCTributeBenefitWnd_x                                  0xF5AE80
#define pinstCTributeMasterWnd_x                                   0xF5AF30
#define pinstCTributeTrophyWnd_x                                   0xF5AFE0
#define pinstCVideoModesWnd_x                                      0xDE6884
#define pinstCVoiceMacroWnd_x                                      0xF44330
#define pinstCVoteResultsWnd_x                                     0xDE6744
#define pinstCVoteWnd_x                                            0xDE6730
#define pinstCWebManager_x                                         0xF449AC
#define pinstCZoneGuideWnd_x                                       0xDE6858
#define pinstCZonePathWnd_x                                        0xDE6870

#define pinstEQSuiteTextureLoader_x                                0xC85658
#define pinstItemIconCache_x                                       0xF53EC0
#define pinstLootFiltersManager_x                                  0xCB7A08
#define pinstRewardSelectionWnd_x                                  0xF58F50

//// 
// Section 3: Miscellaneous Offsets 
////
#define __CastRay_x                                                0x5C1850
#define __CastRay2_x                                               0x5C18A0
#define __ConvertItemTags_x                                        0x5DD440
#define __CleanItemTags_x                                          0x47CD90
#define __DoesFileExist_x                                          0x8F6DE0
#define __EQGetTime_x                                              0x8F3990
#define __ExecuteCmd_x                                             0x5BA1D0
#define __FixHeading_x                                             0x988D10
#define __FlushDxKeyboard_x                                        0x6AD2A0
#define __GameLoop_x                                               0x6B05D0
#define __get_bearing_x                                            0x5C13B0
#define __get_melee_range_x                                        0x5C1A90
#define __GetAnimationCache_x                                      0x715B10
#define __GetGaugeValueFromEQ_x                                    0x807770
#define __GetLabelFromEQ_x                                         0x809250
#define __GetXTargetType_x                                         0x98A760
#define __HandleMouseWheel_x                                       0x6B14B0
#define __HeadingDiff_x                                            0x988D80
#define __HelpPath_x                                               0xF3F86C
#define __LoadFrontEnd_x                                           0x6AD8E0
#define __NewUIINI_x                                               0x807440
#define __ProcessGameEvents_x                                      0x6222F0
#define __ProcessMouseEvent_x                                      0x621A80
#define __SaveColors_x                                             0x55AD80
#define __STMLToText_x                                             0x92C1E0
#define __ToggleKeyRingItem_x                                      0x519670
#define CMemoryMappedFile__SetFile_x                               0xA77FA0
#define CrashDetected_x                                            0x6AF390
#define DrawNetStatus_x                                            0x64E4B0
#define Expansion_HoT_x                                            0xEA17C0
#define Teleport_Table_Size_x                                      0xE98EB8
#define Teleport_Table_x                                           0xE99380
#define Util__FastTime_x                                           0x8F3560
#define __CopyLayout_x                                             0x63C740
#define __WndProc_x                                                0x6AF890
#define __ProcessKeyboardEvent_x                                   0x6AEE30

//// 
// Section 4: Function Offsets 
//// 
// CAdvancedLootWnd
#define CAdvancedLootWnd__CAdvancedLootWnd_x                       0x490120
#define CAdvancedLootWnd__DoAdvLootAction_x                        0x499120
#define CAdvancedLootWnd__DoSharedAdvLootAction_x                  0x498EF0
#define CAdvancedLootWnd__AddPlayerToList_x                        0x4936C0
#define CAdvancedLootWnd__UpdateMasterLooter_x                     0x492B10

// AltAdvManager 
#define AltAdvManager__GetCalculatedTimer_x                        0x562E70
#define AltAdvManager__IsAbilityReady_x                            0x561C10
#define AltAdvManager__GetAAById_x                                 0x561FA0
#define AltAdvManager__CanTrainAbility_x                           0x562010
#define AltAdvManager__CanSeeAbility_x                             0x562370

// CharacterZoneClient
#define CharacterZoneClient__CharacterZoneClient_x                 0x4CACB0
#define CharacterZoneClient__HasSkill_x                            0x4D5B30
#define CharacterZoneClient__MakeMeVisible_x                       0x4D7250
#define CharacterZoneClient__IsStackBlocked_x                      0x4C2580
#define CharacterZoneClient__CanUseMemorizedSpellSlot_x            0x4BBA10
#define CharacterZoneClient__GetItemCountWorn_x                    0x4D9C00
#define CharacterZoneClient__GetItemCountInInventory_x             0x4D9CE0
#define CharacterZoneClient__GetCursorItemCount_x                  0x4D9DC0
#define CharacterZoneClient__FindAffectSlot_x                      0x4C4670
#define CharacterZoneClient__BardCastBard_x                        0x4C70A0
#define CharacterZoneClient__GetMaxEffects_x                       0x4BF420
#define CharacterZoneClient__GetEffect_x                           0x4BB950
#define CharacterZoneClient__GetOpenEffectSlot_x                   0x4C56E0
#define CharacterZoneClient__GetFirstEffectSlot_x                  0x4C57B0
#define CharacterZoneClient__GetLastEffectSlot_x                   0x4C5800
#define CharacterZoneClient__CalcAffectChange_x                    0x4C5950
#define CharacterZoneClient__CalcAffectChangeGeneric_x             0x4C5B30
#define CharacterZoneClient__GetFocusReuseMod_x                    0x4B3780
#define CharacterZoneClient__FindItemByGuid_x                      0x4D81B0
#define CharacterZoneClient__FindItemByRecord_x                    0x4D7C30

// CBankWnd 
#define CBankWnd__GetNumBankSlots_x                                0x6D70C0
#define CBankWnd__WndNotification_x                                0x6D6EA0

// CBazaarSearchWnd 
#define CBazaarSearchWnd__HandleBazaarMsg_x                        0x6E46C0

// CButtonWnd 
#define CButtonWnd__SetCheck_x                                     0x9299C0
#define CButtonWnd__CButtonWnd_x                                   0x9285C0

// CChatManager 
#define CChatManager__GetRGBAFromIndex_x                           0x704CE0
#define CChatManager__InitContextMenu_x                            0x6FDE10
#define CChatManager__FreeChatWindow_x                             0x703820
#define CChatManager__GetLockedActiveChatWindow_x                  0x4E9540
#define CChatManager__SetLockedActiveChatWindow_x                  0x704960
#define CChatManager__CreateChatWindow_x                           0x703E60

//ChatManagerClient
#define ChatManagerClient__Instance_x                              0x4E9650

// CContextMenu 
#define CContextMenu__CContextMenu_x                               0x93CC30
#define CContextMenu__dCContextMenu_x                              0x93CE70
#define CContextMenu__AddMenuItem_x                                0x93CE80
#define CContextMenu__RemoveMenuItem_x                             0x93D190
#define CContextMenu__RemoveAllMenuItems_x                         0x93D1B0
#define CContextMenu__CheckMenuItem_x                              0x93D230
#define CContextMenu__SetMenuItem_x                                0x93D0B0
#define CContextMenu__AddSeparator_x                               0x93D010

//CContextMenuManager
#define CContextMenuManager__AddMenu_x                             0x93D7D0
#define CContextMenuManager__RemoveMenu_x                          0x93D840
#define CContextMenuManager__PopupMenu_x                           0x93D900
#define CContextMenuManager__Flush_x                               0x93D770
#define CContextMenuManager__GetMenu_x                             0x49B3C0
#define CContextMenuManager__CreateDefaultMenu_x                   0x710470

// CChatService
#define CChatService__GetNumberOfFriends_x                         0x8CE620
#define CChatService__GetFriendName_x                              0x8CE630

// CChatWindow 
#define CChatWindow__CChatWindow_x                                 0x705550
#define CChatWindow__Clear_x                                       0x706820
#define CChatWindow__WndNotification_x                             0x706FB0
#define CChatWindow__AddHistory_x                                  0x7060E0

// CComboWnd 
#define CComboWnd__DeleteAll_x                                     0x93A090
#define CComboWnd__Draw_x                                          0x939580
#define CComboWnd__GetCurChoice_x                                  0x939ED0
#define CComboWnd__GetListRect_x                                   0x939A30
#define CComboWnd__GetTextRect_x                                   0x93A100
#define CComboWnd__InsertChoice_x                                  0x939BC0
#define CComboWnd__SetColors_x                                     0x939B90
#define CComboWnd__SetChoice_x                                     0x939EA0
#define CComboWnd__GetItemCount_x                                  0x939EE0
#define CComboWnd__GetCurChoiceText_x                              0x939F20
#define CComboWnd__GetChoiceText_x                                 0x939EF0
#define CComboWnd__InsertChoiceAtIndex_x                           0x939C60

// CContainerWnd 
#define CContainerWnd__HandleCombine_x                             0x70DE80
#define CContainerWnd__vftable_x                                   0xAFB4B4
#define CContainerWnd__SetContainer_x                              0x70F3D0

// CDisplay
#define CDisplay__ZoneMainUI_x                                     0x555730
#define CDisplay__PreZoneMainUI_x                                  0x555740
#define CDisplay__CleanGameUI_x                                    0x55AB40
#define CDisplay__GetClickedActor_x                                0x54DB90
#define CDisplay__GetUserDefinedColor_x                            0x546200
#define CDisplay__GetWorldFilePath_x                               0x54F600
#define CDisplay__is3dON_x                                         0x54A7F0
#define CDisplay__ReloadUI_x                                       0x554C40
#define CDisplay__WriteTextHD2_x                                   0x54A5E0
#define CDisplay__TrueDistance_x                                   0x5512C0
#define CDisplay__SetViewActor_x                                   0x54D4B0
#define CDisplay__GetFloorHeight_x                                 0x54A8B0
#define CDisplay__SetRenderWindow_x                                0x549230
#define CDisplay__ToggleScreenshotMode_x                           0x54CF80

// CEditBaseWnd 
#define CEditBaseWnd__SetSel_x                                     0x95CA00

// CEditWnd 
#define CEditWnd__DrawCaret_x                                      0x940140
#define CEditWnd__EnsureCaretVisible_x                             0x942310
#define CEditWnd__GetCaretPt_x                                     0x9412C0
#define CEditWnd__GetCharIndexPt_x                                 0x941070
#define CEditWnd__GetDisplayString_x                               0x940F10
#define CEditWnd__GetHorzOffset_x                                  0x93F7E0
#define CEditWnd__GetLineForPrintableChar_x                        0x942210
#define CEditWnd__GetSelStartPt_x                                  0x941320
#define CEditWnd__GetSTMLSafeText_x                                0x940D30
#define CEditWnd__PointFromPrintableChar_x                         0x941E40
#define CEditWnd__SelectableCharFromPoint_x                        0x941FB0
#define CEditWnd__SetEditable_x                                    0x9413F0
#define CEditWnd__SetWindowTextA_x                                 0x940AB0
#define CEditWnd__ReplaceSelection_x                               0x941AB0
#define CEditWnd__ReplaceSelection1_x                              0x941A30

// CEverQuest 
#define CEverQuest__DoPercentConvert_x                             0x60F8F0
#define CEverQuest__ClickedPlayer_x                                0x601AA0
#define CEverQuest__CreateTargetIndicator_x                        0x61F460
#define CEverQuest__DeleteTargetIndicator_x                        0x61F4F0
#define CEverQuest__DoTellWindow_x                                 0x4E9730
#define CEverQuest__OutputTextToLog_x                              0x4E9A00
#define CEverQuest__DropHeldItemOnGround_x                         0x5F6BF0
#define CEverQuest__dsp_chat_x                                     0x4E9D90
#define CEverQuest__trimName_x                                     0x61B610
#define CEverQuest__Emote_x                                        0x610150
#define CEverQuest__EnterZone_x                                    0x60A250
#define CEverQuest__GetBodyTypeDesc_x                              0x614BA0
#define CEverQuest__GetClassDesc_x                                 0x6151E0
#define CEverQuest__GetClassThreeLetterCode_x                      0x6157E0
#define CEverQuest__GetDeityDesc_x                                 0x61DCB0
#define CEverQuest__GetLangDesc_x                                  0x6159A0
#define CEverQuest__GetRaceDesc_x                                  0x6151C0
#define CEverQuest__InterpretCmd_x                                 0x61E280
#define CEverQuest__LeftClickedOnPlayer_x                          0x5FAD90
#define CEverQuest__LMouseUp_x                                     0x5F9110
#define CEverQuest__RightClickedOnPlayer_x                         0x5FB670
#define CEverQuest__RMouseUp_x                                     0x5FA0A0
#define CEverQuest__SetGameState_x                                 0x5F6980
#define CEverQuest__UPCNotificationFlush_x                         0x61B510
#define CEverQuest__IssuePetCommand_x                              0x616DB0
#define CEverQuest__ReportSuccessfulHit_x                          0x6114A0

// CGaugeWnd 
//not sure, need to call it to see if its correct CGaugeWnd__CalcFillRect_x
#define CGaugeWnd__CalcFillRect_x                                  0x71FCD0
#define CGaugeWnd__CalcLinesFillRect_x                             0x71FD30
#define CGaugeWnd__Draw_x                                          0x71F350

// CGuild
#define CGuild__FindMemberByName_x                                 0x4AFB30
#define CGuild__GetGuildName_x                                     0x4AE5E0
#define CGuild__GetGuildIndex_x                                    0x4AEBE0

// CHotButtonWnd 
#define CHotButtonWnd__DoHotButton_x                               0x739A10

//CHotButton
#define CHotButton__SetCheck_x                                     0x630C30
#define CHotButton__SetButtonSize_x                                0x630FF0

// CInvSlotMgr 
#define CInvSlotMgr__FindInvSlot_x                                 0x746B10
#define CInvSlotMgr__MoveItem_x                                    0x7457E0
#define CInvSlotMgr__SelectSlot_x                                  0x746BE0

// CInvSLot
#define CInvSlot__HandleRButtonUp_x                                0x743FD0
#define CInvSlot__SliderComplete_x                                 0x741D10
#define CInvSlot__GetItemBase_x                                    0x741680
#define CInvSlot__UpdateItem_x                                     0x7417F0

// CInvSlotWnd
#define CInvSlotWnd__DrawTooltip_x                                 0x748670
#define CInvSlotWnd__CInvSlotWnd_x                                 0x747800
#define CInvSlotWnd__HandleLButtonUp_x                             0x7481F0

// CItemDisplayWnd 
#define CItemDisplayWnd__SetSpell_x                                0x802B00
#define CItemDisplayWnd__UpdateStrings_x                           0x756EB0
#define CItemDisplayWnd__InsertAugmentRequest_x                    0x750B20
#define CItemDisplayWnd__RemoveAugmentRequest_x                    0x751050
//only 1 arg now? need to check
#define CItemDisplayWnd__SetItem_x                                 0x7574C0
#define CItemDisplayWnd__AboutToShow_x                             0x756B00
#define CItemDisplayWnd__WndNotification_x                         0x758740
#define CItemDisplayWnd__RequestConvertItem_x                      0x758070
#define CItemDisplayWnd__CItemDisplayWnd_x                         0x755B60
#define CItemDisplayWnd__dCItemDisplayWnd_x                        0x756920

//CWndDisplayManager
#define CWndDisplayManager__FindWindowA_x                          0x8334C0

//CItemDisplayManager
#define CItemDisplayManager__CreateWindowInstance_x                0x75CC80

// CLabel 
#define CLabel__Draw_x                                             0x762750

// CListWnd
#define CListWnd__CListWnd_x                                       0x912810
#define CListWnd__dCListWnd_x                                      0x912B30
#define CListWnd__AddColumn_x                                      0x916FA0
#define CListWnd__AddColumn1_x                                     0x916FF0
#define CListWnd__AddLine_x                                        0x917380
#define CListWnd__AddString_x                                      0x917180
#define CListWnd__CalculateFirstVisibleLine_x                      0x916D60
#define CListWnd__CalculateVSBRange_x                              0x916B50
#define CListWnd__ClearSel_x                                       0x917B60
#define CListWnd__ClearAllSel_x                                    0x917BC0
#define CListWnd__CloseAndUpdateEditWindow_x                       0x9185E0
#define CListWnd__Compare_x                                        0x916480
#define CListWnd__Draw_x                                           0x912C60
#define CListWnd__DrawColumnSeparators_x                           0x915450
#define CListWnd__DrawHeader_x                                     0x9158C0
#define CListWnd__DrawItem_x                                       0x915DC0
#define CListWnd__DrawLine_x                                       0x9155C0
#define CListWnd__DrawSeparator_x                                  0x9154F0
#define CListWnd__EnableLine_x                                     0x914D30
#define CListWnd__EnsureVisible_x                                  0x918500
#define CListWnd__ExtendSel_x                                      0x917A90
#define CListWnd__GetColumnTooltip_x                               0x914870
#define CListWnd__GetColumnMinWidth_x                              0x9148E0
#define CListWnd__GetColumnWidth_x                                 0x9147E0
#define CListWnd__GetCurSel_x                                      0x914170
#define CListWnd__GetItemAtPoint_x                                 0x914FB0
#define CListWnd__GetItemAtPoint1_x                                0x915020
#define CListWnd__GetItemData_x                                    0x9141F0
#define CListWnd__GetItemHeight_x                                  0x9145B0
#define CListWnd__GetItemIcon_x                                    0x914380
#define CListWnd__GetItemRect_x                                    0x914E20
#define CListWnd__GetItemText_x                                    0x914230
#define CListWnd__GetSelList_x                                     0x917C10
#define CListWnd__GetSeparatorRect_x                               0x915260
#define CListWnd__InsertLine_x                                     0x917770
#define CListWnd__RemoveLine_x                                     0x9178C0
#define CListWnd__SetColors_x                                      0x916B20
#define CListWnd__SetColumnJustification_x                         0x916850
#define CListWnd__SetColumnLabel_x                                 0x917120
#define CListWnd__SetColumnWidth_x                                 0x916770
#define CListWnd__SetCurSel_x                                      0x9179D0
#define CListWnd__SetItemColor_x                                   0x9181B0
#define CListWnd__SetItemData_x                                    0x918170
#define CListWnd__SetItemText_x                                    0x917D80
#define CListWnd__ShiftColumnSeparator_x                           0x916910
#define CListWnd__Sort_x                                           0x916600
#define CListWnd__ToggleSel_x                                      0x917A00
#define CListWnd__SetColumnsSizable_x                              0x9169C0
#define CListWnd__SetItemWnd_x                                     0x918030
#define CListWnd__GetItemWnd_x                                     0x9143D0
#define CListWnd__SetItemIcon_x                                    0x917E00
#define CListWnd__CalculateCustomWindowPositions_x                 0x916E60
#define CListWnd__SetVScrollPos_x                                  0x916750

// CMapViewWnd 
#define CMapViewWnd__CMapViewWnd_x                                 0x777E60
#define CMapViewWnd__GetWorldCoordinates_x                         0x776570
#define CMapViewWnd__HandleLButtonDown_x                           0x7735B0

// CMerchantWnd 
#define CMerchantWnd__DisplayBuyOrSellPrice_x                      0x7982C0
#define CMerchantWnd__PurchasePageHandler__RequestGetItem_x        0x798BA0
#define CMerchantWnd__PurchasePageHandler__RequestPutItem_x        0x7990D0
#define CMerchantWnd__SelectRecoverySlot_x                         0x79C080
#define CMerchantWnd__MerchantPageHandler__SelectBuySellSlot_x     0x796E30
#define CMerchantWnd__SelectBuySellSlot_x                          0x7A1C50
#define CMerchantWnd__PurchasePageHandler__UpdateList_x            0x797ED0

// CPacketScrambler
#define CPacketScrambler__ntoh_x                                   0x89C710
#define CPacketScrambler__hton_x                                   0x89C700

// CSidlManager 
#define CSidlManager__FindAnimation1_x                             0x9317B0
#define CSidlManager__FindScreenPieceTemplate_x                    0x931BC0
#define CSidlManager__FindScreenPieceTemplate1_x                   0x9319B0
#define CSidlManager__CreateLabel_x                                0x7F9D40
#define CSidlManager__CreateXWndFromTemplate_x                     0x934B20
#define CSidlManager__CreateXWndFromTemplate1_x                    0x934CF0
#define CSidlManager__CreateXWnd_x                                 0x7F9C70
#define CSidlManager__CreateHotButtonWnd_x                         0x7FA230

// CSidlScreenWnd 
#define CSidlScreenWnd__CalculateHSBRange_x                        0x92E290
#define CSidlScreenWnd__CalculateVSBRange_x                        0x92E190
#define CSidlScreenWnd__ConvertToRes_x                             0x9574E0
#define CSidlScreenWnd__CreateChildrenFromSidl_x                   0x92DC80
#define CSidlScreenWnd__CSidlScreenWnd1_x                          0x92D970
#define CSidlScreenWnd__CSidlScreenWnd2_x                          0x92DA40
#define CSidlScreenWnd__dCSidlScreenWnd_x                          0x92DB10
#define CSidlScreenWnd__DrawSidlPiece_x                            0x92E730
#define CSidlScreenWnd__EnableIniStorage_x                         0x92EC00
#define CSidlScreenWnd__GetSidlPiece_x                             0x92E920
#define CSidlScreenWnd__Init1_x                                    0x92D570
#define CSidlScreenWnd__LoadIniInfo_x                              0x92EC50
#define CSidlScreenWnd__LoadIniListWnd_x                           0x92F790
#define CSidlScreenWnd__LoadSidlScreen_x                           0x92C990
#define CSidlScreenWnd__StoreIniInfo_x                             0x92F310
#define CSidlScreenWnd__StoreIniVis_x                              0x92F670
#define CSidlScreenWnd__WndNotification_x                          0x92E640
#define CSidlScreenWnd__GetChildItem_x                             0x92EB80
#define CSidlScreenWnd__HandleLButtonUp_x                          0x91E370
#define CSidlScreenWnd__m_layoutCopy_x                             0x15D5AF0

// CSkillMgr
#define CSkillMgr__IsAvailable_x                                   0x6964F0
#define CSkillMgr__GetSkillCap_x                                   0x6966D0
#define CSkillMgr__GetNameToken_x                                  0x695C70
#define CSkillMgr__IsActivatedSkill_x                              0x695DB0
#define CSkillMgr__IsCombatSkill_x                                 0x695CF0

// CSliderWnd 
#define CSliderWnd__GetValue_x                                     0x93E5B0
#define CSliderWnd__SetValue_x                                     0x93E420
#define CSliderWnd__SetNumTicks_x                                  0x93E480

// CSpellBookWnd 
#define CSpellBookWnd__MemorizeSet_x                               0x7FFE90

// CStmlWnd
#define CStmlWnd__AppendSTML_x                                     0x946F10
#define CStmlWnd__ParseSTML_x                                      0x948260
#define CStmlWnd__CalculateHSBRange_x                              0x948040
#define CStmlWnd__CalculateVSBRange_x                              0x947FB0
#define CStmlWnd__CanBreakAtCharacter_x                            0x94C370
#define CStmlWnd__FastForwardToEndOfTag_x                          0x94D000
#define CStmlWnd__ForceParseNow_x                                  0x947500
#define CStmlWnd__GetNextTagPiece_x                                0x94C2D0
#define CStmlWnd__GetSTMLText_x                                    0x50AA00
#define CStmlWnd__GetVisibleText_x                                 0x947520
#define CStmlWnd__InitializeWindowVariables_x                      0x94CE50
#define CStmlWnd__MakeStmlColorTag_x                               0x946580
#define CStmlWnd__MakeWndNotificationTag_x                         0x9465F0
#define CStmlWnd__SetSTMLText_x                                    0x945630
#define CStmlWnd__StripFirstSTMLLines_x                            0x94E100
#define CStmlWnd__UpdateHistoryString_x                            0x94D210

// CTabWnd 
#define CTabWnd__Draw_x                                            0x944780
#define CTabWnd__DrawCurrentPage_x                                 0x944EB0
#define CTabWnd__DrawTab_x                                         0x944BD0
#define CTabWnd__GetCurrentPage_x                                  0x943F80
#define CTabWnd__GetPageInnerRect_x                                0x9441C0
#define CTabWnd__GetTabInnerRect_x                                 0x944100
#define CTabWnd__GetTabRect_x                                      0x943FB0
#define CTabWnd__InsertPage_x                                      0x9443D0
#define CTabWnd__RemovePage_x                                      0x944540
#define CTabWnd__SetPage_x                                         0x944240
#define CTabWnd__SetPageRect_x                                     0x9446C0
#define CTabWnd__UpdatePage_x                                      0x944A90
#define CTabWnd__GetPageFromTabIndex_x                             0x944B10
#define CTabWnd__GetCurrentTabIndex_x                              0x943F70

// CPageWnd 
#define CPageWnd__GetTabText_x                                     0x75D090
#define CPageWnd__SetTabText_x                                     0x943AC0
#define CPageWnd__FlashTab_x                                       0x943B20

// CTextOverlay 
#define CTextOverlay__DisplayText_x                                0x4A8EB0

// CTextureFont
#define CTextureFont__DrawWrappedText_x                            0x91B230
#define CTextureFont__GetTextExtent_x                              0x91B3F0

//CWebManager
#define CWebManager__CreateHtmlWnd_x                               0x6AA750
#define CHtmlComponentWnd__ValidateUri_x                           0x73B400
#define CHtmlWnd__SetClientCallbacks_x                             0x636090
#define CHtmlWnd__AddObserver_x                                    0x6360B0
#define CHtmlWnd__RemoveObserver_x                                 0x636110
#define Window__getProgress_x                                      0x851FB0
#define Window__getStatus_x                                        0x851FD0
#define Window__getURI_x                                           0x851FE0

// CXMLDataManager 
#define CXMLDataManager__GetXMLData_x                              0x9542F0

// CXMLSOMDocumentBase 
#define CXMLSOMDocumentBase__XMLRead_x                             0x9082F0

// CXStr 
// WARNING:  Be sure that none of these offsets are identical! 
// 
// Note:  dCXStr, CXStr1, & CXStr3 can be found in the 'BookWindow' constructor. 
#define CXStr__CXStr_x                                             0x4E9430
#define CXStr__CXStr1_x                                            0x505AF0
#define CXStr__CXStr3_x                                            0x8EF910
#define CXStr__dCXStr_x                                            0x477FA0
#define CXStr__operator_equal_x                                    0x8EFB40
#define CXStr__operator_equal1_x                                   0x8EFB80
#define CXStr__operator_plus_equal1_x                              0x8F0610
#define CXStr__SetString_x                                         0x8F2500
#define CXStr__operator_char_p_x                                   0x8F0050
#define CXStr__GetChar_x                                           0x8F1E50
#define CXStr__Delete_x                                            0x8F1700
#define CXStr__GetUnicode_x                                        0x8F1EC0
#define CXStr__GetLength_x                                         0x47D140
#define CXStr__Mid_x                                               0x47D150
#define CXStr__Insert_x                                            0x8F1F20
#define CXStr__FindNext_x                                          0x8F1B70

// CXWnd
//CTileLayoutWnd__SetOriginalAutoStretchInfo_x is here because its where I lookup the actual CXWnd Size.
#define CTileLayoutWnd__SetOriginalAutoStretchInfo_x               0x93C850
#define CXWnd__BringToTop_x                                        0x921B50
#define CXWnd__Center_x                                            0x9216D0
#define CXWnd__ClrFocus_x                                          0x9214F0
#define CXWnd__Destroy_x                                           0x921590
#define CXWnd__DoAllDrawing_x                                      0x91DB00
#define CXWnd__DrawChildren_x                                      0x91DAD0
#define CXWnd__DrawColoredRect_x                                   0x91DF60
#define CXWnd__DrawTooltip_x                                       0x91C610
#define CXWnd__DrawTooltipAtPoint_x                                0x91C6D0
#define CXWnd__GetBorderFrame_x                                    0x91DDC0
#define CXWnd__GetChildWndAt_x                                     0x921BF0
#define CXWnd__GetClientClipRect_x                                 0x91FD70
#define CXWnd__GetScreenClipRect_x                                 0x91FE40
#define CXWnd__GetScreenRect_x                                     0x920010
#define CXWnd__GetRelativeRect_x                                   0x9200E0
#define CXWnd__GetTooltipRect_x                                    0x91DFB0
#define CXWnd__GetWindowTextA_x                                    0x49CAC0
#define CXWnd__IsActive_x                                          0x91E6F0
#define CXWnd__IsDescendantOf_x                                    0x920A40
#define CXWnd__IsReallyVisible_x                                   0x920A70
#define CXWnd__IsType_x                                            0x922260
#define CXWnd__Move_x                                              0x920AD0
#define CXWnd__Move1_x                                             0x920B80
#define CXWnd__ProcessTransition_x                                 0x921680
#define CXWnd__Refade_x                                            0x920E60
#define CXWnd__Resize_x                                            0x921100
#define CXWnd__Right_x                                             0x921910
#define CXWnd__SetFocus_x                                          0x9214B0
#define CXWnd__SetFont_x                                           0x921520
#define CXWnd__SetKeyTooltip_x                                     0x922080
#define CXWnd__SetMouseOver_x                                      0x91EF30
#define CXWnd__StartFade_x                                         0x920920
#define CXWnd__GetChildItem_x                                      0x921D60
#define CXWnd__SetParent_x                                         0x9207D0
#define CXWnd__Minimize_x                                          0x921170

// CScreenPieceTemplate
#define CScreenPieceTemplate__IsType_x                             0x958560

// CXWndManager 
#define CXWndManager__DrawCursor_x                                 0x924AA0
#define CXWndManager__DrawWindows_x                                0x924AC0
#define CXWndManager__GetKeyboardFlags_x                           0x9271E0
#define CXWndManager__HandleKeyboardMsg_x                          0x926D90
#define CXWndManager__RemoveWnd_x                                  0x927410
#define CXWndManager__RemovePendingDeletionWnd_x                   0x927990

// CDBStr
#define CDBStr__GetString_x                                        0x5451B0

// EQ_Character 
#define EQ_Character__CastSpell_x                                  0x4BFB80
#define EQ_Character__Cur_HP_x                                     0x4D2B70
#define EQ_Character__Cur_Mana_x                                   0x4DA2C0
#define EQ_Character__GetCastingTimeModifier_x                     0x4C2BF0
#define EQ_Character__GetFocusCastingTimeModifier_x                0x4B33C0
#define EQ_Character__GetFocusRangeModifier_x                      0x4B3510
#define EQ_Character__GetHPRegen_x                                 0x4E01C0
#define EQ_Character__GetEnduranceRegen_x                          0x4E07C0
#define EQ_Character__GetManaRegen_x                               0x4E0C00
#define EQ_Character__Max_Endurance_x                              0x65C990
#define EQ_Character__Max_HP_x                                     0x4D29A0
#define EQ_Character__Max_Mana_x                                   0x65C790
#define EQ_Character__doCombatAbility_x                            0x65ED70
#define EQ_Character__UseSkill_x                                   0x4E2A00
#define EQ_Character__GetConLevel_x                                0x655400
#define EQ_Character__IsExpansionFlag_x                            0x5B8990
#define EQ_Character__TotalEffect_x                                0x4C6990
#define EQ_Character__GetPCSpellAffect_x                           0x4C3940
#define EQ_Character__SpellDuration_x                              0x4C3470
#define EQ_Character__MySpellDuration_x                            0x4B1CF0
#define EQ_Character__GetAdjustedSkill_x                           0x4D58F0
#define EQ_Character__GetBaseSkill_x                               0x4D6890
#define EQ_Character__CanUseItem_x                                 0x4DA5D0

//ProfileManager
#define ProfileManager__GetCurrentProfile_x                        0x8BB6F0

//PlayerClient (to us known as EQPlayer)
#define PlayerClient__GetPcClient_x                                0x672D40

//PcClient
#define PcClient__vftable_x                                        0xAEEF34
#define PcClient__PcClient_x                                       0x652B60

//CCharacterListWnd
#define CCharacterListWnd__SelectCharacter_x                       0x4BA5F0
#define CCharacterListWnd__EnterWorld_x                            0x4B9FC0
#define CCharacterListWnd__Quit_x                                  0x4B9D10
#define CCharacterListWnd__UpdateList_x                            0x4BA1B0

// EQ_Item (ItemClient)
#define EQ_Item__CanDrop_x                                         0x637250
#define EQ_Item__CreateItemTagString_x                             0x895D00
#define EQ_Item__IsStackable_x                                     0x89A9A0
#define EQ_Item__GetImageNum_x                                     0x897880
#define EQ_Item__CreateItemClient_x                                0x636490
#define EQ_Item__GetItemValue_x                                    0x898B80
#define EQ_Item__ValueSellMerchant_x                               0x89C2F0
#define EQ_Item__IsKeyRingItem_x                                   0x89A2C0
#define EQ_Item__CanGoInBag_x                                      0x637370
#define EQ_Item__IsEmpty_x                                         0x899E00
#define EQ_Item__GetMaxItemCount_x                                 0x898F90
#define EQ_Item__GetHeldItem_x                                     0x897750
#define EQ_Item__GetAugmentFitBySlot_x                             0x895560

// EQ_LoadingS 
#define EQ_LoadingS__SetProgressBar_x                              0x563B40
#define EQ_LoadingS__Array_x                                       0xC0EC68

// EQ_PC 
#define EQ_PC__DestroyHeldItemOrMoney_x                            0x65D290
#define EQ_PC__GetAlternateAbilityId_x                             0x8A52E0
#define EQ_PC__GetCombatAbility_x                                  0x8A5950
#define EQ_PC__GetCombatAbilityTimer_x                             0x8A59C0
#define EQ_PC__GetItemRecastTimer_x                                0x65F2F0
#define EQ_PC__HasLoreItem_x                                       0x655C10
#define EQ_PC__AlertInventoryChanged_x                             0x654D30
#define EQ_PC__GetPcZoneClient_x                                   0x681CC0
#define EQ_PC__RemoveMyAffect_x                                    0x662520
#define EQ_PC__GetKeyRingItems_x                                   0x8A6250
#define EQ_PC__GetItemContainedRealEstateIds_x                     0x8A5FE0
#define EQ_PC__GetNonArchivedOwnedRealEstates_x                    0x8A6550

// EQItemList 
#define EQItemList__EQItemList_x                                   0x5BEDD0
#define EQItemList__add_object_x                                   0x5EC2D0
#define EQItemList__add_item_x                                     0x5BF330
#define EQItemList__delete_item_x                                  0x5BF380
#define EQItemList__FreeItemList_x                                 0x5BF280

// EQMisc
#define EQMisc__GetActiveFavorCost_x                               0x541D80

// EQPlayer 
#define EQPlayer__ChangeBoneStringSprite_x                         0x674640
#define EQPlayer__dEQPlayer_x                                      0x6679C0
#define EQPlayer__DoAttack_x                                       0x67C460
#define EQPlayer__EQPlayer_x                                       0x668080
#define EQPlayer__SetNameSpriteState_x                             0x66C350
#define EQPlayer__SetNameSpriteTint_x                              0x66D230
#define PlayerBase__HasProperty_j_x                                0x987120
#define EQPlayer__IsTargetable_x                                   0x9875C0
#define EQPlayer__CanSee_x                                         0x987420
#define EQPlayer__CanSee1_x                                        0x9874F0
#define PlayerBase__GetVisibilityLineSegment_x                     0x9871E0

//PlayerZoneClient
#define PlayerZoneClient__ChangeHeight_x                           0x67F760
#define PlayerZoneClient__GetLevel_x                               0x681D00
#define PlayerZoneClient__IsValidTeleport_x                        0x5ED440
#define PlayerZoneClient__LegalPlayerRace_x                        0x55C970

//EQPlayerManager
#define EQPlayerManager__GetSpawnByID_x                            0x677360
#define EQPlayerManager__GetSpawnByName_x                          0x677410
#define EQPlayerManager__GetPlayerFromPartialName_x                0x6774A0

// KeyPressHandler
#define KeypressHandler__Get_x                                     0x63A9A0
#define KeypressHandler__AttachAltKeyToEqCommand_x                 0x63AA20
#define KeypressHandler__AttachKeyToEqCommand_x                    0x63AA60
#define KeypressHandler__ClearCommandStateArray_x                  0x63BE50
#define KeypressHandler__HandleKeyDown_x                           0x63BE70
#define KeypressHandler__HandleKeyUp_x                             0x63BF10
#define KeypressHandler__SaveKeymapping_x                          0x63C360

// MapViewMap 
#define MapViewMap__Clear_x                                        0x773CD0
#define MapViewMap__SaveEx_x                                       0x777090
#define MapViewMap__SetZoom_x                                      0x77B750

// PlayerPointManager
#define PlayerPointManager__GetAltCurrency_x                       0x8B9760

// StringTable 
#define StringTable__getString_x                                   0x8B4520

// PcZoneClient
#define PcZoneClient__GetPcSkillLimit_x                            0x662190
#define PcZoneClient__RemovePetEffect_x                            0x6627C0
#define PcZoneClient__HasAlternateAbility_x                        0x65C5C0
#define PcZoneClient__GetCurrentMod_x                              0x4E5B20
#define PcZoneClient__GetModCap_x                                  0x4E5A20
#define PcZoneClient__CanEquipItem_x                               0x65CC70
#define PcZoneClient__GetItemByID_x                                0x65F760
#define PcZoneClient__GetItemByItemClass_x                         0x65F8B0
#define PcZoneClient__RemoveBuffEffect_x                           0x6627E0
#define PcZoneClient__BandolierSwap_x                              0x65D840
#define PcZoneClient__GetLinkedSpellReuseTimer_x                   0x65F290

//Doors
#define EQSwitch__UseSwitch_x                                      0x5F2070

//IconCache
#define IconCache__GetIcon_x                                       0x715510

//CContainerMgr
#define CContainerMgr__OpenContainer_x                             0x70D020
#define CContainerMgr__CloseContainer_x                            0x70D2F0
#define CContainerMgr__OpenExperimentContainer_x                   0x70DD70

//CQuantityWnd
#define CQuantityWnd__Open_x                                       0x7CC470

//CHelpWnd
#define CHelpWnd__SetFile_x                                        0x62F3F0

//CLootWnd
#define CLootWnd__LootAll_x                                        0x76A1F0
#define CLootWnd__RequestLootSlot_x                                0x7694D0

//EQ_Spell
#define EQ_Spell__GetSpellLevelNeeded_x                            0x58B340
#define EQ_Spell__SpellAffects_x                                   0x58B7B0
#define EQ_Spell__SpellAffectBase_x                                0x58B570
#define EQ_Spell__IsStackable_x                                    0x4CA590
#define EQ_Spell__GetSpellAffectBySlot_x                           0x4CA3B0
#define EQ_Spell__GetSpellAffectByIndex_x                          0x4B7890
#define EQ_Spell__IsSPAStacking_x                                  0x58C600
#define EQ_Spell__IsSPAIgnoredByStacking_x                         0x58BB10
#define EQ_Spell__IsNoRemove_x                                     0x4CA570
#define EQ_Spell__IsDegeneratingLevelMod_x                         0x58C610
#define __IsResEffectSpell_x                                       0x4C98D0

//EQSpellStrings
#define EQSpellStrings__GetString_x                                0x4AD080

//EQ_Affect
#define EQ_Affect__GetAffectData_x                                 0x8C3CC0

//CTargetWnd
#define CTargetWnd__GetBuffCaster_x                                0x813200
#define CTargetWnd__WndNotification_x                              0x812A40
#define CTargetWnd__RefreshTargetBuffs_x                           0x812D10
#define CTargetWnd__HandleBuffRemoveRequest_x                      0x811BA0

//CTaskWnd
#define CTaskWnd__UpdateTaskTimers_x                               0x817870
#define CTaskWnd__ConfirmAbandonTask_x                             0x81A4B0

//CTaskManager
#define CTaskManager__GetEntry_x                                   0x53F7A0
#define CTaskManager__HandleMessage_x                              0x53DC20
#define CTaskManager__GetTaskStatus_x                              0x53F850
#define CTaskManager__GetElementDescription_x                      0x53F8D0

//EqSoundManager
#define EqSoundManager__WavePlay_x                                 0x586430
#define EqSoundManager__PlayScriptMp3_x                            0x5866F0
#define EqSoundManager__SoundAssistPlay_x                          0x69A490
#define EqSoundManager__WaveInstancePlay_x                         0x699A00

//CCombatSkillsSelectWnd
#define CCombatSkillsSelectWnd__ShouldDisplayThisSkill_x           0x530D60

//CTextureAnimation
#define CTextureAnimation__SetCurCell_x                            0x942FD0
#define CTextureAnimation__Draw_x                                  0x9431D0

//CTAFrameDraw
#define CTAFrameDraw__Draw_x                                       0x93EF20

//CAltAbilityData
#define CAltAbilityData__GetMercCurrentRank_x                      0x56B6F0
#define CAltAbilityData__GetMercMaxRank_x                          0x56B680
#define CAltAbilityData__GetMaxRank_x                              0x560A90

//CTargetRing
#define CTargetRing__Cast_x                                        0x62D510

//CharacterBase
#define CharacterBase__GetMemorizedSpell_x                         0x4CA390
#define CharacterBase__CreateItemGlobalIndex_x                     0x517760
#define CharacterBase__CreateItemIndex_x                           0x635640
#define CharacterBase__GetItemPossession_x                         0x5033D0
#define CharacterBase__GetItemByGlobalIndex_x                      0x8CC310
#define CharacterBase__GetItemByGlobalIndex1_x                     0x8CC370
//#define CharacterBase__GetEffectId_x                               

//CCastSpellWnd
#define CCastSpellWnd__ForgetMemorizedSpell_x                      0x6F3EE0
#define CCastSpellWnd__IsBardSongPlaying_x                         0x6F4710
#define CCastSpellWnd__RefreshSpellGemButtons_x                    0x6F47C0

//messages
#define msg_spell_worn_off_x                                       0x5B08E0
#define msg_new_text_x                                             0x5A5270
#define __msgTokenTextParam_x                                      0x5B2B80
#define msgTokenText_x                                             0x5B2DD0

//SpellManager
#define SpellManager__vftable_x                                    0xAD72C4
#define SpellManager__SpellManager_x                               0x69D7C0
#define Spellmanager__LoadTextSpells_x                             0x69E0B0
#define SpellManager__GetSpellByGroupAndRank_x                     0x69D990

//CCollisionInfoTargetVisibility
#define CCollisionInfoTargetVisibility__CCollisionInfoTargetVisibility_x   0x98B050

//ItemGlobalIndex
#define ItemGlobalIndex__IsKeyRingLocation_x                       0x517DB0
#define ItemGlobalIndex__ItemGlobalIndex_x                         0x5B6D40
#define ItemGlobalIndex__IsEquippedLocation_x                      0x64E310
#define ItemGlobalIndex__IsValidIndex_x                            0x517E40

//ItemBaseContainer
#define ItemBaseContainer__ItemBaseContainer_x                     0x8C4660
#define ItemBaseContainer__CreateItemGlobalIndex_x                 0x8C48E0

//CLargeDialogWnd
#define CLargeDialogWnd__Open_x                                    0x762A90

//CCursorAttachment
#define CCursorAttachment__AttachToCursor_x                        0x711710
#define CCursorAttachment__AttachToCursor1_x                       0x711750
#define CCursorAttachment__Deactivate_x                            0x712740

//CSidlManagerBase
#define CSidlManagerBase__CreateTabWnd_x                           0x935B40
#define CSidlManagerBase__CreatePageWnd_x                          0x935340
//int
#define CSidlManagerBase__FindButtonDrawTemplate_x                 0x9315D0
//CXStr
#define CSidlManagerBase__FindButtonDrawTemplate1_x                0x931560

//CEQSuiteTextureLoader
#define CEQSuiteTextureLoader__GetDefaultUIPath_x                  0x952DF0
#define CEQSuiteTextureLoader__GetTexture_x                        0x952AB0

//CFindItemWnd
#define CFindItemWnd__CFindItemWnd_x                               0x514CE0
#define CFindItemWnd__WndNotification_x                            0x5157C0
#define CFindItemWnd__Update_x                                     0x516330
#define CFindItemWnd__PickupSelectedItem_x                         0x514500

//IString
#define IString__Append_x                                          0x504C00

//Camera
#define CDisplay__cameraType_x                                     0xDE6890
#define EverQuest__Cameras_x                                       0xEA1B0C

//LootFiltersManager
#define LootFiltersManager__AddItemLootFilter_x                    0x51D810
#define LootFiltersManager__GetItemFilterData_x                    0x51D110
#define LootFiltersManager__RemoveItemLootFilter_x                 0x51D140
#define LootFiltersManager__SetItemLootFilter_x                    0x51D360

//COptionsWnd
#define COptionsWnd__FillChatFilterList_x                          0x7BA2B0

//CResolutionHandler__UpdateResolution_x
#define CResolutionHandler__UpdateResolution_x                     0x990C20
#define CResolutionHandler__GetWindowedStyle_x                     0x694DE0

//CColorPickerWnd
#define CColorPickerWnd__Open_x                                    0x709CD0

//CDistillerInfo
#define CDistillerInfo__GetIDFromRecordNum_x                       0x8D9C60
#define CDistillerInfo__Instance_x                                 0x8D9C00

//CGroupWnd
#define CGroupWnd__WndNotification_x                               0x728D00
#define CGroupWnd__UpdateDisplay_x                                 0x728050

//ItemBase
#define ItemBase__IsLore_x                                         0x89A370
#define ItemBase__IsLoreEquipped_x                                 0x89A3E0

//EQPlacedItemManager
#define EQPlacedItemManager__Instance_x                            0x5EC230
#define EQPlacedItemManager__GetItemByGuid_x                       0x5EC370
#define EQPlacedItemManager__GetItemByRealEstateAndRealEstateItemIds_x 0x5EC3D0

//RealEstateManagerClient
#define RealEstateManagerClient__Instance_x                        0x68A3F0
#define RealEstateManagerClient__GetItemByRealEstateAndItemIds_x   0x68DD50

//CFactionWnd
#define CFactionWnd__CFactionWnd_x                                 0x50B000

//FactionManagerClient
#define FactionManagerClient__Instance_x                           0x4F7410
#define FactionManagerClient__HandleFactionMessage_x               0x4F7A80
//work in progress, these needs to be investigated
#define FactionManagerClient__GetFactionStanding_x                 0x4F8080
#define FactionManagerClient__GetMaxFaction_x                      0x4F809F
#define FactionManagerClient__GetMinFaction_x                      0x4F8050

//BaseProfile (PCHARINFO2)
#define BaseProfile__GetItemPossession_x                           0x5033A0

//ArrayClass
#define ArrayClass__DeleteElement_x                                0x91A280

//AggroMeterManagerClient
#define AggroMeterManagerClient__Instance_x                        0x49BDD0

//ClientSOIManager
#define ClientSOIManager__GetSingleton_x                           0x503610

//MercenaryAlternateAdvancementManagerClient
#define MercenaryAlternateAdvancementManagerClient__Instance_x     0x56AB70

//CTargetManager
#define CTargetManager__Get_x                                      0x6A1060

//RealEstateManagerClient
#define RealEstateManagerClient__Instance_x                        0x68A3F0

//CBroadcast (returns what we called pinstCTextOverlay_x)
#define CBroadcast__Get_x                                          0x4A8C70

//EQGroundItemListManager
#define EQGroundItemListManager__Instance_x                        0x5BF220

//EverQuestinfo
#define EverQuestinfo__IsItemPending_x                             0xF3F1A0

//CAAWnd
#define CAAWnd__ShowAbility_x                                      0x6BD7C0
#define CAAWnd__UpdateSelected_x                                   0x6BA030
#define CAAWnd__Update_x                                           0x6BCAE0

//CXRect
#define CXRect__operator_and_x                                     0x71FDE0

//CUnSerializeBuffer
#define CUnSerializeBuffer__GetString_x                            0x482D50

//patterns
//LoginController__GiveTime_x
// 56 8B F1 E8 ? FD FF FF 8B CE 5E E9 ? ? FF FF C7 01
//Feb 16 2018 Test
//IDA Style Sig: 56 8B F1 E8 ? ? ? ? 8B CE
static PBYTE lpPattern = (PBYTE)"\x56\x8B\xF1\xE8\x00\x00\x00\x00\x8B\xCE";
static char lpMask[] = "xxxx????xx";

//__eqgraphics_fopen
//Jan 09 2020 EQGraphicsDX9.dll
//IDA Style Sig: E8 ? ? ? ? 83 C4 08 89 45 A0
//#define __eqgraphics_fopen_x                                       0x101473C8
static PBYTE efPattern = (PBYTE)"\xE8\x00\x00\x00\x00\x83\xC4\x08\x89\x45\xA0";
static char efMask[] = "x????xxxxxx";
