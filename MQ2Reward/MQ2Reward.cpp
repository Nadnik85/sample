    // MQ2Reward.cpp : Defines the entry point for the DLL application.
    //
     
    // PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
    // are shown below. Remove the ones your plugin does not use.  Always use Initialize
    // and Shutdown for setup and cleanup, do NOT do it in DllMain.
     
    #include "../MQ2Plugin.h"
     
    PreSetup("MQ2Reward");
     
    VOID AutoReward(PSPAWNINFO pChar, PCHAR szLine);
    VOID AcceptReward(CXWnd *pWnd);
     
    CHAR szBuffer[MAX_STRING] = {0};
     
    bool debug = false;
     
    // Called once, when the plugin is to initialize
    PLUGIN_API VOID InitializePlugin(VOID)
    {
       DebugSpewAlways("Initializing MQ2Reward");
       AddCommand("/reward",AutoReward);
    }
     
    // Called once, when the plugin is to shutdown
    PLUGIN_API VOID ShutdownPlugin(VOID)
    {
       DebugSpewAlways("Shutting down MQ2Reward");
       RemoveCommand("/reward");
    }
     
    VOID AutoReward(PSPAWNINFO pChar, PCHAR szLine)
    {
       CHAR szArg1[MAX_STRING] = {0};
       CHAR szArg2[MAX_STRING] = {0};
     
       GetArg(szArg1, szLine, 1);
       GetArg(szArg2, szLine, 2);
     
       int arg1 = atoi(szArg1) - 1;
       int arg2 = atoi(szArg2) - 1;
     
       CHAR cxStr[MAX_STRING] = {0};
       CXStr Str;
     
       if (arg1 < 0) {
          sprintf_s(szBuffer, "Illegal option index: '%s'.", szArg1);
          WriteChatColor(szBuffer,USERCOLOR_DEFAULT);
          return;
       }
     
       if (arg2 < 0 && strlen(szArg2) > 0) {
          sprintf_s(szBuffer, "Illegal item index: '%s'.", szArg2);
          WriteChatColor(szBuffer,USERCOLOR_DEFAULT);
          return;
       }
     
       CHAR descr[MAX_STRING] = {0};
       sprintf_s(descr, "option index: %s and item index: %s", szArg1, (arg2 > -1 ? szArg2 : "NONE"));
     
       if (debug) {
          sprintf_s(szBuffer, "Trying to find reward at %s.", descr);
          WriteChatColor(szBuffer,USERCOLOR_DEFAULT);
       }
     
        //             Parent                               TabWindow           PageTemplate
        CXWnd *pWnd1 = (CXWnd*)FindMQ2Window("RewardSelectionWnd")->GetFirstChildWnd()->GetFirstChildWnd();
       
       while (pWnd1) {
          if (((PCSIDLWND)pWnd1)->IsVisible()) {
     
             CListWnd *pListOpt = (CListWnd*)((CSidlScreenWnd*)(pWnd1->GetChildItem("RewardSelectionOptionList")));
             pListOpt->SetCurSel(arg1);
     
             //CXMLData *pXMLData = ((CXWnd*)pListOpt)->GetXMLData();
             //GetCXStr(pXMLData->Name.Ptr,cxStr,MAX_STRING);
     
             //sprintf_s(szBuffer, "Name: %s", cxStr);
             //WriteChatColor(szBuffer,USERCOLOR_DEFAULT);
     
             if (strlen(szArg2) == 0) {
                AcceptReward(pWnd1);
             }
             else {
                // Once the RewardSelectionOptionList has been selected we
                // have to get the PageTemplate again to get the regenerated
                // RewardSelectionItemList.
     
                //             Parent                               TabWindow           PageTemplate
                // CXWnd *pWnd2 = FindMQ2Window("RewardSelectionWnd")->GetFirstChildWnd()->GetFirstChildWnd();
                CXWnd *pWnd2 = (CXWnd*)FindMQ2Window("RewardSelectionWnd")->GetFirstChildWnd()->GetFirstChildWnd();
     
                while (pWnd2) {
                   if (((PCSIDLWND)pWnd2)->IsVisible()) {
     
                      CListWnd *pListItem = (CListWnd*)((CSidlScreenWnd*)(pWnd2->GetChildItem("RewardSelectionItemList")));
                      pListItem->SetCurSel(arg2);
     
                      AcceptReward(pWnd2);
     
                      return;
                   }
     
                   // pWnd2 = pWnd2->GetNextSib();
                   pWnd2 = (CXWnd*)pWnd2->GetNextSiblingWnd();
                }
             }
          }
     
          pWnd1 = (CXWnd*)pWnd1->GetNextSiblingWnd();
       }
     
       sprintf_s(szBuffer, "Could not find reward at %s.", descr);
       WriteChatColor(szBuffer,USERCOLOR_DEFAULT);
    }
     
    VOID AcceptReward(CXWnd *pWnd)
    {
       if (debug) {
          WriteChatColor("Clicking 'Select Option' button.",USERCOLOR_DEFAULT);
       }
     
       CXWnd *pButton = pWnd->GetChildItem("RewardSelectionChooseButton");
       CXRect rect = pButton->GetScreenRect();
       CXPoint pt = rect.CenterPoint();
       pButton->HandleLButtonDown(&pt,0);
       pButton->HandleLButtonUp(&pt,0);
    }
