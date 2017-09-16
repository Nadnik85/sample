//**************************************************************//
// Buff Tool Plugin v1.01
// Next Step evolution of MQ2BuffUtils v1.01
// Based on code from Zeus & JGC84
// by s0rCieR (2005/11/03)
//**************************************************************//
//
// With latest SONY improvement to EQ, reporting buff that fades
// out or get clicked off to the buff caster, it was more harder
// to hides ourself and not look like a robotic buff off clicker.
// Even with some humanish delay to not instantly click off buff
// it was becoming to much evident that we were using such kind
// of tools like MQ2BuffUtils.
//
// I really enjoy MQ2BuffUtils, but with those changes in game,
// i was forced to stop using it. I really miss it during raid
// and decide to resurrect it, give him some nice improvements
// and try to stay as much compatible to MQ2BuffUtils so i dont
// end up rewriting hundreds of macros!
//
// I designed a new approch on way to click off unwanted buffs,
// using free buff slot logic, if i dont have x number of free
// buff slot then click this crap buff out. It theory you could
// have a crap buff for hours on ya, but when you start getting
// more good buff, it will click off crap one to make sure you
// have some free slots for good ones.
//
// You will need to specify for each bloc spells to watch, how
// many free buff slot you should have free before considering
// to click this one off. Supported number of free slot is set
// BLOC_MAXSLOT define in c++ files. (0) means to always click
// off (like MQ2BuffUtils plugin way to handle spells with few
// seconds delay before click it),(1+) mean to click it off if
// you dont have at least 1+ free slot.
//
// Plugin is using onpulse logic to check out buffs and update
// XML Windows. (i try to setup some nice delay to not end ups
// using to much processing times). It will only click off one
// buff max per passes in the onpulse subroutine, it will scan
// first for buff that are "always" to click off, then proceed
// from BLOC_MAXSLOT downto 1 (looking first for buff that are
// most wanted to be clicked off).
//
// New features are XML windows support, they are not required
// to be open for regular operation, but it simplify a lot way
// to setup and manage block spell list, it also bring up some
// nice way to visualize time remaining on buffs (self & pet).
//
//**************************************************************//
// Features:
//
// - XML Windows, fully resizable, cloasable, movable, lockable
//   that remember their save location and states. Auto-disable
//   close (button and escape closing) if window is locked, auto
//   save/load window position and size (on plugin shutdown, or
//   on clean ui or on reaload ui).
//
// - Buff Window List (Self & Pet) sorted by time left on buff,
//   colorfull visual effects(red=detrimental, white=less then
//   1min, teal=less then 3mins, blue=less then 6mins or green
//   if time left greater then 6mins) Toggle option to include
//   songs or not in visual buff list. Buffs Free and Max Slot
//   display and same for Songs. Right or Left Clicking on the
//   Self Buff window will add or remove buff to the bloc list.
//
// - Bloc Window List sorted by buff name, green mean buff that
//   will always been blocked, white for others. Toggle options
//   for On/Off state and Popups. Load and Save Button to saves
//   or load settings. Right or Left Clicking on selected buff,
//   will increase or decrease # of free slots attach to it. If
//   # of free slots reach -1, it will remove that bloc buff.
//
// - TLO supports for macro scripters or HUD maniacs.
//
//**************************************************************//
// MQ2Data:
//
// Block.Ready    (bool Plugin Loaded && InGame && Bloc Enable)
// Block.Quiet    (bool Always true or NULL if plugin not loaded)
// Block.Popup    (bool Should i Popup Msg on Spell Block?)
// Block.Count    (int  # of buff in bloc list)
// Block.Total    (int  # of buff blocked so far)
// Block.Spell[X] (int  # of free slot for a spell -1 not found)
// Block.Maxim[X] (int  Maxim # of Buff (1=buff,2=song,3=pet))
// Block.Avail[X] (int  Avail # of Buff (1=buff,2=song,3=pet))
//
//**************************************************************//
//
// Usage: /Command Common to ALL (ie /block /unblock /clickoff)
//        /Command /Help   (Show Help)
//        /Command /List   (Show List of Spells Being Blocked)
//        /Command /Load   (Load Settings and Bloc List from INI)
//        /Command /Save   (Save Settings and Bloc List to INI)
//        /Command /Block  (Toggle on/off Scan for Bloc Buffs)
//        /Command /Popop  (Toggle on/off Display of Popups)
//        /Command /Window (Popups XML Windows)
//
//        /Block <##/Name/[Set]> <MODE>
//        /Block ##   <MODE> (spell on slot ##, -## for song)
//        /Block Name <MODE> (spell matching that name)
//        /Block [Set]       (spell list from INI)
//
//        <MODE> 0=always(default) 1+ free buff to keep (Optional)
//
//        /Unblock <##/Name/[Set]>
//        /Unblock ##        (remove Block from bloc list ##)
//        /Unblock Name      (remove Block for that spell name)
//        /Unblock [Set]     (remove Block for this List from INI)
//
//        /Clickoff <##/Name/[Set]>
//        /Clickoff ##       (click off slot ##, -## for song)
//        /Clickoff Name     (click off buff matching that name)
//        /Clickoff [Set]    (click off buff from List in INI)
//
//****************************************************************//
// Known Issue:
//
// - Clicking off buff on buff window wont work if eq buff window
//   not open, and same thing also for song. (sending notification
//   to close window wont work wells :P If anyone know a detour to
//   achieve this click off buff when effect/songs window not open
//   let me know :P)
//
// - If i dont find a better way to click off spell (see previous
//   paragraph) i will just add codes to not scan for buff or song
//   if their respective window are not active and open.
//
//****************************************************************//

#include "../MQ2Plugin.h"
using namespace std;
#include <string>
#include <list>

PreSetup("MQ2BuffTool");

// ==================================================================

#define     SKIP_SOMETIC 4        // Skipping Some Tics
#define     BLOC_MAXSLOT 7        // Maximum # of Free Slot
#define     MAXI_REFRESH 15000    // 15sec timer refresh on scan for maxi buff slot
#define     BLOC_REFRESH 12000    // 12sec timer refresh on show bloc buff list
#define     SELF_REFRESH 02000    //  2sec timer refresh on show self buff list
#define     PETS_REFRESH 03000    //  3sec timer refresh on show pets buff list
#define     SCAN_REFRESH 07000    //  7sec timer refresh on scan for bloc buff

bool        InGame=false;         // InGame Flag?
long        Pulses=0;             // Tick Counter
long        Totale=0;             // Totale Blocked

bool        doBLOC=false;         // Bloc Active
bool        onBLOC=false;         // Bloc Popups
bool        pSONGS=false;         // Pets Songs
bool        sSONGS=false;         // Self Songs

long        FreeSong;             // Free # of Song Slots
long        FreeBuff;             // Free # of Buff Slots
long        FreePets;             // Free # of Buff Slots on Pet
long        MaxiSong=12;          // Maxi # of Song Slots
long        MaxiBuff=25;          // Maxi # of Buff Slots
long        MaxiPets=30;          // Maxi # of Buff Slots on Pet

long        NUON_MAXI=0;          // Next Update on Maxi Buff Slot
long        NUON_BLOC=0;          // Next Update on Bloc Buff Page Time Mark
long        NUON_SELF=0;          // Next Update on Self Buff Page Time Mark
long        NUON_PETS=0;          // Next Update on Pets Buff Page Time Mark
long        NUON_SCAN=0;          // Next Update on Scan Buff Bloc Time Mark

CHAR        INIGen[MAX_STRING];   // General Setting INI File
CHAR        INIWnd[MAX_STRING];   // Windows Setting INI File

class CBlocList *BlocBuffWnd=0;   // XML Bloc Buff List
class CSelfList *SelfBuffWnd=0;   // XML Self Buff List
class CPetsList *PetsBuffWnd=0;   // XML Pets Buff List

class MQ2BlockType *pBlockType=0; // MQ2 TLO

// ==================================================================

class bDATA {
public:
  CHAR  Name[MAX_STRING];            // Buff Name
  long  ID;                       // Buff ID
  long  Slot;                     // Buff Buff[SLOT] / Free[BLOC] / Tick[WARN]
  long  Type;                     // Buff Type (0=Detrimental) (1+ Beneficial)
  long  Last;                     // Buff Last Duration
};

list<bDATA> ListBloc;             // List Bloc Buff
list<bDATA> ListSelf;             // List Self Buff (Song+Buff)
list<bDATA> ListPets;             // List Pets Buff (Song+Buff)

void ListDelete(list<bDATA> *MyList, long ID) {
  list<bDATA>::iterator CurList=MyList->begin();
  list<bDATA>::iterator EndList=MyList->end();
  while(CurList!=EndList) {
    if(CurList->ID==ID) {
      MyList->erase(CurList);
      break;
    }
    CurList++;
  }
}

void ListInsert(list<bDATA> *MyList, long ID, long Slot, long Last) {
  if(PSPELL pSpell=GetSpellByID(ID)) {
    list<bDATA>::iterator CurList=MyList->begin();
    list<bDATA>::iterator EndList=MyList->end();
    bDATA rDATA;
    strcpy_s(rDATA.Name,pSpell->Name);
    rDATA.ID=pSpell->ID;
    rDATA.Type=pSpell->SpellType;
    rDATA.Slot=Slot;
    rDATA.Last=Last;
    while(CurList!=EndList && (Last>CurList->Last || (Last==CurList->Last && (strcmp(pSpell->Name,CurList->Name)>0)))) CurList++;
    MyList->insert(CurList,rDATA);
  }
}

void ListExport(list<bDATA> *MyList, PCHAR fNam, PCHAR fSec, PCHAR fKey) {
  list<bDATA>::iterator CurList=MyList->begin();
  list<bDATA>::iterator EndList=MyList->end();
  CHAR Keys[MAX_STRING];
  CHAR Temp[MAX_STRING];
  long Line=0;
  while(CurList!=EndList) {
    sprintf_s(Keys,"%s%d",fKey,++Line);
    sprintf_s(Temp,"%s|%d",CurList->Name,CurList->Slot);
    WritePrivateProfileString(fSec,Keys,Temp,fNam);
    CurList++;
  }
}

void ListImport(list<bDATA> *MyList, PCHAR fNam, PCHAR fSec, PCHAR fKey, long Mini, long Maxi) {
  long Counter,TempVal;
  char Buffers[MAX_STRING];
  CHAR Working[MAX_STRING];
  MyList->clear(); Counter=0;
  while(true) {
    sprintf_s(Buffers,"%s%d",fKey,++Counter);
    GetPrivateProfileString(fSec,Buffers,"EOF",Working,MAX_STRING,fNam);
    if(!_strnicmp(Working,"EOF",3)) break;
    GetArg(Buffers,Working,1,FALSE,FALSE,FALSE,'|');
    if(PSPELL pSpell=GetSpellByName(Buffers)) {
      GetArg(Buffers,Working,2,FALSE,FALSE,FALSE,'|');
      TempVal=atoi(Buffers);
      if(pSpell->SpellType && TempVal>=Mini && TempVal<=Maxi) {
        ListDelete(MyList,pSpell->ID);
        ListInsert(MyList,pSpell->ID,TempVal,0);
      }
    }
  }
}

PSTR ListFormat(PSTR Dest, PCHAR Form, PCHAR Name, long ID, long Slot, long Type, long Last, long Line) {
  string Temp=Form;
  CHAR   Data[MAX_STRING];
  long   Posi;
  if(0<=(Posi=Temp.find("<NAME>"))) {
    Temp.replace(Posi,6,Name);
  }
  if(0<=(Posi=Temp.find("<SLOT>"))) {
    sprintf_s(Data,"%d",Slot);
    Temp.replace(Posi,6,Data);
  }
  if(0<=(Posi=Temp.find("<LAST>"))) {
    sprintf_s(Data,"%01d:%02d:%02d",int(((Last)/3600)),int(((Last)%3600)/60),int(((Last)%60)));
    Temp.replace(Posi,6,Data);
  }
  if(0<=(Posi=Temp.find("<LINE>"))) {
    sprintf_s(Data,"%03d",Line);
    Temp.replace(Posi,6,Data);
  }
  strcpy_s(Dest, MAX_STRING, Temp.c_str());
  return Dest;
}

void ListOutput(list<bDATA> *MyList, PCHAR Format) {
  if(MyList->empty()) WriteChatf("List is Empty");
  else {
    list<bDATA>::iterator CurList=MyList->begin();
    list<bDATA>::iterator EndList=MyList->end();
    CHAR Temp[MAX_STRING];
    long Line=0;
    while(CurList!=EndList) {
      WriteChatf(ListFormat(Temp,Format,CurList->Name,CurList->ID,CurList->Slot,CurList->Type,CurList->Last,++Line));
      CurList++;
    }
  }
}

// ==================================================================

void BlocThis(long ID, long Free) {
  if(PSPELL pSpell=GetSpellByID(ID)) {
    ListDelete(&ListBloc,pSpell->ID);
    if(pSpell->SpellType && Free>=0 && Free<=BLOC_MAXSLOT)
      ListInsert(&ListBloc,pSpell->ID,Free,0);
    NUON_BLOC=0;
  }
}

void BuffAvail() {
  MaxiBuff=15;
  NUON_MAXI=(long)clock()+MAXI_REFRESH;
  if(GetCharInfo2()->Level>=72) MaxiBuff++;
  if(GetCharInfo2()->Level>=75) MaxiBuff++;
  if(GetAAIndexByName("Embrace of the Dark Reign")>0) MaxiBuff++;
  else if(GetAAIndexByName("Embrace of the Keepers")>0) MaxiBuff++;
  if(DWORD Index=GetAAIndexByName("Mystical Attuning"))
  if(PlayerHasAAAbility(Index))
  for(int j=0; j<NUM_ALT_ABILITIES && GetCharInfo2()->AAList[j].AAIndex!=0; j++)
  if(GetCharInfo2()->AAList[j].AAIndex==Index) {
    MaxiBuff+=GetCharInfo2()->AAList[j].PointsSpent/5;
    return;
  }
}

DWORD BuffColor(long Type, long Last) {
  if(!Type) return 0xFFFF0000;           //red
  else if(Last<61)  return 0xFFF0F0F0;   //white
  else if(Last<181) return 0xF080FFFF;   //teal
  else if(Last<601) return 0xFF00F0F0;   //blue
  return 0xFF00F000;                     //green
}

long BuffTimer(PCHAR WindowName, PCHAR ButtonName, long SlotID) {
  if(CXWnd *pWin=FindMQ2Window(WindowName)) {
    CHAR Temp[MAX_STRING]; sprintf_s(Temp,"%s%d",ButtonName,SlotID);
    if(CXWnd *pBut=((CSidlScreenWnd*)(pWin))->GetChildItem(Temp)) {
      GetCXStr(pBut->Tooltip,Temp,MAX_STRING);
      string Work=Temp;
      return atoi(Work.substr(Work.find('(')+1,Work.find(':')-Work.find('(')-1).c_str())*60+
             atoi(Work.substr(Work.find(':')+1,2).c_str());
    }
  }
  return 0;
}

void ClickOff(long ID, long SLOT) {
  if(PSPELL pSpell=GetSpellByID(ID)) {
    if(!SLOT || !pSpell->SpellType) return;
    CHAR zSlot[MAX_STRING];
    CHAR zName[MAX_STRING];
    if(onBLOC) {
      sprintf_s(zName,"Click Off: <%s>",pSpell->Name);
      DisplayOverlayText(zName,CONCOLOR_LIGHTBLUE,100,500,500,3000);
    }
    WriteChatf("MQ2BuffTimer::Clicking Off <%s>",pSpell->Name);
    sprintf_s(zSlot,"Buff%d",abs(SLOT)-1);
    bool KeyboardFlags[4];
    *(DWORD*)&KeyboardFlags=*(DWORD*)&((PCXWNDMGR)pWndMgr)->KeyboardFlags;
    *(DWORD*)&((PCXWNDMGR)pWndMgr)->KeyboardFlags=0;
    SendWndClick((SLOT<0)?"ShortDurationBuffWindow":"BuffWindow",zSlot,"leftmouseup");
    *(DWORD*)&((PCXWNDMGR)pWndMgr)->KeyboardFlags=*(DWORD*)&KeyboardFlags;
    NUON_SELF=0;
  }
}

void MakeBuff(list<bDATA> *MyList,bool Create, bool Insert) {
  if(Create) MyList->clear();
  FreeBuff=MaxiBuff; long ID;
  for(int b=0; b<MaxiBuff; b++) if((ID=GetCharInfo2()->Buff[b].SpellID)>0) {
    FreeBuff--;
    if(Insert) ListInsert(MyList,ID,b+1,GetCharInfo2()->Buff[b].Duration*6);
  }
}

void MakeSong(list<bDATA> *MyList,bool Create, bool Insert) {
  if(Create) MyList->clear();
  FreeSong=MaxiSong; long ID;
  for(int s=0; s<MaxiSong; s++) if((ID=GetCharInfo2()->ShortBuff[s].SpellID)>0) {
    FreeSong--;
    if(Insert) ListInsert(MyList,ID,-(s+1),GetCharInfo2()->ShortBuff[s].Duration*6);
  }
}

void MakePets(list<bDATA> *MyList,bool Create, bool Insert) {
  if(Create) MyList->clear();
  FreePets=MaxiPets; long ID;
  if((long)GetCharInfo()->pSpawn->PetID>0 && pPetInfoWnd) {
    for(int p=0; p<MaxiPets; p++) if((ID=((PEQPETINFOWINDOW)pPetInfoWnd)->Buff[p])>0) {
       FreePets--;
       if(PSPELL pSpell=GetSpellByID(ID))
        if(Insert || !pSpell->DurationWindow)
         ListInsert(MyList,ID,p+1,BuffTimer("PetInfoWindow","PetBuff",p));
    }
  }
}

long SlotFree(long ID) {
  list<bDATA>::iterator CurList=ListBloc.begin();
  list<bDATA>::iterator EndList=ListBloc.end();
  while(CurList!=EndList) {
    if(CurList->ID==ID) return CurList->Slot;
    CurList++;
  }
  return -1;
}

long SlotScan(long ID) {
  if(ID>0) {
    for(int b=0; b<MaxiBuff; b++) if(GetCharInfo2()->Buff[b].SpellID==ID) return b+1;
    for(int s=0; s<MaxiSong; s++) if(GetCharInfo2()->ShortBuff[s].SpellID==ID) return -(s+1);
  }
  return 0;
}

void ReadINI() {
  BuffAvail();
  doBLOC=GetPrivateProfileInt(GetCharInfo()->Name,"BlocActive",0,INIGen)?true:false;
  onBLOC=GetPrivateProfileInt(GetCharInfo()->Name,"BlocPopups",0,INIGen)?true:false;
  Totale=GetPrivateProfileInt(GetCharInfo()->Name,"BlocTotals",0,INIGen);
  sSONGS=GetPrivateProfileInt(GetCharInfo()->Name,"SongOnSelf",0,INIGen)?true:false;
  pSONGS=GetPrivateProfileInt(GetCharInfo()->Name,"SongOnPets",0,INIGen)?true:false;
  ListImport(&ListBloc,INIGen,GetCharInfo()->Name,"Buff",0,BLOC_MAXSLOT);
}
template <unsigned int _Size>LPSTR SafeItoa(int _Value, char(&_Buffer)[_Size], int _Radix)
{
	errno_t err = _itoa_s(_Value, _Buffer, _Radix);
	if (!err) {
		return _Buffer;
	}
	return "";
}
void SaveINI() {
  CHAR Buffers[MAX_STRING];
  WritePrivateProfileString(GetCharInfo()->Name,NULL,NULL,INIGen);
  WritePrivateProfileString(GetCharInfo()->Name,"BlocActive",doBLOC?"1":"0",INIGen);
  WritePrivateProfileString(GetCharInfo()->Name,"BlocPopups",onBLOC?"1":"0",INIGen);
  WritePrivateProfileString(GetCharInfo()->Name,"BlocTotals",SafeItoa(Totale,Buffers,10),INIGen);
  WritePrivateProfileString(GetCharInfo()->Name,"SongOnSelf",sSONGS?"1":"0",INIGen);
  WritePrivateProfileString(GetCharInfo()->Name,"SongOnPets",pSONGS?"1":"0",INIGen);
  ListExport(&ListBloc,INIGen,GetCharInfo()->Name,"Buff");
}

void WindowLock(CCustomWnd *MyWin, bool Lock, bool Show) {
  if(Lock) BitOff(MyWin->WindowStyle,CWS_CLOSE)
  else BitOn(MyWin->WindowStyle,CWS_CLOSE)
  if(MyWin->dShow) ((CXWnd*)MyWin)->Show(false,false);
  if(Show) ((CXWnd*)MyWin)->Show(true,true);
}

void WindowLoad(CCustomWnd *MyWin, PCHAR Title) {
	CHAR Section[MAX_STRING] = { 0 };
  ((CXWnd*)MyWin)->Show(false,false);
  BitOn(MyWin->WindowStyle,(CWS_MINIMIZE|CWS_RESIZEALL));
  GetCXStr(MyWin->INIStorageName,Section);
  SetCXStr(&MyWin->WindowText,Title);
  MyWin->Location.top    = GetPrivateProfileInt(Section,"Top",         MyWin->Location.top   ,INIWnd);
  MyWin->Location.bottom = GetPrivateProfileInt(Section,"Bottom",      MyWin->Location.bottom,INIWnd);
  MyWin->Location.left   = GetPrivateProfileInt(Section,"Left",        MyWin->Location.left  ,INIWnd);
  MyWin->Location.right  = GetPrivateProfileInt(Section,"Right",       MyWin->Location.right ,INIWnd);
  MyWin->Fades           = (GetPrivateProfileInt(Section,"Fades",       MyWin->Fades          ,INIWnd) ? true:false);
  MyWin->FadeDelay   = GetPrivateProfileInt(Section,"Delay",       MyWin->FadeDelay  ,INIWnd);
  MyWin->FadeDuration    = GetPrivateProfileInt(Section,"Duration",    MyWin->FadeDuration   ,INIWnd);
  MyWin->Alpha           = GetPrivateProfileInt(Section,"Alpha",       MyWin->Alpha          ,INIWnd);
  MyWin->FadeToAlpha     = GetPrivateProfileInt(Section,"FadeToAlpha", MyWin->FadeToAlpha    ,INIWnd);
  MyWin->BGType          = GetPrivateProfileInt(Section,"BGType",      MyWin->BGType         ,INIWnd);
  ARGBCOLOR col = { 0 };
  col.A					 = GetPrivateProfileInt(Section,"BGTint.alpha",  255,INIWnd);
  col.R       = GetPrivateProfileInt(Section,"BGTint.red",  0      ,INIWnd);
  col.G       = GetPrivateProfileInt(Section,"BGTint.green",0      ,INIWnd);
  col.B       = GetPrivateProfileInt(Section,"BGTint.blue", 0      ,INIWnd);
  MyWin->BGColor = col.ARGB;
  MyWin->Locked          = (GetPrivateProfileInt(Section,"Locked",      MyWin->Locked         ,INIWnd) ? true:false);
  WindowLock(MyWin,MyWin->Locked?true:false,GetPrivateProfileInt(Section,"Show",0,INIWnd)?true:false);
}

void WindowSave(CCustomWnd *MyWin) {
  if(!MyWin) return;
  RECT SaveLoc=MyWin->Minimized?MyWin->OldLocation:MyWin->Location;
  CHAR Buffers[MAX_STRING] = { 0 };
  CHAR Section[MAX_STRING] = { 0 };
  GetCXStr(MyWin->INIStorageName,Section);
  GetCXStr(MyWin->WindowText,Buffers);
  WritePrivateProfileString(Section,NULL,NULL,INIWnd);
  WritePrivateProfileString(Section,"WindowTitle",                               Buffers,INIWnd);
  WritePrivateProfileString(Section,"Top",         SafeItoa(SaveLoc.top,         Buffers,10),INIWnd);
  WritePrivateProfileString(Section,"Bottom",      SafeItoa(SaveLoc.bottom,      Buffers,10),INIWnd);
  WritePrivateProfileString(Section,"Left",        SafeItoa(SaveLoc.left,        Buffers,10),INIWnd);
  WritePrivateProfileString(Section,"Right",       SafeItoa(SaveLoc.right,       Buffers,10),INIWnd);
  WritePrivateProfileString(Section,"Locked",      SafeItoa(MyWin->Locked,       Buffers,10),INIWnd);
  WritePrivateProfileString(Section,"Fades",       SafeItoa(MyWin->Fades,        Buffers,10),INIWnd);
  WritePrivateProfileString(Section,"Delay",       SafeItoa(MyWin->FadeDelay,    Buffers,10),INIWnd);
  WritePrivateProfileString(Section,"Duration",    SafeItoa(MyWin->FadeDuration, Buffers,10),INIWnd);
  WritePrivateProfileString(Section,"Alpha",       SafeItoa(MyWin->Alpha,        Buffers,10),INIWnd);
  WritePrivateProfileString(Section,"FadeToAlpha", SafeItoa(MyWin->FadeToAlpha,  Buffers,10),INIWnd);
  WritePrivateProfileString(Section,"BGType",      SafeItoa(MyWin->BGType,       Buffers,10),INIWnd);
  ARGBCOLOR col = { 0 };
  col.ARGB = MyWin->BGColor;
  WritePrivateProfileString(Section,"BGTint.alpha",  SafeItoa(col.A,    Buffers,10),INIWnd);
  WritePrivateProfileString(Section,"BGTint.red",  SafeItoa(col.R,    Buffers,10),INIWnd);
  WritePrivateProfileString(Section,"BGTint.green",SafeItoa(col.G,    Buffers,10),INIWnd);
  WritePrivateProfileString(Section,"BGTint.blue", SafeItoa(col.B,    Buffers,10),INIWnd);
  WritePrivateProfileString(Section,"Show",        MyWin->dShow?"1":"0"                  ,INIWnd);
}

// ==================================================================

class CBlocList : public CCustomWnd {
public:
  CListWnd   *wBlocList, *wBlocActive, *wBlocPopups;
  CButtonWnd *wBlocLoad, *wBlocSave;

  void Updates() {
    if(this->dShow) {
      if(this->wBlocList) {
        this->wBlocList->DeleteAll();
        CHAR Buffers[MAX_STRING];
        list<bDATA>::iterator CurList=ListBloc.begin();
        list<bDATA>::iterator EndList=ListBloc.end();
        while(CurList!=EndList) {
          sprintf_s(Buffers,"  [%01d]  %s",CurList->Slot,CurList->Name);
          this->wBlocList->AddString(Buffers,(CurList->Slot)?0xFFFFFFFF:0xFF00FF00,0,0);
          CurList++;
        }
      }
      if(this->wBlocActive) {
        this->wBlocActive->DeleteAll();
        this->wBlocActive->AddString(doBLOC?"Enable":"Disable",doBLOC?0xFF00FF00:0xFFFFFFFF,0,0);
      }
      if(this->wBlocPopups) {
        this->wBlocPopups->DeleteAll();
        this->wBlocPopups->AddString(onBLOC?"  Popup":"NoPopup",onBLOC?0xFFFFFFFF:0xFF00FF00,0,0);
      }
    }
    NUON_BLOC=(long)clock()+BLOC_REFRESH;
  }

  int WndNotification(CXWnd *pWnd, unsigned int Message, void *unknown) {
    if(Message==XWM_CLOSE && this->Locked) this->Visible(true);
    else if (Message==20) WindowLock(this,this->Locked?false:true,this->dShow?true:false);
    else if(pWnd && (Message==XWM_LCLICK || Message==XWM_RCLICK)) {
      if(pWnd==(CXWnd*)wBlocLoad) ReadINI();
      else if(pWnd==(CXWnd*)wBlocSave) SaveINI();
      else if(pWnd==(CXWnd*)wBlocActive) doBLOC = !doBLOC;
      else if(pWnd==(CXWnd*)wBlocPopups) onBLOC = !onBLOC;
      else if(pWnd==(CXWnd*)wBlocList) {
        list<bDATA>::iterator CurPtr; CurPtr=ListBloc.begin();
        long CurSel=((CListWnd*)pWnd)->GetCurSel();
        while(CurSel>0) { CurPtr++; CurSel--; }
        if(CurPtr->Type) {
          if(Message==XWM_LCLICK) CurPtr->Slot--;
          if(Message==XWM_RCLICK) CurPtr->Slot++;
          if(CurPtr->Slot > BLOC_MAXSLOT) CurPtr->Slot = BLOC_MAXSLOT;
          if(CurPtr->Slot < 0) ListBloc.erase(CurPtr);
        }
      }
      this->Updates();
    }
    return CSidlScreenWnd::WndNotification(pWnd,Message,unknown);
  }

  void Visible(bool Wanted) {
    ((CXWnd*)this)->Show(Wanted,Wanted);
    if(dShow) this->Updates();
  }

  ~CBlocList() { WindowSave(this); }

  CBlocList():CCustomWnd("BuffBloc") {
    WindowLoad(this,"Bloc Buff");
    wBlocActive=(CListWnd*)GetChildItem("BT_BlocActive");
    wBlocPopups=(CListWnd*)GetChildItem("BT_BlocPopups");
    wBlocList=(CListWnd*)GetChildItem("BT_BlocList");
    wBlocLoad=(CButtonWnd*)GetChildItem("BT_BlocLoad");
    wBlocSave=(CButtonWnd*)GetChildItem("BT_BlocSave");
    SetWndNotification(CBlocList);
    this->Updates();
  }
};

// ==================================================================

class CSelfList : public CCustomWnd {
public:
  CListWnd    *wBuffFree, *wBuffShow, *wSongFree, *wBuffList;

  void Updates() {
    if(this->dShow) {
      CHAR Buffers[MAX_STRING];
      MakeBuff(&ListSelf,true,true);
      MakeSong(&ListSelf,false,sSONGS);
      if(this->wBuffList) {
        this->wBuffList->DeleteAll();
        list<bDATA>::iterator CurList=ListSelf.begin();
        list<bDATA>::iterator EndList=ListSelf.end();
        while(CurList!=EndList) {
          ListFormat(Buffers," <LAST>   <NAME>",CurList->Name,0,0,0,CurList->Last,0);
          this->wBuffList->AddString(Buffers,BuffColor(CurList->Type,CurList->Last),0,0);
          CurList++;
        }
      }
      if(this->wBuffShow) {
        this->wBuffShow->DeleteAll();
        this->wBuffShow->AddString(sSONGS?"+SONG":"-SONG",sSONGS?0xFF00FF00:0xFFFFFFFF,0,0);
      }
      if(this->wBuffFree) {
        sprintf_s(Buffers,"%02d/%02d",FreeBuff,MaxiBuff);
        this->wBuffFree->DeleteAll();
        this->wBuffFree->AddString(Buffers,0xFFFFFF00,0,0);
      }
      if(this->wSongFree) {
        sprintf_s(Buffers,"%02d/%02d",FreeSong,MaxiSong);
        this->wSongFree->DeleteAll();
        this->wSongFree->AddString(Buffers,0xFFFFFF00,0,0);
      }
    }
    NUON_SELF=(long)clock()+SELF_REFRESH;
  }

  int WndNotification(CXWnd *pWnd, unsigned int Message, void *unknown) {
    if(Message==XWM_CLOSE && this->Locked) this->Visible(true);
    else if (Message==20) WindowLock(this,this->Locked?false:true,this->dShow?true:false);
    else if(pWnd && (Message==XWM_LCLICK || Message==XWM_RCLICK)) {
      if(pWnd==(CXWnd*)wBuffShow) sSONGS = !sSONGS;
      else if(pWnd==(CXWnd*)wBuffList) {
        list<bDATA>::iterator CurPtr; CurPtr=ListSelf.begin();
        long CurSel=((CListWnd*)pWnd)->GetCurSel();
        while(CurSel>0) { CurPtr++; CurSel--; }
        if(CurPtr->Type) {
          ListDelete(&ListBloc,CurPtr->ID);
          if(Message==XWM_RCLICK) ListInsert(&ListBloc,CurPtr->ID,1,0);
          if(BlocBuffWnd) BlocBuffWnd->Updates();
        }
      }
      this->Updates();
    }
    return CSidlScreenWnd::WndNotification(pWnd,Message,unknown);
  }

  void Visible(bool Wanted) {
    ((CXWnd*)this)->Show(Wanted,Wanted);
    if(Wanted) this->Updates();
  }

  ~CSelfList() { WindowSave(this); }

  CSelfList():CCustomWnd("BuffSelf") {
    WindowLoad(this,"Self Buff");
    wBuffList=(CListWnd*)GetChildItem("BT_SelfList");
    wBuffFree=(CListWnd*)GetChildItem("BT_SelfBuff");
    wBuffShow=(CListWnd*)GetChildItem("BT_SelfShow");
    wSongFree=(CListWnd*)GetChildItem("BT_SelfSong");
    SetWndNotification(CSelfList);
    this->Updates();
  }
};

// ==================================================================

class CPetsList : public CCustomWnd {
public:
  CListWnd *wBuffFree, *wBuffShow, *wBuffList;

  void Updates() {
    if(this->dShow) {
      if(this->wBuffList) this->wBuffList->DeleteAll();
      if(this->wBuffFree) this->wBuffFree->DeleteAll();
      CHAR Buffers[MAX_STRING];
      MakePets(&ListPets,true,pSONGS);
      if((long)GetCharInfo()->pSpawn->PetID>0 && pPetInfoWnd) {
        if(this->wBuffList) {
          list<bDATA>::iterator CurList=ListPets.begin();
          list<bDATA>::iterator EndList=ListPets.end();
          while(CurList!=EndList) {
            ListFormat(Buffers," <LAST>   <NAME>",CurList->Name,0,0,0,CurList->Last,0);
            this->wBuffList->AddString(Buffers,BuffColor(CurList->Type,CurList->Last),0,0);
            CurList++;
          }
        }
        if(this->wBuffShow) {
           this->wBuffShow->DeleteAll();
           this->wBuffShow->AddString(pSONGS?"+SONG":"-SONG",pSONGS?0xFF00FF00:0xFFFFFFFF,0,0);
        }
        if(this->wBuffFree) {
          sprintf_s(Buffers,"%02d/%02d",FreePets,MaxiPets);
          this->wBuffFree->AddString(Buffers,0xFFFFFF00,0,0);
        }
      }
    }
    NUON_PETS=(long)clock()+PETS_REFRESH;
  }

  int WndNotification(CXWnd *pWnd, unsigned int Message, void *unknown) {
    if(Message==XWM_CLOSE && this->Locked) this->Visible(true);
    else if (Message==20) WindowLock(this,this->Locked?false:true,this->dShow?true:false);
    else if(pWnd && (Message==XWM_LCLICK || Message==XWM_RCLICK)) {
      if(pWnd==(CXWnd*)wBuffShow) pSONGS = !pSONGS;
      this->Updates();
    }
    return CSidlScreenWnd::WndNotification(pWnd,Message,unknown);
  }

  void Visible(bool Wanted) {
    ((CXWnd*)this)->Show(Wanted,Wanted);
    if(dShow) this->Updates();
  }

  ~CPetsList() { WindowSave(this); }

  CPetsList():CCustomWnd("BuffPets") {
    WindowLoad(this,"Pet Buff");
    wBuffList=(CListWnd*)GetChildItem("BT_PetsList");
    wBuffFree=(CListWnd*)GetChildItem("BT_PetsFree");
    wBuffShow=(CListWnd*)GetChildItem("BT_PetsShow");
    SetWndNotification(CPetsList);
    this->Updates();
  }
};

// ==================================================================

class MQ2BlockType : public MQ2Type {
public:
  enum BlockMembers {
    Ready=1,
    Quiet=2,
    Popup=3,
    Count=4,
    Total=5,
    Spell=6,
    Maxim=7,
    Avail=8,
  };

  MQ2BlockType():MQ2Type("Block") {
    TypeMember(Ready);
    TypeMember(Quiet);
    TypeMember(Popup);
    TypeMember(Count);
    TypeMember(Total);
    TypeMember(Spell);
    TypeMember(Maxim);
    TypeMember(Avail);
  }
  ~MQ2BlockType() { }

  bool GetMember(MQ2VARPTR VarPtr, PCHAR Member, PCHAR Index, MQ2TYPEVAR &Dest)  {
    PMQ2TYPEMEMBER pMember=MQ2BlockType::FindMember(Member);
    if (!pMember) return false;
    switch((BlockMembers)pMember->ID) {
      case Ready:
        Dest.Int=InGame && doBLOC;
        Dest.Type=pBoolType;
        return true;
      case Quiet:
        Dest.Int=1;
        Dest.Type=pBoolType;
        return true;
      case Popup:
        Dest.Int=onBLOC;
        Dest.Type=pBoolType;
        return true;
      case Count:
        Dest.Int=ListBloc.size();
        Dest.Type=pIntType;
        return true;
      case Total:
        Dest.Int=Totale;
        Dest.Type=pIntType;
        return true;
      case Spell:
        Dest.Type=pIntType;
        Dest.Int=-1;
        if(Index[0]) {
          list<bDATA>::iterator CurPtr; CurPtr=ListBloc.begin();
          list<bDATA>::iterator EndPtr; EndPtr=ListBloc.end();
          if(IsNumber(Index)) {
            long Numb=(atoi(Index)-1);
            if(Numb>=0 && Numb<(long)ListBloc.size()) {
              while(Numb>0) { CurPtr++; Numb--; }
              Dest.Int=CurPtr->Slot;
            }
          } else if(PSPELL pSpell=GetSpellByName(Index)) {
            while(CurPtr != EndPtr) {
              if(CurPtr->ID==pSpell->ID) {
                Dest.Int=CurPtr->Slot;
                return true;
              }
              CurPtr++;
            }
          }
        }
        return true;
      case Maxim:
        Dest.Type=pIntType;
        Dest.Int=0;
        if(Index[0])
        if(IsNumber(Index))
        if(long T=atoi(Index))
        if(T==1) Dest.Int=MaxiBuff;
        else if(T==2) Dest.Int=MaxiSong;
        else if(T==3) Dest.Int=MaxiPets;
        return true;
      case Avail:
        Dest.Type=pIntType;
        Dest.Int=0;
        if(Index[0])
        if(IsNumber(Index))
        if(long T=atoi(Index))
        if(T==1) Dest.Int=FreeBuff;
        else if(T==2) Dest.Int=FreeSong;
        else if(T==3) Dest.Int=FreePets;
        return true;
    }
    return false;
  }

  bool ToString(MQ2VARPTR VarPtr, PCHAR Destination) {
    strcpy_s(Destination, MAX_STRING, InGame?"TRUE":"FALSE");
    return true;
  }

  bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source) {
    return false;
  }

  bool FromString(MQ2VARPTR &VarPtr, PCHAR Source) {
    return false;
  }
};

BOOL dataBlock(PCHAR szName, MQ2TYPEVAR &Dest) {
   Dest.DWord=1;
   Dest.Type=pBlockType;
   return true;
}

// ==================================================================

void WindowCreate() {
  if(pSidlMgr->FindScreenPieceTemplate("BuffSelf") && !SelfBuffWnd) SelfBuffWnd= new CSelfList();
  if(pSidlMgr->FindScreenPieceTemplate("BuffPets") && !PetsBuffWnd) PetsBuffWnd= new CPetsList();
  if(pSidlMgr->FindScreenPieceTemplate("BuffBloc") && !BlocBuffWnd) BlocBuffWnd= new CBlocList();
}

void WindowRemove() {
  if(SelfBuffWnd) { delete SelfBuffWnd; SelfBuffWnd=0; }
  if(PetsBuffWnd) { delete PetsBuffWnd; PetsBuffWnd=0; }
  if(BlocBuffWnd) { delete BlocBuffWnd; BlocBuffWnd=0; }
}

void WindowPopups() {
  if(SelfBuffWnd) SelfBuffWnd->Visible(true);
  //if(PetsBuffWnd) PetsBuffWnd->Visible(true);
  //if(BlocBuffWnd) BlocBuffWnd->Visible(true);
}

// ==================================================================

void BatchFromINI(PCHAR Sec, char Mode) {
  CHAR zSec[MAX_STRING]; long s=0; long t=0;
  while(Sec[s]!=0) { if(Sec[s]!='[' && Sec[s]!=']') zSec[t++]=Sec[s]; s++; } zSec[t]=0;
  list<bDATA> ListTemp; ListTemp.clear();
  ListImport(&ListTemp, INIGen, zSec, "Buff", 0, BLOC_MAXSLOT);
  list<bDATA>::iterator CurList=ListTemp.begin();
  list<bDATA>::iterator EndList=ListTemp.end();
  while(CurList!=EndList) {
    if(Mode=='=') ClickOff(CurList->ID,SlotScan(CurList->ID));
    else {
      if(Mode=='-') CurList->Slot=-1;
      BlocThis(CurList->ID,CurList->Slot);
    }
    CurList++;
  }
}

void BlockCommand(PSPAWNINFO pCHAR, PCHAR zLine) {
  CHAR zParm[MAX_STRING]; GetArg(zParm,zLine,1); bool nHelp=false;
  if(zParm[0]==0 || !_strnicmp(zParm,"/help",5)) nHelp=true;
  else if(!_strnicmp(zParm,"/load",5)) ReadINI();
  else if(!_strnicmp(zParm,"/save",5)) SaveINI();
  else if(!_strnicmp(zParm,"/list",5)) ListOutput(&ListBloc,"[<LINE>]=[<SLOT>] (<NAME>)");
  else if(!_strnicmp(zParm,"/block",6)) doBLOC = !doBLOC;
  else if(!_strnicmp(zParm,"/popup",6)) onBLOC = !onBLOC;
  else if(!_strnicmp(zParm,"/quiet",6)) ;
  else if(!_strnicmp(zParm,"/window",7)) WindowPopups();
  else if(!strncmp(zParm,"[",1)) BatchFromINI(zParm,'+');
  else {
    long SL=atoi(zParm);
    long ID=0; if(PSPELL pSpell=GetSpellByName(zParm)) ID=pSpell->ID;
    if(SL && SL>=-MaxiSong && SL<=MaxiBuff)
      ID=(SL>0)?GetCharInfo2()->Buff[abs(SL)-1].SpellID:GetCharInfo2()->ShortBuff[abs(SL)-1].SpellID;
    if(ID>0) { GetArg(zParm,zLine,2); BlocThis(ID,atoi(zParm)); return; }
    nHelp=true;
  }
  if(nHelp) {
    WriteChatf("Usage: /Block ##/Name/[Set] <Mode>");
    WriteChatf("       /Block /help /list /load /save /block /popup /window");
    WriteChatf("       /Block [Set] (Entire [SET] from INI");
    WriteChatf("       /Block Name <Mode> (Block that Spell Name)");
    WriteChatf("       /Block ## <Mode> (on Slot ##, -## for Song)");
    WriteChatf("      <Mode> 0=always(default) 1+ free buff to keep");
  }
}

void ClickCommand(PSPAWNINFO pCHAR, PCHAR zLine) {
  CHAR zParm[MAX_STRING]; GetArg(zParm,zLine,1); bool nHelp=false;
  if(zParm[0]==0 || !_strnicmp(zParm,"/help",5)) nHelp=true;
  else if(!_strnicmp(zParm,"/load",5)) ReadINI();
  else if(!_strnicmp(zParm,"/save",5)) SaveINI();
  else if(!_strnicmp(zParm,"/list",5)) ListOutput(&ListBloc,"[<LINE>]=[<SLOT>] (<NAME>)");
  else if(!_strnicmp(zParm,"/block",6)) doBLOC = !doBLOC;
  else if(!_strnicmp(zParm,"/popup",6)) onBLOC = !onBLOC;
  else if(!_strnicmp(zParm,"/quiet",6)) ;
  else if(!_strnicmp(zParm,"/window",7)) WindowPopups();
  else if(!strncmp(zParm,"[",1)) BatchFromINI(zParm,'=');
  else {
    long SL=atoi(zParm);
    long ID=0; if(PSPELL pSpell=GetSpellByName(zParm)) ID=pSpell->ID;
    if(SL && SL>=-MaxiSong && SL<=MaxiBuff)
    ID=(SL>0)?GetCharInfo2()->Buff[abs(SL)-1].SpellID:GetCharInfo2()->ShortBuff[abs(SL)-1].SpellID;
    if(SL=SlotScan(ID)) { ClickOff(ID,SL); return; }
    nHelp=true;
  }
  if(nHelp) {
  WriteChatf("Usage: /ClickOff [Set] Name or #");
  WriteChatf("       /ClickOff /help /list /load /save /block /popup /window");
  WriteChatf("       /ClickOff [Set] (Entire [SET] from INI");
  WriteChatf("       /ClickOff ## (on Slot ##, -## for Song");
  WriteChatf("       /ClickOff Name (Spell Name)");
  }
}

void UnBlockCommand(PSPAWNINFO pCHAR, PCHAR zLine) {
  CHAR zParm[MAX_STRING]; GetArg(zParm,zLine,1); bool nHelp=false;
  if(zParm[0]==0 || !_strnicmp(zParm,"/help",5)) nHelp=true;
  else if(!_strnicmp(zParm,"/load",5)) ReadINI();
  else if(!_strnicmp(zParm,"/save",5)) SaveINI();
  else if(!_strnicmp(zParm,"/list",5)) ListOutput(&ListBloc,"[<LINE>]=[<SLOT>] (<NAME>)");
  else if(!_strnicmp(zParm,"/block",6)) doBLOC = !doBLOC;
  else if(!_strnicmp(zParm,"/popup",6)) onBLOC = !onBLOC;
  else if(!_strnicmp(zParm,"/quiet",6)) ;
  else if(!_strnicmp(zParm,"/window",7)) WindowPopups();
  else if(!strncmp(zParm,"[",1)) BatchFromINI(zParm,'-');
  else {
    long SL=atoi(zParm);
    long ID=0; if(PSPELL pSpell=GetSpellByName(zParm)) ID=pSpell->ID;
    if(SL>0 && SL<=(long)ListBloc.size()) {
      list<bDATA>::iterator CurPtr=ListBloc.begin(); SL--;
      while(SL>0) { CurPtr++; SL--; }
      ID=CurPtr->ID;
    }
    if(ID>0) { BlocThis(ID,-1); return; }
    nHelp=true;
  }
  if(nHelp) {
    WriteChatf("Usage: /UnBlock [Set]/##/Name");
    WriteChatf("       /UnBlock /help /list /load /save /block /popup /window");
    WriteChatf("       /UnBlock [Set] (Remove Block Entire [SET] from INI");
    WriteChatf("       /UnBlock Name (Remove Block for that Spell Name)");
    WriteChatf("       /UnBlock ## (Remove Block # from Block List)");
  }
}

// ==================================================================

PLUGIN_API VOID InitializePlugin(VOID) {
  DebugSpewAlways("Initializing MQ2BuffTool");
  AddXMLFile("MQUI_BuffTool.xml");
  AddCommand("/block",BlockCommand);
  AddCommand("/clickoff",ClickCommand);
  AddCommand("/unblock",UnBlockCommand);
  AddMQ2Data("Block",dataBlock);
  pBlockType = new MQ2BlockType;
}

PLUGIN_API VOID ShutdownPlugin(VOID) {
  DebugSpewAlways("Shutting down MQ2BuffTool");
  if(pSidlMgr->FindScreenPieceTemplate("BuffTool")) {
    WindowRemove();
    RemoveXMLFile("MQUI_BuffTool.xml");
  }
  RemoveCommand("/block");
  RemoveCommand("/clickoff");
  RemoveCommand("/unblock");
  RemoveMQ2Data("Block");
  delete pBlockType;
}

PLUGIN_API VOID OnCleanUI(VOID) {
  DebugSpewAlways("MQ2BuffTool::OnCleanUI()");
  WindowRemove();
}

PLUGIN_API VOID OnReloadUI(VOID) {
  DebugSpewAlways("MQ2BuffTool::OnReloadUI()");
  WindowCreate();
}

PLUGIN_API VOID OnPulse(VOID) {
  if(!InGame || ++Pulses<SKIP_SOMETIC) return; Pulses=0;
  long Current=(long)clock();
  if(!NUON_MAXI || Current>NUON_MAXI) BuffAvail();
  if(doBLOC && (!NUON_SCAN || Current>NUON_SCAN)) {
  long ID, BT;
  long BF[BLOC_MAXSLOT+1]; long UB=MaxiBuff;
  long SF[BLOC_MAXSLOT+1]; long US=MaxiSong;
    ZeroMemory(BF,sizeof(BF));
    ZeroMemory(SF,sizeof(SF));
    for(int b=0; b<MaxiBuff; b++) if((ID=GetCharInfo2()->Buff[b].SpellID)>0) {
      UB--;
      if((BT=SlotFree(ID))>-1) BF[BT]=ID;
    }
    ID=BF[0];
    for(int jb=BLOC_MAXSLOT;!ID && jb>0; jb--) if(BF[jb] && jb>UB) ID=BF[jb];
    if(!ID) {
      for(int s=0; s<MaxiSong; s++) if((ID=GetCharInfo2()->ShortBuff[s].SpellID)>0) {
        US--;
        if((BT=SlotFree(ID))>-1) SF[BT]=ID;
      }
      ID=SF[0];
      for(int js=BLOC_MAXSLOT;!ID && js>0; js--) if(SF[js] && js>US) ID=SF[js];
    }
    if(ID) ClickOff(ID,SlotScan(ID));
    NUON_SCAN=(long)clock()+SCAN_REFRESH;
  }
  if(BlocBuffWnd && (!NUON_BLOC || Current>NUON_BLOC)) BlocBuffWnd->Updates();
  if(SelfBuffWnd && (!NUON_SELF || Current>NUON_SELF)) SelfBuffWnd->Updates();
  if(PetsBuffWnd && (!NUON_PETS || Current>NUON_PETS)) PetsBuffWnd->Updates();
}

PLUGIN_API VOID SetGameState(DWORD GameState) {
  DebugSpewAlways("MQ2BuffTool::SetGameState(%d)",GameState);
  InGame=(GameState==GAMESTATE_INGAME && GetCharInfo());
  if(InGame) {
    sprintf_s(INIGen,"%s\\MQ2BuffTool.ini",gszINIPath);
    sprintf_s(INIWnd,"%s\\MQ2BuffTool_%s_%s.ini",gszINIPath,GetCharInfo()->Name,EQADDR_SERVERNAME);
    ReadINI();
    WindowCreate();
  } else WindowRemove();
}