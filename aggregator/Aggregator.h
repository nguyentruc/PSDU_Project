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
#include "Sim900Handler.h"
#include "Client.h"

#include <list>
#include <map>

class Client;
class Sim900Handler;

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

private:
	Sim900Handler *mSim900Hdl;

	// Status id -> list of Client
	map<int, list<string> > mSubscriberList;
	boost::mutex mMtx_SubscriberList;

	string mAdminPwd;
	boost::mutex mMtx_AdminPwd;

	string mSubscriberPwd;
	boost::mutex mMtx_SubscriberPwd;
};

#endif /* AGGREGATOR_AGGREGATOR_H_ */
