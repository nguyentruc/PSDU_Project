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
class GSM;

class Client
{
public:
	Client();
	virtual ~Client();

	void start();

protected:
	Aggregator *mAggregator;
	GSM *mGSM;

	int sessionInitiation(const char* aRcvMsg, int aRcvMsgSize);

	int receivedCmdHandler(const char* aRcvMsg, int aRcvMsgSize);

	virtual void clientHandler() {};
	virtual int sendToClient(const Json::Value& aRoot) {return -1; };

private:
	uint8_t mUser;
	const static uint8_t ADMIN = 1;
	const static uint8_t SUBSCRIBER = 0;

	int subscribe(const string& aStatus, const string& aPhoneNum);
	int unSubscribe(const string& aStatus, const string& aPhoneNum);
	int getSubscriberlist(const string& aStatus, list<string>& aSubscriberList);

	int addSubscriberHdl(Json::Value &aRoot);
	int delSubscriberHdl(Json::Value &aRoot);
	int getSubscriberListHdl(Json::Value &aRoot);
	int changeAccPwdHdl(Json::Value &aRoot);
	int checkAccHdl(Json::Value &aRoot);
	int refillAccHdl(Json::Value &aRoot);

	int subscribeHdl(Json::Value &aRoot);
	int unSubscribeHdl(Json::Value &aRoot);
	int updateSttHdl(Json::Value &aRoot);

	void cmdNotPermitted();
	void cmdNotSupported(const string& aCmd);
};

class ClientBLE: public Client
{
public:
	ClientBLE(Aggregator *anAggregator, GSM *aGSM, int aSockFd);
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
	ClientGSM(Aggregator *anAggregator, const string& aMessage, const string& aPhoneNum,
			GSM *aGSM);
	~ClientGSM();

private:
	string mMessage;
	string mPhoneNum;

	void clientHandler();
	int sendToClient(const Json::Value& aRoot);
};

#endif /* AGGREGATOR_CLIENT_H_ */
