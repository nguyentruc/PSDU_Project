/*
 * PowerHandler.h
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#ifndef AGGREGATOR_POWERHANDLER_H_
#define AGGREGATOR_POWERHANDLER_H_

#include "Aggregator.h"
#include "gpio/GPIO.h"

class Aggregator;

class PowerHandler
{
public:
	PowerHandler(Aggregator *anAggregator);
	virtual ~PowerHandler();

	void start();
	uint8_t getValue();

private:
	Aggregator *mAggregator;

	GPIO *mPinPwr;
	static const int mPIN_NUM = 13;

	void powerHdl();
};

#endif /* AGGREGATOR_POWERHANDLER_H_ */
