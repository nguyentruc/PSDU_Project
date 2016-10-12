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
	uint8_t mUser;// mode = 0: subscriber ; mode = 1: admin

	int sessionInitiation(const char* aRcvMsg, int aRcvMsgSize);
	int addSubscriberHdl(Json::Value &aRoot);
	int getSubscriberListHdl(Json::Value &aRoot);
	int changeAccPwdHdl(Json::Value &aRoot);

	int receivedCmdHandler(const char* aRcvMsg, int aRcvMsgSize);

	virtual void clientHandler() {};
	virtual int sendToClient(const Json::Value& aRoot) {return -1; };

private:
	int subscribe(const string& aStatus, const string& aPhoneNum);
	int unSubscribe(const string& aStatus, const string& aPhoneNum);
	int getSubscriberlist(const string& aStatus, list<string>& aSubscriberList);
};

class ClientBLE: public Client
{
public:
	ClientBLE(Aggregator *anAggregator, int aSockFd);
	~ClientBLE();

private:
	const static int RCV_BUF_SIZE = 1024;
	int mSockFd;

	void clientHandler();
	int sendToClient(const Json::Value& aRoot);
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
	int sendToClient(const Json::Value& aRoot);
};

#endif /* AGGREGATOR_CLIENT_H_ */
