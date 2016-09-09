/*
 * PowerHandler.cpp
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#include "PowerHandler.h"

PowerHandler::PowerHandler(Aggregator *anAggregator)
{
	mAggregator = anAggregator;
}

PowerHandler::~PowerHandler() {
	// TODO Auto-generated destructor stub
}

void PowerHandler::start()
{
	boost::thread(&PowerHandler::powerHdl, this);
}


void PowerHandler::powerHdl()
{
	/* Simulate power outage by reading number 7 from stdin */
	int input;
	for (;;)
	{
		scanf("%d", &input);

		if (input == 7)
		{
			mAggregator->notifySubscribers(POWER_STATUS, false);
		}
	}
}
