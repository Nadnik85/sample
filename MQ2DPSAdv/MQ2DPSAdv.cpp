// DPS ADV CREATED BY WARNEN 2008-2009
// MQ2DPSAdv.cpp

#include "../MQ2Plugin.h"
#include "MQ2DPSAdv.h"

PreSetup("MQ2DPSAdv");
PLUGIN_VERSION(1.6);

// ############################### DPSEntry Start ############################################

DPSMob::DPSEntry::DPSEntry() {
	Init();
}

DPSMob::DPSEntry::DPSEntry(CHAR EntName[], DPSMob* pParent, bool PCOnly /* = false */) {
	Init();
	Parent = pParent;
	strncpy_s(Name, EntName, sizeof(Name) - 1);
	GetSpawn(PCOnly);
}

void DPSMob::DPSEntry::Init() {
	Parent = nullptr;
	SpawnType = -1;
	Mercenary = false;
	Class = 0;
	Spawn = nullptr;
	Master = nullptr;
	DoSort = false;
	Pets = false;
	CheckPetName = false;
	UsingPetName = false;
	strcpy_s(Name, "");
	Damage.Total = 0;
	Damage.First = 0;
	Damage.Last = 0;
	Damage.AddTime = 0;
}

void DPSMob::DPSEntry::GetSpawn(bool PCOnly /* = false */) {
	PSPAWNINFO pSpawn = (PSPAWNINFO)pSpawnList;
	while (pSpawn) {
		if (!_stricmp(pSpawn->DisplayedName, Name) && (!PCOnly || pSpawn->Type == SPAWN_PLAYER)) {
			if (pSpawn->Type != SPAWN_CORPSE) {
				Spawn = pSpawn;
				SpawnType = Spawn->Type;
				Class = Spawn->mActorClient.Class;
				Mercenary = pSpawn->Mercenary;
				return;
			}
		}
		pSpawn = pSpawn->pNext;
	}
	if (Debug && !Spawn) WriteChatf("\arError: Could not find Ent Spawn (%s)", Name);
}

bool DPSMob::DPSEntry::CheckMaster() {
	if (DoSort) return Master ? true : false;
	if (UsingPetName) return true;
	if (!CheckPetName) {
		CheckPetName = true;
		if (strstr(Name, "`s pet")) {
			UsingPetName = true;
			CHAR szMaster[MAX_STRING] = { 0 };
			strcpy_s(szMaster, Name);
			if (char* pDest = strstr(szMaster, "`s pet")) {
				pDest[0] = '\0';
			}
			Master = Parent->GetEntry(szMaster);
			return true;
		}
	}
	if (Master != nullptr && (Spawn == nullptr || Spawn->MasterID <= 0 || Master->Spawn == nullptr || Master->Spawn->SpawnID != Spawn->MasterID)) Master = nullptr;
	if (Master != nullptr) return true;
	if (Spawn && Spawn->MasterID > 0) {
		PSPAWNINFO NewMaster = (PSPAWNINFO)GetSpawnByID(Spawn->MasterID);
		if (NewMaster) {
			Master = Parent->GetEntry(NewMaster->DisplayedName, true, true);
			return Master != nullptr;
		}
	}
	return false;
}

void DPSMob::DPSEntry::AddDamage(int Damage1) {
	if (CheckMaster()) {
		Master->Pets = true;
		Master->AddDamage(Damage1);
		DoSort = true;
		return;
	}

	DoSort = true;
	time_t timenow = time(nullptr);

	if (Damage.First && (timenow - Damage.Last >= EntTO)) {
		Damage.AddTime += (int)(Damage.Last - Damage.First) + 1;
		Damage.First = 0;
	}

	Damage.Total += Damage1;
	if (!Damage.First)
		Damage.First = timenow;

	Damage.Last = timenow;

	Parent->AddDamage(Damage1);
}

uint64_t DPSMob::DPSEntry::GetDPS() {
	uint64_t dividetotal = (((Damage.Last - Damage.First) + 1) + Damage.AddTime);
	if (dividetotal == 0)
		return 0;

	return (Damage.Total / dividetotal);
}

uint64_t DPSMob::DPSEntry::GetSDPS() {
	uint64_t dividetotal = ((CurListMob->Damage.Last - CurListMob->Damage.First) + 1) + CurListMob->Damage.AddTime;
	if (dividetotal == 0)
		return 0;

	return (Damage.Total / dividetotal);
}

void DPSMob::DPSEntry::Sort() {
	DoSort = false;
	if (Master && Master->DoSort) Master->Sort();
	if (Damage.Total == 0) return;
	//   DPSWnd->ReSort = true;
	int i = 0, x = -1;
	bool Inserted = false;
	for (i = 0; i < (int)Parent->EntList.size(); i++) {
		if (Parent->EntList[i] == this) {
			Inserted = true;
			if (x == -1) break;
			Parent->EntList.erase(Parent->EntList.begin() + i);
			Parent->EntList.insert(Parent->EntList.begin() + x, this);
			break;
		}
		else if (x == -1 && Parent->EntList[i]->Damage.Total < Damage.Total) x = i;
	}
	if (!Inserted)
		if (x != -1) Parent->EntList.insert(Parent->EntList.begin() + x, this);
		else Parent->EntList.push_back(this);
	if (LiveUpdate && Parent == CurListMob && !CurListMob->Dead) DPSWnd->DrawList();
};

// ############################### DPSMob START ############################################

DPSMob::DPSMob() {
	Init();
}

DPSMob::DPSMob(PCHAR MobName, size_t MobLen) {
	Init();
	strncpy_s(Name, MobName, sizeof(Name) - 1);
	GetSpawn();
	if (strstr(Name, "`s pet"))
		PetName = true;
}

void DPSMob::Init() {
	strcpy_s(Name, "");
	Damage.Total = 0;
	Damage.First = 0;
	Damage.Last = 0;
	LastEntry = 0;
	SpawnType = -1;
	Mercenary = false;
	Spawn = 0;
	Active = false;
	Dead = false;
	PetName = false;
}

void DPSMob::GetSpawn() {
	PSPAWNINFO pSpawn = (PSPAWNINFO)pSpawnList;
	while (pSpawn) {
		if (!_stricmp(pSpawn->DisplayedName, Name)) {
			SpawnType = pSpawn->Type;
			Mercenary = pSpawn->Mercenary;
			Spawn = pSpawn;
			if (SpawnType != SPAWN_CORPSE) {
				Dead = false;
				return;
			}
			Dead = true;
		}
		pSpawn = pSpawn->pNext;
	}
	if (Debug && !Spawn) WriteChatf("\arError: Could not find Mob Spawn (%s)", Name);
}

bool DPSMob::IsPet() {
	return PetName || (Spawn && Spawn->MasterID > 0);
}

void DPSMob::AddDamage(int Damage1) {
	Damage.Total += Damage1;
	time_t timenow = time(nullptr);

	if (!Damage.First)
		Damage.First = timenow;

	Damage.Last = timenow;

	if (!Active) {
		Active = true;
		sprintf_s(Tag, "[A] ");

		if (!IsPet() && !Mercenary)
			DPSWnd->DrawCombo();
	}
}

DPSMob::DPSEntry* DPSMob::GetEntry(char EntName[], bool Create /* = true */, bool PCOnly /* = false */) {
	char szBuffer[256] = { 0 };
	strcpy_s(szBuffer, 256, EntName);
	if (!_stricmp(szBuffer, "You")) {
		strcpy_s(szBuffer, 256, ((PSPAWNINFO)pLocalPlayer)->Name);
	}

	if (LastEntry && !strcmp(LastEntry->Name, szBuffer) && (!PCOnly || LastEntry->SpawnType == SPAWN_PLAYER)) return LastEntry;

	if (LastEntry && LastEntry->DoSort) LastEntry->Sort();
	// TODO: This and other areas like this should probably be range based for loops.
	for (int i = 0; i < (int)EntList.size(); i++) {
		if (EntList[i] != nullptr && !strcmp(EntList[i]->Name, szBuffer) && (!PCOnly || EntList[i]->SpawnType == SPAWN_PLAYER)) {
			LastEntry = EntList[i];
			return LastEntry;
		}
	}

	if (Create) {
		LastEntry = new DPSEntry(szBuffer, this, PCOnly);
		EntList.push_back(LastEntry);
		return LastEntry;
	}
	return nullptr;
}

// ############################### CDPSAdvWnd START ############################################

CDPSAdvWnd::CDPSAdvWnd() :CCustomWnd("DPSAdvWnd") {
	// TODO:  This looks like it can all be done with WrongUI
	bool CheckUI = false;
	//Tabs holder exists?
	if (!(Tabs = (CTabWnd*)GetChildItem("DPS_Tabs"))) CheckUI = true;

	//DPS Settings Tab
	if (!(CShowMeTop = (CCheckBoxWnd*)GetChildItem("DPS_ShowMeTopBox"))) CheckUI = true;
	if (!(CShowMeMin = (CCheckBoxWnd*)GetChildItem("DPS_ShowMeMinBox"))) CheckUI = true;
	if (!(TShowMeMin = (CEditWnd*)GetChildItem("DPS_ShowMeMinInput"))) CheckUI = true;
	if (!(CUseRaidColors = (CCheckBoxWnd*)GetChildItem("DPS_UseRaidColorsBox"))) CheckUI = true;
	if (!(CLiveUpdate = (CCheckBoxWnd*)GetChildItem("DPS_LiveUpdateBox"))) CheckUI = true;
	if (!(CUseTBMKOutput = (CCheckBoxWnd*)GetChildItem("DPS_UseTBMKOutputBox"))) CheckUI = true;
	if (!(TFightIA = (CEditWnd*)GetChildItem("DPS_FightIAInput"))) CheckUI = true;
	if (!(TFightTO = (CEditWnd*)GetChildItem("DPS_FightTOInput"))) CheckUI = true;
	if (!(TEntTO = (CEditWnd*)GetChildItem("DPS_EntTOInput"))) CheckUI = true;
	if (!(THistoryLimit = (CEditWnd*)GetChildItem("DPS_HistoryLimitInput"))) CheckUI = true;
	if (!(CShowTotal = (CComboWnd*)GetChildItem("DPS_ShowTotal"))) CheckUI = true;
	//End DPS Settings Tab

	//DPS Tab
	if (!(LTopList = (CListWnd*)GetChildItem("DPS_TopList"))) CheckUI = true;
	if (!(CMobList = (CComboWnd*)GetChildItem("DPS_MobList"))) CheckUI = true;

	this->SetBGColor(0xFF000000);//Setting this here sets it for the entire window. Setting everything individually was blacking out the checkboxes!

	if (CheckUI) {
		WriteChatf("\ar[MQ2DPSAdv] Incorrect UI File in use. Please update to latest and reload plugin.");
		WrongUI = true;
	}
	else
	{
		WrongUI = false;

		LoadLoc();
		SetWndNotification(CDPSAdvWnd);
		CMobList->SetColors(0xFFCC3333, 0xFF666666, 0xFF000000);
		Tabs->UpdatePage();
		DrawCombo();
	}
}

CDPSAdvWnd::~CDPSAdvWnd() {}

void CDPSAdvWnd::DrawCombo() {
	int CurSel = CMobList->GetCurChoice();
	CMobList->DeleteAll();
	CHAR szTemp[MAX_STRING] = { 0 };
	// TODO:  There's probably a better way to do this since this code is duplicated, but this moves the previous ternaries into a more readable format.
	strcpy_s(szTemp, "<Target> ");
	if (CListType == CLISTTARGET && CurListMob != nullptr)
	{
		strcat_s(szTemp, CurListMob->Tag);
		strcat_s(szTemp, CurListMob->Name);
	}
	else
	{
		strcat_s(szTemp, "None");
	}
	CMobList->InsertChoice(szTemp);

	strcpy_s(szTemp, "<MaxDmg> ");
	if (CListType == CLISTMAXDMG && CurListMob != nullptr)
	{
		strcat_s(szTemp, CurListMob->Tag);
		strcat_s(szTemp, CurListMob->Name);
	}
	else
	{
		strcat_s(szTemp, "None");
	}
	CMobList->InsertChoice(szTemp);

	DPSMob* Mob = nullptr;
	int ListSize = 0;
	for (int i = 0; i < (int)MobList.size(); ++i) {
		Mob = MobList[i];
		if (Mob->SpawnType == 1 && Mob->Active && !Mob->IsPet() && !Mob->Mercenary) {
			ListSize++;
			sprintf_s(szTemp, "%s(%i) %s", Mob->Tag, ListSize, Mob->Name);
			CMobList->InsertChoice(szTemp);
		}
	}

	if (ListSize < 6) CMobList->InsertChoice("");

	if (CListType == CLISTTARGET) CMobList->SetChoice(0);
	else if (CListType == CLISTMAXDMG) CMobList->SetChoice(1);
	else CMobList->SetChoice(CurSel >= 0 ? CurSel : 0);
}

void CDPSAdvWnd::SetTotal(int LineNum, DPSMob* Mob) {
	CHAR szTemp[MAX_STRING] = { 0 };
	SetLineColors(LineNum, 0, true);
	//Index
	LTopList->SetItemText(LineNum, 0, &CXStr("-"));
	//Name
	LTopList->SetItemText(LineNum, 1, &CXStr("Total"));
	//Total
	sprintf_s(szTemp, "%llu", CurListMob->Damage.Total);
	if (!UseTBMKOutputs)
		PrettifyNumber(szTemp, sizeof(szTemp));
	else {
		MakeItTBMK(szTemp);
		//Need to turn the strings from like 125556 to 125.5k
	}
	LTopList->SetItemText(LineNum, 2, &CXStr(szTemp));
	//DPS
	sprintf_s(szTemp, "%llu", CurListMob->Damage.Total / (int)((CurListMob->Damage.Last - CurListMob->Damage.First) + 1));
	if (!UseTBMKOutputs)
		PrettifyNumber(szTemp, sizeof(szTemp));
	else {
		MakeItTBMK(szTemp);
		//Need to turn the strings from like 125556 to 125.5k
	}
	LTopList->SetItemText(LineNum, 3, &CXStr(szTemp));
	//Time
	sprintf_s(szTemp, "%I64is", CurListMob->Damage.Last - CurListMob->Damage.First);
	LTopList->SetItemText(LineNum, 4, &CXStr(szTemp));
	//This is where Percentage would go, if it wasn't always going to be 100%, if more columns are added, make sure to skip 5!
}

void MakeItTBMK(PCHAR szLine) {
	uint64_t value = _atoi64(szLine);
	if (value >= 1000000000000) {
		value = value / 100000000000;
		_i64toa_s(value, szLine, MAX_STRING, 10);
		std::string temp = szLine;
		temp.insert(temp.end() - 1, '.');
		sprintf_s(szLine, MAX_STRING, temp.c_str());
		strcat_s(szLine, MAX_STRING, " t");
	}
	else if (value >= 1000000000) {
		value = value / 100000000;
		_i64toa_s(value, szLine, MAX_STRING, 10);
		std::string temp = szLine;
		temp.insert(temp.end() - 1, '.');
		sprintf_s(szLine, MAX_STRING, temp.c_str());
		strcat_s(szLine, MAX_STRING, " b");
	}
	else if (value >= 1000000) {
		value = value / 100000;
		_i64toa_s(value, szLine, MAX_STRING, 10);
		std::string temp = szLine;
		temp.insert(temp.end() - 1, '.');
		sprintf_s(szLine, MAX_STRING, temp.c_str());
		strcat_s(szLine, MAX_STRING, " m");
	}
	else if (value >= 1000) {
		value = value / 100;
		_i64toa_s(value, szLine, MAX_STRING, 10);
		std::string temp = szLine;
		temp.insert(temp.end() - 1, '.');
		sprintf_s(szLine, MAX_STRING, temp.c_str());
		strcat_s(szLine, MAX_STRING, " k");
	}
}

void CDPSAdvWnd::DrawList(bool DoDead) {
	if (!CurListMob || (!DoDead && CurListMob->Dead)) return;
	int ScrollPos = LTopList->GetVScrollPos();
	int CurSel = LTopList->GetCurSel();
	CHAR szTemp[MAX_STRING] = { 0 };
	LTopList->DeleteAll();
	int i = 0, LineNum = 0, RankAdj = 0, ShowMeLineNum = 0;
	bool FoundMe = false, ThisMe = false;
	if (ShowTotal == TOTALABOVE) {
		LineNum = LTopList->AddString(CXStr(""), 0, 0, 0);
		SetTotal(LineNum, CurListMob);
		RankAdj++;
	}
	if (ShowMeTop) {
		ShowMeLineNum = LTopList->AddString(CXStr(""), 0, 0, 0);
		SetLineColors(ShowMeLineNum, 0, false, true);
		RankAdj++;
	}
	if (ShowTotal == TOTALSECOND) {
		LineNum = LTopList->AddString(CXStr(""), 0, 0, 0);
		SetTotal(LineNum, CurListMob);
		RankAdj++;
	}
	for (i = 0; i < (int)CurListMob->EntList.size(); i++) {
		DPSMob::DPSEntry* Ent = CurListMob->EntList[i];
		if (Ent->Damage.Total == 0) break;
		if (ShowMeTop && !strcmp(Ent->Name, ((PSPAWNINFO)pCharSpawn)->DisplayedName)) {
			if (!ShowMeMin || (LineNum - RankAdj + 1) > ShowMeMinNum) FoundMe = true;
			ThisMe = true;
		}
		else ThisMe = false;
		LineNum = LTopList->AddString(CXStr(""), 0, 0, 0);
		SetLineColors(LineNum, Ent);
		//Placement Index
		sprintf_s(szTemp, "%i", LineNum - RankAdj + 1);
		LTopList->SetItemText(LineNum, 0, &CXStr(szTemp));
		if (ThisMe) LTopList->SetItemText(ShowMeLineNum, 0, &CXStr(szTemp));
		sprintf_s(szTemp, "%s", Ent->Name);
		if (gAnonymize) {
			Anonymize(szTemp, 256, 2);
		}
		sprintf_s(szTemp, "%s%s%s", Ent->Mercenary ? "[M] " : "", szTemp, Ent->Pets ? "*" : "");
		LTopList->SetItemText(LineNum, 1, &CXStr(szTemp));
		if (ThisMe) LTopList->SetItemText(ShowMeLineNum, 1, &CXStr(szTemp));
		//Total Damage
		sprintf_s(szTemp, "%llu", Ent->Damage.Total);
		if (!UseTBMKOutputs)
			PrettifyNumber(szTemp, sizeof(szTemp));
		else {
			MakeItTBMK(szTemp);
			//Need to turn the strings from like 125556 to 125.5k
		}
		LTopList->SetItemText(LineNum, 2, &CXStr(szTemp));
		if (ThisMe) LTopList->SetItemText(ShowMeLineNum, 2, &CXStr(szTemp));
		//DPS
		char DPSoutput[MAX_STRING] = { 0 };
		sprintf_s(DPSoutput, "%llu", Ent->GetDPS());
		if (!UseTBMKOutputs)
			PrettifyNumber(DPSoutput, sizeof(DPSoutput));
		else {
			MakeItTBMK(DPSoutput);
			//Need to turn the strings from like 125556 to 125.5k
		}

		//SDPS (DPS over duration of entire fight?)
		char SDPSoutput[MAX_STRING] = { 0 };
		sprintf_s(SDPSoutput, "%llu", Ent->GetSDPS());
		if (!UseTBMKOutputs)
			PrettifyNumber(SDPSoutput, sizeof(SDPSoutput));
		else {
			MakeItTBMK(SDPSoutput);
			//Need to turn the strings from like 125556 to 125.5k
		}
		sprintf_s(szTemp, "%s (%s)", DPSoutput, SDPSoutput);
		LTopList->SetItemText(LineNum, 3, &CXStr(szTemp));
		if (ThisMe) LTopList->SetItemText(ShowMeLineNum, 3, &CXStr(szTemp));
		//Time
		sprintf_s(szTemp, "%I64is", Ent->Damage.Last - Ent->Damage.First);
		LTopList->SetItemText(LineNum, 4, &CXStr(szTemp));
		if (ThisMe) LTopList->SetItemText(ShowMeLineNum, 4, &CXStr(szTemp));
		//Percentage of Damage
		sprintf_s(szTemp, "%2.0f", (((float)Ent->Damage.Total / (float)CurListMob->Damage.Total) * 100.0f));
		strcat_s(szTemp, "%");
		LTopList->SetItemText(LineNum, 5, &CXStr(szTemp));
		if (ThisMe) LTopList->SetItemText(ShowMeLineNum, 5, &CXStr(szTemp));

	}
	if (ShowTotal == TOTALBOTTOM) {
		LineNum = LTopList->AddString(CXStr(""), 0, 0, 0);
		SetTotal(LineNum, CurListMob);
	}
	if (ShowMeTop && !FoundMe) LTopList->RemoveLine(ShowMeLineNum);
	LTopList->SetVScrollPos(ScrollPos);
	LTopList->CalculateFirstVisibleLine();
	LTopList->SetCurSel(CurSel);
}

void CDPSAdvWnd::SetLineColors(int LineNum, DPSMob::DPSEntry* Ent, bool Total, bool MeTop) {
	if (MeTop) {
		LTopList->SetItemColor(LineNum, 0, MeTopColor);
		LTopList->SetItemColor(LineNum, 1, MeTopColor);
		LTopList->SetItemColor(LineNum, 2, MeTopColor);
		LTopList->SetItemColor(LineNum, 3, MeTopColor);
		LTopList->SetItemColor(LineNum, 4, MeTopColor);
		LTopList->SetItemColor(LineNum, 5, MeTopColor);
	}
	else if (Total) {
		LTopList->SetItemColor(LineNum, 0, TotalColor);
		LTopList->SetItemColor(LineNum, 1, TotalColor);
		LTopList->SetItemColor(LineNum, 2, TotalColor);
		LTopList->SetItemColor(LineNum, 3, TotalColor);
		LTopList->SetItemColor(LineNum, 4, TotalColor);
		LTopList->SetItemColor(LineNum, 5, TotalColor);
	}
	else if (!strcmp(Ent->Name, ((PSPAWNINFO)pCharSpawn)->DisplayedName)) {
		LTopList->SetItemColor(LineNum, 0, MeColor);
		LTopList->SetItemColor(LineNum, 1, MeColor);
		LTopList->SetItemColor(LineNum, 2, MeColor);
		LTopList->SetItemColor(LineNum, 3, MeColor);
		LTopList->SetItemColor(LineNum, 4, MeColor);
		LTopList->SetItemColor(LineNum, 5, MeColor);
	}
	else if (UseRaidColors && Ent->Class && (Ent->SpawnType == SPAWN_PLAYER || Ent->Mercenary)) {
		LTopList->SetItemColor(LineNum, 0, NormalColor);
		LTopList->SetItemColor(LineNum, 1, ((PEQRAIDWINDOW)pRaidWnd)->ClassColors[ClassInfo[Ent->Class].RaidColorOrder]);
		LTopList->SetItemColor(LineNum, 2, NormalColor);
		LTopList->SetItemColor(LineNum, 3, NormalColor);
		LTopList->SetItemColor(LineNum, 4, NormalColor);
		LTopList->SetItemColor(LineNum, 5, NormalColor);
	}
	else {
		LTopList->SetItemColor(LineNum, 0, NormalColor);
		LTopList->SetItemColor(LineNum, 1, NPCColor);
		LTopList->SetItemColor(LineNum, 2, NormalColor);
		LTopList->SetItemColor(LineNum, 3, NormalColor);
		LTopList->SetItemColor(LineNum, 4, NormalColor);
		LTopList->SetItemColor(LineNum, 5, NormalColor);
	}
}

void CDPSAdvWnd::SaveLoc() {
	if (!GetCharInfo() || GetCharInfo()->Name[0] == '\0')
		return;

	CHAR szTemp[MAX_STRING] = { 0 };
	WritePrivateProfileString(GetCharInfo()->Name, "Saved", "1", INIFileName);
	sprintf_s(szTemp, "%i", GetLocation().top);
	WritePrivateProfileString(GetCharInfo()->Name, "Top", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetLocation().bottom);
	WritePrivateProfileString(GetCharInfo()->Name, "Bottom", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetLocation().left);
	WritePrivateProfileString(GetCharInfo()->Name, "Left", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetLocation().right);
	WritePrivateProfileString(GetCharInfo()->Name, "Right", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetAlpha());
	WritePrivateProfileString(GetCharInfo()->Name, "Alpha", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetFadeToAlpha());
	WritePrivateProfileString(GetCharInfo()->Name, "FadeToAlpha", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", CListType);
	WritePrivateProfileString(GetCharInfo()->Name, "CListType", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", LiveUpdate ? 1 : 0);
	WritePrivateProfileString(GetCharInfo()->Name, "LiveUpdate", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", IsVisible());
	WritePrivateProfileString(GetCharInfo()->Name, "Show", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", ShowMeTop ? 1 : 0);
	WritePrivateProfileString(GetCharInfo()->Name, "ShowMeTop", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", ShowMeMin ? 1 : 0);
	WritePrivateProfileString(GetCharInfo()->Name, "ShowMeMin", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", ShowMeMinNum);
	WritePrivateProfileString(GetCharInfo()->Name, "ShowMeMinNum", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", UseRaidColors ? 1 : 0);
	WritePrivateProfileString(GetCharInfo()->Name, "UseRaidColors", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", ShowTotal);
	WritePrivateProfileString(GetCharInfo()->Name, "ShowTotal", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", FightIA);
	WritePrivateProfileString(GetCharInfo()->Name, "FightIA", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", FightTO);
	WritePrivateProfileString(GetCharInfo()->Name, "FightTO", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", EntTO);
	WritePrivateProfileString(GetCharInfo()->Name, "EntTO", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", UseTBMKOutputs ? 1 : 0);
	WritePrivateProfileString(GetCharInfo()->Name, "UseTBMKOutputs", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", HistoryLimit);
	WritePrivateProfileString(GetCharInfo()->Name, "HistoryLimit", szTemp, INIFileName);

	//Save the column widths
	for (int i = 0; i <= 5; i++) {
		char szColumn[16] = { 0 };
		sprintf_s(szColumn, 16, "Column%iWidth", i);
		sprintf_s(szTemp, "%i", LTopList->GetColumnWidth(i));
		WritePrivateProfileString(GetCharInfo()->Name, szColumn, szTemp, INIFileName);
	}
}

void CDPSAdvWnd::LoadSettings() {
	CHAR szTemp[MAX_STRING] = { 0 };

	//Set the check boxes to their current settings.
	CShowMeTop->Checked = ShowMeTop ? 1 : 0;
	CShowMeMin->Checked = ShowMeMin ? 1 : 0;
	CUseRaidColors->Checked = UseRaidColors ? 1 : 0;
	CLiveUpdate->Checked = LiveUpdate ? 1 : 0;
	CUseTBMKOutput->Checked = UseTBMKOutputs ? 1 : 0;

	//Set the text in the Inputboxes to their current setting.
	sprintf_s(szTemp, "%i", ShowMeMinNum);
	SetCXStr(&TShowMeMin->InputText, szTemp);
	sprintf_s(szTemp, "%i", FightIA);
	SetCXStr(&TFightIA->InputText, szTemp);
	sprintf_s(szTemp, "%i", FightTO);
	SetCXStr(&TFightTO->InputText, szTemp);
	sprintf_s(szTemp, "%i", EntTO);
	SetCXStr(&TEntTO->InputText, szTemp);
	sprintf_s(szTemp, "%i", HistoryLimit);
	SetCXStr(&THistoryLimit->InputText, szTemp);

	//Clear and populate the ComboBox options for Show Total
	CShowTotal->DeleteAll();
	CShowTotal->InsertChoice("Don't Show Total");
	CShowTotal->InsertChoice("Above ShowMeTop");
	CShowTotal->InsertChoice("Below ShowMeTop");
	CShowTotal->InsertChoice("Show Bottom");
	CShowTotal->InsertChoice("");
	CShowTotal->SetChoice(ShowTotal);
	//Make Columns resizable.
	LTopList->SetColumnsSizable(true);
}

void CDPSAdvWnd::LoadLoc(char szChar[256]) {
	if (!GetCharInfo()) return;
	if (WrongUI)
		return;
	char szName[256] = { 0 };
	if (!szChar) strcpy_s(szName, GetCharInfo()->Name);
	else strcpy_s(szName, szChar);
	Saved = (GetPrivateProfileInt(szName, "Saved", 0, INIFileName) > 0 ? true : false);
	if (Saved) {
		SetLocation({
			(LONG)GetPrivateProfileInt(szName, "Left", 0, INIFileName),
			(LONG)GetPrivateProfileInt(szName, "Top", 0, INIFileName),
			(LONG)GetPrivateProfileInt(szName, "Right", 0, INIFileName),
			(LONG)GetPrivateProfileInt(szName, "Bottom", 0, INIFileName)
		});

		SetAlpha((BYTE)GetPrivateProfileInt(szName, "Alpha", 0, INIFileName));
		SetFadeToAlpha((BYTE)GetPrivateProfileInt(szName, "FadeToAlpha", 0, INIFileName));
	}
	CListType = GetPrivateProfileInt(szName, "CListType", 0, INIFileName);
	LiveUpdate = (GetPrivateProfileInt(szName, "LiveUpdate", 0, INIFileName) > 0 ? true : false);
	WarnedYHO = (GetPrivateProfileInt(szName, "WarnedYHO", 0, INIFileName) > 0 ? true : false);
	WarnedOHO = (GetPrivateProfileInt(szName, "WarnedOHO", 0, INIFileName) > 0 ? true : false);
	Debug = (GetPrivateProfileInt(szName, "Debug", 0, INIFileName) > 0 ? true : false);
	SetVisible((GetPrivateProfileInt(szName, "Show", 1, INIFileName) > 0 ? true : false));
	ShowMeTop = (GetPrivateProfileInt(szName, "ShowMeTop", 0, INIFileName) > 0 ? true : false);
	ShowMeMin = (GetPrivateProfileInt(szName, "ShowMeMin", 0, INIFileName) > 0 ? true : false);
	ShowMeMinNum = GetPrivateProfileInt(szName, "ShowMeMinNum", 0, INIFileName);
	UseRaidColors = (GetPrivateProfileInt(szName, "UseRaidColors", 0, INIFileName) > 0 ? true : false);
	ShowTotal = GetPrivateProfileInt(szName, "ShowTotal", 0, INIFileName);
	FightIA = GetPrivateProfileInt(szName, "FightIA", 8, INIFileName);
	FightTO = GetPrivateProfileInt(szName, "FightTO", 30, INIFileName);
	EntTO = GetPrivateProfileInt(szName, "EntTO", 8, INIFileName);
	MeColor = GetPrivateProfileInt(szName, "MeColor", 0xFF00CC00, INIFileName);
	MeTopColor = GetPrivateProfileInt(szName, "MeTopColor", 0xFF00CC00, INIFileName);
	NormalColor = GetPrivateProfileInt(szName, "NormalColor", 0xFFFFFFFF, INIFileName);
	NPCColor = GetPrivateProfileInt(szName, "NPCColor", 0xFFFFFFFF, INIFileName);
	TotalColor = GetPrivateProfileInt(szName, "TotalColor", 0xFF66FFFF, INIFileName);
	EntHover = GetPrivateProfileInt(szName, "EntHover", 0xFFCC3333, INIFileName);
	EntHighlight = GetPrivateProfileInt(szName, "EntHighlight", 0xFF666666, INIFileName);
	FightNormal = GetPrivateProfileInt(szName, "FightNormal", NormalColor, INIFileName);
	FightHover = GetPrivateProfileInt(szName, "FightHover", EntHover, INIFileName);
	FightHighlight = GetPrivateProfileInt(szName, "FightHighlight", EntHighlight, INIFileName);
	FightActive = GetPrivateProfileInt(szName, "FightActive", 0xFF00CC00, INIFileName);
	FightInActive = GetPrivateProfileInt(szName, "FightInActive", 0xFF777777, INIFileName);
	FightDead = GetPrivateProfileInt(szName, "FightDead", 0xFF330000, INIFileName);
	UseTBMKOutputs = (GetPrivateProfileInt(szName, "UseTBMKOutputs", 0, INIFileName) > 0 ? true : false);
	HistoryLimit = GetPrivateProfileInt(szName, "HistoryLimit", 25, INIFileName);

	//Restore the column widths
	for (int i = 0; i <= 5; i++) {
		char szTemp[16] = { 0 };
		sprintf_s(szTemp, 16, "Column%iWidth", i);
		int temp = GetPrivateProfileInt(szName, szTemp, 0, INIFileName);
		if (temp != 0) {
			LTopList->SetColumnWidth(i, temp);
		}
	}

	if (FightIA < 3) FightIA = 8;
	if (FightTO < 3) FightTO = 30;
	if (EntTO < 3) EntTO = 8;
	if (CListType > 1) CListType = CLISTTARGET;
	LTopList->SetColors(NormalColor, EntHover, EntHighlight);
	CMobList->SetChoice(CListType);
	LoadSettings();
}

int CDPSAdvWnd::WndNotification(CXWnd* pWnd, unsigned int Message, void* unknown) {
	if (Debug && Message != 21) WriteChatf("Notify: %i", Message);
	if (Message == XWM_CLOSE) CheckActive();
	if (Message == XWM_RCLICK && pWnd == (CXWnd*)LTopList) LTopList->SetCurSel(-1);
	else if (Message == XWM_CLOSE && pWnd == (CXWnd*)DPSWnd) CheckActive();
	else if (Message == XWM_LCLICK) {
		if (pWnd == (CXWnd*)Tabs) LoadSettings();
		else if (pWnd == (CXWnd*)CShowMeTop) ShowMeTop = CShowMeTop->Checked ? true : false;
		else if (pWnd == (CXWnd*)CShowMeMin) ShowMeMin = CShowMeMin->Checked ? true : false;
		else if (pWnd == (CXWnd*)CUseRaidColors) UseRaidColors = CUseRaidColors->Checked ? true : false;
		else if (pWnd == (CXWnd*)CLiveUpdate) LiveUpdate = CLiveUpdate->Checked ? true : false;
		else if (pWnd == (CXWnd*)CUseTBMKOutput) UseTBMKOutputs = CUseTBMKOutput->Checked ? true : false;
		//else if (pWnd == (CXWnd*)LTopList) WriteChatf("CurSel: %i", LTopList->GetCurSel());
		else if (pWnd == (CXWnd*)CShowTotal) {
			ShowTotal = CShowTotal->GetCurChoice();
			if (ShowTotal == 4) ShowTotal = 0;
			LoadSettings();
		}
		else if (pWnd == (CXWnd*)CMobList) {
			CurListMob = nullptr;
			LTopList->DeleteAll();
			bool FoundMob = false;
			if ((int)CMobList->GetCurChoice() > 1) {
				CListType = 2;
				DPSMob* ListMob = nullptr;
				int i = 0, x = 0;
				for (i = 0; i < (int)MobList.size() - 1; i++) {
					ListMob = MobList[i];
					if (ListMob->SpawnType == 1 && ListMob->Active && !ListMob->IsPet() && !ListMob->Mercenary) {
						if (x + 2 == (int)CMobList->GetCurChoice()) {
							FoundMob = true;
							ListSwitch(ListMob);
							break;
						}
						x++;
					}
				}
				if (!FoundMob) {
					CListType = 0;
					DPSWnd->DrawCombo();
				}
			}
			else CListType = (int)CMobList->GetCurChoice();
			Intervals -= 1; // Force update next Pulse.
		}
	}
	else if (Message == XWM_NEWVALUE) {
		CHAR szTemp[MAX_STRING] = { 0 };
		GetCXStr(((CEditWnd*)pWnd)->InputText, szTemp);
		if (pWnd == (CXWnd*)TShowMeMin) {
			if (strlen(szTemp)) {
				szTemp[2] = 0;
				ShowMeMinNum = atoi(szTemp);
				sprintf_s(szTemp, "%i", ShowMeMinNum);
				SetCXStr(&TShowMeMin->InputText, szTemp);
				TShowMeMin->SetSel(strlen(szTemp), 0);
			}
		}
		else if (pWnd == (CXWnd*)TFightIA) {
			if (strlen(szTemp)) {
				szTemp[2] = 0;
				FightIA = atoi(szTemp);
				if (FightIA < 3) FightIA = 8;
			}
		}
		else if (pWnd == (CXWnd*)TFightTO) {
			if (strlen(szTemp)) {
				szTemp[2] = 0;
				FightTO = atoi(szTemp);
				if (FightTO < 3) FightTO = 30;
			}
		}
		else if (pWnd == (CXWnd*)TEntTO) {
			if (strlen(szTemp)) {
				szTemp[2] = 0;
				EntTO = atoi(szTemp);
				if (EntTO < 3) EntTO = 8;
			}
		}
		else if (pWnd == (CXWnd*)THistoryLimit) {
			if (strlen(szTemp)) {
				HistoryLimit = atoi(szTemp);
				if (HistoryLimit < 1) HistoryLimit = 1;
			}
		}
	}

	return CSidlScreenWnd::WndNotification(pWnd, Message, unknown);
};

template <unsigned int _NameSize>DPSMob* GetMob(CHAR(&Name)[_NameSize], bool Create, bool Alive) {
	//ParsePet(Name);
	if (LastMob && (!Alive || (Alive && !LastMob->Dead)) && !_stricmp(LastMob->Name, Name))
		return LastMob;

	if (LastMob && LastMob->LastEntry && LastMob->LastEntry->DoSort) LastMob->LastEntry->Sort();
	// TODO: Range based for loop.
	for (int i = 0; i < (int)MobList.size(); i++) {
		if ((!Alive || (Alive && !MobList[i]->Dead)) && !_stricmp(MobList[i]->Name, Name)) {
			LastMob = MobList[i];
			return LastMob;
		}
	}
	if (Create) {
		LastMob = new DPSMob(Name, _NameSize);
		MobList.push_back(LastMob);
		return LastMob;
	}
	return nullptr;
}

template <unsigned int _EntSize, unsigned int _MobSize>bool SplitStringOtherHitOther(PCHAR Line, CHAR(&EntName)[_EntSize], CHAR(&MobName)[_MobSize], int* Damage) {
	if (strstr(Line, "injured by falling"))
		return false;
	int HitPos = 0, Action = 0;
	if (!strpbrk(Line, "1234567890"))
		return false;

	*Damage = atoi(strpbrk(Line, "1234567890"));
	while (OtherHits[Action]) {
		HitPos = (int)(strstr(Line, OtherHits[Action]) - Line);
		if (HitPos > 0)
			break;
		Action++;
	}
	if (HitPos <= 0 || HitPos > 2048) {
		if (*Damage > 0 && !WarnedOHO) {
			WriteChatf("\ar[MQ2DPSAdv] Error: Can not use Other Hits Other: Numbers Only Hitmodes for DPS Parsing.");
			if (Debug) WriteChatf("[Debug] OtherHitOther - Line: \ap%s", Line);
			WarnedOHO = true;
		}
		return false;
	}
	strncpy_s(EntName, &Line[0], HitPos);
	EntName[HitPos] = 0;
	int DmgPos = (int)(strstr(Line, " for ") - Line);
	int MobStart = HitPos + strlen(OtherHits[Action]);
	int MobLength = DmgPos - MobStart;
	strncpy_s(MobName, &Line[MobStart], MobLength);
	MobName[MobLength] = 0;
	if (!_stricmp(MobName, "himself") || !_stricmp(MobName, "herself") || !_stricmp(MobName, "itself")) strcpy_s(MobName, EntName);
	return true;
}

template <unsigned int _MobSize> bool SplitStringYouHitOther(PCHAR Line, CHAR(&MobName)[_MobSize], int* Damage)
{
	int Action = 0, HitPos = 0, DmgPos, MobStart, MobLength;
	if (!strpbrk(Line, "1234567890"))
		return false;

	*Damage = atoi(strpbrk(Line, "1234567890"));
	while (YourHits[Action]) {
		HitPos = (int)(strstr(Line, YourHits[Action]) - Line);
		if (HitPos >= 0) break;
		Action++;
	}
	if (HitPos < 0 || HitPos > 2048) {
		if (*Damage > 0 && CurTarMob) {
			strcpy_s(MobName, CurTarMob->Name);
			if (!WarnedYHO) {
				WriteChatf("\ay[MQ2DPSAdv] Warning: You Hit Other: Numbers Only may result in inaccuracy due to non-targetted attacks.");
				if (Debug) WriteChatf("[Debug] Line: %s", Line);
				WarnedYHO = true;
			}
			return true;
		}
		return false;
	}
	DmgPos = (int)(strstr(Line, " for ") - Line);
	MobStart = HitPos + strlen(YourHits[Action]);
	MobLength = DmgPos - MobStart;
	strncpy_s(MobName, &Line[MobStart], MobLength);
	MobName[MobLength] = 0;
	return true;
}

template <unsigned int _EntSize, unsigned int _MobSize>bool SplitStringNonMelee(PCHAR Line, CHAR(&EntName)[_EntSize], CHAR(&MobName)[_MobSize], int* Damage) {
	if (strstr(Line, "You were hit"))
		return false;
	if (!strpbrk(Line, "1234567890"))
		return false;
	if (strstr(Line, " healed "))
		return false;

	*Damage = atoi(strpbrk(Line, "1234567890"));

	char temp[MAX_STRING] = "";
	sprintf_s(temp, " for %i", *Damage);
	int MobEnd = (int)(strstr(Line, temp) - Line);
	//If this was a direct damage ability [EntName] hit [MobName] for [Damage] points of [DamageType] damage.
	if (MobEnd <= 0) {
		if (Debug) WriteChatf("NonMelee Failed, No MobEnd - Line: %s", Line);
	}
	if (MobEnd > 0) {
		if ((strstr(Line, ((PSPAWNINFO)pCharSpawn)->DisplayedName) || strstr(Line, "YOUR") || strstr(Line, " is "))) {
			sprintf_s(temp, ((PSPAWNINFO)pCharSpawn)->DisplayedName);
			if (strstr(Line, " YOUR ") || strstr(Line, " is ")) {
				//Process your damage shields.
				MobEnd = (int)(strstr(Line, " is ") - Line);
				sprintf_s(temp, " is ");
				strncpy_s(MobName, &Line[0], MobEnd);
			}
			else {
				//Normal Non-Melee
				strncpy_s(MobName, &Line[strlen(temp) + 5], MobEnd - strlen(temp) - 5);
			}
			if (strstr(Line, " is ") && !strstr(Line, " YOUR ")) {
				//Process other's Damage Shields.
				int EntEnd = 0;
				int EntStart = 0;
				if (strstr(Line, " pierced by ")) {
					//Thorns DS
					EntEnd = (int)(strstr(Line, "'s thorns for ") - Line);
					EntStart = (int)(strstr(Line, " pierced by ") - Line) + 12;
					strncpy_s(EntName, &Line[EntStart], EntEnd - EntStart);
				}
				else if (strstr(Line, " burned by ")) {
					//Fire DS
					EntEnd = (int)(strstr(Line, "'s flames for ") - Line);
					EntStart = (int)(strstr(Line, " burned by ") - Line) + 11;
					strncpy_s(EntName, &Line[EntStart], EntEnd - EntStart);
				}
				else if (strstr(Line, " tormented by ")) {
					//Frost DS
					EntEnd = (int)(strstr(Line, "'s frost for ") - Line);
					EntStart = (int)(strstr(Line, " tormented by ") - Line) + 14;
					strncpy_s(EntName, &Line[EntStart], EntEnd - EntStart);
				}
				if (EntEnd < 0 || EntStart < 0)
					return false;
			}
			else {
				strcpy_s(EntName, ((PSPAWNINFO)pCharSpawn)->DisplayedName);
			}
			if (!strlen(MobName) || !strlen(EntName)) {
				if (Debug) WriteChatf("NonMelee Fail: %s", Line);
				return false;
			}
			if (MobEnd > 0 && MobEnd < 256) {
				MobName[MobEnd] = 0;
				return true;
			}
			else {
				return false;
			}
		}
		sprintf_s(temp, " hit %s", MobName);
		int EntEnd = (int)(strstr(Line, temp) - Line);
		if (EntEnd <= 0) {
			if (Debug) WriteChatf("NonMelee Fail: %s", Line);
			return false;
		}
		strncpy_s(EntName, &Line[0], EntEnd);
		EntName[EntEnd] = 0;
		MobEnd = (int)(strstr(Line, " for ") - Line);
		int MobLength = MobEnd - 5 - strlen(EntName);
		strncpy_s(MobName, &Line[EntEnd + 5], MobLength);
		MobName[MobLength] = 0;
		if (!strlen(MobName) || !strlen(EntName)) {
			if (Debug) WriteChatf("NonMelee Fail: %s", Line);
			return false;
		}
		return true;
	}
	return false;
}

template <unsigned int _MobSize>bool SplitStringDeath(PCHAR Line, CHAR(&MobName)[_MobSize])
{
	int MobStart = 0, MobLength = 0;
	MobLength = (int)(strstr(Line, " died.") - Line);
	if (MobLength <= 0) MobLength = (int)(strstr(Line, " has been slain by") - Line);
	if (MobLength <= 0) {
		MobStart = 15;
		MobLength = strlen(Line) - 16;
	}
	if (MobLength <= 0)
		return false;
	strncpy_s(MobName, &Line[MobStart], MobLength);
	MobName[MobLength] = 0;
	return true;
}

template <unsigned int _EntSize, unsigned int _MobSize>bool SplitStringDOT(PCHAR Line, CHAR(&EntName)[_EntSize], CHAR(&MobName)[_MobSize], int* Damage) {
	if (!strpbrk(Line, "1234567890"))
		return false;
	if (strstr(Line, " healed "))
		return false;

	*Damage = atoi(strpbrk(Line, "1234567890"));
	if (*Damage <= 0 || strstr(Line, " damage by "))
		return false;
	int MobEnd = (int)(strstr(Line, " has taken ") - Line);

	if (strstr(Line, " damage from your ")) {
		strcpy_s(EntName, ((PSPAWNINFO)pCharSpawn)->DisplayedName);
	}
	else {
		int EntEnd = 0;
		if (!strstr(Line, "Rk. I")) {
			EntEnd = (int)(strstr(Line, ".") - Line - 6);
		}
		else {
			EntEnd = (int)(strrchr(Line, '.') - Line - 6);
		}
		if (MobEnd <= 0 || EntEnd <= 0) return false;
		int SpellNameStart = (int)(strstr(Line, " damage from ") - Line);
		int SpellNameEnd = (int)(strstr(Line, " by ") - Line);
		char temp[MAX_STRING] = "";
		strncpy_s(temp, &Line[SpellNameStart + 13], SpellNameEnd - SpellNameStart - 13);
		int EntStart = (int)(strstr(Line, temp) - Line + strlen(temp) + 4);
		strcpy_s(EntName, &Line[EntEnd, EntStart]);
		EntName[EntEnd - EntStart + 6] = 0;
	}
	strncpy_s(MobName, &Line[0], MobEnd);
	int Corpse = (int)(strstr(MobName, "'s corpse") - MobName);
	if (Corpse > 0)
		MobName[Corpse] = 0;
	else
		MobName[MobEnd] = 0;
	return true;
}

void AddMyDamage(char* EntName, int Damage1) {
	//if Me Do this
	uint64_t MyTotalBefore = MyTotal;
	uint64_t MyPetTotalBefore = MyPetTotal;
	time_t timenow = time(nullptr);
	int flag1 = 0;
	if ((!_stricmp(MyName, EntName) || !_stricmp(EntName, "You"))
		|| (strstr(EntName, "`s pet") && strstr(EntName, MyName))) {
		if (MyDebug) WriteChatf("[AddMyDamage] 1 -%s-", EntName);
		if (!MyFirst) MyFirst = timenow;
		MyLast = timenow;
		if (!_stricmp(MyName, EntName) || !_stricmp(EntName, "You")) {
			MyTotal += Damage1;
			flag1 = 1;
		}
		else {
			MyPetTotal += Damage1;
			flag1 = 2;
		}
	}
	// If I have a pet is it my pet
	else {
		PSPAWNINFO pSpawn = GetCharInfo()->pSpawn;
		if (pSpawn && pSpawn->PetID != -1) {
			int iPetID = pSpawn->PetID;
			if (MyDebug) WriteChatf("[AddMyDamage] 3 -%s- -%s- %i", EntName, MyName, iPetID);
			if (PSPAWNINFO dPet = (PSPAWNINFO)GetSpawnByID(iPetID)) {
				if (MyDebug) WriteChatf("[AddMyDamage] 4 -%s-", dPet->DisplayedName);
				if (!_stricmp(dPet->DisplayedName, EntName)) {
					if (MyDebug) WriteChatf("[AddMyDamage] 5 %i", Damage1);
					if (!MyFirst) MyFirst = timenow;
					MyLast = timenow;
					MyPetTotal += Damage1;
					flag1 = 3;
				}
			}
		}
	}
	if (MyDebug) WriteChatf("[AddMyDamage] %s did %i Damage  My B: %lu A: %lu  Pet B: %lu A: %lu Flag: %i", EntName, Damage1, MyTotalBefore, MyTotal, MyPetTotalBefore, MyPetTotal, flag1);
}

void HandleNonMelee(PCHAR Line) {
	CHAR EntName[256] = { 0 };
	CHAR MobName[256] = { 0 };
	int Damage;
	if (!SplitStringNonMelee(Line, EntName, MobName, &Damage))
		return;
	if (Debug) WriteChatf("[HandleNonMelee] \ap%s \awhit \ap%s \awfor \ar%i", EntName, MobName, Damage);
	if (Active) GetMob(MobName, true, true)->GetEntry(EntName)->AddDamage(Damage);
	if (MyActive) AddMyDamage(EntName, Damage);
}

void HandleDOT(PCHAR Line) {
	CHAR EntName[256] = { 0 };
	CHAR MobName[256] = { 0 };
	int Damage;
	if (!SplitStringDOT(Line, EntName, MobName, &Damage))
		return;
	if (Debug) WriteChatf("[HandleDot] \ap%s \awhit \ap%s \awfor \ar%i", EntName, MobName, Damage);
	if (Active) GetMob(MobName, true, true)->GetEntry(EntName)->AddDamage(Damage);
	if (MyActive) AddMyDamage(EntName, Damage);
}

void HandleOtherHitOther(PCHAR Line) {
	char EntName[256] = { 0 }, MobName[256] = { 0 };
	int Damage;
	if (!SplitStringOtherHitOther(Line, EntName, MobName, &Damage))
		return;
	if (Debug) WriteChatf("[OtherHitOther] \ap%s \awhit \ap%s \awfor \ar%i", EntName, MobName, Damage);
	if (Active) {
		if (DPSMob* mob = GetMob(MobName, true, true)) {
			if (DPSMob::DPSEntry* entry = mob->GetEntry(EntName)) {
				entry->AddDamage(Damage);
			}
		}
	}
	if (MyActive) AddMyDamage(EntName, Damage);
}

void HandleYouHitOther(PCHAR Line) {
	char MobName[256] = { 0 };
	int Damage;
	if (!SplitStringYouHitOther(Line, MobName, &Damage))
		return;
	if (Debug) WriteChatf("[YouHitOther] \apYou \awhit \ap%s \awfor \ar%i \awdamage!", MobName, Damage);
	if (pCharSpawn) {
		if (Active) {
			if (DPSMob* mob = GetMob(MobName, true, true)) {
				if (DPSMob::DPSEntry* entry = mob->GetEntry(((PSPAWNINFO)pCharSpawn)->DisplayedName)) {
					entry->AddDamage(Damage);
				}
			}
		}
		if (MyActive) AddMyDamage(MyName, Damage);
	}
}

void HandleDeath(PCHAR Line) {
	char MobName[256] = { 0 };
	if (!SplitStringDeath(Line, MobName)) return;
	if (Debug) WriteChatf("[HandleDeath] Death Name: \ap%s", MobName);
	if (Active) {
		if (DPSMob* DeadMob = GetMob(MobName, false, true)) {
			HandleDeath(DeadMob);
			if (!DeadMob->IsPet() && !DeadMob->Mercenary) {
				DPSWnd->DrawCombo();
			}
		}
	}
	if (MyActive) MyActive = false;
}

void HandleDeath(DPSMob* DeadMob) {
	if (DeadMob == CurListMob) {
		DPSWnd->DrawList();
		//CurListMob = 0;
	}
	if (DeadMob == CurTarMob) {
		CurTarMob = 0;
		CurTarget = 0;
	}
	if (DeadMob == CurMaxMob) {
		MaxDmgLast = (int)CurMaxMob->Damage.Last;
		CurMaxMob = 0;
	}
	sprintf_s(DeadMob->Tag, "[D] ");
	DeadMob->Dead = true;
}

void DPSAdvCmd(PSPAWNINFO pChar, PCHAR szLine) {
	char Arg1[MAX_STRING];
	GetArg(Arg1, szLine, 1);
	if (!_stricmp(Arg1, "show")) {
		if (!DPSWnd || WrongUI) {
			if (!DPSWnd) WriteChatf("\arDPSWnd does not exist. Try reloading your UI (/loadskin default).");
			if (WrongUI) WriteChatf("\arIncorrect UI File in use. Make sure you don't have a copy of MQUI_DPSAdvWnd stored in the EQ\\uifiles\\Default or in your custom UI Folder.");
		}
		else {
			DPSWnd->SetVisible(1);
		}
	}
	else if (!_stricmp(Arg1, "colors") && !WrongUI)
		((CXWnd*)pRaidOptionsWnd)->Show(1, 1);
	else if (DPSWnd && !_stricmp(Arg1, "reload") && !WrongUI)
		DPSWnd->LoadLoc();
	else if (DPSWnd && !_stricmp(Arg1, "save") && !WrongUI)
		DPSWnd->SaveLoc();
	else if (!_stricmp(Arg1, "listsize") && !WrongUI)
		WriteChatf("\ayMobList Size: %i", MobList.size());
	else if (!_stricmp(Arg1, "limit")) {
		GetArg(Arg1, szLine, 2);
		char *szEndPtr = nullptr;
		errno = 0;
		const int i = strtol(Arg1, &szEndPtr, 0);
		// If out of range || extra stuff left || no conversion
		if (errno == ERANGE || *szEndPtr != '\0' || Arg1 == szEndPtr || i <= 0) {
			WriteChatf("\ar[MQ2DPSAdv] Invalid history limit");
		}
		else {
			HistoryLimit = i;
			WriteChatf("[MQ2DPSAdv] History limit set to %s", Arg1);
		}
	}
	else if (!_stricmp(Arg1, "copy") && !WrongUI) {
		char szCopy[MAX_STRING];
		GetArg(szCopy, szLine, 2);
		if (DPSWnd) {
			DPSWnd->LoadLoc(szCopy);
			DPSWnd->SaveLoc();
		}
		else
			WriteChatf("\arFailed to Copy: DPS Window not loaded.");
	}
	else if (!_stricmp(Arg1, "Debug")) {
		Debug = Debug ? false : true;
		WriteChatf("Debug is now: %s", Debug ? "\agOn" : "\arOff");
	}
	else if (!_stricmp(Arg1, "MyDebug")) {
		MyDebug = MyDebug ? false : true;
		WriteChatf("MyDebug is now: %s", MyDebug ? "\agOn" : "\arOff");
	}
	else if (!_stricmp(Arg1, "MyStart")) {
		MyFirst = 0;
		MyLast = 0;
		MyTime = 0;
		MyTotal = 0;
		MyPetTotal = 0;
		MyDPSValue = 0;
		MyPetDPS = 0;
		TotalDPSValue = 0;
		MyActive = true;
	}
	else if (!_stricmp(Arg1, "MyStop")) {
		if (MyActive) {
			MyLast = time(nullptr);
			MyActive = false;
			MyTime = (MyLast - MyFirst);
			MyDPSValue = MyTime ? (float)MyTotal / MyTime : MyTotal;
			MyPetDPS = MyTime ? (float)MyPetTotal / MyTime : MyPetTotal;
			TotalDPSValue = MyTime ? (float)(MyTotal + MyPetTotal) / MyTime : MyTotal + MyPetTotal;
		}
	}
	else if (!_stricmp(Arg1, "MyReset")) {
		MyActive = false;
		MyFirst = 0;
		MyLast = 0;
		MyTime = 0;
		MyTotal = 0;
		MyPetTotal = 0;
		MyDPSValue = 0;
		MyPetDPS = 0;
		TotalDPSValue = 0;
	}
	else if (!_stricmp(Arg1, "tlo")) {
		DisplayHelp("tlo");
	}
	else {
		DisplayHelp("dps");
	}
	CheckActive();
}

void DisplayHelp(PCHAR hTemp) {
	if (!_stricmp(hTemp, "dps")) {
		WriteChatf("[DPSAdv] - Valid Commands are:");
		WriteChatf("[DPSAdv]     show - Opens DPSAdv Window.");
		WriteChatf("[DPSAdv]     colors - switch to Raid tab in Window.");
		WriteChatf("[DPSAdv]     reload - reload your ini settings.");
		WriteChatf("[DPSAdv]     save - Save your current settings to ini.");
		WriteChatf("[DPSAdv]     listsize - displays the number of mobs in the DPS list.");
		WriteChatf("[DPSAdv]     copy - Copy the DPS window.");
		WriteChatf("[DPSAdv]     debug - Toggles on/off debug messages.");
		WriteChatf("[DPSAdv]     mydebug - Toggles on/off myDPS debug messages");
		WriteChatf("[DPSAdv]     mystart - Starts My DPS capture process.");
		WriteChatf("[DPSAdv]     mystop - Stops my DPS capture process");
		WriteChatf("[DPSAdv]     myreset - Turns off my DPS capture and set myDPS totals to zero.");
		WriteChatf("[DPSAdv]     tlo - get a list of the DPSAdv TLO members.");
		WriteChatf("[DPSAdv]     limit - Limit the history of mobs in the DPS list.");
	}
	else if (!_stricmp(hTemp, "tlo")) {
		WriteChatf("[DPSAdv] - DPSAdv TLO Members:");
		WriteChatf("[DPSAdv]    MyDamage - My Damage as a number");
		WriteChatf("[DPSAdv]    PetDamage - My pet Damage as a number");
		WriteChatf("[DPSAdv]    TotalDamage - My Damage and Pet Damage together as a number");
		WriteChatf("[DPSAdv]    MyDPS - My DPS as a number");
		WriteChatf("[DPSAdv]    PetDPS - My pet DPS as a number");
		WriteChatf("[DPSAdv]    TotalDPS - My DPS and pet DPS together as a number");
		WriteChatf("[DPSAdv]    TimeElapsed - Number in seconds of time of fight");
	}
}

void CreateDPSWindow() {
	if (DPSWnd) DestroyDPSWindow();
	if (pSidlMgr->FindScreenPieceTemplate("DPSAdvWnd")) {
		DPSWnd = new CDPSAdvWnd();
		if (DPSWnd->IsVisible()) {
			((CXWnd*)DPSWnd)->Show(1, 1);
		}
		char szTitle[MAX_STRING];
		sprintf_s(szTitle, "DPS Advanced v%.1f", MQ2Version);
		DPSWnd->CSetWindowText(szTitle);
	}
	CheckActive();
}

void DestroyDPSWindow() {
	if (DPSWnd) {
		DPSWnd->SaveLoc();
		delete DPSWnd;
		DPSWnd = 0;
	}
	CheckActive();
}

// ***********************************************************************************************************
// Adding TLO for DPS Meter
// ***********************************************************************************************************

class MQ2DPSAdvType : public MQ2Type
{
public:
	enum DpsAdvMembers {
		MyDamage = 1,
		PetDamage,
		TotalDamage,
		MyDPS,
		PetDPS,
		TotalDPS,
		TimeElapsed,
		MyStatus,
		MyPetID
	};

	MQ2DPSAdvType() :MQ2Type("DPSAdv")
	{
		TypeMember(MyDamage);
		TypeMember(PetDamage);
		TypeMember(TotalDamage);
		TypeMember(MyDPS);
		TypeMember(PetDPS);
		TypeMember(TotalDPS);
		TypeMember(TimeElapsed);
		TypeMember(MyStatus);
		TypeMember(MyPetID);
	}

	~MQ2DPSAdvType()
	{
	}

	bool GetMember(MQ2VARPTR VarPtr, PCHAR Member, PCHAR Index, MQ2TYPEVAR& Dest) override
	{
		PMQ2TYPEMEMBER pMember = MQ2DPSAdvType::FindMember(Member);
		if (!pMember)
			return false;

		switch ((DpsAdvMembers)pMember->ID)
		{
		case MyDamage:
			Dest.Int64 = MyTotal;
			Dest.Type = pInt64Type;
			return true;

		case PetDamage:
			Dest.Int64 = MyPetTotal;
			Dest.Type = pInt64Type;
			return true;

		case TotalDamage:
			Dest.Int64 = MyTotal + MyPetTotal;
			Dest.Type = pInt64Type;
			return true;

		case MyDPS:
			if (MyActive) {
				if (MyTotal > 0 && MyFirst > 0) {
					MyTime = time(nullptr) - MyFirst;
					MyDPSValue = MyTime ? (float)MyTotal / MyTime : MyTotal;
				}
				else {
					MyTime = 0;
					MyDPSValue = 0;
				}
			}
			else {
				if (MyTotal > 0 && MyFirst > 0 && MyLast > 0) {
					MyTime = MyLast - MyFirst;
					MyDPSValue = MyTime ? (float)MyTotal / MyTime : MyTotal;
				}
				else {
					MyTime = 0;
					MyDPSValue = 0;
				}

			}

			Dest.Float = MyDPSValue;
			Dest.Type = pFloatType;
			return true;

		case PetDPS:
			if (MyActive) {
				if (MyPetTotal > 0 && MyFirst > 0) {
					MyTime = time(nullptr) - MyFirst;
					MyPetDPS = MyTime ? (float)MyPetTotal / MyTime : MyPetTotal;
				}
				else {
					MyTime = 0;
					MyPetDPS = 0;
				}
			}
			else {
				if (MyPetTotal > 0 && MyFirst > 0 && MyLast > 0) {
					MyTime = MyLast - MyFirst;
					MyPetDPS = MyTime ? (float)MyPetTotal / MyTime : MyPetTotal;
				}
				else {
					MyTime = 0;
					MyPetDPS = 0;
				}
			}

			Dest.Float = MyPetDPS;
			Dest.Type = pFloatType;
			return true;

		case TotalDPS:
			if (MyActive) {
				if (MyTotal > 0 && MyFirst > 0) {
					MyTime = time(nullptr) - MyFirst;
					TotalDPSValue = MyTime ? (float)(MyTotal + MyPetTotal) / MyTime : MyTotal + MyPetTotal;
				}
				else {
					MyTime = 0;
					TotalDPSValue = 0;
				}
			}
			else {
				if (MyTotal > 0 && MyFirst > 0 && MyLast > 0) {
					MyTime = MyLast - MyFirst;
					TotalDPSValue = MyTime ? (float)(MyTotal + MyPetTotal) / MyTime : MyTotal + MyPetTotal;
				}
				else {
					MyTime = 0;
					TotalDPSValue = 0;
				}
			}

			Dest.Float = TotalDPSValue;
			Dest.Type = pFloatType;
			return true;

		case TimeElapsed:
			if (MyActive) {
				if (MyFirst) {
					MyTime = time(nullptr) - MyFirst;
				}
				else {
					MyTime = 0;
				}
			}
			else if (MyFirst & MyLast) {
				MyTime = MyLast - MyFirst;
			}
			else {
				MyTime = 0;
			}
			Dest.UInt64 = static_cast<uint64_t>(MyTime);
			Dest.Type = pTimeStampType;
			return true;

		case MyStatus:
			if (MyActive) {
				Dest.Int = 1;
			}
			else {
				Dest.Int = 0;
			}
			Dest.Type = pIntType;

			return true;

		case MyPetID:
			PSPAWNINFO pSpawn = GetCharInfo()->pSpawn;
			if (pSpawn && pSpawn->PetID != -1)
			{
				Dest.Int = pSpawn->PetID;
				Dest.Type = pIntType;
				return true;
			}
			else {
				Dest.Int = 0;
				Dest.Type = pIntType;
				return true;
			}
		}
		return false;
	}

	bool ToString(MQ2VARPTR VarPtr, PCHAR Destination) override
	{
		strcpy_s(Destination, MAX_STRING, Active ? "TRUE" : "FALSE");
		return true;
	}

	bool FromData(MQ2VARPTR& VarPtr, MQ2TYPEVAR& Source) override
	{
		return false;
	}

	bool FromString(MQ2VARPTR& VarPtr, PCHAR Source) override
	{
		return false;
	}
};

MQ2DPSAdvType* pDpsAdvType = nullptr;


BOOL dataDPSAdv(PCHAR szName, MQ2TYPEVAR &Dest)
{
	Dest.DWord = 1;
	Dest.Type = pDpsAdvType;
	return true;
}

PLUGIN_API VOID SetGameState(DWORD GameState)
{
	if (GameState == GAMESTATE_INGAME) {
		if (!DPSWnd) CreateDPSWindow();
		// Name could change without the gamestate changing, but it's rare
		if (GetCharInfo()) {
			PSPAWNINFO pSpawn = GetCharInfo()->pSpawn;
			if (pSpawn) {
				strcpy_s(MyName, pSpawn->DisplayedName);
				return;
			}
		}
		// MyName is used for matching DPS and we don't want false positive matches.
		strcpy_s(MyName, "NameNotFound");
	}
}

PLUGIN_API VOID OnCleanUI()
{
	DestroyDPSWindow();
}

PLUGIN_API VOID OnReloadUI()
{
	if (gGameState == GAMESTATE_INGAME && pCharSpawn) CreateDPSWindow();
}

PLUGIN_API VOID InitializePlugin()
{
	LastMob = nullptr;
	CurTarget = nullptr;
	CurTarMob = nullptr;
	CurListMob = nullptr;
	CurMaxMob = nullptr;
	Zoning = false;
	ShowMeTop = false;
	WrongUI = false;
	// TODO:  Add as a resource to automatically unpack
	if (IsXMLFilePresent("MQUI_DPSAdvWnd.xml")) {
		AddXMLFile("MQUI_DPSAdvWnd.xml");
	} else {
		WriteChatf("[MQ2DPSAdv] Could not find MQUI_DPSAdvWnd.xml.  Please place in uifiles/default");
	}
	AddCommand("/dpsadv", DPSAdvCmd);
	// additions for DPS Meter
	pDpsAdvType = new MQ2DPSAdvType;
	AddMQ2Data("DPSAdv", dataDPSAdv);
	// Additions End
	CheckActive();
	MyActive = false;
}

PLUGIN_API VOID ShutdownPlugin()
{
	DestroyDPSWindow();
	RemoveCommand("/dpsadv");
	// additions for DPS Meter
	RemoveMQ2Data("DPSAdv");
	delete pDpsAdvType;
	// Additions End
}

PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
	if (gGameState != GAMESTATE_INGAME || !pCharSpawn) return 0;
	if (Active || MyActive) {
		if (Debug) {
			char szMsg[MAX_STRING] = { 0 };
			strcpy_s(szMsg, Line);
			if (strchr(szMsg, '\x12')) {
				// Message includes link (item tags/spell), must clean first
				const int len = strlen(szMsg);

				auto pszCleanOrg = std::make_unique<char[]>(len + 64);
				char* szClean = pszCleanOrg.get();

				strcpy_s(szClean, len + 64, szMsg);

				CXStr out;
#if defined(ROF2EMU) || defined(UFEMU)
				if (CXStr* str = CleanItemTags(&out, szClean)) {
#else
				if (CXStr* str = CleanItemTags(&out, szClean, false)) {
#endif
					GetCXStr(str->Ptr, szClean, len + 64);
				}
				strncpy_s(szMsg, szClean, MAX_STRING - 1);
			}
			WriteChatf("[\ar%i\ax]\a-t: \ap%s", Color, szMsg);
		}
		switch (Color) {
		case 0://Color: 0 - Task Updates

		case 2: //Color: 2 - Consider Message - Green

		case 4: //Color: 4 - Consider messages - Dark Blue

		case 6://Color: 6 - Consider message - Grey

		case 10://Color: 10 - Merchant says "Hello there, Soandso. How about a nice Iron Ration?"

		case 13://Color: 13 - Attack is on/off - Other invites to raid.

		case 15://Color: 15 - Help messages, Consider Message - Yellow (right click on the NPC to consider it, Targeted (NPC): a slatescale serpent, You have gained an ability point)

		case 18://Color: 18 - Consider message - Light blue

		case 20://Color: 20 - You regain some experience from resurrection.

		case 256://Color: 256 - Other player /say messages
		case 257://Color: 257 - Other /tell's you
		case 258://Color: 258 - Other tells /group
		case 259://Color: 259 - Guild Chat - Incoming
		case 260://Color: 260 - Other /ooc
		case 261://Color: 261 - Other /auction
		case 262://Color: 262 - Other /shout
		case 263://Color: 263 - other Emote commands/eat food messages (/cry, Chomp, chomp, chomp... Soandso takes a bite...etc)
		case 264://Color: 264 - You begin casting
			break;
		case 265://Color: 265 - You hit other (melee/melee crit)
			HandleYouHitOther(Line);
			break;
		case 266://Color: 266 - Other hits you
		case 267://Color: 267 - You miss other
		case 268://Color: 268 - Riposte/Dodge/Other Misses You
		case 269://Color: 269 - Announcement (GM Events etc)
		case 270://Color: 270 - You have become better at [Skill]! (Number)
		case 271://Color: 271 - Song wear off message?? (Ritual Scarification wear off message)
		case 272://Color: 272 - Pet thanks for stuff you give them?
		case 273://Color: 273 - Charged mercenary upkeep/Group Invites/Joins/Banker tells you welcome to bank.
		case 274://Color: 274 - Faction hits
		case 275://Color: 275 - [Merchant] tells you, that'll be [price] for the [item]
		case 276://Color: 276 - You sold [QTY] [Item Name] to [Player] for [Amount]/You Purchased [Item Name] from [NPC] for [Amount]
		case 277://Color: 277 - Your death message.
		case 278://Color: 278 - Other PC death message.
			break;
		case 279://Color: 279 - Others Hits Other
			HandleOtherHitOther(Line); //Other hits other
			break;
		case 280://Color: 280 - Other Misses Other
		case 281://Color: 281 - /who
		case 282://Color: 282 - Other /yell
			break;
		case 283://Color: 283 - Your Non-Melee spell dmg.
			HandleNonMelee(Line); //Your Non-melee
			break;
		case 284://Color: 284 - Spell wear off messages
		case 285://Color: 285 - Coin collection/Split
		case 286://Color: 286 - Loot Related (Master looter actions, you loot)
		case 287://Color: 287 - Dice Rolls
		case 288://Color: 288 - Other Starts Casting Messages
		case 289://Color: 289 - Target is out of range, get closer/Interupt/Fizzle/other spell failure
		case 290://Color: 290 - Channel Lists after logged in
		case 291://Color: 291 - Chat Channel 1 - Incoming
		case 292://Color: 292 - Chat Channel 2 - Incoming
		case 293://Color: 293 - Chat Channel 3 - Incoming
		case 294://Color: 294 - Chat Channel 4 - Incoming
		case 295://Color: 295 - Chat Channel 5 - Incoming
		case 296://Color: 296 - Chat Channel 6 - Incoming
		case 297://Color: 297 - Chat Channel 7 - Incoming
		case 298://Color: 298 - Chat Channel 8 - Incoming
		case 299://Color: 299 - Chat Channel 9 - Incoming
		case 300://Color: 300 - Chat Channel 10 - Incoming
			break;
		case 301://Color: 301 - Critical (YouHitOther)
			HandleYouHitOther(Line);
			break;
		case 302://Color: 302 - Critical Dot & Spells (Yours)
			// This requires checking both handle nonmelee and dot
			// Using both of these here can cause an error in debug reporting, but that is expected.
			HandleNonMelee(Line);
			HandleDOT(Line);
			break;
		case 303://Color: 303 - Errors (You must first click on the being you wish to attack, Can't hit them from here)
			break;
		case 305://Color: 305 - Other Pet Flurry (OtherHitOther)
			HandleOtherHitOther(Line);
			break;
		case 306://Color: 306 - Enrage (Showed for pet)
		case 307://Color: 307 - Your /say messages, mob advances messages.
		case 308://Color: 308 - You tell other.
		case 309://Color: 309 - Group conversation
		case 310://Color: 310 - Guild conversation - Outgoing
		case 311://Color: 311 - you /ooc
		case 312://Color: 312 - you /auction
		case 313://Color: 313 - you shout.
		case 314://Color: 314 - /emote messages
		case 315://Color: 315 - Chat Channel 1 - Outgoing
		case 316://Color: 316 - Chat Channel 2 - Outgoing
		case 317://Color: 317 - Chat Channel 3 - Outgoing
		case 318://Color: 318 - Chat Channel 4 - Outgoing
		case 319://Color: 319 - Chat Channel 5 - Outgoing
		case 320://Color: 320 - Chat Channel 6 - Outgoing
		case 321://Color: 321 - Chat Channel 7 - Outgoing
		case 322://Color: 322 - Chat Channel 8 - Outgoing
		case 323://Color: 323 - Chat Channel 9 - Outgoing
		case 324://Color: 324 - Chat Channel 10 - Outgoing

		case 327://Color: 327 - Any /rsay
			break;
		case 328://Color: 328 - My pet hits (OtherHitOther)
			HandleOtherHitOther(Line); // Your Pet
			break;
		case 329://Color: 329 - Damage Shield hits you.
		case 330://Color: 330 - Raid Role messages.
			break;
		case 331://Color: 331 - My Pet Flurry (Other Hit Other) My Pet
		case 332://Color: 332 - My Pet Critical (Other Hit Other)
			HandleOtherHitOther(Line);
			break;
		case 333://Color: 333 - Item Focus messages
		case 334://Color: 334 - You gain Experience messages
		case 335://Color: 335 - You have already finished collecting [Item].
			break;
		case 336://Color: 336 - Pet Non-Melee/Pet beings to cast
			HandleNonMelee(Line); //All pet non-melee (yours and others)
			break;
		case 337://Color: 337 - Pet messages (YourPet says, "Following you master.")

		case 339://Color: 339 - Strike thru (yours and others)
		case 340://Color: 340 - You are stunned/unstunned messages.

		case 342://Color: 342 - fellowship messages
		case 343://Color: 343 - corpse emote

		case 345://Color: 345 - Guild plants banner
		case 346://Color: 346 - Mercenary tells group

		case 348://Color: 348 - Achievement - you and other.
		case 349://Color: 349 - Achievement - Guildmate
			break;
		case 356://Color: 356 - Flurry (Yours)
			HandleYouHitOther(Line);
			break;
		case 358://Color: 358 - NPC Death message
			HandleDeath(Line); //NPC died is Color: 358 Your death is Color: 277(this was 278)
			break;
		case 360://Color: 360 - Other Rolls Dice.
		case 361://Color: 361 - Injured by falling (self)
		case 362://Color: 362 - Injured by falling (other)
			break;
		case 363://Color: 363 - Your damage shield
			//HandleNonMelee(Line); //Your Damage shield
		case 364://Color: 364 - Other's damage shield
			HandleNonMelee(Line); //Other's damage shield
			break;
		case 366://Color: 366 - Your [Spell Name] has been overwritten - Detrimental
		case 367://Color: 367 - Your [Spell Name] has been overwritten - Beneficial
		case 368://Color: 368 - You cannot use that command right now (clicking disc too fast)
		case 369://Color: 369 - You can use [Ability Name] again in [Time till you can use it again]
		case 370://Color: 370 - AA Reuse Timer failed
		case 371://Color: 371 - Destroy item message

		case 373://Color: 373 - Heal over time on other?
		case 374://Color: 374 - You heal other
		case 375://Color: 375 - Other buffs other/Other Heal Other
			break;
		case 376://Color: 376 - Your DoT's
			//HandleDOT(Line); //Your DoTs
		case 377://Color: 377 - Other's DoT's
			HandleDOT(Line); //Your DoTs
			break;
		case 378://Color: 378 - Song messages - Soandso begins to sing a song. <Selo's Sonata I>
			break;
		case 379://Color: 379 - Others Non-Melee
			HandleNonMelee(Line); //Others Non-melee
			break;
		case 380://Color: 380 - Your spell messages
			break;
		default:
			//Don't Know these, lets see what it is.
			if (Debug) WriteChatf("[\ar%i\ax]\a-t: \ap%s", Color, Line);
			break;
		}
	}
	return 0;
}

bool CheckInterval() {
	time_t timenow = time(nullptr);
	if (!Intervals)
		Intervals = timenow;
	else if (Intervals != timenow) {
		Intervals = timenow;
		return true;
	}
	return false;
}

void CheckActive() {
	if (DPSWnd && DPSWnd->IsVisible() && !Zoning && !WrongUI) Active = true;
	else Active = false;
}

void ListSwitch(DPSMob* Switcher) {
	CurListMob = Switcher;
	DPSWnd->LTopList->SetCurSel(-1);
	DPSWnd->LTopList->SetVScrollPos(0);
	DPSWnd->DrawList(true);
	DPSWnd->DrawCombo();
}

void TargetSwitch() {
	CurTarget = (PSPAWNINFO)pTarget;
	CurTarMob = GetMob(CurTarget->DisplayedName, true, CurTarget->Type == SPAWN_CORPSE ? false : true);
	//if (CurTarMob->Active && CurTarMob->SpawnType == 1) ListSwitch();
}

void IntPulse() {
	bool CChange = false;
	CurMaxMob = nullptr;
	const auto MobListSizeStart = MobList.size();
	for (int i = 0; i < (int)MobList.size(); i++) {
		DPSMob* Mob = MobList[i];
		if (Mob == nullptr // Somehow this was a null pointer
		        || (MobList.size() > static_cast<size_t>(HistoryLimit + 1) // We have exceeded the limit of mobs in the config (+1 due to display of the "Active" mob)
		                || (!Mob->Active || (Mob->IsPet() || Mob->SpawnType == SPAWN_PLAYER || Mob->Mercenary) && Mob->InActive)) // It's not active or it's a pet/player/merc that's inactive?
		            && Mob != CurTarMob && Mob != CurListMob && Mob != LastMob && Mob != CurMaxMob) // It's not one of our UI mobs
		{
			MobList.erase(MobList.begin() + i);
			delete Mob;
			i--;
		}
		else {
			const time_t timenow = time(nullptr);
			if (Mob->Active && !Mob->Dead && timenow - Mob->Damage.Last > FightTO) {
				HandleDeath(Mob);
				CChange = true;
			}
			else if (Mob->Active && !Mob->Dead && !Mob->InActive && timenow - Mob->Damage.Last > FightIA) {
				Mob->InActive = true;
				sprintf_s(Mob->Tag, "[IA] ");
				if (!Mob->IsPet() && !Mob->Mercenary) CChange = true;
			}
			else if (Mob->Active && !Mob->Dead && Mob->InActive && timenow - Mob->Damage.Last < FightIA) {
				Mob->InActive = false;
				sprintf_s(Mob->Tag, "[A] ");
				if (!Mob->IsPet() && !Mob->Mercenary) CChange = true;
			}
			if (Mob->Active && !Mob->InActive && !Mob->Dead && !Mob->IsPet() && Mob->SpawnType == SPAWN_NPC && (int)Mob->Damage.Last > MaxDmgLast && (!CurMaxMob || Mob->Damage.Total > CurMaxMob->Damage.Total)) CurMaxMob = Mob;
		}
	}
	if (MobListSizeStart > MobList.size() || CListType == CLISTMAXDMG && CurMaxMob && CurMaxMob != CurListMob)
		ListSwitch(CurMaxMob);
	if (CChange) DPSWnd->DrawCombo();
	DPSWnd->DrawList();
	//WriteChatf("Active: %s", Active ? "Yes" : "No");
}

PLUGIN_API VOID OnPulse() {
	if (gGameState != GAMESTATE_INGAME || !pCharSpawn) return;

	if (Active && !WrongUI) {
		if (LastMob && LastMob->LastEntry && LastMob->LastEntry->DoSort) LastMob->LastEntry->Sort();
		if ((PSPAWNINFO)pTarget && (PSPAWNINFO)pTarget != CurTarget) TargetSwitch();
		if (CListType == CLISTTARGET && CurTarMob && CurTarMob != CurListMob && CurTarMob->Active && CurTarMob->SpawnType == 1) ListSwitch(CurTarMob);
		if (CheckInterval()) IntPulse();
	}
}

PLUGIN_API VOID OnRemoveSpawn(PSPAWNINFO pSpawn)
{
	if (!Zoning) {
		DPSMob* pMob = nullptr;
		DPSMob::DPSEntry* pEnt = nullptr;
		// TODO: Both of these should probably be range based for loops.
		for (int i = 0; i < (int)MobList.size(); i++) {
			pMob = MobList[i];
			if (pMob->Spawn && pMob->Spawn->SpawnID == pSpawn->SpawnID) pMob->Spawn = 0;
			for (int x = 0; x < (int)pMob->EntList.size(); x++) {
				pEnt = pMob->EntList[x];
				if (pEnt->Spawn && pEnt->Spawn->SpawnID == pSpawn->SpawnID) pEnt->Spawn = 0;
			}
		}
	}
}

void ZoneProcess() {
	LastMob = nullptr;
	CurTarget = nullptr;
	CurTarMob = nullptr;
	CurListMob = nullptr;
	CurMaxMob = nullptr;
	DPSMob* pMob = nullptr;
	DPSMob::DPSEntry* pEnt = nullptr;
	// TODO: Both of these should probably be range based for loops.
	for (int i = 0; i < (int)MobList.size(); i++) {
		pMob = MobList[i];
		pMob->Spawn = 0;
		for (int x = 0; x < (int)pMob->EntList.size(); x++) {
			pEnt = pMob->EntList[x];
			pEnt->Spawn = 0;
		}
		if (!pMob->Dead) HandleDeath(pMob);
	}
}

PLUGIN_API VOID OnBeginZone() {
	ZoneProcess();
	Zoning = true;
	CheckActive();
}

PLUGIN_API VOID OnEndZone() {
	Zoning = false;
	CheckActive();
}
