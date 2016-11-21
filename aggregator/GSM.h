/*
 * GSM.h
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#ifndef AGGREGATOR_GSM_H_
#define AGGREGATOR_GSM_H_

#include "Aggregator.h"
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <boost/circular_buffer.hpp>

using namespace std;

class Aggregator;

class GSM
{
public:
	GSM(Aggregator* aAggregator, const char *aDevice, int aBaudrate);
	virtual ~GSM();

	void start();
	int sendSms(const string &aPhoneNum, const string &aMsg);
	string checkAccMoney();
	string refillAccMoney(const char* aCode);

private:
	static const int BUFFER_SIZE = 1024;

	Aggregator *mAggregator;

	boost::mutex mMtx_RcvCharBuf;
	boost::condition_variable mCv_RcvCharBuf;
	boost::circular_buffer<uint8_t> mRcvCharBuf; //Buffer of received character

	int mUartFd;

	/* Notify upon receiving "OK" to other threads */
	boost::mutex mMtx_IsOk;
	boost::condition_variable mCv_IsOk;
	bool mIsOk;
	//TODO: delete this, use mSharedMsg instead

	boost::mutex mMtx_sendAT; //Only one thread can send AT commands at a time

	/* Share message between threads */
	boost::mutex mMtx_SharedMsg;
	boost::condition_variable mCv_SharedMsg;
	string mSharedMsg;

	void uartTest();
	void GSMInit();
	void uartSetup(const char *aDevice, int aBaudrate);
	void GSMHandler();
	void GSMMonitor();
	string GSMGet();
	int waitForOk();
	int uartBaudrate(int aBaudrate);
};

#endif /* AGGREGATOR_GSM_H_ */
