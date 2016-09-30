/*
 * Aggregator.h
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#ifndef AGGREGATOR_AGGREGATOR_H_
#define AGGREGATOR_AGGREGATOR_H_

#include <stdio.h>
#include <iostream>
#include "utils.h"
#include "Client.h"
#include "PowerHandler.h"

#include <list>
#include <map>
#include "GSM.h"

class Client;
class GSM;
class PowerHandler;

extern po::variables_map gPROG_ARGUMENT;

using namespace std;

const int POWER_STATUS = 0;

class Aggregator
{
public:
	Aggregator();
	virtual ~Aggregator();

	void start();
	void addSubscriber(int aStatusId, const string& aPhoneNum);
	bool compareAdminPwd(const string& anAdminPwd);
	bool compareSubscriberPwd(const string& aSubscriberPwd);
	void notifySubscribers(int aStatusId, bool aValue);
	list<string> getSubscriberList(int aStatusId);

private:
	GSM *mGSMHdl;
	PowerHandler *mPowerHdl;

	// Status id -> list of Client
	map<int, list<string> > mSubscriberList;
	boost::mutex mMtx_SubscriberList;

	string mAdminPwd;
	boost::mutex mMtx_AdminPwd;

	string mSubscriberPwd;
	boost::mutex mMtx_SubscriberPwd;
};

#endif /* AGGREGATOR_AGGREGATOR_H_ */
