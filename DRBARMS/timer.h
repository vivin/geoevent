#ifndef TIMER_H
#define TIMER_H

#include <ctime>

class Timer{
public:
	Timer();
	~Timer(){};
	Timer(unsigned int mta);
	void start();
	bool isTimeOver();
	void setMaxTimeAllowed(unsigned int mta);
	int getElapsedTime();
	int getStartTime();
	int getCurrentTime();
private:
	unsigned int maxTimeAllowed;
	unsigned int startTime;
};

#endif

