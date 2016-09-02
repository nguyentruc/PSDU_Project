/*
 * Client.cpp
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#include "Client.h"

Client::Client()
{
	mAggregator = NULL;
}

Client::~Client()
{

}

void Client::start()
{
	boost::thread ClntThread(&Client::clientHandler, this);
}

int Client::subscribe(const string& aStatus, const string& aPhoneNum)
{
	if (aStatus == "Power")
	{
		mAggregator->addSubscriber(POWER_STATUS, aPhoneNum);
	}
	else
	{
		return -1;
	}

	return 0;
}

int Client::unSubscribe(const string& aStatus, const string& aPhoneNum)
{
}

ClientSim900::ClientSim900(Aggregator *anAggregator, const string& aMessage, const string& aPhoneNum)
{
	mAggregator = anAggregator;
	mMessage = aMessage;
	mPhoneNum = aPhoneNum;
}

ClientSim900::~ClientSim900()
{
}

void ClientSim900::clientHandler()
{
	pthread_setname_np(pthread_self(), "Sim900");
	cout << "Sim900 Handler\n";

	//TODO: Handle subscribers

	delete this;
}

int Client::receivedCmdHandler(const char* aRcvMsg, int aRcvMsgSize)
{
	Json::Reader reader;
	Json::Value root;

	if (reader.parse(aRcvMsg, aRcvMsg + aRcvMsgSize, root) == 0)
	{
		cout << "Error: Parse JSON failed" << endl;
		return -1;
	}

	if (root["action"] == "AddSubscriber")
	{
		addSubscriberHdl(root);
	}

	return 0;
}

void Client::addSubscriberHdl(Json::Value &aRoot)
{
	Json::Value response;

	response["result"] = "SUCCESS";
	response["desc"] = "Added";

	if (aRoot.isMember("status") == false || aRoot.isMember("phone") == false)
	{
		response["result"] = "FAILED";
		response["desc"] = "Parameter missing";
	}
	else if (subscribe(aRoot["status"].asString(), aRoot["phone"].asString()) < 0)
	{
		response["result"] = "FAILED";
		response["desc"] = string(aRoot["status"].asString() + " is not supported");
	}

	Json::FastWriter writer;
	string outMsg = writer.write(response);

	cout << "outMsg = " << outMsg << endl;
	cout << "outMsg's size = " << outMsg.size() << endl;

	//TODO: send to Client
}
