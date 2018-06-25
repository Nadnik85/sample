#include "MQ2Cast.h"

using namespace MQ2Cast;

void CastingState::pushCmd(const CastCommand* cmd) {
	if (!cmd) {
		if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Queue]: Null Command Passed to pushCmd.", MQGetTickCount64()); }
		return;
	}

	// if we have no commands on the queue, we need to push the pre-command actions here
	if (cmdQueue.empty()) {
		for (auto iter = cmd->PreQueue.begin(); iter != cmd->PreQueue.end(); ++iter) {
			pushImmediate(*iter);
		}
	}

	// last I heard, MSVC didn't do emplace very well, so just use push
	if (DEBUGGING) { WriteChatf("[%I64u] MQ2Cast:[Queue]: Pushing a Command of Type %d to the Queue.", MQGetTickCount64(), cmd->Type); }
	cmdQueue.push(cmd);
	updateTimeout();
}

const CastCommand* CastingState::getCmd() {
	if (cmdQueue.empty()) return nullptr;

	return cmdQueue.front();
}

CastType CastingState::getCmdType() {
	if (cmdQueue.empty() || !cmdQueue.front()) {
		return CastType::None;
	}

	return cmdQueue.front()->Type;
}

void CastingState::clearCmds() {
	while (!cmdQueue.empty()) {
		popCmd();
	}
}

void CastingState::popCmd() {
	// if we have an unexpected cast, we don't want to add things to the queue, but we still want this to clean up things
	if (getCurrentID() != NOID) {
		setLastID(getCurrentID());
		setLastResult(getCurrentResult());

		setOnTargetID(NOID);
		setCurrentID(NOID);
		setCurrentResult(CastResult::Idle);
	}

	if (!cmdQueue.empty()) {
		auto cmd = cmdQueue.front();

		if (cmd && !cmd->PostQueue.empty()) {
			for (auto iter = cmd->PostQueue.begin(); iter != cmd->PostQueue.end(); ++iter) {
				pushImmediate(*iter);
			}
		}

		cmdQueue.pop();
		if (cmd) delete cmd;
	}

	// if we have another command, push the pre-command actions here
	if (!cmdQueue.empty()) {
		if (auto cmd = cmdQueue.front()) {
			for (auto iter = cmd->PreQueue.begin(); iter != cmd->PreQueue.end(); ++iter) {
				pushImmediate(*iter);
			}
		}
	}
}

void CastingState::doNext(const ImmediateCommand* cmd) {
	immediateQueue.push_front(cmd);
}

void CastingState::pushImmediate(const ImmediateCommand* cmd) {
	immediateQueue.push_back(cmd);
}

// we need to perform the immediate queue front and if it finished (returns true) pop it.
bool CastingState::doImmediate() {
	if (immediateQueue.empty()) {
		return true;
	}

	auto cmd = immediateQueue.front();
	if (!cmd) {
		immediateQueue.pop_front();
		return true;
	}

	auto finished = cmd->execute();
	if (finished) {
		immediateQueue.pop_front();
		delete cmd;
	}

	return finished && immediateQueue.empty();
}
