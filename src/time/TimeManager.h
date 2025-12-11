#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <wx/stopwatch.h>

class TimeManager
{
private:
	TimeManager() {}
	~TimeManager() {}

public:
	static double DeltaTime;
	static int    FPS;

private:
	static wxStopWatch deltaTimer;
	static wxStopWatch totalTimer;

public:
	static void Start();
	static long TimeElapsedMS();
	static void UpdateFPS();

};

#endif
