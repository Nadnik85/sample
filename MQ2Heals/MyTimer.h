#pragma once
#include "windows.h"

class MyTimer
{
private:
	bool mStarted;

	ULONGLONG mStart;
	ULONGLONG mEnd;
	ULONGLONG mElapsed;

	void Elapsed();

public:
	MyTimer();
	bool Running();
	void Start();
	void Reset();
	void Stop();
	int msElapsed();
};
