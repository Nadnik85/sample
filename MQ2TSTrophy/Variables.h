#pragma once

PCONTENTS BakingTrophy = 0, AlchemistTrophy = 0, BrewingTrophy = 0, FletchingTrophy = 0, JewelerTrophy = 0, TinkeringTrophy = 0, PotteryTrophy = 0, ResearchTrophy = 0, BlacksmithTrophy = 0, TailorTrophy = 0, PoisonTrophy = 0, ammoDefault = 0;

bool bAlchemistTrophy = false, bBakingTrophy = false, bBrewingTrophy = false, bFletchingTrophy = false, bJewelerTrophy = false, bTinkeringTrophy = false, bPotteryTrophy = false, bResearchTrophy = false, bBlacksmithTrophy = false, bTailorTrophy = false, bPoisonTrophy = false, bActivated = false, containerfound = false;

int iPulseDelay = 25, iPulse = 0, iStep = 1;

char szContainerName[MAX_STRING] = { 0 };
