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

#include "ArrayClass.h"
#include "SharedClasses.h"

namespace EQData
{

// ***************************************************************************
// Defines
// ***************************************************************************

#define AllClassesMASK		0xFFFF
#define WarriorMASK			0x0001
#define ClericMASK			0x0002
#define RogueMASK			0x0100
#define WizardMASK			0x0400

enum PlayerClass
{
    Unknown=                            0,
    Warrior=                            1,
    Cleric=                             2,
    Paladin=                            3,
    Ranger=                             4,
    Shadowknight=                       5,
    Druid=                              6,
    Monk=                               7,
    Bard=                               8,
    Rogue=                              9,
    Shaman=                            10,
    Necromancer=                       11,
    Wizard=                            12,
    Mage=                              13,
    Enchanter=                         14,
    Beastlord=                         15,
    Berserker=                         16,
	Mercenary=                         17,
    TotalClasses=                      17,
};
// class 20 through 35 are the NPC GMs (same class order as above)
// class 41   Merchant
// class 60   LDoN Recruiter
// class 61   LDoN Merchant
// class 63   Tribute Master
// class 67   DoN Merchant (Norrath's Keepers)
// class 68   DoN Merchant (Dark Reign)

typedef struct _ClassInfo
{
    bool CanCast;
    bool PureCaster;
    bool PetClass;
    bool DruidType;
    bool NecroType;
    bool ClericType;
    bool ShamanType;
	bool MercType;
    char RaidColorOrder;
    PCHAR Name;
    PCHAR ShortName;
	PCHAR UCShortName;
} CLASSINFO, *PCLASSINFO;

static _ClassInfo ClassInfo[]=
{
    {0,0,0,0,0,0,0,0,0,"","",""},                         //unk
    {0,0,0,0,0,0,0,0,13,"Warrior","war","WAR"},           //war
    {1,1,0,0,0,1,0,0,2,"Cleric","clr","CLR"},             //clr
    {1,0,0,0,0,1,0,0,8,"Paladin","pal","PAL"},            //pal
    {1,0,0,1,0,0,0,0,9,"Ranger","rng","RNG"},             //rng
    {1,0,0,0,1,0,0,0,11,"Shadowknight","shd","SHD"},      //shd
    {1,1,0,1,0,0,0,0,3,"Druid","dru","DRU"},              //dru
    {0,0,0,0,0,0,0,0,6,"Monk","mnk","MNK"},               //mnk
    {1,0,0,0,0,0,0,0,0,"Bard","brd","BRD"},               //brd
    {0,0,0,0,0,0,0,0,10,"Rogue","rog","ROG"},             //rog
    {1,1,1,0,0,0,1,0,12,"Shaman","shm","SHM"},            //shm
    {1,1,1,0,1,0,0,0,7,"Necromancer","nec","NEC"},        //nec
    {1,1,0,0,0,0,0,0,14,"Wizard","wiz","WIZ"},            //wiz
    {1,1,1,0,0,0,0,0,5,"Mage","mag","MAG"},               //mag
    {1,1,0,0,0,0,0,0,4,"Enchanter","enc","ENC"},          //enc
    {1,0,1,0,0,0,1,0,1,"Beastlord","bst","BST"},          //bst
    {0,0,0,0,0,0,0,0,15,"Berserker","ber","BER"},         //ber
	{1,1,0,0,0,0,0,1,16,"Mercenary","mer","MER"},         //mer
};

#define BI_TARGETABLE                   1
#define BI_TRIGGER                      2 
#define BI_TRAP                         4
#define BI_TIMER                        8

typedef struct _BodyInfo
{
    PCHAR Name;
    DWORD Flags;
} BODYINFO, *PBODYINFO;

#define SPAWN_PLAYER                    0
#define SPAWN_NPC                       1
#define SPAWN_CORPSE                    2
//#define SPAWN_ANY                       3
//#define SPAWN_PET                       4

#define SKILL_PICKLOCK					35

#define ITEM_NORMAL1                    0x0031
#define ITEM_NORMAL2                    0x0036
#define ITEM_NORMAL3                    0x315f
#define ITEM_NORMAL4                    0x3336
#define ITEM_NORMAL5                    0x0032
#define ITEM_NORMAL6                    0x0033
#define ITEM_NORMAL7                    0x0034
#define ITEM_NORMAL8                    0x0039
#define ITEM_CONTAINER                  0x7900
#define ITEM_CONTAINER_PLAIN            0x7953
#define ITEM_BOOK                       0x7379

#define ITEMITEMTYPE_FOOD               0x0e 
#define ITEMITEMTYPE_WATER              0x0f 
#define ITEMITEMTYPE_SCROLL				0x14
#define ITEMITEMTYPE_ALCOHOL            0x26 
#define ITEMITEMTYPE_POISON             0x2a 
#define ITEMITEMTYPE_AUGUMENT           0x36 

#define ITEMEFFECTTYPE_COMBAT           0x00 
#define ITEMEFFECTTYPE_INVENTORY1       0x01 // Bards need to stop sing to cast 
#define ITEMEFFECTTYPE_WORN             0x02 
#define ITEMEFFECTTYPE_INVENTORY2       0x03 
#define ITEMEFFECTTYPE_MUSTEQUIP        0x04 // Bards need to stop sing to cast 
#define ITEMEFFECTTYPE_INVENTORY3       0x05 

#define SKILLMINDAMAGEMOD_BASH          0x00 
#define SKILLMINDAMAGEMOD_BACKSTAB      0x01 
#define SKILLMINDAMAGEMOD_DRAGONPUNCH   0x02 
#define SKILLMINDAMAGEMOD_EAGLESTRIKE   0x03 
#define SKILLMINDAMAGEMOD_FLYINGKICK    0x04 
#define SKILLMINDAMAGEMOD_KICK          0x05 
#define SKILLMINDAMAGEMOD_ROUNDKICK     0x06 
#define SKILLMINDAMAGEMOD_TIGERCLAW     0x07 
#define SKILLMINDAMAGEMOD_FRENZY        0x08 

//Heroic Mods
#define HEROIC_MOD_SPELL_SHIELDING      0x4
#define HEROIC_MOD_COMBAT_EFFECTS       0x5
#define HEROIC_MOD_MELEE_SHIELDING      0x6
#define HEROIC_MOD_DAMAGE_SHIELDING     0x7
#define HEROIC_MOD_DOT_SHIELDING        0x8
#define HEROIC_MOD_DAMAGE_SHIELD_MITIG  0x9
#define HEROIC_MOD_AVOIDANCE            0xa
#define HEROIC_MOD_ACCURACY             0xb
#define HEROIC_MOD_STUN_RESIST          0xc
#define HEROIC_MOD_STRIKETHROUGH        0xd

#define COLOR_DEFAULT                   0x00
#define COLOR_DARKGREY                  0x01
#define COLOR_DARKGREEN                 0x02
#define COLOR_DARKBLUE                  0x03
#define COLOR_PURPLE                    0x05
#define COLOR_LIGHTGREY                 0x06

#define CONCOLOR_GREY                   0x06
#define CONCOLOR_GREEN                  0x02
#define CONCOLOR_LIGHTBLUE              0x12
#define CONCOLOR_BLUE                   0x04
#define CONCOLOR_BLACK                  0x14
#define CONCOLOR_WHITE                  0x0a
#define CONCOLOR_YELLOW                 0x0f
#define CONCOLOR_RED                    0x0d

#define USERCOLOR_SAY                   0xFF +   1 //  1  - Say
#define USERCOLOR_TELL                  0xFF +   2 //  2  - Tell
#define USERCOLOR_GROUP                 0xFF +   3 //  3  - Group
#define USERCOLOR_GUILD                 0xFF +   4 //  4  - Guild
#define USERCOLOR_OOC                   0xFF +   5 //  5  - OOC
#define USERCOLOR_AUCTION               0xFF +   6 //  6  - Auction
#define USERCOLOR_SHOUT                 0xFF +   7 //  7  - Shout
#define USERCOLOR_EMOTE                 0xFF +   8 //  8  - Emote
#define USERCOLOR_SPELLS                0xFF +   9 //  9  - Spells (meming, scribing, casting, etc.)
#define USERCOLOR_YOU_HIT_OTHER         0xFF +  10 //  10 - You hit other
#define USERCOLOR_OTHER_HIT_YOU         0xFF +  11 //  11 - Other hits you
#define USERCOLOR_YOU_MISS_OTHER        0xFF +  12 //  12 - You miss other
#define USERCOLOR_OTHER_MISS_YOU        0xFF +  13 //  13 - Other misses you
#define USERCOLOR_DUELS                 0xFF +  14 //  14 - Some broadcasts (duels)
#define USERCOLOR_SKILLS                0xFF +  15 //  15 - Skills (ups, non-combat use, etc.)
#define USERCOLOR_DISCIPLINES           0xFF +  16 //  16 - Disciplines or special abilities
#define USERCOLOR_UNUSED001             0xFF +  17 //  17 - Unused at this time
#define USERCOLOR_DEFAULT               0xFF +  18 //  18 - Default text and stuff you type
#define USERCOLOR_UNUSED002             0xFF +  19 //  19 - Unused at this time
#define USERCOLOR_MERCHANT_OFFER        0xFF +  20 //  20 - Merchant Offer Price
#define USERCOLOR_MERCHANT_EXCHANGE     0xFF +  21 //  21 - Merchant Buy/Sell
#define USERCOLOR_YOUR_DEATH            0xFF +  22 //  22 - Your death message
#define USERCOLOR_OTHER_DEATH           0xFF +  23 //  23 - Others death message
#define USERCOLOR_OTHER_HIT_OTHER       0xFF +  24 //  24 - Other damage other
#define USERCOLOR_OTHER_MISS_OTHER      0xFF +  25 //  25 - Other miss other
#define USERCOLOR_WHO                   0xFF +  26 //  26 - /who command
#define USERCOLOR_YELL                  0xFF +  27 //  27 - yell for help
#define USERCOLOR_NON_MELEE             0xFF +  28 //  28 - Hit for non-melee
#define USERCOLOR_SPELL_WORN_OFF        0xFF +  29 //  29 - Spell worn off
#define USERCOLOR_MONEY_SPLIT           0xFF +  30 //  30 - Money splits
#define USERCOLOR_LOOT                  0xFF +  31 //  31 - Loot message
#define USERCOLOR_RANDOM                0xFF +  32 //  32 - Dice Roll (/random)
#define USERCOLOR_OTHERS_SPELLS         0xFF +  33 //  33 - Others spells
#define USERCOLOR_SPELL_FAILURE         0xFF +  34 //  34 - Spell Failures (resists, fizzles, missing component, bad target, etc.)
#define USERCOLOR_CHAT_CHANNEL          0xFF +  35 //  35 - Chat Channel Messages
#define USERCOLOR_CHAT_1                0xFF +  36 //  36 - Chat Channel 1
#define USERCOLOR_CHAT_2                0xFF +  37 //  37 - Chat Channel 2
#define USERCOLOR_CHAT_3                0xFF +  38 //  38 - Chat Channel 3
#define USERCOLOR_CHAT_4                0xFF +  39 //  39 - Chat Channel 4
#define USERCOLOR_CHAT_5                0xFF +  40 //  40 - Chat Channel 5
#define USERCOLOR_CHAT_6                0xFF +  41 //  41 - Chat Channel 6
#define USERCOLOR_CHAT_7                0xFF +  42 //  42 - Chat Channel 7
#define USERCOLOR_CHAT_8                0xFF +  43 //  43 - Chat Channel 8
#define USERCOLOR_CHAT_9                0xFF +  44 //  44 - Chat Channel 9
#define USERCOLOR_CHAT_10               0xFF +  45 //  45 - Chat Channel 10
#define USERCOLOR_MELEE_CRIT            0xFF +  46 //  46 - Melee Crits
#define USERCOLOR_SPELL_CRIT            0xFF +  47 //  47 - Spell Crits
#define USERCOLOR_TOO_FAR_AWAY          0xFF +  48 //  48 - Too far away (melee)
#define USERCOLOR_NPC_RAMPAGE           0xFF +  49 //  49 - NPC Rampage
#define USERCOLOR_NPC_FLURRY            0xFF +  50 //  50 - NPC Furry
#define USERCOLOR_NPC_ENRAGE            0xFF +  51 //  51 - NPC Enrage
#define USERCOLOR_ECHO_SAY              0xFF +  52 //  52 - say echo
#define USERCOLOR_ECHO_TELL             0xFF +  53 //  53 - tell echo
#define USERCOLOR_ECHO_GROUP            0xFF +  54 //  54 - group echo
#define USERCOLOR_ECHO_GUILD            0xFF +  55 //  55 - guild echo
#define USERCOLOR_ECHO_OOC              0xFF +  56 //  56 - group echo
#define USERCOLOR_ECHO_AUCTION          0xFF +  57 //  57 - auction echo
#define USERCOLOR_ECHO_SHOUT            0xFF +  58 //  58 - shout echo
#define USERCOLOR_ECHO_EMOTE            0xFF +  59 //  59 - emote echo
#define USERCOLOR_ECHO_CHAT_1           0xFF +  60 //  60 - chat 1 echo
#define USERCOLOR_ECHO_CHAT_2           0xFF +  61 //  61 - chat 2 echo
#define USERCOLOR_ECHO_CHAT_3           0xFF +  62 //  62 - chat 3 echo
#define USERCOLOR_ECHO_CHAT_4           0xFF +  63 //  63 - chat 4 echo
#define USERCOLOR_ECHO_CHAT_5           0xFF +  64 //  64 - chat 5 echo
#define USERCOLOR_ECHO_CHAT_6           0xFF +  65 //  65 - chat 6 echo
#define USERCOLOR_ECHO_CHAT_7           0xFF +  66 //  66 - chat 7 echo
#define USERCOLOR_ECHO_CHAT_8           0xFF +  67 //  67 - chat 8 echo
#define USERCOLOR_ECHO_CHAT_9           0xFF +  68 //  68 - chat 9 echo
#define USERCOLOR_ECHO_CHAT_10          0xFF +  69 //  69 - chat 10 echo
#define USERCOLOR_RESERVED              0xFF +  70 //  70 - "unused at this time" 
#define USERCOLOR_LINK                  0xFF +  71 //  71 - item links 
#define USERCOLOR_RAID                  0xFF +  72 //  72 - raid 
#define USERCOLOR_PET                   0xFF +  73 //  73 - my pet 
#define USERCOLOR_DAMAGESHIELD          0xFF +  74 //  74 - damage shields 
#define USERCOLOR_LEADER                0xFF +  75 //  75 - LAA-related messages 
#define USERCOLOR_PETRAMPFLURRY         0xFF +  76 //  76 - pet rampage/flurry 
#define USERCOLOR_PETCRITS              0xFF +  77 //  77 - pet's critical hits 
#define USERCOLOR_FOCUS                 0xFF +  78 //  78 - focus item activation 
#define USERCOLOR_XP                    0xFF +  79 //  79 - xp gain/loss 
#define USERCOLOR_SYSTEM                0xFF +  80 //  80 - system broadcasts etc 
#define USERCOLOR_PET_SPELLS            0xFF +  81 //  81 - pet spells
#define USERCOLOR_PET_RESPONSES         0xFF +  82 //  82 - pet responses
#define USERCOLOR_ITEM_SPEECH           0xFF +  83 //  83 - item speech
#define USERCOLOR_STRIKETHROUGH         0xFF +  84 //  84 - strikethrough messages
#define USERCOLOR_STUN                  0XFF +  85 //  85 - stun messages
#define USERCOLOR_RESERVED2             0xFF +  86 //  86 - "unused at this time" (or unknown!?)
#define USERCOLOR_FELLOWSHIP            0xFF +  87 //  87 - fellowship messages
#define USERCOLOR_NPC_SPEECH            0xFF +  88 //  88 - npc dialogue
#define USERCOLOR_NPC_SPEECH_TO_YOU     0xFF +  89 //  89 - npc dialogue to you
#define USERCOLOR_GUILD_MSG             0xFF +  90 //  90 - guild messages

#define DEITY_Bertoxxulous              201
#define DEITY_BrellSerilis              202
#define DEITY_CazicThule                203
#define DEITY_ErollisiMarr              204
#define DEITY_Bristlebane               205
#define DEITY_Innoruuk                  206
#define DEITY_Karana                    207
#define DEITY_MithanielMarr             208
#define DEITY_Prexus                    209
#define DEITY_Quellious                 210
#define DEITY_RallosZek                 211
#define DEITY_RodcetNife                212
#define DEITY_SolusekRo                 213
#define DEITY_TheTribunal               214
#define DEITY_Tunare                    215
#define DEITY_Veeshan                   216

#define ITEMTYPE_NORMAL                 0
#define ITEMTYPE_PACK                   1
#define ITEMTYPE_BOOK                   2

#define COMP_NONE                       0
#define COMP_EQ                         1
#define COMP_NE                         2
#define COMP_GT                         3
#define COMP_LT                         4
#define COMP_GE                         5
#define COMP_LE                         6
#define COMP_CONT                       7
#define COMP_NOTCONT                    8
#define COMP_BITAND                     9
#define COMP_BITOR                      10

#define COMP_TYPE_STRING                0
#define COMP_TYPE_NUMERIC               1
#define COMP_TYPE_BIT                   2

#define CMD_MQ                          1
#define CMD_EQ                          2        

#define LASTFIND_NOTFOUND               96
#define LASTFIND_PRIMARY                97

#define EVENT_CHAT                      0
#define EVENT_TIMER                     1
#define EVENT_CUSTOM                    2
#define EVENT_EVAL                      3
#define EVENT_EXEC                      4
#define EVENT_PULSE                     5
#define EVENT_SHUTDOWN                  6
#define EVENT_BREAK                     7
#define NUM_EVENTS                      8

#define CHAT_SAY                        1
#define CHAT_TELL                       2
#define CHAT_OOC                        4
#define CHAT_SHOUT                      8
#define CHAT_AUC                        16
#define CHAT_GUILD                      32
#define CHAT_GROUP                      64
#define CHAT_CHAT                       128
#define CHATEVENT(x)                    (gEventChat & x)

#define FILTERSKILL_ALL                 0
#define FILTERSKILL_INCREASE            1
#define FILTERSKILL_NONE                2

#define FILTERMACRO_ALL                 0
#define FILTERMACRO_ENHANCED            1
#define FILTERMACRO_NONE                2
#define FILTERMACRO_MACROENDED			3


#define MAX_STRING                      2048
#define MAX_VARNAME                     64

// Defines for CXWnd WindowStyle
#define CWS_VSCROLL                     0x1
#define CWS_HSCROLL                     0x2
#define CWS_TITLE                       0x4
#define CWS_CLOSE                       0x8
#define CWS_TILEBOX                     0x10
#define CWS_MINIMIZE                    0x20
#define CWS_BORDER                      0x40
//#define CWS_UNKNOWN                     0x80
//#define CWS_UNKNOWN                     0x100
#define CWS_RESIZEALL                   0x200
#define CWS_TRANSPARENT                 0x400
//#define CWS_UNKNOWN                     0x800
#define CWS_NOMOVE                      0x8000
#define CWS_QMARK                       0x10000
#define CWS_MAXIMIZE                    0x40000
#define ToggleBit(field,bit)            field^=bit;
#define BitOn(field,bit)                field|=bit;
#define BitOff(field,bit)               field&=~bit;
#define BitSet(field,bit)					field|=(1<<bit);
#define BitClear(field,bit)				field&=~(1<<bit);
// End CXWnd WindowStyle Defines

#define ALTCURRENCY_DOUBLOONS           0xa
#define ALTCURRENCY_ORUX                0xb
#define ALTCURRENCY_PHOSPHENES          0xc
#define ALTCURRENCY_PHOSPHITES          0xd
#define ALTCURRENCY_FAYCITES            0xe
#define ALTCURRENCY_CHRONOBINES         0xf
#define ALTCURRENCY_SILVERTOKENS        0x10
#define ALTCURRENCY_GOLDTOKENS          0x11
#define ALTCURRENCY_MCKENZIE            0x12
#define ALTCURRENCY_BAYLE               0x13
#define ALTCURRENCY_RECLAMATION         0x14
#define ALTCURRENCY_BRELLIUM            0x15
#define ALTCURRENCY_MOTES               0x16
#define ALTCURRENCY_REBELLIONCHITS      0x17
#define ALTCURRENCY_DIAMONDCOINS        0x18
#define ALTCURRENCY_BRONZEFIATS         0x19
#define ALTCURRENCY_VOUCHER             0x1a
#define ALTCURRENCY_VELIUMSHARDS        0x1b
#define ALTCURRENCY_CRYSTALLIZEDFEAR    0x1c
#define ALTCURRENCY_SHADOWSTONES        0x1d
#define ALTCURRENCY_DREADSTONES         0x1e
#define ALTCURRENCY_MARKSOFVALOR        0x1f
#define ALTCURRENCY_MEDALSOFHEROISM     0x20
#define ALTCURRENCY_COMMEMORATIVE_COINS 0x21
#define ALTCURRENCY_FISTSOFBAYLE	    0x22
#define ALTCURRENCY_NOBLES			    0x23
#define ALTCURRENCY_ENERGYCRYSTALS	    0x24
#define ALTCURRENCY_PIECESOFEIGHT	    0x25

enum MOUSE_DATA_TYPES {
   MD_Unknown = -1,
   MD_Button0Click=0,
   MD_Button1Click,
   MD_Button0,
   MD_Button1
};

//eqmule mar 10 2013
//KeypressHandler__HandleKeyUp_x has this one
#define nEQMappableCommands             0x1F4

#define MAX_PC_LEVEL                    105
#define MAX_NPC_LEVEL                   200
#define MAX_SPELL_LEVEL					255
#define NUM_SPELL_GEMS                  0xc
#define NUM_SPELL_SETS                  30
#define NUM_BUFF_SLOTS					0x61
#define NUM_LONG_BUFFS                  0x2a
#define NUM_SHORT_BUFFS                 0x37

#define EQ_EXPANSION(x)                 (1 << (x - 1))
#define EXPANSION_RoK                   EQ_EXPANSION(1)
#define EXPANSION_SoV                   EQ_EXPANSION(2)
#define EXPANSION_SoL                   EQ_EXPANSION(3)
#define EXPANSION_PoP                   EQ_EXPANSION(4)
#define EXPANSION_LoY                   EQ_EXPANSION(5)
#define EXPANSION_LDoN                  EQ_EXPANSION(6)
#define EXPANSION_GoD                   EQ_EXPANSION(7)
#define EXPANSION_OoW                   EQ_EXPANSION(8)
#define EXPANSION_DoN                   EQ_EXPANSION(9)
#define EXPANSION_DoD                   EQ_EXPANSION(10)
#define EXPANSION_PoR                   EQ_EXPANSION(11)
#define EXPANSION_TSS                   EQ_EXPANSION(12)
#define EXPANSION_TBS                   EQ_EXPANSION(13)
#define EXPANSION_SoF                   EQ_EXPANSION(14)
#define EXPANSION_SoD                   EQ_EXPANSION(15)
#define EXPANSION_UFT                   EQ_EXPANSION(16)
#define EXPANSION_HoT                   EQ_EXPANSION(17)
#define EXPANSION_VoA                   EQ_EXPANSION(18)
#define EXPANSION_RoF                   EQ_EXPANSION(19)
#define EXPANSION_CotF                  EQ_EXPANSION(20)
#define EXPANSION_TDS                   EQ_EXPANSION(21)
#define EXPANSION_TBM                   EQ_EXPANSION(22)
#define EXPANSION_EoK                   EQ_EXPANSION(23)
#define NUM_EXPANSIONS                  23

#if _MSC_VER < 1600
#define nullptr                         NULL
#endif

#define CALC_1TICK						107
#define CALC_2TICK						108
#define CALC_5TICK						120
#define CALC_12TICK						122
#define CALC_RANDOM						123

#define SPA_AC                          1
#define SPA_MOVEMENTRATE                3
#define SPA_LURE                        10
#define SPA_HASTE                       11
#define SPA_ILLUSION                    58
#define SPA_MAGNIFICATION               87
#define SPA_PLAYERSIZE                  89
#define SPA_SUMMONCORPSE                91
#define SPA_BARDOVERHASTE               98
#define SPA_SPELLDAMAGE                 124
#define SPA_HEALING                     125
#define SPA_REAGENTCHANCE               131
#define SPA_SPELLMANACOST               132
#define SPA_DOUBLEATTACK                177
#define SPA_STUNRESIST                  195
#define SPA_PROCMOD                     200
#define SPA_DIVINEREZ                   232
#define SPA_METABOLISM                  233
#define SPA_PLACEHOLDER                 254
#define SPA_TRIPLEBACKSTAB              258
#define SPA_DOTCRIT                     273
#define SPA_HEALCRIT                    274
#define SPA_MENDCRIT                    275
#define SPA_FLURRY                      279
#define SPA_PETFLURRY                   280
#define SPA_SPELLCRITCHANCE             294
#define SPA_INCSPELLDMG                 296
#define SPA_DAMAGECRITMOD               302
#define SPA_SHIELDBLOCKCHANCE           320
#define SPA_AC2                         416
#define SPA_SPELLDAMAGETAKEN			483

#define TT_PBAE                         0x04
#define TT_TARGETED_AE                  0x08
#define TT_AE_PC_V2                     0x28
#define TT_DIRECTIONAL                  0x2a

#define EQHeading(heading) (int)(((heading + 16) % 256) / 32) * 2


// ***************************************************************************
// Structures
// ***************************************************************************

typedef struct _MOUSESPOOF {
   MOUSE_DATA_TYPES     mdType;
   DWORD                dwData;
   struct _MOUSESPOOF   *pNext;
} MOUSESPOOF, *PMOUSESPOOF;

typedef struct _MOUSECLICK {
   BYTE Confirm[8];         // DO NOT WRITE TO THIS BYTE
   BYTE Click[8];         // Left = 0, Right = 1, etc
} MOUSECLICK, *PMOUSECLICK;

typedef struct _UILOCATION {
   DWORD x;
   DWORD y;
   DWORD w;
   DWORD h;
   CHAR error[MAX_STRING];
} UILOCATION, *PUILOCATION;

//5-15-2003   eqmule
typedef struct _CXSTR {
/*0x00*/   DWORD   Font;            // maybe, dont know.  04 = Window 01 = button
/*0x04*/   DWORD   MaxLength;
/*0x08*/   DWORD   Length;
/*0x0c*/   BOOL    Encoding;        // 0: ASCII, 1:Unicode
/*0x10*/   PCRITICAL_SECTION pLock;
/*0x14*/   CHAR    Text[1];         // Stub, can be anywhere from Length to MaxLength (which is how much is malloc'd to this CXStr)
} CXSTR, *PCXSTR;

#define ITEM_NAME_LEN                   0x40
#define LORE_NAME_LEN                   0x50

// size is 0x64 02-16-2007
typedef struct _ITEMSPELLS { 
/*0x00*/ DWORD SpellID; 
/*0x04*/ BYTE  RequiredLevel; 
/*0x05*/ BYTE  EffectType; 
/*0x06*/ BYTE  Unknown[0x2]; 
/*0x08*/ DWORD Unknown0x08;
/*0x0c*/ DWORD MaxCharges;
/*0x10*/ DWORD CastTime;
/*0x14*/ DWORD TimerID;
/*0x18*/ DWORD RecastType;
/*0x1c*/ DWORD ProcRate;
/*0x20*/ CHAR  OtherName[0x40];//some kind of override
/*0x60*/ DWORD OtherID;
/*0x64*/
} ITEMSPELLS, *PITEMSPELLS; 

class ArmorProperties
{
public:
/*0x00*/ UINT Type;
/*0x04*/ UINT Variation;
/*0x08*/ UINT Material;
/*0x0c*/ UINT NewArmorID;
/*0x10*/ UINT NewArmorType;
/*0x14*/
};
struct AugSocket
{
	int Type;
	bool bVisible;
	bool bInfusible;
};
class ItemSocketData
{
public:
	AugSocket Sockets[6];
};
//CItemInfo__CItemInfo
// actual size: 0x634 in Mar 10 2013 LIVE (see 68FC67) - eqmule
typedef struct _ITEMINFO {
/*0x0000*/ CHAR         Name[ITEM_NAME_LEN];
/*0x0040*/ CHAR         LoreName[LORE_NAME_LEN];
/*0x0090*/ CHAR         AdvancedLoreName[0x20];
/*0x00b0*/ CHAR         IDFile[0x20];
/*0x00d0*/ BYTE         Unknown0x00d0[0x1c];
/*0x00ec*/ DWORD        ItemNumber;
/*0x00f0*/ DWORD        EquipSlots;
/*0x00f4*/ DWORD        Cost;
/*0x00f8*/ DWORD        IconNumber;
/*0x00fc*/ BYTE         eGMRequirement;//todo figure out this enum
/*0x00fd*/ bool         bPoofOnDeath;
/*0x0100*/ int         Unknown0x0100;
/*0x0104*/ int         Unknown0x0104;
/*0x0108*/ DWORD        Weight;
/*0x010c*/ bool         NoRent; // 0=temp, 1=default
/*0x010d*/ bool         NoDrop; // 0=no drop, 1=can drop
/*0x010e*/ bool         Attuneable;
/*0x010f*/ bool         Heirloom;
/*0x0110*/ bool         Collectible;
/*0x0111*/ bool         NoDestroy;
/*0x0112*/ bool         MarketPlace;
/*0x0113*/ bool         NoZone;
/*0x0114*/ DWORD        MakerID;//0-?? I did up to 12, I think it asks server for the name - eqmule
/*0x0118*/ bool         NoGround;
/*0x0119*/ BYTE         Size;
/*0x011a*/ BYTE         Type;
/*0x011b*/ bool         TradeSkills;
/*0x011c*/ int          Lore;
/*0x0120*/ bool         Artifact;
/*0x0121*/ bool         Summoned;
/*0x0122*/ CHAR         SvCold;
/*0x0123*/ CHAR         SvFire;
/*0x0124*/ CHAR         SvMagic;
/*0x0125*/ CHAR         SvDisease;
/*0x0126*/ CHAR         SvPoison;
/*0x0127*/ CHAR         SvCorruption;
/*0x0128*/ CHAR         STR;
/*0x0129*/ CHAR         STA;
/*0x012a*/ CHAR         AGI;
/*0x012b*/ CHAR         DEX;
/*0x012c*/ CHAR         CHA;
/*0x012d*/ CHAR         INT;
/*0x012e*/ CHAR         WIS;
/*0x012f*/ BYTE         Unknown0x012f;//this is just a filler
/*0x0130*/ int			HP;
/*0x0134*/ int	        Mana;
/*0x0138*/ int	        AC;
/*0x013c*/ int	        RequiredLevel;
/*0x0140*/ int	        RecommendedLevel;
/*0x0144*/ int	        RecommendedSkill;
/*0x0148*/ int	        SkillModType;
/*0x014c*/ int	        SkillModValue;
/*0x0150*/ int	        SkillModMax;
/*0x0154*/ int	        SkillModBonus;
/*0x0158*/ int	        BaneDMGRace;
/*0x015c*/ int	        BaneDMGBodyType;
/*0x0160*/ int	        BaneDMGBodyTypeValue;
/*0x0164*/ int	        BaneDMGRaceValue;
/*0x0168*/ int	        InstrumentType;
/*0x016c*/ int	        InstrumentMod;
/*0x0170*/ int	        Classes;
/*0x0174*/ int	        Races;
/*0x0178*/ int	        Diety;
/*0x017c*/ UINT         MaterialTintIndex;
/*0x0180*/ bool         Magic;
/*0x0181*/ BYTE         Light;
/*0x0182*/ BYTE         Delay;
/*0x0183*/ BYTE         ElementalFlag;//used to be called DmgBonusType;
/*0x0184*/ BYTE         ElementalDamage;//used to be called DmgBonusVal
/*0x0185*/ BYTE         Range;
/*0x0188*/ DWORD        Damage;
/*0x018c*/ DWORD        BackstabDamage;
/*0x0190*/ DWORD        DamageShieldMitigation;
/*0x0194*/ DWORD        HeroicSTR;
/*0x0198*/ DWORD        HeroicINT;
/*0x019c*/ DWORD        HeroicWIS;
/*0x01a0*/ DWORD        HeroicAGI;
/*0x01a4*/ DWORD        HeroicDEX;
/*0x01a8*/ DWORD        HeroicSTA;
/*0x01ac*/ DWORD        HeroicCHA;
/*0x01b0*/ DWORD        HeroicSvMagic;
/*0x01b4*/ DWORD        HeroicSvFire;
/*0x01b8*/ DWORD        HeroicSvCold;
/*0x01bc*/ DWORD        HeroicSvDisease;
/*0x01c0*/ DWORD        HeroicSvPoison;
/*0x01c4*/ DWORD        HeroicSvCorruption;
/*0x01c8*/ DWORD        HealAmount;
/*0x01cc*/ DWORD        SpellDamage;
/*0x01d0*/ int	        Prestige;
/*0x01d4*/ BYTE         ItemType;
/*0x01d8*/ ArmorProperties ArmorProps;//size is 0x14
/*0x01ec*/ ItemSocketData AugData;
/*0x021c*/ DWORD        AugType;
/*0x0220*/ DWORD        AugSkinTypeMask;
/*0x0224*/ DWORD        AugRestrictions;
/*0x0228*/ DWORD        SolventItemID; //ID# of Solvent (Augs only)
/*0x022c*/ DWORD        LDTheme;
/*0x0230*/ DWORD        LDCost;
/*0x0234*/ DWORD        LDType;
/*0x0238*/ BYTE         Unknown0x0238[0x8];
/*0x0240*/ DWORD        FactionModType[0x4];
/*0x0250*/ DWORD        FactionModValue[0x4];
/*0x0260*/ CHAR         CharmFile[0x20];
/*0x0280*/ BYTE         Unknown0x0280[0x4];
/*0x0284*/ struct _ITEMSPELLS   Clicky;
/*0x02e8*/ struct _ITEMSPELLS   Proc;
/*0x034c*/ struct _ITEMSPELLS   Worn;
/*0x03b0*/ struct _ITEMSPELLS   Focus;
/*0x0414*/ struct _ITEMSPELLS   Scroll;
/*0x0478*/ struct _ITEMSPELLS   Focus2;
/*0x04dc*/ DWORD		SkillMask1;//this is just an array but I dont have time to figure it out for now. -eqmule
/*0x04e0*/ DWORD		SkillMask2;
/*0x04e4*/ DWORD		SkillMask3;
/*0x04e8*/ DWORD		SkillMask4;
/*0x04ec*/ DWORD		SkillMask5;
/*0x04f0*/ DWORD        CombatEffects;
/*0x04f4*/ DWORD        Shielding;
/*0x04f8*/ DWORD        StunResist;
/*0x04fc*/ DWORD        DoTShielding;
/*0x0500*/ DWORD        StrikeThrough;
/*0x0504*/ DWORD        DmgBonusSkill; // SkillMinDamageMod;
/*0x0508*/ DWORD        DmgBonusValue; // MinDamageMod;
/*0x050c*/ DWORD        SpellShield;
/*0x0510*/ DWORD        Avoidance;
/*0x0514*/ DWORD        Accuracy;
/*0x0518*/ DWORD        CharmFileID;
/*0x051c*/ DWORD        FoodDuration;//0-5 snack 6-20 meal 21-30 hearty 31-40 banquet 41-50 feast 51-60 enduring 60- miraculous
/*0x0520*/ BYTE         Combine;
/*0x0521*/ BYTE         Slots;
/*0x0522*/ BYTE         SizeCapacity;
/*0x0523*/ BYTE         WeightReduction;
/*0x0524*/ BYTE         BookType;      // 0=note, !0=book
/*0x0525*/ BYTE         BookLang;
/*0x0526*/ CHAR         BookFile[0x1e];
/*0x0544*/ DWORD        Favor;         // Tribute Value
/*0x0548*/ DWORD        GuildFavor;
/*0x054c*/ bool         bIsFVNoDrop;
/*0x0550*/ DWORD        Endurance;
/*0x0554*/ DWORD        Attack;
/*0x0558*/ DWORD        HPRegen;
/*0x055c*/ DWORD        ManaRegen;
/*0x0560*/ DWORD        EnduranceRegen;
/*0x0564*/ DWORD        Haste;
/*0x0568*/ DWORD        DamShield;
/*0x056c*/ int          AnimationOverride;
/*0x0570*/ int          PaletteTintIndex;
/*0x0574*/ bool         bNoPetGive;
/*0x0575*/ bool			bSomeProfile;
/*0x0576*/ bool			bPotionBeltAllowed;
/*0x0578*/ int			NumPotionSlots;
/*0x057c*/ int			SomeIDFlag;
/*0x0580*/ DWORD        StackSize;
/*0x0584*/ bool         bNoStorage;
/*0x0588*/ DWORD        MaxPower;
/*0x058c*/ DWORD        Purity;
/*0x0590*/ bool         bIsEpic;
/*0x0594*/ int          RightClickScriptID;
/*0x0598*/ int          ItemLaunchScriptID;
/*0x059c*/ BYTE         QuestItem;
/*0x059d*/ BYTE         Expendable;
/*0x05a0*/ DWORD        Clairvoyance;
/*0x05a4*/ int          SubClass;
/*0x05a8*/ bool         bLoginRegReqItem;
/*0x05ac*/ DWORD        Placeable;
/*0x05b0*/ bool         bPlaceableIgnoreCollisions;
/*0x05b4*/ int          PlacementType;//todo: this is an enum need to figure out.
/*0x05b8*/ int          RealEstateDefID;
/*0x05bc*/ FLOAT        PlaceableScaleRangeMin;
/*0x05c0*/ FLOAT        PlaceableScaleRangeMax;
/*0x05c4*/ int          RealEstateUpkeepID;
/*0x05c8*/ int          MaxPerRealEstate;
/*0x05cc*/ CHAR			HousepetFileName[0x20];
/*0x05ec*/ int			TrophyBenefitID;
/*0x05f0*/ bool			bDisablePlacementRotation;
/*0x05f1*/ bool			bDisableFreePlacement;
/*0x05f4*/ int			NpcRespawnInterval;
/*0x05f8*/ FLOAT		PlaceableDefScale;
/*0x05fc*/ FLOAT		PlaceableDefHeading;
/*0x0600*/ FLOAT		PlaceableDefPitch;
/*0x0604*/ FLOAT		PlaceableDefRoll;
/*0x0608*/ bool			bInteractiveObject;
/*0x0609*/ BYTE			SocketSubClassCount;
/*0x060c*/ int			SocketSubClass[0xa];
/*0x0634*/
} ITEMINFO, *PITEMINFO;

//ok this is rediculous, so I had to change this, there are 40 slot bags now and they could add more in the future
//this is now dynamic... -eqmule
typedef struct _ITEMBASEARRAY {
    struct  _CONTENTS *Item[1];
} ITEMBASEARRAY, *PITEMBASEARRAY;

class EqItemGuid
{
public:
	enum { GUID = 18 };
	EqItemGuid()
	{
		ZeroMemory(guid, sizeof(guid));
	}

	char guid[GUID];
};
enum ItemContainerInstance
{
	eItemContainerInvalid = -1,
	eItemContainerPossessions,
	eItemContainerBank,
	eItemContainerSharedBank,
	eItemContainerTrade,
	eItemContainerWorld,
	eItemContainerLimbo,
	eItemContainerTribute,
	eItemContainerTrophyTribute,
	eItemContainerGuildTribute,
	eItemContainerMerchant,
	eItemContainerDeleted,
	eItemContainerCorpse,
	eItemContainerBazaar,
	eItemContainerInspect,
	eItemContainerRealEstate,
	eItemContainerViewModPC,
	eItemContainerViewModBank,
	eItemContainerViewModSharedBank,
	eItemContainerViewModLimbo,
	eItemContainerAltStorage,
	eItemContainerArchived,
	eItemContainerMail,
	eItemContainerGuildTrophyTribute,
	eItemContainerKrono,
	eItemContainerOther,
	eItemContainerMercenaryItems,
    eItemContainerViewModMercenaryItems,
    eItemContainerMountKeyRingItems,
	eItemContainerViewModMountKeyRingItems,
	eItemContainerIllusionKeyRingItems,
	eItemContainerViewModIllusionKeyRingItems,
	eItemContainerFamiliarKeyRingItems,
	eItemContainerViewModFamiliarKeyRingItems,
	eItemContainerCursor,
};
class ItemIndex
{
public:
/*0x00*/ short Slot1;
/*0x02*/ short Slot2;
/*0x04*/ short Slot3;
/*0x06*/
};

class ItemGlobalIndex2
{
public:
/*0x00*/ ItemGlobalIndex2::ItemContainerInstance Location;
/*0x04*/ ItemGlobalIndex2::ItemIndex Index;
/*0x0a*/
};

class ItemArray
{
public:
/*0x08*/	struct _ITEMBASEARRAY	*pItems;
/*0x0c*/	UINT Size;
/*0x10*/	UINT Capacity;
};

class ItemBaseContainer2
{
public:
/*0x00*/	UINT ContentSize;
/*0x04*/	int ItemLocation;
/*0x08*/	ItemArray ContainedItems;
/*0x14*/	BYTE Depth;
/*0x16*/	short ItemSlot;
/*0x18*/	short ItemSlot2;
/*0x1a*/	bool bDynamic;
/*0x1c*/
};

//Actual Size: 150 (see 555af7 in eqgame.exe Live dated May 10 2013) - eqmule
typedef struct _CONTENTS {
/*0x0000*/ void*        vtable;
/*0x0004*/ DWORD        ItemType;           // ? 1 = normal, 2 = pack ?
/*0x0008*/ void*        punknown;
//start of ItemBase
/*0x000C*/ int			AugFlag; \
/*0x0010*/ struct _CXSTR *SaveString; \
/*0x0014*/ int			Open; \
/*0x0018*/ UINT			ItemHash; \
/*0x001C*/ bool			bCopied; \
/*0x0020*/ DOUBLE		EvolvingExpPct; \
/*0x0028*/ int			Price; \
/*0x002C*/ int			EvolvingMaxLevel; \
/*0x0030*/ bool			bItemNeedsUpdate; \
/*0x0034*/ UINT			LastCastTime; \
/*0x0038*/ ItemGlobalIndex2 GlobalIndex; /* Size is 0xa (0xc)*/ \
/*0x0044*/ int			Charges; \
/*0x0048*/ UINT			NewArmorID; \
/*0x004C*/ CHAR			ActorTag1[0x1e]; \
/*0x006C*/ int			NoteStatus; \
/*0x0070*/ int			ScriptIndex; \
/*0x0074*/ bool			bRealEstateItemPlaceable; \
/*0x0078*/ UINT			RespawnTime; \
/*0x007C*/ int			RealEstateID; \
/*0x0080*/ int			NoDropFlag; \
/*0x0084*/ int			OrnamentationIcon; \
/*0x0088*/ ArrayClass_RO<UINT> RealEstateArray; \
/*0x0098*/ int			StackCount; \
/*0x009C*/ struct _ITEMINFO *Item1; \
/*0x00A0*/ ItemBaseContainer2 Contents; /* Size is 0x1c */ \
/*0x00BC*/ UINT Uknown0x00bc;//ok this is so stupid, this might be another ItemBaseContainer2 but I dont know
/*0x00c0*/ UINT Uknown0x00c0;//and it makes no sense either
/*0x00c4*/ UINT Uknown0x00c4;//perhaps it was a bug in the rof2 client... *shrug*
/*0x00c8*/ UINT Uknown0x00c8;
/*0x00cc*/ UINT Uknown0x00cc;
/*0x00d0*/ UINT Uknown0x00d0;
/*0x00d4*/ UINT Uknown0x00d4;
/*0x00D8*/ EqItemGuid	ItemGUID; \
/*0x00ec*/ DWORD        GroupID; \
/*0x00f0*/ int			ArmorType; \
/*0x00f4*/ LONG			LastEquipped; \
/*0x00f8*/ __int64      MerchantSlot;       /* slot id on a merchant */ \
/*0x0100*/ bool			bDisableAugTexture; \
/*0x0101*/ bool			bRankDisabled; \
/*0x0104*/ DWORD        MerchantQuantity; \
/*0x0108*/ DWORD        EvolvingCurrentLevel; \
/*0x010c*/ BYTE         IsEvolvingItem; \
/*0x0110*/ DWORD        Power; \
/*0x0114*/ CHAR         ActorTag2[0x1e]; \
/*0x0132*/ BYTE         EvolvingExpOn; \
/*0x0134*/ int			ItemColor; \
/*0x0138*/ UINT			Tint; \
/*0x013c*/ int			ID;
//end of ItemBase
//start of ItemClient
/*0x0140*/ int		Filler;
/*0x0144*/ struct _ITEMINFO*    Item2;
/*0x0148*/ struct _CXSTR *ClientString;
/*0x014c*/ UINT		DontKnow2;
/*0x0150*/
__declspec(dllexport)  struct _CONTENTS *GetContent(UINT index);
} CONTENTS, *PCONTENTS;

// Size 0x58 20110810 - dkaa
// Size 0x58 20150326 - demonstar55
typedef struct _SPELLBUFF {
	/*0x00*/    BYTE      Unknown0x0;
	/*0x01*/    BYTE      Level;
	/*0x02*/    BYTE      Unknown0x2;    // pretty sure is padding
	/*0x03*/    CHAR      DamageShield;  // also probably padding now
	/*0x04*/    FLOAT     Modifier;      // Bard song modifier, 1.0 is default
	/*0x08*/    LONG      SpellID;       // -1 or 0 for no spell..
	/*0x0c*/    DWORD     Duration;
	/*0x10*/    DWORD     Unknown0x10;   // This might be source ID?
	/*0x14*/    DWORD     HitCount;
	/*0x18*/    FLOAT     Y;             // Referenced by SPA 441 (distance removal)
	/*0x1c*/    FLOAT     X;
	/*0x20*/    FLOAT     Z;
	/*0x24*/    DWORD     Unknown0x24;
	/*0x28*/    DWORD     SlotData[0xC]; // used for book keeping of various effects (debuff counter, rune/vie damage remaining)
	/*0x58*/
} SPELLBUFF, *PSPELLBUFF;

// 20101012 - ieatacid
typedef struct _INVENTORY { 
/*0x00*/  struct    _CONTENTS* Charm;
/*0x04*/  struct    _CONTENTS* LeftEar;
/*0x08*/  struct    _CONTENTS* Head;
/*0x0c*/  struct    _CONTENTS* Face;
/*0x10*/  struct    _CONTENTS* RightEar;
/*0x14*/  struct    _CONTENTS* Neck;
/*0x18*/  struct    _CONTENTS* Shoulders;
/*0x1c*/  struct    _CONTENTS* Arms;
/*0x20*/  struct    _CONTENTS* Back;
/*0x24*/  struct    _CONTENTS* LeftWrist;
/*0x28*/  struct    _CONTENTS* RightWrist;
/*0x2c*/  struct    _CONTENTS* Range;
/*0x30*/  struct    _CONTENTS* Hands;
/*0x34*/  struct    _CONTENTS* Primary;
/*0x38*/  struct    _CONTENTS* Secondary;
/*0x3c*/  struct    _CONTENTS* LeftFinger;
/*0x40*/  struct    _CONTENTS* RightFinger;
/*0x44*/  struct    _CONTENTS* Chest;
/*0x48*/  struct    _CONTENTS* Legs;
/*0x4c*/  struct    _CONTENTS* Feet;
/*0x50*/  struct    _CONTENTS* Waist;
/*0x54*/  struct    _CONTENTS* PowerSource;
/*0x58*/  struct    _CONTENTS* Ammo;
/*0x5c*/  struct    _CONTENTS* Pack[0xa];
/*0x84*/  struct    _CONTENTS* Cursor;
} INVENTORY, *PINVENTORY;

//need to find this one
#define NUM_ALT_ABILITIES_ARRAY         0x1F7      
// see 4F77C6 in eqgame.exe dated May 10 2013 - eqmule
#define NUM_ALT_ABILITIES               0xC34F

//these two will merge when i get a chance - ieatacid wrote this?
#define AA_CHAR_MAX                     0xF5
//EQ_PC__GetAltAbilityIndex_x
//size is at 7EE7F8 in eqgame dated jun 13 2014 - eqmule
#define AA_CHAR_MAX_REAL                0x12C

typedef struct _AALIST { 
/*0x0*/   DWORD        AAIndex;
/*0x4*/   DWORD        PointsSpent;
/*0x8*/   DWORD        ChargeSpent;        // charges spent in the last 10 min?
} AALIST, *PAALIST;

#define NUM_INV_SLOTS                   0x1f
#define NUM_BANK_SLOTS                  0x18
#define NUM_SHAREDBANK_SLOTS            0x02
#define MAX_KEYRINGITEMS				0x1B //not really sure need to confirm this
//found in CSpellBookWnd__GetBookSlot_x (see 7756CA in Oct 29 2014 Live) -eqmule 
#define NUM_BOOK_SLOTS                  0x2d0
#define NUM_COMBAT_ABILITIES            0x12c
#define BAG_SLOT_START                  23
#define ExactLocation                   0
#define NUM_SKILLS						0x64
#define CONCURRENT_SKILLS				2

typedef struct _LEADERABILITIES {
/*0x00*/ DWORD MarkNPC;
/*0x04*/ DWORD NPCHealth;
/*0x08*/ DWORD Unknown0x8;
/*0x0c*/ DWORD DelegateMA;
/*0x10*/ DWORD DelegateMarkNPC;
/*0x14*/ DWORD Unknown0x14;
/*0x18*/ DWORD InspectBuffs;
/*0x1c*/ DWORD Unknown0x1c;
/*0x20*/ DWORD SpellAwareness;
/*0x24*/ DWORD OffenseEnhancement;
/*0x28*/ DWORD ManaEnhancement;
/*0x2c*/ DWORD HealthEnhancement;
/*0x30*/ DWORD HealthRegen;
/*0x34*/ DWORD FindPathPC;
/*0x38*/ DWORD HoTT;
/*0x3c*/
} LEADERABILITIES, *PLEADERABILITIES;

typedef struct _EQC_INFO {
/* 0x0000 */    DWORD   minus4;
/* 0x0004 */    DWORD   stuff_offset;
} EQC_INFO, *PEQC_INFO;

typedef struct _CI_INFO {
/* 0x0000 */    DWORD   minus8;
/* 0x0004 */    DWORD   stuff_offset;
/* 0x0008 */    DWORD   Unknown0x8;    // 2000
/* 0x000c */    DWORD   Unknown0xc;    // 2500
/* 0x0010 */    DWORD   Unknown0x10;   // 3000
/* 0x0014 */    DWORD   Unknown0x14;   // 4000
/* 0x0018 */    DWORD   Unknown0x18;   // 10000
} CI_INFO, *PCI_INFO;

typedef struct _CI2_INFO {
/* 0x0000 */    DWORD   Unknown0x0;
/* 0x0004 */    struct  _CHARINFO2* pCharInfo2;
/* 0x0008 */    struct  _CHARINFO2* pCharInfo3;
/* 0x000c */    BYTE    Unknown0xc[0x14];
/* 0x0010 */    void    *Unknown0x10[0x10];
/* 0x0060 */
} CI2_INFO, *PCI2_INFO;

typedef struct _GROUPMEMBER {
/*0x00*/ void   *vftable;
/*0x04*/ struct _CXSTR *pName;
/*0x08*/ BYTE   Mercenary;
/*0x09*/ BYTE   Unknown0x8[0x3];
/*0x0c*/ struct _CXSTR *pOwner; // name of mercenary's owner
/*0x10*/ DWORD  Level;
/*0x14*/ BYTE   Offline;//1 if groupmember is offline
/*0x15*/ BYTE   Unknown0x15;
/*0x16*/ BYTE   MainTank;
/*0x17*/ BYTE   MainAssist;
/*0x18*/ BYTE   Puller;
/*0x19*/ BYTE   Unknown0x19[0x3];
/*0x1c*/ DWORD  Roles;          // (Roles & 1) = MainTank, 2 = MainAssist, 4 = Puller
/*0x20*/ BYTE   Unknown0x20[0x8];
/*0x28*/ struct _SPAWNINFO *pSpawn;
/*0x2c*/
} GROUPMEMBER, *PGROUPMEMBER;

typedef struct _GROUPINFO {
/*0x00*/ void   *vftable;
/*0x04*/ struct _GROUPMEMBER *pMember[0x6];
/*0x1c*/ struct _GROUPMEMBER *pLeader;
/*0x20*/
} GROUPINFO, *PGROUPINFO;

typedef struct _BANKARRAY {
/*0x00*/ struct _CONTENTS* Bank[NUM_BANK_SLOTS];
/*0x60*/
} BANKARRAY, *PBANKARRAY;

typedef struct _SHAREDBANKARRAY {
/*0x00*/ struct _CONTENTS* SharedBank[NUM_SHAREDBANK_SLOTS];
/*0x10*/
} SHAREDBANKARRAY, *PSHAREDBANKARRAY;

typedef struct _MERCEQUIPMENT {
	/*0x00*/ struct _CONTENTS* MercEquipment[4];
	/*0x68*/
} MERCEQUIPMENT, *PMERCEQUIPMENT;

//added dec 08 2014 -eqmule
typedef struct _KEYRINGARRAY {
	union {
		/*0x00*/ struct _CONTENTS* Mounts[MAX_KEYRINGITEMS];
		/*0x00*/ struct _CONTENTS* Illusions[MAX_KEYRINGITEMS];
		/*0x00*/ struct _CONTENTS* Familiars[MAX_KEYRINGITEMS];
	};
	/*0x28*/
} KEYRINGARRAY, *PKEYRINGARRAY;

#pragma pack(push)
#pragma pack(8)
union EqGuid
{
	unsigned __int64 GUID;
	struct Split
	{
		UINT UniqueEntityID;
		WORD WorldUniqueID;
		WORD Unknown;
	};
};
#pragma pack(pop)

struct Point 
{
	UINT PointType;
	UINT PointSubtype;
	UINT CurrentCount;
	UINT TotalEver;
};
class CPlayerPointManager
{
public:
/*0x00*/	PVOID vfTable;
/*0x04*/	ArrayClass_RO<Point *>Points;
/*0x14*/
};
struct PointNamesEntry
{
	UINT PointTypeId;
	UINT PointSubtypeId;
	int DBStringId;
	int PointItemId;
	int ImageId;
	int MaxStackSize;
	bool bStationCashRelated;
};
class PointSystemBase
{
public:
/*0x00*/	PVOID vfTable;
/*0x04*/	ArrayClass_RO<PointNamesEntry *>PointNameEntries;
/*0x14*/
};
template<class T> class LinkedListNode
{
public:
	T               Object;
	LinkedListNode* pNext;
	LinkedListNode* pPrev;
};
template <class T> class DoublyLinkedList
{
public:
/*0x00*/    PVOID vfTable;
/*0x04*/    LinkedListNode<T>* pHead;
/*0x08*/    LinkedListNode<T>* pTail;
/*0x0c*/    LinkedListNode<T>* pCurObject;
/*0x10*/    LinkedListNode<T>* pCurObjectNext;
/*0x14*/    LinkedListNode<T>* pCurObjectPrev;
/*0x18*/    int NumObjects;
/*0x1c*/    int RefCount;
/*0x20*/
};
class PendingReward
{
public:
	PVOID vfTable;
	int		ID;
	int		SetID;
	FLOAT	RewardAdjustment;
	CHAR	RewardTitle[0x80];
};
class PendingRewardList : public DoublyLinkedList<PendingReward*>
{
public:
/*0x20*/	int	NextUID;
/*0x24*/	int MaxPending;
/*0x28*/	int ZoneMaxPending;
/*0x2c*/
};
enum ELockoutCharacterReason
{
	LCR_AllowNormalPlay,
	LCR_InvalidWornItem,
	LCR_Something,
};
struct ProgressionExperience
{
/*0x00*/	int ProgressionID;
/*0x08*/	double ProgressionExp;
/*0x10*/
};


//start of reorganisation for charinfo etc...
//todo: finish it...

template <typename TItem> class ItemContainer
{
public:
/*0x00*/	UINT Size;
/*0x04*/	int Spec;
/*0x08*/	VeArray<VePointer<TItem>> Items;
/*0x0c*/	BYTE AtDepth;
/*0x10*/	short Slots[2];
/*0x14*/	bool bDynamic;
/*0x18*/
};

class ItemBaseContainer : public ItemContainer<PCONTENTS>
{
public:

};

class CharacterPropertyHash : public HashTable<int>
{
public:

};

enum eProfileType {
	ePTMain,
	ePTAlt,
	ePTMonsterMission,
	ePTUnknown
};

enum eProfileListType
{
	ePLT_Regular,
	cPLT_MonsterMission
};
//BaseProfile Todo: fill it in...
class BaseProfile
{
	//has a vftable
public:
/*0x0000*/	PVOID vfTable;
/*0x0004*/	BaseProfile*		pNextProfile;
/*0x0008*/	BaseProfile*		pPrevProfile;
/*0x000c*/  eProfileListType	profileListType;
/*0x0010*/	ItemBaseContainer	pInventoryArray;
/*0x0000*/	ItemBaseContainer	TributeBenefitItems;
/*0x0000*/	ItemBaseContainer	TrophyTributeBenefitItems;
/*0x0000*/	TSafeArrayStatic<struct _SPELLBUFF, NUM_LONG_BUFFS>		Buff; 
/*0x0000*/	TSafeArrayStatic<struct _SPELLBUFF, NUM_SHORT_BUFFS>	ShortBuff;
/*0x0000*/	TSafeArrayStatic<unsigned int, NUM_BUFF_SLOTS>	CasterSpellIDs;
/*0x0000*/	TSafeArrayStatic<unsigned int, NUM_BUFF_SLOTS>	DiseaseSpreadTimers;
/*0x0000*/	TSafeArrayStatic<int, NUM_BOOK_SLOTS>	SpellBook;
/*0x0000*/	TSafeArrayStatic<int, 0x10>	MemorizedSpells;
/*0x0000*/	TSafeArrayStatic<int, 0x64>	Skill;
/*0x0000*/	TSafeArrayStatic<int, 0x19>	InnateSkill;
	//todo: add tthe rest sync it up with charinfo2
};

class ProfileList
{
	//has no vftable
public:
/*0x00*/	eProfileListType ListType;
/*0x04*/	BaseProfile* pFirst;
/*0x08*/	BaseProfile* pLast;
/*0x0c*/	ProfileList* pNext;
/*0x10*/	ProfileList* pPrev;
/*0x14*/
};

class CProfileManager
{
	//has no vftable
public:
/*0x00*/	ProfileList* pFirst;
/*0x04*/	eProfileListType CurProfileList;
/*0x08*/
};
struct PCTaskStatus
{
/*0x00*/	int TaskID;
/*0x04*/	int MovingStartTime;
/*0x08*/	int InitialStartTime;
/*0x0c*/	bool ElementActive[0x14];
/*0x20*/	int CurrentCounts[0x14];
/*0x70*/
};

struct BenefitSelection
{
/*0x00*/	int BenefitID;
/*0x04*/	int BenefitTier;
/*0x08*/
};
struct MailItemData
{
/*0x00*/	UINT	SendTime;
/*0x04*/	PCXSTR	SenderName;
/*0x08*/	PCXSTR	Note;
/*0x0c*/
};

enum eDynamicZoneType
{
	eDZT_Unknown,
	eDZT_Expedition,
	eDZT_Tutorial,
	eDZT_Task,
	eDZT_Mission,
	eDZT_Quest
};

struct DynamicZoneData
{
/*0x00*/	int		DynamicZoneID;
/*0x04*/	int		SetID;
/*0x08*/	eDynamicZoneType	Type;
/*0x0c*/
};
struct TradeskillRecipeCount
{
/*0x00*/	int		SkillID;
/*0x04*/	int		RecipeCount;
/*0x08*/
};

struct DynamicZoneTimerData
{
/*0x00*/	int		DataSetID;
/*0x04*/	UINT	TimerExpiration;
/*0x08*/	int		EventID;
/*0x0c*/	int		DynamicZoneID;
/*0x10*/	DynamicZoneTimerData	*pNext;
/*0x14*/
};

struct PvPKill
{
/*0x00*/	CHAR VictimName[0x40];
/*0x04*/	int VictimLevel;
/*0x08*/	DWORD VictimRace;
/*0x0c*/	DWORD VictimClass;
/*0x10*/	int ZoneID;
/*0x14*/	long Lastkilltime;
/*0x18*/	int PointsEarned;
/*0x1c*/
};

struct PvPDeath
{
/*0x00*/	CHAR KillerName[0x40];
/*0x04*/	int KillerLevel;
/*0x08*/	DWORD KillerRace;
/*0x0c*/	DWORD KillerClass;
/*0x10*/	int ZoneID;
/*0x14*/	long LastDeathTime;
/*0x18*/	int PointsLost;
/*0x1c*/
};

struct PvPKill24HourData : public PvPKill
{
	//nothing here?
};

enum eAreaCorner
{
	eAC_None = -1,
	eAC_TopLeftCorner,
	eAC_TopRightCorner,
	eAC_BottomLeftCorner,
	eAC_BottomRightCorner,
};

struct PCAdventureThemeStats
{
/*0x00*/	int SucceededNormal;
/*0x04*/	int FailedNormal;
/*0x08*/	int SucceededHard;
/*0x0c*/	int FailedHard;
/*0x10*/	int AdventureTotalPointsEarned;
/*0x14*/
};

struct PCAdventureData
{
/*0x000*/	int AdventureLastAdventureDefinitionSeen[5];
/*0x014*/	UINT AdventureLastAdventureDefinitionSeenTime[5];
/*0x028*/	int AdventureActiveAdventureId;
/*0x02c*/	int AdventureActiveAdventureTheme;
/*0x030*/	int AdventureActiveAdventureRisk;
/*0x034*/	int AdventureSafeReturnZoneId;
/*0x038*/	FLOAT AdventureSafeReturnX;
/*0x03c*/	FLOAT AdventureSafeReturnY;
/*0x040*/	FLOAT AdventureSafeReturnZ;
/*0x044*/	int AdventureStatAccepted;
/*0x048*/	int AdventureStatRejected;
/*0x04c*/	int AdventureStatEntered;
/*0x050*/	int AdventureStatFailedEnter;
/*0x054*/	int AdventurePointsAvailable;
/*0x058*/	int AdventurePointsAvailableMax;
/*0x05c*/	UINT AdventureLastSuccessTime;
/*0x060*/	PCAdventureThemeStats ThemeStats[6];
/*0x078*/
};

struct MonsterMissionTemplate
{
/*0x00*/	int		TemplateID;
/*0x04*/	int		Min;
/*0x08*/	int		Max;
/*0x0c*/	int		NumSelected;
/*0x10*/	bool	CanSelect;
/*0x14*/	CHAR	TemplateName[0x40];
/*0x54*/
};

struct PCSharedTaskData
{
/*0x00*/	int	ActiveSharedTaskID;
/*0x04*/	bool bIsMonsterMission;
/*0x08*/	PCTaskStatus Status;//size 0x70
/*0x78*/	ArrayClass_RO<MonsterMissionTemplate> Templates;//size is 0x10
/*0x88*/	FLOAT	RewardAdjustment;
/*0x8c*/
};

struct TaskTimerData
{
/*0x00*/	int GroupID;
/*0x04*/	int TimerSeconds;
/*0x08*/	UINT TimerExpiration;
/*0x0c*/	int TimerType;
/*0x10*/	int OrigTaskID;
/*0x14*/	TaskTimerData *pNext;
/*0x18*/
};

struct PCCompletedQuest
{
/*0x00*/	int QuestID;
/*0x04*/	int ElementBitmask;
/*0x08*/	UINT TimeCompleted;
/*0x0c*/
};

struct PCQuestHistoryData
{
/*0x000*/	PCCompletedQuest Quests[0x32];//size 0xc * 0x32 = 0x258
/*0x258*/
};

class StatCounter
{
public:
/*0x00*/	UINT Value;
/*0x04*/
};

class StatElapsedTime
{
public:
/*0x00*/	UINT StartTick;
/*0x04*/	UINT ElapsedTotal;
/*0x08*/	bool bCurrentState;
/*0x0c*/
};

class PCStatistics
{
public:
	enum eStatisticType {
		S_TotalExpEarned,
		S_GroupExpEarned,
		S_ExpRaidEarned,
		S_ExpSoloEarned,
		S_NonExpKills,
		S_ExpKills,
		S_ZonesVisited,
		S_ChatShouts,
		S_ChatOOCs,
		S_ChatSays,
		S_ChatGroup,
		S_ChatTells,
		S_Deaths, 
		S_Resurrections, 
		S_PlatEarned,
		S_TradeskillCombines,
		S_Forages,
		S_Quests,
		S_LastStat,
	};
/*0x000*/	StatElapsedTime StatTimeSession; //size 0xc
/*0x00c*/	StatElapsedTime StatTimeLFG;
/*0x018*/	StatElapsedTime StatTimeGrouped;
/*0x024*/	StatElapsedTime StatTimeSolo;
/*0x030*/	StatElapsedTime StatTimeRaid;
/*0x03c*/	StatElapsedTime StatTimeInBazaar;
/*0x048*/	StatCounter Statistics[S_LastStat];//size is 0x48
/*0x090*/	UINT LastUpdateTime;
/*0x094*/	CHAR PlayerName[0x40];
/*0x0d4*/	CHAR PlayerStationID[0x20];
/*0x0f4*/	int PlayerLevel;
/*0x0f8*/	DWORD PlayerRace;
/*0x0fc*/	DWORD PlayerClass;
/*0x100*/	UINT UniquePlayerID;
/*0x104*/
};

class GroupMemberStats
{
public:
	enum eStatisticType {
		S_Mez,
		S_Root,
		S_Charmed,
		S_Stunned,
		S_Slowed,
		S_FirstAgro,
		S_DmgMelee,
		S_DmgRanged,
		S_DmgSpell,
		S_DmgDot,
		S_DmgPet,
		S_DmgTaken,
		S_DmgHealed,
		S_ExpTotalEarned,
		S_Deaths,
		S_ExpKills,
		S_NonExpKills,
		S_ManaUsed,
		S_EnduranceUsed,
		S_LastStat,
	};
/*0x000*/	CHAR PlayerName[0x40];
/*0x040*/	int PlayerLevel;
/*0x044*/	int PlayerRace;
/*0x048*/	int PlayerClass;
/*0x04c*/	UINT UniquePlayerId;
/*0x050*/	UINT LastMemberUpdateTime;
/*0x054*/	PCXSTR StationID;
/*0x058*/	int PlayerGuild;
/*0x05c*/	TSafeArrayStatic<int, NUM_LONG_BUFFS> BuffIDs;
/*0xxxx*/	TSafeArrayStatic<StatCounter, S_LastStat> Statistics;
/*0xxxx*/
};

struct ClaimData
{
/*0x00*/	int FeatureID;
/*0x04*/	int Count;
/*0x08*/
};

class ClaimDataCollection
{
public:
/*0x00*/	ArrayClass<ClaimData> ClaimData;
/*0x10*/
};

class MercenaryAbilityInfo
{
public:
/*0x00*/	int Index;
/*0x04*/	int Cost;
/*0x08*/
};

struct CompletedAchievementData
{
/*0x00*/	int AchievementID;
/*0x04*/	UINT CompletedTimestamp;
/*0x08*/	int CompletedVersion;
/*0x0c*/
};

enum eAchievementSubReqType
{ 
	eASCRT_Invalid, 
	eASCRT_Requirement, 
	eASCRT_KillNpcRaceMat, 
	eASCRT_RightClickItem,
	eASCRT_KillNpc,
	eASCRT_Count,
};

struct AchievementSubComponentCountData
{
/*0x00*/	int AchievementID;
/*0x04*/	int ComponentID;
/*0x08*/	int RequirementID;
/*0x0c*/	eAchievementSubReqType SubReqType;
/*0x10*/	int Count;
/*0x14*/
};

struct WorldLocation
{
/*0x00*/ DWORD        ZoneBoundID;
/*0x04*/ FLOAT        ZoneBoundY;
/*0x08*/ FLOAT        ZoneBoundX;
/*0x0c*/ FLOAT        ZoneBoundZ;
/*0x10*/ FLOAT        ZoneBoundHeading;
/*0x14*/
};

struct RaidData
{
/*0x00*/	int MainAssists[3];
/*0x0c*/	CHAR MainAssistNames[3][0x40];
/*0xcc*/	int MainMarkers[3];
/*0xd8*/	int MasterLooter;
/*0xdc*/
};

class CCharacterBase
{
public:
	PVOID vfTable;
	CProfileManager ProfileManager;
	//todo: fil in the rest and sync it
};

//need to check CHARINFO
class PcBase : public CCharacterBase
{
public:
	TSafeArrayStatic<int, 0xa>	RecentTasks;
	TSafeArrayStatic<PCTaskStatus, 1>	Tasks;
	TSafeArrayStatic<PCTaskStatus, 0x1d>	Quests;
	TSafeArrayStatic<BYTE, 6400 / 8>	BitFlags;
	TSafeArrayStatic<BenefitSelection, 5> ActiveTributeBenefits;
	TSafeArrayStatic<BenefitSelection, 10> ActiveTrophyTributeBenefits;
	ItemBaseContainer	BankItems;
	ItemBaseContainer	SharedBankItems;
	ItemBaseContainer	LimboBufferItems;
    ItemBaseContainer	MercenaryItems;
	ItemBaseContainer	MountKeyRingItems;
	ItemBaseContainer	IllusionKeyRingItems;
	ItemBaseContainer	FamiliarKeyRingItems;
	ItemBaseContainer	AltStorageItems;
	ItemBaseContainer	ArchivedDeletedItems;
	ItemBaseContainer	MailItems;
	HashTable<MailItemData, EqItemGuid, ResizePolicyNoShrink>	MailItemsData;
	TSafeArrayStatic<UINT, 1>	RecentMoves;
	HashTable<DynamicZoneData>	CurrentDynamicZones;
	HashTable<int>	LearnedRecipes;	
	EQList<TradeskillRecipeCount*>	QualifyingRecipeCounts;
	HashTable<int>	NonrepeatableQuests;
	HashTable<int>	CompletedTasks;
	UINT	AlchemyTimestamp;
	bool	bSomethingHome;
	DWORD	LoginTime;
	// more todo here:
	// ...
	//
	__int64 Vitality;
	int AAVitality;
	int FPStuff[0x1d];
};

//aStartingLoad_
// actual size: 0x5804 May 10 2013 (see 53BE57) - eqmule
typedef struct _CHARINFO {
/*0x0000*/ void*        vtable1;
/*0x0004*/ void*        punknown;
/*0x0008*/ struct _CI_INFO*     charinfo_info;
/*0x000c*/   BYTE       Unknown0xc[0x1d94];
/*0x1da0*/   struct     _LEADERABILITIES MyAbilities; //points spent in each ability (size 0x3c)
/*0x1ddc*/   BYTE       Unknown0x1ddc[0xcc];
/*0x1ea8*/ struct _BANKARRAY*   pBankArray;
/*0x1eac*/ DWORD        NumBankSlots;//how many bank slots we have
/*0x1eb0*/ BYTE         Unknown0x1eb0[0x30];
/*0x1ee0*/ struct _SHAREDBANKARRAY*     pSharedBankArray;
/*0x1ee4*/ DWORD        NumSharedSlots;//how many sharedbank slots we have
/*0x1ee8*/ BYTE         Unknown0x1ee8[0x164];
/*0x204c*/ DWORD        GuildID;//GuildID_0
/*0x2050*/ DWORD        FellowshipID;
/*0x2054*/ bool			Unknown0x2054;
/*0x2058*/ int			Unknown0x2058;
/*0x205c*/ int			GuildStatus;
/*0x2060*/ int			GuildFlags;
/*0x2064*/ bool			GuildShowSprite;
/*0x2068*/ UINT			CreationTime;//0x2068 for sure see 7C9CE9
/*0x206c*/ UINT			AccountCreationTime;
/*0x2070*/ UINT			LastPlayedTime;
/*0x2074*/ DWORD		MinutesPlayed;
/*0x2078*/ BYTE			Anonymous;
/*0x2079*/ bool			bGM;
/*0x207a*/ bool			bGMStealth;
/*0x207c*/ DWORD        AAExp;
/*0x2080*/ BYTE         NobilityRank;
/*0x2081*/ BYTE         PercentEXPtoAA;
/*0x2082*/ BYTE         Unknown0x2082[0x42];
/*0x20c4*/ DWORD        TributeTimer;
/*0x20c8*/ DWORD        BenefitTimer;
/*0x20cc*/ BYTE         Unknown0x20cc[0x4];
/*0x20d0*/ DWORD        CareerFavor;
/*0x20d4*/ BYTE         Unknown0x20d4[0x4];
/*0x20d8*/ DWORD        CurrFavor;
/*0x20dc*/ BYTE         Unknown0x20dc[0xc];
/*0x20e8*/ DOUBLE       GroupLeadershipExp;
/*0x20f0*/ DOUBLE       RaidLeadershipExp;
/*0x20f8*/ DWORD        GroupLeadershipPoints;
/*0x20fc*/ DWORD        RaidLeadershipPoints;
/*0x2100*/ BYTE         Unknown0x2100[0x194];
/*0x2294*/ DWORD        RadiantCrystals;
/*0x2298*/ BYTE         Unknown0x2298[0x4];
/*0x229c*/ DWORD        EbonCrystals;
/*0x22a0*/ BYTE         Unknown0x22a0[0x670];
/*0x2910*/ __int64      Exp;
/*0x2918*/ int	        DaysEntitled;
/*0x291c*/ int	        SpentVeteranRewards;
/*0x2920*/ bool	        bVeteranRewardEntitled;
/*0x2921*/ bool	        bAutoConsentGroup;
/*0x2922*/ bool	        bAutoConsentRaid;
/*0x2923*/ bool	        bAutoConsentGuild;
/*0x2924*/ bool	        bPrivateForEqPlayers;
/*0x2928*/ long	        AchievementFilesModificationTime;
/*0x292c*/ CHAR	        StationID[0x20];
/*0x2950*/ EqGuid       Guid;//size 8
/*0x2958*/ bool	        bBetaBuffed;
/*0x295c*/ int	        StartingCity;
/*0x2960*/ int	        MainLevel;
/*0x2964*/ bool	        bShowHelm;
/*0x2968*/ __int64      LastTestCopyTime;
/*0x2970*/ int		    Unknown0x2970;
/*0x2974*/ int		    Unknown0x2974;
/*0x2978*/ CPlayerPointManager        PointManager;
/*0x298c*/ PointSystemBase PointSystem;
/*0x29a0*/ UINT			LoyaltyVelocity;
/*0x29a4*/ UINT			LoyaltyTokens;
/*0x29a8*/ bool			bHasLoyaltyInfo;
/*0x29ac*/ ArrayClass_RO<int> OwnedRealEstates;
/*0x29bc*/ ArrayClass_RO<int> OwnedItemRealEstates;
/*0x29cc*/ ArrayClass_RO<int> ArchivedRealEstates;
/*0x29dc*/ CHAR			OverridePetName[0x40];
/*0x2A1C*/ bool			bCanRequestPetNameChange;
/*0x2A1d*/ CHAR			OverrideFamiliarName[0x40];//guessing 
/*0x2A5D*/ bool         bCanRequestFamiliarNameChange;//still a guess todo: confirm
/*0x2A60*/ _CXSTR		*OverrideMercName[0xb];
/*0x2A8c*/ bool			bCanRequestMercNameChange;
/*0x2A90*/ PendingRewardList PendingRewards;//size 0x2c
/*0x2Abc*/ UINT         DowntimeReductionTime;
/*0x2Ac0*/ UINT         DowntimeTimerStart;
/*0x2Ac4*/ FLOAT        ActivityValue;
/*0x2Ac8*/ _CXSTR        *SharedBank;
/*0x2Acc*/ _CXSTR        *BankBuffer;
/*0x2Ad0*/ _CXSTR        *LimboBuffer;
/*0x2Ad4*/ _CXSTR        *MercenaryBuffer;
/*0x2Ad8*/ _CXSTR        *AltStorageBuffer;
/*0x2Adc*/ UINT         AltStorageTimestamp;
/*0x2Ae0*/ ELockoutCharacterReason LCR;
/*0x2Ae4*/ HashTable<ProgressionExperience> ProgressionExp;
/*0x2Af4*/ _CXSTR        *ArchivedStorageBuffer;
/*0x2af8*/ PCXSTR       MailItemsBuffer;
/*0x2afc*/ PCXSTR       MailItemsDataBuffer;
/*0x2b00*/ int          MailItemsOverCapWarningCount;
/*0x2b04*/ int			Unknown0x2b04;
/*0x2b08*/ int			Unknown0x2b08;
/*0x2b0c*/ int			Unknown0x2b0c;
/*0x2b10*/ int			Unknown0x2b10;
/*0x2b14*/ BYTE			Unknown0x2b14;
/*0x2b15*/ BYTE			Unknown0x2b15;
/*0x2b16*/ BYTE         UseAdvancedLooting;             //0=off 1=on
/*0x2b17*/ BYTE         MasterLootCandidate;                     //0=off 1=on
/*0x2b18*/ BYTE         Unknown0x2b18[0x20c];
/*0x2d24*/ DWORD        Krono;
/*0x2d28*/ DWORD        CursorKrono;
/*0x2d2c*/ int			MercAAExp;//wrong wrong wrong
/*0x2d30*/ DWORD        MercAAPoints;
/*0x2d34*/ DWORD        MercAAPointsSpent;
/*0x2d38*/ BYTE		    Unknown0x2d3c[0x28];
/*0x2D60*/ __int64      Vitality;
/*0x2D68*/ int		    AAVitality;
/*0x2D6c*/ int          FPStuff[0x17];
/******************* End PCBASE ************/
/*0x2dc8*/ void*        vtable2;//vtable2_0 below aTimeIsDAndCanU
/*0x2dcc*/ struct _EQC_INFO*    eqc_info;
/*0x2dd0*/ struct _SPAWNINFO*   pSpawn;//pSpawn_0
/*0x2dd4*/ BYTE         UpdateStuff;
/*0x2dd5*/ BYTE         Unknown0x2dd5[0x3];
/*0x2dd8*/ DWORD        ArmorClassBonus;//vtable2+10
/*0x2ddc*/ DWORD        CurrWeight;//vtable2+14
/*0x2de0*/ BYTE         Unknown0x2de0[0xc];
/*0x2dec*/ DWORD        HPBonus;//vtable2+24
/*0x2df0*/ DWORD        ManaBonus;//vtable2+28
/*0x2df4*/ DWORD        EnduranceBonus;//vtable2+2c
/*0x2df8*/ DWORD        CombatEffectsCap;//vtable2+30 CombatEffectsBonus
/*0x2dfc*/ DWORD        ShieldingCap;//vtable2+34 ShieldingBonus
/*0x2e00*/ DWORD        SpellShieldCap;//vtable2+38 SpellShieldBonus
/*0x2e04*/ DWORD        AvoidanceCap;//vtable2+3c AvoidanceBonus
/*0x2e08*/ DWORD        AccuracyCap;//vtable2+40 AccuracyBonus
/*0x2e0c*/ DWORD        StunResistCap;//vtable2+44 StunResistBonus
/*0x2e10*/ DWORD        StrikeThroughCap;//vtable2+48 StrikeThroughBonus
/*0x2e14*/ BYTE         Unknown0x2e14[0x24];
/*0x2e38*/ DWORD        SkillMinDamageModBonus[0x9];//vtable2+70
/*0x2e5c*/ BYTE         Unknown0x2e5c[0x4];
/*0x2e60*/ DWORD        DoTShieldCap;//DoTShieldBonus //vtable2+98
/*0x2e64*/ DWORD        DamageShieldMitigationCap;//DamageShieldMitigationBonus
/*0x2e68*/ DWORD        CombatEffectsBonus;
/*0x2e6c*/ DWORD        SpellShieldBonus;
/*0x2e70*/ DWORD        ShieldingBonus;
/*0x2e74*/ DWORD        DamageShieldBonus;
/*0x2e78*/ DWORD        DoTShieldBonus;
/*0x2e7c*/ DWORD        DamageShieldMitigationBonus;
/*0x2e80*/ DWORD        AvoidanceBonus;
/*0x2e84*/ DWORD        AccuracyBonus;
/*0x2e88*/ DWORD        StunResistBonus;
/*0x2e8c*/ DWORD        StrikeThroughBonus;
/*0x2e90*/ DWORD        HeroicSTRBonus;
/*0x2e94*/ DWORD        HeroicINTBonus;
/*0x2e98*/ DWORD        HeroicWISBonus;
/*0x2e9c*/ DWORD        HeroicAGIBonus;
/*0x2ea0*/ DWORD        HeroicDEXBonus;
/*0x2ea4*/ DWORD        HeroicSTABonus;
/*0x2ea8*/ DWORD        HeroicCHABonus;
/*0x2eac*/ DWORD        HeroicSvMagicBonus;
/*0x2eb0*/ DWORD        HeroicSvFireBonus;
/*0x2eb4*/ DWORD        HeroicSvColdBonus;
/*0x2eb8*/ DWORD        HeroicSvDiseaseBonus;
/*0x2ebc*/ DWORD        HeroicSvPoisonBonus;
/*0x2ec0*/ DWORD        HeroicSvCorruptionBonus;
/*0x2ec4*/ DWORD        HealAmountBonus;
/*0x2ec8*/ DWORD        SpellDamageBonus;
/*0x2ecc*/ BYTE         Unknown0x2ecc[0x8];
/*0x2ed4*/ DWORD        ClairvoyanceBonus;
/*0x2ed8*/ DWORD        AttackBonus;
/*0x2edc*/ DWORD        HPRegenBonus;
/*0x2ee0*/ DWORD        ManaRegenBonus;
/*0x2ee4*/ DWORD        EnduranceRegenBonus;
/*0x2ee8*/ DWORD        DamageShieldCap;
/*0x2eec*/ DWORD        AttackSpeed;
/*0x2ef0*/ BYTE         Unknown0x2ef0[0x2c8];
/*0x31b8*/ ExtendedTargetList*  pXTargetMgr;
/*0x31bc*/ DWORD        InCombat;
/*0x31c0*/ DWORD        Downtime;
/*0x31c4*/ DWORD        DowntimeStamp;
/*0x31c8*/ BYTE         Unknown0x31c8[0x4];
/*0x31cc*/ struct _GROUPINFO*   pGroupInfo;
/*0x31d0*/ BYTE         Unknown0x31d0[0x14];
/*0x31e4*/ DWORD        CharBaseBegin;//we use this for finding the next members of this struct
/*0x31e8*/ BYTE         Unknown0x31e8[0x4];
/*0x31ec*/ void*        pCharacterBase;
/*0x31f0*/ struct _CI2_INFO*    pCI2;//cant find a pointer to this so lets just say its always at pCharacterBase+4
/*0x31f4*/ BYTE         Unknown0x31f4[0x4];
/*0x31f8*/ BYTE         languages[0x20];//CharBaseBegin+14
/*0x3218*/ BYTE         Unknown0x3218[0x10];
/*0x3228*/ CHAR         Name[0x40];//CharBaseBegin+44
/*0x3268*/ CHAR         Lastname[0x20];//CharBaseBegin+84
/*0x3288*/ BYTE         Unknown0x3288[0x60];
/*0x32e8*/ BYTE         Stunned;//CharBaseBegin+104
/*0x32e9*/ BYTE         Unknown0x32e9[0x3];
/*0x32ec*/ WORD         zoneId;//CharBaseBegin+108 Zone_0
/*0x32ee*/ WORD         instance;
/*0x32f0*/ DWORD        standstate;//CharBaseBegin+10c
/*0x32f4*/ BYTE         Unknown0x32f4[0x354];
/*0x3648*/ DWORD        ExpansionFlags;//CharBaseBegin+464
/*0x364c*/ BYTE         Unknown0x364c[0x20];
/*0x366c*/ DWORD        BankSharedPlat;//31e4 CharBaseBegin+488
/*0x3670*/ DWORD        BankSharedGold;//CharBaseBegin+48c
/*0x3674*/ DWORD        BankSharedSilver;//CharBaseBegin+490
/*0x3678*/ DWORD        BankSharedCopper;//CharBaseBegin+494
/*0x367c*/ DWORD        BankPlat;//CharBaseBegin+498
/*0x3680*/ DWORD        BankGold;//CharBaseBegin+49c
/*0x3684*/ DWORD        BankSilver;//CharBaseBegin+4a0
/*0x3688*/ DWORD        BankCopper;//CharBaseBegin+4a4
/*0x368c*/ DWORD        STR;//CharBaseBegin+4a8
/*0x3690*/ DWORD        STA;//CharBaseBegin+4ac
/*0x3694*/ DWORD        CHA;//CharBaseBegin+4b0
/*0x3698*/ DWORD        DEX;//CharBaseBegin+4b4
/*0x369c*/ DWORD        INT;//CharBaseBegin+4b8
/*0x36a0*/ DWORD        AGI;//CharBaseBegin+4bc
/*0x36a4*/ DWORD        WIS;//CharBaseBegin+4c0
/*0x36a8*/ DWORD        SavePoison;//CharBaseBegin+4c4
/*0x36ac*/ DWORD        SaveMagic;//CharBaseBegin+4c8
/*0x36b0*/ DWORD        SaveDisease;//CharBaseBegin+4cc
/*0x36b4*/ DWORD        SaveCorruption;//CharBaseBegin+4d0
/*0x36b8*/ DWORD        SaveFire;//CharBaseBegin+4d4
/*0x36bc*/ DWORD        SaveCold;//CharBaseBegin+4d8
/*0x36c0*/ BYTE			Unknown0x36c0[0x2144];
/*0x5804*/
} CHARINFO, *PCHARINFO;

typedef struct _INVENTORYARRAY {
union {
/*0x00*/ struct _INVENTORY Inventory;
/*0x00*/ struct _CONTENTS* InventoryArray[NUM_INV_SLOTS];
};
} INVENTORYARRAY, *PINVENTORYARRAY;

// actual size: 0x9950 May 10 2013 (see 7CD1E5) - eqmule
typedef struct _CHARINFO2 {
/*0x0000*/ BYTE         Unknown0x0000[0x20];
/*0x0020*/ struct _INVENTORYARRAY*      pInventoryArray;
/*0x0024*/ BYTE         Unknown0x0024[0x9c];
/*0x00c0*/ struct _SPELLBUFF    Buff[NUM_LONG_BUFFS];
/*0x0f30*/ struct _SPELLBUFF    ShortBuff[NUM_SHORT_BUFFS];
/*0x2218*/ DWORD        ZoneBuffs[NUM_BUFF_SLOTS]; // caster IDs for the Effects Window of whoever cast the buff on you in the same zone...
/*0x239c*/ DWORD        ZoneSongs[NUM_BUFF_SLOTS]; // just a guess
/*0x2520*/ DWORD        SpellBook[NUM_BOOK_SLOTS];
/*0x3060*/ DWORD        MemorizedSpells[0x10];
/*0x30a0*/ DWORD        Skill[0x64];
/*0x3230*/ DWORD        InnateSkill[0x19];
/*0x3294*/ BYTE         Unknown0x3294[0xd8];
/*0x336c*/ DWORD        Gender;
/*0x3370*/ DWORD        Race;
/*0x3374*/ DWORD        Class;
/*0x3378*/ BYTE         Unknown0x3378[0x10];
/*0x3388*/ DWORD        Level;
/*0x338c*/ DWORD        Mana;
/*0x3390*/ DWORD        Endurance;
/*0x3394*/ DWORD        BaseHP;
/*0x3398*/ DWORD        BaseSTR;
/*0x339c*/ DWORD        BaseSTA;
/*0x33a0*/ DWORD        BaseCHA;
/*0x33a4*/ DWORD        BaseDEX;
/*0x33a8*/ DWORD        BaseINT;
/*0x33ac*/ DWORD        BaseAGI;
/*0x33b0*/ DWORD        BaseWIS;
/*0x33b4*/ BYTE         Unknown0x33b4[0x4];
/*0x33b8*/ DWORD        Plat;
/*0x33bc*/ DWORD        Gold;
/*0x33c0*/ DWORD        Silver;
/*0x33c4*/ DWORD        Copper;
/*0x33c8*/ DWORD        CursorPlat;
/*0x33cc*/ DWORD        CursorGold;
/*0x33d0*/ DWORD        CursorSilver;
/*0x33d4*/ DWORD        CursorCopper;
/*0x33d8*/ BYTE         Unknown0x33d8[0x28];
/*0x3400*/ DWORD        thirstlevel;
/*0x3404*/ DWORD        hungerlevel;
/*0x3408*/ BYTE         Unknown0x3408[0x4];
/*0x340c*/ DWORD        Shrouded;
/*0x3410*/ BYTE         Unknown0x3410[0x78];
/*0x3488*/ TSafeArrayStatic<WorldLocation, 5> BoundLocations;//size 0x64
/*0x34ec*/ DWORD        ArmorType[0x16];
/*0x3544*/ BYTE         Unknown0x3544[0x160];
/*0x36a4*/ AALIST       AAList[AA_CHAR_MAX_REAL];
/*0x44b4*/ DWORD        BodyColor[0x9];
/*0x44d8*/ BYTE         Unknown0x44d8[0x2000];
/*0x64d8*/ DWORD        CombatAbilities[NUM_COMBAT_ABILITIES];
/*0x6988*/ BYTE         Unknown0x6988[0x34];
/*0x69bc*/ DWORD        CombatAbilityTimes[0x14];
/*0x6a0c*/ BYTE         Unknown0x6a0c[0x1ed8];
/*0x88e4*/ DWORD        Deity;
/*0x88e8*/ BYTE         Unknown0x88e8[0x4];
/*0x88ec*/ DWORD        Drunkenness;
/*0x88f0*/ BYTE         Unknown0x88f0[0x10];
/*0x8900*/ DWORD        AAPoints;
/*0x8904*/ BYTE         Unknown0x8904[0x100c];
/*0x9910*/ DWORD        AAPointsSpent;
/*0x9914*/ DWORD        AAPointsAssigned;
/*0x9918*/ DWORD        Unknown0x9918[0x38];
/*0x9950*/
} CHARINFO2, *PCHARINFO2;

typedef struct _MODELINFONAME {
/*0x00*/    DWORD Unknown0000;
/*0x04*/    DWORD Unknown0004;
/*0x08*/    PCHAR Name;
/*0x0c*/
} MODELINFONAME, *PMODELINFONAME;

typedef struct _MODELINFO_GENERIC {
/*0x00*/    DWORD Type;
/*0x04*/    DWORD Unknown0x04;
/*0x08*/    DWORD Unknown0x08;
/*0x0c*/    DWORD Unknown0x0c;
/*0x10*/    DWORD Unknown0x10;
/*0x14*/
} MODELINFO_GENERIC, *PMODELINFO_GENERIC;

typedef struct _MODELINFO_48 {
/*0x00*/    struct _MODELINFO_GENERIC Header;
/*0x14*/    FLOAT Float1;
/*0x18*/    FLOAT Float2;
/*0x1c*/    FLOAT Float3;
/*0x20*/    struct _MODELINFONAME *pModelName;
/*0x24*/
} MODELINFO_48, *PMODELINFO_48;

typedef struct _MODELINFO_51 {
/*0x00*/    struct _MODELINFO_GENERIC Header;
/*0x14*/    struct _MODELINFONAME *pFontName;
/*0x18*/    PCHAR LabelText;
/*0x1c*/
} MODELINFO_51, *PMODELINFO_51;

// 10/09/2003 build      plazmic
// 12/24/2003 verified   Amadeus
typedef struct _MODELINFO {
/*0x00*/    DWORD   Unknown;
/*0x04*/    PCHAR NameDAG;
/*0x08*/    struct _MODELINFO_GENERIC *pModelInfo;
/*0x0c*/    struct _MODELINFO *pNextInChain;
/*0x10*/    PVOID pUnknown;
/*0x14*/    struct _MODELINFONAME *pTrack1;
/*0x18*/    struct _MODELINFONAME *pTrack2;
   //moredata
} MODELINFO, *PMODELINFO;

// 4/30/2003 build      eqmule
typedef struct _CAMERAINFO {
/*0x00*/   DWORD Unknown0x00;
/*0x04*/   DWORD Unknown0x04;
/*0x08*/   BYTE Unknown0x08[0x8];
/*0x10*/   FLOAT Y;
/*0x14*/   FLOAT X;
/*0x18*/   FLOAT Z;
/*0x1c*/   FLOAT LightRadius;
/*0x20*/
} CAMERAINFO, *PCAMERAINFO;


#define MODEL_LABEL                     0 
#define MODEL_LABELINFO                 1
#define MODEL_NULL2                     2
#define MODEL_HELD_R                    3
#define MODEL_HELD_L                    4
#define MODEL_SHIELD                    5
#define MODEL_NULL6                     6

/*
//Work in progress...
#define MODEL_HEAD                      0x00
#define MODEL_HEAD_POINT                0x01
#define MODEL_NULL_1                    0x02
#define MODEL_HELD_R                    0x03
#define MODEL_HELD_L                    0x04
#define MODEL_SHIELD                    0x05
#define MODEL_NULL_2                    0x06
#define MODEL_TUNIC                     0x07
#define MODEL_HAIR                      0x08
#define MODEL_BEARD                     0x09
#define MODEL_CHEST                     0x0a
#define MODEL_GLOVES                    0x0b
#define MODEL_GLOVES2                   0x0c
*/

typedef struct _ARGBCOLOR {
    union {
        struct {
            BYTE B;
            BYTE G;
            BYTE R;
            BYTE A;
        };
        DWORD ARGB;
    };
} ARGBCOLOR, *PARGBCOLOR;

// actual size: 0x14 05-14-2013 - eqmule
typedef struct _ARMOR {
/*0x00*/ DWORD  ID; //idfile on Lucy
/*0x04*/ DWORD  Unknown0x04;
/*0x08*/ DWORD  Unknown0x08;
/*0x0c*/ DWORD  Unknown0x0c;
/*0x10*/ DWORD  Unknown0x10;
/*0x14*/
} ARMOR, *PARMOR;

// 10-22-2003 Lax
typedef struct _EQUIPMENT {
   union {
      struct// EQUIPARRAY
      {
         struct _ARMOR Item[9];
      };// Array;
      struct //EQUIPUNIQUE
      {
         struct _ARMOR Head;//0x14
         struct _ARMOR Chest;
         struct _ARMOR Arms;
         struct _ARMOR Wrists;
         struct _ARMOR Hands;
         struct _ARMOR Legs;
         struct _ARMOR Feet;
         struct _ARMOR Primary;
         struct _ARMOR Offhand;
      };// Unique;
   };
} EQUIPMENT, *PEQUIPMENT;

// 10-14-07 - ieatacid
typedef struct _FELLOWSHIPMEMBER {
/*0x00*/  DWORD   WorldID;
/*0x04*/  CHAR    Name[0x40];
/*0x44*/  DWORD   ZoneID;
/*0x48*/  DWORD   Level;
/*0x4c*/  DWORD   Class;
/*0x50*/  DWORD   LastOn;    // FastTime() timestamp
/*0x54*/
} FELLOWSHIPMEMBER, *PFELLOWSHIPMEMBER;
struct FSDATA
{
	CHAR Strings[0x20];//need to check what these are...
};
// 20121128 - ieatacid  0x9e4
// Rof2 - eqmule  0x9e4 see 5909EF
typedef struct _FELLOWSHIPINFO {
/*0x000*/  DWORD  Version;
/*0x004*/  DWORD  FellowshipID;
/*0x008*/  CHAR   Leader[0x40];
/*0x048*/  CHAR   MotD[0x400];
/*0x448*/  DWORD  Members;
/*0x44c*/  struct _FELLOWSHIPMEMBER  FellowshipMember[0xc];
/*0x83c*/  DWORD  Sync;
/*0x840*/  FSDATA Somedata[0xc];//size 0x180
/*0x9c0*/  bool bExpSharingEnabled[0xc];
/*0x9cc*/  bool bSharedExpCapped[0xc];
/*0x9d8*/  BYTE Unknown0x9d8[0xc];
/*0x9e4*/
} FELLOWSHIPINFO, *PFELLOWSHIPINFO;

// offsets are relative to their position in _LAUNCHSPELLDATA
typedef struct _ITEMLOCATION {
/*0x0c*/	ItemContainerInstance Location;
/*0x10*/	short InvSlot;
/*0x12*/	short BagSlot;
/*0x14*/	short AugSlot;
/*0x16*/
} ITEMLOCATION, *PITEMLOCATION;

enum ItemSpellTypes
{
	eActivatableSpell,
	eProcSpell,
	eWornSpell,
	eFocusSpell,
	eScrollSpell,
	eFocus2Spell,
	eMountSpell,
	eIllusionSpell,
	eFamiliarSpell
};

typedef struct _LAUNCHSPELLDATA {
/*0x00*/ UINT    SpellETA;           //Calculated TimeStamp when current spell being cast will land. 0 while not casting.
/*0x04*/ int     SpellID;            // -1 = not casting a spell
/*0x08*/ BYTE    SpellSlot;          // 0xFF if not casting, otherwise it's the spell gem number (0 - 8)
/*0x09*/ BYTE    SpellLevel;
/*0x0a*/ BYTE     Padding0x9[0x2];
/*0x0c*/ struct   _ITEMLOCATION ItemLocation;
/*0x18*/ ItemSpellTypes ItemCastType;
/*0x1c*/ DWORD    ItemID;
/*0x20*/ FLOAT    CastingY;
/*0x24*/ FLOAT    CastingX;
/*0x28*/ BYTE     Unknown0x28[0x4];
/*0x2c*/ DWORD    TargetID;
/*0x30*/ bool    bDetrimental;
/*0x31*/ bool    bResetMeleeTimers;
/*0x32*/ bool    bResetAAOnNotTakeHold;
/*0x33*/ bool    bFreeTarget;
/*0x34*/ FLOAT   TargetPosY;
/*0x38*/ FLOAT   TargetPosX;
/*0x3c*/ FLOAT   TargetPosZ;
/*0x40*/ bool    bTwinCast;
/*0x41*/ bool    bLanded;
/*0x42*/ bool    bNPCTarget;
/*0x43*/ bool    bHasHitRecourse;
/*0x44*/ FLOAT   AnchorPosY;
/*0x48*/ FLOAT   AnchorPosX;
/*0x4c*/ FLOAT   AnchorPosZ;
/*0x50*/
} LAUNCHSPELLDATA, *PLAUNCHSPELLDATA;

template <class Type> class TListNode
{
	/*0x0004*/ Type *pPrev;
	/*0x0008*/ Type *pNext;
	/*0x000c*/ BYTE		Unknown0x000c[0xc];
};
class PlayerBaseVfTable
{
	/*0x0000*/ void*	vtable;
};
class PlayerBase : public PlayerBaseVfTable,public TListNode<PlayerBase>
{
public:

};

enum EActorType
{
	Undefined,
	Player,
	Corpse,
	Switch,
	Missile,
	Object,
	Ladder,
	Tree,
	Wall,
	PlacedObject
};

class CPhysicsInfo
{
public:
	/*0x00*/ FLOAT	Y;//Position
	/*0x04*/ FLOAT	X;
	/*0x08*/ FLOAT	Z;
	/*0x0c*/ FLOAT	SpeedY;//Velocity  
	/*0x10*/ FLOAT	SpeedX;
	/*0x14*/ FLOAT	SpeedZ;
	/*0x18*/ FLOAT	SpeedRun;
	/*0x1c*/ FLOAT	Heading;
	/*0x20*/ FLOAT	Angle;//Pitch
	/*0x24*/ FLOAT	AccelAngle;//PitchAccel
	/*0x28*/ FLOAT	SpeedHeading;//HeadingAccel
	/*0x2c*/ FLOAT	CameraAngle;//ViewPitch
	/*0x30*/
};
#define SPAWNINFOHEADER \
/*0x0000*/ void*	vtable; \
/*0x0004*/ struct _SPAWNINFO *pPrev; \
/*0x0008*/ struct _SPAWNINFO *pNext; \
/*0x000c*/ void	*List; \
/*0x0010*/

#define InnateETA 0xc
//we dont need a fully implemented version
//this does the job just fine
template <typename TheType,UINT _Len> class TCircularBuffer
{
public:
	TheType Type[_Len];
	UINT	Len;
	UINT	Index;
};
struct SDoCollisionMovementStats
{
/*0x00*/	CPhysicsInfo Source; /*size 0x30 */
/*0x30*/	FLOAT DestY;
/*0x34*/	FLOAT DestX;
/*0x38*/	FLOAT DestZ;
/*0x3c*/	FLOAT SourceFloor;
/*0x40*/	FLOAT DestFloor;
/*0x44*/	CPhysicsInfo Dest;
/*0x74*/
};

#define PLAYERZONECLIENT \
/*0x018C*/ BYTE		IsPassenger; /* if u are on a boat or airship or whatever */ \
/*0x0190*/ FLOAT	ViewHeight; \
/*0x0194*/ bool		bOfflineMode; \
/*0x0198*/ int		RealEstateID; \
/*0x019C*/ BYTE		TitleVisible; \
/*0x019D*/ bool		bStationary; \
/*0x019E*/ BYTE		CharClass; /* Im pretty sure this isnt working */ \
/*0x01A0*/ int		EnduranceCurrent; \
/*0x01A4*/ UINT		MinuteTimer; \
/*0x01A8*/ CHAR		LoginRelated[0x20]; \
/*0x01C8*/ CHAR		DraggingPlayer[0x40]; \
/*0x0208*/ bool		Mercenary; \
/*0x020C*/ FLOAT	FallingStartZ; \
/*0x0210*/ DWORD	Zone; \
/*0x0214*/ BYTE		Light; \
/*0x0218*/ UINT		LastSecondaryUseTime; \
/*0x021C*/ int		IsAttacking; /* need to investigate */ \
/*0x0220*/ FLOAT	BearingToTarget; \
/*0x0224*/ BYTE		GMRank; /* i think */ \
/*0x0228*/ FLOAT	AnimationSpeedRelated; \
/*0x022C*/ BYTE		Type2; /* check */ \
/*0x022D*/ bool		bBuffTimersOnHold; /* no you can't control this client side so unless you want to get banned leave it alone */ \
/*0x0230*/ UINT		LastRangedUsedTime; \
/*0x0234*/ void		*pRaceGenderInfo; /* todo this is not a void* so... map the CRaceGenderInfo class */ \
/*0x0238*/ BYTE		LastAttack; \
/*0x023C*/ UINT		LastPrimaryUseTime; \
/*0x0240*/ int		CurrIOState; \
/*0x0244*/ UINT		LastRefresh; \
/*0x0248*/ int		GuildStatus; \
/*0x024C*/ UINT		LastTick; \
/*0x0250*/ BYTE		Level; \
/*0x0251*/ bool		bAttackRelated; \
/*0x0254*/ DWORD	LoginSerial; \
/*0x0258*/ int		SomethingElse; \
/*0x025C*/ CHAR		GM; \
/*0x0260*/ UINT		FishingETA; \
/*0x0264*/ bool		Sneak; \
/*0x0265*/ bool		bAnimationOnPop; \
/*0x0268*/ UINT		LastResendAddPlayerPacket; \
/*0x026C*/ struct _SPAWNINFO*	pViewPlayer; \
/*0x0270*/ UINT		ACounter; /* not sure what this one is for*/ \
/*0x0274*/ FLOAT	CameraOffset; \
/*0x0278*/ DWORD	Meditating; \
/*0x027C*/ BYTE		FindBits; \
/*0x0280*/ int		SecondaryTintIndex; \
/*0x0284*/ CPhysicsInfo	LastCollision; \
/*0x02B4*/ int		PetID; \
/*0x02B8*/ int		Anon; /* found in EQPlayer__SetNameSpriteTint_x */ \
/*0x02BC*/ UINT		RespawnTimer; /* its actually TimeOfDeath...TimeStamp of when RespawnWnd will close - 0 when you're alive */ \
/*0x02C0*/ int		LastCastNum; \
/*0x02C4*/ int		AARank;/* this really is the title id */ \
/*0x02C8*/ DWORD	LastCastTime; \
/*0x02CC*/ struct _EQC_INFO*	spawneqc_info; \
/*0x02D0*/ FLOAT	MerchantGreed; \
/*0x02D4*/ void		*pTouchingSwitch; /* need to get that in here : struct _EQSWITCH */ \
/*0x02D8*/ UINT		TimeStamp; /* updates all the time including when on a mount */ \
/*0x02DC*/ int		HPMax; \
/*0x02E0*/ bool		bSummoned; /* by a PC or by anyone? need to investigate */ \
/*0x02E1*/ BYTE		InPvPArea; /* are we in a PvP area? */ \
/*0x02E4*/ int		HPCurrent; \
/*0x02E8*/ UINT		SpellGemETA[0xd]; /*InnateETA is now the last member in this array */ \
/*0x031C*/ FLOAT	GetMeleeRangeVar1; /* used by GetMeleeRange */ \
/*0x0320*/ int		Trader;	/* found in CEverQuest__RightClickedOnPlayer_x */ \
/*0x0324*/ int		SomeData[0x2]; \
/*0x032C*/ bool		bSwitchMoved; /* true when a door moves near the spawn */ \
/*0x0330*/ FLOAT	MyWalkSpeed; \
/*0x0334*/ BYTE		HoldingAnimation; /* todo: create enum for this byte. Holding: Nothing=0 A RightHand Weapon=1 A Shield=2 Dual Wielding Two Weapons=3 A Spear=4 A LeftHand Weapon=5 A Two Handed Weapon=6 A bow=7 */ \
/*0x0338*/ int		HideMode; \
/*0x033C*/ int		Stuff; \
/*0x0340*/ int		AltAttack; \
/*0x0344*/ DWORD	PotionTimer; \
/*0x0348*/ BYTE		HmmWhat; /* todo: figure out */ \
/*0x0349*/ bool		PvPFlag; \
/*0x034C*/ UINT	GuildID; \
/*0x0350*/ FLOAT	RunSpeed; /*0.70 on runspeed 5... */ \
/*0x0354*/ int		ManaMax; \
/*0x0358*/ UINT		LastMealTime; /* last time we eat or drank */ \
/*0x035C*/ BYTE		StandState; \
/*0x0360*/ UINT		LastTimeStoodStill; \
/*0x0364*/ int		DoSpecialMelee; \
/*0x0368*/ UINT		CombatSkillTicks[0x2]; \
/*0x0370*/ FLOAT	MissileRangeToTarget; \
/*0x0374*/ UINT		SitStartTime; \
/*0x0378*/ EqItemGuid 	realEstateItemGuid; \
/*0x038A*/ bool		bShowHelm; \
/*0x038C*/ UINT		MasterID; \
/*0x0390*/ UINT		CombatSkillUsed[0x2]; \
/*0x0398*/ CHAR		Handle[0x20]; \
/*0x03B8*/ BYTE		FD; \
/*0x03BC*/ int		PrimaryTintIndex; \
/*0x03C0*/ int		AFK; \
/*0x03C4*/ UINT		EnduranceMax; \
/*0x03C8*/ UINT		NextIntimidateTime; \
/*0x03CC*/ UINT		SpellCooldownETA; \
/*0x03D0*/ bool		Linkdead; \
/*0x03D4*/ int		Buyer; \
/*0x03D8*/ UINT		LastTrapDamageTime; \
/*0x03DC*/ bool		bTempPet; \
/*0x03DD*/ CHAR		Title[0x20]; \
/*0x03FD*/ BYTE		FishingEvent; \
/*0x03FE*/ CHAR		Suffix[0x20]; \
/*0x0420*/ int		RealEstateItemId; \
/*0x0424*/ int		WarCry; \
/*0x0428*/ BYTE		Blind; \
/*0x0429*/ bool		bBetaBuffed; \
/*0x042C*/ UINT		NextSwim; \
/*0x0430*/ UINT		CorpseDragCount; \
/*0x0434*/ BYTE		IntimidateCount; \
/*0x0438*/ int		berserker; \
/*0x043C*/ UINT		StunTimer; \
/*0x0440*/ bool		LFG; \
/*0x0444*/ struct _LAUNCHSPELLDATA	CastingData; /* size: 0x50 */ \
/*0x0494*/ int		ManaCurrent; \
/*0x0498*/ CHAR		DragNames[0x2][0x40]; \
/*0x0518*/ int		Deity; \
/*0x051C*/ int		NpcTintIndex; \
/*0x0520*/ DWORD	**ppUDP; /* UdpLibrary::UdpConnection? */ \
/*0x0524*/ bool		bAlwaysShowAura; \
/*0x0528*/

class CParticlePointInterface
{
	//we dont actually need this for now.
};
class CLightInterface
{
	//we dont actually need this for now.
};
class ActorAnimation
{
	//we dont actually need this for now
	//but it has members so todo: fill in.
};
template <typename TheType> class TList
{
public:
	TheType* pFirst;
	TheType* pLast;
};
class CObjectGroupStageInstance : public TListNode<CObjectGroupStageInstance>
{
public:
	void* pActor1;//CActorInterface
	void* pActor2;//CActorInterface
	void* pStage;//CObjectGroupStage
};
class CActorApplicationData
{

};
class ActorBase
{
public:
/*0x0ea4*/ void		*vfTableActorClient;
/*0x0ea8*/ CHAR		TextureType;
/*0x0ea9*/ CHAR		Material;
/*0x0eaa*/ CHAR		Variation;
/*0x0eab*/ CHAR		HeadType;
/*0x0eac*/ BYTE		FaceStyle;
/*0x0ead*/ BYTE		HairColor;
/*0x0eae*/ BYTE		FacialHairColor;
/*0x0eaf*/ BYTE		EyeColor1;
/*0x0eb0*/ BYTE		EyeColor2;
/*0x0eb1*/ BYTE		HairStyle;
/*0x0eb2*/ BYTE		FacialHair;
/*0x0eb4*/ signed int	Race;
/*0x0eb8*/ BYTE		Class;
/*0x0eb9*/ BYTE		Gender;//it really is at 0x0eb9
/*0x0eba*/ CHAR		ActorDef[0x40];
/*0x0efc*/ UINT		ArmorColor[0x9];
/*0x0f20*/ bool		bShowHelm;
/*0x0f24*/ int		Heritage;               //drakkin only face setting
/*0x0f28*/ int		Tattoo;                 //drakkin only face setting
/*0x0f2c*/ int		Details;                //drakkin only face setting
/*0x0f30*/ struct _EQUIPMENT	ActorEquipment;  //size 0xb4
/*0x0fe4*/
};
class ActorClient : public ActorBase
{
public:
//EQLIB_OBJECT class CVector3 const & ActorClient::GetPosition(void)const;
//EQLIB_OBJECT void ActorClient::GetPosition(class CVector3 *)const;
public:
/*0x0fe4*/ int		LeftEyeMaterialIndex;
/*0x0fe8*/ int		RightEyeMaterialIndex;
/*0x0fec*/ CParticlePointInterface* m_pParticlePoints[0xa];
/*0x1014*/ void* pLowerBones;
/*0x1018*/ void* pUpperBones;
/*0x101c*/ void*	pcactorex; // todo: move to ActorInterface*
/*0x1020*/ CLightInterface	*pLight;
/*0x1024*/ ActorAnimation	*pActorAnimation;
/*0x1028*/ TList<CObjectGroupStageInstance> StageInstances;//size 0x8
/*0x1030*/ bool		bActiveTransition;
/*0x1034*/ UINT		CurrentStage;
/*0x1038*/ FLOAT	ZOffset;
/*0x103c*/ FLOAT	TempY;//related to ZOffset adjustments I *think*
/*0x1040*/ FLOAT	TempX;
/*0x1044*/ FLOAT	TempZ;
/*0x1048*/ bool		bReplacedStaticObject;
/*0x104c*/ int		PartialFaceNumber;
/*0x1050*/ bool		bNewArmorDisabled;
/*0x1054*/ CActorApplicationData* pAppData;
/*0x1048*/
};
class PlayerAnimationBase
{
	//todo: add members and maybe a vftable
};
class CLineBase
{
public:
/*0x00*/	FLOAT OriginY;
/*0x04*/	FLOAT OriginX;
/*0x08*/	FLOAT OriginZ;
/*0x0c*/	FLOAT DirectionY;
/*0x10*/	FLOAT DirectionX;
/*0x14*/	FLOAT DirectionZ;
/*0x18*/
};
class CLineSegment : public CLineBase
{
	//no members here

};
class CCapsule : public CLineSegment
{
public:
/*0x18*/	FLOAT	Radius;
/*0x1c*/
};
struct PhysicsEffect
{
/*0x00*/ int SpellID;
/*0x04*/ int CasterID;
/*0x08*/ int EffectID;
/*0x0c*/ int BaseEffect;
/*0x10*/
};

enum InvisibleTypes
{
	eAll,
	eUndead,
	eAnimal
};

enum GravityBehavior
{
	Ground,
	Flying,
	Levitating,
	Water,
	Floating,//boat
	LevitateWhileRunning
};
class PhysicsBase
{
public:	
/*0x1fA4*/ void*	vtable2;
/*0x1FA8*/	bool bApplyGravity;
};
class PlayerPhysics : public PhysicsBase
{
public:
/*0x1FAC*/	struct _SPAWNINFO*	pSpawn;//PlayerBase*
/*0x1FB0*/	GravityBehavior Levitate;
//bobbing control
/*0x1Fb4*/	int UpDownIndex;
/*0x1Fb8*/	int LeftRightIndex;
/*0x1Fbc*/	int UpDownSpeed;
/*0x1Fc0*/	int LeftRightSpeed;
/*0x1Fc4*/	FLOAT LeftRightDist;
/*0x1Fc8*/	FLOAT UpDownDist;
/*0x1Fcc*/
};
class PlayerPhysicsClient : public PlayerPhysics
{
public:
/*0x1Fb8*/
};

//this is the size of EQPlayer__EQPlayer_x
//(note to self:Sneak not in usual place in 2014-01-21)
//(note to self:Standstate not in usual place in 2014-06-19)
// actual size 0x1F08 in May 10 2013 LIVE (see 0x599BE8) - eqmule
typedef struct _SPAWNINFO {
	SPAWNINFOHEADER
/* ******************** PlayerBase Starts Here ***************** */
/*0x0010*/ FLOAT	JumpStrength;
/*0x0014*/ FLOAT	SwimStrength;
/*0x0018*/ FLOAT	SpeedMultiplier;
/*0x001c*/ FLOAT	AreaFriction;
/*0x0020*/ FLOAT	AccelerationFriction;
/*0x0024*/ EActorType CollidingType; /* ok finally had time to get this one right, when we collide with something this gets set. */
/*0x0028*/ FLOAT	FloorHeight;
/*0x002c*/ bool		bSinksInWater;
/*0x0030*/ UINT		PlayerTimeStamp; /* doesn't update when on a Vehicle (mounts/boats etc) */
/*0x0034*/ UINT		LastTimeIdle;
/*0x0038*/ CHAR		Lastname[0x20];
/*0x0058*/ FLOAT	AreaHPRegenMod; /*from guild hall pools etc. */
/*0x005c*/ FLOAT	AreaEndRegenMod;
/*0x0060*/ FLOAT	AreaManaRegenMod;
/*0x0064*/ FLOAT	Y;
/*0x0068*/ FLOAT	X;
/*0x006c*/ FLOAT	Z;
/*0x0070*/ FLOAT	SpeedY;
/*0x0074*/ FLOAT	SpeedX;
/*0x0078*/ FLOAT	SpeedZ;
/*0x007c*/ FLOAT	SpeedRun;
/*0x0080*/ FLOAT	Heading;
/*0x0084*/ FLOAT	Angle;
/*0x0088*/ FLOAT	AccelAngle;
/*0x008c*/ FLOAT	SpeedHeading;
/*0x0090*/ FLOAT	CameraAngle;
/*0x0094*/ UINT		UnderWater; /*LastHeadEnvironmentType */
/*0x0098*/ UINT		LastBodyEnvironmentType;
/*0x009c*/ UINT		LastFeetEnvironmentType;
/*0x00a0*/ BYTE		HeadWet; /*these really are environment related, like lava as well for example */
/*0x00a1*/ BYTE		FeetWet;
/*0x00a2*/ BYTE		BodyWet;
/*0x00a3*/ BYTE		LastBodyWet;
/*0x00a4*/ CHAR		Name[0x40];             /* ie priest_of_discord00 */
/*0x00e4*/ CHAR		DisplayedName[0x40];    /* ie Priest of Discord*/
/*0x0124*/ BYTE		PossiblyStuck;          /* never seen this be 1 so maybe it was used a a point but not now... */
/*0x0125*/ BYTE		Type;
/*0x0128*/ DWORD**	BodyType;	/* this really should be renamed to charprops or something its broken anyway*/
/*0x012c*/ BYTE		CharPropFiller[0xc]; /* well since the above is a CharacterPropertyHash we have to pad here...*/
/*0x0138*/ FLOAT	AvatarHeight;           /* height of avatar from groundwhen standing*/
/*0x013c*/ FLOAT	Height;
/*0x0140*/ FLOAT	Width;
/*0x0144*/ FLOAT	Length;
/*0x0148*/ DWORD	SpawnID;
/*0x014c*/ DWORD	PlayerState;         /* 0=Idle 1=Open 2=WeaponSheathed 4=Aggressive 8=ForcedAggressive 0x10=InstrumentEquipped 0x20=Stunned 0x40=PrimaryWeaponEquipped 0x80=SecondaryWeaponEquipped */
/*0x0150*/ struct _SPAWNINFO*	Vehicle;    /* NULL until you collide with a vehicle (boat,airship etc) */
/*0x0154*/ struct _SPAWNINFO*	Mount;      /* NULL if no mount present */
/*0x0158*/ struct _SPAWNINFO*	Rider;      /* _SPAWNINFO of mount's rider */
/*0x015c*/ DWORD	Unknown0x015c;
/*0x0160*/ bool		Targetable;	/* true if mob is targetable */
/*0x0161*/ bool		bTargetCyclable;
/*0x0162*/ bool		bClickThrough;
/*0x0163*/ bool		bBeingFlung;
/*0x0164*/ UINT		FlingActiveTimer;
/*0x0168*/ UINT		FlingTimerStart;
/*0x016c*/ bool		bFlingSomething;
/*0x0170*/ FLOAT	FlingY;
/*0x0174*/ FLOAT	FlingX;
/*0x0178*/ FLOAT	FlingZ;
/*0x017c*/ bool		bFlingSnapToDest;
/*0x0180*/ int		SplineID;
/*0x0184*/ int		SplineRiderID;
/* *********************** PlayerBase Ends Here ********************** */
/* ******************** PlayerZoneClient Starts Here ***************** */
/*0x0188*/ UINT		LastIntimidateUse;
/*0x018c*/ PLAYERZONECLIENT
/*0x0528*/
/*0x0528*/ TCircularBuffer<SDoCollisionMovementStats, 0x14>MovementStats; /* size (0x74 * 0x14) +8 = 0x918 */ \
/*0x0e40*/ struct _SPAWNINFO*   WhoFollowing; // NULL if autofollow off
/*0x0e44*/ DWORD        GroupAssistNPC[0x1];
/*0x0e48*/ DWORD        RaidAssistNPC[0x3];
/*0x0e54*/ DWORD        GroupMarkNPC[0x3];
/*0x0e60*/ DWORD        RaidMarkNPC[0x3];
/*0x0e6c*/ DWORD        TargetOfTarget;
/*0x0e70*/ BYTE         Unknown0x0e70[0x34];
/*0x0ea4*/ ActorClient	mActorClient;          // start of ActorClient struct
/*0x1048*/
/*0x1048*/ PlayerAnimationBase *pAnimation;
/*0x105c*/ FLOAT        MeleeRadius;      // used by GetMeleeRange
/*0x1060*/ UINT			CollisionCounter;
/*0x1064*/ FLOAT		CachedFloorLocationY;
/*0x1068*/ FLOAT		CachedFloorLocationX;
/*0x106c*/ FLOAT		CachedFloorLocationZ;
/*0x1070*/ FLOAT		CachedFloorHeight;
/*0x1074*/ FLOAT		CachedCeilingLocationY;
/*0x1078*/ FLOAT		CachedCeilingLocationX;
/*0x107c*/ FLOAT		CachedCeilingLocationZ;
/*0x1080*/ FLOAT		CachedCeilingHeight;
/*0x1084*/ CCapsule		StaticCollision;//size 0x1c
/*0x10a0*/ ArrayClass_RO<PhysicsEffect> mPhysicsEffects;//size is 0x10
/*0x10b0*/ ArrayClass_RO<bool> PhysicsEffectsUpdated;//size is 0x10
/* ********************* PlayerZoneClient Ends Here ******************* */
/* ********************** PlayerClient Starts Here ******************** */
/*0x10c0*/ int		Animation;
/*0x10c4*/ int		NextAnim;
/*0x10c8*/ int		CurrLowerBodyAnim;
/*0x10cc*/ int		NextLowerBodyAnim;
/*0x10d0*/ int		CurrLowerAnimVariation;
/*0x10d4*/ int		CurrAnimVariation;
/*0x10d8*/ int		CurrAnimRndVariation;
/* ********************sound ID's BEGIN ******************* */
/*0x10dc*/ int		Loop3d_SoundID;

/*0x10e0*/ int		Step_SoundID;;
/*0x10e4*/ int		CurLoop_SoundID;
/*0x10e8*/ int		Idle3d1_SoundID;
/*0x10f0*/ int		Idle3d2_SoundID;
/*0x10f4*/ int		Jump_SoundID;
/*0x10f8*/ int		Hit1_SoundID;
/*0x10fc*/ int		Hit2_SoundID;
/*0x1100*/ int		Hit3_SoundID;

/*0x1104*/ int		Hit4_SoundID;
/*0x1108*/ int		Gasp1_SoundID;
/*0x110c*/ int		Gasp2_SoundID;
/*0x1110*/ int		Drown_SoundID;
/*0x1114*/ int		Death_SoundID;
/*0x1118*/ int		Attk1_SoundID;
/*0x111c*/ int		Attk2_SoundID;
/*0x1120*/ int		Attk3_SoundID;

/*0x1124*/ int		Walk_SoundID;
/*0x1128*/ int		Run_SoundID;
/*0x112c*/ int		Crouch_SoundID;
/*0x1130*/ int		Swim_SoundID;
/*0x1134*/ int		TreadWater_SoundID;
/*0x1138*/ int		Climb_SoundID;
/*0x113c*/ int		Sit_SoundID;
/*0x1140*/ int		Kick_SoundID;

/*0x1140*/ int		Bash_SoundID;
/*0x1144*/ int		FireBow_SoundID;
/*0x1148*/ int		MonkAttack1_SoundID;
/*0x114c*/ int		MonkAttack2_SoundID;
/*0x1150*/ int		MonkSpecial_SoundID;
/*0x1154*/ int		PrimaryBlunt_SoundID;
/*0x1158*/ int		PrimarySlash_SoundID;
/*0x115c*/ int		PrimaryStab_SoundID;

/*0x1160*/ int		Punch_SoundID;
/*0x1164*/ int		Roundhouse_SoundID;
/*0x1168*/ int		SecondaryBlunt_SoundID;
/*0x116c*/ int		SecondarySlash_SoundID;
/*0x1170*/ int		SecondaryStab_SoundID;
/*0x1174*/ int		SwimAttack_SoundID;
/*0x1178*/ int		TwoHandedBlunt_SoundID;
/*0x117c*/ int		TwoHandedSlash_SoundID;

/*0x1180*/ int		TwoHandedStab_SoundID;
/*0x1184*/ int		SecondaryPunch_SoundID;
/*0x1188*/ int		JumpAcross_SoundID;
/*0x118c*/ int		WalkBackwards_SoundID;
/*0x1190*/ int		CrouchWalk_SoundID;
/* ******************** sound ID's END ****************** */
/*0x1194*/ int			LastWalkTime;
/*0x1198*/ //int			ShipRelated;
/*0x119c*/ int			RightHolding; //119c see 514929
/*0x11a0*/ DWORD        LeftHolding; //1 holding 0 not holding 11a0 see 514946
/*0x11a8*/ UINT			LastBubblesTime;
/*0x11ac*/ UINT			LastColdBreathTime;
/*0x11b4*/ UINT			LastParticleUpdateTime;
/*0x11b8*/ DWORD        MercID;                         //if the spawn is player and has a merc up this is it's spawn ID -eqmule 16 jul 2014
/*0x11b0*/ DWORD        ContractorID;           //if the spawn is a merc this is its contractor's spawn ID -eqmule 16 jul 2014
/*0x11b4*/ FLOAT		CeilingHeightAtCurrLocation;
/*0x11b8*/ void			*MobileEmitter;//todo: change and map to EqMobileEmitter*
/*0x11bc*/ bool			bInstantHPGaugeChange;
/*0x11c0*/ UINT			LastUpdateReceivedTime;
/*0x11c4*/ FLOAT		MaxSpeakDistance;
/*0x11c8*/ FLOAT        WalkSpeed;
/*0x11cc*/ BYTE         Unknown0x11cc[0x4];
/*0x11d0*/ CHAR         AssistName[0x40];
/*0x1210*/ BYTE         InvitedToGroup;
/*0x1211*/ BYTE         Unknown0x1211[0x3];
/*0x1214*/ DWORD        GroupMemberTargeted;    // 0xFFFFFFFF if no target, else 1 through 5
/*0x1218*/ bool			bRemovalPending;
/*0x121c*/ void			*pCorpse;//look into 0x12f4 for sure!
/*0x1220*/ FLOAT		EmitterScalingRadius;//0x12f8 FOR SURE
/*0x1224*/ int			DefaultEmitterID;
/*0x1228*/ bool			bDisplayNameSprite;
/*0x1229*/ bool			bIdleAnimationOff;
/*0x122a*/ bool			bIsInteractiveObject;
/*0x122b*/ BYTE			InteractiveObjectModelName[0x80];
/*0x12ab*/ BYTE			InteractiveObjectOtherName[0x80];
/*0x132b*/ BYTE			InteractiveObjectName[0x40];
/*0x136b*/
/*0x136c*/ CPhysicsInfo PhysicsBeforeLastPort;//size IS /*0x30*/
/*0x139c*/ struct _FELLOWSHIPINFO       Fellowship; // size 0x9e4
/*0x1d80*/ FLOAT        CampfireY;
/*0x1d84*/ FLOAT        CampfireX;
/*0x1d88*/ FLOAT        CampfireZ;
/*0x1d8c*/ int          CampfireZoneID;         // zone ID where campfire is
/*0x1d90*/ DWORD        CampfireTimestamp;      // CampfireTimestamp-FastTime()=time left on campfire
/*0x1d94*/ int          FellowShipID;
/*0x1d94*/ int          CampType;
/*0x1d9c*/ DWORD        Campfire;               // do we have a campfire up?
/*0x1da0*/ struct _EQUIPMENT	Equipment;// size 0xb4 (0x1da0 for sure see 59090E)
/*0x1e54*/ bool		bIsPlacingItem;
/*0x1e55*/ bool		bGMCreatedNPC;
/*0x1e58*/ int		ObjectAnimationID;
/*0x1e5c*/ bool		bInteractiveObjectCollidable;
/*0x1e60*/ int		InteractiveObjectType;
/*0x1e64*/ int		SoundIDs[0xa];//0x28 bytes (for sure 1E64 see 58B832)
/*0x1e8c*/ UINT		LastHistorySentTime;//for sure 1e8c see 5909C7
/*0x1e90*/ ArrayClass2_RO<UINT>	BardTwistSpells;//size 0x1c
/*0x1eac*/ UINT		CurrentBardTwistIndex;
/*0x1eb0*/ PlayerPhysicsClient mPlayerPhysicsClient;//size 0x28 for sure at 1EB0 see 597FE0
/*0x1ed8*/ int		SpawnStatus[6];//todo: look closer at these i think they can show like status of mobs slowed, mezzed etc, but not sure
/*0x1ef0*/ int		BannerIndex0;//guild banners
/*0x1ef4*/ int		BannerIndex1;
/*0x1ef8*/ ARGBCOLOR BannerTint0;
/*0x1efc*/ ARGBCOLOR BannerTint1;
/*0x1f00*/ int		MountAnimationRelated;
/*0x1f04*/ bool		bGuildShowAnim;//or sprite? need to check
/*0x1f05*/ bool		bWaitingForPort;//check this
/* ********************** PlayerClient Ends Here ******************** */
/*0x1f08*/ //see SpawnInfoSize

} SPAWNINFO, *PSPAWNINFO;

#define STANDSTATE_STAND                0x64
#define STANDSTATE_CASTING              0x66
#define STANDSTATE_BIND                 0x69
#define STANDSTATE_SIT                  0x6E
#define STANDSTATE_DUCK                 0x6F
#define STANDSTATE_FEIGN                0x73
#define STANDSTATE_DEAD                 0x78

#define MONITOR_SPAWN_X                 1
#define MONITOR_SPAWN_Y                 2
#define MONITOR_SPAWN_Z                 4
#define MONITOR_SPAWN_HEADING           8
#define MONITOR_SPAWN_SPEEDRUN          16
#define MONITOR_SPAWN_HPCURRENT         32

typedef struct _SPAWNMONITORINFO {
    WORD SpawnID;
    FLOAT Y;
    FLOAT X;
    FLOAT Z;
    FLOAT Heading;
    FLOAT SpeedRun;
    DWORD HPCurrent;
    DWORD MonitorMask;
} SPAWNMONITORINFO, *PSPAWNMONITORINFO;

typedef struct _HASHENTRY {
    SPAWNINFO   *spawn;
    DWORD       key;            // same as SpawnID for spawns
    struct      _HASHENTRY *next;
} HASHENTRY, *PHASHENTRY;

typedef struct _HASHTABLE {
    struct _HASHENTRY **table;
    int size;
} HASHTABLE, *PHASHTABLE;

typedef struct _SPAWNMANAGER {
/*0x000*/ void          *vftable;
/*0x004*/ DWORD         random; // rand()%20000
/*0x008*/ struct  _SPAWNINFO        *FirstSpawn;
/*0x00c*/ struct  _SPAWNINFO        *LastSpawn;
/*0x010*/ DWORD         unknown10;
/*0x014*/ struct        _HASHTABLE  *SPHash;
} SPAWNMANAGER, *PSPAWNMANAGER;


#if 0

void *gethashedentry(struct _HASHTABLE  *table, DWORD key)
{
    PHASHENTRY p = NULL;
    p = table->array[key%table->size];

    while(p) {
        if (p->key == key) return p;
        p = p->next;
    }
    return NULL;
}

#endif
struct Matrix4x4
{
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;
		};
		float m[4][4];
	};
};
//eqmule oct 31 2013
typedef struct _SWITCHCLICK
{
	FLOAT Y;
	FLOAT X;
	FLOAT Z;
	FLOAT Y1;
	FLOAT X1;
	FLOAT Z1;
} SWITCHCLICK,*PSWITCHCLICK;

enum eMemPoolType
{
	eGlobal,
	eOnDemand,
	eClearOnZone,
};
// this is actually ActorInterface
// actual size: 0x120 3-3-2009
// semi corrected on dec 16 2013 eqmule
// i *think* the size is 0x190
//however i couldnt confirm from 0x38 to 0x114
//more work is needed... anyone feel free to step up...

//updated on dec 16 2015 by brainiac
typedef struct _EQSWITCH {
/*0x00*/    void*		vfTable;
/*0x04*/    eMemPoolType  MemType;
/*0x08*/    bool        bIsS3DCreated;
/*0x09*/    bool        bHasParentBone;
/*0x0a*/    bool        bUpdateScaledAmbient;
/*0x0c*/    float		ScaledAmbient;
/*0x10*/    float		ScaledAmbientTarget;
/*0x14*/    float		ParticleScaleFactor;
/*0x18*/    float		CollisionSphereScaleFactor;
/*0x1c*/    UINT        UpdateAmbientTick;
/*0x20*/    UINT        InterpolateAmbientTick;
/*0x24*/    void*		pParentActor;//its a  CActor*
/*0x28*/    void*       pDPVSObject;
/*0x2C*/    FLOAT		Y;
/*0x30*/    FLOAT		X;
/*0x34*/    FLOAT		Z;
/*0x38*/    FLOAT		SurfaceNormalY;
/*0x3c*/    FLOAT		SurfaceNormalX;
/*0x40*/    FLOAT		SurfaceNormalZ;
/*0x44*/    UINT        VisibleIndex;
/*0x48*/    FLOAT       Alpha;
/*0x4c*/    bool	    bCastShadow;
/*0x4d*/    bool	    bNeverClip;
/*0x4e*/    bool	    bClientCreated;
/*0x50*/    FLOAT       ZOffset;
/*0x54*/    FLOAT       EmitterScalingRadius;
/*0x58*/    void*		pDuplicateActor;//its a  CActor*
/*0x5c*/    bool	    bShowParticlesWhenInvisible;
/*0x60*/    void*       pAreaPortalVolumeList;//CAreaPortalVolumeList*
/*0x64*/    void*       CleanupList;//a TListNode<CActor*>? not sure
/*0x68*/    BYTE        CleanupListFiller[0xc];
/*0x74*/    void*       pActorApplicationData;//CActorApplicationData* 74 for sure see 1003AE70
/*0x78*/    EActorType  ActorType;
/*0x7c*/    void*       pTerrainObject;//CTerrainObject*
/*0x80*/    void*       HighlightData;//HighlightData*
/*0x84*/    float        yAdjustment1;//from this point on im not sure -eqmule 2013 dec 16
/*0x88*/    float        xAdjustment1;
/*0x8c*/    float        zAdjustment1;
/*0x90*/    float        headingAdjustment1;
/*0x94*/    float        yAdjustment2;
/*0x98*/    float        xAdjustment2;
/*0x9c*/    float        zAdjustment2;
/*0xa0*/    float        headingAdjustment2;
/*0xa4*/    float        yAdjustment3;
/*0xa8*/    float        xAdjustment3;
/*0xac*/    float        zAdjustment3;
/*0xb0*/    float        headingAdjustment3;
/*0xb4*/    float        adjustments4[3];
/*0xc0*/    float        adjustments5[4];
/*0xd0*/    bool        bbHasAttachSRT;
/*0xd1*/    bool        bDisableDesignOverride;
/*0xd4*/    int         Unknown0x1d4[4];
/*0xe4*/    Matrix4x4   transformMatrix;//used for new armor
/*0x128*/   FLOAT        Heading;
/*0x12c*/   BYTE         Unknown0x12c[0x14];
/*0x140*/   float        HeightAdjustment;//this is most likely wrong dec 16 2013 eqmule
/*0x144*/   BYTE         Unknown0x144[0x4c];
/*0x190*/
} EQSWITCH, *PEQSWITCH;

//Size is 0xe0 see 54933E in dec 13 2016 live - eqmule
typedef struct _DOOR {
/*0x00*/ void  *vtable;
/*0x04*/ BYTE  ObjType;          // always 5
/*0x05*/ BYTE  ID;
/*0x06*/ CHAR  Name[0x20];
/*0x26*/ BYTE  Type;
/*0x27*/ BYTE  State;               // 0 = closed, 1 = open, 2 = opening, 3 = closing
/*0x28*/ FLOAT DefaultY;
/*0x2c*/ FLOAT DefaultX;
/*0x30*/ FLOAT DefaultZ;
/*0x34*/ FLOAT DefaultHeading;
/*0x38*/ FLOAT DefaultDoorAngle;
/*0x3c*/ FLOAT TopSpeed1;
/*0x40*/ FLOAT TopSpeed2;
/*0x44*/ FLOAT Y;
/*0x48*/ FLOAT X;
/*0x4c*/ FLOAT Z;
/*0x50*/ FLOAT Heading;
/*0x54*/ FLOAT DoorAngle;
/*0x58*/ BYTE  DefaultState;
/*0x59*/ BYTE  SelfActivated;
/*0x5a*/ BYTE  Dependent;
/*0x5b*/ bool  bTemplate;
/*0x5c*/ BYTE  Difficulty;//pick/disarm...
/*0x5d*/ BYTE  AffectSlots[5];
/*0x62*/ BYTE  CurrentCombination[5];
/*0x67*/ BYTE  ReqCombination[5];
/*0x6c*/ BYTE  RandomCombo;
/*0x70*/ int   Key;
/*0x74*/ SHORT ScaleFactor;         // divide by 100 to get scale multiplier
/*0x78*/ int SpellID;
/*0x7c*/ BYTE  TargetID[0x5];
/*0x81*/ CHAR  Script[0x20];
/*0xa4*/ PEQSWITCH pSwitch;         // (CActorInterface*)
/*0xa8*/ void  *particle;       // (CParticleCloudInterface*)
/*0xac*/ DWORD TimeStamp;//last time UseSwitch
/*0xb0*/ FLOAT Accel;
/*0xb4*/ BYTE  AlwaysActive;
/*0xb8*/ int   AdventureDoorID;
/*0xbc*/ FLOAT ReturnY;
/*0xc0*/ FLOAT ReturnX;
/*0xc4*/ FLOAT ReturnZ;
/*0xc8*/ int   DynDoorID;
/*0xcc*/ bool  bHasScript;
/*0xd0*/ int   SomeID;
/*0xd4*/ bool  bUsable;
/*0xd5*/ bool  bRemainOpen;
/*0xd6*/ bool  bVisible;
/*0xd7*/ bool  bHeadingChanged;
/*0xd8*/ bool  bAllowCorpseDrag;
/*0xdc*/ int   RealEstateDoorID;
/*0xe0*/
} DOOR, *PDOOR; 

// 7-21-2003    Stargazer
typedef struct _DOORTABLE {
/*0x000*/   DWORD NumEntries;
/*0x004*/   PDOOR pDoor[0x0FF];
/*0x400*/
} DOORTABLE, *PDOORTABLE;

// actual size 0x80 20101012 - ieatacid
typedef struct _GROUNDITEM {
/*0x00*/ struct _GROUNDITEM *pPrev;
/*0x04*/ struct _GROUNDITEM *pNext;
/*0x08*/ int ID;
/*0x0c*/ DWORD  DropID;
/*0x10*/ DWORD  ZoneID;
/*0x14*/ DWORD  DropSubID;//well zonefile id, but yeah...
/*0x18*/ PEQSWITCH pSwitch; // (class EQSwitch *)
/*0x1c*/ CHAR   Name[0x40];
/*0x5c*/ long	Expires;
/*0x60*/ FLOAT  Heading;
/*0x64*/ FLOAT	Pitch;
/*0x68*/ FLOAT	Roll;
/*0x6c*/ FLOAT	Scale;
/*0x70*/ FLOAT  Y;
/*0x74*/ FLOAT  X;
/*0x78*/ FLOAT  Z;
/*0x7c*/ int   Weight;//-1 means it can't be picked up
/*0x80*/
} GROUNDITEM, *PGROUNDITEM;

#define   MAX_ZONES                     0x3e8
extern    PCHAR szZoneExpansionName[];     //defined in LibEQ_Utilities.cpp

//I looked into this one on dec 27 2013
//I confirmed it all the way to 0x198 see (7F9735 and 7F9D50) in eqgame.exe dated 10 dec 2013
// however... ZoneFlags seems to be 0 for a lot of zones...
// more investigation is needed
// im creating a new tlo member for ZoneFlags
// so I can continue testing...
typedef struct _ZONELIST { 
/*0x000*/   DWORD   Header; 
/*0x004*/   DWORD   Unknown0x4;         //pointer to something? 
/*0x008*/   DWORD   Expansion;          // szZoneExpansionName[] 
/*0x00c*/   WORD    Id; 
/*0x00e*/   WORD    Instance; 
/*0x010*/   CHAR    ShortName[0x81]; 
/*0x091*/   CHAR    LongName[0x103]; 
/*0x194*/   DWORD   Unknown0x194; 
/*0x198*/   DWORD   ZoneFlags;              // (Flags & 0x100000) = HasMinLevel, 0x4000 no air, 0x2 newbie zone, 0x20 no bind, 0x400000 something, 0x80000000 guild hall
/*0x19c*/   DWORD   Unknown0x19c; 
/*0x1a0*/   DWORD   Id2;                // This is Id+2242 
/*0x1a4*/   DWORD   PoPValue;           // This has something to do with PoP zones. 
/*0x1a8*/   DWORD   MinLevel;           // Minimum level to access 
/*0x1ac*/   WORD    Unknown0x1ac;          
/*0x1ae*/   BYTE    Unknown0x1ae[0x6];    
/*0x1b4*/   DWORD   Unknown0x1b4; 
/*0x1b8*/   DWORD   Unknown0x1b8; 
/*0x1bc*/   DWORD   Unknown0x1bc; 
/*0x1c0      next zone in list*/ 
} ZONELIST, *PZONELIST;

typedef struct _WORLDDATA {
/*0x000*/ PVOID lpvTable;
/*0x004*/ BYTE Hour;
/*0x005*/ BYTE Minute;
/*0x006*/ BYTE Day;
/*0x007*/ BYTE Month;
/*0x008*/ DWORD Year;
/*0x00C*/ BYTE Unknown0x00c[0x14];
/*0x020*/ PZONELIST ZoneArray[MAX_ZONES];
} WORLDDATA, *PWORLDDATA;
enum EOutDoor : BYTE
{
	IndoorDungeon,		// Zones without sky SolB for example.
	Outdoor,			// Zones with sky like Commonlands for example.
	OutdoorCity,		// A Player City with sky Plane of Knowledge for example.
	DungeonCity,		// A Player City without sky Ak'anon for example.
	IndoorCity,			// A Player City without sky Erudin for example.
	OutdoorDungeon,		// Dungeons with sky like Blackburrow for example.
};
enum EPlace
{
	CannotPlace,
	CanOnlyPlace,
	CanPlaceAndGoto,
};
typedef struct _ZONEINFO {
/*0x000*/   CHAR    CharacterName[0x40];
/*0x040*/   CHAR    ShortName[0x80];
/*0x0c0*/   CHAR    LongName[0x80];
/*0x140*/   CHAR    ZoneDesc[0x5][0x1e];  //zone description strings
/*0x1d6*/   BYTE    FogOnOff; // (usually FF)
/*0x1d7*/   ARGBCOLOR FogRed;
/*0x1db*/   ARGBCOLOR FogGreen;
/*0x1df*/   ARGBCOLOR FogBlue;
/*0x1e4*/   FLOAT   FogStart[0x4]; //fog distance
/*0x1f4*/   FLOAT   FogEnd[0x4];
/*0x204*/   FLOAT   ZoneGravity;
/*0x208*/   EOutDoor    OutDoor;//this is what we want instead of ZoneType, see the enum
/*0x209*/   BYTE    Unknown0x209[0x3];
/*0x20c*/   BYTE    Unknown0x20c[0x2e];
/*0x23a*/   BYTE    SkyType;   
/*0x23b*/   BYTE    Unknown0x23b[0xd];
/*0x248*/   FLOAT   ZoneExpModifier;    //This has been nerfed ..now reads 1.0 for all zones
/*0x24c*/   FLOAT   SafeYLoc;
/*0x250*/   FLOAT   SafeXLoc;
/*0x254*/   FLOAT   SafeZLoc;
/*0x258*/   BYTE    Unknown0x258[0x4];
/*0x25c*/   FLOAT   Ceiling;
/*0x260*/   FLOAT   Floor;
/*0x264*/   FLOAT   MinClip;
/*0x268*/   FLOAT   MaxClip;
/*0x26c*/   BYTE    Unknown0x26c[0x18];
/*0x284*/   BYTE    Unknown0x284[0x20];
/*0x2a4*/
} ZONEINFO, *PZONEINFO;

#define   TOTAL_SPELL_COUNT             0xAFC8      // # of spells allocated in memory (05/10/2013) -eqmule
#define   SPELLMGR_FILLER				0x2C180
//this is actually the size of the struct thats populated from aSpells_S_txt
//SpellManager__SpellManager_x
// size: 0x0AFEEC 20130510 - eqmule
typedef struct _SPELLMGR {
/*0x00000*/	BYTE			Unknown0x00000[SPELLMGR_FILLER];
/*0x2C180*/	struct _SPELL*  Spells[TOTAL_SPELL_COUNT];
/*0x580A0*/	BYTE			Unknown0x580A0[0x57E4C];
/*0xAFEEC*/
} SPELLMGR, *PSPELLMGR;

// actual size: 0x4ed 20120316 - ieatacid
// confirmed in mar 31 2014 eqgame - eqmule
// EQ_Spell__EQ_Spell_x
typedef struct _SPELL {
/*0x000*/   FLOAT   Range;
/*0x004*/   FLOAT   AERange;
/*0x008*/   FLOAT   PushBack;
/*0x00c*/   FLOAT   Unknown0x00c;
/*0x010*/   DWORD   CastTime;
/*0x014*/   DWORD   FizzleTime;
/*0x018*/   DWORD   RecastTime;
/*0x01c*/   DWORD   DurationType;       //DurationFormula on Lucy
/*0x020*/   DWORD   DurationValue1;
/*0x024*/   BYTE    Unknown0x24[0x4];
/*0x028*/   DWORD   Mana;
/*0x02c*/   LONG    Base[0x0c];         //Base1-Base12
/*0x05c*/   LONG    Base2[0x0c];        //SpellID of spell for added effects
/*0x08c*/   LONG    Max[0xc];           //Max1-Max12
/*0x0bc*/   DWORD   ReagentId[0x4];     //ReagentId1-ReagentId4d
/*0x0cc*/   DWORD   ReagentCount[0x4];  //ReagentCount1-ReagentCount4
/*0x0dc*/   BYTE    Unknown0xdc[0x10];
/*0x0ec*/   LONG    Calc[0x0c];         //Calc1-Calc12
/*0x11c*/   LONG    Attrib[0xc];       //Attrib1-Attrib12
/*0x14c*/   DWORD   BookIcon;
/*0x150*/   DWORD   GemIcon;
/*0x154*/   DWORD   DescriptionNumber;
/*0x158*/   DWORD   ResistAdj;
/*0x15c*/   DWORD    Diety;
/*0x160*/   DWORD   spaindex;
/*0x164*/   DWORD   SpellAnim;
/*0x168*/   DWORD   SpellIcon;
/*0x16c*/   DWORD   DurationParticleEffect;
/*0x170*/   DWORD   NPCUsefulness;
/*0x174*/   DWORD   ID;
/*0x178*/   DWORD   Autocast;         //SpellID of spell to instacast on caster when current spell lands on target
/*0x17c*/   DWORD   Category;         //Unknown144 from lucy
/*0x180*/   DWORD   Subcategory;      //Subcat to Category. Unknown145 from lucy
/*0x184*/   DWORD   Subcategory2;
/*0x188*/   DWORD   HateMod;          //Additional hate
/*0x18c*/   DWORD   ResistPerLevel;
/*0x190*/   DWORD   ResistCap;
/*0x194*/   DWORD   EnduranceCost;      //CA Endurance Cost
/*0x198*/   DWORD   CARecastTimerID;    //ID of combat timer, i think.
/*0x19c*/   DWORD   EndurUpkeep;
/*0x1a0*/   DWORD   HateGenerated;
/*0x1a4*/   DWORD   HitCountType;
/*0x1a8*/   DWORD   HitCount;
/*0x1ac*/   DWORD   ConeStartAngle;
/*0x0b0*/   DWORD   ConeEndAngle;
/*0x1b4*/   DWORD   PvPResistBase;
/*0x1b8*/   DWORD   PvPCalc;
/*0x1bc*/   DWORD   PvPResistCap;
/*0x1c0*/   DWORD   PvPDuration;        //DurationType for PVP
/*0x1c4*/   DWORD   PvPDurationValue1;  //DurationValue1 for PVP
/*0x1c8*/   DWORD   GlobalGroup;
/*0x1cc*/   DWORD   PCNPCOnlyFlag;      // no idea
/*0x1d0*/   DWORD   NPCMemCategory;
/*0x1d4*/   DWORD   SpellGroup;
/*0x1d8*/   DWORD   SubSpellGroup;		//unknown237 on Lucy it is checked at 0x76FE18 in jun 11 2014 and if 0 will ask if we want to replace our spell with a rk. x version
/*0x1dc*/   DWORD   SpellRank;			//Unknown209 on Lucy jun 11 2014 0x76FEE0 Original = 1 , Rk. II = 5 , Rk. III = 10 , I suppose if they add Rk. IV it will be 15 and so on -eqmule
/*0x1e0*/   DWORD   SpellClass;         //Unknown222 on Lucy
/*0x1e4*/   DWORD   SpellSubClass;         //Unknown223 on Lucy
/*0x1e8*/   DWORD   SpellReqAssociationID;
/*0x1ec*/   DWORD   CasterRequirementID;
/*0x1f0*/   DWORD   MaxResist;
/*0x1f4*/   DWORD   MinResist;
/*0x1f8*/   DWORD   MinSpreadTime;
/*0x1fc*/   DWORD   MaxSpreadTime;
/*0x200*/   DWORD   SpreadRadius;
/*0x204*/   DWORD   BaseEffectsFocusCap; //song cap, maybe other things?
/*0x208*/   DWORD   CritChanceOverride;
/*0x20c*/   DWORD   MaxTargets;     //how many targets a spell will affect
/*0x210*/   DWORD   AIValidTargets;
/*0x214*/   DWORD   BaseEffectsFocusOffset;
/*0x218*/   FLOAT   BaseEffectsFocusSlope;
/*0x21c*/   FLOAT   DistanceModCloseDist;
/*0x220*/   FLOAT   DistanceModCloseMult;
/*0x224*/   FLOAT   DistanceModFarDist;
/*0x228*/   FLOAT   DistanceModFarMult;
/*0x22c*/   FLOAT   MinRange;
/*0x230*/   BYTE    NoNPCLOS; // NPC skips LOS checks
/*0x231*/   BYTE    Feedbackable; // nothing uses this
/*0x232*/   BYTE    Reflectable;
/*0x233*/   BYTE    NoPartialSave;
/*0x234*/   BYTE    NoResist;
/*0x235*/   BYTE    UsesPersistentParticles;
/*0x236*/   BYTE    SmallTargetsOnly;
/*0x237*/   BYTE    DurationWindow;     //0=Long, 1=Short
/*0x238*/   BYTE    Uninterruptable;
/*0x239*/   BYTE    NotStackableDot;
/*0x23a*/   BYTE    Deletable;
/*0x23b*/   BYTE    BypassRegenCheck;
/*0x23c*/   BYTE    CanCastInCombat;
/*0x23d*/   BYTE    CanCastOutOfCombat;
/*0x23e*/   BYTE    NoHealDamageItemMod; //disable worn focus bonuses
/*0x23f*/   BYTE    OnlyDuringFastRegen;
/*0x240*/   BYTE    CastNotStanding;
/*0x241*/   BYTE    CanMGB;
/*0x242*/   BYTE    NoDisspell;
/*0x243*/   BYTE    AffectInanimate; //ldon trap spells etc
/*0x244*/   BYTE    IsSkill;
/*0x245*/   BYTE    ShowDoTMessage;
/*0x246*/   BYTE    ClassLevel[0x24];        //per class., yes there are allocations for 0x24 see 4B5776 in eqgame dated 12 mar 2014 -eqmule
/*0x26a*/   BYTE    LightType;
/*0x26b*/   BYTE    SpellType;          //0=detrimental, 1=Beneficial, 2=Beneficial, Group Only
/*0x26c*/   BYTE    Activated;
/*0x26d*/   BYTE    Resist;             //0=un 1=mr 2=fr 3=cr 4=pr 5=dr 6=chromatic 7=prismatic 8=physical(skills,etc) 9=corruption
/*0x26e*/   BYTE    TargetType;         //03=Group v1, 04=PB AE, 05=Single, 06=Self, 08=Targeted AE, 0e=Pet, 28=AE PC v2, 29=Group v2, 2a=Directional
/*0x26f*/   BYTE    FizzleAdj;
/*0x270*/   BYTE    Skill;
/*0x271*/   BYTE    Location;           //01=Outdoors, 02=dungeons, ff=Any
/*0x272*/   BYTE    Environment;
/*0x273*/   BYTE    TimeOfDay;          // 0=any, 1=day only, 2=night only
/*0x274*/   BYTE    CastingAnim;
/*0x275*/   BYTE    AnimVariation;
/*0x276*/   BYTE    TargetAnim;
/*0x277*/   BYTE    TravelType;
/*0x278*/   BYTE    CancelOnSit;
/*0x279*/   BYTE    IsCountdownHeld;
/*0x27a*/   CHAR    Name[0x40];
/*0x2ba*/   CHAR    Target[0x20];
/*0x2da*/   CHAR    Extra[0x20];        //This is 'Extra' from Lucy (portal shortnames etc)
/*0x2fa*/   CHAR    CastByMe[0x60];
/*0x35a*/   CHAR    CastByOther[0x60];  //cast by other
/*0x3ba*/   CHAR    CastOnYou[0x60];
/*0x41a*/   CHAR    CastOnAnother[0x60];
/*0x47a*/   CHAR    WearOff[0x60];
/*0x4da*/   BYTE    ShowWearOffMessage;
/*0x4db*/   BYTE    NPCNoCast;
/*0x4dc*/   BYTE    SneakAttack;
/*0x4dd*/   BYTE    NotFocusable; //ignores all(?) focus effects
/*0x4de*/   BYTE    NoDetrimentalSpellAggro;
/*0x4df*/   BYTE    StacksWithSelf;
/*0x4e0*/   BYTE    NoBuffBlock;
/*0x4e1*/   BYTE    Scribable;
/*0x4e2*/   BYTE    NoStripOnDeath;
/*0x4e3*/   BYTE    NoRemove; // spell can't be clicked off?
/*0x4e4*/   DWORD   Unknown177; // new spell field valid range (0, 3], will set to 1 if not in that range (0, 1, 2)
/*0x4e8*/   DWORD   SpellRecourseType;
/*0x4ec*/   BYTE    Padding; //nothing here? I don't see it setting this at least
/*0x4ed*/
} SPELL, *PSPELL;

// actual size: 0x148 10-25-2006  ieatacid
// actual size: 0x148 04-11-2017 confirmed see 5F7150 -eqmule
enum eSkillCombatType
{
	SCT_NonCombat,
	SCT_Combat,
	SCT_Special 
};
typedef struct _SKILL {
/*0x000*/  int    ImageNumber;
/*0x004*/  int	  ImageDep;
/*0x008*/  DWORD  nName;
/*0x00c*/  DWORD  ReuseTimer;
/*0x010*/  DWORD  BaseDamage;
/*0x014*/  eSkillCombatType  SkillCombatType;
/*0x018*/  int    EnduranceCost;
/*0x01c*/  FLOAT  Force;
/*0x020*/  bool   Activated;
/*0x021*/  bool   LevelCappedSkill;
/*0x024*/  DWORD  MinLevel[0x24];//the level each class gains this skill
/*0x0b4*/  DWORD  Available[0x24];   //FF=not available for that class (its actually how difficult it is to learn... low num easy, high hard...)
/*0x144*/  bool  bSkillupable;//thats not a real word...
/*0x148*/  
} SKILL, *PSKILL;

typedef struct _SKILLMGR {
/*0x000000*/	struct _SKILL *pSkill[NUM_SKILLS];
/*0x000190*/	int SkillCaps[0x24][NUM_SKILLS][MAX_PC_LEVEL+1];
/*0x174C10*/	FLOAT SkillMods[0x24][NUM_SKILLS][MAX_PC_LEVEL+1];
/*0x2E9690*/	CHAR SkillCapsFilename[MAX_PATH];
/*0x2E9794*/	UINT SkillLastUsed[NUM_SKILLS];
/*0x2E9924*/	UINT SkillTimerDuration[NUM_SKILLS];
/*0x2E9AB4*/	UINT CombatSkillLastUsed[CONCURRENT_SKILLS];
/*0x2E9ABC*/	UINT CombatSkillDuration[CONCURRENT_SKILLS];
/*0x2E9AC4*/	bool bSkillCanUse[NUM_SKILLS];
/*0x2E9B28*/	bool bCombatSkillCanUse[CONCURRENT_SKILLS];
/*0x2E9B2C*/
} SKILLMGR, *PSKILLMGR;

class SkillManager
{
public:
/*0x000000*/ TSafeArrayStatic<PSKILL, NUM_SKILLS> pSkill;
/*0x000190*/ int SkillCaps[0x24][NUM_SKILLS][MAX_PC_LEVEL+1];
/*0x174C10*/ FLOAT SkillMods[0x24][NUM_SKILLS][MAX_PC_LEVEL+1];
/*0x2E9690*/ CHAR SkillCapsFilename[MAX_PATH];
/*0x2E9794*/ UINT SkillLastUsed[NUM_SKILLS];
/*0x2E9924*/ UINT SkillTimerDuration[NUM_SKILLS];
/*0x2E9AB4*/ UINT CombatSkillLastUsed[CONCURRENT_SKILLS];
/*0x2E9ABC*/ UINT CombatSkillDuration[CONCURRENT_SKILLS];
/*0x2E9AC4*/ bool bSkillCanUse[NUM_SKILLS];
/*0x2E9B28*/ bool bCombatSkillCanUse[CONCURRENT_SKILLS];
/*0x2E9B2C*/
};

//actual size 0x3a8 11-15-11  ieatacid
typedef struct _GUILDMEMBER {
/*0x000*/ struct  _GUILDMEMBER *pNext;
/*0x004*/ BYTE    Online;
/*0x005*/ BYTE    Unknown0x5[0x3];
/*0x008*/ WORD    ZoneID;
/*0x00a*/ WORD    Instance;
/*0x00c*/ BYTE    Unknown0xc[0x5];
/*0x011*/ CHAR    Name[0x40];
/*0x054*/ DWORD   Level;
/*0x058*/ DWORD   Flags; //1=banker, 2=alt
/*0x05c*/ DWORD   Class;
/*0x060*/ DWORD   Rank; //0=member 1=officer 2=leader

// not updated
#if 0
/*0x060*/ DWORD   LastSeen; //last seen timestamp
/*0x064*/ CHAR    PublicNote[0x100];//0x1a0
/*0x164*/ CHAR    PersonalNote[0x100];
/*0x264*/ DWORD   TributeStatus;
/*0x268*/ DWORD   TributeDonations;
/*0x26c*/ DWORD   LastDonation;//timestamp
/*0x270*/ DWORD   Unknown0x270;
/*0x274*/
#endif
} GUILDMEMBER, *PGUILDMEMBER;

typedef struct _GUILDDATA
{
/*0x00*/ char   Name[0x40];
/*0x04*/ DWORD  ID;
/*0x08*/
} GUILDDATA, *PGUILDDATA;

typedef struct _GUILD
{
/*0x00*/ struct _GUILDDATA *pGuildData;
/*0x04*/ DWORD  ID;
/*0x08*/ _GUILD *pNext;
/*0x0c*/ BYTE   Unknown0xc[0xc];
/*0x18*/
} GUILD, *PGUILD;

// 11-15-11 - ieatacid
typedef struct _GUILDS {
/*0x0000*/ PVOID    pOneEntryVTable;
/*0x0004*/ BYTE     Unknown0x4[0x4];
/*0x0008*/ DWORD    GuildMemberCount;
/*0x000c*/ struct   _GUILDMEMBER *pMember;
/*0x0010*/ BYTE     Unknown0x10[0x69];
/*0x0079*/ CHAR     GuildMotD[0x200];
/*0x0279*/ CHAR     GuildLeader[0x40];
/*0x02b9*/ BYTE     Unknown0x2b9[0x3];
/*0x02bc*/ DWORD    GuildTribute;
/*0x02c0*/ BYTE     Unknown0x2c0[0x8dc6];
/*0x9086*/ CHAR     GuildUrl[0x200];
/*0x9286*/ CHAR     GuildChannelName[0x80];
/*0x9306*/ BYTE     Unknown0x9306[0x1e];
/*0x9324*/ struct _GUILD **GuildList;
/*0x9328*/ DWORD    HashValue;
/* more data */
} GUILDS, *PGUILDS;

typedef struct _INTERACTGROUNDITEM {
    DWORD DropID;
    DWORD SpawnID;
} INTERACTGROUNDITEM, *PINTERACTGROUNDITEM;

typedef struct _SPELLFAVORITE {
/*0x000*/   DWORD   SpellId[NUM_SPELL_GEMS];
/*0x030*/   CHAR    Name[0x19];
/*0x049*/   BYTE    inuse;
/*0x04a*/   BYTE    Byte_0x4a;
/*0x04b*/   BYTE    Byte_0x4b;
/*0x04c*/
} SPELLFAVORITE, *PSPELLFAVORITE;

typedef struct _CMDLIST {
    DWORD LocalizedStringID;
    PCHAR szName;
    PCHAR szLocalized;
    VOID  (__cdecl *fAddress)(PSPAWNINFO, PCHAR);
    DWORD Restriction;
    DWORD Category;
    DWORD Unknown;
} CMDLIST, *PCMDLIST;

typedef struct _EQSOCIAL {
/*0x0000*/   CHAR   Name[0x10];
/*0x0010*/   CHAR   Line[0x5][0x100];
/*0x0510*/   BYTE   Color; 
} EQSOCIAL, *PEQSOCIAL;

typedef struct _EQFRIENDSLIST {
/*0x0000*/ CHAR Name[0x64][0x40];
/*0x1900*/
} EQFRIENDSLIST, *PEQFRIENDSLIST;

typedef struct _AAEFFECTDATA
{
/*0x00*/	int EffectType;
/*0x00*/	int BaseEffect1;
/*0x00*/	int BaseEffect2;
/*0x00*/	int Slot;
} AAEFFECTDATA,*PAAEFFECTDATA;

// Size 0xa4    11/15/2011 ieatacid in msg_send_alt_data
// Size 0xa8    06/11/2014 eqmule in msg_send_alt_data
// Size 0xa8    See 4EF12F (msg_send_alt_data) in 2015-09-24 -eqmule
typedef struct _ALTABILITY {
	/*0x00*/ DWORD Index;
	/*0x04*/ bool bShowInAbilityWindow;					//[0] = enabled flag? everything 1
	/*0x05*/ BYTE bShowInAbilityWindowdPadding[0x3];
	/*0x08*/ DWORD nShortName;
	/*0x0c*/ DWORD nShorterName;
	/*0x10*/ DWORD nName;					// now a database number
	/*0x14*/ DWORD nDesc;					// now a database number
	/*0x18*/ DWORD MinLevel;				//LevelNeeded
	/*0x1c*/ DWORD Cost;					//Initial Cost or cost the last time you bought a level of it
	/*0x20*/ DWORD ID;					//ID of the AA group (/alt activate id)
	/*0x24*/ DWORD CurrentRank;				// GroupLevel the current rank of this AA first rank is 1 etc
	/*0x28*/ DWORD *RequiredGroups;		    // array of required groups
	/*0x2c*/ DWORD *RequiredGroupLevels;	// array of required group ID;
	/*0x30*/ DWORD Unknown0x30[2];			// ArrayClass stuff
	/*0x38*/ DWORD prereq_count2;			// count of next array, which contains rank required
	/*0x3c*/ DWORD *RequiresAbilityPoints;	//this is part of their ArrayClass
	/*0x40*/ DWORD Unknown0x40;				// ArrayClass stuff
	/*0x44*/ DWORD pointer;					// ArrayClass stuff
	/*0x48*/ DWORD Type;					// 1 General 2 Archetype 3 Class 4 special 5 focus
	/*0x4c*/ int  SpellID;					// -1 for no Spell
	/*0x50*/ DWORD Unknown0x50;
	/*0x54*/ DWORD Unknown0x54;				// this uses the same class as prereqs, so count?
	/*0x58*/ DWORD *reuse_id;
	/*0x5c*/ bool  bRefund;
	/*0x5d*/ BYTE  bRefundPadding[0x3];
	/*0x60*/ DWORD *TimerIds;				//ArrayClass of timer ids starts here
	/*0x64*/ DWORD ReuseTimer;				// in seconds
	/*0x68*/ DWORD Classes;					// Classes/2 is the actual value we want.
	/*0x6c*/ DWORD MaxRank;					//so like x/25, this is the 25	
	/*0x70*/ DWORD TotalPoints;				
	/*0x74*/ DWORD PreviousGroupAbilityId;	// -1 if none, although sometimes it can be -1 when there is ...
	/*0x78*/ DWORD NextGroupAbilityId;		// ID of the next rank
	/*0x7C*/ bool QuestOnly;				// vet AA, quest AA
	/*0x7D*/ BYTE QuestOnlypadding[0x3];
	/*0x80*/ DWORD max_charges;				// charges on expendable AAs (limited use aa's)
	/*0x84*/ BOOL bIgnoreDeLevel;
	/*0x88*/ DWORD Expansion;
	/*0x8c*/ DWORD special_category;		// 7 is expendable, -1 none
	/*0x90*/ bool bShroud;					//shrouds are also called "templates"
	/*0x91*/ bool bBetaOnlyAbility;
	/*0x92*/ bool bResetOnDeath;			// true for lay on hands for example.
	/*0x93*/ bool bAutogrant;				// true if its an autogrant ability? or if auto grant is enabled? need to check cause this could be useful
	/*0x94*/ DWORD AutoGrantAssociationId;		// Usually the same as normal expansion if enabled
	//this is where a list of Ability Effect Data starts in the packet
	/*0x98*/ DWORD effects_count;			// Count of spell effects for AA
	/*0x9c*/ DWORD **effects;				// this is repeated a few times some times depending on list size
	/*0xA0*/ BYTE Unknown0xA0[0x8];			//part of their internal list class, I need to re that at some point
	/*0xa8*/
} ALTABILITY, *PALTABILITY;

typedef struct _ALTABILITIESLISTMGR {
/*0x00*/ struct _ALTABILITY* Ability;
/*0x04*/ DWORD Index;
} ALTABILITIESLISTMGR, *PALTABILITIESLISTMGR;

//NUM_ALT_ABILITIES is now defined before CHARINFO definition in this same header file.
typedef struct _ALTABILITIESLIST {
/*0x00*/ struct _ALTABILITIESLISTMGR* Abilities[NUM_ALT_ABILITIES_ARRAY];
} ALTABILITIESLIST, *PALTABILITIESLIST;

typedef struct _NEWALTADVMGR {
/*0x00*/ struct _ALTABILITIESLIST* AltAbilityList;
/*0x04*/ DWORD NumAltAbilities;  // NUM_ALT_ABILITIES
/*0x08*/ DWORD Unknown0x08; // data here
/*0x0c*/ DWORD Unknown0x0c; // data here
/*0x10*/ BYTE  Unknown0x10[0x8];
} NEWALTADVMGR, *PNEWALTADVMGR;

typedef struct _ALTADVMGR {
/*0x00*/ struct _NEWALTADVMGR* AltAbilities;
} ALTADVMGR, *PALTADVMGR;

// size 0x94 (5-10-2013 see 47D926) -eqmule
typedef struct _EQRAIDMEMBER { 
/*0x000*/ CHAR      Name[0x40]; 
/*0x040*/ CHAR      RaidNote[0x40]; 
/*0x080*/ DWORD     nLevel;
/*0x084*/ DWORD     nClass; 
/*0x088*/ BYTE      RaidLeader; 
/*0x089*/ BYTE      GroupLeader; 
/*0x08a*/ BYTE      RaidMainAssist;
/*0x08b*/ BYTE      RaidMarker;
/*0x08c*/ DWORD     GroupNumber;
/*0x090*/ BYTE      Unknown0x90[0x4];
/*0x094*/ 
} EQRAIDMEMBER, *PEQRAIDMEMBER;

// sizeof(_EQRAID) is 0x3668 (12-09-2009)
//instCRaid_x
//May 10 2013 eqgame.exe CRaid__ResetRaid - eqmule
//as for the size... I have no idea...
typedef struct _EQRAID {
/*0x0000*/ BYTE     Unknown0x0000[0x218];
/*0x0218*/ CHAR     RaidMemberUsed[0x48];
/*0x0260*/ struct   _EQRAIDMEMBER RaidMember[0x48];
/*0x2c00*/ BYTE     Unknown0x2c00[0x4];
/*0x2c04*/ DWORD    RaidMemberCount;
/*0x2c08*/ CHAR     RaidLeaderName[0x40];
/*0x2c48*/ BYTE     Unknown0x2c48[0x100];
/*0x2d48*/ CHAR     RaidMOTD[0x400];
/*0x3148*/ BYTE     Unknown0x3148[0x40];
/*0x3188*/ BYTE     Invited; // 1 = default?, 2 = invited, 4 = in raid
/*0x3189*/ BYTE     Unknown0x3189[0x8];
/*0x3191*/ BYTE     IsRaidLeader;
/*0x3192*/ BYTE     Unknown0x0x3192[0x2];
/*0x3194*/ DWORD    RaidTarget;
/*0x3198*/ DWORD    LootType;
/*0x319c*/ CHAR     RaidLooters[0x13][0x40];
/*0x365c*/ DWORD    TotalRaidMemberLevels; // TotalRaidMemberLevels/RaidMemberCount=RaidAvgLevel
/*0x3660*/ BYTE     Locked;
/*0x3661*/ BYTE     Unknown0x3661[0x7];
/*0x3668*/
} EQRAID, *PEQRAID;

// size 0x19C 3-23-2005
typedef struct _EQGROUP {
/*0x000*/ BYTE MemberExists[5];
/*0x005*/ CHAR MemberName[5][0x40];
/*0x145*/ BYTE Unused[3];
/*0x148*/ struct _SPAWNINFO* pMember[5];
/*0x15C*/ CHAR LeaderName[0x40];
/*0x19C*/
} EQGROUP, *PEQGROUP;

// size 0x08
typedef struct _EQSTRING {
/*0x00*/    DWORD ID;
/*0x04*/    PCHAR String;
} EQSTRING, *PEQSTRING;

// size 0x10 4-28-2004 lax
typedef struct _EQSTRINGTABLE {
/*0x00*/ struct _EQSTRING **StringItems;
/*0x04*/ DWORD Size;
/*0x08*/ DWORD Count;
/*0x0c*/ DWORD Unknown0x0c;
/*0x10*/
} EQSTRINGTABLE, *PEQSTRINGTABLE;

//updated by eqmule
//see 7B000C in eqgame.exe dated jul 16 2014
//not sure about size...
typedef struct connection_t {
/*0x000*/ void  *vtable;
/*0x004*/ BYTE  Unknown0x4[0x110];
/*0x114*/ DWORD Master;
/*0x118*/ DWORD Average;
/*0x11c*/ DWORD Low;
/*0x120*/ DWORD High;
/*0x124*/ DWORD Last;
/*0x128*/
} CONNECTION_T, *PCONNECTION_T;

//actual size 0x2dfc 3-19-09 - ieatacid 
typedef struct _CDISPLAY {
/*0x0000*/ BYTE   Unknown0x000[0x12c];
/*0x012c*/ void   *pActorTagManager;
/*0x0130*/ BYTE   Unknown0x130[0x24];
/*0x0154*/ DWORD  TimeStamp;
/*0x0158*/ BYTE   Unknown0x158[0x2c12];
/*0x2d6a*/ BYTE   NpcNames; // show npc names
} CDISPLAY, *PCDISPLAY;

//5-16-06 - ieatacid
typedef struct _DZTIMERINFO {
/*0x000*/ CHAR   ExpeditionName[0x80];
/*0x080*/ CHAR   EventName[0x100];
/*0x180*/ DWORD  TimeStamp;  // TimeStamp - Util__FastTime = time left
/*0x184*/ DWORD  TimerID;
/*0x188*/ struct _DZTIMERINFO *pNext;
/*0x18c*/
} DZTIMERINFO, *PDZTIMERINFO;

typedef struct _DZMEMBER {
/*0x000*/ CHAR   Name[0x40];
/*0x040*/ DWORD  Status;  // 0="unknown", 1="Online", 2="Offline", 3="In Dynamic Zone", 4="Link Dead"
/*0x044*/ struct _DZMEMBER *pNext;
/*0x048*/
} DZMEMBER, *PDZMEMBER;

typedef struct _TASKMEMBER {
/*0x000*/ CHAR   Name[0x40];
/*0x040*/ DWORD  Unknown0x40;
/*0x044*/ DWORD  IsLeader;
/*0x048*/ struct _TASKMEMBER *pNext;
/*0x04c*/
} TASKMEMBER, *PTASKMEMBER;

typedef struct _DYNAMICZONE {
/*0x000*/ void   *vftable;
/*0x004*/ BYTE   Unknown0x04[0x46];
/*0x04a*/ CHAR   Name[0x40]; // Leaders name
/*0x08a*/ CHAR   ExpeditionName[0x80];
/*0x10a*/ BYTE   Unknown0x10a[0x2];
/*0x10c*/ WORD   MaxPlayers;
/*0x10e*/ BYTE   Unknown0x10e[0x2];
/*0x110*/ struct _DZMEMBER *pMemberList;
/*0x114*/ PCHAR  expeditionName;
/*0x118*/ // more?
} DYNAMICZONE, *PDYNAMICZONE;

typedef struct _CHATCHANNELS {
/*0x000*/ PCHAR  ChannelName[0xa];
/*0x004*/
} CHATCHANNELS, *PCHATCHANNELS;

typedef struct _CHATSERVICE {
/*0x000*/ BYTE   Unknown0x0[0xc];
/*0x00c*/ struct _CHATCHANNELS *ChannelList;
/*0x010*/ DWORD  ActiveChannels;   // number of channels joined
/*0x014*/ BYTE   Unknown0x14[0x8];
/*0x01c*/ CHAR   ChatID[0x30];     // "ServerName.CharName" (0x60?)
/*0x04c*/ BYTE   Unknown0x4c[0x30];
/*0x07c*/ CHAR   ServerName[0x30]; // "ServerName."
/*0x0b0*/ // more data
} CHATSERVICE, *PCHATSERVICE;

//size 0x160 see 430510 - eqmule
typedef struct _CSINFO
{
/*0x000*/	CHAR Name[0x40]; 
/*0x040*/	UINT Class;
/*0x044*/	UINT Race;
/*0x048*/	BYTE Level; 
/*0x049*/	BYTE Padding0x049[0x3]; 
/*0x04c*/	UINT Class2;
/*0x050*/	UINT Race2;
/*0x054*/	int CurZoneID;
/*0x058*/	BYTE morestuff[0x108];
//todo: add the stuff below and pad it, but for now I just need the zone id.
///*0x058*/	BYTE Sex; 
///*0x059*/	BYTE Face; 
///*0x05a*/	BYTE Padding0x05a[0x2]; 
///*0x05c*/	struct _ARMOR Armor[9]; 
///*0x000*/	unsigned long ArmorTint[9]; 
///*0x000*/	CHAR TextureType;
///*0x000*/	CHAR Armor1;
///*0x000*/	CHAR Armor2;
///*0x000*/	CHAR headType;
///*0x000*/	int Tattoo;
///*0x000*/	int Details;
///*0x000*/	int Deity; 
///*0x000*/	void* pActorClient; 
///*0x000*/	void* pActorClient2; 
///*0x000*/	BYTE HairColor;
///*0x000*/	BYTE FacialHairColor;
///*0x000*/	BYTE EyeColor1;//left
///*0x000*/	BYTE EyeColor2;//right
///*0x000*/	BYTE HairStyle;
///*0x000*/	BYTE FacialHair;
///*0x000*/	bool bCanGoHome;
///*0x000*/	bool bCanTutorial;
///*0x000*/	int ID;
///*0x000*/	bool bTooHighLevel;
///*0x000*/	bool bUnknown;
///*0x000*/	long lUnknown;
///*0x000*/	bool bWhat;
///*0x000*/    bool bHeroicCharacter;
///*0x000*/    bool bShrouded;
/*0x160*/
} CSINFO, *PCSINFO;
typedef struct _CharSelectPlayerArray
{
	//note that CharSelectPlayerCount determines how many are actully here
	CSINFO CharacterInfo[13];//is 13 chars the max u can have?
} CharSelectPlayerArray,*PCharSelectPlayerArray;

typedef struct _EVERQUEST {
	/*0x000*/ BYTE   Unknown[0x2a4];
	/*0x2a4*/ struct _CHATSERVICE *ChatService;
	/*0x2a8*/ BYTE   Unknown0x2a8[0x8];
	/*0x2b0*/ bool   bJoinedChannel;
	/*0x2b1*/ CHAR   ChannelPlayerName[0x100];
	/*0x3b1*/ CHAR   ChannelName[0xa][0x30];
	/*0x591*/ BYTE   Unknown0x591[0x3];
	/*0x594*/ DWORD  ChannelNumber[0xa];
	/*0x5bc*/ DWORD  ChannelQty;
	/*0x5c0*/ DWORD  TargetRing;
	/*0x5c4*/ DWORD	 WorldState;//0 everything is fine, 1 we are getting disconnected 2 player not released from zone
	/*0x5c8*/ DWORD  GameState;
	/*0x5cc*/ BYTE  Unknown0x5cc[0x388A0];
	/*0x38E6C*/ DWORD CharSelectPlayerMaxCount;
	/*0x38E70*/ DWORD CharSelectPlayerCount;
	/*0x38E74*/ PCharSelectPlayerArray pCharSelectPlayerArray;
	/*0x5cc*/ // more data
} EVERQUEST, *PEVERQUEST;

typedef struct _AURAINFO {
/*0x000*/ CHAR    Name[0x40];
/*0x040*/ BYTE    Unknown0x14[0xc];
/*0x04c*/
} AURAINFO, *PAURAINFO;

typedef struct _AURAS {
/*0x000*/ _AURAINFO Aura[0x2];
} AURAS, *PAURAS;

typedef struct _AURAMGR {
/*0x000*/ DWORD   NumAuras;
/*0x004*/ BYTE    Unknown0x4[0xc];
/*0x010*/ _AURAS  **pAuraInfo;
/*0x014*/ BYTE    Unknown0x14[0x8];
/*0x01c*/
} AURAMGR, *PAURAMGR;

typedef struct _INTERACTSWITCH {
/*0x000*/ DWORD   switchID;
/*0x004*/ DWORD   dwzero;
/*0x008*/ DWORD   dwneg1;
/*0x00c*/ DWORD   spawnID;
} INTERACTSWITCH, *PINTERACTSWITCH;

typedef struct _MERCSTANCEDATA {
DWORD nStance;
DWORD nDbStance;
} MERCSTANCEDATA, *PMERCSTANCEDATA;

typedef struct _MERCSINFO {
/*0x00*/ DWORD Unknown0x00;
/*0x04*/ DWORD nMercCategory;
/*0x08*/ DWORD nMercDesc;
/*0x0c*/ DWORD Purchase; //in copper...
/*0x10*/ DWORD Upkeep; //in copper...
/*0x14*/ BYTE Unknown0x14[0x2c];
/*0x40*/ CHAR Name[0xC];
/*0x4c*/ BYTE Unknown0x4c[0x88];
/*0xD4*/
} MERCSINFO, *PMERCSINFO;
//Size 0xD4 in eqgame.exe dated 01 22 2015 -eqmule
typedef struct _MERCSLIST {
/*0x00*/ MERCSINFO mercinfo[7];//is 7 max, even with slots u can buy for sc?
} MERCSLIST, *PMERCSLIST;

//Actual Size: 0x2fc (See 57117F in eqgame dated dec 10 2013) - eqmule
//CMercenaryInfo__CMercenaryInfo
typedef struct _MERCENARYINFO {
/*0x000*/ BYTE Unknown0x0[0x110];
/*0x110*/ DWORD HaveMerc;
/*0x114*/ DWORD MercState; // 1 = suspended, 5 = active
/*0x118*/ BYTE  Unknown0x118[0x30];
/*0x148*/ DWORD ActiveStance;
/*0x14c*/ BYTE  Unknown0x14c[0x10];
/*0x15c*/ CHAR	MercName[0x18];
/*0x174*/ BYTE  Unknown0x174[0x7c];
/*0x1F0*/ DWORD MercenaryCount;//how many mercenaries we have
/*0x1F4*/ PMERCSLIST pMercsList; 
/*0x1F8*/ BYTE  Unknown0x1f4[0xc];
/*0x204*/ DWORD MaxMercsCount;//max you can have
/*0x208*/ BYTE  Unknown0x208[0x10];
/*0x218*/ DWORD CurrentMercIndex;
/*0x21c*/ BYTE  Unknown0x21c[0x8];
/*0x224*/ DWORD MercSpawnId;//yes its the spawnID of the mercenary
/*0x228*/ BYTE  Unknown0x228[0x30];
/*0x258*/ DWORD NumStances;
/*0x25c*/ _MERCSTANCEDATA **pMercStanceData;
/*0x260*/ BYTE  Unknown0x25c[0x9c];
/*0x2Fc*/
} MERCENARYINFO, *PMERCENARYINFO;

#define MAX_XTARGETS 20

enum xTargetTypes
{
    XTARGET_EMPTY_TARGET,
    XTARGET_AUTO_HATER,
    XTARGET_SPECIFIC_PC,
    XTARGET_SPECIFIC_NPC,
    XTARGET_TARGETS_TARGET,
    XTARGET_GROUP_TANK,
    XTARGET_GROUP_TANKS_TARGET,
    XTARGET_GROUP_ASSIST,
    XTARGET_GROUP_ASSIST_TARGET,
    XTARGET_GROUP_PULLER,
    XTARGET_GROUP_PULLER_TARGET,
    XTARGET_GROUP_MARK1,
    XTARGET_GROUP_MARK2,
    XTARGET_GROUP_MARK3,
    XTARGET_RAID_ASSIST1,
    XTARGET_RAID_ASSIST2,
    XTARGET_RAID_ASSIST3,
    XTARGET_RAID_ASSIST1_TARGET,
    XTARGET_RAID_ASSIST2_TARGET,
    XTARGET_RAID_ASSIST3_TARGET,
    XTARGET_RAID_MARK1,
    XTARGET_RAID_MARK2,
    XTARGET_RAID_MARK3,
    XTARGET_MY_PET,
    XTARGET_MY_PET_TARGET,
    XTARGET_MY_MERCENARY,
    XTARGET_MY_MERCENTARY_TARGET
};

typedef struct _AGGRODATA {
/*0x00*/ DWORD Unknown0x0;
/*0x04*/ BYTE  AggroPct;
/*0x05*/ BYTE  padding[3];
/*0x08*/
} AGGRODATA, *PAGGRODATA;

// size 0xe4 11-28-12 - ieatacid (in GetAggroInfo)
typedef struct _AGGROINFO {
/*0x00*/ struct _AGGRODATA aggroData[0x1b];
/*0xd8*/ DWORD  AggroLockID;//this can be 0, I dont know what it is... -eqmule
/*0xdc*/ DWORD  AggroTargetID;//this is id of whoever we are fighting -eqmule
/*0xe0*/ DWORD  AggroSecondaryID;//this is id of whoever the npc is fighting -eqmule
/*0xe4*/
} AGGROINFO, *PAGGROINFO;

enum AggroDataTypes
{
    AD_Player,
    AD_Secondary,
    AD_Group1,
    AD_Group2,
    AD_Group3,
    AD_Group4,
    AD_Group5,
    AD_xTarget1,
    AD_xTarget2,
    AD_xTarget3,
    AD_xTarget4,
    AD_xTarget5,
    AD_xTarget6,
    AD_xTarget7,
    AD_xTarget8,
    AD_xTarget9,
    AD_xTarget10,
    AD_xTarget11,
    AD_xTarget12,
    AD_xTarget13,
    AD_xTarget14,
    AD_xTarget15,
    AD_xTarget16,
    AD_xTarget17,
    AD_xTarget18,
    AD_xTarget19,
    AD_xTarget20,
};
typedef struct _GROUPAGGRO {
/*0x00*/ DWORD Unknown0x00;
/*0x04*/ DWORD Unknown0x04;
/*0x08*/ DWORD  GroupMemberAggro[6];//player is ALWAYS the 6th member...
} GROUPAGGRO, *PGROUPAGGRO;

//May 10 2013 -eqmule
#define EQ_ASSIST_CALC                  0x020c0f19
#define EQ_ASSIST_COMPLETE              0x7CEB
#define EQ_ASSIST                       0x4478  // do_assist(PlayerClient *,char const *)+399 20160212 live (see 52C319)
#define EQ_BEGIN_ZONE                   0x600D  // CEverQuest__SavePCForce				
#define EQ_END_ZONE                     0x166C  // CEverQuest__DoMainLoop+B2F			
#define EQ_LoadingS__ArraySize          0x5b    // EQ_LoadingS__SetProgressBar_x+8C		20130619
};
using namespace EQData;
