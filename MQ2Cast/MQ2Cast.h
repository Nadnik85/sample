#pragma once

#ifndef MQ2CAST_H_
#define MQ2CAST_H_

// this is the ID that CastingData.SpellID returns if we aren't casting anything
#define NOID		-1

enum class CastResult {
	Idle,
	Memorizing,
	Casting,
	Success, // from here on down, the result is "complete" -- above this, we are still waiting for completion
	Interrupted,
	Resist,
	Collapse,
	Recover,
	Fizzle,
	Standing,
	Stunned,
	Invisible,
	NotReady,
	OutOfMana,
	OutOfRange,
	NoTarget,
	CannotSee,
	Components,
	Outdoors,
	TakeHold,
	Immune,
	Distracted,
	Cancelled,
	Unknown
};

enum class CastType {
	Item,
	AltAbility,
	Discipline,
	Ability,
	Spell,
	Memorize,
	None
};

enum class PauseCommand {
	Stick		= 0x01,
	FollowFlag	= 0x02,
	Navigation	= 0x04,
	FollowPath	= 0x08,
	Path		= 0x10,
	MoveUtils	= 0x20
};

enum class CastRecast {
	Dead,
	Land,
	NTimes
};

class CastingState {
public:
	typedef std::function<void()> MQ2CastFunc;
	typedef std::function<bool()> ImmediateCommand;

	class MQ2CastCommand {
	public:
		CastType castType;
		MQ2CastFunc castFunc;
		std::queue<ImmediateCommand> preQueue;
		std::queue<ImmediateCommand> postQueue;

		MQ2CastCommand(CastType cType, MQ2CastFunc cFunc, std::queue<ImmediateCommand> preQ = std::queue<ImmediateCommand>(), std::queue<ImmediateCommand> postQ = std::queue<ImmediateCommand>()) {
			castType = cType;
			castFunc = cFunc;
			preQueue = preQ;
			postQueue = postQ;
		}
	};

protected:
	// options
	bool        CastWhileInvis;	// Invisibility Check?

	// state variables
	bool        Immobile;		// Immobile?
	ULONGLONG   ImmobileTime;	// Estimate when it be immobilized!
	UINT		PauseMask;		// Bitmask to know what was paused when it comes to unpause
	bool		Ducking;		// Are we currently interrupting a cast?
	CastRecast	RecastType;		// Are we doing recasts?
	UINT		RecastTimes;	// how many times to recast or attempt to recast

	// casting state variables
	DWORD		CurrentID;		// Casting Current ID
	CastResult	CurrentResult;	// Casting Current Result
	DWORD		LastID;			// Casting LastOne ID
	CastResult	LastResult;		// Casting LastOne Result
	ULONGLONG	Timeout;		// Casting Timeout
	DWORD		LastTimestamp;	// Last Timestamp of Pulse
	DWORD		OnTargetID;		// Casting OnTarget

	// mem variables
	//SPELLFAVORITE CastSpellFavorite;

	// we need these to check if they are open
	CXWnd *TributeMasterWnd;
	CXWnd *GuildBankWnd;

	// the input command queue
	std::queue<MQ2CastCommand> cmdQueue;

	// the immediate queue (do these first)
	// these return boolean because we don't pop them until the return true (so we can execute on multiple pulses)
	// TODO: I feel like this needs to be a deque so we can prioritize things, but it can be changed to a queue later if that's not necessary
	std::deque<ImmediateCommand> immediateQueue;

	// helper functions
	PCONTENTS getContentsFromID(PCHAR ID);
	std::pair<DWORD, int> getAAIndexFromID(PCHAR ID);
	DWORD getSpellIDFromDiscID(PCHAR ID);
	ULONG getDiscTimer(PSPELL pSpell);
	DWORD getAbilityIDFromID(PCHAR ID);

public:
	CastingState() {
		Reset();
	}

	void Reset() {
		CastWhileInvis = true;

		Immobile = false;
		ImmobileTime = 0;
		PauseMask = 0x00;
		Ducking = false;
		RecastType = CastRecast::NTimes;
		RecastTimes = 1;


		CurrentID = NOID;
		CurrentResult = CastResult::Idle;
		LastID = NOID;
		LastResult = CastResult::Idle;
		Timeout = 0;
		LastTimestamp = 0;
		OnTargetID = NOID;

		ReloadUI();

	}

	void ReloadUI() {
		TributeMasterWnd = nullptr;
		GuildBankWnd = nullptr;
	}

	bool setCastWhileInvis(bool doInvis) { CastWhileInvis = doInvis; return CastWhileInvis; }
	bool getCastWhileInvis() { return CastWhileInvis; }

	bool isImmobile();
	bool isBard();
	bool isTwisting();
	bool isCasting() { return NOID != detectSpellCast(); }
	bool isWindowOpen();
	bool hasCastBar();

	DWORD detectSpellCast();

	CastType findCastableType(PCHAR ID);
	bool isCastableReady(PCHAR ID, CastType cType);
	bool isCastableReady(PCHAR ID);

	// use these function generators to prettify the binding of functions when we push commands
	// essentially so we aren't always calling `pushCmd(Type, std::bind(Func, args...));`
	std::function<void()> castItem(PCHAR ID);
	std::function<void()> castAltAbility(PCHAR ID);
	std::function<void()> castDisc(PCHAR ID);
	std::function<void()> castAbility(PCHAR ID);
	std::function<void()> castSpell(PCHAR ID, int gemSlot = -1);
	std::function<void()> memSpell(PCHAR ID, int gemSlot);

	bool doImmobilize();
	bool doRemobilize();

	void addPauseMask(PauseCommand pCmd) { PauseMask |= (UINT)pCmd; }
	bool isPaused() { return PauseMask != 0x00; }
	bool isPaused(PauseCommand pCmd) { return (PauseMask & (UINT)pCmd) != 0x00; }
	void clearPauseMask() { PauseMask = 0x00; }

	bool setDucking(bool ducking) { Ducking = ducking; return Ducking; }
	bool isDucking() { return Ducking; }

	std::pair<CastRecast, UINT> doRecast(CastRecast rcType, UINT nTimes) { RecastType = rcType; RecastTimes = nTimes; return std::make_pair(RecastType, RecastTimes); }
	std::pair<CastRecast, UINT> getRecast() { return std::make_pair(RecastType, RecastTimes); }
	int decRecast() { return --RecastTimes; }

	long castTimeRemaining();
	long cooldownTimeRemaining(int gemIdx);

	int getGem(LONG spellID);
	int getGem(PCHAR ID);

	// queue functions
	void pushCmd(MQ2CastCommand cmd);
	void pushCmd(CastType type, MQ2CastFunc func);
	void pushCmd(CastType type, CastingState::MQ2CastFunc func, std::queue<ImmediateCommand> preCmd, std::queue<ImmediateCommand> postCmd);
	int getCmdSize();
	MQ2CastCommand getCmd();
	CastType getCmdType();
	void clearCmds();
	void popCmd();
	void doNext(ImmediateCommand cmd);
	void pushImmediate(ImmediateCommand cmd);
	bool doImmediate();
	
	DWORD setCurrentID(DWORD id) { CurrentID = id; return CurrentID; }
	DWORD getCurrentID() { return CurrentID; }

	CastResult getCurrentResult() { return CurrentResult; }
	CastResult setCurrentResult(CastResult result) { CurrentResult = result; return CurrentResult; }

	DWORD setLastID(DWORD id) { LastID = id; return LastID; }
	DWORD getLastID() { return LastID; }

	CastResult getLastResult() { return LastResult; }
	CastResult setLastResult(CastResult result) { LastResult = result; return LastResult; }

	ULONGLONG getTimeout() { return Timeout; }
	ULONGLONG updateTimeout();

	bool shouldPulse();

	DWORD getOnTargetID() { return OnTargetID; }
	DWORD setOnTargetID(DWORD id) { OnTargetID = id; return OnTargetID; }
};

#endif // MQ2CAST_H_