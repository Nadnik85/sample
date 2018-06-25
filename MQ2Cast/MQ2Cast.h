#ifndef MQ2CAST_H_
#define MQ2CAST_H_

#include "../MQ2Plugin.h"

// this is the ID that CastingData.SpellID returns if we aren't casting anything
#define NOID		-1

namespace MQ2Cast {

	extern bool DEBUGGING;

#pragma region Enums

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
		Stick = 0x01,
		FollowFlag = 0x02,
		Navigation = 0x04,
		FollowPath = 0x08,
		Path = 0x10,
		MoveUtils = 0x20
	};

	enum class CastRecast {
		Dead,
		Land,
		NTimes
	};

#pragma endregion

	/* v12.0 -- Here's how this plugin works.
	 *
	 * It's based on queuing commands in two separate queues. When the user commands something to be done, this plugin figures out what needs
	 * to be done and puts it in the appropriate queue. The first queue is the "command" queue, and is the most obvious -- it's where the actual
	 * actions that were commanded go, and can have return values and spell ID's to accompany them. The second queue is the "immediate" queue.
	 * It's where things that need to happen in order for the command to execute go. The best example of this is immobilize, where we put all of
	 * our possible immobilization actions into the immediate queue. The important bit is that the immediate queue is checked before the command
	 * queue is, so that we can do things like stop and interrupt. To that end, commands have pre- and post-run immediate actions that get
	 * embedded during construction of the command (the preQueue and postQueue). They do what they seem like they do, which is manipulate the
	 * immediate queue when the command comes up for execution, and when the command finishes execution. Immediate queue functions are also special
	 * in that they return booleans. The boolean that is returned evaluates whether or not the command is finished so that we can have more than
	 * just instant commands in that queue.
	 */

#pragma region Helpers

	void BlechSpell(PSPELL Cast);
	const std::string GetReturnString(const CastResult Result);

	// this function grabs commands registered by other plugins (including MQ2Main)
	const PMQCOMMAND FindMQCommand(const PCHAR Command);

	// this function only grabs the DWord result of the Type::Member
	const DWORD GetDWordMember(MQ2Type* Type, const PCHAR Member);

	// This is for direct keyboard/EQ command manipulation
	void SetEQInput(const DWORD Offset, const PCHAR Command, const char Value);

	// and this is to get the actual EQ command
	const fEQCommand FindEQCommand(const PCHAR Command);

	// this is for one-off uses because it must loop through all commands and strcmp every time
	void Execute(const PCHAR Command, const PCHAR zFormat, ...);

	// this is more efficient since you can store off the command
	void Execute(const fEQCommand Command, const PCHAR zFormat, ...);

#pragma endregion

#pragma region ImmediateCommand

	class ImmediateCommand {
	public:
		virtual bool execute() const = 0;
	};

	class ImmobilizeCommand : public ImmediateCommand {
	public:
		bool execute() const;
	};

	class RemobilizeCommand : public ImmediateCommand {
	public:
		bool execute() const;
	};

	class RecastCommand : public ImmediateCommand {
	public:
		const CastRecast Type;
		const int Times;

		RecastCommand(const CastRecast Type, const int Times) :
			ImmediateCommand(), Type(Type), Times(Times) {}

		bool execute() const;
	};

	class TargetCommand : public ImmediateCommand {
	public:
		const DWORD TargetID;

		TargetCommand(const DWORD TargetID) :
			ImmediateCommand(), TargetID(TargetID) {}

		bool execute() const;
	};

	class BandolierCommand : public ImmediateCommand {
	private:
		fEQCommand BandolierCmd;

	public:
		const PCHAR Command;
		const PCHAR Name;

		BandolierCommand(const PCHAR Command, const PCHAR Name);

		bool execute() const;
	};

	class InvisCommand : public ImmediateCommand {
	public:
		const bool CastWhileInvis;

		InvisCommand(const bool CastWhileInvis) :
			ImmediateCommand(), CastWhileInvis(CastWhileInvis) {}

		bool execute() const;
	};

	class StopCastCommand : public ImmediateCommand {
	private:
		fEQCommand StopSongCmd;
		fEQCommand StopCastCmd;

	public:
		StopCastCommand();

		bool execute() const;
	};

#pragma endregion

#pragma region CastCommand

	class CastCommand {
	protected:
		CastCommand(CastType Type, std::list<const ImmediateCommand*> PreQueue, std::list<const ImmediateCommand*> PostQueue) :
			Type(Type), PreQueue(PreQueue), PostQueue(PostQueue) {}

	public:
		// the type of thing to cast, so we know how to prep for it
		const CastType Type;

		// immediate actions to run before running the command -- not objects because we will use copy ctors to create the objects
		const std::list<const ImmediateCommand*> PreQueue;

		// immediate actions to run after running the command -- not objects because we will use copy ctors to create the objects
		const std::list<const ImmediateCommand*> PostQueue;

		// this is a std::function<void()> that will do all the things like set cast results and spell id's
		virtual void execute() const = 0;
	};

	class MemCommand : public CastCommand {
	private:
		const int GemSlot;
		const PSPELL Spell;

		MemCommand(const int GemSlot, const PSPELL Spell, const std::list<const ImmediateCommand*> PreQueue, const std::list<const ImmediateCommand*> PostQueue) :
			CastCommand(CastType::Memorize, PreQueue, PostQueue), GemSlot(GemSlot), Spell(Spell) {}

	public:
		static MemCommand* Create(
			const PCHAR ID,
			const int GemSlot,
			const std::list<const ImmediateCommand*> PreQueue = std::list<const ImmediateCommand*>(),
			const std::list<const ImmediateCommand*> PostQueue = std::list<const ImmediateCommand*>()
		);

		void execute() const;
	};

	class SpellCommand : public CastCommand {
	private:
		const int GemSlot;
		const DWORD SpellID;
		const bool Wait;

		SpellCommand(
			const int GemSlot,
			const DWORD SpellID,
			const bool Wait,
			const std::list<const ImmediateCommand*> PreQueue,
			const std::list<const ImmediateCommand*> PostQueue
		) : CastCommand(CastType::Spell, PreQueue, PostQueue), GemSlot(GemSlot), SpellID(SpellID), Wait(Wait) {}

	public:
		static SpellCommand* Create(
			const PCHAR ID,
			const int GemSlot,
			const bool Wait = false,
			const std::list<const ImmediateCommand*> PreQueue = std::list<const ImmediateCommand*>(),
			const std::list<const ImmediateCommand*> PostQueue = std::list<const ImmediateCommand*>()
		);

		static long GetCooldown(int GemSlot);
		static int GetGem(LONG spellID);
		static int GetGem(PCHAR ID);

		void execute() const;
	};

	class AbilityCommand : public CastCommand {
	private:
		const DWORD AbilityIndex;

		AbilityCommand(const DWORD AbilityIndex, const std::list<const ImmediateCommand*> PreQueue, const std::list<const ImmediateCommand*> PostQueue) :
			CastCommand(CastType::Ability, PreQueue, PostQueue), AbilityIndex(AbilityIndex) {}

	public:
		static AbilityCommand* Create(
			const PCHAR ID,
			const std::list<const ImmediateCommand*> PreQueue = std::list<const ImmediateCommand*>(),
			const std::list<const ImmediateCommand*> PostQueue = std::list<const ImmediateCommand*>()
		);

		static DWORD FindAbilityIndex(PCHAR ID);

		void execute() const;
	};

	class DiscCommand : public CastCommand {
	private:
		const DWORD SpellID;
		fEQCommand DiscCmd;

		DiscCommand(const DWORD SpellID, const std::list<const ImmediateCommand*> PreQueue, const std::list<const ImmediateCommand*> PostQueue) :
			CastCommand(CastType::Discipline, PreQueue, PostQueue), SpellID(SpellID) {}

	public:
		static DiscCommand* Create(
			const PCHAR ID,
			const std::list<const ImmediateCommand*> PreQueue = std::list<const ImmediateCommand*>(),
			const std::list<const ImmediateCommand*> PostQueue = std::list<const ImmediateCommand*>()
		);

		static DWORD FindDiscID(PCHAR ID);
		static ULONG GetDiscTimer(PSPELL pSpell);

		void execute() const;
	};

	class AltAbilityCommand : public CastCommand {
	private:
		const DWORD AAIndex;
		const int Level;
		fEQCommand AltCmd;

		AltAbilityCommand(const DWORD AAIndex, const int Level, const std::list<const ImmediateCommand*> PreQueue, const std::list<const ImmediateCommand*> PostQueue) :
			CastCommand(CastType::AltAbility, PreQueue, PostQueue), AAIndex(AAIndex), Level(Level) {}

	public:
		static AltAbilityCommand* Create(
			const PCHAR ID,
			const std::list<const ImmediateCommand*> PreQueue = std::list<const ImmediateCommand*>(),
			const std::list<const ImmediateCommand*> PostQueue = std::list<const ImmediateCommand*>()
		);

		static std::pair<DWORD, int> GetAAIndex(PCHAR ID);

		void execute() const;
	};

	class ItemCommand : public CastCommand {
	private:
		const PCONTENTS Contents;

		ItemCommand(const PCONTENTS Contents, const std::list<const ImmediateCommand*> PreQueue, const std::list<const ImmediateCommand*> PostQueue) :
			CastCommand(CastType::Discipline, PreQueue, PostQueue), Contents(Contents) {}

	public:
		static ItemCommand* Create(
			const PCHAR ID,
			const std::list<const ImmediateCommand*> PreQueue = std::list<const ImmediateCommand*>(),
			const std::list<const ImmediateCommand*> PostQueue = std::list<const ImmediateCommand*>()
		);

		static PCONTENTS GetContents(PCHAR ID);

		void execute() const;
	};

#pragma endregion

#pragma region CastingState

	class CastingState {
	public:
		// let's make this a singleton, since it's stateful
		static CastingState& instance() {
			static CastingState instance;
			return instance;
		}

		CastingState(CastingState const&) = delete;
		void operator=(CastingState const&) = delete;

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

		// we need these to check if they are open
		CXWnd *TributeMasterWnd;
		CXWnd *GuildBankWnd;

		// the input command queue
		std::queue<const CastCommand*> cmdQueue;

		// the immediate queue (do these first)
		// these return boolean because we don't pop them until the return true (so we can execute on multiple pulses)
		// TODO: I feel like this needs to be a deque so we can prioritize things, but it can be changed to a queue later if that's not necessary
		std::deque<const ImmediateCommand*> immediateQueue;

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

		// queue functions
		void pushCmd(const CastCommand* cmd);
		int getCmdSize() { return cmdQueue.size(); }
		const CastCommand* getCmd();
		CastType getCmdType();
		void clearCmds();
		void popCmd();
		void doNext(const ImmediateCommand* cmd);
		void pushImmediate(const ImmediateCommand* cmd);
		bool doImmediate();

		DWORD setCurrentID(DWORD id) { CurrentID = id; LastTimestamp = 0;  return CurrentID; }
		DWORD getCurrentID() { return CurrentID; }

		CastResult getCurrentResult() { return CurrentResult; }
		CastResult setCurrentResult(CastResult result) { CurrentResult = result; LastTimestamp = 0; return CurrentResult; }

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

#pragma endregion
}

#endif // MQ2CAST_H_