#include "MQ2Cast.h"

using namespace MQ2Cast;

#pragma region ImmobilizeCommand

bool ImmobilizeCommand::execute() const {
	auto pMyChar = GetCharInfo();
	if (!pMyChar) return true;

	// TODO: should bound the timeframe for stopping in case some unaccounted for plugin won't stop moving you
	// go through various known plugins that move your character and request that they pause while casting
	if (auto dtStick = FindMQ2DataType("Stick")) {
		auto pStickCommand = FindMQCommand("/stick");

		if (GetDWordMember(dtStick, "Active") && pStickCommand) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: Stick Pause Request.", MQGetTickCount64()); }
			CastingState::instance().addPauseMask(PauseCommand::Stick);
			pStickCommand->Function(pMyChar->pSpawn, "pause");
		}
	}

	//looks like AdvPath.inc is loaded... ok then, fine we will check its tlo...
	if (auto dvFollowFlag = FindMQ2DataVariable("FollowFlag")) {
		// assume this is a bool/int because it should be
		if (dvFollowFlag->Var.DWord) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: AdvPath Pause Request.", MQGetTickCount64()); }
			CastingState::instance().addPauseMask(PauseCommand::FollowFlag);
			NewVarset(pMyChar->pSpawn, "PauseFlag 1");
		}
	}

	if (auto dtNavigation = FindMQ2DataType("Navigation")) {
		auto pNavCommand = FindMQCommand("/nav");

		if (GetDWordMember(dtNavigation, "Active") && !GetDWordMember(dtNavigation, "Paused") && pNavCommand) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: MQ2Nav Pause Request.", MQGetTickCount64()); }
			CastingState::instance().addPauseMask(PauseCommand::Navigation);
			pNavCommand->Function(pMyChar->pSpawn, "pause");
		}
	}

	if (auto dtAdvPath = FindMQ2DataType("AdvPath")) {
		auto Paused = GetDWordMember(dtAdvPath, "Paused");

		auto pFollowCommand = FindMQCommand("/afollow");
		if (GetDWordMember(dtAdvPath, "Following") && !Paused && pFollowCommand) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: MQ2AdvPath Pause Request.", MQGetTickCount64()); }
			CastingState::instance().addPauseMask(PauseCommand::FollowPath);
			pFollowCommand->Function(pMyChar->pSpawn, "pause");
		}

		auto pPlayCommand	= FindMQCommand("/play");
		if (GetDWordMember(dtAdvPath, "Playing") && !Paused && pPlayCommand) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: MQ2AdvPath Pause Request.", MQGetTickCount64()); }
			CastingState::instance().addPauseMask(PauseCommand::Path);
			pPlayCommand->Function(pMyChar->pSpawn, "pause");
		}
	}

	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: Manual Stop Movement.", MQGetTickCount64()); }

	// This is basically MQ2MoveUtils stuff, but without all the fancy locking -- just stop moving, but don't prevent it
	SetEQInput(__pulAutoRun_x, "autorun", 0);
	SetEQInput(__pulForward_x, "forward", 0);
	SetEQInput(__pulBackward_x, "back", 0);
	SetEQInput(__pulStrafeRight_x, "strafe_right", 0);
	SetEQInput(__pulStrafeLeft_x, "strafe_left", 0);

	return CastingState::instance().isImmobile();
}

#pragma endregion

#pragma region RemobilizeCommand

bool RemobilizeCommand::execute() const {
	auto pMyChar = GetCharInfo();
	if (!pMyChar) return true;

	// go through various known plugins that move your character and request that they pause while casting
	if (CastingState::instance().isPaused(PauseCommand::Stick)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: Stick Unpause Request.", MQGetTickCount64()); }
		if (auto pStickCommand = FindMQCommand("/stick")) {
			pStickCommand->Function(pMyChar->pSpawn, "unpause");
		}
	}

	if (CastingState::instance().isPaused(PauseCommand::FollowFlag)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: AdvPath Unpause Request.", MQGetTickCount64()); }
		NewVarset(pMyChar->pSpawn, "PauseFlag 0");
	}

	if (CastingState::instance().isPaused(PauseCommand::Navigation)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: MQ2Nav Unpause Request.", MQGetTickCount64()); }
		if (auto pNavCommand = FindMQCommand("/nav")) {
			pNavCommand->Function(pMyChar->pSpawn, "pause");
		}
	}

	if (CastingState::instance().isPaused(PauseCommand::FollowPath)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: MQ2AdvPath Unpause Request.", MQGetTickCount64()); }
		if (auto pFollowCommand = FindMQCommand("/afollow")) {
			pFollowCommand->Function(pMyChar->pSpawn, "unpause");
		}
	}

	if (CastingState::instance().isPaused(PauseCommand::Path)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: MQ2AdvPath Unpause Request.", MQGetTickCount64()); }
		if (auto pPlayCommand = FindMQCommand("/play")) {
			pPlayCommand->Function(pMyChar->pSpawn, "unpause");
		}
	}

	CastingState::instance().clearPauseMask();
	return true; // this is an instant action
}

#pragma endregion

#pragma region RecastCommand

bool RecastCommand::execute() const {
	CastingState::instance().doRecast(Type, Times);
	return true;
}

#pragma endregion

#pragma region TargetCommand

bool TargetCommand::execute() const {
	if (auto target = (PSPAWNINFO)GetSpawnByID(TargetID)) {
		*(PSPAWNINFO*)ppTarget = target;
	}

	CastingState::instance().setOnTargetID(TargetID);

	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[TargetCommand]: Targeting ID %d.", MQGetTickCount64(), TargetID); }

	return true;
}

#pragma endregion

#pragma region BandolierCommand

BandolierCommand::BandolierCommand(const PCHAR Command, const PCHAR Name) :
	ImmediateCommand(), Command(Command), Name(Name) {
	BandolierCmd = FindEQCommand("/bandolier");
}

bool BandolierCommand::execute() const {
	Execute(BandolierCmd, "%s %s", Command, Name);

	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Casting]: Bandolier %s %s.", MQGetTickCount64(), Command, Name); }

	return true;
}

#pragma endregion

#pragma region InvisCommand

bool InvisCommand::execute() const {
	CastingState::instance().setCastWhileInvis(CastWhileInvis);
	return true;
}

#pragma endregion

#pragma region StopCastCommand

StopCastCommand::StopCastCommand() : ImmediateCommand() {
	StopTwistCmd = FindMQCommand("/stoptwist");
	StopSongCmd = FindEQCommand("/stopsong");
	StopCastCmd = FindEQCommand("/stopcast");
}

bool StopCastCommand::execute() const {
	auto pMyChar = GetCharInfo();
	if (!pMyChar) return true;

	if (CastingState::instance().isBard()) {
		if (CastingState::instance().isTwisting() && StopTwistCmd) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[StopCastCommand]: Twisting %d.", MQGetTickCount64(), StopTwistCmd); }
			StopTwistCmd->Function(pMyChar->pSpawn, "");
		}

		if (CastingState::instance().isCasting() && StopSongCmd) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[StopCastCommand]: Singing %d.", MQGetTickCount64(), StopSongCmd); }
			StopSongCmd(pMyChar->pSpawn, "");
		}

		return !gbInZone || !CastingState::instance().setDucking(CastingState::instance().isTwisting() || CastingState::instance().isCasting());
	} else {
		if (CastingState::instance().isCasting() && StopCastCmd) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[StopCastCommand]: Casting %d.", MQGetTickCount64(), StopCastCmd); }
			StopCastCmd(pMyChar->pSpawn, "");
		}
		return !gbInZone || !CastingState::instance().setDucking(CastingState::instance().isCasting());
	}
}

#pragma endregion
