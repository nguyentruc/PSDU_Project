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

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>

class Client;
class GSM;
class PowerHandler;

extern po::variables_map gPROG_ARGUMENT;

using namespace std;

const char PASSWORD_CONFIG_FILE[] = "DATA0";
const char SUBSCRIBERLIST_CONFIG_FILE[] = "DATA1";

const int POWER_STATUS = 0;

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

	void notifySubscribers(int aStatusId, bool aValue);
	list<string> getSubscriberList(int aStatusId);

private:
	/* Save configuration data to file */
    void savePassword();
    void saveSubscriberList();

    void loadData(); //load configuration data from file

    void printInternalDataStructure();

    GSM *mGSMHdl;
	PowerHandler *mPowerHdl;

	// Status id -> list of Client
	map<int, list<string> > mSubscriberList;
	boost::mutex mMtx_SubscriberList;

	string mAdminPwd;
	string mSubscriberPwd;
	boost::mutex mMtx_Password; // use 1 mutex for 2 passwords to save memory
};

#endif /* AGGREGATOR_AGGREGATOR_H_ */
