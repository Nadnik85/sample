// MQ2XPTracker.cpp : A (hopefully) simple XP tracker (by Cr4zyb4rd)
//
// Version 2.1
//
// Loosely based on XPTracker.mac by Kambic
//
// Usage: /xptracker       - Display time tracking was started.
//        /xptracker reset - Reset all events and begin tracking as if plugin was just loaded
//		  /xptracker resetonzone - Toggle only. Should you reset Totals when you zone. Default is true. Value stored in MQ2XPTracker.ini
//        /xptracker total - Display total gains since tracking start
//        /xptracker quiet - Toggle output of tracking messages
//        /xpevents        - list the events/timestamps of the events we've tracked
//        /xpevents [#]    - lists the events tracked in the past [#] seconds
//        /xpaverage       - lists the average (mean) xp per-kill
//
// MQ2Data:
//
// xptracker XPTracker[xp|aa|laa|rlaa]
//
// members:
// float    Total         Total % gained since tracking started
// float    Average       Average gain per-change/kill (Everquest "points" format)
// float    AveragePct    Average gain per-change/kill as a %
// float    TimeToDing    Estimated hours until ding
//
// with no index:
// string   RunTime       Time since tracking started in HH:MM:SS format
// float    RunTimeHours  Time since tracking started in hours
// float    KillsPerHour  Estimated number of changes per hour
// float    Changes       Total number of changes tracked so far
//
// Changes:
//	11-22-20 -- CWTN
//		Refactor
//		Added /xptracker resetonzone
//		Added saving and loading of "resetonzone" and "quiet" options from/to MQ2XPTracker.ini

//  10-20-19 -- CWTN
//		Updated to remove LossFromDeath if statement and variables. Modified the formula to account for loss of level. When doing a += value, if the value is negative,
//			it actually subtracts the result. Allowing us to do the same math for loss from death and gain from kill. However, the formula required tweaking in the event
//			you lost a level from death.
//		Updated to add some additional color while I was in here to the /xp command for the averages and time to ding etc output.
//		Updated to change variable types to match each other so as to cutback on the multiple types of variables doing math with each other to reduce the chance of issues
//			in calculations.
//		Update to fix "/xp reset" not resetting Totals.
//		Potentially TODO: Update GetTickCount() to use GetTickCount64() as recommended by VS.

//  07-03-19 -- CWTN
//		Update to reflect gaining multiple AA's per kill.
//		Update to correct the AA Exp formula.
//		Update to fix indentation.
//		Update to add color to XP and AAXP output after each kill.
//		Update to change output format of XP and AAXP for each kill to show the AA earned as a number of actual AAs.

//  06-22-19 -- CWTN
//		Updated to follow new XP formula while preserving previous formula for RoF2EMU and UFEMU
//
//  02-20-14
//     Removed all leadership AA tracking. - dewey2461
//
//  10-31-04
//     Finished off(??) the MQ2Data, bumping the version number to 2.0
//
//  10-29-04
//     Finally added time-to-ding to /xpaverage.
//
//  10-23-04
//     Added a small fix to the "reset" option as a possible workaround for those with no HUD.
//     Disabled INI writing until something's actually done with the data.
//
//  09-21-04
//     Made XP check vs a local variable instead of having an extra DWORD in the struct
//
//  09-19-04
//     New changelog. Less clutter.  Meh.
//     Everything's done with the points straight from the client.  1-330 for xp/aa
//     and 1-330 for laa/rlaa.  Percentages are only displayed when needed in output.
//     No sense dealing with the issues in storing a float, and the possible rounding
//     errors etc.
//////////////////////////////////////////////////////////////////////////////

#include "../MQ2Plugin.h"

//////////////////////////////////////////////////////////////////////////////
// Change this if the plugin runs too slow or too fast.  It simply specifies
// how many MQ2 "pulses" to skip between experience checks.
const int SKIP_PULSES = 3;
//////////////////////////////////////////////////////////////////////////////
const int SECOND = 1000;
const int MINUTE = (60 * SECOND);
const int HOUR = (60 * MINUTE);
const int DAY = (24 * HOUR);

#include <list>

PreSetup("MQ2XPTracker");

#if defined(UFEMU) || defined(ROF2EMU)
	const long long XPTotalPerLevel = 330;
	const float XPTotalDivider = 3.30f;
#else
	const long long XPTotalPerLevel = 100000;
	const float XPTotalDivider = 1000.0f;
#endif

void VerifyINI(char* Section, char* Key, char* Default, char* ININame)
{
	char temp[MAX_STRING] = { 0 };
	if (GetPrivateProfileString(Section, Key, 0, temp, MAX_STRING, ININame) == 0)
	{
		WritePrivateProfileString(Section, Key, Default, ININame);
	}
}

enum XP_TYPES {
	Experience,
	AltExperience,
};

struct _expdata {
	long long Base;
	long long Gained;
	long long Total;
} TrackXP[4];

typedef struct _timestamp {
	SYSTEMTIME systime;
	uint64_t systicks;
} TIMESTAMP;

struct _XP_EVENT {
	long long xp;
	long long aa;
	TIMESTAMP Timestamp;
};

bool bTrackXP = false;
bool bDoInit = true;
bool bQuietXP = false;
bool bFirstCall = true;
bool bResetOnZone = true;
char szININame[MAX_PATH] = { 0 };
unsigned long PlayerLevel = 0;
unsigned long PlayerAA = 0;
TIMESTAMP StartTime;
std::list<_XP_EVENT> Events;
std::list<_XP_EVENT>::iterator pEvents;

unsigned long GetTotalAA()
{
	return GetCharInfo2()->AAPoints + GetCharInfo2()->AAPointsSpent;
}

//XP Gained this kill
float GetXPGainedPercent() {
	return (float)TrackXP[Experience].Gained / XPTotalDivider;
}

//XP Gained total.
float GetXPTotalPercent() {
	return (float)TrackXP[Experience].Total / XPTotalDivider;
}

//AA Gained this kill
float GetAAGainedTotal() {
	return (float)TrackXP[AltExperience].Gained / XPTotalPerLevel;
}

float GetAAGainedPercent() {
	return (float)TrackXP[AltExperience].Gained / XPTotalDivider;
}

//AA Gained total
float GetAATotalPercent() {
	return (float)TrackXP[AltExperience].Total / XPTotalPerLevel;
}

float GetAATotalGained() {
	return (float)TrackXP[AltExperience].Total / XPTotalDivider;
}

class MQ2XPTrackerType *pXPTrackerType=0;

class MQ2XPTrackerType : public MQ2Type
{
	private:
		int _id;
		struct {
			float xp;
			float aa;
		} Averages;

	public:
		enum XPTrackerMembers
		{
			Total=1,
			Average=2,
			AveragePct=3,
			TimeToDing=4,
			KillsPerHour=5,
			Changes=6,
			RunTime=7,
			RunTimeHours=8,
			PctExpPerHour=9,
		};

		MQ2XPTrackerType():MQ2Type("xptracker")
		{
			TypeMember(Total);
			TypeMember(Average);
			TypeMember(AveragePct);
			TypeMember(TimeToDing);
			TypeMember(KillsPerHour);
			TypeMember(Changes);
			TypeMember(RunTime);
			TypeMember(RunTimeHours);
			TypeMember(PctExpPerHour);
		}

		~MQ2XPTrackerType()
		{
		}

		void SetIndex(int id)
		{
			_id = id;
		}

		void GetAverages()
		{
			Averages.xp = 0;
			Averages.aa = 0;
			if (Events.empty())
				return;

			pEvents = Events.begin();
			int i = 0;

			while (pEvents!=Events.end()) {
				Averages.xp+=pEvents->xp;
				Averages.aa+=pEvents->aa;
				i++;
				pEvents++;
			}

			Averages.xp=Averages.xp/i;
			Averages.aa=Averages.aa/i;
		}

		float GetKPH()
		{
			if (Events.empty())
				return 0.0f;

			unsigned long Kills = Events.size();
			uint64_t RunningTime = GetTickCount64() - StartTime.systicks;
			float RunningTimeFloat = (float)RunningTime/HOUR;
			return (float)Kills/RunningTimeFloat;
		}

		float GetEPH(char* Type)
		{
			uint64_t RunningTime = GetTickCount64() - StartTime.systicks;
			float RunningTimeFloat = (float)RunningTime/HOUR;

			if(!strcmp(Type,"Experience"))
			{
				float TotalXP = GetXPTotalPercent();
				return (float)TotalXP/RunningTimeFloat;
			}

			if(!strcmp(Type,"AltExperience"))
			{
				float TotalXP = GetAATotalGained();
				return (float)TotalXP/RunningTimeFloat;
			}

			//Default is overall
			float TotalXP = GetXPTotalPercent() + GetAATotalGained();
			return (float)TotalXP / RunningTimeFloat;
		}

		char* GetRunTime(char* szTemp)
		{
			uint64_t RunningTime = GetTickCount64() - StartTime.systicks;
			uint64_t RunningTimeHours = RunningTime / HOUR;
			uint64_t RunningTimeMinutes = (RunningTime - (RunningTimeHours * HOUR)) / MINUTE;
			uint64_t RunningTimeSeconds = (RunningTime - (RunningTimeHours * HOUR + RunningTimeMinutes * MINUTE)) / SECOND;
			sprintf_s(szTemp, MAX_STRING, "%llu:%llu:%llu", RunningTimeHours, RunningTimeMinutes, RunningTimeSeconds);
			return szTemp;
		}

		bool GetMember(MQ2VARPTR VarPtr, char* Member, char* Index, MQ2TYPEVAR &Dest)
		{
			PMQ2TYPEMEMBER pMember = MQ2XPTrackerType::FindMember(Member);
			if (!pMember)
				return false;

			switch((XPTrackerMembers)pMember->ID)
			{
				case Total:
					switch (_id)
					{
						case 0:
							Dest.Float = GetXPTotalPercent() + (float)TrackXP[AltExperience].Total / XPTotalDivider ;
							break;
						case 1:
							Dest.Float = GetXPTotalPercent();
							break;
						case 2:
							Dest.Float = (float)TrackXP[AltExperience].Total / XPTotalDivider;
							break;
						default:
							return false;
					}

					Dest.Type=pFloatType;
					return true;
				case Average:
					GetAverages();
					switch (_id)
					{
						case 0:
							return false;
						case 1:
							Dest.Float = Averages.xp;
							break;
						case 2:
							Dest.Float = Averages.aa;
							break;
						default:
							return false;
					}

					Dest.Type = pFloatType;
					return true;
				case AveragePct:
					GetAverages();
					switch (_id)
					{
						case 1:
							Dest.Float = Averages.xp / XPTotalDivider;
							break;
						case 2:
							Dest.Float = Averages.aa / XPTotalDivider;
							break;
						default:
							return false;
					}

					Dest.Type = pFloatType;
					return true;
				case TimeToDing:
					long long needed;
					GetAverages();
					switch (_id)
					{
						case 1:
							needed = XPTotalPerLevel-GetCharInfo()->Exp;
							Dest.Float = (float)needed / (Averages.xp*GetKPH());
							break;
						case 2:
							needed = XPTotalPerLevel-GetCharInfo()->AAExp;
							Dest.Float= (float)needed / (Averages.aa*GetKPH());
							break;
						default:
							return false;
					}

					Dest.Type = pFloatType;
					return true;
				case KillsPerHour:
					if (_id)
						return false;

					Dest.Float = GetKPH();
					Dest.Type = pFloatType;
					return true;
				case Changes:
					if (_id)
						return false;

					Dest.Int = Events.size();
					Dest.Type = pIntType;
					return true;
				case RunTime:
					if (_id)
						return false;

					Dest.Ptr = GetRunTime(DataTypeTemp);
					Dest.Type = pStringType;
					return true;
				case RunTimeHours:
					if (_id)
						return false;

					Dest.Float = (float)((GetTickCount64() - StartTime.systicks) / HOUR);
					Dest.Type = pFloatType;
					return true;
				case PctExpPerHour:
					switch (_id)
					{
						case 0:
							Dest.Float = GetEPH("Overall");
							break;
						case 1:
							Dest.Float = GetEPH("Experience");
							break;
						case 2:
							Dest.Float = GetEPH("AltExperience");
							break;
						default:
							return false;
					}
					Dest.Type = pFloatType;
					return true;
			}
			return false;
		}

		bool ToString(MQ2VARPTR VarPtr, char* Destination)
		{
			if (bTrackXP)
				strcpy_s(Destination,MAX_STRING,"TRUE");
			else
				strcpy_s(Destination,MAX_STRING,"FALSE");
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

int dataXPTracker(char* szIndex, MQ2TYPEVAR &Ret)
{
	int id;
	if (!szIndex[0])
	{
		id = 0;
	}
	else if (IsNumber(szIndex))
	{
		id = atoi(szIndex);
	}
	else if (!_stricmp(szIndex,"xp"))
	{
		id = 1;
	}
	else if (!_stricmp(szIndex,"aa"))
	{
		id = 2;
	} else {
		return false;
	}

	pXPTrackerType->SetIndex(id);
	Ret.DWord = 1;
	Ret.Type = pXPTrackerType;
	return true;
}

void AddElement(long long Experience, long long AA)
{
	_XP_EVENT event;
	event.xp = Experience;
	event.aa = AA;
	event.Timestamp.systicks = GetTickCount64();
	::GetLocalTime(&event.Timestamp.systime);
	Events.push_back(event);
  /*   pEvents=Events.end();
  pEvents--;
  char szTemp1[MAX_STRING];
  char szTemp2[MAX_STRING];
  sprintf_s(szTemp1,"XPTrack%d",Events.size()+1);
  sprintf_s(szTemp2,"%d,%d,%d,%d,%d,%d,%d,%d,%d",
  pEvents->xp,
  pEvents->aa,
  pEvents->laa,
  pEvents->rlaa,
  pEvents->Timestamp.systime.wMonth,
  pEvents->Timestamp.systime.wDay,
  pEvents->Timestamp.systime.wHour,
  pEvents->Timestamp.systime.wMinute,
  pEvents->Timestamp.systime.wSecond );
  WritePrivateProfileString("MQ2XPTracker",szTemp1,szTemp2,INIFileName);
  */
}

void SetBaseValues()
{
	PCHARINFO pCharInfo = GetCharInfo();
	PCHARINFO2 pCharInfo2 = GetCharInfo2();
	TrackXP[Experience].Base = pCharInfo->Exp;

	if (bResetOnZone)
		TrackXP[Experience].Total = 0;

	TrackXP[AltExperience].Base = pCharInfo->AAExp;

	if (bResetOnZone)
		TrackXP[AltExperience].Total = 0;

	PlayerLevel = pCharInfo2->Level;
	PlayerAA = GetTotalAA();
}

void InitXP() {
	if (bDoInit) {
		SetBaseValues();
		if (bFirstCall) {
			Events.clear();
			::GetLocalTime(&StartTime.systime);
			StartTime.systicks = GetTickCount64();
			TrackXP[Experience].Total = 0;
			TrackXP[AltExperience].Total = 0;
			bFirstCall = false;
		}
		bDoInit = false;
		bTrackXP = true;
	}
}

bool CheckExpChange() {
	PCHARINFO pCharInfo = GetCharInfo();
	PCHARINFO2 pCharInfo2 = GetCharInfo2();
	long long Current = pCharInfo->Exp;

	if (Current != TrackXP[Experience].Base) {
		TrackXP[Experience].Gained = (pCharInfo2->Level == PlayerLevel ? Current - TrackXP[Experience].Base : (pCharInfo2->Level > PlayerLevel ? XPTotalPerLevel - TrackXP[Experience].Base + Current : TrackXP[Experience].Base - XPTotalPerLevel + Current));
		TrackXP[Experience].Total += TrackXP[Experience].Gained;
		TrackXP[Experience].Base = Current;
		PlayerLevel = pCharInfo2->Level;
		return true;
	}

	return false;
}

bool CheckAAChange()
{
	PCHARINFO pCharInfo = GetCharInfo();
	PCHARINFO2 pCharInfo2 = GetCharInfo2();
	unsigned long Current = pCharInfo->AAExp;

	if (Current!=TrackXP[AltExperience].Base) {
		TrackXP[AltExperience].Gained = (GetTotalAA() == PlayerAA ? Current - TrackXP[AltExperience].Base : Current - TrackXP[AltExperience].Base + ((GetTotalAA() - PlayerAA) * XPTotalPerLevel));
		TrackXP[AltExperience].Total += TrackXP[AltExperience].Gained;
		TrackXP[AltExperience].Base = Current;
		PlayerAA = GetTotalAA();
		return true;
	}

	return false;
}

void XPEventsCommand(PSPAWNINFO pChar, char* szLine)
{
	char szTemp[MAX_STRING];
	uint64_t TargetTick;
	GetArg(szTemp,szLine,1);

	if (!strlen(szTemp)) {
		TargetTick = GetTickCount64() - HOUR;
	} else {
		if (!IsNumber(szTemp)) {
			if (!_strnicmp(szTemp, "hour", 4)) {
				TargetTick = GetTickCount64() - HOUR;
			} else {
				WriteChatColor("/xpevents requires a numeric argument in seconds",CONCOLOR_RED);
				return;
			}
		}
		else {
			TargetTick = GetTickCount64() - (atoi(szTemp) * SECOND);
		}
	}

	if (Events.empty()) {
		WriteChatColor("MQ2XPTracker::No experience changes tracked");
		return;
	}

	WriteChatf("%d experiences changes tracked:",Events.size());
	pEvents = Events.begin();
	int i = 1;

	while (pEvents!=Events.end()) {
		if (pEvents->Timestamp.systicks > TargetTick) {
			sprintf_s(szTemp,"%02d:%02d:%02d",pEvents->Timestamp.systime.wHour,pEvents->Timestamp.systime.wMinute,pEvents->Timestamp.systime.wSecond);
			WriteChatf("%03d - %02.2f%%XP %02.2f%%AA %02.2f%% at %s (%d system ticks):", i, (float)pEvents->xp/XPTotalDivider, (float)pEvents->aa/XPTotalDivider, szTemp, pEvents->Timestamp.systicks);
		}

		pEvents++;
		i++;
	}
}

void XPTrackerCommand(PSPAWNINFO pChar, char* szLine)
{
	char szTemp[MAX_STRING];
	GetArg(szTemp,szLine,1);

	if (!_strnicmp(szTemp, "reset", 6)) {
		bDoInit=true;
		bFirstCall=true;
		WriteChatColor("MQ2XPTracker::XP tracking reset.");
		InitXP();
	}

	if (!_strnicmp(szTemp, "resetonzone", 12)) {
		bResetOnZone = !bResetOnZone;
		WritePrivateProfileString("General", "ResetOnZone", (bResetOnZone ? "true" : "false"), szININame);
		WriteChatf("MQ2XPTracker::Reset XP Tracking when zoning is now %s", (bResetOnZone ? "\agOn" : "\arOff"));
		return;
	}

	if (!_strnicmp(szTemp, "total", 6)) {
		WriteChatf("Total XP Gained (Normal/AA): %03.3f%%/%03.3f%%", GetXPTotalPercent(), GetAATotalGained());
		return;
	}

	if (!_strnicmp(szTemp,"quiet",6)) {
		bQuietXP = !bQuietXP;
		WriteChatf("MQ2XPTracker::Quiet mode %s", (bQuietXP ? "\agTrue" : "\arFalse"));
		WritePrivateProfileString("General", "Quiet", (bQuietXP ? "true" : "false"), szININame);
		return;
	}

	WriteChatf("MQ2XPTracker::XP tracking started at %02d:%02d:%02d (%d system ticks)",StartTime.systime.wHour,StartTime.systime.wMinute,StartTime.systime.wSecond,StartTime.systicks);
}

void XPAverageCommand(PSPAWNINFO pChar, char* szLine)
{
	float xp=0;
	float aa=0;

	char szTemp[MAX_STRING];
	GetArg(szTemp, szLine, 1);

	if (!_strnicmp(szTemp,"reset",5)) {
		XPTrackerCommand(pChar,szLine);
	}

	if (Events.empty()) {
		WriteChatColor("MQ2XPTracker::No experience changes tracked");
		return;
	}

	pEvents = Events.begin();
	int i = 0;

	while (pEvents!=Events.end()) {
		xp+=pEvents->xp;
		aa+=pEvents->aa;
		i++;
		pEvents++;
	}

	uint64_t RunningTime = GetTickCount64() - StartTime.systicks;
	uint64_t RunningTimeHours = RunningTime / HOUR;
	uint64_t RunningTimeMinutes = (RunningTime - (RunningTimeHours * HOUR)) / MINUTE;
	uint64_t RunningTimeSeconds = (RunningTime - (RunningTimeHours * HOUR + RunningTimeMinutes * MINUTE)) / SECOND;
	float RunningTimeFloat = (float)RunningTime / HOUR;
	float KPH = (float)i / RunningTimeFloat;
	WriteChatf("\a-tTotal run time: \ag%llu \a-thours \ag%llu \a-tminutes \ag%llu \a-tseconds\ax", RunningTimeHours, RunningTimeMinutes, RunningTimeSeconds);
	WriteChatf("\a-tAverage \atEXP \a-tper kill: \ag%02.3f%% \a-tper-hour: \ag%02.1f%%\ax", (float)(xp / XPTotalDivider) / i, (float)(xp / XPTotalDivider) / i * KPH);
	WriteChatf("\a-tAverage \atAAEXP \a-tper kill: \ag%02.3f%% \a-tper-hour: \ag%02.1f%%\ax", (float)(aa / XPTotalDivider) / i, (float)(aa / XPTotalDivider) / i * KPH);
	WriteChatf("\a-tAverage \ag%1.2f \a-tkills-per-hour", KPH);

	float perkill;
	float perhour;
	long long needed;

	if (xp)
	{
		needed = XPTotalPerLevel - GetCharInfo()->Exp;
		perkill = xp / i;
		perhour = perkill * KPH;
		WriteChatf("\ayEstimated time to \atLevel \ag%1.2f \ayhours", (float)needed / perhour);
	}

	if (aa)
	{
		needed = XPTotalPerLevel - GetCharInfo()->AAExp;
		perkill = aa / i;
		perhour = perkill * KPH;
		WriteChatf("\ayEstimated time to \atAA \ayding \ag%1.2f \ayhours", (float)needed / perhour);
	}
}


// Called once, when the plugin is to initialize
PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("Initializing MQ2XPTracker");

	AddCommand("/xpevents", XPEventsCommand);
	AddCommand("/xptracker", XPTrackerCommand);
	AddCommand("/xpaverage", XPAverageCommand);
	AddMQ2Data("XPTracker", dataXPTracker);
	pXPTrackerType = new MQ2XPTrackerType;

	//Load any stored options. Not character/server specific so can do this in the Init.
	char buffer[MAX_STRING] = { 0 };
	sprintf_s(szININame, MAX_PATH, "%s\\%s", gszINIPath, "MQ2XPTracker.ini");

	VerifyINI("General", "ResetOnZone", "true", szININame);
	GetPrivateProfileString("General", "ResetOnZone", "true", buffer, MAX_STRING, szININame);
	bResetOnZone = (!_stricmp(buffer, "true") ? true : false);

	VerifyINI("General", "Quiet", "false", szININame);
	GetPrivateProfileString("General", "Quiet", "false", buffer, MAX_STRING, szININame);
	bQuietXP = (!_stricmp(buffer, "true") ? true : false);
}

// Called once, when the plugin is to shutdown
PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("Shutting down MQ2XPTracker");
	Events.clear();
	RemoveCommand("/xpevents");
	RemoveCommand("/xptracker");
	RemoveCommand("/xpaverage");
	RemoveMQ2Data("XPTracker");

	delete pXPTrackerType;
}

PLUGIN_API void SetGameState(unsigned long GameState)
{
	DebugSpewAlways("MQ2XPTracker::SetGameState()");
	if (GameState != GAMESTATE_INGAME) {
		bTrackXP = false; // don't track while not in game
	} else {
		bDoInit = true;
		InitXP();
	}
}

PLUGIN_API void OnPulse()
{
	static int PulseDelay = 0;

	if (!bTrackXP || MQ2Globals::gGameState != GAMESTATE_INGAME || ++PulseDelay <= SKIP_PULSES)
		return;

	PulseDelay = 0;
	bool gainedxp = false;

	if (CheckExpChange()) {
		gainedxp = true;
		if (!bQuietXP){
			WriteChatf("\ayXP Gained: \ag%02.3f%% \aw|| \ayXP Total: \ag%02.3f%%", GetXPGainedPercent(), GetXPTotalPercent());
		}
	}

	if (GetCharInfo()->PercentEXPtoAA && CheckAAChange()) {
		gainedxp = true;
		if (!bQuietXP) {
			WriteChatf("\ayAA Gained: \ag%2.2f \aw(\at%02.3f%%\aw) \aw|| \ayAA Total: \ag%2.2f \aw(\at%02.3f%%\aw)", GetAAGainedTotal(), GetAAGainedPercent(), GetAATotalPercent(), GetAATotalGained());
		}
	}

	if (gainedxp)
		AddElement(TrackXP[Experience].Gained, TrackXP[AltExperience].Gained);

	return;
}