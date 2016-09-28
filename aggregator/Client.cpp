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
	mUser = 0;
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

int Client::sessionInitiation(const char* aRcvMsg, int aRcvMsgSize)
{
	Json::Value response, root;
	Json::Reader reader;
	int8_t ret = 0;

	if (reader.parse(aRcvMsg, aRcvMsg + aRcvMsgSize, root) == 0)
	{
		cout << "Error: Parse JSON failed" << endl;
		return -1;
	}

	response["result"] = "SUCCESS";
	response["desc"] = "Session initialized";

	if (root.isMember("user") == false || root.isMember("pass") == false
			|| root.isMember("action") == false)
	{
		ret = -3;
	}
	else if (root["action"].asString() != "SessionInitiation")
	{
		ret = -3;
	}
	else
	{
		string passwd = root["pass"].asString();

		if (root["user"] == "admin")
		{
			if (mAggregator->compareAdminPwd(passwd) == false)
			{
				ret = -1;
			}
			else mUser = 1;
		}
		else if (root["user"] == "subscriber")
		{
			if (mAggregator->compareSubscriberPwd(passwd) == false)
			{
				ret = -1;
			}
			else mUser = 0;
		}
		else // user doesn't match
		{
			ret = -2;
		}
	}

	if (ret < 0)
	{
		response["result"] = "FAILED";
		if (ret == -1) response["desc"] = "Authentication failed";
		else if (ret == -2) response["desc"] = "Wrong user";
		else if (ret == -3) response["desc"] = "Parameter missing";
	}

	Json::FastWriter writer;
	string outMsg = writer.write(response);

	cout << "outMsg = " << outMsg << endl;
	cout << "outMsg's size = " << outMsg.size() << endl;

	if (send(mSockFd, outMsg.c_str(), outMsg.size() + 1, 0) != outMsg.size() + 1)
	{
		cout << "send() failed from: " << __FILE__ << ":" << __LINE__ << endl;
	}

	return ret;
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
		return addSubscriberHdl(root);
	}
	else
	{
		cout << "ERR: Invalid command: " << aRcvMsg << endl;
	}

	return 0;
}

int Client::addSubscriberHdl(Json::Value &aRoot)
{
	Json::Value response;
	int8_t ret = 0;

	response["result"] = "SUCCESS";
	response["desc"] = "Added";

	if (aRoot.isMember("status") == false || aRoot.isMember("phone") == false)
	{
		response["result"] = "FAILED";
		response["desc"] = "Parameter missing";
		ret = -1;
	}
	else if (subscribe(aRoot["status"].asString(), aRoot["phone"].asString()) < 0)
	{
		response["result"] = "FAILED";
		response["desc"] = string(aRoot["status"].asString() + " is not supported");
		ret = -1;
	}

	Json::FastWriter writer;
	string outMsg = writer.write(response);

	cout << "outMsg = " << outMsg << endl;
	cout << "outMsg's size = " << outMsg.size() << endl;

	if (send(mSockFd, outMsg.c_str(), outMsg.size() + 1, 0) != outMsg.size() + 1)
	{
		cout << "send() failed from: " << __FILE__ << ":" << __LINE__ << endl;
	}

	return ret;
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

	/* Session Initiation */
	if ((rcvMsgSize = recv(mSockFd, rcvBuffer, RCV_BUF_SIZE, 0)) <= 0)
	{
		cout << "recv() failed from: " << __FILE__ << ":" << __LINE__ << endl;
		cout << "rcvMsgSize = " << rcvMsgSize << endl;

		delete this;
		return;
	}
	//TODO: timeout

	if (sessionInitiation(rcvBuffer, rcvMsgSize) < 0)
	{
		cout << "Cannot initiate connection\n";

		delete this;
		return;
	}

	/* Receive messages from client */
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
		cout << "Received BLE: " << rcvBuffer << endl;

		receivedCmdHandler(rcvBuffer, rcvMsgSize);
	}
}
