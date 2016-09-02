/*
 * Aggregator.cpp
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#include "Aggregator.h"

Aggregator::Aggregator()
{
	mSim900Hdl = new Sim900Handler(this, gPROG_ARGUMENT["serialDevice"].as<string>().c_str(),
			gPROG_ARGUMENT["baudrate"].as<int>());
}

void Aggregator::start()
{
	mSim900Hdl->start();
	mSim900Hdl->waitForDone();
}

Aggregator::~Aggregator()
{
	delete mSim900Hdl;
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
