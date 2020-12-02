// Project: MQ2PortalSetter
// Author: DigitalPixel
// Version: October 31, 2014 6:45 PM
// Updated for Burning Lands - Dewey 12-11-2018
#include "../MQ2Plugin.h"

PreSetup("MQ2PortalSetter");

//#define MQ2PORTALSETTER_DEBUG

bool pluginEnabled = false;
int currentRoutineStep = 0;
unsigned long nextCommandAtTick = 0;
char portalStoneName[MAX_STRING];

static void setPortal();

class CPortalSetterWindow : public CCustomWnd {
 public:
  CPortalSetterWindow(char *Template):CCustomWnd(Template) {
    SetWndNotification(CPortalSetterWindow);
    CobaltScarTwo_Button      = (CButtonWnd*)GetChildItem("CobaltScarTwoButton");
	EasternWasteTwo_Button    = (CButtonWnd*)GetChildItem("EasternWastesTwoButton");
	Stratos_button            = (CButtonWnd*)GetChildItem("StratosButton");
	Overthere_button          = (CButtonWnd*)GetChildItem("OverthereButton");
	Lceanium_button           = (CButtonWnd*)GetChildItem("LceaniumButton");
    kattaCastrumDeluge_button = (CButtonWnd*)GetChildItem("KattaCastrumDelugeButton");
    westKarana_button         = (CButtonWnd*)GetChildItem("WestKaranaButton");
    shardsLanding_button      = (CButtonWnd*)GetChildItem("ShardsLandingButton");
    argath_button             = (CButtonWnd*)GetChildItem("ArgathButton");
    guildBanner_button        = (CButtonWnd*)GetChildItem("GuildBannerButton");
    feerrott_button           = (CButtonWnd*)GetChildItem("FeerrottButton");
    brellsRest_button         = (CButtonWnd*)GetChildItem("BrellsRestButton");
    dragonscaleHills_button   = (CButtonWnd*)GetChildItem("DragonscaleHillsButton");
    planeOfTime_button        = (CButtonWnd*)GetChildItem("PlaneOfTimeButton");
    kattaCastrum_button       = (CButtonWnd*)GetChildItem("KattaCastrumButton");
    gorukarMesa_button        = (CButtonWnd*)GetChildItem("GorukarMesaButton");
    arcstone_button           = (CButtonWnd*)GetChildItem("ArcstoneButton");
    planeOfSky_button         = (CButtonWnd*)GetChildItem("PlaneOfSkyButton");
    cobaltScar_button         = (CButtonWnd*)GetChildItem("CobaltScarButton");
    planeOfHate_button        = (CButtonWnd*)GetChildItem("PlaneOfHateButton");
    barindu_button            = (CButtonWnd*)GetChildItem("BarinduButton");
    wallOfSlaughter_button    = (CButtonWnd*)GetChildItem("WallOfSlaughterButton");
    twilightSea_button        = (CButtonWnd*)GetChildItem("TwilightSeaButton");
    undershore_button         = (CButtonWnd*)GetChildItem("UndershoreButton");
    dreadlands_button         = (CButtonWnd*)GetChildItem("DreadlandsButton");
    stonebrunt_button         = (CButtonWnd*)GetChildItem("StonebruntButton");
    icecladOcean_button       = (CButtonWnd*)GetChildItem("IcecladOceanButton");
    lavastorm_button          = (CButtonWnd*)GetChildItem("LavastormButton");
    toxxulia_button           = (CButtonWnd*)GetChildItem("ToxxuliaButton");
    northKarana_button        = (CButtonWnd*)GetChildItem("NorthKaranaButton");
    commonlands_button        = (CButtonWnd*)GetChildItem("CommonlandsButton");
    greaterFaydark_button     = (CButtonWnd*)GetChildItem("GreaterFaydarkButton");
  }

  ~CPortalSetterWindow() {}

  int WndNotification(CXWnd *pWnd, unsigned int Message, void *unknown);
  CButtonWnd* CobaltScarTwo_Button;
  CButtonWnd* EasternWasteTwo_Button;
  CButtonWnd *Stratos_button;
  CButtonWnd *Overthere_button;
  CButtonWnd *Lceanium_button;
  CButtonWnd *kattaCastrumDeluge_button;
  CButtonWnd *westKarana_button;
  CButtonWnd *shardsLanding_button;
  CButtonWnd *argath_button;
  CButtonWnd *guildBanner_button;
  CButtonWnd *feerrott_button;
  CButtonWnd *brellsRest_button;
  CButtonWnd *dragonscaleHills_button;
  CButtonWnd *planeOfTime_button;
  CButtonWnd *kattaCastrum_button;
  CButtonWnd *gorukarMesa_button;
  CButtonWnd *arcstone_button;
  CButtonWnd *planeOfSky_button;
  CButtonWnd *cobaltScar_button;
  CButtonWnd *planeOfHate_button;
  CButtonWnd *barindu_button;
  CButtonWnd *wallOfSlaughter_button;
  CButtonWnd *twilightSea_button;
  CButtonWnd *undershore_button;
  CButtonWnd *dreadlands_button;
  CButtonWnd *stonebrunt_button;
  CButtonWnd *icecladOcean_button;
  CButtonWnd *lavastorm_button;
  CButtonWnd *toxxulia_button;
  CButtonWnd *northKarana_button;
  CButtonWnd *commonlands_button;
  CButtonWnd *greaterFaydark_button;
};

int CPortalSetterWindow::WndNotification(CXWnd *pWnd, unsigned int Message, void *unknown) {

  if (pWnd==0) {
    if (Message==XWM_CLOSE) {
        SetVisible(1);
        return 1;
    }
  }

  if (pWnd == (CXWnd*)CobaltScarTwo_Button) {
      if (Message == XWM_LCLICK) {
#ifdef MQ2PORTALSETTER_DEBUG
          WriteChatf("PortalSetterWindow::CobaltScarTwo_Button - LCLICK");
#endif
          sprintf_s(portalStoneName, "Othmir Clamshell");
          currentRoutineStep = 1;
          setPortal();
      }
  }

  if (pWnd == (CXWnd*)EasternWasteTwo_Button) {
	  if (Message == XWM_LCLICK) {
			#ifdef MQ2PORTALSETTER_DEBUG
			WriteChatf("PortalSetterWindow::EasternWasteTwo_Button - LCLICK");
			#endif
		  sprintf_s(portalStoneName, "Brilliant Frigid Gemstone");
		  currentRoutineStep = 1;
		  setPortal();
	  }
  }

  if (pWnd == (CXWnd*)Stratos_button) {
	  if (Message == XWM_LCLICK) {
			#ifdef MQ2PORTALSETTER_DEBUG
			WriteChatf("PortalSetterWindow::Stratos_button - LCLICK");
			#endif
		  sprintf_s(portalStoneName, "Burning Lamp");
		  currentRoutineStep = 1;
		  setPortal();
	  }
  }

  if (pWnd == (CXWnd*)Overthere_button) {
	  if (Message == XWM_LCLICK) {
			#ifdef MQ2PORTALSETTER_DEBUG
			WriteChatf("PortalSetterWindow::Overthere_button - LCLICK");
			#endif
		  sprintf_s(portalStoneName, "Miniature Worker's Sledge Mallet");
		  currentRoutineStep = 1;
		  setPortal();
	  }
  }

  if (pWnd == (CXWnd*)Lceanium_button) {
	  if (Message == XWM_LCLICK) {
		  #ifdef MQ2PORTALSETTER_DEBUG
		  WriteChatf("PortalSetterWindow::Lceanim_button - LCLICK");
		  #endif
		  sprintf_s(portalStoneName, "Fragment of the Combine Spire");
		  currentRoutineStep = 1;
		  setPortal();
	  }
  } else if (pWnd==(CXWnd*)kattaCastrumDeluge_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::kattaCastrumDeluge_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Drowned Katta Castrum Powerstone");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if (pWnd==(CXWnd*)westKarana_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::westKarana_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Stormstone of the West");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)shardsLanding_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::shardsLanding_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Stone of the Shard's Fall");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)argath_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::argath_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Chunk of Argathian Steel");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)guildBanner_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::guildBanner_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Splinter from a Guild Standard");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)feerrott_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::feerrott_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Crystallized Dream of the Feerrott");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)brellsRest_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::brellsRest_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Unrefined Brellium Ore");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)dragonscaleHills_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::dragonscaleHills_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Dragonscale Faycite");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)planeOfTime_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::planeOfTime_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Broken Timestone");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)kattaCastrum_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::kattaCastrum_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Katta Castrum Powerstone");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)gorukarMesa_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::gorukarMesa_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Goru'kar Mesa Sandstone");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)arcstone_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::arcstone_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Arcstone Spirit Sapphire");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)planeOfSky_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::planeOfSky_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Cloudy Stone of Veeshan");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)cobaltScar_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::cobaltScar_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Velium Shard of Cobalt Scar");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)planeOfHate_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::planeOfHate_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Fuligan Soulstone of Innoruuk");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)barindu_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::barindu_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Etched Marble of Barindu");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)wallOfSlaughter_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::wallOfSlaughter_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Chipped Shard of Slaughter");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)twilightSea_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::twilightSea_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Shadowed Sand of the Twilight Sea");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)undershore_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::undershore_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Undershore Coral");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)dreadlands_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::dreadlands_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Shattered Bone of the Dreadlands");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)stonebrunt_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::stonebrunt_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Moss Agate of Stonebrunt");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)icecladOcean_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::icecladOcean_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Frozen Shard of Iceclad");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)lavastorm_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::lavastorm_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Lavastorm Magma");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)toxxulia_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::toxxulia_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Opal of Toxxulia");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)northKarana_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::northKarana_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Karana Plains Pebble");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)commonlands_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::commonlands_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Grassy Pebble Of The Commonlands");
      currentRoutineStep = 1;
      setPortal();
    }
  } else if(pWnd==(CXWnd*)greaterFaydark_button) {
    if(Message==XWM_LCLICK) {
      #ifdef MQ2PORTALSETTER_DEBUG
      WriteChatf("PortalSetterWindow::greaterFaydark_button - LCLICK");
      #endif
      sprintf_s(portalStoneName, "Forest Emerald of Faydark");
      currentRoutineStep = 1;
      setPortal();
    }
  }
  return CSidlScreenWnd::WndNotification(pWnd,Message,unknown);
};

CPortalSetterWindow *pCPortalSetterWindow=0;

void CreatePortalSetterWindow() {
  if (pCPortalSetterWindow) return;
  pCPortalSetterWindow = new CPortalSetterWindow("PortalSetterWindow");
  pCPortalSetterWindow->SetVisible(0);
  pCPortalSetterWindow->SetZLayer(9999);
};

void DestroyPortalSetterWindow() {
  if (!pCPortalSetterWindow) return;
  delete pCPortalSetterWindow;
  pCPortalSetterWindow=0;
};

static char* getPortalVendorName() {
  if (GetCharInfo()->zoneId == 345) return "Zeflmin Werlikanin";
  return "Teleportation Assistant";
}

static double getPortalMerchantDistance() {
  char zPortalMerchantDistance[MAX_STRING];
  sprintf_s(zPortalMerchantDistance, "${Spawn[%s].Distance}", getPortalVendorName());
  ParseMacroData(zPortalMerchantDistance,MAX_STRING);
  return atof(zPortalMerchantDistance);
}

static bool isMerchantPortalSetter() {
	char zMerchantName[MAX_STRING];
	sprintf_s(zMerchantName, "${Merchant.CleanName}");
	ParseMacroData(zMerchantName, MAX_STRING);
	if (strstr(zMerchantName, getPortalVendorName())) {
		return true;
	}
	return false;
}


static bool inPortalMerchantRange() {
  if (getPortalMerchantDistance() < 20.0f) return true;
  return false;
}

static bool inGuildHall() {
  if(strstr(((PZONEINFO)pZoneInfo)->LongName, "Guild Hall") != NULL) return true;
  return false;
}

static void setPortal() {
  if (currentRoutineStep == 1) {
    if (GetCharInfo2()->pInventoryArray->Inventory.Cursor) {
      WriteChatColor("[MQ2PortalSetter] Your cursor must be empty to use portal setter.", CONCOLOR_YELLOW);
      currentRoutineStep = 0;
    }
    char zFreeInventory[MAX_STRING] = "${Me.FreeInventory}";
    ParseMacroData(zFreeInventory,MAX_STRING);
    if(atoi(zFreeInventory) == 0) {
      WriteChatColor("[MQ2PortalSetter] You must have a free inventory slot to use portal setter.", CONCOLOR_YELLOW);
      currentRoutineStep = 0;
    }
  }
  switch (currentRoutineStep) {
    case 1: {
      char zStoneListPosition[MAX_STRING];
      sprintf_s(zStoneListPosition, "${Window[MerchantWnd].Child[MW_ItemList].List[=%s,2]}", portalStoneName);
      ParseMacroData(zStoneListPosition, MAX_STRING);
      SendListSelect("MerchantWnd", "MW_ItemList", (atoi(zStoneListPosition) - 1));
      currentRoutineStep++;
      break;
    }
    case 2: {
      EzCommand("/ctrl /notify MerchantWnd MW_Buy_Button leftmouseup");
      currentRoutineStep++;
      break;
    }
    case 3: {
      PCONTENTS stone = FindItemByName(portalStoneName, true);
      if(stone) {
        char zNotifyCommand[MAX_STRING];
        pCPortalSetterWindow->SetVisible(0);
        SendWndClick("MerchantWnd", "MW_DONE_BUTTON", "leftmouseup");
        sprintf_s(zNotifyCommand, "/itemnotify \"%s\" leftmouseup", portalStoneName);
        EzCommand(zNotifyCommand);
        currentRoutineStep++;
      }
      break;
    }
    case 4: {
      Target(GetCharInfo()->pSpawn, getPortalVendorName());
      currentRoutineStep++;
      break;
    }
    case 5: {
      if ((PSPAWNINFO)pTarget && inPortalMerchantRange()) {
        if (GetCharInfo2()->pInventoryArray->Inventory.Cursor) {
          EzCommand("/click left target");
          currentRoutineStep++;
        }
      } else {
        currentRoutineStep--;
      }
      if (((PCSIDLWND)pGiveWnd)->IsVisible() && currentRoutineStep == 5) {
        currentRoutineStep++;
      }
      break;
    }
    case 6: {
      if (((PCSIDLWND)pGiveWnd)->IsVisible()) {
        SendWndClick("GiveWnd", "GVW_Give_Button", "leftmouseup");
        currentRoutineStep++;
      } else {
        currentRoutineStep--;
      }
      break;
    }
    case 7: {
      if(!((PCSIDLWND)pGiveWnd)->IsVisible()) {
        currentRoutineStep = 0;
      }
      break;
    }
  }
};

PLUGIN_API void InitializePlugin(void)
{
  DebugSpewAlways("Initializing MQ2PortalSetter");
  AddXMLFile("MQUI_PortalSetterWindow.xml");
}

PLUGIN_API VOID ShutdownPlugin(VOID)
{
  DebugSpewAlways("Shutting down MQ2PortalSetter");
  DestroyPortalSetterWindow();
  RemoveXMLFile("MQUI_PortalSetterWindow.xml");
}

PLUGIN_API VOID OnCleanUI(VOID)
{
  DebugSpewAlways("MQ2PortalSetter::OnCleanUI()");
  DestroyPortalSetterWindow();
}

PLUGIN_API VOID OnReloadUI(VOID)
{
  DebugSpewAlways("MQ2PortalSetter::OnReloadUI()");
  CreatePortalSetterWindow();
}

PLUGIN_API VOID SetGameState(DWORD GameState)
{
  DebugSpewAlways("MQ2PortalSetter::SetGameState()");
  if (gGameState == GAMESTATE_INGAME) {
    if(inGuildHall()) {
      if (!pluginEnabled) {
        pluginEnabled = true;
      }
    } else {
      if (pluginEnabled) {
        pluginEnabled = false;
      }
    }
  } else {
    if (pluginEnabled) {
      pluginEnabled = false;
    }
  }
}

PLUGIN_API VOID OnPulse(VOID)
{
	//DebugSpewAlways("MQ2PortalSetter::OnPulse()");
	if (!pluginEnabled) return;
	DWORD currentTick = GetTickCount();
	if (currentTick < nextCommandAtTick) return;
	if (!pCPortalSetterWindow) return;

	//-- Check to see if we need to hide / show the portal window
	if (((PCSIDLWND)pMerchantWnd)->IsVisible() == true) {
		if (currentRoutineStep < 4) {
			if(pCPortalSetterWindow->IsVisible() == false && inPortalMerchantRange() && isMerchantPortalSetter()) {
				pCPortalSetterWindow->SetVisible(true);
			}
		}
	} else {
		if(pCPortalSetterWindow->IsVisible() == true) {
			pCPortalSetterWindow->SetVisible(false);
		}
	}

	//-- If we are out of range then reset state.
	if (currentRoutineStep && !inPortalMerchantRange()) {
		WriteChatColor("[MQ2PortalSetter] Out of range of portal attendant, aborting.", CONCOLOR_RED);
		currentRoutineStep = 0;
	}
	setPortal();
	nextCommandAtTick = currentTick + 60;
}
