/*
 * ClientSimulator.h
 *
 *  Created on: Sep 30, 2016
 *      Author: trucndt
 */

#ifndef CLIENTSIMULATOR_CLIENTSIMULATOR_H_
#define CLIENTSIMULATOR_CLIENTSIMULATOR_H_

#include "utils.h"
#include "json/json.h"

using namespace std;

extern po::variables_map gPROG_ARGUMENT;

class ClientSimulator
{
public:
	ClientSimulator();
	virtual ~ClientSimulator();

	void connectToPSDU();
	void receive();
	void sendToPSDU(const Json::Value& aRoot);

	void sessionInitiation_Normal(const string& anUser);
	void sessionInitiation_WrongPass(const string& anUser);
	void sessionInitiation_WrongUser();
	void sessionInitiation_MissingPara();
	void sessionInitiation_Skipped();

	void addSubscriber_Normal(const string& aNumber, const string& aStatus);
	void addSubscriber_MissingPara();

	void delSubscriber_Normal(const string& aNumber, const string& aStatus);
	void delSubscriber_MissingPara();

	void getSubscriberList_Normal(const string& aStatus);
	void getSubscriberList_MissingPara();

	void changeAccPwd_Normal(const string& aNewPass);
	void changeAccPwd_MissingPara();
	void changeAccPwd_WrongUser();

	void subscribe_Normal(const string& aNumber, const string& aStatus);
	void subscribe_MissingPara();

	void unsubscribe_Normal(const string& aNumber, const string& aStatus);
	void unsubscribe_MissingPara();

private:
	int mSock;
};

int processCommandLineArgument(int argc, char **argv);

#endif /* CLIENTSIMULATOR_CLIENTSIMULATOR_H_ */
