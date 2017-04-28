// MQ2ThrowIt.cpp : Defines the entry point for the DLL application.
// Version:
// 2.1 - item pointer update
// 2.0 - update for HoT slots and general cleanup, pms
//
// 1.2 - updated to fix issues with items that do not autoload from inventory
//         into the ammo slot (for example, shissar fangs don't, arrows do)
//       fixed data type issue
// 1.1 - updated with line of site check
// 1.0 - original release

// Swap the range slot item with an item from the ammo slot, range attack,
// then put the range slot item back.

// Commands:
// /throwit      - performs the range attack using throwing

// ${ThrowIt} returns count of ammo.
// ${ThrowIt.Name} returns name of ammo.
// ${ThrowIt.Count} returns total count of ammo in all bags/slots.
// ${ThrowIt.SlotCount} returns count of ammo in the ammo slot.
// ${ThrowIt.TotalCount} returns total count of ammo in all bags/slots.

#include "../MQ2Plugin.h"
#include "../moveitem.h"

const char*  PLUGIN_NAME = "MQ2ThrowIt";
const double PLUGIN_VER  = 2.0;

PreSetup(PLUGIN_NAME);
PLUGIN_VERSION(PLUGIN_VER);

unsigned long AmmoSlotCount()
{
    if (!GetCharInfo2()) return 0;
    PCONTENTS pAmmo = GetCharInfo2()->pInventoryArray->Inventory.Ammo;
    PITEMINFO pAmmoInfo = GetItemFromContents(pAmmo);
    if (!pAmmo || !pAmmoInfo) return 0;
    if (!ItemIsStackable(pAmmo)) return 1;
    return pAmmo->StackCount;
}

unsigned long AmmoTotalCount()
{
    if (!GetCharInfo2()) return 0;
    char szItem[MAX_STRING] = {0};

    PCONTENTS pAmmo = GetCharInfo2()->pInventoryArray->Inventory.Ammo;
    PITEMINFO pAmmoInfo = GetItemFromContents(pAmmo);
    if (!pAmmo || !pAmmoInfo) return 0;
    sprintf(szItem, "%s", pAmmoInfo->Name);
    return CountItem(szItem);
}

class MQ2ThrowItType : public MQ2Type
{
public:
    enum ThrowItMembers
    {
        Name       = 1,
        Count      = 2,
        SlotCount  = 3,
        TotalCount = 4,
    };
    MQ2ThrowItType():MQ2Type("ThrowIt")
    {
        TypeMember(Name);
        TypeMember(Count);
        TypeMember(SlotCount);
        TypeMember(TotalCount);
    }
    bool GetMember(MQ2VARPTR VarPtr, char* Member, char* Index, MQ2TYPEVAR &Dest)
    {
        PMQ2TYPEMEMBER pMember = MQ2ThrowItType::FindMember(Member);
        if (!pMember) return false;
        PITEMINFO pAmmoItem = NULL;
        switch((ThrowItMembers)pMember->ID)
        {
        case Name:
            pAmmoItem = GetItemFromContents(GetCharInfo2()->pInventoryArray->Inventory.Ammo);
            sprintf(DataTypeTemp, "%s", pAmmoItem ? pAmmoItem->Name : "NULL");
            Dest.Ptr  = DataTypeTemp;
            Dest.Type = pStringType;
            return true;
        case TotalCount:
        case Count:
            sprintf(DataTypeTemp, "%d", AmmoTotalCount());
            Dest.Ptr  = DataTypeTemp;
            Dest.Type = pStringType;
            return true;
        case SlotCount:
            sprintf(DataTypeTemp, "%d", AmmoSlotCount());
            Dest.Ptr  = DataTypeTemp;
            Dest.Type = pStringType;
            return true;
        }
        return false;
    }
    bool ToString(MQ2VARPTR VarPtr, char* Destination)
    {
        sprintf(Destination, "%d", AmmoTotalCount());
        return true;
    }
    bool FromData(MQ2VARPTR &VarPtr, MQ2TYPEVAR &Source)
    {
        return false;
    }
    bool FromString(MQ2VARPTR &VarPtr, char* Source)
    {
        return false;
    }
};
MQ2ThrowItType* pThrowItType = NULL;

int dataThrowIt(char* szName, MQ2TYPEVAR &Ret)
{
    Ret.DWord = 1;
    Ret.Type  = pThrowItType;
    return true;
}

void ThrowItCmd(PSPAWNINFO pLPlayer, char* szLine)
{
    if (!GetCharInfo2()) return;

    if (!BagWndLoaded())
    {
        WriteChatf("\ay%s\aw:: \arThis plugin currently requires MQ2BagWindow to be loaded.", PLUGIN_NAME);
        return;
    }

    unsigned long ulAmmo    = 0;
    unsigned char ucRange   = 0;
    char szItem[MAX_STRING] = {0};
    char szAmmo[MAX_STRING] = {0};
    char szRang[MAX_STRING] = {0};

    PCONTENTS pAmmo = GetCharInfo2()->pInventoryArray->Inventory.Ammo;
    PITEMINFO pAmmoInfo = GetItemFromContents(pAmmo);
    if (!pAmmo || !pAmmoInfo || CursorHasItem()) return;

    if (!pTarget)
    {
        WriteChatf("\ay%s\aw:: \arNeed a target!\ax", PLUGIN_NAME);
        return;
    }
    EQPlayer* pRangedTarget = pTarget;

    strcpy(szItem, pAmmoInfo->Name);
    ucRange = pAmmoInfo->Range;

    if (!strnicmp(szItem,"Shissar Fangs", 13))
    {
        ulAmmo = AmmoSlotCount();
    }
    else
    {
        ulAmmo = AmmoTotalCount();
    }

    if (ulAmmo < 2)
    {
        WriteChatf("\ay%s\aw:: \arCannot use the last piece of ammo, please reload!\ax", PLUGIN_NAME);
        return;
    }

    PSPAWNINFO psTarget = (PSPAWNINFO)pTarget;
    if (DistanceToSpawn(pLPlayer, psTarget) > ucRange)
    {
        WriteChatf("\ay%s\aw:: \arTarget too far! %s max range is %u!\ax", PLUGIN_NAME, szItem, ucRange);
        return;
    }


    if (!LineOfSight(pLPlayer, psTarget))
    {
        WriteChatf("\ay%s\aw:: \arTarget not in line of sight!\ax", PLUGIN_NAME);
        return;
    }


    long lAmmo = SlotFind("ammo");
    long lRange = SlotFind("ranged");
    sprintf(szAmmo, "InvSlot%d", lAmmo);
    sprintf(szRang, "InvSlot%d", lRange);

    SendWornClick(szAmmo, CTRLKEY);  // pickup a single item from ammo
    ulAmmo -= 1;                      // decrement our local count
    SendWornClick(szRang, NOKEY);    // place the item from ammo into the ranged
    AttackRanged(pRangedTarget);     // DoCommand is ghetto
    if (CursorHasItem())
    {
        SendWornClick(szRang, SHIFTKEY); // place anything we may have on cursor back into ranged
        SendWornClick(szAmmo, NOKEY);
    }

    // reset facing angle, DoCommand is ghetto
    ((PSPAWNINFO)pCharSpawn)->CameraAngle = 0.0f;
    gLookAngle = 10000.0f;

    WriteChatf("\ay%s\aw:: \arAmmo: %s - Remaining %d\ax", PLUGIN_NAME, szItem, ulAmmo);
}

PLUGIN_API void InitializePlugin()
{
    AddCommand("/throwit", ThrowItCmd);
    pThrowItType = new MQ2ThrowItType;
    AddMQ2Data("ThrowIt", dataThrowIt);
}

PLUGIN_API void ShutdownPlugin()
{
    RemoveMQ2Data("ThrowIt");
    delete pThrowItType;
    RemoveCommand("/throwit");
}