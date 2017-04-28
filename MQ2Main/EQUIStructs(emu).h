/*****************************************************************************
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
#pragma pack(push)
#pragma pack(8)
namespace EQUIStructs
{
enum UIType
{
   UI_Unknown=-1,
   UI_Class=0,
   UI_RGB=1,
   UI_Point=2,
   UI_Size=3,
   UI_TextureInfo=4,
   UI_Frame=5,
   UI_Ui2DAnimation=6,
   UI_ButtonDrawTemplate=7,
   UI_GaugeDrawTemplate=8,
   UI_SpellGemDrawTemplate=9,
   UI_FrameTemplate=10,
   UI_ScrollbarDrawTemplate=11,
   UI_WindowDrawTemplate=12,
   UI_SliderDrawTemplate=13,
   UI_ScreenPiece=14,
   UI_StaticScreenPiece=15,
   UI_StaticAnimation=16,
   UI_StaticTintedBlendAnimation=17,
   UI_StaticText=18,
   UI_StaticFrame=19,
   UI_StaticHeader=20,
   UI_LayoutStrategy=21,
   UI_LayoutVertical=22,
   UI_LayoutHorizontal=23,
   UI_Control=24,
   UI_TemplateAssoc=25,
   UI_TemplateScreen=26,
   UI_ListboxColumn=27,
   UI_Listbox=28,
   UI_Button=29,
   UI_Gauge=30,
   UI_SpellGem=31,
   UI_HtmlComponent=32,
   UI_InvSlot=33,
   UI_EditBox=34,
   UI_Slider=35,
   UI_Label=36,
   UI_STMLBox=37,
   UI_TreeView=38,
   UI_Combobox=39,
   UI_Page=40,
   UI_TabBox=41,
   UI_LayoutBox=42,
   UI_HorizontalLayoutBox=43,
   UI_VerticalLayoutBox=44,
   UI_FinderBox=45,
   UI_TileLayoutBox=46,
   UI_NamedTemplatePiece=47,
   UI_TemplateContainer=48,
   UI_Screen=49,
   UI_SuiteDefaults=50,
   UI_Screens=51,
   UI_TopLevelWindowList=52,
   UI_HotButton=53,
};

// ***************************************************************************
// Structures
// ***************************************************************************

// 10-24-2003 Lax - Massive overhaul.
// See http://macroquest.sourceforge.net/phpBB2/viewtopic.php?t=3569 for
// standards on structures to make things easier on everyone!
// -- ONLY applies to EQ structs, not MQ structs like alerts, etc.

typedef struct _EQCURRENTSELECTION {
/*0x00*/   DWORD   Unknown;
/*0x04*/   DWORD   SelectedWnd;     //address to selection in tree
/*0x08*/   DWORD   Unknownlodncrap;
/*0x0c*/   DWORD   NumberOfSlots;   //merchantslots == 79
/*0x10*/   DWORD   TextureAnim;
/*0x14*/
} EQCURRENTSELECTION, *PEQCURRENTSELECTION;

//CXWnd see B0BED4 in apr 21 2016 eqgame.exe (Live) -eqmule
#define VFTABLE \
/*0x000*/ LPVOID  IsValid; \
/*0x004*/ LPVOID  vector_deleting_destructor; \
/*0x008*/ LPVOID  DrawNC; \
/*0x00c*/ LPVOID  PostDraw; \
/*0x010*/ LPVOID  PostDraw2; \
/*0x014*/ LPVOID  DrawCursor; \
/*0x018*/ LPVOID  CXWnd__OnResize; \
/*0x01c*/ LPVOID  PostDraw3; \
/*0x020*/ LPVOID  DrawBackground; \
/*0x024*/ LPVOID  DrawTooltip; \
/*0x028*/ LPVOID  DrawTooltipAtPoint; \
/*0x02c*/ LPVOID  GetMinimizedRect; \
/*0x030*/ LPVOID  DrawTitleBar; \
/*0x034*/ LPVOID  GetCursorToDisplay; \
/*0x038*/ LPVOID  HandleLButtonDown; \
/*0x03c*/ LPVOID  HandleLButtonUp; \
/*0x040*/ LPVOID  HandleLButtonHeld; \
/*0x044*/ LPVOID  HandleLButtonUpAfterHeld; \
/*0x048*/ LPVOID  HandleRButtonDown; \
/*0x04c*/ LPVOID  HandleRButtonUp; \
/*0x050*/ LPVOID  HandleRButtonHeld; \
/*0x054*/ LPVOID  HandleRButtonUpAfterHeld; \
/*0x058*/ LPVOID  HandleWheelButtonDown; \
/*0x05c*/ LPVOID  HandleWheelButtonUp; \
/*0x060*/ LPVOID  HandleMouseMove; \
/*0x064*/ LPVOID  HandleWheelMove; \
/*0x068*/ LPVOID  GetDragDropCursor; \
/*0x06c*/ LPVOID  PostDraw4; \
/*0x070*/ LPVOID  OnDragDrop; \
/*0x074*/ LPVOID  GetClickStickCursor; \
/*0x078*/ LPVOID  QueryDropOK; \
/*0x07c*/ LPVOID  OnClickStick; \
/*0x080*/ LPVOID  GetClickStickCursor2; \
/*0x084*/ LPVOID  QueryClickStickDropOK; \
/*0x088*/ LPVOID  WndNotification; \
/*0x08c*/ LPVOID  Unknown8c; \
/*0x090*/ LPVOID  ShowWindow; /* Activate */ \
/*0x094*/ LPVOID  Deactivate; \
/*0x098*/ LPVOID  PostDraw5; \
/*0x09c*/ LPVOID  OnResize; \
/*0x0a0*/ LPVOID  OnResize2; \
/*0x0a4*/ LPVOID  Unknown0xa4; \
/*0x0a8*/ LPVOID  Unknown0xa8; \
/*0x0ac*/ LPVOID  OnMinimizeBox; \
/*0x0b0*/ LPVOID  Unknown0xb0; \
/*0x0b4*/ LPVOID  OnTileBox; \
/*0x0b8*/ LPVOID  PostDraw6; \
/*0x0bc*/ LPVOID  OnMove2; \
/*0x0c0*/ LPVOID  OnMove3; \
/*0x0c4*/ LPVOID  OnProcessFrame; /*Dofade*/ \
/*0x0c8*/ LPVOID  OnVScroll; \
/*0x0cc*/ LPVOID  OnHScroll; \
/*0x0d0*/ LPVOID  PostDraw7; \
/*0x0d4*/ LPVOID  OnMove4; \
/*0x0d8*/ LPVOID  Show; \
/*0x0dc*/ LPVOID  Unknown0xdc; \
/*0x0e0*/ LPVOID  Unknown0xe0; \
/*0x0e4*/ LPVOID  OnMove5; \
/*0x0e8*/ LPVOID  GetTooltip; \
/*0x0ec*/ LPVOID  HitTest; \
/*0x0f0*/ LPVOID  GetHitTestRect; \
/*0x0f4*/ LPVOID  GetInnerRect; \
/*0x0f8*/ LPVOID  GetClientRect; \
/*0x0fc*/ LPVOID  GetMinSize; \
/*0x100*/ LPVOID  GetMaxSize; \
/*0x104*/ LPVOID  GetUntileSize; \
/*0x108*/ LPVOID  IsPointTransparent; \
/*0x10c*/ LPVOID  Unknown10c; \
/*0x110*/ LPVOID  Unknown110; \
/*0x114*/ LPVOID  Unknown114; \
/*0x118*/ LPVOID  SetDrawTemplate; \
/*0x11c*/ LPVOID  Move_Rect; /*CXWnd__Move1_x*/ \
/*0x120*/ LPVOID  Move_Point; /*CXWnd__Move_x*/ \
/*0x124*/ LPVOID  SetWindowTextA; \
/*0x128*/ LPVOID  Unknown0x128; /*CXWnd__GetChildWndAt_x*/ \
/*0x12c*/ LPVOID  Unknown0x12c; /*CSidlScreenWnd__GetSidlPiece_x*/ \
/*0x130*/ LPVOID  Unknown0x130; \
/*0x134*/ LPVOID  SetVScrollPos; \
/*0x138*/ LPVOID  SetHScrollPos; \
/*0x13c*/ LPVOID  Unknown0x13c; \
/*0x140*/ LPVOID  Unknown0x140; \
/*0x144*/ LPVOID  SetAttributesFromSidl; \
/*0x148*/ LPVOID  nullsub_1; \
/*0x14c*/ LPVOID  Unknown0x14c; \
/*0x150*/ LPVOID  Unknown0x150; \
/*0x154*/ LPVOID  Unknown0x154; \
/*0x158*/ LPVOID  Unknown0x158; \
/*0x15c*/ LPVOID  PostDraw8; \
/*0x160*/ LPVOID  Unknown0x160; \
/*0x164*/ LPVOID  Unknown0x164; \
/*0x168*/

//CSidlScreenWnd see B0CA84 in apr 21 2016 eqgame.exe (Live) -eqmule
typedef struct _CSIDLWNDVFTABLE {
	VFTABLE
	/*0x168*/ LPVOID  Unknown0x168;
	/*0x16c*/ LPVOID  LoadIniInfo;
	/*0x170*/ LPVOID  StoreIniInfo;
	/*0x174*/ LPVOID  Unknown0x174;
	/*0x178*/ LPVOID  Unknown0x178;
	/*0x17c*/
} CSIDLWNDVFTABLE, *PCSIDLWNDVFTABLE;

typedef struct _CXWNDVFTABLE {
VFTABLE
} CXWNDVFTABLE, *PCXWNDVFTABLE;

// in CChatWindow__CChatWindow
#define EQ_CHAT_HISTORY_OFFSET 0x288
// in CChatWindow__SetChatFont
#define EQ_CHAT_FONT_OFFSET    0x114

#define CXW_NO_VTABLE_BEGIN \
/*0x0004*/ struct _CSIDLWND*    pPrevSiblingWnd; \
/*0x0008*/ struct _CSIDLWND*    pNextSiblingWnd; \
/*0x000c*/ void*        pUnknown0xc; \
/*0x0010*/ struct _CSIDLWND*    pFirstChildWnd; \
/*0x0014*/ BYTE         Unknown0x0014[0x4];

//Actual Size: 1d0 (see 8C1FED in eqgame dated Jun 19 2014) - eqmule
//Actual Size: 1ec (see 8C4FDD in eqgame dated Aug 21 2014) - eqmule
//Actual Size: 1c8 (see 8C614D in eqgame dated Sep 18 2014) - eqmule
//Actual Size: 1d4 (see 8CA01D in eqgame dated Oct 23 2014) - eqmule
//Actual Size: 1e4 (see 8C9A5D in eqgame dated Nov 18 2014) - eqmule
//Actual Size: 1d8 (see 8C90DD in eqgame dated Jan 15 2015) - eqmule
//Actual Size: 1f0 (see 8C945D in eqgame dated Jan 22 2014) - eqmule
//Actual Size: 1d8 (see 8C964D in eqgame dated Feb 09 2014) - eqmule
//Actual Size: 1e8 (see 8C99DD in eqgame dated Feb 20 2014) - eqmule
//Actual Size: 1dc (see 66A2D8 in eqgame dated Mar 10 2015) - eqmule
// actual size 0x1D8 in May 10 2013 Live (see 0x656FC8) - eqmul
#define CXW_NO_VTABLE \
CXW_NO_VTABLE_BEGIN \
/*0x0018*/ bool		bClipToParent; \
/*0x0019*/ BYTE		Alpha; \
/*0x001A*/ bool		Enabled; \
/*0x001C*/ int		Transition; \
/*0x0020*/ bool		MouseOver; /* found in CXWnd__SetMouseOver_x */ \
/*0x0021*/ bool		bBottomAnchoredToTop; \
/*0x0022*/ bool		bScreenClipRectChanged; \
/*0x0024*/ int		BlinkState; \
/*0x0028*/ DWORD	BackgroundDrawType; \
/*0x002C*/ bool		bUseInLayoutHorizontal; \
/*0x002D*/ bool		Locked; /* found in CSidlScreenWnd__LoadIniInfo_x */ \
/*0x0030*/ int		ParentAndContextMenuArrayIndex; \
/*0x0034*/ UINT		Data; \
/*0x0038*/ bool		bClientClipRectChanged; \
/*0x003C*/ void		*pController; \
/*0x0040*/ bool		bRightAnchoredToLeft; \
/*0x0044*/ ArrayClass2_RO<UINT> RuntimeTypes; \
/*0x0060*/ RECT		Location; \
/*0x0070*/ UINT		LastBlinkFadeRefreshTime; \
/*0x0074*/ bool		bMarkedForDelete; \
/*0x0075*/ bool		bClientRectChanged; \
/*0x0076*/ bool		bTiled; \
/*0x0078*/ LONG		BlinkStartTimer; \
/*0x007C*/ bool		bCaptureTitle; \
/*0x0080*/ UINT		FadeDuration; \
/*0x0084*/ DWORD	WindowStyle;    /* bit 1 - vertical scroll, bit 2 - horizontal scroll, bit 4 - title bar?, bit 8 - border */ \
/*0x0088*/ RECT		ClipRectScreen; \
/*0x0098*/ bool		CloseOnESC;     /* found in CSidlScreenWnd__StoreIniInfo_x, close when ESC is pressed */ \
/*0x009C*/ UINT		BlinkFadeFreq; \
/*0x00A0*/ void		*pTipTextObject; \
/*0x00A4*/ bool		bIsParentOrContextMenuWindow; \
/*0x00A8*/ RECT		ClipRectClient; \
/*0x00B8*/ SIZE		MinClientSize; \
/*0x00C0*/ RECT		OldLocation; \
/*0x00D0*/ struct _CSIDLWND	*pWndFocusOther; \
/*0x00D4*/ DWORD	LastTimeMouseOver; \
/*0x00D8*/ DWORD	XMLIndex; \
/*0x00DC*/ UINT		BlinkFadeStartTime; \
/*0x00E0*/ bool		bBringToTopWhenClicked; \
/*0x00E4*/ struct _CXSTR	*DataStr; \
/*0x00E8*/ struct _CXSTR	*Tooltip; /* found in CSidlManager__CreateLabel_x */ \
/*0x00EC*/ void		*IconTextureAnim; \
/*0x00F0*/ DWORD	FadeDelay; \
/*0x00F4*/ bool		bUseInLayoutVertical; \
/*0x00F8*/ UINT		TransitionDuration; \
/*0x00FC*/ int		RightOffset; \
/*0x0100*/ int		BottomOffset; \
/*0x0104*/ bool		bLeftAnchoredToLeft; \
/*0x0108*/ void		*pLayoutStrategy; \
/*0x010C*/ RECT		TransitionRect; \
/*0x011C*/ int		DeleteCount; \
/*0x0120*/ int		ZLayer; /* found in CXWndManager__DrawWindows_x */ \
/*0x0124*/ struct _CXSTR	*XMLToolTip; /* found in CSidlManager__CreateLabel_x */ \
/*0x0128*/ COLORREF	BGColor; /* DO NOT CHNAGE THIS TO AN ARGBCOLOR, it will break the padding since its a union that has bytes in it. */ \
/*0x012C*/ COLORREF	CRNormal; /* found in OnProcessFrame */ \
/*0x0130*/ bool		bAction; \
/*0x0134*/ void		*TitlePiece2; \
/*0x0138*/ bool		Clickable;      /* found in CChatWindow__CChatWindow_x and the button handlers */ \
/*0x0139*/ bool		bMaximized; \
/*0x013C*/ SIZE		MaxClientSize; \
/*0x0144*/ void		*pTextObject; \
/*0x0148*/ BYTE     bResizableMask; \
/*0x014C*/ int		BlinkDuration; \
/*0x0150*/ bool		bTopAnchoredToTop; \
/*0x0154*/ void		*TitlePiece; \
/*0x0158*/ bool		bKeepOnScreen; \
/*0x015C*/ void	    *pFont; \
/*0x0160*/ int		managerArrayIndex; \
/*0x0164*/ RECT		IconRect; \
/*0x0174*/ struct _CSIDLWND	*pParentWindow; /* CXWnd__IsDescendantOf_x has this one, If this is NULL, coordinates are absolute... */ \
/*0x0178*/ BYTE		FadeAlpha; \
/*0x017C*/ int		VScrollPos; \
/*0x0180*/ UINT		BlinkFadeDuration; \
/*0x0184*/ int		LeftOffset; \
/*0x0188*/ void		*DrawTemplate; \
/*0x018C*/ bool		Faded; \
/*0x0190*/ int		VScrollMax; \
/*0x0194*/ bool		ValidCXWnd; /* IsValid has this one */ \
/*0x0195*/ bool		bNeedsSaving; /* will be true if you move or resize the window */ \
/*0x0196*/ bool		dShow; \
/*0x0198*/ int		HScrollPos; \
/*0x019C*/ bool		bEscapableLocked; \
/*0x019D*/ bool		Fades; \
/*0x01A0*/ int		HScrollMax; \
/*0x01A4*/ bool		bHCenterTooltip; \
/*0x01A8*/ struct _CXSTR	*WindowText; /* CXWnd__GetWindowTextA_x has this one */ \
/*0x01AC*/ BYTE		StartAlpha; \
/*0x01AD*/ bool		bIsTransitioning; \
/*0x01AE*/ BYTE		FadeToAlpha; /* found in CSidlScreenWnd__StoreIniInfo_x */ \
/*0x01AF*/ bool		bFullyScreenClipped; \
/*0x01B0*/ DWORD	BGType;         /* found in CSidlScreenWnd__StoreIniInfo_x */ \
/*0x01B4*/ RECT		ClientRect; \
/*0x01C4*/ bool		Unlockable;     /* found in CSidlScreenWnd__LoadIniInfo_x related to Locked */ \
/*0x01C8*/ UINT		TransitionStartTick; \
/*0x01CC*/ bool		bMaximizable; \
/*0x01CD*/ BYTE		TargetAlpha; \
/*0x01CE*/ bool		Minimized; \
/*0x01D0*/ int		TopOffset; \
/*0x01D4*/ bool		bActive;
/*0x01d8*/

#define CXW \
/*0x000*/   struct  _CXWNDVFTABLE   *pvfTable; \
CXW_NO_VTABLE \
/*0x1d8*/

typedef struct _CXWND {
CXW
} CXWND, *PCXWND;
#define GateBind          0
//CSidlScreenWnd__CSidlScreenWnd1_x
//size is 220h in eqgame dated Mayb 10 2013 (see 5B9B4D (cswsize)) -eqmule
#define SIDL \
/*0x1d8*/   union { \
				bool bControlsCreated; \
		        struct _CXSTR  *SidlText; /*found in CChatWindow__WndNotification_x*/\
                LONG   Items; \
            }; \
/*0x1dc*/   union { \
                struct _CXSTR  *SidlScreen; \
                DWORD   SlotID; \
            }; \
/*0x1e0*/   LPVOID  SidlPiece;      /* CScreenPieceTemplate (important) */ \
/*0x1e4*/   union { /*find in CSidlScreenWnd__dCSidlScreenWnd_x*/ \
                struct { \
                    BYTE    Checked; \
                    BYTE    Highlighted; \
                    BYTE    Unused0x1c8[0x2]; \
                }; \
                DWORD MaxChars; \
            }; \
/*0x1e8*/   DWORD   TextureAnim; \
/*0x1ec*/   struct  _CXSTR *   InputText; /*found in CChatWindow__CChatWindow_x*/\
/*0x1f0*/   DWORD   Selector; /*found in CSidlScreenWnd__CSidlScreenWnd1_x*/\
/*0x1f4*/   DWORD   PushToSelector; /*found in CSidlScreenWnd__LoadIniInfo_x*/\
/*0x1f8*/   DWORD   EnableINIStorage; /*found in CSidlScreenWnd__LoadSidlScreen_x*/\
/*0x1fc*/   union { \
                struct _CXSTR *INIStorageName; /*found in CSidlScreenWnd__LoadSidlScreen_x*/\
                struct _EQINVSLOT *pEQInvSlot; \
            }; \
/*0x200*/   BYTE    Unknown0x200[0x10];  \
/*0x210*/   LPVOID  ContextMenu;    /* CTextureAnimation */ \
/*0x214*/   DWORD   Unknown0x214;   /* CTextureAnimation */ \
/*0x218*/   DWORD   Unknown0x218;   /* CTextureAnimation */ \
/*0x21c*/   DWORD   Unknown0x21c; \
/*0x220*/

#define CSW \
/*0x000*/ struct _CSIDLWNDVFTABLE* pvfTable; \
CXW_NO_VTABLE \
SIDL \

typedef struct _CSIDLWND {
CSW
} CSIDLWND, *PCSIDLWND;

typedef struct _CCONTEXTMENUVFTABLE {
VFTABLE
/*0x164*/ LPVOID CListWnd_OnHeaderClick;
/*0x168*/ LPVOID CListWnd__DrawColumnSeparators;
/*0x16c*/ LPVOID CListWnd__DrawSeparator;
/*0x170*/ LPVOID CListWnd__DrawLine;
/*0x174*/ LPVOID CListWnd__DrawHeader;
/*0x178*/ LPVOID CListWnd__DrawItem;
/*0x17c*/ LPVOID CListWnd__DeleteAll;
/*0x180*/ LPVOID CListWnd__Compare;
/*0x184*/ LPVOID CListWnd__Sort;
/*0x188*/ LPVOID CContextMenu__ShowAt;
/*0x18c*/
} CCONTEXTMENUVFTABLE, *PCCONTEXTMENUVFTABLE; 

// size 0x298  09-03-2008
typedef struct _CSIDLMGR {
/*0x000*/    BYTE    Unknown0x000[0x144];
/*0x144*/    PVOID   pXMLDataMgr;
/*0x148*/    BYTE    Unknown0x11c[0x150];
/*0x298*/
} CSIDLMGR, *PCSIDLMGR;

//11-6-2003 lax
//Actual size 0x1a4 Jul 18 2015 (see 4ACB15) - eqmule
typedef struct _CXWNDMGR {
/*0x000*/ LPVOID CXWNDMGR__vtable;
/*0x004*/ DWORD Count;
/*0x008*/ struct _CSIDLWND **pWindows;
/*0x00c*/ BYTE Unknown0x00C[0x40];
/*0x04c*/ DWORD LastInteractionTimeStamp;//when mouse or keyboard was last clicked
/*0x050*/ DWORD MouseMoveTimeStamp;//when mouse was last moved
/*0x054*/ DWORD KeyHeldTimeStamp;//how long current/last key has been held
/*0x058*/ BYTE Unknown0x058[0xc];
/*0x064*/ PCSIDLWND LastWindowSelected;//when you select a window its pointer shows up here
/*0x068*/ PCSIDLWND CurrDraggedWindow;//when you drag a window its pointer shows up here
/*0x06c*/ BYTE Unknown0x06c[0x4];
/*0x070*/ PCSIDLWND LastMouseOver;//Current window pointer is over if this is 0 we are above MAIN Window
/*0x074*/ PCSIDLWND Tooltip;//Last Tooltip to show?
/*0x078*/ ArrayClass_RO<PCXWND> GlobalFocusWindows;
/*0x094*/ POINT MousePoint;
/*0x09c*/ bool bCapsLock;
/*0x09d*/ bool KeyboardFlags[4];    // 7d-80
/*0x0a1*/ BYTE Unknown0x0a1[0x27];
/*0x0c8*/ DWORD ScreenMouseXCopy;
/*0x0cc*/ DWORD ScreenMouseYCopy;
/*0x0d0*/ CHAR Unknown0x0d0[0x3c];
/*0x10c*/ DWORD ScreenExtentX;
/*0x110*/ DWORD ScreenExtentY;
/*0x114*/ ArrayClass_RO<void *>  FontsArray;
/*0x124*/ void *pfontSystem;
/*0x128*/ bool bWindowModeToggling;
/*0x12c*/ HWND* pGlobalHwnd;
/*0x130*/ DWORD ScreenMouseLookX;//last position MouseX was at before we mouselooked
/*0x134*/ DWORD ScreenMouseLookY;//last position MouseY was at before we mouselooked
/*0x138*/ BYTE Unknown0x138[0x6c];
/*0x1a4*/
} CXWNDMGR, *PCXWNDMGR;


typedef struct _CONTENTSARRAY {
    struct _CONTENTS* Array[1];
} CONTENTSARRAY, *PCONTENTSARRAY;

 //Size is 0x39 see 655077 in rof2 live exe -eqmule
enum ChatFilterEnum {
	CHAT_FILTER_SAY,
	CHAT_FILTER_TELL,
	CHAT_FILTER_GROUP,
	CHAT_FILTER_RAID,
	CHAT_FILTER_GUILD,
	CHAT_FILTER_OOC,
	CHAT_FILTER_AUCTION,
	CHAT_FILTER_SHOUT,
	CHAT_FILTER_EMOTE,
	CHAT_FILTER_MELEE_YOUR_HITS,
	CHAT_FILTER_SPELLS_MINE,
	CHAT_FILTER_SKILLS,
	CHAT_FILTER_CHAT1,
	CHAT_FILTER_CHAT2,
	CHAT_FILTER_CHAT3,
	CHAT_FILTER_CHAT4,
	CHAT_FILTER_CHAT5,
	CHAT_FILTER_CHAT6,
	CHAT_FILTER_CHAT7,
	CHAT_FILTER_CHAT8,
	CHAT_FILTER_CHAT9,
	CHAT_FILTER_CHAT10,
	CHAT_FILTER_OTHER,
	CHAT_FILTER_MELEE_YOUR_MISSES,
	CHAT_FILTER_MELEE_YOU_BEING_HIT,
	CHAT_FILTER_MELEE_YOU_BEING_MISSED,
	CHAT_FILTER_MELEE_OTHERS_HITS,
	CHAT_FILTER_MELEE_OTHERS_MISSES,
	CHAT_FILTER_MELEE_MY_DEATH,
	CHAT_FILTER_MELEE_OTHER_PC_DEATH,
	CHAT_FILTER_MELEE_CRITICAL_HITS,
	CHAT_FILTER_MELEE_DISCIPLINES,
	CHAT_FILTER_MELEE_WARNINGS,
	CHAT_FILTER_MELEE_NPC_RAMPAGE,
	CHAT_FILTER_MELEE_NPC_FLURRY,
	CHAT_FILTER_MELEE_NPC_ENRAGE,
	CHAT_FILTER_SPELLS_OTHERS,
	CHAT_FILTER_SPELLS_FAILURES	,
	CHAT_FILTER_SPELLS_CRITICALS,
	CHAT_FILTER_SPELLS_WORN_OFF	,
	CHAT_FILTER_SPELLS_NON_MELEE_HITS,
	CHAT_FILTER_FOCUS_EFFECTS,
	CHAT_FILTER_RANDOM_YOUR_ROLLS,
	CHAT_FILTER_PET_MESSAGES,
	CHAT_FILTER_PET_RAMPAGE_FLURRY,
	CHAT_FILTER_PET_CRITICALS,
	CHAT_FILTER_DAMAGE_SHIELDS_YOU_ATTACKING,
	CHAT_FILTER_EXPERIENCE_MESSAGES,
	CHAT_FILTER_NPC_EMOTES,
	CHAT_FILTER_SYSTEM_MESSAGES,
	CHAT_FILTER_WHO,
	CHAT_FILTER_PET_SPELLS,
	CHAT_FILTER_PET_RESPONSES,
	CHAT_FILTER_ITEM_SPEECH,
	CHAT_FILTER_FELLOWSHIP_MESSAGES,
	CHAT_FILTER_MERCENARY_MESSAGES,
    CHAT_FILTER_PVP_MESSAGES,
	MAX_CHAT_FILTERS,//57 (0x39)
};
// 10-27-2003 Lax
// Actual size 0x208 11-5-2003
//Size 0x248 in rof2 (see 49754A) -eqmule
//todo: go through and correct this struct
typedef struct _EQCHATMGR {
/*0x000*/ struct _EQCHATWINDOW* ChatWnd[0x20];
/*0x080*/ DWORD NumWindows;
/*0x084*/ DWORD ActiveWindow; // CChat::GetActiveChatWindow
/*0x088*/ DWORD LockedWindow; // CChatManager::GetActiveChatWindow
/*0x08c*/ struct _EQCHATWINDOW* ChannelMap[MAX_CHAT_FILTERS];   // channel map
/*0x170*/ LPVOID	pCM_MainMenu;//these are all CContextMenu* but we LPVOID them here since its easy...
/*0x174*/ int		ScrollbarIndex;
/*0x178*/ LPVOID	pCM_LanguageMenu;
/*0x17c*/ int		LanguageMenuIndex;
/*0x180*/ LPVOID	pCM_FilterMenu;
/*0x184*/ int		FilterMenuIndex;
/*0x188*/ int		ChatChannelFilterMenuIndex;
/*0x18c*/ int		MeleeFilterSubMenuIndex;
/*0x190*/ int		SpellsFilterSubMenuIndex;
/*0x194*/ LPVOID	pCM_MeleeMenu;
/*0x198*/ int		MeleeFilterMenuIndex;
/*0x19c*/ LPVOID	pCM_SpellMenu;
/*0x1a0*/ int		SpellsMenuIndex;
/*0x1a4*/ LPVOID	pCM_ChannelMenu;
/*0x1a8*/ int		ChannelMenuIndex;
/*0x1ac*/ LPVOID	pCM_DefaultChannelMenu;
/*0x1b0*/ int		DefaultChannelMenu;
/*0x1b4*/ int		DefaultChannelMenu2;
/*0x1b8*/ LPVOID	pCM_ChatChannelDefChan;
/*0x1bc*/ int		ChatChannelDefChanIndex;
/*0x1c0*/ LPVOID	pCM_YourHitsMenu;
/*0x1c4*/ int		YourHitsMenuIndex;
/*0x1c8*/ LPVOID	pCM_YourMissesMenu;
/*0x1cc*/ int		YourMissesMenuindex;
/*0x1d0*/ LPVOID	pCM_YouBeingHitMenu;
/*0x1d4*/ int		YouBeingHitMenuindex;
/*0x1d8*/ LPVOID	pCM_OthersHitsMenu;
/*0x1dc*/ int		OthersHitsMenuindex;
/*0x1e0*/ LPVOID	pCM_OthersMissesMenu;
/*0x1e4*/ int		OthersMissesMenuindex;
/*0x1e8*/ LPVOID	pCM_AllContextMenu;
/*0x1ec*/ int		AllContextMenuindex;
/*0x1f0*/ LPVOID	pCM_HitModesMenu;
/*0x1f4*/ int		HitModesMenuindex;
/*0x1f8*/ LPVOID	pCM_ReplyToMenu;
/*0x1fc*/ int		ReplyToMenuindex;
/*0x200*/ LPVOID	pCM_TellFriendMenu;
/*0x204*/ int		TellFriendMenuindex;
/*0x208*/ LPVOID	pCM_TellRaidmemberMenu;
/*0x20c*/ int		TellRaidmemberMenuindex;
/*0x210*/ int		ReplyToSubIndex;
/*0x214*/ int		TellFriendSubIndex;
/*0x218*/ int		TellRaidmemberSubIndex;
/*0x21c*/ int		HitModes[0x8];
/*0x23c*/ int		DefaultChannel;
/*0x240*/ LPVOID	pCM_RandomFilterMenu;
/*0x244*/ int		RandomFilterIndex;
/*0x248*/
} EQCHATMGR, *PEQCHATMGR;

// Actual size 0x26c 02-14-2007
// 10-27-2003 Lax
//Size 0x32c see 654E2B in rof2 live exe -eqmule
typedef struct _EQCHATWINDOW {
/*0x000*/ struct _CSIDLWND Wnd;
/*0x220*/ struct _EQCHATMGR *ChatManager; 
/*0x224*/ struct _CSIDLWND* InputWnd;
/*0x228*/ struct _CSIDLWND* OutputWnd;
/*0x22c*/ int ChatChannel;
/*0x230*/ int ChatChannelIndex;
/*0x234*/ CHAR TellTarget[0x40];
/*0x274*/ int Language;
/*0x278*/ bool bIsMainChat;
/*0x279*/ bool bIsTellWnd;
/*0x27c*/ struct _CXSTR *CommandHistory[0x28]; // ->0x198
/*0x31c*/ int HistoryIndex;
/*0x320*/ int HistoryLastShown;
/*0x324*/ int FontSize;//style but yeah...
/*0x328*/ int AlwaysChathereIndex;//menu
/*0x32c*/
} EQCHATWINDOW, *PEQCHATWINDOW;

// actual size 0x14 10-12-2010
//I think this is correct:
//see (69FF1E) in eqgame.exe dated 2013 11 13 -eqmule
typedef struct _EQINVSLOT {
/*0x00*/    LPVOID pvfTable;        // not based on cxwnd
/*0x04*/    struct _EQINVSLOTWND *pInvSlotWnd;
/*0x08*/    DWORD Unknown0x08;
/*0x0C*/    DWORD InvSlot;
/*0x10*/    BYTE Valid;
/*0x11*/    BYTE Unknown0x11[3];
/*0x14*/    
} EQINVSLOT, *PEQINVSLOT;

// actual size 0x2014 10-12-2010
// confirmed (size) 2013 dec 10 eqgame.exe at 497E7A -eqmule
// confirmed 2014-03-31 - eqmule
typedef struct _EQINVSLOTMGR {
/*0x0000*/    DWORD Unknown0x0000;
/*0x0004*/    struct _EQINVSLOT *SlotArray[0x800];
/*0x2004*/    DWORD TotalSlots;
/*0x2008*/    DWORD Unknown0x2008;
/*0x200c*/    struct _EQINVSLOT *pSelectedItem;
/*0x2010*/    DWORD Unknown0x2010;
/*0x2014*/
} EQINVSLOTMGR, *PEQINVSLOTMGR;

// onetimehero 09-17-03
// ContainerWindow
// Actual Size 0x17C old
typedef struct _EQCONTAINERWINDOW {
/*0x000*/ struct _CSIDLWND Wnd;
/*0x148*/ struct _CONTENTS*   pContents;         // Pointer to the contents of the container;
                                                 // Matches the pointer in CHARINFO.Inventory/Bank/World
/*0x14c*/ struct _CSIDLWND*   pSlots[0x0a];
/*0x000*/ struct _CSIDLWND*   pCombine;
/*0x168*/ struct _CSIDLWND*   pDone;
/*0x16c*/ struct _CSIDLWND*   pIcon;
/*0x170*/ struct _CSIDLWND*   pUnknown;
/*0x174*/ struct _CSIDLWND*   pLabel;
/*0x178*/ BYTE Unknown0x178[4];
/*0x17c*/
} EQCONTAINERWINDOW, *PEQCONTAINERWINDOW;

// Actual Size 0x98  03/15/06
typedef struct _EQ_CONTAINERWND_MANAGER {
/*0x000*/   DWORD pvfTable;                           // NOT based on CXWnd.  Contains only destructor
/*0x004*/   PEQCONTAINERWINDOW pPCContainers[0x22];   // All open containers, including World, in order of opening...
/*0x08c*/   PCONTENTS   pWorldContents;               // Pointer to the contents of the world   If NULL, world container isn't open;
/*0x090*/   DWORD dwWorldContainerID;                 // ID of container in zone, starts at one (zero?) and goes up.
/*0x094*/   DWORD dwTimeSpentWithWorldContainerOpen;  // Cumulative counter?
/*0x078*/
} EQ_CONTAINERWND_MANAGER, *PEQ_CONTAINERWND_MANAGER;

typedef struct _POINT3 {
    FLOAT X;
    FLOAT Y;
    FLOAT Z;
} POINT3, *PPOINT3;

// plazmic 08-17-03
// Map Window
typedef struct _MAPLABEL { // sizeof() = 0x38
/*0x00*/    DWORD Unknown0x0;
/*0x04*/    struct _MAPLABEL *pNext;
/*0x08*/    struct _MAPLABEL *pPrev;
/*0x0c*/    POINT3 Location;
/*0x18*/    ARGBCOLOR Color;
/*0x1c*/    DWORD Size;     //1-3;
/*0x20*/    PCHAR Label;
/*0x24*/    DWORD Layer;    //0-3;
/*0x28*/    DWORD Width;
/*0x2c*/    DWORD Height;
/*0x30*/    DWORD unk_0x2c;
/*0x34*/    DWORD unk_0x30;
} MAPLABEL, *PMAPLABEL;

typedef struct _MAPLINE { // sizeof() = 0x28 (think this might be 0x34 now)
    struct _MAPLINE *pNext;
    struct _MAPLINE *pPrev;
    POINT3 Start;
    POINT3 End;
    ARGBCOLOR Color;
    DWORD Layer;          //0-3;
} MAPLINE, *PMAPLINE;

typedef struct _EQTRADESKILLRECIPE {
/*0x00*/ DWORD Unknown0x00;
/*0x04*/ DWORD Trivial;
/*0x08*/ DWORD Unknown0x08;
/*0x0c*/ DWORD Unknown0x0c;
/*0x10*/ DWORD Unknown0x10;
/*0x14*/ CHAR  Name[0x40];
/*0x54*/ DWORD Unknown0x54;
/*0x58*/ DWORD Ingredient[0xa];     // ID# of ingredient(s)
/*0x80*/ DWORD IngredientIcon[0xa]; //Icon# of ingredient(s) (Note: 499 is blank)
} EQTRADESKILLRECIPE, *PEQTRADESKILLRECIPE;

//Size 0x3d8 (02/10/2004)
typedef struct _EQTRADESKILLWINDOW {
/*0x000*/   struct _CSIDLWND Wnd;
/*0x148*/ DWORD Unknown0x148[0x1b];  
/*0x1b4*/ PEQTRADESKILLRECIPE SearchResults[0x64]; 
/*0x344*/ DWORD    Unknown0x344;
/*0x348*/ DWORD Unknown0x348;
/*0x34c*/ DWORD Unknown0x34c;
/*0x350*/ PCONTENTS Container;
/*0x354*/ DWORD Unknown0x354;
/*0x358*/ PEQTRADESKILLRECIPE SelectedRecipe;
/*0x35c*/ DWORD Unknown0x35c;
/*0x360*/ DWORD SkillLevel;
/*0x364*/ DWORD Unknown0x364;
/*0x368*/ DWORD Unknown0x368;
/*0x36c*/ BYTE  Unknown0x36c[0x58];
/*0x3c4*/ DWORD Unknown0x3c4;
/*0x3c8*/ DWORD Unknown0x3c8;
/*0x3cc*/ BYTE  Unknonw0x3cc[0x8];
/*0x3d4*/ DWORD Unknown0x3d4;
/*0x3d8*/ DWORD Unknown0x3d8;
} EQTRADESKILLWINDOW, *PTRADESKILLWINDOW;

typedef struct _GUILDMEMBERINFO {
/*0x000*/ DWORD Unknown0x000; //pointer?
/*0x004*/ BYTE OnlineStatus;  // 00=offline, 01=online
/*0x005*/ BYTE UnknownData0x005;
/*0x006*/ BYTE UnknownData0x006;
/*0x007*/ BYTE Unknown0x007;
/*0x008*/ BYTE Unknown0x008[0x8];
/*0x010*/ CHAR Name[0x40];
/*0x050*/ DWORD Level;
/*0x054*/ DWORD Class; 
/*0x058*/ DWORD Rank;         //0=member, 1=officer, 2=leader
/*0x05c*/ FLOAT UnknownData0x05c;
/*0x060*/ CHAR PublicNote[0x80];
/*0x0e0*/ BYTE Unknown0x0e0[0x180];
/*0x260*/ WORD UnknownData0x260;
/*0x262*/ WORD UnknownData0x262;
/*0x264*/ WORD UnknownData0x264;
/*0x266*/ WORD UnknownData0x266;
} GUILDMEMBERINFO, *PGUILDMEMBERINFO;

//Size: 0x3d0 (02/18/2004)
typedef struct _EQGUILDWINDOW {
/*0x000*/ struct _CSIDLWND Wnd;
/*0x148*/ BYTE  Unknown0x148[0x4c];
/*0x194*/ BYTE Unknown0x194;
/*0x195*/ BYTE ShowOffline;                  // 01 = show offline box is checked
/*0x196*/ BYTE Unknown0x196[0x2];
/*0x198*/ struct _GUILDMEMBERINFO **pMember; // This is a pointer to the beginning of a list of pointers, 
                                             // each representing one player in the guild.  The number of 
                                             // pointers depends upon TotalMemberCount.
/*0x19c*/ DWORD TotalMemberCount;
/*0x1a0*/ DWORD Unknown0x1a0;                // 150?
/*0x1a4*/ DWORD Unknown0x1a4;                // 1?
/*0x1a8*/ DWORD TotalMemberCountAgain;
/*0x1ac*/ BYTE Unknown0x1ac[0x1c];
/*0x1c8*/ CHAR PersonalNotesFilePath[0x40];  // path to personal notes file
/*0x208*/ BYTE Unknown0x208[0x1c0];          // empty
/*0x3c8*/ DWORD Unknown0x3c8;                // 1?
/*0x3cc*/ DWORD Unknown0x3cc;                // some sort of bizaare pointer...
/*0x3d0*/
} EQGUILDWINDOW, *PEQGUILDWINDOW;

typedef struct _EQNOTESWINDOW {
/*0x000*/ struct _CSIDLWND Wnd;
/*0x148*/ BYTE  Unknown0x148[0x04];
/*0x14c*/ struct _CSIDLWND *pEditWnd;
} EQNOTESWINDOW, *PEQNOTESWINDOW;

// Actual size 0x1c8 07-09-2008
typedef struct _EQSPELLINFOWINDOW 
{ 
/*0x000*/ struct _CSIDLWND Wnd;
/*0x208*/ PVOID Unknown0x190;
/*0x20c*/ PVOID Unknown0x194;
/*0x210*/ struct _CSIDLWND *DisplayWnd;
/*0x214*/ PVOID Unknown0x19c; 
/*0x218*/ PVOID Unknown0x1a0; 
/*0x21c*/ PCXSTR ItemInfo;
/*0x220*/ PVOID Unknown0x1a8;
/*0x224*/ DWORD Unknown0x1ac;
/*0x228*/ DWORD Unknown0x1b0;
/*0x22c*/ DWORD Unknown0x1b4;
/*0x230*/ DWORD Unknown0x1b8;
/*0x234*/ PVOID TextureAnim;
/*0x238*/ DWORD SpellID;
/*0x23c*/ DWORD TimeStamp;
/*0x240*/ 
} EQSPELLINFOWINDOW, *PEQSPELLINFOWINDOW;

// Size: 0x1a4 (4/14/2005)
typedef struct _EQMAILWINDOW {
/*0x000*/ struct _CSIDLWND Wnd;
/*0x158  Finish... */
} EQMAILWINDOW, *PEQMAILWINDOW;

#define Fly                     0

// Actual size 0x1c4 10-9-2003
typedef struct _EQHOTBUTTONWINDOW {
/*0x000*/ struct _CSIDLWND Wnd;
/*0x148*/ BYTE Unknown0x138[0xc];
/*0x000*/ DWORD   HotButtonWndView;     //0 to 9 for the different views
/*0x14c*/ struct _CSIDLWND   *HotButtons[0x0a];//these will change when you switch page...
/*0x174*/
} EQHOTBUTTONWINDOW, *PEQHOTBUTTONWINDOW;

typedef struct _LOOTCORPSE {
/*0x000*/   BYTE    Unknown0x000;   // 03 seems very common (for NPC anyway)
/*0x001*/   CHAR    Name[0x4b];
/*0x04c*/   FLOAT   Timer;          //Uncertain the equation used here, but fairly certain it's the timer
/*0x050*/   FLOAT   X;
/*0x054*/   BYTE    Unknown0x054[0x4];
/*0x058*/   FLOAT   Y;
/*0x05c*/   FLOAT   Z;
/*0x060*/   BYTE    Unknown0x060[0x24];
/*0x084*/   DWORD   Flags;
/*0x088*/   BYTE    Unknown0x088[0x4];
/*0x08c*/   DWORD   Unknown0x08c;   //256 seems very common
/*0x090*/
// I think there might be pointers at 0x0e0..but no clue what to
} LOOTCORPSE, *PLOOTCORPSE;

// size 0x180 3-10-2004
typedef struct _EQCOMPASSWINDOW {
/*0x000*/    struct _CSIDLWND Wnd;
/*0x148*/    struct _CSIDLWND *pStrip1;
/*0x14C*/    struct _CSIDLWND *pStrip2;
// in progress.
/*0x16C*/   BOOL DrawLine;
/*0x170*/   DWORD Unknown0x170;
/*0x174*/    DWORD LineRed;
/*0x178*/    DWORD LineGreen;
/*0x17C*/    DWORD LineBlue;
/*0x180*/
} EQCOMPASSWINDOW, *PEQCOMPASSWINDOW;

// used by options window
typedef struct _EQKBASSIGN
{
/*0x00*/    struct _CXSTR *pDescription;
/*0x04*/    DWORD nAssignmentNumber;
/*0x08*/
} EQKBASSIGN, *PEQKBASSIGN;

// size 0x904 3-10-2004
typedef struct _EQOPTIONSWINDOW {
/*0x000*/    struct _CSIDLWND Wnd;
/*0x148*/

/*0x264*/ struct _EQKBASSIGN Binds[0xA1];
/*0x508*/

/*0x904*/
} EQOPTIONSWINDOW, *PEQOPTIONSWINDOW;

// size 0x28 4-24-2004 Lax
typedef struct _EQGRAPHICSENGINE {
/*0x00*/ // Graphics file handler
/*0x04*/
} EQGRAPHICSENGINE, *PEQGRAPHICSENGINE;
#define SafeXLoc 0

//CButtonWnd__CButtonWnd_x
//size is 0x260 see 86D16E in eqgame.exe in May 10 2013 Live - eqmule
typedef struct _CBUTTONWND {
/*0x000*/ struct    _CXWND Wnd;
/*0x1d8*/ BYTE      Unknown[0x4];
/*0x1dc*/ BYTE      State;        //1=down 0=up
/*0x1dd*/ BYTE      Unknown0x1dd; //something to do with State
/*0x1de*/ BYTE      Unknown0x1de[0x12];
/*0x1f0*/ ARGBCOLOR Color;
/*0x1f4*/ BYTE      Unknown0x1f4[0x4c];
/*0x240*/ DWORD     TextureAnim;//im guessing now but I think this is the icon displayed use iconcache->geticon to fill it in...
/*0x244*/ BYTE      Unknown0x23c[0x1c];
/*0x260*/
} CBUTTONWND, *PCBUTTONWND;
 
typedef struct _CTEXTENTRYWND {
/*0x000*/ struct    _CXWND Wnd;
/*0x1c4*/ DWORD     CursorPos1;
/*0x13c*/ DWORD     CursorPos2; //same as above but only this needs to be set to update cursor position
/*0x140*/
} CTEXTENTRYWND, *PCTEXTENTRYWND;

typedef struct _CLABELWND {
/*0x000*/ struct    _CXWND Wnd;
/*0x1c4*/ BYTE      Unknown[0x8];
/*0x1cc*/ DWORD     SidlPiece;
/*0x1d0*/
} CLABELWND, *PCLABELWND;

//
//.text:005FC640 ; public: __thiscall CTargetIndicator::CTargetIndicator(void)
//size 0x48 see 4BA434 in Oct 26 2015 -eqmule
typedef struct _TARGETINDICATOR
{
	/*0x00*/	BYTE Unknown0x00;
	/*0x01*/	BYTE Unknown0x01;
	/*0x02*/	BYTE Unknown0x02[0x2];
	/*0x04*/	PSPAWNINFO lasttarget;
	/*0x08*/	BYTE Unknown0x08[0x10];
	/*0x18*/	BYTE IndicatorHidden;//if this is 1 our mouse is over another window and not on the main one - shouldnt cast when this is 1
	/*0x19*/	BYTE CanActivate;//if 0 the indicator is red if 1 its green, i.e. it can be activated.
	/*0x1a*/	BYTE Unknown0x1a[0x2];
	/*0x1c*/	DWORD somepointer;
	/*0x20*/	DWORD somepointer2;
	/*0x24*/	BYTE ValidForSplash;//this is reliable IF you actually have a target: 3 is self and your merc, 4 is other player, 0 is npc...
	/*0x25*/	BYTE Unknown0x25[0x23];
	/*0x48*/
} TARGETINDICATOR, *PTARGETINDICATOR;

//.text:005FE3E0 ; private: __thiscall CTargetManager::CTargetManager(void)
//size 0x1a4 see 5FE489 in Oct 26 2015 -eqmule
typedef struct _TARGETMANAGER
{
	/*0x000*/ BYTE Unknown0x000[0x184];
	/*0x184*/ DWORD TargetID;
	/*0x188*/ BYTE Unknown0x188[0x4];
	/*0x18c*/ BYTE target_ring_in_focus;
	/*0x18d*/ BYTE Unknown0x18d[3];
	/*0x190*/ float target_ring_range; // squared for easier math
	/*0x194*/ float target_ring_x; // probably vec3 type class, this is whats passed to castspell
	/*0x198*/ float target_ring_y;
	/*0x19c*/ float target_ring_z;
	/*0x1a0*/ BYTE target_ring_good; //0=red 1=green 
	/*0x1a1*/ BYTE Unknown0x1a1[3];
	/*0x1a4*/
} TARGETMANAGER, *PTARGETMANAGER;

//Jul 18 2015 -eqmule
typedef struct _ScreenVector3
{
	union {
		float x; // left to right screen coordinate
		DWORD dx;
	};
	union {
		float y; // top to bottom screen coordinate
		DWORD dy;
	};
	union {
		float z;
		DWORD dz;
	};
} ScreenVector3, *PScreenVector3;
//oct 26 2015 - eqmule 
typedef struct _TARGETRING {
/*0x00*/	DWORD Gem;//the gem the spell below is memmed in... 0-11
/*0x04*/	PSPELL thespell;
/*0x08*/	ScreenVector3 thevec;
/*0x14*/	BOOL Param;
/*0x18*/
} TARGETRING, *PTARGETRING;
//Nov 14 Live 2016 7A9422 //fix this
typedef struct _INVENTORYWND {
/*0x0000*/ struct _CSIDLWND     Wnd;
/*0x0220*/ BYTE         Unknown0x0220[0x80];
/*0x02a0*/ __int64		VitalityCap;
/*0x02a8*/ int			AAVitalityCap;
//more
} INVENTORYWND, *PINVENTORYWND;
/********************************************** AUTO CORRECTED *******************************************/

//CBazaarSearchWnd__CBazaarSearchWnd aBazaarsearchwn
// Actual Size: 0x92bc (see 499033) in May 10 2013 Live - eqmule
typedef struct _BAZAARSEARCHWND {
/*0x0000*/ struct _CSIDLWND     Wnd;
/*0x0220*/ BYTE         Unknown0x0220[0x8ff8];
/*0x9218*/ void **      ppTraderData;
/*0x921c*/ DWORD        hashVal;//find in CBazaarSearchWnd__HandleBazaarMsg_x
/*0x9220*/ BYTE         Unknown0x9220[0x9c];
/*0x92bc*/
} BAZAARSEARCHWND, *PBAZAARSEARCHWND;

//Note to self: cant actually find CombatState in it, so no point in looking through IDA for it, but it IS the last dword... so... until that changes, im just gonna accept it...
// Actual Size: 0x2d4 (see 49896E) in May 10 2013 Live - eqmule
typedef struct _CPLAYERWND {
/*0x0000*/ struct _CSIDLWND     Wnd;
/*0x0220*/ BYTE         Unknown0x0220[0xb0];
/*0x02d0*/ DWORD        CombatState;   // 1=debuffed, 2=combat cooldown, 3=stand, 4=sit
/*0x02d4*/
} CPLAYERWND, *PCPLAYERWND;

// CTargetWindow__CTargetWindow (aTargetwindow)
// Actual Size: 0x8b4 (see 498802) in May 10 2013 Live - eqmule
typedef struct _CTARGETWND {
/*0x0000*/ struct  _CSIDLWND    Wnd;
/*0x0220*/ BYTE         Unknown0x0220[0x198];
/*0x03b8*/ struct _CBUTTONWND * pTargetBuff[NUM_BUFF_SLOTS]; // buff icons
/*0x053c*/ int          BuffSpellID[NUM_BUFF_SLOTS]; // 0xffffffff if no buff
/*0x06c0*/ DWORD        BuffTimer[NUM_BUFF_SLOTS];
/*0x0844*/ BYTE         Unknown0x0844[0x24];
/*0x0868*/ DWORD        Type;              // 1 = self, 4 = group member, 5 = PC, 7 = NPC
/*0x086c*/ BYTE         Unknown0x086c[0x48];
/*0x08b4*/
} CTARGETWND, *PCTARGETWND;

// CBuffWindow__CBuffWindow aBuffwindow
// this is used for both long and shortbuffs...
// Actual Size: 0x708 (see 498696) in May 10 2013 Live - eqmule
typedef struct _EQBUFFWINDOW {
/*0x0000*/ struct _CSIDLWND     Wnd;
/*0x0220*/ BYTE         Unknown0x0220[0xbc];
/*0x02dc*/ struct _CBUTTONWND * pBuff[0x24];    // CButton*
/*0x036c*/ BYTE         Unknown0x036c[0x210];
/*0x057c*/ DWORD        BuffId[NUM_LONG_BUFFS];
/*0x0624*/ DWORD        BuffTimer[NUM_LONG_BUFFS];
/*0x06cc*/ BYTE         Unknown0x06cc[0x28];
/*0x06f4*/ DWORD        MaxLongBuffs;           //0x2a (NUM_LONG_BUFFS)
/*0x06f8*/ DWORD        MaxShortBuffs;          //0x37 (NUM_SHORT_BUFFS)
/*0x06fc*/ BYTE         Unknown0x06fc[0xc];
/*0x0708*/
} EQBUFFWINDOW, *PEQBUFFWINDOW;

// CSpellGemWnd__CSpellGemWnd
// Individual Gems
// Actual Size: 0x2f8 (see 755968) in May 10 2013 Live - eqmule
typedef struct _EQCASTSPELLGEM {
/*0x0000*/ struct    _CXWND     Wnd;
/*0x01d8*/ BYTE         Unknown0x01d8[0x44];
/*0x021c*/ DWORD        TimeStamp;
/*0x0220*/ DWORD        RecastTime;
/*0x0224*/ BYTE         Unknown0x0224[0xa8];
/*0x02cc*/ DWORD        spellicon;       //if this is equal to FFFFFFFF there is no spell memmed in this slot...
/*0x02d0*/ DWORD        spellstate;      // 1 = cast in progress or refreshtime not met 2 means we ducked or aborted cast, 0 means its ok to cast
/*0x02d4*/ BYTE         Unknown0x02d4[0x24];
/*0x02f8*/
} EQCASTSPELLGEM, *PEQCASTSPELLGEM;

// pinstCCastSpellWnd_x
// CCastSpellWnd__CCastSpellWnd  aCastspellwnd
// Actual Size: 0x2b0 (see 498A82) in May 10 2013 Live - eqmule
typedef struct _EQCASTSPELLWINDOW {
/*0x0000*/ struct _CSIDLWND     Wnd;
/*0x0220*/ BYTE         Unknown0x0220[0x14];
/*0x0234*/ struct _EQCASTSPELLGEM   *   SpellSlots[NUM_SPELL_GEMS];//CSPW_Spell%d
/*0x0264*/ BYTE         Unknown0x0264[0x4c];
/*0x02b0*/
} EQCASTSPELLWINDOW, *PEQCASTSPELLWINDOW;

// Actual Size: 0x2ac (see 755A79) in May 10 2013 Live - eqmule
typedef struct _EQINVSLOTWND {
/*0x0000*/ struct _CXWND        Wnd;      //----/ actually CButtonWnd
/*0x01d8*/ BYTE         Unknown0x01d8[0x88];
/*0x0260*/ BYTE         Unknown0x0260[0x4];
/*0x0264*/ LONG         WindowType; // ieatacid has this as InventoryType
									// 0 for inventory
                                    // 01 for bank
                                    // 02 for shared bank
                                    // 03 for trader window
                                    // 04 for World/Tradeskill/Combine
                                    // 11 for loot window
/*0x0268*/ WORD         InvSlot;
/*0x026a*/ WORD         BagSlot;
/*0x026c*/ WORD         GlobalSlot;
/*0x026e*/ WORD         RandomNum;              //no idea what this is, it changes upon login but we need it for moveitem... -eqmule
/*0x0270*/ BYTE         Unknown0x0270[0x20];
/*0x0290*/ struct _EQINVSLOT *  pInvSlot;
/*0x0294*/ BYTE         Unknown0x0294[0x8];
/*0x029c*/ BOOL         ProcessClick;
/*0x02a0*/ BYTE         Unknown0x02a0[0xc];
/*0x02ac*/
} EQINVSLOTWND, *PEQINVSLOTWND;

//CItemDisplayWindow__CItemDisplayWindow (aItemdisplaywin)
// Actual Size: 0x5f4 (see 6A1AFF) in May 10 2013 Live - eqmule
typedef struct _EQITEMWINDOW {
/*0x0000*/ struct _CSIDLWND     Wnd;
/*0x0220*/ struct _CSIDLWND *   DisplayWnd;
/*0x0224*/ BYTE         Unknown0x0224[0x70];
/*0x0294*/ PCXSTR       ItemInfo;
/*0x0298*/ PCXSTR       WindowTitle;
/*0x029c*/ PCXSTR       ItemAdvancedLoreText;
/*0x02a0*/ PCXSTR       ItemMadeByText;
/*0x02a4*/ PCXSTR       UnknownCXStr; // if this is NULL don't populate item data in MQ2ItemDisplay
/*0x02a8*/ BYTE         Unknown0x02a8[0x4];
/*0x02ac*/ PCXSTR       ItemInformationText;//Item Information: Placing this augment into blah blah, this armor can only be used in blah blah
/*0x02b0*/ PCONTENTS    pItem;
/*0x02b4*/ BYTE         Unknown0x02b4[0x33c];
/*0x05f0*/ DWORD        ItemWndIndex;//0-5? you can have max 6 windows up I think before it starts overwriting the sixth.
/*0x05F4*/
} EQITEMWINDOW, *PEQITEMWINDOW;

//CLootWnd__CLootWnd (aLootwnd)
// Actual Size: 0x3a8 (see 498E05) in May 10 2013 Live - eqmule
typedef struct _EQLOOTWINDOW {
/*0x0000*/ struct _CSIDLWND     Wnd;
/*0x0220*/ void *       vftable; // for CLootWnd::DialogResponse handler
/*0x0224*/ BYTE         Unknown0x0224[0x9c];
/*0x02c0*/ DWORD        NumOfSlots;
/*0x02c4*/ BYTE         Unknown0x02c4[0x4];
/*0x02c8*/ struct _INVENTORYARRAY *     pInventoryArray;
/*0x02cc*/ DWORD        Size;
/*0x02d0*/ DWORD        NumOfSlots3;
/*0x02d4*/ BYTE         Unknown0x02d4[0x24];
/*0x02f8*/ BYTE         Unknown0x02f8;
/*0x02f9*/ BYTE         Unknown0x02f9;
/*0x02fa*/ BYTE         Unknown0x02fa;
/*0x02fb*/ BYTE         Unknown0x02fb;
/*0x02fc*/ struct _CSIDLWND *   LootInvWnd;
/*0x0300*/ struct _CSILDWND *   LootSlotWnd[0x22];
/*0x0388*/ struct _CSIDLWND *   LW_CorpseName;
/*0x038c*/ struct _CSIDLWND *   DoneButton;
/*0x0390*/ struct _CSIDLWND *   BroadcastButton;
/*0x0394*/ struct _CSIDLWND *   LootAllButton;
/*0x0398*/ BYTE         Unknown0x0398[0x10];
/*0x03a8*/
} EQLOOTWINDOW, *PEQLOOTWINDOW;

// CMapViewWnd__CMapViewWnd_x
// pLines address (0x5b0) = 0x254 + 0x035c (address of pMapViewMapVfTable)
// Actual Size: 0x624 (see 4984F7) in May 10 2013 Live - eqmule
typedef struct _EQMAPWINDOW {
/*0x0000*/ struct _CSIDLWND     Wnd;
/*0x0220*/ BYTE         Unknown0x0220[0x40];
/*0x0260*/ CHAR         shortzonename[0x20];
/*0x0280*/ BYTE         Unknown0x0280[0x9c];
/*0x031c*/ struct _CXWND  *     wnd;           // its the MVW_MapRenderArea window... found at aMvw_maprendera
/*0x0320*/ BYTE         Unknown0x0320[0x38];
/*0x0358*/ struct _CSIDLWNDVFTABLE    * pMapViewMapVfTable; // found at aMapviewmap
/*0x035c*/ BYTE         Unknown0x035c[0x24c];
/*0x05a8*/ PMAPLINE     pLines;     //0x250
/*0x05ac*/ PMAPLABEL    pLabels;    //0x254
/*0x05b0*/ BYTE         Unknown0x05b0[0x74];
/*0x0624*/
} EQMAPWINDOW, *PEQMAPWINDOW;

struct merchdata {
/*0x00*/ void   *vftable;
/*0x04*/ BYTE   Unknown0x4[0xc];
/*0x10*/ DWORD  MerchSlots;
/*0x14*/ DWORD  SelectedListItem;
/*0x18*/ struct _CONTENTSARRAY *pMerchArray;
/*0x1c*/ DWORD  MerchMaxSlots;
/*0x20*/
};
//confirmed 20130819 - eqmule
struct merch_other {
    merchdata *pMerchData;
    void *other;
    void *other2;
};
//CMerchantWnd__CMerchantWnd (aMerchantwnd)
// Actual Size: 0x2b0 (see 498F1F) in May 10 2013 Live - eqmule
typedef struct _EQMERCHWINDOW {
/*0x0000*/ struct _CSIDLWND     Wnd;
/*0x0220*/ BYTE         Unknown0x0220[0xc];
/*0x022c*/ merch_other *        pMerchOther;//found in CMerchantWnd__CMerchantWnd
/*0x0230*/ BYTE         Unknown0x0230[0x8];
/*0x0238*/ FLOAT        Markup;//found in CMerchantWnd__DisplayBuyOrSellPrice_x
/*0x023c*/ BYTE         Unknown0x023c[0xc];
/*0x0248*/ DWORD        SelectedSlotID;//found in CMerchantWnd__RequestBuyItem_x
/*0x024c*/ BYTE         Unknown0x024c[0x64];
/*0x02b0*/
} EQMERCHWINDOW, *PEQMERCHWINDOW;

//CPetInfoWindow__CPetInfoWindow aPetinfowindow
// Actual Size: 0x920 (see 497F32) in May 10 2013 Live - eqmule
typedef struct _EQPETINFOWINDOW {
/*0x0000*/ struct _CSIDLWND     Wnd;
/*0x0220*/ BYTE         Unknown0x0220[0x8];
/*0x0228*/ DWORD                PetSpawnID;//The pets SpawnID
/*0x022c*/ BYTE         Unknown0x022c[0x8];
/*0x0234*/ struct _CBUTTONWND * pButton[0xe];//there are 14 buttons on the petinfowin with text that can be set to attack,none,back and so on, these are those...
/*0x026c*/ BYTE         Unknown0x026c[0x8];
/*0x0274*/ struct _CBUTTONWND * pAttackButton;//if 0 its not assigned.
/*0x0278*/ struct _CBUTTONWND * pQAttackButton;//if 0 its not assigned.
/*0x027c*/ struct _CBUTTONWND * pFollowButton;//if 0 its not assigned.
/*0x0280*/ struct _CBUTTONWND * pGuardButton;//if 0 its not assigned.
/*0x0284*/ struct _CBUTTONWND * pSitButton;//if 0 its not assigned.
/*0x0288*/ struct _CBUTTONWND * pStopButton;//if 0 its not assigned.
/*0x028c*/ BYTE         Unknown0x028c[0x1bc];
/*0x0448*/ struct _CSIDLWND *   pWnd[NUM_BUFF_SLOTS]; // buff icons?
/*0x05cc*/ int          Buff[NUM_BUFF_SLOTS];        // Spell ID# of each buff -- 97 total
/*0x0750*/ BYTE         Unknown0x0750[0x20];
/*0x0770*/ DWORD        PetBuffTimer[NUM_BUFF_SLOTS]; // duration until buff fades, in thousands of a second
/*0x08f4*/ BYTE         Unknown0x08f4[0x1f];
/*0x0913*/ BYTE         Sit;//0/1 is off/on
/*0x0914*/ BYTE         Stop;//0/1 is off/on
/*0x0915*/ BYTE         ReGroup;//0/1 is off/on
/*0x0916*/ BYTE         Follow;//0/1 is off/on
/*0x0917*/ BYTE         Guard;//0/1 is off/on
/*0x0918*/ BYTE         Taunt;//0/1 is off/on
/*0x0919*/ BYTE         Hold;//0/1 is off/on
/*0x091a*/ BYTE         GHold;//0/1 is off/on
/*0x091b*/ BYTE         Focus;//0/1 is off/on
/*0x091c*/ BYTE         Unknown0x091c[0x4];
/*0x0920*/
} EQPETINFOWINDOW, *PEQPETINFOWINDOW;

// CRaidWindow__CRaidWindow aRaidwindow
// Actual Size: 0x394 (see 498225) in May 10 2013 Live - eqmule
typedef struct _EQRAIDWINDOW {
/*0x0000*/ struct _CSIDLWND     Wnd;
/*0x0220*/ BYTE         Unknown0x0220[0xb8];
/*0x02d8*/ DWORD        ClassColors[0x10];
/*0x0318*/ BYTE         Unknown0x0314[0x7c];
/*0x0394*/
} EQRAIDWINDOW, *PEQRAIDWINDOW;

//CTradeWnd__CTradeWnd (aTradewnd)
// Actual Size: 0x300 (see 498F7B) in May 10 2013 test - eqmule
typedef struct _EQTRADEWINDOW {
/*0x0000*/ struct _CSIDLWND     Wnd;
/*0x0220*/ BYTE         Unknown0x0220[0xd8];
/*0x02f8*/ BYTE         HisTradeReady;
/*0x02f9*/ BYTE         MyTradeReady;
/*0x02fa*/ BYTE         TradeWndOpen;
/*0x02fb*/ BYTE         Unknown0x02fb[0x5];
/*0x0300*/
} EQTRADEWINDOW, *PEQTRADEWINDOW;
};
using namespace EQUIStructs;
#pragma pack(pop)