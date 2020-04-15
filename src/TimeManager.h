#pragma  once

#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <string>
#include <queue>
#include <chrono>

class TimeManager
{
	
public:
	static TimeManager* Instance();
	void Update();
	float DeltaTime();
	float FrameRate();

private:
	TimeManager::TimeManager() {};
	static TimeManager* sInstance;
	std::queue<long long> frameTimes;
	int numberFrames;
	long long lastFrame;
	long long sumFrames;
};

#endif // TIMEMANAGER_H