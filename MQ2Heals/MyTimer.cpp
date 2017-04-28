#include "MyTimer.h"

void MyTimer::Elapsed()
{
	if (mStarted)
	{
		mEnd = GetTickCount64();
		mElapsed = mEnd - mStart;
	}
}

MyTimer::MyTimer()
{
	Reset();
}

bool MyTimer::Running()
{
	return mStarted;
}

void MyTimer::Start()
{
	mStart = GetTickCount64();
	mStarted = true;
}

void MyTimer::Reset()
{
	mStarted = false;
	mStart = 0;
	mEnd = 0;
	mElapsed = 0;
}

void MyTimer::Stop()
{
	mStarted = false;
}

int MyTimer::msElapsed()
{
	Elapsed();
	return (int)mElapsed;
}
