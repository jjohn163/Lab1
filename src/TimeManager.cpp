#include "TimeManager.h"
#include <chrono>

using namespace std::chrono;

TimeManager* TimeManager::sInstance = NULL;


TimeManager * TimeManager::Instance()
{
	if (!sInstance) {
		sInstance = new TimeManager;
		sInstance->numberFrames = 5;
		sInstance->lastFrame = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		sInstance->sumFrames = 0;
	}
	return sInstance;
}


void TimeManager::Update()
{
	long long currentFrame = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	long long deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	sumFrames += deltaTime;
	frameTimes.push(deltaTime);
	if (frameTimes.size() > numberFrames) {
		sumFrames -= frameTimes.front();
		frameTimes.pop();
	}
}

float TimeManager::DeltaTime()
{
	return frameTimes.back()/1000.0;
}

float TimeManager::FrameRate()
{
	if (frameTimes.empty() || sumFrames == 0) {
		return 0;
	}
	return 1.0/((sumFrames/frameTimes.size())/1000.0);
}
