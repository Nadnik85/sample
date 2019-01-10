// MQ2Yes.cpp
// Will attempt to say to most windows that ask for confirmation, good for boxing.
// 01-10-2019 Kaen01: Cleanup and possible fixes

#include "../MQ2Plugin.h"

PreSetup("MQ2Yes");

VOID YesCommand(PSPAWNINFO pChar, PCHAR szLine)
{
	DoCommand(GetCharInfo()->pSpawn,"/squelch /notify LargeDialogWindow LDW_YesButton leftmouseup");
	DoCommand(GetCharInfo()->pSpawn,"/squelch /notify LargeDialogWindow LDW_OkButton leftmouseup");
	DoCommand(GetCharInfo()->pSpawn,"/squelch /notify ConfirmationDialogBox CD_Yes_Button leftmouseup");
	DoCommand(GetCharInfo()->pSpawn,"/squelch /notify ConfirmationDialogBox CD_OK_Button leftmouseup");
	DoCommand(GetCharInfo()->pSpawn,"/squelch /notify TradeWND TRDW_Trade_Button leftmouseup");
	DoCommand(GetCharInfo()->pSpawn,"/squelch /notify GiveWnd GVW_Give_Button leftmouseup");
	DoCommand(GetCharInfo()->pSpawn,"/squelch /notify ProgressionSelectionWnd ProgressionTemplateSelectAcceptButton leftmouseup");
	DoCommand(GetCharInfo()->pSpawn,"/squelch /notify TaskSelectWnd TSEL_AcceptButton leftmouseup");
	DoCommand(GetCharInfo()->pSpawn,"/squelch /notify RaidWindow RAID_AcceptButton leftmouseup");
}
// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin()
{
	AddCommand("/yes",YesCommand);
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin()
{
  RemoveCommand("/yes");
}
