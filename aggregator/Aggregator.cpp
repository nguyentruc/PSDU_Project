/*
 * Aggregator.cpp
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#include "Aggregator.h"

Aggregator::Aggregator()
{
	mGSMHdl = new GSM(this, gPROG_ARGUMENT["serialDevice"].as<string>().c_str(),
			gPROG_ARGUMENT["baudrate"].as<int>());
	mPowerHdl = new PowerHandler(this);

	mPin[0] = new GPIO(14, 2);
	mPin[1] = new GPIO(15, 2);
	mPin[2] = new GPIO(49, 2);
	mPin[3] = new GPIO(12, 2);
}

void Aggregator::start()
{
	if (gPROG_ARGUMENT.count("noGSM") == 0)
	{
		mGSMHdl->start();
	}
	mPowerHdl->start();

	/* Create bluetooth server */
    struct sockaddr_rc servAddr, clntAddr;
    int servSock, clntSock;
    socklen_t clntLen = sizeof(clntAddr);

    // allocate socket
    if ((servSock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0)
    {
		dieWithError("socket() failed");
    }

    // bind socket to port 1 of the first available
    // local bluetooth adapter
    str2ba("00:00:00:00:00:00", &servAddr.rc_bdaddr);
    servAddr.rc_family = AF_BLUETOOTH;
    servAddr.rc_channel = (uint8_t) 1;

    if ((bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0)
	{
		dieWithError("Aggregator bind() failed");
	}

    // put socket into listening mode
    if (listen(servSock, 5) < 0)
    {
		dieWithError("listen() failed");
    }

	while (1)
	{
		cout << "Waiting for clients..." << endl;

		if ((clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &clntLen)) < 0)
		{
			cout << "ERR: accept() failed";
			continue;
		}

		char buf[100];
		ba2str( &clntAddr.rc_bdaddr, buf );
	    cout << "accepted connection from " << buf << endl;
	    memset(buf, 0, sizeof(buf));

		ClientBLE *clnt = new ClientBLE(this, clntSock);
		clnt->start();
	}
}

Aggregator::~Aggregator()
{
	delete mGSMHdl;
}

void Aggregator::addSubscriber(int aStatusId, const string& aPhoneNum)
{
	boost::lock_guard<boost::mutex> guard(mMtx_SubscriberList);

	mSubscriberList[aStatusId].push_back(aPhoneNum);
}

bool Aggregator::compareAdminPwd(const string& anAdminPwd)
{
	boost::lock_guard<boost::mutex> guard(mMtx_AdminPwd);

	return (anAdminPwd == mAdminPwd);
}

bool Aggregator::compareSubscriberPwd(const string& aSubscriberPwd)
{
	boost::lock_guard<boost::mutex> guard(mMtx_SubscriberPwd);

	return (aSubscriberPwd == mSubscriberPwd);
}

void Aggregator::notifySubscribers(int aStatusId, bool aValue)
{
	const list<string>& subscriberList = getSubscriberList(aStatusId); //need to double-check thread safe
	if (subscriberList.empty())
	{
		return;
	}

	cout << "Notify " << subscriberList.size() << " clients" << endl;

	/* Construct JSON */
	Json::Value root;

	root["action"] = "Noti";
	if (aStatusId == POWER_STATUS)
	{
		root["status"] = "Power";
	}
	root["val"] = aValue;

	Json::FastWriter writer;
	string outMsg = writer.write(root);

	cout << "outMsg = " << outMsg << endl;
	cout << "outMsg's size = " << outMsg.size() << endl;

	for (list<string>::const_iterator it = subscriberList.begin(); it != subscriberList.end(); ++it)
	{
		mGSMHdl->sendSms(*it, outMsg);
	}
}

list<string> Aggregator::getSubscriberList(int aStatusId)
{
	boost::lock_guard<boost::mutex> guard(mMtx_SubscriberList);

	return mSubscriberList[aStatusId];
}
