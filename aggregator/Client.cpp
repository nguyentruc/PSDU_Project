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
	mSockFd = -1;
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

ClientGSM::ClientGSM(Aggregator *anAggregator, const string& aMessage, const string& aPhoneNum)
{
	mAggregator = anAggregator;
	mMessage = aMessage;
	mPhoneNum = aPhoneNum;
}

ClientGSM::~ClientGSM()
{
}

void ClientGSM::clientHandler()
{
	pthread_setname_np(pthread_self(), "GSM");
	cout << "GSM Handler\n";

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

	if (send(mSockFd, outMsg.c_str(), outMsg.size() + 1, 0) != outMsg.size() + 1)
	{
		cout << "send() failed from: " << __FILE__ << ":" << __LINE__ << endl;
	}
}

ClientBLE::ClientBLE(Aggregator* anAggregator, int aSockFd)
{
	mAggregator = anAggregator;
	mSockFd = aSockFd;
}

ClientBLE::~ClientBLE()
{
}

void ClientBLE::clientHandler()
{
	int rcvMsgSize;
	char rcvBuffer[RCV_BUF_SIZE];
	char threadName[10];

	sprintf(threadName, "BLE %d", mSockFd);
	pthread_setname_np(pthread_self(), threadName);
	cout << "Handling BLE client" << endl;

	while (1)
	{
		if ((rcvMsgSize = recv(mSockFd, rcvBuffer, RCV_BUF_SIZE, 0)) <= 0)
		{
			cout << "recv() failed from: " << __FILE__ << ":" << __LINE__ << endl;
			cout << "rcvMsgSize = " << rcvMsgSize << endl;

			delete this;
			return;
		}

		rcvBuffer[rcvMsgSize] = NULL;
		printf("Received BLE: %s\n", rcvBuffer);
		iotDemoDeviceController(rcvBuffer, rcvMsgSize);
	}
}

void Client::iotDemoDeviceController(const char* aRcvMsg, int aRcvMsgSize)
{
	Json::Reader reader;
	Json::Value root;

	if (reader.parse(aRcvMsg, aRcvMsg + aRcvMsgSize, root) == 0)
	{
		cout << "Error: Parse JSON failed" << endl;
		return;
	}

	for (Json::ValueIterator itr = root["ON"].begin(); itr != root["ON"].end(); ++itr)
	{
		int index = itr->asInt();
		//cout << "index " << index << endl;
		mAggregator->mPin[index - 1]->setValue(0);
	}

	for (Json::ValueIterator itr = root["OFF"].begin(); itr != root["OFF"].end(); ++itr)
	{
		int index = itr->asInt();
		//cout << "index " << index << endl;
		mAggregator->mPin[index - 1]->setValue(1);
	}
}
