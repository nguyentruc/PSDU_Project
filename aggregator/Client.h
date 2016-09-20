/*
 * Client.h
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#ifndef AGGREGATOR_CLIENT_H_
#define AGGREGATOR_CLIENT_H_

#include "Aggregator.h"
#include "json/json.h"

using namespace std;

class Aggregator;

class Client
{
public:
	Client();
	virtual ~Client();

	void start();

protected:
	Aggregator *mAggregator;
	int mSockFd;

	int subscribe(const string& aStatus, const string& aPhoneNum);
	int unSubscribe(const string& aStatus, const string& aPhoneNum);
	void addSubscriberHdl(Json::Value &aRoot);
	int receivedCmdHandler(const char* aRcvMsg, int aRcvMsgSize);
	virtual void clientHandler() {};
};

class ClientBLE: public Client
{
public:
	ClientBLE(Aggregator *anAggregator, int aSockFd);
	~ClientBLE();

private:
	const static int RCV_BUF_SIZE = 1024;
	void clientHandler();
};

class ClientGSM: public Client
{
public:
	ClientGSM(Aggregator *anAggregator, const string& aMessage, const string& aPhoneNum);
	~ClientGSM();

private:
	string mMessage;
	string mPhoneNum;

	void clientHandler();
};

#endif /* AGGREGATOR_CLIENT_H_ */
