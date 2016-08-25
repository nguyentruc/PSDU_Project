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

	params.__sched_priority = sched_get_priority_max(SCHED_FIFO);
	return pthread_setschedparam(pthread_self(), SCHED_FIFO, &params);
}
