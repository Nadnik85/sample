#pragma once
#include "..\MQ2Plugin.h"
#include <ctime>

void CleanBuffsFile();
void WriteBuffs();
PSPELL MyBlockedBuff(int i);
inline bool InGame();
int NumXTargets();
PSPAWNINFO Me();
void VerifyINI(char* Section, char* Key, char* Default, char* ININame);
bool IsDefined(PCHAR szLine);
int GetNumDiseaseCounters();
int GetNumPoisonCounters();
int GetNumCurseCounters();
int GetNumCorruptionCounters();
int GroupSize();
unsigned long MainAssistID();

extern int iDiseaseCounters;
extern int iPoisonCounters;
extern int iCurseCounters;
extern int iCorruptionCounters;

extern char MyLastID[64];
extern unsigned __int64 WriteBuffsTimer;

extern char CurrentBuffs[MAX_STRING];
extern char CurrentShortBuffs[MAX_STRING];
extern char CurrentBlockedBuffs[MAX_STRING];
extern char CurrentPetBuffs[MAX_STRING];
extern unsigned __int64 WriteBuffsTimer;
extern int Pulse;
extern int PulseDelay;