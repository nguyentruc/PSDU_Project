/*
 * PowerHandler.h
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#ifndef AGGREGATOR_POWERHANDLER_H_
#define AGGREGATOR_POWERHANDLER_H_

#include "Aggregator.h"

class Aggregator;

class PowerHandler
{
public:
	PowerHandler(Aggregator *anAggregator);
	virtual ~PowerHandler();

	void start();

private:
	Aggregator *mAggregator;
	static const int mPIN_NUM = 13;

	void powerHdl();
};

#endif /* AGGREGATOR_POWERHANDLER_H_ */
