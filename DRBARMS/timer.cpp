#include "timer.h"

Timer::Timer(){
	maxTimeAllowed = 0;
}

Timer::Timer(unsigned int mta){
	maxTimeAllowed = mta;
}


void Timer::start(){
	startTime = time(0);
}

bool Timer::isTimeOver(){
	if(maxTimeAllowed > 0)
		return (time(0) - startTime) > maxTimeAllowed;
	else 
		return false;
}

void Timer::setMaxTimeAllowed(unsigned int mta){
	maxTimeAllowed = mta;
}

int Timer::getElapsedTime(){
	if(maxTimeAllowed == 0)
		return 0;
	else
		return time(0) - startTime;
}

int Timer::getStartTime(){
	if(maxTimeAllowed == 0)
		return 0;
	else
		return startTime;
}

int Timer::getCurrentTime(){
	return time(0);
}

