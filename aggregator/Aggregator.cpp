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
}

void Aggregator::start()
{
	mGSMHdl->start();
	mPowerHdl->start();

	for (;;)
	{
		pause();
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

	cout << "Notify clients" << endl;

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
