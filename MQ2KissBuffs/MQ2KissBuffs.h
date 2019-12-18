#pragma once
#include "..\MQ2Plugin.h"
#include <ctime>

void CleanBuffsFile();
void WriteBuffs();
PSPELL MyBlockedBuff(int i);
inline bool InGame();
int NumXTargets();
PSPAWNINFO Me();
void VerifyINI(char Section[MAX_STRING], char Key[MAX_STRING], char Default[MAX_STRING], char ININame[MAX_STRING]);
bool IsDefined(PCHAR szLine);

extern char MyLastID[64];
extern unsigned __int64 WriteBuffsTimer;
extern int Pulse;
extern int PulseDelay;