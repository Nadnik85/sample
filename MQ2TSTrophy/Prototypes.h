#pragma once

void UpdateTrophyGroup(PCONTENTS* group, char list[MAX_STRING], char groupname[MAX_STRING]);
void UpdateTrophies();
void UpdateTrophy(PCONTENTS* DiscVariable, PCHAR szLine, PCHAR Typename);
void WorldContainerCheck();
void SwapToTrophy(PCONTENTS* Trophy);
void PluginOn();

PCONTENTS FindAmmoDefault();
PCONTENTS Cursor();

inline bool InGame();