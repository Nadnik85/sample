#pragma once

void UpdateTrophyGroup(PCONTENTS* group, char list[MAX_STRING], char groupname[MAX_STRING]);
void UpdateTrophies();
void UpdateTrophy(PCONTENTS* DiscVariable, PCHAR szLine, PCHAR Typename);
void WorldContainerCheck();
void SwapSlot(PCONTENTS* Trophy,int slot);
void PluginOn();

PCONTENTS FindSlotDefault(int slot);
PCONTENTS Cursor();

inline bool InGame();