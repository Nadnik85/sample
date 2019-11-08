if (!_stricmp(Arg, "merc")) {
	char stat[256] = "/bc ";
	if (pMercInfo->HaveMerc) {
		SPAWNINFO MercenarySpawn;
		if (pMercInfo->MercState == 1) {
			sprintf_s(stat, "1 Our merc state is: %s", &DataTypeTemp);
		}
		if (pMercInfo->MercState == 0) {
			sprintf_s(stat, "0 Our merc state is: %s", MercenarySpawn.Name);
		}
		if (pMercInfo->MercState == 5) {
			sprintf_s(stat, "5 Our merc state is: %s, Class: %s, Stance: %s", &MercenarySpawn.Name, pMercInfo->CurrentMercIndex, pMercInfo->ActiveStance);
		}
	}
	else {
		sprintf_s(stat, "We do not have a merc.");
	}
	strcat_s(buffer, stat);
	EzCommand(buffer);
}


Tank:
ClassID - 1
Stance 2 - Passive
Stance 1 - Assist
Stance 0 - Aggressive

Healer
ClassID - 2
Stance 0 - Balanced
Stance 1 - Efficient
Stance 3 - Passive
Stance 2 - Reactive

Wizard
ClassID 
Stance 1 - Balanced
Stance 2 - Burn
Stance 3 - Burn AE
Stance 0 - Passive

Melee
ClassID - 9
Stance 0 - Passive
Stance 2 - Burn
Stance 1 - Balanced


		}
		}
		if (!_stricmp(Arg, "merc")) {
			if (pMercInfo) {
				char temp[32] = { 0 };
				ZeroMemory(&MercenarySpawn, sizeof(MercenarySpawn));
				if (pMercInfo->HaveMerc == 1) {
					switch (pMercInfo->MercState)
					{
					case 0:
						strcat_s(buffer, "Mercenary State: \arDEAD\aw");
						break;
					case 1:
						strcat_s(buffer, "Mercenary State: \arSUSPENDED\aw");
						break;
					case 5:
						strcat_s(buffer, "Mercenary State: \agALIVE\aw");
						break;
					default:
						strcat_s(buffer, "Mercenary State: UNKNOWN");
						break;
					}
				}
			}
			else {
				strcat_s(buffer, "We do not have a mercenary");
			}
			EzCommand(buffer);
		}