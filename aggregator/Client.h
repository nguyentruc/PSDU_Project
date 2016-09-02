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

	int subscribe(const string& aStatus, const string& aPhoneNum);
	int unSubscribe(const string& aStatus, const string& aPhoneNum);
	void addSubscriberHdl(Json::Value &aRoot);
	int receivedCmdHandler(const char* aRcvMsg, int aRcvMsgSize);
	virtual void clientHandler() {};
};

class ClientSim900: public Client
{
public:
	ClientSim900(Aggregator *anAggregator, const string& aMessage, const string& aPhoneNum);
	~ClientSim900();

private:
	string mMessage;
	string mPhoneNum;

	void clientHandler();
};

#endif /* AGGREGATOR_CLIENT_H_ */
