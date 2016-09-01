/*
 * Sim900Handler.h
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#ifndef AGGREGATOR_SIM900HANDLER_H_
#define AGGREGATOR_SIM900HANDLER_H_

#include "Aggregator.h"
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <boost/circular_buffer.hpp>

using namespace std;

class Aggregator;

class Sim900Handler
{
public:
	Sim900Handler(Aggregator* aAggregator, const char *aDevice, int aBaudrate);
	virtual ~Sim900Handler();

	void start();
	void waitForDone();

private:
	static const int BUFFER_SIZE = 1024;

	boost::thread mMonitorThread;
	boost::thread mHandleThead;
	Aggregator *mAggregator;

	boost::mutex mMtx_RcvCharBuf;
	boost::condition_variable mCv_RcvCharBuf;
	boost::circular_buffer<uint8_t> mRcvCharBuf; //Buffer of received character

	int mUartFd;

	void uartTest();
	void sim900Init();
	void uartSetup(const char *aDevice, int aBaudrate);
	void sim900Hdl();
	void sim900Monitor();
	string sim900Get();
	int waitForOk();
	int uartBaudrate(int aBaudrate);
};

#endif /* AGGREGATOR_SIM900HANDLER_H_ */
