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
const int CAMERA_STATUS = 1;
const int THIEF_STATUS = 2;
const int NUM_OF_STATUS = 3;

class Aggregator
{
public:
	Aggregator();
	virtual ~Aggregator();

	void start();

	void addSubscriber(int aStatusId, const string& aPhoneNum);
	void delSubscriber(int aStatusId, const string& aPhoneNum);

	bool compareAdminPwd(const string& anAdminPwd);
	bool compareSubscriberPwd(const string& aSubscriberPwd);
	void setAdminPwd(const string& anAdminPwd);
	void setSubscriberPwd(const string& aSubscriberPwd);

	void notifySubscribers(int aStatusId, uint8_t aValue);
	list<string> getSubscriberList(int aStatusId);

	uint8_t getStatus(int8_t aStatusId);

private:
	GSM *mGSMHdl;
	PowerHandler *mPowerHdl;

	// Status id -> list of Client
	map<int, list<string> > mSubscriberList;
	boost::mutex mMtx_SubscriberList;

	vector<uint8_t> mStatus;
	boost::mutex mMtx_Status;

	string mAdminPwd;
	boost::mutex mMtx_AdminPwd;

	string mSubscriberPwd;
	boost::mutex mMtx_SubscriberPwd;

	void setStatus(int8_t aStatusId, uint8_t aValue);
};

#endif /* AGGREGATOR_AGGREGATOR_H_ */
