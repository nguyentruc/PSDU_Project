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
	if (aStatus == "Power")
	{
		mAggregator->delSubscriber(POWER_STATUS, aPhoneNum);
	}
	else
	{
		return -1;
	}

	return 0;
}

int Client::getSubscriberlist(const string& aStatus, list<string>& aSubscriberList)
{
	if (aStatus == "Power")
	{
		aSubscriberList = mAggregator->getSubscriberList(POWER_STATUS);
	}
	else
	{
		return -1;
	}

	return 0;
}

void Client::cmdNotPermitted()
{
	Json::Value response;

	response["result"] = "FAILED";
	response["desc"] = "Command not permitted";

	sendToClient(response);
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
			else mUser = ADMIN;
		}
		else if (root["user"] == "subscriber")
		{
			if (mAggregator->compareSubscriberPwd(passwd) == false)
			{
				ret = -1;
			}
			else mUser = SUBSCRIBER;
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

	sendToClient(response);

	return ret;
}

int Client::getSubscriberListHdl(Json::Value& aRoot)
{
	Json::Value response;
	int8_t ret = 0;

	list<string> subscriberList;

	response["result"] = "SUCCESS";
	response["desc"] = "Subscriber list";

	if (aRoot.isMember("status") == false)
	{
		ret = -1;
	}
	else if (getSubscriberlist(aRoot["status"].asString(), subscriberList) < 0)
	{
		ret = -2;
	}

	if (ret < 0)
	{
		response["result"] = "FAILED";
		if (ret == -1) response["desc"] = "Parameter missing";
		else if (ret == -2) response["desc"] = string(aRoot["status"].asString() + " is not supported");
	}
	else
	{
		response["list"] = Json::arrayValue;

		for (list<string>::const_iterator it = subscriberList.begin(); it != subscriberList.end(); ++it)
		{
			response["list"].append(*it);
		}
	}

	sendToClient(response);

	return ret;
}

int Client::changeAccPwdHdl(Json::Value& aRoot)
{
	Json::Value response;
	int8_t ret = 0;

	list<string> subscriberList;

	response["result"] = "SUCCESS";
	response["desc"] = "Password changed";

	if (aRoot.isMember("user") == false || aRoot.isMember("newpass") == false)
	{
		ret = -1;
	}
	else if (aRoot["user"] == "admin")
	{
		mAggregator->setAdminPwd(aRoot["newpass"].asString());
	}
	else if (aRoot["user"] == "subscriber")
	{
		mAggregator->setSubscriberPwd(aRoot["newpass"].asString());
	}
	else ret = -2;

	if (ret < 0)
	{
		response["result"] = "FAILED";
		if (ret == -1) response["desc"] = "Parameter missing";
		else if (ret == -2) response["desc"] = "Wrong user";
	}

	sendToClient(response);

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
		if (mUser != ADMIN) cmdNotPermitted();
		else return addSubscriberHdl(root);
	}
	else if (root["action"] == "DelSubscriber")
	{
		if (mUser != ADMIN) cmdNotPermitted();
		else return delSubscriberHdl(root);
	}
	else if (root["action"] == "GetSubscriberList")
	{
		if (mUser != ADMIN) cmdNotPermitted();
		else return getSubscriberListHdl(root);
	}
	else if (root["action"] == "ChangePassword")
	{
		if (mUser != ADMIN) cmdNotPermitted();
		else return changeAccPwdHdl(root);
	}
	else if (root["action"] == "Subscribe")
	{
		return subscribeHdl(root);
	}
	else if (root["action"] == "Update")
	{
		return updateSttHdl(root);
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

	ret = sendToClient(response);

	return ret;
}

int Client::delSubscriberHdl(Json::Value &aRoot)
{
	Json::Value response;
	int8_t ret = 0;

	response["result"] = "SUCCESS";
	response["desc"] = "Deleted";

	if (aRoot.isMember("status") == false || aRoot.isMember("phone") == false)
	{
		ret = -1;
	}
	else if (unSubscribe(aRoot["status"].asString(), aRoot["phone"].asString()) < 0)
	{
		ret = -2;
	}

	if (ret < 0)
	{
		response["result"] = "FAILED";
		if (ret == -1) response["desc"] = "Parameter missing";
		else if (ret == -2) response["desc"] = string(aRoot["status"].asString() + " is not supported");
	}

	sendToClient(response);

	return ret;
}

int Client::subscribeHdl(Json::Value &aRoot)
{
	Json::Value response;
	int8_t ret = 0;

	response["result"] = "SUCCESS";
	response["desc"] = "Subscribed";

	if (aRoot.isMember("status") == false || aRoot.isMember("phone") == false)
	{
		ret = -1;
	}
	else if (subscribe(aRoot["status"].asString(), aRoot["phone"].asString()) < 0)
	{
		ret = -2;
	}

	if (ret < 0)
	{
		response["result"] = "FAILED";
		if (ret == -1) response["desc"] = "Parameter missing";
		else if (ret == -2) response["desc"] = string(aRoot["status"].asString() + " is not supported");
	}

	sendToClient(response);

	return ret;
}

int Client::unSubscribeHdl(Json::Value &aRoot)
{
	Json::Value response;
	int8_t ret = 0;

	response["result"] = "SUCCESS";
	response["desc"] = "Unsubscribed";

	if (aRoot.isMember("status") == false || aRoot.isMember("phone") == false)
	{
		ret = -1;
	}
	else if (unSubscribe(aRoot["status"].asString(), aRoot["phone"].asString()) < 0)
	{
		ret = -2;
	}

	if (ret < 0)
	{
		response["result"] = "FAILED";
		if (ret == -1) response["desc"] = "Parameter missing";
		else if (ret == -2) response["desc"] = string(aRoot["status"].asString() + " is not supported");
	}

	sendToClient(response);

	return ret;
}

int Client::updateSttHdl(Json::Value& aRoot)
{
	Json::Value response;
	int8_t ret = 0;

	response["result"] = "SUCCESS";

	if (aRoot.isMember("pass") == false)
	{
		ret = -1;
	}
	else if (mAggregator->compareSubscriberPwd(aRoot["pass"].asString()) == false)
	{
		ret = -2;
	}

	if (ret < 0)
	{
		response["result"] = "FAILED";
	}
	else
	{
		response["val"]["Power"] = (bool) mAggregator->getStatus(POWER_STATUS);
		response["val"]["Camera"] = (bool) mAggregator->getStatus(CAMERA_STATUS); // num of camera?
		response["val"]["Thief"] = (bool) mAggregator->getStatus(THIEF_STATUS);
	}

	sendToClient(response);

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

		close(mSockFd);
		delete this;
		return;
	}
	//TODO: timeout
	rcvBuffer[rcvMsgSize] = NULL;
	cout << "Received BLE " << rcvMsgSize << ": " << rcvBuffer << endl;

	if (sessionInitiation(rcvBuffer, rcvMsgSize) < 0)
	{
		cout << "Cannot initiate connection\n";

		close(mSockFd);
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

			close(mSockFd);
			delete this;
			return;
		}

		rcvBuffer[rcvMsgSize] = NULL;
		cout << "Received BLE " << rcvMsgSize << ": " << rcvBuffer << endl;

		receivedCmdHandler(rcvBuffer, rcvMsgSize);
	}
}

int ClientBLE::sendToClient(const Json::Value& aRoot)
{
	Json::FastWriter writer;
	string outMsg = writer.write(aRoot);

	cout << "outMsg's size = " << outMsg.size() << endl;
	cout << "outMsg = " << outMsg << endl;

	if (send(mSockFd, outMsg.c_str(), outMsg.size() + 1, 0) != outMsg.size() + 1)
	{
		cout << "send() failed from: " << __FILE__ << ":" << __LINE__ << endl;
		return -1;
	}

	return 0;
}

ClientGSM::ClientGSM(Aggregator *anAggregator, const string& aMessage, const string& aPhoneNum,
		GSM *aGSM)
{
	mAggregator = anAggregator;
	mMessage = aMessage;
	mPhoneNum = aPhoneNum;
	mGSM = aGSM;
}

ClientGSM::~ClientGSM()
{
}

void ClientGSM::clientHandler()
{
	pthread_setname_np(pthread_self(), "GSM");
	cout << "GSM Handler\n";

	receivedCmdHandler(mMessage.c_str(), mMessage.size());

	delete this;
}

int ClientGSM::sendToClient(const Json::Value& aRoot)
{
	Json::FastWriter writer;
	string outMsg = writer.write(aRoot);

	cout << "outMsg's size = " << outMsg.size() << endl;
	cout << "outMsg = " << outMsg << endl;

	if (mGSM->sendSms(mPhoneNum, outMsg.c_str()) == 0)
	{
		cout << "sendSms() failed from: " << __FILE__ << ":" << __LINE__ << endl;
		return -1;
	}

	return 0;
}
