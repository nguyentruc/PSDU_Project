/*
 * Aggregator.cpp
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#include "Aggregator.h"

Aggregator::Aggregator()
{
	mStatus.resize(NUM_OF_STATUS);
	mGSMHdl = new GSM(this, gPROG_ARGUMENT["serialDevice"].as<string>().c_str(),
			gPROG_ARGUMENT["baudrate"].as<int>());
	mPowerHdl = new PowerHandler(this);

	/* Initialize data */
	loadData();
	initStatus();
}

void Aggregator::initStatus()
{
	mStatus[POWER_STATUS] = mPowerHdl->getValue();
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

		char buf[20];
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
	delete mPowerHdl;
}

void Aggregator::addSubscriber(int aStatusId, const string& aPhoneNum)
{
	boost::lock_guard<boost::mutex> guard(mMtx_SubscriberList);

	// If the phone number is already in the list, stop.
	if (find(mSubscriberList[aStatusId].begin(), mSubscriberList[aStatusId].end(),
			aPhoneNum) != mSubscriberList[aStatusId].end())
	{
		return;
	}

	mSubscriberList[aStatusId].push_back(aPhoneNum);

	saveSubscriberList();
}

void Aggregator::delSubscriber(int aStatusId, const string& aPhoneNum)
{
	boost::lock_guard<boost::mutex> guard(mMtx_SubscriberList);

	mSubscriberList[aStatusId].remove(aPhoneNum);
}

bool Aggregator::compareAdminPwd(const string& anAdminPwd)
{
	boost::lock_guard<boost::mutex> guard(mMtx_Password);

	return (anAdminPwd == mAdminPwd);
}

bool Aggregator::compareSubscriberPwd(const string& aSubscriberPwd)
{
	boost::lock_guard<boost::mutex> guard(mMtx_Password);

	return (aSubscriberPwd == mSubscriberPwd);
}

void Aggregator::setAdminPwd(const string& anAdminPwd)
{
	boost::lock_guard<boost::mutex> guard(mMtx_Password);

	mAdminPwd = anAdminPwd;

	savePassword();
}

void Aggregator::setSubscriberPwd(const string& aSubscriberPwd)
{
	boost::lock_guard<boost::mutex> guard(mMtx_Password);

	mSubscriberPwd = aSubscriberPwd;

	savePassword();
}

void Aggregator::notifySubscribers(int aStatusId, uint8_t aValue)
{
	/* Update status */
	setStatus(aStatusId, aValue);

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
		root["val"] = (bool) aValue;
	}

	Json::FastWriter writer;
	string outMsg = writer.write(root);

	cout << "outMsg's size = " << outMsg.size() << endl;
	cout << "outMsg = " << outMsg << endl;

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

uint8_t Aggregator::getStatus(int8_t aStatusId)
{
	boost::lock_guard<boost::mutex> guard(mMtx_Status);

	return mStatus[aStatusId];
}

void Aggregator::setStatus(int8_t aStatusId, uint8_t aValue)
{
	boost::lock_guard<boost::mutex> guard(mMtx_Status);

	mStatus[aStatusId] = aValue;
}

void Aggregator::savePassword()
{
	ofstream ofs(PASSWORD_CONFIG_FILE);
	boost::archive::binary_oarchive oa(ofs);

	oa << mAdminPwd;
	oa << mSubscriberPwd;
}

void Aggregator::saveSubscriberList()
{
	ofstream ofs(SUBSCRIBERLIST_CONFIG_FILE);
	boost::archive::binary_oarchive oa(ofs);

	oa << mSubscriberList;
}

void Aggregator::loadData()
{
	/* Load Subscriber List */
	ifstream ifsSubriberList(SUBSCRIBERLIST_CONFIG_FILE);
	if (ifsSubriberList.good())
	{
		cout << "Subscriber list data exist, loading ...\n";

		boost::archive::binary_iarchive iaSubscriberList(ifsSubriberList);
		iaSubscriberList >> mSubscriberList;

		printInternalDataStructure();
	}
	else
	{
		cout << "Subscriber list data not found, creating one ...\n";
		saveSubscriberList();
	}

	/* Load Password */
	ifstream ifsPassword(PASSWORD_CONFIG_FILE);

	if (ifsPassword.good())
	{
		cout << "Password data exist, loading ...\n";

		boost::archive::binary_iarchive iaPassword(ifsPassword);

		iaPassword >> mAdminPwd;
		iaPassword >> mSubscriberPwd;
	}
	else
	{
		cout << "Password data not found, creating one ...\n";
		mAdminPwd = "admin"; // default
		mSubscriberPwd = "";

		savePassword();
	}
}

void Aggregator::printInternalDataStructure()
{
	boost::lock_guard<boost::mutex> guard(mMtx_SubscriberList);

	map<int, list<string> >::const_iterator subscriberListItr;
	list<string>::const_iterator listItr;

	cout << "Current subscriber list: \n";

	for (subscriberListItr = mSubscriberList.begin(); subscriberListItr != mSubscriberList.end();
			++subscriberListItr)
	{
		cout << subscriberListItr->first << ": ";
		if (subscriberListItr->second.empty())
		{
			cout << "NULL\n";
		}
		else
		{
			for (listItr = subscriberListItr->second.begin(); listItr != subscriberListItr->second.end();
					++listItr)
			{
				cout << *listItr << " ";
			}
			cout << "\n";
		}
	}
}
