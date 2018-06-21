#include "MQ2Cast.h"

using namespace MQ2Cast;

#pragma region ImmobilizeCommand

bool ImmobilizeCommand::execute() const {
	// TODO: should bound the timeframe for stopping in case some unaccounted for plugin won't stop moving you

	// go through various known plugins that move your character and request that they pause while casting
	if (Evaluate("${If[${Stick.Status.Equal[ON]},1,0]}")) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: Stick Pause Request.", MQGetTickCount64()); }
		CastingState::instance().addPauseMask(PauseCommand::Stick);
		Stick("pause");
	}

	//looks like AdvPath.inc is loaded... ok then, fine we will check its tlo...
	if (FindMQ2DataVariable("FollowFlag") && Evaluate("${If[${Bool[${FollowFlag}]},1,0]}")) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: AdvPath Pause Request.", MQGetTickCount64()); }
		CastingState::instance().addPauseMask(PauseCommand::FollowFlag);
		Execute("/varcalc PauseFlag 1");
	}

	if (FindMQ2DataType("Navigation") && Evaluate("${If[${Navigation.Active} && !${Navigation.Paused},1,0]}")) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: MQ2Nav Pause Request.", MQGetTickCount64()); }
		CastingState::instance().addPauseMask(PauseCommand::Navigation);
		Nav("pause");
	}

	if (FindMQ2DataType("AdvPath")) {
		if (Evaluate("${If[${AdvPath.Following} && !${AdvPath.Paused},1,0]}")) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: MQ2AdvPath Pause Request.", MQGetTickCount64()); }
			CastingState::instance().addPauseMask(PauseCommand::FollowPath);
			FollowPath("pause");
		}
		if (Evaluate("${If[${AdvPath.Playing} && !${AdvPath.Paused},1,0]}")) {
			if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: MQ2AdvPath Pause Request.", MQGetTickCount64()); }
			CastingState::instance().addPauseMask(PauseCommand::Path);
			Path("pause");
		}
	}

	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Immobilize]: Manual Stop Movement.", MQGetTickCount64()); }

	// This is basically MQ2MoveUtils stuff, but without all the fancy prevention. It's a one-shot and go kind of thing: no need to reload.
	auto pulAutoRun = (unsigned long *)FixOffset(__pulAutoRun_x);
	auto pulForward = (unsigned long *)FixOffset(__pulForward_x);
	auto pulBackward = (unsigned long *)FixOffset(__pulBackward_x);
	auto pulTurnRight = (unsigned long *)FixOffset(__pulTurnRight_x);
	auto pulTurnLeft = (unsigned long *)FixOffset(__pulTurnLeft_x);
	auto pulStrafeLeft = (unsigned long *)FixOffset(__pulStrafeLeft_x);
	auto pulStrafeRight = (unsigned long *)FixOffset(__pulStrafeRight_x);

	pKeypressHandler->CommandState[FindMappableCommand("autorun")] = 0;
	*pulAutoRun = 0;
	pKeypressHandler->CommandState[FindMappableCommand("strafe_left")] = 0;
	*pulStrafeLeft = 0;
	pKeypressHandler->CommandState[FindMappableCommand("strafe_right")] = 0;
	*pulStrafeRight = 0;
	pKeypressHandler->CommandState[FindMappableCommand("forward")] = 0;
	*pulForward = 0;
	pKeypressHandler->CommandState[FindMappableCommand("back")] = 0;
	*pulBackward = 0;

	return CastingState::instance().isImmobile();
}

#pragma endregion

#pragma region RemobilizeCommand

bool RemobilizeCommand::execute() const {
	// go through various known plugins that move your character and request that they pause while casting
	if (CastingState::instance().isPaused(PauseCommand::Stick)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: Stick Unpause Request.", MQGetTickCount64()); }
		Stick("unpause");
	}

	if (CastingState::instance().isPaused(PauseCommand::FollowFlag)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: AdvPath Unpause Request.", MQGetTickCount64()); }
		Execute("/varcalc PauseFlag 0");
	}

	if (CastingState::instance().isPaused(PauseCommand::Navigation)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: MQ2Nav Unpause Request.", MQGetTickCount64()); }
		Nav("pause");
	}

	if (CastingState::instance().isPaused(PauseCommand::FollowPath)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: MQ2AdvPath Unpause Request.", MQGetTickCount64()); }
		FollowPath("unpause");
	}

	if (CastingState::instance().isPaused(PauseCommand::Path)) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Remobilize]: MQ2AdvPath Unpause Request.", MQGetTickCount64()); }
		Path("unpause");
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

bool BandolierCommand::execute() const {
	Execute("/bandolier %s %s", Command, Name);

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

bool StopCastCommand::execute() const {
	if (CastingState::instance().isBard()) {
		if (CastingState::instance().isTwisting()) Execute("/stoptwist");
		if (CastingState::instance().isCasting()) Execute("/stopsong");
		return !gbInZone || !CastingState::instance().setDucking(CastingState::instance().isTwisting() || CastingState::instance().isCasting());
	} else {
		if (CastingState::instance().isCasting()) Execute("/stopcast");
		return !gbInZone || !CastingState::instance().setDucking(CastingState::instance().isCasting());
	}
}

#pragma endregion
