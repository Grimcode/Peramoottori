#include "Time.h"

pm::Time::Time()
{
	inFrame = false;
	timeInFrame = 0;
	rawTime = 0;
}

double pm::Time::calculateTimeInFrame()
{
	if (inFrame == false)
	{	
		rawTime = clock_gettime(CLOCK_MONOTONIC, &start);

		inFrame = true;
		
		return 0;
	}

	else if (inFrame == true)
	{
		clock_gettime(CLOCK_MONOTONIC, &end);

		timeInFrame = difftime(end.tv_sec, start.tv_sec) * 1000000000LL + difftime(end.tv_nsec, start.tv_nsec);

		start = end;
	
		return timeInFrame;
	}

	return 0;
}

pm::Time::~Time()
{
}