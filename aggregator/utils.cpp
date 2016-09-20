/*
 * utils.cpp
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#include "utils.h"

int makeRealTimeThread()
{
	struct sched_param params;

	params.__sched_priority = 60; //should not get too high priority, may override system's modules
	return pthread_setschedparam(pthread_self(), SCHED_FIFO, &params);
}

void dieWithError(const char *anErrMess)
{
	std::cout << "ERR: " << anErrMess << std::endl;
	exit(1);
}
