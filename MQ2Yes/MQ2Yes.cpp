// MQ2Yes.cpp
// Will attempt to say to most windows that ask for confirmation, good for boxing.
// -------------------------------------------------------------------------------
// Super many thanks to eqmule, knightly, brainiac and wired420 for helping me
// figure this stuff out, i am so in deep water, but they helped alot.
// -------------------------------------------------------------------------------
// 01-10-2019 Kaen01: Cleanup and possible fixes
// 01-11-2019 Kaen01: rewrite to use proper code and calls.
// 01-12-2019 Kaen01: another complete rewrite to make it super awesome
// 01-12-2019 Kaen01: Added a /no command

#include "../MQ2Plugin.h"

PreSetup("MQ2Yes");

inline bool ClickTheThing(PCHAR MyWndName, PCHAR MyButton) {
	if (CSidlScreenWnd* pMyWnd = (CSidlScreenWnd*)FindMQ2Window(MyWndName)) {
		if (pMyWnd->IsVisible() && pMyWnd->IsEnabled()) {
			if (CXWnd* pWnd = pMyWnd->GetChildItem(MyButton)) {
				SendWndClick2(pWnd, "leftmouseup");
				WriteChatf("\ag[MQ2Yes] \awClicked \at%s \awon \ag%s", MyButton, MyWndName);
			}
		}
		return false;
	}
	return false;
}


VOID YesCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	//CXWnd *pWnd = 0;
	ClickTheThing("LargeDialogWindow", "LDW_YesButton");
	ClickTheThing("LargeDialogWindow", "LDW_OKButton");
	ClickTheThing("ConfirmationDialogBox", "CD_Yes_Button");
	ClickTheThing("ConfirmationDialogBox", "CD_OK_Button");
	ClickTheThing("TradeWND", "TRDW_Trade_Button");
	ClickTheThing("GiveWnd", "GVW_Give_Button");
	ClickTheThing("ProgressionSelectionWnd", "ProgressionTemplateSelectAcceptButton");
	ClickTheThing("TaskSelectWnd", "TSEL_AcceptButton");
	ClickTheThing("RaidWindow", "RAID_AcceptButton");
}

VOID NoCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	//CXWnd *pWnd = 0;
	ClickTheThing("LargeDialogWindow", "LDW_NoButton");
	ClickTheThing("ConfirmationDialogBox", "CD_No_Button");
	ClickTheThing("ConfirmationDialogBox", "CD_Cancel_Button");
	ClickTheThing("TradeWND", "TRDW_Cancel_Button");
	ClickTheThing("GiveWnd", "GVW_Cancel_Button");
	ClickTheThing("ProgressionSelectionWnd", "ProgressionTemplateSelectCancelButton");
	ClickTheThing("TaskSelectWnd", "TSEL_DeclineButton");
	ClickTheThing("RaidWindow", "RAID_DeclineButton");
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin()
{
	AddCommand("/yes", YesCommand);
	AddCommand("/no", NoCommand);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin()
{
	RemoveCommand("/yes");
	RemoveCommand("/no");
}
