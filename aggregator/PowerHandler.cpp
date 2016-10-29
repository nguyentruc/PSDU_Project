/*
 * PowerHandler.cpp
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#include "PowerHandler.h"
#include <poll.h>

PowerHandler::PowerHandler(Aggregator *anAggregator)
{
	mAggregator = anAggregator;
	mPinPwr = new GPIO(mPIN_NUM, 0);
}

PowerHandler::~PowerHandler()
{
	delete mPinPwr;
}

void PowerHandler::start()
{
	boost::thread(&PowerHandler::powerHdl, this);
}

void PowerHandler::powerHdl()
{
	pthread_setname_np(pthread_self(), "PowerHandler");

	int gpioFd = mPinPwr->getFd();
	char buf[1];
	struct pollfd fdset;

	mPinPwr->setEdge("both");

//	for (;;)
//	{
//		scanf("%d", &input);
//
//		if (input == 7)
//		{
//			mAggregator->notifySubscribers(POWER_STATUS, false);
//		}
//		/* Test GPIO */
//		else if (input == 0)
//		{
//			pin14.setValue(0);
//		}
//		else if (input == 1)
//		{
//			pin14.setValue(1);
//		}
//	}
	makeRealTimeThread();

	fdset.fd = gpioFd;
	fdset.events = POLLPRI;

	lseek(gpioFd, 0, SEEK_SET);    /* consume any prior interrupt */
	read(gpioFd, buf, sizeof buf);

	while (1)
	{
		int rc = poll(&fdset, 1, -1);

		if (rc < 0)
		{
			printf("\npoll() failed!\n");
			return;
		}

		if (rc == 0)
		{
			printf(".");
		}

		if (fdset.revents & POLLPRI)
		{
			lseek(gpioFd, 0, SEEK_SET);    /* consume interrupt */
			read(gpioFd, buf, 1);

			cout << "poll() GPIO " << mPIN_NUM << " interrupt occurred " << buf[0] << endl;

			/* Check the current power status */
			mAggregator->notifySubscribers(POWER_STATUS, buf[0] - 48);
		}
	}
}

uint8_t PowerHandler::getValue()
{
	return mPinPwr->getValue();
}
