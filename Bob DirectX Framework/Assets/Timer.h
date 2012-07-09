
#pragma once

#include <windows.h>

class Timer
{
public:
	Timer();
	void StartWatch();
	void StopWatch();
	float GetTimeMilli() const;
private:
	float invFreqMilli;
	bool watchStopped;
	__int64 currentCount;
	__int64 startCount;
};